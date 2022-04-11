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

#ifndef INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_MANAGER_H
#define INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_MANAGER_H

#include <string>

#include <native_engine/native_engine.h>

#include "rs_window_animation_controller.h"

namespace OHOS {
namespace Rosen {
class RSWindowAnimationManager final {
public:
    RSWindowAnimationManager() = default;
    ~RSWindowAnimationManager() = default;

    static NativeValue* Init(NativeEngine* engine, NativeValue* exportObj);

    static void Finalizer(NativeEngine* engine, void* data, void* hint);

    static NativeValue* SetController(NativeEngine* engine, NativeCallbackInfo* info);

private:
    NativeValue* OnSetController(NativeEngine& engine, NativeCallbackInfo& info);

    std::shared_ptr<AppExecFwk::EventHandler> handler_;
};
} // namespace Rosen
} // namespace OHOS

#endif // INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_MANAGER_H
