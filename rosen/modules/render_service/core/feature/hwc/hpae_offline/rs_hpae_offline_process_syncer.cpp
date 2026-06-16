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
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<ProcessOfflineFuture> RSHpaeOfflineProcessSyncer::RegisterPostedTask(
    offlineTaskId taskId)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: Register task [%" PRIu64 "-%" PRIu64 "]", taskId.first, taskId.second);
    RS_OFFLINE_LOGD("Register task, taskId=[%{public}" PRIu64 "-%{public}" PRIu64 "]", taskId.first, taskId.second);
    {
        std::lock_guard<std::mutex> lock(poolMtx_);
        if (resultPool_.find(taskId) == resultPool_.end()) {
            auto future = std::make_shared<ProcessOfflineFuture>();
            resultPool_.insert({taskId, future});
            return future;
        }
    }
    RS_OFFLINE_LOGW("Register task, taskId=[%{public}" PRIu64 "-%{public}" PRIu64 "] already exists.",
         taskId.first, taskId.second);
    return nullptr;
}

void RSHpaeOfflineProcessSyncer::MarkTaskDoneAndSetResult(
    std::shared_ptr<ProcessOfflineFuture> futurePtr, const ProcessOfflineResult& processOfflineResult)
{
    {
        std::lock_guard<std::mutex> lock(futurePtr->mtx);
        futurePtr->done = true;
        futurePtr->result = processOfflineResult;
        futurePtr->cv.notify_one();
    }
}

std::shared_ptr<ProcessOfflineFuture> RSHpaeOfflineProcessSyncer::GetTaskFuture(offlineTaskId taskId)
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

bool RSHpaeOfflineProcessSyncer::WaitForTaskAndGetResult(offlineTaskId taskId,
    std::chrono::milliseconds timeout, ProcessOfflineResult& processOfflineResult)
{
    RS_TRACE_NAME_FMT("Wait for offline task.");
    {
        std::lock_guard<std::mutex> lock(poolMtx_);
        auto it = directResultCache_.find(taskId);
        if (it != directResultCache_.end()) {
            processOfflineResult = it->second;
            directResultCache_.erase(it);
            RS_OFFLINE_LOGD("task[%{public}" PRIu64 "-%{public}" PRIu64 "] use direct result",
                taskId.first, taskId.second);
            return true;
        }
    }
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
            RS_OFFLINE_LOGW("%{public}s, wait task[%{public}" PRIu64 "-%{public}" PRIu64 "] timeout!!!", __func__,
                taskId.first, taskId.second);
            return false;
        }
    }
    processOfflineResult = future->result;
    RS_OFFLINE_LOGD("wait for task[%{public}" PRIu64 "-%{public}" PRIu64 "] done!",
        taskId.first, taskId.second);
    return true;
}

int32_t RSHpaeOfflineProcessSyncer::GetResultPoolSize()
{
    std::lock_guard<std::mutex> lock(poolMtx_);
    return resultPool_.size() + directResultCache_.size();
}

int32_t RSHpaeOfflineProcessSyncer::GetResultPoolSizeByNode(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(poolMtx_);
    int32_t count = 0;
    for (auto it = resultPool_.begin(); it != resultPool_.end(); ++it) {
        if (it->first.second == nodeId) {
            count++;
        }
    }
    for (auto it = directResultCache_.begin(); it != directResultCache_.end(); ++it) {
        if (it->first.second == nodeId) {
            count++;
        }
    }
    return count;
}

void RSHpaeOfflineProcessSyncer::ClearResultPool()
{
    std::lock_guard<std::mutex> lock(poolMtx_);
    resultPool_.clear();
    directResultCache_.clear();
}

void RSHpaeOfflineProcessSyncer::SetDirectResult(offlineTaskId taskId, const ProcessOfflineResult& result)
{
    std::lock_guard<std::mutex> lock(poolMtx_);
    directResultCache_[taskId] = result;
}

} // namespace Rosen
} // namespace OHOS