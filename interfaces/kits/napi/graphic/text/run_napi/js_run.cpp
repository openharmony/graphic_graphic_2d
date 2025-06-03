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

#include "js_run.h"
#include "napi_common.h"
#include "utils/text_log.h"
#include "canvas_napi/js_canvas.h"
#include "font_napi/js_font.h"
#include "recording/recording_canvas.h"
#include "typography_types.h"

namespace OHOS::Rosen {
thread_local napi_ref JsRun::constructor_ = nullptr;
const std::string CLASS_NAME = "Run";
napi_value JsRun::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get parameter, ret %{public}d", status);
        return nullptr;
    }

    JsRun* jsRun = new(std::nothrow) JsRun();
    if (!jsRun) {
        TEXT_LOGE("Failed to new run");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsRun,
        JsRun::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRun;
        TEXT_LOGE("Failed to wrap run, ret %{public}d", status);
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
        DECLARE_NAPI_FUNCTION("getStringRange", JsRun::GetStringRange),
        DECLARE_NAPI_FUNCTION("getStringIndices", JsRun::GetStringIndices),
        DECLARE_NAPI_FUNCTION("getImageBounds", JsRun::GetImageBounds),
        DECLARE_NAPI_FUNCTION("getTypographicBounds", JsRun::GetTypographicBounds),
        DECLARE_NAPI_FUNCTION("paint", JsRun::Paint),
        DECLARE_NAPI_FUNCTION("getTextDirection", JsRun::GetTextDirection),
        DECLARE_NAPI_FUNCTION("getAdvances", JsRun::GetAdvances),
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
        TEXT_LOGE("Failed to create reference, ret %{public}d", status);
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set named property, ret %{public}d", status);
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
        TEXT_LOGE("Failed to get reference, ret %{public}d", status);
        return nullptr;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to new instance, ret %{public}d", status);
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
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetGlyphCount run is nullptr.");
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
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed run is nullptr");
    }

    size_t argc = ARGC_ONE;
    int64_t start = 0;
    int64_t end = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get parameter, ret %{public}d", static_cast<int>(status));
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    } else if (argc == ARGC_ONE) {
        if (!GetStartEndParams(env, argv[0], start, end)) {
            return NapiGetUndefined(env);
        }
    }

    std::vector<uint16_t> glyphs = run_->GetGlyphs(start, end);
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
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed run is nullptr.");
    }

    size_t argc = ARGC_ONE;
    int64_t start = 0;
    int64_t end = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get the info");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    } else if (argc == ARGC_ONE) {
        if (!GetStartEndParams(env, argv[0], start, end)) {
            return NapiGetUndefined(env);
        }
    }

    std::vector<Drawing::Point> positions = run_->GetPositions(start, end);
    napi_value napiPositions = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiPositions));
    size_t positionSize = positions.size();
    for (size_t index = 0; index < positionSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiPositions, index,
            GetPointAndConvertToJsValue(env, positions.at(index))));
    }
    return napiPositions;
}

napi_value JsRun::GetAdvances(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetAdvances(env, info) : nullptr;
}

napi_value JsRun::OnGetAdvances(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("Failed run is nullptr");
        return NapiGetUndefined(env);
    }

    size_t argc = ARGC_ONE;
    int64_t start = 0;
    int64_t end = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (!GetStartEndParams(env, argv[0], start, end)) {
        return NapiGetUndefined(env);
    }

    std::vector<Drawing::Point> advances = run_->GetAdvances(start, end);
    napi_value napiAdvances = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiAdvances));
    size_t advanceSize = advances.size();
    for (size_t index = 0; index < advanceSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiAdvances, index,
            GetPointAndConvertToJsValue(env, advances.at(index))));
    }
    return napiAdvances;
}

napi_value JsRun::GetTextDirection(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetTextDirection(env, info) : nullptr;
}

napi_value JsRun::OnGetTextDirection(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("Failed run is nullptr");
        return NapiGetUndefined(env);
    }

    TextDirection textDirection = run_->GetTextDirection();
    return CreateJsNumber(env, (int)textDirection);
}

napi_value JsRun::GetOffsets(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetOffsets(env, info) : nullptr;
}

napi_value JsRun::OnGetOffsets(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetOffsets run is nullptr.");
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
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont run is nullptr.");
    }

    std::shared_ptr<Drawing::Font> fontPtr = std::make_shared<Drawing::Font>(run_->GetFont());
    if (!fontPtr) {
        TEXT_LOGE("Null font");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont fontPtr is nullptr.");
    }

    napi_value resultValue  = Drawing::JsFont::CreateFont(env, info);
    if (!resultValue) {
        TEXT_LOGE("Failed to create font");
        return nullptr;
    }
    Drawing::JsFont* jsFont = nullptr;
    napi_unwrap(env, resultValue, reinterpret_cast<void**>(&jsFont));
    if (!jsFont) {
        TEXT_LOGE("Failed to unwrap font");
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
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnPaint run is nullptr.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (argv[0] == nullptr) {
        TEXT_LOGE("Invalid argv[0]");
        return NapiGetUndefined(env);
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsCanvas));
    if (!jsCanvas || !jsCanvas->GetCanvas() ||
        !(argv[ARGC_ONE] != nullptr && ConvertFromJsValue(env, argv[ARGC_ONE], x) &&
        argv[ARGC_TWO] != nullptr && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        TEXT_LOGE("Failed to get paint parameter");
        return NapiGetUndefined(env);
    }
    run_->Paint(jsCanvas->GetCanvas(), x, y);

    return NapiGetUndefined(env);
}

void JsRun::SetParagraph(std::shared_ptr<Typography> paragraph)
{
    paragraph_ = paragraph;
}

napi_value JsRun::GetStringRange(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetStringRange(env, info) : nullptr;
}

napi_value JsRun::OnGetStringRange(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed run is nullptr.");
    }
    uint64_t location = 0;
    uint64_t length = 0;
    run_->GetStringRange(&location, &length);
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_status status = napi_set_named_property(env, objValue, "start", CreateJsNumber(env, location));
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set named property, ret %{public}d", status);
            return nullptr;
        }
        status = napi_set_named_property(env, objValue, "end", CreateJsNumber(env, length));
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set named property, ret %{public}d", status);
            return nullptr;
        }
    }
    return objValue;
}

napi_value JsRun::GetStringIndices(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetStringIndices(env, info) : nullptr;
}

napi_value JsRun::OnGetStringIndices(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed run is nullptr.");
    }

    size_t argc = ARGC_ONE;
    int64_t start = 0;
    int64_t end = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get the info");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    } else if (argc == ARGC_ONE) {
        if (!GetStartEndParams(env, argv[0], start, end)) {
            return NapiGetUndefined(env);
        }
    }

    std::vector<uint64_t> stringIndices = run_->GetStringIndices(start, end);
    napi_value napiStringIndices = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiStringIndices));
    size_t stringIndicesSize = stringIndices.size();
    for (size_t index = 0; index < stringIndicesSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiStringIndices, index,
            CreateJsNumber(env, stringIndices.at(index))));
    }
    return napiStringIndices;
}

napi_value JsRun::GetImageBounds(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetImageBounds(env, info) : nullptr;
}

napi_value JsRun::OnGetImageBounds(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed run is nullptr.");
    }
    Drawing::Rect imageBounds = run_->GetImageBounds();
    return GetRectAndConvertToJsValue(env, imageBounds);
}

napi_value JsRun::GetTypographicBounds(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetTypographicBounds(env, info) : nullptr;
}

napi_value JsRun::OnGetTypographicBounds(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("Null run");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Failed run is nullptr.");
    }

    float ascent = 0.0;
    float descent = 0.0;
    float leading = 0.0;
    float width = run_->GetTypographicBounds(&ascent, &descent, &leading);
    return GetTypographicBoundsAndConvertToJsValue(env, ascent, descent, leading, width);
}
} // namespace OHOS::Rosen
