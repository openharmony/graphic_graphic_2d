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

#ifndef JS_TEXT_CONTRAST_H
#define JS_TEXT_CONTRAST_H
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

namespace OHOS::Rosen::SrvText {
class JsTextGlobal final {
public:
    JsTextGlobal() {}

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value SetTextHighContrast(napi_env env, napi_callback_info info);
    static napi_value SetTextNoGlyphShow(napi_env env, napi_callback_info info);
};
}
#endif