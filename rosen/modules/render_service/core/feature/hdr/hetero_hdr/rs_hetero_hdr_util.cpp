/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_hetero_hdr_util.h"

#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int RS_RECT_ROTATION_90 = -90;
constexpr int RS_RECT_ROTATION_180 = -180;
constexpr int RS_RECT_ROTATION_270 = -270;
constexpr uint16_t MAX_VIDEO_RECT_PIXEL = 4096;
constexpr uint16_t MIN_VIDEO_RECT_PIXEL = 16;
}

void RSHeteroHDRUtil::GenDrawHDRBufferParams(const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable,
    MDCRectT& hpaeDstRect, bool isFixedDstBuffer, BufferDrawParam& drawableParams)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable.GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRUtil GenDrawHDRBufferParams surfaceParams is nullptr");
        return;
    }
    auto surfaceBuffer = surfaceParams->GetBuffer();
    if (!surfaceBuffer) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRUtil GenDrawHDRBufferParams surfaceBuffer is nullptr");
        return;
    }
    BufferDrawParam localParam;
    CalculateDrawBufferRectMatrix(surfaceParams, hpaeDstRect, isFixedDstBuffer, localParam);
    localParam.dstRect = { 0, 0, hpaeDstRect.right, hpaeDstRect.bottom };
    if (!isFixedDstBuffer) {
        auto layerSrc = surfaceParams->GetLayerInfo().srcRect;
        Drawing::Matrix matrixLoc;
        float sx = static_cast<float>(layerSrc.w) / static_cast<float>(hpaeDstRect.right);
        float sy = static_cast<float>(layerSrc.h) / static_cast<float>(hpaeDstRect.bottom);
        float tx = static_cast<float>(layerSrc.x);
        float ty = static_cast<float>(layerSrc.y);
        matrixLoc.SetScaleTranslate(sx, sy, tx, ty);
        localParam.matrix = drawableParams.matrix;
        localParam.matrix.PreConcat(matrixLoc);
    } else {
        ScalingMode scalingMode = surfaceBuffer->GetSurfaceBufferScalingMode();
        if (ROSEN_EQ(drawableParams.srcRect.GetWidth(), 0.0f) || ROSEN_EQ(drawableParams.srcRect.GetHeight(), 0.0f)) {
            RS_LOGE("[hdrHetero]:RSHeteroHDRUtil GenDrawHDRBufferParams srcRect params is invalid");
            return;
        }
        if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
            auto newWidth = localParam.srcRect.GetWidth();
            auto newHeight = localParam.srcRect.GetHeight();
            float ratioH = newHeight / drawableParams.srcRect.GetHeight();
            float ratioW = newWidth / drawableParams.srcRect.GetWidth();
            newWidth = drawableParams.dstRect.GetWidth() * ratioW;
            newHeight = drawableParams.dstRect.GetHeight() * ratioH;
            float newLeft = drawableParams.dstRect.GetLeft() * ratioW;
            float newTop = drawableParams.dstRect.GetTop() * ratioH;
            localParam.dstRect = { newLeft, newTop, newLeft + newWidth, newTop + newHeight };
        }
    }
    drawableParams.dstRect = localParam.dstRect;
    drawableParams.srcRect = localParam.srcRect;
    drawableParams.matrix = localParam.matrix;
}

bool RSHeteroHDRUtil::ValidateSurface(RSSurfaceRenderParams* surfaceParams)
{
    if (surfaceParams == nullptr) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRUtil ValidateSurface surfaceParams is nullptr");
        return false;
    }
    sptr<SurfaceBuffer> srcBuffer = surfaceParams->GetBuffer();
    const auto& dstRect = surfaceParams->GetLayerInfo().dstRect;
    const auto& srcRect = surfaceParams->GetLayerInfo().srcRect;

    if (srcBuffer == nullptr ||
        dstRect.w > MAX_VIDEO_RECT_PIXEL || dstRect.h > MAX_VIDEO_RECT_PIXEL ||
        dstRect.w <= MIN_VIDEO_RECT_PIXEL || dstRect.h <= MIN_VIDEO_RECT_PIXEL ||
        srcRect.w <= MIN_VIDEO_RECT_PIXEL || srcRect.h <= MIN_VIDEO_RECT_PIXEL ||
        srcRect.w > MAX_VIDEO_RECT_PIXEL || srcRect.h > MAX_VIDEO_RECT_PIXEL ||
        srcBuffer->GetSurfaceBufferWidth() == 0 || srcBuffer->GetSurfaceBufferHeight() == 0) {
        RS_LOGI("[hdrHetero]:RSHeteroHDRUtil ValidateSurface srcBuffer, dstRect or srcRect are invalid");
        return false;
    }
    return true;
}

void RSHeteroHDRUtil::CalculateDrawBufferRectMatrix(const RSSurfaceRenderParams* surfaceParams, MDCRectT& hpaeDstRect,
    bool isFixedDstBuffer, BufferDrawParam& param)
{
    auto dstRect = surfaceParams->GetLayerInfo().dstRect;
    auto surfaceBuffer = surfaceParams->GetBuffer();
    GraphicTransformType bufferTransform = surfaceBuffer->GetSurfaceBufferTransform();
    auto cacheSize = surfaceParams->GetCacheSize();
    Drawing::Rect initSrc = { 0, 0, hpaeDstRect.right, hpaeDstRect.bottom };

    param.srcRect = initSrc;
    if (isFixedDstBuffer) {
        param.dstRect = { 0, 0, cacheSize.x_, cacheSize.y_ };
        if (bufferTransform == GraphicTransformType::GRAPHIC_ROTATE_90 ||
            bufferTransform == GraphicTransformType::GRAPHIC_ROTATE_270) {
            param.srcRect = { 0, 0, hpaeDstRect.bottom, hpaeDstRect.right };
        }
    } else {
        param.dstRect = params.srcRect;
    }
    bool sizeIsInvalid = (ROSEN_EQ(param.srcRect.GetWidth(), 0.0f) || ROSEN_EQ(param.srcRect.GetHeight(), 0.0f));
    if (sizeIsInvalid) {
        RS_LOGE("[hdrHetero]:RSHeteroHDRUtil CalculateDrawBufferRectMatrix srcRect params is invalid");
        return;
    }

    auto sx = param.dstRect.GetWidth() / param.srcRect.GetWidth();
    auto sy = param.dstRect.GetHeight() / param.srcRect.GetHeight();
    auto tx = param.dstRect.GetLeft() - param.srcRect.GetLeft() * sx;
    auto ty = param.dstRect.GetTop() - param.srcRect.GetTop() * sy;
    param.matrix.SetScaleTranslate(sx, sy, tx, ty);

    switch (bufferTransform) {
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            param.matrix.PreTranslate(0, param.srcRect.GetHeight());
            param.matrix.PreRotate(RS_RECT_ROTATION_90); // rotate 90 degrees anti-clockwise at last.
            param.srcRect = initSrc;
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            param.matrix.PreTranslate(param.srcRect.GetWidth(), param.srcRect.GetHeight());
            param.matrix.PreRotate(RS_RECT_ROTATION_180); // rotate 180 degrees anti-clockwise at last.
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            param.matrix.PreTranslate(param.srcRect.GetWidth(), 0);
            param.matrix.PreRotate(RS_RECT_ROTATION_270); // rotate 270 degrees anti-clockwise at last.
            param.srcRect = initSrc;
            break;
        }
        default: break;
    }
    RS_LOGD("[hdrHetero]:RSHeteroHDRUtil CalculateDrawBufferRectMatrix the rect of hpae output: "
        "bottom:%{public}d, right:%{public}d", hpaeDstRect.bottom, hpaeDstRect.right);
}
} // namespace Rosen
} // namespace OHOS