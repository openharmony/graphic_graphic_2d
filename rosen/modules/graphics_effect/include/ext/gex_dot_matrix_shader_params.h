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

#include "draw/color.h"
#include "ge_common.h"
#include "parcel.h"
#include "utils/point.h"
#include "utils/rect.h"

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

    bool Marshalling(Parcel& parcel);
    bool Unmarshalling(Parcel& parcel);
};

struct RippleEffectParams {
    std::vector<Drawing::Color> effectColors_;
    std::vector<float> colorFractions_;
    std::vector<Drawing::Point> startPoints_;
    float pathWidth_ = 0.;
    bool inverseEffect_ = false;

    bool Marshalling(Parcel& parcel);
    bool Unmarshalling(Parcel& parcel);
};

struct DotMatrixNormalParams {
    Drawing::Color dotColor_;
    float dotSpacing_ = 0.f;
    float dotRadius_ = 0.f;
    Drawing::Color bgColor_;

    bool Marshalling(Parcel& parcel);
    bool Unmarshalling(Parcel& parcel);
};

struct GE_EXPORT DotMatrixShaderParams {
    DotMatrixEffectType effectType_ = DotMatrixEffectType::NONE;
    DotMatrixNormalParams normalParams_;
    RotateEffectParams rotateEffectParams_;
    RippleEffectParams rippleEffectParams_;

    bool Marshalling(Parcel& parcel);
    bool Unmarshalling(Parcel& parcel);
};

} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_EXT_DOT_MATRIX_SHADER_PARAMS_H