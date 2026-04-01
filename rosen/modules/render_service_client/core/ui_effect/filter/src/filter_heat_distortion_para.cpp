/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "ui_effect/filter/include/filter_heat_distortion_para.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

HeatDistortionPara::HeatDistortionPara(const HeatDistortionPara& other)
{
    this->type_ = other.type_;
    this->intensity_ = other.intensity_;
    this->riseSpeed_ = other.riseSpeed_;
    this->noiseScale_ = other.noiseScale_;
    this->noiseSpeed_ = other.noiseSpeed_;
    this->riseWeight_ = other.riseWeight_;
}

bool HeatDistortionPara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_));
    if (!isSuccess) {
        RS_LOGE("[ui_effect] HeatDistortionPara Marshalling write type failed");
        return false;
    }

    bool isInvalid = !parcel.WriteFloat(intensity_) ||
        !parcel.WriteFloat(riseSpeed_) ||
        !parcel.WriteFloat(noiseScale_) ||
        !parcel.WriteFloat(noiseSpeed_) ||
        !parcel.WriteFloat(riseWeight_);
    if (isInvalid) {
        RS_LOGE("[ui_effect] HeatDistortionPara Marshalling write para failed");
        return false;
    }
    return true;
}

void HeatDistortionPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::HEAT_DISTORTION, OnUnmarshalling);
}

bool HeatDistortionPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::HEAT_DISTORTION) {
        RS_LOGE("[ui_effect] HeatDistortionPara OnUnmarshalling read type failed");
        return false;
    }

    auto heatDistortionPara = std::make_shared<HeatDistortionPara>();
    float intensity = 1.0f;
    float riseSpeed = 1.0f;
    float noiseScale = 1.0f;
    float noiseSpeed = 0.4f;
    float riseWeight = 0.2f;
    bool isInvalid = !parcel.ReadFloat(intensity) ||
        !parcel.ReadFloat(riseSpeed) ||
        !parcel.ReadFloat(noiseScale) ||
        !parcel.ReadFloat(noiseSpeed) ||
        !parcel.ReadFloat(riseWeight);
    if (isInvalid) {
        RS_LOGE("[ui_effect] HeatDistortionPara OnUnmarshalling read para failed");
        return false;
    }

    heatDistortionPara->SetIntensity(intensity);
    heatDistortionPara->SetRiseSpeed(riseSpeed);
    heatDistortionPara->SetNoiseScale(noiseScale);
    heatDistortionPara->SetNoiseSpeed(noiseSpeed);
    heatDistortionPara->SetRiseWeight(riseWeight);
    val = std::move(heatDistortionPara);
    return true;
}

std::shared_ptr<FilterPara> HeatDistortionPara::Clone() const
{
    return std::make_shared<HeatDistortionPara>(*this);
}

} // namespace Rosen
} // namespace OHOS
