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

static constexpr uint32_t POLY_POINT_COUNT_MAX = 4;

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsMatrix::constructor_ = nullptr;
const std::string CLASS_NAME = "Matrix";

static const napi_property_descriptor g_properties[] = {
    DECLARE_NAPI_FUNCTION("getValue", JsMatrix::GetValue),
    DECLARE_NAPI_FUNCTION("postConcat", JsMatrix::PostConcat),
    DECLARE_NAPI_FUNCTION("postRotate", JsMatrix::PostRotate),
    DECLARE_NAPI_FUNCTION("postSkew", JsMatrix::PostSkew),
    DECLARE_NAPI_FUNCTION("postTranslate", JsMatrix::PostTranslate),
    DECLARE_NAPI_FUNCTION("preRotate", JsMatrix::PreRotate),
    DECLARE_NAPI_FUNCTION("preScale", JsMatrix::PreScale),
    DECLARE_NAPI_FUNCTION("preSkew", JsMatrix::PreSkew),
    DECLARE_NAPI_FUNCTION("preTranslate", JsMatrix::PreTranslate),
    DECLARE_NAPI_FUNCTION("setConcat", JsMatrix::SetConcat),
    DECLARE_NAPI_FUNCTION("setRotation", JsMatrix::SetRotation),
    DECLARE_NAPI_FUNCTION("setScale", JsMatrix::SetScale),
    DECLARE_NAPI_FUNCTION("setSinCos", JsMatrix::SetSinCos),
    DECLARE_NAPI_FUNCTION("setSkew", JsMatrix::SetSkew),
    DECLARE_NAPI_FUNCTION("setTranslation", JsMatrix::SetTranslation),
    DECLARE_NAPI_FUNCTION("setMatrix", JsMatrix::SetMatrix),
    DECLARE_NAPI_FUNCTION("preConcat", JsMatrix::PreConcat),
    DECLARE_NAPI_FUNCTION("isAffine", JsMatrix::IsAffine),
    DECLARE_NAPI_FUNCTION("isEqual", JsMatrix::IsEqual),
    DECLARE_NAPI_FUNCTION("invert", JsMatrix::Invert),
    DECLARE_NAPI_FUNCTION("isIdentity", JsMatrix::IsIdentity),
    DECLARE_NAPI_FUNCTION("mapPoints", JsMatrix::MapPoints),
    DECLARE_NAPI_FUNCTION("mapRadius", JsMatrix::MapRadius),
    DECLARE_NAPI_FUNCTION("postScale", JsMatrix::PostScale),
    DECLARE_NAPI_FUNCTION("rectStaysRect", JsMatrix::RectStaysRect),
    DECLARE_NAPI_FUNCTION("reset", JsMatrix::Reset),
    DECLARE_NAPI_FUNCTION("getAll", JsMatrix::GetAll),
    DECLARE_NAPI_FUNCTION("setPolyToPoly", JsMatrix::SetPolyToPoly),
    DECLARE_NAPI_FUNCTION("setRectToRect", JsMatrix::SetRectToRect),
    DECLARE_NAPI_FUNCTION("mapRect", JsMatrix::MapRect),
};

napi_value JsMatrix::Init(napi_env env, napi_value exportObj)
{
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(g_properties) / sizeof(g_properties[0]), g_properties, &constructor);
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
    size_t argCount = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsMatrix::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    std::shared_ptr<Matrix> matrix = nullptr;
    if (argCount == ARGC_ONE) {
        JsMatrix* jsOther = nullptr;
        GET_UNWRAP_PARAM(ARGC_ZERO, jsOther);
        std::shared_ptr<Matrix> other = jsOther->GetMatrix();
        matrix = other == nullptr ? std::make_shared<Matrix>() : std::make_shared<Matrix>(*other);
    } else {
        matrix = std::make_shared<Matrix>();
    }
    JsMatrix* jsMatrix = new JsMatrix(matrix);
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

napi_value JsMatrix::CreateJsMatrix(napi_env env, const std::shared_ptr<Matrix> matrix)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        auto jsMatrix = new JsMatrix(matrix);
        napi_create_object(env, &result);
        if (result == nullptr) {
            delete jsMatrix;
            ROSEN_LOGE("JsMatrix::CreateJsMatrix Create matrix object failed!");
            return nullptr;
        }
        status = napi_wrap(env, result, jsMatrix, JsMatrix::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            delete jsMatrix;
            ROSEN_LOGE("JsMatrix::CreateJsMatrix failed to wrap native instance");
            return nullptr;
        }
        napi_define_properties(env, result, sizeof(g_properties) / sizeof(g_properties[0]), g_properties);
        return result;
    }
    return result;
}

napi_value JsMatrix::GetValue(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnGetValue(env, info) : nullptr;
}

napi_value JsMatrix::OnGetValue(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnGetValue matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t index = -1;
    GET_INT32_PARAM(ARGC_ZERO, index);

    if (index < 0 || index >= Matrix::MATRIX_SIZE) {
        ROSEN_LOGE("JsMatrix::OnGetValue index is out of range");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double value = m_matrix->Get(index);
    return GetDoubleAndConvertToJsValue(env, value);
}

napi_value JsMatrix::PostConcat(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostConcat(env, info) : nullptr;
}

napi_value JsMatrix::OnPostConcat(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostConcat matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);

    std::shared_ptr<Matrix> matrix = jsMatrix->GetMatrix();
    if (matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostConcat matrix is nullptr");
        return nullptr;
    }
    m_matrix->PostConcat(*matrix);
    return nullptr;
}

napi_value JsMatrix::PostRotate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostRotate(env, info) : nullptr;
}

napi_value JsMatrix::OnPostRotate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostRotate matrix is nullptr");
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

    JS_CALL_DRAWING_FUNC(m_matrix->PostRotate(degree, px, py));

    return nullptr;
}

napi_value JsMatrix::PostSkew(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostSkew(env, info) : nullptr;
}

napi_value JsMatrix::OnPostSkew(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostSkew matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double kx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, kx);
    double ky = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, ky);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py);

    m_matrix->PostSkew(kx, ky, px, py);
    return nullptr;
}

napi_value JsMatrix::PostTranslate(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostTranslate(env, info) : nullptr;
}

napi_value JsMatrix::OnPostTranslate(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostTranslate matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    JS_CALL_DRAWING_FUNC(m_matrix->PostTranslate(dx, dy));
    return nullptr;
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

napi_value JsMatrix::PreSkew(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreSkew(env, info) : nullptr;
}

napi_value JsMatrix::OnPreSkew(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreSkew matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double kx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, kx);
    double ky = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, ky);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py);

    m_matrix->PreSkew(kx, ky, px, py);
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

napi_value JsMatrix::SetConcat(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetConcat(env, info) : nullptr;
}

napi_value JsMatrix::OnSetConcat(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetConcat matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    JsMatrix* jsMatrix1 = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix1);
    JsMatrix* jsMatrix2 = nullptr;
    GET_UNWRAP_PARAM(ARGC_ONE, jsMatrix2);

    std::shared_ptr<Matrix> matrix1 = jsMatrix1->GetMatrix();
    std::shared_ptr<Matrix> matrix2 = jsMatrix2->GetMatrix();
    if (matrix1 == nullptr || matrix2 == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetConcat matrix is nullptr");
        return nullptr;
    }

    m_matrix->SetConcat(*matrix1, *matrix2);
    return nullptr;
}

napi_value JsMatrix::SetRotation(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetRotation(env, info) : nullptr;
}

napi_value JsMatrix::OnSetRotation(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetRotation matrix is nullptr");
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

    JS_CALL_DRAWING_FUNC(m_matrix->Rotate(degree, px, py));
    return nullptr;
}

napi_value JsMatrix::SetScale(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetScale(env, info) : nullptr;
}

napi_value JsMatrix::OnSetScale(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetScale matrix is nullptr");
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

    JS_CALL_DRAWING_FUNC(m_matrix->Scale(sx, sy, px, py));
    return nullptr;
}

napi_value JsMatrix::SetSinCos(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetSinCos(env, info) : nullptr;
}

napi_value JsMatrix::OnSetSinCos(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetSinCos matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double sin = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, sin);
    double cos = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, cos);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py);

    m_matrix->SetSinCos(sin, cos, px, py);
    return nullptr;
}

napi_value JsMatrix::SetSkew(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetSkew(env, info) : nullptr;
}

napi_value JsMatrix::OnSetSkew(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetSkew matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double kx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, kx);
    double ky = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, ky);
    double px = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px);
    double py = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py);

    m_matrix->SetSkew(kx, ky, px, py);
    return nullptr;
}

napi_value JsMatrix::SetTranslation(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetTranslation(env, info) : nullptr;
}

napi_value JsMatrix::OnSetTranslation(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetTranslation matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    JS_CALL_DRAWING_FUNC(m_matrix->Translate(dx, dy));
    return nullptr;
}

napi_value JsMatrix::SetMatrix(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetMatrix(env, info) : nullptr;
}

napi_value JsMatrix::OnSetMatrix(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetMatrix matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    uint32_t arrayLength = 0;
    if ((napi_get_array_length(env, argv[ARGC_ZERO], &arrayLength) != napi_ok)) {
        JsMatrix* jsMatrix = nullptr;
        GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);

        std::shared_ptr<Matrix> matrix = jsMatrix->GetMatrix();
        if (matrix == nullptr) {
            ROSEN_LOGE("JsMatrix::OnSetMatrix matrix is nullptr");
            return nullptr;
        }

        *m_matrix = *matrix;
        return nullptr;
    }

    if (arrayLength != ARGC_NINE) { // arrayLength must be an nine number
        ROSEN_LOGE("JsMatrix::OnSetMatrix count of array is not nine : %{public}u", arrayLength);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "parameter array length verification failed.");
    }

    scalar matrixPara[arrayLength];
    for (size_t i = 0; i < arrayLength; i++) {
        bool hasElement = false;
        napi_has_element(env, argv[ARGC_ZERO], i, &hasElement);
        if (!hasElement) {
            ROSEN_LOGE("JsMatrix::OnSetMatrix parameter check error");
            return nullptr;
        }

        napi_value element = nullptr;
        napi_get_element(env, argv[ARGC_ZERO], i, &element);

        double value = 0.0;
        ConvertFromJsNumber(env, element, value);
        matrixPara[i] = value;
    }

    m_matrix->SetMatrix(matrixPara[ARGC_ZERO], matrixPara[ARGC_ONE], matrixPara[ARGC_TWO],
        matrixPara[ARGC_THREE], matrixPara[ARGC_FOUR], matrixPara[ARGC_FIVE], matrixPara[ARGC_SIX],
        matrixPara[ARGC_SEVEN], matrixPara[ARGC_EIGHT]);
    return nullptr;
}

napi_value JsMatrix::PreConcat(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPreConcat(env, info) : nullptr;
}

napi_value JsMatrix::OnPreConcat(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreConcat matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPreConcat matrix is nullptr");
        return nullptr;
    }

    m_matrix->PreConcat(*jsMatrix->GetMatrix());
    return nullptr;
}

napi_value JsMatrix::IsAffine(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnIsAffine(env, info) : nullptr;
}

napi_value JsMatrix::OnIsAffine(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnIsAffine matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, m_matrix->IsAffine());
}

napi_value JsMatrix::IsEqual(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnIsEqual(env, info) : nullptr;
}

napi_value JsMatrix::OnIsEqual(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnIsEqual matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsMatrix::OnIsEqual matrix is nullptr");
        return CreateJsValue(env, false);
    }

    return CreateJsValue(env, m_matrix->operator == (*jsMatrix->GetMatrix()));
}

napi_value JsMatrix::Invert(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnInvert(env, info) : nullptr;
}

napi_value JsMatrix::OnInvert(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnInvert matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsMatrix::OnInvert matrix is nullptr");
        return nullptr;
    }

    return CreateJsValue(env, m_matrix->Invert(*jsMatrix->GetMatrix()));
}

napi_value JsMatrix::IsIdentity(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnIsIdentity(env, info) : nullptr;
}

napi_value JsMatrix::OnIsIdentity(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnIsIdentity matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, m_matrix->IsIdentity());
}

napi_value JsMatrix::MapPoints(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnMapPoints(env, info) : nullptr;
}

napi_value JsMatrix::OnMapPoints(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnMapPoints matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    napi_value srcArray = argv[ARGC_ZERO];

    /* Check size */
    uint32_t srcPointsSize = 0;
    if (napi_get_array_length(env, srcArray, &srcPointsSize) != napi_ok || (srcPointsSize == 0)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect src array size.");
    }

    std::vector<Point> dstPoints(srcPointsSize);
    std::vector<Point> srcPoints(srcPointsSize);
    /* Fill vector with data from input array */
    if (!ConvertFromJsPointsArray(env, srcArray, srcPoints.data(), srcPointsSize)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect src array data.");
    }

    JS_CALL_DRAWING_FUNC(m_matrix->MapPoints(dstPoints, srcPoints, srcPointsSize));

    napi_value resultArray = nullptr;
    if (napi_create_array_with_length(env, dstPoints.size(), &resultArray) != napi_ok) {
        ROSEN_LOGE("JsMatrix::OnMapPoints failed to create array");
        return nullptr;
    }
    for (uint32_t idx = 0; idx < dstPoints.size(); idx++) {
        NAPI_CALL(env, napi_set_element(env, resultArray, idx, ConvertPointToJsValue(env, dstPoints.at(idx))));
    }

    return resultArray;
}

napi_value JsMatrix::MapRadius(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnMapRadius(env, info) : nullptr;
}

napi_value JsMatrix::OnMapRadius(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnMapRadius matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double radius = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, radius);

    return CreateJsNumber(env, m_matrix->MapRadius(radius));
}

napi_value JsMatrix::PostScale(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnPostScale(env, info) : nullptr;
}

napi_value JsMatrix::OnPostScale(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnPostScale matrix is nullptr");
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

    JS_CALL_DRAWING_FUNC(m_matrix->PostScale(sx, sy, px, py));

    return nullptr;
}

napi_value JsMatrix::RectStaysRect(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnRectStaysRect(env, info) : nullptr;
}

napi_value JsMatrix::OnRectStaysRect(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnRectStaysRect matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, m_matrix->RectStaysRect());
}

napi_value JsMatrix::Reset(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnReset(env, info) : nullptr;
}

napi_value JsMatrix::OnReset(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnReset matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    m_matrix->Reset();
    return nullptr;
}

napi_value JsMatrix::GetAll(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnGetAll(env, info) : nullptr;
}

napi_value JsMatrix::OnGetAll(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnGetAll matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Matrix::Buffer matrData;
    m_matrix->GetAll(matrData);

    napi_value array = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, Matrix::MATRIX_SIZE, &array));
    for (int i = 0; i < Matrix::MATRIX_SIZE; ++i) {
        NAPI_CALL(env, napi_set_element(env, array, i, CreateJsValue(env, matrData[i])));
    }
    return array;
}

napi_value JsMatrix::SetPolyToPoly(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetPolyToPoly(env, info) : nullptr;
}

napi_value JsMatrix::OnSetPolyToPoly(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetPolyToPoly matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    uint32_t count = 0;
    GET_UINT32_PARAM(ARGC_TWO, count);

    if (count > POLY_POINT_COUNT_MAX) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Invalid param 'count' > 4");
    }

    Drawing::Point srcPoints[POLY_POINT_COUNT_MAX];
    if (!ConvertFromJsPointsArray(env, argv[ARGC_ZERO], srcPoints, count)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Invalid 'src' Array<Point> type.");
    }
    Drawing::Point dstPoints[POLY_POINT_COUNT_MAX];
    if (!ConvertFromJsPointsArray(env, argv[ARGC_ONE], dstPoints, count)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Invalid 'dst' Array<Point> type.");
    }

    bool result = m_matrix->SetPolyToPoly(srcPoints, dstPoints, count);
    return CreateJsValue(env, result);
}

napi_value JsMatrix::SetRectToRect(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnSetRectToRect(env, info) : nullptr;
}

napi_value JsMatrix::OnSetRectToRect(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnSetRectToRect matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double ltrb[ARGC_FOUR];
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect 'src' Rect type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect srcRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect 'dst' Rect type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect dstRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    int32_t stf;
    GET_ENUM_PARAM(ARGC_TWO, stf, 0, static_cast<int32_t>(ScaleToFit::END_SCALETOFIT));

    bool result = m_matrix->SetRectToRect(srcRect, dstRect, static_cast<ScaleToFit>(stf));
    return CreateJsValue(env, result);
}

napi_value JsMatrix::MapRect(napi_env env, napi_callback_info info)
{
    JsMatrix* me = CheckParamsAndGetThis<JsMatrix>(env, info);
    return (me != nullptr) ? me->OnMapRect(env, info) : nullptr;
}

napi_value JsMatrix::OnMapRect(napi_env env, napi_callback_info info)
{
    if (m_matrix == nullptr) {
        ROSEN_LOGE("JsMatrix::OnMapRect matrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double src[ARGC_FOUR];
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], src, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsMatrix::OnMapRect Incorrect parameter type for src rect. The types must be numbers.");
    }
    Rect dstRect;
    Rect srcRect{ src[ARGC_ZERO], src[ARGC_ONE], src[ARGC_TWO], src[ARGC_THREE]};
    auto res = CreateJsValue(env, m_matrix->MapRect(dstRect, srcRect));
    auto objValue = argv[ARGC_ZERO];
    if (napi_set_named_property(env, objValue, "left", CreateJsNumber(env, dstRect.GetLeft())) != napi_ok ||
        napi_set_named_property(env, objValue, "top",  CreateJsNumber(env, dstRect.GetTop())) != napi_ok ||
        napi_set_named_property(env, objValue, "right", CreateJsNumber(env, dstRect.GetRight())) != napi_ok ||
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, dstRect.GetBottom())) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsMatrix::OnMapRect Cannot fill 'dst' Rect type.");
    }
    return res;
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
