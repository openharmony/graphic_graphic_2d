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

#ifndef OHOS_ROSEN_JS_STRUT_STYLE_H
#define OHOS_ROSEN_JS_STRUT_STYLE_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "napi_common.h"
#include "text_line_base.h"
#include "typography_style.h"
#include "typography.h"

namespace OHOS::Rosen {
class JsStrutStyleManager final {
public:
    JsStrutStyleManager() = default;
    ~JsStrutStyleManager() = default;
    static void Destructor(napi_env env, void *data, void *hint);
    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value IsStrutStyleEqual(napi_env env, napi_callback_info info);

private:
    static thread_local napi_ref constructor_;
};

} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_STRUT_STYLE_H