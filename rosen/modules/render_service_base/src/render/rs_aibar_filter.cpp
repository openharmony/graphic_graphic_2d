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
#include "render/rs_aibar_filter.h"

#include <unordered_map>

#include "src/core/SkOpts.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_properties_painter.h"
#include "render/rs_kawase_blur.h"

#include "include/effects/SkImageFilters.h"
#include "include/core/SkTileMode.h"

namespace OHOS {
namespace Rosen {

const bool KAWASE_BLUR_ENABLED = RSSystemProperties::GetKawaseEnabled();
RSAIBarFilter::RSAIBarFilter()
    : RSDrawingFilter(nullptr)
{
    type_ = RSFilter::AIBAR;
    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
}

RSAIBarFilter::~RSAIBarFilter() = default;

void RSAIBarFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    std::vector<float> aiInvertCoef = GetAiInvertCoef();
    float radius = aiInvertCoef[5]; // aiInvertCoef[5] is filter_radius
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float imageWidth = image->GetWidth();
    float imageHeight = image->GetHeight();
    auto builder = MakeBinarizationShader(
        imageWidth, imageHeight, imageShader);
    auto invertedImage = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
    if (invertedImage == nullptr) {
        ROSEN_LOGE("RSAIBarFilter::DrawImageRect invertedImage is null");
        return;
    }
    KawaseParameter param = KawaseParameter(src, dst, radius, nullptr, 1.0);
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR &&
        KawaseBlurFilter::GetKawaseBlurFilter()->ApplyKawaseBlur(canvas, invertedImage, param)) {
        return;
    }
    // if kawase blur failed, use gauss blur
    Drawing::Brush brush;
    brush.SetShaderEffect(builder->MakeShader(nullptr, false));
    Drawing::Filter filter;
    if (KAWASE_BLUR_ENABLED) {
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(radius, radius, Drawing::TileMode::CLAMP,
            nullptr, Drawing::ImageBlurType::KAWASE));
    } else {
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(radius, radius, Drawing::TileMode::CLAMP,
            nullptr));
    }
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
}

std::string RSAIBarFilter::GetDescription()
{
    return "RSAIBarFilter";
}

std::string RSAIBarFilter::GetDetailedDescription()
{
    return "RSAIBarFilterBlur";
}

std::vector<float> RSAIBarFilter::GetAiInvertCoef()
{
    const auto& aiInvertCoef = RSSystemProperties::GetAiInvertCoef();
    if (!IsAiInvertCoefValid(aiInvertCoef)) {
        static std::vector<float> aiInvertCoefDefault {0.0, 1.0, 0.55, 0.4, 1.6, 45.0};
        return aiInvertCoefDefault;
    }
    return aiInvertCoef;
}

bool RSAIBarFilter::IsAiInvertCoefValid(const std::vector<float>& aiInvertCoef)
{
    return ROSEN_LNE(aiInvertCoef[0], aiInvertCoef[1]) &&
        ROSEN_GE(aiInvertCoef[0], 0.0) && ROSEN_LE(aiInvertCoef[0], 1.0) && // aiInvertCoef[0] is low
        ROSEN_GE(aiInvertCoef[1], 0.0) && ROSEN_LE(aiInvertCoef[1], 1.0) && // aiInvertCoef[1] is high
        ROSEN_GE(aiInvertCoef[2], 0.0) && ROSEN_LE(aiInvertCoef[2], 1.0) && // aiInvertCoef[2] is threshold
        ROSEN_GE(aiInvertCoef[3], 0.0) && ROSEN_LE(aiInvertCoef[3], 1.0) && // aiInvertCoef[3] is opacity
        ROSEN_GE(aiInvertCoef[4], 0.0) && ROSEN_LE(aiInvertCoef[4], 2.0) && // aiInvertCoef[4] is saturation <= 2.0
        ROSEN_GNE(aiInvertCoef[5], 0.0); // aiInvertCoef[5] is filter_radius
}


std::shared_ptr<Drawing::RuntimeShaderBuilder> RSAIBarFilter::MakeBinarizationShader(
    float imageWidth, float imageHeight, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;
    // coefficient of saturation borrowed from
    // the saturate filter in RSProperties::GenerateColorFilter()
    static constexpr char prog[] = R"(
        uniform shader imageShader;
        uniform float low;
        uniform float high;
        uniform float threshold;
        uniform float opacity;
        uniform float saturation;

        const vec3 toLuminance = vec3(0.3086, 0.6094, 0.0820);

        vec4 main(vec2 drawing_coord) {
            vec3 c = imageShader(drawing_coord).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float bin = mix(high, low, step(threshold, gray));
            vec3 invert = vec3(bin, bin, bin);
            float luminance = dot(c, toLuminance);
            vec3 satAdjust = mix(vec3(luminance), c, saturation);
            vec3 res = mix(satAdjust, invert, opacity);
            return vec4(res, 1.0);
        }
    )";
    std::vector<float> aiInvertCoef = GetAiInvertCoef();
    if (binarizationShaderEffect_ == nullptr) {
        binarizationShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (binarizationShaderEffect_ == nullptr) {
            ROSEN_LOGE("MakeBinarizationShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(binarizationShaderEffect_);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("low", aiInvertCoef[0]); // aiInvertCoef[0] is low
    builder->SetUniform("high", aiInvertCoef[1]); // aiInvertCoef[1] is high
    builder->SetUniform("threshold", aiInvertCoef[2]); // aiInvertCoef[2] is threshold
    builder->SetUniform("opacity", aiInvertCoef[3]); // aiInvertCoef[3] is opacity
    builder->SetUniform("saturation", aiInvertCoef[4]); // aiInvertCoef[4] is saturation

    return builder;
}

} // namespace Rosen
} // namespace OHOS