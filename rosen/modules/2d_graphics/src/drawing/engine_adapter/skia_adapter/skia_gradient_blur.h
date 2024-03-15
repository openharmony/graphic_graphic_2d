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

#ifndef SKIA_GRADIENT_BLUR_H
#define SKIA_GRADIENT_BLUR_H

#include "impl_interface/gradient_blur_impl.h"
#include "impl_interface/image_filter_impl.h"
#include "effect/runtime_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaGradientBlur : public GradientBlurImpl {
public:
     SkiaGradientBlur() noexcept {};
    ~SkiaGradientBlur() override {};
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }
    void DrawGradientBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst, Drawing::GradientBlurParam& param) override;

private:
    void ComputeScale(float width, float height, Drawing::GradientBlurParam& param);
    void TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias);
    void MakeHorizontalMeanBlurEffect();
    void MakeVerticalMeanBlurEffect();

    inline static float imageScale_ = 1.f;
    float imageFilterBlurRadius_ = 0.0f;

    bool GetGradientDirectionPoints(
        Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, Drawing::GradientDir direction);
    std::shared_ptr<Drawing::ShaderEffect> MakeAlphaGradientShader(
        const Drawing::Rect& clipBounds, Drawing::GradientBlurParam& param);
    void DrawMaskLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas,
        std::shared_ptr<Drawing::ImageFilter>& blurFilter, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
        const Drawing::Rect& dst);
    std::shared_ptr<Drawing::ShaderEffect> MakeMaskLinearGradientBlurShader(
        std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> gradientShader);
    void DrawMeanLinearGradientBlur(const std::shared_ptr<Drawing::Image>& image, Drawing::Canvas& canvas,
        float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::Rect& dst);

    static std::shared_ptr<Drawing::RuntimeEffect> horizontalMeanBlurShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> verticalMeanBlurShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> maskBlurShaderEffect_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif