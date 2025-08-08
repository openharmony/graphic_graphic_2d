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

#ifndef OHOS_ROSEN_ANI_FONT_H
#define OHOS_ROSEN_ANI_FONT_H

#include "ani_drawing_utils.h"

#include "text/font.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniFont final {
public:
    AniFont() = default;
    explicit AniFont(const Font& font) : font_(font) {}
    ~AniFont() = default;

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);

    static ani_object GetMetrics(ani_env* env, ani_object obj);
    static ani_double GetSize(ani_env* env, ani_object obj);
    static ani_object GetTypeface(ani_env* env, ani_object obj);
    static void SetSize(ani_env* env, ani_object obj, ani_double alpha);
    static void SetTypeface(ani_env* env, ani_object obj, ani_object typeface);

    Font& GetFont();

private:
    Font font_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_FONT_H