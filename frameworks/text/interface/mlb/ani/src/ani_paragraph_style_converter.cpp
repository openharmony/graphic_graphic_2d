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
    ani_status ret =
        AniTextUtils::ReadOptionalField(env, obj, AniGlobalMethod::paragraphStyleMaxLines, ref);
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
    if (AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::paragraphStyleTextStyle, textStyleRef) == ANI_OK &&
        textStyleRef != nullptr) {
        ret = AniTextStyleConverter::ParseTextStyleToNative(env, reinterpret_cast<ani_object>(textStyleRef), textStyle);
        if (ret == ANI_OK) {
            paragraphStyle->SetTextStyle(textStyle);
        }
    }
    paragraphStyle->ellipsis = textStyle.ellipsis;
    paragraphStyle->ellipsisModal = textStyle.ellipsisModal;
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textDirection,
        AniGlobalMethod::paragraphStyleTextDirection, paragraphStyle->textDirection);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textAlign,
        AniGlobalMethod::paragraphStyleAlign, paragraphStyle->textAlign);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::wordBreakType,
        AniGlobalMethod::paragraphStyleWordBreak, paragraphStyle->wordBreakType);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::breakStrategy,
        AniGlobalMethod::paragraphStyleBreakStrategy, paragraphStyle->breakStrategy);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textHeightBehavior,
        AniGlobalMethod::paragraphStyleTextHeightBehavior, paragraphStyle->textHeightBehavior);

    ani_ref strutStyleRef = nullptr;
    if (AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::paragraphStyleStrutStyle, strutStyleRef) == ANI_OK &&
        strutStyleRef != nullptr) {
        ParseParagraphStyleStrutStyleToNative(env, reinterpret_cast<ani_object>(strutStyleRef), paragraphStyle);
    }

    ani_ref tabRef = nullptr;
    if (AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::paragraphStyleTab, tabRef) == ANI_OK && tabRef != nullptr) {
        ParseTextTabToNative(env, reinterpret_cast<ani_object>(tabRef), paragraphStyle->tab);
    }

    return paragraphStyle;
}

void AniParagraphStyleConverter::ParseParagraphStyleStrutStyleToNative(
    ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontStyle,
        AniGlobalMethod::strutStyleFontStyle, paragraphStyle->lineStyleFontStyle);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontWidth,
        AniGlobalMethod::strutStyleFontWidth, paragraphStyle->lineStyleFontWidth);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontWeight,
        AniGlobalMethod::strutStyleFontWeight, paragraphStyle->lineStyleFontWeight);

    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::strutStyleFontSize, paragraphStyle->lineStyleFontSize);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::strutStyleHeight, paragraphStyle->lineStyleHeightScale);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::strutStyleLeading, paragraphStyle->lineStyleSpacingScale);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::strutStyleForceHeight, paragraphStyle->lineStyleOnly);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::strutStyleEnabled, paragraphStyle->useLineStyle);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::strutStyleHeightOverride, paragraphStyle->lineStyleHeightOnly);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::strutStyleHalfLeading, paragraphStyle->lineStyleHalfLeading);

    ani_ref aniFontFamilies = nullptr;
    if (AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::strutStyleFontFamilies, aniFontFamilies) == ANI_OK &&
        aniFontFamilies != nullptr) {
        std::vector<std::string> fontFamilies;
        ParseFontFamiliesToNative(env, reinterpret_cast<ani_array>(aniFontFamilies), fontFamilies);
        paragraphStyle->lineStyleFontFamilies = fontFamilies;
    }
}

void AniParagraphStyleConverter::ParseTextTabToNative(ani_env* env, ani_object obj, TextTab& textTab)
{
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::textAlign, AniGlobalMethod::textTabAlignment, textTab.alignment);
    ani_double tempLocation;
    env->Object_CallMethod_Double(obj, AniGlobalMethod::textTabLocation, &tempLocation);
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