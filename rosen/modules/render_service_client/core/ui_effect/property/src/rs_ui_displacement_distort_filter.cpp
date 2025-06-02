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

#include "ui_effect/property/include/rs_ui_displacement_distort_filter.h"
#include "ui_effect/property/include/rs_ui_pixel_map_mask.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_displacement_distort_filter.h"
namespace OHOS {
namespace Rosen {

bool RSUIDispDistortFilterPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != GetType()) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::Equals type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return false;
    }

    return true;
}

void RSUIDispDistortFilterPara::Dump(std::string& out) const
{
    out += "RSUIDispDistortFilterPara: [";
    char buffer[UINT8_MAX] = { 0 };
    auto iter = properties_.find(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
    if (iter != properties_.end()) {
        auto factor = std::static_pointer_cast<RSProperty<Vector2f>>(iter->second);
        if (factor) {
            sprintf_s(buffer, UINT8_MAX, "[factor: %f %f]", factor->Get().x_, factor->Get().y_);
            out.append(buffer);
        } else {
            out += "nullptr";
        }
    }
}

void RSUIDispDistortFilterPara::SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || other->GetType() != GetType()) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::SetProperty type NG %{public}d!",
            other == nullptr ? -1 : static_cast<int>(other->GetType()));
        return;
    }
    auto disortProperty = std::static_pointer_cast<RSUIDispDistortFilterPara>(other);
    if (disortProperty == nullptr) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::SetProperty disortProperty NG!");
        return;
    }
    auto factor = disortProperty->GetRSProperty(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
    auto mask = disortProperty->GetRSProperty(maskType_);
    auto oldMask = GetRSProperty(maskType_);
    if (factor == nullptr || mask == nullptr || oldMask == nullptr) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::SetProperty factor or mask, oldMask null NG!");
        return;
    }
    auto f = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(factor);
    auto m = std::static_pointer_cast<RSUIMaskPara>(mask);
    auto oldM = std::static_pointer_cast<RSUIMaskPara>(oldMask);
    if (f == nullptr || m == nullptr || oldM == nullptr) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::SetProperty factor or mask, oldMask animatable null NG!");
        return;
    }
    SetFactor(f->Get());
    oldM->SetProperty(m);
}

void RSUIDispDistortFilterPara::SetDisplacementDistort(
    const std::shared_ptr<DisplacementDistortPara>& displacementDistort)
{
    if (displacementDistort == nullptr) {
        return;
    }
    // get factor
    auto factorPara = displacementDistort->GetFactor();
    SetFactor(factorPara);
    // get mask
    auto maskPara = displacementDistort->GetMask();
    if (maskPara == nullptr) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::SetDisplacementDistort maskPara nullptr");
        return;
    }
    maskType_ = RSUIMaskPara::ConvertMaskType(maskPara->GetMaskParaType());
    std::shared_ptr<RSUIMaskPara> maskProperty = CreateMask(maskType_);
    if (maskProperty == nullptr) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::SetDisplacementDistort maskProperty nullptr");
        return;
    }
    maskProperty->SetPara(maskPara);
    SetMask(maskProperty);
}

void RSUIDispDistortFilterPara::SetFactor(Vector2f factor)
{
    Setter<RSAnimatableProperty<Vector2f>>(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR, factor);
}

void RSUIDispDistortFilterPara::SetMask(std::shared_ptr<RSUIMaskPara> mask)
{
    if (mask == nullptr) {
        ROSEN_LOGW("RSUIDispDistortFilterPara::SetMask mask nullptr");
        return;
    }
    properties_[mask->GetType()] = mask;
}

std::shared_ptr<RSRenderFilterParaBase> RSUIDispDistortFilterPara::CreateRSRenderFilter()
{
    auto frProperty = std::make_shared<RSRenderDispDistortFilterPara>(id_, maskType_);
    auto factor = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR));
    if (factor == nullptr) {
        ROSEN_LOGE("RSUIDispDistortFilterPara::CreateRSRenderFilter not found factor");
        return nullptr;
    }
    auto factProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
        factor->Get(), factor->GetId(), RSRenderPropertyType::PROPERTY_VECTOR2F);
    frProperty->Setter(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR, factProperty);

    auto mask = std::static_pointer_cast<RSUIMaskPara>(GetRSProperty(maskType_));
    if (mask == nullptr) {
        ROSEN_LOGE("RSUIDispDistortFilterPara::CreateRSRenderFilter not found mask");
        return nullptr;
    }
    auto maskProperty = mask->CreateRSRenderFilter();
    if (maskProperty == nullptr) {
        ROSEN_LOGE("RSUIDispDistortFilterPara::CreateRSRenderFilter not found mask render");
        return nullptr;
    }
    frProperty->Setter(maskType_, maskProperty);
    return frProperty;
}

std::vector<std::shared_ptr<RSPropertyBase>> RSUIDispDistortFilterPara::GetLeafProperties()
{
    auto factor = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(
        GetRSProperty(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR));
    if (factor == nullptr) {
        ROSEN_LOGE("RSUIDispDistortFilterPara::GetLeafProperties not found factor");
        return {};
    }
    auto mask = std::static_pointer_cast<RSUIMaskPara>(GetRSProperty(maskType_));
    if (mask == nullptr) {
        ROSEN_LOGE("RSUIDispDistortFilterPara::GetLeafProperties not found mask");
        return {};
    }
    std::vector<std::shared_ptr<RSPropertyBase>> out = mask->GetLeafProperties();
    out.emplace_back(factor);
    return out;
}

std::shared_ptr<RSUIMaskPara> RSUIDispDistortFilterPara::CreateMask(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::RIPPLE_MASK: {
            return std::make_shared<RSUIRippleMaskPara>();
        }
        case RSUIFilterType::PIXEL_MAP_MASK: {
            return std::make_shared<RSUIPixelMapMaskPara>();
        }
        default:
            return nullptr;
    }
}

} // namespace Rosen
} // namespace OHOS