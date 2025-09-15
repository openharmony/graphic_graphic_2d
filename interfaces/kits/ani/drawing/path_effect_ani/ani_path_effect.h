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

#ifndef OHOS_ROSEN_ANI_PATH_EFFECT_H
#define OHOS_ROSEN_ANI_PATH_EFFECT_H

#include "ani_drawing_utils.h"
#include "effect/path_effect.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniPathEffect final {
public:
    explicit AniPathEffect(std::shared_ptr<PathEffect> pathEffect) : pathEffect_(pathEffect) {}
    ~AniPathEffect();

    static ani_status AniInit(ani_env *env);

    static ani_object CreateDiscretePathEffect(ani_env* env, ani_object obj, ani_double aniSegLength,
        ani_double aniDev, ani_object aniSeedAssistObj);
    static ani_object CreateSumPathEffect(ani_env* env, ani_object obj,
        ani_object aniFirstPathEffectObj, ani_object aniSecondPathEffectObj);
    static ani_object CreatePathDashEffect(ani_env* env, ani_object obj, ani_object aniPathObj,
        ani_double aniAdvance, ani_double aniPhase, ani_enum_item aniStyleEnum);
    static ani_object CreateComposePathEffect(ani_env* env, ani_object obj,
        ani_object aniOuterPathEffectObj, ani_object aniInnerPathEffectObj);
    static ani_object CreateCornerPathEffect(ani_env* env, ani_object obj, ani_double aniRadius);
    static ani_object CreateDashPathEffect(
        ani_env* env, ani_object obj, ani_object aniIntervalsArray, ani_double aniPhase);

    DRAWING_API std::shared_ptr<PathEffect> GetPathEffect();

private:
    std::shared_ptr<PathEffect> pathEffect_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_PATH_EFFECT_H
