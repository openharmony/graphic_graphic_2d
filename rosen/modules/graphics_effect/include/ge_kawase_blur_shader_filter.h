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
#ifndef GRAPHICS_EFFECT_GE_KAWASE_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_KAWASE_BLUR_SHADER_FILTER_H

#include <memory>

#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

#include "draw/canvas.h"
#include "effect/color_filter.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
class GEKawaseBlurShaderFilter : public GEShaderFilter {
public:
    GEKawaseBlurShaderFilter(const Drawing::GEKawaseBlurShaderFilterParams& params);
    ~GEKawaseBlurShaderFilter() override = default;
    int GetRadius() const;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    static Drawing::Matrix GetShaderTransform(
        const Drawing::Canvas* canvas, const Drawing::Rect& blurRect, float scaleW = 1.0f, float scaleH = 1.0f);
    bool InitBlurEffect();
    bool InitMixEffect();
    bool InitSimpleFilter();

    bool InitBlurEffectForAdvancedFilter();
    void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& checkedImage, const Drawing::Rect& src) const;
    void OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const;

    std::shared_ptr<Drawing::Image> ScaleAndAddRandomColor(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Image>& blurImage,
        const Drawing::Rect& src, const Drawing::Rect& dst, int& width, int& height) const;
    std::shared_ptr<Drawing::ShaderEffect> ApplySimpleFilter(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& input, const std::shared_ptr<Drawing::ShaderEffect>& prevShader,
        const Drawing::ImageInfo& scaledInfo, const Drawing::SamplingOptions& linear) const;
    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    std::string GetDescription() const;
    void SetBlurBuilderParam(Drawing::RuntimeShaderBuilder& blurBuilder, const float offsetXY,
        const Drawing::ImageInfo& scaledInfo, const int width, const int height);
    bool IsInputValid(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const Drawing::Rect& dst);
    const OHOS::Rosen::Drawing::Matrix BuildMatrix(
        const Drawing::Rect& src, const Drawing::ImageInfo& scaledInfo, const std::shared_ptr<Drawing::Image>& input);

    int radius_;
    float blurRadius_ = 0.0f;
    float blurScale_ = 0.25f;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_KAWASE_BLUR_SHADER_FILTER_H
