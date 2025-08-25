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

#include "ani_brush.h"
#include "color_filter_ani/ani_color_filter.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/brush_napi/js_brush.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_BRUSH_NAME = "L@ohos/graphics/drawing/drawing/Brush;";

ani_status AniBrush::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_BRUSH_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_BRUSH_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":V", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "L@ohos/graphics/drawing/drawing/Brush;:V",
            reinterpret_cast<void*>(ConstructorWithBrush) },
        ani_native_function { "getAlpha", ":I", reinterpret_cast<void*>(GetAlpha) },
        ani_native_function { "reset", ":V", reinterpret_cast<void*>(Reset) },
        ani_native_function { "setAlpha", "I:V", reinterpret_cast<void*>(SetAlpha) },
        ani_native_function { "setBlendMode", "L@ohos/graphics/drawing/drawing/BlendMode;:V",
            reinterpret_cast<void*>(SetBlendMode) },
        ani_native_function { "setColorFilter", "L@ohos/graphics/drawing/drawing/ColorFilter;:V",
            reinterpret_cast<void*>(SetColorFilter) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_BRUSH_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "brushTransferStaticNative", nullptr, reinterpret_cast<void*>(BrushTransferStatic) },
        ani_native_function { "getBrushAddr", nullptr, reinterpret_cast<void*>(GetBrushAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_BRUSH_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniBrush::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Brush> brush = std::make_shared<Brush>();
    AniBrush* aniBrush = new AniBrush(brush);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniBrush))) {
        ROSEN_LOGE("AniBrush::Constructor failed create aniBrush");
        delete aniBrush;
        return;
    }
}

void AniBrush::ConstructorWithBrush(ani_env* env, ani_object obj, ani_object aniBrushObj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, aniBrushObj);
    if (aniBrush == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }
    std::shared_ptr<Brush> other = aniBrush->GetBrush();
    std::shared_ptr<Brush> brush = other == nullptr ? std::make_shared<Brush>() : std::make_shared<Brush>(*other);
    AniBrush* newAniBrush = new AniBrush(brush);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniBrush))) {
        ROSEN_LOGE("AniBrush::Constructor failed create aniBrush");
        delete newAniBrush;
        return;
    }
}

ani_int AniBrush::GetAlpha(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return -1;
    }

    return aniBrush->GetBrush()->GetAlpha();
}

void AniBrush::Reset(ani_env* env, ani_object obj)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniBrush->GetBrush()->Reset();
}

void AniBrush::SetAlpha(ani_env* env, ani_object obj, ani_int alpha)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    if (CheckInt32OutOfRange(alpha, 0, Color::RGB_MAX)) {
        AniThrowError(env, "alpha out of range. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniBrush->GetBrush()->SetAlpha(alpha);
}

void AniBrush::SetBlendMode(ani_env* env, ani_object obj, ani_enum_item aniBlendMode)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendMode)) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniBrush->GetBrush()->SetBlendMode(static_cast<BlendMode>(blendMode));
}

std::shared_ptr<Brush> AniBrush::GetBrush()
{
    return brush_;
}

std::shared_ptr<Brush>* AniBrush::GetBrushPtrAddr()
{
    return &brush_;
}

AniBrush::~AniBrush()
{
    brush_ = nullptr;
}

void AniBrush::SetColorFilter(ani_env* env, ani_object obj, ani_object objColorFilter)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, obj);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    auto aniColorFilter = GetNativeFromObj<AniColorFilter>(env, objColorFilter);
    if (aniColorFilter == nullptr || aniColorFilter->GetColorFilter() == nullptr) {
        ROSEN_LOGE("AniBrush::SetColorFilter colorFilter is nullptr");
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    Filter filter = aniBrush->GetBrush()->GetFilter();
    filter.SetColorFilter(aniColorFilter->GetColorFilter());
    aniBrush->GetBrush()->SetFilter(filter);
}

ani_object AniBrush::BrushTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsBrush = reinterpret_cast<JsBrush*>(unwrapResult);
    if (jsBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic jsBrush is null");
        return nullptr;
    }
    auto aniBrush = new AniBrush(jsBrush->GetBrush());
    if (ANI_OK != env->Object_SetFieldByName_Long(output, NATIVE_OBJ, reinterpret_cast<ani_long>(aniBrush))) {
        ROSEN_LOGE("AniBrush::BrushTransferStatic failed create aniBrush");
        delete aniBrush;
        return nullptr;
    }
    return output;
}

ani_long AniBrush::GetBrushAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniBrush = GetNativeFromObj<AniBrush>(env, input);
    if (aniBrush == nullptr || aniBrush->GetBrush() == nullptr) {
        ROSEN_LOGE("AniBrush::GetBrushAddr aniBrush is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniBrush->GetBrushPtrAddr());
}
} // namespace Drawing
} // namespace OHOS::Rosen