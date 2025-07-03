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
#include "recording/recording_canvas.h"
#include "run_napi/js_run.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
namespace {
const std::string CLASS_NAME = "TextLine";
}

std::mutex JsTextLine::constructorMutex_;
thread_local napi_ref JsTextLine::constructor_ = nullptr;

napi_value JsTextLine::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get parameter, ret %{public}d", status);
        return nullptr;
    }
    JsTextLine *jsTextLineBase = new(std::nothrow) JsTextLine();
    if (!jsTextLineBase) {
        TEXT_LOGE("Failed to new text line");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsTextLineBase,
                       JsTextLine::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsTextLineBase;
        TEXT_LOGE("Failed to wrap text line, ret %{public}d", status);
        return nullptr;
    }
    return jsThis;
}


napi_value JsTextLine::Init(napi_env env, napi_value exportObj)
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
        TEXT_LOGE("Failed to set named property, ret %{public}d", status);
        return nullptr;
    }
    return exportObj;
}

bool JsTextLine::CreateConstructor(napi_env env)
{
    std::lock_guard<std::mutex> lock(constructorMutex_);
    if (constructor_) {
        return true;
    }
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getGlyphCount", JsTextLine::GetGlyphCount),
        DECLARE_NAPI_FUNCTION("getGlyphRuns", JsTextLine::GetGlyphRuns),
        DECLARE_NAPI_FUNCTION("getTextRange", JsTextLine::GetTextRange),
        DECLARE_NAPI_FUNCTION("paint", JsTextLine::Paint),
        DECLARE_NAPI_FUNCTION("createTruncatedLine", JsTextLine::CreateTruncatedLine),
        DECLARE_NAPI_FUNCTION("getTrailingSpaceWidth", JsTextLine::GetTrailingSpaceWidth),
        DECLARE_NAPI_FUNCTION("getTypographicBounds", JsTextLine::GetTypographicBounds),
        DECLARE_NAPI_FUNCTION("getImageBounds", JsTextLine::GetImageBounds),
        DECLARE_NAPI_FUNCTION("getStringIndexForPosition", JsTextLine::GetStringIndexForPosition),
        DECLARE_NAPI_FUNCTION("getOffsetForStringIndex", JsTextLine::GetOffsetForStringIndex),
        DECLARE_NAPI_FUNCTION("enumerateCaretOffsets", JsTextLine::EnumerateCaretOffsets),
        DECLARE_NAPI_FUNCTION("getAlignmentOffset", JsTextLine::GetAlignmentOffset),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to define class, ret %{public}d", status);
        return false;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to create reference, ret %{public}d", status);
        return false;
    }
    return true;
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
    if (!CreateConstructor(env)) {
        TEXT_LOGE("Failed to create constructor");
        return nullptr;
    }
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

napi_value JsTextLine::CreateTruncatedLine(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnCreateTruncatedLine(env, info, constructor_) : nullptr;
}

napi_value JsTextLine::GetTypographicBounds(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetTypographicBounds(env, info) : nullptr;
}

napi_value JsTextLine::GetImageBounds(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetImageBounds(env, info) : nullptr;
}

napi_value JsTextLine::GetTrailingSpaceWidth(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetTrailingSpaceWidth(env, info) : nullptr;
}

napi_value JsTextLine::GetStringIndexForPosition(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetStringIndexForPosition(env, info) : nullptr;
}

napi_value JsTextLine::GetOffsetForStringIndex(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetOffsetForStringIndex(env, info) : nullptr;
}

napi_value JsTextLine::EnumerateCaretOffsets(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnEnumerateCaretOffsets(env, info) : nullptr;
}

napi_value JsTextLine::GetAlignmentOffset(napi_env env, napi_callback_info info)
{
    JsTextLine* me = CheckParamsAndGetThis<JsTextLine>(env, info);
    return (me != nullptr) ? me->OnGetAlignmentOffset(env, info) : nullptr;
}

napi_value JsTextLine::OnGetGlyphCount(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    uint32_t textSize = textLine_->GetGlyphCount();
    return CreateJsValue(env, textSize);
}

napi_value JsTextLine::OnGetGlyphRuns(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::vector<std::unique_ptr<Run>> runs = textLine_->GetGlyphRuns();
    if (runs.empty()) {
        TEXT_LOGE("Run is empty");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value array = nullptr;
    NAPI_CALL(env, napi_create_array(env, &array));
    uint32_t index = 0;
    for (std::unique_ptr<Run>& item : runs) {
        napi_value itemObject = JsRun::CreateRun(env, info);
        if (!itemObject) {
            TEXT_LOGE("Failed to create run");
            continue;
        }
        JsRun* jsRun = nullptr;
        napi_unwrap(env, itemObject, reinterpret_cast<void**>(&jsRun));
        if (!jsRun) {
            TEXT_LOGE("Failed to unwrap run");
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
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
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
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        TEXT_LOGE("Failed to get paramter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (argv[0] == nullptr) {
        TEXT_LOGE("Null argv[0]");
        return NapiGetUndefined(env);
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsCanvas));
    if (!jsCanvas || !jsCanvas->GetCanvas()) {
        TEXT_LOGE("Failed to get canvas");
        return NapiGetUndefined(env);
    }
    double x = 0.0;
    double y = 0.0;
    if (!(argv[ARGC_ONE] != nullptr && ConvertFromJsValue(env, argv[ARGC_ONE], x) &&
        argv[ARGC_TWO] != nullptr && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        return NapiGetUndefined(env);
    }
    textLine_->Paint(jsCanvas->GetCanvas(), x, y);

    return NapiGetUndefined(env);
}

napi_value JsTextLine::OnCreateTruncatedLine(napi_env env, napi_callback_info info, napi_ref constructor)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        TEXT_LOGE("Failed to get argc(%{public}zu)", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double width = 0.0;
    if (!ConvertFromJsValue(env, argv[0], width)) {
        TEXT_LOGE("Failed to convert width");
        return NapiGetUndefined(env);
    }
    uint32_t ellipsisMode = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], ellipsisMode)) {
        TEXT_LOGE("Failed to convert ellipsisMode");
        return NapiGetUndefined(env);
    }
    std::string ellipsisStr = "";
    if (!ConvertFromJsValue(env, argv[ARGC_TWO], ellipsisStr)) {
        TEXT_LOGE("Failed to convert ellipsisStr");
        return NapiGetUndefined(env);
    }

    std::unique_ptr<TextLineBase> textLine = textLine_->CreateTruncatedLine(width, EllipsisModal(ellipsisMode),
        ellipsisStr);
    if (textLine == nullptr) {
        TEXT_LOGE("Failed to create truncated textLine");
        return NapiGetUndefined(env);
    }

    napi_value itemObject = JsTextLine::CreateTextLine(env, info);
    if (itemObject == nullptr) {
        TEXT_LOGE("Failed to create js textLine");
        return NapiGetUndefined(env);
    }

    JsTextLine* jsTextLine = nullptr;
    status = napi_unwrap(env, itemObject, reinterpret_cast<void**>(&jsTextLine));
    if (status != napi_ok || jsTextLine == nullptr) {
        TEXT_LOGE("Failed to napi_unwrap jsTextLine");
        return NapiGetUndefined(env);
    }
    jsTextLine->SetTextLine(std::move(textLine));
    jsTextLine->SetParagraph(paragraph_);

    return itemObject;
}

napi_value JsTextLine::OnGetTypographicBounds(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    double width = textLine_->GetTypographicBounds(&ascent, &descent, &leading);

    napi_value objValue = nullptr;
    napi_status status = napi_create_object(env, &objValue);
    if (status != napi_ok || objValue == nullptr) {
        TEXT_LOGE("Failed to create object, ret %{public}d", static_cast<int>(status));
        return NapiGetUndefined(env);
    }

    status = napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, ascent));
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set ascent, ret %{public}d", static_cast<int>(status));
        return NapiGetUndefined(env);
    }
    status = napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, descent));
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set descent, ret %{public}d", static_cast<int>(status));
        return NapiGetUndefined(env);
    }
    status = napi_set_named_property(env, objValue, "leading", CreateJsNumber(env, leading));
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set leading, ret %{public}d", static_cast<int>(status));
        return NapiGetUndefined(env);
    }
    status = napi_set_named_property(env, objValue, "width", CreateJsNumber(env, width));
    if (status != napi_ok) {
        TEXT_LOGE("Failed to set width, ret %{public}d", static_cast<int>(status));
        return NapiGetUndefined(env);
    }

    return objValue;
}

napi_value JsTextLine::OnGetImageBounds(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    Drawing::Rect rect = textLine_->GetImageBounds();
    return GetRectAndConvertToJsValue(env, rect);
}

napi_value JsTextLine::OnGetTrailingSpaceWidth(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double width = textLine_->GetTrailingSpaceWidth();
    return CreateJsValue(env, width);
}

napi_value JsTextLine::OnGetStringIndexForPosition(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get argc(%{public}zu)", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double x = 0.0;
    double y = 0.0;
    napi_value tempValue = nullptr;
    status = napi_get_named_property(env, argv[0], "x", &tempValue);
    if (status != napi_ok || tempValue == nullptr) {
        TEXT_LOGE("Failed to get x, ret %{public}d", static_cast<int>(status));
        return NapiGetUndefined(env);
    }
    if (!ConvertFromJsValue(env, tempValue, x)) {
        TEXT_LOGE("Failed to convert x");
        return NapiGetUndefined(env);
    }
    status = napi_get_named_property(env, argv[0], "y", &tempValue);
    if (status != napi_ok || tempValue == nullptr) {
        TEXT_LOGE("Failed to get y, ret %{public}d", static_cast<int>(status));
        return NapiGetUndefined(env);
    }
    if (!ConvertFromJsValue(env, tempValue, y)) {
        TEXT_LOGE("Failed to convert y");
        return NapiGetUndefined(env);
    }

    SkPoint point = {x, y};
    int32_t index = textLine_->GetStringIndexForPosition(point);
    return CreateJsValue(env, index);
}

napi_value JsTextLine::OnGetOffsetForStringIndex(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get argc(%{public}zu)", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    int32_t index = 0;
    if (!ConvertFromJsValue(env, argv[0], index)) {
        TEXT_LOGE("Failed to convert index");
        return NapiGetUndefined(env);
    }

    double offset = textLine_->GetOffsetForStringIndex(index);
    return CreateJsValue(env, offset);
}

bool CallJsFunc(napi_env env, napi_value callback, int32_t index, double leftOffset, double rightOffset)
{
    static napi_value jsLeadingEdgeTrue = CreateJsValue(env, true);
    static napi_value jsLeadingEdgeFalse = CreateJsValue(env, false);
    napi_value jsIndex = CreateJsValue(env, index);
    for (size_t i = 0; i < ARGC_TWO; i++) {
        napi_value jsOffset = (i == 0) ? CreateJsValue(env, leftOffset) : CreateJsValue(env, rightOffset);
        napi_value jsLeadingEdge = (i == 0) ? jsLeadingEdgeTrue : jsLeadingEdgeFalse;
        napi_value retVal = nullptr;
        napi_value params[ARGC_THREE] = {jsOffset, jsIndex, jsLeadingEdge};
        napi_status status = napi_call_function(env, nullptr, callback, ARGC_THREE, params, &retVal);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to napi_call_function");
            return false;
        }

        bool stop = false;
        if (!ConvertFromJsValue(env, retVal, stop)) {
            TEXT_LOGE("Failed to convert stop");
            return false;
        }
        if (stop) {
            TEXT_LOGI("Js call stoped");
            return false;
        }
    }

    return true;
}

napi_value JsTextLine::OnEnumerateCaretOffsets(napi_env env, napi_callback_info info)
{
    TEXT_ERROR_CHECK(textLine_ != nullptr,
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params."), "TextLine is nullptr");

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE];
    napi_value jsCallback = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsCallback, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get argc(%{public}zu)", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    if (status != napi_ok || valueType != napi_function) {
        TEXT_LOGE("Failed to get napi type or argc is not function");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_ref refCallback = nullptr;
    std::unique_ptr<napi_ref, std::function<void(napi_ref*)>> refCallbackGuard(&refCallback, [env](napi_ref* ref) {
        if (ref != nullptr && *ref != nullptr) {
            TEXT_CHECK(napi_delete_reference(env, *ref) == napi_ok, TEXT_LOGE("Failed to release ref callback"));
        }
    });
    status = napi_create_reference(env, argv[0], 1, &refCallback);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to create reference");
        return NapiGetUndefined(env);
    }

    napi_value callback = nullptr;
    if ((napi_get_reference_value(env, refCallback, &callback)) != napi_ok) {
        TEXT_LOGE("Failed to get reference");
        return NapiGetUndefined(env);
    }

    bool isHardBreak = false;
    std::map<int32_t, double> offsetMap = textLine_->GetIndexAndOffsets(isHardBreak);
    double leftOffset = 0.0;
    for (auto it = offsetMap.begin(); it != offsetMap.end(); ++it) {
        if (!CallJsFunc(env, callback, it->first, leftOffset, it->second)) {
            return NapiGetUndefined(env);
        }
        leftOffset = it->second;
    }
    if (isHardBreak && offsetMap.size() > 0) {
        if (!CallJsFunc(env, callback, offsetMap.rbegin()->first + 1, leftOffset, leftOffset)) {
            return NapiGetUndefined(env);
        }
    }

    return NapiGetUndefined(env);
}

napi_value JsTextLine::OnGetAlignmentOffset(napi_env env, napi_callback_info info)
{
    if (textLine_ == nullptr) {
        TEXT_LOGE("Null text line");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        TEXT_LOGE("Failed to get argc(%{public}zu)", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double alignmentFactor = 0.0;
    if (!ConvertFromJsValue(env, argv[0], alignmentFactor)) {
        TEXT_LOGE("Failed to convert alignmentFactor");
        return NapiGetUndefined(env);
    }
    double alignmentWidth = 0.0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], alignmentWidth)) {
        TEXT_LOGE("Failed to convert alignmentWidth");
        return NapiGetUndefined(env);
    }

    double offset = textLine_->GetAlignmentOffset(alignmentFactor, alignmentWidth);
    return CreateJsValue(env, offset);
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