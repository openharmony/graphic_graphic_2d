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
#ifndef GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H

#include "ge_gradient_blur_para.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT GELinearGradientBlurShaderFilter : public GEShaderFilter {
public:
    GELinearGradientBlurShaderFilter(const Drawing::GELinearGradientBlurShaderFilterParams& params);
    GELinearGradientBlurShaderFilter(const GELinearGradientBlurShaderFilter&) = delete;
    GELinearGradientBlurShaderFilter operator=(const GELinearGradientBlurShaderFilter&) = delete;
    ~GELinearGradientBlurShaderFilter() override;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

    std::string GetDescription();
    std::string GetDetailedDescription();
    void SetBoundsGeometry(float geoWidth, float geoHeight)
    {
        geoWidth_ = geoWidth;
        geoHeight_ = geoHeight;
    }

    std::shared_ptr<Drawing::Image> DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const;

private:
    static void TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias);
    static void ComputeScale(float width, float height, bool useMaskAlgorithm);
    static void MakeHorizontalMeanBlurEffect();
    static void MakeVerticalMeanBlurEffect();

    friend class RSMarshallingHelper;
    std::shared_ptr<GELinearGradientBlurPara> linearGradientBlurPara_ = nullptr;
    inline static float imageScale_ = 1.f;
    inline static float geoWidth_ = 0.f;
    inline static float geoHeight_ = 0.f;
    inline static float tranX_ = 0.f;
    inline static float tranY_ = 0.f;
    inline static bool isOffscreenCanvas_ = true;

    static Drawing::Rect ComputeRectBeforeClip(const uint8_t directionBias, const Drawing::Rect& dst);
    static uint8_t CalcDirectionBias(const Drawing::Matrix& mat);
    static bool GetGEGradientDirectionPoints(
        Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GEGradientDirection direction);
    static std::shared_ptr<Drawing::ShaderEffect> MakeAlphaGradientShader(
        const Drawing::Rect& clipBounds, const std::shared_ptr<GELinearGradientBlurPara>& para, uint8_t directionBias);
    static std::shared_ptr<Drawing::Image> DrawMaskLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image,
        Drawing::Canvas& canvas, std::shared_ptr<GEShaderFilter>& blurFilter,
        std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst);
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeMaskLinearGradientBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
        std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    static void DrawMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas,
        float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst);

    static std::shared_ptr<Drawing::RuntimeEffect> horizontalMeanBlurShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> verticalMeanBlurShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> maskBlurShaderEffect_;
    inline static Drawing::Matrix mat_;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_LINEAR_GRADIENT_BLUR_SHADER_FILTER_H
