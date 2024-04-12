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
class RSB_EXPORT GEKawaseBlurShaderFilter : public GEShaderFilter {
public:
    GEKawaseBlurShaderFilter(const Drawing::GEKawaseBlurShaderFilterParams& params);
    ~GEKawaseBlurShaderFilter() override;
    int GetRadius() const;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    friend class RSMarshallingHelper;

    static Drawing::Matrix GetShaderTransform(
        const Drawing::Canvas* canvas, const Drawing::Rect& blurRect, float scale = 1.0f);
    bool InitBlurEffect();
    bool InitMixEffect();
    // Advanced Filter
    bool InitBlurEffectForAdvancedFilter();
    void CheckInputImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& checkedImage, const Drawing::Rect& src) const;
    void OutputOriginalImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const;

    std::shared_ptr<Drawing::Image> ScaleAndAddRandomColor(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Image>& blurImage,
        const Drawing::Rect& src, const Drawing::Rect& dst, int& width, int& height) const;
    void ComputeRadiusAndScale(int radius);
    void AdjustRadiusAndScale();
    std::string GetDescription() const;
    void SetBlurBuilderParam(Drawing::RuntimeShaderBuilder& blurBuilder, const float offsetXY,
        const Drawing::ImageInfo& scaledInfo, const int width, const int height);
    bool IsInputValid(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const Drawing::Rect& dst);

    std::shared_ptr<Drawing::RuntimeEffect> blurEffect_;
    std::shared_ptr<Drawing::RuntimeEffect> mixEffect_;
    int radius_;
    float blurRadius_ = 0.0f;
    float blurScale_ = 0.25f;
    std::shared_ptr<Drawing::RuntimeEffect> blurEffectAF_;
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_KAWASE_BLUR_SHADER_FILTER_H
