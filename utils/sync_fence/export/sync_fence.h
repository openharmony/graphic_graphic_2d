/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef UTILS_INCLUDE_SYNC_FENCE_H
#define UTILS_INCLUDE_SYNC_FENCE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <mutex>
#include <vector>

#include <refbase.h>
#include <message_parcel.h>
#include <unique_fd.h>

namespace OHOS {
// same to linux/sync_file define
enum FenceStatus {
    ERROR = -1,
    ACTIVE = 0,
    SIGNALED = 1
};

struct SyncPointInfo {
    uint64_t timestampNs;
    FenceStatus status;
};

class SyncTimeline : public RefBase {
public:
    SyncTimeline() noexcept;
    ~SyncTimeline() noexcept;

    SyncTimeline(const SyncTimeline& rhs) = delete;
    SyncTimeline& operator=(const SyncTimeline& rhs) = delete;
    SyncTimeline(SyncTimeline&& rhs) = delete;
    SyncTimeline& operator=(SyncTimeline&& rhs) = delete;

    bool IsValid();
    int32_t IncreaseSyncPoint(uint32_t step = 1);
    int32_t GenerateFence(std::string name, uint32_t point);
    int32_t Dup() const;
    /* this is dangerous, when you use it, do not operator the fd */
    int32_t Get() const;

private:
    int32_t timeLineFd_ = -1;
    int32_t timeLineNextPoint = 0;
    bool isValid_ = false;
};

class SyncFence : public RefBase {
public:
    explicit SyncFence(int32_t fenceFd);
    /* When the SyncFence is destroyed, the fd will be closed in UniqueFd */
    virtual ~SyncFence();

    SyncFence(const SyncFence& rhs) = delete;
    SyncFence& operator=(const SyncFence& rhs) = delete;
    SyncFence(SyncFence&& rhs) = delete;
    SyncFence& operator=(SyncFence&& rhs) = delete;

    static const sptr<SyncFence> INVALID_FENCE;
    int32_t Wait(uint32_t timeout);
    static sptr<SyncFence> MergeFence(const std::string &name,
            const sptr<SyncFence>& fence1, const sptr<SyncFence>& fence2);
    int64_t SyncFileReadTimestamp();
    int32_t Dup() const;

    /* this is dangerous, when you use it, do not operator the fd */
    int32_t Get() const;

    void WriteToMessageParcel(MessageParcel &parcel);
    void ReadFromMessageParcel(MessageParcel &parcel);

private:
    std::vector<SyncPointInfo> GetFenceInfo();
    FenceStatus GetStatus();

    UniqueFd fenceFd_;
};

}

#endif // UTILS_INCLUDE_SYNC_FENCE_H