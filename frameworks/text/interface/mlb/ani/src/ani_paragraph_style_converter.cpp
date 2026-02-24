/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
void AniParagraphStyleConverter::ParseSimpleParagraphStyleToNative(
    ani_env* env, ani_object obj, std::unique_ptr<OHOS::Rosen::TypographyStyle>& paragraphStyle)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textDirection,
        AniGlobalMethod::GetInstance().paragraphStyleTextDirection, paragraphStyle->textDirection);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textAlign,
        AniGlobalMethod::GetInstance().paragraphStyleAlign, paragraphStyle->textAlign);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::wordBreakType,
        AniGlobalMethod::GetInstance().paragraphStyleWordBreak, paragraphStyle->wordBreakType);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::breakStrategy,
        AniGlobalMethod::GetInstance().paragraphStyleBreakStrategy, paragraphStyle->breakStrategy);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textHeightBehavior,
        AniGlobalMethod::GetInstance().paragraphStyleTextHeightBehavior, paragraphStyle->textHeightBehavior);
    AniTextUtils::ReadOptionalBoolField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleTrailingSpaceOptimized,
        paragraphStyle->isTrailingSpaceOptimized);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::GetInstance().paragraphStyleAutoSpace, paragraphStyle->enableAutoSpace);
    AniTextUtils::ReadOptionalBoolField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleCompressHeadPunctuation,
        paragraphStyle->compressHeadPunctuation);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::textVerticalAlign,
        AniGlobalMethod::GetInstance().paragraphStyleVerticalAlign, paragraphStyle->verticalAlignment);
    AniTextUtils::ReadOptionalBoolField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleIncludeFontPadding,
        paragraphStyle->includeFontPadding);
    AniTextUtils::ReadOptionalBoolField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleFallbackLineSpacing,
        paragraphStyle->fallbackLineSpacing);
    AniTextUtils::ReadOptionalDoubleField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleLineSpacing,
        paragraphStyle->lineSpacing);
}

ani_status ParagraphStyleGetMaxLine(ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
{
    ani_ref ref = nullptr;
    ani_status ret =
        AniTextUtils::ReadOptionalField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleMaxLines, ref);
    if (ret == ANI_OK && ref != nullptr) {
        int maxLines = 0;
        ret = env->Object_CallMethod_Int(
            reinterpret_cast<ani_object>(ref), AniGlobalMethod::GetInstance().intGet, &maxLines);
        if (ret == ANI_OK) {
            maxLines = maxLines < 0 ? 0 : maxLines;
            paragraphStyle->maxLines = static_cast<size_t>(maxLines);
        } else {
            TEXT_LOGE("Failed to get maxLines, ret %{public}d", ret);
        }
    }
    return ret;
}

std::unique_ptr<TypographyStyle> AniParagraphStyleConverter::ParseParagraphStyleToNative(ani_env* env, ani_object obj)
{
    std::unique_ptr<TypographyStyle> paragraphStyle = std::make_unique<TypographyStyle>();

    ParagraphStyleGetMaxLine(env, obj, paragraphStyle);

    ani_ref textStyleRef = nullptr;
    TextStyle textStyle;
    ani_status ret =
        AniTextUtils::ReadOptionalField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleTextStyle, textStyleRef);
    if (ret == ANI_OK && textStyleRef != nullptr) {
        ret = AniTextStyleConverter::ParseTextStyleToNative(env, reinterpret_cast<ani_object>(textStyleRef), textStyle);
        if (ret == ANI_OK) {
            paragraphStyle->SetTextStyle(textStyle);
        }
    }
    paragraphStyle->ellipsis = textStyle.ellipsis;
    paragraphStyle->ellipsisModal = textStyle.ellipsisModal;

    ani_ref strutStyleRef = nullptr;
    ret = AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::GetInstance().paragraphStyleStrutStyle, strutStyleRef);
    if (ret == ANI_OK && strutStyleRef != nullptr) {
        ParseParagraphStyleStrutStyleToNative(env, reinterpret_cast<ani_object>(strutStyleRef), paragraphStyle);
    }

    ani_ref tabRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, AniGlobalMethod::GetInstance().paragraphStyleTab, tabRef) == ANI_OK &&
        tabRef != nullptr) {
        ParseTextTabToNative(env, reinterpret_cast<ani_object>(tabRef), paragraphStyle->tab);
    }

    ParseSimpleParagraphStyleToNative(env, obj, paragraphStyle);

    return paragraphStyle;
}

void AniParagraphStyleConverter::ParseParagraphStyleStrutStyleToNative(
    ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontStyle,
        AniGlobalMethod::GetInstance().strutStyleFontStyle, paragraphStyle->lineStyleFontStyle);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontWidth,
        AniGlobalMethod::GetInstance().strutStyleFontWidth, paragraphStyle->lineStyleFontWidth);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::fontWeight,
        AniGlobalMethod::GetInstance().strutStyleFontWeight, paragraphStyle->lineStyleFontWeight);

    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleFontSize, paragraphStyle->lineStyleFontSize);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleHeight, paragraphStyle->lineStyleHeightScale);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleLeading, paragraphStyle->lineStyleSpacingScale);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleForceHeight, paragraphStyle->lineStyleOnly);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleEnabled, paragraphStyle->useLineStyle);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleHeightOverride, paragraphStyle->lineStyleHeightOnly);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleHalfLeading, paragraphStyle->lineStyleHalfLeading);

    ani_ref aniFontFamilies = nullptr;
    ani_status ret = AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::GetInstance().strutStyleFontFamilies, aniFontFamilies);
    if (ret == ANI_OK && aniFontFamilies != nullptr) {
        std::vector<std::string> fontFamilies;
        ParseFontFamiliesToNative(env, reinterpret_cast<ani_array>(aniFontFamilies), fontFamilies);
        paragraphStyle->lineStyleFontFamilies = fontFamilies;
    }
}

void AniParagraphStyleConverter::ParseTextTabToNative(ani_env* env, ani_object obj, TextTab& textTab)
{
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::textAlign, AniGlobalMethod::GetInstance().textTabAlignment, textTab.alignment);
    ani_double tempLocation;
    ani_status ret = env->Object_CallMethod_Double(obj, AniGlobalMethod::GetInstance().textTabLocation, &tempLocation);
    if (ret == ANI_OK) {
        textTab.location = static_cast<float>(tempLocation);
    } else {
        TEXT_LOGE("Failed to read text tab location, ret %{public}d", ret);
    }
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