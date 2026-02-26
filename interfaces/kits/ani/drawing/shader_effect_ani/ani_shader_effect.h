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

#ifndef OHOS_ROSEN_ANI_SHADER_EFFECT_H
#define OHOS_ROSEN_ANI_SHADER_EFFECT_H

#include "ani_drawing_utils.h"
#include "effect/shader_effect.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniShaderEffect final {
public:
    explicit AniShaderEffect(std::shared_ptr<ShaderEffect> shaderEffect = nullptr) : shaderEffect_(shaderEffect) {};
    ~AniShaderEffect();

    static ani_status AniInit(ani_env *env);

    static ani_object CreateColorShader(ani_env* env, ani_object obj, ani_int color);
    static ani_object CreateLinearGradient(ani_env* env, ani_object obj, ani_object startPt, ani_object endPt,
        ani_array colorsArray, ani_enum_item aniTileMode, ani_object aniPos, ani_object aniMatrix);
    static ani_object CreateConicalGradient(ani_env* env, ani_object obj, ani_object startPt, ani_double startRadius,
        ani_object endPt, ani_double endRadius, ani_array colorsArray, ani_enum_item aniTileMode, ani_object aniPos,
        ani_object aniMatrix);
    static ani_object CreateSweepGradient(ani_env* env, ani_object obj, ani_object centerPt, ani_array colorsArray,
        ani_enum_item aniTileMode, ani_double startAngle, ani_double endAngle, ani_object aniPos, ani_object aniMatrix);
    static ani_object CreateRadialGradient(ani_env* env, ani_object obj, ani_object centerPt, ani_double radius,
        ani_array colorsArray, ani_enum_item aniTileMode, ani_object aniPos, ani_object aniMatrix);
    std::shared_ptr<ShaderEffect> GetShaderEffect();
    static ani_object CreateImageShader(ani_env* env, ani_object obj, ani_object pixelmapObj, ani_enum_item aniTileX, ani_enum_item aniTileY,
        ani_object samplingOptionsobj, ani_object aniMatrix);
    static ani_object CreateComposeShader(ani_env* env, ani_object obj, ani_object dstShaderEffect,
        ani_object srcShaderEffect, ani_enum_item blendModeobj);
private:
    static ani_object CreateAniShaderEffect(ani_env* env, std::shared_ptr<ShaderEffect> shaderEffect);
    std::shared_ptr<ShaderEffect> shaderEffect_ = nullptr;;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_SHADER_EFFECT_H