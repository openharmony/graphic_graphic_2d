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

#ifndef OHOS_ROSEN_ANI_PEN_H
#define OHOS_ROSEN_ANI_PEN_H

#include "ani_drawing_utils.h"
#include "draw/pen.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniPen final {
public:
    AniPen() = default;
    explicit AniPen(const Pen& pen) : pen_(pen) {}
    ~AniPen() = default;

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithPen(ani_env* env, ani_object obj, ani_object aniPenObj);

    static ani_int GetAlpha(ani_env* env, ani_object obj);
    static void SetAlpha(ani_env* env, ani_object obj, ani_int alpha);
    static void SetColorFilter(ani_env* env, ani_object obj, ani_object objColorFilter);
    static void SetBlendMode(ani_env* env, ani_object obj, ani_enum_item aniBlendMode);
    static void Reset(ani_env*  env, ani_object obj);

    Pen& GetPen();

private:
    Pen pen_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_PEN_H