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

#ifndef RENDER_SERVICE_BASE_TRANSACTION_RS_ASHMEM_HELPER_H
#define RENDER_SERVICE_BASE_TRANSACTION_RS_ASHMEM_HELPER_H

#include <message_parcel.h>
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "memory/rs_memory_flow_control.h"

namespace OHOS {
namespace Rosen {
class AshmemAllocator : public Allocator {
public:
    static std::unique_ptr<AshmemAllocator> CreateAshmemAllocator(size_t size, int mapType);
    static std::unique_ptr<AshmemAllocator> CreateAshmemAllocatorWithFd(int fd, size_t size, int mapType);
    bool MapAshmem(int mapType);
    bool WriteToAshmem(const void *data, size_t size);
    void* CopyFromAshmem(size_t size);

    int GetFd() const; // the fd is only valid during the object lifetime
    size_t GetSize() const;
    void* GetData() const;

    AshmemAllocator(int fd, size_t size);
    ~AshmemAllocator() override;
    void Dealloc(void* data) override;
    void* Alloc(size_t size) override;
    void* Realloc(void* data, size_t newSize) override;

private:
    int fd_;
    size_t size_;
    void* data_ = nullptr;
};

class AshmemFdWorker;
class RSB_EXPORT AshmemFdContainer {
public:
    static AshmemFdContainer& Instance();
    ~AshmemFdContainer() = default;
    static void SetIsUnmarshalThread(bool isUnmarshalThread);
    int ReadSafeFd(Parcel& parcel, std::function<int(Parcel&)> readFdDefaultFunc = nullptr);

private:
    AshmemFdContainer() = default;
    DISALLOW_COPY_AND_MOVE(AshmemFdContainer);

    void Merge(const std::unordered_map<binder_size_t, int>& fds);
    void Clear();
    std::string PrintFds() const;

    std::unordered_map<binder_size_t, int> fds_;
    bool isUseFdContainer_ = false;

    friend class AshmemFdWorker;
};

class RSB_EXPORT AshmemFdWorker {
public:
    explicit AshmemFdWorker(const pid_t callingPid);
    ~AshmemFdWorker();

    void InsertFdWithOffset(int fd, binder_size_t offset, bool shouldCloseFd);
    void PushFdsToContainer();
    void EnableManualCloseFds();

private:
    DISALLOW_COPY_AND_MOVE(AshmemFdWorker);

    const pid_t callingPid_;

    std::unordered_map<binder_size_t, int> fds_;
    std::unordered_set<int> fdsToBeClosed_;
    bool isFdContainerUpdated_ = false;
    bool needManualCloseFds_ = false;
};

class RSB_EXPORT RSAshmemHelper {
public:
    static std::shared_ptr<MessageParcel> CreateAshmemParcel(std::shared_ptr<MessageParcel>& dataParcel);
    static std::shared_ptr<MessageParcel> ParseFromAshmemParcel(MessageParcel* ashmemParcel,
        std::unique_ptr<AshmemFdWorker>& ashmemFdWorker,
        std::shared_ptr<AshmemFlowControlUnit> &ashmemFlowControlUnit, pid_t callingPid = 0);

    static void CopyFileDescriptor(
        MessageParcel* ashmemParcel, std::shared_ptr<MessageParcel>& dataParcel);
    static void InjectFileDescriptor(std::shared_ptr<MessageParcel>& dataParcel, MessageParcel* ashmemParcel,
        std::unique_ptr<AshmemFdWorker>& ashmemFdWorker, pid_t callingPid = 0);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_TRANSACTION_RS_ASHMEM_HELPER_H
