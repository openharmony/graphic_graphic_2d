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

#include "ani_round_rect.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/roundRect_napi/js_roundrect.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_ROUND_RECT_NAME = "@ohos.graphics.drawing.drawing.RoundRect";

ani_status AniRoundRect::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_ROUND_RECT_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_ROUND_RECT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", "C{@ohos.graphics.common2D.common2D.Rect}dd:",
            reinterpret_cast<void*>(ConstructorWithRect) },
        ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.RoundRect}:",
            reinterpret_cast<void*>(ConstructorWithRoundRect) },
        ani_native_function { "setCorner", "C{@ohos.graphics.drawing.drawing.CornerPos}dd:",
            reinterpret_cast<void*>(SetCorner) },
        ani_native_function { "getCorner", "C{@ohos.graphics.drawing.drawing.CornerPos}:"
            "C{@ohos.graphics.common2D.common2D.Point}", reinterpret_cast<void*>(GetCorner) },
        ani_native_function { "offset", "dd:", reinterpret_cast<void*>(Offset) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_ROUND_RECT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "roundRectTransferStaticNative", nullptr,
            reinterpret_cast<void*>(RoundRectTransferStatic) },
        ani_native_function { "getRoundRectAddr", nullptr, reinterpret_cast<void*>(GetRoundRectAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_ROUND_RECT_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniRoundRect::ConstructorWithRect(ani_env* env, ani_object obj, ani_object aniRectObj, ani_double xRadii,
    ani_double yRadii)
{
    Drawing::Rect drawingRect;
    if (!GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
        AniThrowError(env, "Invalid params. ");
        return;
    }
    AniRoundRect* aniRoundRect = new AniRoundRect(drawingRect, xRadii, yRadii);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniRoundRect))) {
        ROSEN_LOGE("AniRoundRect::Constructor failed create AniRoundRect");
        delete aniRoundRect;
        return;
    }
}

void AniRoundRect::ConstructorWithRoundRect(ani_env* env, ani_object obj, ani_object aniRoundRectObj)
{
    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, aniRoundRectObj);
    if (aniRoundRect == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }
    std::shared_ptr<RoundRect> other = aniRoundRect->GetRoundRect();
    std::shared_ptr<RoundRect> roundRect = other == nullptr ? std::make_shared<RoundRect>() :
        std::make_shared<RoundRect>(*other);
    AniRoundRect* newAniRoundRect = new AniRoundRect(roundRect);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniRoundRect))) {
        ROSEN_LOGE("AniRoundRect::Constructor failed create AniRoundRect");
        delete newAniRoundRect;
        return;
    }
}

void AniRoundRect::SetCorner(ani_env* env, ani_object obj, ani_enum_item aniPos, ani_double x, ani_double y)
{
    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, obj);
    if (aniRoundRect == nullptr || aniRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRoundRect::SetCorner aniRoundRect is nullptr.");
        return;
    }
    ani_int pos;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniPos, &pos)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRoundRect::SetCorner get CornerPos enum failed.");
        return;
    }
    aniRoundRect->GetRoundRect()->SetCornerRadius(static_cast<RoundRect::CornerPos>(pos), x, y);
}

ani_object AniRoundRect::GetCorner(ani_env* env, ani_object obj, ani_enum_item aniPos)
{
    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, obj);
    if (aniRoundRect == nullptr || aniRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRoundRect::GetCorner aniRoundRect is nullptr.");
        return CreateAniUndefined(env);
    }
    ani_int pos;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniPos, &pos)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniRoundRect::GetCorner get CornerPos enum failed.");
        return CreateAniUndefined(env);
    }
    auto point = aniRoundRect->GetRoundRect()->GetCornerRadius(static_cast<RoundRect::CornerPos>(pos));
    ani_object aniObj = nullptr;
    if (CreatePointObj(env, point, aniObj) != ANI_OK) {
        return CreateAniUndefined(env);
    }
    return aniObj;
}

void AniRoundRect::Offset(ani_env* env, ani_object obj, ani_double x, ani_double y)
{
    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, obj);
    if (aniRoundRect == nullptr || aniRoundRect->GetRoundRect() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniRoundRect::Offset aniRoundRect is nullptr.");
        return;
    }
    aniRoundRect->GetRoundRect()->Offset(x, y);
}

ani_object AniRoundRect::RoundRectTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniRoundRect::RoundRectTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniRoundRect::RoundRectTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsRoundRect = reinterpret_cast<JsRoundRect*>(unwrapResult);
    if (jsRoundRect->GetRoundRectPtr() == nullptr) {
        ROSEN_LOGE("AniRoundRect::RoundRectTransferStatic jsRoundRect is null");
        return nullptr;
    }

    auto aniRoundRect = new AniRoundRect(jsRoundRect->GetRoundRectPtr());
    ani_object aniRoundRectObj = CreateAniObject(env, ANI_CLASS_ROUND_RECT_NAME, ":V");
    if (ANI_OK != env->Object_SetFieldByName_Long(aniRoundRectObj,
        NATIVE_OBJ, reinterpret_cast<ani_long>(aniRoundRect))) {
        ROSEN_LOGE("AniRoundRect::RoundRectTransferStatic failed create aniRoundRect");
        delete aniRoundRect;
        return nullptr;
    }
    return aniRoundRectObj;
}

ani_long AniRoundRect::GetRoundRectAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniRoundRect = GetNativeFromObj<AniRoundRect>(env, input);
    if (aniRoundRect == nullptr || aniRoundRect->GetRoundRect() == nullptr) {
        ROSEN_LOGE("AniRoundRect::GetRoundRectAddr aniRoundRect is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniRoundRect->GetRoundRectPtrAddr());
}

std::shared_ptr<RoundRect>* AniRoundRect::GetRoundRectPtrAddr()
{
    return &roundRect_;
}

std::shared_ptr<RoundRect> AniRoundRect::GetRoundRect()
{
    return roundRect_;
}

AniRoundRect::~AniRoundRect()
{
    roundRect_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen