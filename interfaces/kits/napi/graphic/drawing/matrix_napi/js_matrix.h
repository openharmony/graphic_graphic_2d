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

#ifndef OHOS_ROSEN_JS_MATRIX_H
#define OHOS_ROSEN_JS_MATRIX_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "utils/matrix.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsMatrix final {
public:
    explicit JsMatrix(std::shared_ptr<Matrix> matrix)
        : m_matrix(matrix) {};
    ~JsMatrix();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    static napi_value PreRotate(napi_env env, napi_callback_info info);
    static napi_value PreScale(napi_env env, napi_callback_info info);
    static napi_value PreTranslate(napi_env env, napi_callback_info info);

    std::shared_ptr<Matrix> GetMatrix();
private:

    napi_value OnPreRotate(napi_env env, napi_callback_info info);
    napi_value OnPreScale(napi_env env, napi_callback_info info);
    napi_value OnPreTranslate(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    std::shared_ptr<Matrix> m_matrix = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_MATRIX_H