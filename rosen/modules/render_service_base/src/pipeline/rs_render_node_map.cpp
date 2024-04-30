/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node_map.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* ENTRY_VIEW = "SCBDesktop";
constexpr const char* WALLPAPER_VIEW = "SCBWallpaper";
constexpr const char* SCREENLOCK_WINDOW = "SCBScreenLock";
constexpr const char* SYSUI_DROPDOWN = "SCBDropdownPanel";
constexpr const char* NEGATIVE_SCREEN = "SCBNegativeScreen";
constexpr const char* ARKTS_CARD_NODE = "ArkTSCardNode";
constexpr const int ABILITY_COMPONENT_LIMIT = 100;
};
RSRenderNodeMap::RSRenderNodeMap()
{
    // add animation fallback node, NOTE: this is different from RSContext::globalRootRenderNode_
    renderNodeMap_.emplace(0, new RSRenderNode(0));
}

void RSRenderNodeMap::Initialize(const std::weak_ptr<RSContext>& context)
{
    context_ = context;
}

void RSRenderNodeMap::ObtainLauncherNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        return;
    }
    if (surfaceNode->GetName().find(ENTRY_VIEW) != std::string::npos) {
        entryViewNodeId_ = surfaceNode->GetId();
    }
    if (surfaceNode->GetName().find(WALLPAPER_VIEW) != std::string::npos) {
        wallpaperViewNodeId_ = surfaceNode->GetId();
    }
    if (surfaceNode->GetName().find(NEGATIVE_SCREEN) != std::string::npos) {
        negativeScreenNodeId_ = surfaceNode->GetId();
    }
}

void RSRenderNodeMap::ObtainScreenLockWindowNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        return;
    }
    if (surfaceNode->GetName().find(SCREENLOCK_WINDOW) != std::string::npos) {
        screenLockWindowNodeId_ = surfaceNode->GetId();
    }
}

NodeId RSRenderNodeMap::GetEntryViewNodeId() const
{
    return entryViewNodeId_;
}

NodeId RSRenderNodeMap::GetWallPaperViewNodeId() const
{
    return wallpaperViewNodeId_;
}

NodeId RSRenderNodeMap::GetScreenLockWindowNodeId() const
{
    return screenLockWindowNodeId_;
}

NodeId RSRenderNodeMap::GetNegativeScreenNodeId() const
{
    return negativeScreenNodeId_;
}

static bool IsResidentProcess(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    return surfaceNode->GetName().find(ENTRY_VIEW) != std::string::npos ||
           surfaceNode->GetName().find(SYSUI_DROPDOWN) != std::string::npos ||
           surfaceNode->GetName().find(SCREENLOCK_WINDOW) != std::string::npos ||
           surfaceNode->GetName().find(WALLPAPER_VIEW) != std::string::npos;
}

void RSRenderNodeMap::CalCulateAbilityComponentNumsInProcess(NodeId id)
{
    if (abilityComponentNumsInProcess_[ExtractPid(id)] > ABILITY_COMPONENT_LIMIT) {
        renderNodeMap_.erase(id);
        surfaceNodeMap_.erase(id);
        return;
    }
    abilityComponentNumsInProcess_[ExtractPid(id)]++;
}

uint32_t RSRenderNodeMap::GetVisibleLeashWindowCount() const
{
    if (surfaceNodeMap_.empty()) {
        return 0;
    }

    return std::count_if(surfaceNodeMap_.begin(), surfaceNodeMap_.end(),
        [](const auto& pair) -> bool {
            return pair.second && pair.second->IsLeashWindowSurfaceNodeVisible();
        });
}

bool RSRenderNodeMap::IsResidentProcessNode(NodeId id) const
{
    auto nodePid = ExtractPid(id);
    return std::any_of(residentSurfaceNodeMap_.begin(), residentSurfaceNodeMap_.end(),
        [nodePid](const auto& pair) -> bool { return ExtractPid(pair.first) == nodePid; });
}

bool RSRenderNodeMap::RegisterRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr)
{
    NodeId id = nodePtr->GetId();
    if (renderNodeMap_.count(id)) {
        return false;
    }
    renderNodeMap_.emplace(id, nodePtr);
    nodePtr->OnRegister(context_);
    if (nodePtr->GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = nodePtr->ReinterpretCastTo<RSSurfaceRenderNode>();
        surfaceNodeMap_.emplace(id, surfaceNode);
        if (IsResidentProcess(surfaceNode)) {
            residentSurfaceNodeMap_.emplace(id, surfaceNode);
        }
        ObtainLauncherNodeId(surfaceNode);
        ObtainScreenLockWindowNodeId(surfaceNode);
    } else if (nodePtr->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasDrawingNode = nodePtr->ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        canvasDrawingNodeMap_.emplace(id, canvasDrawingNode);
    }
    return true;
}

bool RSRenderNodeMap::RegisterDisplayRenderNode(const std::shared_ptr<RSDisplayRenderNode>& nodePtr)
{
    NodeId id = nodePtr->GetId();
    if (renderNodeMap_.count(id)) {
        return false;
    }
    renderNodeMap_.emplace(id, nodePtr);
    displayNodeMap_.emplace(id, nodePtr);
    nodePtr->OnRegister(context_);
    return true;
}

void RSRenderNodeMap::EraseAbilityComponentNumsInProcess(NodeId id)
{
    auto surfaceNodeIter = surfaceNodeMap_.find(id);
    if (surfaceNodeIter != surfaceNodeMap_.end()) {
        auto surfaceNode = GetRenderNode<RSSurfaceRenderNode>(id);
        if ((surfaceNode->IsAbilityComponent()) && (surfaceNode->GetName() != ARKTS_CARD_NODE)) {
            auto pid = ExtractPid(id);
            auto iter = abilityComponentNumsInProcess_.find(pid);
            if (iter != abilityComponentNumsInProcess_.end()) {
                if (--abilityComponentNumsInProcess_[pid] == 0) {
                    abilityComponentNumsInProcess_.erase(pid);
                }
            }
        }
    }
}

void RSRenderNodeMap::UnregisterRenderNode(NodeId id)
{
    EraseAbilityComponentNumsInProcess(id);
    renderNodeMap_.erase(id);
    surfaceNodeMap_.erase(id);
    residentSurfaceNodeMap_.erase(id);
    displayNodeMap_.erase(id);
    canvasDrawingNodeMap_.erase(id);
}

void RSRenderNodeMap::MoveRenderNodeMap(
    std::shared_ptr<std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> subRenderNodeMap, pid_t pid)
{
    std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>::iterator iter = renderNodeMap_.begin();
    for (; iter != renderNodeMap_.end();) {
        if (ExtractPid(iter->first) != pid) {
            ++iter;
            continue;
        }
        // update node flag to avoid animation fallback
        iter->second->fallbackAnimationOnDestroy_ = false;
        // remove node from tree
        iter->second->RemoveFromTree(false);
        subRenderNodeMap->emplace(iter->first, iter->second);
        iter = renderNodeMap_.erase(iter);
    }
}

void RSRenderNodeMap::FilterNodeByPid(pid_t pid)
{
    ROSEN_LOGD("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %{public}llu",
        (unsigned long long)pid);
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)

    EraseIf(surfaceNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(residentSurfaceNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(canvasDrawingNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(abilityComponentNumsInProcess_, [pid](const auto& pair) -> bool {
        return pair.first == pid;
    });

    EraseIf(displayNodeMap_, [pid](const auto& pair) -> bool {
        if (ExtractPid(pair.first) != pid && pair.second) {
            ROSEN_LOGD("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %{public}llu",
                (unsigned long long)pid);
            pair.second->FilterModifiersByPid(pid);
        }
        return ExtractPid(pair.first) == pid;
    });

    if (auto fallbackNode = GetAnimationFallbackNode()) {
        // remove all fallback animations belong to given pid
        fallbackNode->GetAnimationManager().FilterAnimationByPid(pid);
    }
}

void RSRenderNodeMap::TraversalNodes(std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const
{
    for (const auto& [_, node] : renderNodeMap_) {
        func(node);
    }
}

void RSRenderNodeMap::TraverseCanvasDrawingNodes(
    std::function<void(const std::shared_ptr<RSCanvasDrawingRenderNode>&)> func) const
{
    for (const auto& [_, node] : canvasDrawingNodeMap_) {
        func(node);
    }
}

void RSRenderNodeMap::TraverseSurfaceNodes(std::function<void (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const
{
    for (const auto& [_, node] : surfaceNodeMap_) {
        func(node);
    }
}

bool RSRenderNodeMap::ContainPid(pid_t pid) const
{
    return std::any_of(surfaceNodeMap_.begin(), surfaceNodeMap_.end(),
        [pid](const auto& pair) -> bool { return ExtractPid(pair.first) == pid; });
}

void RSRenderNodeMap::TraverseDisplayNodes(std::function<void (const std::shared_ptr<RSDisplayRenderNode>&)> func) const
{
    for (const auto& [_, node] : displayNodeMap_) {
        func(node);
    }
}

std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>> RSRenderNodeMap::GetResidentSurfaceNodeMap() const
{
    return residentSurfaceNodeMap_;
}

template<>
const std::shared_ptr<RSBaseRenderNode> RSRenderNodeMap::GetRenderNode(NodeId id) const
{
    auto itr = renderNodeMap_.find(id);
    if (itr == renderNodeMap_.end()) {
        return nullptr;
    }
    return itr->second;
}

const std::shared_ptr<RSRenderNode> RSRenderNodeMap::GetAnimationFallbackNode() const
{
    auto itr = renderNodeMap_.find(0);
    if (itr == renderNodeMap_.end()) {
        return nullptr;
    }
    return itr->second;
}
} // namespace Rosen
} // namespace OHOS
