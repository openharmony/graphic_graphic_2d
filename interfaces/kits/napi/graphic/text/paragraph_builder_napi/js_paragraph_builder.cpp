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
#include "../drawing/js_drawing_utils.h"
#include "fontcollection_napi/js_fontcollection.h"
#include "js_paragraph_builder.h"
#include "paragraph_napi/js_paragraph.h"
#include "../text/js_text_utils.h"
#include "utils/log.h"

namespace OHOS::Rosen {
std::unique_ptr<Typography> drawingTypography;
thread_local napi_ref JsParagraphBuilder::constructor_ = nullptr;
const std::string CLASS_NAME = "ParagraphBuilder";
napi_value JsParagraphBuilder::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = ARGC_TWO;
    napi_value jsThis = nullptr;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, &jsThis, nullptr);
    if (status != napi_ok || argCount < ARGC_ONE || argCount > ARGC_TWO) {
        ROSEN_LOGE("JsParagraphBuilder::Constructor Argc is invalid: %{public}zu", argCount);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    TypographyStyle typographyStyle;
    GetParagraphStyleFromJS(env, argv[0], typographyStyle);
    JsFontCollection* jsFontCollection = nullptr;
    status = napi_unwrap(env, argv[1], reinterpret_cast<void**>(&jsFontCollection));

    if (jsFontCollection == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (status != napi_ok) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Get jsFontCollection error.");
    }

    std::shared_ptr<FontCollection> fontCollection = jsFontCollection->GetFontCollection();
    std::unique_ptr<TypographyCreate> typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);

    if (!typographyCreate) {
        ROSEN_LOGE("JsParagraphBuilder::Constructor TypographyCreate Create error");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "TypographyCreate Create error.");
    }
    JsParagraphBuilder* jsParagraphBuilder = new(std::nothrow) JsParagraphBuilder();
    if (!jsParagraphBuilder) {
        ROSEN_LOGE("JsParagraphBuilder::Constructor jsParagraphBuilder Create error");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsParagraphBuilder Create error.");
    }
    jsParagraphBuilder->SetTypographyCreate(std::move(typographyCreate));

    status = napi_wrap(env, jsThis, jsParagraphBuilder,
        JsParagraphBuilder::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsParagraphBuilder;
        ROSEN_LOGE("JsParagraphBuilder::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsParagraphBuilder::SetTypographyCreate(std::unique_ptr<TypographyCreate> typographyCreate)
{
    typographyCreate_ = std::move(typographyCreate);
}

napi_value JsParagraphBuilder::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("pushStyle", JsParagraphBuilder::PushStyle),
        DECLARE_NAPI_FUNCTION("addText", JsParagraphBuilder::AddText),
        DECLARE_NAPI_FUNCTION("popStyle", JsParagraphBuilder::PopStyle),
        DECLARE_NAPI_FUNCTION("addPlaceholder", JsParagraphBuilder::AddPlaceholder),
        DECLARE_NAPI_FUNCTION("build", JsParagraphBuilder::Build),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsParagraphBuilder::Init Failed to define FontCollection class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsParagraphBuilder::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsParagraphBuilder::Init Failed to set constructor");
        return nullptr;
    }
    return exportObj;
}

void JsParagraphBuilder::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsParagraphBuilder* napi = reinterpret_cast<JsParagraphBuilder *>(nativeObject);
        delete napi;
    }
}

napi_value JsParagraphBuilder::PushStyle(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    return (me != nullptr) ? me->OnPushStyle(env, info) : nullptr;
}

napi_value JsParagraphBuilder::OnPushStyle(napi_env env, napi_callback_info info)
{
    if (typographyCreate_ == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        return NapiGetUndefined(env);
    }
    TextStyle textStyle;
    if (GetTextStyleFromJS(env, argv[0], textStyle)) {
        typographyCreate_->PushStyle(textStyle);
    }
    return NapiGetUndefined(env);
}

napi_value JsParagraphBuilder::AddText(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    return (me != nullptr) ? me->OnAddText(env, info) : nullptr;
}
napi_value JsParagraphBuilder::OnAddText(napi_env env, napi_callback_info info)
{
    if (typographyCreate_ == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok) {
        return NapiGetUndefined(env);
    }
    std::string text = "";
    if (ConvertFromJsValue(env, argv[0], text)) {
        typographyCreate_->AppendText(Str8ToStr16(text));
    }
    return NapiGetUndefined(env);
}

napi_value JsParagraphBuilder::PopStyle(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    return (me != nullptr) ? me->OnPopStyle(env, info) : nullptr;
}

napi_value JsParagraphBuilder::OnPopStyle(napi_env env, napi_callback_info info)
{
    if (typographyCreate_ == nullptr) {
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    typographyCreate_->PopStyle();
    return NapiGetUndefined(env);
}

napi_value JsParagraphBuilder::AddPlaceholder(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    return (me != nullptr) ? me->OnAddPlaceholder(env, info) : nullptr;
}

napi_value JsParagraphBuilder::OnAddPlaceholder(napi_env env, napi_callback_info info)
{
    if (typographyCreate_ == nullptr) {
        ROSEN_LOGE("OnAddPlaceholder typographyCreate_ is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsParagraphBuilder::AddPlaceholder Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE("JsParagraphBuilder::AddPlaceholder Argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    PlaceholderSpan placeholderSpan;
    bool res = GetPlaceholderSpanFromJS(env, argv[0], placeholderSpan);
    if (!res) {
        return NapiGetUndefined(env);
    }
    typographyCreate_->AppendPlaceholder(placeholderSpan);
    return NapiGetUndefined(env);
}

napi_value JsParagraphBuilder::Build(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    return (me != nullptr) ? me->OnBuild(env, info) : nullptr;
}
napi_value JsParagraphBuilder::OnBuild(napi_env env, napi_callback_info info)
{
    if (typographyCreate_ == nullptr) {
        ROSEN_LOGE("JsParagraphBuilder::OnAddPlaceholder typographyCreate_ is null");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate_->CreateTypography();
    return JsParagraph::CreateJsTypography(env, std::move(typography));
}

} // namespace OHOS::Rosen
