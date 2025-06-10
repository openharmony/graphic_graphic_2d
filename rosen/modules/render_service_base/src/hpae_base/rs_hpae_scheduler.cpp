/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "hpae_base/rs_hpae_scheduler.h"
#include "hpae_base/rs_hpae_filter_cache_manager.h"
#include "hpae_base/rs_hpae_log.h"
#include "hpae_base/rs_hpae_perf_thread.h"

namespace OHOS::Rosen {

RSHpaeScheduler& RSHpaeScheduler::GetInstance()
{
    static RSHpaeScheduler scheduler_;
    return scheduler_;
}

void RSHpaeScheduler::Reset()
{
    cachedItem_ = {}; // reset
    hpaeFrameId_ = 0;
}

void RSHpaeScheduler::CacheHpaeItem(const HpaeBackgroundCacheItem& item)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cachedItem_ = item;
}

HpaeBackgroundCacheItem RSHpaeScheduler::GetCacheHpaeItem()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return cachedItem_;
}

void RSHpaeScheduler::SetHpaeFrameId(uint64_t frameId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    HPAE_TRACE_NAME_FMT("shenh: SetHpaeFrameId: %u", frameId);
    hpaeFrameId_ = frameId;
}

uint64_t RSHpaeScheduler::GetHpaeFrameId() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return hpaeFrameId_;
}

void RSHpaeScheduler::SetToWaitBuildTask()
{
    std::unique_lock<std::mutex> lock(buildTaskMutex_);
    buildTaskDone_ = false;
}

void RSHpaeScheduler::NotifyBuildTaskDone()
{
    std::unique_lock<std::mutex> lock(buildTaskMutex_);
    buildTaskDone_ = true;
    buildTaskCv_.notify_all();
}

void RSHpaeScheduler::WaitBulidTask()
{
    using namespace std::chrono_literals;
    std::unique_lock<std::mutex> lock(buildTaskMutex_);
    if (buildTaskDone_) {
        HPAE_TRACE_NAME_FMT("WaitBulidTask");
        buildTaskCv_.wait_for(lock, 10ms, [this]() {
            return this->buildTaskDone_;
        });
        buildTaskDone_ = true;
    }
}

} // OHOS::Rosen