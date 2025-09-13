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
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/region_napi/js_region.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_REGION_NAME = "@ohos.graphics.drawing.drawing.Region";

ani_status AniRegion::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_REGION_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.Region}:",
            reinterpret_cast<void*>(ConstructorWithRegion) },
        ani_native_function { "constructorNative", "iiii:", reinterpret_cast<void*>(ConstructorWithRect) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "regionTransferStaticNative", nullptr, reinterpret_cast<void*>(RegionTransferStatic) },
        ani_native_function { "getRegionAddr", nullptr, reinterpret_cast<void*>(GetRegionAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_REGION_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniRegion::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Region> region = std::make_shared<Region>();
    AniRegion* aniRegion = new AniRegion(region);
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
    std::shared_ptr<Region> other = aniRegion->GetRegion();
    std::shared_ptr<Region> region = other == nullptr ? std::make_shared<Region>() : std::make_shared<Region>(*other);
    AniRegion* newAniRegion = new AniRegion(region);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete newAniRegion;
        return;
    }
}

void AniRegion::ConstructorWithRect(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
    ani_int bottom)
{
    std::shared_ptr<Region> region = std::make_shared<Region>();
    AniRegion* aniRegion = new AniRegion(region);
    RectI rect = Drawing::RectI(left, top, right, bottom);
    aniRegion->GetRegion()->SetRect(rect);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniRegion))) {
        ROSEN_LOGE("AniRegion::Constructor failed create AniRegion");
        delete aniRegion;
        return;
    }
}

ani_object AniRegion::RegionTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniRegion::RegionTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniRegion::RegionTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsRegion = reinterpret_cast<JsRegion*>(unwrapResult);
    if (jsRegion->GetRegionPtr() == nullptr) {
        ROSEN_LOGE("AniRegion::RegionTransferStatic jsRegion is null");
        return nullptr;
    }

    auto aniRegion = new AniRegion(jsRegion->GetRegionPtr());
    if (ANI_OK != env->Object_SetFieldByName_Long(output, NATIVE_OBJ, reinterpret_cast<ani_long>(aniRegion))) {
        ROSEN_LOGE("AniFont::RegionTransferStatic failed create aniFont");
        delete aniRegion;
        return nullptr;
    }
    return output;
}

ani_long AniRegion::GetRegionAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniRegion = GetNativeFromObj<AniRegion>(env, input);
    if (aniRegion == nullptr || aniRegion->GetRegion() == nullptr) {
        ROSEN_LOGE("AniRegion::GetRegionAddr aniRegion is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniRegion->GetRegionPtrAddr());
}

std::shared_ptr<Region>* AniRegion::GetRegionPtrAddr()
{
    return &region_;
}

std::shared_ptr<Region> AniRegion::GetRegion()
{
    return region_;
}

AniRegion::~AniRegion()
{
    region_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen