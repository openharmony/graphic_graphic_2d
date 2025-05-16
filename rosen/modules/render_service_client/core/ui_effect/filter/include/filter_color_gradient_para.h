/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef UIEFFECT_FILTER_COLOR_GRADIENT_PARA_H
#define UIEFFECT_FILTER_COLOR_GRADIENT_PARA_H

#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"

namespace OHOS {
namespace Rosen {
class ColorGradientPara : public FilterPara {
public:
    ColorGradientPara()
    {
        this->type_ = FilterPara::ParaType::COLOR_GRADIENT;
    }
    ~ColorGradientPara() override = default;

    void SetColors(std::vector<float> colors)
    {
        colors_ = colors;
    }

    const std::vector<float> GetColors() const
    {
        return colors_;
    }

    void SetPositions(std::vector<float> positions)
    {
        positions_ = positions;
    }

    const std::vector<float> GetPositions() const
    {
        return positions_;
    }

    void SetStrengths(std::vector<float> strengths)
    {
        strengths_ = strengths;
    }

    const std::vector<float> GetStrengths() const
    {
        return strengths_;
    }

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

private:
    std::vector<float> colors_;
    std::vector<float> positions_;
    std::vector<float> strengths_;
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_COLOR_GRADIENT_PARA_H
