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
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* ENTRY_VIEW = "EntryView";
constexpr const char* WALLPAPER_VIEW = "WallpaperView";
constexpr const char* SCREENLOCK_WINDOW = "ScreenLockWindow";
constexpr const char* SYSUI_DROPDOWN = "SysUI_Dropdown";
};
RSRenderNodeMap::RSRenderNodeMap()
{
    // add animation fallback node
    renderNodeMap_.emplace(0, new RSCanvasRenderNode(0));
}

void RSRenderNodeMap::ObtainLauncherNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        return;
    }
    if (surfaceNode->GetName() == ENTRY_VIEW) {
        entryViewNodeId_ = surfaceNode->GetId();
    }
    if (surfaceNode->GetName() == WALLPAPER_VIEW) {
        wallpaperViewNodeId_ = surfaceNode->GetId();
    }
}

void RSRenderNodeMap::ObtainScreenLockWindowNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        return;
    }
    if (surfaceNode->GetName() == SCREENLOCK_WINDOW) {
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

static bool IsResidentProcess(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    return surfaceNode->GetName() == ENTRY_VIEW || surfaceNode->GetName() == SYSUI_DROPDOWN ||
           surfaceNode->GetName() == SCREENLOCK_WINDOW || surfaceNode->GetName() == WALLPAPER_VIEW;
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
    if (nodePtr->GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = nodePtr->ReinterpretCastTo<RSSurfaceRenderNode>();
        surfaceNodeMap_.emplace(id, surfaceNode);
        if (IsResidentProcess(surfaceNode)) {
            residentSurfaceNodeMap_.emplace(id, surfaceNode);
        }
        ObtainLauncherNodeId(surfaceNode);
        ObtainScreenLockWindowNodeId(surfaceNode);
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
    return true;
}

void RSRenderNodeMap::UnregisterRenderNode(NodeId id)
{
    renderNodeMap_.erase(id);
    surfaceNodeMap_.erase(id);
    drivenRenderNodeMap_.erase(id);
    residentSurfaceNodeMap_.erase(id);
    displayNodeMap_.erase(id);
}

void RSRenderNodeMap::AddDrivenRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr)
{
    NodeId id = nodePtr->GetId();
    if (!renderNodeMap_.count(id)) {
        return;
    }
    drivenRenderNodeMap_.emplace(id, nodePtr);
}

void RSRenderNodeMap::RemoveDrivenRenderNode(NodeId id)
{
    drivenRenderNodeMap_.erase(id);
}

void RSRenderNodeMap::FilterNodeByPid(pid_t pid)
{
    ROSEN_LOGD("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %{public}llu",
        (unsigned long long)pid);
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    EraseIf(renderNodeMap_, [pid](const auto& pair) -> bool {
        if (ExtractPid(pair.first) != pid) {
            return false;
        }
        // update node flag to avoid animation fallback
        pair.second->fallbackAnimationOnDestroy_ = false;
        // remove node from tree
        pair.second->RemoveFromTree(false);
        return true;
    });

    EraseIf(surfaceNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(drivenRenderNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(residentSurfaceNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(displayNodeMap_, [pid](const auto& pair) -> bool {
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

void RSRenderNodeMap::TraverseDrivenRenderNodes(std::function<void (const std::shared_ptr<RSRenderNode>&)> func) const
{
    for (const auto& [_, node] : drivenRenderNodeMap_) {
        func(node);
    }
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
