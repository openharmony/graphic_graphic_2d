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
#include "ge_magnifier_shader_filter.h"

#include "ge_log.h"
#include "ge_system_properties.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<Drawing::RuntimeEffect> GEMagnifierShaderFilter::magnifierShaderEffect_ = nullptr;

GEMagnifierShaderFilter::GEMagnifierShaderFilter(const Drawing::GEMagnifierShaderFilterParams& params)
{
    if (!InitMagnifierEffect()) {
        LOGE("GEMagnifierShaderFilter::GEMagnifierShaderFilter failed when initializing MagnifierEffect.");
        return;
    }

    magnifierPara_ = std::make_shared<GEMagnifierParams>();
    magnifierPara_->factor_ = params.factor;
    magnifierPara_->width_ = params.width;
    magnifierPara_->height_ = params.height;
    magnifierPara_->borderWidth_ = params.borderWidth;
    magnifierPara_->cornerRadius_ = params.cornerRadius;
    magnifierPara_->shadowOffsetX_ = params.shadowOffsetX;
    magnifierPara_->shadowOffsetY_ = params.shadowOffsetY;
    magnifierPara_->shadowSize_ = params.shadowSize;
    magnifierPara_->shadowStrength_ = params.shadowStrength;
    magnifierPara_->gradientMaskColor1_ = params.gradientMaskColor1;
    magnifierPara_->gradientMaskColor2_ = params.gradientMaskColor2;
    magnifierPara_->outerContourColor1_ = params.outerContourColor1;
    magnifierPara_->outerContourColor2_ = params.outerContourColor2;
}

std::shared_ptr<Drawing::Image> GEMagnifierShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr || magnifierPara_ == nullptr) {
        LOGE("GEMagnifierShaderFilter::ProcessImage image or para is null");
        return image;
    }

    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float imageWidth = image->GetWidth();
    float imageHeight = image->GetHeight();
    auto builder = MakeMagnifierShader(imageShader, imageWidth, imageHeight);

    auto resultImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (resultImage == nullptr) {
        LOGE("GEMagnifierShaderFilter::ProcessImage resultImage is null");
        return image;
    }

    return resultImage;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEMagnifierShaderFilter::MakeMagnifierShader(
    std::shared_ptr<Drawing::ShaderEffect> imageShader, float imageWidth, float imageHeight)
{
    return nullptr;
}

bool GEMagnifierShaderFilter::InitMagnifierEffect()
{
    return false;
}

void GEMagnifierShaderFilter::ConvertToRgba(uint32_t rgba, float* color)
{
    int16_t alpha = static_cast<int16_t>(rgba & 0xFF);               // 0xff byte mask
    int16_t red = static_cast<int16_t>((rgba & 0xFF000000) >> 24);   // 0xff000000 red mask, 24 red shift
    int16_t green = static_cast<int16_t>((rgba & 0x00FF0000) >> 16); // 0x00ff0000 green mask, 16 green shift
    int16_t blue = static_cast<int16_t>((rgba & 0x0000FF00) >> 8);   // 0x0000ff00 blue mask, 8 blue shift

    color[0] = red * 1.0f / 255.0f;     // 255.0f is the max value, 0 red
    color[1] = green * 1.0f / 255.0f;   // 255.0f is the max value, 1 green
    color[2] = blue * 1.0f / 255.0f;    // 255.0f is the max value, 2 blue
    color[3] = alpha * 1.0f / 255.0f;   // 255.0f is the max value, 3 alpha
}

const std::string GEMagnifierShaderFilter::GetDescription() const
{
    return "GEMagnifierShaderFilter";
}

} // namespace Rosen
} // namespace OHOS
