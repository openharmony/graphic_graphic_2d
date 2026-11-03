/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "transaction/rs_ashmem_helper.h"

#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ipc_file_descriptor.h"
#include "ipc_types.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "securec.h"
#include "sys_binder.h"
#include "sandbox_utils.h"
#include "platform/ohos/transaction/zidl/rs_iclient_to_render_connection.h"
#include "rs_profiler.h"

#ifndef MFD_CLOEXEC
#define MFD_CLOEXEC 0x0001U
#endif
#ifndef MFD_ALLOW_SEALING
#define MFD_ALLOW_SEALING 0x0002U
#endif
#ifndef F_ADD_SEALS
#define F_ADD_SEALS 1033
#endif
#ifndef F_GET_SEALS
#define F_GET_SEALS 1034
#endif
#ifndef F_SEAL_SEAL
#define F_SEAL_SEAL 0x0001
#endif
#ifndef F_SEAL_SHRINK
#define F_SEAL_SHRINK 0x0002
#endif
#ifndef F_SEAL_GROW
#define F_SEAL_GROW 0x0004
#endif
#ifndef F_SEAL_WRITE
#define F_SEAL_WRITE 0x0008
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t LARGE_MALLOC = 200000000;
constexpr int REQUIRED_MEMFD_SEALS = F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE;
thread_local bool g_isUnmarshalThread = false;

// Only BINDER_TYPE_FD is supported in copied/ashmem parcels; remote objects and any other
// binder object types are rejected (sender falls back to a normal binder transaction).
bool IsSupportedObjectType(uint32_t type)
{
    return type == BINDER_TYPE_FD;
}

bool GetFlatObjectAt(const MessageParcel& parcel, binder_size_t offset, const flat_binder_object*& flat)
{
    size_t dataSize = parcel.GetDataSize();
    if (offset > dataSize || sizeof(flat_binder_object) > dataSize - offset) {
        return false;
    }
    flat = reinterpret_cast<const flat_binder_object*>(parcel.GetData() + offset);
    return true;
}

// Collect kernel-translated fds from ashmemParcel into ashmemFdWorker keyed by their offsets.
// The object types are read from the sealed (immutable) shared memory; fd payloads in shared
// memory have been erased by the sender and are never used.
bool CollectSupportedFdsFromAshmemParcel(MessageParcel& dataParcel, MessageParcel* ashmemParcel,
    const binder_size_t* offsets, int32_t offsetSize, AshmemFdWorker& ashmemFdWorker)
{
    std::unordered_set<binder_size_t> seenOffsets;
    for (int32_t i = 0; i < offsetSize; i++) {
        const flat_binder_object* flat = nullptr;
        if (!GetFlatObjectAt(dataParcel, offsets[i], flat)) {
            ROSEN_LOGE("CollectSupportedFdsFromAshmemParcel invalid offset %{public}" PRIu64,
                static_cast<uint64_t>(offsets[i]));
            return false;
        }
        if (!seenOffsets.insert(offsets[i]).second) {
            ROSEN_LOGE("CollectSupportedFdsFromAshmemParcel duplicate offset %{public}" PRIu64,
                static_cast<uint64_t>(offsets[i]));
            return false;
        }
        if (!IsSupportedObjectType(flat->hdr.type)) {
            ROSEN_LOGE("CollectSupportedFdsFromAshmemParcel unsupported object type:%{public}u",
                flat->hdr.type);
            return false;
        }
        int fd = ashmemParcel->ReadFileDescriptor();
        if (fd < 0) {
            ROSEN_LOGE("CollectSupportedFdsFromAshmemParcel ReadFileDescriptor failed");
            return false;
        }
        ashmemFdWorker.InsertFdWithOffset(fd, offsets[i], true);
    }
    return true;
}
}
std::unique_ptr<AshmemAllocator> AshmemAllocator::CreateAshmemAllocator(size_t size, int mapType)
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> shmemCount = 0;
    uint64_t id = ((uint64_t)pid_ << 32) | shmemCount++;
    std::string name = "RSMemfd" + std::to_string(id);

    int fd = memfd_create(name.c_str(), MFD_CLOEXEC | MFD_ALLOW_SEALING);
    if (fd < 0) {
        ROSEN_LOGE("CreateAshmemAllocator: memfd_create failed, errno:%{public}d", errno);
        return nullptr;
    }
    if (::ftruncate(fd, static_cast<off_t>(size)) < 0) {
        ROSEN_LOGE("CreateAshmemAllocator: ftruncate failed, errno:%{public}d", errno);
        ::close(fd);
        return nullptr;
    }
    auto allocator = std::make_unique<AshmemAllocator>(fd, size);

    if (!allocator->MapAshmem(mapType)) {
        ROSEN_LOGE("CreateAshmemAllocator: MapAshmem failed");
        return nullptr;
    }

    return allocator;
}

bool AshmemAllocator::ValidateSealedMemfd(int fd, size_t size)
{
    if (fd < 0) {
        ROSEN_LOGE("AshmemAllocator::ValidateSealedMemfd: fd < 0");
        return false;
    }
    // check the seals before the size: F_SEAL_SHRINK | F_SEAL_GROW freeze the memfd size for
    // every holder, so the size check below cannot be raced once the seals are verified
    int seals = ::fcntl(fd, F_GET_SEALS);
    if (seals < 0 || (seals & REQUIRED_MEMFD_SEALS) != REQUIRED_MEMFD_SEALS) {
        ROSEN_LOGE("AshmemAllocator::ValidateSealedMemfd: memfd is not fully sealed, seals:%{public}d",
            seals);
        return false;
    }
    // lseek(SEEK_END) is used instead of fstat to query the size: it does not require
    // getattr permission on the fd and works for both memfd and ashmem
    off_t fileSize = ::lseek(fd, 0, SEEK_END);
    if (fileSize < 0 || static_cast<uint64_t>(fileSize) < size) {
        ROSEN_LOGE("AshmemAllocator::ValidateSealedMemfd: invalid memfd size, fd:%{public}d, "
            "fileSize:%{public}" PRId64 ", size:%{public}zu, errno:%{public}d",
            fd, static_cast<int64_t>(fileSize), size, errno);
        return false;
    }
    return true;
}

void* AshmemAllocator::CopyFromMemfd(int fd, size_t size)
{
    if (size > LARGE_MALLOC) {
        ROSEN_LOGW("AshmemAllocator::CopyFromMemfd this time malloc large memory, size:%{public}zu", size);
    }
    void* base = malloc(size);
    if (base == nullptr) {
        ROSEN_LOGE("AshmemAllocator::CopyFromMemfd malloc failed, size:%{public}zu", size);
        return nullptr;
    }
    // pread does not involve mmap, works on platforms that forbid mapping a sealed memfd
    ssize_t readSize = ::pread(fd, base, size, 0);
    if (readSize < 0 || static_cast<size_t>(readSize) != size) {
        ROSEN_LOGE("AshmemAllocator::CopyFromMemfd pread failed, fd:%{public}d, read:%{public}zd, "
            "size:%{public}zu, errno:%{public}d", fd, readSize, size, errno);
        free(base);
        return nullptr;
    }
    return base;
}

AshmemAllocator::AshmemAllocator(int fd, size_t size) : fd_(fd), size_(size) {}

AshmemAllocator::~AshmemAllocator()
{
    Dealloc(data_);
}

bool AshmemAllocator::MapAshmem(int mapType)
{
    void *startAddr = ::mmap(nullptr, size_, mapType, MAP_SHARED, fd_, 0);
    if (startAddr == MAP_FAILED) {
        ROSEN_LOGE("AshmemAllocator::MapAshmem MAP_FAILED, fd:%{public}d, size:%{public}zu, "
            "mapType:%{public}d, errno:%{public}d", fd_, size_, mapType, errno);
        return false;
    }
    data_ = startAddr;
    return true;
}

bool AshmemAllocator::Seal()
{
    if (fd_ < 0) {
        ROSEN_LOGE("AshmemAllocator::Seal fd < 0");
        return false;
    }
    if (data_ != nullptr) {
        // F_SEAL_WRITE requires that no writable shared mapping exists
        ::munmap(data_, size_);
        data_ = nullptr;
    }
    if (::fcntl(fd_, F_ADD_SEALS, REQUIRED_MEMFD_SEALS) < 0) {
        ROSEN_LOGE("AshmemAllocator::Seal fcntl F_ADD_SEALS failed, errno:%{public}d", errno);
        return false;
    }
    return true;
}

bool AshmemAllocator::WriteToAshmem(const void *data, size_t size)
{
    if (data == nullptr || size_ < size) {
        ROSEN_LOGE("AshmemAllocator::WriteToAshmem invalid param, data null:%{public}d, "
            "size_:%{public}zu, size:%{public}zu", data == nullptr, size_, size);
        return false;
    }
    if (!data_) {
        ROSEN_LOGE("AshmemAllocator::WriteToAshmem data_ is nullptr");
        return false;
    }
    errno_t err = memcpy_s(data_, size, data, size);
    if (err != EOK) {
        ROSEN_LOGE("AshmemAllocator::WriteToAshmem memcpy_s failed, err:%{public}d", err);
        return false;
    }
    return true;
}

void* AshmemAllocator::CopyFromAshmem(size_t size)
{
    if (size_ < size) {
        ROSEN_LOGE("AshmemAllocator::CopyFromAshmem size_:%{public}zu < size:%{public}zu", size_, size);
        return nullptr;
    }
    if (size > LARGE_MALLOC) {
        ROSEN_LOGW("AshmemAllocator::CopyFromAshmem this time malloc large memory, size:%{public}zu", size);
    }
    if (!data_) {
        ROSEN_LOGE("AshmemAllocator::CopyFromAshmem data_ is nullptr");
        return nullptr;
    }
    void* base = malloc(size);
    if (base == nullptr) {
        ROSEN_LOGE("AshmemAllocator::CopyFromAshmem malloc failed, size:%{public}zu", size);
        return nullptr;
    }
    errno_t err = memcpy_s(base, size, data_, size);
    if (err != EOK) {
        free(base);
        base = nullptr;
        ROSEN_LOGE("AshmemAllocator::CopyFromAshmem memcpy_s failed, err:%{public}d", err);
        return nullptr;
    }
    return base;
}

// the fd is only valid during the object lifetime
int AshmemAllocator::GetFd() const
{
    return fd_;
}

size_t AshmemAllocator::GetSize() const
{
    return size_;
}

void* AshmemAllocator::GetData() const
{
    return data_;
}

void AshmemAllocator::Dealloc(void* data)
{
    if (data != data_) {
        ROSEN_LOGW("AshmemAllocator::Dealloc data addr not match!");
    }
    if (data_ != nullptr) {
        ::munmap(data_, size_);
        data_ = nullptr;
    }
    if (fd_ > 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

void* AshmemAllocator::Alloc(size_t size)
{
    ROSEN_LOGW("AshmemAllocator::Alloc");
    return nullptr;
}

void* AshmemAllocator::Realloc(void* data, size_t newSize)
{
    ROSEN_LOGW("AshmemAllocator::Realloc");
    return data_;
}

AshmemFdContainer& AshmemFdContainer::Instance()
{
    thread_local AshmemFdContainer instance;
    return instance;
}

void AshmemFdContainer::SetIsUnmarshalThread(bool isUnmarshalThread)
{
    g_isUnmarshalThread = isUnmarshalThread;
}

int AshmemFdContainer::ReadSafeFd(Parcel &parcel, std::function<int(Parcel&)> readFdDefaultFunc)
{
    if (!g_isUnmarshalThread || !isUseFdContainer_) {
        if (readFdDefaultFunc == nullptr) {
            return static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
        }
        return readFdDefaultFunc(parcel);
    }

    size_t offset = parcel.GetReadPosition();
    // consume the flat_binder_object to move the read cursor, but never trust its payload:
    // the fd in shared memory was erased by the sender and is not used here
    if (parcel.ReadBuffer(sizeof(flat_binder_object), false) == nullptr) {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd failed: cannot consume flat object at offset %{public}zu",
            offset);
        return INVALID_FD;
    }

    auto it = fds_.find(offset);
    if (it == fds_.end()) {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd failed: offset %{public}zu not found", offset);
        return INVALID_FD;
    }
    int containerFd = it->second;
    fds_.erase(it);
    if (containerFd < 0) {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd failed: invalid containerFd = %{public}d", containerFd);
        return INVALID_FD;
    }

    int safeFd = dup(containerFd);
    if (safeFd < 0) {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd dup failed: containerFd = %{public}d, errno = %{public}d",
            containerFd, errno);
    }
    return safeFd;
}

void AshmemFdContainer::Merge(const std::unordered_map<binder_size_t, int>& fds)
{
    if (!g_isUnmarshalThread) {
        return;
    }
    isUseFdContainer_ = true;
    fds_ = fds;
    ROSEN_LOGI_IF(DEBUG_IPC, "AshmemFdContainer::Merge fds_ = %s", PrintFds().c_str());
}

void AshmemFdContainer::Clear()
{
    if (!g_isUnmarshalThread) {
        return;
    }
    isUseFdContainer_ = false;
    if (!fds_.empty()) {
        ROSEN_LOGW_IF(DEBUG_IPC, "AshmemFdContainer::Clear not empty, fds = %{public}s", PrintFds().c_str());
        fds_.clear();
    }
}

std::string AshmemFdContainer::PrintFds() const
{
    std::string ret = "{";
    for (const auto& [offset, fd]: fds_) {
        ret += std::to_string(offset);
        ret += ":";
        ret += std::to_string(fd);
        ret += ", ";
    }
    ret += "}";
    return ret;
}

AshmemFdWorker::AshmemFdWorker(const pid_t callingPid) : callingPid_(callingPid) {}

AshmemFdWorker::~AshmemFdWorker()
{
    // the worker exclusively owns the fds dup'd from the ashmem parcel (ReadSafeFd hands out
    // its own dups), so they are always closed here regardless of how far parsing got
    for (const int fd : fdsToBeClosed_) {
        if (fd > 0) {
            ::close(fd);
        }
    }
    if (!isFdContainerUpdated_) {
        return;
    }
    AshmemFdContainer::Instance().Clear();
}

void AshmemFdWorker::InsertFdWithOffset(int fd, binder_size_t offset, bool shouldCloseFd)
{
    if (shouldCloseFd) {
        fdsToBeClosed_.insert(fd);
    }
    if (isFdContainerUpdated_) {
        ROSEN_LOGE("AshmemFdWorker::InsertFdWithOffset fd container has been updated, skip fd %{public}d with "
            "offset %{public}" PRIu64, fd, static_cast<uint64_t>(offset));
        return;
    }
    auto [it, isNewElement] = fds_.try_emplace(offset, fd);
    if (isNewElement) {
        return;
    }
    ROSEN_LOGW("AshmemFdWorker::InsertFdWithOffset existed offset %{public}" PRIu64 "is overriden, "
        "old fd = %{public}d, new fd = %{public}d", static_cast<uint64_t>(offset), it->second, fd);
}

void AshmemFdWorker::PushFdsToContainer()
{
    if (isFdContainerUpdated_) {
        ROSEN_LOGE("AshmemFdWorker::PushFdsToContainer fd container has been updated, skip push operation");
        return;
    }
    ROSEN_LOGI_IF(DEBUG_IPC, "AshmemFdWorker::PushFdsToContainer from callingPid %{public}d",
        static_cast<int>(callingPid_));
    AshmemFdContainer::Instance().Merge(fds_);
    isFdContainerUpdated_ = true;
}

bool RSAshmemHelper::CopySupportedObjectsToParcel(
    MessageParcel* ashmemParcel, std::shared_ptr<MessageParcel>& dataParcel)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel->GetObjectOffsets());
    size_t objectNum = dataParcel->GetOffsetsSize();
    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = nullptr;
        if (!GetFlatObjectAt(*dataParcel, object[i], flat)) {
            ROSEN_LOGE("RSAshmemHelper::CopySupportedObjectsToParcel invalid offset");
            return false;
        }
        if (!IsSupportedObjectType(flat->hdr.type)) {
            ROSEN_LOGW("RSAshmemHelper::CopySupportedObjectsToParcel unsupported object type:%{public}u",
                flat->hdr.type);
            return false;
        }
        if (!ashmemParcel->WriteFileDescriptor(static_cast<int32_t>(flat->handle))) {
            ROSEN_LOGE("RSAshmemHelper::CopySupportedObjectsToParcel WriteFileDescriptor failed, fd:%{public}u",
                flat->handle);
            return false;
        }
    }
    return true;
}

void RSAshmemHelper::EraseSupportedObjectsInAshmem(void* ashmemData, const MessageParcel& dataParcel)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel.GetObjectOffsets());
    size_t objectNum = dataParcel.GetOffsetsSize();
    size_t dataSize = dataParcel.GetDataSize();
    for (size_t i = 0; i < objectNum; i++) {
        binder_size_t offset = object[i];
        if (offset > dataSize || sizeof(flat_binder_object) > dataSize - offset) {
            continue; // offsets were already validated when copying objects
        }
        flat_binder_object* flat =
            reinterpret_cast<flat_binder_object*>(reinterpret_cast<uintptr_t>(ashmemData) + offset);
        // keep hdr.type so the receiver can classify the object, erase the fd payload
        if (IsSupportedObjectType(flat->hdr.type)) {
            flat->handle = static_cast<uint32_t>(INVALID_FD);
            flat->cookie = 0;
        }
    }
}

bool RSAshmemHelper::CopySupportedObjectsForParcelCopy(MessageParcel& oldParcel, MessageParcel& copiedParcel)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(oldParcel.GetObjectOffsets());
    size_t objectNum = oldParcel.GetOffsetsSize();
    uintptr_t copiedData = copiedParcel.GetData();
    size_t copiedDataSize = copiedParcel.GetDataSize();
    auto onFailure = [&object, objectNum, copiedData, copiedDataSize](size_t fromIndex) {
        // invalidate fd payloads that still hold the origin parcel's fds, so that
        // ~MessageParcel (ClearFileDescriptor) of the copied parcel never closes them
        for (size_t j = fromIndex; j < objectNum; j++) {
            if (object[j] > copiedDataSize || sizeof(flat_binder_object) > copiedDataSize - object[j]) {
                continue;
            }
            flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(copiedData + object[j]);
            if (IsSupportedObjectType(flat->hdr.type)) {
                flat->handle = 0;
            }
        }
        return false;
    };
    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = nullptr;
        if (!GetFlatObjectAt(oldParcel, object[i], flat) || object[i] > copiedDataSize ||
            sizeof(flat_binder_object) > copiedDataSize - object[i]) {
            return onFailure(i);
        }
        if (!IsSupportedObjectType(flat->hdr.type)) {
            ROSEN_LOGW("RSAshmemHelper::CopySupportedObjectsForParcelCopy unsupported object type:%{public}u, "
                "fall back to no copy", flat->hdr.type);
            return onFailure(i);
        }
        // duplicate the fd so it survives BC_FREE_BUFFER of the origin parcel; the duplicate
        // is closed by ~MessageParcel (ClearFileDescriptor) of the copied parcel
        int32_t dupFd = dup(static_cast<int32_t>(flat->handle));
        if (dupFd < 0) {
            ROSEN_LOGE("RSAshmemHelper::CopySupportedObjectsForParcelCopy dup failed, errno:%{public}d", errno);
            return onFailure(i);
        }
        flat_binder_object* copiedFlat = reinterpret_cast<flat_binder_object*>(copiedData + object[i]);
        copiedFlat->handle = static_cast<uint32_t>(dupFd);
    }
    return true;
}

std::shared_ptr<MessageParcel> RSAshmemHelper::CreateAshmemParcel(std::shared_ptr<MessageParcel>& dataParcel)
{
    size_t dataSize = dataParcel->GetDataSize();
    RS_TRACE_NAME("CreateAshmemParcel data size:" + std::to_string(dataSize));

    // 1. save origin parcel data to a memfd and record the fd to new parcel
    // 2. save all fds and their offsets in new parcel (only BINDER_TYPE_FD is supported;
    //    any other binder object refuses the ashmem parcel, sender falls back to binder)
    // 3. erase all fd payloads in the memfd copy
    // 4. seal the memfd so nobody can modify or writable-map it anymore
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    if (!ashmemAllocator) {
        ROSEN_LOGE("CreateAshmemParcel failed, ashmemAllocator is nullptr");
        return nullptr;
    }
    if (!ashmemAllocator->WriteToAshmem(reinterpret_cast<void*>(dataParcel->GetData()), dataSize)) {
        ROSEN_LOGE("CreateAshmemParcel: WriteToAshmem failed");
        return nullptr;
    }
    // 1. save data
    int fd = ashmemAllocator->GetFd();
    std::shared_ptr<MessageParcel> ashmemParcel = std::make_shared<MessageParcel>();
    // 1: indicate ashmem parcel
    if (!ashmemParcel->WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor()) ||
        !ashmemParcel->WriteInt32(1) || !ashmemParcel->WriteUint32(dataSize) ||
        !ashmemParcel->WriteFileDescriptor(fd)) {
        ROSEN_LOGE("CreateAshmemParcel: write ashmem header failed, dataSize:%{public}zu", dataSize);
        return nullptr;
    }

    // 2. save fds and their offsets
    size_t offsetSize = dataParcel->GetOffsetsSize();
    if (!ashmemParcel->WriteInt32(offsetSize)) {
        ROSEN_LOGE("CreateAshmemParcel: WriteInt32 offsetSize failed");
        return nullptr;
    }
    if (offsetSize > 0) {
        // save array that record the offsets of all fds
        ashmemParcel->WriteBuffer(
            reinterpret_cast<void*>(dataParcel->GetObjectOffsets()), sizeof(binder_size_t) * offsetSize);
        // save all fds of origin parcel; refuse ashmem parcel for non-fd objects
        if (!CopySupportedObjectsToParcel(ashmemParcel.get(), dataParcel)) {
            ROSEN_LOGE("CreateAshmemParcel: CopySupportedObjectsToParcel failed");
            return nullptr;
        }
        // 3. erase fd payloads in the memfd copy
        EraseSupportedObjectsInAshmem(ashmemAllocator->GetData(), *dataParcel);
    }

    // 4. seal the memfd to make the content immutable for everyone
    if (!ashmemAllocator->Seal()) {
        ROSEN_LOGE("CreateAshmemParcel: Seal failed");
        return nullptr;
    }

    return ashmemParcel;
}

std::shared_ptr<MessageParcel> RSAshmemHelper::ParseFromAshmemParcel(MessageParcel* ashmemParcel,
    std::unique_ptr<AshmemFdWorker>& ashmemFdWorker,
    std::shared_ptr<AshmemFlowControlUnit> &ashmemFlowControlUnit, pid_t callingPid)
{
    if (!ashmemParcel) {
        ROSEN_LOGE("ParseFromAshmemParcel ashmemParcel is nullptr");
        return nullptr;
    }
    uint32_t dataSize{0};
    if (!ashmemParcel->ReadUint32(dataSize)) {
        ROSEN_LOGE("ParseFromAshmemParcel Read dataSize is failed");
        return nullptr;
    }
    RS_TRACE_NAME("ParseFromAshmemParcel data size:" + std::to_string(dataSize));
    // ashmem parcel flow control begins
    ashmemFlowControlUnit = AshmemFlowControlUnit::CheckOverflowAndCreateInstance(callingPid, dataSize);
    if (ashmemFlowControlUnit == nullptr) {
        // discard this ashmem parcel since callingPid is submitting too many data to RS simultaneously
        RS_TRACE_NAME_FMT("RSAshmemHelper::ParseFromAshmemParcel reject ashmem buffer size %" PRIu32
            " from pid %d", dataSize, static_cast<int>(callingPid));
        ROSEN_LOGE("RSAshmemHelper::ParseFromAshmemParcel reject ashmem buffer size %{public}" PRIu32
            " from pid %{public}d", dataSize, static_cast<int>(callingPid));
        return nullptr;
    }

    int fd = ashmemParcel->ReadFileDescriptor();
    if (!AshmemAllocator::ValidateSealedMemfd(fd, dataSize)) {
        ROSEN_LOGE("ParseFromAshmemParcel failed, invalid memfd");
        if (fd >= 0) {
            ::close(fd);
        }
        return nullptr;
    }
    // read the whole payload into an owned buffer instead of mapping the sealed memfd:
    // some platforms forbid mapping a sealed memfd at all (mmap returns EPERM)
    void* data = AshmemAllocator::CopyFromMemfd(fd, dataSize);
    if (fd >= 0) {
        ::close(fd);
    }
    if (data == nullptr) {
        ROSEN_LOGE("ParseFromAshmemParcel failed, CopyFromMemfd failed");
        return nullptr;
    }
    // the parcel owns the buffer via DefaultAllocator (freed on destruction)
    auto dataParcel = RS_PROFILER_COPY_PARCEL(*ashmemParcel);
    dataParcel->ParseFrom(reinterpret_cast<uintptr_t>(data), dataSize);

    int32_t offsetSize = ashmemParcel->ReadInt32();
    if (offsetSize > 0) {
        // each binder object occupies at least one flat_binder_object in the data buffer,
        // so offsetSize can never exceed dataSize / sizeof(flat_binder_object)
        if (static_cast<uint64_t>(offsetSize) > dataParcel->GetDataSize() / sizeof(flat_binder_object)) {
            ROSEN_LOGE("ParseFromAshmemParcel: invalid offsetSize %{public}d, dataSize %{public}zu",
                offsetSize, dataParcel->GetDataSize());
            return nullptr;
        }
        auto* offsets = ashmemParcel->ReadBuffer(sizeof(binder_size_t) * offsetSize);
        if (offsets == nullptr) {
            ROSEN_LOGE("ParseFromAshmemParcel: read object offsets failed");
            return nullptr;
        }
        ashmemFdWorker = std::make_unique<AshmemFdWorker>(callingPid);
        // collect kernel-translated fds into the fd container keyed by offset;
        // never inject offsets into dataParcel nor write anything back to shared memory
        if (!CollectSupportedFdsFromAshmemParcel(*dataParcel, ashmemParcel,
                reinterpret_cast<const binder_size_t*>(offsets), offsetSize, *ashmemFdWorker)) {
            ROSEN_LOGE("ParseFromAshmemParcel: CollectSupportedFdsFromAshmemParcel failed");
            return nullptr;
        }
    }

    auto token = dataParcel->ReadInterfaceToken();
    if (token != RSIClientToRenderConnection::GetDescriptor()) {
        ROSEN_LOGE("RSAshmemHelper::ParseFromAshmemParcel interface token mismatch");
        return nullptr;
    }

    if (dataParcel->ReadInt32() != 0) { // identify normal parcel
        ROSEN_LOGE("RSAshmemHelper::ParseFromAshmemParcel failed");
        return nullptr;
    }

    return dataParcel;
}
} // namespace Rosen
} // namespace OHOS
