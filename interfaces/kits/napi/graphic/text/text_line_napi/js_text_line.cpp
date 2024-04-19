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

#include "canvas_napi/js_canvas.h"
#include "js_text_line.h"
#include "js_drawing_utils.h"
#include "recording/recording_canvas.h"
#include "run_napi/js_run.h"
#include "utils/log.h"

namespace OHOS::Rosen {
thread_local napi_ref JsTextLine::constructor_ = nullptr;
const std::string CLASS_NAME = "TextLine";
napi_value JsTextLine::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }
    JsTextLine *jsTextLineBase = new(std::nothrow) JsTextLine();
    if (!jsTextLineBase) {
        ROSEN_LOGE("failed to create JsTextLine");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsTextLineBase,
                       JsTextLine::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTextLineBase;
        ROSEN_LOGE("JsTextLine::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}


napi_value JsTextLine::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getGlyphCount", JsTextLine::GetGlyphCount),
        DECLARE_NAPI_FUNCTION("getGlyphRuns", JsTextLine::GetGlyphRuns),
        DECLARE_NAPI_FUNCTION("getTextRange", JsTextLine::GetTextRange),
        DECLARE_NAPI_FUNCTION("paint", JsTextLine::Paint),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define TextLine class");
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
    return exportObj;
}

void JsTextLine::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsTextLine *napi = reinterpret_cast<JsTextLine *>(nativeObject);
        delete napi;
    }
}

napi_value JsTextLine::CreateTextLine(napi_env env, napi_callback_info info)
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

JsTextLine::JsTextLine()
{
}

void JsTextLine::SetTextLine(std::unique_ptr<TextLineBase> textLine)
{
    textLine_ = std::move(textLine);
}

napi_value JsTextLine::GetGlyphCount(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetGlyphCount(env, info) : nullptr;
}

napi_value JsTextLine::GetGlyphRuns(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetGlyphRuns(env, info) : nullptr;
}

napi_value JsTextLine::GetTextRange(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetTextRange(env, info) : nullptr;
}

napi_value JsTextLine::Paint(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnPaint(env, info) : nullptr;
}

napi_value JsTextLine::OnGetGlyphCount(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        ROSEN_LOGE("JsTextLine::OnGetGlyphCount textLine_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    uint32_t textSize = textLine_->GetGlyphCount();
    return CreateJsValue(env, textSize);
}

napi_value JsTextLine::OnGetGlyphRuns(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        ROSEN_LOGE("JsTextLine::OnGetGlyphRuns TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::vector<std::unique_ptr<Run>> runs = textLine_->GetGlyphRuns();
    if (runs.empty()) {
        ROSEN_LOGE("JsTextLine::OnGetGlyphRuns runs is empty");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value array = nullptr;
    NAPI_CALL(env, napi_create_array(env, &array));
    uint32_t index = 0;
    for (std::unique_ptr<Run>& item : runs) {
        napi_value itemObject = JsRun::CreateRun(env, info);
        if (!itemObject) {
            ROSEN_LOGE("JsTextLine::OnGetGlyphRuns itemObject is null");
            continue;
        }
        JsRun* jsRun = nullptr;
        napi_unwrap(env, itemObject, reinterpret_cast<void**>(&jsRun));
        if (!jsRun) {
            ROSEN_LOGE("JsTextLine::OnGetGlyphRuns napi_unwrap jsRun is null");
            continue;
        }
        jsRun->SetRun(std::move(item));
        jsRun->SetParagraph(paragraph_);
        napi_set_element(env, array, index++, itemObject);
    }
    return array;
}

napi_value JsTextLine::OnGetTextRange(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        ROSEN_LOGE("JsTextLine::OnGetTextRange TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Boundary boundary = textLine_->GetTextRange();
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "start", CreateJsNumber(env, boundary.leftIndex));
        napi_set_named_property(env, objValue, "end", CreateJsNumber(env, boundary.rightIndex));
    }
    return objValue;
}

napi_value JsTextLine::OnPaint(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        ROSEN_LOGE("JsTextLine::OnPaint TextLine is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsTextLine::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsCanvas));
    if (jsCanvas == nullptr) {
        ROSEN_LOGE("JsTextLine::OnPaint jsCanvas is nullptr");
        return NapiGetUndefined(env);
    }
    if (jsCanvas->GetCanvas() == nullptr) {
        ROSEN_LOGE("JsTextLine::OnPaint canvas is nullptr");
        return NapiGetUndefined(env);
    }
    double x = 0.0;
    double y = 0.0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        return NapiGetUndefined(env);
    }
    if (jsCanvas->GetCanvas()->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        Drawing::RecordingCanvas* recordingCanvas = (Drawing::RecordingCanvas*)jsCanvas->GetCanvas();
        recordingCanvas->SetIsCustomTypeface(true);
        recordingCanvas->SetIsCustomTextType(true);
        textLine_->Paint(recordingCanvas, x, y);
    } else {
        textLine_->Paint(jsCanvas->GetCanvas(), x, y);
    }

    return NapiGetUndefined(env);
}

std::unique_ptr<TextLineBase> JsTextLine::GetTextLineBase()
{
    return std::move(textLine_);
}

void JsTextLine::SetParagraph(std::shared_ptr<Typography> paragraph)
{
    paragraph_ = paragraph;
}
} // namespace OHOS::Rosen