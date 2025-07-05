/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ui_effect/filter/include/filter_displacement_distort_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

DisplacementDistortPara::DisplacementDistortPara(const DisplacementDistortPara& other)
{
    this->type_ = other.type_;
    this->maskPara_ = other.maskPara_ ? other.maskPara_->Clone() : nullptr;
    this->factor_ = other.factor_;
}

bool DisplacementDistortPara::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(static_cast<uint16_t>(type_)) ||
        !parcel.WriteUint16(static_cast<uint16_t>(type_))) {
        RS_LOGE("[ui_effect] DisplacementDistortPara Marshalling write type failed");
        return false;
    }

    bool isInvalidMask = false;
    if (maskPara_ != nullptr) {
        isInvalidMask = !parcel.WriteBool(true) || !maskPara_->Marshalling(parcel);
    } else {
        isInvalidMask = !parcel.WriteBool(false);
    }
    if (isInvalidMask) {
        RS_LOGE("[ui_effect] DisplacementDistortPara Marshalling write mask failed");
        return false;
    }
    bool isInvalid = !parcel.WriteFloat(factor_.x_) || !parcel.WriteFloat(factor_.y_);
    if (isInvalid) {
        RS_LOGE("[ui_effect] DisplacementDistortPara Marshalling write factor failed");
        return false;
    }
    return true;
}

void DisplacementDistortPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::DISPLACEMENT_DISTORT, OnUnmarshalling);
}

bool DisplacementDistortPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::DISPLACEMENT_DISTORT) {
        RS_LOGE("[ui_effect] DisplacementDistortPara OnUnmarshalling read type failed");
        return false;
    }

    std::shared_ptr<MaskPara> maskPara = nullptr;
    bool isMasPara = false;
    bool isInvalidMask = !parcel.ReadBool(isMasPara) ||
        (isMasPara && (!MaskPara::Unmarshalling(parcel, maskPara) || maskPara == nullptr));
    if (isInvalidMask) {
        RS_LOGE("[ui_effect] DisplacementDistortPara OnUnmarshalling read mask failed");
        return false;
    }
    auto displacementDistortPara = std::make_shared<DisplacementDistortPara>();
    displacementDistortPara->SetMask(maskPara);
    Vector2f factor;
    if (!parcel.ReadFloat(factor.x_) || !parcel.ReadFloat(factor.y_)) {
        RS_LOGE("[ui_effect] DisplacementDistortPara OnUnmarshalling read factor failed");
        return false;
    }
    displacementDistortPara->SetFactor(factor);
    val = std::move(displacementDistortPara);
    return true;
}

std::shared_ptr<FilterPara> DisplacementDistortPara::Clone() const
{
    return std::make_shared<DisplacementDistortPara>(*this);
}

} // namespace Rosen
} // namespace OHOS