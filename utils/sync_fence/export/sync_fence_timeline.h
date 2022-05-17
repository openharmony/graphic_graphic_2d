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

#ifndef UTILS_INCLUDE_SYNC_FENCE_TIMELINE_H
#define UTILS_INCLUDE_SYNC_FENCE_TIMELINE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <refbase.h>

#include "sync_fence.h"

namespace OHOS {
class SyncFenceTime {
public:
    explicit SyncFenceTime(const sptr<SyncFence>& fence);
    explicit SyncFenceTime(sptr<SyncFence>&& fence);

    // Do not allow default construction.
    SyncFenceTime() = delete;
    ~SyncFenceTime() = default;

    // Do not allow copy, assign, move
    SyncFenceTime(const SyncFence& rhs) = delete;
    SyncFenceTime& operator=(const SyncFence& rhs) = delete;
    SyncFenceTime(SyncFence&& rhs) = delete;
    SyncFenceTime& operator=(SyncFence&& rhs) = delete;

    bool IsValid() const;

    ns_sec_t GetSignalTimestamp();
    ns_sec_t GetCachedSignalTimestamp() const;
private:
    mutable std::mutex mutex_;
    sptr<SyncFence> fence_ {SyncFence::INVALID_FENCE};
    std::atomic<ns_sec_t> signaledTimestamps_ {SyncFence::INVALID_TIMESTAMP};
};

class SyncFenceTimeline {
public:
    static const size_t MAX_COUNT = 128;

    SyncFenceTimeline() = default;
    ~SyncFenceTimeline() = default;

    void Push(const std::shared_ptr<SyncFenceTime>& fence);
    void UpdateFenceTimeline();

private:
    mutable std::mutex mutex_;
    std::queue<std::weak_ptr<SyncFenceTime>> queue_;
};
} // namespace OHOS

#endif