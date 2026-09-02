/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ui_effect/mask/include/sweep_refraction_mask_para.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

SweepRefractionMaskPara::SweepRefractionMaskPara(const SweepRefractionMaskPara& other)
{
    this->type_ = other.type_;
    this->maskRadius_ = other.maskRadius_;
    this->edgeThickness_ = other.edgeThickness_;
    this->refractAmount_ = other.refractAmount_;
    this->rippleWidth_ = other.rippleWidth_;
    this->sweepOffset_ = other.sweepOffset_;
    this->chromaDelta_ = other.chromaDelta_;
    this->shapeType_ = other.shapeType_;
    this->cornerRadius_ = other.cornerRadius_;
    this->prismWidth_ = other.prismWidth_;
    this->prismHeight_ = other.prismHeight_;
    this->sweepCenterX_ = other.sweepCenterX_;
    this->sweepCenterY_ = other.sweepCenterY_;
}

bool SweepRefractionMaskPara::Marshalling(Parcel& parcel) const
{
    auto isSuccess = parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteUint16(static_cast<uint16_t>(type_)) &&
        parcel.WriteFloat(maskRadius_) &&
        parcel.WriteFloat(edgeThickness_) &&
        parcel.WriteFloat(refractAmount_) &&
        parcel.WriteFloat(rippleWidth_) &&
        parcel.WriteFloat(sweepOffset_) &&
        parcel.WriteFloat(chromaDelta_) &&
        parcel.WriteInt32(shapeType_) &&
        parcel.WriteFloat(cornerRadius_) &&
        parcel.WriteFloat(prismWidth_) &&
        parcel.WriteFloat(prismHeight_) &&
        parcel.WriteFloat(sweepCenterX_) &&
        parcel.WriteFloat(sweepCenterY_);
    if (!isSuccess) {
        RS_LOGE("[ui_effect] SweepRefractionMaskPara Marshalling write failed");
        return false;
    }
    return true;
}

void SweepRefractionMaskPara::RegisterUnmarshallingCallback()
{
    MaskPara::RegisterUnmarshallingCallback(MaskPara::Type::SWEEP_REFRACTION_MASK, OnUnmarshalling);
}

bool SweepRefractionMaskPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val)
{
    uint16_t type = MaskPara::Type::NONE;
    if (!parcel.ReadUint16(type) || type != MaskPara::Type::SWEEP_REFRACTION_MASK) {
        RS_LOGE("[ui_effect] SweepRefractionMaskPara OnUnmarshalling read type failed, type=%{public}d", type);
        return false;
    }

    auto para = std::make_shared<SweepRefractionMaskPara>();
    float maskRadius = 0.0f;
    float edgeThickness = 0.0f;
    float refractAmount = 0.0f;
    float rippleWidth = 0.0f;
    float sweepOffset = 0.0f;
    float chromaDelta = 0.0f;
    int32_t shapeType = 0;
    float cornerRadius = 0.16f;
    float prismWidth = 1.0f;
    float prismHeight = 1.0f;
    float sweepCenterX = 0.0f;
    float sweepCenterY = 0.0f;
    if (!(parcel.ReadFloat(maskRadius) && parcel.ReadFloat(edgeThickness) &&
        parcel.ReadFloat(refractAmount) && parcel.ReadFloat(rippleWidth) &&
        parcel.ReadFloat(sweepOffset) && parcel.ReadFloat(chromaDelta) &&
        parcel.ReadInt32(shapeType) && parcel.ReadFloat(cornerRadius) &&
        parcel.ReadFloat(prismWidth) && parcel.ReadFloat(prismHeight) &&
        parcel.ReadFloat(sweepCenterX) && parcel.ReadFloat(sweepCenterY))) {
        RS_LOGE("[ui_effect] SweepRefractionMaskPara OnUnmarshalling read params failed");
        return false;
    }
    para->SetMaskRadius(maskRadius);
    para->SetEdgeThickness(edgeThickness);
    para->SetRefractAmount(refractAmount);
    para->SetRippleWidth(rippleWidth);
    para->SetSweepOffset(sweepOffset);
    para->SetChromaDelta(chromaDelta);
    para->SetShapeType(shapeType);
    para->SetCornerRadius(cornerRadius);
    para->SetPrismWidth(prismWidth);
    para->SetPrismHeight(prismHeight);
    Vector2f center(sweepCenterX, sweepCenterY);
    para->SetSweepCenter(center);
    val = std::move(para);
    return true;
}

std::shared_ptr<MaskPara> SweepRefractionMaskPara::Clone() const
{
    return std::make_shared<SweepRefractionMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS