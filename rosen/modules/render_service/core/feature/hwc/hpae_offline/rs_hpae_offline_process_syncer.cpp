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
#include "feature/hwc/hpae_offline/rs_hpae_offline_process_syncer.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_util.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<ProcessOfflineFuture> RSHpaeOfflineProcessSyncer::RegisterPostedTask(
    uint64_t taskId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: Register task [%" PRIu64 "].", taskId);
    RS_OFFLINE_LOGD("hpae_offline: Register task [%{public}" PRIu64 "].", taskId);
    {
        std::lock_guard<std::mutex> lock(poolMtx_);
        if (resultPool_.find(taskId) == resultPool_.end()) {
            auto future = std::make_shared<ProcessOfflineFuture>();
            resultPool_.insert({taskId, future});
            return future;
        }
    }
    RS_OFFLINE_LOGW("Task with %{public}" PRIu64 " already exists.", taskId);
    return nullptr;
}

void RSHpaeOfflineProcessSyncer::MarkTaskDoneAndSetResult(
    std::shared_ptr<ProcessOfflineFuture> futurePtr, const ProcessOfflineResult& processOfflineResult)
{
    RS_OFFLINE_LOGD("task done, set result and notify.");
    {
        std::lock_guard<std::mutex> lock(futurePtr->mtx);
        futurePtr->done = true;
        futurePtr->result = processOfflineResult;
        futurePtr->cv.notify_one();
    }
}

std::shared_ptr<ProcessOfflineFuture> RSHpaeOfflineProcessSyncer::GetTaskFuture(uint64_t taskId)
{
    std::lock_guard<std::mutex> lock(poolMtx_);
    auto iter = resultPool_.find(taskId);
    if (iter == resultPool_.end()) {
        RS_OFFLINE_LOGW("%{public}s, taskPool has not wanted task.", __func__);
        return nullptr;
    }
    auto future = std::move(iter->second);
    resultPool_.erase(iter);
    return future;
}

bool RSHpaeOfflineProcessSyncer::WaitForTaskAndGetResult(uint64_t taskId,
    std::chrono::milliseconds timeout, ProcessOfflineResult& processOfflineResult)
{
    RS_TRACE_NAME_FMT("Wait for offline task.", __func__);
    auto future = GetTaskFuture(taskId);
    if (!future) {
        RS_OFFLINE_LOGE("%{public}s, task is null.", __func__);
        return false;
    }

    std::unique_lock<std::mutex> lock(future->mtx);
    if (timeout == std::chrono::milliseconds::max()) {
        future->cv.wait(lock, [future] { return future->done; });
    } else {
        if (!future->cv.wait_for(lock, timeout, [future] { return future->done; })) {
            future->timeout = true;
            RS_OFFLINE_LOGW("%{public}s, wait task[%{public}" PRIu64 "] timeout!!!", __func__, taskId);
            return false;
        }
    }
    processOfflineResult = future->result;
    return true;
}

int32_t RSHpaeOfflineProcessSyncer::GetResultPoolSize()
{
    std::lock_guard<std::mutex> lock(poolMtx_);
    return resultPool_.size();
}

void RSHpaeOfflineProcessSyncer::ClearResultPool()
{
    std::lock_guard<std::mutex> lock(poolMtx_);
    resultPool_.clear();
}

} // namespace Rosen
} // namespace OHOS