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

#ifndef OHOS_ROSEN_JS_LINE_TYPESET_H
#define OHOS_ROSEN_JS_LINE_TYPESET_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "line_typography.h"
#include "typography.h"

namespace OHOS::Rosen {
class JsLineTypeset final {
public:
    explicit JsLineTypeset(std::shared_ptr<LineTypography> lineTypography);
    explicit JsLineTypeset();
    ~JsLineTypeset();

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsLineTypeset(napi_env env, std::unique_ptr<LineTypography> lineTypography);
    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value GetLineBreak(napi_env env, napi_callback_info info);
    static napi_value CreateLine(napi_env env, napi_callback_info info);

private:
    static bool CreateConstructor(napi_env env);
    static thread_local napi_ref constructor_;
    static std::mutex constructorMutex_;
    napi_value OnGetLineBreak(napi_env env, napi_callback_info info);
    napi_value OnCreateLine(napi_env env, napi_callback_info info);
    std::shared_ptr<LineTypography> lineTypography_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_LINE_TYPESET_H
