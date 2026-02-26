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
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_surface_render_node_utils.h"

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
constexpr size_t MAX_NUM_SOLID_LAYER = 2;
constexpr int MIN_OVERLAP = 2;
constexpr float EPSILON_SCALE = 0.00001f;

// IsSolidLayerEnable : effective in all scenarios
// GetIsWhiteListForSolidColorLayerFlag : applicable to a single app
// IsSolidLayerInMultiWindowEnable : effective in floating window and multi-window scenarios
// GetSolidLayerHwcEnabled : Scheme Enable Switch
bool GetSolidLayerEnabled()
{
    return (HWCParam::IsSolidLayerEnable() || RsCommonHook::Instance().GetIsWhiteListForSolidColorLayerFlag() ||
            HWCParam::IsSolidLayerInMultiWindowEnable()) && RSSystemParameters::GetSolidLayerHwcEnabled();
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
        node.SetSrcRect({0, 0, uniRenderVisitor_.curScreenNode_->GetScreenInfo().width,
            uniRenderVisitor_.curScreenNode_->GetScreenInfo().height});
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
    if (!node.IsHardwareEnabledTopSurface() && !node.GetHwcGlobalPositionEnabled()) {
        // If the screen is expanded, intersect the destination rectangle with the screen rectangle
        dstRect = dstRect.IntersectRect(RectI(0, 0, uniRenderVisitor_.curScreenNode_->GetScreenInfo().width,
            uniRenderVisitor_.curScreenNode_->GetScreenInfo().height));
        // global positon has been transformd to screen position in absRect
    }
    // If the node is a hardware-enabled type, intersect its destination rectangle with the prepare clip rectangle
    if ((node.IsHardwareEnabledType() || node.IsHardwareEnabledTopSurface()) && !node.IsHwcCrossNode()) {
        dstRect = dstRect.IntersectRect(clipRect);
    }
    if (node.IsHardwareEnabledTopSurface()) {
        auto widthRatio = uniRenderVisitor_.curScreenNode_->GetScreenInfo().GetRogWidthRatio();
        auto heightRatio = uniRenderVisitor_.curScreenNode_->GetScreenInfo().GetRogHeightRatio();
        dstRect.left_ = static_cast<int32_t>(std::floor(dstRect.left_ * widthRatio));
        dstRect.top_ = static_cast<int32_t>(std::floor(dstRect.top_ * heightRatio));
        dstRect.width_ = static_cast<int32_t>(std::ceil(dstRect.width_ * widthRatio));
        dstRect.height_ = static_cast<int32_t>(std::ceil(dstRect.height_ * heightRatio));
    }
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
    ((surfaceParams != nullptr && surfaceParams->GetIsHwcEnabledBySolidLayer()) || apiCompatibleVersion >= API18 ||
        node.GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED) ||
        node.GetName().find("RenderFitSurface") != std::string::npos ||
        node.GetFrameGravityNewVersionEnabled()) ?
        RSUniHwcComputeUtil::DealWithNodeGravity(node, totalMatrix) :
        RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(node, uniRenderVisitor_.curScreenNode_->GetScreenInfo());
    RSUniHwcComputeUtil::DealWithScalingMode(node, totalMatrix);
    RSUniHwcComputeUtil::LayerRotate(node, uniRenderVisitor_.curScreenNode_->GetScreenInfo());
    RSUniHwcComputeUtil::LayerCrop(node, uniRenderVisitor_.curScreenNode_->GetScreenInfo());
    RSUniHwcComputeUtil::CalcSrcRectByBufferFlip(node, uniRenderVisitor_.curScreenNode_->GetScreenInfo());
    node.SetCalcRectInPrepare(true);
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
    if (!absRect.IsEmpty() && !surfaceNodeAbsRect.IsEmpty() && !absRect.IntersectRect(surfaceNodeAbsRect).IsEmpty()) {
        if (node->GetType() != RSRenderNodeType::CANVAS_NODE) {
            RS_LOGD("solidLayer: node type isn't canvas node, id:%{public}" PRIu64, node->GetId());
            return true;
        }

        bool willNotDraw = node->IsPureBackgroundColor() && !node->HasDrawCmdModifiers();
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
    // Recursive operation, checking branches from top to bottom, looking for background color
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
                RS_LOGD("solidLayer: branch color, branch id:%{public}" PRIu64 ", color:%{public}08x",
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
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) == MAX_ALPHA &&
        node.GetSelfDrawingNodeType() != SelfDrawingNodeType::XCOM) {
        RS_LOGD("solidLayer: solid color surface node: %{public}s, go hwc directly", node.GetName().c_str());
        return;
    }
    if (static_cast<uint8_t>(appBackgroundColor.GetAlpha()) < MAX_ALPHA) {
        bool isSpecialNodeType = RsCommonHook::Instance().GetHardwareEnabledByBackgroundAlphaFlag() ||
            node.IsHardwareEnableHint();
        if (!isSpecialNodeType || node.IsRosenWeb()) {
            auto parentNode = node.GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "background color alpha < 1", node.GetName().c_str(), node.GetId(),
                parentNode ? parentNode->GetId() : 0);
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
    if (!GetSolidLayerEnabled() || (GetSolidLayerHwcEnableCount() >= MAX_NUM_SOLID_LAYER)) {
        RS_LOGD("solidLayer: solidLayer enabling condition is met, but the switch is not turned on or exceeds the "
            "upper limit! solidLayer num: %{public}zu, name: %{public}s", GetSolidLayerHwcEnableCount(),
            node.GetName().c_str());
        auto parentNode = node.GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: disabled by solidLayer enabling condition is met, but the switch is not "
            "turned on or exceeds the upper limit! solidLayer num: %zu, parentId:%" PRIu64 ", name: %s",
            GetSolidLayerHwcEnableCount(), parentNode ? parentNode->GetId() : 0, node.GetName().c_str());
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, node.GetName());
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
        auto parentNode = node.GetParent().lock();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by background "
            "color not found", node.GetName().c_str(), node.GetId(), parentNode ? parentNode->GetId() : 0);
        RS_LOGD("solidLayer: disabled by background color not found: %{public}s", node.GetName().c_str());
        PrintHiperfLog(&node, "background color not found");
        node.SetHardwareForcedDisabledState(true);
        Statistics().UpdateHwcDisabledReasonForDFX(
            node.GetId(), HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, node.GetName());
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("solidLayer: Set solid layer color, name:%s id:%" PRIu64 ", color:%08x",
        node.GetName().c_str(), node.GetId(), appBackgroundColor.AsArgbInt());
    RS_LOGD("solidLayer: Set solid layer color:%{public}08x", appBackgroundColor.AsArgbInt());
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    stagingSurfaceParams->SetIsHwcEnabledBySolidLayer(true);
    stagingSurfaceParams->SetSolidLayerColor(appBackgroundColor);
    IncreaseSolidLayerHwcEnableCount();
}

bool RSUniHwcVisitor::IsTargetSolidLayer(RSSurfaceRenderNode& node)
{
    const std::string& bundleName = node.GetBundleName();
    return (RsCommonHook::Instance().IsSolidColorLayerConfig(bundleName) ||
            RsCommonHook::Instance().IsHwcSolidColorLayerConfig(bundleName)) &&
           RSMainThread::Instance()->GetSystemAnimatedScenes() != SystemAnimatedScenes::DRAG_WINDOW;
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByBackgroundAlpha(RSSurfaceRenderNode& node)
{
    auto stagingSurfaceParams = static_cast<RSSurfaceRenderParams *>(node.GetStagingRenderParams().get());
    stagingSurfaceParams->SetIsHwcEnabledBySolidLayer(false);
    if (node.GetAncoForceDoDirect()) {
        return;
    }
    RS_LOGD("solidLayer: %{public}s, name: %{public}s id: %{public}" PRIu64,
        __func__, node.GetName().c_str(), node.GetId());
    const auto& renderProperties = node.GetRenderProperties();
    Color appBackgroundColor = renderProperties.GetBackgroundColor();
    // Enabling conditions: target type, overall a==1, pure color non-black/pure transparent,
    // no bullet screen
    bool isTargetNodeType = node.GetSelfDrawingNodeType() == SelfDrawingNodeType::XCOM ||
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
    bool isTargetAppBundle = IsTargetSolidLayer(node);
    bool isSolidLayerEnabled =
        isTargetNodeType && isTargetColor && renderProperties.GetAlpha() == 1 &&
        !isSpecialNodeType && isTargetAppBundle;
    bool isHdrOn = false;
    bool hasBrightness = false;
    if (isSolidLayerEnabled) {
        isHdrOn = uniRenderVisitor_.curScreenNode_->GetHasUniRenderHdrSurface() || GetHwcNodeHdrEnabled();
        hasBrightness = renderProperties.IsBgBrightnessValid();
        if (!isHdrOn && !hasBrightness) {
            ProcessSolidLayerEnabled(node);
            return;
        }
    }
    RS_LOGD("solidLayer: SolidLayer enabling conditions, isTargetNodeType: %{public}d, isTargetColor: %{public}d, "
        "Alpha: %{public}d, isTargetAppBundle: %{public}d, !isSpecialNodeType: %{public}d, !isHdrOn: %{public}d, "
        "!hasBrightness: %{public}d",
        isTargetNodeType, isTargetColor, renderProperties.GetAlpha() == 1,
        !isSpecialNodeType, isTargetAppBundle, !isHdrOn, !hasBrightness);
    ProcessSolidLayerDisabled(node);
}

bool RSUniHwcVisitor::IsScaleSceneHwcEnabled(RSSurfaceRenderNode& node)
{
    return HWCParam::IsDisableHwcInScaleScene() && IsTargetSolidLayer(node);
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByBufferSize(RSSurfaceRenderNode& node)
{
    if (!(node.IsRosenWeb() || IsScaleSceneHwcEnabled(node)) ||
        node.IsHardwareForcedDisabled() || !node.GetRSSurfaceHandler()) {
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
            "hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " buffer:[%d, %d] bounds:[%f, %f] "
            "disabled by buffer nonmatching", node.GetName().c_str(), node.GetId(),
            parentNode ? parentNode->GetId() : 0, bufferWidth, bufferHeight, boundsWidth, boundsHeight);
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
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by rotation:%f",
            hwcNode->GetName().c_str(), hwcNode->GetId(), parentNode ? parentNode->GetId() : 0, degree);
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
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by accumulated alpha:%f",
            hwcNode->GetName().c_str(), hwcNode->GetId(), parentNode ? parentNode->GetId() : 0, alpha);
        PrintHiperfLog(hwcNode, "accumulated alpha");
        hwcNode->SetHardwareForcedDisabledState(true);
        if (ROSEN_NE(alpha, 0.f)) {
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                HwcDisabledReasons::DISABLED_BY_ACCUMULATED_ALPHA, hwcNode->GetName());
        }
    }
}

void RSUniHwcVisitor::UpdateHwcNodeEnable()
{
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> ancoNodes;
    int inputHwclayers = 3;
    auto& curMainAndLeashSurfaces = uniRenderVisitor_.curScreenNode_->GetAllMainAndLeashSurfaces();
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
            UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcNodePtr, hwcRects);
            if ((hwcNodePtr->GetAncoFlags() & static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE)) != 0) {
                ancoNodes.push_back(hwcNodePtr);
            }
        }
    });
    PrintHiperfCounterLog("counter1", static_cast<uint64_t>(inputHwclayers));

    // NEW: Apply ColorPicker disable
    UpdateHwcNodeEnableByColorPicker();

    uniRenderVisitor_.PrevalidateHwcNode();
    UpdateHwcNodeEnableByNodeBelow();
    uniRenderVisitor_.UpdateAncoNodeHWCDisabledState(ancoNodes);
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByNodeBelow()
{
    auto& curMainAndLeashSurfaces = uniRenderVisitor_.curScreenNode_->GetAllMainAndLeashSurfaces();
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
                Statistics().UpdateHwcDisabledReasonForDFX(transparentHwcNodeSPtr->GetId(),
                    HwcDisabledReasons::DISABLED_BY_TRANSPARENT_NODE, transparentHwcNodeSPtr->GetName());
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
    if (!uniRenderVisitor_.curScreenNode_) {
        RS_LOGE("%{public}s: curScreenNode is null", __func__);
        return;
    }
    if (hwcNode->IsHardwareForcedDisabled()) {
        if (RSHdrUtil::CheckIsHdrSurface(*hwcNode) != HdrStatus::NO_HDR) {
            uniRenderVisitor_.curScreenNode_->SetHasUniRenderHdrSurface(true);
        }
        return;
    }
    auto absBound = hwcNode->GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    if (!isIntersectWithRoundCorner) {
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
                RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by corner "
                    "radius + hwc node below, rect:%s", hwcNode->GetName().c_str(), hwcNode->GetId(),
                    parentNode ? parentNode->GetId() : 0, rect.ToString().c_str());
                PrintHiperfLog(hwcNode, "corner radius + hwc node below");
                hwcNode->SetHardwareForcedDisabledState(true);
                if (RSHdrUtil::CheckIsHdrSurface(*hwcNode) != HdrStatus::NO_HDR) {
                    uniRenderVisitor_.curScreenNode_->SetHasUniRenderHdrSurface(true);
                }
                Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                    HwcDisabledReasons::DISABLED_BY_HWC_NODE_ABOVE, hwcNode->GetName());
                return;
            }
        }
    }
    hwcRects.emplace_back(absBound);
}

void RSUniHwcVisitor::UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode,
    std::vector<RectI>& hwcRects)
{
    if (hwcNode->IsHardwareForcedDisabled()) {
        return;
    }
    auto dst = hwcNode->GetDstRect();
    if (hwcNode->GetAncoForceDoDirect()) {
        hwcRects.emplace_back(dst);
        return;
    }
    if (RsCommonHook::Instance().GetOverlappedHwcNodeInAppEnabledConfig(hwcNode->GetBundleName()) == "1" &&
        hwcNode->GetName().find("overlapped_surface") != std::string::npos) {
        hwcRects.emplace_back(dst);
        return;
    }
    for (const auto& rect : hwcRects) {
        const bool isIntersect = !dst.IntersectRect(rect).IsEmpty();
        if (isIntersect && !RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag() &&
            !hwcNode->IsHardwareEnableHint()) {
            if (RsCommonHook::Instance().GetVideoSurfaceFlag() &&
                ((dst.GetBottom() - rect.GetTop() <= MIN_OVERLAP && dst.GetBottom() - rect.GetTop() >= 0) ||
                 (rect.GetBottom() - dst.GetTop() <= MIN_OVERLAP && rect.GetBottom() - dst.GetTop() >= 0))) {
                return;
            }
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by hwc node below self in app",
                hwcNode->GetName().c_str(), hwcNode->GetId());
            PrintHiperfLog(hwcNode.get(), "hwc node below self in app");
            hwcNode->SetHardwareForcedDisabledState(true);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNode->GetId(),
                HwcDisabledReasons::DISABLED_BY_HWC_NODE_BELOW_SELF_IN_APP, hwcNode->GetName());
            return;
        }
    }
    hwcRects.emplace_back(dst);
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
                Statistics().UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                    HwcDisabledReasons::DISABLED_BY_ABOVED_BOUND_NE_DST_RECT, hwcNodePtr->GetName());
                continue;
            }
        }

        // Anco nodes do not collect
        if (hwcNodePtr->GetAncoForceDoDirect()) {
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
            hwcNodePtr->GetRenderProperties().GetBoundsGeometry()->GetAbsRect()).IsEmpty();
        if (isHardwareEnableByBackgroundAlpha && !hwcNodePtr->IsHardwareForcedDisabled() && isIntersect) {
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            auto parentNode = hwcNodePtr->GetParent().lock();
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by "
                "cannot cover above transparent hwc node", hwcNodePtr->GetName().c_str(),
                hwcNodePtr->GetId(), parentNode ? parentNode->GetId() : 0);
            Statistics().UpdateHwcDisabledReasonForDFX(hwcNodePtr->GetId(),
                HwcDisabledReasons::DISABLED_BY_BACKGROUND_ALPHA, hwcNodePtr->GetName());
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
        } else if (!hwcNodePtr->IsHardwareForcedDisabled()) {
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

bool RSUniHwcVisitor::IsBackgroundFilterUnderSurface(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode,
    const std::shared_ptr<RSRenderNode>& filterNode)
{
    auto buildRSRenderNodePath =
        [this](const std::shared_ptr<RSRenderNode>& startNode) -> std::stack<std::shared_ptr<RSRenderNode>> {
        std::stack<std::shared_ptr<RSRenderNode>> nodeStack;
        auto currentNode = startNode;
        while (currentNode && currentNode != uniRenderVisitor_.curSurfaceNode_) {
            nodeStack.push(currentNode);
            currentNode = currentNode->GetParent().lock();
        }
        if (currentNode != nullptr) {
            nodeStack.push(currentNode);
        }
        return nodeStack;
    };
    std::stack<std::shared_ptr<RSRenderNode>> surfaceNodeStack = buildRSRenderNodePath(hwcNode);
    std::stack<std::shared_ptr<RSRenderNode>> filterNodeStack = buildRSRenderNodePath(filterNode);
    if (surfaceNodeStack.top() != filterNodeStack.top()) {
        return false;
    }

    std::shared_ptr<RSRenderNode> publicParentNode = nullptr;
    while (surfaceNodeStack.size() > 1 && filterNodeStack.size() > 1 &&
        surfaceNodeStack.top() == filterNodeStack.top()) {
        publicParentNode = surfaceNodeStack.top();
        surfaceNodeStack.pop();
        filterNodeStack.pop();
    }
    if (!publicParentNode) {
        return false;
    }

    auto surfaceParent = surfaceNodeStack.top();
    auto filterParent = filterNodeStack.top();
    if (surfaceParent == filterParent) {
        return (surfaceParent != hwcNode && filterParent == filterNode);
    } else {
        uint32_t surfaceZOrder = surfaceParent->GetHwcRecorder().GetZOrderForHwcEnableByFilter();
        uint32_t filterZOrder = filterParent->GetHwcRecorder().GetZOrderForHwcEnableByFilter();
        return publicParentNode->GetCurFrameInfoDetail().curFrameReverseChildren ?
            (filterZOrder > surfaceZOrder) : (filterZOrder < surfaceZOrder);
    }
}

void RSUniHwcVisitor::CalcHwcNodeEnableByFilterRect(std::shared_ptr<RSSurfaceRenderNode>& node,
    RSRenderNode& filterNode, uint32_t filterZOrder)
{
    if (!node) {
        return;
    }
    if (node->GetInstanceRootNodeId() == filterNode.GetInstanceRootNodeId() &&
        RSUniHwcComputeUtil::IsBlendNeedBackground(filterNode) && uniRenderVisitor_.curSurfaceNode_ &&
        uniRenderVisitor_.curSurfaceNode_->GetId() == node->GetInstanceRootNodeId() &&
        RsCommonHook::Instance().GetFilterUnderHwcConfigByApp(node->GetBundleName()) == "1") {
        if (IsBackgroundFilterUnderSurface(node, filterNode.shared_from_this())) {
            return;
        }
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
    RSRenderNode& filterNode, uint32_t filterZOrder)
{
    if (filterNode.GetOldDirtyInSurface().IsEmpty()) {
        return;
    }
    if (!node) {
        auto screenNodeId = uniRenderVisitor_.curScreenNode_->GetId();
        const auto& selfDrawingNodes = RSMainThread::Instance()->GetSelfDrawingNodes();
        for (auto hwcNode : selfDrawingNodes) {
            if (!hwcNode || hwcNode->GetScreenNodeId() != screenNodeId) {
                continue;
            }
            CalcHwcNodeEnableByFilterRect(hwcNode, filterNode, filterZOrder);
        }
    } else {
        const auto& hwcNodes = node->GetChildHardwareEnabledNodes();
        if (hwcNodes.empty()) {
            return;
        }
        for (auto hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr) {
                continue;
            }
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
    auto& curMainAndLeashSurfaces = uniRenderVisitor_.curScreenNode_->GetAllMainAndLeashSurfaces();
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
        for (auto& hwcNode : hwcNodes) {
            auto hwcNodePtr = hwcNode.lock();
            if (!hwcNodePtr || hwcNodePtr->IsHardwareForcedDisabled()) {
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
        if (!geo->GetAbsRect().IntersectRect(filter->second).IsEmpty()) {
            auto& renderNode = nodeMap.GetRenderNode<RSRenderNode>(filter->first);
            if (renderNode == nullptr) {
                ROSEN_LOGD("UpdateHwcNodeByFilter: renderNode is null");
                continue;
            }

            if (renderNode->IsAIBarFilter()) {
                // curScreenNode_ is guaranteed not nullptr in UpdateHwcNodeEnable
                auto screenId = uniRenderVisitor_.curScreenNode_->GetScreenId();
                RSMainThread::Instance()->GetMutableAIBarNodes()[screenId].insert(renderNode);
                intersectedWithAIBar = true;
                bool intersectHwcDamage = RSSystemProperties::GetAIBarOptEnabled() ?
                    RSSurfaceRenderNodeUtils::IntersectHwcDamageWith(hwcNode, filter->second) : true;
                if (renderNode->CheckAndUpdateAIBarCacheStatus(intersectHwcDamage)) {
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

namespace {
void ColorPickerCheckHwcIntersection(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode,
    const RectI& colorPickerRect)
{
    if (!hwcNode || !hwcNode->IsOnTheTree() || hwcNode->IsHardwareForcedDisabled()) {
        return;
    }

    RectI hwcNodeRect = hwcNode->GetRenderProperties().GetBoundsGeometry()->GetAbsRect();
    if (!colorPickerRect.IntersectRect(hwcNodeRect).IsEmpty()) {
        hwcNode->SetHardwareForcedDisabledState(true);
        RS_OPTIONAL_TRACE_FMT("ColorPicker: rect [%d,%d,%d,%d] intersects node %s id:%" PRIu64
            " (rect: [%d,%d,%d,%d]) - disabling",
            colorPickerRect.left_, colorPickerRect.top_, colorPickerRect.width_, colorPickerRect.height_,
            hwcNode->GetName().c_str(), hwcNode->GetId(), hwcNodeRect.left_, hwcNodeRect.top_,
            hwcNodeRect.width_, hwcNodeRect.height_);
    }
}
} // namespace

void RSUniHwcVisitor::UpdateHwcNodeEnableByColorPicker()
{
    for (const auto& [_, colorPickerRect] : colorPickerHwcDisabledSurfaces_) {
        auto& curMainAndLeashSurfaces = uniRenderVisitor_.curScreenNode_->GetAllMainAndLeashSurfaces();
        for (auto& surfaceNodePtr : curMainAndLeashSurfaces) {
            auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(surfaceNodePtr);
            if (!surfaceNode) {
                continue;
            }

            const auto& hwcNodes = surfaceNode->GetChildHardwareEnabledNodes();
            for (const auto& hwcNodeWeak : hwcNodes) {
                auto hwcNode = hwcNodeWeak.lock();
                ColorPickerCheckHwcIntersection(hwcNode, colorPickerRect);
            }
        }
    }
}

bool RSUniHwcVisitor::IsDisableHwcOnExpandScreen() const
{
    if (uniRenderVisitor_.curScreenNode_ == nullptr) {
        RS_LOGE("curScreenNode is null");
        return false;
    }

    // virtual expand screen
    if (uniRenderVisitor_.curScreenNode_->GetCompositeType() == CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        return true;
    }

    // screenId equals 0 or 5 means primary screen normally
    if (uniRenderVisitor_.curScreenNode_->GetScreenId() != 0 && uniRenderVisitor_.curScreenNode_->GetScreenId() != 5) {
        return true;
    }

    // screenId > 0 means non-primary screen normally
    if (HWCParam::IsDisableHwcOnExpandScreen() && uniRenderVisitor_.curScreenNode_->GetScreenId() > 0) {
        return true;
    }

    return false;
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
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " parentId:%" PRIu64 " disabled by no "
                "buffer in skippedSubTree, HasBuffer[%d]", hwcNodePtr->GetName().c_str(), hwcNodePtr->GetId(),
                parentNode ? parentNode->GetId() : 0,
                hwcNodePtr->GetRSSurfaceHandler() && hwcNodePtr->GetRSSurfaceHandler()->GetBuffer());
            hwcNodePtr->SetHardwareForcedDisabledState(true);
            continue;
        }
        auto renderNode = hwcNodePtr->ReinterpretCastTo<RSRenderNode>();
        if (!IsFindRootSuccess(renderNode, rootNode)) {
            continue;
        }
        RectI clipRect;
        Drawing::Matrix matrix = rootNode.GetRenderProperties().GetBoundsGeometry()->GetAbsMatrix();
        UpdateHwcNodeClipRectAndMatrix(hwcNodePtr, rootNode, clipRect, matrix);
        auto surfaceHandler = hwcNodePtr->GetMutableRSSurfaceHandler();
        auto& properties = hwcNodePtr->GetMutableRenderProperties();
        auto offset = std::nullopt;
        properties.UpdateGeometryByParent(&matrix, offset);
        const auto& geoPtr = properties.GetBoundsGeometry();
        const auto& originalMatrix = geoPtr->GetMatrix();
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
        UpdateHwcNodeInfo(*hwcNodePtr, matrix, true);
        hwcNodePtr->SetTotalMatrix(matrix);
        hwcNodePtr->SetOldDirtyInSurface(geoPtr->MapRect(hwcNodePtr->GetSelfDrawRect(), matrix));
    }
}

bool RSUniHwcVisitor::IsFindRootSuccess(std::shared_ptr<RSRenderNode>& parent, const RSRenderNode& rootNode)
{
    bool isFindInRootSubTree = false;
    do {
        auto cloneNodeParent = parent->GetCurCloneNodeParent().lock();
        parent = cloneNodeParent ? cloneNodeParent : parent->GetParent().lock();
        if (!parent) {
            break;
        }
        isFindInRootSubTree |= parent->GetId() == rootNode.GetId();
    } while (!isFindInRootSubTree && parent != uniRenderVisitor_.curSurfaceNode_);
    return isFindInRootSubTree;
}

void RSUniHwcVisitor::UpdateHwcNodeClipRect(const std::shared_ptr<RSRenderNode>& hwcNodeParent,
    Drawing::Rect& childRectMapped)
{
    if (hwcNodeParent->GetType() != RSRenderNodeType::CANVAS_NODE) {
        return;
    }

    const auto& parentProperties = hwcNodeParent->GetRenderProperties();
    const auto& parentGeoPtr = parentProperties.GetBoundsGeometry();
    const auto& matrix = parentGeoPtr->GetMatrix();
    matrix.MapRect(childRectMapped, childRectMapped);

    const auto ApplyClip = [&childRectMapped, &matrix](const Drawing::Rect& rect) {
        Drawing::Rect clipRect;
        matrix.MapRect(clipRect, rect);
        RSUniHwcComputeUtil::IntersectRect(childRectMapped, clipRect);
    };

    if (parentProperties.GetClipToBounds()) {
        const auto& selfDrawRectF = hwcNodeParent->GetSelfDrawRect();
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
        const RectF& rectF = parentProperties.GetClipRRect().rect_;
        Drawing::Rect clipRect(rectF.left_, rectF.top_, rectF.GetRight(), rectF.GetBottom());
        ApplyClip(clipRect);
    }
}

void RSUniHwcVisitor::UpdateHwcNodeMatrix(const std::shared_ptr<RSRenderNode>& hwcNodeParent,
    Drawing::Matrix& accumulatedMatrix)
{
    if (auto opt = RSUniHwcComputeUtil::GetMatrix(hwcNodeParent)) {
        accumulatedMatrix.PostConcat(opt.value());
    }
}

void RSUniHwcVisitor::UpdateHwcNodeClipRectAndMatrix(const std::shared_ptr<RSSurfaceRenderNode>& hwcNodePtr,
    const RSRenderNode& rootNode, RectI& clipRect, Drawing::Matrix& matrix)
{
    // The value here represents an imaginary plane of infinite width and infinite height,
    // using values summarized empirically.
    constexpr float MAX_FLOAT = std::numeric_limits<float>::max() * 1e-30;
    Drawing::Rect childRectMapped(0.0f, 0.0f, MAX_FLOAT, MAX_FLOAT);
    Drawing::Matrix accumulatedMatrix;
    RSRenderNode::SharedPtr hwcNodeParent = hwcNodePtr;
    while (hwcNodeParent && hwcNodeParent->GetType() != RSRenderNodeType::SCREEN_NODE &&
           hwcNodeParent->GetId() != rootNode.GetId()) {
        UpdateHwcNodeClipRect(hwcNodeParent, childRectMapped);
        if (hwcNodePtr->GetId() != hwcNodeParent->GetId()) {
            UpdateHwcNodeMatrix(hwcNodeParent, accumulatedMatrix);
        }
        auto cloneNodeParent = hwcNodeParent->GetCurCloneNodeParent().lock();
        hwcNodeParent = cloneNodeParent ? cloneNodeParent : hwcNodeParent->GetParent().lock();
    }
    Drawing::Rect absClipRect;
    matrix.MapRect(absClipRect, childRectMapped);
    Drawing::Rect prepareClipRect(uniRenderVisitor_.prepareClipRect_.left_,
                                  uniRenderVisitor_.prepareClipRect_.top_,
                                  uniRenderVisitor_.prepareClipRect_.GetRight(),
                                  uniRenderVisitor_.prepareClipRect_.GetBottom());
    RSUniHwcComputeUtil::IntersectRect(absClipRect, prepareClipRect);
    clipRect.left_ = static_cast<int>(std::floor(absClipRect.GetLeft()));
    clipRect.top_ = static_cast<int>(std::floor(absClipRect.GetTop()));
    clipRect.width_ = static_cast<int>(std::ceil(absClipRect.GetRight() - clipRect.left_));
    clipRect.height_ = static_cast<int>(std::ceil(absClipRect.GetBottom() - clipRect.top_));
    matrix.PreConcat(accumulatedMatrix);
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
    auto canvas = std::make_unique<Rosen::Drawing::Canvas>(uniRenderVisitor_.curScreenNode_->GetScreenInfo().phyWidth,
                                                           uniRenderVisitor_.curScreenNode_->GetScreenInfo().phyHeight);
    canvas->ConcatMatrix(absMatrix);

    const auto& dstRect = node.GetDstRect();
    Drawing::RectI dst = { dstRect.GetLeft(), dstRect.GetTop(), dstRect.GetRight(), dstRect.GetBottom() };
    node.UpdateSrcRect(*canvas.get(), dst);
    if (node.GetRSSurfaceHandler() && node.GetRSSurfaceHandler()->GetBuffer()) {
        RSUniHwcComputeUtil::UpdateRealSrcRect(node, absRect);
    }
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

void RSUniHwcVisitor::PrintHiperfLog(const std::shared_ptr<RSSurfaceRenderNode>& node,
    const char* const disabledContext)
{
    PrintHiperfLog(node.get(), disabledContext);
}

void RSUniHwcVisitor::UpdateHwcNodeInfo(RSSurfaceRenderNode& node,
    const Drawing::Matrix& absMatrix, bool subTreeSkipped)
{
    if (node.IsHardwareForcedDisabled() != node.HwcSurfaceRecorder().IsIntersectWithPreviousFilter()) {
        node.SetHardwareForcedDisabledState(node.HwcSurfaceRecorder().IsIntersectWithPreviousFilter());
        if (node.HwcSurfaceRecorder().IsIntersectWithPreviousFilter()) {
            RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by node intersect with previous filter",
                node.GetName().c_str(), node.GetId());
            Statistics().UpdateHwcDisabledReasonForDFX(node.GetId(),
                HwcDisabledReasons::DISABLED_BY_PREVIOUS_FLITER_RECT, node.GetName());
        }
    }
    node.HwcSurfaceRecorder().SetIntersectWithPreviousFilter(false);
    node.SetInFixedRotation(uniRenderVisitor_.displayNodeRotationChanged_ ||
                            uniRenderVisitor_.isScreenRotationAnimating_);
    bool isHardwareForcedDisabled = !node.GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED) &&
        (!uniRenderVisitor_.IsHardwareComposerEnabled() || !node.IsDynamicHardwareEnable() ||
         IsDisableHwcOnExpandScreen() || uniRenderVisitor_.curSurfaceNode_->GetVisibleRegion().IsEmpty() ||
         !node.GetRSSurfaceHandler() || !node.GetRSSurfaceHandler()->GetBuffer());
    if (isHardwareForcedDisabled) {
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
    UpdateSrcRect(node, absMatrix);
    UpdateHwcNodeEnableByBackgroundAlpha(node);
    UpdateHwcNodeByTransform(node, absMatrix);
    // dstRect transform to global position after UpdateHwcNodeByTransform
    UpdateDstRectByGlobalPosition(node);
    UpdateHwcNodeEnableByBufferSize(node);
}

void RSUniHwcVisitor::UpdateDstRectByGlobalPosition(RSSurfaceRenderNode& node)
{
    // dstRect transform to globalposition when node is move
    if (node.GetHwcGlobalPositionEnabled()) {
        auto dstRect = node.GetDstRect();
        const auto& screenProperty = uniRenderVisitor_.curScreenNode_->GetScreenProperty();
        dstRect.left_ += screenProperty.GetOffsetX();
        dstRect.top_ += screenProperty.GetOffsetY();
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " dstRect:[%s]",
            node.GetName().c_str(), node.GetId(), dstRect.ToString().c_str());
        node.SetDstRect(dstRect);
    }
}
} // namespace Rosen
} // namespace OHOS