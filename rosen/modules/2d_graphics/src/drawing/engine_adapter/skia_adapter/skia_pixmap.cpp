/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "skia_pixmap.h"
#include "skia_image_info.h"

#include "image/pixmap.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPixmap::SkiaPixmap() : skiaPixmap_() {}
SkiaPixmap::SkiaPixmap(const ImageInfo& imageInfo, const void* addr, size_t rowBytes)
{
    auto skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    skiaPixmap_ = SkPixmap(skImageInfo, addr, rowBytes);
}

void SkiaPixmap::ImportSkiaPixmap(const SkPixmap& skPixmap)
{
    skiaPixmap_ = skPixmap;
}

const SkPixmap& SkiaPixmap::ExportSkiaPixmap() const
{
    return skiaPixmap_;
}

SkiaPixmap::~SkiaPixmap() {}

std::shared_ptr<ColorSpace> SkiaPixmap::GetColorSpace() const
{
    std::shared_ptr<SkiaColorSpace> skiaColorSpace = std::make_shared<SkiaColorSpace>();
    skiaColorSpace->SetColorSpace(skiaPixmap_.refColorSpace());
    return ColorSpace::CreateFromImpl(skiaColorSpace);
}

ColorType SkiaPixmap::GetColorType() const
{
    return SkiaImageInfo::ConvertToColorType(skiaPixmap_.colorType());
}

AlphaType SkiaPixmap::GetAlphaType() const
{
    return SkiaImageInfo::ConvertToAlphaType(skiaPixmap_.alphaType());
}

ColorQuad SkiaPixmap::GetColor(int x, int y) const
{
    SkColor color = skiaPixmap_.getColor(x, y);
    return static_cast<ColorQuad>(color);
}

size_t SkiaPixmap::GetRowBytes() const
{
    return skiaPixmap_.rowBytes();
}

const void* SkiaPixmap::GetAddr() const
{
    return skiaPixmap_.addr();
}

int32_t SkiaPixmap::GetWidth() const
{
    return skiaPixmap_.width();
}

int32_t SkiaPixmap::GetHeight() const
{
    return skiaPixmap_.height();
}

bool SkiaPixmap::ScalePixels(const Pixmap& dst, const SamplingOptions& options) const
{
    SkSamplingOptions skSamplingOptions;
    if (options.GetUseCubic()) {
        skSamplingOptions = SkSamplingOptions({ options.GetCubicCoffB(), options.GetCubicCoffC() });
    } else {
        skSamplingOptions = SkSamplingOptions(static_cast<SkFilterMode>(options.GetFilterMode()),
            static_cast<SkMipmapMode>(options.GetMipmapMode()));
    }
    return skiaPixmap_.scalePixels(dst.GetImpl<SkiaPixmap>()->ExportSkiaPixmap(), skSamplingOptions);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
