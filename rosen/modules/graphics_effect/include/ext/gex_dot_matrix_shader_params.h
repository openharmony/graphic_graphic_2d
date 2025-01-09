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
#ifndef GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_PARAMS_H
#define GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_PARAMS_H

#include <vector>
#include <string>
#include "draw/color.h"
#include "utils/point.h"
#include "utils/rect.h"
#include <parcel.h>

namespace OHOS {
namespace Rosen {
enum class DotMatrixEffectType {
    NONE,
    ROTATE,
    RIPPLE,
};

enum class DotMatrixDirection {
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT,
    TOP_LEFT,
    MAX = TOP_LEFT,
};

struct RotateEffectParams {
    DotMatrixDirection pathDirection_ = DotMatrixDirection::TOP_LEFT;
    std::vector<Drawing::Color> effectColors_;

    bool Marshalling(Parcel& parcel)
    {
        if (!parcel.WriteUint32((uint32_t)pathDirection_)) {
            return false;
        }
        auto size = (uint32_t)effectColors_.size();
        if (!parcel.WriteUint32(size)) {
            return false;
        }
        for (auto color : effectColors_) {
            if (!parcel.WriteUint32((uint32_t)color.CastToColorQuad())) {
                return false;
            }
        }
        return true;
    }
    
    bool Unmarshalling(Parcel& parcel)
    {
        pathDirection_ = (DotMatrixDirection)parcel.ReadUint32();
        auto size = parcel.ReadUint32();
        effectColors_.clear();
        for (uint32_t i = 0; i < size; i++) {
            effectColors_.emplace_back(Drawing::Color(parcel.ReadUint32()));
        }
        return true;
    }
};

struct RippleEffectParams {
    std::vector<Drawing::Color> effectColors_;
    std::vector<float> colorFractions_;
    std::vector<Drawing::Point> startPoints_;
    float pathWidth_ = 0.;
    bool inverseEffect_ = false;

    bool Marshalling(Parcel& parcel)
    {
        auto size = (uint32_t)effectColors_.size();
        if (!parcel.WriteUint32(size)) {
            return false;
        }
        for (auto color : effectColors_) {
            if (!parcel.WriteUint32((uint32_t)color.CastToColorQuad())) {
                return false;
            }
        }

        size = (uint32_t)colorFractions_.size();
        if (!parcel.WriteUint32(size)) {
            return false;
        }
        for (auto colorFraction : colorFractions_) {
            if (!parcel.WriteFloat(colorFraction)) {
                return false;
            }
        }

        size = startPoints_.size();
        if (!parcel.WriteUint32(size)) {
            return false;
        }
        for (auto startPoint : startPoints_) {
            if (!parcel.WriteFloat(startPoint.GetX()) || !parcel.WriteFloat(startPoint.GetY())) {
                return false;
            }
        }

        return parcel.WriteFloat(pathWidth_) && parcel.WriteBool(inverseEffect_);
    }
    
    bool Unmarshalling(Parcel& parcel)
    {
        auto size = parcel.ReadUint32();
        effectColors_.clear();
        for (uint32_t i = 0; i < size; i++) {
            effectColors_.emplace_back(Drawing::Color(parcel.ReadUint32()));
        }

        size = parcel.ReadUint32();
        colorFractions_.clear();
        for (uint32_t i = 0; i < size; i++) {
            colorFractions_.emplace_back(parcel.ReadFloat());
        }

        size = parcel.ReadUint32();
        startPoints_.clear();
        for (uint32_t i = 0; i < size; i++) {
            startPoints_.emplace_back(Drawing::Point(parcel.ReadFloat(), parcel.ReadFloat()));
        }
        pathWidth_ = parcel.ReadFloat();
        inverseEffect_ = parcel.ReadBool();
        return true;
    }
};

struct DotMatrixNormalParams {
    Drawing::Color dotColor_;
    float dotSpacing_ = 0.f;
    float dotRadius_ = 0.f;
    Drawing::Color bgColor_;

    bool Marshalling(Parcel& parcel)
    {
        if (!parcel.WriteUint32((uint32_t)dotColor_.CastToColorQuad())) {
            return false;
        }
        if (!parcel.WriteFloat(dotSpacing_) || !parcel.WriteFloat(dotRadius_)) {
            return false;
        }
        if (!parcel.WriteUint32(bgColor_.CastToColorQuad())) {
            return false;
        }
        return true;
    }
    
    bool Unmarshalling(Parcel& parcel)
    {
        dotColor_ = Drawing::Color(parcel.ReadUint32());
        dotSpacing_ = parcel.ReadFloat();
        dotRadius_ = parcel.ReadFloat();
        bgColor_ = Drawing::Color(parcel.ReadUint32());
        return true;
    }
};

struct DotMatrixShaderParams {
    DotMatrixEffectType effectType_ = DotMatrixEffectType::NONE;
    DotMatrixNormalParams normalParams_;
    RotateEffectParams rotateEffectParams_;
    RippleEffectParams rippleEffectParams_;

    bool Marshalling(Parcel& parcel)
    {
        if (!normalParams_.Marshalling(parcel) || !parcel.WriteUint32((uint32_t)effectType_)) {
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

    bool Unmarshalling(Parcel& parcel)
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
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_PARAMS_H