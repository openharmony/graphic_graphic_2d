/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "js_text_global.h"
#include "napi_common.h"
#include "text_global_config.h"
#include "utils/text_log.h"

namespace OHOS::Rosen::SrvText {

napi_value JsTextGlobal::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("setTextHighContrast", JsTextGlobal::SetTextHighContrast),
        DECLARE_NAPI_STATIC_FUNCTION("setTextUndefinedGlyphDisplay", JsTextGlobal::SetTextUndefinedGlyphDisplay),
    };
    napi_status status = napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to define TextGlobal properties, status %{public}d", status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to define TextGlobal properties");
    }
    return exportObj;
}

napi_value JsTextGlobal::SetTextHighContrast(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if ((status != napi_ok) || (argc < ARGC_ONE)) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argc is invalid");
    }
    uint32_t textHighContrast = 0;
    if (!ConvertFromJsValue(env, argv[0], textHighContrast)) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argv convert failed");
    }

    auto result = TextGlobalConfig::SetTextHighContrast(textHighContrast);
    if (result != TEXT_SUCCESS) {
        TEXT_LOGE("Failed to set text high contrast, high contrast value: %{public}u", textHighContrast);
    }
    return NapiGetUndefined(env);
}

napi_value JsTextGlobal::SetTextUndefinedGlyphDisplay(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if ((status != napi_ok) || (argc < ARGC_ONE)) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argc is invalid");
    }
    uint32_t textUndefinedGlyphDisplay = 0;
    if (!ConvertFromJsValue(env, argv[0], textUndefinedGlyphDisplay)) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argv convert failed");
    }

    auto result = TextGlobalConfig::SetTextUndefinedGlyphDisplay(textUndefinedGlyphDisplay);
    if (result != TEXT_SUCCESS) {
        TEXT_LOGE("Failed to set text undefined glyph display, undefined glyph display: %{public}u",
            textUndefinedGlyphDisplay);
    }
    return NapiGetUndefined(env);
}

} // namespace OHOS::Rosen