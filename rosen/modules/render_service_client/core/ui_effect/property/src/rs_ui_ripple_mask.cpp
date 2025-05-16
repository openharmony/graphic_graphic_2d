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

#include "platform/common/rs_log.h"
#include "render/rs_render_ripple_mask.h"
#include "ui_effect/property/include/rs_ui_ripple_mask.h"
#include "ui_effect/mask/include/ripple_mask_para.h"

namespace OHOS {
namespace Rosen {
bool RSUIRippleMaskPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::RIPPLE_MASK) {
        ROSEN_LOGW("RSUIRippleMaskPara::Equals type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }
    return true;
}

void RSUIRippleMaskPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::RIPPLE_MASK) {
        ROSEN_LOGW("RSUIRippleMaskPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    auto maskProperty = std::static_pointer_cast<RSUIRippleMaskPara>(other);
    if (maskProperty == nullptr) {
        ROSEN_LOGW("RSUIRippleMaskPara::SetProperty maskProperty is nullptr!");
        return;
    }
    auto radius = maskProperty->GetRSProperty(RSUIFilterType::RIPPLE_MASK_RADIUS);
    auto width = maskProperty->GetRSProperty(RSUIFilterType::RIPPLE_MASK_WIDTH);
    auto center = maskProperty->GetRSProperty(RSUIFilterType::RIPPLE_MASK_CENTER);
    auto widthCenterOffset = maskProperty->GetRSProperty(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET);
    if (radius == nullptr || width == nullptr || center == nullptr || widthCenterOffset == nullptr) {
        ROSEN_LOGW("RSUIRippleMaskPara::SetProperty radius or width, center, widthCenterOffset is nullptr!");
        return;
    }
    auto radiusProp = std::static_pointer_cast<RSAnimatableProperty<float>>(radius);
    auto widthProp = std::static_pointer_cast<RSAnimatableProperty<float>>(width);
    auto centerProp = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(center);
    auto offsetProp = std::static_pointer_cast<RSAnimatableProperty<float>>(widthCenterOffset);
    if (radiusProp == nullptr || widthProp == nullptr || centerProp == nullptr || offsetProp == nullptr) {
        return;
    }
    SetRadius(radiusProp->Get());
    SetWidth(widthProp->Get());
    SetCenter(centerProp->Get());
    SetWidthCenterOffset(offsetProp->Get());
}

void RSUIRippleMaskPara::SetPara(const std::shared_ptr<MaskPara>& para)
{
    if (para->GetMaskParaType() != MaskPara::Type::RIPPLE_MASK) {
        return;
    }

    auto maskPara = std::static_pointer_cast<RippleMaskPara>(para);
    if (!maskPara) {
        return;
    }
    SetRadius(maskPara->GetRadius());
    SetWidth(maskPara->GetWidth());
    SetCenter(maskPara->GetCenter());
    SetWidthCenterOffset(maskPara->GetWidthCenterOffset());
}

void RSUIRippleMaskPara::SetRadius(float radius)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::RIPPLE_MASK_RADIUS, radius);
}

void RSUIRippleMaskPara::SetWidth(float width)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::RIPPLE_MASK_WIDTH, width);
}

void RSUIRippleMaskPara::SetCenter(Vector2f center)
{
    Setter<RSAnimatableProperty<Vector2f>>(RSUIFilterType::RIPPLE_MASK_CENTER, center);
}

void RSUIRippleMaskPara::SetWidthCenterOffset(float widthCenterOffset)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET, widthCenterOffset);
}

std::shared_ptr<RSRenderFilterParaBase> RSUIRippleMaskPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderRippleMaskPara>(id_);
    auto rProperty = GetAnimatRenderProperty<float>(
        RSUIFilterType::RIPPLE_MASK_RADIUS, RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::RIPPLE_MASK_RADIUS, rProperty);
    auto wProperty = GetAnimatRenderProperty<float>(
        RSUIFilterType::RIPPLE_MASK_WIDTH, RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::RIPPLE_MASK_WIDTH, wProperty);
    auto cProperty = GetAnimatRenderProperty<Vector2f>(
        RSUIFilterType::RIPPLE_MASK_CENTER, RSRenderPropertyType::PROPERTY_VECTOR2F);
    frProperty->Setter(RSUIFilterType::RIPPLE_MASK_CENTER, cProperty);
    auto wCenterOffsetProperty = GetAnimatRenderProperty<float>(
        RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET, RSRenderPropertyType::PROPERTY_FLOAT);
    frProperty->Setter(RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET, wCenterOffsetProperty);
    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIRippleMaskPara::GetLeafProperties()
{
    std::vector<std::shared_ptr<RSPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}
} // namespace Rosen
} // namespace OHOS

 