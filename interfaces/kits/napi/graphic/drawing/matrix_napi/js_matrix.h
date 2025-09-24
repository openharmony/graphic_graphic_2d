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
    static napi_value CreateJsMatrix(napi_env env, const std::shared_ptr<Matrix> matrix);
    static napi_value GetValue(napi_env env, napi_callback_info info);
    static napi_value PostConcat(napi_env env, napi_callback_info info);
    static napi_value PostRotate(napi_env env, napi_callback_info info);
    static napi_value PostSkew(napi_env env, napi_callback_info info);
    static napi_value PostTranslate(napi_env env, napi_callback_info info);
    static napi_value PreRotate(napi_env env, napi_callback_info info);
    static napi_value PreScale(napi_env env, napi_callback_info info);
    static napi_value PreSkew(napi_env env, napi_callback_info info);
    static napi_value PreTranslate(napi_env env, napi_callback_info info);
    static napi_value SetConcat(napi_env env, napi_callback_info info);
    static napi_value SetRotation(napi_env env, napi_callback_info info);
    static napi_value SetSinCos(napi_env env, napi_callback_info info);
    static napi_value SetSkew(napi_env env, napi_callback_info info);
    static napi_value MapPoints(napi_env env, napi_callback_info info);
    static napi_value MapRadius(napi_env env, napi_callback_info info);
    static napi_value PostScale(napi_env env, napi_callback_info info);
    static napi_value RectStaysRect(napi_env env, napi_callback_info info);
    static napi_value Reset(napi_env env, napi_callback_info info);
    static napi_value SetPolyToPoly(napi_env env, napi_callback_info info);
    static napi_value SetRectToRect(napi_env env, napi_callback_info info);
    static napi_value MapRect(napi_env env, napi_callback_info info);
    static napi_value SetScale(napi_env env, napi_callback_info info);
    static napi_value SetTranslation(napi_env env, napi_callback_info info);
    static napi_value PreConcat(napi_env env, napi_callback_info info);
    static napi_value IsEqual(napi_env env, napi_callback_info info);
    static napi_value Invert(napi_env env, napi_callback_info info);
    static napi_value IsAffine(napi_env env, napi_callback_info info);
    static napi_value IsIdentity(napi_env env, napi_callback_info info);
    static napi_value SetMatrix(napi_env env, napi_callback_info info);

    std::shared_ptr<Matrix> GetMatrix();
    static napi_value GetAll(napi_env env, napi_callback_info info);
private:

    napi_value OnGetValue(napi_env env, napi_callback_info info);
    napi_value OnPostConcat(napi_env env, napi_callback_info info);
    napi_value OnPostRotate(napi_env env, napi_callback_info info);
    napi_value OnPostSkew(napi_env env, napi_callback_info info);
    napi_value OnPostTranslate(napi_env env, napi_callback_info info);
    napi_value OnPreRotate(napi_env env, napi_callback_info info);
    napi_value OnPreScale(napi_env env, napi_callback_info info);
    napi_value OnPreSkew(napi_env env, napi_callback_info info);
    napi_value OnPreTranslate(napi_env env, napi_callback_info info);
    napi_value OnSetConcat(napi_env env, napi_callback_info info);
    napi_value OnSetRotation(napi_env env, napi_callback_info info);
    napi_value OnSetSinCos(napi_env env, napi_callback_info info);
    napi_value OnSetSkew(napi_env env, napi_callback_info info);
    napi_value OnMapPoints(napi_env env, napi_callback_info info);
    napi_value OnMapRadius(napi_env env, napi_callback_info info);
    napi_value OnPostScale(napi_env env, napi_callback_info info);
    napi_value OnRectStaysRect(napi_env env, napi_callback_info info);
    napi_value OnReset(napi_env env, napi_callback_info info);
    napi_value OnGetAll(napi_env env, napi_callback_info info);
    napi_value OnSetPolyToPoly(napi_env env, napi_callback_info info);
    napi_value OnSetRectToRect(napi_env env, napi_callback_info info);
    napi_value OnMapRect(napi_env env, napi_callback_info info);
    napi_value OnSetScale(napi_env env, napi_callback_info info);
    napi_value OnSetTranslation(napi_env env, napi_callback_info info);
    napi_value OnPreConcat(napi_env env, napi_callback_info info);
    napi_value OnIsAffine(napi_env env, napi_callback_info info);
    napi_value OnIsEqual(napi_env env, napi_callback_info info);
    napi_value OnInvert(napi_env env, napi_callback_info info);
    napi_value OnIsIdentity(napi_env env, napi_callback_info info);
    napi_value OnSetMatrix(napi_env env, napi_callback_info info);

    static thread_local napi_ref constructor_;
    std::shared_ptr<Matrix> m_matrix = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_MATRIX_H
