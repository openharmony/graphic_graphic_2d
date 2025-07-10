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

#include <memory>
#include <sys/mman.h>
#include <unistd.h>

#include "ashmem.h"
#include "ipc_file_descriptor.h"
#include "ipc_types.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "securec.h"
#include "sys_binder.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t LARGE_MALLOC = 200000000;
thread_local bool g_isUnmarshalThread = false;
}
std::unique_ptr<AshmemAllocator> AshmemAllocator::CreateAshmemAllocator(size_t size, int mapType)
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> shmemCount = 0;
    uint64_t id = ((uint64_t)pid_ << 32) | shmemCount++;
    std::string name = "RSAshmem" + std::to_string(id);

    int fd = AshmemCreate(name.c_str(), size);
    if (fd < 0) {
        ROSEN_LOGE("CreateAshmemAllocator: AshmemCreate failed, fd:%{public}d", fd);
        return nullptr;
    }
    auto allocator = std::make_unique<AshmemAllocator>(fd, size);

    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        ROSEN_LOGE("CreateAshmemAllocator: AshmemSetProt failed, result:%{public}d", result);
        return nullptr;
    }

    if (!allocator->MapAshmem(mapType)) {
        ROSEN_LOGE("CreateAshmemAllocator: MapAshmem failed");
        return nullptr;
    }

    return allocator;
}

std::unique_ptr<AshmemAllocator> AshmemAllocator::CreateAshmemAllocatorWithFd(int fd, size_t size, int mapType)
{
    if (fd < 0) {
        ROSEN_LOGE("CreateAshmemAllocatorWithFd: fd < 0");
        return nullptr;
    }
    auto allocator = std::make_unique<AshmemAllocator>(fd, size);

    int ashmemSize = AshmemGetSize(fd);
    if (ashmemSize < 0 || size_t(ashmemSize) < size) {
        ROSEN_LOGE("CreateAshmemAllocatorWithFd: ashmemSize < size");
        return nullptr;
    }

    if (!allocator->MapAshmem(mapType)) {
        ROSEN_LOGE("CreateAshmemAllocatorWithFd: MapAshmem failed");
        return nullptr;
    }

    return allocator;
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
        ROSEN_LOGE("AshmemAllocator::MapAshmem MAP_FAILED");
        return false;
    }
    data_ = startAddr;
    return true;
}

bool AshmemAllocator::WriteToAshmem(const void *data, size_t size)
{
    if (data == nullptr || size_ < size) {
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
    static AshmemFdContainer instance;
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
    sptr<IPCFileDescriptor> descriptor = parcel.ReadObject<IPCFileDescriptor>();

    int parcelFd = INVALID_FD;
    if (descriptor == nullptr) {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd ReadObject failed");
    } else {
        parcelFd = descriptor->GetFd();
    }
    if (parcelFd < 0) {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd failed: invalid parcelFd = %{public}d", parcelFd);
    }

    int containerFd = INVALID_FD;
    auto it = fds_.find(offset);
    if (it != fds_.end()) {
        containerFd = it->second;
        fds_.erase(it);
    } else {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd failed: offset %{public}zu not found", offset);
    }
    if (containerFd < 0) {
        ROSEN_LOGE("AshmemFdContainer::ReadSafeFd failed: invalid containerFd = %{public}d", containerFd);
    }

    if (parcelFd != containerFd) {
        ROSEN_LOGW("AshmemFdContainer::ReadSafeFd inconsistent parcelFd = %{public}d, containerFd = %{public}d",
            parcelFd, containerFd);
    }

    if (parcelFd >= 0 && containerFd < 0) {
        int fd = dup(parcelFd);
        if (fd < 0) {
            ROSEN_LOGE("AshmemFdContainer::ReadSafeFd dup failed: parcelFd = %{public}d, errno = %{public}d",
                parcelFd, errno);
        }
        return fd;
    }

    if (containerFd < 0) {
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
    ROSEN_LOGI_IF(DEBUG_IPC, "AshmemFdContainer::Merge fds_ = %{public}s", PrintFds().c_str());
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
    if (needManualCloseFds_) {
        for (const int fd : fdsToBeClosed_) {
            if (fd > 0) {
                ::close(fd);
            }
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
    it->second = fd;
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

void AshmemFdWorker::EnableManualCloseFds()
{
    needManualCloseFds_ = true;
}

void RSAshmemHelper::CopyFileDescriptor(
    MessageParcel* ashmemParcel, std::shared_ptr<MessageParcel>& dataParcel)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel->GetObjectOffsets());
    size_t objectNum = dataParcel->GetOffsetsSize();
    uintptr_t data = dataParcel->GetData();
    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + object[i]);
        if (flat->hdr.type == BINDER_TYPE_FD) {
            if (!ashmemParcel->WriteFileDescriptor(flat->handle)) {
                ROSEN_LOGE("RSAshmemHelper::CopyFileDescriptor failed, fd:%{public}d", flat->handle);
            }
        }
    }
}

void RSAshmemHelper::InjectFileDescriptor(std::shared_ptr<MessageParcel>& dataParcel, MessageParcel* ashmemParcel,
    std::unique_ptr<AshmemFdWorker>& ashmemFdWorker, pid_t callingPid)
{
    ashmemFdWorker = std::make_unique<AshmemFdWorker>(callingPid);
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel->GetObjectOffsets());
    size_t objectNum = dataParcel->GetOffsetsSize();
    uintptr_t data = dataParcel->GetData();
    for (size_t i = 0; i < objectNum; i++) {
        binder_size_t offset = object[i];
        if (offset + sizeof(flat_binder_object) > dataParcel->GetDataSize()) {
            ROSEN_LOGW("RSAshmemHelper::InjectFileDescriptor offset invalid");
            continue;
        }
        flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + offset);
        if (flat->hdr.type == BINDER_TYPE_FD || flat->hdr.type == BINDER_TYPE_FDR) {
            int32_t val = ashmemParcel->ReadFileDescriptor();
            if (val < 0) {
                ROSEN_LOGW("RSAshmemHelper::InjectFileDescriptor failed, fd:%{public}d, handle:%{public}d", val,
                    flat->handle);
            }
            flat->handle = static_cast<uint32_t>(val);
            if (ashmemFdWorker) {
                bool shouldCloseFd = flat->hdr.type == BINDER_TYPE_FD;
                ashmemFdWorker->InsertFdWithOffset(val, offset, shouldCloseFd);
            }
        }
    }
}

std::shared_ptr<MessageParcel> RSAshmemHelper::CreateAshmemParcel(std::shared_ptr<MessageParcel>& dataParcel)
{
    size_t dataSize = dataParcel->GetDataSize();
    RS_TRACE_NAME("CreateAshmemParcel data size:" + std::to_string(dataSize));

    // if want a full copy of parcel, need to save its data and fds both:
    // 1. save origin parcel data to ashmeme and record the fd to new parcel
    // 2. save all fds and their offsets in new parcel
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
    ashmemParcel->WriteInt32(1); // 1: indicate ashmem parcel
    ashmemParcel->WriteUint32(dataSize);
    ashmemParcel->WriteFileDescriptor(fd);

    // 2. save fds and their offsets
    size_t offsetSize = dataParcel->GetOffsetsSize();
    ashmemParcel->WriteInt32(offsetSize);
    if (offsetSize > 0) {
        // save array that record the offsets of all fds
        ashmemParcel->WriteBuffer(
            reinterpret_cast<void*>(dataParcel->GetObjectOffsets()), sizeof(binder_size_t) * offsetSize);
        // save all fds of origin parcel
        ROSEN_LOGD("CreateAshmemParcel invoke CopyFileDescriptor, offsetSize:%{public}zu", offsetSize);
        CopyFileDescriptor(ashmemParcel.get(), dataParcel);
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
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, dataSize, PROT_READ | PROT_WRITE);
    if (!ashmemAllocator) {
        ROSEN_LOGE("ParseFromAshmemParcel failed, ashmemAllocator is nullptr");
        return nullptr;
    }
    void* data = ashmemAllocator->GetData();
    auto dataParcel = std::make_shared<MessageParcel>(ashmemAllocator.release());
    dataParcel->ParseFrom(reinterpret_cast<uintptr_t>(data), dataSize);

    int32_t offsetSize = ashmemParcel->ReadInt32();
    if (offsetSize > 0) {
        auto* offsets = ashmemParcel->ReadBuffer(sizeof(binder_size_t) * offsetSize);
        if (offsets == nullptr) {
            ROSEN_LOGE("ParseFromAshmemParcel: read object offsets failed");
            return nullptr;
        }
        // restore array that record the offsets of all fds
        dataParcel->InjectOffsets(reinterpret_cast<binder_size_t>(offsets), offsetSize);
        // restore all fds
        InjectFileDescriptor(dataParcel, ashmemParcel, ashmemFdWorker, callingPid);
    }

    if (dataParcel->ReadInt32() != 0) { // identify normal parcel
        ROSEN_LOGE("RSAshmemHelper::ParseFromAshmemParcel failed");
        return nullptr;
    }

    return dataParcel;
}
} // namespace Rosen
} // namespace OHOS
