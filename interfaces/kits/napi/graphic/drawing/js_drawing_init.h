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

#ifndef OHOS_JS_DRAWING_INIT_H
#define OHOS_JS_DRAWING_INIT_H

#include <map>

#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"

#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
DRAWING_API napi_value DrawingInit(napi_env env, napi_value exportObj);
napi_value CreateFont(napi_env env);
class JsDraw final {
public:
    static napi_value CreateFont(napi_env env, napi_callback_info info);
private:
    napi_value OnCreateFont(napi_env env, napi_callback_info info);
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_JS_DRAWING_INIT_H