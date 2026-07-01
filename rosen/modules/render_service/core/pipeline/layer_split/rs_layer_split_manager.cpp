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
#include "rs_layer_split_manager.h"

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS::Rosen {
RSLayerSplitManager* RSLayerSplitManager::GetInstance()
{
    static RSLayerSplitManager instance;
    return &instance;
}

RSLayerSplitManager::RSLayerSplitManager()
{
    isGlobalEnabled_ = RSSystemParameters::GetLayerSplitterEnable();
    LAYER_SPLIT_LOGI("%{public}s isGlobalEnabled: %{public}d", __func__, isGlobalEnabled_);
}

void RSLayerSplitManager::SetEnabled(bool isEnabled)
{
    if (!isGlobalEnabled_ || isEnabled_ == isEnabled) {
        return;
    }
    isEnabled_ = isEnabled;
    LAYER_SPLIT_LOGI("%{public}s isEnabled: %{public}d", __func__, isEnabled_);
}

void RSLayerSplitManager::Reset(uint64_t vsyncId)
{
    LAYER_SPLIT_LOGD("Reset vsync:%{public}" PRIu64, vsyncId);
    if (isEnabled_ && selectorVec_.empty()) {
        selectorVec_.emplace_back(OpincSplitNodeSelector::GetInstance());
    } else if (!isEnabled_ && !selectorVec_.empty()) {
        for (const auto& selector : selectorVec_) {
            selector->SetCurrParentNode(nullptr);
        }
        selectorVec_.clear();
    }
    for (const auto& [id, planner] : plannerMap_) {
        planner->Reset();
    }
}

void RSLayerSplitManager::MoveSplitSurfaceNode()
{
    for (const auto& [id, planner] : plannerMap_) {
        planner->MoveSplitSurfaceNode();
    }
}

void RSLayerSplitManager::InitSplitSurface(const ScreenInfo& screenInfo)
{
    for (const auto& [id, planner] : plannerMap_) {
        planner->InitSplitSurface(screenInfo);
    }
}

void RSLayerSplitManager::RecordSplitNode(std::shared_ptr<RSRenderNode> node)
{
    if (!node) {
        return;
    }
    for (const auto& selector : selectorVec_) {
        selector->RecordSplitNode(node);
    }
}

void RSLayerSplitManager::CheckNeedLeave()
{
    for (const auto& selector : selectorVec_) {
        auto parentNode = selector->SelectParentNode();
        selector->SetCurrParentNode(parentNode);
        if (!parentNode) {
            continue;
        }

        auto lastParentNode = selector->GetLastParentNode();
        if (lastParentNode && parentNode->GetId() != lastParentNode->GetId()) {
            auto plannerIt = plannerMap_.find(lastParentNode->GetId());
            if (plannerIt != plannerMap_.end() && plannerIt->second) {
                plannerIt->second->UnregisterSplitSurfaceNode();
            }
        }

        NodeId nodeId = parentNode->GetId();
        auto plannerIt = plannerMap_.find(nodeId);
        if (plannerIt == plannerMap_.end()) {
            auto planner = selector->MakePlanner();
            auto processor = selector->MakeProcessor();
            planner->SetOpIncParentNode(parentNode);
            plannerMap_[nodeId] = planner;
            parentNodeMap_[nodeId] = parentNode;
            processorMap_[nodeId] = processor;
        }
    }

    for (const auto& [id, planner] : plannerMap_) {
        planner->CheckNeedLeave();
    }
}

void RSLayerSplitManager::CheckSplitNodeIntersectFilter(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (plannerMap_.empty()) {
        return;
    }
    for (const auto& [id, planner] : plannerMap_) {
        planner->CheckSplitNodeIntersectFilter(hwcNode);
    }
}

void RSLayerSplitManager::UpdatePlanAndDirtyRegion(std::shared_ptr<RSDirtyRegionManager> dirtyManager)
{
    if (plannerMap_.empty()) {
        return;
    }
    for (const auto& [id, planner] : plannerMap_) {
        planner->UpdateSplitPlan();
        planner->UpdateScreenDirtyRegion(dirtyManager);
    }
}

void RSLayerSplitManager::Sync(uint64_t vsyncId)
{
    std::vector<NodeId> nodeId2RemoveVec;

    for (const auto& [id, planner] : plannerMap_) {
        if (planner->GetSurfaceStatus() == SurfaceStatus::UNREGISTER) {
            nodeId2RemoveVec.push_back(id);
        }
    }

    for (auto nodeId : nodeId2RemoveVec) {
        plannerMap_.erase(nodeId);
        processorMap_.erase(nodeId);
        parentNodeMap_.erase(nodeId);
    }

    for (const auto& [id, planner] : plannerMap_) {
        auto parentIt = parentNodeMap_.find(id);
        if (parentIt == parentNodeMap_.end()) {
            continue;
        }
        auto parent = parentIt->second;
        if (!parent) {
            continue;
        }

        auto processorIt = processorMap_.find(id);
        if (processorIt == processorMap_.end()) {
            continue;
        }

        planner->UpdateChildren(parent);
        if (processorIt->second) {
            processorIt->second->Sync(planner);
            planner->Sync(processorIt->second);
        }
    }
}

void RSLayerSplitManager::DrawDfx(std::shared_ptr<RSPaintFilterCanvas> canvas, uint64_t vsyncId)
{
    if (!canvas) {
        return;
    }
    for (const auto& [id, planner] : plannerMap_) {
        auto processorIt = processorMap_.find(id);
        if (processorIt == processorMap_.end() || !processorIt->second) {
            continue;
        }
        processorIt->second->DrawDfx();
    }
}

bool RSLayerSplitManager::CheckDoDirectCompositionWithSplitLayer(
    std::shared_ptr<TransactionDataMap> transactionDataEffective, bool doDirectComposition)
{
    if (!doDirectComposition) {
        return false;
    }

    if (plannerMap_.empty()) {
        return false;
    }

    for (const auto& [id, planner] : plannerMap_) {
        if (!planner->CheckDoDirectCompositionWithSplitLayer(transactionDataEffective)) {
            return false;
        }
    }

    return true;
}

} // namespace OHOS::Rosen