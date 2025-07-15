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

#include "ui_effect/filter/include/filter_dispersion_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

DispersionPara::DispersionPara(const DispersionPara& other)
{
    this->type_ = other.type_;
    this->maskPara_ = other.maskPara_ ? other.maskPara_->Clone() : nullptr;
    this->opacity_ = other.opacity_;
    this->redOffset_ = other.redOffset_;
    this->greenOffset_ = other.greenOffset_;
    this->blueOffset_ = other.blueOffset_;
}

bool DispersionPara::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(static_cast<uint16_t>(type_)) ||
        !parcel.WriteUint16(static_cast<uint16_t>(type_))) {
        RS_LOGE("[ui_effect] DispersionPara Marshalling write type failed");
        return false;
    }

    bool isInvalidMask = false;
    if (maskPara_ != nullptr) {
        isInvalidMask = !parcel.WriteBool(true) || !maskPara_->Marshalling(parcel);
    } else {
        isInvalidMask = !parcel.WriteBool(false);
    }
    if (isInvalidMask) {
        RS_LOGE("[ui_effect] DispersionPara Marshalling write mask failed");
        return false;
    }
    bool isInvalid = !parcel.WriteFloat(opacity_) || (
        !parcel.WriteFloat(redOffset_.x_) || !parcel.WriteFloat(redOffset_.y_)) || (
        !parcel.WriteFloat(greenOffset_.x_) || !parcel.WriteFloat(greenOffset_.y_)) || (
        !parcel.WriteFloat(blueOffset_.x_) || !parcel.WriteFloat(blueOffset_.y_));
    if (isInvalid) {
        RS_LOGE("[ui_effect] DispersionPara Marshalling write para failed");
        return false;
    }
    return true;
}

void DispersionPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::DISPERSION, OnUnmarshalling);
}

bool DispersionPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::DISPERSION) {
        RS_LOGE("[ui_effect] DispersionPara OnUnmarshalling read type failed");
        return false;
    }

    std::shared_ptr<MaskPara> maskPara = nullptr;
    bool isMaskPara = false;
    bool isInvalidMask = !parcel.ReadBool(isMaskPara) ||
        (isMaskPara && (!MaskPara::Unmarshalling(parcel, maskPara) || maskPara == nullptr));
    if (isInvalidMask) {
        RS_LOGE("[ui_effect] DispersionPara OnUnmarshalling read mask failed");
        return false;
    }
    auto dispersionPara = std::make_shared<DispersionPara>();
    dispersionPara->SetMask(maskPara);
    float opacity = 0.0f;
    Vector2f redOffset;
    Vector2f greenOffset;
    Vector2f blueOffset;
    bool isInvalid = !parcel.ReadFloat(opacity) || (
        !parcel.ReadFloat(redOffset.x_) || !parcel.ReadFloat(redOffset.y_)) || (
        !parcel.ReadFloat(greenOffset.x_) || !parcel.ReadFloat(greenOffset.y_)) || (
        !parcel.ReadFloat(blueOffset.x_) || !parcel.ReadFloat(blueOffset.y_));
    if (isInvalid) {
        RS_LOGE("[ui_effect] DispersionPara OnUnmarshalling read para failed");
        return false;
    }
    dispersionPara->SetOpacity(opacity);
    dispersionPara->SetRedOffset(redOffset);
    dispersionPara->SetGreenOffset(greenOffset);
    dispersionPara->SetBlueOffset(blueOffset);
    val = std::move(dispersionPara);
    return true;
}

std::shared_ptr<FilterPara> DispersionPara::Clone() const
{
    return std::make_shared<DispersionPara>(*this);
}

} // namespace Rosen
} // namespace OHOS