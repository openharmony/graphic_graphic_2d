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

#ifndef SKIA_IMAGE_INFO_H
#define SKIA_IMAGE_INFO_H

#include "include/core/SkImageInfo.h"
#include "include/core/SkEncodedImageFormat.h"

#include "skia_color_space.h"

#include "image/image_info.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaImageInfo {
public:
    static SkColorType ConvertToSkColorType(const ColorType& format)
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
            case COLORTYPE_RGBA_F16:
                return kRGBA_F16_SkColorType;
            case COLORTYPE_N32:
                return kN32_SkColorType;
            case COLORTYPE_RGBA_1010102:
                return kRGBA_1010102_SkColorType;
            case COLORTYPE_GRAY_8:
                return kGray_8_SkColorType;
            default:
                return kUnknown_SkColorType;
        }
    }

    static SkAlphaType ConvertToSkAlphaType(const AlphaType& format)
    {
        switch (format) {
            case ALPHATYPE_UNKNOWN:
                return kUnknown_SkAlphaType;
            case ALPHATYPE_OPAQUE:
                return kOpaque_SkAlphaType;
            case ALPHATYPE_PREMUL:
                return kPremul_SkAlphaType;
            case ALPHATYPE_UNPREMUL:
                return kUnpremul_SkAlphaType;
            default:
                return kUnknown_SkAlphaType;
        }
    }

    static SkImageInfo ConvertToSkImageInfo(const ImageInfo& imageInfo)
    {
        auto colorSpace = imageInfo.GetColorSpace();
        return SkImageInfo::Make(imageInfo.GetWidth(), imageInfo.GetHeight(),
                                 ConvertToSkColorType(imageInfo.GetColorType()),
                                 ConvertToSkAlphaType(imageInfo.GetAlphaType()),
                                 colorSpace ? colorSpace->GetImpl<SkiaColorSpace>()->GetColorSpace() : nullptr);
    }

    static ColorType ConvertToColorType(const SkColorType& format)
    {
        switch (format) {
            case kUnknown_SkColorType:
                return COLORTYPE_UNKNOWN;
            case kAlpha_8_SkColorType:
                return COLORTYPE_ALPHA_8;
            case kRGB_565_SkColorType:
                return COLORTYPE_RGB_565;
            case kARGB_4444_SkColorType:
                return COLORTYPE_ARGB_4444;
            case kRGBA_8888_SkColorType:
                return COLORTYPE_RGBA_8888;
            case kBGRA_8888_SkColorType:
                return COLORTYPE_BGRA_8888;
            case kRGBA_F16_SkColorType:
                return COLORTYPE_RGBA_F16;
            case kRGBA_1010102_SkColorType:
                return COLORTYPE_RGBA_1010102;
            case kGray_8_SkColorType:
                return COLORTYPE_GRAY_8;
            default:
                return COLORTYPE_UNKNOWN;
        }
    }

    static AlphaType ConvertToAlphaType(const SkAlphaType& format)
    {
        switch (format) {
            case kUnknown_SkAlphaType:
                return ALPHATYPE_UNKNOWN;
            case kOpaque_SkAlphaType:
                return ALPHATYPE_OPAQUE;
            case kPremul_SkAlphaType:
                return ALPHATYPE_PREMUL;
            case kUnpremul_SkAlphaType:
                return ALPHATYPE_UNPREMUL;
            default:
                return ALPHATYPE_UNKNOWN;
        }
    }

    static ImageInfo ConvertToRSImageInfo(const SkImageInfo& skImageInfo)
    {
        std::shared_ptr<SkiaColorSpace> skiaColorSpace = std::make_shared<SkiaColorSpace>();
        skiaColorSpace->SetColorSpace(skImageInfo.refColorSpace());
        return {skImageInfo.width(), skImageInfo.height(),
                ConvertToColorType(skImageInfo.colorType()),
                ConvertToAlphaType(skImageInfo.alphaType()),
                ColorSpace::CreateFromImpl(skiaColorSpace)};
    }

    static SkEncodedImageFormat ConvertToSkEncodedImageFormat(const EncodedImageFormat& encodedImageFormat)
    {
        switch (encodedImageFormat) {
            case EncodedImageFormat::JPEG:
                return SkEncodedImageFormat::kJPEG;
            case EncodedImageFormat::PNG:
                return SkEncodedImageFormat::kPNG;
            case EncodedImageFormat::WEBP:
                return SkEncodedImageFormat::kWEBP;
            default:
                return SkEncodedImageFormat::kJPEG;
        }
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif