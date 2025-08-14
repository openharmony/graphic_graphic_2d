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

#include "render/rs_hps_blur.h"

#include <mutex>

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "draw/surface.h"
#include "effect/runtime_shader_builder.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {

static constexpr uint32_t MAX_SURFACE_SIZE = 10000;
constexpr float MAX_ALPHA = 255.0f;
static std::shared_ptr<Drawing::RuntimeEffect> g_mixEffect;
std::mutex g_mixEffectMutex;

Drawing::Matrix HpsBlurFilter::GetShaderTransform(const Drawing::Rect& blurRect, float scaleW, float scaleH)
{
    Drawing::Matrix matrix;
    matrix.SetScale(scaleW, scaleH);
    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(blurRect.GetLeft(), blurRect.GetTop());
    matrix.PostConcat(translateMatrix);
    return matrix;
}

std::shared_ptr<Drawing::RuntimeEffect> HpsBlurFilter::GetMixEffect() const
{
    static const std::string mixString(R"(
        uniform shader blurredInput;
        uniform float inColorFactor;

        highp float random(float2 xy) {
            float t = dot(xy, float2(78.233, 12.9898));
            return fract(sin(t) * 43758.5453);
        }
        half4 main(float2 xy) {
            highp float noiseGranularity = inColorFactor / 255.0;
            half4 finalColor = blurredInput.eval(xy);
            float noise  = mix(-noiseGranularity, noiseGranularity, random(xy));
            finalColor.rgb += noise;
            return finalColor;
        }
    )");

    if (g_mixEffect == nullptr) {
        std::unique_lock<std::mutex> lock(g_mixEffectMutex);
        if (g_mixEffect == nullptr) {
            g_mixEffect = Drawing::RuntimeEffect::CreateForShader(mixString);
            if (g_mixEffect == nullptr) {
                return nullptr;
            }
        }
    }
    return g_mixEffect;
}

float HpsBlurFilter::ApplyMaskColorFilter(Drawing::Canvas& offscreenCanvas, float alpha,
    const RSColor& maskColor) const
{
    float newAlpha = alpha;
    if(maskColor != RgbPalette::Transparent()) {
        float maskColorAlpha = static_cast<float>(maskColor.GetAlpha()) / MAX_ALPHA;
        newAlpha += maskColorAlpha - alpha * maskColorAlpha;
        Drawing::Brush maskBrush;
        maskBrush.SetColor(maskColor.AsArgbInt());
        if (ROSEN_EQ(newAlpha, 0.f)) {
            maskBrush.SetAlphaF(0.f);
        } else {
            maskBrush.SetAlphaF(maskColorAlpha / newAlpha);
        }
        ROSEN_LOGD("HpsBlurFilter::ApplyHpsBlur newMaskColor %{public}#x, alpha = %{public}f",
            maskColor.AsArgbInt(), alpha);
        offscreenCanvas.DrawBackground(maskBrush);
    }
    return newAlpha;
}

bool HpsBlurFilter::ApplyHpsBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::HpsBlurParameter& param, float alpha, std::shared_ptr<Drawing::ColorFilter> colorFilter,
    const RSColor& maskColor) const
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr || image == nullptr) {
        return false;
    }
    std::array<int, 2> dimension = canvas.CalcHpsBluredImageDimension(param); // There are 2 variables
    if (dimension[0] <= 0 || dimension[1] <= 0 || dimension[0] >= static_cast<int>(MAX_SURFACE_SIZE)
        || dimension[1] >= static_cast<int>(MAX_SURFACE_SIZE)) {
        ROSEN_LOGD("HpsBlurFilter::ApplyHpsBlur CalcHpsBluredImageDimension error");
        return false;
    }
    std::shared_ptr<Drawing::Surface> offscreenSurface = surface->MakeSurface(dimension[0], dimension[1]);
    if (offscreenSurface == nullptr) {
        return false;
    }

    std::shared_ptr<Drawing::Canvas> offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return false;
    }
    Drawing::Rect dimensionRect = {0, 0, dimension[0], dimension[1]};
    auto offscreenHpsParam = Drawing::HpsBlurParameter(param.src, dimensionRect, param.sigma,
        param.saturation, param.brightness);
    if (!offscreenCanvas->DrawBlurImage(*image, offscreenHpsParam)) {
        return false;
    }
    RS_PROFILER_ADD_HPS_BLUR_METRICS(static_cast<uint32_t>(image->GetWidth() * image->GetHeight()));

    alpha = ApplyMaskColorFilter(*offscreenCanvas, alpha, maskColor);

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        return false;
    }
    auto dst = param.dst;
    Drawing::Brush brush;
    Drawing::SamplingOptions linear(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    const auto blurMatrix = GetShaderTransform(dst, dst.GetWidth() / imageCache->GetWidth(),
        dst.GetHeight() / imageCache->GetHeight());
    const auto blurShader = Drawing::ShaderEffect::CreateImageShader(*imageCache, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, linear, blurMatrix);
    if (!SetShaderEffect(brush, blurShader, imageCache)) { return false; }
    if (colorFilter != nullptr) {
        Drawing::Filter filter;
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
    }
    brush.SetAlphaF(alpha);
    canvas.AttachBrush(brush);
    canvas.DrawRect(dst);
    canvas.DetachBrush();
    return true;
}

bool HpsBlurFilter::SetShaderEffect(Drawing::Brush& brush, std::shared_ptr<Drawing::ShaderEffect> blurShader,
    std::shared_ptr<Drawing::Image> imageCache) const
{
    if (blurShader == nullptr || imageCache == nullptr) {
        return false;
    }
    static auto factor = RSSystemProperties::GetHpsBlurNoiseFactor();
    ROSEN_LOGD("HpsBlurFilter::ApplyHpsBlur HpsBlurNoise %{public}f", factor);
    static constexpr float epsilon = 0.1f;
    if (!ROSEN_LE(factor, epsilon)) {
        auto mixEffect = GetMixEffect();
        if (mixEffect == nullptr) {
            return false;
        }
        Drawing::RuntimeShaderBuilder mixBuilder(mixEffect);
        mixBuilder.SetChild("blurredInput", blurShader);
        ROSEN_LOGD("HpsBlurFilter::HpsBlurNoise factor : %{public}f", factor);
        mixBuilder.SetUniform("inColorFactor", factor);
        brush.SetShaderEffect(mixBuilder.MakeShader(nullptr, imageCache->IsOpaque()));
    } else {
        brush.SetShaderEffect(blurShader);
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS