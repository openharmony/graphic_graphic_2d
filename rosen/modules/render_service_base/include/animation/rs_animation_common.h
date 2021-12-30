/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_COMMON_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_COMMON_H

#include <cstdint>

namespace OHOS {
namespace Rosen {
static constexpr float FRACTION_MIN = 0.0f;
static constexpr float FRACTION_MAX = 1.0f;
static constexpr float EPSILON = 1e-6;
static constexpr int64_t MS_TO_NS = 1000000;

enum class RotationMode {
    ROTATE_NONE,
    ROTATE_AUTO,
    ROTATE_AUTO_REVERSE
};

enum class RSTransitionEffectType {
    FADE_IN,
    FADE_OUT,
    SCALE_IN,
    SCALE_OUT,
    ROTATE_IN,
    ROTATE_OUT,
    TRANSLATE_IN,
    TRANSLATE_OUT,
    UNDEFINED,
};

struct ScaleParams {
    ScaleParams() = default;
    ScaleParams(float x, float y, float z, float pivotX, float pivotY)
        : scaleX(x), scaleY(y), scaleZ(z), pivotX(pivotX), pivotY(pivotY) {}
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
};

struct TranslateParams {
    TranslateParams() = default;
    TranslateParams(float dx, float dy, float dz) : dx(dx), dy(dy), dz(dz) {}
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
};

struct RotateParams {
    RotateParams() = default;
    RotateParams(float x, float y, float z, float angle, float pivotX, float pivotY)
        : dx(x), dy(y), dz(z), angle(angle), pivotX(pivotX), pivotY(pivotY) {};
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
    float angle = 0.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
};

class RSTransitionEffect final {
public:
    explicit RSTransitionEffect(RSTransitionEffectType type, float opacity = 0.0f)
    {
        bool isValid = (type == RSTransitionEffectType::FADE_IN) ||
                       (type == RSTransitionEffectType::FADE_OUT);
        type_ = isValid ? type : RSTransitionEffectType::UNDEFINED;
        opacity_ = opacity;
    };
    RSTransitionEffect(RSTransitionEffectType type, ScaleParams scaleParams)
    {
        bool isValid = (type == RSTransitionEffectType::SCALE_IN) ||
                       (type == RSTransitionEffectType::SCALE_OUT);
        type_ = isValid ? type : RSTransitionEffectType::UNDEFINED;
        scaleParams_ = isValid ? scaleParams : ScaleParams();
    };
    RSTransitionEffect(RSTransitionEffectType type, TranslateParams translateParams)
    {
        bool isValid = (type == RSTransitionEffectType::TRANSLATE_IN) ||
                       (type == RSTransitionEffectType::TRANSLATE_OUT);
        type_ = isValid ? type : RSTransitionEffectType::UNDEFINED;
        translateParams_ = isValid ? translateParams : TranslateParams();
    };
    RSTransitionEffect(RSTransitionEffectType type, RotateParams rotateParams)
    {
        bool isValid = (type == RSTransitionEffectType::ROTATE_IN) ||
                       (type == RSTransitionEffectType::ROTATE_OUT);
        type_ = isValid ? type : RSTransitionEffectType::UNDEFINED;
        rotateParams_ = isValid ? rotateParams : RotateParams();
    };
    RSTransitionEffect()
    {
        type_ = RSTransitionEffectType::UNDEFINED;
    };
    ~RSTransitionEffect() {};

    RSTransitionEffectType GetType() const { return type_; };
    float GetOpacity() const { return opacity_; };
    ScaleParams GetScaleParams() const { return scaleParams_; };
    TranslateParams GetTranslateParams() const { return translateParams_; };
    RotateParams GetRotateParams() const { return rotateParams_; };
private:
    RSTransitionEffectType type_ = RSTransitionEffectType::UNDEFINED;
    union {
        float opacity_;
        ScaleParams scaleParams_;
        TranslateParams translateParams_;
        RotateParams rotateParams_;
    };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_COMMON_H
