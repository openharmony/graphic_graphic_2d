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

#include "js_roundrect.h"
#include "../js_drawing_utils.h"
#include "native_value.h"
#include "../js_common.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsRoundRect::constructor_ = nullptr;
const std::string CLASS_NAME = "RoundRect";
napi_value JsRoundRect::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Init Failed to define RoundRect class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsRoundRect::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRoundRect::Constructor failed to napi_get_cb_info");
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsRoundRect::Constructor Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    double ltrb[ARGC_FOUR] = {0};
    if (!ConvertFromJsRect(env, argv[ARGC_ZERO], ltrb, ARGC_FOUR)) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Incorrect parameter0 type. The type of left, top, right and bottom must be number.");
    }
    Drawing::Rect drawingRect = Drawing::Rect(ltrb[ARGC_ZERO], ltrb[ARGC_ONE], ltrb[ARGC_TWO], ltrb[ARGC_THREE]);

    double xRad = 0.0;
    GET_DOUBLE_PARAM(ARGC_ONE, xRad);
    double yRad = 0.0;
    GET_DOUBLE_PARAM(ARGC_TWO, yRad);

    JsRoundRect* jsRoundRect = nullptr;
    std::shared_ptr<RoundRect> roundRect = nullptr;
    roundRect = std::make_shared<RoundRect>(drawingRect, xRad, yRad);
    jsRoundRect = new(std::nothrow) JsRoundRect(roundRect);
    status = napi_wrap(env, jsThis, jsRoundRect,
                       JsRoundRect::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRoundRect;
        ROSEN_LOGE("JsRoundRect::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsRoundRect::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRoundRect* napi = reinterpret_cast<JsRoundRect*>(nativeObject);
        delete napi;
    }
}

JsRoundRect::~JsRoundRect()
{
    m_roundRect = nullptr;
}

std::shared_ptr<RoundRect> JsRoundRect::GetRoundRect()
{
    return m_roundRect;
}
} // namespace Drawing
} // namespace OHOS::Rosen

