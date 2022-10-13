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

#include "sync_fence_timeline.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <utility>

namespace OHOS {
SyncFenceTime::SyncFenceTime(const sptr<SyncFence>& fence) : fence_(fence)
{
    signaledTimestamps_ = fence->IsValid() ? SyncFence::FENCE_PENDING_TIMESTAMP :
        SyncFence::INVALID_TIMESTAMP;
}

SyncFenceTime::SyncFenceTime(sptr<SyncFence>&& fence) : fence_(std::move(fence))
{
    signaledTimestamps_ = fence_->IsValid() ? SyncFence::FENCE_PENDING_TIMESTAMP :
        SyncFence::INVALID_TIMESTAMP;
}

bool SyncFenceTime::IsValid() const
{
    return fence_->IsValid();
}

ns_sec_t SyncFenceTime::GetSignalTimestamp()
{
    ns_sec_t timestamp = signaledTimestamps_.load(std::memory_order_relaxed);
    if (timestamp != SyncFence::FENCE_PENDING_TIMESTAMP) {
        return timestamp;
    }

    sptr<SyncFence> fence;
    
    std::lock_guard<std::mutex> lock(mutex_);
    if (fence_.GetRefPtr() == nullptr) {
        return signaledTimestamps_.load(std::memory_order_relaxed);
    }
    fence = fence_;

    timestamp = fence->SyncFileReadTimestamp();
    if (timestamp != SyncFence::FENCE_PENDING_TIMESTAMP) {
        std::lock_guard<std::mutex> lockTimestampStore(mutex_);
        fence_.clear();
        signaledTimestamps_.store(timestamp, std::memory_order_relaxed);
    }
    return timestamp;
}

ns_sec_t SyncFenceTime::GetCachedSignalTimestamp() const
{
    return signaledTimestamps_.load(std::memory_order_relaxed);
}

void SyncFenceTimeline::Push(const std::shared_ptr<SyncFenceTime> &fence)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.size() >= MAX_COUNT) {
        std::shared_ptr<SyncFenceTime> front = queue_.front().lock();
        if (front) {
            front->GetSignalTimestamp();
        }
        queue_.pop();
    }
    queue_.push(fence);
}

void SyncFenceTimeline::UpdateFenceTimeline()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queue_.empty()) {
        std::shared_ptr<SyncFenceTime> fenceTime = queue_.front().lock();
        if (!fenceTime) {
            queue_.pop();
            continue;
        } else if (fenceTime->GetSignalTimestamp() != SyncFence::FENCE_PENDING_TIMESTAMP) {
            queue_.pop();
            continue;
        } else {
            // the fence is not signal yet. and later ones are not signal either.
            break;
        }
    }
}
} // namespace OHOS

