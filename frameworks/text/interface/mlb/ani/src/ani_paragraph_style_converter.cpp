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
std::unique_ptr<TypographyStyle> AniParagraphStyleConverter::ParseParagraphStyleToNative(ani_env* env, ani_object obj)
{
    std::unique_ptr<TypographyStyle> paragraphStyle = std::make_unique<TypographyStyle>();

    ani_ref ref = nullptr;
    static AniCacheParam param = { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>maxLines", ANI_WRAP_RETURN_C(ANI_INT) };
    ani_status ret = AniTextUtils::ReadOptionalField(env, obj, param, ref);
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
    static AniCacheParam textStyleParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textStyle", ANI_WRAP_RETURN_C(ANI_INTERFACE_TEXT_STYLE) };
    if (AniTextUtils::ReadOptionalField(env, obj, textStyleParam, textStyleRef) == ANI_OK && textStyleRef != nullptr) {
        ret = AniTextStyleConverter::ParseTextStyleToNative(env, reinterpret_cast<ani_object>(textStyleRef), textStyle);
        if (ret == ANI_OK) {
            paragraphStyle->SetTextStyle(textStyle);
        }
    }
    paragraphStyle->ellipsis = textStyle.ellipsis;
    paragraphStyle->ellipsisModal = textStyle.ellipsisModal;
    static AniCacheParam textDirectionParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textDirection", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DIRECTION) };
    AniTextUtils::ReadOptionalEnumField(env, obj, textDirectionParam, paragraphStyle->textDirection);
    static AniCacheParam textAlignParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>align", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_ALIGN) };
    AniTextUtils::ReadOptionalEnumField(env, obj, textAlignParam, paragraphStyle->textAlign);
    static AniCacheParam wordBreakParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>wordBreak", ANI_WRAP_RETURN_E(ANI_ENUM_WORD_BREAK) };
    AniTextUtils::ReadOptionalEnumField(env, obj, wordBreakParam, paragraphStyle->wordBreakType);
    static AniCacheParam breakStrategyParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>breakStrategy", ANI_WRAP_RETURN_E(ANI_ENUM_BREAK_STRATEGY) };
    AniTextUtils::ReadOptionalEnumField(env, obj, breakStrategyParam, paragraphStyle->breakStrategy);
    static AniCacheParam textHeightBehaviorParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>textHeightBehavior", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_HEIGHT_BEHAVIOR) };
    AniTextUtils::ReadOptionalEnumField(env, obj, textHeightBehaviorParam, paragraphStyle->textHeightBehavior);

    ani_ref strutStyleRef = nullptr;
    static AniCacheParam strutStyleParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>strutStyle", ANI_WRAP_RETURN_C(ANI_INTERFACE_STRUT_STYLE) };
    if (AniTextUtils::ReadOptionalField(env, obj, strutStyleParam, strutStyleRef) == ANI_OK && strutStyleRef != nullptr) {
        ParseParagraphStyleStrutStyleToNative(env, reinterpret_cast<ani_object>(strutStyleRef), paragraphStyle);
    }

    ani_ref tabRef = nullptr;
    static AniCacheParam tabParam =
        { ANI_INTERFACE_PARAGRAPH_STYLE, "<get>tab", ANI_WRAP_RETURN_C(ANI_INTERFACE_TEXT_TAB) };
    if (AniTextUtils::ReadOptionalField(env, obj, tabParam, tabRef) == ANI_OK && tabRef != nullptr) {
        ParseTextTabToNative(env, reinterpret_cast<ani_object>(tabRef), paragraphStyle->tab);
    }

    return paragraphStyle;
}

void AniParagraphStyleConverter::ParseParagraphStyleStrutStyleToNative(
    ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
{
    static AniCacheParam fontStyleParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE) };
    AniTextUtils::ReadOptionalEnumField(env, obj, fontStyleParam, paragraphStyle->lineStyleFontStyle);
    static AniCacheParam fontWidthParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>fontWidth", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WIDTH) };
    AniTextUtils::ReadOptionalEnumField(env, obj, fontWidthParam, paragraphStyle->lineStyleFontWidth);
    static AniCacheParam fontWeightParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT) };
    AniTextUtils::ReadOptionalEnumField(env, obj, fontWeightParam, paragraphStyle->lineStyleFontWeight);

    static AniCacheParam fontSizeParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>fontSize", ANI_WRAP_RETURN_C(ANI_DOUBLE) };
    AniTextUtils::ReadOptionalDoubleField(env, obj, fontSizeParam, paragraphStyle->lineStyleFontSize);
    static AniCacheParam heightParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>height", ANI_WRAP_RETURN_C(ANI_DOUBLE) };
    AniTextUtils::ReadOptionalDoubleField(env, obj, heightParam, paragraphStyle->lineStyleHeightScale);
    static AniCacheParam leadingParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>leading", ANI_WRAP_RETURN_C(ANI_DOUBLE) };
    AniTextUtils::ReadOptionalDoubleField(env, obj, leadingParam, paragraphStyle->lineStyleSpacingScale);
    static AniCacheParam forceHeightParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>forceHeight", ANI_WRAP_RETURN_C(ANI_BOOLEAN) };
    AniTextUtils::ReadOptionalBoolField(env, obj, forceHeightParam, paragraphStyle->lineStyleOnly);
    static AniCacheParam enabledParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>enabled", ANI_WRAP_RETURN_C(ANI_BOOLEAN) };
    AniTextUtils::ReadOptionalBoolField(env, obj, enabledParam, paragraphStyle->useLineStyle);
    static AniCacheParam heightOverrideParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>heightOverride", ANI_WRAP_RETURN_C(ANI_BOOLEAN) };
    AniTextUtils::ReadOptionalBoolField(env, obj, heightOverrideParam, paragraphStyle->lineStyleHeightOnly);
    static AniCacheParam halfLeadingParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>halfLeading", ANI_WRAP_RETURN_C(ANI_BOOLEAN) };
    AniTextUtils::ReadOptionalBoolField(env, obj, halfLeadingParam, paragraphStyle->lineStyleHalfLeading);

    ani_ref aniFontFamilies = nullptr;
    static AniCacheParam fontFamiliesParam =
        { ANI_INTERFACE_STRUT_STYLE, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY) };
    if (AniTextUtils::ReadOptionalField(env, obj, fontFamiliesParam, aniFontFamilies) == ANI_OK
        && aniFontFamilies != nullptr) {
        std::vector<std::string> fontFamilies;
        ParseFontFamiliesToNative(env, reinterpret_cast<ani_array>(aniFontFamilies), fontFamilies);
        paragraphStyle->lineStyleFontFamilies = fontFamilies;
    }
}

void AniParagraphStyleConverter::ParseTextTabToNative(ani_env* env, ani_object obj, TextTab& textTab)
{
    static AniCacheParam alignmentParam =
        { ANI_INTERFACE_TEXT_TAB, "<get>alignment", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_ALIGN) };
    AniTextUtils::ReadOptionalEnumField(env, obj, alignmentParam, textTab.alignment);
    ani_double tempLocation;
    static AniCacheParam param = { ANI_INTERFACE_TEXT_TAB, "<get>location", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, obj, param, tempLocation);
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