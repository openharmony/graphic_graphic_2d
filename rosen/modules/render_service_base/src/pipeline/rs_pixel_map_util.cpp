/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {
using namespace Media;

sk_sp<SkImage> RSPixelMapUtil::PixelMapToSkImage(const std::shared_ptr<Media::PixelMap>& pixmap)
{
    auto skImageInfo = MakeSkImageInfoFromPixelMap(pixmap);
    SkPixmap imagePixmap(skImageInfo, reinterpret_cast<const void*>(pixmap->GetPixels()), pixmap->GetRowBytes());
    return SkImage::MakeFromRaster(imagePixmap, nullptr, nullptr);
}

SkImageInfo RSPixelMapUtil::MakeSkImageInfoFromPixelMap(const std::shared_ptr<Media::PixelMap>& pixmap)
{
    SkColorType ct = PixelFormatToSkColorType(pixmap);
    SkAlphaType at = AlphaTypeToSkAlphaType(pixmap);
    sk_sp<SkColorSpace> cs = ColorSpaceToSkColorSpace(pixmap);
    return SkImageInfo::Make(pixmap->GetWidth(), pixmap->GetHeight(), ct, at, cs);
}

sk_sp<SkColorSpace> RSPixelMapUtil::ColorSpaceToSkColorSpace(const std::shared_ptr<Media::PixelMap>& pixmap)
{
    switch (pixmap->GetColorSpace()) {
        case ColorSpace::LINEAR_SRGB:
            return SkColorSpace::MakeSRGBLinear();
        case ColorSpace::SRGB:
        default:
            return SkColorSpace::MakeSRGB();
    }
}

SkColorType RSPixelMapUtil::PixelFormatToSkColorType(const std::shared_ptr<Media::PixelMap>& pixmap)
{
    switch (pixmap->GetPixelFormat()) {
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

SkAlphaType RSPixelMapUtil::AlphaTypeToSkAlphaType(const std::shared_ptr<Media::PixelMap>& pixmap)
{
    switch (pixmap->GetAlphaType()) {
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
} // namespace Rosen
} // namespace OHOS
