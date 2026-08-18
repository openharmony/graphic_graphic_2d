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
    LAYER_SPLIT_LOGD("%{public}s isGlobalEnabled: %{public}d", __func__, isGlobalEnabled_);
}

void RSLayerSplitManager::SetEnabled(bool isEnabled)
{
    if (!isGlobalEnabled_ || isEnabled_ == isEnabled) {
        return;
    }
    isEnabled_ = isEnabled;
    LAYER_SPLIT_LOGD("%{public}s isEnabled: %{public}d", __func__, isEnabled_);
}

void RSLayerSplitManager::Reset()
{
    if (isEnabled_ && selectorVec_.empty()) {
        selectorVec_.emplace_back(OpincSplitNodeSelector::GetInstance());
    } else if (!isEnabled_ && !selectorVec_.empty()) {
        for (const auto& selector : selectorVec_) {
            selector->SetCurrParentNode(nullptr);
        }
        selectorVec_.clear();
    }
    for (const auto& [node, planner] : plannerMap_) {
        planner->Reset();
    }
}

void RSLayerSplitManager::MoveSplitSurfaceNode()
{
    for (const auto& [node, planner] : plannerMap_) {
        planner->MoveSplitSurfaceNode();
    }
}

void RSLayerSplitManager::InitSplitSurface(const ScreenInfo& screenInfo)
{
    for (const auto& [node, planner] : plannerMap_) {
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

void RSLayerSplitManager::CheckNeedLeave(RSScreenRenderNode& screenNode)
{
    for (const auto& selector : selectorVec_) {
        auto parentNode = selector->SelectParentNode();
        selector->SetCurrParentNode(parentNode);
        if (!parentNode) {
            continue;
        }

        auto lastParentNode = selector->GetLastParentNode();
        if (lastParentNode && parentNode != lastParentNode) {
            auto plannerIt = plannerMap_.find(lastParentNode);
            if (plannerIt != plannerMap_.end() && plannerIt->second) {
                plannerIt->second->UnregisterSplitSurfaceNode();
            }
        }

        auto plannerIt = plannerMap_.find(parentNode);
        if (plannerIt == plannerMap_.end()) {
            auto planner = selector->MakePlanner();
            auto processor = selector->MakeProcessor();
            planner->SetOpIncParentNode(parentNode);
            plannerMap_[parentNode] = planner;
            processorMap_[parentNode] = processor;
        }
    }

    for (const auto& [node, planner] : plannerMap_) {
        if (node->GetScreenNodeId() != screenNode.GetId()) {
            continue;
        }
        planner->CheckNeedLeave();
    }
}

void RSLayerSplitManager::CheckSplitNodeIntersectFilter(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (plannerMap_.empty()) {
        return;
    }
    for (const auto& [node, planner] : plannerMap_) {
        planner->CheckSplitNodeIntersectFilter(hwcNode);
    }
}

void RSLayerSplitManager::UpdatePlanAndDirtyRegion(RSScreenRenderNode& screenNode,
    std::shared_ptr<RSDirtyRegionManager> dirtyManager)
{
    if (plannerMap_.empty()) {
        return;
    }
    for (const auto& [node, planner] : plannerMap_) {
        if (node->GetScreenNodeId() != screenNode.GetId()) {
            continue;
        }
        planner->UpdateSplitPlan();
        planner->UpdateScreenDirtyRegion(dirtyManager);
    }
}

void RSLayerSplitManager::Sync()
{
    std::vector<std::shared_ptr<RSRenderNode>> nodeId2RemoveVec;

    for (const auto& [node, planner] : plannerMap_) {
        planner->CheckParentNodeOnTheTree();
        if (planner->GetSurfaceStatus() == SurfaceStatus::UNREGISTER) {
            nodeId2RemoveVec.push_back(node);
        }
    }

    for (auto node : nodeId2RemoveVec) {
        plannerMap_.erase(node);
        processorMap_.erase(node);
    }

    for (const auto& [node, planner] : plannerMap_) {
        if (!node) {
            continue;
        }

        auto processorIt = processorMap_.find(node);
        if (processorIt == processorMap_.end()) {
            continue;
        }

        planner->UpdateChildren(node);
        if (processorIt->second) {
            processorIt->second->Sync(planner);
            planner->Sync(processorIt->second);
        }
    }
}

bool RSLayerSplitManager::CheckOpIncNodeFromCommand(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    if (plannerMap_.empty() || rsTransactionData == nullptr) {
        return false;
    }

    for (const auto& [node, planner] : plannerMap_) {
        for (auto& [nodeId, followType, command] : rsTransactionData->GetPayload()) {
            if (command == nullptr) {
                continue;
            }

            if (!planner->CheckOpIncNodeFromCommand(command->GetNodeId())) {
                return false;
            }
        }
    }

    return true;
}

bool RSLayerSplitManager::CheckDoDirectCompositionWithSplitLayer()
{
    if (plannerMap_.empty()) {
        return false;
    }

    for (const auto& [node, planner] : plannerMap_) {
        if (!planner->CheckCanDoDirectComposition()) {
            return false;
        }
    }

    return true;
}

} // namespace OHOS::Rosen