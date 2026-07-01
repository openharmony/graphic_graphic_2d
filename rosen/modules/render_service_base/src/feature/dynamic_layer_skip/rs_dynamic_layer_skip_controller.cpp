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
#include "feature/dynamic_layer_skip/rs_dynamic_layer_skip_controller.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {

void LayerSkipContext::SyncFrom(const RSDynamicLayerSkipController& srcController)
{
    screenLayerInvalid_ = srcController.screenLayerInvalid_;
    relevantSurfaceNodeIds_.clear();
    std::for_each(
        srcController.targetSelfDrawingSurface_.begin(), srcController.targetSelfDrawingSurface_.end(),
        [this](const auto& surface) {
            if (auto surfacePtr = surface.lock()) {
                relevantSurfaceNodeIds_.push_back(surfacePtr->GetId());
            }
        });
}

void LayerSkipContext::Reset()
{
    screenLayerInvalid_ = false;
    relevantSurfaceNodeIds_.clear();
}

bool RSDynamicLayerSkipController::IsScreenLayerInvalid() const
{
    return screenLayerInvalid_;
}

void RSDynamicLayerSkipController::Init(const RectI& screenRect, bool globalDisabled)
{
    globalDisabled_ = globalDisabled;
    screenRect_ = screenRect;
    screenLayerInvalid_ = false;
    globalOccluderDetected_ = false;
    occluderInstanceRootNodeId_ = INVALID_NODEID;
    targetSelfDrawingSurface_.clear();
    visitedRenderNodeCount_ = 0;
    lastFrameHasFullScreenSurface_ = hasFullScreenSurface_;
    hasFullScreenSurface_ = false;
}

void RSDynamicLayerSkipController::CheckNodeDrawProperty(RSRenderNode& node)
{
    // functions to decide whether a node has draw content or not.
    if (GetBit(node, LayerDrawContent::UPDATE)) {
        visitedRenderNodeCount_++;
        bool nodeVisible = ROSEN_NE(node.GetGlobalAlpha(), 0.f) && !node.IsPureContainer() && node.ShouldPaint();
        bool hasValidDrawableSlot = false;
        if (nodeVisible) {
            hasValidDrawableSlot =
            HasDrawContentDrawableInRange(
                node, RSDrawableSlot::TRANSITION_PROPERTIES_BEGIN, RSDrawableSlot::TRANSITION_PROPERTIES_END)
            || HasDrawContentDrawableInRange(
                node, RSDrawableSlot::BG_PROPERTIES_BEGIN, RSDrawableSlot::BG_PROPERTIES_END)
            || HasDrawContentDrawableInRange(
                node, RSDrawableSlot::CONTENT_BEGIN, RSDrawableSlot::CONTENT_END)
            || HasDrawContentDrawableInRange(
                node, RSDrawableSlot::FG_PROPERTIES_BEGIN, RSDrawableSlot::FG_PROPERTIES_END)
            || HasDrawContentDrawableInRange(
                node, RSDrawableSlot::EXTRA_PROPERTIES_BEGIN, RSDrawableSlot::EXTRA_PROPERTIES_END);
        }
        SetBit(node, LayerDrawContent::SELF, nodeVisible && hasValidDrawableSlot);
        // self-drawing surface node is considered to has draw content.
        auto surfaceNode = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        OrBit(node, LayerDrawContent::SELF, surfaceNode && surfaceNode->IsSelfDrawingType());
        SetBit(node, LayerDrawContent::UPDATE, false);
    }
}

bool RSDynamicLayerSkipController::HasDrawContentDrawableInRange(
    const RSRenderNode& node, RSDrawableSlot begin, RSDrawableSlot end) const
{
    if (UNLIKELY(node.drawableVec_ == nullptr)) {
        return false;
    }
    auto& drawableVec = *node.drawableVec_;
    auto beginIt = drawableVec.lower_bound(static_cast<int8_t>(begin));
    auto endIt = drawableVec.upper_bound(static_cast<int8_t>(end));
    return std::any_of(beginIt, endIt, [](const auto& drawablePair) {
        return drawablePair.first != static_cast<int8_t>(RSDrawableSlot::CHILDREN)
                && drawablePair.first != static_cast<int8_t>(RSDrawableSlot::CLIP_TO_FRAME)
                && drawablePair.second != nullptr;
    });
}

void RSDynamicLayerSkipController::VisitRenderNodeInner(std::shared_ptr<RSRenderNode> node, bool needTraverse)
{
    if (UNLIKELY(node == nullptr)) {
        return;
    }

    CheckNodeDrawProperty(*node);
    auto children = node->GetSortedChildren();
    if (needTraverse && children != nullptr && GetBit(*node, LayerDrawContent::SUBTREE_UPDATE)) {
        SetBit(*node, LayerDrawContent::SUBTREE_UPDATE, false);
        SetBit(*node, LayerDrawContent::SUBTREE, false);
        std::for_each((*children).begin(), (*children).end(), [this, &node](const auto& child) {
            VisitRenderNodeInner(child, true);
            OrBit(*node, LayerDrawContent::SUBTREE, child == nullptr ?
                false : GetBit(*child, LayerDrawContent::SUBTREE) || GetBit(*child, LayerDrawContent::SELF));
        });
    }
    if (!globalOccluderDetected_) {
        // for canvas node above surface node, check its prop without considering its subtree.
        // for surface node and other nodes below it, check its prop and also its subtree.
        bool occluderDetected =
        (!needTraverse && GetBit(*node, LayerDrawContent::SELF)) ||
        (needTraverse && (GetBit(*node, LayerDrawContent::SELF) || GetBit(*node, LayerDrawContent::SUBTREE)));
        if (occluderDetected) {
            globalOccluderDetected_ = true;
            occluderInstanceRootNodeId_ = node->GetInstanceRootNodeId();
            RS_TRACE_NAME_FMT("%s node[%" PRIu64 "] is detected as global occluder", __func__, node->GetId());
        }
    }
}

void RSDynamicLayerSkipController::VisitRenderNode(std::shared_ptr<RSSurfaceRenderNode> surfaceNode, RSRenderNode& node)
{
    if (!lastFrameHasFullScreenSurface_ || globalOccluderDetected_) {
        return;
    }
    if (surfaceNode && surfaceNode->GetSkipDraw()) {
        RS_OPTIONAL_TRACE_NAME_FMT("%s visit node:%s, which will skip draw", __func__, surfaceNode->GetName().c_str());
        return;
    }
    if (surfaceNode == nullptr) {
        // for canvas node above surface node, check its prop without traverse its subtree.
        VisitRenderNodeInner(node.shared_from_this(), false);
    } else if (surfaceNode->GetId() == node.GetId()) {
        // for surfaceNode, traverse itself and its subtree.
        VisitRenderNodeInner(node.shared_from_this(), true);
    }
}

bool RSDynamicLayerSkipController::HasFullScreenSelfDrawingSurface(RSSurfaceRenderNode& rootNode)
{
    if (rootNode.GetSurfaceWindowType() != SurfaceWindowType::DEFAULT_WINDOW) {
        return false;
    }
    const auto& childrenHardwareEnabledNodes = rootNode.GetChildHardwareEnabledNodes();
    bool hasFullScreenSurface = std::any_of(childrenHardwareEnabledNodes.begin(), childrenHardwareEnabledNodes.end(),
        [screenRect = screenRect_](const auto& childNode) {
            auto nodePtr = childNode.lock();
            if (!nodePtr) {
                return false;
            }
            RS_OPTIONAL_TRACE_NAME_FMT(
                "Fullscreen surface?[%" PRIu64 "] dst: %s", nodePtr->GetId(), nodePtr->GetDstRect().ToString().c_str());
            return nodePtr->GetDstRect() == screenRect;
        });
    hasFullScreenSurface_ = hasFullScreenSurface_ || hasFullScreenSurface;
    return hasFullScreenSurface;
}

void RSDynamicLayerSkipController::DetectScreenLayerValidityInner(
    std::shared_ptr<RSRenderNode> node, Occlusion::Region& targetArea, int32_t totalTargetCount)
{
    if (UNLIKELY(node == nullptr)) {
        return;
    }
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>();
    if (surfaceNode != nullptr && surfaceNode->IsSelfDrawingType()) {
        // case1: for self-drawing node, collect its region.
        RS_OPTIONAL_TRACE_NAME_FMT("Detect potential surface %s", surfaceNode->GetName().c_str());
        targetSelfDrawingSurface_.emplace_back(surfaceNode);
        targetArea.OrSelf(Occlusion::Rect(surfaceNode->GetDstRect()));
    } else if (targetSelfDrawingSurface_.size() < static_cast<size_t>(totalTargetCount)) {
        // case2: for other render nodes, if there are still self-drawing nodes, recording detailed information.
        if (GetBit(*node, LayerDrawContent::SELF)) {
            targetArea.SubSelf(Occlusion::Rect(node->GetAbsDrawRect()));
            RS_OPTIONAL_TRACE_NAME_FMT("Detected Occluder[%" PRIu64 "]: %s.",
                node->GetId(), node->GetAbsDrawRect().ToString().c_str());
        }
    } else {
        // case3: for other render nodes, if self-drawing nodes are all visited, recording rough information.
        if (GetBit(*node, LayerDrawContent::SELF) || GetBit(*node, LayerDrawContent::SUBTREE)) {
            const auto& boundsGeo = node->GetRenderProperties().GetBoundsGeometry();
            auto nodeSubtreeRect =
                boundsGeo->MapAbsRect(node->selfDrawRect_.JoinRect(node->childrenRect_.ConvertTo<float>()));
            targetArea.SubSelf(Occlusion::Rect(nodeSubtreeRect));
            RS_OPTIONAL_TRACE_NAME_FMT("Detected Occluder Subtree [%" PRIu64 "]: %s.",
                node->GetId(), nodeSubtreeRect.ToString().c_str());
        }
        return;
    }
    if (auto children = node->GetSortedChildren()) {
        std::for_each((*children).begin(), (*children).end(),
            [this, &targetArea, totalTargetCount](const auto& child) {
                DetectScreenLayerValidityInner(child, targetArea, totalTargetCount);
            });
    }
}

void RSDynamicLayerSkipController::DetectScreenLayerValidity(RSSurfaceRenderNode& rootNode)
{
    // functions to detect whether a self-drawing node makes screen layer invalid or not.
    // 1. skip if node is not app-window / no-child self-drawing surface / no global occluder already detected
    bool hasFullScreenSelfDrawingSurface = HasFullScreenSelfDrawingSurface(rootNode);
    bool shouldProceedDetection = !globalDisabled_ &&
                                  rootNode.IsAppWindow() &&
                                  hasFullScreenSelfDrawingSurface &&
                                  (!globalOccluderDetected_ || occluderInstanceRootNodeId_ == rootNode.GetId()) &&
                                  targetSelfDrawingSurface_.empty() && lastFrameHasFullScreenSurface_;
    if (!shouldProceedDetection) {
        RS_OPTIONAL_TRACE_NAME_FMT("%s skip detect node:[%s], isApp[%d] hasFullSurface[%d] occluderDetected[%d]"
            " fullscreen mode[%d]", __func__, rootNode.GetName().c_str(), rootNode.IsAppWindow(),
            hasFullScreenSelfDrawingSurface, globalOccluderDetected_, lastFrameHasFullScreenSurface_);
        return;
    }

    // 2. Detection begins, recording self-drawing area and target nodes for this app node.
    int32_t targetSelfDrawingNodeCnt = static_cast<int32_t>(rootNode.GetChildHardwareEnabledNodes().size());
    Occlusion::Region targetSelfDrawingArea = {};
    DetectScreenLayerValidityInner(rootNode.shared_from_this(), targetSelfDrawingArea, targetSelfDrawingNodeCnt);

    // The scenario is considered valid only if the fllowing creteria are all met:
    // 1. The self-drawing area covers the full screen (determined by bounds and area).
    // 2. The hardware enable status of these self-drawing nodes will be re-checked later.
    bool isValidTargetApp = targetSelfDrawingArea.GetBound() == Occlusion::Rect(screenRect_) &&
        targetSelfDrawingArea.Area() == screenRect_.GetWidth() * screenRect_.GetHeight();
    RS_TRACE_NAME_FMT("Target App: %s, targetSelfDrawingArea: %s, screenRect: %s", rootNode.GetName().c_str(),
        targetSelfDrawingArea.GetRegionInfo().c_str(), screenRect_.ToString().c_str());
    if (!isValidTargetApp) {
        targetSelfDrawingSurface_.clear();
    }
}

void RSDynamicLayerSkipController::VerifyScreenLayerValidity(float screenNodeGlobalZOrder)
{
    screenLayerInvalid_ = false;
    // functions to finally decide whether screen layer is invalid or not (based on hardware enable status).
    if (globalDisabled_ || targetSelfDrawingSurface_.empty()) {
        RS_TRACE_NAME_FMT("%s skip verifying, visited node cnt:%d", __func__, visitedRenderNodeCount_);
        return;
    }
    // find any condition that needs screen layer.
    // 1. any layer that is hardware disabled.
    // 2. any layer that is above screen layer and transparent.
    screenLayerInvalid_ = !std::any_of(
        targetSelfDrawingSurface_.begin(), targetSelfDrawingSurface_.end(),
        [screenNodeGlobalZOrder](const auto& surfaceWeakPtr) {
            auto surfacePtr = surfaceWeakPtr.lock();
            auto surfaceHandler = surfacePtr ? surfacePtr->GetRSSurfaceHandler() : nullptr;
            if (UNLIKELY(surfacePtr == nullptr || surfaceHandler == nullptr)) {
                return true;
            }
            if (surfacePtr->IsHardwareForcedDisabled()) {
                RS_OPTIONAL_TRACE_NAME_FMT(
                    "%s SurfaceNode[%" PRIu64 "]" " is hardware disabled.", __func__, surfacePtr->GetId());
                return true;
            }
            if (surfaceHandler->GetGlobalZOrder() > screenNodeGlobalZOrder) {
                RS_OPTIONAL_TRACE_NAME_FMT("%s SurfaceNode[%" PRIu64 "] above screen layer, alpha status:[%d, %d, %d]",
                    __func__, surfacePtr->GetId(), surfacePtr->IsNodeHasBackgroundColorAlpha(),
                    !surfacePtr->GetSurfaceBufferOpaque(), ROSEN_NE(surfacePtr->GetGlobalAlpha(), 1.f));
                return surfacePtr->IsNodeHasBackgroundColorAlpha() ||
                    !surfacePtr->GetSurfaceBufferOpaque() || ROSEN_NE(surfacePtr->GetGlobalAlpha(), 1.f);
            }
            return false;
        });
    RS_TRACE_NAME_FMT("%s visitRenderNode %d, fullScreenSelfDrawingSurface size [%zu], screenLayerInvalid[%d]",
        __func__, visitedRenderNodeCount_, targetSelfDrawingSurface_.size(), screenLayerInvalid_);
}
} // namespace Rosen
} // namespace OHOS