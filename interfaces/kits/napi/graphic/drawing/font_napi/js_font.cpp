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

#include "js_font.h"

#include "native_value.h"

#include "js_drawing_utils.h"
#include "js_typeface.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsFont::constructor_ = nullptr;
const std::string CLASS_NAME = "Font";
napi_value JsFont::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("enableSubpixel", JsFont::EnableSubpixel),
        DECLARE_NAPI_FUNCTION("enableEmbolden", JsFont::EnableEmbolden),
        DECLARE_NAPI_FUNCTION("enableLinearMetrics", JsFont::EnableLinearMetrics),
        DECLARE_NAPI_FUNCTION("setSize", JsFont::SetSize),
        DECLARE_NAPI_FUNCTION("setTypeface", JsFont::SetTypeface),
        DECLARE_NAPI_FUNCTION("getTypeface", JsFont::GetTypeface),
        DECLARE_NAPI_FUNCTION("getSize", JsFont::GetSize),
        DECLARE_NAPI_FUNCTION("getMetrics", JsFont::GetMetrics),
        DECLARE_NAPI_FUNCTION("measureText", JsFont::MeasureText),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Font class");
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

    napi_property_descriptor staticProperty[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createFont", JsFont::CreateFont),
    };
    status = napi_define_properties(env, exportObj, 1, staticProperty);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

napi_value JsFont::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    std::shared_ptr<Font> font = std::make_shared<Font>();
    font->SetTypeface(JsTypeface::LoadZhCnTypeface());
    JsFont *jsFont = new(std::nothrow) JsFont(font);

    status = napi_wrap(env, jsThis, jsFont,
                       JsFont::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsFont;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsFont::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsFont *napi = reinterpret_cast<JsFont *>(nativeObject);
        delete napi;
    }
}

napi_value JsFont::CreateFont(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to get the representation of constructor object");
        return nullptr;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to instantiate JavaScript font instance");
        return nullptr;
    }
    return result;
}

JsFont::~JsFont()
{
    m_font = nullptr;
}

napi_value JsFont::EnableSubpixel(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableSubpixel(env, info) : nullptr;
}

napi_value JsFont::EnableEmbolden(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableEmbolden(env, info) : nullptr;
}

napi_value JsFont::EnableLinearMetrics(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableLinearMetrics(env, info) : nullptr;
}

napi_value JsFont::SetSize(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetSize(env, info) : nullptr;
}

napi_value JsFont::SetTypeface(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetTypeface(env, info) : nullptr;
}

napi_value JsFont::GetTypeface(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetTypeface(env, info) : nullptr;
}

napi_value JsFont::GetSize(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetSize(env, info) : nullptr;
}

napi_value JsFont::GetMetrics(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetMetrics(env, info) : nullptr;
}

napi_value JsFont::MeasureText(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnMeasureText(env, info) : nullptr;
}

napi_value JsFont::OnEnableSubpixel(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableSubpixel font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsFont::OnEnableSubpixel Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool isSubpixel = false;
    if (!ConvertFromJsValue(env, argv[0], isSubpixel)) {
        ROSEN_LOGE("JsFont::OnEnableSubpixel Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    m_font->SetSubpixel(isSubpixel);
    return NapiGetUndefined(env);
}

napi_value JsFont::OnEnableEmbolden(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableEmbolden font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsFont::OnEnableEmbolden Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool isEmbolden = false;
    if (!ConvertFromJsValue(env, argv[0], isEmbolden)) {
        ROSEN_LOGE("JsFont::OnEnableEmbolden Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    m_font->SetEmbolden(isEmbolden);
    return NapiGetUndefined(env);
}

napi_value JsFont::OnEnableLinearMetrics(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableLinearMetrics font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsFont::OnEnableLinearMetrics Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool isLinearMetrics = false;
    if (!ConvertFromJsValue(env, argv[0], isLinearMetrics)) {
        ROSEN_LOGE("JsFont::OnEnableLinearMetrics Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    m_font->SetLinearMetrics(isLinearMetrics);
    return NapiGetUndefined(env);
}

napi_value JsFont::OnSetSize(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetSize font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsFont::OnSetSize Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double textSize = 0.0;
    if (!ConvertFromJsValue(env, argv[0], textSize)) {
        ROSEN_LOGE("JsFont::OnSetSize Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    m_font->SetSize((float)textSize);
    return NapiGetUndefined(env);
}

napi_value JsFont::OnGetSize(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetSize font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double textSize = m_font->GetSize();
    return GetDoubleAndConvertToJsValue(env, textSize);
}

napi_value JsFont::OnGetMetrics(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetMetrics font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    FontMetrics metrics;
    m_font->GetMetrics(&metrics);
    return GetFontMetricsAndConvertToJsValue(env, &metrics);
}

napi_value JsFont::OnSetTypeface(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetTypeface font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsFont::OnSetTypeface Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    JsTypeface *jsTypeface = nullptr;
    napi_unwrap(env, argv[0], (void **)&jsTypeface);
    if (jsTypeface == nullptr) {
        ROSEN_LOGE("JsFont::OnSetTypeface jsTypeface is nullptr");
        return NapiGetUndefined(env);
    }

    m_font->SetTypeface(jsTypeface->GetTypeface());
    return NapiGetUndefined(env);
}

napi_value JsFont::OnGetTypeface(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetTypeface font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::shared_ptr<Typeface> typeface = m_font->GetTypeface();
    return JsTypeface::CreateJsTypeface(env, typeface);
}

napi_value JsFont::OnMeasureText(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnMeasureText font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE("JsFont::OnMeasureText Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::string text = "";
    if (!ConvertFromJsValue(env, argv[0], text)) {
        ROSEN_LOGE("JsFont::OnMeasureText Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    TextEncoding TextEncoding = TextEncoding::UTF8;
    if (!ConvertFromJsTextEncoding(env, TextEncoding, argv[1])) {
        ROSEN_LOGE("JsFont::OnMeasureText ConvertFromJsTextEncoding failed");
        return NapiGetUndefined(env);
    }

    double textSize = m_font->MeasureText(text.c_str(), text.length(), TextEncoding);
    return GetDoubleAndConvertToJsValue(env, textSize);
}

std::shared_ptr<Font> JsFont::GetFont()
{
    return m_font;
}

void JsFont::SetFont(std::shared_ptr<Font> font)
{
    m_font = font;
}
} // namespace Drawing
} // namespace OHOS::Rosen