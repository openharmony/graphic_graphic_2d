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
#ifndef GRAPHICS_EFFECT_GE_GRADIENT_BLUR_PARA_H
#define GRAPHICS_EFFECT_GE_GRADIENT_BLUR_PARA_H

#include <array>

#include "ge_kawase_blur_shader_filter.h"
#include "ge_shader_filter.h"
#include "ge_visual_effect.h"

namespace OHOS {
namespace Rosen {

enum class GEGradientDirection {
    LEFT = 0,
    TOP,
    RIGHT,
    BOTTOM,
    LEFT_TOP,
    LEFT_BOTTOM,
    RIGHT_TOP,
    RIGHT_BOTTOM,
    NONE,
    START_TO_END,
    END_TO_START,
};

class GELinearGradientBlurPara {
public:
    float blurRadius_;
    // Each pair in fractionStops_ represents <blur degree, position scale>
    std::vector<std::pair<float, float>> fractionStops_;
    GEGradientDirection direction_;
    std::shared_ptr<GEShaderFilter> LinearGradientBlurFilter_;
    bool useMaskAlgorithm_ = true;
    float originalBase_ = 1000.0f; // 1000.0f represents original radius_base
    explicit GELinearGradientBlurPara(const float blurRadius, const std::vector<std::pair<float, float>> fractionStops,
        const GEGradientDirection direction, const bool maskLinearBlurEnabled)
    {
        if (blurRadius > originalBase_) {
            useMaskAlgorithm_ = false;
        } else {
            useMaskAlgorithm_ = true;
        }
        blurRadius_ = blurRadius;
        fractionStops_ = fractionStops;
        direction_ = direction;
        if (maskLinearBlurEnabled && useMaskAlgorithm_) {
            auto kawaseParams = std::make_shared<Drawing::GEKawaseBlurShaderFilterParams>();
            kawaseParams->radius = blurRadius_ / 2; // 2: experience factor
            LinearGradientBlurFilter_ = std::make_shared<GEKawaseBlurShaderFilter>(*kawaseParams);
        }
    }
    ~GELinearGradientBlurPara() = default;
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_GRADIENT_BLUR_PARA_H
