/*
 * Copyright (centerProp) 2025 Huawei Device Co., Ltd.
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

#include "ui_effect/property/include/rs_ui_radial_gradient_mask.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_radial_gradient_mask.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"

namespace OHOS {
namespace Rosen {
bool RSUIRadialGradientMaskPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::RADIAL_GRADIENT_MASK) {
        ROSEN_LOGW("RSUIRadialGradientMaskPara::Equals type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }
    return true;
}

void RSUIRadialGradientMaskPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::RADIAL_GRADIENT_MASK) {
        ROSEN_LOGW("RSUIRadialGradientMaskPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    auto maskProperty = std::static_pointer_cast<RSUIRadialGradientMaskPara>(other);
    if (maskProperty == nullptr) {
        ROSEN_LOGW("RSUIRadialGradientMaskPara::SetProperty maskProperty is nullptr!");
        return;
    }
    auto radiusX = maskProperty->GetRSProperty(RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSX);
    auto radiusY = maskProperty->GetRSProperty(RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSY);
    auto center = maskProperty->GetRSProperty(RSUIFilterType::RADIAL_GRADIENT_MASK_CENTER);
    auto colors = maskProperty->GetRSProperty(RSUIFilterType::RADIAL_GRADIENT_MASK_COLORS);
    auto positions = maskProperty->GetRSProperty(RSUIFilterType::RADIAL_GRADIENT_MASK_POSITIONS);
    if (radiusX == nullptr || radiusY == nullptr || center == nullptr || colors == nullptr || positions == nullptr) {
        ROSEN_LOGW("RSUIRadialGradientMaskPara::SetProperty para is nullptr!");
        return;
    }
    auto radiusXProp = std::static_pointer_cast<RSAnimatableProperty<float>>(radiusX);
    auto radiusYProp = std::static_pointer_cast<RSAnimatableProperty<float>>(radiusY);
    auto centerProp = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(center);
    auto colorsProp = std::static_pointer_cast<RSAnimatableProperty<std::vector<float>>>(colors);
    auto positionsProp = std::static_pointer_cast<RSAnimatableProperty<std::vector<float>>>(positions);
    if (radiusXProp == nullptr || radiusYProp == nullptr || centerProp == nullptr ||
        colorsProp == nullptr || positionsProp == nullptr) {
        return;
    }
    SetRadius(radiusXProp->Get(), radiusYProp->Get());
    SetCenter(centerProp->Get());
    SetValues(colorsProp->Get(), positionsProp->Get());
}

void RSUIRadialGradientMaskPara::SetPara(const std::shared_ptr<MaskPara>& para)
{
    if (para == nullptr || para->GetMaskParaType() != MaskPara::Type::RADIAL_GRADIENT_MASK) {
        return;
    }

    auto maskPara = std::static_pointer_cast<RadialGradientMaskPara>(para);
    if (!maskPara) {
        return;
    }
    SetRadius(maskPara->GetRadiusX(), maskPara->GetRadiusY());
    SetCenter(maskPara->GetCenter());
    SetValues(maskPara->GetColors(), maskPara->GetPositions());
}

void RSUIRadialGradientMaskPara::SetRadius(const float radiusX, const float radiusY)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSX, radiusX);
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSY, radiusY);
}

void RSUIRadialGradientMaskPara::SetCenter(const Vector2f center)
{
    Setter<RSAnimatableProperty<Vector2f>>(RSUIFilterType::RADIAL_GRADIENT_MASK_CENTER, center);
}

void RSUIRadialGradientMaskPara::SetValues(const std::vector<float> colors, const std::vector<float> positions)
{
    Setter<RSAnimatableProperty<std::vector<float>>>(RSUIFilterType::RADIAL_GRADIENT_MASK_COLORS, colors);
    Setter<RSAnimatableProperty<std::vector<float>>>(RSUIFilterType::RADIAL_GRADIENT_MASK_POSITIONS, positions);
}

std::shared_ptr<RSRenderFilterParaBase> RSUIRadialGradientMaskPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderRadialGradientMaskPara>(id_);
    auto xProperty = GetAnimatRenderProperty<float>(
        RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSX);
    frProperty->Setter(RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSX, xProperty);
    auto yProperty = GetAnimatRenderProperty<float>(
        RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSY);
    frProperty->Setter(RSUIFilterType::RADIAL_GRADIENT_MASK_RADIUSY, yProperty);
    auto cProperty = GetAnimatRenderProperty<Vector2f>(
        RSUIFilterType::RADIAL_GRADIENT_MASK_CENTER);
    frProperty->Setter(RSUIFilterType::RADIAL_GRADIENT_MASK_CENTER, cProperty);
    auto colorsProperty = GetAnimatRenderProperty<std::vector<float>>(
        RSUIFilterType::RADIAL_GRADIENT_MASK_COLORS);
    frProperty->Setter(RSUIFilterType::RADIAL_GRADIENT_MASK_COLORS, colorsProperty);
    auto positionsProperty = GetAnimatRenderProperty<std::vector<float>>(
        RSUIFilterType::RADIAL_GRADIENT_MASK_POSITIONS);
    frProperty->Setter(RSUIFilterType::RADIAL_GRADIENT_MASK_POSITIONS, positionsProperty);
    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIRadialGradientMaskPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS
