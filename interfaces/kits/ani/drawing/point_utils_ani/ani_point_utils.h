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

#ifndef OHOS_ROSEN_ANI_POINT_UTILS_H
#define OHOS_ROSEN_ANI_POINT_UTILS_H

#include "ani_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniPointUtils final {
public:
    AniPointUtils() = default;
    ~AniPointUtils() = default;

    static ani_status AniInit(ani_env *env);

    static void Negate(ani_env* env, ani_object obj, ani_object aniPointObj);
    static void Offset(ani_env* env, ani_object obj, ani_object aniPointObj, ani_double dx, ani_double dy);
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_POINT_UTILS_H
