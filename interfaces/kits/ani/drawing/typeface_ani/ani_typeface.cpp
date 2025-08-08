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

#include "ani_typeface.h"
#include "typeface_arguments_ani/ani_typeface_arguments.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_TYPEFACE_NAME = "L@ohos/graphics/drawing/drawing/Typeface;";

ani_status AniTypeface::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_TYPEFACE_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_TYPEFACE_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "getFamilyName", ":Lstd/core/String;",
            reinterpret_cast<void*>(GetFamilyName) },
        ani_native_function { "makeFromFile", "Lstd/core/String;:L@ohos/graphics/drawing/drawing/Typeface;",
            reinterpret_cast<void*>(MakeFromFile) },
        ani_native_function { "makeFromFileWithArguments", "Lstd/core/String;"
            "L@ohos/graphics/drawing/drawing/TypefaceArguments;:L@ohos/graphics/drawing/drawing/Typeface;",
            reinterpret_cast<void*>(MakeFromFileWithArguments) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_TYPEFACE_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

ani_string AniTypeface::GetFamilyName(ani_env* env, ani_object obj)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, obj);
    if (aniTypeface == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return ani_string{};
    }

    std::shared_ptr<Typeface> typeface = aniTypeface->GetTypeface();
    if (typeface == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return ani_string{};
    }
    return CreateAniString(env, typeface->GetFamilyName());
}

ani_object AniTypeface::MakeFromFile(ani_env* env, ani_object obj, ani_string aniFilePath)
{
    std::string filePath = CreateStdString(env, aniFilePath);
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(filePath.c_str());
    AniTypeface* aniTypeface = new AniTypeface(typeface);
    ani_object aniObj = CreateAniObjectStatic(env, "L@ohos/graphics/drawing/drawing/Typeface;", aniTypeface);
    return aniObj;
}

ani_object AniTypeface::MakeFromFileWithArguments(ani_env* env, ani_object obj, ani_string aniFilePath,
    ani_object argumentsObj)
{
    std::string filePath = CreateStdString(env, aniFilePath);
    auto aniTypefaceArguments = GetNativeFromObj<AniTypefaceArguments>(env, argumentsObj);
    if (aniTypefaceArguments == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return CreateAniUndefined(env);
    }
    FontArguments fontArguments;
    AniTypefaceArguments::ConvertToFontArguments(aniTypefaceArguments->GetTypefaceArgumentsHelper(), fontArguments);
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(filePath.c_str(), fontArguments);
    AniTypeface* aniTypeface = new AniTypeface(typeface);
    ani_object aniObj = CreateAniObjectStatic(env, "L@ohos/graphics/drawing/drawing/Typeface;", aniTypeface);
    return aniObj;
}

std::shared_ptr<Typeface> AniTypeface::GetTypeface()
{
    return typeface_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
