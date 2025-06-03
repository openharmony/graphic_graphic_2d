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

#include "render/rs_render_color_gradient_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_ripple_mask.h"
#include "render/rs_shader_mask.h"

namespace OHOS {
namespace Rosen {
void RSRenderColorGradientFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderColorGradientFilterPara";
}

bool RSRenderColorGradientFilterPara::WriteToParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Marshalling(parcel, id_) || !RSMarshallingHelper::Marshalling(parcel, type_) ||
        !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel type Error");
        return false;
    }

    if (!parcel.WriteUint32(properties_.size())) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel properties_.size = 0");
        return false;
    }

    auto colorsProperty = GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_COLOR);
    auto positionsProperty = GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_POSITION);
    auto strengthProperty = GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_STRENGTH);
    if (colorsProperty == nullptr || positionsProperty == nullptr || strengthProperty == nullptr) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel empty");
        return false;
    }

    if (!RSMarshallingHelper::Marshalling(parcel, RSUIFilterType::COLOR_GRADIENT_COLOR) ||
        !RSRenderPropertyBase::Marshalling(parcel, colorsProperty)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel color error");
        return false;
    }

    if (!RSMarshallingHelper::Marshalling(parcel, RSUIFilterType::COLOR_GRADIENT_POSITION) ||
        !RSRenderPropertyBase::Marshalling(parcel, positionsProperty)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel positions error");
        return false;
    }

    if (!RSMarshallingHelper::Marshalling(parcel, RSUIFilterType::COLOR_GRADIENT_STRENGTH) ||
        !RSRenderPropertyBase::Marshalling(parcel, strengthProperty)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel strength error");
        return false;
    }

    if (!RSMarshallingHelper::Marshalling(parcel, maskType_)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel maskType_ error");
        return false;
    }
    if (maskType_ != RSUIFilterType::NONE) {
        auto maskProperty = GetRenderPropert(maskType_);
        if (maskProperty == nullptr) {
            ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel empty mask");
            return false;
        }
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
        if (mask == nullptr || !mask->WriteToParcel(parcel)) {
            ROSEN_LOGE("RSRenderColorGradientFilterPara::WriteToParcel mask error");
            return false;
        }
    }

    return true;
}

bool RSRenderColorGradientFilterPara::ReadFromParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Unmarshalling(parcel, id_) || !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
        return false;
    }

    uint32_t size = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) { return false; }

    RSUIFilterType colorsType = RSUIFilterType::NONE;
    if (!RSMarshallingHelper::Unmarshalling(parcel, colorsType)) { return false; }
    std::shared_ptr<RSRenderPropertyBase> color = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>();
    if (colorsType != RSUIFilterType::COLOR_GRADIENT_COLOR ||
        !RSRenderPropertyBase::Unmarshalling(parcel, color)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::ReadFromParcel colors error");
        return false;
    }
    Setter(RSUIFilterType::COLOR_GRADIENT_COLOR, color);

    RSUIFilterType positionsType = RSUIFilterType::NONE;
    if (!RSMarshallingHelper::Unmarshalling(parcel, positionsType)) { return false; }
    std::shared_ptr<RSRenderPropertyBase> position = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>();
    if (positionsType != RSUIFilterType::COLOR_GRADIENT_POSITION ||
        !RSRenderPropertyBase::Unmarshalling(parcel, position)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::ReadFromParcel positions error");
        return false;
    }
    Setter(RSUIFilterType::COLOR_GRADIENT_POSITION, position);

    RSUIFilterType strengthType = RSUIFilterType::NONE;
    if (!RSMarshallingHelper::Unmarshalling(parcel, strengthType)) { return false; }
    std::shared_ptr<RSRenderPropertyBase> strength = std::make_shared<RSRenderAnimatableProperty<std::vector<float>>>();
    if (strengthType != RSUIFilterType::COLOR_GRADIENT_STRENGTH ||
        !RSRenderPropertyBase::Unmarshalling(parcel, strength)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::ReadFromParcel strength error");
        return false;
    }
    Setter(RSUIFilterType::COLOR_GRADIENT_STRENGTH, strength);

    if (!RSMarshallingHelper::Unmarshalling(parcel, maskType_)) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::ReadFromParcel maskType error");
        return false;
    }
    if (maskType_ != RSUIFilterType::NONE) {
        std::shared_ptr<RSRenderFilterParaBase> maskProperty = CreateMaskRenderProperty(maskType_);
        if (maskProperty == nullptr || !maskProperty->ReadFromParcel(parcel)) {
            ROSEN_LOGE("RSRenderColorGradientFilterPara::ReadFromParcel mask error");
            return false;
        }
        Setter(maskType_, maskProperty);
    }

    return true;
}

std::shared_ptr<RSRenderMaskPara> RSRenderColorGradientFilterPara::CreateMaskRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::RIPPLE_MASK : {
            return std::make_shared<RSRenderRippleMaskPara>(0);
        }
        default: {
            ROSEN_LOGD("RSRenderColorGradientFilterPara::CreateMaskRenderProperty RSUIFilterType nullptr");
            return nullptr;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderColorGradientFilterPara::GetLeafRenderProperties()
{
    auto colorsProperty = GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_COLOR);
    if (colorsProperty == nullptr) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::GetLeafRenderProperties empty colors");
        return {};
    }

    auto positionsProperty = GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_POSITION);
    if (positionsProperty == nullptr) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::GetLeafRenderProperties empty positions");
        return {};
    }

    auto strengthProperty = GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_STRENGTH);
    if (strengthProperty == nullptr) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::GetLeafRenderProperties empty strength");
        return {};
    }

    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    out.push_back(colorsProperty);
    out.push_back(positionsProperty);
    out.push_back(strengthProperty);

    if (maskType_ != RSUIFilterType::NONE) {
        auto maskProperty = GetRenderPropert(maskType_);
        if (maskProperty == nullptr) {
            ROSEN_LOGE("RSRenderColorGradientFilterPara::GetLeafRenderProperties empty mask");
            return {};
        }
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
        if (mask == nullptr) {
            ROSEN_LOGE("RSRenderColorGradientFilterPara::GetLeafRenderProperties empty mask property");
            return {};
        }

        std::vector<std::shared_ptr<RSRenderPropertyBase>> temp = mask->GetLeafRenderProperties();
        out.insert(out.end(), temp.begin(), temp.end());
    }

    return out;
}

std::shared_ptr<RSRenderMaskPara> RSRenderColorGradientFilterPara::GetRenderMask()
{
    auto property = GetRenderPropert(maskType_);
    if (property == nullptr) {
        return nullptr;
    }
    return std::static_pointer_cast<RSRenderMaskPara>(property);
}

bool RSRenderColorGradientFilterPara::ParseFilterValues()
{
    auto colorProperty = std::static_pointer_cast<RSRenderProperty<std::vector<float>>>(
        GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_COLOR));
    auto positionProperty = std::static_pointer_cast<RSRenderProperty<std::vector<float>>>(
        GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_POSITION));
    auto strengthProperty = std::static_pointer_cast<RSRenderProperty<std::vector<float>>>(
        GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_STRENGTH));
    auto maskProperty = std::static_pointer_cast<RSRenderMaskPara>(GetRenderPropert(maskType_));
    if (!colorProperty || !positionProperty || !strengthProperty) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::ParseFilterValues GetRenderPropert has some nullptr.");
        return false;
    }
    colors_ = colorProperty->Get();
    positions_ = positionProperty->Get();
    strengths_ = strengthProperty->Get();
    mask_ = maskProperty ? std::make_shared<RSShaderMask>(maskProperty) : nullptr;
#ifndef ENABLE_M133_SKIA
    const auto hashFunc = SkOpts::hash;
#else
    const auto hashFunc = SkChecksum::Hash32;
#endif
    hash_ = hashFunc(colors_.data(), colors_.size() * sizeof(float), hash_);
    hash_ = hashFunc(positions_.data(), positions_.size() * sizeof(float), hash_);
    hash_ = hashFunc(strengths_.data(), strengths_.size() * sizeof(float), hash_);
    if (mask_) {
        auto maskHash = mask_->Hash();
        hash_ = hashFunc(&maskHash, sizeof(maskHash), hash_);
    }
    return true;
}

void RSRenderColorGradientFilterPara::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    if (visualEffectContainer == nullptr) {
        return;
    }
    auto colorGradientFilter = std::make_shared<Drawing::GEVisualEffect>("COLOR_GRADIENT",
        Drawing::DrawingPaintType::BRUSH);
    colorGradientFilter->SetParam("COLOR", colors_);
    colorGradientFilter->SetParam("POSITION", positions_);
    colorGradientFilter->SetParam("STRENGTH", strengths_);
    if (mask_) {
        colorGradientFilter->SetParam("MASK", mask_->GenerateGEShaderMask());
    }

    visualEffectContainer->AddToChainedFilter(colorGradientFilter);
}

const std::vector<float> RSRenderColorGradientFilterPara::GetColors() const
{
    return colors_;
}

const std::vector<float> RSRenderColorGradientFilterPara::GetPositions() const
{
    return positions_;
}

const std::vector<float> RSRenderColorGradientFilterPara::GetStrengths() const
{
    return strengths_;
}

const std::shared_ptr<RSShaderMask>& RSRenderColorGradientFilterPara::GetMask() const
{
    return mask_;
}

} // namespace Rosen
} // namespace OHOS
