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
    explicit AniFont(std::shared_ptr<Font> font = nullptr) : font_(font) {}
    ~AniFont();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);

    static ani_object GetMetrics(ani_env* env, ani_object obj);
    static ani_double GetSize(ani_env* env, ani_object obj);
    static ani_object GetTypeface(ani_env* env, ani_object obj);
    static void SetSize(ani_env* env, ani_object obj, ani_double alpha);
    static void SetTypeface(ani_env* env, ani_object obj, ani_object typeface);

    std::shared_ptr<Font> GetFont();

private:
    static ani_object FontTransferStatic(
        ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input);
    static ani_long GetFontAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<Font>* GetFontPtrAddr();
    std::shared_ptr<Font> font_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_FONT_H