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
#ifndef GRAPHICS_EFFECT_GE_HPS_BLUR_SHADER_FILTER_H
#define GRAPHICS_EFFECT_GE_HPS_BLUR_SHADER_FILTER_H

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
class GEHpsBlurShaderFilter : public GEShaderFilter {
public:
    GEHpsBlurShaderFilter(const Drawing::HpsBlurFilterParams& params);
    ~GEHpsBlurShaderFilter() override = default;

    std::shared_ptr<Drawing::Image> ProcessImage(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image> image,
        const Drawing::Rect& src, const Drawing::Rect& dst) override;

private:
    std::shared_ptr<Drawing::Image> ApplyHpsBlur(
        Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::HpsBlurParameter& param);
    static Drawing::Matrix GetShaderTransform(const Drawing::Rect& blurRect, float scaleW = 1.0f, float scaleH = 1.0f);
    std::shared_ptr<Drawing::Image> ApplyBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        std::shared_ptr<Drawing::Image>& blurImage, const Drawing::HpsBlurParameter& param) const;

private:
    float radius_ { 0.0 };
    float saturation_ { 1.0 };
    float brightness_ { 1.0 };
};
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_HPS_BLUR_SHADER_FILTER_H