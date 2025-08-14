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

#include "ui_effect/filter/include/filter_direction_light_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

DirectionLightPara::DirectionLightPara(const DirectionLightPara& other)
{
    this->type_ = other.type_;
    this->maskPara_ = other.maskPara_ ? other.maskPara_->Clone() : nullptr;
    this->maskFactor_ = other.maskFactor_;
    this->lightDirection_ = other.lightDirection_;
    this->lightColor_ = other.lightColor_;
    this->lightIntensity_ = other.lightIntensity_;
}

bool DirectionLightPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_));

    if (maskPara_ != nullptr) {
        isSuccess = isSuccess && parcel.WriteBool(true) && maskPara_->Marshalling(parcel) &&
            parcel.WriteFloat(maskFactor_);
    } else {
        isSuccess = isSuccess && parcel.WriteBool(false);
    }

    isSuccess = isSuccess && parcel.WriteFloat(lightDirection_.x_) &&
        parcel.WriteFloat(lightDirection_.y_) && parcel.WriteFloat(lightDirection_.z_);

    isSuccess = isSuccess &&
        parcel.WriteFloat(lightColor_.x_) && parcel.WriteFloat(lightColor_.y_) &&
        parcel.WriteFloat(lightColor_.z_) && parcel.WriteFloat(lightColor_.w_) &&
        parcel.WriteFloat(lightIntensity_);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] DirectionLightPara Marshalling write para failed");
        return false;
    }
    return true;
}

void DirectionLightPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::DIRECTION_LIGHT, OnUnmarshalling);
}

bool DirectionLightPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::DIRECTION_LIGHT) {
        RS_LOGE("[ui_effect] DirectionLightPara OnUnmarshalling read type failed");
        return false;
    }

    std::shared_ptr<MaskPara> maskPara = nullptr;
    float maskFactor = 0.0f;
    Vector3f lightDirection;
    Vector4f lightColor;
    float lightIntensity = 0.0f;
    bool isSuccess = true;
    bool hasMask = false;
    if (!parcel.ReadBool(hasMask)) {
        RS_LOGE("[ui_effect] DirectionLightPara OnUnmarshalling read hasMask failed");
        return false;
    }
    if (hasMask) {
        isSuccess = MaskPara::Unmarshalling(parcel, maskPara) && parcel.ReadFloat(maskFactor);
    }
    isSuccess = isSuccess && parcel.ReadFloat(lightDirection.x_) &&
        parcel.ReadFloat(lightDirection.y_) && parcel.ReadFloat(lightDirection.z_);
    isSuccess = isSuccess &&
        parcel.ReadFloat(lightColor.x_) && parcel.ReadFloat(lightColor.y_) &&
        parcel.ReadFloat(lightColor.z_) && parcel.ReadFloat(lightColor.w_) &&
        parcel.ReadFloat(lightIntensity);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] DirectionLightPara OnUnmarshalling read para failed");
        return false;
    }

    auto directionLightPara = std::make_shared<DirectionLightPara>();
    directionLightPara->SetMask(maskPara);
    directionLightPara->SetMaskFactor(maskFactor);
    directionLightPara->SetLightDirection(lightDirection);
    directionLightPara->SetLightColor(lightColor);
    directionLightPara->SetLightIntensity(lightIntensity);
    val = std::move(directionLightPara);
    return true;
}

std::shared_ptr<FilterPara> DirectionLightPara::Clone() const
{
    return std::make_shared<DirectionLightPara>(*this);
}

} // namespace Rosen
} // namespace OHOS