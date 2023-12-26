/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "recording/adaptive_image_helper.h"

#ifdef SUPPORT_OHOS_PIXMAP
#include "pixel_map.h"
#endif
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr int32_t CORNER_SIZE = 4;
constexpr int32_t DATA_CALCULATION_MULTIPLE = 2;

Rect AdaptiveImageHelper::GetDstRect(const AdaptiveImageInfo& rsImageInfo, const float srcRectWidth,
    const float srcRectHeight, const float frameRectWidth, const float frameRectHeight)
{
    Rect dstRect = {};
    if (srcRectWidth == 0 || srcRectHeight == 0 || rsImageInfo.scale == 0) {
        LOGE("AdaptiveImageHelper::GetDstRect, invalid parameter.");
        return dstRect;
    }

    const float srcW = srcRectWidth / rsImageInfo.scale;
    const float srcH = srcRectHeight / rsImageInfo.scale;
    const float frameW = frameRectWidth;
    const float frameH = frameRectHeight;
    float dstW = frameW;
    float dstH = frameH;
    float ratio = srcW / srcH;

    switch (static_cast<ImageFit>(rsImageInfo.fitNum)) {
        case ImageFit::TOP_LEFT:
            dstRect = Rect(0.f, 0.f, srcW, srcH);
            return dstRect;
        case ImageFit::FILL:
            break;
        case ImageFit::NONE:
            dstW = srcW;
            dstH = srcH;
            break;
        case ImageFit::COVER:
            dstW = std::max(frameW, frameH * ratio);
            dstH = std::max(frameH, frameW / ratio);
            break;
        case ImageFit::FIT_WIDTH:
            dstH = frameW / ratio;
            break;
        case ImageFit::FIT_HEIGHT:
            dstW = frameH * ratio;
            break;
        case ImageFit::SCALE_DOWN:
            if (srcW < frameW && srcH < frameH) {
                dstW = srcW;
                dstH = srcH;
            } else {
                dstW = std::min(frameW, frameH * ratio);
                dstH = std::min(frameH, frameW / ratio);
            }
            break;
        case ImageFit::CONTAIN:
        default:
            dstW = std::min(frameW, frameH * ratio);
            dstH = std::min(frameH, frameW / ratio);
            break;
    }
    dstRect = Rect((frameW - dstW) / DATA_CALCULATION_MULTIPLE, (frameH - dstH) / DATA_CALCULATION_MULTIPLE,
        dstW + (frameW - dstW) / DATA_CALCULATION_MULTIPLE, dstH + (frameH - dstH) / DATA_CALCULATION_MULTIPLE);
    return dstRect;
}

void AdaptiveImageHelper::ApplyCanvasClip(Canvas& canvas, const Rect& rect, const AdaptiveImageInfo& rsImageInfo,
    const float srcRectWidth, const float srcRectHeight)
{
    Rect drawRect;
    if (rsImageInfo.repeatNum != static_cast<int32_t>(ImageRepeat::NO_REPEAT)) {
        drawRect = rect;
    } else {
        Rect dstRect = GetDstRect(rsImageInfo, srcRectWidth, srcRectHeight, rect.GetWidth(), rect.GetHeight());
        if (!dstRect.IsValid()) {
            LOGE("AdaptiveImageHelper::ApplyCanvasClip, get dst rect failed.");
            return;
        }
        float left = std::max(dstRect.GetLeft(), rect.GetLeft());
        float top = std::max(dstRect.GetTop(), rect.GetTop());
        float width = std::min(dstRect.GetRight(), rect.GetRight()) - left;
        float height = std::min(dstRect.GetBottom(), rect.GetBottom()) - top;
        if ((width <= 0) || (height <= 0)) {
            drawRect = Rect();
        } else {
            drawRect = Rect(left, top, width + left, height + top);
        }
    }

    std::vector<Point> radiusValue(rsImageInfo.radius, rsImageInfo.radius + CORNER_SIZE);
    RoundRect rrect(drawRect, radiusValue);
    canvas.ClipRoundRect(rrect, ClipOp::INTERSECT, true);
}

void AdaptiveImageHelper::DrawImage(Canvas& canvas, const Rect& rect, const std::shared_ptr<Image>& image,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
    if (image == nullptr) {
        LOGE("AdaptiveImageHelper::DrawImage, image is nullptr.");
        return;
    }
    canvas.Save();
    ApplyCanvasClip(canvas, rect, rsImageInfo, image->GetWidth(), image->GetHeight());
    DrawImageRepeatRect(canvas, rect, image, rsImageInfo, smapling);
    canvas.Restore();
}

void AdaptiveImageHelper::DrawImage(Canvas& canvas, const Rect& rect, const std::shared_ptr<Data>& data,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
    if (data == nullptr) {
        LOGE("AdaptiveImageHelper::DrawImage, data is nullptr.");
        return;
    }
    canvas.Save();
    ApplyCanvasClip(
        canvas, rect, rsImageInfo, static_cast<float>(rsImageInfo.width), static_cast<float>(rsImageInfo.height));
    DrawImageRepeatRect(canvas, rect, data, rsImageInfo, smapling);
    canvas.Restore();
}

void AdaptiveImageHelper::DrawPixelMap(Canvas& canvas, const Rect& rect,
    const std::shared_ptr<Media::PixelMap>& pixelMap, const AdaptiveImageInfo& rsImageInfo,
    const SamplingOptions& smapling)
{
#ifdef SUPPORT_OHOS_PIXMAP
    if (pixelMap == nullptr) {
        LOGE("AdaptiveImageHelper::DrawPixelMap, pixelMap is nullptr.");
        return;
    }
    canvas.Save();
    ApplyCanvasClip(canvas, rect, rsImageInfo, pixelMap->GetWidth(), pixelMap->GetHeight());
    DrawPixelMapRepeatRect(canvas, rect, pixelMap, rsImageInfo, smapling);
    canvas.Restore();
#else
    LOGE("Not support drawing PixelMap");
#endif
}

void AdaptiveImageHelper::GetRectCropMultiple(
    const Rect& rect, const int32_t repeatNum, const Rect& dstRect, BoundaryRect& boundaryRect)
{
    int32_t minX = 0;
    int32_t minY = 0;
    int32_t maxX = 0;
    int32_t maxY = 0;

    float eps = 0.01;
    float left = rect.GetLeft();
    float top = rect.GetTop();
    float right = rect.GetRight();
    float bottom = rect.GetBottom();

    if (static_cast<ImageRepeat>(repeatNum) == ImageRepeat::REPEAT_X ||
        static_cast<ImageRepeat>(repeatNum) == ImageRepeat::REPEAT) {
        while (dstRect.GetLeft() + minX * dstRect.GetWidth() > left + eps) {
            --minX;
        }
        while (dstRect.GetLeft() + maxX * dstRect.GetWidth() < right - eps) {
            ++maxX;
        }
    }
    if (static_cast<ImageRepeat>(repeatNum) == ImageRepeat::REPEAT_Y ||
        static_cast<ImageRepeat>(repeatNum) == ImageRepeat::REPEAT) {
        while (dstRect.GetTop() + minY * dstRect.GetHeight() > top + eps) {
            --minY;
        }
        while (dstRect.GetTop() + maxY * dstRect.GetHeight() < bottom - eps) {
            ++maxY;
        }
    }
    if (minX > 0 || maxX < 0 || minY > 0 || maxY < 0) {
        LOGE("AdaptiveImageHelper::GetRectCropMultiple, data is invalid.");
        boundaryRect.minX = 0;
        boundaryRect.minY = 0;
        boundaryRect.maxX = 0;
        boundaryRect.maxY = 0;
    } else {
        boundaryRect.minX = minX;
        boundaryRect.minY = minY;
        boundaryRect.maxX = maxX;
        boundaryRect.maxY = maxY;
    }
}

void AdaptiveImageHelper::DrawImageRepeatRect(Canvas& canvas, const Rect& rect, const std::shared_ptr<Image>& image,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
    Rect dstRect = GetDstRect(rsImageInfo, image->GetWidth(), image->GetHeight(), rect.GetWidth(), rect.GetHeight());
    if (!dstRect.IsValid()) {
        LOGE("AdaptiveImageHelper::DrawImageRepeatRect, get dst rect failed.");
        return;
    }

    BoundaryRect boundaryRect = {};
    GetRectCropMultiple(rect, rsImageInfo.repeatNum, dstRect, boundaryRect);
    int32_t minX = boundaryRect.minX;
    int32_t minY = boundaryRect.minY;
    int32_t maxX = boundaryRect.maxX;
    int32_t maxY = boundaryRect.maxY;

    auto src = Rect(0.0, 0.0, image->GetWidth(), image->GetHeight());
    for (int32_t i = minX; i <= maxX; ++i) {
        for (int32_t j = minY; j <= maxY; ++j) {
            auto dst = Rect(dstRect.GetLeft() + i * dstRect.GetWidth(), dstRect.GetTop() + j * dstRect.GetHeight(),
                dstRect.GetLeft() + (i + 1) * dstRect.GetWidth(), dstRect.GetTop() + (j + 1) * dstRect.GetHeight());
            canvas.DrawImageRect(*image, src, dst, smapling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        }
    }
}

void AdaptiveImageHelper::DrawImageRepeatRect(Canvas& canvas, const Rect& rect, const std::shared_ptr<Data>& data,
    const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& smapling)
{
#ifdef ACE_ENABLE_GPU
    Rect dstRect = GetDstRect(rsImageInfo, static_cast<float>(rsImageInfo.width),
        static_cast<float>(rsImageInfo.height), rect.GetWidth(), rect.GetHeight());
    if (!dstRect.IsValid()) {
        LOGE("AdaptiveImageHelper::DrawImageRepeatRect, get dst rect failed.");
        return;
    }

    BoundaryRect boundaryRect = {};
    GetRectCropMultiple(rect, rsImageInfo.repeatNum, dstRect, boundaryRect);
    int32_t minX = boundaryRect.minX;
    int32_t minY = boundaryRect.minY;
    int32_t maxX = boundaryRect.maxX;
    int32_t maxY = boundaryRect.maxY;

    // make compress image
    if (canvas.GetGPUContext() == nullptr) {
        LOGE("AdaptiveImageHelper::DrawImageRepeatRect, GetGPUContext is nullptr.");
        return;
    }
    auto image = std::make_shared<Image>();
    if (!image->BuildFromCompressed(*canvas.GetGPUContext(), data, static_cast<int>(rsImageInfo.width),
        static_cast<int>(rsImageInfo.height), CompressedType::ASTC_RGBA8_4x4)) {
        LOGE("AdaptiveImageHelper::DrawImageRepeatRect, image is nullptr.");
        return;
    }

    auto src = Rect(0.0, 0.0, static_cast<float>(rsImageInfo.width), static_cast<float>(rsImageInfo.height));
    for (int32_t i = minX; i <= maxX; ++i) {
        for (int32_t j = minY; j <= maxY; ++j) {
            auto dst = Rect(dstRect.GetLeft() + i * dstRect.GetWidth(), dstRect.GetTop() + j * dstRect.GetHeight(),
                dstRect.GetLeft() + (i + 1) * dstRect.GetWidth(), dstRect.GetTop() + (j + 1) * dstRect.GetHeight());
            canvas.DrawImageRect(*image, src, dst, smapling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        }
    }
#endif
}

#ifdef SUPPORT_OHOS_PIXMAP
static ColorType PixelFormatToColorType(Media::PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case Media::PixelFormat::RGB_565:
            return ColorType::COLORTYPE_RGB_565;
        case Media::PixelFormat::RGBA_8888:
            return ColorType::COLORTYPE_RGBA_8888;
        case Media::PixelFormat::BGRA_8888:
            return ColorType::COLORTYPE_BGRA_8888;
        case Media::PixelFormat::ALPHA_8:
            return ColorType::COLORTYPE_ALPHA_8;
        case Media::PixelFormat::RGBA_F16:
            return ColorType::COLORTYPE_RGBA_F16;
        default:
            return ColorType::COLORTYPE_UNKNOWN;
    }
}

static AlphaType AlphaTypeToAlphaType(Media::AlphaType alphaType)
{
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return AlphaType::ALPHATYPE_OPAQUE;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return AlphaType::ALPHATYPE_PREMUL;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return AlphaType::ALPHATYPE_UNKNOWN;
    }
}

static BitmapFormat MakeBitmapFormat(const Media::ImageInfo& imageInfo)
{
    ColorType ct = PixelFormatToColorType(imageInfo.pixelFormat);
    AlphaType at = AlphaTypeToAlphaType(imageInfo.alphaType);
    return BitmapFormat { ct, at };
}
#endif

void AdaptiveImageHelper::DrawPixelMapRepeatRect(Canvas& canvas, const Rect& rect,
    const std::shared_ptr<Media::PixelMap>& pixelMap, const AdaptiveImageInfo& rsImageInfo,
    const SamplingOptions& smapling)
{
#ifdef SUPPORT_OHOS_PIXMAP
    Rect dstRect =
        GetDstRect(rsImageInfo, pixelMap->GetWidth(), pixelMap->GetHeight(), rect.GetWidth(), rect.GetHeight());
    if (!dstRect.IsValid()) {
        LOGE("AdaptiveImageHelper::DrawPixelMapRepeatRect, get dst rect failed.");
        return;
    }
    BoundaryRect boundaryRect = {};
    GetRectCropMultiple(rect, rsImageInfo.repeatNum, dstRect, boundaryRect);
    int32_t minX = boundaryRect.minX;
    int32_t minY = boundaryRect.minY;
    int32_t maxX = boundaryRect.maxX;
    int32_t maxY = boundaryRect.maxY;

    // convert pixelMap to drawing image
    Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    BitmapFormat format = MakeBitmapFormat(imageInfo);
    Bitmap bitmap;
    bitmap.Build(imageInfo.size.width, imageInfo.size.height, format);
    bitmap.SetPixels(const_cast<void*>(static_cast<const void*>(pixelMap->GetPixels())));
    auto image = std::make_shared<Image>();
    image->BuildFromBitmap(bitmap);

    auto src = Rect(0.0, 0.0, pixelMap->GetWidth(), pixelMap->GetHeight());
    for (int32_t i = minX; i <= maxX; ++i) {
        for (int32_t j = minY; j <= maxY; ++j) {
            auto dst = Rect(dstRect.GetLeft() + i * dstRect.GetWidth(), dstRect.GetTop() + j * dstRect.GetHeight(),
                dstRect.GetLeft() + (i + 1) * dstRect.GetWidth(), dstRect.GetTop() + (j + 1) * dstRect.GetHeight());
            canvas.DrawImageRect(*image, src, dst, smapling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        }
    }
#else
    LOGE("Not support drawing PixelMap");
#endif
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
