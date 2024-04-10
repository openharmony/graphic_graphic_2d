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
#include "utils/log.h"
#include "js_paragraph.h"
#include "paragraph_builder_napi/js_paragraph_builder.h"
#include "../js_text_utils.h"
#include "draw/canvas.h"
#include "canvas_napi/js_canvas.h"
#include "text_line_napi/js_text_line.h"
#include "../drawing/js_drawing_utils.h"

namespace OHOS::Rosen {
std::unique_ptr<Typography> g_Typography = nullptr;
thread_local napi_ref JsParagraph::constructor_ = nullptr;
const std::string CLASS_NAME = "JsParagraph";

napi_value JsParagraph::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsParagraph::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    if (!g_Typography) {
        ROSEN_LOGE("JsParagraph::Constructor g_Typography is nullptr");
        return nullptr;
    }

    JsParagraph *jsParagraph = new(std::nothrow) JsParagraph(std::move(g_Typography));

    status = napi_wrap(env, jsThis, jsParagraph,
        JsParagraph::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsParagraph;
        ROSEN_LOGE("JsParagraph::Constructor failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

napi_value JsParagraph::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("layout", JsParagraph::Layout),
        DECLARE_NAPI_FUNCTION("paint", JsParagraph::Paint),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Paragraph class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of result");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set result");
        return nullptr;
    }
    return exportObj;
}


void JsParagraph::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsParagraph *napi = reinterpret_cast<JsParagraph *>(nativeObject);
        delete napi;
    }
}

napi_value JsParagraph::Layout(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnLayout(env, info) : nullptr;
}

napi_value JsParagraph::OnLayout(napi_env env, napi_callback_info info)
{
    if (!paragraphCurrent_) {
        ROSEN_LOGE("JsParagraph::OnLayout paragraphCurrent is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE("JsParagraph::OnLayout Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double width = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], width))) {
        ROSEN_LOGE("JsParagraph::OnLayout Argv is invalid");
        return NapiGetUndefined(env);
    }

    paragraphCurrent_->Layout(width);
    return NapiGetUndefined(env);
}

napi_value JsParagraph::Paint(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnPaint(env, info) : nullptr;
}

napi_value JsParagraph::OnPaint(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE("JsParagraph::OnPaint paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsParagraph::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsCanvas));
    if (jsCanvas == nullptr ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        ROSEN_LOGE("JsParagraph::OnPaint Argv is invalid");
        return NapiGetUndefined(env);
    }
    paragraphCurrent_->Paint(jsCanvas->GetCanvas(), x, y);
    return NapiGetUndefined(env);
}

JsParagraph::JsParagraph(std::shared_ptr<Typography> typography)
    : paragraphCurrent_(typography)
{
}

JsParagraph::~JsParagraph()
{
}

napi_value JsParagraph::CreateJsTypography(napi_env env, std::unique_ptr<Typography> typography)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        //paragraphCurrent_ = std::move(typography);
        g_Typography = std::move(typography);
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            return result;
        } else {
            ROSEN_LOGE("CreateJsTypography: New instance could not be obtained");
        }
    }
    return result;
}
} // namespace OHOS::Rosen