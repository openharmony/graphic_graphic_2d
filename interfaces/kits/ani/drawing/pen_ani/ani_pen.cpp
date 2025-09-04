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
#include "ani_pen.h"
#include "color_filter_ani/ani_color_filter.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/pen_napi/js_pen.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_PEN_NAME = "L@ohos/graphics/drawing/drawing/Pen;";

ani_status AniPen::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_PEN_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_PEN_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":V", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "L@ohos/graphics/drawing/drawing/Pen;:V",
            reinterpret_cast<void*>(ConstructorWithPen) },
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
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_PEN_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "penTransferStaticNative", nullptr, reinterpret_cast<void*>(PenTransferStatic) },
        ani_native_function { "getPenAddr", nullptr, reinterpret_cast<void*>(GetPenAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_PEN_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniPen::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Pen> pen = std::make_shared<Pen>();
    AniPen* aniPen = new AniPen(pen);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniPen))) {
        ROSEN_LOGE("AniPen::Constructor failed create aniPen");
        delete aniPen;
        return;
    }
}

void AniPen::ConstructorWithPen(ani_env* env, ani_object obj, ani_object aniPenObj)
{
    auto aniPen = GetNativeFromObj<AniPen>(env, aniPenObj);
    if (aniPen == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }
    std::shared_ptr<Pen> other = aniPen->GetPen();
    std::shared_ptr<Pen> pen = other == nullptr ? std::make_shared<Pen>() : std::make_shared<Pen>(*other);
    AniPen* newAniPen = new AniPen(pen);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniPen))) {
        ROSEN_LOGE("AniPen::Constructor failed create aniPen");
        delete newAniPen;
        return;
    }
}

ani_int AniPen::GetAlpha(ani_env* env, ani_object obj)
{
    auto aniPen = GetNativeFromObj<AniPen>(env, obj);
    if (aniPen == nullptr || aniPen->GetPen() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return -1;
    }

    return aniPen->GetPen()->GetAlpha();
}

void AniPen::Reset(ani_env* env, ani_object obj)
{
    auto aniPen = GetNativeFromObj<AniPen>(env, obj);
    if (aniPen == nullptr || aniPen->GetPen() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniPen->GetPen()->Reset();
}

void AniPen::SetAlpha(ani_env* env, ani_object obj, ani_int alpha)
{
    auto aniPen = GetNativeFromObj<AniPen>(env, obj);
    if (aniPen == nullptr || aniPen->GetPen() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    if (CheckDoubleOutOfRange(alpha, 0, Color::RGB_MAX)) {
        AniThrowError(env, "alpha out of range. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniPen->GetPen()->SetAlpha(alpha);
}

void AniPen::SetBlendMode(ani_env* env, ani_object obj, ani_enum_item aniBlendMode)
{
    auto aniPen = GetNativeFromObj<AniPen>(env, obj);
    if (aniPen == nullptr || aniPen->GetPen() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    ani_int blendMode;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniBlendMode, &blendMode)) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    aniPen->GetPen()->SetBlendMode(static_cast<BlendMode>(blendMode));
}


void AniPen::SetColorFilter(ani_env* env, ani_object obj, ani_object objColorFilter)
{
    auto aniPen = GetNativeFromObj<AniPen>(env, obj);
    if (aniPen == nullptr || aniPen->GetPen() == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    auto aniColorFilter = GetNativeFromObj<AniColorFilter>(env, objColorFilter);
    if (aniColorFilter == nullptr || aniColorFilter->GetColorFilter() == nullptr) {
        ROSEN_LOGE("AniPen::SetColorFilter colorFilter is nullptr");
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    Filter filter = aniPen->GetPen()->GetFilter();
    filter.SetColorFilter(aniColorFilter->GetColorFilter());
    aniPen->GetPen()->SetFilter(filter);
}

ani_object AniPen::PenTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniPen::PenTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniPen::PenTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsPen = reinterpret_cast<JsPen*>(unwrapResult);
    if (jsPen->GetPen() == nullptr) {
        ROSEN_LOGE("AniPen::PenTransferStatic jsPen is null");
        return nullptr;
    }

    auto aniPen = new AniPen(jsPen->GetPen());
    if (ANI_OK != env->Object_SetFieldByName_Long(output, NATIVE_OBJ, reinterpret_cast<ani_long>(aniPen))) {
        ROSEN_LOGE("AniPen::PenTransferStatic failed create aniPen");
        delete aniPen;
        return nullptr;
    }
    return output;
}

ani_long AniPen::GetPenAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniPen = GetNativeFromObj<AniPen>(env, input);
    if (aniPen == nullptr || aniPen->GetPen() == nullptr) {
        ROSEN_LOGE("AniPen::GetPenAddr aniPen is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniPen->GetPenPtrAddr());
}

std::shared_ptr<Pen>* AniPen::GetPenPtrAddr()
{
    return &pen_;
}

AniPen::~AniPen()
{
    pen_ = nullptr;
}

std::shared_ptr<Pen> AniPen::GetPen()
{
    return pen_;
}
} // namespace Drawing
} // namespace OHOS::Rosen