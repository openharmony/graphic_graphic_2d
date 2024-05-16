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

#include "color_filter_napi/js_color_filter.h"
#include "js_drawing_utils.h"
#include "mask_filter_napi/js_mask_filter.h"
#include "path_effect_napi/js_path_effect.h"
#include "shadow_layer_napi/js_shadow_layer.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsPen::constructor_ = nullptr;
const std::string CLASS_NAME = "Pen";
napi_value JsPen::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setAntiAlias", SetAntiAlias),
        DECLARE_NAPI_FUNCTION("setAlpha", SetAlpha),
        DECLARE_NAPI_FUNCTION("setBlendMode", SetBlendMode),
        DECLARE_NAPI_FUNCTION("setCapStyle", SetCapStyle),
        DECLARE_NAPI_FUNCTION("getCapStyle", GetCapStyle),
        DECLARE_NAPI_FUNCTION("setColor", SetColor),
        DECLARE_NAPI_FUNCTION("setColorFilter", SetColorFilter),
        DECLARE_NAPI_FUNCTION("setDither", SetDither),
        DECLARE_NAPI_FUNCTION("setJoinStyle", SetJoinStyle),
        DECLARE_NAPI_FUNCTION("getJoinStyle", GetJoinStyle),
        DECLARE_NAPI_FUNCTION("setMaskFilter", SetMaskFilter),
        DECLARE_NAPI_FUNCTION("setPathEffect", SetPathEffect),
        DECLARE_NAPI_FUNCTION("setStrokeWidth", SetStrokeWidth),
        DECLARE_NAPI_FUNCTION("setShadowLayer", SetShadowLayer),
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
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetColor pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    int32_t argb[ARGC_FOUR] = {0};
    if (!ConvertFromJsColor(env, argv[ARGC_ZERO], argb, ARGC_FOUR)) {
        ROSEN_LOGE("JsPen::SetColor Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The range of color channels must be [0, 255].");
    }

    Color color(Color::ColorQuadSetARGB(argb[ARGC_ZERO], argb[ARGC_ONE], argb[ARGC_TWO], argb[ARGC_THREE]));
    pen->SetColor(color);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetStrokeWidth(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetStrokeWidth pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double width = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, width);

    pen->SetWidth(static_cast<float>(width));
    return NapiGetUndefined(env);
}


napi_value JsPen::SetAntiAlias(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAntiAlias pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_boolean);

    bool aa = true;
    if (!ConvertFromJsValue(env, argv[0], aa)) {
        ROSEN_LOGE("JsPen::SetAntiAlias Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    pen->SetAntiAlias(aa);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetAlpha(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t alpha = 0;
    if (!ConvertFromJsNumber(env, argv[ARGC_ZERO], alpha, 0, Color::RGB_MAX)) {
        ROSEN_LOGE("JsPen::SetAlpha Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    pen->SetAlpha(alpha);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetBlendMode(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t mode = 0;
    GET_INT32_CHECK_GE_ZERO_PARAM(ARGC_ZERO, mode);

    pen->SetBlendMode(static_cast<BlendMode>(mode));
    return NapiGetUndefined(env);
}

napi_value JsPen::SetColorFilter(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (!jsPen) {
        return nullptr;
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetAlpha pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    JsColorFilter* jsColorFilter = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsColorFilter));
    if (jsColorFilter == nullptr) {
        ROSEN_LOGE("JsPen::SetColorFilter jsColorFilter is nullptr");
        return NapiGetUndefined(env);
    }

    Filter filter = pen->GetFilter();
    filter.SetColorFilter(jsColorFilter->GetColorFilter());
    pen->SetFilter(filter);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetMaskFilter(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetMaskFilter jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetMaskFilter pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    JsMaskFilter* jsMaskFilter = nullptr;
    napi_unwrap(env, argv[ARGC_ZERO], reinterpret_cast<void **>(&jsMaskFilter));
    if (jsMaskFilter == nullptr) {
        ROSEN_LOGE("JsPen::SetMaskFilter jsMaskFilter is nullptr");
        return NapiGetUndefined(env);
    }

    Filter filter = pen->GetFilter();
    filter.SetMaskFilter(jsMaskFilter->GetMaskFilter());
    pen->SetFilter(filter);
    return NapiGetUndefined(env);
}

napi_value JsPen::SetDither(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_boolean);

    bool dither = false;
    if (!ConvertFromJsValue(env, argv[0], dither)) {
        ROSEN_LOGE("JsPen::SetDither Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    return NapiGetUndefined(env);
}

napi_value JsPen::SetJoinStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetJoinStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetJoinStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t joinStyle = 0;
    GET_INT32_PARAM(ARGC_ZERO, joinStyle);

    pen->SetJoinStyle(static_cast<Pen::JoinStyle>(joinStyle));
    return NapiGetUndefined(env);
}

napi_value JsPen::GetJoinStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetJoinStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetJoinStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    return CreateJsNumber(env, static_cast<int32_t>(pen->GetJoinStyle()));
}

napi_value JsPen::SetCapStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetCapStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetCapStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    int32_t capStyle = 0;
    GET_INT32_PARAM(ARGC_ZERO, capStyle);

    pen->SetCapStyle(static_cast<Pen::CapStyle>(capStyle));
    return NapiGetUndefined(env);
}

napi_value JsPen::GetCapStyle(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::GetCapStyle jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::GetCapStyle pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Pen::CapStyle capStyle = pen->GetCapStyle();
    return CreateJsNumber(env, static_cast<int32_t>(capStyle));
}
napi_value JsPen::SetPathEffect(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetPathEffect jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetPathEffect pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    JsPathEffect* jsPathEffect = nullptr;
    napi_unwrap(env, argv[ARGC_ZERO], reinterpret_cast<void **>(&jsPathEffect));
    if (jsPathEffect == nullptr) {
        ROSEN_LOGE("JsPen::SetPathEffect jsPathEffect is nullptr");
        return NapiGetUndefined(env);
    }

    pen->SetPathEffect(jsPathEffect->GetPathEffect());
    return NapiGetUndefined(env);
}

napi_value JsPen::SetShadowLayer(napi_env env, napi_callback_info info)
{
    JsPen* jsPen = CheckParamsAndGetThis<JsPen>(env, info);
    if (jsPen == nullptr) {
        ROSEN_LOGE("JsPen::SetShadowLayer jsPen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Pen* pen = jsPen->GetPen();
    if (pen == nullptr) {
        ROSEN_LOGE("JsPen::SetShadowLayer pen is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    JsShadowLayer* jsShadowLayer = nullptr;
    napi_unwrap(env, argv[ARGC_ZERO], reinterpret_cast<void **>(&jsShadowLayer));
    if (jsShadowLayer == nullptr) {
        ROSEN_LOGE("JsPen::SetShadowLayer jsShadowLayer is nullptr");
        return NapiGetUndefined(env);
    }
    pen->SetLooper(jsShadowLayer->GetBlurDrawLooper());
    return NapiGetUndefined(env);
}

Pen* JsPen::GetPen()
{
    return pen_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
