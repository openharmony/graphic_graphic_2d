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
#include "ge_aibar_shader_filter.h"

#include <unordered_map>

#include "ge_log.h"
#include "include/core/SkTileMode.h"
#include "include/effects/SkImageFilters.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {

GEAIBarShaderFilter::GEAIBarShaderFilter(const Drawing::GEAIBarShaderFilterParams& params)
    : aiBarLow_(params.aiBarLow), aiBarHigh_(params.aiBarHigh), aiBarThreshold_(params.aiBarThreshold),
    aiBarOpacity_(params.aiBarOpacity), aiBarSaturation_(params.aiBarSaturation)
{}

const std::string GEAIBarShaderFilter::GetDescription() const
{
    return "GEAIBarShaderFilter";
}

std::shared_ptr<Drawing::Image> GEAIBarShaderFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("GEAIBarShaderFilter::ProcessImage image is null");
        return image;
    }

    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float imageWidth = image->GetWidth();
    float imageHeight = image->GetHeight();
    auto builder = MakeBinarizationShader(imageWidth, imageHeight, imageShader);
    auto invertedImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (invertedImage == nullptr) {
        LOGE("GEAIBarShaderFilter::ProcessImage invertedImage is null");
        return image;
    }

    return invertedImage;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> GEAIBarShaderFilter::MakeBinarizationShader(
    float imageWidth, float imageHeight, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;

    // coefficient of saturation borrowed from
    // the saturate filter in RSProperties::GenerateColorFilter()
    static constexpr char prog[] = R"(
        uniform half low;
        uniform half high;
        uniform half threshold;
        uniform half opacity;
        uniform half saturation;
        uniform shader imageShader;

        const vec3 toLuminance = vec3(0.3086, 0.6094, 0.0820);

        half4 main(float2 coord) {
            half3 c = imageShader.eval(coord).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float bin = mix(high, low, step(threshold, gray));
            half3 invert = half3(bin, bin, bin);
            float luminance = dot(c, toLuminance);
            half3 satAdjust = mix(vec3(luminance), c, saturation);
            half3 res = mix(satAdjust, invert, opacity);
            return half4(res, 1.0);
        }
    )";

    if (binarizationShaderEffect_ == nullptr) {
        binarizationShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (binarizationShaderEffect_ == nullptr) {
            LOGE("MakeBinarizationShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(binarizationShaderEffect_);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("low", aiBarLow_);               // aiInvertCoef[0] is low
    builder->SetUniform("high", aiBarHigh_);             // aiInvertCoef[1] is high
    builder->SetUniform("threshold", aiBarThreshold_);   // aiInvertCoef[2] is threshold
    builder->SetUniform("opacity", aiBarOpacity_);       // aiInvertCoef[3] is opacity
    builder->SetUniform("saturation", aiBarSaturation_); // aiInvertCoef[4] is saturation

    return builder;
}

} // namespace Rosen
} // namespace OHOS
