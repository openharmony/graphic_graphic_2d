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

#include "platform/common/rs_log.h"
#include "render/rs_render_dispersion_filter.h"
#include "render/rs_render_mask.h"

namespace OHOS {
namespace Rosen {

constexpr RSUIFilterType DISPERSION_FILTER_TYPE[] = {
    RSUIFilterType::DISPERSION_OPACITY,
    RSUIFilterType::DISPERSION_RED_OFFSET,
    RSUIFilterType::DISPERSION_GREEN_OFFSET,
    RSUIFilterType::DISPERSION_BLUE_OFFSET,
};

void RSRenderDispersionFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderDispersionFilterPara::[maskType_:" + std::to_string(static_cast<int>(maskType_)) + "]";
}

std::shared_ptr<RSRenderPropertyBase> RSRenderDispersionFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::PIXEL_MAP_MASK : {
            return nullptr;
        }
        case RSUIFilterType::DISPERSION_OPACITY : {
            return std::make_shared<RSRenderAnimatableProperty<float>>();
        }
        case RSUIFilterType::DISPERSION_RED_OFFSET :
        case RSUIFilterType::DISPERSION_GREEN_OFFSET :
        case RSUIFilterType::DISPERSION_BLUE_OFFSET : {
            return std::make_shared<RSRenderAnimatableProperty<Vector2f>>();
        }
        default: {
            ROSEN_LOGD("RSRenderDispersionFilterPara::CreateRenderPropert nullptr");
            return nullptr;
        }
    }
}

bool RSRenderDispersionFilterPara::WriteToParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderDispersionFilterPara::WriteToParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::Marshalling(parcel, id_) || !RSMarshallingHelper::Marshalling(parcel, type_) ||
        !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel Error");
        return false;
    }

    auto maskProperty = GetRenderPropert(maskType_);
    if (maskProperty == nullptr) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel empty mask");
        return false;
    }
    auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
    if (mask == nullptr || !RSMarshallingHelper::Marshalling(parcel, maskType_) || !mask->WriteToParcel(parcel)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel mask error");
        return false;
    }

    for (auto type : DISPERSION_FILTER_TYPE) {
        auto property = GetRenderPropert(type);
        if (property == nullptr) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel empty type: %{public}d", static_cast<int>(type));
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, type) ||
            !RSRenderPropertyBase::Marshalling(parcel, property)) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::WriteToParcel write error: %{public}d", static_cast<int>(type));
            return false;
        }
    }

    return true;
}

bool RSRenderDispersionFilterPara::ReadFromParcel(Parcel& parcel)
{
    ROSEN_LOGD("RSRenderDispersionFilterPara::ReadFromParcel %{public}d %{public}d %{public}d",
        static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
    if (!RSMarshallingHelper::Unmarshalling(parcel, id_) || !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel Error");
        return false;
    }

    if (!RSMarshallingHelper::Unmarshalling(parcel, maskType_)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel maskType error");
        return false;
    }
    auto property = CreateRenderProperty(maskType_);
    if (property == nullptr) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel empty mask, maskType: %{public}d",
            static_cast<int>(maskType_));
        return false;
    }
    std::shared_ptr<RSRenderFilterParaBase> maskProperty = std::static_pointer_cast<RSRenderFilterParaBase>(property);
    if (maskProperty == nullptr || !maskProperty->ReadFromParcel(parcel)) {
        ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel mask error");
        return false;
    }
    Setter(maskType_, maskProperty);

    for (auto type : DISPERSION_FILTER_TYPE) {
        RSUIFilterType realType = RSUIFilterType::NONE;
        if (!RSMarshallingHelper::Unmarshalling(parcel, realType)) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel empty type: %{public}d", static_cast<int>(type));
            return false;
        }
        auto property = CreateRenderProperty(type);
        if (type != realType || !RSRenderPropertyBase::Unmarshalling(parcel, property)) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::ReadFromParcel read error: %{public}d", static_cast<int>(type));
            return false;
        }
        Setter(type, property);
    }

    return true;
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderDispersionFilterPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    if (maskType_ != RSUIFilterType::NONE) {
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(GetRenderPropert(maskType_));
        if (mask == nullptr) {
            ROSEN_LOGE("RSRenderDispersionFilterPara::GetLeafRenderProperties mask not found, maskType: %{public}d",
                static_cast<int>(maskType_));
            return {};
        }
        out = mask->GetLeafRenderProperties();
    }
    for (const auto& filterType : DISPERSION_FILTER_TYPE) {
        auto value = GetRenderPropert(filterType);
        if (value == nullptr) {
            continue;
        }
        out.emplace_back(value);
    }
    return out;
}

std::shared_ptr<RSRenderMaskPara> RSRenderDispersionFilterPara::GetRenderMask()
{
    auto property = GetRenderPropert(maskType_);
    if (property == nullptr) {
        return nullptr;
    }
    return std::static_pointer_cast<RSRenderMaskPara>(property);
}

} // namespace Rosen
} // namespace OHOS