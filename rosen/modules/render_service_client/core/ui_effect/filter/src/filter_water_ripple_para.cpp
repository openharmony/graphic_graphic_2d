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

#include "ui_effect/filter/include/filter_water_ripple_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

WaterRipplePara::WaterRipplePara(const WaterRipplePara& other)
{
    this->type_ = other.type_;
    this->rippleCenterX_ = other.rippleCenterX_;
    this->rippleCenterY_ = other.rippleCenterY_;
    this->progress_ = other.progress_;
    this->waveCount_ = other.waveCount_;
    this->rippleMode_ = other.rippleMode_;
}

bool WaterRipplePara::Marshalling(Parcel& parcel) const
{
    auto isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteFloat(rippleCenterX_) && parcel.WriteFloat(rippleCenterY_) &&
        parcel.WriteFloat(progress_) && parcel.WriteUint32(waveCount_) && parcel.WriteUint32(rippleMode_);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] WaterRipplePara Marshalling write para failed");
        return false;
    }
    return true;
}

void WaterRipplePara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::WATER_RIPPLE, OnUnmarshalling);
}

bool WaterRipplePara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::WATER_RIPPLE) {
        RS_LOGE("[ui_effect] WaterRipplePara OnUnmarshalling read type failed");
        return false;
    }

    auto waterRipplePara = std::make_shared<WaterRipplePara>();
    float rippleCenterX = 0.0f;
    float rippleCenterY = 0.0f;
    float progress = 0.0f;
    uint32_t waveCount = 0;
    uint32_t rippleMode = 0;
    auto isSuccess = parcel.ReadFloat(rippleCenterX) && parcel.ReadFloat(rippleCenterY) &&
        parcel.ReadFloat(progress) && parcel.ReadUint32(waveCount) && parcel.ReadUint32(rippleMode);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] WaterRipplePara OnUnmarshalling read para failed");
        return false;
    }
    waterRipplePara->SetRippleCenterX(rippleCenterX);
    waterRipplePara->SetRippleCenterY(rippleCenterY);
    waterRipplePara->SetProgress(progress);
    waterRipplePara->SetWaveCount(waveCount);
    waterRipplePara->SetRippleMode(rippleMode);
    val = std::move(waterRipplePara);
    return true;
}

std::shared_ptr<FilterPara> WaterRipplePara::Clone() const
{
    return std::make_shared<WaterRipplePara>(*this);
}

} // namespace Rosen
} // namespace OHOS