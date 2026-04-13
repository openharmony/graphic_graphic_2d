/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ani_point_utils.h"

namespace OHOS::Rosen {
namespace Drawing {

ani_status AniPointUtils::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().pointUtils;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_POINT_UTILS_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "negate", nullptr, reinterpret_cast<void*>(Negate) },
        ani_native_function { "offset", nullptr, reinterpret_cast<void*>(Offset) },
    };

    ani_status ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_POINT_UTILS_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniPointUtils::Negate(ani_env* env, ani_object obj, ani_object aniPointObj)
{
    Drawing::Point drawingPoint;
    if (GetPointFromPointObj(env, aniPointObj, drawingPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniPointUtils::Negate incorrect parameter0 type.");
        return;
    }

    drawingPoint.Negate();
    if (!DrawingPointConvertToAniPoint(env, aniPointObj, drawingPoint)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniPointUtils::Negate cannot fill 'point' Point type.");
        return;
    }
}

void AniPointUtils::Offset(ani_env* env, ani_object obj, ani_object aniPointObj, ani_double dx, ani_double dy)
{
    Drawing::Point drawingPoint;
    if (GetPointFromPointObj(env, aniPointObj, drawingPoint) != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniPointUtils::Offset incorrect parameter0 type.");
        return;
    }

    drawingPoint.Offset(dx, dy);
    if (!DrawingPointConvertToAniPoint(env, aniPointObj, drawingPoint)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniPointUtils::Offset cannot fill 'point' Point type.");
        return;
    }
}

} // namespace Drawing
} // namespace OHOS::Rosen
