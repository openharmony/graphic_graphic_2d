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

#include "js_path.h"

#include "native_value.h"

#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPath::constructor_ = nullptr;
const std::string CLASS_NAME = "Path";
napi_value JsPath::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("moveTo", JsPath::MoveTo),
        DECLARE_NAPI_FUNCTION("lineTo", JsPath::LineTo),
        DECLARE_NAPI_FUNCTION("arcTo", JsPath::ArcTo),
        DECLARE_NAPI_FUNCTION("quadTo", JsPath::QuadTo),
        DECLARE_NAPI_FUNCTION("cubicTo", JsPath::CubicTo),
        DECLARE_NAPI_FUNCTION("close", JsPath::Close),
        DECLARE_NAPI_FUNCTION("reset", JsPath::Reset),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Path class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsPath::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    Path *path = new Path();
    JsPath *jsPath = new(std::nothrow) JsPath(path);

    status = napi_wrap(env, jsThis, jsPath,
                       JsPath::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPath;
        ROSEN_LOGE("JsPath::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsPath::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPath *napi = reinterpret_cast<JsPath *>(nativeObject);
        delete napi;
    }
}

JsPath::~JsPath()
{
    if (m_path != nullptr) {
        delete m_path;
        m_path = nullptr;
    }
}

napi_value JsPath::MoveTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnMoveTo(env, info) : nullptr;
}

napi_value JsPath::LineTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnLineTo(env, info) : nullptr;
}

napi_value JsPath::ArcTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnArcTo(env, info) : nullptr;
}

napi_value JsPath::QuadTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnQuadTo(env, info) : nullptr;
}

napi_value JsPath::CubicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnCubicTo(env, info) : nullptr;
}

napi_value JsPath::Close(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnClose(env, info) : nullptr;
}

napi_value JsPath::Reset(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnReset(env, info) : nullptr;
}

napi_value JsPath::OnMoveTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnMoveTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);

    JS_CALL_DRAWING_FUNC(m_path->MoveTo(x, y));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnLineTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnLineTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);

    JS_CALL_DRAWING_FUNC(m_path->LineTo(x, y));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnArcTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnArcTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_SIX] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_SIX);

    double x1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x1);
    double y1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y1);
    double x2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, x2);
    double y2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, y2);
    double startDeg = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, startDeg);
    double sweepDeg = 0.0;
    GET_DOUBLE_PARAM(ARGC_FIVE, sweepDeg);

    JS_CALL_DRAWING_FUNC(m_path->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnQuadTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnQuadTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double ctrlPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, ctrlPtX);
    double ctrlPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, ctrlPtY);
    double endPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, endPtX);
    double endPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, endPtY);

    JS_CALL_DRAWING_FUNC(m_path->QuadTo(ctrlPtX, ctrlPtY, endPtX, endPtY));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnCubicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnCubicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_SIX] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_SIX);

    double px1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, px1);
    double py1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, py1);
    double px2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, px2);
    double py2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, py2);
    double px3 = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, px3);
    double py3 = 0.0;
    GET_DOUBLE_PARAM(ARGC_FIVE, py3);

    JS_CALL_DRAWING_FUNC(m_path->CubicTo(Point(px1, py1), Point(px2, py2), Point(px3, py3)));
    return NapiGetUndefined(env);
}

napi_value JsPath::OnClose(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnClose path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JS_CALL_DRAWING_FUNC(m_path->Close());
    return NapiGetUndefined(env);
}

napi_value JsPath::OnReset(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnReset path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JS_CALL_DRAWING_FUNC(m_path->Reset());
    return NapiGetUndefined(env);
}

Path* JsPath::GetPath()
{
    return m_path;
}
} // namespace Drawing
} // namespace OHOS::Rosen