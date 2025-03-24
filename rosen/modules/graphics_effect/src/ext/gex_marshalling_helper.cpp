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

#include "ge_log.h"
#include "ext/gex_dot_matrix_shader.h"
#include "ext/gex_flow_light_sweep_shader.h"

namespace OHOS {
namespace Rosen {

constexpr uint32_t MARSHALLING_SIZE_MAX_LIMIT = 100;  // 100 max length

bool RotateEffectParams::Marshalling(Parcel& parcel)
{
    if (!parcel.WriteUint32((uint32_t)pathDirection_)) {
        GE_LOGE("RotateEffectParams::Marshalling Write pathDirection failed!");
        return false;
    }
    auto size = (uint32_t)effectColors_.size();
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RotateEffectParams::Marshalling effectColors size exceeded the limit.");
        return false;
    }
    if (!parcel.WriteUint32(size)) {
        GE_LOGE("RotateEffectParams::Marshalling Write size failed!");
        return false;
    }
    for (auto color : effectColors_) {
        if (!parcel.WriteUint32((uint32_t)color.CastToColorQuad())) {
            GE_LOGE("RotateEffectParams::Marshalling Write color failed!");
            return false;
        }
    }
    return true;
}

bool RotateEffectParams::Unmarshalling(Parcel& parcel)
{
    uint32_t valueUint32 = 0;
    if (!parcel.ReadUint32(valueUint32)) {
        GE_LOGE("RotateEffectParams::Unmarshalling Read pathDirection failed!");
        return false;
    }
    pathDirection_ = static_cast<DotMatrixDirection>(valueUint32);
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        GE_LOGE("RotateEffectParams::Unmarshalling Read size failed!");
        return false;
    }
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RotateEffectParams::Unmarshalling effectColors size exceeded the limit.");
        return false;
    }
    effectColors_.clear();
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.ReadUint32(valueUint32)) {
            GE_LOGE("RotateEffectParams::Unmarshalling Read effectColors failed!");
            return false;
        }
        effectColors_.emplace_back(Drawing::Color(valueUint32));
    }
    return true;
}

bool RippleEffectParams::Marshalling(Parcel& parcel)
{
    auto size = (uint32_t)effectColors_.size();
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RippleEffectParams::Marshalling effectColors size exceeded the limit.");
        return false;
    }
    if (!parcel.WriteUint32(size)) {
        GE_LOGE("RippleEffectParams::Marshalling Write effectColorsSize failed!");
        return false;
    }
    for (auto color : effectColors_) {
        if (!parcel.WriteUint32((uint32_t)color.CastToColorQuad())) {
            GE_LOGE("RippleEffectParams::Marshalling Write effectColors failed!");
            return false;
        }
    }

    size = (uint32_t)colorFractions_.size();
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RippleEffectParams::Marshalling colorFractions size exceeded the limit.");
        return false;
    }
    if (!parcel.WriteUint32(size)) {
        GE_LOGE("RippleEffectParams::Marshalling Write colorFractionsSize failed!");
        return false;
    }
    for (auto colorFraction : colorFractions_) {
        if (!parcel.WriteFloat(colorFraction)) {
            GE_LOGE("RippleEffectParams::Marshalling Write colorFraction failed!");
            return false;
        }
    }

    size = startPoints_.size();
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RippleEffectParams::Marshalling startPoints size exceeded the limit.");
        return false;
    }
    if (!parcel.WriteUint32(size)) {
        GE_LOGE("RippleEffectParams::Marshalling Write startPointsSize failed!");
        return false;
    }
    for (auto startPoint : startPoints_) {
        if (!parcel.WriteFloat(startPoint.GetX()) || !parcel.WriteFloat(startPoint.GetY())) {
            GE_LOGE("RippleEffectParams::Marshalling Write startPointX or startPointY failed!");
            return false;
        }
    }
    bool success = parcel.WriteFloat(pathWidth_) && parcel.WriteBool(inverseEffect_);
    if (!success) {
        GE_LOGE("RippleEffectParams::Marshalling Write pathWidth or inverseEffect failed!");
    }
    return success;
}

bool RippleEffectParams::Unmarshalling(Parcel& parcel)
{
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        GE_LOGE("RippleEffectParams::Unmarshalling Read size failed!");
        return false;
    }
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RippleEffectParams::Unmarshalling effectColors size exceeded the limit.");
        return false;
    }
    uint32_t valueUint32 = 0;
    effectColors_.clear();
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.ReadUint32(valueUint32)) {
            GE_LOGE("RippleEffectParams::Unmarshalling Read effectColors failed!");
            return false;
        }
        effectColors_.emplace_back(Drawing::Color(valueUint32));
    }

    if (!parcel.ReadUint32(size)) {
        GE_LOGE("RippleEffectParams::Unmarshalling Read size failed!");
        return false;
    }
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RippleEffectParams::Unmarshalling colorFractions size exceeded the limit.");
        return false;
    }
    float valueFloat = 0.f;
    colorFractions_.clear();
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.ReadFloat(valueFloat)) {
            GE_LOGE("RippleEffectParams::Unmarshalling Read colorFractions failed!");
            return false;
        }
        colorFractions_.emplace_back(valueFloat);
    }

    if (!parcel.ReadUint32(size)) {
        GE_LOGE("RippleEffectParams::Unmarshalling Read startPoints size failed!");
        return false;
    }
    if (size > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("RippleEffectParams::Unmarshalling startPoints size exceeded the limit.");
        return false;
    }
    float valueFloatTwo = 0.f;
    startPoints_.clear();
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.ReadFloat(valueFloat) || !parcel.ReadFloat(valueFloatTwo)) {
            GE_LOGE("RippleEffectParams::Unmarshalling Read valueFloat or valueFloatTwo failed!");
            return false;
        }
        startPoints_.emplace_back(Drawing::Point(valueFloat, valueFloatTwo));
    }
    bool success = parcel.ReadFloat(pathWidth_) && parcel.ReadBool(inverseEffect_);
    if (!success) {
        GE_LOGE("RippleEffectParams::Unmarshalling Read pathWidth or inverseEffect failed!");
    }
    return success;
}

bool DotMatrixNormalParams::Marshalling(Parcel& parcel)
{
    if (!parcel.WriteUint32((uint32_t)dotColor_.CastToColorQuad())) {
        GE_LOGE("DotMatrixNormalParams::Marshalling Write dotColor failed!");
        return false;
    }
    if (!parcel.WriteFloat(dotSpacing_) || !parcel.WriteFloat(dotRadius_)) {
        GE_LOGE("DotMatrixNormalParams::Marshalling Write dotSpacing or dotRadius failed!");
        return false;
    }
    if (!parcel.WriteUint32(bgColor_.CastToColorQuad())) {
        GE_LOGE("DotMatrixNormalParams::Marshalling Write bgColor failed!");
        return false;
    }
    return true;
}

bool DotMatrixNormalParams::Unmarshalling(Parcel& parcel)
{
    uint32_t valueUint32 = 0;
    uint32_t valueUint32Two = 0;
    if (!parcel.ReadUint32(valueUint32) || !parcel.ReadFloat(dotSpacing_) || !parcel.ReadFloat(dotRadius_) ||
        !parcel.ReadUint32(valueUint32Two)) {
        GE_LOGE("DotMatrixNormalParams::Unmarshalling Read parcel failed!");
        return false;
    }
    dotColor_ = Drawing::Color(valueUint32);
    bgColor_ = Drawing::Color(valueUint32Two);
    return true;
}

bool DotMatrixShaderParams::Marshalling(Parcel& parcel)
{
    if (!normalParams_.Marshalling(parcel) || !parcel.WriteUint32((uint32_t)effectType_)) {
        GE_LOGE("DotMatrixShaderParams::Marshalling Read effectType failed!");
        return false;
    }

    if (effectType_ == DotMatrixEffectType::ROTATE) {
        return rotateEffectParams_.Marshalling(parcel);
    }

    if (effectType_ == DotMatrixEffectType::RIPPLE) {
        return rippleEffectParams_.Marshalling(parcel);
    }
    return true;
}

bool DotMatrixShaderParams::Unmarshalling(Parcel& parcel)
{
    if (!normalParams_.Unmarshalling(parcel)) {
        return false;
    }

    effectType_ = (DotMatrixEffectType)parcel.ReadUint32();
    if (effectType_ == DotMatrixEffectType::ROTATE) {
        return rotateEffectParams_.Unmarshalling(parcel);
    }

    if (effectType_ == DotMatrixEffectType::RIPPLE) {
        return rippleEffectParams_.Unmarshalling(parcel);
    }
    return true;
}

bool GEXFlowLightSweepParams::Marshalling(Parcel& parcel)
{
    uint32_t effectColorsSize = static_cast<uint32_t>(effectColors_.size());
    if (effectColorsSize > MARSHALLING_SIZE_MAX_LIMIT) {
        GE_LOGE("GEXFlowLightSweepParams::Marshalling effectColors size exceeded the limit.");
        return false;
    }

    if (!parcel.WriteUint32(effectColorsSize)) {
        GE_LOGE("GEXFlowLightSweepParams::Marshalling Write effectColorsSize failed!");
        return false;
    }

    for (size_t i = 0; i < effectColorsSize; i++) {
        if (!parcel.WriteUint32((uint32_t)effectColors_[i].first.CastToColorQuad())) {
            GE_LOGE("GEXFlowLightSweepParams::Marshalling Write effectColorsFirst failed!");
            return false;
        }
        if (!parcel.WriteFloat(effectColors_[i].second)) {
            GE_LOGE("GEXFlowLightSweepParams::Marshalling Write effectColorsSecond failed!");
            return false;
        }
    }

    return true;
}

bool GEXFlowLightSweepParams::Unmarshalling(Parcel& parcel)
{
        uint32_t effectColorsSize = 0;
        if (!parcel.ReadUint32(effectColorsSize)) {
            GE_LOGE("GEXFlowLightSweepParams::Unmarshalling Read effectColorsSize failed!");
            return false;
        }
        if (effectColorsSize > MARSHALLING_SIZE_MAX_LIMIT) {
            GE_LOGE("GEXFlowLightSweepParams::Unmarshalling effectColors size exceeded the limit.");
            return false;
        }
 
        uint32_t valueUint32 = 0;
        float valueFloat = 0.f;
        effectColors_.clear();
        effectColors_.reserve(effectColorsSize);
        for (size_t i = 0; i < effectColorsSize; i++) {
            if (!parcel.ReadUint32(valueUint32) || !parcel.ReadFloat(valueFloat)) {
                GE_LOGE("GEXFlowLightSweepParams::Unmarshalling Read effectColor failed!");
                return false;
            }
            std::pair<Drawing::Color, float> effectColor;
            effectColor.first = Drawing::Color(valueUint32);
            effectColor.second = valueFloat;
 
            effectColors_.emplace_back(effectColor);
        }
 
        return true;
}
} // namespace Rosen
} // namespace OHOS
