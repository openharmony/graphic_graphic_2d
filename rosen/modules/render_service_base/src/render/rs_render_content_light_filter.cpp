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
#include "render/rs_render_content_light_filter.h"

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void RSRenderContentLightFilterPara::GetDescription(std::string& out) const
{
    out += "RSRenderContentLightFilterPara";
}

bool RSRenderContentLightFilterPara::WriteToParcel(Parcel& parcel)
{
    if (!RSMarshallingHelper::Marshalling(parcel, id_) ||
        !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(type_)) ||
        !RSMarshallingHelper::Marshalling(parcel, static_cast<int16_t>(modifierType_))) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::WriteToParcel type Error");
        return false;
    }
    if (properties_.size() > static_cast<size_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::ReadFromParcel size large Error");
        return false;
    }
    if (!parcel.WriteUint32(properties_.size())) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::WriteToParcel size Error");
        return false;
    }
    for (const auto& [key, value] : properties_) {
        if (!RSMarshallingHelper::Marshalling(parcel, key) ||
            !RSMarshallingHelper::Marshalling(parcel, value)) {
            return false;
        }
        ROSEN_LOGD("RSRenderContentLightFilterPara::WriteToParcel type %{public}d", static_cast<int>(key));
    }
    return true;
}

bool RSRenderContentLightFilterPara::ReadFromParcel(Parcel& parcel)
{
    int16_t type = 0;
    int16_t modifierType = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, id_) ||
        !RSMarshallingHelper::Unmarshalling(parcel, type) ||
        !RSMarshallingHelper::Unmarshalling(parcel, modifierType)) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::ReadFromParcel type Error");
        return false;
    }
    type_ = static_cast<RSUIFilterType>(type);
    modifierType_ = static_cast<RSModifierType>(modifierType);

    uint32_t size = 0;
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::ReadFromParcel size Error");
        return false;
    }
    if (size > static_cast<uint32_t>(RSMarshallingHelper::UNMARSHALLING_MAX_VECTOR_SIZE)) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::ReadFromParcel size large Error");
        return false;
    }
    properties_.clear();
    for (uint32_t i = 0; i < size; ++i) {
        RSUIFilterType key;
        if (!RSMarshallingHelper::Unmarshalling(parcel, key)) {
            ROSEN_LOGE("RSRenderContentLightFilterPara::ReadFromParcel type %{public}d", static_cast<int>(key));
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> value = CreateRenderProperty(key);
        if (!RSMarshallingHelper::Unmarshalling(parcel, value)) {
            ROSEN_LOGE("RSRenderContentLightFilterPara::ReadFromParcel value %{public}d", static_cast<int>(key));
            return false;
        }
        Setter(key, value);
    }
    return true;
}

std::shared_ptr<RSRenderPropertyBase> RSRenderContentLightFilterPara::CreateRenderProperty(RSUIFilterType type)
{
    switch (type) {
        case RSUIFilterType::LIGHT_POSITION : {
            return std::make_shared<RSRenderAnimatableProperty<Vector3f>>(Vector3f(0.f, 0.f, 0.f), 0);
        }
        case RSUIFilterType::LIGHT_COLOR : {
            return std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f, 0.f, 0.f, 0.f), 0);
        }
        case RSUIFilterType::LIGHT_INTENSITY : {
            return std::make_shared<RSRenderAnimatableProperty<float>>(0, 0);
        }
        default: {
            ROSEN_LOGD("RSRenderContentLightFilterPara::CreateRenderProperty is nullptr");
            return std::shared_ptr<RSRenderPropertyBase>();
        }
    }
    return std::shared_ptr<RSRenderPropertyBase>();
}

std::vector<std::shared_ptr<RSRenderPropertyBase>> RSRenderContentLightFilterPara::GetLeafRenderProperties()
{
    std::vector<std::shared_ptr<RSRenderPropertyBase>> out;
    for (auto& [k, v] : properties_) {
        out.emplace_back(v);
    }
    return out;
}

bool RSRenderContentLightFilterPara::ParseFilterValues()
{
    std::shared_ptr<RSRenderAnimatableProperty<Vector3f>> lightPositionProperty =
        std::static_pointer_cast<RSRenderAnimatableProperty<Vector3f>>(
            GetRenderProperty(RSUIFilterType::LIGHT_POSITION));
    if (lightPositionProperty == nullptr) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::ParseFilterValues lightPositionProperty nullptr");
    return false;
    }
    lightPosition_ = lightPositionProperty->Get();
    hash_ = SkOpts::hash(&lightPosition_, sizeof(lightPosition_), hash_);

    std::shared_ptr<RSRenderAnimatableProperty<Vector4f>> lightColorProperty =
        std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(GetRenderProperty(RSUIFilterType::LIGHT_COLOR));
    if (lightColorProperty == nullptr) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::ParseFilterValues lightColorProperty nullptr");
    return false;
    }
    lightColor_ = lightColorProperty->Get();
    hash_ = SkOpts::hash(&lightColor_, sizeof(lightColor_), hash_);

    std::shared_ptr<RSRenderAnimatableProperty<float>> lightIntensityProperty =
        std::static_pointer_cast<RSRenderAnimatableProperty<float>>(GetRenderProperty(RSUIFilterType::LIGHT_INTENSITY));
    if (lightIntensityProperty == nullptr) {
        ROSEN_LOGE("RSRenderContentLightFilterPara::ParseFilterValues lightIntensityProperty nullptr");
    return false;
    }
    lightIntensity_ = lightIntensityProperty->Get();
    hash_ = SkOpts::hash(&lightIntensity_, sizeof(lightIntensity_), hash_);
    return true;
}

const Vector3f& RSRenderContentLightFilterPara::GetLightPosition() const
{
    return lightPosition_;
}

const Vector4f& RSRenderContentLightFilterPara::GetLightColor() const
{
    return lightColor_;
}

float RSRenderContentLightFilterPara::GetLightIntensity() const
{
    return lightIntensity_;
}

const Vector3f& RSRenderContentLightFilterPara::GetRotationAngel() const
{
    return rotationAngel_;
}

void RSRenderContentLightFilterPara::GenerateGEVisualEffect(
    std::shared_ptr<Drawing::GEVisualEffectContainer> visualEffectContainer)
{
    auto contentLightFilter = std::make_shared<Drawing::GEVisualEffect>("CONTENT_LIGHT",
        Drawing::DrawingPaintType::BRUSH);

    contentLightFilter->SetParam("CONTENT_LIGHT_POSITION", lightPosition_);
    contentLightFilter->SetParam("CONTENT_LIGHT_COLOR", lightColor_);
    contentLightFilter->SetParam("CONTENT_LIGHT_INTENSITY", lightIntensity_);
    contentLightFilter->SetParam("CONTENT_LIGHT_ROTATION_ANGLE", rotationAngel_);
    visualEffectContainer->AddToChainedFilter(contentLightFilter);
}

} // namespace Rosen
} // namespace OHOS