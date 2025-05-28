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

#include "ani_region.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_REGION_NAME = "L@ohos/graphics/drawing/drawing/Region;";

ani_status AniRegion::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_REGION_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "<ctor>", ":V", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "<ctor>", "L@ohos/graphics/drawing/drawing/Region;:V",
            reinterpret_cast<void*>(ConstructorWithRegion) },
        ani_native_function { "<ctor>", "DDDD:V", reinterpret_cast<void*>(ConstructorWithRect) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniRegion::Constructor(ani_env* env, ani_object obj)
{
    AniRegion* aniRegion = new AniRegion();
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete aniRegion;
        return;
    }
}

void AniRegion::ConstructorWithRegion(ani_env* env, ani_object obj, ani_object aniRegionObj)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, aniRegionObj);
    if (aniRegion == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    AniRegion* newAniRegion = new AniRegion(aniRegion->GetRegion());
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete newAniRegion;
        return;
    }
}

void AniRegion::ConstructorWithRect(ani_env* env, ani_object obj, ani_double left, ani_double top, ani_double right,
    ani_double bottom)
{
    AniRegion* aniRegion = new AniRegion();
    RectI rect = Drawing::RectI(left, top, right, bottom);
    aniRegion->GetRegion().SetRect(rect);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete aniRegion;
        return;
    }
}


Region& AniRegion::GetRegion()
{
    return region_;
}
} // namespace Drawing
} // namespace OHOS::Rosen