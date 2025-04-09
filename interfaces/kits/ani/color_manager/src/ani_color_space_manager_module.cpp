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

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        ACMLOGE("Unsupported %{public}d", ANI_VERSION_1);
        return ANI_ERROR;
    }

    static const char *staticNsName = "L@ohos/graphics/colorSpaceManager/colorSpaceManager;";
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
        ACMLOGI("AniColorSpaceManager init fail");
        return ANI_ERROR;
    }

    *result = ANI_VERSION_1;
    return ANI_OK;
}
