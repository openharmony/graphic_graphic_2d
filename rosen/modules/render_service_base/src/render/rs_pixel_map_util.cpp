/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "render/rs_pixel_map_util.h"
#include <memory>

#include "pixel_map.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkImage.h"
#else
#include "drawing/engine_adapter/impl_interface/bitmap_impl.h"
#endif

namespace OHOS {
namespace Rosen {
using namespace Media;

#ifndef USE_ROSEN_DRAWING
static sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(ColorSpace colorSpace)
{
    switch (colorSpace) {
        case ColorSpace::LINEAR_SRGB:
            return SkColorSpace::MakeSRGBLinear();
        case ColorSpace::SRGB:
        default:
            return SkColorSpace::MakeSRGB();
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
static SkColorType PixelFormatToSkColorType(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::RGB_565:
            return SkColorType::kRGB_565_SkColorType;
        case PixelFormat::RGBA_8888:
            return SkColorType::kRGBA_8888_SkColorType;
        case PixelFormat::BGRA_8888:
            return SkColorType::kBGRA_8888_SkColorType;
        case PixelFormat::ALPHA_8:
            return SkColorType::kAlpha_8_SkColorType;
        case PixelFormat::RGBA_F16:
            return SkColorType::kRGBA_F16_SkColorType;
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return SkColorType::kUnknown_SkColorType;
    }
}
#else
static Drawing::ColorType PixelFormatToDrawingColorType(PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case PixelFormat::RGBA_F16:
        case PixelFormat::UNKNOWN:
        case PixelFormat::ARGB_8888:
        case PixelFormat::RGB_888:
        case PixelFormat::NV21:
        case PixelFormat::NV12:
        case PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
static SkAlphaType AlphaTypeToSkAlphaType(AlphaType alphaType)
{
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return SkAlphaType::kUnknown_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return SkAlphaType::kOpaque_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return SkAlphaType::kPremul_SkAlphaType;
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return SkAlphaType::kUnpremul_SkAlphaType;
        default:
            return SkAlphaType::kUnknown_SkAlphaType;
    }
}
#else
static Drawing::AlphaType AlphaTypeToDrawingAlphaType(AlphaType alphaType)
{
    switch (alphaType) {
        case AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
static SkImageInfo MakeSkImageInfo(const ImageInfo& imageInfo)
{
    SkColorType ct = PixelFormatToSkColorType(imageInfo.pixelFormat);
    SkAlphaType at = AlphaTypeToSkAlphaType(imageInfo.alphaType);
    sk_sp<SkColorSpace> cs = ColorSpaceToSkColorSpace(imageInfo.colorSpace);
    return SkImageInfo::Make(imageInfo.size.width, imageInfo.size.height, ct, at, cs);
}
#else
static Drawing::BitmapFormat MakeDrawingBitmapFormat(const ImageInfo& imageInfo)
{
    Drawing::ColorType ct = PixelFormatToDrawingColorType(imageInfo.pixelFormat);
    Drawing::AlphaType at = AlphaTypeToDrawingAlphaType(imageInfo.alphaType);
    return Drawing::BitmapFormat { ct, at };
}
#endif

struct PixelMapReleaseContext {
    explicit PixelMapReleaseContext(std::shared_ptr<PixelMap> pixelMap) : pixelMap_(pixelMap) {}

    ~PixelMapReleaseContext()
    {
        pixelMap_ = nullptr;
    }

private:
    std::shared_ptr<PixelMap> pixelMap_;
};

#ifndef USE_ROSEN_DRAWING
static void PixelMapReleaseProc(const void* /* pixels */, void* context)
{
    PixelMapReleaseContext* ctx = static_cast<PixelMapReleaseContext*>(context);
    if (ctx) {
        delete ctx;
        ctx = nullptr;
    }
}

sk_sp<SkImage> RSPixelMapUtil::ExtractSkImage(std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        return nullptr;
    }
    ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    auto skImageInfo = MakeSkImageInfo(imageInfo);
    SkPixmap skPixmap(skImageInfo, reinterpret_cast<const void*>(pixelMap->GetPixels()), pixelMap->GetRowBytes());
    return SkImage::MakeFromRaster(skPixmap, PixelMapReleaseProc, new PixelMapReleaseContext(pixelMap));
}
#else
std::shared_ptr<Drawing::Image> RSPixelMapUtil::ExtractDrawingImage(
    std::shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        return nullptr;
    }
    ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);

    Drawing::BitmapFormat format = MakeDrawingBitmapFormat(imageInfo);
    Drawing::Bitmap bitmap;
    bitmap.Build(imageInfo.size.width, imageInfo.size.height, format);
    bitmap.SetPixels(const_cast<void*>(static_cast<const void*>(pixelMap->GetPixels())));

    auto image = std::make_shared<Drawing::Image>();
    image->BuildFromBitmap(bitmap);
    return image;
}

#endif
} // namespace Rosen
} // namespace OHOS
