/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "js_point_utils.h"
#include "native_value.h"
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPointUtils::constructor_ = nullptr;
const std::string CLASS_NAME = "PointUtils";

napi_value JsPointUtils::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("negate", JsPointUtils::Negate),
        DECLARE_NAPI_STATIC_FUNCTION("offset", JsPointUtils::Offset)
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPointUtils::Init Failed to define JsPointUtils class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPointUtils::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPointUtils::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsPointUtils::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPointUtils::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsPointUtils *jsPointUtils = new JsPointUtils();
    status = napi_wrap(env, jsThis, jsPointUtils, JsPointUtils::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPointUtils;
        ROSEN_LOGE("JsPointUtils::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsPointUtils::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPointUtils* napi = reinterpret_cast<JsPointUtils*>(nativeObject);
        delete napi;
    }
}

napi_value JsPointUtils::Negate(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double point[ARGC_TWO] = { 0 };
    if (!ConvertFromJsPoint(env, argv[ARGC_ZERO], point, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsPointUtils::Negate incorrect parameter0 type.");
    }

    Drawing::Point drawingPoint(point[ARGC_ZERO], point[ARGC_ONE]);
    drawingPoint.Negate();
    if (!SetPointToJsValue(env, argv[ARGC_ZERO], drawingPoint)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsPointUtils::Negate cannot fill 'point' Point type.");
    }
    return nullptr;
}

napi_value JsPointUtils::Offset(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_THREE] = { nullptr };
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    double point[ARGC_TWO] = { 0 };
    if (!ConvertFromJsPoint(env, argv[ARGC_ZERO], point, ARGC_TWO)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsPointUtils::Offset incorrect parameter0 type.");
    }

    double dx = 0;
    double dy = 0;
    GET_DOUBLE_PARAM(ARGC_ONE, dx);
    GET_DOUBLE_PARAM(ARGC_TWO, dy);

    Drawing::Point drawingPoint(point[ARGC_ZERO], point[ARGC_ONE]);
    drawingPoint.Offset(dx, dy);
    if (!SetPointToJsValue(env, argv[ARGC_ZERO], drawingPoint)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "JsPointUtils::Offset cannot fill 'point' Point type.");
    }
    return nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen
