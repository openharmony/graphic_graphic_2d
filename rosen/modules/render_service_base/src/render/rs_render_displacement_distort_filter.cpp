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
#include "render/rs_render_displacement_distort_filter.h"

namespace OHOS {
namespace Rosen {
    void RSRenderDispDistortFilterPara::GetDescription(std::string& out) const
    {
        out += "RSRenderDispDistortFilterPara";
    }

    bool RSRenderDispDistortFilterPara::WriteToParcel(Parcel& parcel)
    {
        ROSEN_LOGD("RSRenderDispDistortFilterPara::WriteToParcel %{public}d %{public}d %{public}d",
            static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
        if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
            !RSMarshallingHelper::Marshalling(parcel, type_) ||
            !RSMarshallingHelper::Marshalling(parcel, modifierType_)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel type Error");
            return false;
        }
        auto factProperty = GetRenderPropert(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
        if (factProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel empty factor");
            return false;
        }
        if (!RSMarshallingHelper::Marshalling(parcel, RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR) ||
            !RSRenderPropertyBase::Marshalling(parcel, factProperty)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel factor error");
            return false;
        }
        auto maskProperty = GetRenderPropert(maskType_);
        if (maskProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel empty mask");
            return false;
        }
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
        if (mask == nullptr ||
            !RSMarshallingHelper::Marshalling(parcel, maskType_) ||
            !mask->WriteToParcel(parcel)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::WriteToParcel mask error");
            return false;
        }
        return true;
    }

    bool RSRenderDispDistortFilterPara::ReadFromParcel(Parcel& parcel)
    {
        ROSEN_LOGD("RSRenderDispDistortFilterPara::ReadFromParcel %{public}d %{public}d %{public}d",
            static_cast<int>(id_), static_cast<int>(type_), static_cast<int>(modifierType_));
        if (!RSMarshallingHelper::Unmarshalling(parcel, id_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, type_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, modifierType_)) {
            return false;
        }
        RSUIFilterType factType = RSUIFilterType::NONE;
        if (!RSMarshallingHelper::Unmarshalling(parcel, factType)) {
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> factProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>();
        if (RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR != factType ||
            !RSRenderPropertyBase::Unmarshalling(parcel, factProperty)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::ReadFromParcel factor error");
            return false;
        }
        Setter(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR, factProperty);
        if (!RSMarshallingHelper::Unmarshalling(parcel, maskType_)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::ReadFromParcel maskType error");
            return false;
        }
        std::shared_ptr<RSRenderFilterParaBase> maskProperty = CreateRenderPropert(maskType_);
        if (maskProperty == nullptr || !maskProperty->ReadFromParcel(parcel)) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::ReadFromParcel mask error");
            return false;
        }
        Setter(maskType_, maskProperty);
        return true;
    }

    std::shared_ptr<RSRenderMaskPara> RSRenderDispDistortFilterPara::CreateRenderPropert(RSUIFilterType type)
    {
        switch (type) {
            case RSUIFilterType::RIPPLE_MASK : {
                return std::make_shared<RSRenderRippleMaskPara>(0);
            }
            default: {
                ROSEN_LOGD("RSRenderDispDistortFilterPara::CreateRenderPropert mask nullptr");
                return nullptr;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderDispDistortFilterPara::GetLeafRenderProperties()
    {
        auto factProperty = GetRenderPropert(RSUIFilterType::DISPLACEMENT_DISTORT_FACTOR);
        if (factProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::GetLeafRenderProperties empty factor");
            return {};
        }
        auto maskProperty = GetRenderPropert(maskType_);
        if (maskProperty == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::GetLeafRenderProperties empty mask");
            return {};
        }
        auto mask = std::static_pointer_cast<RSRenderMaskPara>(maskProperty);
        if (mask == nullptr) {
            ROSEN_LOGE("RSRenderDispDistortFilterPara::GetLeafRenderProperties empty mask property");
            return {};
        }
        std::vector<std::shared_ptr<RSRenderPropertyBase>> out = mask->GetLeafRenderProperties();
        out.push_back(factProperty);
        return out;
    }
} // namespace Rosen
} // namespace OHOS