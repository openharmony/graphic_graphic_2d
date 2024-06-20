/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "js_matrix.h"
#include "js_drawing_utils.h"
#include "native_value.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsMatrix::constructor_ = nullptr;
const std::string CLASS_NAME = "Matrix";
napi_value JsMatrix::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("preRotate", JsMatrix::PreRotate),
        DECLARE_NAPI_FUNCTION("preScale", JsMatrix::PreScale),
        DECLARE_NAPI_FUNCTION("preTranslate", JsMatrix::PreTranslate),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Init Failed to define Matrix class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsMatrix::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsMatrix* jsMatrix = new(std::nothrow) JsMatrix(std::make_shared<Matrix>());
    if (jsMatrix == nullptr) {
        ROSEN_LOGE("JsMatrix::Constructor Failed to create jsMatrix");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsMatrix, JsMatrix::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsMatrix;
        ROSEN_LOGE("JsMatrix::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsMatrix::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsMatrix* napi = reinterpret_cast<JsMatrix*>(nativeObject);
        delete napi;
    }
}

napi_value JsMatrix::PreRotate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreRotate(env, info) : nullptr;
}

napi_value JsMatrix::OnPreRotate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreRotate matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double degree = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, degree);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, py);

    JS_CALL_DRAWING_FUNC(m_matrix->PreRotate(degree, px, py));

    return nullptr;
}

napi_value JsMatrix::PreScale(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreScale(env, info) : nullptr;
}

napi_value JsMatrix::OnPreScale(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreScale matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double sx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, sx);
    double sy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, sy);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py);

    JS_CALL_DRAWING_FUNC(m_matrix->PreScale(sx, sy, px, py));

    return nullptr;
}

napi_value JsMatrix::PreTranslate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreTranslate(env, info) : nullptr;
}

napi_value JsMatrix::OnPreTranslate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreTranslate matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    JS_CALL_DRAWING_FUNC(m_matrix->PreTranslate(dx, dy));

    return nullptr;
}

JsMatrix::~JsMatrix()
{
    m_matrix = nullptr;
}

std::shared_ptr<Matrix> JsMatrix::GetMatrix()
{
    return m_matrix;
}
}
}