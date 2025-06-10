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

#include "ui_effect/property/include/rs_ui_dispersion_filter.h"
#include "render/rs_render_dispersion_filter.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool RSUIDispersionFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::DISPERSION) {
        ROSEN_LOGW("RSUIDispersionFilterPara::Equals type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }

    return true;
}

void RSUIDispersionFilterPara::Dump(std::string& out) const
{
    out += "RSUIDispersionFilterPara:[";

    char buffer[UINT8_MAX] = { 0 };
    auto iter = properties_.find(RSUIFilterType::DISPERSION_OPACITY);
    if (iter != properties_.end()) {
        auto opacity = std::static_pointer_cast<RSAnimatableProperty<float>>(iter->second);
        if (opacity != nullptr) {
            sprintf_s(buffer, UINT8_MAX, "opacity:%f", opacity->Get());
            out.append(buffer);
        }
    }

    iter = properties_.find(RSUIFilterType::DISPERSION_RED_OFFSET);
    if (iter != properties_.end()) {
        auto offset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
        if (offset != nullptr) {
            sprintf_s(buffer, UINT8_MAX, " redOffset:[%f, %f]", offset->Get()[0], offset->Get()[1]);
            out.append(buffer);
        }
    }

    iter = properties_.find(RSUIFilterType::DISPERSION_GREEN_OFFSET);
    if (iter != properties_.end()) {
        auto offset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
        if (offset != nullptr) {
            sprintf_s(buffer, UINT8_MAX, " greenOffset:[%f, %f]", offset->Get()[0], offset->Get()[1]);
            out.append(buffer);
        }
    }

    iter = properties_.find(RSUIFilterType::DISPERSION_BLUE_OFFSET);
    if (iter != properties_.end()) {
        auto offset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(iter->second);
        if (offset != nullptr) {
            sprintf_s(buffer, UINT8_MAX, " blueOffset:[%f, %f]", offset->Get()[0], offset->Get()[1]);
            out.append(buffer);
        }
    }

    out += "]";
}

void RSUIDispersionFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != RSUIFilterType::DISPERSION) {
        ROSEN_LOGW("RSUIDispersionFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }

    auto dispersionProperty = std::static_pointer_cast<RSUIDispersionFilterPara>(other);
    if (dispersionProperty == nullptr) {
        ROSEN_LOGW("RSUIDispersionFilterPara::SetProperty other is null NG!");
        return;
    }

    auto mask = dispersionProperty->GetRSProperty(maskType_);
    auto oldMask = GetRSProperty(maskType_);
    auto opacity = dispersionProperty->GetRSProperty(RSUIFilterType::DISPERSION_OPACITY);
    auto redOffset = dispersionProperty->GetRSProperty(RSUIFilterType::DISPERSION_RED_OFFSET);
    auto greenOffset = dispersionProperty->GetRSProperty(RSUIFilterType::DISPERSION_GREEN_OFFSET);
    auto blueOffset = dispersionProperty->GetRSProperty(RSUIFilterType::DISPERSION_BLUE_OFFSET);
    if (mask == nullptr || oldMask == nullptr || opacity == nullptr || redOffset == nullptr || greenOffset == nullptr
        || blueOffset == nullptr) {
        ROSEN_LOGW("RSUIDispersionFilterPara::SetProperty params, mask or oldMask null NG!");
        return;
    }

    auto m = std::static_pointer_cast<RSUIMaskPara>(mask);
    auto oldM = std::static_pointer_cast<RSUIMaskPara>(oldMask);
    auto o = std::static_pointer_cast<RSAnimatableProperty<float>>(opacity);
    auto r = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(redOffset);
    auto g = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(greenOffset);
    auto b = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(blueOffset);
    if (m == nullptr || oldM == nullptr || o == nullptr || r == nullptr || g == nullptr || b == nullptr) {
        ROSEN_LOGW("RSUIDispersionFilterPara::SetProperty params, mask or oldMask animatable null NG!");
        return;
    }

    oldM->SetProperty(m);
    SetOpacity(o->Get());
    SetRedOffset(r->Get());
    SetGreenOffset(g->Get());
    SetBlueOffset(b->Get());
}

void RSUIDispersionFilterPara::SetDispersion(const std::shared_ptr<DispersionPara>& dispersion)
{
    if (dispersion == nullptr) {
        return;
    }

    auto maskPara = dispersion->GetMask();
    if (maskPara == nullptr) {
        ROSEN_LOGW("RSUIDispersionFilterPara::SetDispersion maskPara nullptr");
        return;
    }
    maskType_ = RSUIMaskPara::ConvertMaskType(maskPara->GetMaskParaType());
    std::shared_ptr<RSUIMaskPara> maskProperty = CreateMask(maskType_);
    if (maskProperty == nullptr) {
        ROSEN_LOGW("RSUIDispersionFilterPara::SetDispersion maskType:%{public}d not support NG!",
            static_cast<int>(maskType_));
        return;
    }
    maskProperty->SetPara(maskPara);
    SetMask(maskProperty);

    SetOpacity(dispersion->GetOpacity());
    SetRedOffset(dispersion->GetRedOffset());
    SetGreenOffset(dispersion->GetGreenOffset());
    SetBlueOffset(dispersion->GetBlueOffset());
}

void RSUIDispersionFilterPara::SetMask(const std::shared_ptr<RSUIMaskPara>& mask)
{
    if (mask == nullptr) {
        ROSEN_LOGW("RSUIDispersionFilterPara::SetMask mask nullptr");
        return;
    }
    properties_[mask->GetType()] = mask;
}

void RSUIDispersionFilterPara::SetOpacity(float opacity)
{
    Setter<RSAnimatableProperty<float>>(RSUIFilterType::DISPERSION_OPACITY, opacity);
}

void RSUIDispersionFilterPara::SetRedOffset(const Vector2f& redOffset)
{
    Setter<RSAnimatableProperty<Vector2f>>(RSUIFilterType::DISPERSION_RED_OFFSET, redOffset);
}

void RSUIDispersionFilterPara::SetGreenOffset(const Vector2f& greenOffset)
{
    Setter<RSAnimatableProperty<Vector2f>>(RSUIFilterType::DISPERSION_GREEN_OFFSET, greenOffset);
}

void RSUIDispersionFilterPara::SetBlueOffset(const Vector2f& blueOffset)
{
    Setter<RSAnimatableProperty<Vector2f>>(RSUIFilterType::DISPERSION_BLUE_OFFSET, blueOffset);
}

std::shared_ptr<RSRenderFilterParaBase> RSUIDispersionFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderDispersionFilterPara>(id_, maskType_);
    
    auto mask = std::static_pointer_cast<RSUIMaskPara>(GetRSProperty(maskType_));
    if (mask == nullptr) {
        ROSEN_LOGE("RSUIDispersionFilterPara::CreateRSRenderFilter mask not found");
        return nullptr;
    }
    auto maskProperty = mask->CreateRSRenderFilter();
    if (maskProperty == nullptr) {
        ROSEN_LOGE("RSUIDispersionFilterPara::CreateRSRenderFilter no mask render");
        return nullptr;
    }
    frProperty->Setter(maskType_, maskProperty);

    auto opacity = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::DISPERSION_OPACITY));
    auto redOffset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPERSION_RED_OFFSET));
    auto greenOffset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPERSION_GREEN_OFFSET));
    auto blueOffset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPERSION_BLUE_OFFSET));
    if (opacity == nullptr || redOffset == nullptr || greenOffset == nullptr || blueOffset == nullptr) {
        ROSEN_LOGE("RSUIDispersionFilterPara::CreateRSRenderFilter some params not found");
        return nullptr;
    }

    auto opacityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(
        opacity->Get(), opacity->GetId(), RSPropertyType::FLOAT);
    auto redOffsetProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        redOffset->Get(), redOffset->GetId(), RSPropertyType::VECTOR2F);
    auto greenOffsetProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        greenOffset->Get(), greenOffset->GetId(), RSPropertyType::VECTOR2F);
    auto blueOffsetProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        blueOffset->Get(), blueOffset->GetId(), RSPropertyType::VECTOR2F);

    frProperty->Setter(RSUIFilterType::DISPERSION_OPACITY, opacityProperty);
    frProperty->Setter(RSUIFilterType::DISPERSION_RED_OFFSET, redOffsetProperty);
    frProperty->Setter(RSUIFilterType::DISPERSION_GREEN_OFFSET, greenOffsetProperty);
    frProperty->Setter(RSUIFilterType::DISPERSION_BLUE_OFFSET, blueOffsetProperty);

    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIDispersionFilterPara::GetLeafProperties()
{
    auto mask = std::static_pointer_cast<RSUIMaskPara>(GetRSProperty(maskType_));
    auto opacity = std::static_pointer_cast<RSAnimatableProperty<float>>(
        GetRSProperty(RSUIFilterType::DISPERSION_OPACITY));
    auto redOffset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPERSION_RED_OFFSET));
    auto greenOffset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPERSION_GREEN_OFFSET));
    auto blueOffset = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPERSION_BLUE_OFFSET));

    if (mask == nullptr || opacity == nullptr || redOffset == nullptr || greenOffset == nullptr ||
        blueOffset == nullptr) {
        ROSEN_LOGE("RSUIDispersionFilterPara::GetLeafProperties some params not found");
        return {};
    }
    std::vector<std::shared_ptr<RSPropertyBase>> out = mask->GetLeafProperties();
    out.emplace_back(opacity);
    out.emplace_back(redOffset);
    out.emplace_back(greenOffset);
    out.emplace_back(blueOffset);

    return out;
}

std::shared_ptr<RSUIMaskPara> RSUIDispersionFilterPara::CreateMask(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::PIXEL_MAP_MASK: {
            return nullptr;
        }
        default:
            return nullptr;
    }
}
} // namespace Rosen
} // namespace OHOS