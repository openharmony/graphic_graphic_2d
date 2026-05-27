/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/opinc/rs_opinc_manager.h"

#include "common/rs_optional_trace.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
constexpr int32_t LAYER_PART_RENDER_DIRTY_MANAGER_BUFFER_AGE = 4;

namespace {
void DisableLayerPartRender(RSRenderNode& node, RSRenderParams& stagingRenderParams)
{
    node.MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, false, false);
    node.CheckDrawingCacheType();
    stagingRenderParams.SetDrawingCacheType(node.GetDrawingCacheType());
    stagingRenderParams.SetLayerPartRenderEnabled(false);
}
} // namespace

RSOpincManager& RSOpincManager::Instance()
{
    static RSOpincManager instance;
    return instance;
}

void RSOpincManager::AddOpincCacheMem(int64_t cacheMem)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cacheMem_ += cacheMem;
    cacheCount_++;
}

void RSOpincManager::ReduceOpincCacheMem(int64_t cacheMem)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (cacheMem_ >= cacheMem) {
        cacheMem_ -= cacheMem;
        cacheCount_--;
        return;
    }
    RS_LOGD("Opinc ReduceOpincCacheMem: CacheMem:%{public}" PRId64
        " exceed current cache mem:%{public}" PRId64, cacheMem * COLOR_CHANNEL,
        cacheMem_ * COLOR_CHANNEL);
    cacheMem_ = 0;
    cacheCount_ = 0;
}

int64_t RSOpincManager::GetOpincCacheMem()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return cacheMem_;
}

int32_t RSOpincManager::GetOpincCacheCount()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return cacheCount_;
}

bool RSOpincManager::OpincGetNodeSupportFlag(RSRenderNode& node)
{
    auto nodeType = node.GetType();
    auto supportFlag = false;
    if (nodeType == RSRenderNodeType::CANVAS_NODE) {
        supportFlag = OpincGetCanvasNodeSupportFlag(node);
    } else if (nodeType == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto* opincRootCache = node.TryGetOpincRootCachePtr();
        supportFlag =
            (opincRootCache == nullptr || !opincRootCache->IsSuggestOpincNode()) && OpincGetCanvasNodeSupportFlag(node);
    }

    node.GetOpincCache().SetCurNodeTreeSupportFlag(supportFlag);
    return supportFlag;
}

bool RSOpincManager::OpincGetCanvasNodeSupportFlag(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    if (node.GetSharedTransitionParam() ||
        property.IsSpherizeValid() ||
        property.IsAttractionValid() ||
        property.NeedFilter() ||
        property.GetUseEffect() ||
        property.HasHarmonium() ||
        property.GetColorBlend().has_value() ||
        node.ChildHasVisibleFilter() ||
        node.ChildHasVisibleEffect()) {
        return false;
    }
    return node.GetOpincCache().GetSubTreeSupportFlag();
}

bool RSOpincManager::IsOpincSubTreeDirty(RSRenderNode& node, bool opincEnable)
{
    const auto* opincRootCache = node.TryGetOpincRootCachePtr();
    if (opincRootCache == nullptr) {
        return false;
    }
    auto& mutableOpincRootCache = node.GetOpincRootCache();
    auto subTreeDirty = mutableOpincRootCache.OpincForcePrepareSubTree(opincEnable,
        node.IsSubTreeDirty() || node.IsContentDirty(), OpincGetNodeSupportFlag(node));
    return subTreeDirty;
}

void RSOpincManager::QuickMarkStableNode(RSRenderNode& node, bool& unchangeMarkInApp, bool& unchangeMarkEnable,
    bool isAccessibilityConfigChanged)
{
    const auto* opincRootCache = node.TryGetOpincRootCachePtr();
    // The opinc state needs to be reset in the following cases:
    // 1. subtree is dirty
    // 2. content is dirty
    // 3. the node is marked as a node group
    // 4. the node is marked as a root node of opinc and the high-contrast state change
    auto isSelfDirty = node.IsSubTreeDirty() || node.IsContentDirty() ||
        node.GetNodeGroupType() > RSRenderNode::NodeGroupType::NONE ||
        (opincRootCache != nullptr && opincRootCache->OpincGetRootFlag() && isAccessibilityConfigChanged);
    if (opincRootCache != nullptr) {
        auto& mutableOpincRootCache = node.GetOpincRootCache();
        mutableOpincRootCache.OpincQuickMarkStableNode(unchangeMarkInApp, unchangeMarkEnable, isSelfDirty);
    }
    auto& stagingRenderParams = node.GetStagingRenderParams();
    if (unchangeMarkEnable && stagingRenderParams) {
        stagingRenderParams->OpincSetCacheChangeFlag(
            opincRootCache != nullptr && opincRootCache->GetCacheChangeFlag(), node.GetLastFrameSync());
        stagingRenderParams->OpincUpdateRootFlag(opincRootCache != nullptr && opincRootCache->OpincGetRootFlag());
        stagingRenderParams->OpincUpdateSupportFlag(node.GetOpincCache().GetCurNodeTreeSupportFlag());
    }
}

void RSOpincManager::UpdateRootFlag(RSRenderNode& node, bool& unchangeMarkEnable)
{
    const auto* opincRootCache = node.TryGetOpincRootCachePtr();
    if (opincRootCache == nullptr) {
        return;
    }
    auto& mutableOpincRootCache = node.GetOpincRootCache();
    mutableOpincRootCache.OpincUpdateRootFlag(unchangeMarkEnable, OpincGetNodeSupportFlag(node));
    auto& stagingRenderParams = node.GetStagingRenderParams();
    if (opincRootCache->isOpincRootFlag_ && stagingRenderParams) {
        stagingRenderParams->OpincUpdateRootFlag(true);
    }
}

void RSOpincManager::QuickCheckOpincStable(
    RSRenderNode& node, bool& unchangeMarkInApp, bool& unchangeMarkEnable, bool& hasUnstableOpincNode)
{
    const auto* nodeOpincRootCache = node.TryGetOpincRootCachePtr();
    if (!GetOPIncSwitch() || !node.GetOpincCache().HasUnstableOpincNode()) {
        return;
    }

    bool allChildHasUnstableOpincNode = false;

    for (const auto& child : *node.GetSortedChildren()) {
        bool childHasUnstableOpincNode = false;
        auto& opincCache = child->GetOpincCache();
        const auto* childOpincRootCache = child->TryGetOpincRootCachePtr();

        if (childOpincRootCache == nullptr || !childOpincRootCache->IsSuggestOpincNode()
            || childOpincRootCache->IsOpincUnchangeState()) {
            QuickCheckOpincStable(*child, unchangeMarkInApp, unchangeMarkEnable, childHasUnstableOpincNode);
            opincCache.SetHasUnstableOpincNode(childHasUnstableOpincNode);
            allChildHasUnstableOpincNode |= childHasUnstableOpincNode;
            continue;
        }

        QuickMarkStableNode(*child, unchangeMarkInApp, unchangeMarkEnable, false);
        QuickCheckOpincStable(*child, unchangeMarkInApp, unchangeMarkEnable, childHasUnstableOpincNode);
        UpdateRootFlag(*child, unchangeMarkEnable);
        child->AddToPendingSyncList();

        opincCache.SetHasUnstableOpincNode(
            childHasUnstableOpincNode ||
            (childOpincRootCache != nullptr && childOpincRootCache->IsSuggestOpincNode() &&
                !childOpincRootCache->IsOpincUnchangeState()));
        allChildHasUnstableOpincNode |= opincCache.HasUnstableOpincNode();
    }
    node.GetOpincCache().SetHasUnstableOpincNode(
        allChildHasUnstableOpincNode ||
        (nodeOpincRootCache != nullptr && nodeOpincRootCache->IsSuggestOpincNode() &&
            !nodeOpincRootCache->IsOpincUnchangeState()));
    hasUnstableOpincNode = node.GetOpincCache().HasUnstableOpincNode();
}

OpincUnsupportType RSOpincManager::GetUnsupportReason(RSRenderNode& node)
{
    if (!node.GetOpincCache().GetSubTreeSupportFlag()) {
        return OpincUnsupportType::CHILD_NOT_SUPPORT;
    }
    if (node.GetSharedTransitionParam()) {
        return OpincUnsupportType::SHARED_TRANSITION;
    }
    const auto& property = node.GetRenderProperties();
    if (property.IsSpherizeValid()) {
        return OpincUnsupportType::SPHERIZE;
    }
    if (property.IsAttractionValid()) {
        return OpincUnsupportType::ATTRACTION;
    }
    if (property.NeedFilter()) {
        return OpincUnsupportType::HAS_FILTER;
    }
    if (property.GetUseEffect()) {
        return OpincUnsupportType::USE_EFFECT;
    }
    if (property.HasHarmonium()) {
        return OpincUnsupportType::HAS_HARMONIUM;
    }
    if (property.GetColorBlend().has_value()) {
        return OpincUnsupportType::COLOR_BLEND;
    }
    if (node.ChildHasVisibleFilter()) {
        return OpincUnsupportType::CHILD_HAS_FILTER;
    }
    if (node.ChildHasVisibleEffect()) {
        return OpincUnsupportType::CHILD_HAS_EFFECT;
    }
    return OpincUnsupportType::NONE;
}

std::string RSOpincManager::QuickGetNodeDebugInfo(RSRenderNode& node)
{
    std::string ret("");
    auto* opincRootCache = node.TryGetOpincRootCachePtr();
    auto nodeState = opincRootCache == nullptr ? NodeCacheState::STATE_INIT : opincRootCache->GetNodeCacheState();
    auto unchangeCount = opincRootCache == nullptr ? 0 : opincRootCache->GetUnchangeCount();
    auto suggestFlag = opincRootCache == nullptr ? false : opincRootCache->IsSuggestOpincNode();
    auto supportFlag = node.GetOpincCache().GetCurNodeTreeSupportFlag();
    auto rootFlag = opincRootCache == nullptr ? false : opincRootCache->OpincGetRootFlag();
    AppendFormat(ret,
        "%" PRIu64 ", subD:%d conD:%d s:%d uc:%d suggest:%d support:%d rootF:%d not_sup_reason:%d",
        node.GetId(), node.IsSubTreeDirty(), node.IsContentDirty(), nodeState, unchangeCount,
        suggestFlag, supportFlag, rootFlag, GetUnsupportReason(node));
    return ret;
}

void RSOpincManager::OpincSetInAppStateEnd(RSRenderNode& node, bool& unchangeMarkInApp)
{
    auto& opincCache = node.GetOpincCache();
    opincCache.OpincSetInAppStateEnd(unchangeMarkInApp);
}

void RSOpincManager::OpincSetInAppStateStart(RSRenderNode& node, bool& unchangeMarkInApp)
{
    auto& opincCache = node.GetOpincCache();
    opincCache.OpincSetInAppStateStart(unchangeMarkInApp);
}

bool RSOpincManager::IsSuggestOpincNode(RSRenderNode& node)
{
    const auto* opincRootCache = node.TryGetOpincRootCachePtr();
    return opincRootCache != nullptr && opincRootCache->IsSuggestOpincNode();
}

bool RSOpincManager::OpincGetRootFlag(RSRenderNode& node)
{
    const auto* opincRootCache = node.TryGetOpincRootCachePtr();
    return opincRootCache != nullptr && opincRootCache->OpincGetRootFlag();
}

bool RSOpincManager::IsOpincUnchangeState(RSRenderNode& node)
{
    const auto* opincRootCache = node.TryGetOpincRootCachePtr();
    return opincRootCache != nullptr && opincRootCache->IsOpincUnchangeState();
}

void RSOpincManager::InitLayerPartRenderNode(bool isCCMLayerPartRenderEnabled, RSRenderNode& node,
    std::shared_ptr<RSDirtyRegionManager>& layerPartRenderDirtyManager)
{
    if (!isCCMLayerPartRenderEnabled) {
        return;
    }
    const auto* layerPartRenderCache = node.TryGetLayerPartRenderCachePtr();
    if (layerPartRenderCache == nullptr) {
        return;
    }
    if (!layerPartRenderCache->IsSuggestLayerPartRenderNode() &&
        layerPartRenderCache->GetLayerPartRenderNodeStrategyType() != NodeStrategyType::CACHE_DISABLE) {
        return;
    }

    auto& mutableLayerPartRenderCache = node.GetLayerPartRenderCache();
    layerPartRenderDirtyManager = mutableLayerPartRenderCache.GetLayerPartRenderDirtyManager();
    if (layerPartRenderDirtyManager != nullptr) {
        layerPartRenderDirtyManager->Clear();
    }
    mutableLayerPartRenderCache.SetLayerPartRender(true);
}

bool RSOpincManager::CalculateLayerPartRenderDirtyRegion(RSRenderNode& node,
    std::shared_ptr<RSDirtyRegionManager>& layerPartRenderDirtyManager, const RectI& visibleFilterRect,
    RectI& layerCurDirty)
{
    auto& geoPtr = node.GetRenderProperties().GetBoundsGeometry();
    if (geoPtr == nullptr) {
        return false;
    }
    auto matrix = geoPtr->GetAbsMatrix();
    Drawing::Matrix invertMatrix;
    if (!matrix.Invert(invertMatrix)) {
        return false;
    }

    auto& layerPartRenderCache = node.GetLayerPartRenderCache();
    if (node.GetNodeGroupType() != RSRenderNode::NodeGroupType::GROUPED_BY_USER) {
        layerPartRenderCache.ResetLayerPartRenderUnchangeState();
    }

    const RectI& nodeAbsRect = geoPtr->GetAbsRect();
    if (!layerPartRenderCache.IsLayerPartRenderUnchangeState()) {
        layerCurDirty = nodeAbsRect;
        layerPartRenderDirtyManager->MergeDirtyRect(layerCurDirty);
        layerPartRenderDirtyManager->UpdateDirty();
        layerCurDirty = geoPtr->MapRect(layerCurDirty.ConvertTo<float>(), invertMatrix);
        [[maybe_unused]] auto nodeAbsRectMap = geoPtr->MapRect(nodeAbsRect.ConvertTo<float>(), invertMatrix);
        RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", UnchangeState convert to layerCurDirtyMap:[%d,%d,%d,%d], "
            "nodeAbsRectMap:[%d,%d,%d,%d], nodeAbsRect:[%d,%d,%d,%d]", node.GetId(),
            layerCurDirty.GetLeft(), layerCurDirty.GetTop(), layerCurDirty.GetWidth(), layerCurDirty.GetHeight(),
            nodeAbsRectMap.GetLeft(), nodeAbsRectMap.GetTop(), nodeAbsRectMap.GetWidth(), nodeAbsRectMap.GetHeight(),
            nodeAbsRect.GetLeft(), nodeAbsRect.GetTop(), nodeAbsRect.GetWidth(), nodeAbsRect.GetHeight());
        return true;
    }

    layerPartRenderDirtyManager->MergeDirtyRectIfIntersect(visibleFilterRect);
    layerPartRenderDirtyManager->SetBufferAge(LAYER_PART_RENDER_DIRTY_MANAGER_BUFFER_AGE);
    layerPartRenderDirtyManager->UpdateDirty();
    layerCurDirty = layerPartRenderDirtyManager->GetDirtyRegion();
    {
        RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", layerCurDirty:[%d,%d,%d,%d], visibleFilterRect:[%d,%d,%d,%d], "
            "nodeAbsRect:[%d,%d,%d,%d]", node.GetId(),
            layerCurDirty.GetLeft(), layerCurDirty.GetTop(), layerCurDirty.GetWidth(), layerCurDirty.GetHeight(),
            visibleFilterRect.GetLeft(), visibleFilterRect.GetTop(),
            visibleFilterRect.GetWidth(), visibleFilterRect.GetHeight(),
            nodeAbsRect.GetLeft(), nodeAbsRect.GetTop(), nodeAbsRect.GetWidth(), nodeAbsRect.GetHeight());
    }
    if (!layerCurDirty.IsInsideOf(nodeAbsRect)) {
        // If intersect, use intersection; otherwise use full node rect
        if (layerCurDirty.Intersect(nodeAbsRect)) {
            layerCurDirty = layerCurDirty.IntersectRect(nodeAbsRect);
            RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", clip layerCurDirty to intersect with nodeAbsRect:[%d,%d,%d,%d]",
                node.GetId(), layerCurDirty.GetLeft(), layerCurDirty.GetTop(), layerCurDirty.GetWidth(),
                layerCurDirty.GetHeight());
            layerPartRenderDirtyManager->MergeDirtyRect(layerCurDirty);
            layerPartRenderDirtyManager->UpdateDirty();
        } else {
            layerCurDirty = nodeAbsRect;
            RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", layerCurDirty not intersect, use full nodeAbsRect:[%d,%d,%d,%d]",
                node.GetId(), layerCurDirty.GetLeft(), layerCurDirty.GetTop(), layerCurDirty.GetWidth(),
                layerCurDirty.GetHeight());
            layerPartRenderDirtyManager->MergeDirtyRect(layerCurDirty);
            layerPartRenderDirtyManager->UpdateDirty();
        }
    }
    layerCurDirty = geoPtr->MapRect(layerCurDirty.ConvertTo<float>(), invertMatrix);
    [[maybe_unused]] auto nodeAbsRectMap = geoPtr->MapRect(nodeAbsRect.ConvertTo<float>(), invertMatrix);
    RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", convert to layerCurDirtyMap:[%d,%d,%d,%d], nodeAbsRectMap:[%d,%d,%d,%d]",
        node.GetId(), layerCurDirty.GetLeft(), layerCurDirty.GetTop(),
        layerCurDirty.GetWidth(), layerCurDirty.GetHeight(),
        nodeAbsRectMap.GetLeft(), nodeAbsRectMap.GetTop(), nodeAbsRectMap.GetWidth(), nodeAbsRectMap.GetHeight());

    return true;
}

void RSOpincManager::CalculateAndUpdateLayerPartRenderDirtyRegion(RSRenderNode& node,
    std::shared_ptr<RSDirtyRegionManager>& layerPartRenderDirtyManager, const RectI& visibleFilterRect,
    bool isDisableAnimation)
{
    if (layerPartRenderDirtyManager == nullptr) {
        return;
    }

    auto& stagingRenderParams = node.GetStagingRenderParams();
    if (stagingRenderParams == nullptr) {
        return;
    }

    // mark is cleared, cache need to be cleared.
    const auto* layerPartRenderCache = node.TryGetLayerPartRenderCachePtr();
    if (layerPartRenderCache == nullptr) {
        return;
    }
    auto& mutableLayerPartRenderCache = node.GetLayerPartRenderCache();
    if (layerPartRenderCache->GetLayerPartRenderNodeStrategyType() == NodeStrategyType::CACHE_DISABLE) {
        RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", LayerPartRender clean cache", node.GetId());
        DisableLayerPartRender(node, *stagingRenderParams);
        mutableLayerPartRenderCache.SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_NONE);
        layerPartRenderDirtyManager = nullptr;
        return;
    }
    if (!layerPartRenderCache->IsLayerPartRender()) {
        return;
    }

    if (node.GetOpincCache().IsMaterialNode()) {
        RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", has material node, suspend layer part render", node.GetId());
        mutableLayerPartRenderCache.MarkSuggestLayerPartRenderNode(false);
        mutableLayerPartRenderCache.SetLayerPartRenderNodeStrategyType(NodeStrategyType::CACHE_DISABLE);
        return;
    }

    RectI layerCurDirty;
    if (!CalculateLayerPartRenderDirtyRegion(node, layerPartRenderDirtyManager, visibleFilterRect, layerCurDirty) ||
        layerPartRenderDirtyManager->HasUifirstChild() || isDisableAnimation) {
        DisableLayerPartRender(node, *stagingRenderParams);
        RS_OPTIONAL_TRACE_FMT("id:%" PRIu64 ", isDisableAnimation or Calculate error or has uifirst node, clear",
            node.GetId());
        return;
    }

    if (node.GetNodeGroupType() != RSRenderNode::NodeGroupType::GROUPED_BY_USER) {
        node.MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_USER, true, false);
    }
    stagingRenderParams->SetLayerPartRenderEnabled(true);
    stagingRenderParams->SetLayerPartRenderCurrentFrameDirtyRegion(layerCurDirty);
    node.AddToPendingSyncList();
    layerPartRenderDirtyManager = nullptr;
}
}
}