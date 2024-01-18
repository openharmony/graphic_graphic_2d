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

#include "js_pen.h"

#include <cstdint>

#include "../js_drawing_utils.h"
#include "../color_filter_napi/js_color_filter.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPen::constructor_ = nullptr;
const std::string CLASS_NAME = "Pen";
napi_value JsPen::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setColor", SetColor),
        DECLARE_NAPI_FUNCTION("setStrokeWidth", SetStrokeWidth),
        DECLARE_NAPI_FUNCTION("setAntiAlias", SetAntiAlias),
        DECLARE_NAPI_FUNCTION("setAlpha", SetAlpha),
        DECLARE_NAPI_FUNCTION("setColorFilter", SetColorFilter),
        DECLARE_NAPI_FUNCTION("setBlendMode", SetBlendMode),
        DECLARE_NAPI_FUNCTION("setDither", SetDither),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Init Failed to define Pen class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsPen::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsPen::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsPen* jsPen = new(std::nothrow) JsPen();
    status = napi_wrap(env, jsThis, jsPen,
                       JsPen::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsPen;
        ROSEN_LOGE("JsPen::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsPen::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsPen* napi = reinterpret_cast<JsPen*>(nativeObject);
        delete napi;
    }
}

JsPen::JsPen()
{
    pen_ = new Pen();
}

JsPen::~JsPen()
{
    delete pen_;
}

napi_value JsPen::SetColor(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetColor pen is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPen::SetColor Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsPen::SetColor Argv[0] is invalid");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_value tempValue = nullptr;
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, argv[0], "alpha", &tempValue);
    ConvertClampFromJsValue(env, tempValue, alpha, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "red", &tempValue);
    ConvertClampFromJsValue(env, tempValue, red, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "green", &tempValue);
    ConvertClampFromJsValue(env, tempValue, green, 0, Color::RGB_MAX);
    napi_get_named_property(env, argv[0], "blue", &tempValue);
    ConvertClampFromJsValue(env, tempValue, blue, 0, Color::RGB_MAX);

    Color color(Color::ColorQuadSetARGB(alpha, red, green, blue));
    pen->SetColor(color);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetStrokeWidth(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetStrokeWidth pen is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPen::SetStrokeWidth Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_number) {
        ROSEN_LOGE("JsPen::SetStrokeWidth Argv[0] is invalid");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    double width;
    ConvertFromJsNumber(env, argv[0], width);
    pen->SetWidth(static_cast<float>(width));
    return NapiGetUndefined(env);
}


napi_value JsPen::SetAntiAlias(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAntiAlias pen is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPen::SetAntiAlias Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_boolean) {
        ROSEN_LOGE("JsPen::SetAntiAlias Argv[0] is invalid");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    bool aa;
    ConvertFromJsNumber(env, argv[0], aa);
    pen->SetAntiAlias(aa);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetAlpha(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPen::SetAlpha Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_number) {
        ROSEN_LOGE("JsPen::SetAlpha Argv[0] is invalid");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    int32_t alpha = 0;
    ConvertClampFromJsValue(env, argv[0], alpha, 0, Color::RGB_MAX);
    pen->SetAlpha(alpha);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetBlendMode(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPen::SetBlendMode Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_number) {
        ROSEN_LOGE("JsPen::SetBlendMode Argv[0] is invalid");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    uint32_t mode = 0;
    ConvertFromJsNumber(env, argv[0], mode);
    pen->SetBlendMode(static_cast<BlendMode>(mode));
    return NapiGetUndefined(env);
}

napi_value JsPen::SetColorFilter(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_NULLPTR);
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPen::SetColorFilter Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsPen::SetColorFilter Argv[0] is invalid");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    JsColorFilter* jsColorFilter = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsColorFilter));

    Filter filter = pen->GetFilter();
    filter.SetColorFilter(jsColorFilter->GetColorFilter());
    pen->SetFilter(filter);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetDither(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsPen::SetDither Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_boolean) {
        ROSEN_LOGE("JsPen::SetDither Argv[0]");
        return NapiThrowError(env, DrawingError::DRAWING_ERROR_INVALID_PARAM);
    }

    bool dither;
    ConvertFromJsNumber(env, argv[0], dither);
    return NapiGetUndefined(env);
}

Pen* JsPen::GetPen()
{
    return pen_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
