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

#include "sync_fence_tracker.h"
#include "hilog/log.h"
#include "rs_trace.h"

namespace OHOS {
using namespace OHOS::HiviewDFX;
namespace {
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400
#undef LOG_TAG
#define LOG_TAG "SyncFence"
}

SyncFenceTracker::SyncFenceTracker(const std::string threadName)
    : threadName_(threadName),
    fencesQueued_(0),
    fencesSignaled_(0)
{
    runner_ = OHOS::AppExecFwk::EventRunner::Create(threadName_);
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);
}

void SyncFenceTracker::TrackFence(const sptr<SyncFence>& fence)
{
    if (fence->SyncFileReadTimestamp() != SyncFence::FENCE_PENDING_TIMESTAMP) {
        RS_TRACE_NAME_FMT("%s %d has signaled", threadName_.c_str(), fencesQueued_.load());
        fencesQueued_++;
        fencesSignaled_++;
        return;
    }

    RS_TRACE_NAME_FMT("%s %d", threadName_.c_str(), fencesQueued_.load());
    if (handler_) {
        handler_->PostTask([this, fence]() {
            Loop(fence);
        });
        fencesQueued_++;
    }
}

void SyncFenceTracker::Loop(const sptr<SyncFence>& fence)
{
    uint32_t fenceIndex = 0;
    fenceIndex = fencesSignaled_.load();
    {
        RS_TRACE_NAME_FMT("Waiting for %s %d", threadName_.c_str(), fenceIndex);
        int32_t result = fence->Wait(SYNC_TIME_OUT);
        if (result < 0) {
            HILOG_DEBUG(LOG_CORE, "Error waiting for SyncFence: %s", strerror(result));
        }
    }
    fencesSignaled_++;
}
} // namespace OHOS