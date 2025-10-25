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
#include "ani_color_space_manager.h"
#include "ani_color_space_utils.h"

using namespace std;
using namespace OHOS::ColorManager;

ani_status CacheColorSpaceManagerObjects(ani_env *env)
{
    if (ANI_OK != env->FindEnum("@ohos.graphics.colorSpaceManager.colorSpaceManager.ColorSpace",
        &AniColorSpaceManager::enumType_)) {
        ACMLOGE("[ANI]Find Enum Failed");
        return ANI_ERROR;
    }

    ani_enum_item enumItem = nullptr;
    for (auto& iter : NATIVE_TO_STRING_MAP) {
        env->Enum_GetEnumItemByName(AniColorSpaceManager::enumType_, iter.second.c_str(), &enumItem);
        AniColorSpaceManager::nativeToEnumMap_.emplace(iter.first, enumItem);
    }

    if (ANI_OK != env->FindClass("@ohos.graphics.colorSpaceManager.colorSpaceManager.ColorSpaceManagerInner",
        &AniColorSpaceManager::colorSpaceManagerClass_)) {
        ACMLOGE("[ANI]FindClass Failed");
        return ANI_ERROR;
    }

    if (ANI_OK != env->Class_FindField(AniColorSpaceManager::colorSpaceManagerClass_, "nativePtr",
        &AniColorSpaceManager::nativePtrField_)) {
        ACMLOGE("[ANI]FindField Failed");
        return ANI_ERROR;
    }

    if (ANI_OK != env->Class_FindMethod(AniColorSpaceManager::colorSpaceManagerClass_, "makePoint", nullptr,
        &AniColorSpaceManager::makePointMethod_)) {
        ACMLOGE("[ANI]FindMethod Failed");
        return ANI_ERROR;
    }
    return ANI_OK;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        ACMLOGE("Unsupported %{public}d", ANI_VERSION_1);
        return ANI_ERROR;
    }

    static const char *staticNsName = "@ohos.graphics.colorSpaceManager.colorSpaceManager";
    ani_namespace kitNs;
    if (ANI_OK != env->FindNamespace(staticNsName, &kitNs)) {
        ACMLOGE("FindNamespace err");
        return ANI_ERROR;
    }

    std::array kitFunctions = {
        ani_native_function {
            "createByColorSpace",
            nullptr,
            reinterpret_cast<void *>(AniColorSpaceManager::CreateByColorSpace)},
        ani_native_function {
            "createByColorSpacePrimaries",
            nullptr,
            reinterpret_cast<void *>(AniColorSpaceManager::CreateByColorSpacePrimaries)},
    };

    if (ANI_OK != env->Namespace_BindNativeFunctions(kitNs, kitFunctions.data(), kitFunctions.size())) {
        ACMLOGE("Cannot bind native methods to %{public}s", staticNsName);
        return ANI_ERROR;
    }

    if (ANI_OK != AniColorSpaceManager::AniColorSpaceManagerInit(env)) {
        ACMLOGE("AniColorSpaceManager init failed");
        return ANI_ERROR;
    }

    if (ANI_OK != CacheColorSpaceManagerObjects(env)) {
        ACMLOGE("CacheColorSpaceManagerObjects failed");
        return ANI_ERROR;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}
