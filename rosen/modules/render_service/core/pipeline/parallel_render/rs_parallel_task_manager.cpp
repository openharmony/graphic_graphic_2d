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

#include "rs_parallel_task_manager.h"
#include <memory>
#include "rs_parallel_render_manager.h"
#include "rs_parallel_render_ext.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "rs_node_cost_manager.h"

namespace OHOS {
namespace Rosen {

RSParallelTaskManager::RSParallelTaskManager()
    : threadNum_(0),
      taskNum_(0),
      isParallelRenderExtEnabled_(RSParallelRenderExt::OpenParallelRenderExt())
{
    if (isParallelRenderExtEnabled_ && (RSParallelRenderExt::initParallelRenderLBFunc_ != nullptr)) {
        auto initParallelRenderExt = reinterpret_cast<int*(*)()>(RSParallelRenderExt::initParallelRenderLBFunc_);
        loadBalance_ = initParallelRenderExt();
        if (loadBalance_ == nullptr) {
            isParallelRenderExtEnabled_ = false;
            RS_LOGE("init parallel render load balance failed!!!");
        }
    } else {
        RS_LOGE("open graphic 2d extension failed!!!");
    }
}

RSParallelTaskManager::~RSParallelTaskManager()
{
    if (isParallelRenderExtEnabled_ && (RSParallelRenderExt::freeParallelRenderLBFunc_ != nullptr)) {
        auto freeParallelRenderExt = reinterpret_cast<void (*)(int*)>(RSParallelRenderExt::freeParallelRenderLBFunc_);
        if (freeParallelRenderExt) {
            freeParallelRenderExt(loadBalance_);
        }
    }
    RSParallelRenderExt::CloseParallelRenderExt();
}

void RSParallelTaskManager::Initialize(uint32_t threadNum)
{
    threadNum_ = threadNum;
    if (isParallelRenderExtEnabled_ && (RSParallelRenderExt::setSubRenderThreadNumFunc_ != nullptr)) {
        auto parallelRenderExtSetThreadNumCall = reinterpret_cast<void(*)(int*, uint32_t)>(
            RSParallelRenderExt::setSubRenderThreadNumFunc_);
        parallelRenderExtSetThreadNumCall(loadBalance_, threadNum);
    }
}

void RSParallelTaskManager::PushRenderTask(std::unique_ptr<RSRenderTask> renderTask)
{
    if (isParallelRenderExtEnabled_ && (RSParallelRenderExt::addRenderLoadFunc_ != nullptr)) {
        auto parallelRenderExtAddRenderLoad = reinterpret_cast<void(*)(int*, uint64_t, float)>(
            RSParallelRenderExt::addRenderLoadFunc_);
        parallelRenderExtAddRenderLoad(loadBalance_, renderTask->GetIdx(), 0.f);
    }
    renderTaskList_.push_back(std::move(renderTask));
}

void RSParallelTaskManager::PushCompositionTask(std::unique_ptr<RSCompositionTask> compositionTask)
{
    compositionTaskList_.push_back(std::move(compositionTask));
}

void RSParallelTaskManager::LBCalcAndSubmitSuperTask(std::shared_ptr<RSBaseRenderNode> displayNode)
{
    if (renderTaskList_.size() == 0) {
        taskNum_ = 0;
        return;
    }
    std::vector<uint32_t> loadNumPerThread = LoadBalancing();
    uint32_t index = 0;
    uint32_t loadNumSum = 0;
    taskNum_ = 0;
    cachedSuperRenderTask_ = std::make_unique<RSSuperRenderTask>(displayNode);
    for (uint32_t i = 0; i < loadNumPerThread.size(); i++) {
        loadNumSum += loadNumPerThread[i];
        while (index < loadNumSum) {
            cachedSuperRenderTask_->AddTask(std::move(renderTaskList_[index]));
            index++;
        }
        RSParallelRenderManager::Instance()->SubmitSuperTask(taskNum_, std::move(cachedSuperRenderTask_));
        taskNum_++;
        cachedSuperRenderTask_ = std::make_unique<RSSuperRenderTask>(displayNode);
    }
}

void RSParallelTaskManager::LBCalcAndSubmitCompositionTask(std::shared_ptr<RSBaseRenderNode> baseNode)
{
    taskNum_ = 0;
    for (decltype(compositionTaskList_.size()) i = 0; i < compositionTaskList_.size(); i++) {
        RSParallelRenderManager::Instance()->SubmitCompositionTask(taskNum_, std::move(compositionTaskList_[i]));
        taskNum_++;
    }
}

std::vector<uint32_t> RSParallelTaskManager::LoadBalancing()
{
    RS_TRACE_FUNC();
    if (parallelPolicy_.size() > 0) {
        return parallelPolicy_;
    }

    std::vector<uint32_t> loadNumPerThread{};
    if (isParallelRenderExtEnabled_ && (RSParallelRenderExt::loadBalancingFunc_ != nullptr)) {
        auto parallelRenderExtLB = reinterpret_cast<void(*)(int*, std::vector<uint32_t> &)>(
            RSParallelRenderExt::loadBalancingFunc_);
        parallelRenderExtLB(loadBalance_, loadNumPerThread);
    } else {
        if (threadNum_ == 0) {
            RS_LOGE("RSParallelTaskManager::LoadBalancing threadNum_ == 0");
            return loadNumPerThread;
        }
        uint32_t avgLoadNum = renderTaskList_.size() / threadNum_;
        uint32_t loadMod = renderTaskList_.size() % threadNum_;
        for (uint32_t i = threadNum_; i > 0; i--) {
            if (i <= loadMod) {
                loadNumPerThread.push_back(avgLoadNum + 1);
            } else {
                loadNumPerThread.push_back(avgLoadNum);
            }
        }
    }
    return loadNumPerThread;
}

uint32_t RSParallelTaskManager::GetTaskNum() const
{
    return taskNum_;
}

void RSParallelTaskManager::Reset()
{
    taskNum_ = 0;
    renderTaskList_.clear();
    superRenderTaskList_.clear();
    parallelPolicy_.clear();
    compositionTaskList_.clear();
    if (isParallelRenderExtEnabled_ && (RSParallelRenderExt::clearRenderLoadFunc_ != nullptr)) {
        auto parallelRenderExtClearRenderLoad = reinterpret_cast<void(*)(int*)>(
            RSParallelRenderExt::clearRenderLoadFunc_);
        parallelRenderExtClearRenderLoad(loadBalance_);
    }
}

void RSParallelTaskManager::SetSubThreadRenderTaskLoad(uint32_t threadIdx, uint64_t loadId, float cost)
{
    if (isParallelRenderExtEnabled_ && (RSParallelRenderExt::updateLoadCostFunc_ != nullptr)) {
        auto parallelRenderExtUpdateLoadCost = reinterpret_cast<void(*)(int*, uint32_t, uint64_t, float)>(
            RSParallelRenderExt::updateLoadCostFunc_);
        parallelRenderExtUpdateLoadCost(loadBalance_, threadIdx, loadId, cost);
    }
}

void RSParallelTaskManager::UpdateNodeCost(RSDisplayRenderNode& node, std::vector<uint32_t>& parallelPolicy) const
{
    if (!isParallelRenderExtEnabled_ || (RSParallelRenderExt::updateNodeCostFunc_ == nullptr)) {
        return;
    }
    RS_TRACE_NAME("UpdateNodeCost");
    auto surfaceNodes = node.GetSortedChildren();
    std::map<uint64_t, int32_t> costs;
    for (auto& it : *surfaceNodes) {
        auto surface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(it);
        costs.insert(std::pair<uint64_t, int32_t>(surface->GetId(), surface->GetNodeCost()));
    }
    auto updateNodeCost = reinterpret_cast<void(*)(int*, std::map<uint64_t, int32_t> &, std::vector<uint32_t> &)>(
        RSParallelRenderExt::updateNodeCostFunc_);
    updateNodeCost(loadBalance_, costs, parallelPolicy);
}

void RSParallelTaskManager::GetCostFactor(std::map<std::string, int32_t>& costFactor,
    std::map<int64_t, int32_t>& imageFactor) const
{
    if (!isParallelRenderExtEnabled_ || (RSParallelRenderExt::getCostFactorFunc_ == nullptr)) {
        return;
    }
    auto getCostFactor = reinterpret_cast<void(*)(int*, std::map<std::string, int32_t> &,
        std::map<int64_t, int32_t> &)>(RSParallelRenderExt::getCostFactorFunc_);
    getCostFactor(loadBalance_, costFactor, imageFactor);
}

void RSParallelTaskManager::LoadParallelPolicy(std::vector<uint32_t>& parallelPolicy)
{
    parallelPolicy_.swap(parallelPolicy);
}
} // namespace Rosen
} // namespace OHOS