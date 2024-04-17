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

#include "rs_sub_thread_manager.h"
#include <chrono>
#include "rs_trace.h"

#include "common/rs_singleton.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_task_dispatcher.h"
#include "memory/rs_memory_manager.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "pipeline/round_corner_display/rs_sub_thread_rcd.h"
#include "pipeline/round_corner_display/rs_message_bus.h"

namespace OHOS::Rosen {
static constexpr uint32_t SUB_THREAD_NUM = 3;
static constexpr uint32_t WAIT_NODE_TASK_TIMEOUT = 5 * 1000; // 5s
constexpr const char* RELEASE_RESOURCE = "releaseResource";
constexpr const char* RELEASE_TEXTURE = "releaseTexture";

RSSubThreadManager* RSSubThreadManager::Instance()
{
    static RSSubThreadManager instance;
    return &instance;
}

void RSSubThreadManager::Start(RenderContext *context)
{
    if (!threadList_.empty()) {
        return;
    }
    renderContext_ = context;
    if (context) {
        for (uint32_t i = 0; i < SUB_THREAD_NUM; ++i) {
            auto curThread = std::make_shared<RSSubThread>(context, i);
            auto tid = curThread->Start();
            threadIndexMap_.emplace(tid, i);
            reThreadIndexMap_.emplace(i, tid);
            threadList_.push_back(curThread);
            auto taskDispatchFunc = [tid, this](const RSTaskDispatcher::RSTask& task, bool isSyncTask = false) {
                RSSubThreadManager::Instance()->PostTask(task, threadIndexMap_[tid], isSyncTask);
            };
            RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(tid, taskDispatchFunc);
        }
    }
}

void RSSubThreadManager::StartColorPickerThread(RenderContext* context)
{
#if defined(NEW_SKIA) && (defined(RS_ENABLE_GL) || defined (RS_ENABLE_VK))
    if (!RSSystemProperties::GetColorPickerPartialEnabled() || !RSUniRenderJudgement::IsUniRender()) {
        RS_LOGD("RSSubThreadManager::StartColorPickerThread:Filter thread not run");
        return;
    }
    if (colorPickerThread_ != nullptr) {
        return;
    }
    renderContext_ = context;
    if (context) {
        colorPickerThread_ = std::make_shared<RSFilterSubThread>(context);
        colorPickerThread_->StartColorPicker();
    }
#endif
}

void RSSubThreadManager::StartRCDThread(RenderContext* context)
{
    renderContext_ = context;
    if (context) {
        RS_LOGD("RSSubThreadManager::StartRCDThread");
        auto threadRcd = &(RSSingleton<RSSubThreadRCD>::GetInstance());
        threadRcd->Start(context);
        if (!isRcdServiceRegister_) {
            auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
            auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
            msgBus.RegisterTopic<uint32_t, uint32_t>(
                TOPIC_RCD_DISPLAY_SIZE, &rcdInstance,
                &RoundCornerDisplay::UpdateDisplayParameter);
            msgBus.RegisterTopic<ScreenRotation>(
                TOPIC_RCD_DISPLAY_ROTATION, &rcdInstance,
                &RoundCornerDisplay::UpdateOrientationStatus);
            msgBus.RegisterTopic<int>(
                TOPIC_RCD_DISPLAY_NOTCH, &rcdInstance,
                &RoundCornerDisplay::UpdateNotchStatus);
            isRcdServiceRegister_ = true;
            RS_LOGD("RSSubThreadManager::StartRCDThread Registed rcd renderservice end");
        }
        RS_LOGD("RSSubThreadManager::StartRCDThread Registed rcd renderservice already.");
    }
}

void RSSubThreadManager::PostTask(const std::function<void()>& task, uint32_t threadIndex, bool isSyncTask)
{
    if (threadIndex >= threadList_.size()) {
        RS_LOGE("taskIndex geq thread num");
        return;
    }
    if (isSyncTask) {
        threadList_[threadIndex]->PostSyncTask(task);
    } else {
        threadList_[threadIndex]->PostTask(task);
    }
}

void RSSubThreadManager::DumpMem(DfxString& log)
{
    if (threadList_.empty()) {
        return;
    }
    for (auto subThread : threadList_) {
        if (!subThread) {
            continue;
        }
        subThread->DumpMem(log);
    }
    if (colorPickerThread_) {
        colorPickerThread_->DumpMem(log);
    }
}

float RSSubThreadManager::GetAppGpuMemoryInMB()
{
    if (threadList_.empty()) {
        return 0.f;
    }
    float total = 0.f;
    for (auto& subThread : threadList_) {
        if (!subThread) {
            continue;
        }
        total += subThread->GetAppGpuMemoryInMB();
    }
    if (colorPickerThread_) {
        total += colorPickerThread_->GetAppGpuMemoryInMB();
    }
    return total;
}

void RSSubThreadManager::SubmitFilterSubThreadTask()
{
    if (filterThread) {
        filterThread->FlushAndSubmit();
    }
}

void RSSubThreadManager::SubmitSubThreadTask(const std::shared_ptr<RSDisplayRenderNode>& node,
    const std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    RS_TRACE_NAME("RSSubThreadManager::SubmitSubThreadTask");
    bool ifNeedRequestNextVsync = false;

    if (node == nullptr) {
        ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask display node is null");
        return;
    }
    if (subThreadNodes.empty()) {
        return;
    }
    CancelReleaseTextureTask();
    CancelReleaseResourceTask();
    std::vector<std::unique_ptr<RSRenderTask>> renderTaskList;
    auto cacheSkippedNodeMap = RSMainThread::Instance()->GetCacheCmdSkippedNodes();
    for (const auto& child : subThreadNodes) {
        if (!child) {
            ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask !child");
            continue;
        }
        if (!child->ShouldPaint()) {
            RS_OPTIONAL_TRACE_NAME_FMT("SubmitTask skip node: [%s, %llu]", child->GetName().c_str(), child->GetId());
            ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask child->ShouldPaint()");
            continue;
        }
        if (!child->GetNeedSubmitSubThread()) {
            RS_OPTIONAL_TRACE_NAME_FMT("subThreadNodes : static skip %s", child->GetName().c_str());
            ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask !child->GetNeedSubmitSubThread()");
            continue;
        }
        if (cacheSkippedNodeMap.count(child->GetId()) != 0 && child->HasCachedTexture()) {
            RS_OPTIONAL_TRACE_NAME_FMT("SubmitTask cacheCmdSkippedNode: [%s, %llu]",
                child->GetName().c_str(), child->GetId());
            ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask "
                "cacheSkippedNodeMap.count(child->GetId()) != 0 && child->HasCachedTexture()");
            continue;
        }
        nodeTaskState_[child->GetId()] = 1;
        if (child->GetCacheSurfaceProcessedStatus() != CacheProcessStatus::DOING) {
            child->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
        }
        renderTaskList.push_back(std::make_unique<RSRenderTask>(*child, RSRenderTask::RenderNodeStage::CACHE));
    }
    if (renderTaskList.size()) {
        ifNeedRequestNextVsync = true;
    }

    std::vector<std::shared_ptr<RSSuperRenderTask>> superRenderTaskList;
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        superRenderTaskList.emplace_back(std::make_shared<RSSuperRenderTask>(node,
            RSMainThread::Instance()->GetFrameCount()));
    }

    for (auto& renderTask : renderTaskList) {
        auto renderNode = renderTask->GetNode();
        auto surfaceNode = renderNode->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode == nullptr) {
            ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask surfaceNode is null");
            continue;
        }
        auto threadIndex = surfaceNode->GetSubmittedSubThreadIndex();
        if (threadIndex != INT_MAX && superRenderTaskList[threadIndex]) {
            RS_OPTIONAL_TRACE_NAME("node:[ " + surfaceNode->GetName() + ", " + std::to_string(surfaceNode->GetId()) +
                ", " + std::to_string(threadIndex) + " ]; ");
            superRenderTaskList[threadIndex]->AddTask(std::move(renderTask));
        } else {
            if (superRenderTaskList[minLoadThreadIndex_]) {
                RS_OPTIONAL_TRACE_NAME("node:[ " + surfaceNode->GetName() +
                    ", " + std::to_string(surfaceNode->GetId()) +
                    ", " + std::to_string(minLoadThreadIndex_) + " ]; ");
                superRenderTaskList[minLoadThreadIndex_]->AddTask(std::move(renderTask));
                surfaceNode->SetSubmittedSubThreadIndex(minLoadThreadIndex_);
            }
        }
        uint32_t minLoadThreadIndex = 0;
        auto minNodesNum = superRenderTaskList[0]->GetTaskSize();
        for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
            auto num = superRenderTaskList[i]->GetTaskSize();
            if (num < minNodesNum) {
                minNodesNum = num;
                minLoadThreadIndex = i;
            }
        }
        minLoadThreadIndex_ = minLoadThreadIndex;
    }

    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->PostTask([subThread, superRenderTaskList, i]() {
            subThread->RenderCache(superRenderTaskList[i]);
        });
    }
    needResetContext_ = true;
    if (ifNeedRequestNextVsync) {
        RSMainThread::Instance()->SetIsCachedSurfaceUpdated(true);
        RSMainThread::Instance()->RequestNextVSync();
    }
}

void RSSubThreadManager::WaitNodeTask(uint64_t nodeId)
{
    RS_TRACE_NAME_FMT("SSubThreadManager::WaitNodeTask for node %d", nodeId);
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    cvParallelRender_.wait_for(lock, std::chrono::milliseconds(WAIT_NODE_TASK_TIMEOUT), [&]() {
        return !nodeTaskState_[nodeId];
    });
}

void RSSubThreadManager::NodeTaskNotify(uint64_t nodeId)
{
    {
        std::unique_lock<std::mutex> lock(parallelRenderMutex_);
        nodeTaskState_[nodeId] = 0;
    }
    cvParallelRender_.notify_one();
}

void RSSubThreadManager::ResetSubThreadGrContext()
{
    if (threadList_.empty()) {
        return;
    }
    if (!needResetContext_) {
        ReleaseTexture();
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->PostTask([subThread]() {
            subThread->ResetGrContext();
        }, RELEASE_RESOURCE);
    }
    needResetContext_ = false;
    needCancelTask_ = true;
}

void RSSubThreadManager::CancelReleaseResourceTask()
{
    if (!needCancelTask_) {
        return;
    }
    if (threadList_.empty()) {
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->RemoveTask(RELEASE_RESOURCE);
    }
    needCancelTask_ = false;
}

void RSSubThreadManager::ReleaseTexture()
{
    if (threadList_.empty()) {
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->PostTask(
            [subThread]() {
                subThread->ThreadSafetyReleaseTexture();
            },
            RELEASE_TEXTURE);
    }
    needCancelReleaseTextureTask_ = true;
}

void RSSubThreadManager::CancelReleaseTextureTask()
{
    if (!needCancelReleaseTextureTask_) {
        return;
    }
    if (threadList_.empty()) {
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->RemoveTask(RELEASE_TEXTURE);
    }
    needCancelReleaseTextureTask_ = false;
}

void RSSubThreadManager::ForceReleaseResource()
{
    needResetContext_ = true;
}

void RSSubThreadManager::ReleaseSurface(uint32_t threadIndex) const
{
    if (threadList_.size() <= threadIndex) {
        return;
    }
    auto subThread = threadList_[threadIndex];
    subThread->PostTask([subThread]() {
        subThread->ReleaseSurface();
    });
}

void RSSubThreadManager::AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex)
{
    if (threadList_.size() <= threadIndex) {
        return;
    }
    threadList_[threadIndex]->AddToReleaseQueue(std::move(surface));
}

std::vector<MemoryGraphic> RSSubThreadManager::CountSubMem(int pid)
{
    std::vector<MemoryGraphic> memsContainer;
    if (threadList_.empty()) {
        return memsContainer;
    }

    for (auto& subThread : threadList_) {
        if (!subThread) {
            MemoryGraphic memoryGraphic;
            memsContainer.push_back(memoryGraphic);
            continue;
        }
        memsContainer.push_back(subThread->CountSubMem(pid));
    }
    return memsContainer;
}

std::unordered_map<uint32_t, pid_t> RSSubThreadManager::GetReThreadIndexMap() const
{
    return reThreadIndexMap_;
}

void RSSubThreadManager::ScheduleRenderNodeDrawable(DrawableV2::RSSurfaceRenderNodeDrawable* nodeDrawable)
{
    if (!nodeDrawable) {
        return;
    }
    auto& param = nodeDrawable->GetRenderNode()->GetRenderParams();
    if (!param) {
        return;
    }

    auto minDoingCacheProcessNum = threadList_[0]->GetDoingCacheProcessNum();
    minLoadThreadIndex_ = 0;
    for (unsigned int j = 1; j < SUB_THREAD_NUM; j++) {
        if (minDoingCacheProcessNum > threadList_[j]->GetDoingCacheProcessNum()) {
            minDoingCacheProcessNum = threadList_[j]->GetDoingCacheProcessNum();
            minLoadThreadIndex_ = j;
        }
    }
    auto nowIdx = minLoadThreadIndex_;
    if (threadIndexMap_.count(nodeDrawable->GetLastFrameUsedThreadIndex()) != 0) {
        nowIdx = threadIndexMap_[nodeDrawable->GetLastFrameUsedThreadIndex()];
    }

    auto subThread = threadList_[nowIdx];
    auto tid = reThreadIndexMap_[nowIdx];
    nodeTaskState_[param->GetId()] = 1;
    subThread->DoingCacheProcessNumInc();
    subThread->PostTask([subThread, nodeDrawable, tid]() {
        nodeDrawable->SetLastFrameUsedThreadIndex(tid);
        subThread->DrawableCache(nodeDrawable);
    });
    needResetContext_ = true;
}
} // namespace OHOS::Rosen
