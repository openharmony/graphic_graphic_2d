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
    if (node.GetDRMGlobalPositionEnabled()) {
        dstRect.left_ -= uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetX();
        dstRect.top_ -= uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetY();
    }
    if (!node.IsHardwareEnabledTopSurface() && !node.GetDRMGlobalPositionEnabled()) {
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
    auto offsetX = node.GetDRMGlobalPositionEnabled() ? uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetX() : 0;
    auto offsetY = node.GetDRMGlobalPositionEnabled() ? uniRenderVisitor_.curDisplayNode_->GetDisplayOffsetY() : 0;
    dstRect.left_ = static_cast<int>(std::round(dstRect.left_ *
        uniRenderVisitor_.screenInfo_.GetRogWidthRatio()) + offsetX);
    dstRect.top_ = static_cast<int>(std::round(dstRect.top_ *
        uniRenderVisitor_.screenInfo_.GetRogHeightRatio()) + offsetY);
    dstRect.width_ = static_cast<int>(std::round(dstRect.width_ *
        uniRenderVisitor_.screenInfo_.GetRogWidthRatio()));
    dstRect.height_ = static_cast<int>(std::round(dstRect.height_ *
        uniRenderVisitor_.screenInfo_.GetRogHeightRatio()));

    if (uniRenderVisitor_.curSurfaceNode_ && (node.GetId() != uniRenderVisitor_.curSurfaceNode_->GetId()) &&
        !node.GetDRMGlobalPositionEnabled()) {
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

} // namespace Rosen
} // namespace OHOS