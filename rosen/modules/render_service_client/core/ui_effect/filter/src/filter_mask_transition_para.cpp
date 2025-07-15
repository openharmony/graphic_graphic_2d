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

#include "ui_effect/filter/include/filter_mask_transition_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

MaskTransitionPara::MaskTransitionPara(const MaskTransitionPara& other)
{
    this->type_ = other.type_;
    this->maskPara_ = other.maskPara_ ? other.maskPara_->Clone() : nullptr;
    this->factor_ = other.factor_;
    this->inverse_ = other.inverse_;
}

bool MaskTransitionPara::Marshalling(Parcel& parcel) const
{
    auto isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        maskPara_ && maskPara_->Marshalling(parcel) &&
        parcel.WriteFloat(factor_) && parcel.WriteBool(inverse_);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] MaskTransitionPara Marshalling write para failed");
        return false;
    }
    return true;
}

void MaskTransitionPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::MASK_TRANSITION, OnUnmarshalling);
}

bool MaskTransitionPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::MASK_TRANSITION) {
        RS_LOGE("[ui_effect] MaskTransitionPara OnUnmarshalling read type failed");
        return false;
    }

    auto transitionPara = std::make_shared<MaskTransitionPara>();
    std::shared_ptr<MaskPara> maskPara = nullptr;
    float factor = 1.0f;
    bool inverse = false;
    bool isSuccess = MaskPara::Unmarshalling(parcel, maskPara) && maskPara != nullptr &&
        parcel.ReadFloat(factor) && parcel.ReadBool(inverse);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] MaskTransitionPara OnUnmarshalling read para failed");
        return false;
    }
    transitionPara->SetMask(maskPara);
    transitionPara->SetFactor(factor);
    transitionPara->SetInverse(inverse);
    val = std::move(transitionPara);
    return true;
}

std::shared_ptr<FilterPara> MaskTransitionPara::Clone() const
{
    return std::make_shared<MaskTransitionPara>(*this);
}

} // namespace Rosen
} // namespace OHOS