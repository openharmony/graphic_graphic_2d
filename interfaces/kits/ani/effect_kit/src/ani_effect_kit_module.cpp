/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "ani_color_picker.h"

#include "effect_utils.h"

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        EFFECT_LOG_I("[ANI_Constructor] Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    static const char* staticClassName = "L@ohos/effectKit/effectKit;";
    ani_namespace effectKitNamespace;
    if (env->FindNamespace(staticClassName, &effectKitNamespace) != ANI_OK) {
        EFFECT_LOG_I("[ANI_Constructor] FindNamespace failed");
        return ANI_ERROR;
    }
    std::array staticMethods = {
        ani_native_function { "createEffect", nullptr, reinterpret_cast<void*>(OHOS::Rosen::AniFilter::CreateEffect) },
        ani_native_function { "createColorPicker1", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniColorPicker::createColorPicker1) },
        ani_native_function { "createColorPicker2", nullptr,
            reinterpret_cast<void*>(OHOS::Rosen::AniColorPicker::createColorPicker2) }
    };
    if (env->Namespace_BindNativeFunctions(effectKitNamespace, staticMethods.data(), staticMethods.size()) != ANI_OK) {
        EFFECT_LOG_I("[ANI_Constructor] Namespace_BindNativeFunctions failed");
        return ANI_ERROR;
    };
    OHOS::Rosen::AniFilter::Init(env);
    OHOS::Rosen::AniColorPicker::Init(env);
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}