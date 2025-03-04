/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "rs_render_thread_util.h"

#include "platform/common/rs_log.h"
#include "utils/camera3d.h"
#ifdef ROSEN_OHOS
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#endif

namespace OHOS {
namespace Rosen {
void RSRenderThreadUtil::SrcRectScaleDown(TextureExportBufferDrawParam& params, const RectF& localBounds)
{
    uint32_t newWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
    uint32_t newHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
    // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
    uint32_t boundsWidth = static_cast<uint32_t>(localBounds.GetWidth());
    uint32_t boundsHeight = static_cast<uint32_t>(localBounds.GetHeight());

    uint32_t newWidthBoundsHeight = newWidth * boundsHeight;
    uint32_t newHeightBoundsWidth = newHeight * boundsWidth;

    if (newWidthBoundsHeight > newHeightBoundsWidth) {
        newWidth = boundsWidth * newHeight / boundsHeight;
    } else if (newWidthBoundsHeight < newHeightBoundsWidth) {
        newHeight = boundsHeight * newWidth / boundsWidth;
    } else {
        return;
    }

    uint32_t currentWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
    uint32_t currentHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
    if (newWidth < currentWidth) {
        // the crop is too wide
        uint32_t dw = currentWidth - newWidth;
        auto halfdw = dw / 2;
        params.srcRect = Drawing::Rect(params.srcRect.GetLeft() + static_cast<int32_t>(halfdw), params.srcRect.GetTop(),
            params.srcRect.GetLeft() + static_cast<int32_t>(halfdw) + static_cast<int32_t>(newWidth),
            params.srcRect.GetTop() + params.srcRect.GetHeight());
    } else {
        // thr crop is too tall
        uint32_t dh = currentHeight - newHeight;
        auto halfdh = dh / 2;
        params.srcRect = Drawing::Rect(params.srcRect.GetLeft(), params.srcRect.GetTop() + static_cast<int32_t>(halfdh),
            params.srcRect.GetLeft() + params.srcRect.GetWidth(),
            params.srcRect.GetTop() + static_cast<int32_t>(halfdh) + static_cast<int32_t>(newHeight));
    }
}

void RSRenderThreadUtil::SrcRectScaleFit(TextureExportBufferDrawParam& params, const RectF& localBounds)
{
    uint32_t srcWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
    uint32_t srcHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
    float newWidth = 0.0f;
    float newHeight = 0.0f;
    // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
    uint32_t boundsWidth = static_cast<uint32_t>(localBounds.GetWidth());
    uint32_t boundsHeight = static_cast<uint32_t>(localBounds.GetHeight());
    if (boundsWidth == 0 || boundsHeight == 0 || srcWidth == 0 || srcHeight == 0) {
        return;
    }

    if (srcWidth * boundsHeight > srcHeight * boundsWidth) {
        newWidth = boundsWidth;
        newHeight = srcHeight * newWidth / srcWidth;
    } else if (srcWidth * boundsHeight < srcHeight * boundsWidth) {
        newHeight = boundsHeight;
        newWidth = newHeight * srcWidth / srcHeight;
    } else {
        newWidth = boundsWidth;
        newHeight = boundsHeight;
    }
    newHeight = newHeight * srcHeight / boundsHeight;
    newWidth = newWidth * srcWidth / boundsWidth;
    if (newWidth < srcWidth) {
        float halfdw = (srcWidth - newWidth) / 2;
        params.dstRect = Drawing::Rect(params.srcRect.GetLeft() + halfdw, params.srcRect.GetTop(),
            params.srcRect.GetLeft() + halfdw + newWidth, params.srcRect.GetTop() + params.srcRect.GetHeight());
    } else if (newHeight < srcHeight) {
        float halfdh = (srcHeight - newHeight) / 2;
        params.dstRect = Drawing::Rect(params.srcRect.GetLeft(), params.srcRect.GetTop() + halfdh,
            params.srcRect.GetLeft() + params.srcRect.GetWidth(), params.srcRect.GetTop() + halfdh + newHeight);
    }
}

GraphicTransformType RSRenderThreadUtil::GetFlipTransform(GraphicTransformType transform)
{
    switch (transform) {
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT90:
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT180:
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT270: {
            return GraphicTransformType::GRAPHIC_FLIP_H;
        }
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT90:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT180:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT270: {
            return GraphicTransformType::GRAPHIC_FLIP_V;
        }
        default: {
            return transform;
        }
    }
}

void RSRenderThreadUtil::FlipMatrix(GraphicTransformType transform, TextureExportBufferDrawParam& params)
{
    GraphicTransformType type = GetFlipTransform(transform);
    if (type != GraphicTransformType::GRAPHIC_FLIP_H && type != GraphicTransformType::GRAPHIC_FLIP_V) {
        return;
    }

    const int angle = 180;
    Drawing::Camera3D camera3D;
    switch (type) {
        case GraphicTransformType::GRAPHIC_FLIP_H: {
            camera3D.RotateYDegrees(angle);
            break;
        }
        case GraphicTransformType::GRAPHIC_FLIP_V: {
            camera3D.RotateXDegrees(angle);
            break;
        }
        default: {
            return;
        }
    }
    Drawing::Matrix flip;
    camera3D.ApplyToMatrix(flip);
    const float half = 0.5f;
    flip.PreTranslate(-half * params.dstRect.GetWidth(), -half * params.dstRect.GetHeight());
    flip.PostTranslate(half * params.dstRect.GetWidth(), half * params.dstRect.GetHeight());
    params.matrix.PreConcat(flip);
}

Drawing::Matrix RSRenderThreadUtil::GetSurfaceTransformMatrix(
    GraphicTransformType rotationTransform, const RectF& bufferBounds)
{
    Drawing::Matrix matrix;
    const float bufferWidth = bufferBounds.GetWidth();
    const float bufferHeight = bufferBounds.GetHeight();

    switch (rotationTransform) {
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            matrix.PreTranslate(0, bufferHeight);
            matrix.PreRotate(-90);  // rotate 90 degrees anti-clockwise at last.
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            matrix.PreTranslate(bufferWidth, bufferHeight);
            matrix.PreRotate(-180);  // rotate 180 degrees anti-clockwise at last.
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            matrix.PreTranslate(bufferWidth, 0);
            matrix.PreRotate(-270); // rotate 270 degrees anti-clockwise at last.
            break;
        }
        default:
            break;
    }
    return matrix;
}

Drawing::Matrix RSRenderThreadUtil::GetGravityMatrix(
    Gravity gravity, const RectF& bufferBounds, const RectF& bounds)
{
    Drawing::Matrix gravityMatrix;
    auto frameWidth = bufferBounds.GetWidth();
    auto frameHeight = bufferBounds.GetHeight();
    const float boundsWidth = bounds.GetWidth();
    const float boundsHeight = bounds.GetHeight();
    if (ROSEN_EQ(frameWidth, boundsWidth) && ROSEN_EQ(frameHeight, boundsHeight)) {
        return gravityMatrix;
    }

    if (!RSPropertiesPainter::GetGravityMatrix(gravity,
        RectF {0.0f, 0.0f, boundsWidth, boundsHeight}, frameWidth, frameHeight, gravityMatrix)) {
        RS_LOGD("RSRenderThreadUtil::DealWithNodeGravity did not obtain gravity matrix.");
    }

    return gravityMatrix;
}

GraphicTransformType RSRenderThreadUtil::GetRotateTransform(GraphicTransformType transform)
{
    switch (transform) {
        case GraphicTransformType::GRAPHIC_FLIP_H:
        case GraphicTransformType::GRAPHIC_FLIP_V: {
            return GraphicTransformType::GRAPHIC_ROTATE_NONE;
        }
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT90:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT90: {
            return GraphicTransformType::GRAPHIC_ROTATE_90;
        }
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT180:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT180: {
            return GraphicTransformType::GRAPHIC_ROTATE_180;
        }
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT270:
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT270: {
            return GraphicTransformType::GRAPHIC_ROTATE_270;
        }
        default: {
            return transform;
        }
    }
}

#ifdef ROSEN_OHOS
void RSRenderThreadUtil::DealWithSurfaceRotationAndGravity(GraphicTransformType transform, Gravity gravity,
    RectF& localBounds, TextureExportBufferDrawParam& params)
{
    auto rotationTransform = GetRotateTransform(transform);
    RectF bufferBounds = {0.0f, 0.0f, 0.0f, 0.0f};
    if (params.buffer != nullptr) {
        bufferBounds = {0.0f, 0.0f, params.buffer->GetSurfaceBufferWidth(), params.buffer->GetSurfaceBufferHeight()};
        if (rotationTransform == GraphicTransformType::GRAPHIC_ROTATE_90 ||
            rotationTransform == GraphicTransformType::GRAPHIC_ROTATE_270) {
            std::swap(bufferBounds.width_, bufferBounds.height_);
        }
    }
    // deal with gravity
    params.matrix.PreConcat(GetGravityMatrix(gravity, bufferBounds, localBounds));
    // deal with transformType
    params.matrix.PreConcat(GetSurfaceTransformMatrix(rotationTransform, bufferBounds));
    
    if (rotationTransform == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        rotationTransform == GraphicTransformType::GRAPHIC_ROTATE_270) {
        // after rotate, we should swap dstRect and bound's width and height.
        std::swap(localBounds.width_, localBounds.height_);
    }
    // because we use the gravity matrix above(which will implicitly includes scale effect),
    // we must disable the scale effect that from srcRect to dstRect.
    params.dstRect = params.srcRect;
}

TextureExportBufferDrawParam RSRenderThreadUtil::CreateTextureExportBufferDrawParams(
    RSSurfaceRenderNode& surfaceNode, GraphicTransformType graphicTransformType, sptr<OHOS::SurfaceBuffer> buffer)
{
    TextureExportBufferDrawParam params;
    if (buffer == nullptr) {
        return params;
    }
    params.buffer = buffer;
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    const auto& property = surfaceNode.GetRenderProperties();
    const float boundsWidth = property.GetBoundsWidth();
    const float boundsHeight = property.GetBoundsHeight();
    params.dstRect = Drawing::Rect(0, 0, boundsWidth, boundsHeight);
    RectF localBounds = {0.f, 0.f, boundsWidth, boundsHeight};
    auto gravity = property.GetFrameGravity();
    DealWithSurfaceRotationAndGravity(graphicTransformType, gravity, localBounds, params);
    FlipMatrix(graphicTransformType, params);
    ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        SrcRectScaleDown(params, localBounds);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        SrcRectScaleFit(params, localBounds);
    }
    return params;
}
#endif
} // namespace Rosen
} // namespace OHOS