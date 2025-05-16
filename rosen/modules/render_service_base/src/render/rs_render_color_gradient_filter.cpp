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

#include "platform/common/rs_log.h"
#include "render/rs_render_ripple_mask.h"

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
    if (size > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) { return false; }

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
        std::shared_ptr<RSRenderFilterParaBase> maskProperty = CreateRenderProperty(maskType_);
        if (maskProperty == nullptr || !maskProperty->ReadFromParcel(parcel)) {
            ROSEN_LOGE("RSRenderColorGradientFilterPara::ReadFromParcel mask error");
            return false;
        }
        Setter(maskType_, maskProperty);
    }

    return true;
}

std::shared_ptr<RSRenderMaskPara> RSRenderColorGradientFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::RIPPLE_MASK : {
            return std::make_shared<RSRenderRippleMaskPara>(0);
        }
        default: {
            ROSEN_LOGD("RSRenderColorGradientFilterPara::CreateRenderProperty RSUIFilterType nullptr");
            return nullptr;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderColorGradientFilterPara::GetLeafRenderProperties()
{
    auto colorsProperty = GetRenderPropert(RSUIFilterType::COLOR_GRADIENT_COLOR);
    if (colorsProperty == nullptr) {
        ROSEN_LOGE("RSRenderColorGradientFilterPara::GetLeafRenderProperties empty factor");
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

} // namespace Rosen
} // namespace OHOS
