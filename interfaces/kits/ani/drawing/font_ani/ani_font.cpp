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
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/font_napi/js_font.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_FONT_NAME = "@ohos.graphics.drawing.drawing.Font";

ani_object CreateAniFontMetrics(ani_env* env, const FontMetrics& fontMetrics)
{
    ani_object aniFontMetrics = CreateAniObject(env, "@ohos.graphics.drawing.drawing.FontMetricsInner", "ddddd:",
        ani_double(fontMetrics.fTop),
        ani_double(fontMetrics.fAscent),
        ani_double(fontMetrics.fDescent),
        ani_double(fontMetrics.fBottom),
        ani_double(fontMetrics.fLeading));
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
        ani_native_function { "constructorNative", ":",
            reinterpret_cast<void*>(Constructor) },
        ani_native_function { "getMetrics", ":C{@ohos.graphics.drawing.drawing.FontMetrics}",
            reinterpret_cast<void*>(GetMetrics) },
        ani_native_function { "getSize", ":d",
            reinterpret_cast<void*>(GetSize) },
        ani_native_function { "getTypeface", ":C{@ohos.graphics.drawing.drawing.Typeface}",
            reinterpret_cast<void*>(GetTypeface) },
        ani_native_function { "setSize", "d:",
            reinterpret_cast<void*>(SetSize) },
        ani_native_function { "setTypeface", "C{@ohos.graphics.drawing.drawing.Typeface}:",
            reinterpret_cast<void*>(SetTypeface) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_FONT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "fontTransferStaticNative", nullptr, reinterpret_cast<void*>(FontTransferStatic) },
        ani_native_function { "getFontAddr", nullptr, reinterpret_cast<void*>(GetFontAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_FONT_NAME);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniFont::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Font> font = std::make_shared<Font>();
    font->SetTypeface(AniTypeface::GetZhCnTypeface());
    AniFont* aniFont = new AniFont(font);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniFont))) {
        ROSEN_LOGE("AniFont::Constructor failed create aniFont");
        delete aniFont;
        return;
    }
}

ani_object AniFont::GetMetrics(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return ani_object{};
    }

    FontMetrics metrics;
    aniFont->GetFont()->GetMetrics(&metrics);
    return CreateAniFontMetrics(env, metrics);
}

ani_double AniFont::GetSize(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return -1;
    }

    return aniFont->GetFont()->GetSize();
}

ani_object AniFont::GetTypeface(ani_env* env, ani_object obj)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return ani_object{};
    }

    std::shared_ptr<Typeface> typeface = aniFont->GetFont()->GetTypeface();
    AniTypeface* aniTypeface = new AniTypeface(typeface);
    ani_object aniObj = CreateAniObject(env, "@ohos.graphics.drawing.drawing.Typeface", nullptr);
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
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniFont->GetFont()->SetSize(size);
}

void AniFont::SetTypeface(ani_env* env, ani_object obj, ani_object typeface)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, obj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, typeface);
    if (aniTypeface == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniFont->GetFont()->SetTypeface(aniTypeface->GetTypeface());
}

ani_object AniFont::FontTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniFont::FontTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniFont::FontTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsFont = reinterpret_cast<JsFont*>(unwrapResult);
    if (jsFont->GetFont() == nullptr) {
        ROSEN_LOGE("AniFont::FontTransferStatic jsFont is null");
        return nullptr;
    }

    auto aniFont = new AniFont(jsFont->GetFont());
    if (ANI_OK != env->Object_SetFieldByName_Long(output, NATIVE_OBJ, reinterpret_cast<ani_long>(aniFont))) {
        ROSEN_LOGE("AniFont::FontTransferStatic failed create aniFont");
        delete aniFont;
        return nullptr;
    }
    return output;
}

ani_long AniFont::GetFontAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, input);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ROSEN_LOGE("AniFont::GetFontAddr aniFont is null");
        return 0;
    }

    return reinterpret_cast<ani_long>(aniFont->GetFontPtrAddr());
}

std::shared_ptr<Font> AniFont::GetFont()
{
    return font_;
}

std::shared_ptr<Font>* AniFont::GetFontPtrAddr()
{
    return &font_;
}

AniFont::~AniFont()
{
    font_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen
