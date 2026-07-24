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
#include "pipeline/rs_ui_render_director.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "rs_trace.h"

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
        RSRenderNodeGC::Instance().SetScbPid(ExtractPid(entryViewNodeId_));
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
    } else if (nodePtr->GetType() == RSRenderNodeType::PROTECTIVE_SOLID_NODE) {
        auto surfaceNode = nodePtr->ReinterpretCastTo<RSSurfaceRenderNode>();
        protectiveSolidNodeMap_.emplace(id, surfaceNode);
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

static constexpr size_t MAX_UNTREE_NODE_SET_SIZE = 10000;
void RSRenderNodeMap::RegisterUnTreeNode(NodeId id)
{
    if (unInTreeNodeSet_.size() >= MAX_UNTREE_NODE_SET_SIZE) {
        RS_LOGE("RegisterUnTreeNode: unInTreeNodeSet_ size exceededlimit %{public}zu, nodeId %{public}" PRIu64,
            MAX_UNTREE_NODE_SET_SIZE, id);
        return;
    }
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
    }
    residentSurfaceNodeMap_.erase(id);
    screenNodeMap_.erase(id);
    logicalDisplayNodeMap_.erase(id);
    canvasDrawingNodeMap_.erase(id);
    protectiveSolidNodeMap_.erase(id);
    auto removeIter = std::remove_if(needAttachedNode_.begin(), needAttachedNode_.end(), [id](const auto& node) {
        if (node && node->GetId() == id) {
            node->GetAttachedInfo() = std::nullopt;
            return true;
        }
        return false;
    });
    needAttachedNode_.erase(removeIter, needAttachedNode_.end());
#ifndef ROSEN_CROSS_PLATFORM
    RemoveSurfaceHandlerInfo(id);
#endif
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
    bool optMode = RSSystemProperties::GetOptBatchRemovingOnRemoteDiedEnabled();
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    auto iter = renderNodeMap_.find(pid);
    if (iter != renderNodeMap_.end()) {
        auto& subMap = iter->second;
        RS_TRACE_NAME_FMT("process renderNodeMap:pid=%d, immediate=%d, useBatchRemoving=%d, optMode=%d, subMapSize=%u,"
            " renderNodeMapSize=%u", pid, immediate, useBatchRemoving, optMode, subMap.size(), renderNodeMap_.size());
        for (auto subIter = subMap.begin(); subIter != subMap.end();) {
            if (subIter->second == nullptr) {
                subIter = subMap.erase(subIter);
                continue;
            }
            if (optMode && useBatchRemoving) {
                RSRenderNodeGC::Instance().AddToOffTreeNodeBucket(iter->first, iter->second);
                break;
            }
            if (useBatchRemoving) {
                RSRenderNodeGC::Instance().AddToOffTreeNodeBucket(subIter->second);
            } else if (auto parent = subIter->second->GetParent().lock()) {
                parent->RemoveChildFromFulllist(subIter->second->GetId());
                subIter->second->RemoveFromTree(false);
            } else {
                subIter->second->RemoveFromTree(false);
            }
            if (auto animationManager = subIter->second->GetAnimationManager()) {
                animationManager->FilterAnimationByPid(pid);
            }
            subIter = subMap.erase(subIter);
        }
        renderNodeMap_.erase(iter);
    }
    RS_TRACE_NAME_FMT("MapSize: [%u, %u, %u, %u, %u, %u]",
        surfaceNodeMap_.size(), residentSurfaceNodeMap_.size(), canvasDrawingNodeMap_.size(),
        selfDrawingNodeInProcess_.size(), logicalDisplayNodeMap_.size(), unInTreeNodeSet_.size());

    RS_TRACE_BEGIN("process surfaceNodeMap");
    EraseIf(surfaceNodeMap_, [pid, useBatchRemoving, this](const auto& pair) -> bool {
        bool shouldErase = (ExtractPid(pair.first) == pid);
        if (shouldErase) {
            RemoveUIExtensionSurfaceNode(pair.second);
        }
        if (shouldErase && pair.second && useBatchRemoving) {
            if (auto parent = pair.second->GetParent().lock()) {
                parent->RemoveChildFromFulllist(pair.second->GetId());
            }
            // delete attachedInfo when clearing surfaceNodeMap to avoid incorrect attached nodes
            pair.second->GetAttachedInfo() = std::nullopt;
            pair.second->RemoveFromTree(false);
        }
        return shouldErase;
    });
    RS_TRACE_END();

#ifndef ROSEN_CROSS_PLATFORM
    RS_TRACE_BEGIN("process surfaceHandlerInfoMap_");
    {
        std::lock_guard<std::mutex> lock(surfaceHandlerInfoMutex_);
        EraseIf(
            surfaceHandlerInfoMap_, [pid, this](const auto& pair) -> bool { return (ExtractPid(pair.first) == pid); });
    }
    RS_TRACE_END();
#endif

    RS_TRACE_BEGIN("process residentSurfaceNodeMap");
    EraseIf(residentSurfaceNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });
    RS_TRACE_END();

    RS_TRACE_BEGIN("process canvasDrawingNodeMap");
    EraseIf(canvasDrawingNodeMap_, [pid](const auto& pair) -> bool {
        return ExtractPid(pair.first) == pid;
    });
    RS_TRACE_END();

    RS_TRACE_BEGIN("process selfDrawingNodeInProcess");
    EraseIf(selfDrawingNodeInProcess_, [pid](const auto& pair) -> bool {
        return pair.first == pid;
    });
    RS_TRACE_END();

    RS_TRACE_BEGIN("process logicalDisplayNodeMap");
    EraseIf(logicalDisplayNodeMap_, [pid](const auto& pair) -> bool {
        if (ExtractPid(pair.first) != pid && pair.second) {
            ROSEN_LOGD("RSRenderNodeMap::FilterNodeByPid removing all nodes belong to pid %{public}llu",
                (unsigned long long)pid);
            pair.second->FilterModifiersByPid(pid);
        }
        if (ExtractPid(pair.first) == pid && pair.second) {
            if (auto parent = pair.second->GetParent().lock()) {
                parent->RemoveChildFromFulllist(pair.first);
            }
            pair.second->RemoveFromTree(false);
        }
        return ExtractPid(pair.first) == pid;
    });
    RS_TRACE_END();

    RS_TRACE_BEGIN("process unInTreeNodeSet");
    EraseIf(unInTreeNodeSet_, [pid](const auto& nodeId) -> bool {
        return ExtractPid(nodeId) == pid;
    });
    RS_TRACE_END();

    if (auto fallbackNode = GetAnimationFallbackNode()) {
        // remove all fallback animations belong to given pid
        if (auto animationManager = fallbackNode->GetAnimationManager()) {
            animationManager->FilterAnimationByPid(pid);
        }
    }
    RSRenderNodeGC::Instance().ReleaseNodeNotOnTree(pid);
}

void RSRenderNodeMap::DestroyTokenNode(pid_t pid, uint64_t token)
{
    // remove all nodes belong to given pid (by matching higher 32 bits of node id)
    RS_TRACE_NAME_FMT("RSRenderNodeMap::DestroyTokenNode pid is %d token is %lu", pid, token);
    auto iter = renderNodeMap_.find(pid);
    if (iter != renderNodeMap_.end()) {
        auto& subMap = iter->second;
        EraseIf(subMap, [token](const auto& pair) -> bool {
            if (!pair.second || pair.second->GetUIContextToken() != token) {
                return false;
            }
            if (pair.second->GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
#ifdef RS_MODIFIERS_DRAW_ENABLE
                if (!RSCanvasDrawingRenderNode::IsHybridEnabled()) {
                    return false;
                }
                if (!std::static_pointer_cast<RSCanvasDrawingRenderNode>(pair.second)->IsBufferDraw()) {
                    return false;
                }
#else
                return false;
#endif
            }
            pair.second->ReleaseNodeInRender();

            auto surfaceNode = pair.second->template ReinterpretCastTo<RSSurfaceRenderNode>();
            if (surfaceNode && surfaceNode->IsAppWindow()) {
                surfaceNode->SetHasDestoryRebuild(true);
            }
            if (surfaceNode && !surfaceNode->IsSelfDrawingType()) {
                return false;
            }
            if (surfaceNode && !surfaceNode->GetIsTextureExportNode()) {
                return false;
            }
            if (pair.second->GetType() == RSRenderNodeType::ROOT_NODE) {
                auto parent = pair.second->GetParent().lock();
                if (parent) {
                    parent->RemoveChildFromFulllist(pair.first);
                }
            }
            return true;
        });
    }

    RS_TRACE_BEGIN("DestroyTokenNode process surfaceNodeMap");
    EraseIf(surfaceNodeMap_, [pid, token, this](const auto& pair) -> bool {
        bool shouldErase = (ExtractPid(pair.first) == pid) && pair.second &&
                           (pair.second->GetUIContextToken() == token) && pair.second->IsSelfDrawingType();
        if (shouldErase) {
            if (auto parent = pair.second->GetParent().lock()) {
                parent->RemoveChildFromFulllist(pair.second->GetId());
            }
            // delete attachedInfo when clearing surfaceNodeMap to avoid incorrect attached nodes
            pair.second->GetAttachedInfo() = std::nullopt;
            pair.second->RemoveFromTree(false);
        }
        return shouldErase;
    });
    RS_TRACE_END();

    RS_TRACE_BEGIN("DestroyTokenNode process residentSurfaceNodeMap");
    EraseIf(residentSurfaceNodeMap_, [pid, token](const auto& pair) -> bool {
        return (ExtractPid(pair.first) == pid) && pair.second && (pair.second->GetUIContextToken() == token);
    });
    RS_TRACE_END();

#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (RSCanvasDrawingRenderNode::IsHybridEnabled()) {
        RS_TRACE_BEGIN("DestroyTokenNode process canvasDrawingNodeMap");
        EraseIf(canvasDrawingNodeMap_, [pid, token](const auto& pair) -> bool {
            bool needErase = ExtractPid(pair.first) == pid && pair.second &&
                             pair.second->GetUIContextToken() == token && pair.second->IsBufferDraw();
            if (needErase) {
                pair.second->OnDestoryTokenNode();
            }
            return needErase;
        });
        RS_TRACE_END();
    }
#endif

    RS_TRACE_BEGIN("DestroyTokenNode process selfDrawingNodeInProcess");
    auto selfDrawingIter = selfDrawingNodeInProcess_.find(pid);
    if (selfDrawingIter != selfDrawingNodeInProcess_.end()) {
        auto& selfDrawingSubMap = selfDrawingIter->second;
        EraseIf(selfDrawingSubMap, [token](const auto &pair) -> bool {
            if (pair.second && pair.second->GetUIContextToken() == token) {
                auto parent = pair.second->GetParent().lock();
                if (parent) {
                    parent->RemoveChildFromFulllist(pair.first);
                }
                RS_TRACE_NAME_FMT("DestroyTokenNode selfDrawingNodeInProcess_ id is %lu", pair.first);
                return true;
            }
            return false;
        });
    }
    RS_TRACE_END();

    RS_TRACE_BEGIN("DestroyTokenNode process unInTreeNodeSet");
    EraseIf(unInTreeNodeSet_, [pid, token](const auto& nodeId) -> bool {
        return ExtractPid(nodeId) == pid;
    });
    RS_TRACE_END();
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

size_t RSRenderNodeMap::GetNodeCountByPid(pid_t pid) const
{
    const auto& itr = renderNodeMap_.find(pid);
    if (itr != renderNodeMap_.end()) {
        return itr->second.size();
    }
    return 0;
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

void RSRenderNodeMap::TraverseProtectiveSolidNodes(
    std::function<void (const std::shared_ptr<RSSurfaceRenderNode>&)> func) const
{
    for (const auto& [_, node] : protectiveSolidNodeMap_) {
        func(node);
    }
}

size_t RSRenderNodeMap::GetProtectiveSolidNodeMapSize() const
{
    return protectiveSolidNodeMap_.size();
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

bool RSRenderNodeMap::AttachToDisplay(
    std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode, ScreenId screenId, bool toContainer) const
{
#ifndef ROSEN_ARKUI_X
    bool result = false;
    surfaceRenderNode->GetAttachedInfo() = std::nullopt;
    std::shared_ptr<RSRenderNode> displayRenderNodeTop = nullptr;
    for (const auto& [_, displayRenderNode] : logicalDisplayNodeMap_) {
        if (displayRenderNode == nullptr || displayRenderNode->GetScreenId() != screenId ||
            !displayRenderNode->IsOnTheTree() ||
            displayRenderNode->GetBootAnimation() != surfaceRenderNode->GetBootAnimation()) {
            continue;
        }
        auto parentNode = displayRenderNode->GetWindowContainer();
        if (!toContainer || parentNode == nullptr || !parentNode->IsOnTheTree()) {
            parentNode = displayRenderNode;
            RS_LOGI("RSRenderNodeMap::AttachToDisplay %{public}" PRIu64 " attach to %{public}" PRIu64,
                surfaceRenderNode->GetId(), parentNode->GetId());
        } else {
            RS_LOGI("RSRenderNodeMap::AttachToWindowContainer %{public}" PRIu64 " attach to %{public}" PRIu64,
                surfaceRenderNode->GetId(), parentNode->GetId());
        }
        if (parentNode == surfaceRenderNode->GetParent().lock()) {
            continue;
        }
        if (displayRenderNodeTop == nullptr) {
            displayRenderNodeTop = parentNode;
        } else {
            auto positionZcur = parentNode->GetRenderProperties().GetPositionZ();
            auto positionZd = displayRenderNodeTop->GetRenderProperties().GetPositionZ();
            if (positionZcur > positionZd) {
                displayRenderNodeTop = parentNode;
            }
        }
    }
    if (displayRenderNodeTop != nullptr) {
        RS_LOGI("RSRenderNodeMap::AttachToDisplay surfaceRenderNode:%{public}" PRIu64
                " attach to displayRenderNodeTop:%{public}" PRIu64,
            surfaceRenderNode->GetId(),
            displayRenderNodeTop->GetId());
        displayRenderNodeTop->AddChild(surfaceRenderNode);
        result = true;
    }
    return result;
#else
    return false;
#endif
}

void RSRenderNodeMap::RegisterNeedAttachedNode(std::shared_ptr<RSSurfaceRenderNode> surfaceRenderNode)
{
    needAttachedNode_.emplace_back(surfaceRenderNode);
}

void RSRenderNodeMap::RegisterSurfaceHandler(NodeId nodeId, std::shared_ptr<RSSurfaceHandler> surfaceHandler)
{
    std::lock_guard<std::mutex> lock(surfaceHandlerMutex_);
    surfaceHandlerMap_.emplace(nodeId, surfaceHandler);
}

std::shared_ptr<RSSurfaceHandler> RSRenderNodeMap::GetSurfaceHandler(NodeId nodeId, bool unregister)
{
    std::lock_guard<std::mutex> lock(surfaceHandlerMutex_);
    auto it = surfaceHandlerMap_.find(nodeId);
    if (it == surfaceHandlerMap_.end()) {
        return nullptr;
    }

    auto surfaceHandler = it->second;
    if (unregister) {
        surfaceHandlerMap_.erase(it);
    }
    return surfaceHandler;
}

void RSRenderNodeMap::UnregisterSurfaceHandlerByPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(surfaceHandlerMutex_);
    auto it = surfaceHandlerMap_.begin();
    while (it != surfaceHandlerMap_.end()) {
        if (ExtractPid(it->first) == pid) {
            it = surfaceHandlerMap_.erase(it);
        } else {
            ++it;
        }
    }
}

#ifndef ROSEN_CROSS_PLATFORM
void RSRenderNodeMap::SaveSurfaceHandlerInfo(
    NodeId nodeId, const std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> &info)
{
    std::lock_guard<std::mutex> lock(surfaceHandlerInfoMutex_);
    surfaceHandlerInfoMap_[nodeId] = info;
}

std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>> RSRenderNodeMap::GetSurfaceHandlerInfo(
    NodeId nodeId) const
{
    std::lock_guard<std::mutex> lock(surfaceHandlerInfoMutex_);
    auto it = surfaceHandlerInfoMap_.find(nodeId);
    if (it != surfaceHandlerInfoMap_.end()) {
        return it->second;
    }
    return std::pair<std::shared_ptr<RSSurfaceHandler>, sptr<IBufferConsumerListener>>();
}

void RSRenderNodeMap::RemoveSurfaceHandlerInfo(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(surfaceHandlerInfoMutex_);
    surfaceHandlerInfoMap_.erase(nodeId);
}
#endif // ROSEN_CROSS_PLATFORM
} // namespace Rosen
} // namespace OHOS
