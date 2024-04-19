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
#include "ge_render.h"
#include "ge_visual_effect.h"

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
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    if (!visualEffectContainer) {
        return;
    }
    auto aiBarPara = GetAiInvertCoef();
    auto aiBarFilter = std::make_shared<Drawing::GEVisualEffect>("AIBAR", Drawing::DrawingPaintType::BRUSH);
    if (!aiBarFilter) {
        return;
    }
    aiBarFilter->SetParam("AIBAR_LOW", aiBarPara[0]); // 0 low
    aiBarFilter->SetParam("AIBAR_HIGH", aiBarPara[1]); // 1 high
    aiBarFilter->SetParam("AIBAR_THRESHOLD", aiBarPara[2]); // 2 threshold
    aiBarFilter->SetParam("AIBAR_OPACITY", aiBarPara[3]); // 3 opacity
    aiBarFilter->SetParam("AIBAR_SATURATION", aiBarPara[4]); // 4 saturation
    auto radius = aiBarPara[5];  // 5 模糊半径
    visualEffectContainer->AddToChainedFilter(aiBarFilter);
    auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
    if (!geRender) {
        return;
    }
    static bool DDGR_ENABLED = RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR;
    if (!DDGR_ENABLED && KAWASE_BLUR_ENABLED) {
        auto kawaseFilter = std::make_shared<Drawing::GEVisualEffect>("KAWASE_BLUR", Drawing::DrawingPaintType::BRUSH);
        kawaseFilter->SetParam("KAWASE_BLUR_RADIUS", (int)radius); // 5 模糊半径
        visualEffectContainer->AddToChainedFilter(kawaseFilter);
        auto outImage = geRender->ApplyImageEffect(canvas, *visualEffectContainer,
            image, src, src, Drawing::SamplingOptions());
        Drawing::Brush brushKawas;
        canvas.AttachBrush(brushKawas);
        canvas.DrawImageRect(*outImage, src, dst, Drawing::SamplingOptions());
        canvas.DetachBrush();
        return;
    }
    Drawing::Brush brush;
    Drawing::Filter filter;
    auto invertedImage = geRender->ApplyImageEffect(canvas, *visualEffectContainer,
        image, src, src, Drawing::SamplingOptions());
    auto blurType = KAWASE_BLUR_ENABLED ? Drawing::ImageBlurType::KAWASE : Drawing::ImageBlurType::GAUSS;
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(radius, radius, Drawing::TileMode::CLAMP,
        nullptr, blurType));
    brush.SetFilter(filter);
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*invertedImage, src, dst, Drawing::SamplingOptions());
    canvas.DetachBrush();
    return;
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
