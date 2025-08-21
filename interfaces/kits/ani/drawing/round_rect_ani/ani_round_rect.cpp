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

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_ROUND_RECT_NAME = "L@ohos/graphics/drawing/drawing/RoundRect;";

ani_status AniRoundRect::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_ROUND_RECT_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_ROUND_RECT_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", "L@ohos/graphics/common2D/common2D/Rect;DD:V",
            reinterpret_cast<void*>(ConstructorWithRect) },
        ani_native_function { "constructorNative", "L@ohos/graphics/drawing/drawing/RoundRect;:V",
            reinterpret_cast<void*>(ConstructorWithRoundRect) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_ROUND_RECT_NAME);
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

    AniRoundRect* newAniRoundRect = new AniRoundRect(aniRoundRect->GetRoundRect());
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniRoundRect))) {
        ROSEN_LOGE("AniRoundRect::Constructor failed create AniRoundRect");
        delete newAniRoundRect;
        return;
    }
}

RoundRect& AniRoundRect::GetRoundRect()
{
    return roundRect_;
}
} // namespace Drawing
} // namespace OHOS::Rosen