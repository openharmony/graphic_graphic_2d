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
#include "render/rs_maskcolor_shader_filter.h"

namespace OHOS {
namespace Rosen {
RSMaskColorShaderFilter::RSMaskColorShaderFilter(int colorMode, RSColor maskColor)
    : colorMode_(colorMode), maskColor_(maskColor)
{
    type_ = ShaderFilterType::MASK_COLOR;
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&colorMode_, sizeof(colorMode_), hash_);
    hash_ = SkOpts::hash(&maskColor_, sizeof(maskColor_), hash_);
}

RSMaskColorShaderFilter::~RSMaskColorShaderFilter() = default;

void RSMaskColorShaderFilter::InitColorMod()
{
    if (colorMode_ == FASTAVERAGE && RSColorPickerCacheTask::ColorPickerPartialEnabled) {
        colorPickerTask_ = std::make_shared<RSColorPickerCacheTask>();
    }
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
    if (colorMode_ == FASTAVERAGE && RSColorPickerCacheTask::ColorPickerPartialEnabled &&
        image != nullptr) {
        RSColor color;
        if (colorPickerTask_->GetWaitRelease()) {
            if (colorPickerTask_->GetColor(color) && colorPickerTask_->GetFirstGetColorFinished()) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            return;
        }
        if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask_, image)) {
            if (colorPickerTask_->GetColor(color)) {
                maskColor_ = RSColor(color.GetRed(), color.GetGreen(), color.GetBlue(), maskColor_.GetAlpha());
            }
            colorPickerTask_->SetStatus(CacheProcessStatus::WAITING);
        }
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

const std::shared_ptr<RSColorPickerCacheTask>& RSMaskColorShaderFilter::GetColorPickerCacheTask() const
{
    return colorPickerTask_;
}

void RSMaskColorShaderFilter::ReleaseColorPickerFilter()
{
    if (colorPickerTask_ == nullptr) {
        return;
    }
    colorPickerTask_->ReleaseColorPicker();
}
} // namespace Rosen
} // namespace OHOS