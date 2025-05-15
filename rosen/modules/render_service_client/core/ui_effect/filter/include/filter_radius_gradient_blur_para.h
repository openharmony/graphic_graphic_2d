/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FILTER_RADIUS_GRADIENT_BLUR_PARA_H
#define UIEFFECT_FILTER_RADIUS_GRADIENT_BLUR_PARA_H
#include <iostream>
#include "filter_para.h"
#include "common/rs_vector2.h"
#include "render/rs_gradient_blur_para.h"

namespace OHOS {
namespace Rosen {

class RadiusGradientBlurPara : public FilterPara {
public:
    RadiusGradientBlurPara()
    {
        this->type_ = FilterPara::ParaType::RADIUS_GRADIENT_BLUR;
    }
    ~RadiusGradientBlurPara() override = default;

    void SetBlurRadius(float blurRadius)
    {
        blurRadius_ = blurRadius;
    }

    float GetBlurRadius() const
    {
        return blurRadius_;
    }

    void SetFractionStops(const std::vector<std::pair<float, float>>& fractionStops)
    {
        fractionStops_ = fractionStops;
    }

    const std::vector<std::pair<float, float>>& GetFractionStops() const
    {
        return fractionStops_;
    }

    void SetDirection(GradientDirection direction)
    {
        direction_ = direction;
    }

    GradientDirection GetDirection() const
    {
        return direction_;
    }

private:
    float blurRadius_ = 0.0f;   // 0.0f represents default blur radius
    std::vector<std::pair<float, float>> fractionStops_;
    GradientDirection direction_ = GradientDirection::NONE;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_RADIUS_GRADIENT_BLUR_PARA_H
