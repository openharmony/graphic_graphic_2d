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
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"

namespace OHOS {
namespace Rosen {

using ResidentSurfaceNodeMap = std::unordered_map<NodeId, std::shared_ptr<RSSurfaceRenderNode>>;

RSRenderNodeMap::RSRenderNodeMap()
{
    // add animation fallback node, NOTE: this is different from RSContext::globalRootRenderNode_
    renderNodeMap_[0][0] = std::make_shared<RSBaseRenderNode>(0);
    renderNodeMap_[0][0]->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
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
    if (surfaceNode->GetSurfaceWindowType() == SurfaceWindowType::SCB_DESKTOP) {
        entryViewNodeId_ = surfaceNode->GetId();
    }
    if (surfaceNode->GetSurfaceWindowType() == SurfaceWindowType::SCB_WALLPAPER) {
        wallpaperViewNodeId_ = surfaceNode->GetId();
    }
    if (surfaceNode->GetSurfaceWindowType() == SurfaceWindowType::SCB_NEGATIVE_SCREEN) {
        negativeScreenNodeId_ = surfaceNode->GetId();
    }
}

void RSRenderNodeMap::ObtainScreenLockWindowNodeId(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode == nullptr) {
        return;
    }
    if (surfaceNode->GetSurfaceWindowType() == SurfaceWindowType::SCB_SCREEN_LOCK) {
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
    switch (surfaceNode->GetSurfaceWindowType()) {
        case SurfaceWindowType::SCB_DESKTOP:
        case SurfaceWindowType::SCB_DROPDOWN_PANEL:
        case SurfaceWindowType::SCB_SCREEN_LOCK:
        case SurfaceWindowType::SCB_WALLPAPER:
            return true;
        default:
            return false;
    }
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

uint64_t RSRenderNodeMap::GetSize() const
{
    size_t mapSize = 0;
    for (const auto& [_, subMap] : renderNodeMap_) {
        mapSize += subMap.size();
    }
    return static_cast<uint64_t>(mapSize);
}

bool RSRenderNodeMap::IsResidentProcessNode(NodeId id) const
{
    auto nodePid = ExtractPid(id);
    return std::any_of(residentSurfaceNodeMap_.begin(), residentSurfaceNodeMap_.end(),
        [nodePid](const auto& pair) -> bool { return ExtractPid(pair.first) == nodePid; });
}

bool RSRenderNodeMap::IsUIExtensionSurfaceNode(NodeId id) const
{
    std::lock_guard<std::mutex> lock(uiExtensionSurfaceNodesMutex_);
    return uiExtensionSurfaceNodes_.find(id) != uiExtensionSurfaceNodes_.end();
}

void RSRenderNodeMap::AddUIExtensionSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode && surfaceNode->IsUIExtension()) {
        std::lock_guard<std::mutex> lock(uiExtensionSurfaceNodesMutex_);
        uiExtensionSurfaceNodes_.insert(surfaceNode->GetId());
    }
}

void RSRenderNodeMap::RemoveUIExtensionSurfaceNode(const std::shared_ptr<RSSurfaceRenderNode> surfaceNode)
{
    if (surfaceNode && surfaceNode->IsUIExtension()) {
        std::lock_guard<std::mutex> lock(uiExtensionSurfaceNodesMutex_);
        uiExtensionSurfaceNodes_.erase(surfaceNode->GetId());
    }
}

bool RSRenderNodeMap::RegisterRenderNode(const std::shared_ptr<RSBaseRenderNode>& nodePtr)
{
    NodeId id = nodePtr->GetId();
    pid_t pid = ExtractPid(id);
    if (!(renderNodeMap_[pid].insert({ id, nodePtr })).second) {
        ROSEN_LOGE("RegisterRenderNode insert to Map failed, pid:%{public}d, nodeId:%{public}" PRIu64 " ", static_cast<int32_t>(pid), id);
        return false;
    }
    nodePtr->OnRegister(context_);
    if (nodePtr->GetType() == RSRenderNodeType::SURFACE_NODE) {
        auto surfaceNode = nodePtr->ReinterpretCastTo<RSSurfaceRenderNode>();
        surfaceNodeMap_.emplace(id, surfaceNode);
        if (IsResidentProcess(surfaceNode)) {
            residentSurfaceNodeMap_.emplace(id, surfaceNode);
        }
        AddUIExtensionSurfaceNode(surfaceNode);
        ObtainLauncherNodeId(surfaceNode);
        ObtainScreenLockWindowNodeId(surfaceNode);
        if (surfaceNode->IsSelfDrawingType()) {
            RSSurfaceFpsManager::GetInstance().RegisterSurfaceFps(id, surfaceNode->GetName());
        }
    } else if (nodePtr->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasDrawingNode = nodePtr->ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        canvasDrawingNodeMap_.emplace(id, canvasDrawingNode);
    } else if (nodePtr->GetType() == RSRenderNodeType::LOGICAL_DISPLAY_NODE) {
        auto logicalDisplayNode = nodePtr->ReinterpretCastTo<RSLogicalDisplayRenderNode>();
        logicalDisplayNodeMap_.emplace(id, logicalDisplayNode);
    } else if (nodePtr->GetType() == RSRenderNodeType::SCREEN_NODE) {
        auto screenNode = nodePtr->ReinterpretCastTo<RSScreenRenderNode>();
        screenNodeMap_.emplace(id, screenNode);
    }
    return true;
}

void RSRenderNodeMap::RegisterUnTreeNode(NodeId id)
{
    unInTreeNodeSet_.emplace(id);
}

bool RSRenderNodeMap::UnRegisterUnTreeNode(NodeId id)
{
    auto iter = unInTreeNodeSet_.find(id);
    if (iter != unInTreeNodeSet_.end()) {
        unInTreeNodeSet_.erase(iter);
        return true;
    }
    return false;
}

void RSRenderNodeMap::UnregisterRenderNode(NodeId id)
{
    pid_t pid = ExtractPid(id);
    auto iter = renderNodeMap_.find(pid);
    if (iter != renderNodeMap_.end()) {
        auto& subMap = iter->second;
        subMap.erase(id);
        if (subMap.empty()) {
            renderNodeMap_.erase(iter);
        }
    }

    auto it = surfaceNodeMap_.find(id);
    if (it != surfaceNodeMap_.end()) {
        RemoveUIExtensionSurfaceNode(it->second);
        surfaceNodeMap_.erase(id);
        RSSurfaceFpsManager::GetInstance().UnregisterSurfaceFps(id);
    }
    residentSurfaceNodeMap_.erase(id);
    screenNodeMap_.erase(id);
    canvasDrawingNodeMap_.erase(id);
}

void RSRenderNodeMap::MoveRenderNodeMap(
    std::shared_ptr<std::unordered_map<NodeId, std::shared_ptr<RSBaseRenderNode>>> subRenderNodeMap, pid_t pid)
{
    if (!subRenderNodeMap) {
        return;
    }
    auto iter = renderNodeMap_.find(pid);
    if (iter != renderNodeMap_.end()) {
        auto& subMap = iter->second;
        // remove node from tree
        for (auto subIter = subMap.begin(); subIter != subMap.end();) {
            subIter->second->RemoveFromTree(false);
            subRenderNodeMap->emplace(subIter->first, subIter->second);
            subIter = subMap.erase(subIter);
        }
        renderNodeMap_.erase(iter);
    }
}

void RSRenderNodeMap::FilterNodeByPid(pid_t pid, bool immediate)
{
    ROSEN_LOGD("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %{public}llu",
        (unsigned long long)pid);
    bool useBatchRemoving = !immediate &&
        RSUniRenderJudgement::IsUniRender() && RSSystemProperties::GetBatchRemovingOnRemoteDiedEnabled();
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    auto iter = renderNodeMap_.find(pid);
    if (iter != renderNodeMap_.end()) {
        auto& subMap = iter->second;
        for (auto subIter = subMap.begin(); subIter != subMap.end();) {
            if (subIter->second == nullptr) {
                subIter = subMap.erase(subIter);
                continue;
            }
            if (useBatchRemoving) {
                RSRenderNodeGC::Instance().AddToOffTreeNodeBucket(subIter->second);
            } else if (auto parent = subIter->second->GetParent().lock()) {
                parent->RemoveChildFromFulllist(subIter->second->GetId());
                subIter->second->RemoveFromTree(false);
            } else {
                subIter->second->RemoveFromTree(false);
            }
            subIter->second->GetAnimationManager().FilterAnimationByPid(pid);
            subIter = subMap.erase(subIter);
        }
        renderNodeMap_.erase(iter);
    }
    EraseIf(surfaceNodeMap_, [pid, useBatchRemoving, this](const auto& pair) -> bool {
        bool shouldErase = (ExtractPid(pair.first) == pid);
        if (shouldErase) {
            RSSurfaceFpsManager::GetInstance().UnregisterSurfaceFps(pair.first);
            RemoveUIExtensionSurfaceNode(pair.second);
        }
        if (shouldErase && pair.second && useBatchRemoving) {
            if (auto parent = pair.second->GetParent().lock()) {
                parent->RemoveChildFromFulllist(pair.second->GetId());
            }
            pair.second->RemoveFromTree(false);
        }
        return shouldErase;
    });

    EraseIf(residentSurfaceNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(canvasDrawingNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(selfDrawingNodeInProcess_, [pid](const auto& pair) -> bool {
        return pair.first == pid;
    });

    EraseIf(screenNodeMap_, [pid](const auto& pair) -> bool {
        if (ExtractPid(pair.first) != pid && pair.second) {
            ROSEN_LOGD("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %{public}llu",
                (unsigned long long)pid);
            pair.second->FilterModifiersByPid(pid);
        }
        return ExtractPid(pair.first) == pid;
    });

    EraseIf(unInTreeNodeSet_, [pid](const auto& nodeId) -> bool {
        return ExtractPid(nodeId) == pid;
    });

    if (auto fallbackNode = GetAnimationFallbackNode()) {
        // remove all fallback animations belong to given pid
        fallbackNode->GetAnimationManager().FilterAnimationByPid(pid);
    }
}

void RSRenderNodeMap::TraversalNodes(std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const
{
    for (const auto& [_, subMap] : renderNodeMap_) {
        for (const auto& [_, node] : subMap) {
            func(node);
        }
    }
}

void RSRenderNodeMap::TraversalNodesByPid(int pid,
    std::function<void (const std::shared_ptr<RSBaseRenderNode>&)> func) const
{
    const auto& itr = renderNodeMap_.find(pid);
    if (itr != renderNodeMap_.end()) {
        for (const auto& [_, node] : itr->second) {
            func(node);
        }
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

void RSRenderNodeMap::TraverseSurfaceNodesBreakOnCondition(
    std::function<bool (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const
{
    for (const auto& [_, node] : surfaceNodeMap_) {
        if (func(node)) {
            break;
        }
    }
}

bool RSRenderNodeMap::ContainPid(pid_t pid) const
{
    return std::any_of(surfaceNodeMap_.begin(), surfaceNodeMap_.end(),
        [pid](const auto& pair) -> bool { return ExtractPid(pair.first) == pid; });
}

void RSRenderNodeMap::TraverseScreenNodes(std::function<void (const std::shared_ptr<RSScreenRenderNode>&)> func) const
{
    for (const auto& [_, node] : screenNodeMap_) {
        func(node);
    }
}

void RSRenderNodeMap::TraverseLogicalDisplayNodes(
    std::function<void (const std::shared_ptr<RSLogicalDisplayRenderNode>&)> func) const
{
    for (const auto& [_, node] : logicalDisplayNodeMap_) {
        func(node);
    }
}

const ResidentSurfaceNodeMap& RSRenderNodeMap::GetResidentSurfaceNodeMap() const
{
    return residentSurfaceNodeMap_;
}

template<>
const std::shared_ptr<RSBaseRenderNode> RSRenderNodeMap::GetRenderNode(NodeId id) const
{
    pid_t pid = ExtractPid(id);
    auto iter = renderNodeMap_.find(pid);
    if (iter != renderNodeMap_.end()) {
        auto subIter = (iter->second).find(id);
        if (subIter != (iter->second).end()) {
            return subIter->second;
        }
    }
    return nullptr;
}

const std::shared_ptr<RSRenderNode> RSRenderNodeMap::GetAnimationFallbackNode() const
{
    auto iter = renderNodeMap_.find(0);
    if (iter != renderNodeMap_.cend()) {
        if (auto subIter = iter->second.find(0); subIter != iter->second.end()) {
            return subIter->second;
        }
    }
    return nullptr;
}

std::vector<NodeId> RSRenderNodeMap::GetSelfDrawingNodeInProcess(pid_t pid)
{
    std::vector<NodeId> selfDrawingNodes;
    std::vector<NodeId> sortedSelfDrawingNodes;
    std::map<NodeId, std::shared_ptr<RSBaseRenderNode>> instanceNodeMap;
    auto iter = renderNodeMap_.find(pid);
    std::shared_ptr<RSBaseRenderNode> instanceRootNode;

    if (iter == renderNodeMap_.end()) {
        return selfDrawingNodes;
    }
    for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter) {
        if (!subIter->second) {
            continue;
        }
        auto surfaceNode = subIter->second->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (surfaceNode && surfaceNode->IsSelfDrawingType() && surfaceNode->IsOnTheTree()) {
            selfDrawingNodes.push_back(surfaceNode->GetId());
            auto rootNode = surfaceNode->GetInstanceRootNode();
            if (rootNode) {
                instanceNodeMap.insert({ rootNode->GetId(), rootNode });
            }
        }
    }

    if (selfDrawingNodes.size() <= 1) {
        return selfDrawingNodes;
    }

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> instanceNodeVector;
    for (const auto& pair : instanceNodeMap) {
        auto node = pair.second->ReinterpretCastTo<RSSurfaceRenderNode>();
        if (node && node->IsAppWindow()) {
            instanceNodeVector.push_back(node);
        } else {
            return selfDrawingNodes;
        }
    }

    std::stable_sort(
        instanceNodeVector.begin(), instanceNodeVector.end(), [](const auto& first, const auto& second) -> bool {
        return first->GetAppWindowZOrder() < second->GetAppWindowZOrder();
    });

    for (const auto& instanceNode : instanceNodeVector) {
        instanceNode->CollectSelfDrawingChild(instanceNode, sortedSelfDrawingNodes);
    }
    return sortedSelfDrawingNodes;
}

const std::string RSRenderNodeMap::GetSelfDrawSurfaceNameByPid(pid_t nodePid) const
{
    for (auto &t : surfaceNodeMap_) {
        if (ExtractPid(t.first) == nodePid && t.second->IsSelfDrawingType() && !t.second->IsRosenWeb()) {
            return t.second->GetName();
        }
    }
    ROSEN_LOGD("RSRenderNodeMap::GetSurfaceNameByPid no self drawing nodes belong to pid %{public}d",
        static_cast<int32_t>(nodePid));
    return "";
}

} // namespace Rosen
} // namespace OHOS
