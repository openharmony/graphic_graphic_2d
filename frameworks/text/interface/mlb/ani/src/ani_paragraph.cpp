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

#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "ani_common.h"
#include "ani_index_and_affinity_converter.h"
#include "ani_line_metrics_converter.h"
#include "ani_paragraph.h"
#include "ani_text_rect_converter.h"
#include "ani_text_utils.h"
#include "canvas_ani/ani_canvas.h"
#include "font_collection.h"
#include "text/font_metrics.h"
#include "typography.h"
#include "typography_create.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
ani_object AniParagraph::SetTypography(ani_env* env, std::unique_ptr<OHOS::Rosen::Typography>& typography)
{
    ani_object pargraphObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_PARAGRAPH, ":V");
    Typography* typographyPtr = typography.release();
    ani_status ret =
        env->Object_SetFieldByName_Long(pargraphObj, NATIVE_OBJ, reinterpret_cast<ani_long>(typographyPtr));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani Paragraph obj");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return pargraphObj;
}

ani_status AniParagraph::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK) {
        TEXT_LOGE("null env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::string paintSignature = std::string(ANI_CLASS_CANVAS) + "DD:V";
    std::string paintOnPathSignature = std::string(ANI_CLASS_CANVAS) + std::string(ANI_CLASS_PATH) + "DD:V";
    std::string getRectsForRangeSignature = std::string(ANI_INTERFACE_RANGE) + std::string(ANI_ENUM_RECT_WIDTH_STYLE)
                                            + std::string(ANI_ENUM_RECT_HEIGHT_STYLE) + ":" + std::string(ANI_ARRAY);
    std::string getGlyphPositionAtCoordinateSignature = "DD:" + std::string(ANI_INTERFACE_POSITION_WITH_AFFINITY);
    std::string getWordBoundarySignature = "D:" + std::string(ANI_INTERFACE_RANGE);
    std::string getTextLinesSignature = ":" + std::string(ANI_ARRAY);
    std::string getActualTextRangeSignature = "DZ:" + std::string(ANI_INTERFACE_RANGE);
    std::array methods = {
        ani_native_function{"layoutSync", "D:V", reinterpret_cast<void*>(LayoutSync)},
        ani_native_function{"paint", paintSignature.c_str(), reinterpret_cast<void*>(Paint)},
        ani_native_function{"paintOnPath", paintOnPathSignature.c_str(), reinterpret_cast<void*>(PaintOnPath)},
        ani_native_function{"getMaxWidth", ":D", reinterpret_cast<void*>(GetMaxWidth)},
        ani_native_function{"getHeight", ":D", reinterpret_cast<void*>(GetHeight)},
        ani_native_function{"getLongestLine", ":D", reinterpret_cast<void*>(GetLongestLine)},
        ani_native_function{"getLongestLineWithIndent", ":D", reinterpret_cast<void*>(GetLongestLineWithIndent)},
        ani_native_function{"getMinIntrinsicWidth", ":D", reinterpret_cast<void*>(GetMinIntrinsicWidth)},
        ani_native_function{"getMaxIntrinsicWidth", ":D", reinterpret_cast<void*>(GetMaxIntrinsicWidth)},
        ani_native_function{"getAlphabeticBaseline", ":D", reinterpret_cast<void*>(GetAlphabeticBaseline)},
        ani_native_function{"getIdeographicBaseline", ":D", reinterpret_cast<void*>(GetIdeographicBaseline)},
        ani_native_function{
            "getRectsForRange", getRectsForRangeSignature.c_str(), reinterpret_cast<void*>(GetRectsForRange)},
        ani_native_function{
            "getRectsForPlaceholders", ":Lescompat/Array;", reinterpret_cast<void*>(GetRectsForPlaceholders)},
        ani_native_function{"getGlyphPositionAtCoordinate", getGlyphPositionAtCoordinateSignature.c_str(),
            reinterpret_cast<void*>(GetGlyphPositionAtCoordinate)},
        ani_native_function{
            "getWordBoundary", getWordBoundarySignature.c_str(), reinterpret_cast<void*>(GetWordBoundary)},
        ani_native_function{"getLineCount", ":D", reinterpret_cast<void*>(GetLineCount)},
        ani_native_function{"getLineHeight", "D:D", reinterpret_cast<void*>(GetLineHeight)},
        ani_native_function{"getLineWidth", "D:D", reinterpret_cast<void*>(GetLineWidth)},
        ani_native_function{"didExceedMaxLines", ":Z", reinterpret_cast<void*>(DidExceedMaxLines)},
        ani_native_function{
            "getActualTextRange", getActualTextRangeSignature.c_str(), reinterpret_cast<void*>(GetActualTextRange)},
        ani_native_function{"getLineMetrics", ":Lescompat/Array;", reinterpret_cast<void*>(GetLineMetrics)},
        ani_native_function{"nativeGetLineMetricsAt", "D:L@ohos/graphics/text/text/LineMetrics;",
            reinterpret_cast<void*>(GetLineMetricsAt)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for Paragraph, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniParagraph::LayoutSync(ani_env* env, ani_object object, ani_double width)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    typography->Layout(width);
}

void AniParagraph::Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =  AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "canvas unavailable.");
        return;
    }
    typography->Paint(aniCanvas->GetCanvas(), x, y);
}

void AniParagraph::PaintOnPath(
    ani_env* env, ani_object object, ani_object canvas, ani_object path, ani_double hOffset, ani_double vOffset)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    Drawing::AniCanvas* aniCanvas =  AniTextUtils::GetNativeFromObj<Drawing::AniCanvas>(env, canvas);
    if (aniCanvas == nullptr || aniCanvas->GetCanvas() == nullptr) {
        TEXT_LOGE("Canvas is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Canvas unavailable.");
        return;
    }
    Drawing::Path* pathInternal = AniTextUtils::GetNativeFromObj<Drawing::Path>(env, path);
    if (pathInternal == nullptr) {
        TEXT_LOGE("Path is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Path unavailable.");
        return;
    }
    typography->Paint(aniCanvas->GetCanvas(), pathInternal, hOffset, vOffset);
}

ani_double AniParagraph::GetMaxWidth(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetMaxWidth();
}

ani_double AniParagraph::GetHeight(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetHeight();
}

ani_double AniParagraph::GetLongestLine(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetActualWidth();
}

ani_double AniParagraph::GetLongestLineWithIndent(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetLongestLineWithIndent();
}

ani_double AniParagraph::GetMinIntrinsicWidth(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetMinIntrinsicWidth();
}

ani_double AniParagraph::GetMaxIntrinsicWidth(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetMaxIntrinsicWidth();
}

ani_double AniParagraph::GetAlphabeticBaseline(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetAlphabeticBaseline();
}

ani_double AniParagraph::GetIdeographicBaseline(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetIdeographicBaseline();
}

ani_object AniParagraph::GetRectsForRange(
    ani_env* env, ani_object object, ani_object range, ani_object widthStyle, ani_object heightStyle)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    OHOS::Text::ANI::RectRange rectRange;
    TextRectWidthStyle widthStyleInner;
    TextRectHeightStyle heightStyleInner;

    if (ANI_OK != AniTextRectConverter::ParseRangeToNative(env, range, rectRange)) {
        TEXT_LOGE("Failed to parse range");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    if (ANI_OK != AniTextRectConverter::ParseWidthStyleToNative(env, widthStyle, widthStyleInner)) {
        TEXT_LOGE("Failed to parse width style");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    if (ANI_OK != AniTextRectConverter::ParseHeightStyleToNative(env, heightStyle, heightStyleInner)) {
        TEXT_LOGE("Failed to parse height style");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<TextRect> rectsForRange =
        typography->GetTextRectsByBoundary(rectRange.start, rectRange.end, heightStyleInner, widthStyleInner);

    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, rectsForRange.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& textBox : rectsForRange) {
        ani_object aniObj{nullptr};
        ani_status status = AniTextRectConverter::ParseTextBoxToAni(env, textBox, aniObj);
        if (ANI_OK != status
            || ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Failed to set textBox item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetRectsForPlaceholders(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    std::vector<TextRect> rectsForRange = typography->GetTextRectsOfPlaceholders();

    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    arrayObj = AniTextUtils::CreateAniArray(env, rectsForRange.size());
    ani_boolean isUndefined;
    env->Reference_IsUndefined(arrayObj, &isUndefined);
    if (isUndefined) {
        TEXT_LOGE("Failed to create arrayObject");
        return AniTextUtils::CreateAniUndefined(env);
    }
    ani_size index = 0;
    for (const auto& textBox : rectsForRange) {
        ani_object aniObj{nullptr};
        ani_status status = AniTextRectConverter::ParseTextBoxToAni(env, textBox, aniObj);
        if (ANI_OK != status
            || ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj)) {
            TEXT_LOGE("Failed to set textBox item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetGlyphPositionAtCoordinate(ani_env* env, ani_object object, ani_double x, ani_double y)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    IndexAndAffinity indexAndAffinity = typography->GetGlyphIndexByCoordinate(x, y);
    ani_object indexAndAffinityObj{nullptr};
    ani_status ret =
        AniIndexAndAffinityConverter::ParseIndexAndAffinityToAni(env, indexAndAffinity, indexAndAffinityObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return indexAndAffinityObj;
}

ani_object AniParagraph::GetWordBoundary(ani_env* env, ani_object object, ani_double offset)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }

    Boundary boundary = typography->GetWordBoundaryByIndex(static_cast<size_t>(offset));
    ani_object boundaryObj{nullptr};
    ani_status ret = AniTextRectConverter::ParseBoundaryToAni(env, boundary, boundaryObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return boundaryObj;
}

ani_double AniParagraph::GetLineCount(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetLineCount();
}

ani_double AniParagraph::GetLineHeight(ani_env* env, ani_object object, ani_double line)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }
    return typography->GetLineHeight(static_cast<int>(line));
}

ani_double AniParagraph::GetLineWidth(ani_env* env, ani_object object, ani_double line)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    return typography->GetLineWidth(static_cast<int>(line));
}

ani_boolean AniParagraph::DidExceedMaxLines(ani_env* env, ani_object object)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }

    return typography->DidExceedMaxLines();
}

ani_object AniParagraph::GetActualTextRange(
    ani_env* env, ani_object object, ani_double lineNumber, ani_boolean includeSpaces)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    Boundary Boundary = typography->GetActualTextRange(static_cast<int>(lineNumber), static_cast<bool>(includeSpaces));
    ani_object boundaryObj{nullptr};
    ani_status ret = AniTextRectConverter::ParseBoundaryToAni(env, Boundary, boundaryObj);
    if (ret != ANI_OK) {
        return AniTextUtils::CreateAniUndefined(env);
    }
    return boundaryObj;
}

ani_ref AniParagraph::GetLineMetrics(ani_env* env, ani_object object)
{
    ani_object arrayObj = AniTextUtils::CreateAniUndefined(env);
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return arrayObj;
    }
    std::vector<LineMetrics> vectorLineMetrics = typography->GetLineMetrics();
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
        ani_status ret = env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, aniObj);
        if (ret != ANI_OK) {
            TEXT_LOGE("Failed to set lineMetrics item %{public}zu", index);
            continue;
        }
        index++;
    }
    return arrayObj;
}

ani_object AniParagraph::GetLineMetricsAt(ani_env* env, ani_object object, ani_double lineNumber)
{
    Typography* typography = AniTextUtils::GetNativeFromObj<Typography>(env, object);
    if (typography == nullptr) {
        TEXT_LOGE("Paragraph is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    LineMetrics lineMetrics;
    if (!typography->GetLineMetricsAt(lineNumber, &lineMetrics)) {
        TEXT_LOGE("Failed to get line metrics");
        return AniTextUtils::CreateAniUndefined(env);
    }
    return AniLineMetricsConverter::ParseLineMetricsToAni(env, lineMetrics);
}
} // namespace OHOS::Text::ANI