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
#ifndef UIEFFECT_EFFECT_COLOR_GRADIENT_EFFECT_PARA_H
#define UIEFFECT_EFFECT_COLOR_GRADIENT_EFFECT_PARA_H

#include "visual_effect_para.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "ui_effect/mask/include/mask_para.h"

namespace OHOS {
namespace Rosen {
class ColorGradientEffectPara : public VisualEffectPara {
public:
    ColorGradientEffectPara()
    {
        this->type_ = VisualEffectPara::ParaType::COLOR_GRADIENT_EFFECT;
    }
    ~ColorGradientEffectPara() override = default;

    void SetColors(const std::vector<Vector4f>& colors)
    {
        colors_ = colors;
    }

    const std::vector<Vector4f>& GetColors() const
    {
        return colors_;
    }

    void SetPositions(const std::vector<Vector2f>& positions)
    {
        positions_ = positions;
    }

    const std::vector<Vector2f>& GetPositions() const
    {
        return positions_;
    }

    void SetStrengths(const std::vector<float>& strengths)
    {
        strengths_ = strengths;
    }

    const std::vector<float>& GetStrengths() const
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
    std::vector<Vector4f> colors_;
    std::vector<Vector2f> positions_;
    std::vector<float> strengths_;
    std::shared_ptr<MaskPara> maskPara_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_COLOR_GRADIENT_EFFECT_PARA_H