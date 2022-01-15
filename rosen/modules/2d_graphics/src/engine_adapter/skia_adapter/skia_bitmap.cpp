/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "include/core/SkImageInfo.h"

#include "image/bitmap.h"

#include "skia_bitmap.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaBitmap::SkiaBitmap() : skiaBitmap_() {}

static inline SkColorType ConvertToSkColorType(const ColorType& format)
{
    switch (format) {
        case COLORTYPE_UNKNOWN:
            return kUnknown_SkColorType;
        case COLORTYPE_ALPHA_8:
            return kAlpha_8_SkColorType;
        case COLORTYPE_RGB_565:
            return kRGB_565_SkColorType;
        case COLORTYPE_ARGB_4444:
            return kARGB_4444_SkColorType;
        case COLORTYPE_RGBA_8888:
            return kRGBA_8888_SkColorType;
        case COLORTYPE_BGRA_8888:
            return kBGRA_8888_SkColorType;
        default:
            return kUnknown_SkColorType;
    }
}

static inline SkAlphaType ConvertToSkAlphaType(const AlphaType& format)
{
    switch (format) {
        case ALPHATYPE_UNKNOWN:
            return kUnknown_SkAlphaType;
        case ALPHATYPE_OPAQUYE:
            return kOpaque_SkAlphaType;
        case ALPHATYPE_PREMUL:
            return kPremul_SkAlphaType;
        case ALPHATYPE_UNPREMUL:
            return kUnpremul_SkAlphaType;
        default:
            return kUnknown_SkAlphaType;
    }
}

static inline SkImageInfo MakeSkImageInfo(const int width, const int height, const BitmapFormat& format)
{
    auto imageInfo = SkImageInfo::Make(width, height, ConvertToSkColorType(format.colorType),
        ConvertToSkAlphaType(format.alphaType));
    return imageInfo;
}

void SkiaBitmap::Build(const int width, const int height, const BitmapFormat& format)
{
    auto imageInfo = MakeSkImageInfo(width, height, format);
    skiaBitmap_.setInfo(imageInfo);
    skiaBitmap_.allocPixels();
}

int SkiaBitmap::GetWidth()
{
    return skiaBitmap_.width();
}

int SkiaBitmap::GetHeight()
{
    return skiaBitmap_.height();
}

void* SkiaBitmap::GetPixels()
{
    return skiaBitmap_.getPixels();
}

void SkiaBitmap::SetPixels(void* pixels)
{
    skiaBitmap_.setPixels(pixels);
}

const SkBitmap& SkiaBitmap::ExportSkiaBitmap() const
{
    return skiaBitmap_;
}

void SkiaBitmap::CopyPixels(Bitmap& dst, int srcLeft, int srcTop, int width, int height)
{
    int w = dst.GetWidth();
    int h = dst.GetHeight();
    BitmapFormat format = dst.GetFormat();
    void* dstPixels = dst.GetPixels();

    SkImageInfo imageInfo = MakeSkImageInfo(w, h, format);
    int srcX = srcLeft;
    int srcY = srcTop;
    size_t dstRowBytes = width * height;

    skiaBitmap_.readPixels(imageInfo, dstPixels, dstRowBytes, srcX, srcY);
}

void SkiaBitmap::ClearWithColor(const ColorQuad& color)
{
    SkColor skColor = static_cast<SkColor>(color);
    skiaBitmap_.eraseColor(skColor);
}

ColorQuad SkiaBitmap::GetColor(int x, int y)
{
    SkColor color;
    color = skiaBitmap_.getColor(x, y);
    return static_cast<ColorQuad>(color);
}

void SkiaBitmap::Free()
{
    skiaBitmap_.reset();
}

bool SkiaBitmap::IsValid()
{
    return skiaBitmap_.drawsNothing();
}
}
}
}
