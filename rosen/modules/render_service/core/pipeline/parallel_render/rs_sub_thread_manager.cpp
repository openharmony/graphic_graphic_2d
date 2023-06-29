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

#include "pipeline/rs_main_thread.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
static constexpr uint32_t SUB_THREAD_NUM = 3;

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
            curThread->Start();
            threadList_.push_back(curThread);
        }
    }
}

void RSSubThreadManager::PostTask(const std::function<void()>& task, uint32_t threadIndex)
{
    if (threadIndex >= threadList_.size()) {
        RS_LOGE("taskIndex geq thread num");
        return;
    }
    threadList_[threadIndex]->PostTask(task);
}

void RSSubThreadManager::SubmitSubThreadTask(const std::shared_ptr<RSDisplayRenderNode>& node,
    const std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    RS_TRACE_NAME("RSSubThreadManager::SubmitSubThreadTask");
    if (node == nullptr) {
        ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask display node is null");
        return;
    }
    if (subThreadNodes.empty()) {
        ROSEN_LOGD("RSSubThreadManager::SubmitSubThreadTask subThread does not have any nodes");
        return;
    }
    std::vector<std::unique_ptr<RSRenderTask>> renderTaskList;
    auto cacheSkippedNodeMap = RSMainThread::Instance()->GetCacheCmdSkippedNodes();
    for (const auto& child : subThreadNodes) {
        if (!child->ShouldPaint()) {
            continue;
        }
        if (cacheSkippedNodeMap.count(child->GetId()) != 0 && child->HasCachedTexture()) {
            RS_TRACE_NAME_FMT("SubmitTask cacheCmdSkippedNode: [%s, %llu]", child->GetName().c_str(), child->GetId());
            continue;
        }
        nodeTaskState_[child->GetId()] = 1;
        child->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
        renderTaskList.push_back(std::make_unique<RSRenderTask>(*child, RSRenderTask::RenderNodeStage::CACHE));
    }

    std::vector<std::shared_ptr<RSSuperRenderTask>> superRenderTaskList;
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        superRenderTaskList.emplace_back(std::make_shared<RSSuperRenderTask>(node,
            RSMainThread::Instance()->GetFrameCount()));
    }

    for (size_t i = 0; i < renderTaskList.size(); i++) {
        auto renderNode = renderTaskList[i]->GetNode();
        auto surfaceNode = renderNode->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode == nullptr) {
            ROSEN_LOGE("RSSubThreadManager::SubmitSubThreadTask surfaceNode is null");
            continue;
        }
        auto threadIndex = surfaceNode->GetSubmittedSubThreadIndex();
        if (threadIndex != INT_MAX && superRenderTaskList[threadIndex]) {
            superRenderTaskList[threadIndex]->AddTask(std::move(renderTaskList[i]));
        } else {
            if (superRenderTaskList[minLoadThreadIndex_]) {
                superRenderTaskList[minLoadThreadIndex_]->AddTask(std::move(renderTaskList[i]));
                surfaceNode->SetSubmittedSubThreadIndex(minLoadThreadIndex_);
            }
        }
        uint32_t minLoadThreadIndex = 0;
        auto minNodesNum = superRenderTaskList[0]->GetTaskSize();
        for (uint32_t j = 0; j < SUB_THREAD_NUM; j++) {
            auto num = superRenderTaskList[j]->GetTaskSize();
            if (num < minNodesNum) {
                minNodesNum = num;
                minLoadThreadIndex = j;
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
}

void RSSubThreadManager::WaitNodeTask(uint64_t nodeId)
{
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    cvParallelRender_.wait(lock, [&]() {
        return !nodeTaskState_[nodeId];
    });
}

void RSSubThreadManager::NodeTaskNotify(uint64_t nodeId)
{
    nodeTaskState_[nodeId] = 0;
    cvParallelRender_.notify_one();
}

void RSSubThreadManager::SaveCacheTexture(RSRenderNode& node) const
{
#ifdef NEW_SKIA
    auto surface = node.GetCompletedCacheSurface(UNI_MAIN_THREAD_INDEX, true);
    if (surface == nullptr || (surface->width() == 0 || surface->height() == 0)) {
        RS_LOGE("invalid cache surface");
        return;
    }
    if (renderContext_ == nullptr) {
        RS_LOGE("SaveCacheTexture render context is nullptr");
        return;
    }
    auto mainGrContext = renderContext_->GetGrContext();
    if (mainGrContext == nullptr) {
        RS_LOGE("SaveCacheTexture GrDirectContext is nullptr");
        return;
    }
    auto sharedBackendTexture =
        surface->getBackendTexture(SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess);
    if (!sharedBackendTexture.isValid()) {
        RS_LOGE("SaveCacheTexture does not has GPU backend, %llu", node.GetId());
        return;
    }
    auto sharedTexture = SkImage::MakeFromTexture(mainGrContext, sharedBackendTexture,
        kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, kPremul_SkAlphaType, nullptr);
    if (sharedTexture == nullptr) {
        RS_LOGE("SaveCacheTexture shared texture is nullptr, %llu", node.GetId());
        return;
    }
    node.SetCacheTexture(sharedTexture);
#endif
}
}