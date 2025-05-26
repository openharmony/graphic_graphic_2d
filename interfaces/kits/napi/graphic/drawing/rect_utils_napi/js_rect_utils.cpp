/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "js_rect_utils.h"

#include "native_value.h"

#include "js_drawing_utils.h"
#include "utils/log.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsRectUtils::constructor_ = nullptr;
const std::string CLASS_NAME = "RectUtils";

napi_value JsRectUtils::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("centerX", JsRectUtils::CenterX),
        DECLARE_NAPI_STATIC_FUNCTION("centerY", JsRectUtils::CenterY),
        DECLARE_NAPI_STATIC_FUNCTION("contains", JsRectUtils::Contains),
        DECLARE_NAPI_STATIC_FUNCTION("getHeight", JsRectUtils::GetHeight),
        DECLARE_NAPI_STATIC_FUNCTION("getWidth", JsRectUtils::GetWidth),
        DECLARE_NAPI_STATIC_FUNCTION("inset", JsRectUtils::Inset),
        DECLARE_NAPI_STATIC_FUNCTION("intersect", JsRectUtils::Intersect),
        DECLARE_NAPI_STATIC_FUNCTION("isEmpty", JsRectUtils::IsEmpty),
        DECLARE_NAPI_STATIC_FUNCTION("isEqual", JsRectUtils::IsEqual),
        DECLARE_NAPI_STATIC_FUNCTION("isIntersect", JsRectUtils::IsIntersect),
        DECLARE_NAPI_STATIC_FUNCTION("makeCopy", JsRectUtils::MakeCopy),
        DECLARE_NAPI_STATIC_FUNCTION("makeEmpty", JsRectUtils::MakeEmpty),
        DECLARE_NAPI_STATIC_FUNCTION("makeLtrb", JsRectUtils::MakeLtrb),
        DECLARE_NAPI_STATIC_FUNCTION("offset", JsRectUtils::Offset),
        DECLARE_NAPI_STATIC_FUNCTION("offsetTo", JsRectUtils::OffsetTo),
        DECLARE_NAPI_STATIC_FUNCTION("setEmpty", JsRectUtils::SetEmpty),
        DECLARE_NAPI_STATIC_FUNCTION("setLtrb", JsRectUtils::SetLtrb),
        DECLARE_NAPI_STATIC_FUNCTION("setRect", JsRectUtils::SetRect),
        DECLARE_NAPI_STATIC_FUNCTION("sort", JsRectUtils::Sort),
        DECLARE_NAPI_STATIC_FUNCTION("union", JsRectUtils::Union)
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRectUtils::Init Failed to define JsRectUtils class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRectUtils::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRectUtils::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsRectUtils::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRectUtils::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsRectUtils *jsRectUtils = new JsRectUtils();
    status = napi_wrap(env, jsThis, jsRectUtils, JsRectUtils::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRectUtils;
        ROSEN_LOGE("JsRectUtils::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsRectUtils::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRectUtils* napi = reinterpret_cast<JsRectUtils*>(nativeObject);
        delete napi;
    }
}

napi_value JsRectUtils::CenterX(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    return CreateJsNumber(env, (ltrb[ARGC_ZERO] + ltrb[ARGC_TWO]) / 2); // 2 represents the average of left and right.
}

napi_value JsRectUtils::CenterY(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    return CreateJsNumber(env, (ltrb[ARGC_ONE] + ltrb[ARGC_THREE]) / 2); // 2 represents the average of top and bottom.
}

napi_value JsRectUtils::Contains(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FIVE;
    napi_value argv[ARGC_FIVE] = {nullptr};
    CHECK_PARAM_NUMBER_WITH_OPTIONAL_PARAMS(argv, argc, ARGC_TWO, ARGC_FIVE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    if (argc == ARGC_TWO) {
        double ltrb2[ARGC_FOUR] = {0};
        if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb2, ARGC_FOUR)) {
            return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
                "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
        }
        Drawing::Rect other = Drawing::Rect(ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
        return CreateJsValue(env, rect.Contains(other));
    } else if (argc == ARGC_THREE) {
        double x = 0.0;
        GET_DOUBLE_PARAM(ARGC_ONE, x);
        double y = 0.0;
        GET_DOUBLE_PARAM(ARGC_TWO, y);
        return CreateJsValue(env, rect.Contains(x, y));
    } else if (argc == ARGC_FIVE) {
        double left = 0.0;
        GET_DOUBLE_PARAM(ARGC_ONE, left);
        double top = 0.0;
        GET_DOUBLE_PARAM(ARGC_TWO, top);
        double right = 0.0;
        GET_DOUBLE_PARAM(ARGC_THREE, right);
        double bottom = 0.0;
        GET_DOUBLE_PARAM(ARGC_FOUR, bottom);
        Drawing::Rect other = Drawing::Rect(left, top, right, bottom);
        return CreateJsValue(env, rect.Contains(other));
    }
    return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect number of Contains parameters.");
}

napi_value JsRectUtils::GetHeight(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    return CreateJsNumber(env, rect.GetHeight());
}

napi_value JsRectUtils::GetWidth(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    return CreateJsNumber(env, rect.GetWidth());
}

napi_value JsRectUtils::Inset(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_FIVE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FIVE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double left = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, left);
    double top = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, top);
    double right = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, right);
    double bottom = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, bottom);

    LtrbConvertToJsRect(env, argv[ARGC_ZERO],
        ltrb[ARGC_ZERO] + left, ltrb[ARGC_ONE] + top, ltrb[ARGC_TWO] - right, ltrb[ARGC_THREE] - bottom);
    return nullptr;
}

napi_value JsRectUtils::Intersect(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double ltrb2[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb2, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    Drawing::Rect other = Drawing::Rect(ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
    bool isIntersect = rect.Intersect(other);
    if (isIntersect) {
        DrawingRectConvertToJsRect(env, argv[ARGC_ZERO], rect);
    }
    return CreateJsValue(env, isIntersect);
}

napi_value JsRectUtils::IsEmpty(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    return CreateJsValue(env, rect.IsEmpty());
}

napi_value JsRectUtils::IsEqual(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double ltrb2[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb2, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    Drawing::Rect otherRect = Drawing::Rect(ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
    return CreateJsValue(env, rect == otherRect);
}

napi_value JsRectUtils::IsIntersect(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double ltrb2[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb2, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    Drawing::Rect other = Drawing::Rect(ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
    return CreateJsValue(env, rect.IsIntersect(other));
}

napi_value JsRectUtils::MakeCopy(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    return GetRectAndConvertToJsValue(env, ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
}

napi_value JsRectUtils::MakeEmpty(napi_env env, napi_callback_info info)
{
    return GetRectAndConvertToJsValue(env, Drawing::Rect());
}

napi_value JsRectUtils::MakeLtrb(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    double left = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, left);
    double top = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, top);
    double right = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, right);
    double bottom = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, bottom);

    return GetRectAndConvertToJsValue(env, left, top, right, bottom);
}

napi_value JsRectUtils::Offset(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_THREE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double dx = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, dx);
    double dy = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, dy);

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    rect.Offset(dx, dy);
    DrawingRectConvertToJsRect(env, argv[ARGC_ZERO], rect);
    return nullptr;
}

napi_value JsRectUtils::OffsetTo(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_THREE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double newLeft = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, newLeft);
    double newTop = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, newTop);

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    double dx = newLeft - ltrb[ARGC_ZERO];
    double dy = newTop - ltrb[ARGC_ONE];
    rect.Offset(dx, dy);
    DrawingRectConvertToJsRect(env, argv[ARGC_ZERO], rect);
    return nullptr;
}

napi_value JsRectUtils::SetEmpty(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    DrawingRectConvertToJsRect(env, argv[ARGC_ZERO], Drawing::Rect());
    return nullptr;
}

napi_value JsRectUtils::SetLtrb(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_FIVE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FIVE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double left = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, left);
    double top = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, top);
    double right = 0.0;
    GET_DOUBLE_PARAM(ARGC_THREE, right);
    double bottom = 0.0;
    GET_DOUBLE_PARAM(ARGC_FOUR, bottom);

    LtrbConvertToJsRect(env, argv[ARGC_ZERO], left, top, right, bottom);
    return nullptr;
}

napi_value JsRectUtils::SetRect(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double ltrb2[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb2, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
    }

    LtrbConvertToJsRect(env, argv[ARGC_ZERO], ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
    return nullptr;
}

napi_value JsRectUtils::Sort(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    rect.Sort();
    DrawingRectConvertToJsRect(env, argv[ARGC_ZERO], rect);
    return nullptr;
}

napi_value JsRectUtils::Union(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }

    double ltrb2[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ONE], ltrb2, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter1 type. The type of left, top, right and bottom must be number.");
    }

    Drawing::Rect rect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);
    Drawing::Rect other = Drawing::Rect(ltrb2[ARGC_ZERO], ltrb2[ARGC_ONE], ltrb2[ARGC_TWO], ltrb2[ARGC_THREE]);
    bool isJoin = rect.Join(other);
    if (isJoin) {
        DrawingRectConvertToJsRect(env, argv[ARGC_ZERO], rect);
    }
    return nullptr;
}

} // namespace Drawing
} // namespace OHOS::Rosen