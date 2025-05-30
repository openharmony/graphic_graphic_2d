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
#include "rs_uni_hwc_compute_util.h"

#include "hwc/rs_hwc_recorder.h"
#include "modifier/rs_render_property.h"
#include "property/rs_properties_painter.h"

#include "common/rs_optional_trace.h"

#undef LOG_TAG
#define LOG_TAG "RSUniHwcComputeUtil"

namespace OHOS {
namespace Rosen {
// Only can be used by smart pointer and raw pointer
// If any nullptr is found, it will return immediately
#define CHECK_NULL_VOID(...)                                    \
    do {                                                        \
        if (RSUniHwcComputeUtil::IS_ANY_NULLPTR(__VA_ARGS__)) { \
            return;                                             \
        }                                                       \
    } while (0)

GraphicTransformType RSUniHwcComputeUtil::GetRotateTransformForRotationFixed(RSSurfaceRenderNode& node,
    sptr<IConsumerSurface> consumer)
{
    auto transformType = RSBaseRenderUtil::GetRotateTransform(RSBaseRenderUtil::GetSurfaceBufferTransformType(
        node.GetRSSurfaceHandler()->GetConsumer(), node.GetRSSurfaceHandler()->GetBuffer()));
    int extraRotation = 0;
    int degree = static_cast<int>(round(node.GetAbsRotation()));
    auto surfaceParams = node.GetStagingRenderParams() == nullptr
                             ? nullptr
                             : static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    int32_t rotationDegree = RSBaseRenderUtil::GetScreenRotationOffset(surfaceParams);
    extraRotation = (degree - rotationDegree) % ROUND_ANGLE;
    transformType = static_cast<GraphicTransformType>(
        (transformType + extraRotation / RS_ROTATION_90 + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    return transformType;
}

GraphicTransformType RSUniHwcComputeUtil::GetConsumerTransform(const RSSurfaceRenderNode& node,
    const sptr<SurfaceBuffer> buffer, const sptr<IConsumerSurface> consumer)
{
    auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer != nullptr && buffer != nullptr) {
        if (consumer->GetSurfaceBufferTransformType(buffer, &transformType) != GSERROR_OK) {
            RS_LOGE("GetConsumerTransform GetSurfaceBufferTransformType failed");
        }
    }
    GraphicTransformType consumerTransform = RSBaseRenderUtil::GetRotateTransform(transformType);
    return consumerTransform;
}

bool RSUniHwcComputeUtil::IsHwcEnabledByGravity(RSSurfaceRenderNode& node, const Gravity frameGravity)
{
    // When renderfit mode is not Gravity::RESIZE or Gravity::TOP_LEFT,
    // we currently disable hardware composer.
    if (frameGravity != Gravity::RESIZE && frameGravity != Gravity::TOP_LEFT) {
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by frameGravity[%d]",
            node.GetName().c_str(), node.GetId(), static_cast<int32_t>(frameGravity));
        node.SetHardwareForcedDisabledState(true);
        return false;
    }
    return true;
}

Drawing::Rect RSUniHwcComputeUtil::CalcSrcRectByBufferRotation(const SurfaceBuffer& buffer,
    const GraphicTransformType consumerTransformType, Drawing::Rect newSrcRect)
{
    const auto frameWidth = buffer.GetSurfaceBufferWidth();
    const auto frameHeight = buffer.GetSurfaceBufferHeight();
    auto left = newSrcRect.left_;
    auto top = newSrcRect.top_;
    auto width = newSrcRect.right_ - newSrcRect.left_;
    auto height = newSrcRect.bottom_ - newSrcRect.top_;
    switch (consumerTransformType) {
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            newSrcRect = Drawing::Rect(frameWidth - width - left, top, frameWidth - left, top + height);
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            newSrcRect = Drawing::Rect(frameWidth - width - left, frameHeight - height - top,
                frameWidth - left, frameHeight - top);
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            newSrcRect = Drawing::Rect(left, frameHeight - height - top, left + width, frameHeight - top);
            break;
        }
        default:
            break;
    }
    newSrcRect.left_ = std::clamp<int>(std::floor(newSrcRect.GetLeft()), 0, frameWidth);
    newSrcRect.top_ = std::clamp<int>(std::floor(newSrcRect.GetTop()), 0, frameHeight);
    newSrcRect.right_ = std::clamp<int>(std::ceil(newSrcRect.GetRight()), left, frameWidth);
    newSrcRect.bottom_ = std::clamp<int>(std::ceil(newSrcRect.GetBottom()), top, frameHeight);
    return newSrcRect;
}

void RSUniHwcComputeUtil::DealWithNodeGravity(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix)
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
    float bufferWidth = buffer->GetSurfaceBufferWidth();
    float bufferHeight = buffer->GetSurfaceBufferHeight();
    const float boundsWidth = node.GetRenderProperties().GetBoundsWidth();
    const float boundsHeight = node.GetRenderProperties().GetBoundsHeight();
    const Gravity frameGravity = node.GetRenderProperties().GetFrameGravity();
    const GraphicTransformType consumerTransformType = node.GetFixRotationByUser() ?
        GetRotateTransformForRotationFixed(node, consumer) : GetConsumerTransform(node, buffer, consumer);
    CheckForceHardwareAndUpdateDstRect(node);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(frameGravity,
        {0.f, 0.f, boundsWidth, boundsHeight}, bufferWidth, bufferHeight, gravityMatrix);
    Drawing::Matrix inverseTotalMatrix;
    Drawing::Matrix inverseGravityMatrix;
    // We don't have to do additional works when renderfit mode is Gravity::RESIZE or frameSize == boundsSize.
    if (frameGravity == Gravity::RESIZE ||
        (ROSEN_EQ(bufferWidth, boundsWidth) && ROSEN_EQ(bufferHeight, boundsHeight)) ||
        !totalMatrix.Invert(inverseTotalMatrix) || !gravityMatrix.Invert(inverseGravityMatrix)) {
        return;
    }
    Drawing::Rect bound = Drawing::Rect(0.f, 0.f, boundsWidth, boundsHeight);
    Drawing::Rect frame = Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight);
    Drawing::Rect localIntersectRect;
    gravityMatrix.MapRect(localIntersectRect, frame);
    IntersectRect(localIntersectRect, bound);
    Drawing::Rect absIntersectRect;
    totalMatrix.MapRect(absIntersectRect, localIntersectRect);
    const RectI dstRect = node.GetDstRect();
    Drawing::Rect newDstRect(dstRect.left_, dstRect.top_, dstRect.GetRight(), dstRect.GetBottom());
    IntersectRect(newDstRect, absIntersectRect);
    node.SetDstRect({std::floor(newDstRect.GetLeft()), std::floor(newDstRect.GetTop()),
        std::ceil(newDstRect.GetWidth()), std::ceil(newDstRect.GetHeight())});
    Drawing::Rect newSrcRect;
    DealWithNewSrcRect(newSrcRect, newDstRect, inverseTotalMatrix, inverseGravityMatrix, consumerTransformType);
    newSrcRect = CalcSrcRectByBufferRotation(*buffer, consumerTransformType, newSrcRect);
    node.SetSrcRect({newSrcRect.GetLeft(), newSrcRect.GetTop(), newSrcRect.GetWidth(), newSrcRect.GetHeight()});
}

void RSUniHwcComputeUtil::DealWithNewSrcRect(Drawing::Rect& newSrcRect, Drawing::Rect newDstRect,
    Drawing::Matrix inverseTotalMatrix, Drawing::Matrix inverseGravityMatrix,
    const GraphicTransformType consumerTransformType)
{
    inverseTotalMatrix.MapRect(newSrcRect, newDstRect);
    inverseGravityMatrix.MapRect(newSrcRect, newSrcRect);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(newSrcRect.left_, newSrcRect.top_);
        std::swap(newSrcRect.right_, newSrcRect.bottom_);
    }
}

void RSUniHwcComputeUtil::DealWithNodeGravityOldVersion(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    auto buffer = node.GetRSSurfaceHandler()->GetBuffer();
    if (!buffer) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    const float frameWidth = buffer->GetSurfaceBufferWidth();
    const float frameHeight = buffer->GetSurfaceBufferHeight();
    const float boundsWidth = property.GetBoundsWidth();
    const float boundsHeight = property.GetBoundsHeight();
    const Gravity frameGravity = property.GetFrameGravity();

    CheckForceHardwareAndUpdateDstRect(node);
    // we do not need to do additional works for Gravity::RESIZE and if frameSize == boundsSize.
    if (frameGravity == Gravity::RESIZE || (ROSEN_EQ(frameWidth, boundsWidth) && ROSEN_EQ(frameHeight, boundsHeight)) ||
        frameGravity == Gravity::TOP_LEFT) {
        return;
    }
 
    // get current node's translate matrix and calculate gravity matrix.
    auto translateMatrix = Drawing::Matrix();
    translateMatrix.Translate(node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X),
        std::ceil(node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y)));
    Drawing::Matrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(frameGravity,
        RectF {0.0f, 0.0f, boundsWidth, boundsHeight}, frameWidth, frameHeight, gravityMatrix);
    // create a canvas to calculate new dstRect and new srcRect
    int32_t screenWidth = screenInfo.phyWidth;
    int32_t screenHeight = screenInfo.phyHeight;
    const auto screenRotation = screenInfo.rotation;
    if (screenRotation == ScreenRotation::ROTATION_90 || screenRotation == ScreenRotation::ROTATION_270) {
        std::swap(screenWidth, screenHeight);
    }
 
    auto canvas = std::make_unique<Drawing::Canvas>(screenWidth, screenHeight);
    canvas->ConcatMatrix(translateMatrix);
    canvas->ConcatMatrix(gravityMatrix);
    Drawing::Rect clipRect;
    gravityMatrix.MapRect(clipRect, Drawing::Rect(0, 0, frameWidth, frameHeight));
    canvas->ClipRect(Drawing::Rect(0, 0, clipRect.GetWidth(), clipRect.GetHeight()), Drawing::ClipOp::INTERSECT);
    Drawing::RectI newDstRect = canvas->GetDeviceClipBounds();
    auto dstRect = node.GetDstRect();
    // we make the newDstRect as the intersection of new and old dstRect,
    // to deal with the situation that frameSize > boundsSize.
    newDstRect.Intersect(Drawing::RectI(
        dstRect.left_, dstRect.top_, dstRect.width_ + dstRect.left_, dstRect.height_ + dstRect.top_));
    auto localRect = canvas->GetLocalClipBounds();
    int left = std::clamp<int>(localRect.GetLeft(), 0, frameWidth);
    int top = std::clamp<int>(localRect.GetTop(), 0, frameHeight);
    int width = std::clamp<int>(localRect.GetWidth(), 0, frameWidth - left);
    int height = std::clamp<int>(localRect.GetHeight(), 0, frameHeight - top);
 
    node.SetDstRect({newDstRect.GetLeft(), newDstRect.GetTop(), newDstRect.GetWidth(), newDstRect.GetHeight()});
    node.SetSrcRect({left, top, width, height});
}

void RSUniHwcComputeUtil::CheckForceHardwareAndUpdateDstRect(RSSurfaceRenderNode& node)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    if (!surfaceHandler->GetConsumer() || !node.IsInFixedRotation() || !surfaceHandler->GetBuffer()) {
        return;
    }
    node.SetSrcRect(node.GetOriginalSrcRect());
    auto dstRect = node.GetDstRect();
    auto originalDstRect = node.GetOriginalDstRect();
    int fixedConstant = 2;
    dstRect.left_ += (dstRect.width_ - originalDstRect.width_) / fixedConstant;
    dstRect.top_ += (dstRect.height_ - originalDstRect.height_) / fixedConstant;
    dstRect.width_ = originalDstRect.width_;
    dstRect.height_ = originalDstRect.height_;
    node.SetDstRect(dstRect);
}

void RSUniHwcComputeUtil::DealWithScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix)
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
    float bufferWidth = buffer->GetSurfaceBufferWidth();
    float bufferHeight = buffer->GetSurfaceBufferHeight();
    const float boundsWidth = node.GetRenderProperties().GetBoundsWidth();
    const float boundsHeight = node.GetRenderProperties().GetBoundsHeight();
    const GraphicTransformType consumerTransformType = node.GetFixRotationByUser() ?
        GetRotateTransformForRotationFixed(node, consumer) : GetConsumerTransform(node, buffer, consumer);
    const Gravity frameGravity = node.GetRenderProperties().GetFrameGravity();
    const ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_TO_WINDOW || !IsHwcEnabledByScalingMode(node, scalingMode) ||
        !IsHwcEnabledByGravity(node, frameGravity)) {
        return;
    }
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(frameGravity,
        {0.f, 0.f, boundsWidth, boundsHeight}, bufferWidth, bufferHeight, gravityMatrix);
    Drawing::Matrix rawScalingModeMatrix;
    RSPropertiesPainter::GetScalingModeMatrix(scalingMode,
        {0.f, 0.f, boundsWidth, boundsHeight}, bufferWidth, bufferHeight, rawScalingModeMatrix);
    Drawing::Rect rawSrcRect;
    rawScalingModeMatrix.MapRect(rawSrcRect, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    float xScale = ROSEN_EQ(boundsWidth, 0.f) ? 1.0f :
        rawSrcRect.GetWidth() / (boundsWidth == 0.0f ? 1.0f : boundsWidth);
    float yScale = ROSEN_EQ(boundsHeight, 0.f) ? 1.0f :
        rawSrcRect.GetHeight() / (boundsHeight == 0.0f ? 1.0f : boundsHeight);
    Drawing::Matrix scalingModeMatrix;
    scalingModeMatrix.PostScale(xScale, yScale);
    Drawing::Rect adjustedSrcRect;
    scalingModeMatrix.MapRect(adjustedSrcRect, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    float xTranslate = (bufferWidth - adjustedSrcRect.GetWidth()) / 2.0f;
    float yTranslate = (bufferHeight - adjustedSrcRect.GetHeight()) / 2.0f;
    scalingModeMatrix.PostTranslate(xTranslate, yTranslate);
    UpdateHwcNodeByScalingMode(node, totalMatrix, gravityMatrix, scalingModeMatrix);
}

void RSUniHwcComputeUtil::LayerRotate(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    const auto screenWidth = static_cast<int32_t>(screenInfo.width);
    const auto screenHeight = static_cast<int32_t>(screenInfo.height);
    const auto screenRotation = screenInfo.rotation;
    const auto rect = node.GetDstRect();
    switch (screenRotation) {
        case ScreenRotation::ROTATION_90: {
            node.SetDstRect({rect.top_, screenHeight - rect.left_ - rect.width_, rect.height_, rect.width_});
            break;
        }
        case ScreenRotation::ROTATION_180: {
            node.SetDstRect({screenWidth - rect.left_ - rect.width_, screenHeight - rect.top_ - rect.height_,
                rect.width_, rect.height_});
            break;
        }
        case ScreenRotation::ROTATION_270: {
            node.SetDstRect({screenWidth - rect.top_ - rect.height_, rect.left_, rect.height_, rect.width_});
            break;
        }
        default:  {
            break;
        }
    }
}

void RSUniHwcComputeUtil::LayerCrop(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    if (node.GetHwcGlobalPositionEnabled()) {
        return;
    }
    auto dstRect = node.GetDstRect();
    auto srcRect = node.GetSrcRect();
    auto originSrcRect = srcRect;

    RectI dstRectI(dstRect.left_, dstRect.top_, dstRect.width_, dstRect.height_);
    int32_t screenWidth = static_cast<int32_t>(screenInfo.phyWidth);
    int32_t screenHeight = static_cast<int32_t>(screenInfo.phyHeight);
    RectI screenRectI(0, 0, screenWidth, screenHeight);
    RectI resDstRect = dstRectI.IntersectRect(screenRectI);
    if (resDstRect == dstRectI) {
        return;
    }
    if (node.IsInFixedRotation()) {
        node.SetDstRect(resDstRect);
        return;
    }
    dstRect = {resDstRect.left_, resDstRect.top_, resDstRect.width_, resDstRect.height_};
    srcRect.left_ = (resDstRect.IsEmpty() || dstRectI.IsEmpty()) ? 0 : std::ceil((resDstRect.left_ - dstRectI.left_) *
        originSrcRect.width_ / dstRectI.width_);
    srcRect.top_ = (resDstRect.IsEmpty() || dstRectI.IsEmpty()) ? 0 : std::ceil((resDstRect.top_ - dstRectI.top_) *
        originSrcRect.height_ / dstRectI.height_);
    srcRect.width_ = dstRectI.IsEmpty() ? 0 : originSrcRect.width_ * resDstRect.width_ / dstRectI.width_;
    srcRect.height_ = dstRectI.IsEmpty() ? 0 : originSrcRect.height_ * resDstRect.height_ / dstRectI.height_;
    node.SetDstRect(dstRect);
    node.SetSrcRect(srcRect);
}

void RSUniHwcComputeUtil::CalcSrcRectByBufferFlip(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    auto buffer = surfaceHandler->GetBuffer();
    if (!consumer || !buffer) {
        return;
    }
    const auto bufferWidth = buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = buffer->GetSurfaceBufferHeight();
    const auto dstRect = node.GetDstRect();
    RectI srcRect = node.GetSrcRect();
    auto bufferFlipTransformType =
        RSBaseRenderUtil::GetFlipTransform(RSBaseRenderUtil::GetSurfaceBufferTransformType(consumer, buffer));
    switch (bufferFlipTransformType) {
        case GraphicTransformType::GRAPHIC_FLIP_H: {
            if (srcRect.left_ >= 0) {
                srcRect.left_ = bufferWidth - srcRect.left_ - srcRect.width_;
            } else if (dstRect.left_ + dstRect.width_ >= static_cast<int32_t>(screenInfo.width)) {
                srcRect.left_ = bufferWidth - srcRect.width_;
            }
            break;
        }
        case GraphicTransformType::GRAPHIC_FLIP_V: {
            if (srcRect.top_ >= 0) {
                srcRect.top_ = bufferHeight - srcRect.top_ - srcRect.height_;
            } else if (dstRect.top_ + dstRect.height_ >= static_cast<int32_t>(screenInfo.height)) {
                srcRect.top_ = bufferHeight - srcRect.height_;
            }
            break;
        }
        default:
            break;
    }
    node.SetSrcRect(srcRect);
}

bool RSUniHwcComputeUtil::IsHwcEnabledByScalingMode(RSSurfaceRenderNode& node, const ScalingMode scalingMode)
{
    // We temporarily disabled HWC when scalingMode is freeze or no_scale_crop
    if (scalingMode == ScalingMode::SCALING_MODE_FREEZE || scalingMode == ScalingMode::SCALING_MODE_NO_SCALE_CROP) {
        RS_OPTIONAL_TRACE_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by scalingMode[%d]",
            node.GetName().c_str(), node.GetId(), static_cast<int32_t>(scalingMode));
        node.SetHardwareForcedDisabledState(true);
        return false;
    }
    return true;
}

void RSUniHwcComputeUtil::UpdateHwcNodeByScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix,
    const Drawing::Matrix& gravityMatrix, const Drawing::Matrix& scalingModeMatrix)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    const auto& property = node.GetRenderProperties();
    const auto buffer = surfaceHandler->GetBuffer();
    const auto consumer = surfaceHandler->GetConsumer();
    float bufferWidth = buffer->GetSurfaceBufferWidth();
    float bufferHeight = buffer->GetSurfaceBufferHeight();
    const GraphicTransformType consumerTransformType = node.GetFixRotationByUser() ?
        GetRotateTransformForRotationFixed(node, consumer) : GetConsumerTransform(node, buffer, consumer);
    Drawing::Matrix inverseScalingModeMatrix;
    Drawing::Matrix inverseGravityMatrix;
    Drawing::Matrix inverseTotalMatrix;
    if (!totalMatrix.Invert(inverseTotalMatrix) || !scalingModeMatrix.Invert(inverseScalingModeMatrix) ||
        !gravityMatrix.Invert(inverseGravityMatrix)) {
        return;
    }
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    Drawing::Rect newDstRect;
    scalingModeMatrix.MapRect(newDstRect, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    gravityMatrix.MapRect(newDstRect, newDstRect);
    totalMatrix.MapRect(newDstRect, newDstRect);
    Drawing::Rect dstRectWithoutScaling;
    gravityMatrix.MapRect(dstRectWithoutScaling, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    totalMatrix.MapRect(dstRectWithoutScaling, dstRectWithoutScaling);
    IntersectRect(newDstRect, dstRectWithoutScaling);
    Drawing::Rect bounds = node.GetDstRectWithoutRenderFit();
    IntersectRect(newDstRect, bounds);
    node.SetDstRect({std::floor(newDstRect.GetLeft()), std::floor(newDstRect.GetTop()),
        std::ceil(newDstRect.GetWidth()), std::ceil(newDstRect.GetHeight())});
    Drawing::Rect newSrcRect;
    inverseTotalMatrix.MapRect(newSrcRect, newDstRect);
    inverseGravityMatrix.MapRect(newSrcRect, newSrcRect);
    inverseScalingModeMatrix.MapRect(newSrcRect, newSrcRect);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(newSrcRect.left_, newSrcRect.top_);
        std::swap(newSrcRect.right_, newSrcRect.bottom_);
    }
    newSrcRect = CalcSrcRectByBufferRotation(*buffer, consumerTransformType, newSrcRect);
    node.SetSrcRect({newSrcRect.GetLeft(), newSrcRect.GetTop(), newSrcRect.GetWidth(), newSrcRect.GetHeight()});
}

RectI RSUniHwcComputeUtil::SrcRectRotateTransform(const SurfaceBuffer& buffer,
    const GraphicTransformType bufferRotateTransformType, const RectI& newSrcRect)
{
    const auto bufferWidth = buffer.GetSurfaceBufferWidth();
    const auto bufferHeight = buffer.GetSurfaceBufferHeight();
    int left = newSrcRect.GetLeft();
    int top = newSrcRect.GetTop();
    int width = newSrcRect.GetWidth();
    int height = newSrcRect.GetHeight();
    RectI srcRect(newSrcRect);
    switch (bufferRotateTransformType) {
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            srcRect = RectI {bufferWidth - width - left, top, width, height};
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            srcRect = RectI {bufferWidth - width - left, bufferHeight - height - top, width, height};
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            srcRect = RectI {left, bufferHeight - height - top, width, height};
            break;
        }
        default:
            break;
    }
    return srcRect;
}

void RSUniHwcComputeUtil::UpdateRealSrcRect(RSSurfaceRenderNode& node, const RectI& absRect)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    auto buffer = surfaceHandler->GetBuffer();
    if (!consumer || !buffer) {
        return;
    }
    auto bufferRotateTransformType = node.GetFixRotationByUser() ?
        GetRotateTransformForRotationFixed(node, consumer) :
        RSBaseRenderUtil::GetRotateTransform(RSBaseRenderUtil::GetSurfaceBufferTransformType(consumer, buffer));
    const auto& property = node.GetRenderProperties();
    auto bufferWidth = buffer->GetSurfaceBufferWidth();
    auto bufferHeight = buffer->GetSurfaceBufferHeight();
    const auto boundsWidth = property.GetBoundsWidth();
    const auto boundsHeight = property.GetBoundsHeight();
    auto srcRect = node.GetSrcRect();
    if (bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    if ((bufferWidth != boundsWidth || bufferHeight != boundsHeight) &&
        node.GetRenderProperties().GetFrameGravity() != Gravity::TOP_LEFT) {
        float xScale = (ROSEN_EQ(boundsWidth, 0.0f) ? 1.0f : bufferWidth / (boundsWidth == 0.0f ? 1.f : boundsWidth));
        float yScale = (ROSEN_EQ(boundsHeight, 0.0f) ? 1.0f : bufferHeight /
            (boundsHeight == 0.0f ? 1.0f : boundsHeight));
        if (absRect == node.GetDstRect()) {
            // If the SurfaceRenderNode is completely in the DisplayRenderNode,
            // we do not need to crop the buffer.
            srcRect.width_ = bufferWidth;
            srcRect.height_ = bufferHeight;
        } else {
            srcRect.left_ = srcRect.left_ * xScale;
            srcRect.top_ = srcRect.top_ * yScale;
            srcRect.width_ = std::min(static_cast<int32_t>(std::ceil(srcRect.width_ * xScale)), bufferWidth);
            srcRect.height_ = std::min(static_cast<int32_t>(std::ceil(srcRect.height_ * yScale)), bufferHeight);
        }
    }
    RectI bufferRect(0, 0, bufferWidth, bufferHeight);
    RectI calibratedSrcRect = srcRect.IntersectRect(bufferRect);
    if (bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(calibratedSrcRect.left_, calibratedSrcRect.top_);
        std::swap(calibratedSrcRect.width_, calibratedSrcRect.height_);
    }
    auto newSrcRect = SrcRectRotateTransform(*buffer, bufferRotateTransformType, calibratedSrcRect);
    node.SetSrcRect(newSrcRect);
}

inline void RSUniHwcComputeUtil::UpdateHwcNodeDrawingCache(const std::shared_ptr<RSRenderNode>& parent,
    HwcPropertyContext& ctx)
{
    bool& isNodeRenderByDrawingCache = ctx.isNodeRenderByDrawingCache;
    if (isNodeRenderByDrawingCache) {
        return;
    }
    // If parentNode of hwcNode is marked as freeze or nodegroup, RS will disable hwcNode's hardware composer
    isNodeRenderByDrawingCache |= parent->IsStaticCached() ||
        parent->GetNodeGroupType() != RSRenderNode::NodeGroupType::NONE;
}

inline void RSUniHwcComputeUtil::UpdateHwcNodeBlendNeedChildNode(const std::shared_ptr<RSRenderNode>& parent,
    HwcPropertyContext& ctx)
{
    bool& isNodeRenderByChildNode = ctx.isNodeRenderByChildNode;
    if (isNodeRenderByChildNode) {
        return;
    }
    isNodeRenderByChildNode |= IsBlendNeedChildNode(*parent);
}

inline void RSUniHwcComputeUtil::UpdateHwcNodeAlpha(const std::shared_ptr<RSRenderNode>& parent,
    HwcPropertyContext& ctx)
{
    auto& parentProperty = parent->GetRenderProperties();
    ctx.alpha *= parentProperty.GetAlpha();
}

inline void RSUniHwcComputeUtil::UpdateHwcNodeTotalMatrix(const std::shared_ptr<RSRenderNode>& parent,
    HwcPropertyContext& ctx)
{
    if (auto opt = GetMatrix(parent)) {
        ctx.totalMatrix.PostConcat(opt.value());
    }
}

void RSUniHwcComputeUtil::UpdateHwcNodeProperty(const std::shared_ptr<RSSurfaceRenderNode>& hwcNode)
{
    if (hwcNode == nullptr) {
        RS_LOGE("hwcNode is null.");
        return;
    }
    std::vector<RectI> currIntersectedRoundCornerAABBs = {};
    bool hasCornerRadius = !hwcNode->GetRenderProperties().GetCornerRadius().IsZero();
    const auto& hwcNodeGeo = hwcNode->GetRenderProperties().GetBoundsGeometry();
    auto hwcNodeRect = hwcNodeGeo->GetAbsRect();
    hwcNode->SetAbsRotation(hwcNode->GetRenderProperties().GetRotation());
    HwcPropertyContext ctx;
    ctx.alpha = hwcNode->GetRenderProperties().GetAlpha();
    ctx.totalMatrix = hwcNodeGeo->GetMatrix();
    RSUniHwcComputeUtil::TraverseParentNodeAndReduce(
        hwcNode,
        [&ctx](const std::shared_ptr<RSRenderNode>& parent) { UpdateHwcNodeDrawingCache(parent, ctx); },
        [&ctx](const std::shared_ptr<RSRenderNode>& parent) { UpdateHwcNodeBlendNeedChildNode(parent, ctx); },
        [&ctx](const std::shared_ptr<RSRenderNode>& parent) { UpdateHwcNodeAlpha(parent, ctx); },
        [&ctx](const std::shared_ptr<RSRenderNode>& parent) { UpdateHwcNodeTotalMatrix(parent, ctx); },
        [hwcNode](std::shared_ptr<RSRenderNode> parent) {
            hwcNode->SetAbsRotation(hwcNode->GetAbsRotation() + parent->GetRenderProperties().GetRotation());
        },
        [&currIntersectedRoundCornerAABBs, hwcNodeRect](std::shared_ptr<RSRenderNode> parent) {
            auto& parentProperty = parent->GetRenderProperties();
            auto cornerRadius = parentProperty.GetCornerRadius();
            auto maxCornerRadius = *std::max_element(std::begin(cornerRadius.data_), std::end(cornerRadius.data_));
            auto parentGeo = parentProperty.GetBoundsGeometry();
            static const std::array offsetVecs {
                UIPoint { 0, 0 },
                UIPoint { 1, 0 },
                UIPoint { 0, 1 },
                UIPoint { 1, 1 }
            };

            // The logic here is to calculate whether the HWC Node affects
            // the round corner property of the parent node.
            // The method is calculating the rounded AABB of each HWC node
            // with respect to all parent nodes above it and storing the results.
            // When a HWC node is found below, the AABBs and the HWC node
            // are checked for intersection. If there is an intersection,
            // the node above it is disabled from taking the HWC pipeline.
            auto checkIntersectWithRoundCorner = [&currIntersectedRoundCornerAABBs, hwcNodeRect](
                const RectI& rect, float radiusX, float radiusY) {
                if (radiusX <= 0 || radiusY <= 0) {
                    return;
                }
                UIPoint offset { rect.GetWidth() - radiusX, rect.GetHeight() - radiusY };
                UIPoint anchorPoint { rect.GetLeft(), rect.GetTop() };
                std::for_each(std::begin(offsetVecs), std::end(offsetVecs),
                    [&currIntersectedRoundCornerAABBs, hwcNodeRect, offset,
                     radiusX, radiusY, anchorPoint](auto offsetVec) {
                        auto res = anchorPoint + offset * offsetVec;
                        auto roundCornerAABB = RectI(res.x_, res.y_, radiusX, radiusY);
                        if (!roundCornerAABB.IntersectRect(hwcNodeRect).IsEmpty()) {
                            currIntersectedRoundCornerAABBs.push_back(roundCornerAABB);
                        }
                    }
                );
            };
            if (parentGeo) {
                auto parentRect = parentGeo->GetAbsRect();
                checkIntersectWithRoundCorner(parentRect, maxCornerRadius, maxCornerRadius);

                if (parentProperty.GetClipToRRect()) {
                    RRect parentClipRRect = parentProperty.GetClipRRect();
                    RectI parentClipRect = parentGeo->MapAbsRect(parentClipRRect.rect_);
                    float maxClipRRectCornerRadiusX = 0;
                    float maxClipRRectCornerRadiusY = 0;
                    constexpr size_t radiusVecSize = 4;
                    for (size_t i = 0; i < radiusVecSize; ++i) {
                        maxClipRRectCornerRadiusX = std::max(maxClipRRectCornerRadiusX, parentClipRRect.radius_[i].x_);
                        maxClipRRectCornerRadiusY = std::max(maxClipRRectCornerRadiusY, parentClipRRect.radius_[i].y_);
                    }
                    checkIntersectWithRoundCorner(parentClipRect, maxClipRRectCornerRadiusX, maxClipRRectCornerRadiusY);
                }
            }
        });
    if (ctx.isNodeRenderByDrawingCache || ctx.isNodeRenderByChildNode) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by drawing cache or need blend with "
            "childNode, isNodeRenderByDrawingCache[%d], isNodeRenderByChildNode[%d]",
            hwcNode->GetName().c_str(), hwcNode->GetId(), ctx.isNodeRenderByDrawingCache, ctx.isNodeRenderByChildNode);
        hwcNode->SetHardwareForcedDisabledState(true);
    }
    hwcNode->SetTotalMatrix(ctx.totalMatrix);
    hwcNode->SetGlobalAlpha(ctx.alpha);
    hwcNode->SetIntersectedRoundCornerAABBs(std::move(currIntersectedRoundCornerAABBs));
}

bool RSUniHwcComputeUtil::HasNonZRotationTransform(const Drawing::Matrix& matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    if (!ROSEN_EQ(value[Drawing::Matrix::Index::PERSP_0], 0.f) ||
        !ROSEN_EQ(value[Drawing::Matrix::Index::PERSP_1], 0.f)) {
        return true;
    }
    int rotation = static_cast<int>(round(value[Drawing::Matrix::Index::SCALE_X] *
        value[Drawing::Matrix::Index::SKEW_Y] +
        value[Drawing::Matrix::Index::SCALE_Y] *
        value[Drawing::Matrix::Index::SKEW_X]));
    if (rotation != 0) {
        return true;
    }
    int vectorZ = value[Drawing::Matrix::Index::SCALE_X] * value[Drawing::Matrix::Index::SCALE_Y] -
        value[Drawing::Matrix::Index::SKEW_Y] * value[Drawing::Matrix::Index::SKEW_X];
    return vectorZ < 0;
}

GraphicTransformType RSUniHwcComputeUtil::GetLayerTransform(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    if (!surfaceHandler) {
        return GraphicTransformType::GRAPHIC_ROTATE_NONE;
    }
    auto consumer = surfaceHandler->GetConsumer();
    auto surfaceParams = node.GetStagingRenderParams() == nullptr
                             ? nullptr
                             : static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    int32_t rotationDegree = RSBaseRenderUtil::GetScreenRotationOffset(surfaceParams);
    int surfaceNodeRotation = 0;
    if (node.GetFixRotationByUser()) {
        surfaceNodeRotation = -1 * rotationDegree;
    } else {
        surfaceNodeRotation = RSUniRenderUtil::TransferToAntiClockwiseDegrees(
                                static_cast<int>(round(node.GetAbsRotation())) % ROUND_ANGLE);
    }
    auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    auto buffer = node.GetRSSurfaceHandler()->GetBuffer();
    if (consumer != nullptr && buffer != nullptr) {
        if (consumer->GetSurfaceBufferTransformType(buffer, &transformType) != GSERROR_OK) {
            RS_LOGE("RSUniHwcComputeUtil::GetLayerTransform GetSurfaceBufferTransformType failed");
        }
    }
    int consumerTransform = RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(transformType));
    GraphicTransformType consumerFlip = RSBaseRenderUtil::GetFlipTransform(transformType);
    int totalRotation =
        (RSBaseRenderUtil::RotateEnumToInt(screenInfo.rotation) + surfaceNodeRotation + consumerTransform + 360) % 360;
    GraphicTransformType rotateEnum = RSBaseRenderUtil::RotateEnumToInt(totalRotation, consumerFlip);

    RS_OPTIONAL_TRACE_NAME_FMT("RSUniHwcComputeUtil::GetLayerTransform nodeId:[%llu] fixRotationByUser:[%s] "
                               "surfaceNodeRotation:[%d] consumerTransform:[%d] rotateEnum:[%d]",
                               node.GetId(), std::to_string(node.GetFixRotationByUser()).c_str(),
                               surfaceNodeRotation, consumerTransform, rotateEnum);
            
    return rotateEnum;
}

std::optional<Drawing::Matrix> RSUniHwcComputeUtil::GetMatrix(
    const std::shared_ptr<RSRenderNode>& hwcNode)
{
    if (!hwcNode) {
        return std::nullopt;
    }
    const auto& property = hwcNode->GetRenderProperties();
    auto geo = property.GetBoundsGeometry();
    if (LIKELY(!property.GetSandBox().has_value())) {
        return geo->GetMatrix();
    } else {
        auto parent = hwcNode->GetParent().lock();
        if (!parent) {
            return std::nullopt;
        }
        auto parentGeo = parent->GetRenderProperties().GetBoundsGeometry();
        Drawing::Matrix invertAbsParentMatrix;
        parentGeo->GetAbsMatrix().Invert(invertAbsParentMatrix);
        Drawing::Matrix relativeMat = geo->GetAbsMatrix();
        relativeMat.PostConcat(invertAbsParentMatrix);
        return relativeMat;
    }
}

bool RSUniHwcComputeUtil::IntersectRect(Drawing::Rect& result, const Drawing::Rect& other)
{
    float left = std::max(result.left_, other.left_);
    float top = std::max(result.top_, other.top_);
    float right = std::min(result.right_, other.right_);
    float bottom = std::min(result.bottom_, other.bottom_);
    Drawing::Rect intersectedRect(left, top, right, bottom);
    if (!intersectedRect.IsValid()) {
        result = Drawing::Rect();
        return false;
    }
    result = intersectedRect;
    return true;
}

bool RSUniHwcComputeUtil::IsBlendNeedFilter(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    return property.NeedFilter() || node.GetHwcRecorder().IsBlendWithBackground() ||
        IsForegroundColorStrategyValid(node);
}

bool RSUniHwcComputeUtil::IsBlendNeedBackground(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    return property.NeedHwcFilter() || node.GetHwcRecorder().IsBlendWithBackground() ||
        IsForegroundColorStrategyValid(node);
}

bool RSUniHwcComputeUtil::IsBlendNeedChildNode(RSRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    return (property.IsColorBlendApplyTypeOffscreen() && !property.IsColorBlendModeNone()) ||
        IsDangerousBlendMode(
            property.GetColorBlendMode(), property.GetColorBlendApplyType()) ||
        property.GetForegroundFilter() != nullptr ||
        property.GetForegroundFilterCache() != nullptr ||
        property.GetMask() != nullptr ||
        property.GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE ||
        property.GetFilter() != nullptr ||
        property.IsLightUpEffectValid() ||
        property.GetLinearGradientBlurPara() != nullptr ||
        property.IsDynamicDimValid() ||
        property.IsFgBrightnessValid() ||
        property.IsWaterRippleValid() ||
        property.GetColorFilter() != nullptr;
}

template<typename T>
std::shared_ptr<RSRenderProperty<T>> RSUniHwcComputeUtil::GetPropertyFromModifier(
    const RSRenderNode& node, RSModifierType type)
{
    auto& drawCmdModifiers = node.GetDrawCmdModifiers();
    auto itr = drawCmdModifiers.find(type);
    if (itr == drawCmdModifiers.end() || itr->second.empty()) {
        return nullptr;
    }
    const auto& modifier = itr->second.back();
    return std::static_pointer_cast<RSRenderProperty<T>>(modifier->GetProperty());
}

bool RSUniHwcComputeUtil::IsForegroundColorStrategyValid(RSRenderNode& node)
{
    auto property = GetPropertyFromModifier<ForegroundColorStrategyType>(
        node, RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY);
    return (property == nullptr) ? false : property->Get() != ForegroundColorStrategyType::INVALID;
}

bool RSUniHwcComputeUtil::IsDangerousBlendMode(int32_t blendMode, int32_t blendApplyType)
{
    // The NONE blend mode does not create an offscreen buffer
    if (blendMode == static_cast<int32_t>(RSColorBlendMode::NONE)) {
        return false;
    }
    return RSPropertiesPainter::IsDangerousBlendMode(blendMode - 1, blendApplyType);
}

float RSUniHwcComputeUtil::GetFloatRotationDegreeFromMatrix(const Drawing::Matrix& matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    return std::atan2(value[Drawing::Matrix::Index::SKEW_X], value[Drawing::Matrix::Index::SCALE_X]) *
        (RS_ROTATION_180 / PI);
}

#undef CHECK_NULL_VOID
} // namespace Rosen
} // namespace OHOS