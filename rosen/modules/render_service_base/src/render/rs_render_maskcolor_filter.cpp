/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "render/rs_render_maskcolor_filter.h"
#ifdef USE_M133_SKIA
#include "include/core/SkColor.h"
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {

constexpr float MAX_ALPHA = 255.0f;

RSMaskColorShaderFilter::RSMaskColorShaderFilter(int colorMode, RSColor maskColor)
    : RSRenderFilterParaBase(RSUIFilterType::MASK_COLOR), colorMode_(colorMode), maskColor_(maskColor)
{
#ifdef USE_M133_SKIA
    const auto hashFunc = SkChecksum::Hash32;
#else
    const auto hashFunc = SkOpts::hash;
#endif
    hash_ = hashFunc(&type_, sizeof(type_), 0);
    hash_ = hashFunc(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = hashFunc(&maskColor_, sizeof(maskColor_), hash_);
}

RSMaskColorShaderFilter::~RSMaskColorShaderFilter() = default;

int RSMaskColorShaderFilter::GetColorMode() const
{
    return colorMode_;
}

RSColor RSMaskColorShaderFilter::GetMaskColor() const
{
    return maskColor_;
}

Drawing::ColorQuad RSMaskColorShaderFilter::CalcAverageColor(std::shared_ptr<Drawing::Image> image)
{
    // create a 1x1 SkPixmap
    uint32_t pixel[1] = { 0 };
    Drawing::ImageInfo single_pixel_info(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL);
    Drawing::Bitmap single_pixel;
    single_pixel.Build(single_pixel_info, single_pixel_info.GetBytesPerPixel());
    single_pixel.SetPixels(pixel);

    // resize snapshot to 1x1 to calculate average color
    // kMedium_SkFilterQuality will do bilerp + mipmaps for down-scaling, we can easily get average color
    image->ScalePixels(single_pixel,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR));
    // convert color format and return average color
    return SkColor4f::FromBytes_RGBA(pixel[0]).toSkColor();
}

void RSMaskColorShaderFilter::CaclMaskColor(std::shared_ptr<Drawing::Image>& image)
{
    if (colorMode_ == AVERAGE && image != nullptr) {
        // update maskColor while persevere alpha
        auto colorPicker = CalcAverageColor(image);
        maskColor_ = RSColor(Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
            Drawing::Color::ColorQuadGetB(colorPicker), maskColor_.GetAlpha());
    }
}

void RSMaskColorShaderFilter::PreProcess(std::shared_ptr<Drawing::Image>& image)
{
    CaclMaskColor(image);
}

void RSMaskColorShaderFilter::PostProcess(Drawing::Canvas& canvas)
{
    Drawing::Brush brush;
    brush.SetColor(maskColor_.AsArgbInt());

    canvas.DrawBackground(brush);
}

float RSMaskColorShaderFilter::GetPostProcessAlpha() const
{
    float rawAlpha = static_cast<float>(maskColor_.GetAlpha());
    return (rawAlpha / MAX_ALPHA);
}
} // namespace Rosen
} // namespace OHOS
