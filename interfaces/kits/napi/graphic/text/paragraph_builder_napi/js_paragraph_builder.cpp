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

#include "js_paragraph_builder.h"

#include <string>

#include "fontcollection_napi/js_fontcollection.h"
#include "js_native_api.h"
#include "line_typeset_napi/js_line_typeset.h"
#include "napi_common.h"
#include "paragraph_napi/js_paragraph.h"
#include "txt/text_bundle_config_parser.h"
#include "utils/string_util.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
namespace {
const std::string CLASS_NAME = "ParagraphBuilder";
}

thread_local napi_ref JsParagraphBuilder::constructor_ = nullptr;

napi_value JsParagraphBuilder::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = ARGC_TWO;
    napi_value jsThis = nullptr;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argCount, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::unique_ptr<TypographyCreate> typographyCreate = nullptr;
    if (argCount == ARGC_TWO) {
        TypographyStyle typographyStyle;
        GetParagraphStyleFromJS(env, argv[0], typographyStyle);
        JsFontCollection* jsFontCollection = nullptr;
        status = napi_unwrap(env, argv[1], reinterpret_cast<void**>(&jsFontCollection));

        if (jsFontCollection == nullptr) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        }
        if (status != napi_ok) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Get jsFontCollection error.");
        }

        std::shared_ptr<FontCollection> fontCollection = jsFontCollection->GetFontCollection();
        typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
        if (!typographyCreate) {
            TEXT_LOGE("Failed to create typography creator");
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "TypographyCreate Create error.");
        }
    } else if (argCount != ARGC_ZERO) {
        TEXT_LOGE("Invalid params, argc %{public}zu, ret %{public}d", argCount, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    JsParagraphBuilder* jsParagraphBuilder = new(std::nothrow) JsParagraphBuilder();
    if (!jsParagraphBuilder) {
        TEXT_LOGE("Failed to create paragraph builder");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsParagraphBuilder Create error.");
    }
    if (typographyCreate != nullptr) {
        jsParagraphBuilder->SetTypographyCreate(std::move(typographyCreate));
    }

    status = napi_wrap(env, jsThis, jsParagraphBuilder,
        JsParagraphBuilder::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to wrap paragraphy builder, ret %{public}d", status);
        delete jsParagraphBuilder;
        return nullptr;
    }

    return jsThis;
}

void JsParagraphBuilder::SetTypographyCreate(std::unique_ptr<TypographyCreate> typographyCreate)
{
    typographyCreate_ = std::move(typographyCreate);
}

std::shared_ptr<TypographyCreate> JsParagraphBuilder::GetTypographyCreate()
{
    return typographyCreate_;
}

napi_status JsParagraphBuilder::CreateTypographyCreate(napi_env env, napi_value exportObj, napi_value* obj)
{
    return NewInstanceFromConstructor(env, exportObj, CLASS_NAME.c_str(), obj);
}

napi_status JsParagraphBuilder::SetTypographyCreate(
    napi_env env, napi_value obj, std::shared_ptr<TypographyCreate> typographyCreate)
{
    if (env == nullptr || obj == nullptr || typographyCreate == nullptr) {
        TEXT_LOGE("Invalid arguments");
        return napi_invalid_arg;
    }
    JsParagraphBuilder* jsParagraphBuilder = nullptr;
    napi_status status = napi_unwrap(env, obj, reinterpret_cast<void**>(&jsParagraphBuilder));
    if (status != napi_ok || jsParagraphBuilder == nullptr) {
        TEXT_LOGE("Failed to unwrap jsParagraphBuilder, status: %{public}d", status);
        return status;
    }
    jsParagraphBuilder->typographyCreate_ = typographyCreate;
    return napi_ok;
}


napi_value JsParagraphBuilder::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("pushStyle", JsParagraphBuilder::PushStyle),
        DECLARE_NAPI_FUNCTION("addText", JsParagraphBuilder::AddText),
        DECLARE_NAPI_FUNCTION("popStyle", JsParagraphBuilder::PopStyle),
        DECLARE_NAPI_FUNCTION("addPlaceholder", JsParagraphBuilder::AddPlaceholder),
        DECLARE_NAPI_FUNCTION("build", JsParagraphBuilder::Build),
        DECLARE_NAPI_FUNCTION("buildLineTypeset", JsParagraphBuilder::BuildLineTypeset),
        DECLARE_NAPI_FUNCTION("addSymbol", JsParagraphBuilder::AppendSymbol),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to define class, ret %{public}d", status);
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to create reference");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set named property, ret %{public}d", status);
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
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
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
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid typographyCreate");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid param 0");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok) {
        return NapiGetUndefined(env);
    }

    if (SPText::TextBundleConfigParser::GetInstance().IsTargetApiVersion(SPText::SINCE_API18_VERSION)) {
        size_t len = 0;
        if (napi_get_value_string_utf16(env, argv[0], nullptr, 0, &len) != napi_ok) {
            TEXT_LOGE("Failed to get utf16 length");
            return NapiGetUndefined(env);
        }

        auto buffer = std::make_unique<char16_t[]>(len + 1);
        if (napi_get_value_string_utf16(env, argv[0], buffer.get(), len + 1, &len) != napi_ok) {
            TEXT_LOGE("Failed to get utf16");
            return NapiGetUndefined(env);
        }
        typographyCreate_->AppendText(std::u16string {buffer.get()});
    } else {
        std::string text = "";
        if (ConvertFromJsValue(env, argv[0], text)) {
            if (!IsUtf8(text.c_str(), text.size())) {
                TEXT_LOGE("Invalid utf-8 text");
                return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params");
            }
            typographyCreate_->AppendText(Str8ToStr16(text));
        }
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
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
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
        TEXT_LOGE("Null typography creator");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        TEXT_LOGE("Invalid argv[0]");
        return NapiGetUndefined(env);
    }
    PlaceholderSpan placeholderSpan;
    bool res = GetPlaceholderSpanFromJS(env, argv[0], placeholderSpan);
    if (!res) {
        TEXT_LOGE("Failed to get placeholder");
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
        TEXT_LOGE("Null typography builder");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate_->CreateTypography();
    return JsParagraph::CreateJsTypography(env, std::move(typography));
}

napi_value JsParagraphBuilder::BuildLineTypeset(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    return (me != nullptr) ? me->OnBuildLineTypeset(env, info) : nullptr;
}

napi_value JsParagraphBuilder::OnBuildLineTypeset(napi_env env, napi_callback_info info)
{
    if (typographyCreate_ == nullptr) {
        TEXT_LOGE("Null typography creator");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::unique_ptr<OHOS::Rosen::LineTypography> lineTypography = typographyCreate_->CreateLineTypography();
    if (lineTypography == nullptr) {
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed to create line typography.");
    }
    return JsLineTypeset::CreateJsLineTypeset(env, std::move(lineTypography));
}

napi_value JsParagraphBuilder::AppendSymbol(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    return (me != nullptr) ? me->OnAppendSymbol(env, info) : nullptr;
}

napi_value JsParagraphBuilder::OnAppendSymbol(napi_env env, napi_callback_info info)
{
    if (typographyCreate_ == nullptr) {
        TEXT_LOGE("Null typography creator");
        return nullptr;
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if ((status != napi_ok) || (argc < ARGC_ONE)) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return nullptr;
    }

    uint32_t symbolId = 0;
    if (!ConvertFromJsNumber(env, argv[0], symbolId)) {
        TEXT_LOGE("Failed to convert symbol id");
        return nullptr;
    }
    typographyCreate_->AppendSymbol(symbolId);
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
