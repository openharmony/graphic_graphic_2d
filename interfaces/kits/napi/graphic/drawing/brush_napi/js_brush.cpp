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

#include "js_brush.h"

#include <cstdint>

#include "js_drawing_utils.h"
#include "color_filter_napi/js_color_filter.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsBrush::constructor_ = nullptr;
const std::string CLASS_NAME = "Brush";
napi_value JsBrush::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setColor", SetColor),
        DECLARE_NAPI_FUNCTION("setAntiAlias", SetAntiAlias),
        DECLARE_NAPI_FUNCTION("setAlpha", SetAlpha),
        DECLARE_NAPI_FUNCTION("setColorFilter", SetColorFilter),
        DECLARE_NAPI_FUNCTION("setBlendMode", SetBlendMode),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsBrush::Init Failed to define Brush class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsBrush::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsBrush::Init Failed to set constructor");
        return nullptr;
    }
    return exportObj;
}

napi_value JsBrush::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsBrush::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsBrush* jsBrush = new(std::nothrow) JsBrush();
    status = napi_wrap(env, jsThis, jsBrush,
                       JsBrush::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsBrush;
        ROSEN_LOGE("JsBrush::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsBrush::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsBrush* napi = reinterpret_cast<JsBrush*>(nativeObject);
        delete napi;
    }
}

JsBrush::JsBrush()
{
    brush_ = new Brush();
}

JsBrush::~JsBrush()
{
    delete brush_;
}

napi_value JsBrush::SetColor(napi_env env, napi_callback_info info)
{
    JsBrush* jsBrush = CheckParamsAndGetThis<JsBrush>(env, info);
    if (!jsBrush) {
        return nullptr;
    }
    Brush* brush = jsBrush->GetBrush();
    if (brush == nullptr) {
        ROSEN_LOGE("JsBrush::SetColor brush is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsBrush::SetColor Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsBrush::SetColor Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    napi_value tempValue = nullptr;
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, argv[0], "alpha", &tempValue);
    bool isAlphaOk = ConvertClampFromJsValue(env, tempValue, alpha, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "red", &tempValue);
    bool isRedOk = ConvertClampFromJsValue(env, tempValue, red, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "green", &tempValue);
    bool isGreenOk = ConvertClampFromJsValue(env, tempValue, green, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "blue", &tempValue);
    bool isBlueOk = ConvertClampFromJsValue(env, tempValue, blue, 0, Color::RGB_MAX);
    if (!(isAlphaOk && isRedOk && isGreenOk && isBlueOk)) {
        ROSEN_LOGE("JsBrush::SetColor Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    Color color(Color::ColorQuadSetARGB(alpha, red, green, blue));
    brush->SetColor(color);
    return NapiGetUndefined(env);
}

napi_value JsBrush::SetAntiAlias(napi_env env, napi_callback_info info)
{
    JsBrush* jsBrush = CheckParamsAndGetThis<JsBrush>(env, info);
    if (!jsBrush) {
        return nullptr;
    }
    Brush* brush = jsBrush->GetBrush();
    if (brush == nullptr) {
        ROSEN_LOGE("JsBrush::SetAntiAlias brush is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsBrush::SetAntiAlias Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool aa = true;
    if (!ConvertFromJsValue(env, argv[0], aa)) {
        ROSEN_LOGE("JsBrush::SetAntiAlias Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    brush->SetAntiAlias(aa);
    return NapiGetUndefined(env);
}

napi_value JsBrush::SetAlpha(napi_env env, napi_callback_info info)
{
    JsBrush* jsBrush = CheckParamsAndGetThis<JsBrush>(env, info);
    if (!jsBrush) {
        return nullptr;
    }
    Brush* brush = jsBrush->GetBrush();
    if (brush == nullptr) {
        ROSEN_LOGE("JsBrush::SetAlpha brush is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsBrush::SetAlpha Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    int32_t alpha = 0;
    if (!ConvertClampFromJsValue(env, argv[0], alpha, 0, Color::RGB_MAX)) {
        ROSEN_LOGE("JsBrush::SetAlpha Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    brush->SetAlpha(alpha);
    return NapiGetUndefined(env);
}

napi_value JsBrush::SetColorFilter(napi_env env, napi_callback_info info)
{
    JsBrush* jsBrush = CheckParamsAndGetThis<JsBrush>(env, info);
    if (!jsBrush) {
        return nullptr;
    }
    Brush* brush = jsBrush->GetBrush();
    if (brush == nullptr) {
        ROSEN_LOGE("JsBrush::SetColorFilter brush is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsBrush::SetColorFilter Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsColorFilter* jsColorFilter = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsColorFilter));
    if (jsColorFilter == nullptr) {
        ROSEN_LOGE("JsBrush::SetColorFilter jsColorFilter is nullptr");
        return NapiGetUndefined(env);
    }

    Filter filter = brush->GetFilter();
    filter.SetColorFilter(jsColorFilter->GetColorFilter());
    brush->SetFilter(filter);
    return NapiGetUndefined(env);
}

napi_value JsBrush::SetBlendMode(napi_env env, napi_callback_info info)
{
    JsBrush* jsBrush = CheckParamsAndGetThis<JsBrush>(env, info);
    if (!jsBrush) {
        return nullptr;
    }
    Brush* brush = jsBrush->GetBrush();
    if (brush == nullptr) {
        ROSEN_LOGE("JsBrush::SetBlendMode brush is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsBrush::SetBlendMode Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    uint32_t mode = 0;
    if (!ConvertFromJsValue(env, argv[0], mode)) {
        ROSEN_LOGE("JsBrush::SetBlendMode Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    brush->SetBlendMode(static_cast<BlendMode>(mode));
    return NapiGetUndefined(env);
}

Brush* JsBrush::GetBrush()
{
    return brush_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
