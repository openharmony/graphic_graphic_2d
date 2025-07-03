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

#include "js_line_typeset.h"
#include "text_line_napi/js_text_line.h"
#include "utils/log.h"

namespace OHOS::Rosen {
namespace {
const std::string CLASS_NAME = "LineTypeset";
napi_property_descriptor g_properties[] = {
    DECLARE_NAPI_FUNCTION("getLineBreak", JsLineTypeset::GetLineBreak),
    DECLARE_NAPI_FUNCTION("createLine", JsLineTypeset::CreateLine),
};
}
std::mutex JsLineTypeset::constructorMutex_;
thread_local napi_ref JsLineTypeset::constructor_ = nullptr;

napi_value JsLineTypeset::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get callback info, ret %{public}d", status);
        return nullptr;
    }

    JsLineTypeset *jsLineTypeset = new(std::nothrow) JsLineTypeset();
    if (jsLineTypeset == nullptr) {
        TEXT_LOGE("Failed to new JsLineTypeset");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsLineTypeset, JsLineTypeset::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsLineTypeset;
        TEXT_LOGE("Failed to wrap native instance, ret %{public}d", status);
        return nullptr;
    }
    return jsThis;
}

void JsLineTypeset::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsLineTypeset *napi = reinterpret_cast<JsLineTypeset *>(nativeObject);
        delete napi;
    }
}

JsLineTypeset::JsLineTypeset(std::shared_ptr<LineTypography> lineTypography) : lineTypography_(lineTypography)
{
}

JsLineTypeset::JsLineTypeset()
{
}

JsLineTypeset::~JsLineTypeset()
{
}

napi_value JsLineTypeset::CreateJsLineTypeset(napi_env env, std::unique_ptr<LineTypography> lineTypography)
{
    if (!CreateConstructor(env)) {
        TEXT_LOGE("Failed to create constructor");
        return nullptr;
    }
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        napi_create_object(env, &result);
        if (result == nullptr) {
            TEXT_LOGE("Failed to create object.");
            return nullptr;
        }
        auto jsLineTypeset = new(std::nothrow) JsLineTypeset(std::move(lineTypography));
        if (jsLineTypeset == nullptr) {
            TEXT_LOGE("Failed to new JsLineTypeset.");
            return nullptr;
        }
        status = napi_wrap(env, result, jsLineTypeset, JsLineTypeset::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            delete jsLineTypeset;
            TEXT_LOGE("Failed to wrap, ret %{public}d", status);
            return nullptr;
        }
        napi_define_properties(env, result, sizeof(g_properties) / sizeof(g_properties[0]), g_properties);
        return result;
    }
    return result;
}

napi_value JsLineTypeset::Init(napi_env env, napi_value exportObj)
{
    if (!CreateConstructor(env)) {
        TEXT_LOGE("Failed to create constructor");
        return nullptr;
    }
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get reference, ret %{public}d", status);
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set property, ret %{public}d", status);
        return nullptr;
    }
    return exportObj;
}

bool JsLineTypeset::CreateConstructor(napi_env env)
{
    std::lock_guard<std::mutex> lock(constructorMutex_);
    if (constructor_) {
        return true;
    }
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(g_properties) / sizeof(g_properties[0]), g_properties, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to define class, status %{public}d", status);
        return false;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to create reference, status %{public}d", status);
        return false;
    }
    return true;
}

napi_value JsLineTypeset::GetLineBreak(napi_env env, napi_callback_info info)
{
    JsLineTypeset* me = CheckParamsAndGetThis<JsLineTypeset>(env, info);
    return (me != nullptr) ? me->OnGetLineBreak(env, info) : nullptr;
}

napi_value JsLineTypeset::OnGetLineBreak(napi_env env, napi_callback_info info)
{
    if (lineTypography_ == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Line typography is null.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argc is invalid.");
    }
    int64_t index = 0;
    double width = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], index) && ConvertFromJsValue(env, argv[1], width))) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argv convert failed.");
    }
    size_t limitSize = lineTypography_->GetUnicodeSize();
    if (index < 0 || limitSize <= static_cast<size_t>(index) || width <= 0) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Params exceeds reasonable range.");
    }
    size_t count = static_cast<size_t>(lineTypography_->GetLineBreak(static_cast<size_t>(index), width));
    return CreateJsNumber(env, count);
}

napi_value JsLineTypeset::CreateLine(napi_env env, napi_callback_info info)
{
    JsLineTypeset* me = CheckParamsAndGetThis<JsLineTypeset>(env, info);
    return (me != nullptr) ? me->OnCreateLine(env, info) : nullptr;
}

napi_value JsLineTypeset::OnCreateLine(napi_env env, napi_callback_info info)
{
    if (lineTypography_ == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Line typography is null.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argc is invalid.");
    }
    int64_t index = 0;
    int64_t count = 0;
    if (!(ConvertFromJsValue(env, argv[0], index) && ConvertFromJsValue(env, argv[1], count))) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argv convert failed");
    }
    size_t limitSize = lineTypography_->GetUnicodeSize();
    if (index < 0 || limitSize <= static_cast<size_t>(index) || count < 0 ||
        static_cast<size_t>(count + index) > limitSize) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Params exceeds reasonable range.");
    }
    auto textLineBase = lineTypography_->CreateLine(static_cast<size_t>(index), static_cast<size_t>(count));
    if (!textLineBase) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Create line failed.");
    }
    std::shared_ptr<Typography> typography = lineTypography_->GetTempTypography();
    napi_value itemObject = JsTextLine::CreateTextLine(env, info);
    if (!itemObject) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to create JsTextLine object.");
    }
    JsTextLine* jsTextLine = nullptr;
    napi_unwrap(env, itemObject, reinterpret_cast<void**>(&jsTextLine));
    if (!jsTextLine) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to unwrap JsTextLine object.");
    }
    jsTextLine->SetTextLine(std::move(textLineBase));
    jsTextLine->SetParagraph(typography);
    return itemObject;
}

} // namespace OHOS::Rosen
