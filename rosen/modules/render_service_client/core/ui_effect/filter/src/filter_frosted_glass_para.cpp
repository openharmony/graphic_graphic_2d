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

#include "ui_effect/filter/include/filter_frosted_glass_para.h"

#include "platform/common/rs_log.h"
#include "ui_effect/mask/include/mask_para.h"

namespace OHOS {
namespace Rosen {

FrostedGlassPara::FrostedGlassPara(const FrostedGlassPara& other)
{
    type_ = other.type_;
    blurParams_ = other.blurParams_;
    weightsEmboss_ = other.weightsEmboss_;
    weightsEdl_ = other.weightsEdl_;
    bgRates_ = other.bgRates_;
    bgKBS_ = other.bgKBS_;
    bgPos_ = other.bgPos_;
    bgNeg_ = other.bgNeg_;
    bgAlpha_ = other.bgAlpha_;
    refractParams_ = other.refractParams_;
    sdParams_ = other.sdParams_;
    sdRates_ = other.sdRates_;
    sdKBS_ = other.sdKBS_;
    sdPos_ = other.sdPos_;
    sdNeg_ = other.sdNeg_;
    envLightParams_ = other.envLightParams_;
    envLightRates_ = other.envLightRates_;
    envLightKBS_ = other.envLightKBS_;
    envLightPos_ = other.envLightPos_;
    envLightNeg_ = other.envLightNeg_;
    edLightParams_ = other.edLightParams_;
    edLightAngles_ = other.edLightAngles_;
    edLightDir_ = other.edLightDir_;
    edLightRates_ = other.edLightRates_;
    edLightKBS_ = other.edLightKBS_;
    edLightPos_ = other.edLightPos_;
    edLightNeg_ = other.edLightNeg_;
    baseVibrancyEnabled_ = other.baseVibrancyEnabled_;
    baseMaterialType_ = other.baseMaterialType_;
    materialColor_ = other.materialColor_;
    darkScale_ = other.darkScale_;
    samplingScale_ = other.samplingScale_;
    maskPara_ = other.maskPara_ ? other.maskPara_->Clone() : nullptr;
    darkAdaptiveParams_ =
        other.darkAdaptiveParams_ ? std::make_shared<AdaptiveFrostedGlassParams>(*other.darkAdaptiveParams_) : nullptr;
    isSkipFrameEnable_ = other.isSkipFrameEnable_;
}

namespace {
bool WriteVector2f(Parcel& parcel, const Vector2f& v)
{
    return parcel.WriteFloat(v.x_) && parcel.WriteFloat(v.y_);
}

bool ReadVector2f(Parcel& parcel, Vector2f& v)
{
    return parcel.ReadFloat(v.x_) && parcel.ReadFloat(v.y_);
}

bool WriteVector3f(Parcel& parcel, const Vector3f& v)
{
    return parcel.WriteFloat(v.x_) && parcel.WriteFloat(v.y_) && parcel.WriteFloat(v.z_);
}

bool ReadVector3f(Parcel& parcel, Vector3f& v)
{
    return parcel.ReadFloat(v.x_) && parcel.ReadFloat(v.y_) && parcel.ReadFloat(v.z_);
}

bool WriteVector4f(Parcel& parcel, const Vector4f& v)
{
    return parcel.WriteFloat(v.x_) && parcel.WriteFloat(v.y_) && parcel.WriteFloat(v.z_) && parcel.WriteFloat(v.w_);
}

bool ReadVector4f(Parcel& parcel, Vector4f& v)
{
    return parcel.ReadFloat(v.x_) && parcel.ReadFloat(v.y_) && parcel.ReadFloat(v.z_) && parcel.ReadFloat(v.w_);
}

bool WriteAdaptiveParams(Parcel& parcel, const AdaptiveFrostedGlassParams& params)
{
    return WriteVector2f(parcel, params.blurParams) && WriteVector2f(parcel, params.weightsEmboss) &&
           WriteVector2f(parcel, params.bgRates) && WriteVector3f(parcel, params.bgKBS) &&
           WriteVector3f(parcel, params.bgPos) && WriteVector3f(parcel, params.bgNeg);
}

bool ReadAdaptiveParams(Parcel& parcel, AdaptiveFrostedGlassParams& params)
{
    return ReadVector2f(parcel, params.blurParams) && ReadVector2f(parcel, params.weightsEmboss) &&
           ReadVector2f(parcel, params.bgRates) && ReadVector3f(parcel, params.bgKBS) &&
           ReadVector3f(parcel, params.bgPos) && ReadVector3f(parcel, params.bgNeg);
}
} // namespace

bool FrostedGlassPara::WriteBlurAndRefractParams(Parcel& parcel, const FrostedGlassPara& para)
{
    return WriteVector2f(parcel, para.blurParams_) && WriteVector2f(parcel, para.weightsEmboss_) &&
           WriteVector2f(parcel, para.weightsEdl_) && WriteVector3f(parcel, para.refractParams_);
}

bool FrostedGlassPara::ReadBlurAndRefractParams(Parcel& parcel, FrostedGlassPara& para)
{
    Vector2f blurParams;
    Vector2f weightsEmboss;
    Vector2f weightsEdl;
    Vector3f refractParams;
    if (!ReadVector2f(parcel, blurParams) || !ReadVector2f(parcel, weightsEmboss) ||
        !ReadVector2f(parcel, weightsEdl) || !ReadVector3f(parcel, refractParams)) {
        return false;
    }
    para.SetBlurParams(blurParams);
    para.SetWeightsEmboss(weightsEmboss);
    para.SetWeightsEdl(weightsEdl);
    para.SetRefractParams(refractParams);
    return true;
}

bool FrostedGlassPara::WriteBgParams(Parcel& parcel, const FrostedGlassPara& para)
{
    return WriteVector2f(parcel, para.bgRates_) && WriteVector3f(parcel, para.bgKBS_) &&
           WriteVector3f(parcel, para.bgPos_) && WriteVector3f(parcel, para.bgNeg_) && parcel.WriteFloat(para.bgAlpha_);
}

bool FrostedGlassPara::ReadBgParams(Parcel& parcel, FrostedGlassPara& para)
{
    Vector2f bgRates;
    Vector3f bgKBS;
    Vector3f bgPos;
    Vector3f bgNeg;
    float bgAlpha = 1.0f;
    if (!ReadVector2f(parcel, bgRates) || !ReadVector3f(parcel, bgKBS) || !ReadVector3f(parcel, bgPos) ||
        !ReadVector3f(parcel, bgNeg) || !parcel.ReadFloat(bgAlpha)) {
        return false;
    }
    para.SetBgRates(bgRates);
    para.SetBgKBS(bgKBS);
    para.SetBgPos(bgPos);
    para.SetBgNeg(bgNeg);
    para.SetBgAlpha(bgAlpha);
    return true;
}

bool FrostedGlassPara::WriteSdParams(Parcel& parcel, const FrostedGlassPara& para)
{
    return WriteVector3f(parcel, para.sdParams_) && WriteVector2f(parcel, para.sdRates_) &&
           WriteVector3f(parcel, para.sdKBS_) && WriteVector3f(parcel, para.sdPos_) &&
           WriteVector3f(parcel, para.sdNeg_);
}

bool FrostedGlassPara::ReadSdParams(Parcel& parcel, FrostedGlassPara& para)
{
    Vector3f sdParams;
    Vector2f sdRates;
    Vector3f sdKBS;
    Vector3f sdPos;
    Vector3f sdNeg;
    if (!ReadVector3f(parcel, sdParams) || !ReadVector2f(parcel, sdRates) || !ReadVector3f(parcel, sdKBS) ||
        !ReadVector3f(parcel, sdPos) || !ReadVector3f(parcel, sdNeg)) {
        return false;
    }
    para.SetSdParams(sdParams);
    para.SetSdRates(sdRates);
    para.SetSdKBS(sdKBS);
    para.SetSdPos(sdPos);
    para.SetSdNeg(sdNeg);
    return true;
}

bool FrostedGlassPara::WriteEnvLightParams(Parcel& parcel, const FrostedGlassPara& para)
{
    return WriteVector3f(parcel, para.envLightParams_) && WriteVector2f(parcel, para.envLightRates_) &&
           WriteVector3f(parcel, para.envLightKBS_) && WriteVector3f(parcel, para.envLightPos_) &&
           WriteVector3f(parcel, para.envLightNeg_);
}

bool FrostedGlassPara::ReadEnvLightParams(Parcel& parcel, FrostedGlassPara& para)
{
    Vector3f envLightParams;
    Vector2f envLightRates;
    Vector3f envLightKBS;
    Vector3f envLightPos;
    Vector3f envLightNeg;
    if (!ReadVector3f(parcel, envLightParams) || !ReadVector2f(parcel, envLightRates) ||
        !ReadVector3f(parcel, envLightKBS) || !ReadVector3f(parcel, envLightPos) ||
        !ReadVector3f(parcel, envLightNeg)) {
        return false;
    }
    para.SetEnvLightParams(envLightParams);
    para.SetEnvLightRates(envLightRates);
    para.SetEnvLightKBS(envLightKBS);
    para.SetEnvLightPos(envLightPos);
    para.SetEnvLightNeg(envLightNeg);
    return true;
}

bool FrostedGlassPara::WriteEdLightParams(Parcel& parcel, const FrostedGlassPara& para)
{
    return WriteVector2f(parcel, para.edLightParams_) && WriteVector2f(parcel, para.edLightAngles_) &&
           WriteVector2f(parcel, para.edLightDir_) && WriteVector2f(parcel, para.edLightRates_) &&
           WriteVector3f(parcel, para.edLightKBS_) && WriteVector3f(parcel, para.edLightPos_) &&
           WriteVector3f(parcel, para.edLightNeg_);
}

bool FrostedGlassPara::ReadEdLightParams(Parcel& parcel, FrostedGlassPara& para)
{
    Vector2f edLightParams;
    Vector2f edLightAngles;
    Vector2f edLightDir;
    Vector2f edLightRates;
    Vector3f edLightKBS;
    Vector3f edLightPos;
    Vector3f edLightNeg;
    if (!ReadVector2f(parcel, edLightParams) || !ReadVector2f(parcel, edLightAngles) ||
        !ReadVector2f(parcel, edLightDir) || !ReadVector2f(parcel, edLightRates) || !ReadVector3f(parcel, edLightKBS) ||
        !ReadVector3f(parcel, edLightPos) || !ReadVector3f(parcel, edLightNeg)) {
        return false;
    }
    para.SetEdLightParams(edLightParams);
    para.SetEdLightAngles(edLightAngles);
    para.SetEdLightDir(edLightDir);
    para.SetEdLightRates(edLightRates);
    para.SetEdLightKBS(edLightKBS);
    para.SetEdLightPos(edLightPos);
    para.SetEdLightNeg(edLightNeg);
    return true;
}

bool FrostedGlassPara::WriteBaseParams(Parcel& parcel, const FrostedGlassPara& para)
{
    return parcel.WriteBool(para.baseVibrancyEnabled_) && parcel.WriteFloat(para.baseMaterialType_) &&
           WriteVector4f(parcel, para.materialColor_) && parcel.WriteFloat(para.darkScale_) &&
           parcel.WriteFloat(para.samplingScale_);
}

bool FrostedGlassPara::ReadBaseParams(Parcel& parcel, FrostedGlassPara& para)
{
    bool baseVibrancyEnabled = true;
    float baseMaterialType = 0.0f;
    Vector4f materialColor;
    float darkScale = 0.0f;
    float samplingScale = 1.0f;
    if (!parcel.ReadBool(baseVibrancyEnabled) || !parcel.ReadFloat(baseMaterialType) ||
        !ReadVector4f(parcel, materialColor) || !parcel.ReadFloat(darkScale) || !parcel.ReadFloat(samplingScale)) {
        return false;
    }
    para.SetBaseVibrancyEnabled(baseVibrancyEnabled);
    para.SetBaseMaterialType(baseMaterialType);
    para.SetMaterialColor(materialColor);
    para.SetDarkScale(darkScale);
    para.SetSamplingScale(samplingScale);
    return true;
}

bool FrostedGlassPara::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(static_cast<uint16_t>(type_)) || !parcel.WriteUint16(static_cast<uint16_t>(type_)) ||
        !WriteBlurAndRefractParams(parcel, *this) || !WriteBgParams(parcel, *this) || !WriteSdParams(parcel, *this) ||
        !WriteEnvLightParams(parcel, *this) || !WriteEdLightParams(parcel, *this) || !WriteBaseParams(parcel, *this)) {
        RS_LOGE("[ui_effect] FrostedGlassPara Marshalling write params failed");
        return false;
    }
    if (maskPara_ != nullptr) {
        if (!parcel.WriteBool(true) || !maskPara_->Marshalling(parcel)) {
            RS_LOGE("[ui_effect] FrostedGlassPara Marshalling write mask failed");
            return false;
        }
    } else {
        if (!parcel.WriteBool(false)) {
            RS_LOGE("[ui_effect] FrostedGlassPara Marshalling write mask failed");
            return false;
        }
    }
    if (darkAdaptiveParams_ != nullptr) {
        if (!parcel.WriteBool(true) || !WriteAdaptiveParams(parcel, *darkAdaptiveParams_)) {
            RS_LOGE("[ui_effect] FrostedGlassPara Marshalling write dark adaptive params failed");
            return false;
        }
    } else {
        if (!parcel.WriteBool(false)) {
            RS_LOGE("[ui_effect] FrostedGlassPara Marshalling write dark adaptive params failed");
            return false;
        }
    }
    if (!parcel.WriteBool(isSkipFrameEnable_)) {
        RS_LOGE("[ui_effect] FrostedGlassPara Marshalling write skip frame enable failed");
        return false;
    }
    return true;
}

void FrostedGlassPara::RegisterUnmarshallingCallback()
{
    FilterPara::RegisterUnmarshallingCallback(FilterPara::ParaType::FROSTED_GLASS, OnUnmarshalling);
}

bool FrostedGlassPara::OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val)
{
    uint16_t type = FilterPara::ParaType::NONE;
    if (!parcel.ReadUint16(type) || type != FilterPara::ParaType::FROSTED_GLASS) {
        RS_LOGE("[ui_effect] FrostedGlassPara OnUnmarshalling read type failed");
        return false;
    }
    auto para = std::make_shared<FrostedGlassPara>();
    if (!ReadBlurAndRefractParams(parcel, *para) || !ReadBgParams(parcel, *para) || !ReadSdParams(parcel, *para) ||
        !ReadEnvLightParams(parcel, *para) || !ReadEdLightParams(parcel, *para) || !ReadBaseParams(parcel, *para)) {
        RS_LOGE("[ui_effect] FrostedGlassPara OnUnmarshalling read params failed");
        return false;
    }
    bool hasMask = false;
    if (!parcel.ReadBool(hasMask)) {
        RS_LOGE("[ui_effect] FrostedGlassPara OnUnmarshalling read hasMask failed");
        return false;
    }
    if (hasMask) {
        std::shared_ptr<MaskPara> maskPara = nullptr;
        if (!MaskPara::Unmarshalling(parcel, maskPara)) {
            RS_LOGE("[ui_effect] FrostedGlassPara OnUnmarshalling read mask failed");
            return false;
        }
        para->SetMask(maskPara);
    }
    bool hasDarkAdaptiveParams = false;
    if (!parcel.ReadBool(hasDarkAdaptiveParams)) {
        RS_LOGE("[ui_effect] FrostedGlassPara OnUnmarshalling read hasDarkAdaptiveParams failed");
        return false;
    }
    if (hasDarkAdaptiveParams) {
        AdaptiveFrostedGlassParams params;
        if (!ReadAdaptiveParams(parcel, params)) {
            RS_LOGE("[ui_effect] FrostedGlassPara OnUnmarshalling read dark adaptive params failed");
            return false;
        }
        para->SetDarkAdaptiveParams(params);
    }
    bool isSkipFrameEnable = false;
    if (!parcel.ReadBool(isSkipFrameEnable)) {
        RS_LOGE("[ui_effect] FrostedGlassPara OnUnmarshalling read skip frame enable failed");
        return false;
    }
    para->SetSkipFrameEnable(isSkipFrameEnable);
    val = std::move(para);
    return true;
}

std::shared_ptr<FilterPara> FrostedGlassPara::Clone() const
{
    return std::make_shared<FrostedGlassPara>(*this);
}

} // namespace Rosen
} // namespace OHOS