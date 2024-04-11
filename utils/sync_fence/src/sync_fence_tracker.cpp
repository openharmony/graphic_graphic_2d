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

#include <ctime>
#include <cinttypes>

#include "sync_fence_tracker.h"
#include "hilog/log.h"
#include "rs_trace.h"
#include "hisysevent.h"
#include "file_ex.h"

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

bool SyncFenceTracker::CheckGpuSubhealthEventLimit()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    if (gpuSubhealthEventNum == 0 || tm.tm_yday > gpuSubhealthEventDay) {
        gpuSubhealthEventDay = tm.tm_yday;
        gpuSubhealthEventNum = 0;
        HILOG_DEBUG(LOG_CORE, "first event of %{public}" PRId32, gpuSubhealthEventDay);
        gpuSubhealthEventNum++;
        return true;
    } else if (gpuSubhealthEventNum < GPU_SUBHEALTH_EVENT_LIMIT) {
        gpuSubhealthEventNum++;
        HILOG_DEBUG(LOG_CORE, "%{public}" PRId32 "event of %{public}" PRId32 "day",
            gpuSubhealthEventNum, gpuSubhealthEventDay);
        return true;
    }
    HILOG_DEBUG(LOG_CORE, "%{public}" PRId32 "event exceed %{public}" PRId32 "day",
        gpuSubhealthEventNum, gpuSubhealthEventDay);
    return false;
}

inline int32_t SyncFenceTracker::GetValue(const std::string& fileName)
{
    std::string content;
    OHOS::LoadStringFromFile(fileName, content);
    int32_t parseVal = 0;
    std::stringstream ss(content);
    HILOG_DEBUG(LOG_CORE, "GetValue content is %{public}s", ss.str().c_str());
    ss >> parseVal;
    HILOG_DEBUG(LOG_CORE, "GetValue parseVal is %{public}" PRId32, parseVal);
    return parseVal;
}

void SyncFenceTracker::ReportEventGpuhealth(int32_t duration)
{
    auto reportName = "GPU_SUBHEALTH_MONITORING";
    RS_TRACE_NAME_FMT("RSJankStats::ReportEventGpuSubhealth");
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "WAIT_ACQUIRE_FENCE_TIME", duration,
        "GPU_LOAD", GetValue(GPU_LOAD));
}

void SyncFenceTracker::Loop(const sptr<SyncFence>& fence)
{
    uint32_t fenceIndex = 0;
    fenceIndex = fencesSignaled_.load();
    {
        RS_TRACE_NAME_FMT("Waiting for %s %d", threadName_.c_str(), fenceIndex);
        int32_t result;
        
        if (threadName_.compare("Acquire Fence") == 0) {
            int32_t startTimestamp = static_cast<int32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
            result = fence->Wait(SYNC_TIME_OUT);
            int32_t endTimestamp = static_cast<int32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
            int32_t duration = endTimestamp - startTimestamp;
            HILOG_DEBUG(LOG_CORE, "Waiting for Acquire Fence: %{public}" PRId32 "ms", duration);
            if (duration > GPU_SUBHEALTH_EVENT_THRESHOLD && CheckGpuSubhealthEventLimit()) {
                ReportEventGpuSubhealth(duration);
            }
        } else {
            result = fence->Wait(SYNC_TIME_OUT);
        }

        if (result < 0) {
            HILOG_DEBUG(LOG_CORE, "Error waiting for SyncFence: %s", strerror(result));
        }
    }
    fencesSignaled_++;
}
} // namespace OHOS