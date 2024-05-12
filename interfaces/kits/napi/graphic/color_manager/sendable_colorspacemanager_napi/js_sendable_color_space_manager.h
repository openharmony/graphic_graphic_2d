/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_SENDABLE_COLOR_SPACE_MANAGER_H
#define OHOS_JS_SENDABLE_COLOR_SPACE_MANAGER_H

#include "color_space.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ColorManager {
napi_value JsSendableColorSpaceManagerInit(napi_env env, napi_value exportObj);
class JsSendableColorSpaceManager {
public:
    JsSendableColorSpaceManager() {};
    ~JsSendableColorSpaceManager() {};
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value CreateSendableColorSpace(napi_env env, napi_callback_info info);
    static bool ParseColorSpacePrimaries(napi_env env, napi_value jsObject, ColorSpacePrimaries& primaries);
private:
    napi_value OnCreateColorSpace(napi_env env, napi_callback_info info);
};
} // namespace ColorManager
} // namespace OHOS

#endif // OHOS_JS_COLOR_SPACE_MANAGER_H
