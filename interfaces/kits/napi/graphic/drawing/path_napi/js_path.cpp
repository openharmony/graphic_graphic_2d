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
#include "matrix_napi/js_matrix.h"
#include "roundRect_napi/js_roundrect.h"

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
        DECLARE_NAPI_FUNCTION("addOval", JsPath::AddOval),
        DECLARE_NAPI_FUNCTION("addCircle", JsPath::AddCircle),
        DECLARE_NAPI_FUNCTION("addArc", JsPath::AddArc),
        DECLARE_NAPI_FUNCTION("addRect", JsPath::AddRect),
        DECLARE_NAPI_FUNCTION("addRoundRect", JsPath::AddRoundRect),
        DECLARE_NAPI_FUNCTION("addPath", JsPath::AddPath),
        DECLARE_NAPI_FUNCTION("transform", JsPath::Transform),
        DECLARE_NAPI_FUNCTION("contains", JsPath::Contains),
        DECLARE_NAPI_FUNCTION("setFillType", JsPath::SetFillType),
        DECLARE_NAPI_FUNCTION("getBounds", JsPath::GetBounds),
        DECLARE_NAPI_FUNCTION("close", JsPath::Close),
        DECLARE_NAPI_FUNCTION("reset", JsPath::Reset),
        DECLARE_NAPI_FUNCTION("getLength", JsPath::GetLength),
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

napi_value JsPath::OnClose(napi_env env, napi_callback_info info)
{
    if (m_path == nullptr) {
        ROSEN_LOGE("JsPath::OnClose path is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JS_CALL_DRAWING_FUNC(m_path->Close());
    return nullptr;
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

    uint32_t start = 0;
    GET_UINT32_PARAM(ARGC_ONE, start);
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
    auto bounds = m_path->GetBounds();
    std::shared_ptr<Rect> rect = std::make_shared<Rect>(bounds.GetLeft(),
                                                        bounds.GetTop(), bounds.GetRight(), bounds.GetBottom());
    if (!rect) {
        ROSEN_LOGE("JsPath::OnGetBounds return value is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    return GetRectAndConvertToJsValue(env, rect);
}

Path* JsPath::GetPath()
{
    return m_path;
}
} // namespace Drawing
} // namespace OHOS::Rosen