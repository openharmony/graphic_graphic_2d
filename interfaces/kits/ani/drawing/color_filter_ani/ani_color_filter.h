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

#ifndef OHOS_ROSEN_ANI_COLOR_FILTER_H
#define OHOS_ROSEN_ANI_COLOR_FILTER_H

#include "ani_drawing_utils.h"
#include "effect/color_filter.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniColorFilter final {
public:
    explicit AniColorFilter(std::shared_ptr<ColorFilter> colorFilter = nullptr) : colorFilter_(colorFilter) {}
    ~AniColorFilter();

    static ani_status AniInit(ani_env *env);

    static ani_object CreateBlendModeColorFilter(ani_env* env,
        ani_object obj, ani_object objColor, ani_enum_item aniBlendMode);
    static ani_object CreateBlendModeColorFilterWithNumber(ani_env* env,
        ani_object obj, ani_object objColor, ani_enum_item aniBlendMode);

    static ani_object CreateLightingColorFilterWithColor(
        ani_env* env, ani_object obj, ani_object aniMultiplyColorObj, ani_object aniAddColorObj);

    static ani_object CreateLightingColorFilter(
        ani_env* env, ani_object obj, ani_double aniMultiplyColor, ani_double addColor);

    static ani_object CreateMatrixColorFilter(ani_env* env, ani_object obj, ani_object aniMatrixArrayObj);

    static ani_object CreateComposeColorFilter(
    ani_env* env, ani_object obj, ani_object aniOuterColorFilterObj, ani_object aniInnerColorFilterObj);

    static ani_object CreateLinearToSRGBGamma(ani_env* env, ani_object obj);

    static ani_object CreateLumaColorFilter(ani_env* env, ani_object obj);

    static ani_object CreateSRGBGammaToLinear(ani_env* env, ani_object obj);

    DRAWING_API std::shared_ptr<ColorFilter> GetColorFilter();
private:
    static ani_object ColorFilterTransferStatic(ani_env*  env, ani_object obj, ani_object input);
    static ani_long GetColorFilterAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<ColorFilter>* GetColorFilterPtrAddr();
    std::shared_ptr<ColorFilter> colorFilter_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_COLOR_FILTER_H
