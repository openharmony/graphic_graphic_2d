/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <native_engine/native_engine.h>

#include "rs_window_animation_manager.h"

extern "C" __attribute__((constructor)) void NAPI_animation_windowAnimationManager_AutoRegister()
{
    auto moduleManager = NativeModuleManager::GetInstance();
    NativeModule newModuleInfo = {
        .name = "animation.windowAnimationManager",
        .fileName = "animation/libanimation_napi.so/windowAnimationManager.js",
        .registerCallback = OHOS::Rosen::RSWindowAnimationManager::Init,
    };
    moduleManager->Register(&newModuleInfo);
}
