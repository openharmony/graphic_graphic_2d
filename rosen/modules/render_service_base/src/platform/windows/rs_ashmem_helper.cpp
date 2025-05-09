/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Rosen {
std::unique_ptr<AshmemAllocator> AshmemAllocator::CreateAshmemAllocator(size_t size, int mapType)
{
    return {};
}

std::unique_ptr<AshmemAllocator> AshmemAllocator::CreateAshmemAllocatorWithFd(int fd, size_t size, int mapType)
{
    return {};
}

AshmemAllocator::AshmemAllocator(int fd, size_t size)
    : fd_(fd), size_(size)
{
}

AshmemAllocator::~AshmemAllocator()
{
}

bool AshmemAllocator::MapAshmem(int mapType)
{
    return {};
}

bool AshmemAllocator::WriteToAshmem(const void *data, size_t size)
{
    return {};
}

void* AshmemAllocator::CopyFromAshmem(size_t size)
{
    return {};
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
}

void* AshmemAllocator::Alloc(size_t size)
{
    return {};
}

void* AshmemAllocator::Realloc(void* data, size_t newSize)
{
    return {};
}

AshmemFdContainer& AshmemFdContainer::Instance()
{
    static AshmemFdContainer instance;
    return instance;
}

void AshmemFdContainer::SetIsUnmarshalThread(bool isUnmarshalThread)
{
}

int AshmemFdContainer::ReadSafeFd(Parcel &parcel, std::function<int(Parcel&)> readFdDefaultFunc)
{
    if (readFdDefaultFunc == nullptr) {
        return static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    }
    return readFdDefaultFunc(parcel);
}

void AshmemFdContainer::Merge(const std::unordered_map<binder_size_t, int>& fds)
{
}

void AshmemFdContainer::Clear()
{
    isUseFdContainer_ = false;
    fds_.clear();
}

std::string AshmemFdContainer::PrintFds() const
{
    return "";
}

AshmemFdWorker::AshmemFdWorker(const pid_t callingPid) : callingPid_(callingPid) {}

AshmemFdWorker::~AshmemFdWorker()
{
    isFdContainerUpdated_ = false;
    needManualCloseFds_ = false;
    fds_.clear();
    fdsToBeClosed_.clear();
}

void AshmemFdWorker::InsertFdWithOffset(int fd, binder_size_t offset, bool shouldCloseFd)
{
}

void AshmemFdWorker::PushFdsToContainer()
{
    (void)callingPid_;
}

void AshmemFdWorker::EnableManualCloseFds()
{
}

void RSAshmemHelper::CopyFileDescriptor(
    MessageParcel* ashmemParcel, std::shared_ptr<MessageParcel>& dataParcel)
{
}

void RSAshmemHelper::InjectFileDescriptor(std::shared_ptr<MessageParcel>& dataParcel, MessageParcel* ashmemParcel,
    std::unique_ptr<AshmemFdWorker>& ashmemFdWorker, pid_t callingPid)
{
}

std::shared_ptr<MessageParcel> RSAshmemHelper::CreateAshmemParcel(std::shared_ptr<MessageParcel>& dataParcel)
{
    return {};
}

std::shared_ptr<MessageParcel> RSAshmemHelper::ParseFromAshmemParcel(MessageParcel* ashmemParcel,
    std::unique_ptr<AshmemFdWorker>& ashmemFdWorker,
    std::shared_ptr<AshmemFlowControlUnit> &ashmemFlowControlUnit, pid_t callingPid)
{
    return {};
}
} // namespace Rosen
} // namespace OHOS
