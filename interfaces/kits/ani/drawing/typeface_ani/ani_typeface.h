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

#ifndef OHOS_ROSEN_ANI_TYPEFACE_H
#define OHOS_ROSEN_ANI_TYPEFACE_H

#include "ani_drawing_utils.h"

#include "text/typeface.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniTypeface final {
public:
    explicit AniTypeface(std::shared_ptr<Typeface> typeface) : typeface_(typeface) {}
    ~AniTypeface() = default;

    static ani_status AniInit(ani_env *env);

    static void CreateAniTypeface(ani_env* env, ani_object obj, ani_long typeface);

    static ani_string GetFamilyName(ani_env* env, ani_object obj);
    static ani_object MakeFromFile(ani_env* env, ani_object obj, ani_string filePath);
    static ani_object MakeFromFileWithArguments(ani_env* env, ani_object obj, ani_string filePath,
        ani_object argumentsObj);

    std::shared_ptr<Typeface> GetTypeface();

private:
    std::shared_ptr<Typeface> typeface_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_TYPEFACE_H