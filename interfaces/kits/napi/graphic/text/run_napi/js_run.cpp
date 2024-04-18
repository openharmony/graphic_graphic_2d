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


#include "js_drawing_utils.h"
#include "js_run.h"
#include "js_text_utils.h"
#include "canvas_napi/js_canvas.h"
#include "font_napi/js_font.h"
#include "recording/recording_canvas.h"

namespace OHOS::Rosen {
thread_local napi_ref JsRun::constructor_ = nullptr;
const std::string CLASS_NAME = "Run";
napi_value JsRun::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRun::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsRun* jsRun = new(std::nothrow) JsRun();
    if (!jsRun) {
        ROSEN_LOGE("JsRun::Constructor failed to create JsRun");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsRun,
        JsRun::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRun;
        ROSEN_LOGE("JsRun::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

napi_value JsRun::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getGlyphCount", JsRun::GetGlyphCount),
        DECLARE_NAPI_FUNCTION("getGlyphs", JsRun::GetGlyphs),
        DECLARE_NAPI_FUNCTION("getPositions", JsRun::GetPositions),
        DECLARE_NAPI_FUNCTION("getOffsets", JsRun::GetOffsets),
        DECLARE_NAPI_FUNCTION("getFont", JsRun::GetFont),
        DECLARE_NAPI_FUNCTION("paint", JsRun::Paint),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRun::Init Failed to define JsRun class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRun::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRun::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

void JsRun::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRun* jsRun = reinterpret_cast<JsRun*>(nativeObject);
        delete jsRun;
    }
}

napi_value JsRun::CreateRun(napi_env env, napi_callback_info info)
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

JsRun::JsRun()
{
}

void JsRun::SetRun(std::unique_ptr<Run> run)
{
    run_ = std::move(run);
}

napi_value JsRun::GetGlyphCount(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetGlyphCount(env, info) : nullptr;
}

napi_value JsRun::OnGetGlyphCount(napi_env env, napi_callback_info info)
{
    if (!run_) {
        ROSEN_LOGE("JsRun::OnGetGlyphCount run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetGlyphCount run is nullptr.");
    }
    int64_t count = static_cast<int64_t>(run_->GetGlyphCount());
    return CreateJsNumber(env, count);
}

napi_value JsRun::GetGlyphs(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetGlyphs(env, info) : nullptr;
}

napi_value JsRun::OnGetGlyphs(napi_env env, napi_callback_info info)
{
    if (!run_) {
        ROSEN_LOGE("JsRun::OnGetGlyphs run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetGlyphs run is nullptr.");
    }

    std::vector<uint16_t> glyphs = run_->GetGlyphs();
    napi_value napiGlyphs = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiGlyphs));
    size_t glyphSize = glyphs.size();
    for (size_t index = 0; index < glyphSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiGlyphs, index,
            CreateJsNumber(env, static_cast<uint32_t>(glyphs.at(index)))));
    }
    return napiGlyphs;
}

napi_value JsRun::GetPositions(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetPositions(env, info) : nullptr;
}

napi_value JsRun::OnGetPositions(napi_env env, napi_callback_info info)
{
    if (!run_) {
        ROSEN_LOGE("JsRun::OnGetPositions run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetPositions run is nullptr.");
    }

    std::vector<Drawing::Point> positions = run_->GetPositions();
    napi_value napiPositions = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiPositions));
    size_t positionSize = positions.size();
    for (size_t index = 0; index < positionSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiPositions, index,
            GetPointAndConvertToJsValue(env, positions.at(index))));
    }
    return napiPositions;
}

napi_value JsRun::GetOffsets(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetOffsets(env, info) : nullptr;
}

napi_value JsRun::OnGetOffsets(napi_env env, napi_callback_info info)
{
    if (!run_) {
        ROSEN_LOGE("JsRun::OnGetOffsets run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetOffsets run is nullptr.");
    }

    std::vector<Drawing::Point> offsets = run_->GetOffsets();
    napi_value napiOffsets = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiOffsets));
    size_t offsetSize = offsets.size();
    for (size_t index = 0; index < offsetSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiOffsets, index,
            GetPointAndConvertToJsValue(env, offsets.at(index))));
    }
    return napiOffsets;
}

napi_value JsRun::GetFont(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetFont(env, info) : nullptr;
}

napi_value JsRun::OnGetFont(napi_env env, napi_callback_info info)
{
    if (!run_) {
        ROSEN_LOGE("JsRun::OnGetFont run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont run is nullptr.");
    }

    std::shared_ptr<Drawing::Font> fontPtr = std::make_shared<Drawing::Font>(run_->GetFont());
    if (!fontPtr) {
        ROSEN_LOGE("JsRun::OnGetFont fontPtr is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont fontPtr is nullptr.");
    }

    napi_value resultValue  = Drawing::JsFont::CreateFont(env, info);
    if (!resultValue) {
        ROSEN_LOGE("JsRun::OnGetFont JsFont::CreateFont resultValue is null");
        return nullptr;
    }
    Drawing::JsFont* jsFont = nullptr;
    napi_unwrap(env, resultValue, reinterpret_cast<void**>(&jsFont));
    if (!jsFont) {
        ROSEN_LOGE("JsRun::OnGetFont napi_unwrap jsFont is null");
        return nullptr;
    }
    jsFont->SetFont(fontPtr);
    return resultValue;
}

napi_value JsRun::Paint(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnPaint(env, info) : nullptr;
}

napi_value JsRun::OnPaint(napi_env env, napi_callback_info info)
{
    if (run_ == nullptr) {
        ROSEN_LOGE("JsRun::OnPaint run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnPaint run is nullptr.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE("JsRun::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsCanvas));
    if (jsCanvas == nullptr ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        ROSEN_LOGE("JsRun::OnPaint Argv is invalid");
        return NapiGetUndefined(env);
    }
    if (jsCanvas->GetCanvas()->GetDrawingType() == Drawing::DrawingType::RECORDING) {
        Drawing::RecordingCanvas* recordingCanvas = (Drawing::RecordingCanvas*)jsCanvas->GetCanvas();
        recordingCanvas->SetIsCustomTypeface(true);
        recordingCanvas->SetIsCustomTextType(true);
        run_->Paint(recordingCanvas, x, y);
    } else {
        run_->Paint(jsCanvas->GetCanvas(), x, y);
    }

    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
