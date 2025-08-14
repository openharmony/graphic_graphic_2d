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

#include "ani_font.h"

#include "typeface_ani/ani_typeface.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_FONT_NAME = "L@ohos/graphics/drawing/drawing/Font;";

static ani_object CreateAniFontMetrics(ani_env* env, const FontMetrics& fontMetrics)
{
    ani_class aniClass;
    if (env->FindClass("L@ohos/graphics/drawing/drawing/FontMetricsInner;", &aniClass) != ANI_OK) {
        return {};
    }

    ani_method aniConstructor;
    if (env->Class_FindMethod(aniClass, "<ctor>", nullptr, &aniConstructor) != ANI_OK) {
        return {};
    }

    ani_object aniFontMetrics;
    if (env->Object_New(aniClass, aniConstructor, &aniFontMetrics) != ANI_OK) {
        return {};
    }

    env->Object_SetPropertyByName_Double(aniFontMetrics, "top", fontMetrics.fTop);
    env->Object_SetPropertyByName_Double(aniFontMetrics, "ascent", fontMetrics.fAscent);
    env->Object_SetPropertyByName_Double(aniFontMetrics, "descent", fontMetrics.fDescent);
    env->Object_SetPropertyByName_Double(aniFontMetrics, "bottom", fontMetrics.fBottom);
    env->Object_SetPropertyByName_Double(aniFontMetrics, "leading", fontMetrics.fLeading);
    return aniFontMetrics;
}

ani_status AniFont::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_FONT_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_FONT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":V",
            reinterpret_cast<void*>(Constructor) },
        ani_native_function { "getMetrics", ":L@ohos/graphics/drawing/drawing/FontMetrics;",
            reinterpret_cast<void*>(GetMetrics) },
        ani_native_function { "getSize", ":D",
            reinterpret_cast<void*>(GetSize) },
        ani_native_function { "getTypeface", ":L@ohos/graphics/drawing/drawing/Typeface;",
            reinterpret_cast<void*>(GetTypeface) },
        ani_native_function { "setSize", "D:V",
            reinterpret_cast<void*>(SetSize) },
        ani_native_function { "setTypeface", "L@ohos/graphics/drawing/drawing/Typeface;:V",
            reinterpret_cast<void*>(SetTypeface) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_FONT_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniFont::Constructor(ani_env* env, ani_object obj)
{
    AniFont* aniFont = new AniFont();
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniFont))) {
        ROSEN_LOGE("AniFont::Constructor failed create aniFont");
        delete aniFont;
        return;
    }
}

ani_object AniFont::GetMetrics(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return ani_object{};
    }

    FontMetrics metrics;
    aniFont->GetFont().GetMetrics(&metrics);
    return CreateAniFontMetrics(env, metrics);
}

ani_double AniFont::GetSize(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return -1;
    }

    return aniFont->GetFont().GetSize();
}

ani_object AniFont::GetTypeface(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return ani_object{};
    }

    std::shared_ptr<Typeface> typeface = aniFont->GetFont().GetTypeface();
    AniTypeface* aniTypeface = new AniTypeface(typeface);
    ani_object aniObj = CreateAniObject(env, "L@ohos/graphics/drawing/drawing/Typeface;", nullptr);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniObj,
        NATIVE_OBJ, reinterpret_cast<ani_long>(aniTypeface))) {
        ROSEN_LOGE("AniFont::GetTypeface failed cause by Object_SetFieldByName_Long");
        delete aniTypeface;
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniFont::SetSize(ani_env* env, ani_object obj, ani_double size)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniFont->GetFont().SetSize(size);
}

void AniFont::SetTypeface(ani_env* env, ani_object obj, ani_object typeface)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, typeface);
    if (aniTypeface == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniFont->GetFont().SetTypeface(aniTypeface->GetTypeface());
}

Font& AniFont::GetFont()
{
    return font_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
