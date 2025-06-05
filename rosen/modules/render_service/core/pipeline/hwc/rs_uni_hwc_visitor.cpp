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
#include "hwc_param.h"
#include "rs_uni_hwc_visitor.h"

#include "feature/hwc/rs_uni_hwc_compute_util.h"
#include "feature/hdr/rs_hdr_util.h"
#include "pipeline/rs_canvas_render_node.h"

#include "common/rs_common_hook.h"
#include "common/rs_optional_trace.h"
#include "rs_profiler.h"
#include "feature_cfg/graphic_feature_param_manager.h"

#undef LOG_TAG
#define LOG_TAG "RSUniHwcVisitor"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAX_ALPHA = 255;
constexpr uint32_t API18 = 18;
constexpr uint32_t INVALID_API_COMPATIBLE_VERSION = 0;

bool GetSolidLayerEnabled()
{
    return (HWCParam::IsSolidLayerEnable() || RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag()) &&
        OHOS::Rosen::RSSystemParameters::GetSolidLayerHwcEnabled();
}
}

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
    auto surfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    ((surfaceParams != nullptr && surfaceParams->GetIsHwcEnabledBySolidLayer()) || apiCompatibleVersion >= API18) ?
        RSUniHwcComputeUtil::DealWithNodeGravity(node, totalMatrix) :
        RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, uniRenderVisitor_.screenInfo_);
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
    isOffscreen_ |= (property.IsColorBlendApplyTypeOffscreen() && !property.IsColorBlendModeNone()) ||
        RSUniHwcComputeUtil::IsDangerousBlendMode(property.GetColorBlendMode(), property.GetColorBlendApplyType());
    // The meaning of first prepared offscreen node is either its colorBlendApplyType is not FAST or
    // its colorBlendMode is NONE.
    // Node will classified as blendWithBackground if it is the first prepared offscreen node and
    // its colorBlendMode is neither NONE nor SRC_OVER.
    node.GetHwcRecorder().SetBlendWithBackground(!isCurrOffscreen && property.IsColorBlendModeValid());
    return isCurrOffscreen;
}

void RSUniHwcVisitor::UpdateForegroundColorValid(RSCanvasRenderNode& node)
{
    node.GetHwcRecorder().SetForegroundColorValid(RSUniHwcComputeUtil::IsForegroundColorStrategyValid(node));
}

bool RSUniHwcVisitor::CheckNodeOcclusion(const std::shared_ptr<RSRenderNode>& node,
    const RectI& surfaceNodeAbsRect, Color& validBgColor)
{
    validBgColor = RgbPalette::Transparent();
    if (node == nullptr) {
        return false;
    }
    if (node->IsPureContainer() && !node->ShouldPaint()) {
        return false;
    }
    const auto& nodeProperties = node->GetRenderProperties();
    auto absRect = nodeProperties.GetBoundsGeometry()->GetAbsRect();
    // The canvas node intersects with the surface node.
    if (!absRect.IsEmpty() && !surfaceNodeAbsRect.IsEmpty() && absRect.Intersect(surfaceNodeAbsRect)) {
        if (node->GetType() != RSRenderNodeType::CANVAS_NODE) {
            RS_LOGD("solidLayer: node type isn't canvas node, id:%{public}" PRIu64 " ", node->GetId());
            return true;
        }

        bool willNotDraw = (node->GetDrawCmdModifiers().size() == 0);
        RS_LOGD("solidLayer: id:%{public}" PRIu64 ", willNotDraw: %{public}d", node->GetId(), willNotDraw);
        if (!willNotDraw) {
            RS_LOGD("solidLayer: presence drawing, id:%{public}" PRIu64, node->GetId());
            return true;
        }
        const auto& nodeBgColor = nodeProperties.GetBackgroundColor();
        const auto& nodeBgColorAlpha = nodeBgColor.GetAlpha();
        bool isSolid = ROSEN_EQ(nodeProperties.GetAlpha(), 1.f) && (nodeBgColorAlpha == MAX_ALPHA);
        if (isSolid) {
            if (surfaceNodeAbsRect.IsInsideOf(absRect)) {
                validBgColor = nodeBgColor;
                RS_LOGD("solidLayer: canvas node color, id:%{public}" PRIu64 ", color:%{public}08x",
                    node->GetId(), validBgColor.AsArgbInt());
                return false;
            }
            RS_LOGD("solidLayer: the background color node doesn't cover the surface node, id:%{public}" PRIu64,
                node->GetId());
            return true;
        }
        return nodeBgColorAlpha != 0;
    }
    return false;
}

bool RSUniHwcVisitor::CheckSubTreeOcclusion(
    const std::shared_ptr<RSRenderNode>& branchNode, const RectI& surfaceNodeAbsRect, std::stack<Color>& validBgColors)
{
    // Recursive operation, checking branches from top to bottpm, looking for background color
    if (branchNode == nullptr) {
        return false;
    }
    RS_LOGD("solidLayer: check branch, id:%{public}" PRIu64, branchNode->GetId());
    Color bgColor = RgbPalette::Transparent();
    if (CheckNodeOcclusion(branchNode, surfaceNodeAbsRect, bgColor)) {
        RS_LOGD("solidLayer: node is occlusion, id:%{public}" PRIu64, branchNode->GetId());
        while (!validBgColors.empty()) {
            validBgColors.pop();
        }
        return true;
    } else if (bgColor != RgbPalette::Transparent()) {
        validBgColors.push(bgColor);
    }
    for (const auto& child : *branchNode->GetSortedChildren()) {
        if (child != nullptr && CheckSubTreeOcclusion(child, surfaceNodeAbsRect, validBgColors)) {
            while (!validBgColors.empty()) {
                validBgColors.pop();
            }
            return true;
        }
    }
    return false;
}

Color RSUniHwcVisitor::FindAppBackgroundColor(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME_FMT(
        "solidLayer:%s name:%s id:%" PRIu64, __func__, node.GetName().c_str(), node.GetId());

    auto rootNode = uniRenderVisitor_.curSurfaceNode_->GetFirstChild();
    auto parent = node.GetParent().lock();
    auto childId = node.GetId();
    auto surfaceNodeAbsRect = node.GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    // Check the node from bottom up to look for the background color.
    while (rootNode && parent && parent->GetId() != rootNode->GetId()) {
        auto sortedChildren = parent->GetSortedChildren();
        auto iter = std::find_if(
            (*sortedChildren).begin(), (*sortedChildren).end(), [&childId](const std::shared_ptr<RSRenderNode>& node) {
                return node != nullptr && node->GetId() == childId;
            });
        if (iter == (*sortedChildren).end()) {
            return RgbPalette::Transparent();
        }
        int32_t index = static_cast<int32_t>(std::distance((*sortedChildren).begin(), iter));
        // Check branches from near to far
        std::stack<Color> validBgColors;
        for (int32_t i = 1; i <= index; i++) {
            const auto& child = sortedChildren->at(index - i);
            if (child == nullptr) {
                continue;
            }
            if (CheckSubTreeOcclusion(child, surfaceNodeAbsRect, validBgColors)) {
                return RgbPalette::Transparent();
            } else if (!validBgColors.empty()) {
                Color backgroundColor = validBgColors.top();
                RS_LOGD("solidLayer:  branch color, branch id:%{public}" PRIu64 ", color:%{public}08x",
                    node.GetId(), backgroundColor.AsArgbInt());
                return backgroundColor;
            }
        }
        Color bgColor = RgbPalette::Transparent();
        if (CheckNodeOcclusion(parent, surfaceNodeAbsRect, bgColor)) {
            return RgbPalette::Transparent();
        } else if (bgColor != RgbPalette::Transparent()) {
            return bgColor;
        }
        childId = parent->GetId();
        parent = parent->GetParent().lock();
    }
    return RgbPalette::Transparent();
}

void RSUniHwcVisitor::ProcessSolidLayerDisabled(RSSurfaceRenderNode& node)
{
    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: name:%s id:%" PRIu64 " solidLayer enabling condition is not met.",
        node.GetName().c_str(), node.GetId());
    RS_LOGD("solidLayer: solidLayer enabling condition is not met, name: %{public}s", node.GetName().c_str());
    const auto& renderProperties = node.GetRenderProperties();
    Color appBackgroundColor = renderProperties.GetBackgroundColor();
    // Non-xcom or pure black nodes are not processed and are allowed to pass.
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == MAX_ALPHA &&
        appBackgroundColor == RgbPalette::Black()) {
        RS_LOGD("solidLayer: background color is black: %{public}s, go hwc directly", node.GetName().c_str());
        return;
    }
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == MAX_ALPHA &&
        stagingSurfaceParams->GetSelfDrawingNodeType() != SelfDrawingNodeType::XCOM) {
        RS_LOGD("solidLayer: solid color surface node: %{public}s, go hwc directly", node.GetName().c_str());
        return;
    }
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) < MAX_ALPHA) {
        bool isSpecialNodeType = RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() ||
            node.IsHardwareEnableHint();
        if (!isSpecialNodeType || node.IsRosenWeb()) {
            auto parentNode = node.GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "background color alpha < 1", node.GetName().c_str(), node.GetId(), parentNode ? parentNode->GetId() : 0);
            RS_LOGD("solidLayer: disabled by background color alpha < 1: %{public}s", node.GetName().c_str());
            PrintHiperfLog(&node, "background color alpha < 1");
            node.SetHardwareForcedDisabledState(true);
            Statistics().UpdateHwcDisabledReasonForDFX(
                node.GetId(), HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, node.GetName());
            return;
        }
        uniRenderVisitor_.curSurfaceNode_->SetExistTransparentHardwareEnabledNode(true);
        node.SetNodeHasBackgroundColorAlpha(true);
    } else {
        auto parentNode = node.GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by background "
            "solidColor && HDR", node.GetName().c_str(), node.GetId(), parentNode ? parentNode->GetId() : 0);
        RS_LOGD("solidLayer: disabled by background solidColor && HDR: %{public}s", node.GetName().c_str());
        PrintHiperfLog(&node, "background solidColor && HDR");
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_SOLID_BACKGROUND_ALPHA, node.GetName());
    }
}

void RSUniHwcVisitor::ProcessSolidLayerEnabled(RSSurfaceRenderNode& node)
{
    if (!GetSolidLayerEnabled()) {
        RS_LOGD("solidLayer: solidLayer enabling condition is met, but the switch is disabled! name: %{public}s",
            node.GetName().c_str());
        auto parentNode = node.GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by solidLayer "
            "enabling condition is met, but the switch is disabled!", node.GetName().c_str(), node.GetId(),
            parentNode ? parentNode->GetId() : 0);
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_SOLID_BACKGROUND_ALPHA, node.GetName());
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: name:%s id:%" PRIu64 " solidLayer enabling condition is met.",
        node.GetName().c_str(), node.GetId());
    RS_LOGD("solidLayer: solidLayer enabling condition is met, name: %{public}s", node.GetName().c_str());
    const auto& renderProperties = node.GetRenderProperties();
    Color appBackgroundColor = renderProperties.GetBackgroundColor();
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == 0) {
        appBackgroundColor = FindAppBackgroundColor(node);
        auto parentNode = node.GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("solidLayer: background color found upwards in a transparent situation, name:%s "
            "id:%" PRIu64 " parentId:%" PRIu64 " color:%08x", node.GetName().c_str(), node.GetId(),
            parentNode ? parentNode->GetId() : 0, appBackgroundColor.AsArgbInt());
        RS_LOGD("solidLayer: isPureTransparentEnabled color:%{public}08x", appBackgroundColor.AsArgbInt());
        if (appBackgroundColor == RgbPalette::Black()) {
            // The background is black and does not overlap other surface node.
            RS_LOGD("solidLayer: background color is black: %{public}s", node.GetName().c_str());
            return;
        }
    }
    // No background color available
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) < MAX_ALPHA) {
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by background "
            "color not found", node.GetName().c_str(), node.GetId(), parentNode ? parentNode->GetId() : 0);
        RS_LOGD("solidLayer: disabled by background color not found: %{public}s", node.GetName().c_str());
        PrintHiperfLog(&node, "background color not found");
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_SOLID_BACKGROUND_ALPHA, node.GetName());
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: Set solid layer color, name:%s id:%" PRIu64 ", color:%08x",
        node.GetName().c_str(), node.GetId(), appBackgroundColor.AsArgbInt());
    RS_LOGD("solidLayer: Set solid layer color:%{public}08x", appBackgroundColor.AsArgbInt());
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    if (!stagingSurfaceParams) {
        node.SetHardwareForcedDisabledState(true);
        return;
    }
    stagingSurfaceParams->SetIsHwcEnabledBySolidLayer(true);
    stagingSurfaceParams->SetSolidLayerColor(appBackgroundColor);
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node)
{
    if (node.GetAncoForceDoDirect()) {
        return;
    }
    RS_LOGD("solidLayer: %{public}s, name: %{public}s id: %{public}" PRIu64,
        __func__, node.GetName().c_str(), node.GetId());
    const auto& renderProperties = node.GetRenderProperties();
    Color appBackgroundColor = renderProperties.GetBackgroundColor();
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    // Enabling conditions: target type, overall a==1, pure color non-black/pure transparent,
    // no bullet screen
    bool isTargetNodeType = stagingSurfaceParams->GetSelfDrawingNodeType() == SelfDrawingNodeType::XCOM ||
        (node.IsRosenWeb() && static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == 0);
    bool isTargetColor = (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == MAX_ALPHA &&
                          appBackgroundColor != RgbPalette::Black()) ||
                          static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == 0;
    bool isSpecialNodeType = RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() ||
                             node.IsHardwareEnableHint();
    auto GetHwcNodeHdrEnabled = [this]() {
        const auto &selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        for (auto surfaceNode : selfDrawingNodes) {
            if (surfaceNode && RSHdrUtil::CheckIsHdrSurface(*surfaceNode) != HdrStatus::NO_HDR) {
                return true;
            }
        }
        return false;
    };
    bool isSolidLayerEnabled =
        isTargetNodeType && isTargetColor && renderProperties.GetAlpha() == 1 && !isSpecialNodeType;
    bool isHdrOn = false;
    if (isSolidLayerEnabled) {
        isHdrOn = uniRenderVisitor_.curDisplayNode_->GetHasUniRenderHdrSurface() || GetHwcNodeHdrEnabled();
        if (!isHdrOn) {
            ProcessSolidLayerEnabled(node);
            return;
        }
    }
    RS_LOGD("solidLayer: SolidLayer enabling conditions, isTargetNodeType:%{public}d, isTargetColor:%{public}d, "
        "Alpha:%{public}d, !isSpecialNodeType:%{public}d, !isHdrOn: %{public}d", isTargetNodeType, isTargetColor,
        renderProperties.GetAlpha() == 1, !isSpecialNodeType, !isHdrOn);
    ProcessSolidLayerDisabled(node);
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByBufferSize(RSSurfaceRenderNode& node)
{
    if (!node.IsRosenWeb() || node.IsHardwareForcedDisabled() || !node.GetRSSurfaceHandler()) {
        return;
    }
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto buffer = surfaceHandler->GetBuffer();
    auto consumer = surfaceHandler->GetConsumer();
    if (buffer == nullptr || consumer == nullptr) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    const auto boundsWidth = property.GetBoundsWidth();
    const auto boundsHeight = property.GetBoundsHeight();
    auto bufferWidth = buffer->GetSurfaceBufferWidth();
    auto bufferHeight = buffer->GetSurfaceBufferHeight();
    const auto consumerTransformType = node.GetFixRotationByUser() ?
        RSUniHwcComputeUtil::GetRotateTransformForRotationFixed(node, consumer) :
        RSUniHwcComputeUtil::GetConsumerTransform(node, buffer, consumer);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    if (ROSEN_LNE(bufferWidth, boundsWidth) || ROSEN_LNE(bufferHeight, boundsHeight)) {
        auto parentNode = node.GetParent().lock();
        RS_OPTIONAL_TRACE_FMT(
            "hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " bounds:[%f, %f] buffer:[%d, %d] "
            "disabled by rosenWeb buffer nonmatching", node.GetName().c_str(), node.GetId(),
            parentNode ? parentNode->GetId() : 0, boundsWidth, boundsHeight, bufferWidth, bufferHeight);
        PrintHiperfLog(&node, "buffer nonmatching");
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_BUFFER_NONMATCH, node.GetName());
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByRotate(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        return;
    }
    // [planning] degree only multiples of 90 now
    const auto& totalMatrix = hwcNode->GetTotalMatrix();
    float degree = RSUniHwcComputeUtil::GetFloatRotationDegreeFromMatrix(totalMatrix);
    bool hasRotate = !ROSEN_EQ(std::remainder(degree, 90.f), 0.f, EPSILON) ||
                     RSUniHwcComputeUtil::HasNonZRotationTransform(totalMatrix);
    if (hasRotate) {
        auto parentNode = hwcNode->GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
            "surfaceNode rotation:%f", hwcNode->GetName().c_str(), hwcNode->GetId(),
            parentNode ? parentNode->GetId() : 0, degree);
        PrintHiperfLog(hwcNode, "rotation");
        hwcNode->SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
            HwcDisabledReasons::DISABLED_BY_ROTATION, hwcNode->GetName());
        return;
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByAlpha(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        return;
    }
    if (const auto alpha = hwcNode->GetGlobalAlpha(); ROSEN_LNE(alpha, 1.0f)) {
        auto parentNode = hwcNode->GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
            "accumulated alpha:%f", hwcNode->GetName().c_str(), hwcNode->GetId(),
            parentNode ? parentNode->GetId() : 0, alpha);
        PrintHiperfLog(hwcNode, "accumulated alpha");
        hwcNode->SetHardwareForcedDisabledState(true);
        if (ROSEN_NE(alpha, 0.f)) {
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                HwcDisabledReasons::DISABLED_BY_ACCUMULATED_ALPHA, hwcNode->GetName());
        }
        return;
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnable()
{
    std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>> ancoNodes;
    int inputHwclayers = 3;
    auto& curMainAndLeashSurfaces = uniRenderVisitor_.curDisplayNode_->GetAllMainAndLeashSurfaces();
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this, &inputHwclayers, &ancoNodes](RSBaseRenderNode::SharedPtr& nodePtr) {
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
        inputHwclayers += hwcNodes.size();
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
            UpdateHwcNodeEnableByAlpha(hwcNodePtr);
            UpdateHwcNodeEnableByRotate(hwcNodePtr);
            if ((hwcNodePtr->GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)) != 0) {
                ancoNodes.insert(hwcNodePtr);
            }
        }
    });
    RS_LOGW("hiperf_surface_counter1 %{public}" PRIu64 " ", static_cast<uint64_t>(inputHwclayers));
    PrintHiperfCounterLog("counter1", static_cast<uint64_t>(inputHwclayers));
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
    std::vector<RectI> abovedBounds;
    // Top-Down
    std::for_each(curMainAndLeashSurfaces.begin(), curMainAndLeashSurfaces.end(),
        [this, &backgroundAlphaRect, &isHardwareEnableByBackgroundAlpha, &abovedBounds]
        (RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            RS_LOGE("%{public}s: surfaceNode is nullptr", __func__);
            return;
        }
        // use in temporary scheme to realize DSS
        auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
        UpdateHardwareStateByBoundNEDstRectInApps(hwcNodes, abovedBounds);
        if (!hwcNodes.empty() && RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() &&
            RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag()) {
            UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes, backgroundAlphaRect,
                isHardwareEnableByBackgroundAlpha);
        } else if (surfaceNode->ExistTransparentHardwareEnabledNode()) {
            UpdateTransparentHwcNodeEnable(hwcNodes);
        }
    });
    // Down-Top
    std::for_each(curMainAndLeashSurfaces.rbegin(), curMainAndLeashSurfaces.rend(),
        [this, &hwcRects](RSBaseRenderNode::SharedPtr& nodePtr) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodePtr);
        if (!surfaceNode) {
            RS_LOGE("%{public}s: surfaceNode is nullptr", __func__);
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
                RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by transparent hwc node"
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
        RS_LOGE("%{public}s: curDisplayNode is null", __func__);
        return;
    }
    bool isForceCloseHdr = uniRenderVisitor_.curDisplayNode_->GetForceCloseHdr();
    if (hwcNode->IsHardwareForcedDisabled()) {
        if (!isForceCloseHdr && RSHdrUtil::CheckIsHdrSurface(*hwcNode) != HdrStatus::NO_HDR) {
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
                if (hwcNode->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
                    continue;
                }
                auto parentNode = hwcNode->GetParent().lock();
                RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                    "corner radius + hwc node below, rect:%s", hwcNode->GetName().c_str(), hwcNode->GetId(),
                    parentNode ? parentNode->GetId() : 0, rect.ToString().c_str());
                PrintHiperfLog(hwcNode, "corner radius + hwc node below");
                hwcNode->SetHardwareForcedDisabledState(true);
                if (!isForceCloseHdr && RSHdrUtil::CheckIsHdrSurface(*hwcNode) != HdrStatus::NO_HDR) {
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

// called by windows from Top to Down
void RSUniHwcVisitor::UpdateHardwareStateByBoundNEDstRectInApps(
    const std::vector<std::weak_ptr<RSSurfaceRenderNode>>& hwcNodes, std::vector<RectI>& abovedBounds)
{
    // Traverse hwcNodes in a app from Top to Down.
    for (auto reverseIter = hwcNodes.rbegin(); reverseIter != hwcNodes.rend(); ++reverseIter) {
        auto hwcNodePtr = reverseIter->lock();
        if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled()) {
            continue;
        }

        RectI boundRect = hwcNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
        RectI dstRect = hwcNodePtr->GetDstRect();
        if (!abovedBounds.empty()) {
            bool intersectWithAbovedRect = std::any_of(abovedBounds.begin(), abovedBounds.end(),
                [&boundRect](const RectI& abovedBound) { return !abovedBound.IntersectRect(boundRect).IsEmpty(); });
            if (intersectWithAbovedRect) {
                hwcNodePtr->SetHardwareForcedDisabledState(true);
                RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by aboved BoundNEDstRect hwcNode",
                    hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId());
                continue;
            }
        }

        // Anco nodes do not collect
        if ((hwcNodePtr->GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)) != 0) {
            continue;
        }

        // Check if the hwcNode's DstRect is inside of BoundRect, and not equal each other.
        if (dstRect.IsInsideOf(boundRect) && dstRect != boundRect) {
            abovedBounds.emplace_back(boundRect);
        }
    }
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
            auto parentNode = hwcNodePtr->GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "cannot cover above transparent hwc node", hwcNodePtr->GetName().c_str(),
                hwcNodePtr->GetId(), parentNode ? parentNode->GetId() : 0);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                HwcDisabledReasons::DISABLED_BY_HWC_NODE_ABOVE, hwcNodePtr->GetName());
            continue;
        }

        if (!hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled()) {
            hwcRects.push_back(hwcNodePtr->GetDstRect());
        } else if (hwcNodePtr->IsNodeHasBackgroundColorAlpha() && !hwcNodePtr->IsHardwareForcedDisabled() &&
                   hwcRects.size() != 0 && (hwcRects.back().IsInsideOf(hwcNodePtr->GetDstRect()) ||
                                            hwcNodePtr->GetDstRect().IsInsideOf(hwcRects.back()))) {
            isHardwareEnableByBackgroundAlpha = true;
            backgroundAlphaRect = hwcNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
            continue;
        } else {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            auto parentNode = hwcNodePtr->GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "hwc node backgound alpha", hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId(),
                parentNode ? parentNode->GetId() : 0);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, hwcNodePtr->GetName());
        }
    }
}

void RSUniHwcVisitor::CalcHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
    RSRenderNode& filterNode, int32_t filterZOrder)
{
    if (!node) {
        return;
    }
    auto bound = node->GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    bool isIntersect = !bound.IntersectRect(filterNode.GetOldDirtyInSurface()).IsEmpty();
    if (isIntersect) {
        auto parentNode = node->GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
            "filter rect, filterId:%" PRIu64, node->GetName().c_str(), node->GetId(),
            parentNode ? parentNode->GetId() : 0, filterNode.GetId());
        PrintHiperfLog(node, "filter rect");
        node->SetHardwareForcedDisabledState(true);
        node->HwcSurfaceRecorder().SetIntersectWithPreviousFilter(true);
        Statistics().UpdateHwcDisabledReasonForDFX(node->GetId(),
            HwcDisabledReasons::DISABLED_BY_FLITER_RECT, node->GetName());
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
    RSRenderNode& filterNode, int32_t filterZOrder)
{
    if (filterNode.GetOldDirtyInSurface().IsEmpty()) {
        return;
    }
    if (!node) {
        const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        if (selfDrawingNodes.empty()) {
            return;
        }
        for (auto hwcNode : selfDrawingNodes) {
            CalcHwcNodeEnableByFilterRect(hwcNode, filterNode, filterZOrder);
        }
    } else {
        const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            CalcHwcNodeEnableByFilterRect(hwcNodePtr, filterNode, filterZOrder);
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
                UpdateHwcNodeEnableByGlobalDirtyFilter(dirtyFilter->second, *hwcNodePtr);
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
                ROSEN_LOGD("UpdateHwcNodeByFilter: rendernode is null");
                continue;
            }

            if (rendernode->IsAIBarFilter()) {
                intersectedWithAIBar = true;
                if (rendernode->IsAIBarFilterCacheValid()) {
                    ROSEN_LOGD("UpdateHwcNodeByFilter: skip intersection for using cache");
                    continue;
                } else if (RSSystemProperties::GetHveFilterEnabled()) {
                    checkDrawAIBar = true;
                    continue;
                }
            }
            auto parentNode = hwcNode.GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "transparentCleanFilter, filterId:%" PRIu64, hwcNode.GetName().c_str(), hwcNode.GetId(),
                parentNode ? parentNode->GetId() : 0, filter->first);
            PrintHiperfLog(&hwcNode, "transparentCleanFilter");
            hwcNode.SetHardwareForcedDisabledState(true);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNode.GetId(),
                HwcDisabledReasons::DISABLED_BY_TRANSPARENT_CLEAN_FLITER, hwcNode.GetName());
            break;
        }
    }
    RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64", checkDrawAIBar:%d, intersectedWithAIBar:%d",
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
            auto parentNode = hwcNode.GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "transparentDirtyFilter, filterId:%" PRIu64, hwcNode.GetName().c_str(), hwcNode.GetId(),
                parentNode ? parentNode->GetId() : 0, filter->first);
            PrintHiperfLog(&hwcNode, "transparentDirtyFilter");
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
            auto parentNode = hwcNodePtr->GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "no buffer in skippedSubTree, HasBuffer[%d]", hwcNodePtr->GetName().c_str(),
                hwcNodePtr->GetId(), parentNode ? parentNode->GetId() : 0,
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
        if (!FindRootAndUpdateMatrix(parent, matrix, rootNode)) {
            continue;
        }
        auto parent = hwcNodePtr->GetCurCloneNodeParent().lock();
        if (parent == nullptr) {
            parent = hwcNodePtr->GetParent().lock();
        }
        if (parent) {
            const auto& parentGeoPtr = parent->GetRenderProperties().GetBoundsGeometry();
            if (parentGeoPtr) {
                matrix.PostConcat(parentGeoPtr->GetMatrix());
            }
        }
        RectI clipRect;
        UpdateHwcNodeClipRect(hwcNodePtr, rootNode, clipRect);
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
        UpdateCrossInfoForProtectedHwcNode(*hwcNodePtr);
        UpdateDstRect(*hwcNodePtr, rect, clipRect);
        UpdateHwcNodeInfo(*hwcNodePtr, matrix, rect, true);
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

void RSUniHwcVisitor::UpdateHwcNodeClipRect(const std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr,
    const RSRenderNode& rootNode, RectI& clipRect)
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

bool RSUniHwcVisitor::IsDisableHwcOnExpandScreen() const
{
    if (uniRenderVisitor_.curDisplayNode_ == nullptr) {
        RS_LOGE("curDisplayNode is null");
        return false;
    }

    // screenId > 0 means non-primary screen normally.
    return HWCParam::IsDisableHwcOnExpandScreen() &&
        uniRenderVisitor_.curDisplayNode_->GetScreenId() > 0;
}

void RSUniHwcVisitor::UpdateHwcNodeInfo(RSSurfaceRenderNode& node,
    const Drawing::Matrix& absMatrix, const RectI& absRect, bool subTreeSkipped)
{
    node.SetHardwareForcedDisabledState(node.HwcSurfaceRecorder().IsIntersectWithPreviousFilter());
    if (node.HwcSurfaceRecorder().IsIntersectWithPreviousFilter()) {
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by node intersect with previous filter",
            node.GetName().c_str(), node.GetId());
    }
    node.HwcSurfaceRecorder().SetIntersectWithPreviousFilter(false);
    node.SetInFixedRotation(uniRenderVisitor_.displayNodeRotationChanged_ ||
                            uniRenderVisitor_.isScreenRotationAnimating_);
    if (!uniRenderVisitor_.IsHardwareComposerEnabled() || !node.IsDynamicHardwareEnable() ||
        IsDisableHwcOnExpandScreen() || uniRenderVisitor_.curSurfaceNode_->GetVisibleRegion().IsEmpty() ||
        !node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer()) {
        auto parentNode = node.GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
            "param/invisible/no buffer, IsHardwareComposerEnabled[%d], IsDynamicHardwareEnable[%d], "
            "IsDisableHwcOnExpandScreen[%d], IsVisibleRegionEmpty[%d], HasBuffer[%d]", node.GetName().c_str(),
            node.GetId(), parentNode ? parentNode->GetId() : 0,
            uniRenderVisitor_.IsHardwareComposerEnabled(), node.IsDynamicHardwareEnable(),
            IsDisableHwcOnExpandScreen(), uniRenderVisitor_.curSurfaceNode_->GetVisibleRegion().IsEmpty(),
            node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetBuffer());
        PrintHiperfLog(&node, "param/invisible/no buffer");
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(node.GetId(),
            HwcDisabledReasons::DISABLED_BY_INVALID_PARAM, node.GetName());
        if (!node.GetFixRotationByUser() && !subTreeSkipped) {
            return;
        }
    }
    const uint32_t apiCompatibleVersion = node.GetApiCompatibleVersion();
    apiCompatibleVersion >= API18 ?
        UpdateSrcRect(node, absMatrix) :
        UpdateTopSurfaceSrcRect(node, absMatrix, absRect);
    UpdateHwcNodeEnableByBackgroundAlpha(node);
    UpdateHwcNodeByTransform(node, absMatrix);
    UpdateHwcNodeEnableByBufferSize(node);
}

void RSUniHwcVisitor::QuickPrepareChildrenOnlyOrder(RSRenderNode& node)
{
    const auto orderFunc = [this](const std::shared_ptr<RSRenderNode>& child) {
        if (!child) {
            return;
        }
        child->GetHwcRecorder().SetZOrderForHwcEnableByFilter(curZOrderForHwcEnableByFilter_++);
        QuickPrepareChildrenOnlyOrder(*child);
    };
 
    auto children = node.GetSortedChildren();
    if (uniRenderVisitor_.NeedPrepareChindrenInReverseOrder(node)) {
        std::for_each((*children).rbegin(), (*children).rend(), orderFunc);
    } else {
        std::for_each((*children).begin(), (*children).end(), orderFunc);
    }
}

void RSUniHwcVisitor::UpdateCrossInfoForProtectedHwcNode(RSSurfaceRenderNode& hwcNode)
{
    if (hwcNode.GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
        auto firstLevelNode =
            RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(hwcNode.GetFirstLevelNode());
        if (firstLevelNode) {
            hwcNode.SetHwcGlobalPositionEnabled(firstLevelNode->GetGlobalPositionEnabled());
            hwcNode.SetHwcCrossNode(firstLevelNode->IsFirstLevelCrossNode());
        }
    }
}

void RSUniHwcVisitor::PrintHiperfCounterLog(const char* const counterContext, uint64_t counter)
{
#ifdef HIPERF_TRACE_ENABLE
    RS_LOGW("hiperf_surface_%{public}s %{public}" PRIu64, counterContext, counter);
#endif
}

void RSUniHwcVisitor::PrintHiperfLog(RSSurfaceRenderNode* node, const char* const disabledContext)
{
#ifdef HIPERF_TRACE_ENABLE
    RS_LOGW("hiperf_surface: name:%{public}s disabled by %{public}s "
        "surfaceRect:[%{public}d, %{public}d, %{public}d, %{public}d]->"
        "[%{public}d, %{public}d, %{public}d, %{public}d]",
        node->GetName().c_str(), disabledContext,
        node->GetSrcRect().GetLeft(), node->GetSrcRect().GetRight(),
        node->GetSrcRect().GetTop(), node->GetSrcRect().GetBottom(),
        node->GetDstRect().GetLeft(), node->GetDstRect().GetRight(),
        node->GetDstRect().GetTop(), node->GetDstRect().GetBottom());
#endif
}

void RSUniHwcVisitor::PrintHiperfLog(const std::shared_ptr<RSSurfaceRenderNode>& node, const char* const disabledContext)
{
    PrintHiperfLog(node.get(), disabledContext);
}

} // namespace Rosen
} // namespace OHOS