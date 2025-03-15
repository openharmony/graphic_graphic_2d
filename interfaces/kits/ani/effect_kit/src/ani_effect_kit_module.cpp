/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "ani_effect_kit_module.h"

#include "effect_utils.h"

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        EFFECT_LOG_I("[ANI_Constructor] Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    static const char* staticClassName = "L@ohos/effectKit/effectKit;";
    ani_class staticCls;
    if (ANI_OK != env->FindClass(staticClassName, &staticCls)) {
        EFFECT_LOG_I("[ANI_Constructor] FindClass failed");
        return ANI_ERROR;
    }
    std::array staticMethods = {
        ani_native_function { "createEffect", nullptr, reinterpret_cast<void*>(OHOS::Rosen::AniFilter::CreateEffect) },
    };
    if (ANI_OK != env->Class_BindNativeMethods(staticCls, staticMethods.data(), staticMethods.size())) {
        EFFECT_LOG_I("[ANI_Constructor] Class_BindNativeMethods failed");
        return ANI_ERROR;
    };
    OHOS::Rosen::AniFilter::Init(env);
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}