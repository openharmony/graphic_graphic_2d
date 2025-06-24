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
#include "ani_common.h"
#include "ani_paragraph_style_converter.h"
#include "ani_text_style_converter.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
std::unique_ptr<TypographyStyle> AniParagraphStyleConverter::ParseParagraphStyleToNative(ani_env* env, ani_object obj)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_INTERFACE_PARAGRAPH_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return nullptr;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("Object mismatch, ret %{public}d", ret);
        return nullptr;
    }
    std::unique_ptr<TypographyStyle> paragraphStyle = std::make_unique<TypographyStyle>();

    double maxLines;
    if (AniTextUtils::ReadOptionalDoubleField(env, obj, "maxLines", maxLines) == ANI_OK) {
        paragraphStyle->maxLines = static_cast<size_t>(maxLines);
    }

    ani_ref textStyleRef = nullptr;
    TextStyle textStyle;
    if (AniTextUtils::ReadOptionalField(env, obj, "textStyle", textStyleRef) == ANI_OK && textStyleRef != nullptr) {
        ret = AniTextStyleConverter::ParseTextStyleToNative(env, reinterpret_cast<ani_object>(textStyleRef), textStyle);
        if (ret == ANI_OK) {
            paragraphStyle->SetTextStyle(textStyle);
        }
    }
    paragraphStyle->ellipsis = textStyle.ellipsis;
    paragraphStyle->ellipsisModal = textStyle.ellipsisModal;

    AniTextUtils::ReadOptionalEnumField(env, obj, "textDirection", paragraphStyle->textDirection);
    AniTextUtils::ReadOptionalEnumField(env, obj, "align", paragraphStyle->textAlign);
    AniTextUtils::ReadOptionalEnumField(env, obj, "wordBreak", paragraphStyle->wordBreakType);
    AniTextUtils::ReadOptionalEnumField(env, obj, "breakStrategy", paragraphStyle->breakStrategy);
    AniTextUtils::ReadOptionalEnumField(env, obj, "textHeightBehavior", paragraphStyle->textHeightBehavior);

    ani_ref strutStyleRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "strutStyle", strutStyleRef) == ANI_OK && strutStyleRef != nullptr) {
        ParseParagraphStyleStrutStyleToNative(env, reinterpret_cast<ani_object>(strutStyleRef), paragraphStyle);
    }

    ani_ref tabRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "tab", tabRef) == ANI_OK && tabRef != nullptr) {
        ParseTextTabToNative(env, reinterpret_cast<ani_object>(tabRef), paragraphStyle->tab);
    }

    return paragraphStyle;
}

void AniParagraphStyleConverter::ParseParagraphStyleStrutStyleToNative(
    ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, "fontStyle", paragraphStyle->lineStyleFontStyle);
    AniTextUtils::ReadOptionalEnumField(env, obj, "fontWidth", paragraphStyle->lineStyleFontWidth);
    AniTextUtils::ReadOptionalEnumField(env, obj, "fontWeight", paragraphStyle->lineStyleFontWeight);

    AniTextUtils::ReadOptionalDoubleField(env, obj, "fontSize", paragraphStyle->lineStyleFontSize);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "height", paragraphStyle->lineStyleHeightScale);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "leading", paragraphStyle->lineStyleSpacingScale);

    AniTextUtils::ReadOptionalBoolField(env, obj, "forceHeight", paragraphStyle->lineStyleOnly);
    AniTextUtils::ReadOptionalBoolField(env, obj, "enabled", paragraphStyle->useLineStyle);
    AniTextUtils::ReadOptionalBoolField(env, obj, "heightOverride", paragraphStyle->lineStyleHeightOnly);
    AniTextUtils::ReadOptionalBoolField(env, obj, "halfLeading", paragraphStyle->lineStyleHalfLeading);

    ani_ref aniFontFamilies = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "fontFamilies", aniFontFamilies) == ANI_OK
        && aniFontFamilies != nullptr) {
        std::vector<std::string> fontFamilies;
        ParseFontFamiliesToNative(env, reinterpret_cast<ani_array>(aniFontFamilies), fontFamilies);
        paragraphStyle->lineStyleFontFamilies = fontFamilies;
    }
}

void AniParagraphStyleConverter::ParseTextTabToNative(ani_env* env, ani_object obj, TextTab& textTab)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, "alignment", textTab.alignment);
    ani_double tempLocation;
    env->Object_GetPropertyByName_Double(obj, "location", &tempLocation);
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
