/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_virtual_screen_parallel_manager.h"
#include <cinttypes>
#include "drawable/rs_screen_render_node_drawable.h"
#include "ffrt_inner.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "virtual_screen_parallel_param.h"

namespace OHOS {
namespace Rosen {
constexpr int32_t VIRTUAL_SCREEN_TASK_WAIT_TIMEOUT_MS = 1000;
constexpr int32_t MAX_VIRTUAL_SCREEN_COUNT = 99;

void RSVirtualScreenParallelManager::CollectVirtualScreenNodeId(ScreenId screenId,
    NodeId nodeId, CompositeType compositeType)
{
    if (!VirtualScreenParallelParam::IsVirtualScreenParallelEnabled() ||
        !RSSystemProperties::GetVirtualScreenParallelEnabled() ||
        compositeType != CompositeType::UNI_RENDER_VIRTUAL_INDEPENDENT_COMPOSITE) {
        return;
    }
    RS_TRACE_NAME_FMT("%s collect node id: %" PRIu64 " screen id: %" PRIu64, __func__, nodeId, screenId);
    stagingNodeIds_.insert(nodeId);
}

void RSVirtualScreenParallelManager::GetStagingNodeIds(std::unordered_set<NodeId>& nodeIds)
{
    nodeIds = std::move(stagingNodeIds_);
}

void RSVirtualScreenParallelManager::SyncCollectedInfo(const std::unordered_set<NodeId>& nodeIds)
{
    virtualScreenNodeIds_ = std::move(nodeIds);
}

bool RSVirtualScreenParallelManager::ShouldSkipRenderNodeOnDraw(NodeId nodeId)
{
    return virtualScreenNodeIds_.count(nodeId) > 0;
}

RSVirtualScreenParallelManager::ScreenDrawableInfo RSVirtualScreenParallelManager::GetScreenDrawableInfo(NodeId nodeId)
{
    ScreenDrawableInfo info;
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::GetDrawableById(nodeId);
    if (!drawable) {
        return info;
    }
    info.drawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
    if (!info.drawable) {
        return info;
    }
    info.params = static_cast<RSScreenRenderParams*>(info.drawable->GetRenderParams().get());
    if (info.params) {
        info.screenId = info.params->GetScreenId();
    }
    return info;
}

std::shared_ptr<ffrt::queue> RSVirtualScreenParallelManager::AssignThreadIndex(ScreenId screenId)
{
    {
        std::unique_lock<ffrt::mutex> lock(taskMutex_);
        auto iter = ffrtThreadIndexMap_.find(screenId);
        if (iter != ffrtThreadIndexMap_.end()) {
            return iter->second;
        }
    }
    RS_TRACE_NAME_FMT("%s assign screen id: %" PRIu64, __func__, screenId);
    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;
    InitializeThread(screenId, ffrtThread);
    {
        std::unique_lock<ffrt::mutex> lock(taskMutex_);
        ffrtThreadIndexMap_[screenId] = ffrtThread;
    }
    return ffrtThread;
}

void RSVirtualScreenParallelManager::VirtualScreenRenderTask(
    std::shared_ptr<RSRenderThreadParams> renderThreadParams,
    ScreenDrawableInfo info, int32_t tid)
{
    RS_TRACE_NAME_FMT("virtual expand screen submit task, screen id: %" PRIu64 " tid %d", info.screenId, tid);
    auto uniParam = std::make_unique<RSRenderThreadParams>(*renderThreadParams);
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniParam));
    auto engine = GetVirtualScreenRenderEngine(info.screenId);
    if (engine) {
        info.drawable->OnDrawVirtualExpand(engine, tid);
    }
    RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    DecrementPendingTaskCount(info.screenId);
}

void RSVirtualScreenParallelManager::ExecuteAllVirtualScreenRenderTasks(
    std::unique_ptr<RSRenderThreadParams> stagingRenderThreadParams)
{
    SyncCollectedInfo(std::move(stagingRenderThreadParams->GetCollectedVirtualScreenNodeIds()));
    if (virtualScreenNodeIds_.empty()) {
        return;
    }

    IncrementPendingTaskCount(virtualScreenNodeIds_.size());
    RS_TRACE_NAME_FMT("%s virtualScreenNodeIds size %zu", __func__, virtualScreenNodeIds_.size());
    auto renderThreadParams = std::shared_ptr<RSRenderThreadParams>(stagingRenderThreadParams.release());
    for (auto nodeId : virtualScreenNodeIds_) {
        auto info = GetScreenDrawableInfo(nodeId);
        if (!info.IsValid()) {
            DecrementPendingTaskCount(info.screenId);
            RS_LOGE("%{public}s screen param invalid, screen id: %{public}" PRIu64, __func__, info.screenId);
            continue;
        }

        auto ffrtThread = AssignThreadIndex(info.screenId);
        if (ffrtThread == nullptr) {
            RS_LOGE("%{public}s not find thread index for screen id: %{public}" PRIu64, __func__, info.screenId);
            DecrementPendingTaskCount(info.screenId);
            continue;
        }

        RS_TRACE_NAME_FMT("screen id: %" PRIu64, info.screenId);
        int32_t tid = 0;
        {
            std::unique_lock<ffrt::mutex> lock(taskMutex_);
            screenTaskCountMap_[info.screenId]++;
            auto it = screenIdToTidMap_.find(info.screenId);
            if (it != screenIdToTidMap_.end()) {
                tid = it->second;
            } else {
                RS_LOGE("%{public}s not find tid for screen id: %{public}" PRIu64, __func__, info.screenId);
                DecrementPendingTaskCount(info.screenId);
                continue;
            }
        }
        ffrtThread->submit([this, renderThreadParams, info, tid]() {
            VirtualScreenRenderTask(renderThreadParams, info, tid);
        }, ffrt::task_attr().qos(ffrt_qos_user_interactive));
    }
}

void RSVirtualScreenParallelManager::WaitForAllVirtualScreenRenderTasksComplete()
{
    std::unique_lock<ffrt::mutex> lock(taskMutex_);
    RS_TRACE_NAME_FMT("%s task count: %zu", __func__, pendingTaskCount_);
    constexpr auto timeoutMs = std::chrono::milliseconds(VIRTUAL_SCREEN_TASK_WAIT_TIMEOUT_MS);
    if (!taskCondition_.wait_for(lock, timeoutMs, [this]() {
        return pendingTaskCount_ == 0;
    })) {
        RS_LOGW("RSVirtualScreenParallelManager::%{public}s timeout, pendingTaskCount: %{public}zu",
            __func__, pendingTaskCount_);
    }
    virtualScreenNodeIds_.clear();
}

std::shared_ptr<RSUniRenderEngine> RSVirtualScreenParallelManager::GetVirtualScreenRenderEngine(ScreenId screenId)
{
    std::unique_lock<ffrt::mutex> lock(taskMutex_);
    auto it = uniRenderEngineMap_.find(screenId);
    return it != uniRenderEngineMap_.end() ? it->second : nullptr;
}

void RSVirtualScreenParallelManager::CleanupThreadResources(ScreenId screenId)
{
    RS_TRACE_NAME_FMT("%s screenId: %" PRIu64, __func__, screenId);
    RS_LOGI("%{public}s screenId: %{public}" PRIu64, __func__, screenId);
    std::shared_ptr<ffrt::queue> ffrtThread = nullptr;
    int32_t tid;
    {
        std::unique_lock<ffrt::mutex> lock(taskMutex_);
        constexpr auto timeoutMs = std::chrono::milliseconds(VIRTUAL_SCREEN_TASK_WAIT_TIMEOUT_MS);
        bool allTaskCompleted = false;
        while (!allTaskCompleted) {
            allTaskCompleted = screenTaskCondition_.wait_for(lock, timeoutMs, [this, screenId]() {
                auto it = screenTaskCountMap_.find(screenId);
                return it == screenTaskCountMap_.end() || it->second == 0;
            });
            if (!allTaskCompleted) {
                RS_LOGW("RSVirtualScreenParallelManager::%{public}s timeout, screenId: %{public}" PRIu64,
                    __func__, screenId);
            }
        }
        auto iterFfrt = ffrtThreadIndexMap_.find(screenId);
        if (iterFfrt != ffrtThreadIndexMap_.end()) {
            RS_TRACE_NAME_FMT("%s erase screenId: %" PRIu64, __func__, screenId);
            ffrtThread = iterFfrt->second;
            ffrtThreadIndexMap_.erase(iterFfrt);
        }
        screenTaskCountMap_.erase(screenId);
        auto iterTid = screenIdToTidMap_.find(screenId);
        if (iterTid != screenIdToTidMap_.end()) {
            tid = iterTid->second;
            usedTidSet_.erase(tid);
            screenIdToTidMap_.erase(iterTid);
            virtualScreenCnt_--;
        }
    }
    if (ffrtThread != nullptr) {
        auto handle = ffrtThread->submit_h([this, screenId, tid]() {
            RS_TRACE_NAME_FMT("CleanupThreadResources erase engine screenId: %" PRIu64, screenId);
            {
                std::unique_lock<ffrt::mutex> lock(taskMutex_);
                tidToUniRenderEngineMap_.erase(tid);
                uniRenderEngineMap_.erase(screenId);
            }
        }, ffrt::task_attr().qos(ffrt_qos_user_interactive));
        ffrtThread->wait(handle);
    }
}

void RSVirtualScreenParallelManager::InitializeThread(ScreenId screenId, std::shared_ptr<ffrt::queue>& ffrtThread)
{
    RS_TRACE_NAME_FMT("%s post task screenId: %" PRIu64, __func__, screenId);
    RS_LOGI("%{public}s post task screenId: %{public}" PRIu64, __func__, screenId);
    int32_t tid = 0;
    {
        std::unique_lock<ffrt::mutex> lock(taskMutex_);
        if (virtualScreenCnt_ >= MAX_VIRTUAL_SCREEN_COUNT) {
            RS_LOGE("%{public}s virtual screen count more than max, screenId: %{public}" PRIu64, __func__, screenId);
            return;
        }
        /* The range of 'tid' should be between -200 and -299. */
        bool tidFound = false;
        for (tid = -RSVirtualScreenThreadIdAdapt::VIRTUAL_SCREEN_THREAD_INDEX.first;
            tid > -RSVirtualScreenThreadIdAdapt::VIRTUAL_SCREEN_THREAD_INDEX.second; tid--) {
            if (usedTidSet_.find(tid) == usedTidSet_.end()) {
                usedTidSet_.insert(tid);
                tidFound = true;
                virtualScreenCnt_++;
                break;
            }
        }
        if (!tidFound) {
            RS_LOGE("No available tid in the range -200 to -300 for screenId: %" PRIu64, screenId);
            return;
        }
        screenIdToTidMap_[screenId] = tid;
    }
    std::string threadName = "virtualExtended_" + std::to_string(screenId);
    ffrtThread = std::make_shared<ffrt::queue>(threadName.c_str());
    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    auto handle = ffrtThread->submit_h([this, &screenId, &renderEngine, &tid]() {
        RS_TRACE_NAME_FMT("InitializeThread init engine screenId: %" PRIu64 "tid: %d", screenId, tid);
        renderEngine->Init(RenderEngineType::BASIC_RENDER, tid);
        {
            std::unique_lock<ffrt::mutex> lock(taskMutex_);
            uniRenderEngineMap_[screenId] = renderEngine;
            tidToUniRenderEngineMap_[tid] = renderEngine;
        }
    }, ffrt::task_attr().qos(ffrt_qos_user_interactive));
    ffrtThread->wait(handle);
}

void RSVirtualScreenParallelManager::IncrementPendingTaskCount(size_t nodeCount)
{
    std::unique_lock<ffrt::mutex> lock(taskMutex_);
    RS_TRACE_NAME_FMT("%s pendingTaskCount_ %zu, nodeCount: %zu", __func__, pendingTaskCount_, nodeCount);
    pendingTaskCount_ += nodeCount;
}

void RSVirtualScreenParallelManager::DecrementPendingTaskCount(ScreenId screenId)
{
    std::unique_lock<ffrt::mutex> lock(taskMutex_);
    if (pendingTaskCount_ > 0) {
        pendingTaskCount_--;
    }
    if (screenId != INVALID_SCREEN_ID) {
        auto it = screenTaskCountMap_.find(screenId);
        if (it != screenTaskCountMap_.end() && it->second > 0) {
            it->second--;
            if (it->second == 0) {
                screenTaskCondition_.notify_all();
            }
        }
    }
    if (pendingTaskCount_ == 0) {
        RS_TRACE_NAME_FMT("%s notify finish", __func__);
        taskCondition_.notify_all();
    }
}

bool RSVirtualScreenParallelManager::GetRenderEngineByTid(int32_t tid,
    std::shared_ptr<RSBaseRenderEngine>& renderEngine)
{
    std::unique_lock<ffrt::mutex> lock(taskMutex_);
    auto iter = tidToUniRenderEngineMap_.find(tid);
    if (iter == tidToUniRenderEngineMap_.end()) {
        return false;
    }
    renderEngine = iter->second;
    return true;
}
} // namespace Rosen
} // namespace OHOS