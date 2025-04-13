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
#include "feature/hdr/rs_hdr_util.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "feature_cfg/feature_param/feature_param.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_system_properties.h"

#include "common/rs_common_hook.h"
#include "common/rs_optional_trace.h"
#include "rs_profiler.h"

#undef LOG_TAG
#define LOG_TAG "RSUniHwcVisitor"

namespace OHOS {
namespace Rosen {
constexpr int32_t MAX_ALPHA = 255;
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

bool RSUniHwcVisitor::CheckNodeOcclusion(const std::shared_ptr<RSRenderNode>& node,
    const RectI& nodeAbsRect, Color& nodeBgColor)
{
    nodeBgColor = RgbPalette::Transparent();
    if (node == nullptr) {
        return false;
    }
    if (node->IsPureContainer() && !node->ShouldPaint()) {
        return false;
    }
    const auto& canvasProperties = node->GetRenderProperties();
    auto canvasAbsRect = RectI();
    if (auto geo = canvasProperties.GetBoundsGeometry()) {
        canvasAbsRect = geo->GetAbsRect();
    }
    // The canvas node intersects with the surface node.
    if (!canvasAbsRect.IsEmpty() && !nodeAbsRect.IsEmpty() && canvasAbsRect.Intersect(nodeAbsRect)) {
        if (node->GetType() != RSRenderNodeType::CANVAS_NODE) {
            RS_LOGD("solidLayer: node type isn't canvas node, id:%{public}" PRIu64 " ", node->GetId());
            return true;
        }
        bool isSolid =
            canvasProperties.GetAlpha() == 1 && canvasProperties.GetBackgroundColor().GetAlpha() == MAX_ALPHA;
        bool noDrawing = node->GetDrawCmdModifiers().size() == 0;
        bool isTransparent = canvasProperties.GetBackgroundColor().GetAlpha() == 0;
        if (nodeAbsRect.IsInsideOf(canvasAbsRect) && isSolid && noDrawing) {
            nodeBgColor = canvasProperties.GetBackgroundColor();
            RS_LOGD("solidLayer: canvas node color, id:%{public}" PRIu64 ", color:%{public}08x",
                node->GetId(), nodeBgColor.AsArgbInt());
            return false;
        } else if (isTransparent) {
            return false;
        } else {
            RS_LOGD("solidLayer: check node, id:%{public}" PRIu64 ", isSolid:%{public}d, noDrawing:%{public}d, "
                    "isInsideOf:%{public}d", node->GetId(), isSolid, noDrawing,
                    nodeAbsRect.IsInsideOf(canvasAbsRect));
            return true;
        }
    }
    return false;
}

bool RSUniHwcVisitor::CheckBranchOcclusion(const std::shared_ptr<RSRenderNode>& branchNode,
    const RectI& nodeAbsRect, std::stack<Color>& nodeBgColor)
{
    // Recursive operation, checking branches from top to bottpm, looking for background color
    if (branchNode == nullptr) {
        return false;
    }
    RS_LOGD("solidLayer: check branch, id:%{public}" PRIu64 "", branchNode->GetId());
    Color bgColor = RgbPalette::Transparent();
    if (CheckNodeOcclusion(branchNode, nodeAbsRect, bgColor)) {
        RS_LOGD("solidLayer: node is occlusion, id:%{public}" PRIu64 "", branchNode->GetId());
        while (!nodeBgColor.empty()) {
            nodeBgColor.pop();
        }
        return true;
    } else if (bgColor != RgbPalette::Transparent()) {
        nodeBgColor.push(bgColor);
    }
    for (const auto& child : *branchNode->GetSortedChildren()) {
        if (child != nullptr && CheckBranchOcclusion(child, nodeAbsRect, nodeBgColor)) {
            while (!nodeBgColor.empty()) {
                nodeBgColor.pop();
            }
            return true;
        }
    }
    return false;
}

const Color& RSUniHwcVisitor::FindAppBackgroundColor(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: FindAppBackgroundColor name:%s id:%" PRIu64 "",
        node.GetName().c_str(), node.GetId());
    Color backgroundColor = RgbPalette::Transparent();
    auto rootNode = uniRenderVisitor_.curSurfaceNode_->GetFirstChild();
    auto parent = node.GetParent().lock();
    auto childId = node.GetId();
    auto nodeAbsRect = RectI();
    if (auto geo = node.GetRenderProperties().GetBoundsGeometry()) {
        nodeAbsRect = geo->GetAbsRect();
    }
    // Check the node from bottom up to look for the background color.
    while (parent && rootNode && parent->GetId() != rootNode->GetId()) {
        auto sortedChildren = parent->GetSortedChildren();
        std::stack<Color> nodeBgColor;
        auto iter = std::find_if(
            (*sortedChildren).begin(), (*sortedChildren).end(), [&childId](const std::shared_ptr<RSRenderNode>& node) {
                return node != nullptr && node->GetId() == childId;
            });
        if (iter == (*sortedChildren).end()) {
            return RgbPalette::Transparent();
        }
        size_t index = std::distance((*sortedChildren).begin(), iter);
        // Check branches from near to far
        for (size_t i = 1; i <= index; ++i) {
            const auto& child = sortedChildren->at(index - i);
            if (child == nullptr) {
                continue;
            }
            if (CheckBranchOcclusion(child, nodeAbsRect, nodeBgColor)) {
                return RgbPalette::Transparent();
            } else if (!nodeBgColor.empty()) {
                backgroundColor = nodeBgColor.top();
                RS_LOGD("solidLayer: branch color, branch id:%{public}" PRIu64 ", color:%{public}08x",
                    node.GetId(), backgroundColor.AsArgbInt());
                return backgroundColor;
            }
        }
        Color bgColor = RgbPalette::Transparent();
        if (CheckNodeOcclusion(parent, nodeAbsRect, bgColor)) {
            return RgbPalette::Transparent();
        } else if (bgColor != RgbPalette::Transparent()) {
            backgroundColor = bgColor;
            return backgroundColor;
        }
        childId = parent->GetId();
        parent = parent->GetParent().lock();
    }
    return RgbPalette::Transparent();
}

bool RSUniHwcVisitor::GetSolidLayerEnabled()
{
    auto solidLayerFeatureParam = GraphicFeatureParamManager::GetInstance().GetFeatureParam(FEATURE_CONFIGS[HWC]);
    auto solidLayerFeature = std::static_pointer_cast<HWCParam>(solidLayerFeatureParam);
    bool isSolidLayerEnableByCfgSwitch = false;
    if (solidLayerFeature != nullptr) {
        isSolidLayerEnableByCfgSwitch = solidLayerFeature->IsSolidLayerEnable();
    }
    return (isSolidLayerEnableByCfgSwitch || RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag()) &&
        OHOS::Rosen::RSSystemParameters::GetSolidLayerHwcEnabled();
}

void RSUniHwcVisitor::SolidLayerDisabled(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: name:%s id:%" PRIu64 " solidlayer enabling condition is not met.",
        node.GetName().c_str(), node.GetId());
    RS_LOGD("solidLayer: solidlayer enabling condition is not met, name: %{public}s", node.GetName().c_str());
    const auto& renderProperties = node.GetRenderProperties();
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    Color appBackgroundColor = renderProperties.GetBackgroundColor();
    bool isSpecialNodeType = RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() ||
        node.IsHardwareEnableHint();
    // Non-xcom or pure black nodes are not processed and are allowed to pass.
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == MAX_ALPHA &&
        appBackgroundColor == RgbPalette::Black()) {
        RS_LOGD("solidLayer: background color is black: %{public}s, go hwc directly", node.GetName().c_str());
        return;
    }
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == MAX_ALPHA &&
        stagingSurfaceParams->GetSelfDrawingNodeType() != SelfDrawingNodeType::XCOM) {
        RS_LOGD("solidLayer: solid color surface node: %{public}s, go hwc directly", node.GetName().c_str());
        return;
    }
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) < MAX_ALPHA) {
        if (!isSpecialNodeType) {
            RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: name:%s id:%" PRIu64 " disabled by background color alpha < 1",
                node.GetName().c_str(), node.GetId());
            RS_LOGD("solidLayer: disabled by background color alpha < 1: %{public}s", node.GetName().c_str());
#ifdef HIPERF_TRACE_ENABLE
            RS_LOGW("hiperf_surface: name:%{public}s disabled by background color alpha < 1, "
                "surfaceRect: [%{public}d, %{public}d, %{public}d, %{public}d]->"
                "[%{public}d, %{public}d, %{public}d, %{public}d]", node.GetName().c_str(),
                node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
                node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom(),
                node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
                node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom());
#endif
            node.SetHardwareForcedDisabledState(true);
            Statistics().UpdateHwcDisabledReasonForDFX(
                node.GetId(), HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, node.GetName());
        }
    }
    uniRenderVisitor_.curSurfaceNode_->SetExistTransparentHardwareEnabledNode(true);
    node.SetNodeHasBackgroundColorAlpha(true);
}

void RSUniHwcVisitor::SolidLayerEnabled(RSSurfaceRenderNode& node)
{
    if (!GetSolidLayerEnabled()) {
        RS_LOGD("solidLayer: solidlayer enabling condition is met, but the switch is disabled! name: %{public}s",
            node.GetName().c_str());
        node.SetHardwareForcedDisabledState(true);
        return;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: name:%s id:%" PRIu64 " solidlayer enabling condition is met.",
        node.GetName().c_str(), node.GetId());
    RS_LOGD("solidLayer: solidlayer enabling condition is met, name: %{public}s", node.GetName().c_str());
    const auto& renderProperties = node.GetRenderProperties();
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    Color appBackgroundColor = renderProperties.GetBackgroundColor();
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == 0) {
        appBackgroundColor = FindAppBackgroundColor(node);
        RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: background color found upwards in a transparent situation, name:%s "
            "id:%" PRIu64 " color:%08x", node.GetName().c_str(), node.GetId(), appBackgroundColor.AsArgbInt());
        RS_LOGD("solidLayer: isPureTransparentEnabled color:%{public}08x", appBackgroundColor.AsArgbInt());
        if (appBackgroundColor == RgbPalette::Black()) {
            // The background is black and does not overlap other surface node.
            RS_LOGD("solidLayer: background color is black: %{public}s", node.GetName().c_str());
            return;
        }
    }
    // No background color available
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) < MAX_ALPHA) {
        RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: name:%s id:%" PRIu64 " disabled by background color not found",
            node.GetName().c_str(), node.GetId());
        RS_LOGD("solidLayer: disabled by background color not found: %{public}s ", node.GetName().c_str());
#ifdef HIPERF_TRACE_ENABLE
        RS_LOGW("hiperf_surface: name:%{public}s disabled by background color not found, "
            "surfaceRect: [%{public}d, %{public}d, %{public}d, %{public}d]->"
            "[%{public}d, %{public}d, %{public}d, %{public}d]", node.GetName().c_str(),
            node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
            node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom(),
            node.GetSrcRect().GetLeft(), node.GetSrcRect().GetRight(),
            node.GetSrcRect().GetTop(), node.GetSrcRect().GetBottom());
#endif
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_SOLID_BACKGROUND_ALPHA, node.GetName());
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: Set solid layer color, name:%s id:%" PRIu64 ", color:%08x",
        node.GetName().c_str(), node.GetId(), appBackgroundColor.AsArgbInt());
    RS_LOGD("solidLayer: Set solid layer color:%{public}08x", appBackgroundColor.AsArgbInt());
    stagingSurfaceParams->SetIsHwcEnabledBySolidLayer(true);
    stagingSurfaceParams->SetSolidLayerColor(appBackgroundColor);
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node)
{
    if (node.GetAncoForceDoDirect()) {
        return;
    }
    RS_LOGD("solidLayer: ==UpdateHwcNodeEnableByBackgroundAlpha==, name: %{public}s id: %{public}" PRIu64 "",
        node.GetName().c_str(), node.GetId());
    const auto& renderProperties = node.GetRenderProperties();
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    Color appBackgroundColor = renderProperties.GetBackgroundColor();
    // Enabling conditions: target type, overall a==1, pure color non-black/pure transparent,
    // no bullet screen
    bool isTargetNodeType = stagingSurfaceParams->GetSelfDrawingNodeType() == SelfDrawingNodeType::XCOM ||
                            (node.IsRosenWeb() && static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == 0);
    bool isTargetColor = (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == MAX_ALPHA &&
                          appBackgroundColor != RgbPalette::Black()) ||
                          static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == 0;
    bool isSpecialNodeType = RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() ||
        node.IsHardwareEnableHint();
    bool isSolidLayerEnabled =
        isTargetNodeType && isTargetColor && renderProperties.GetAlpha() == 1 && !isSpecialNodeType;
    RS_LOGD("solidLayer: SolidLayer enabling conditions, isTargetNodeType:%{public}d, isTargetColor:%{public}d, "
        "Alpha:%{public}d, !isSpecialNodeType:%{public}d", isTargetNodeType, isTargetColor,
        renderProperties.GetAlpha() == 1, !isSpecialNodeType);
    if (!isSolidLayerEnabled) {
        SolidLayerDisabled(node);
        return;
    }
    SolidLayerEnabled(node);
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
    hasRotate = hasRotate || RSUniHwcComputeUtil::HasNonZRotationTransform(totalMatrix);
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

        UpdateHwcNodeEnableByGlobalFilter(surfaceNode);
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
            RSUniHwcComputeUtil::UpdateHwcNodeProperty(hwcNodePtr);
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
    if (!uniRenderVisitor_.curDisplayNode_) {
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

void RSUniHwcVisitor::UpdateHwcNodeRectInSkippedSubTree(const RSRenderNode& rootNode)
{
    if (!uniRenderVisitor_.curSurfaceNode_ || RS_PROFILER_SHOULD_BLOCK_HWCNODE()) {
        return;
    }
    const auto& hwcNodes = uniRenderVisitor_.curSurfaceNode_->GetChildHardwareEnabledNodes();
    if (hwcNodes.empty()) {
        return;
    }
    for (auto hwcNode : hwcNodes) {
        auto hwcNodePtr = hwcNode.lock();
        if (!hwcNodePtr || !hwcNodePtr->IsOnTheTree() || hwcNodePtr->GetCalcRectInPrepare()) {
            continue;
        }
        if (!hwcNodePtr->GetRSSurfaceHandler() || !hwcNodePtr->GetRSSurfaceHandler()->GetBuffer()) {
            RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by no buffer in skippedSubTree, "
                "HasBuffer[%d]", hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId(),
                hwcNodePtr->GetRSSurfaceHandler() && hwcNodePtr->GetRSSurfaceHandler()->GetBuffer());
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                HwcDisabledReasons::DISABLED_BY_INVALID_PARAM, hwcNodePtr->GetName());
            continue;
        }
        const auto& property = hwcNodePtr->GetRenderProperties();
        auto geoPtr = property.GetBoundsGeometry();
        if (geoPtr == nullptr) {
            return;
        }
        auto originalMatrix = geoPtr->GetMatrix();
        auto matrix = Drawing::Matrix();
        auto parent = hwcNodePtr->GetCurCloneNodeParent().lock();
        if (parent == nullptr) {
            parent = hwcNodePtr->GetParent().lock();
        }
        if (!FindRootAndUpdateMatrix(parent, matrix, rootNode)) {
            continue;
        }
        if (parent) {
            const auto& parentGeoPtr = parent->GetRenderProperties().GetBoundsGeometry();
            if (parentGeoPtr) {
                matrix.PostConcat(parentGeoPtr->GetMatrix());
            }
        }
        RectI clipRect;
        UpdateHWCNodeClipRect(hwcNodePtr, clipRect, rootNode);
        auto surfaceHandler = hwcNodePtr->GetMutableRSSurfaceHandler();
        auto& properties = hwcNodePtr->GetMutableRenderProperties();
        auto offset = std::nullopt;
        properties.UpdateGeometryByParent(&matrix, offset);
        matrix.PreConcat(originalMatrix);
        Drawing::Rect bounds = Drawing::Rect(0, 0, properties.GetBoundsWidth(), properties.GetBoundsHeight());
        Drawing::Rect absRect;
        matrix.MapRect(absRect, bounds);
        RectI rect;
        rect.left_ = static_cast<int>(std::floor(absRect.GetLeft()));
        rect.top_ = static_cast<int>(std::floor(absRect.GetTop()));
        rect.width_ = static_cast<int>(std::ceil(absRect.GetRight() - rect.left_));
        rect.height_ = static_cast<int>(std::ceil(absRect.GetBottom() - rect.top_));
        if (hwcNodePtr->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
            auto firstLevelNode =
                RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(hwcNodePtr->GetFirstLevelNode());
            if (firstLevelNode) {
                hwcNodePtr->SetHwcGlobalPositionEnabled(firstLevelNode->GetGlobalPositionEnabled());
                hwcNodePtr->SetHwcCrossNode(firstLevelNode->IsFirstLevelCrossNode());
            }
        }
        UpdateDstRect(*hwcNodePtr, rect, clipRect);
        UpdateSrcRect(*hwcNodePtr, matrix);
        UpdateHwcNodeByTransform(*hwcNodePtr, matrix);
        UpdateHwcNodeEnableByBackgroundAlpha(*hwcNodePtr);
        UpdateHwcNodeEnableByBufferSize(*hwcNodePtr);
        hwcNodePtr->SetTotalMatrix(matrix);
        hwcNodePtr->SetOldDirtyInSurface(geoPtr->MapRect(hwcNodePtr->GetSelfDrawRect(), matrix));
    }
}

bool RSUniHwcVisitor::FindRootAndUpdateMatrix(std::shared_ptr<RSRenderNode>& parent, Drawing::Matrix& matrix,
    const RSRenderNode& rootNode)
{
    bool findInRoot = parent ? parent->GetId() == rootNode.GetId() : false;
    while (parent && parent->GetType() != RSRenderNodeType::DISPLAY_NODE) {
        if (auto opt = RSUniHwcComputeUtil::GetMatrix(parent)) {
            matrix.PostConcat(opt.value());
        } else {
            break;
        }
        auto cloneNodeParent = parent->GetCurCloneNodeParent().lock();
        parent = cloneNodeParent ? cloneNodeParent : parent->GetParent().lock();
        if (!parent) {
            break;
        }
        findInRoot = parent->GetId() == rootNode.GetId() ? true : findInRoot;
    }
    return findInRoot;
}

void RSUniHwcVisitor::UpdateHWCNodeClipRect(std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr, RectI& clipRect,
    const RSRenderNode& rootNode)
{
    // The value here represents an imaginary plane of infinite width and infinite height,
    // using values summarized empirically.
    constexpr float MAX_FLOAT = std::numeric_limits<float>::max() * 1e-30;
    Drawing::Rect childRectMapped(0.0f, 0.0f, MAX_FLOAT, MAX_FLOAT);
    RSRenderNode::SharedPtr hwcNodeParent = hwcNodePtr;
    while (hwcNodeParent && hwcNodeParent->GetType() != RSRenderNodeType::DISPLAY_NODE &&
           hwcNodeParent->GetId() != rootNode.GetId()) {
        if (hwcNodeParent->GetType() == RSRenderNodeType::CANVAS_NODE) {
            const auto& parentProperties = hwcNodeParent->GetRenderProperties();
            const auto& parentGeoPtr = parentProperties.GetBoundsGeometry();
            const auto matrix = parentGeoPtr->GetMatrix();
            matrix.MapRect(childRectMapped, childRectMapped);

            const auto ApplyClip = [&childRectMapped, &matrix](Drawing::Rect rect) {
                Drawing::Rect clipRect;
                matrix.MapRect(clipRect, rect);
                RSUniHwcComputeUtil::IntersectRect(childRectMapped, clipRect);
            };

            if (parentProperties.GetClipToBounds()) {
                auto selfDrawRectF = hwcNodeParent->GetSelfDrawRect();
                Drawing::Rect clipSelfDrawRect(selfDrawRectF.left_, selfDrawRectF.top_,
                    selfDrawRectF.GetRight(), selfDrawRectF.GetBottom());
                ApplyClip(clipSelfDrawRect);
            }
            if (parentProperties.GetClipToFrame()) {
                auto left = parentProperties.GetFrameOffsetX() * matrix.Get(Drawing::Matrix::SCALE_X);
                auto top = parentProperties.GetFrameOffsetY() * matrix.Get(Drawing::Matrix::SCALE_Y);
                Drawing::Rect clipFrameRect(
                    left, top, left + parentProperties.GetFrameWidth(), top + parentProperties.GetFrameHeight());
                ApplyClip(clipFrameRect);
            }
            if (parentProperties.GetClipToRRect()) {
                RectF rectF = parentProperties.GetClipRRect().rect_;
                Drawing::Rect clipRect(rectF.left_, rectF.top_, rectF.GetRight(), rectF.GetBottom());
                ApplyClip(clipRect);
            }
        }
        hwcNodeParent = hwcNodeParent->GetParent().lock();
    }
    Drawing::Matrix rootNodeAbsMatrix = rootNode.GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix();
    Drawing::Rect absClipRect;
    rootNodeAbsMatrix.MapRect(absClipRect, childRectMapped);
    Drawing::Rect prepareClipRect(uniRenderVisitor_.prepareClipRect_.left_,
                                  uniRenderVisitor_.prepareClipRect_.top_,
                                  uniRenderVisitor_.prepareClipRect_.GetRight(),
                                  uniRenderVisitor_.prepareClipRect_.GetBottom());
    RSUniHwcComputeUtil::IntersectRect(absClipRect, prepareClipRect);
    clipRect.left_ = static_cast<int>(std::floor(absClipRect.GetLeft()));
    clipRect.top_ = static_cast<int>(std::floor(absClipRect.GetTop()));
    clipRect.width_ = static_cast<int>(std::ceil(absClipRect.GetRight() - clipRect.left_));
    clipRect.height_ = static_cast<int>(std::ceil(absClipRect.GetBottom() - clipRect.top_));
}

void RSUniHwcVisitor::UpdatePrepareClip(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto& geoPtr = property.GetBoundsGeometry();
    // Dirty Region use abstract coordinate, property of node use relative coordinate
    // BoundsRect(if exists) is mapped to absRect_ of RSObjAbsGeometry.
    if (property.GetClipToBounds()) {
        uniRenderVisitor_.prepareClipRect_ =
            uniRenderVisitor_.prepareClipRect_.IntersectRect(geoPtr->GetAbsRect());
    }
    // FrameRect(if exists) is mapped to rect using abstract coordinate explicitly by calling MapAbsRect.
    if (property.GetClipToFrame()) {
        // MapAbsRect do not handle the translation of OffsetX and OffsetY
        RectF frameRect{
            property.GetFrameOffsetX() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_X),
            property.GetFrameOffsetY() * geoPtr->GetAbsMatrix().Get(Drawing::Matrix::SCALE_Y),
            property.GetFrameWidth(), property.GetFrameHeight()};
        uniRenderVisitor_.prepareClipRect_ =
            uniRenderVisitor_.prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(frameRect));
    }
    if (property.GetClipToRRect()) {
        RectF rect = property.GetClipRRect().rect_;
        uniRenderVisitor_.prepareClipRect_ =
            uniRenderVisitor_.prepareClipRect_.IntersectRect(geoPtr->MapAbsRect(rect));
    }
}

void RSUniHwcVisitor::UpdateTopSurfaceSrcRect(RSSurfaceRenderNode& node,
    const Drawing::Matrix& absMatrix, const RectI& absRect)
{
    auto canvas = std::make_unique<Rosen::Drawing::Canvas>(uniRenderVisitor_.screenInfo_.phyWidth,
                                                           uniRenderVisitor_.screenInfo_.phyHeight);
    canvas->ConcatMatrix(absMatrix);

    const auto& dstRect = node.GetDstRect();
    Drawing::RectI dst = { std::round(dstRect.GetLeft()), std::round(dstRect.GetTop()), std::round(dstRect.GetRight()),
                           std::round(dstRect.GetBottom()) };
    node.UpdateSrcRect(*canvas.get(), dst);
    if (node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetBuffer()) {
        RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);
    }
}
} // namespace Rosen
} // namespace OHOS