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
#include "rs_uni_hwc_visitor.h"

#include "feature/hwc/rs_uni_hwc_compute_util.h"
#include "pipeline/rs_canvas_render_node.h"

#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {
constexpr uint32_t API18 = 18;
constexpr uint32_t INVALID_API_COMPATIBLE_VERSION = 0;

RSUniHwcVisitor::RSUniHwcVisitor(RSUniRenderVisitor& visitor) : uniRenderVisitor_(visitor) {}
RSUniHwcVisitor::~RSUniHwcVisitor() {}

void RSUniHwcVisitor::UpdateSrcRect(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    if (!surfaceHandler) {
        return;
    }
    const auto buffer = surfaceHandler->GetBuffer();
    const auto consumer = surfaceHandler->GetConsumer();
    if (!consumer || !buffer) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    auto bufferWidth = buffer->GetSurfaceBufferWidth();
    auto bufferHeight = buffer->GetSurfaceBufferHeight();
    const auto boundsWidth = property.GetBoundsWidth();
    const auto boundsHeight = property.GetBoundsHeight();
    const auto frameGravity = Gravity::RESIZE;
    const GraphicTransformType consumerTransformType = node.GetFixRotationByUser() ?
        RSUniHwcComputeUtil::GetRotateTransformForRotationFixed(node, consumer) :
        RSUniHwcComputeUtil::GetConsumerTransform(node, buffer, consumer);
    const auto dstRect = node.GetDstRect();
    Drawing::Rect dst(dstRect.left_, dstRect.top_, dstRect.GetRight(), dstRect.GetBottom());
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(frameGravity,
        {0.f, 0.f, boundsWidth, boundsHeight}, bufferWidth, bufferHeight, gravityMatrix);
    Drawing::Matrix inverseTotalMatrix;
    Drawing::Matrix inverseGravityMatrix;
    if (!totalMatrix.Invert(inverseTotalMatrix) || !gravityMatrix.Invert(inverseGravityMatrix)) {
        node.SetSrcRect({0, 0, uniRenderVisitor_.screenInfo_.width, uniRenderVisitor_.screenInfo_.height});
        return;
    }
    Drawing::Rect srcRect;
    inverseTotalMatrix.MapRect(srcRect, dst);
    inverseGravityMatrix.MapRect(srcRect, srcRect);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(srcRect.left_, srcRect.top_);
        std::swap(srcRect.right_, srcRect.bottom_);
    }
    srcRect = RSUniHwcComputeUtil::CalcSrcRectByBufferRotation(*buffer, consumerTransformType, srcRect);
    node.SetSrcRect({srcRect.GetLeft(), srcRect.GetTop(), srcRect.GetWidth(), srcRect.GetHeight()});
}

void RSUniHwcVisitor::UpdateDstRect(RSSurfaceRenderNode& node, const RectI& absRect, const RectI& clipRect)
{
    auto dstRect = absRect;
    if (node.GetHwcGlobalPositionEnabled()) {
        dstRect.left_ -= uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetX();
        dstRect.top_ -= uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetY();
    }
    if (!node.IsHardwareEnabledTopSurface() && !node.GetHwcGlobalPositionEnabled()) {
        // If the screen is expanded, intersect the destination rectangle with the screen rectangle
        dstRect = dstRect.IntersectRect(RectI(0, 0, uniRenderVisitor_.screenInfo_.width,
            uniRenderVisitor_.screenInfo_.height));
        // global positon has been transformd to screen position in absRect
    }
    // If the node is a hardware-enabled type, intersect its destination rectangle with the prepare clip rectangle
    if (node.IsHardwareEnabledType() || node.IsHardwareEnabledTopSurface()) {
        if (!node.IsDRMCrossNode()) {
            dstRect = dstRect.IntersectRect(clipRect);
        }
    }
    auto offsetX = node.GetHwcGlobalPositionEnabled() ? uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetX() : 0;
    auto offsetY = node.GetHwcGlobalPositionEnabled() ? uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetY() : 0;
    dstRect.left_ = static_cast<int>(std::round(dstRect.left_ *
        uniRenderVisitor_.screenInfo_.GetRogWidthRatio()) + offsetX);
    dstRect.top_ = static_cast<int>(std::round(dstRect.top_ *
        uniRenderVisitor_.screenInfo_.GetRogHeightRatio()) + offsetY);
    dstRect.width_ = static_cast<int>(std::round(dstRect.width_ *
        uniRenderVisitor_.screenInfo_.GetRogWidthRatio()));
    dstRect.height_ = static_cast<int>(std::round(dstRect.height_ *
        uniRenderVisitor_.screenInfo_.GetRogHeightRatio()));

    if (uniRenderVisitor_.curSurfaceNode_ && (node.GetId() != uniRenderVisitor_.curSurfaceNode_->GetId()) &&
        !node.GetHwcGlobalPositionEnabled()) {
        dstRect = dstRect.IntersectRect(uniRenderVisitor_.curSurfaceNode_->GetDstRect());
    }
    // Set the destination rectangle of the node
    node.SetDstRect(dstRect);
    node.SetDstRectWithoutRenderFit(dstRect);
}

void RSUniHwcVisitor::UpdateHwcNodeByTransform(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix)
{
    if (!node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        return;
    }
    node.SetInFixedRotation(uniRenderVisitor_.displayNodeRotationChanged_ ||
        uniRenderVisitor_.isScreenRotationAnimating_);
    const uint32_t apiCompatibleVersion = node.GetApiCompatibleVersion();
    (apiCompatibleVersion != INVALID_API_COMPATIBLE_VERSION && apiCompatibleVersion < API18) ?
        RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, uniRenderVisitor_.screenInfo_) :
        RSUniHwcComputeUtil::DealWithNodeGravity(node, totalMatrix);
    RSUniHwcComputeUtil::DealWithScalingMode(node, totalMatrix);
    RSUniHwcComputeUtil::LayerRotate(node, uniRenderVisitor_.screenInfo_);
    RSUniHwcComputeUtil::LayerCrop(node, uniRenderVisitor_.screenInfo_);
    RSUniHwcComputeUtil::CalcSrcRectByBufferFlip(node, uniRenderVisitor_.screenInfo_);
    node.SetCalcRectInPrepare(true);
}

RectI RSUniHwcVisitor::GetHwcVisibleEffectDirty(RSRenderNode& node, const RectI& globalFilterRect) const
{
    RectI childEffectRect;
    if (!globalFilterRect.IsEmpty()) {
        childEffectRect = globalFilterRect.JoinRect(node.GetFilterRect());
    }
    return childEffectRect;
}

bool RSUniHwcVisitor::UpdateIsOffscreen(RSCanvasRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    bool isCurrOffscreen = isOffscreen_;
    isOffscreen_ |= property.IsColorBlendApplyTypeOffscreen() && !property.IsColorBlendModeNone();
    // The meaning of first prepared offscreen node is either its colorBlendApplyType is not FAST or
    // its colorBlendMode is NONE.
    // Node will classified as blendWithBackground if it is the first prepared offscreen node and
    // its colorBlendMode is neither NONE nor SRC_OVER.
    node.GetHwcRecorder().SetBlendWithBackground(!isCurrOffscreen && isOffscreen_ && property.IsColorBlendModeValid());
    return isCurrOffscreen;
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node)
{
    if (node.GetAncoForceDoDirect()) {
        return;
    }
    bool bgTransport =
        static_cast<uint8_t>(node.GetRenderProperties().GetBackgroundColor().GetAlpha()) < UINT8_MAX;
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    bool isSolidColorEnbaled = stagingSurfaceParams->GetSelfDrawingNodeType() == SelfDrawingNodeType::XCOM &&
        node.GetRenderProperties().GetBackgroundColor() != RgbPalette::Black();
    if (bgTransport) {
        // use in skip updating hardware state for hwcnode with background alpha in specific situation
        if (!RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() &&
            !node.IsHardwareEnableHint()) {
#ifdef HIPERF_TRACE_ENABLE
            RS_LOGW("hiperf_surface: name:%s disabled by background color alpha < 1, "
                "surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]", node.GetName().c_str(),
                node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
                node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom(),
                node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
                node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom());
#endif
            node.SetHardwareForcedDisabledState(true);
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by background color alpha < 1",
                node.GetName().c_str(), node.GetId());
        }
        uniRenderVisitor_.curSurfaceNode_->SetExistTransparentHardwareEnabledNode(true);
        node.SetNodeHasBackgroundColorAlpha(true);
        Statistics().UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, node.GetName());
    } else if (RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag() && isSolidColorEnbaled) {
        if (!RSSystemParameters::GetSolidLayerHwcEnabled()) {
            node.SetHardwareForcedDisabledState(true);
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by solidLayer switch",
                node.GetName().c_str(), node.GetId());
            return;
        }
        stagingSurfaceParams->SetIsHwcEnabledBySolidLayer(true);
    } else if (!RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag() && isSolidColorEnbaled) {
        node.SetHardwareForcedDisabledState(true);
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by solid background color",
            node.GetName().c_str(), node.GetId());
        Statistics().UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_SOLID_BACKGROUND_ALPHA, node.GetName());
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByBufferSize(RSSurfaceRenderNode& node)
{
    if (!node.IsRosenWeb() || node.IsHardwareForcedDisabled()) {
        return;
    }
    if (!node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    auto gravity = property.GetFrameGravity();
    if (gravity != Gravity::TOP_LEFT) {
        return;
    }
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }

    auto buffer = surfaceHandler->GetBuffer();
    const auto bufferWidth = buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = buffer->GetSurfaceBufferHeight();
    auto boundsWidth = property.GetBoundsWidth();
    auto boundsHeight = property.GetBoundsHeight();

    auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer->GetSurfaceBufferTransformType(buffer, &transformType) != GSERROR_OK) {
        RS_LOGE("RSUniRenderVisitor::UpdateHwcNodeEnableByBufferSize GetSurfaceBufferTransformType failed");
    }
    if (transformType == GraphicTransformType::GRAPHIC_ROTATE_270 ||
        transformType == GraphicTransformType::GRAPHIC_ROTATE_90) {
        std::swap(boundsWidth, boundsHeight);
    }
    if ((bufferWidth < boundsWidth) || (bufferHeight < boundsHeight)) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "hwc debug: name:%s id:%" PRIu64 " buffer:[%d, %d] bounds:[%f, %f] disabled by buffer nonmatching",
            node.GetName().c_str(), node.GetId(), bufferWidth, bufferHeight, boundsWidth, boundsHeight);
#ifdef HIPERF_TRACE_ENABLE
            RS_LOGW("hiperf_surface: name:%s buffer:[%d, %d] bounds:[%f, %f] disabled by buffer nonmatching, "
                "surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]",
                node.GetName().c_str(), bufferWidth, bufferHeight, boundsWidth, boundsHeight,
                node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
                node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom(),
                node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
                node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom());
#endif
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_BUFFER_NONMATCH, node.GetName());
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableBySrcRect(RSSurfaceRenderNode& node)
{
    if (node.IsHardwareForcedDisabled()) {
        return;
    }
    bool hasRotation = false;
    if (node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetConsumer()) {
        const auto consumer = node.GetRSSurfaceHandler()->GetConsumer();
        auto rotation = RSBaseRenderUtil::GetRotateTransform(
            RSBaseRenderUtil::GetSurfaceBufferTransformType(consumer, node.GetRSSurfaceHandler()->GetBuffer()));
        hasRotation = rotation == GRAPHIC_ROTATE_90 || rotation == GRAPHIC_ROTATE_270;
    }
    node.UpdateHwcDisabledBySrcRect(hasRotation);
    if (node.IsHardwareDisabledBySrcRect()) {
#ifdef HIPERF_TRACE_ENABLE
        RS_LOGW("hiperf_surface: name:%s disabled by src Rect, surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]",
            node.GetName().c_str(),
            node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
            node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom(),
            node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
            node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom());
#endif
        node.SetHardwareForcedDisabledState(true);
        RS_OPTIONAL_TRACE_NAME_FMT(
            "hwc debug: name:%s id:%" PRIu64 " disabled by isYUVBufferFormat and localClip not match bounds",
            node.GetName().c_str(), node.GetId());
        Statistics().UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_SRC_PIXEL, node.GetName());
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByRotateAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    auto alpha = hwcNode->GetGlobalAlpha();
    auto totalMatrix = hwcNode->GetTotalMatrix();
    if (alpha < 1.0f) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by accumulated alpha:%f",
            hwcNode->GetName().c_str(), hwcNode->GetId(), alpha);
#ifdef HIPERF_TRACE_ENABLE
        RS_LOGW("hiperf_surface: name:%s disabled by accumulated alpha:%.2f, "
            "surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]", hwcNode->GetName().c_str(), alpha,
                hwcNode->GetSrcRect().GetLeft(), hwcNode->GetSrcRect().GetRight(),
                hwcNode->GetSrcRect().GetTop(), hwcNode->GetSrcRect().GetBottom(),
                hwcNode->GetSrcRect().GetLeft(), hwcNode->GetSrcRect().GetRight(),
                hwcNode->GetSrcRect().GetTop(), hwcNode->GetSrcRect().GetBottom());
#endif
        hwcNode->SetHardwareForcedDisabledState(true);
        if (!ROSEN_EQ(alpha, 0.f)) {
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                HwcDisabledReasons::DISABLED_BY_ACCUMULATED_ALPHA, hwcNode->GetName());
        }
        return;
    }
    // [planning] degree only multiples of 90 now
    float degree = RSUniRenderUtil::GetFloatRotationDegreeFromMatrix(totalMatrix);
    bool hasRotate = !ROSEN_EQ(std::remainder(degree, 90.f), 0.f, EPSILON);
    hasRotate = hasRotate || RSUniRenderUtil::HasNonZRotationTransform(totalMatrix);
    if (hasRotate) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by rotation:%f",
            hwcNode->GetName().c_str(), hwcNode->GetId(), degree);
#ifdef HIPERF_TRACE_ENABLE
        RS_LOGW("hiperf_surface: name:%s disabled by rotation:%d, surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]",
            hwcNode->GetName().c_str(), degree,
            hwcNode->GetSrcRect().GetLeft(), hwcNode->GetSrcRect().GetRight(),
            hwcNode->GetSrcRect().GetTop(), hwcNode->GetSrcRect().GetBottom(),
            hwcNode->GetSrcRect().GetLeft(), hwcNode->GetSrcRect().GetRight(),
            hwcNode->GetSrcRect().GetTop(), hwcNode->GetSrcRect().GetBottom());
#endif
        hwcNode->SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
            HwcDisabledReasons::DISABLED_BY_ROTATION, hwcNode->GetName());
        return;
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnable()
{
    std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>> ancoNodes;
#ifdef HIPERF_TRACE_ENABLE
    int inputHwclayers = 3;
#endif
    auto& curMainAndLeashSurfaces = uniRenderVisitor_.curDisplayNode_->GetAllMainAndLeashSurfaces();
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
#ifdef HIPERF_TRACE_ENABLE
        [this, &inputHwclayers, &ancoNodes](RSBaseRenderNode::SharedPtr& nodePtr) {
#else
        [this, &ancoNodes](RSBaseRenderNode::SharedPtr& nodePtr) {
#endif
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            return;
        }

        if (RSSystemProperties::GetHveFilterEnabled()) {
            const auto &preHwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
            for (const auto& preHwcNode : preHwcNodes) {
                auto hwcNodePtr = preHwcNode.lock();
                if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
                    continue;
                }
                hwcNodePtr->ResetMakeImageState();
            }
        }

        uniRenderVisitor_.UpdateHwcNodeEnableByGlobalFilter(surfaceNode);
        surfaceNode->ResetNeedCollectHwcNode();
        const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
#ifdef HIPERF_TRACE_ENABLE
        inputHwclayers += hwcNodes.size();
#endif
        std::vector<RectI> hwcRects;
        for (const auto& hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
                continue;
            }
            if (hwcNodePtr->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
                uniRenderVisitor_.drmNodes_.emplace_back(hwcNode);
                auto firstLevelNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(
                    hwcNodePtr->GetFirstLevelNode());
                hwcNodePtr->SetForceDisableClipHoleForDRM(firstLevelNode != nullptr &&
                    firstLevelNode->GetRenderProperties().IsAttractionValid());
            }
            RSUniRenderUtil::UpdateHwcNodeProperty(hwcNodePtr);
            UpdateHwcNodeEnableByRotateAndAlpha(hwcNodePtr);
            if ((hwcNodePtr->GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)) != 0) {
                ancoNodes.insert(hwcNodePtr);
            }
        }
    });
#ifdef HIPERF_TRACE_ENABLE
    RS_LOGW("hiperf_surface_counter1 %{public}" PRIu64 " ", static_cast<uint64_t>(inputHwclayers));
#endif
    uniRenderVisitor_.PrevalidateHwcNode();
    UpdateHwcNodeEnableByNodeBelow();
    uniRenderVisitor_.UpdateAncoNodeHWCDisabledState(ancoNodes);
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByNodeBelow()
{
    auto& curMainAndLeashSurfaces = uniRenderVisitor_.curDisplayNode_->GetAllMainAndLeashSurfaces();
    // this is used to record mainAndLeash surface accumulatedDirtyRegion by Pre-order traversal
    std::vector<RectI> hwcRects;
    RectI backgroundAlphaRect;
    bool isHardwareEnableByBackgroundAlpha = false;
    std::for_each(curMainAndLeashSurfaces.begin(), curMainAndLeashSurfaces.end(),
        [this, &backgroundAlphaRect, &isHardwareEnableByBackgroundAlpha]
        (RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            RS_LOGE("RSUniRenderVisitor::UpdateHwcNodeEnableByNodeBelow surfaceNode is nullptr");
            return;
        }
        // use in temporary scheme to realize DSS
        auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (!hwcNodes.empty() && RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() &&
            RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag()) {
            UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes, backgroundAlphaRect,
                isHardwareEnableByBackgroundAlpha);
        } else if (surfaceNode->ExistTransparentHardwareEnabledNode()) {
            UpdateTransparentHwcNodeEnable(hwcNodes);
        }
    });

    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this, &hwcRects](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            RS_LOGE("RSUniRenderVisitor::UpdateHwcNodeEnableByNodeBelow surfaceNode is nullptr");
            return;
        }
        // use end
        UpdateChildHwcNodeEnableByHwcNodeBelow(hwcRects, surfaceNode);
    });
}

void RSUniHwcVisitor::UpdateTransparentHwcNodeEnable(
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes)
{
    for (size_t index = 1; index < hwcNodes.size(); ++index) {
        auto transparentHwcNodeSPtr = hwcNodes[index].lock();
        if (!transparentHwcNodeSPtr) {
            continue;
        }
        const bool isTransparentEnableHwcNode = transparentHwcNodeSPtr->IsNodeHasBackgroundColorAlpha() &&
            !transparentHwcNodeSPtr->IsHardwareForcedDisabled() && transparentHwcNodeSPtr->IsHardwareEnableHint();
        if (!isTransparentEnableHwcNode) {
            continue;
        }
        auto transparentDstRect = transparentHwcNodeSPtr->GetDstRect();
        for (size_t lowerIndex = 0; lowerIndex < index; ++lowerIndex) {
            // 'lowerHwcNode' means lower device composition layer.
            auto lowerHwcNodeSPtr = hwcNodes[lowerIndex].lock();
            if (!lowerHwcNodeSPtr || !lowerHwcNodeSPtr->IsHardwareForcedDisabled()) {
                continue;
            }
            auto lowerHwcNodeRect = lowerHwcNodeSPtr->GetDstRect();
            bool isIntersect = !transparentDstRect.IntersectRect(lowerHwcNodeRect).IsEmpty();
            if (isIntersect) {
                RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by transparent hwc node"
                    " cover other disabled hwc node name:%s id:%" PRIu64,
                    transparentHwcNodeSPtr->GetName().c_str(), transparentHwcNodeSPtr->GetId(),
                    lowerHwcNodeSPtr->GetName().c_str(), lowerHwcNodeSPtr->GetId());
                transparentHwcNodeSPtr->SetHardwareForcedDisabledState(true);
                break;
            }
        }
    }
}

void RSUniHwcVisitor::UpdateChildHwcNodeEnableByHwcNodeBelow(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& appNode)
{
    const auto& hwcNodes = appNode->GetChildHardwareEnabledNodes();
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree()) {
            continue;
        }
        UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, hwcNodePtr,
            hwcNodePtr->GetIntersectedRoundCornerAABBsSize() != 0);
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelf(std::vector<RectI>& hwcRects,
    std::shared_ptr<RSSurfaceRenderNode>& hwcNode, bool isIntersectWithRoundCorner)
{
    if (!curDisplayNode_) {
        RS_LOGE("RSUniRenderVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelf curDisplayNode is null");
        return;
    }
    if (hwcNode->IsHardwareForcedDisabled()) {
        if (RSHdrUtil::CheckIsHdrSurface(*hwcNode) != HdrStatus::NO_HDR) {
            uniRenderVisitor_.curDisplayNode_->SetHasUniRenderHdrSurface(true);
        }
        return;
    }
    auto absBound = RectI();
    if (auto geo = hwcNode->GetRenderProperties().GetBoundsGeometry()) {
        absBound = geo->GetAbsRect();
    } else {
        return;
    }
    if (hwcNode->GetAncoForceDoDirect() || !isIntersectWithRoundCorner) {
        hwcRects.emplace_back(absBound);
        return;
    }
    for (const auto& rect : hwcRects) {
        for (auto& roundCornerAABB : hwcNode->GetIntersectedRoundCornerAABBs()) {
            if (!roundCornerAABB.IntersectRect(rect).IsEmpty()) {
#ifdef HIPERF_TRACE_ENABLE
                RS_LOGW("hiperf_surface: name:%s disabled by corner radius + hwc node below, "
                    "surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]", hwcNode->GetName().c_str(),
                    hwcNode->GetSrcRect().GetLeft(), hwcNode->GetSrcRect().GetRight(),
                    hwcNode->GetSrcRect().GetTop(), hwcNode->GetSrcRect().GetBottom(),
                    hwcNode->GetSrcRect().GetLeft(), hwcNode->GetSrcRect().GetRight(),
                    hwcNode->GetSrcRect().GetTop(), hwcNode->GetSrcRect().GetBottom());
#endif
                if (hwcNode->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
                    continue;
                }
                RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by corner radius + "
                    "hwc node below, rect:%s", hwcNode->GetName().c_str(), hwcNode->GetId(), rect.ToString().c_str());
                hwcNode->SetHardwareForcedDisabledState(true);
                if (RSHdrUtil::CheckIsHdrSurface(*hwcNode) != HdrStatus::NO_HDR) {
                    uniRenderVisitor_.curDisplayNode_->SetHasUniRenderHdrSurface(true);
                }
                Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                    HwcDisabledReasons::DISABLED_BY_HWC_NODE_ABOVE, hwcNode->GetName());
                return;
            }
        }
    }
    hwcRects.emplace_back(absBound);
}

void RSUniHwcVisitor::UpdateHardwareStateByHwcNodeBackgroundAlpha(
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes, RectI& backgroundAlphaRect,
    bool& isHardwareEnableByBackgroundAlpha)
{
    std::list<RectI> hwcRects;
    for (size_t i = 0; i < hwcNodes.size(); i++) {
        auto hwcNodePtr = hwcNodes[i].lock();
        if (!hwcNodePtr) {
            continue;
        }

        bool isIntersect = !backgroundAlphaRect.IntersectRect(
            hwcNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect()
            ).IsEmpty();
        if (isHardwareEnableByBackgroundAlpha && !hwcNodePtr->IsHardwareForcedDisabled() && isIntersect) {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            continue;
        }

        if (!hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled()) {
            hwcRects.push_back(hwcNodePtr->GetDstRect());
        } else if (hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled() &&
            hwcRects.size() != 0) {
            isHardwareEnableByBackgroundAlpha = true;
            backgroundAlphaRect = hwcNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
            continue;
        } else {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by hwc node backgound alpha",
                hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
        }
    }
}
void RSUniHwcVisitor::CalcHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
    const RectI& filterRect, NodeId filterNodeId, bool isReverseOrder, int32_t filterZorder)
{
    if (!node) {
        return;
    }
    auto filterNode = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(filterNodeId);
    if (filterNode) {
        bool isBackground = filterNode->GetRenderProperties().GetBackgroundFilter() != nullptr;
        bool isReverseNode = filterNode->GetCurFrameInfoDetail().curFrameReverseChildren ||
            node->GetCurFrameInfoDetail().curFrameReverseChildren;
        if (filterZorder != 0 && node->GetHwcRecorder().zOrderForCalcHwcNodeEnableByFilter_ != 0 &&
            isBackground && !isReverseNode && !isReverseOrder &&
            node->GetHwcRecorder().zOrderForCalcHwcNodeEnableByFilter_ > filterZorder) {
            return;
        }
    }
    auto bound = node->GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    bool isIntersect = !bound.IntersectRect(filterRect).IsEmpty();
    if (isIntersect) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by filter rect, filterId:%" PRIu64,
            node->GetName().c_str(), node->GetId(), filterNodeId);
#ifdef HIPERF_TRACE_ENABLE
        RS_LOGW("hiperf_surface: name:%s disabled by filter rect, surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]",
            node->GetName().c_str(),
            node->GetSrcRect().GetLeft(), node->GetSrcRect().GetRight(),
            node->GetSrcRect().GetTop(), node->GetSrcRect().GetBottom(),
            node->GetSrcRect().GetLeft(), node->GetSrcRect().GetRight(),
            node->GetSrcRect().GetTop(), node->GetSrcRect().GetBottom());
#endif
        node->SetIsHwcPendingDisabled(true);
        node->SetHardwareForcedDisabledState(true);
        node->SetHardWareDisabledByReverse(isReverseOrder);
        Statistics().UpdateHwcDisabledReasonForDFX(node->GetId(),
            HwcDisabledReasons::DISABLED_BY_FLITER_RECT, node->GetName());
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
    const RectI& filterRect, NodeId filterNodeId, bool isReverseOrder, int32_t filterZorder)
{
    if (filterRect.IsEmpty()) {
        return;
    }
    if (!node) {
        const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        if (selfDrawingNodes.empty()) {
            return;
        }
        for (auto hwcNode : selfDrawingNodes) {
            CalcHwcNodeEnableByFilterRect(hwcNode, filterRect, filterNodeId, isReverseOrder, filterZorder);
        }
    } else {
        const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            CalcHwcNodeEnableByFilterRect(hwcNodePtr, filterRect, filterNodeId, isReverseOrder, filterZorder);
        }
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByGlobalFilter(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    auto cleanFilter = transparentHwcCleanFilter_.find(node->GetId());
    bool cleanFilterFound = (cleanFilter != transparentHwcCleanFilter_.end());
    auto dirtyFilter = transparentHwcDirtyFilter_.find(node->GetId());
    bool dirtyFilterFound = (dirtyFilter != transparentHwcDirtyFilter_.end());
    auto& curMainAndLeashSurfaces = uniRenderVisitor_.curDisplayNode_->GetAllMainAndLeashSurfaces();
    for (auto it = curMainAndLeashSurfaces.rbegin(); it != curMainAndLeashSurfaces.rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        if (surfaceNode->GetId() == node->GetId()) {
            return;
        }
        const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            continue;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled() ||
                !hwcNodePtr->GetRenderProperties().GetBoundsGeometry()) {
                continue;
            }
            if (cleanFilterFound) {
                UpdateHwcNodeEnableByGlobalCleanFilter(cleanFilter->second, *hwcNodePtr);
                if (hwcNodePtr->IsHardwareForcedDisabled()) {
                    continue;
                }
            }
            if (dirtyFilterFound) {
                UpdateHwcNodeEnableByGlobalDirtyFilter(cleanFilter->second, *hwcNodePtr);
                if (hwcNodePtr->IsHardwareForcedDisabled()) {
                    continue;
                }
            }
        }
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByGlobalCleanFilter(
    const std::vector<std::pair<NodeId, RectI>>& cleanFilter, RSSurfaceRenderNode& hwcNode)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    bool intersectedWithAIBar = false;
    bool checkDrawAIBar = false;
    for (auto filter = cleanFilter.begin(); filter != cleanFilter.end(); ++filter) {
        auto geo = hwcNode.GetRenderProperties().GetBoundsGeometry();
        if (!geo) {
            return;
        }
        if (!geo->GetAbsRect().IntersectRect(filter->second).IsEmpty()) {
            auto& rendernode = nodeMap.GetRenderNode<RSRenderNode>(filter->first);
            if (rendernode == nullptr) {
                ROSEN_LOGD("RSUniHwcVisitor::UpdateHwcNodeByFilter: rendernode is null");
                continue;
            }

            if (rendernode->IsAIBarFilter()) {
                intersectedWithAIBar = true;
                if (rendernode->IsAIBarFilterCacheValid()) {
                    ROSEN_LOGD("RSUniHwcVisitor::UpdateHwcNodeByFilter: skip intersection for using cache");
                    continue;
                } else if (RSSystemProperties::GetHveFilterEnabled()) {
                    checkDrawAIBar = true;
                    continue;
                }
            }
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by transparentCleanFilter, filterId:"
                "%" PRIu64, hwcNode.GetName().c_str(), hwcNode.GetId(), filter->first);
#ifdef HIPERF_TRACE_ENABLE
            RS_LOGW("hiperf_surface: name:%s disabled by transparentCleanFilter, "
                "surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]", hwcNodePtr.GetName().c_str(),
                hwcNodePtr.GetSrcRect().GetLeft(), hwcNodePtr.GetSrcRect().GetRight(),
                hwcNodePtr.GetSrcRect().GetTop(), hwcNodePtr.GetSrcRect().GetBottom(),
                hwcNodePtr.GetSrcRect().GetLeft(), hwcNodePtr.GetSrcRect().GetRight(),
                hwcNodePtr.GetSrcRect().GetTop(), hwcNodePtr.GetSrcRect().GetBottom());
#endif
hwcNode.SetHardwareForcedDisabledState(true);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNode.GetId(),
                HwcDisabledReasons::DISABLED_BY_TRANSPARENT_CLEAN_FLITER, hwcNode.GetName());
            break;
        }
    }
    RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64", checkDrawAIBar:%d, intersectedWithAIBar:%d",
        hwcNode.GetName().c_str(), hwcNode.GetId(), checkDrawAIBar, intersectedWithAIBar);
    if (checkDrawAIBar) {
        hwcNode.SetHardwareNeedMakeImage(checkDrawAIBar);
    }
    if (intersectedWithAIBar) {
        hwcNode.SetIntersectWithAIBar(intersectedWithAIBar);
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByGlobalDirtyFilter(
    const std::vector<std::pair<NodeId, RectI>>& dirtyFilter, RSSurfaceRenderNode& hwcNode)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (auto filter = dirtyFilter.begin(); filter != dirtyFilter.end(); ++filter) {
        auto geo = hwcNode.GetRenderProperties().GetBoundsGeometry();
        if (!geo) {
            return;
        }
        if (!geo->GetAbsRect().IntersectRect(filter->second).IsEmpty()) {
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by transparentDirtyFilter, "
                "filterId:%" PRIu64, hwcNode.GetName().c_str(), hwcNode.GetId(), filter->first);
#ifdef HIPERF_TRACE_ENABLE
            RS_LOGW("hiperf_surface: name:%s disabled by transparentDirtyFilter, "
                "surfaceRect: [%d, %d, %d, %d]->[%d, %d, %d, %d]", hwcNodePtr->GetName().c_str(),
                hwcNodePtr->GetSrcRect().GetLeft(), hwcNodePtr->GetSrcRect().GetRight(),
                hwcNodePtr->GetSrcRect().GetTop(), hwcNodePtr->GetSrcRect().GetBottom(),
                hwcNodePtr->GetSrcRect().GetLeft(), hwcNodePtr->GetSrcRect().GetRight(),
                hwcNodePtr->GetSrcRect().GetTop(), hwcNodePtr->GetSrcRect().GetBottom());
#endif
            hwcNode.SetHardwareForcedDisabledState(true);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNode.GetId(),
                HwcDisabledReasons::DISABLED_BY_TRANSPARENT_DIRTY_FLITER, hwcNode.GetName());
            break;
        }
    }
}

} // namespace Rosen
} // namespace OHOS