/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "matrix_napi/js_matrix.h"
#include "path_iterator_napi/js_path_iterator.h"
#include "roundRect_napi/js_roundrect.h"
#include "utils/performanceCaculate.h"

#include "matrix_napi/js_matrix.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPath::constructor_ = nullptr;
const std::string CLASS_NAME = "Path";

static const napi_property_descriptor g_properties[] = {
    DECLARE_NAPI_FUNCTION("moveTo", JsPath::MoveTo),
    DECLARE_NAPI_FUNCTION("lineTo", JsPath::LineTo),
    DECLARE_NAPI_FUNCTION("arcTo", JsPath::ArcTo),
    DECLARE_NAPI_FUNCTION("quadTo", JsPath::QuadTo),
    DECLARE_NAPI_FUNCTION("conicTo", JsPath::ConicTo),
    DECLARE_NAPI_FUNCTION("cubicTo", JsPath::CubicTo),
    DECLARE_NAPI_FUNCTION("rMoveTo", JsPath::RMoveTo),
    DECLARE_NAPI_FUNCTION("rLineTo", JsPath::RLineTo),
    DECLARE_NAPI_FUNCTION("rQuadTo", JsPath::RQuadTo),
    DECLARE_NAPI_FUNCTION("rConicTo", JsPath::RConicTo),
    DECLARE_NAPI_FUNCTION("rCubicTo", JsPath::RCubicTo),
    DECLARE_NAPI_FUNCTION("addPolygon", JsPath::AddPolygon),
    DECLARE_NAPI_FUNCTION("addOval", JsPath::AddOval),
    DECLARE_NAPI_FUNCTION("addCircle", JsPath::AddCircle),
    DECLARE_NAPI_FUNCTION("addArc", JsPath::AddArc),
    DECLARE_NAPI_FUNCTION("addRect", JsPath::AddRect),
    DECLARE_NAPI_FUNCTION("addRoundRect", JsPath::AddRoundRect),
    DECLARE_NAPI_FUNCTION("addPath", JsPath::AddPath),
    DECLARE_NAPI_FUNCTION("transform", JsPath::Transform),
    DECLARE_NAPI_FUNCTION("contains", JsPath::Contains),
    DECLARE_NAPI_FUNCTION("set", JsPath::Set),
    DECLARE_NAPI_FUNCTION("setFillType", JsPath::SetFillType),
    DECLARE_NAPI_FUNCTION("setLastPoint", JsPath::SetLastPoint),
    DECLARE_NAPI_FUNCTION("getBounds", JsPath::GetBounds),
    DECLARE_NAPI_FUNCTION("close", JsPath::Close),
    DECLARE_NAPI_FUNCTION("offset", JsPath::Offset),
    DECLARE_NAPI_FUNCTION("rewind", JsPath::ReWind),
    DECLARE_NAPI_FUNCTION("reset", JsPath::Reset),
    DECLARE_NAPI_FUNCTION("op", JsPath::Op),
    DECLARE_NAPI_FUNCTION("getLength", JsPath::GetLength),
    DECLARE_NAPI_FUNCTION("getPositionAndTangent", JsPath::GetPositionAndTangent),
    DECLARE_NAPI_FUNCTION("getFillType", JsPath::GetFillType),
    DECLARE_NAPI_FUNCTION("getSegment", JsPath::GetSegment),
    DECLARE_NAPI_FUNCTION("getMatrix", JsPath::GetMatrix),
    DECLARE_NAPI_FUNCTION("buildFromSvgString", JsPath::BuildFromSvgString),
    DECLARE_NAPI_FUNCTION("isClosed", JsPath::IsClosed),
    DECLARE_NAPI_FUNCTION("isEmpty", JsPath::IsEmpty),
    DECLARE_NAPI_FUNCTION("isRect", JsPath::IsRect),
    DECLARE_NAPI_FUNCTION("getPathIterator", JsPath::GetPathIterator),
    DECLARE_NAPI_FUNCTION("approximate", JsPath::Approximate),
    DECLARE_NAPI_FUNCTION("interpolate", JsPath::Interpolate),
    DECLARE_NAPI_FUNCTION("isInterpolate", JsPath::IsInterpolate),
};

napi_value JsPath::Init(napi_env env, napi_value exportObj)
{
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(g_properties) / sizeof(g_properties[0]), g_properties, &constructor);
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
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_value jsThis = nullptr;
    JsPath* jsPath = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("Path::Constructor Failed to napi_get_cb_info");
        return nullptr;
    }
    if (argc == ARGC_ZERO) {
        Path* path = new Path();
        jsPath = new JsPath(path);
    } else if (argc == ARGC_ONE) {
        napi_valuetype valueType = napi_undefined;
        if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "JsPath::Constructor Argv[0] is invalid");
        }
        JsPath* path = nullptr;
        GET_UNWRAP_PARAM(ARGC_ZERO, path);
        Path* p = new Path(*path->GetPath());
        jsPath = new JsPath(p);
    } else {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect number of parameters.");
    }
    if (!jsPath) {
        ROSEN_LOGE("Failed to create JsPath");
        return nullptr;
    }
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

napi_value JsPath::CreateJsPath(napi_env env, Path* path)
{
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        delete path;
        ROSEN_LOGE("JsPath::CreateJsPath failed to get reference value!");
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_object(env, &result);
    if (result == nullptr) {
        delete path;
        ROSEN_LOGE("JsPath::CreateJsPath create object failed!");
        return nullptr;
    }
    JsPath* jsPath = new JsPath(path);
    status = napi_wrap(env, result, jsPath, JsPath::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPath;
        ROSEN_LOGE("JsPath::CreateJsPath failed to wrap native instance");
        return nullptr;
    }
    napi_define_properties(env, result, sizeof(g_properties) / sizeof(g_properties[0]), g_properties);
    return result;
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

napi_value JsPath::ConicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnConicTo(env, info) : nullptr;
}

napi_value JsPath::CubicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnCubicTo(env, info) : nullptr;
}

napi_value JsPath::RMoveTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRMoveTo(env, info) : nullptr;
}

napi_value JsPath::RLineTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRLineTo(env, info) : nullptr;
}

napi_value JsPath::RQuadTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRQuadTo(env, info) : nullptr;
}

napi_value JsPath::RConicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRConicTo(env, info) : nullptr;
}

napi_value JsPath::RCubicTo(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnRCubicTo(env, info) : nullptr;
}

napi_value JsPath::AddPolygon(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddPolygon(env, info) : nullptr;
}

napi_value JsPath::AddOval(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddOval(env, info) : nullptr;
}

napi_value JsPath::AddCircle(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddCircle(env, info) : nullptr;
}

napi_value JsPath::AddArc(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddArc(env, info) : nullptr;
}

napi_value JsPath::AddRect(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddRect(env, info) : nullptr;
}

napi_value JsPath::AddRoundRect(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddRoundRect(env, info) : nullptr;
}

napi_value JsPath::AddPath(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnAddPath(env, info) : nullptr;
}

napi_value JsPath::Transform(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnTransform(env, info) : nullptr;
}

napi_value JsPath::Contains(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnContains(env, info) : nullptr;
}

napi_value JsPath::SetFillType(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnSetFillType(env, info) : nullptr;
}

napi_value JsPath::GetBounds(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetBounds(env, info) : nullptr;
}

napi_value JsPath::Close(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnClose(env, info) : nullptr;
}

napi_value JsPath::Offset(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnOffset(env, info) : nullptr;
}

napi_value JsPath::Reset(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnReset(env, info) : nullptr;
}

napi_value JsPath::GetLength(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetLength(env, info) : nullptr;
}

napi_value JsPath::GetPositionAndTangent(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetPositionAndTangent(env, info) : nullptr;
}

napi_value JsPath::Set(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnSet(env, info) : nullptr;
}

napi_value JsPath::SetLastPoint(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnSetLastPoint(env, info) : nullptr;
}

napi_value JsPath::GetFillType(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetFillType(env, info) : nullptr;
}

napi_value JsPath::ReWind(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnReWind(env, info) : nullptr;
}

napi_value JsPath::GetSegment(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetSegment(env, info) : nullptr;
}

napi_value JsPath::GetMatrix(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetMatrix(env, info) : nullptr;
}

napi_value JsPath::BuildFromSvgString(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnBuildFromSvgString(env, info) : nullptr;
}

napi_value JsPath::IsClosed(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnIsClosed(env, info) : nullptr;
}

napi_value JsPath::IsEmpty(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnIsEmpty(env, info) : nullptr;
}

napi_value JsPath::IsRect(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnIsRect(env, info) : nullptr;
}

napi_value JsPath::Op(napi_env env, napi_callback_info info)
{
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnOp(env, info) : nullptr;
}

napi_value JsPath::GetPathIterator(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnGetPathIterator(env, info) : nullptr;
}

napi_value JsPath::Approximate(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnApproximate(env, info) : nullptr;
}

napi_value JsPath::Interpolate(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnInterpolate(env, info) : nullptr;
}

napi_value JsPath::IsInterpolate(napi_env env, napi_callback_info info)
{
    DRAWING_PERFORMANCE_TEST_JS_RETURN(nullptr);
    JsPath* me = CheckParamsAndGetThis<JsPath>(env, info);
    return (me != nullptr) ? me->OnIsInterpolate(env, info) : nullptr;
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
    return nullptr;
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
    return nullptr;
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
    return nullptr;
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
    return nullptr;
}

napi_value JsPath::OnConicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnConicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FIVE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FIVE);

    double ctrlPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, ctrlPtX);
    double ctrlPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, ctrlPtY);
    double endPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, endPtX);
    double endPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, endPtY);
    double weight = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, weight);

    m_path->ConicTo(ctrlPtX, ctrlPtY, endPtX, endPtY, weight);
    return nullptr;
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
    return nullptr;
}

napi_value JsPath::OnRMoveTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRMoveTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    m_path->RMoveTo(dx, dy);
    return nullptr;
}

napi_value JsPath::OnRLineTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRLineTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);

    m_path->RLineTo(dx, dy);
    return nullptr;
}

napi_value JsPath::OnRQuadTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRQuadTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double dx1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx1);
    double dy1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy1);
    double dx2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, dx2);
    double dy2 = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, dy2);

    m_path->RQuadTo(dx1, dy1, dx2, dy2);
    return nullptr;
}

napi_value JsPath::OnRConicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRConicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FIVE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FIVE);

    double ctrlPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, ctrlPtX);
    double ctrlPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, ctrlPtY);
    double endPtX = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, endPtX);
    double endPtY = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, endPtY);
    double weight = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, weight);

    m_path->RConicTo(ctrlPtX, ctrlPtY, endPtX, endPtY, weight);
    return nullptr;
}

napi_value JsPath::OnRCubicTo(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRCubicTo path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_SIX] = { nullptr };
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

    m_path->RCubicTo(px1, py1, px2, py2, px3, py3);
    return nullptr;
}

napi_value JsPath::OnAddPolygon(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    napi_value array = argv[ARGC_ZERO];
    uint32_t size = 0;
    napi_get_array_length(env, array, &size);
    if (size == 0) {
        return nullptr;
    }
    std::vector<Point> points;
    points.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
        napi_value tempRunBuffer = nullptr;
        napi_get_element(env, array, i, &tempRunBuffer);

        napi_value tempValue = nullptr;
        Point point = Point();
        double positionX = 0.0;
        double positionY = 0.0;
        napi_get_named_property(env, tempRunBuffer, "x", &tempValue);
        bool isPositionXOk = ConvertFromJsValue(env, tempValue, positionX);
        napi_get_named_property(env, tempRunBuffer, "y", &tempValue);
        bool isPositionYOk = ConvertFromJsValue(env, tempValue, positionY);
        if (!(isPositionXOk && isPositionYOk)) {
            ROSEN_LOGE("JsPath::OnAddPolygon Argv is invalid");
            return napi_value();
        }

        point.SetX(positionX);
        point.SetY(positionY);
        points.emplace_back(point);
    }
    if (points.size() != size) {
        ROSEN_LOGE("JsPath::OnAddPolygon Argv is invalid");
        return nullptr;
    }

    bool close = false;
    GET_BOOLEAN_PARAM(ARGC_ONE, close);

    m_path->AddPoly(points, size, close);
    return nullptr;
}

napi_value JsPath::OnOp(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnOp path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPath);
    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsPath::OnOp path is nullptr");
        return nullptr;
    }

    int32_t jsPathOp = 0;
    GET_ENUM_PARAM(ARGC_ONE, jsPathOp, 0, static_cast<int32_t>(PathOp::REVERSE_DIFFERENCE));
    return CreateJsValue(env, m_path->Op(*m_path, *jsPath->GetPath(), static_cast<PathOp>(jsPathOp)));
}

napi_value JsPath::OnClose(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnClose path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JS_CALL_DRAWING_FUNC(m_path->Close());
    return nullptr;
}

napi_value JsPath::OnOffset(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnOffset path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dy);
    Path* path = new Path();
    m_path->Offset(path, dx, dy);
    return CreateJsPath(env, path);
}

napi_value JsPath::OnReset(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnReset path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JS_CALL_DRAWING_FUNC(m_path->Reset());
    return nullptr;
}

napi_value JsPath::OnGetLength(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetLength path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool forceClosed = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, forceClosed);

    double len = m_path->GetLength(forceClosed);
    return CreateJsNumber(env, len);
}

napi_value JsPath::OnGetPositionAndTangent(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetPositionAndTangent path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    bool forceClosed = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, forceClosed);

    double distance = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, distance);

    Drawing::Point position;
    double startPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_TWO], startPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter2 type. The type of x, y be number.");
    }
    position = Drawing::Point(startPoint[ARGC_ZERO], startPoint[ARGC_ONE]);

    Drawing::Point tangent;
    double endPoint[ARGC_TWO] = {0};
    if (!ConvertFromJsPoint(env, argv[ARGC_THREE], endPoint, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter3 type. The type of x, y be number.");
    }
    tangent = Drawing::Point(endPoint[ARGC_ZERO], endPoint[ARGC_ONE]);

    bool result = m_path->GetPositionAndTangent(distance, position, tangent, forceClosed);
    if (napi_set_named_property(env, argv[ARGC_TWO], "x", CreateJsNumber(env, position.GetX())) != napi_ok ||
        napi_set_named_property(env, argv[ARGC_TWO], "y",  CreateJsNumber(env, position.GetY())) != napi_ok ||
        napi_set_named_property(env, argv[ARGC_THREE], "x", CreateJsNumber(env, tangent.GetX())) != napi_ok ||
        napi_set_named_property(env, argv[ARGC_THREE], "y", CreateJsNumber(env, tangent.GetY())) != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsPath::OnGetPositionAndTangent Cannot fill 'position' and 'tangent' Point type.");
    }
    return CreateJsValue(env, result);
}

napi_value JsPath::OnSet(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnSet path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsPath* path = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, path);
    if (path->GetPath() == nullptr) {
        return nullptr;
    }
    JS_CALL_DRAWING_FUNC(m_path->SetPath(*path->GetPath()));
    return nullptr;
}

napi_value JsPath::OnSetLastPoint(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnSetLastPoint path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);
    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);
    JS_CALL_DRAWING_FUNC(m_path->SetLastPoint(x, y));
    return nullptr;
}

napi_value JsPath::OnGetFillType(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetFillType path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return CreateJsValue(env, m_path->GetFillStyle());
}

napi_value JsPath::OnReWind(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnRewind path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    JS_CALL_DRAWING_FUNC(m_path->ReWind());
    return nullptr;
}

napi_value JsPath::OnGetSegment(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetSegment path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_FIVE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FIVE);

    bool forceClosed = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, forceClosed);
    double start = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, start);
    double stop = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, stop);
    bool startWithMoveTo = false;
    GET_BOOLEAN_PARAM(ARGC_THREE, startWithMoveTo);
    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_FOUR, jsPath);

    Path* dst = jsPath->GetPath();
    if (dst == nullptr) {
        ROSEN_LOGE("JsPath::OnGetSegment dst is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect OnGetSegment parameter3 type.");
    }

    double length = m_path->GetLength(forceClosed);
    if (start < 0) {
        start = 0;
    }
    if (stop > length) {
        stop = length;
    }
    if (start >= stop) {
        return CreateJsValue(env, false);
    }
    return CreateJsValue(env, m_path->GetSegment(start, stop, dst, startWithMoveTo, forceClosed));
}

napi_value JsPath::OnIsEmpty(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnIsEmpty path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool result = m_path->IsEmpty();
    return CreateJsValue(env, result);
}

napi_value JsPath::OnIsRect(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnIsRect path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_ZERO], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect OnSaveLayer parameter0 type.");
    }
    if (valueType == napi_null) {
        return CreateJsValue(env, m_path->IsRect(nullptr));
    }
    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    bool result = m_path->IsRect(&rect);
    DrawingRectConvertToJsRect(env, argv[ARGC_ZERO], rect);
    return CreateJsValue(env, result);
}

napi_value JsPath::OnGetMatrix(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetMatrix path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    bool forceClosed = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, forceClosed);

    double distance = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, distance);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_TWO, jsMatrix);

    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsPath::OnGetMatrix jsMatrix is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    int32_t flag = 0;
    GET_ENUM_PARAM(ARGC_THREE, flag, 0,
        static_cast<int32_t>(PathMeasureMatrixFlags::GET_POS_AND_TAN_MATRIX));
    bool result = m_path->GetMatrix(
        forceClosed,
        distance,
        jsMatrix->GetMatrix().get(),
        static_cast<PathMeasureMatrixFlags>(flag));
    return CreateJsValue(env, result);
}

napi_value JsPath::OnBuildFromSvgString(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnBuildFromSVGString path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    std::string str = {""};
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], str))) {
        ROSEN_LOGE("JsPath::OnBuildFromSVGString Argv is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool result = m_path->BuildFromSVGString(str);
    return CreateJsValue(env, result);
}

napi_value JsPath::OnAddOval(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddOval path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_TWO, ARGC_THREE);

    Drawing::Rect drawingRect;
    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    int32_t start = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ONE, start);
    if (argc == ARGC_TWO) {
        JS_CALL_DRAWING_FUNC(m_path->AddOval(drawingRect, start,
            static_cast<PathDirection>(PathDirection::CW_DIRECTION)));
        return nullptr;
    }
    int32_t jsDirection = 0;
    GET_ENUM_PARAM(ARGC_TWO, jsDirection, 0, static_cast<int32_t>(PathDirection::CCW_DIRECTION));

    JS_CALL_DRAWING_FUNC(m_path->AddOval(drawingRect, start, static_cast<PathDirection>(jsDirection)));
    return nullptr;
}

napi_value JsPath::OnAddCircle(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddCircle path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_THREE, ARGC_FOUR);

    double px1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, px1);
    double py1 = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, py1);
    double radius = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, radius);
    if (argc == ARGC_THREE) {
        JS_CALL_DRAWING_FUNC(m_path->AddCircle(px1, py1, radius));
        return nullptr;
    }
    int32_t jsDirection = 0;
    GET_ENUM_PARAM(ARGC_THREE, jsDirection, 0, static_cast<int32_t>(PathDirection::CCW_DIRECTION));

    JS_CALL_DRAWING_FUNC(m_path->AddCircle(px1, py1, radius, static_cast<PathDirection>(jsDirection)));
    return nullptr;
}

napi_value JsPath::OnAddArc(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddArc path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    Drawing::Rect drawingRect;
    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    double startDeg = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, startDeg);
    double sweepDeg = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, sweepDeg);

    JS_CALL_DRAWING_FUNC(m_path->AddArc(drawingRect, startDeg, sweepDeg));
    return nullptr;
}

napi_value JsPath::OnAddRect(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddRect path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);

    Drawing::Rect drawingRect;
    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    if (argc == ARGC_ONE) {
        JS_CALL_DRAWING_FUNC(m_path->AddRect(drawingRect));
        return nullptr;
    }
    int32_t jsDirection = 0;
    GET_ENUM_PARAM(ARGC_ONE, jsDirection, 0, static_cast<int32_t>(PathDirection::CCW_DIRECTION));

    JS_CALL_DRAWING_FUNC(m_path->AddRect(drawingRect, static_cast<PathDirection>(jsDirection)));
    return nullptr;
}

napi_value JsPath::OnAddRoundRect(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddRoundRect path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);

    JsRoundRect* jsRoundRect = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsRoundRect);
    if (argc == ARGC_ONE) {
        JS_CALL_DRAWING_FUNC(m_path->AddRoundRect(jsRoundRect->GetRoundRect()));
        return nullptr;
    }
    int32_t jsDirection = 0;
    GET_ENUM_PARAM(ARGC_ONE, jsDirection, 0, static_cast<int32_t>(PathDirection::CCW_DIRECTION));

    JS_CALL_DRAWING_FUNC(m_path->AddRoundRect(jsRoundRect->GetRoundRect(), static_cast<PathDirection>(jsDirection)));
    return nullptr;
}

napi_value JsPath::OnAddPath(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_ONE, ARGC_TWO);

    JsPath* jsPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsPath);
    if (jsPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath path is nullptr");
        return nullptr;
    }
    if (argc == ARGC_ONE) {
        JS_CALL_DRAWING_FUNC(m_path->AddPath(*jsPath->GetPath(), Drawing::Matrix()));
        return nullptr;
    }
    Drawing::Matrix* drawingMatrixPtr = nullptr;
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, argv[ARGC_ONE], &valueType) != napi_ok ||
        (valueType != napi_null && valueType != napi_object)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect OnAddPath parameter1 type.");
    }
    if (valueType == napi_object) {
        JsMatrix* jsMatrix = nullptr;
        GET_UNWRAP_PARAM(ARGC_ONE, jsMatrix);
        if (jsMatrix->GetMatrix() != nullptr) {
            drawingMatrixPtr = jsMatrix->GetMatrix().get();
        }
    }
    JS_CALL_DRAWING_FUNC(m_path->AddPath(*jsPath->GetPath(),
        drawingMatrixPtr ? *drawingMatrixPtr : Drawing::Matrix()));
    return nullptr;
}

napi_value JsPath::OnTransform(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnTransform path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsMatrix* jsMatrix = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsMatrix);
    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("JsPath::OnAddPath Matrix is nullptr");
        return nullptr;
    }

    JS_CALL_DRAWING_FUNC(m_path->Transform(*jsMatrix->GetMatrix()));
    return nullptr;
}

napi_value JsPath::OnContains(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnContains path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double x = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, x);
    double y = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, y);

    return CreateJsValue(env, m_path->Contains(x, y));
}

napi_value JsPath::OnSetFillType(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnSetFillType path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t jsFillType = 0;
    GET_ENUM_PARAM(ARGC_ZERO, jsFillType, 0, static_cast<int32_t>(PathFillType::INVERSE_EVENTODD));

    JS_CALL_DRAWING_FUNC(m_path->SetFillStyle(static_cast<PathFillType>(jsFillType)));
    return nullptr;
}

napi_value JsPath::OnGetBounds(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetBounds path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return GetRectAndConvertToJsValue(env, m_path->GetBounds());
}

napi_value JsPath::OnIsClosed(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnIsClosed path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool result = m_path->IsClosed(false);
    return CreateJsValue(env, result);
}

napi_value JsPath::OnGetPathIterator(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnGetPathIterator path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    PathIterator *iter = new PathIterator(*m_path);
    return JsPathIterator::CreateJsPathIterator(env, iter);
}

napi_value JsPath::OnApproximate(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnApproximate path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    double acceptableError = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, acceptableError);
    if (acceptableError < 0.0) {
        ROSEN_LOGE("JsPath::OnApproximate acceptableError is invalid");
        return nullptr;
    }
    std::vector<scalar> points;
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    m_path->Approximate(static_cast<scalar>(acceptableError), points);

    napi_value resultArray;
    napi_status status = napi_create_array(env, &resultArray);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPath::OnApproximate failed to create array");
        return nullptr;
    }
    for (size_t i = 0; i < points.size(); i++) {
        napi_value element = CreateJsValue(env, points[i]);
        napi_set_element(env, resultArray, i, element);
    }

    return resultArray;
}

napi_value JsPath::OnInterpolate(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnInterpolate path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_THREE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    JsPath* other = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, other);
    if (other->GetPath() == nullptr) {
        ROSEN_LOGE("JsPath::OnInterpolate other is nullptr");
        return nullptr;
    }
    double weight = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, weight);
    if (weight < 0.0 || weight > 1.0) {
        ROSEN_LOGE("JsPath::OnInterpolate weight is invalid");
        return nullptr;
    }
    JsPath* interpolatedPath = nullptr;
    GET_UNWRAP_PARAM(ARGC_TWO, interpolatedPath);
    if (interpolatedPath->GetPath() == nullptr) {
        ROSEN_LOGE("JsPath::OnInterpolate interpolatedPath is nullptr");
        return nullptr;
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    bool result = m_path->Interpolate(*other->GetPath(), weight, *interpolatedPath->GetPath());
    return CreateJsValue(env, result);
}

napi_value JsPath::OnIsInterpolate(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnIsInterpolate path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    JsPath* other = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, other);
    if (other->GetPath() == nullptr) {
        ROSEN_LOGE("JsPath::OnIsInterpolate other is nullptr");
        return nullptr;
    }
    DRAWING_PERFORMANCE_TEST_NAP_RETURN(nullptr);
    bool result = m_path->IsInterpolate(*other->GetPath());
    return CreateJsValue(env, result);
}
Path* JsPath::GetPath()
{
    return m_path;
}
} // namespace Drawing
} // namespace OHOS::Rosen
