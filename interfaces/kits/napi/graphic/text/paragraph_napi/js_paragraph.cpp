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

#include "js_paragraph.h"
#include <cmath>
#include "canvas_napi/js_canvas.h"
#include "draw/canvas.h"
#include "napi_async_work.h"
#include "napi_common.h"
#include "paragraph_builder_napi/js_paragraph_builder.h"
#include "path_napi/js_path.h"
#include "recording/recording_canvas.h"
#include "text_line_napi/js_text_line.h"
#include "text_style.h"
#include "typography_style.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
namespace {
const std::string CLASS_NAME = "Paragraph";
}

thread_local napi_ref JsParagraph::constructor_ = nullptr;

napi_value JsParagraph::Constructor(napi_env env, napi_callback_info info)
{
    napi_value jsThis = nullptr;
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &jsThis, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get parameter, ret %{public}d", status);
        return nullptr;
    }

    void* nativePointer = nullptr;
    if (!(argv[0] != nullptr && napi_get_value_external(env, argv[0], &nativePointer) == napi_ok)) {
        TEXT_LOGE("Failed to convert");
        return nullptr;
    }

    std::shared_ptr<Typography> typography(static_cast<Typography*>(nativePointer));
    if (typography == nullptr) {
        TEXT_LOGE("Null typography");
        return nullptr;
    }

    JsParagraph* jsParagraph = new (std::nothrow) JsParagraph(typography);
    if (jsParagraph == nullptr) {
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsParagraph,
        JsParagraph::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsParagraph;
        TEXT_LOGE("Failed to wrap paragraph, ret %{public}d", status);
        return nullptr;
    }
    return jsThis;
}

napi_value JsParagraph::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("layoutSync", JsParagraph::Layout),
        DECLARE_NAPI_FUNCTION("paint", JsParagraph::Paint),
        DECLARE_NAPI_FUNCTION("paintOnPath", JsParagraph::PaintOnPath),
        DECLARE_NAPI_FUNCTION("getMaxWidth", JsParagraph::GetMaxWidth),
        DECLARE_NAPI_FUNCTION("getHeight", JsParagraph::GetHeight),
        DECLARE_NAPI_FUNCTION("getLongestLine", JsParagraph::GetLongestLine),
        DECLARE_NAPI_FUNCTION("getLongestLineWithIndent", JsParagraph::GetLongestLineWithIndent),
        DECLARE_NAPI_FUNCTION("getMinIntrinsicWidth", JsParagraph::GetMinIntrinsicWidth),
        DECLARE_NAPI_FUNCTION("getMaxIntrinsicWidth", JsParagraph::GetMaxIntrinsicWidth),
        DECLARE_NAPI_FUNCTION("getAlphabeticBaseline", JsParagraph::GetAlphabeticBaseline),
        DECLARE_NAPI_FUNCTION("getIdeographicBaseline", JsParagraph::GetIdeographicBaseline),
        DECLARE_NAPI_FUNCTION("getRectsForRange", JsParagraph::GetRectsForRange),
        DECLARE_NAPI_FUNCTION("getRectsForPlaceholders", JsParagraph::GetRectsForPlaceholders),
        DECLARE_NAPI_FUNCTION("getGlyphPositionAtCoordinate", JsParagraph::GetGlyphPositionAtCoordinate),
        DECLARE_NAPI_FUNCTION("getWordBoundary", JsParagraph::GetWordBoundary),
        DECLARE_NAPI_FUNCTION("getLineCount", JsParagraph::GetLineCount),
        DECLARE_NAPI_FUNCTION("getLineHeight", JsParagraph::GetLineHeight),
        DECLARE_NAPI_FUNCTION("getLineWidth", JsParagraph::GetLineWidth),
        DECLARE_NAPI_FUNCTION("didExceedMaxLines", JsParagraph::DidExceedMaxLines),
        DECLARE_NAPI_FUNCTION("getTextLines", JsParagraph::GetTextLines),
        DECLARE_NAPI_FUNCTION("getActualTextRange", JsParagraph::GetActualTextRange),
        DECLARE_NAPI_FUNCTION("getLineMetrics", JsParagraph::GetLineMetrics),
        DECLARE_NAPI_FUNCTION("getFontMetricsByTextStyle", JsParagraph::GetFontMetricsByTextStyle),
        DECLARE_NAPI_FUNCTION("getLineFontMetrics", JsParagraph::GetLineFontMetrics),
        DECLARE_NAPI_FUNCTION("layout", JsParagraph::LayoutAsync),
        DECLARE_NAPI_FUNCTION("isStrutStyleEqual", JsParagraph::IsStrutStyleEqual),
        DECLARE_NAPI_FUNCTION("updateColor", JsParagraph::UpdateColor),
        DECLARE_NAPI_FUNCTION("updateDecoration", JsParagraph::UpdateDecoration),
    };
    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to define paragraph class, ret %{public}d", status);
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
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double width = 0.0;
    if (!(argv[0] != nullptr && ConvertFromJsValue(env, argv[0], width))) {
        TEXT_LOGE("Failed to convert");
        return NapiGetUndefined(env);
    }

    paragraph_->Layout(width);
    return NapiGetUndefined(env);
}

napi_value JsParagraph::Paint(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnPaint(env, info) : nullptr;
}

napi_value JsParagraph::OnPaint(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    if (argv[0] == nullptr) {
        TEXT_LOGE("Null argv[0]");
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
    paragraph_->Paint(jsCanvas->GetCanvas(), x, y);

    return NapiGetUndefined(env);
}

napi_value JsParagraph::PaintOnPath(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnPaintOnPath(env, info) : nullptr;
}

napi_value JsParagraph::OnPaintOnPath(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_FOUR) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    Drawing::JsPath* jsPath = nullptr;
    double hOffset = 0.0;
    double vOffset = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void**>(&jsCanvas));
    GET_UNWRAP_PARAM(ARGC_ONE, jsPath);
    if (!jsCanvas || !jsCanvas->GetCanvas() || !jsPath || !jsPath->GetPath() ||
        !(ConvertFromJsValue(env, argv[ARGC_TWO], hOffset) && ConvertFromJsValue(env, argv[ARGC_THREE], vOffset))) {
        TEXT_LOGE("Failed to get paint parameter");
        return NapiGetUndefined(env);
    }
    paragraph_->Paint(jsCanvas->GetCanvas(), jsPath->GetPath(), hOffset, vOffset);

    return NapiGetUndefined(env);
}

napi_value JsParagraph::GetMaxWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetMaxWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetMaxWidth(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double maxWidth = paragraph_->GetMaxWidth();
    return CreateJsNumber(env, maxWidth);
}

napi_value JsParagraph::GetHeight(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetHeight(env, info) : nullptr;
}

napi_value JsParagraph::OnGetHeight(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double height = paragraph_->GetHeight();
    return CreateJsNumber(env, height);
}

napi_value JsParagraph::GetLongestLine(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLongestLine(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLongestLine(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double longestLine = paragraph_->GetActualWidth();
    return CreateJsNumber(env, longestLine);
}

napi_value JsParagraph::GetLongestLineWithIndent(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLongestLineWithIndent(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLongestLineWithIndent(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double longestLine = paragraph_->GetLongestLineWithIndent();
    return CreateJsNumber(env, longestLine);
}

napi_value JsParagraph::GetMinIntrinsicWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetMinIntrinsicWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetMinIntrinsicWidth(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double minIntrinsicWidth = paragraph_->GetMinIntrinsicWidth();
    return CreateJsNumber(env, minIntrinsicWidth);
}

napi_value JsParagraph::GetMaxIntrinsicWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetMaxIntrinsicWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetMaxIntrinsicWidth(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double maxIntrinsicWidth = paragraph_->GetMaxIntrinsicWidth();
    return CreateJsNumber(env, maxIntrinsicWidth);
}

napi_value JsParagraph::GetAlphabeticBaseline(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetAlphabeticBaseline(env, info) : nullptr;
}

napi_value JsParagraph::OnGetAlphabeticBaseline(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double alphabeticBaseline = paragraph_->GetAlphabeticBaseline();
    return CreateJsNumber(env, alphabeticBaseline);
}

napi_value JsParagraph::GetIdeographicBaseline(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetIdeographicBaseline(env, info) : nullptr;
}

napi_value JsParagraph::OnGetIdeographicBaseline(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double ideographicBaseline = paragraph_->GetIdeographicBaseline();
    return CreateJsNumber(env, ideographicBaseline);
}

napi_value JsParagraph::GetRectsForRange(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetRectsForRange(env, info) : nullptr;
}

napi_value JsParagraph::OnGetRectsForRange(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        TEXT_LOGE("Invalid argv[0]");
        return NapiGetUndefined(env);
    }
    napi_value tempValue = nullptr;
    size_t start = 0;
    size_t end = 0;
    TextRectWidthStyle wstyle;
    TextRectHeightStyle hstyle;
    napi_get_named_property(env, argv[0], "start", &tempValue);
    if (tempValue == nullptr) {
        TEXT_LOGE("Failed to get start property");
        return NapiGetUndefined(env);
    }
    bool isStartOk = ConvertFromJsValue(env, tempValue, start);
    tempValue = nullptr;
    napi_get_named_property(env, argv[0], "end", &tempValue);
    if (tempValue == nullptr) {
        TEXT_LOGE("Failed to get end property");
        return NapiGetUndefined(env);
    }
    bool isEndOk = ConvertFromJsValue(env, tempValue, end);
    if (!(isStartOk && isEndOk && argv[ARGC_ONE] != nullptr && ConvertFromJsValue(env, argv[ARGC_ONE], wstyle) &&
        argv[ARGC_TWO] != nullptr && ConvertFromJsValue(env, argv[ARGC_TWO], hstyle))) {
        TEXT_LOGE("Failed to convert, start ok:%{public}d, end ok:%{public}d", isStartOk, isEndOk);
        return NapiGetUndefined(env);
    }
    std::vector<TextRect> rectsForRange = paragraph_->GetTextRectsByBoundary(start, end, hstyle, wstyle);
    napi_value returnrectsForRange = nullptr;
    NAPI_CALL(env, napi_create_array(env, &returnrectsForRange));
    int num = static_cast<int>(rectsForRange.size());
    for (int index = 0; index < num; ++index) {
        napi_value tempValue2 = CreateTextRectJsValue(env, rectsForRange[index]);
        NAPI_CALL(env, napi_set_element(env, returnrectsForRange, index, tempValue2));
    }
    return returnrectsForRange;
}

napi_value JsParagraph::GetRectsForPlaceholders(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetRectsForPlaceholders(env, info) : nullptr;
}

napi_value JsParagraph::OnGetRectsForPlaceholders(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::vector<TextRect> rectsForPlaceholders = paragraph_->GetTextRectsOfPlaceholders();
    napi_value returnPlaceholders = nullptr;
    NAPI_CALL(env, napi_create_array(env, &returnPlaceholders));
    int num = static_cast<int>(rectsForPlaceholders.size());
    for (int index = 0; index < num; ++index) {
        napi_value tempValue = CreateTextRectJsValue(env, rectsForPlaceholders[index]);
        if (tempValue != nullptr) {
            NAPI_CALL(env, napi_set_element(env, returnPlaceholders, index, tempValue));
        }
    }
    return returnPlaceholders;
}

napi_value JsParagraph::GetGlyphPositionAtCoordinate(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetGlyphPositionAtCoordinate(env, info) : nullptr;
}

napi_value JsParagraph::OnGetGlyphPositionAtCoordinate(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        TEXT_LOGE("Failed to get parameter, arg %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double dx = 0.0;
    double dy = 0.0;
    if (!(argv[0] != nullptr && ConvertFromJsValue(env, argv[0], dx) && argv[1] != nullptr &&
        ConvertFromJsValue(env, argv[1], dy))) {
        TEXT_LOGE("Failed to convert");
        return NapiGetUndefined(env);
    }
    IndexAndAffinity positionWithAffinity = paragraph_->GetGlyphIndexByCoordinate(dx, dy);
    return GetPositionWithAffinityAndConvertToJsValue(env, &positionWithAffinity);
}

napi_value JsParagraph::GetWordBoundary(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetWordBoundary(env, info) : nullptr;
}

napi_value JsParagraph::OnGetWordBoundary(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t offset = 0;
    if (!(argv[0] != nullptr && ConvertFromJsValue(env, argv[0], offset))) {
        TEXT_LOGE("Failed to convert");
        return NapiGetUndefined(env);
    }
    Boundary range = paragraph_->GetWordBoundaryByIndex(offset);
    return GetRangeAndConvertToJsValue(env, &range);
}

napi_value JsParagraph::GetLineCount(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLineCount(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineCount(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t lineCount = static_cast<size_t>(paragraph_->GetLineCount());
    return CreateJsNumber(env, lineCount);
}

napi_value JsParagraph::GetLineHeight(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLineHeight(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineHeight(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    int lineNumber = 0;
    if (!(argv[0] != nullptr && ConvertFromJsValue(env, argv[0], lineNumber))) {
        TEXT_LOGE("Failed to convert");
        return NapiGetUndefined(env);
    }
    double lineHeight = paragraph_->GetLineHeight(lineNumber);
    return CreateJsNumber(env, lineHeight);
}

napi_value JsParagraph::GetLineWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLineWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineWidth(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    int lineNumber = 0;
    if (!(argv[0] != nullptr && ConvertFromJsValue(env, argv[0], lineNumber))) {
        TEXT_LOGE("Failed to convert line number");
        return NapiGetUndefined(env);
    }
    double lineWidth = paragraph_->GetLineWidth(lineNumber);
    return CreateJsNumber(env, lineWidth);
}

napi_value JsParagraph::DidExceedMaxLines(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnDidExceedMaxLines(env, info) : nullptr;
}

napi_value JsParagraph::OnDidExceedMaxLines(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    bool didExceedMaxLines = paragraph_->DidExceedMaxLines();
    return CreateJsValue(env, didExceedMaxLines);
}

napi_value JsParagraph::GetActualTextRange(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetActualTextRange(env, info) : nullptr;
}

napi_value JsParagraph::OnGetActualTextRange(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    int lineNumber = 0;
    if (!(ConvertFromJsValue(env, argv[0], lineNumber))) {
        TEXT_LOGE("Failed to convert line number");
        return NapiGetUndefined(env);
    }
    bool includeSpaces = false;
    if (!(ConvertFromJsValue(env, argv[1], includeSpaces))) {
        TEXT_LOGE("Failed to convert include spaces");
        return NapiGetUndefined(env);
    }
    OHOS::Rosen::Boundary range = paragraph_->GetActualTextRange(lineNumber, includeSpaces);
    return GetRangeAndConvertToJsValue(env, &range);
}

napi_value JsParagraph::GetLineMetrics(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    if (status == napi_ok && argc < ARGC_ONE) {
        return (me != nullptr) ? me->OnGetLineMetrics(env, info) : nullptr;
    }
    return (me != nullptr) ? me->OnGetLineMetricsAt(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineMetrics(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::vector<LineMetrics> vectorLineMetrics = paragraph_->GetLineMetrics();
    napi_value returnLineMetrics = nullptr;
    NAPI_CALL(env, napi_create_array(env, &returnLineMetrics));
    int num = static_cast<int>(vectorLineMetrics.size());
    for (int index = 0; index < num; ++index) {
        napi_value tempValue = CreateLineMetricsJsValue(env, vectorLineMetrics[index]);
        napi_set_element(env, returnLineMetrics, index, tempValue);
    }
    return returnLineMetrics;
}

napi_value JsParagraph::OnGetLineMetricsAt(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    int lineNumber = 0;
    if (!(ConvertFromJsValue(env, argv[0], lineNumber))) {
        TEXT_LOGE("Failed to convert line number");
        return NapiGetUndefined(env);
    }
    LineMetrics lineMetrics;
    if (!paragraph_->GetLineMetricsAt(lineNumber, &lineMetrics)) {
        TEXT_LOGE("Failed to get line metrics");
        return nullptr;
    }
    return CreateLineMetricsJsValue(env, lineMetrics);
}

napi_value JsParagraph::GetFontMetricsByTextStyle(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetFontMetricsByTextStyle(env, info) : nullptr;
}

napi_value JsParagraph::OnGetFontMetricsByTextStyle(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Invalid argc %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    TextStyle textStyle;
    if (!GetTextStyleFromJS(env, argv[0], textStyle)) {
        TEXT_LOGE("Failed to convert text style");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params");
    }

    OHOS::Rosen::Drawing::FontMetrics fontmetrics = paragraph_->GetFontMetrics(textStyle);
    return GetFontMetricsAndConvertToJsValue(env, &fontmetrics);
}

napi_value JsParagraph::GetLineFontMetrics(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLineFontMetrics(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineFontMetrics(napi_env env, napi_callback_info info)
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Invalid argc %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    int lineNumber = 0;
    if (!(ConvertFromJsValue(env, argv[0], lineNumber))) {
        TEXT_LOGE("Invalid argv");
        return NapiGetUndefined(env);
    }

    napi_value returnFontMetrics = nullptr;
    NAPI_CALL(env, napi_create_array(env, &returnFontMetrics));
    size_t fontMetricsSize = 0;
    std::vector<Drawing::FontMetrics> grabFontMetrics;
    if (!paragraph_->GetLineFontMetrics(lineNumber, fontMetricsSize, grabFontMetrics)) {
        TEXT_LOGE("Failed to get line font metrics");
        return returnFontMetrics;
    }

    int num = static_cast<int>(grabFontMetrics.size());
    for (int index = 0; index < num; ++index) {
        napi_value jsValue = GetFontMetricsAndConvertToJsValue(env, &grabFontMetrics[index]);
        napi_set_element(env, returnFontMetrics, index, jsValue);
    }
    return returnFontMetrics;
}

JsParagraph::JsParagraph(std::shared_ptr<Typography> typography)
    : paragraph_(typography)
{
}

JsParagraph::~JsParagraph()
{
}

std::shared_ptr<Typography> JsParagraph::GetParagraph()
{
    std::shared_ptr<Typography> typography = std::move(paragraph_);
    return typography;
}

napi_value JsParagraph::CreateJsTypography(napi_env env, std::unique_ptr<Typography> typography)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        napi_value argv;
        napi_create_external(
            env, typography.release(), [](napi_env env, void* finalizeData, void* finalizeHint) {}, nullptr, &argv);
        status = napi_new_instance(env, constructor, ARGC_ONE, &argv, &result);
        if (status == napi_ok) {
            return result;
        } else {
            TEXT_LOGE("Failed to new instance");
        }
    }
    return result;
}

napi_value JsParagraph::GetTextLines(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetTextLines(env, info) : nullptr;
}

napi_value JsParagraph::OnGetTextLines(napi_env env, napi_callback_info info)
{
    if (!paragraph_) {
        TEXT_LOGE("Null paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::shared_ptr<Typography> paragraphCopy = paragraph_->CloneSelf();
    if (!paragraphCopy) {
        TEXT_LOGE("Failed to clone paragraph");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::vector<std::unique_ptr<TextLineBase>> textlineArr = paragraphCopy->GetTextLines();
    napi_value array = nullptr;
    NAPI_CALL(env, napi_create_array(env, &array));
    uint32_t index = 0;
    for (std::unique_ptr<TextLineBase>& item : textlineArr) {
        napi_value itemObject = JsTextLine::CreateTextLine(env, info);
        if (!itemObject) {
            TEXT_LOGE("Failed to create text line");
            continue;
        }
        JsTextLine* jsTextLine = nullptr;
        napi_unwrap(env, itemObject, reinterpret_cast<void**>(&jsTextLine));
        if (!jsTextLine) {
            TEXT_LOGE("Failed to unwrap text line");
            continue;
        }
        jsTextLine->SetTextLine(std::move(item));
        jsTextLine->SetParagraph(paragraphCopy);

        napi_set_element(env, array, index++, itemObject);
    }
    return array;
}

napi_value JsParagraph::LayoutAsync(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnLayoutAsync(env, info) : nullptr;
}

napi_value JsParagraph::OnLayoutAsync(napi_env env, napi_callback_info info)
{
    NAPI_CHECK_AND_THROW_ERROR(paragraph_ != nullptr, TextErrorCode::ERROR_INVALID_PARAM, "Paragraph is null");

    struct ConcreteContext : public ContextBase {
        double width = 0.0;
    };
    sptr<ConcreteContext> context = sptr<ConcreteContext>::MakeSptr();
    NAPI_CHECK_AND_THROW_ERROR(context != nullptr, TextErrorCode::ERROR_NO_MEMORY, "Failed to make context");
    auto inputParser = [env, context](size_t argc, napi_value* argv) {
        TEXT_ERROR_CHECK(argv, return, "Argv is null");
        NAPI_CHECK_ARGS(context, context->status == napi_ok, napi_invalid_arg,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Status error, status=%d", static_cast<int>(context->status));
        NAPI_CHECK_ARGS(context, argc >= ARGC_ONE, napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM,
            return, "Argc is invalid %zu", argc);
        NAPI_CHECK_ARGS(context, NapiValueTypeIsValid(env, argv[0]) &&
            ConvertFromJsValue(env, argv[0], context->width) && (!std::signbit(context->width)),
            napi_invalid_arg, TextErrorCode::ERROR_INVALID_PARAM, return, "Argv is invalid %f", context->width);
    };

    context->GetCbInfo(env, info, inputParser);

    auto executor = [context]() {
        TEXT_ERROR_CHECK(context != nullptr, return, "Context is null");

        auto* jsParagraph = reinterpret_cast<JsParagraph*>(context->native);
        NAPI_CHECK_ARGS(context, jsParagraph != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "JsParagraph is nullptr");
        NAPI_CHECK_ARGS(context, jsParagraph->paragraph_ != nullptr, napi_generic_failure,
            TextErrorCode::ERROR_INVALID_PARAM, return, "Inner paragraph is null");
        jsParagraph->paragraph_->Layout(context->width);
    };

    auto complete = [env](napi_value& output) {
        output = NapiGetUndefined(env);
    };
    return NapiAsyncWork::Enqueue(env, context, "onLayoutAsync", executor, complete);
}

napi_value JsParagraph::UpdateColor(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnUpdateColor(env, info) : nullptr;
}

napi_value JsParagraph::OnUpdateColor(napi_env env, napi_callback_info info)
{
    NAPI_CHECK_AND_THROW_ERROR(paragraph_ != nullptr, TextErrorCode::ERROR_INVALID_PARAM, "Paragraph is null");

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    TextStyle textStyleTemplate;
    if (!SetColorFromJS(env, argv[0], textStyleTemplate.color)) {
        TEXT_LOGE("Invalid argv[0]");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params");
    }
    textStyleTemplate.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_COLOR));
    paragraph_->UpdateAllTextStyles(textStyleTemplate);
    return NapiGetUndefined(env);
}

napi_value JsParagraph::UpdateDecoration(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnUpdateDecoration(env, info) : nullptr;
}

napi_value JsParagraph::OnUpdateDecoration(napi_env env, napi_callback_info info)
{
    NAPI_CHECK_AND_THROW_ERROR(paragraph_ != nullptr, TextErrorCode::ERROR_INVALID_PARAM, "Paragraph is null");

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        TEXT_LOGE("Failed to get parameter, argc %{public}zu, ret %{public}d", argc, status);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    TextStyle textStyleTemplate;
    GetDecorationFromJSForUpdate(env, argv[0], textStyleTemplate);
    paragraph_->UpdateAllTextStyles(textStyleTemplate);
    return NapiGetUndefined(env);
}

napi_value JsParagraph::IsStrutStyleEqual(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        TEXT_LOGE("Invalid argc %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params");
    }

    TypographyStyle styleFrom;
    if (!SetStrutStyleFromJS(env, argv[0], styleFrom)) {
        TEXT_LOGE("Invalid argv[0]");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params");
    }

    TypographyStyle styleTo;
    if (!SetStrutStyleFromJS(env, argv[1], styleTo)) {
        TEXT_LOGE("Invalid argv[1]");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params");
    }

    bool equal = (styleFrom == styleTo);
    return CreateJsValue(env, equal);
}
} // namespace OHOS::Rosen
