/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "securec.h"
#include "sys_binder.h"

#ifdef ROSEN_OHOS
namespace OHOS {
namespace Rosen {
std::unique_ptr<AshmemAllocator> AshmemAllocator::CreateAshmemAllocator(size_t size, int mapType)
{
    static pid_t pid_ = getpid();
    static std::atomic<uint32_t> shmemCount = 0;
    uint64_t id = ((uint64_t)pid_ << 32) | shmemCount++;
    std::string name = "RSAshmem" + std::to_string(id);

    int fd = AshmemCreate(name.c_str(), size);
    if (fd < 0) {
        ROSEN_LOGE("CreateAshmemAllocator: AshmemCreate failed, fd:%d", fd);
        return nullptr;
    }
    auto allocator = std::make_unique<AshmemAllocator>(fd, size);

    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        ROSEN_LOGE("CreateAshmemAllocator: AshmemSetProt failed, result:%d", result);
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
    if (memcpy_s(data_, size, data, size) != EOK) {
        ROSEN_LOGE("AshmemAllocator::WriteToAshmem memcpy_s failed");
        return false;
    }
    return true;
}

void* AshmemAllocator::CopyFromAshmem(size_t size)
{
    if (size_ < size) {
        return nullptr;
    }
    void* base = malloc(size);
    if (base == nullptr) {
        ROSEN_LOGE("AshmemAllocator::CopyFromAshmem malloc(size) failed");
        return nullptr;
    }
    if (memcpy_s(base, size, data_, size) != EOK) {
        free(base);
        base = nullptr;
        ROSEN_LOGE("AshmemAllocator::CopyFromAshmem memcpy_s failed");
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
    return nullptr;
}

void RSAshmemHelper::CopyFileDescriptor(
    std::shared_ptr<MessageParcel>& ashmemParcel, std::shared_ptr<MessageParcel>& dataParcel)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel->GetObjectOffsets());
    size_t objectNum = dataParcel->GetOffsetsSize();
    uintptr_t data = dataParcel->GetData();
    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + object[i]);
        if (flat->hdr.type == BINDER_TYPE_FD) {
            if (!ashmemParcel->WriteFileDescriptor(flat->handle)) {
                ROSEN_LOGE("RSAshmemHelper::CopyFileDescriptor failed, fd:%d", flat->handle);
            }
        }
    }
}

void RSAshmemHelper::InjectFileDescriptor(std::shared_ptr<MessageParcel>& dataParcel, MessageParcel* ashmemParcel)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel->GetObjectOffsets());
    size_t objectNum = dataParcel->GetOffsetsSize();
    uintptr_t data = dataParcel->GetData();
    for (size_t i = 0; i < objectNum; i++) {
        flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + object[i]);
        if (flat->hdr.type == BINDER_TYPE_FD) {
            flat->handle = ashmemParcel->ReadFileDescriptor();
            if (flat->handle < 0) {
                ROSEN_LOGE("RSAshmemHelper::InjectFileDescriptor failed, fd:%d", flat->handle);
            }
        }
    }
}

std::shared_ptr<MessageParcel> RSAshmemHelper::CreateAshmemParcel(std::shared_ptr<MessageParcel>& dataParcel)
{
    size_t dataSize = dataParcel->GetDataSize();
    RS_TRACE_NAME("CreateAshmemParcel data size:" + std::to_string(dataSize));

    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    if (!ashmemAllocator) {
        ROSEN_LOGE("CreateAshmemParcel failed, ashmemAllocator is nullptr");
        return nullptr;
    }
    if (!ashmemAllocator->WriteToAshmem(reinterpret_cast<void*>(dataParcel->GetData()), dataSize)) {
        ROSEN_LOGE("CreateAshmemParcel: WriteToAshmem failed");
        return nullptr;
    }
    int fd = ashmemAllocator->GetFd();
    std::shared_ptr<MessageParcel> ashmemParcel = std::make_shared<MessageParcel>();
    ashmemParcel->WriteInt32(1); // indicate asheme parcel
    ashmemParcel->WriteInt32(dataSize);
    ashmemParcel->WriteFileDescriptor(fd);

    size_t offsetSize = dataParcel->GetOffsetsSize();
    ashmemParcel->WriteInt32(offsetSize);
    if (offsetSize > 0) {
        ashmemParcel->WriteBuffer(
            reinterpret_cast<void*>(dataParcel->GetObjectOffsets()), sizeof(binder_size_t) * offsetSize);
        CopyFileDescriptor(ashmemParcel, dataParcel);
    }

    return ashmemParcel;
}

std::shared_ptr<MessageParcel> RSAshmemHelper::ParseFromAshmemParcel(MessageParcel* ashmemParcel)
{
    int32_t dataSize = ashmemParcel->ReadInt32();
    RS_TRACE_NAME("ParseFromAshmemParcel data size:" + std::to_string(dataSize));

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
        dataParcel->InjectOffsets(reinterpret_cast<binder_size_t>(offsets), offsetSize);
        InjectFileDescriptor(dataParcel, ashmemParcel);
    }

    if (dataParcel->ReadInt32() != 0) { // identify data parcel
        ROSEN_LOGE("RSAshmemHelper::ParseFromAshmemParcel failed");
        return nullptr;
    }

    return dataParcel;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS
