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

#include <cstddef>
#include <cstdint>
#include <vector>
#include "ani.h"
#include "ani_common.h"
#include "ani_text_utils.h"
#include "typography_types.h"
#include "utils/text_log.h"

namespace OHOS::Text::NAI {
std::unique_ptr<TypographyStyle> AniCommon::ParseParagraphStyle(ani_env* env, ani_object obj)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
        return nullptr;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
        return nullptr;
    }
    std::unique_ptr<TypographyStyle> paragraphStyle = std::make_unique<TypographyStyle>();

    double maxLines;
    if (AniTextUtils::ReadOptionalDoubleField(env, obj, "maxLines", maxLines) == ANI_OK) {
        paragraphStyle->maxLines = static_cast<size_t>(maxLines);
    }

    ani_ref textStyleRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "textStyle", textStyleRef) == ANI_OK && textStyleRef != nullptr) {
        std::unique_ptr<TextStyle> textStyle = ParseTextStyle(env, static_cast<ani_object>(textStyleRef));
        if (textStyle != nullptr) {
            paragraphStyle->SetTextStyle(*textStyle);
        }
    }

    AniTextUtils::ReadOptionalEnumField(env, obj, "textDirection", paragraphStyle->textDirection);
    AniTextUtils::ReadOptionalEnumField(env, obj, "align", paragraphStyle->textAlign);
    AniTextUtils::ReadOptionalEnumField(env, obj, "wordBreak", paragraphStyle->wordBreakType);
    AniTextUtils::ReadOptionalEnumField(env, obj, "breakStrategy", paragraphStyle->breakStrategy);
    AniTextUtils::ReadOptionalEnumField(env, obj, "textHeightBehavior", paragraphStyle->textHeightBehavior);

    ani_ref strutStyleRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "strutStyle", strutStyleRef) == ANI_OK && strutStyleRef != nullptr) {
        SetParagraphStyleStrutStyle(env, static_cast<ani_object>(strutStyleRef), paragraphStyle);
    }

    ani_ref tabRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "tab", tabRef) == ANI_OK && tabRef != nullptr) {
        SetParagraphStyleTab(env, static_cast<ani_object>(tabRef), paragraphStyle);
    }

    return paragraphStyle;
}

void AniCommon::SetParagraphStyleStrutStyle(ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
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
        SetFontFamilies(env, static_cast<ani_array_ref>(aniFontFamilies), fontFamilies);
        paragraphStyle->lineStyleFontFamilies = fontFamilies;
    }
}

void AniCommon::SetFontFamilies(ani_env* env, ani_array_ref obj, std::vector<std::string>& fontFamilies)
{
    ani_size arrayLength = 0;
    env->Array_GetLength(obj, &arrayLength);
    for (int i = 0; i < arrayLength; i++) {
        ani_ref tempString = nullptr;
        env->Array_Get_Ref(obj, i, &tempString);
        ani_string aniTempString = static_cast<ani_string>(tempString);
        std::string fontFamiliesString =AniTextUtils::AniToStdStringUtf8(env, aniTempString);
        fontFamilies.push_back(fontFamiliesString);
    }
}

void AniCommon::SetParagraphStyleTab(ani_env* env, ani_object obj, std::unique_ptr<TypographyStyle>& paragraphStyle)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, "alignment", paragraphStyle->tab.alignment);
    ani_double tempLocation;
    env->Object_GetPropertyByName_Double(obj, "location", &tempLocation);
    paragraphStyle->tab.location = static_cast<float>(tempLocation);
}

std::unique_ptr<TextStyle> AniCommon::ParseTextStyle(ani_env* env, ani_object obj)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_TEXT_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
        return nullptr;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
        return nullptr;
    }
    std::unique_ptr<TextStyle> textStyle = std::make_unique<TextStyle>();
    
    ani_ref decorationRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "decoration", decorationRef) == ANI_OK && decorationRef != nullptr) {
        AniTextUtils::ReadOptionalEnumField(env, static_cast<ani_object>(decorationRef), "textDecoration", textStyle->decoration);
    }

    ParseTextShadow(env, obj, textStyle->shadows);

    ani_ref colorRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "color", colorRef) == ANI_OK && colorRef != nullptr) {
        ParseDrawingColor(env, static_cast<ani_object>(colorRef), textStyle->color);
    }

    AniTextUtils::ReadOptionalEnumField(env, obj, "fontWeight", textStyle->fontWeight);
    AniTextUtils::ReadOptionalEnumField(env, obj, "fontStyle", textStyle->fontStyle);
    if (textStyle->fontStyle == FontStyle::OBLIQUE) {
        textStyle->fontStyle = FontStyle::ITALIC;
    }
    AniTextUtils::ReadOptionalEnumField(env, obj, "baseline", textStyle->baseline);

    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontFamilies", textStyle->fontFamilies,
        [](ani_env* env, ani_ref ref) { return AniTextUtils::AniToStdStringUtf8(env, static_cast<ani_string>(ref)); });

    AniTextUtils::ReadOptionalDoubleField(env, obj, "fontSize", textStyle->fontSize);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "letterSpacing", textStyle->letterSpacing);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "wordSpacing", textStyle->wordSpacing);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "heightScale", textStyle->heightScale);
    AniTextUtils::ReadOptionalBoolField(env, obj, "halfLeading", textStyle->halfLeading);
    AniTextUtils::ReadOptionalBoolField(env, obj, "heightOnly", textStyle->heightOnly);
    AniTextUtils::ReadOptionalU16StringField(env, obj, "ellipsis", textStyle->ellipsis);
    AniTextUtils::ReadOptionalEnumField(env, obj, "ellipsisMode", textStyle->ellipsisModal);
    AniTextUtils::ReadOptionalStringField(env, obj, "locale", textStyle->locale);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "baselineShift", textStyle->baseLineShift);
    ParseFontFeature(env, obj, textStyle->fontFeatures);

    return textStyle;
}

inline void GetPointXFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue{0};
    double targetX = 0;
    ani_status ret;
    ret = env->Object_GetPropertyByName_Double(argValue, "x", &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] The Parameter of number y about JsPoint is invalid:%{public}d", ret);
        return;
    }
    targetX = static_cast<double>(objValue);
    point.SetX(targetX);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue{0};
    double targetY = 0;
    ani_status ret;
    ret = env->Object_GetPropertyByName_Double(argValue, "y", &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] The Parameter of number y about JsPoint is invalid:%{public}d", ret);
        return;
    }
    targetY = static_cast<double>(objValue);
    point.SetY(targetY);
}

inline void GetTextShadowPoint(ani_env* env, ani_object obj, Drawing::Point& point)
{
    GetPointXFromJsBumber(env, obj, point);
    GetPointYFromJsBumber(env, obj, point);
}

void AniCommon::ParseTextShadow(ani_env* env, ani_object obj, std::vector<TextShadow>& textShadow)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "textShadows", array, [&textShadow](ani_env* env, ani_ref ref) {
            ani_object obj = static_cast<ani_object>(ref);
            ani_class cls;
            ani_status ret;
            ret = env->FindClass(ANI_CLASS_TEXTSHADOW, &cls);
            if(ret != ANI_OK) {
                TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(static_cast<ani_object>(ref), cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
                return "";
            }

            double runTimeRadius;
            AniTextUtils::ReadOptionalDoubleField(env, obj, "blurRadius", runTimeRadius);
            
            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            SetTextShadowColor(env, obj, "color", colorSrc);

            ani_ref pointValue = nullptr;
            AniTextUtils::ReadOptionalField(env, obj, "point", pointValue);
            Drawing::Point offset(0,0);
            GetTextShadowPoint(env, static_cast<ani_object>(pointValue), offset);

            textShadow.emplace_back(TextShadow(colorSrc, offset, runTimeRadius));
            return "";
        }
    );
}

inline void ConvertClampFromJsValue(ani_env* env, ani_double jsValue, int32_t& value, int32_t lo, int32_t hi)
{
    value =static_cast<int32_t>(jsValue);
    value = std::clamp(value, lo, hi);
}

void AniCommon::SetTextShadowColor(ani_env* env, ani_object obj, const std::string& str, Drawing::Color& colorSrc)
{
    ani_ref tempValue =nullptr;
    ani_double tempValueChild{0};
    env->Object_GetPropertyByName_Ref(obj, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        TEXT_LOGE("[ANI] set text shadow color faild");
        return;//false
    }
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "alpha", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, alpha, 0, Drawing::Color::RGB_MAX);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "red", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, red, 0, Drawing::Color::RGB_MAX);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "green", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, green, 0, Drawing::Color::RGB_MAX);
    env->Object_GetPropertyByName_Double(static_cast<ani_object>(tempValue), "blue", &tempValueChild);
    ConvertClampFromJsValue(env, tempValueChild, blue, 0, Drawing::Color::RGB_MAX);

    Drawing::Color color(Drawing::Color::ColorQuadSetARGB(alpha, red, green, blue));
    colorSrc = color;
}

void AniCommon::ParseDrawingColor(ani_env* env, ani_object obj, Drawing::Color& color)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_TEXT_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
        return;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
        return;
    }
}

void AniCommon::ParseFontFeature(ani_env* env, ani_object obj, FontFeatures& fontFeatures)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontFeatures", array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = static_cast<ani_object>(ref);
            ani_class cls;
            ani_status ret;
            ret = env->FindClass(ANI_CLASS_FONTFEATURE, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(static_cast<ani_object>(ref), cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
                return "";
            }
            ani_ref nameRef = nullptr;
            ret = env->Object_GetPropertyByName_Ref(obj, "name", &nameRef);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] get filed name failed:%{public}d", ret);
                return "";
            }
            std::string name = AniTextUtils::AniToStdStringUtf8(env, static_cast<ani_string>(nameRef));
            ani_double valueDouble;
            ret = env->Object_GetFieldByName_Double(obj, "value", &valueDouble);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] get filed name failed:%{public}d", ret);
                return "";
            }
            fontFeatures.SetFeature(name, static_cast<int>(valueDouble));
            return "";
        });
}
} // namespace OHOS::Text::NAI