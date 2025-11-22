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

#include "ani_paragraph_style_converter.h"

#include "ani_common.h"
#include "ani_text_style_converter.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
constexpr CacheKey PARAGRAPH_STYLE_MAX_LINES_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>maxLines", ANI_WRAP_RETURN_C(ANI_INT)};
constexpr CacheKey PARAGRAPH_STYLE_TEXT_STYLE_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textStyle", ANI_WRAP_RETURN_C(ANI_INTERFACE_TEXT_STYLE)};
constexpr CacheKey PARAGRAPH_STYLE_TEXT_DIRECTION_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textDirection", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DIRECTION)};
constexpr CacheKey PARAGRAPH_STYLE_ALIGN_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>align", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_ALIGN)};
constexpr CacheKey PARAGRAPH_STYLE_WORD_BREAK_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>wordBreak", ANI_WRAP_RETURN_E(ANI_ENUM_WORD_BREAK)};
constexpr CacheKey PARAGRAPH_STYLE_BREAK_STRATEGY_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>breakStrategy", ANI_WRAP_RETURN_E(ANI_ENUM_BREAK_STRATEGY)};
constexpr CacheKey PARAGRAPH_STYLE_TEXT_HEIGHT_BEHAVIOR_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textHeightBehavior", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_HEIGHT_BEHAVIOR)};
constexpr CacheKey PARAGRAPH_STYLE_STRUT_STYLE_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>strutStyle", ANI_WRAP_RETURN_C(ANI_INTERFACE_STRUT_STYLE)};
constexpr CacheKey PARAGRAPH_STYLE_TAB_KEY{
    ANI_INTERFACE_PARAGRAPH_STYLE, "<get>tab", ANI_WRAP_RETURN_C(ANI_INTERFACE_TEXT_TAB)};

constexpr CacheKey STRUT_STYLE_FONT_STYLE_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE)};
constexpr CacheKey STRUT_STYLE_FONT_WIDTH_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontWidth", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WIDTH)};
constexpr CacheKey STRUT_STYLE_FONT_WEIGHT_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT)};
constexpr CacheKey STRUT_STYLE_FONT_SIZE_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>fontSize", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey STRUT_STYLE_HEIGHT_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>height", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey STRUT_STYLE_LEADING_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>leading", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey STRUT_STYLE_FORCE_HEIGHT_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>forceHeight", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_ENABLED_KEY{ANI_INTERFACE_STRUT_STYLE, "<get>enabled", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_HEIGHT_OVERRIDE_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>heightOverride", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_HALF_LEADING_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>halfLeading", ANI_WRAP_RETURN_C(ANI_BOOLEAN)};
constexpr CacheKey STRUT_STYLE_FONT_FAMILIES_KEY{
    ANI_INTERFACE_STRUT_STYLE, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY)};

constexpr CacheKey TEXT_TAB_ALIGNMENT_KEY{
    ANI_INTERFACE_TEXT_TAB, "<get>alignment", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_ALIGN)};
constexpr CacheKey TEXT_TAB_LOCATION_KEY{ANI_INTERFACE_TEXT_TAB, "<get>location", ":d"};
} // namespace
std::unique_ptr<TypographyStyle> AniParagraphStyleConverter::ParseParagraphStyleToNative(ani_env* env, ani_object obj)
{
    std::unique_ptr<TypographyStyle> paragraphStyle = std::make_unique<TypographyStyle>();

    ani_ref ref = nullptr;
    ani_status ret =
        AniTextUtils::ReadOptionalField(env, obj, AniClassFindMethod(env, PARAGRAPH_STYLE_MAX_LINES_KEY), ref);
    if (ret == ANI_OK && ref != nullptr) {
        int maxLines = 0;
        ret = env->Object_CallMethodByName_Int(reinterpret_cast<ani_object>(ref), "toInt", ":i", &maxLines);
        if (ret == ANI_OK) {
            maxLines = maxLines < 0 ? 0 : maxLines;
            paragraphStyle->maxLines = static_cast<size_t>(maxLines);
        } else {
            TEXT_LOGE("Failed to get maxLines, ret %{public}d", ret);
        }
    }

    ani_ref textStyleRef = nullptr;
    TextStyle textStyle;
    if (AniTextUtils::ReadOptionalField(env, obj, AniClassFindMethod(env, PARAGRAPH_STYLE_TEXT_STYLE_KEY), textStyleRef)
            == ANI_OK
        && textStyleRef != nullptr) {
        ret = AniTextStyleConverter::ParseTextStyleToNative(env, reinterpret_cast<ani_object>(textStyleRef), textStyle);
        if (ret == ANI_OK) {
            paragraphStyle->SetTextStyle(textStyle);
        }
    }
    paragraphStyle->ellipsis = textStyle.ellipsis;
    paragraphStyle->ellipsisModal = textStyle.ellipsisModal;
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textDirection,
        AniClassFindMethod(env, PARAGRAPH_STYLE_TEXT_DIRECTION_KEY), paragraphStyle->textDirection);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textAlign,
        AniClassFindMethod(env, PARAGRAPH_STYLE_ALIGN_KEY), paragraphStyle->textAlign);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::wordBreakType,
        AniClassFindMethod(env, PARAGRAPH_STYLE_WORD_BREAK_KEY), paragraphStyle->wordBreakType);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::breakStrategy,
        AniClassFindMethod(env, PARAGRAPH_STYLE_BREAK_STRATEGY_KEY), paragraphStyle->breakStrategy);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textHeightBehavior,
        AniClassFindMethod(env, PARAGRAPH_STYLE_TEXT_HEIGHT_BEHAVIOR_KEY), paragraphStyle->textHeightBehavior);

    ani_ref strutStyleRef = nullptr;
    if (AniTextUtils::ReadOptionalField(
        env, obj, AniClassFindMethod(env, PARAGRAPH_STYLE_STRUT_STYLE_KEY), strutStyleRef) == ANI_OK &&
        strutStyleRef != nullptr) {
        ParseParagraphStyleStrutStyleToNative(env, reinterpret_cast<ani_object>(strutStyleRef), paragraphStyle);
    }

    ani_ref tabRef = nullptr;
    if (AniTextUtils::ReadOptionalField(
        env, obj, AniClassFindMethod(env, PARAGRAPH_STYLE_TAB_KEY), tabRef) == ANI_OK && tabRef != nullptr) {
        ParseTextTabToNative(env, reinterpret_cast<ani_object>(tabRef), paragraphStyle->tab);
    }

    return paragraphStyle;
}

void AniParagraphStyleConverter::ParseParagraphStyleStrutStyleToNative(
    ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontStyle,
        AniClassFindMethod(env, STRUT_STYLE_FONT_STYLE_KEY), paragraphStyle->lineStyleFontStyle);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontWidth,
        AniClassFindMethod(env, STRUT_STYLE_FONT_WIDTH_KEY), paragraphStyle->lineStyleFontWidth);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontWeight,
        AniClassFindMethod(env, STRUT_STYLE_FONT_WEIGHT_KEY), paragraphStyle->lineStyleFontWeight);

    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_FONT_SIZE_KEY), paragraphStyle->lineStyleFontSize);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_HEIGHT_KEY), paragraphStyle->lineStyleHeightScale);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_LEADING_KEY), paragraphStyle->lineStyleSpacingScale);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_FORCE_HEIGHT_KEY), paragraphStyle->lineStyleOnly);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_ENABLED_KEY), paragraphStyle->useLineStyle);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_HEIGHT_OVERRIDE_KEY), paragraphStyle->lineStyleHeightOnly);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_HALF_LEADING_KEY), paragraphStyle->lineStyleHalfLeading);

    ani_ref aniFontFamilies = nullptr;
    if (AniTextUtils::ReadOptionalField(
        env, obj, AniClassFindMethod(env, STRUT_STYLE_FONT_FAMILIES_KEY), aniFontFamilies) == ANI_OK &&
        aniFontFamilies != nullptr) {
        std::vector<std::string> fontFamilies;
        ParseFontFamiliesToNative(env, reinterpret_cast<ani_array>(aniFontFamilies), fontFamilies);
        paragraphStyle->lineStyleFontFamilies = fontFamilies;
    }
}

void AniParagraphStyleConverter::ParseTextTabToNative(ani_env* env, ani_object obj, TextTab& textTab)
{
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::textAlign, AniClassFindMethod(env, TEXT_TAB_ALIGNMENT_KEY), textTab.alignment);
    ani_double tempLocation;
    env->Object_CallMethod_Double(obj, AniClassFindMethod(env, TEXT_TAB_LOCATION_KEY), &tempLocation);
    textTab.location = static_cast<float>(tempLocation);
}

void AniParagraphStyleConverter::ParseFontFamiliesToNative(
    ani_env* env, ani_array obj, std::vector<std::string>& fontFamilies)
{
    ani_size arrayLength = 0;
    env->Array_GetLength(obj, &arrayLength);
    for (size_t i = 0; i < arrayLength; i++) {
        ani_ref tempString = nullptr;
        env->Array_Get(obj, i, &tempString);
        ani_string aniTempString = reinterpret_cast<ani_string>(tempString);
        std::string fontFamiliesString;
        ani_status ret = AniTextUtils::AniToStdStringUtf8(env, aniTempString, fontFamiliesString);
        if (ret == ANI_OK) {
            fontFamilies.push_back(fontFamiliesString);
        }
    }
}
} // namespace OHOS::Text::ANI