/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ani_paragraph.h"

#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "ani_common.h"
#include "ani_index_and_affinity_converter.h"
#include "ani_line_metrics_converter.h"
#include "ani_text_line.h"
#include "ani_text_rect_converter.h"
#include "ani_text_style_converter.h"
#include "ani_text_layout_result_converter.h"
#include "ani_text_utils.h"
#include "ani_transfer_util.h"
#include "canvas_ani/ani_canvas.h"
#include "font_collection.h"
#include "paragraph_napi/js_paragraph.h"
#include "path_ani/ani_path.h"
#include "text/font_metrics.h"
#include "typography.h"
#include "typography_create.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
const std::string PAINT_SIGN = "C{" + std::string(ANI_CLASS_CANVAS) + "}dd:";
const std::string PAINT_ON_PATH_SIGN =
    "C{" + std::string(ANI_CLASS_CANVAS) + "}C{" + std::string(ANI_CLASS_PATH) + "}dd:";
const std::string GET_RECTS_SIGN = "C{" + std::string(ANI_INTERFACE_RANGE) + "}E{"
    + std::string(ANI_ENUM_RECT_WIDTH_STYLE) + "}E{" + std::string(ANI_ENUM_RECT_HEIGHT_STYLE)
    + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_GLYPH_POSITION_AT_COORDINATE_SIGN =
    "dd:C{" + std::string(ANI_INTERFACE_POSITION_WITH_AFFINITY) + "}";
const std::string GET_WORD_BOUNDARY_SIGN = "i:C{" + std::string(ANI_INTERFACE_RANGE) + "}";
const std::string GET_TEXT_LINES_SIGN = ":C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_ACTUAL_TEXT_RANGE_SIGN = "iz:C{" + std::string(ANI_INTERFACE_RANGE) + "}";
const std::string UPDATE_COLOR_SIGN = "C{" + std::string(ANI_INTERFACE_COLOR) + "}:";
const std::string UPDATE_DECORATION_SIGN = "C{" + std::string(ANI_INTERFACE_DECORATION) + "}:";
const std::string GET_CHARACTER_RANGE_FOR_GLYPH_RANGE_SIGN = "C{" + std::string(ANI_INTERFACE_RANGE) + "}E{"
    + std::string(ANI_ENUM_TEXT_ENCODING) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_GLYPH_RANGE_FOR_CHARACTER_RANGE_SIGN = "C{" + std::string(ANI_INTERFACE_RANGE) + "}E{"
    + std::string(ANI_ENUM_TEXT_ENCODING) + "}:C{" + std::string(ANI_ARRAY) + "}";
const std::string GET_CHARACTER_POSITION_AT_COORDINATE_SIGN = "ddE{" + std::string(ANI_ENUM_TEXT_ENCODING) + "}:C{"
    + std::string(ANI_INTERFACE_POSITION_WITH_AFFINITY) + "}";
} // namespace

ani_object ThrowErrorAndReturnUndefined(ani_env* env)
{
    AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    return AniTextUtils::CreateAniUndefined(env);
}

// Helper function to parse and validate text encoding
static bool ParseTextEncoding(ani_env* env, ani_object encoding, Drawing::TextEncoding& outEncodeType)
{
    ani_int encodingValue = 0;
    ani_status ret = env->EnumItem_GetValue_Int(reinterpret_cast<ani_enum_item>(encoding), &encodingValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to get encoding value, ret: %{public}d", ret);
        return false;
    }
    // Validate encoding value - must match Drawing::TextEncoding enum values
    if (encodingValue != static_cast<int>(Drawing::TextEncoding::UTF8) &&
        encodingValue != static_cast<int>(Drawing::TextEncoding::UTF16)) {
        TEXT_LOGE("Invalid encoding value: %{public}d", encodingValue);
        return false;
    }
    outEncodeType = static_cast<Drawing::TextEncoding>(encodingValue);
    return true;
}

// Helper function to set boundary element in array
ani_status AniParagraph::SetArrayBoundaryElement(ani_env* env, ani_object array, int index,
    const Boundary& range, const char* errorMsg)
{
    ani_object rangeObj = nullptr;
    ani_status status = AniTextRectConverter::ParseBoundaryToAni(env, range, rangeObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to parse %{public}s, status %{public}d", errorMsg, status);
        return status;
    }
    status = env->Object_CallMethod_Void(array, AniGlobalMethod::GetInstance().arraySet, index, rangeObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to set %{public}s, status %{public}d", errorMsg, status);
        return status;
    }
    return ANI_OK;
}

ani_object AniParagraph::SetTypography(ani_env* env, OHOS::Rosen::Typography* typography)
{
    if (typography == nullptr) {
        TEXT_LOGE("Failed to set paragraph, emtpy ptr");
        return AniTextUtils::CreateAniUndefined(env);
    }
    AniParagraph* aniParagraph = new AniParagraph();
    ani_object paragraphObj = AniTextUtils::CreateAniObject(
        env, AniGlobalClass::GetInstance().paragraph, AniGlobalMethod::GetInstance().paragraph);
    aniParagraph->typography_ = std::shared_ptr<OHOS::Rosen::Typography>(typography);
    ani_status ret = env->Object_CallMethodByName_Void(
        paragraphObj, TEXT_BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniParagraph));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani Paragraph obj");
        delete aniParagraph;
        aniParagraph = nullptr;
        return AniTextUtils::CreateAniUndefined(env);
    }
    return paragraphObj;
}

std::vector<ani_native_function> AniParagraph::InitMethods(ani_env* env)
{
    std::vector<ani_native_function> methods = {
        ani_native_function{"layoutSync", "d:", reinterpret_cast<void*>(LayoutSync)},
        ani_native_function{
            "layoutWithConstraints",
            "C{" ANI_INTERFACE_TEXT_RECT_SIZE "}:C{" ANI_INTERFACE_TEXT_LAYOUT_RESULT "}",
            reinterpret_cast<void*>(LayoutWithConstraints)
        },
        ani_native_function{"paint", PAINT_SIGN.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{"paintOnPath", PAINT_ON_PATH_SIGN.c_str(), reinterpret_cast<void*>(PaintOnPath)},
        ani_native_function{"getMaxWidth", ":d", reinterpret_cast<void*>(GetMaxWidth)},
        ani_native_function{"getHeight", ":d", reinterpret_cast<void*>(GetHeight)},
        ani_native_function{"getLongestLine", ":d", reinterpret_cast<void*>(GetLongestLine)},
        ani_native_function{"getLongestLineWithIndent", ":d", reinterpret_cast<void*>(GetLongestLineWithIndent)},
        ani_native_function{"getMinIntrinsicWidth", ":d", reinterpret_cast<void*>(GetMinIntrinsicWidth)},
        ani_native_function{"getMaxIntrinsicWidth", ":d", reinterpret_cast<void*>(GetMaxIntrinsicWidth)},
        ani_native_function{"getAlphabeticBaseline", ":d", reinterpret_cast<void*>(GetAlphabeticBaseline)},
        ani_native_function{"getIdeographicBaseline", ":d", reinterpret_cast<void*>(GetIdeographicBaseline)},
        ani_native_function{"getRectsForRange", GET_RECTS_SIGN.c_str(), reinterpret_cast<void*>(GetRectsForRange)},
        ani_native_function{
            "getRectsForPlaceholders", ":C{std.core.Array}", reinterpret_cast<void*>(GetRectsForPlaceholders)},
        ani_native_function{"getGlyphPositionAtCoordinate", GET_GLYPH_POSITION_AT_COORDINATE_SIGN.c_str(),
            reinterpret_cast<void*>(GetGlyphPositionAtCoordinate)},
        ani_native_function{
            "getWordBoundary", GET_WORD_BOUNDARY_SIGN.c_str(), reinterpret_cast<void*>(GetWordBoundary)},
        ani_native_function{"getLineCount", ":i", reinterpret_cast<void*>(GetLineCount)},
        ani_native_function{"getLineHeight", "i:d", reinterpret_cast<void*>(GetLineHeight)},
        ani_native_function{"getLineWidth", "i:d", reinterpret_cast<void*>(GetLineWidth)},
        ani_native_function{"didExceedMaxLines", ":z", reinterpret_cast<void*>(DidExceedMaxLines)},
        ani_native_function{
            "getActualTextRange", GET_ACTUAL_TEXT_RANGE_SIGN.c_str(), reinterpret_cast<void*>(GetActualTextRange)},
        ani_native_function{"getTextLines", GET_TEXT_LINES_SIGN.c_str(), reinterpret_cast<void*>(GetTextLines)},
        ani_native_function{"getLineMetrics", ":C{std.core.Array}", reinterpret_cast<void*>(GetLineMetrics)},
        ani_native_function{"nativeGetLineMetricsAt", "i:C{@ohos.graphics.text.text.LineMetrics}",
            reinterpret_cast<void*>(GetLineMetricsAt)},
        ani_native_function{"updateColor", UPDATE_COLOR_SIGN.c_str(), reinterpret_cast<void*>(UpdateColor)},
        ani_native_function{"updateDecoration",
            UPDATE_DECORATION_SIGN.c_str(), reinterpret_cast<void*>(UpdateDecoration)},
        ani_native_function{"getCharacterRangeForGlyphRange", GET_CHARACTER_RANGE_FOR_GLYPH_RANGE_SIGN.c_str(),
            reinterpret_cast<void*>(GetCharacterRangeForGlyphRange)},
        ani_native_function{"getGlyphRangeForCharacterRange", GET_GLYPH_RANGE_FOR_CHARACTER_RANGE_SIGN.c_str(),
            reinterpret_cast<void*>(GetGlyphRangeForCharacterRange)},
        ani_native_function{"getCharacterPositionAtCoordinate", GET_CHARACTER_POSITION_AT_COORDINATE_SIGN.c_str(),
            reinterpret_cast<void*>(GetCharacterPositionAtCoordinate)},
    };
    return methods;
}

ani_status AniParagraph::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ret;
    }

    std::vector<ani_native_function> methods = InitMethods(env);
    ret = env->Class_BindNativeMethods(AniGlobalClass::GetInstance().paragraph, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for Paragraph, ret %{public}d", ret);
        return ret;
    }

    std::array staticMethods = {
        ani_native_function{"nativeTransferStatic", "C{std.interop.ESValue}:C{std.core.Object}",
            reinterpret_cast<void*>(NativeTransferStatic)},
        ani_native_function{
            "nativeTransferDynamic", "l:C{std.interop.ESValue}", reinterpret_cast<void*>(NativeTransferDynamic)},
    };
    ret = env->Class_BindStaticNativeMethods(
        AniGlobalClass::GetInstance().paragraph, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind static methods: %{public}s, ret %{public}d", ANI_CLASS_PARAGRAPH, ret);
        return ret;
    }
    return ANI_OK;
}

void AniParagraph::LayoutSync(ani_env* env, ani_object object, ani_double width)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniParagraph->typography_->Layout(width);
}

ani_object AniParagraph::LayoutWithConstraints(
    ani_env* env, ani_object object, ani_object constraint)
{
    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(
        env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    OHOS::Rosen::TextRectSize textRect;
    ani_status ret = AniTextLayoutResultConverter::ParseTextRectSizeToNative(env, constraint, textRect);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to parse constraint, ret %{public}d", ret);
        return AniTextUtils::CreateAniUndefined(env);
    }

    OHOS::Rosen::TextLayoutResult layoutResult = aniParagraph->typography_->LayoutWithConstraints(textRect);

    ani_object resultObj = nullptr;
    ret = AniTextLayoutResultConverter::ParseTextLayoutResultToAni(env, layoutResult, resultObj);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to convert layout result, ret %{public}d", ret);
        return AniTextUtils::CreateAniUndefined(env);
    }

    return resultObj;
}

void AniParagraph::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =
        AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas, AniGlobalMethod::GetInstance().canvasGetNative);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "canvas unavailable.");
        return;
    }
    aniParagraph->typography_->Paint(aniCanvas->GetCanvas(), x, y);
}

void AniParagraph::PaintOnPath(
    ani_env* env, ani_object object, ani_object canvas, ani_object path, ani_double hOffset, ani_double vOffset)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =
        AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas, AniGlobalMethod::GetInstance().canvasGetNative);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Canvas unavailable.");
        return;
    }
    Drawing::AniPath* aniPath =
        AniTextUtils::GetNativeFromObj<Drawing::AniPath>(env, path, AniGlobalMethod::GetInstance().pathGetNative);
    if (aniPath == nullptr) {
        TEXT_LOGE("Path is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Path unavailable.");
        return;
    }
    aniParagraph->typography_->Paint(aniCanvas->GetCanvas(), aniPath->GetPath().get(), hOffset, vOffset);
}

ani_double AniParagraph::GetMaxWidth(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetMaxWidth();
}

ani_double AniParagraph::GetHeight(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetHeight();
}

ani_double AniParagraph::GetLongestLine(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetActualWidth();
}

ani_double AniParagraph::GetLongestLineWithIndent(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetLongestLineWithIndent();
}

ani_double AniParagraph::GetMinIntrinsicWidth(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetMinIntrinsicWidth();
}

ani_double AniParagraph::GetMaxIntrinsicWidth(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetMaxIntrinsicWidth();
}

ani_double AniParagraph::GetAlphabeticBaseline(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetAlphabeticBaseline();
}

ani_double AniParagraph::GetIdeographicBaseline(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetIdeographicBaseline();
}

ani_object AniParagraph::GetRectsForRange(
    ani_env* env, ani_object object, ani_object range, ani_object widthStyle, ani_object heightStyle)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    OHOS::Text::ANI::RectRange rectRange;
    TextRectWidthStyle widthStyleInner;
    TextRectHeightStyle heightStyleInner;
    if (AniTextRectConverter::ParseRangeToNative(env, range, rectRange) != ANI_OK) {
        TEXT_LOGE("Failed to parse range");
        return ThrowErrorAndReturnUndefined(env);
    }
    if (AniTextRectConverter::ParseWidthStyleToNative(env, widthStyle, widthStyleInner) != ANI_OK) {
        TEXT_LOGE("Failed to parse width style");
        return ThrowErrorAndReturnUndefined(env);
    }
    if (AniTextRectConverter::ParseHeightStyleToNative(env, heightStyle, heightStyleInner) != ANI_OK) {
        TEXT_LOGE("Failed to parse height style");
        return ThrowErrorAndReturnUndefined(env);
    }
    std::vector<TextRect> rectsForRange = aniParagraph->typography_->GetTextRectsByBoundary(
        rectRange.start, rectRange.end, heightStyleInner, widthStyleInner);
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, rectsForRange.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& textBox : rectsForRange) {
        ani_object aniObj = nullptr;
        ani_status status = AniTextRectConverter::ParseTextBoxToAni(env, textBox, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to parse text box,index %{public}zu, status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set textBox item,index %{public}zu, status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetRectsForPlaceholders(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return ThrowErrorAndReturnUndefined(env);
    }

    std::vector<TextRect> rectsForRange = aniParagraph->typography_->GetTextRectsOfPlaceholders();

    ani_object arrayObj = AniTextUtils::CreateAniArray(env, rectsForRange.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& textBox : rectsForRange) {
        ani_object aniObj = nullptr;
        ani_status status = AniTextRectConverter::ParseTextBoxToAni(env, textBox, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to parse text box,index %{public}zu, status %{public}d", index, status);
            continue;
        }
        status = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniObj);
        if (status != ANI_OK) {
            TEXT_LOGE("Failed to set textBox item,index %{public}zu, status %{public}d", index, status);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetGlyphPositionAtCoordinate(ani_env* env, ani_object object, ani_double x, ani_double y)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return ThrowErrorAndReturnUndefined(env);
    }

    IndexAndAffinity indexAndAffinity = aniParagraph->typography_->GetGlyphIndexByCoordinate(x, y);
    ani_object indexAndAffinityObj = nullptr;
    ani_status ret =
        AniIndexAndAffinityConverter::ParseIndexAndAffinityToAni(env, indexAndAffinity, indexAndAffinityObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return indexAndAffinityObj;
}

ani_object AniParagraph::GetWordBoundary(ani_env* env, ani_object object, ani_int offset)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return ThrowErrorAndReturnUndefined(env);
    }

    Boundary boundary = aniParagraph->typography_->GetWordBoundaryByIndex(static_cast<size_t>(offset));
    ani_object boundaryObj = nullptr;
    ani_status ret = AniTextRectConverter::ParseBoundaryToAni(env, boundary, boundaryObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return boundaryObj;
}

ani_int AniParagraph::GetLineCount(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetLineCount();
}

ani_double AniParagraph::GetLineHeight(ani_env* env, ani_object object, ani_int line)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return aniParagraph->typography_->GetLineHeight(static_cast<int>(line));
}

ani_double AniParagraph::GetLineWidth(ani_env* env, ani_object object, ani_int line)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return aniParagraph->typography_->GetLineWidth(static_cast<int>(line));
}

ani_boolean AniParagraph::DidExceedMaxLines(ani_env* env, ani_object object)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }

    return aniParagraph->typography_->DidExceedMaxLines();
}

ani_object AniParagraph::GetActualTextRange(
    ani_env* env, ani_object object, ani_int lineNumber, ani_boolean includeSpaces)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return ThrowErrorAndReturnUndefined(env);
    }
    Boundary Boundary =
        aniParagraph->typography_->GetActualTextRange(static_cast<int>(lineNumber), static_cast<bool>(includeSpaces));
    ani_object boundaryObj = nullptr;
    ani_status ret = AniTextRectConverter::ParseBoundaryToAni(env, Boundary, boundaryObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return boundaryObj;
}

ani_ref AniParagraph::GetTextLines(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }
    std::vector<std::unique_ptr<TextLineBase>> textlines = aniParagraph->typography_->GetTextLines();
    arrayObj = AniTextUtils::CreateAniArray(env, textlines.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return arrayObj;
    }
    ani_size index = 0;
    for (auto& textline : textlines) {
        if (textline == nullptr) {
            continue;
        }
        TextLineBase* textLineBasePtr = textline.release();
        ani_object aniObj = AniTextLine::CreateTextLine(env, textLineBasePtr);
        if (AniTextUtils::IsUndefined(env, aniObj)) {
            TEXT_LOGE("Failed to create text line");
            delete textLineBasePtr;
            textLineBasePtr = nullptr;
            continue;
        }
        ani_status ret = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set textline item %{public}zu", index);
            delete textLineBasePtr;
            textLineBasePtr = nullptr;
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_ref AniParagraph::GetLineMetrics(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }
    std::vector<LineMetrics> vectorLineMetrics = aniParagraph->typography_->GetLineMetrics();
    arrayObj = AniTextUtils::CreateAniArray(env, vectorLineMetrics.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return arrayObj;
    }
    ani_size index = 0;
    for (const auto& lineMetrics : vectorLineMetrics) {
        ani_object aniObj = AniLineMetricsConverter::ParseLineMetricsToAni(env, lineMetrics);
        ani_status ret = env->Object_CallMethod_Void(arrayObj, AniGlobalMethod::GetInstance().arraySet, index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set lineMetrics item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetLineMetricsAt(ani_env* env, ani_object object, ani_int lineNumber)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return ThrowErrorAndReturnUndefined(env);
    }
    LineMetrics lineMetrics;
    if (!aniParagraph->typography_->GetLineMetricsAt(lineNumber, &lineMetrics)) {
        TEXT_LOGE("Failed to get line metrics");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return AniLineMetricsConverter::ParseLineMetricsToAni(env, lineMetrics);
}

void AniParagraph::UpdateColor(ani_env* env, ani_object object, ani_object color)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return;
    }

    TextStyle textStyleTemplate;
    Drawing::ColorQuad colorQuad;
    if (OHOS::Rosen::Drawing::GetColorQuadFromColorObj(env, color, colorQuad)) {
        textStyleTemplate.color = Drawing::Color(colorQuad);
    } else {
        TEXT_LOGE("Failed to parse color");
        ThrowErrorAndReturnUndefined(env);
        return;
    }
    textStyleTemplate.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_COLOR));
    aniParagraph->typography_->UpdateAllTextStyles(textStyleTemplate);
}

ani_object AniParagraph::GetCharacterRangeForGlyphRange(ani_env* env, ani_object object, ani_object glyphRange,
    ani_object encoding)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    RectRange glyphRangeNative;
    if (AniTextRectConverter::ParseRangeToNative(env, glyphRange, glyphRangeNative) != ANI_OK) {
        TEXT_LOGE("Failed to parse glyph range");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    // Validate range: start should not be greater than or equal to end
    if (glyphRangeNative.start >= glyphRangeNative.end) {
        TEXT_LOGE("Invalid glyph range: start %{public}" PRId64 ", end %{public}" PRId64,
            glyphRangeNative.start, glyphRangeNative.end);
        return AniTextUtils::CreateAniArray(env, 0);
    }
    Drawing::TextEncoding encodeType;
    if (!ParseTextEncoding(env, encoding, encodeType)) {
        return AniTextUtils::CreateAniArray(env, 0);
    }
    Boundary actualGlyphRange = {0, 0};
    Boundary charRange = aniParagraph->typography_->GetCharacterRangeForGlyphRange(
        glyphRangeNative.start, glyphRangeNative.end, &actualGlyphRange,
        static_cast<Rosen::TextEncoding>(encodeType));
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, 2);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    if (SetArrayBoundaryElement(env, arrayObj, 0, charRange, "char range") != ANI_OK) {
        return AniTextUtils::CreateAniArray(env, 0);
    }
    if (SetArrayBoundaryElement(env, arrayObj, 1, actualGlyphRange, "actual glyph range") != ANI_OK) {
        return AniTextUtils::CreateAniArray(env, 0);
    }
    return arrayObj;
}

ani_object AniParagraph::GetGlyphRangeForCharacterRange(ani_env* env, ani_object object, ani_object characterRange,
    ani_object encoding)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    RectRange charRangeNative;
    if (AniTextRectConverter::ParseRangeToNative(env, characterRange, charRangeNative) != ANI_OK) {
        TEXT_LOGE("Failed to parse character range");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    // Validate range: start should not be greater than or equal to end
    if (charRangeNative.start >= charRangeNative.end) {
        TEXT_LOGE("Invalid character range: start %{public}" PRId64 ", end %{public}" PRId64,
            charRangeNative.start, charRangeNative.end);
        return AniTextUtils::CreateAniArray(env, 0);
    }
    Drawing::TextEncoding encodeType;
    if (!ParseTextEncoding(env, encoding, encodeType)) {
        return AniTextUtils::CreateAniArray(env, 0);
    }
    Boundary actualCharRange = {0, 0};
    Boundary glyphRange = aniParagraph->typography_->GetGlyphRangeForCharacterRange(
        charRangeNative.start, charRangeNative.end, &actualCharRange,
        static_cast<Rosen::TextEncoding>(encodeType));
    ani_object arrayObj = AniTextUtils::CreateAniArray(env, 2);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniArray(env, 0);
    }
    if (SetArrayBoundaryElement(env, arrayObj, 0, glyphRange, "glyph range") != ANI_OK) {
        return AniTextUtils::CreateAniArray(env, 0);
    }
    if (SetArrayBoundaryElement(env, arrayObj, 1, actualCharRange, "actual char range") != ANI_OK) {
        return AniTextUtils::CreateAniArray(env, 0);
    }
    return arrayObj;
}

ani_object AniParagraph::GetCharacterPositionAtCoordinate(ani_env* env, ani_object object, ani_double x, ani_double y,
    ani_object encoding)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return AniTextUtils::CreateAniUndefined(env);
    }
    Drawing::TextEncoding encodeType;
    if (!ParseTextEncoding(env, encoding, encodeType)) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    IndexAndAffinity indexAndAffinity = aniParagraph->typography_->GetCharacterIndexByCoordinate(
        x, y, static_cast<Rosen::TextEncoding>(encodeType));
    ani_object indexAndAffinityObj = nullptr;
    ani_status ret = AniIndexAndAffinityConverter::ParseIndexAndAffinityToAni(
        env, indexAndAffinity, indexAndAffinityObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return indexAndAffinityObj;
}

void AniParagraph::UpdateDecoration(ani_env* env, ani_object object, ani_object decoration)
{
    AniParagraph* aniParagraph =
        AniTextUtils::GetNativeFromObj<AniParagraph>(env, object, AniGlobalMethod::GetInstance().paragraphGetNative);
    if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
        TEXT_LOGE("Paragraph is null");
        return;
    }
    TextStyle textStyleTemplate;
    AniTextStyleConverter::ParseDecorationToNative(env, decoration, true, textStyleTemplate);
    aniParagraph->typography_->UpdateAllTextStyles(textStyleTemplate);
}

ani_object AniParagraph::NativeTransferStatic(ani_env* env, ani_class cls, ani_object input)
{
    return AniTransferUtils::TransferStatic(env, input, [](ani_env* env, void* unwrapResult) {
        JsParagraph* jsParagraph = reinterpret_cast<JsParagraph*>(unwrapResult);
        if (jsParagraph == nullptr) {
            TEXT_LOGE("Null jsParagraph");
            return AniTextUtils::CreateAniUndefined(env);
        }
        ani_object staticObj = AniTextUtils::CreateAniObject(
            env, AniGlobalClass::GetInstance().paragraph, AniGlobalMethod::GetInstance().paragraph);
        std::shared_ptr<Typography> typographyPtr = jsParagraph->GetParagraph();
        if (typographyPtr == nullptr) {
            TEXT_LOGE("Failed to get typography");
            return AniTextUtils::CreateAniUndefined(env);
        }
        AniParagraph* aniParagraph = new AniParagraph();
        aniParagraph->typography_ = typographyPtr;
        ani_status ret = env->Object_CallMethodByName_Void(
            staticObj, TEXT_BIND_NATIVE, "l:", reinterpret_cast<ani_long>(aniParagraph));
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to create ani typography obj, ret %{public}d", ret);
            delete aniParagraph;
            aniParagraph = nullptr;
            return AniTextUtils::CreateAniUndefined(env);
        }
        return staticObj;
    });
}

ani_object AniParagraph::NativeTransferDynamic(ani_env* aniEnv, ani_class cls, ani_long nativeObj)
{
    return AniTransferUtils::TransferDynamic(aniEnv, nativeObj,
        [](napi_env napiEnv, ani_long nativeObj, napi_value objValue) -> napi_value {
            napi_value dynamicObj = nullptr;
            AniParagraph* aniParagraph = reinterpret_cast<AniParagraph*>(nativeObj);
            if (aniParagraph == nullptr || aniParagraph->typography_ == nullptr) {
                TEXT_LOGE("Null aniParagraph");
                return dynamicObj;
            }
            return JsParagraph::CreateJsTypography(napiEnv, aniParagraph->typography_.get());
        });
}
} // namespace OHOS::Text::ANI