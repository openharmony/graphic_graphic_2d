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
#include "render/rs_render_ripple_mask.h"
namespace OHOS {
namespace Rosen {
    std::shared_ptr<RSRenderPropertyBase> RSRenderRippleMaskPara::CreateRenderProperty(RSUIFilterType type)
    {
        switch (type) {
            case RSUIFilterType::RIPPLE_MASK_RADIUS : {
                return std::make_shared<RSRenderAnimatableProperty<float>>(0.f, 0);
            }
            case RSUIFilterType::RIPPLE_MASK_WIDTH: {
                return std::make_shared<RSRenderAnimatableProperty<float>>(0.f, 0);
            }
            case RSUIFilterType::RIPPLE_MASK_CENTER: {
                Vector2f value = { 0.f, 0.f };
                return std::make_shared<RSRenderAnimatableProperty<Vector2f>>(value, 0);
            }
            case RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET : {
                return std::make_shared<RSRenderAnimatableProperty<float>>(
                    0.f, 0);
            }
            default:
                ROSEN_LOGE("RSRenderRippleMaskPara::CreateRenderProperty mask nullptr");
                return nullptr;
        }
        return nullptr;
    }

    void RSRenderRippleMaskPara::GetDescription(std::string& out) const
    {
        out += "RSRenderRippleMaskPara";
    }

    bool RSRenderRippleMaskPara::WriteToParcel(Parcel& parcel)
    {
        if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
            !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(type_)) ||
            !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(modifierType_))) {
            ROSEN_LOGE("RSRenderRippleMaskPara::WriteToParcel type Error");
            return false;
        }
        if (!parcel.WriteUint32(properties_.size())) {
            ROSEN_LOGE("RSRenderRippleMaskPara::WriteToParcel size Error");
            return false;
        }
        for (const auto& [key, value] : properties_) {
            if (!RSMarshallingHelper::Marshalling(parcel, key) ||
                !RSMarshallingHelper::Marshalling(parcel, value)) {
                return false;
            }
            ROSEN_LOGD("RSRenderRippleMaskPara::WriteToParcel type %{public}d", static_cast<int>(key));
        }
        return true;
    }

    bool RSRenderRippleMaskPara::ReadFromParcel(Parcel& parcel)
    {
        int16_t type = 0;
        int16_t modifierType = 0;
        if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id_) ||
            !RSMarshallingHelper::Unmarshalling(parcel, type) ||
            !RSMarshallingHelper::Unmarshalling(parcel, modifierType)) {
            ROSEN_LOGE("RSRenderRippleMaskPara::ReadFromParcel type Error");
            return false;
        }
        type_ = static_cast<RSUIFilterType>(type);
        modifierType_ = static_cast<RSModifierType>(modifierType);

        uint32_t size = 0;
        if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
            ROSEN_LOGE("RSRenderRippleMaskPara::ReadFromParcel size Error");
            return false;
        }
        if (size > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
            ROSEN_LOGE("RSRenderRippleMaskPara::ReadFromParcel size large Error");
            return false;
        }
        properties_.clear();
        for (uint32_t i = 0; i < size; ++i) {
            RSUIFilterType key;
            if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
                ROSEN_LOGE("RSRenderRippleMaskPara::ReadFromParcel type %{public}d", static_cast<int>(key));
                return false;
            }
            std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
            if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
                ROSEN_LOGE("RSRenderRippleMaskPara::ReadFromParcel value %{public}d", static_cast<int>(key));
                return false;
            }
            Setter(key, value);
        }
        return true;
    }

    std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderRippleMaskPara::GetLeafRenderProperties()
    {
        std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
        for (auto& [k, v] : properties_) {
            out.emplace_back(v);
        }
        return out;
    }

std::shared_ptr<RSRenderMaskPara> RSRenderRippleMaskPara::LimitedDeepCopy() const
{
    auto newRenderRippleMaskPara = std::make_shared<RSRenderRippleMaskPara>(id_);
    newRenderRippleMaskPara->type_ = type_;
    newRenderRippleMaskPara->modifierType_ = modifierType_;
    for (auto [filterType, property] : properties_) {
        if (property == nullptr) {
            ROSEN_LOGW("RSRenderRippleMaskPara::DeepCopy, filterType: %{public}d, property is nullptr",
                static_cast<int>(filterType));
            continue;
        }
        std::shared_ptr<RSRenderPropertyBase> newProperty = nullptr;
        switch (filterType) {
            case RSUIFilterType::RIPPLE_MASK_RADIUS:
            case RSUIFilterType::RIPPLE_MASK_WIDTH:
            case RSUIFilterType::RIPPLE_MASK_WIDTH_CENTER_OFFSET: {
                auto prop = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(property);
                newProperty = std::make_shared<RSRenderAnimatableProperty<float>>(
                    prop->Get(), prop->GetId());
                break;
            }
            case RSUIFilterType::RIPPLE_MASK_CENTER: {
                auto prop = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(property);
                newProperty = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(
                    prop->Get(), prop->GetId());
                break;
            }
            default: {
                ROSEN_LOGW("RSRenderRippleMaskPara::DeepCopy, ripple mask invalid filterType: %{public}d",
                    static_cast<int>(filterType));
                continue;
            }
        }
        newRenderRippleMaskPara->Setter(filterType, newProperty);
    }
    return newRenderRippleMaskPara;
}
} // namespace Rosen
} // namespace OHOS
