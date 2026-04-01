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

#include "ui_effect/filter/include/filter_blur_bubbles_rise_para.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

BlurBubblesRisePara::BlurBubblesRisePara(const BlurBubblesRisePara& other)
{
    this->type_ = other.type_;
    this->blurRadius_ = other.blurRadius_;
    this->mixStrength_ = other.mixStrength_;
    this->invertMask_ = other.invertMask_;
    this->maskChannel_ = other.maskChannel_;
    this->maskScrollSpeed_ = other.maskScrollSpeed_;
}

bool BlurBubblesRisePara::Marshalling(Parcel& parcel) const
{
    bool isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_));
    if (!isSuccess) {
        RS_LOGE("[ui_effect] BlurBubblesRisePara Marshalling write type failed");
        return false;
    }

    bool isInvalid = !parcel.WriteFloat(blurRadius_) ||
        !parcel.WriteFloat(mixStrength_) ||
        !parcel.WriteUint32(invertMask_) ||
        !parcel.WriteUint32(maskChannel_) ||
        !parcel.WriteFloat(maskScrollSpeed_);
    if (isInvalid) {
        RS_LOGE("[ui_effect] BlurBubblesRisePara Marshalling write para failed");
        return false;
    }
    return true;
}

void BlurBubblesRisePara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::BLUR_BUBBLES_RISE, OnUnmarshalling);
}

bool BlurBubblesRisePara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::BLUR_BUBBLES_RISE) {
        RS_LOGE("[ui_effect] BlurBubblesRisePara OnUnmarshalling read type failed");
        return false;
    }

    auto blurBubblesRisePara = std::make_shared<BlurBubblesRisePara>();
    float blurRadius = 3.0f;
    float mixStrength = 1.0f;
    uint32_t invertMask = 0;
    uint32_t maskChannel = 0;
    float maskScrollSpeed = 0.07f;
    bool isInvalid = !parcel.ReadFloat(blurRadius) ||
        !parcel.ReadFloat(mixStrength) ||
        !parcel.ReadUint32(invertMask) ||
        !parcel.ReadUint32(maskChannel) ||
        !parcel.ReadFloat(maskScrollSpeed);
    if (isInvalid) {
        RS_LOGE("[ui_effect] BlurBubblesRisePara OnUnmarshalling read para failed");
        return false;
    }

    blurBubblesRisePara->SetBlurRadius(blurRadius);
    blurBubblesRisePara->SetMixStrength(mixStrength);
    blurBubblesRisePara->SetInvertMask(invertMask);
    blurBubblesRisePara->SetMaskChannel(maskChannel);
    blurBubblesRisePara->SetMaskScrollSpeed(maskScrollSpeed);
    val = std::move(blurBubblesRisePara);
    return true;
}

std::shared_ptr<FilterPara> BlurBubblesRisePara::Clone() const
{
    return std::make_shared<BlurBubblesRisePara>(*this);
}

} // namespace Rosen
} // namespace OHOS
