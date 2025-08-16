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
        ani_native_function { "constructorNative", ":V", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "getFamilyName", ":Lstd/core/String;",
            reinterpret_cast<void*>(GetFamilyName) },
    };

    std::array statitMethods = {
        ani_native_function { "makeFromFile", "Lstd/core/String;:L@ohos/graphics/drawing/drawing/Typeface;",
            reinterpret_cast<void*>(MakeFromFile) },
        ani_native_function { "makeFromFileWithArguments", "Lstd/core/String;"
            "L@ohos/graphics/drawing/drawing/TypefaceArguments;:L@ohos/graphics/drawing/drawing/Typeface;",
            reinterpret_cast<void*>(MakeFromFileWithArguments) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s ret: %{public}d", ANI_CLASS_TYPEFACE_NAME, ret);
        return ANI_NOT_FOUND;
    }

    ret = env->Class_BindStaticNativeMethods(cls, statitMethods.data(), statitMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s ret: %{public}d", ANI_CLASS_TYPEFACE_NAME, ret);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniTypeface::Constructor(ani_env* env, ani_object obj)
{
    AniTypeface* aniTypeface = new AniTypeface(GetZhCnTypeface());
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniTypeface))) {
        ROSEN_LOGE("AniTypeface::Constructor failed create aniTypeface");
        delete aniTypeface;
        return;
    }
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
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete aniTypeface;
        ROSEN_LOGE("AniTypeface::MakeFromFile failed cause aniObj is undefined");
    }
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
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete aniTypeface;
        ROSEN_LOGE("AniTypeface::MakeFromFileWithArguments failed cause aniObj is undefined");
    }
    return aniObj;
}

std::shared_ptr<Typeface> LoadZhCnTypeface()
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(AniTypeface::ZH_CN_TTF);
    if (typeface == nullptr) {
        typeface = Typeface::MakeDefault();
    }
    return typeface;
}

std::shared_ptr<Typeface> AniTypeface::GetZhCnTypeface()
{
    static std::shared_ptr<Typeface> zhCnTypeface = LoadZhCnTypeface();
    return zhCnTypeface;
}

std::shared_ptr<Typeface> AniTypeface::GetTypeface()
{
    return typeface_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
