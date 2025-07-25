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

#include "hgm_task_handle_thread.h"

#include <unistd.h>

#include "ffrt_inner.h"
#include "hgm_log.h"
#include "xcollie/watchdog.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t WATCHDOG_TIMEVAL = 30000;
constexpr uint32_t WATCHDOG_DELAY_TIME = 600000;
constexpr uint64_t MILLISECONDS_TO_MICROSECONDS = 1000;
constexpr uint64_t MAX_ALLOWED_DELAY = UINT64_MAX / 1000;

uint64_t ConvertMillisecondsToMicroseconds(int64_t delayTime)
{
    if (delayTime < 0) {
        delayTime = 0;
    } else if (delayTime > MAX_ALLOWED_DELAY) {
        delayTime = MAX_ALLOWED_DELAY;
    }
    return static_cast<uint64_t>(delayTime) * MILLISECONDS_TO_MICROSECONDS;
}
}

HgmTaskHandleThread& HgmTaskHandleThread::Instance()
{
    static HgmTaskHandleThread instance;
    return instance;
}

HgmTaskHandleThread::HgmTaskHandleThread()
{
    queue_ = std::make_shared<ffrt::queue>(
        static_cast<ffrt::queue_type>(ffrt_inner_queue_type_t::ffrt_queue_eventhandler_adapter), "HgmTaskHandleThread",
        ffrt::queue_attr().qos(ffrt::qos_user_interactive));
    PostTask([]() { HiviewDFX::Watchdog::GetInstance().InitFfrtWatchdog(); }, WATCHDOG_DELAY_TIME);
}

void HgmTaskHandleThread::PostTask(const std::function<void()>& task, int64_t delayTime)
{
    if (queue_) {
        auto microDelayTime = ConvertMillisecondsToMicroseconds(delayTime);
        queue_->submit(
            std::move(task), ffrt::task_attr()
                                 .delay(microDelayTime)
                                 .priority(static_cast<ffrt_queue_priority_t>(ffrt_inner_queue_priority_immediate)));
    }
}

bool HgmTaskHandleThread::PostSyncTask(const std::function<void()>& task)
{
    if (queue_) {
        auto handle = queue_->submit_h(std::move(task),
            ffrt::task_attr().priority(static_cast<ffrt_queue_priority_t>(ffrt_inner_queue_priority_vip)));
        queue_->wait(handle);
        return true;
    }
    return false;
}

void HgmTaskHandleThread::PostEvent(std::string eventId, const std::function<void()>& task, int64_t delayTime)
{
    if (queue_) {
        auto microDelayTime = ConvertMillisecondsToMicroseconds(delayTime);
        queue_->submit(std::move(task), ffrt::task_attr().name(eventId.c_str()).delay(microDelayTime));
    }
}

void HgmTaskHandleThread::RemoveEvent(std::string eventId)
{
    if (queue_) {
        ffrt_queue_t* queue = reinterpret_cast<ffrt_queue_t*>(queue_.get());
        ffrt_queue_cancel_by_name(*queue, eventId.c_str());
    }
}

void HgmTaskHandleThread::DetectMultiThreadingCalls()
{
    if (auto newTid = gettid(); curThreadId_ != newTid) {
        // -1 means default curThreadId
        if (curThreadId_ != -1) {
            HGM_LOGE("Concurrent access tid1: %{public}d tid2: %{public}d", curThreadId_, newTid);
        }
        curThreadId_ = newTid;
    }
}
}
