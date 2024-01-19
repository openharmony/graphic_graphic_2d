/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_GRADIENT_BLUR_PARA_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_GRADIENT_BLUR_PARA_H

#include <array>
#include "common/rs_macros.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {

enum class GradientDirection {
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

class RSB_EXPORT RSLinearGradientBlurPara {
public:
    float blurRadius_;
    // Each pair in fractionStops_ represents <blur degree, position scale>
    std::vector<std::pair<float, float>> fractionStops_;
    GradientDirection direction_;
    std::shared_ptr<RSFilter> LinearGradientBlurFilter_;
    bool useMaskAlgorithm_ = true;
    float originalBase_ = 1000.0f;   // 1000.0f represents original radius_base
    explicit RSLinearGradientBlurPara(const float blurRadius,
                    const std::vector<std::pair<float, float>>fractionStops, const GradientDirection direction)
    {
        if (blurRadius > originalBase_) {
            useMaskAlgorithm_ = false;
        } else {
            useMaskAlgorithm_ = true;
        }
        blurRadius_ = blurRadius;
        fractionStops_ = fractionStops;
        direction_ = direction;
        if (RSSystemProperties::GetMaskLinearBlurEnabled() && useMaskAlgorithm_) {
            LinearGradientBlurFilter_ = RSFilter::CreateBlurFilter(blurRadius_ / 2, blurRadius_ / 2);
        }
    }
    ~RSLinearGradientBlurPara() = default;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_GRADIENT_BLUR_PARA_H