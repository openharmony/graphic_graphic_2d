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
    static constexpr char ZH_CN_TTF[] = "/system/fonts/HarmonyOS_Sans_SC.ttf";

    explicit AniTypeface(std::shared_ptr<Typeface> typeface) : typeface_(typeface) {}
    ~AniTypeface();

    static ani_status AniInit(ani_env *env);

    static void CreateAniTypeface(ani_env* env, ani_object obj, ani_long typeface);
    static void Constructor(ani_env* env, ani_object obj);
    static ani_string GetFamilyName(ani_env* env, ani_object obj);
    static ani_object MakeFromFile(ani_env* env, ani_object obj, ani_string filePath);
    static ani_object MakeFromRawFile(ani_env* env, ani_object obj, ani_object resource);
    static ani_object MakeFromFileWithArguments(ani_env* env, ani_object obj, ani_string filePath,
        ani_object argumentsObj);
    static ani_object MakeFromRawFileWithArguments(ani_env* env, ani_object obj, ani_object resource,
        ani_object argumentsObj);
    static ani_object MakeFromCurrent(ani_env* env, ani_object obj, ani_object argumentsObj);

    static std::shared_ptr<Typeface> GetZhCnTypeface();

    static ani_boolean IsBold(ani_env* env, ani_object obj);
    static ani_boolean IsItalic(ani_env* env, ani_object obj);

    std::shared_ptr<Typeface> GetTypeface();

private:
    static ani_object CreateAniTypeface(ani_env* env, std::shared_ptr<Typeface> typeface);
    static ani_object TypefaceTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    static ani_long GetTypefaceAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<Typeface>* GetTypefacePtrAddr();
    std::shared_ptr<Typeface> typeface_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_TYPEFACE_H