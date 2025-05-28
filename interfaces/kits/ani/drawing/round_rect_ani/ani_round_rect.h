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

#ifndef OHOS_ROSEN_ANI_ROUND_RECT_H
#define OHOS_ROSEN_ANI_ROUND_RECT_H

#include "ani_drawing_utils.h"
#include "utils/round_rect.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniRoundRect final {
public:
    explicit AniRoundRect(const Rect& rect, float x, float y) : roundRect_(RoundRect(rect, x, y)) {}
    explicit AniRoundRect(const RoundRect& roundRect) : roundRect_(roundRect) {}
    ~AniRoundRect() = default;

    static ani_status AniInit(ani_env *env);
    static void ConstructorWithRect(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double xRadii,
        ani_double yRadii);
    static void ConstructorWithRoundRect(ani_env* env, ani_object obj, ani_object aniRoundRectObj);

    RoundRect& GetRoundRect();

private:
    RoundRect roundRect_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_ROUND_RECT_H