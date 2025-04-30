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
#include "text_style.h"
#include "typography_types.h"
#include "utils/text_log.h"

namespace OHOS::Text::NAI {
std::unique_ptr<TypographyStyle> AniCommon::ParseParagraphStyleToNative(ani_env* env, ani_object obj)
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
        std::unique_ptr<TextStyle> textStyle = ParseTextStyleToNative(env, static_cast<ani_object>(textStyleRef));
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
        ParseParagraphStyleStrutStyleToNative(env, static_cast<ani_object>(strutStyleRef), paragraphStyle);
    }

    ani_ref tabRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "tab", tabRef) == ANI_OK && tabRef != nullptr) {
        ParseTextTabToNative(env, static_cast<ani_object>(tabRef), paragraphStyle->tab);
    }

    return paragraphStyle;
}

void AniCommon::ParseParagraphStyleStrutStyleToNative(ani_env* env, ani_object obj,
                                                      std::unique_ptr<TypographyStyle>& paragraphStyle)
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
        ParseFontFamiliesToNative(env, static_cast<ani_array_ref>(aniFontFamilies), fontFamilies);
        paragraphStyle->lineStyleFontFamilies = fontFamilies;
    }
}

void AniCommon::ParseFontFamiliesToNative(ani_env* env, ani_array_ref obj, std::vector<std::string>& fontFamilies)
{
    ani_size arrayLength = 0;
    env->Array_GetLength(obj, &arrayLength);
    for (size_t i = 0; i < arrayLength; i++) {
        ani_ref tempString = nullptr;
        env->Array_Get_Ref(obj, i, &tempString);
        ani_string aniTempString = static_cast<ani_string>(tempString);
        std::string fontFamiliesString = AniTextUtils::AniToStdStringUtf8(env, aniTempString);
        fontFamilies.push_back(fontFamiliesString);
    }
}

void AniCommon::ParseTextTabToNative(ani_env* env, ani_object obj, TextTab& textTab)
{
    AniTextUtils::ReadOptionalEnumField(env, obj, "alignment", textTab.alignment);
    ani_double tempLocation;
    env->Object_GetPropertyByName_Double(obj, "location", &tempLocation);
    textTab.location = static_cast<float>(tempLocation);
}

std::unique_ptr<TextStyle> AniCommon::ParseTextStyleToNative(ani_env* env, ani_object obj)
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
        AniTextUtils::ReadOptionalEnumField(env, static_cast<ani_object>(decorationRef), "textDecoration",
                                            textStyle->decoration);
        AniTextUtils::ReadOptionalEnumField(env, static_cast<ani_object>(decorationRef), "decorationStyle",
                                            textStyle->decorationStyle);
        AniTextUtils::ReadOptionalDoubleField(env, static_cast<ani_object>(decorationRef), "decorationThicknessScale",
                                              textStyle->decorationThicknessScale);
        ParseTextColorToNative(env, static_cast<ani_object>(decorationRef), "color", textStyle->decorationColor);
    }

    ani_ref colorRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "color", colorRef) == ANI_OK && colorRef != nullptr) {
        ParseDrawingColorToNative(env, static_cast<ani_object>(colorRef), textStyle->color);
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
    ParseTextShadowToNative(env, obj, textStyle->shadows);
    ParseFontFeatureToNative(env, obj, textStyle->fontFeatures);
    ParseFontVariationToNative(env, obj, textStyle->fontVariations);

    ani_ref backgroundRectRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "backgroundRect", backgroundRectRef) == ANI_OK
        && backgroundRectRef != nullptr) {
        ParseRectStyleToNative(env, static_cast<ani_object>(backgroundRectRef), textStyle->backgroundRect);
    }

    return textStyle;
}

inline void GetPointXFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue{0};
    ani_status ret;
    ret = env->Object_GetPropertyByName_Double(argValue, "x", &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] The Parameter of number y about JsPoint is invalid:%{public}d", ret);
        return;
    }
    point.SetX(objValue);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue{0};
    ani_status ret;
    ret = env->Object_GetPropertyByName_Double(argValue, "y", &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] The Parameter of number y about JsPoint is invalid:%{public}d", ret);
        return;
    }
    point.SetY(objValue);
}

inline void GetTextShadowPoint(ani_env* env, ani_object obj, Drawing::Point& point)
{
    GetPointXFromJsBumber(env, obj, point);
    GetPointYFromJsBumber(env, obj, point);
}

void AniCommon::ParseTextShadowToNative(ani_env* env, ani_object obj, std::vector<TextShadow>& textShadow)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "textShadows", array, [&textShadow](ani_env* env, ani_ref ref) {
            ani_object shadowObj = static_cast<ani_object>(ref);
            ani_class cls;
            ani_status ret;
            ret = env->FindClass(ANI_CLASS_TEXTSHADOW, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(shadowObj, cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
                return "";
            }

            double runTimeRadius;
            AniTextUtils::ReadOptionalDoubleField(env, shadowObj, "blurRadius", runTimeRadius);

            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            ParseTextColorToNative(env, shadowObj, "color", colorSrc);

            Drawing::Point offset(0, 0);
            ani_ref pointValue = nullptr;
            if (ANI_OK == AniTextUtils::ReadOptionalField(env, shadowObj, "point", pointValue)
                && pointValue != nullptr) {
                GetTextShadowPoint(env, static_cast<ani_object>(pointValue), offset);
            }

            textShadow.emplace_back(TextShadow(colorSrc, offset, runTimeRadius));
            return "";
        });
}

inline void ConvertClampFromJsValue(ani_env* env, ani_double jsValue, int32_t& value, int32_t lo, int32_t hi)
{
    value = static_cast<int32_t>(jsValue);
    value = std::clamp(value, lo, hi);
}

void AniCommon::ParseTextColorToNative(ani_env* env, ani_object obj, const std::string& str, Drawing::Color& colorSrc)
{
    ani_ref tempValue = nullptr;
    ani_double tempValueChild{0};
    ani_status result = env->Object_GetPropertyByName_Ref(obj, str.c_str(), &tempValue);
    if (result != ANI_OK || tempValue == nullptr) {
        TEXT_LOGD("[ANI] set color faild");
        return; //false
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

void AniCommon::ParseDrawingColorToNative(ani_env* env, ani_object obj, Drawing::Color& color)
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

void AniCommon::ParseFontFeatureToNative(ani_env* env, ani_object obj, FontFeatures& fontFeatures)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontFeatures", array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = static_cast<ani_object>(ref);
            ani_class cls;
            ani_status ret;
            ret = env->FindClass(ANI_CLASS_FONT_FEATURE, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(obj, cls, &isObj);
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
            ret = env->Object_GetPropertyByName_Double(obj, "value", &valueDouble);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] get filed value failed:%{public}d", ret);
                return "";
            }
            fontFeatures.SetFeature(name, static_cast<int>(valueDouble));
            return "";
        });
}

void AniCommon::ParseFontVariationToNative(ani_env* env, ani_object obj, FontVariations& fontVariations)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontVariations", array, [&fontVariations](ani_env* env, ani_ref ref) {
            ani_object obj = static_cast<ani_object>(ref);
            ani_class cls;
            ani_status ret;
            ret = env->FindClass(ANI_CLASS_FONT_VARIATION, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(obj, cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
                return "";
            }
            ani_ref axisRef = nullptr;
            ret = env->Object_GetPropertyByName_Ref(obj, "axis", &axisRef);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] get filed axis failed:%{public}d", ret);
                return "";
            }
            std::string axis = AniTextUtils::AniToStdStringUtf8(env, static_cast<ani_string>(axisRef));
            ani_double valueDouble;
            ret = env->Object_GetPropertyByName_Double(obj, "value", &valueDouble);
            if (ret != ANI_OK) {
                TEXT_LOGE("[ANI] get filed value failed:%{public}d", ret);
                return "";
            }
            fontVariations.SetAxisValue(axis, static_cast<int>(valueDouble));
            return "";
        });
}

void AniCommon::ParseRectStyleToNative(ani_env* env, ani_object obj, RectStyle& rectStyle)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_RECT_STYLE, &cls);
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
    env->Object_GetPropertyByName_Double(obj, "leftTopRadius", &rectStyle.leftTopRadius);
    env->Object_GetPropertyByName_Double(obj, "rightTopRadius", &rectStyle.rightTopRadius);
    env->Object_GetPropertyByName_Double(obj, "rightBottomRadius", &rectStyle.rightBottomRadius);
    env->Object_GetPropertyByName_Double(obj, "leftBottomRadius", &rectStyle.leftBottomRadius);
}

ani_object AniCommon::ParseTextStyleToAni(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXT_STYLE_I, ":V");
    env->Object_SetPropertyByName_Ref(aniObj, "decoration", AniCommon::ParseDecorationToAni(env, textStyle));
    //env->Object_SetPropertyByName_Ref(aniObj, "color", ani_int(textStyle.color.CastToColorQuad()));
    env->Object_SetPropertyByName_Ref(
        aniObj, "fontWeight",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_FONT_WEIGHT, static_cast<int>(textStyle.fontWeight)));
    env->Object_SetPropertyByName_Ref(
        aniObj, "fontStyle",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_FONT_STYLE, static_cast<int>(textStyle.fontStyle)));
    env->Object_SetPropertyByName_Ref(
        aniObj, "baseline",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_BASELINE, static_cast<int>(textStyle.baseline)));

    ani_object fontFamiliesAniObj = AniTextUtils::CreateAniArrayAndInitData(
        env, textStyle.fontFamilies, textStyle.fontFamilies.size(),
        [](ani_env* env, const std::string& item) { return AniTextUtils::CreateAniStringObj(env, item); });
    env->Object_SetPropertyByName_Ref(aniObj, "fontFamilies", fontFamiliesAniObj);

    env->Object_SetPropertyByName_Ref(aniObj, "fontSize", AniTextUtils::CreateAniDoubleObj(env, textStyle.fontSize));
    env->Object_SetPropertyByName_Ref(aniObj, "letterSpacing",
                                      AniTextUtils::CreateAniDoubleObj(env, textStyle.letterSpacing));
    env->Object_SetPropertyByName_Ref(aniObj, "wordSpacing",
                                      AniTextUtils::CreateAniDoubleObj(env, textStyle.wordSpacing));
    env->Object_SetPropertyByName_Ref(aniObj, "heightScale",
                                      AniTextUtils::CreateAniDoubleObj(env, textStyle.heightScale));
    env->Object_SetPropertyByName_Ref(aniObj, "halfLeading",
                                      AniTextUtils::CreateAniBooleanObj(env, textStyle.halfLeading));
    env->Object_SetPropertyByName_Ref(aniObj, "heightOnly",
                                      AniTextUtils::CreateAniBooleanObj(env, textStyle.heightOnly));
    env->Object_SetPropertyByName_Ref(aniObj, "ellipsis", AniTextUtils::CreateAniStringObj(env, textStyle.ellipsis));
    env->Object_SetPropertyByName_Ref(
        aniObj, "ellipsisMode",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_ELLIPSIS_MODE, static_cast<int>(textStyle.ellipsisModal)));
    env->Object_SetPropertyByName_Ref(aniObj, "locale", AniTextUtils::CreateAniStringObj(env, textStyle.locale));
    env->Object_SetPropertyByName_Ref(aniObj, "baselineShift",
                                      AniTextUtils::CreateAniDoubleObj(env, textStyle.baseLineShift));

    env->Object_SetPropertyByName_Ref(aniObj, "backgroundRect",
                                      AniCommon::ParseRectStyleToAni(env, textStyle.backgroundRect));

    ani_object shadowsAniObj = AniTextUtils::CreateAniArrayAndInitData(
        env, textStyle.shadows, textStyle.shadows.size(),
        [](ani_env* env, const TextShadow& item) { return AniCommon::ParseTextShadowToAni(env, item); });
    env->Object_SetPropertyByName_Ref(aniObj, "textShadows", shadowsAniObj);

    env->Object_SetPropertyByName_Ref(aniObj, "fontFeatures", ParseFontFeaturesToAni(env,textStyle.fontFeatures));
    return aniObj;
}

ani_object AniCommon::ParseFontMetricsToAni(ani_env* env, const Drawing::FontMetrics& fontMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_METRICS, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "flags", ani_int(fontMetrics.fFlags));
    env->Object_SetPropertyByName_Double(aniObj, "top", ani_double(fontMetrics.fTop));
    env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_double(fontMetrics.fAscent));
    env->Object_SetPropertyByName_Double(aniObj, "descent", ani_double(fontMetrics.fDescent));
    env->Object_SetPropertyByName_Double(aniObj, "bottom", ani_double(fontMetrics.fBottom));
    env->Object_SetPropertyByName_Double(aniObj, "leading", ani_double(fontMetrics.fLeading));
    env->Object_SetPropertyByName_Double(aniObj, "avgCharWidth", ani_double(fontMetrics.fAvgCharWidth));
    env->Object_SetPropertyByName_Double(aniObj, "maxCharWidth", ani_double(fontMetrics.fMaxCharWidth));
    env->Object_SetPropertyByName_Double(aniObj, "xMin", ani_double(fontMetrics.fXMin));
    env->Object_SetPropertyByName_Double(aniObj, "xMax", ani_double(fontMetrics.fXMax));
    env->Object_SetPropertyByName_Double(aniObj, "xHeight", ani_double(fontMetrics.fXHeight));
    env->Object_SetPropertyByName_Double(aniObj, "capHeight", ani_double(fontMetrics.fCapHeight));
    env->Object_SetPropertyByName_Double(aniObj, "underlineThickness", ani_double(fontMetrics.fUnderlineThickness));
    env->Object_SetPropertyByName_Double(aniObj, "underlinePosition", ani_double(fontMetrics.fUnderlinePosition));
    env->Object_SetPropertyByName_Double(aniObj, "strikethroughThickness", ani_double(fontMetrics.fStrikeoutThickness));
    env->Object_SetPropertyByName_Double(aniObj, "strikethroughPosition", ani_double(fontMetrics.fStrikeoutPosition));
    return aniObj;
}

ani_object AniCommon::ParseRunMetricsToAni(ani_env* env, const std::map<size_t, RunMetrics>& map)
{
    ani_object mapAniObj = AniTextUtils::CreateAniMap(env);
    ani_ref mapRef = nullptr;
    for (const auto& [key, runMetrics] : map) {
        ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RUNMETRICS_I, ":V");
        if (runMetrics.textStyle != nullptr) {
            env->Object_SetPropertyByName_Ref(aniObj, "textStyle", ParseTextStyleToAni(env, *runMetrics.textStyle));
        }
        //env->Object_SetPropertyByName_Ref(aniObj, "fontMetrics", AniConvertFontMetrics(env, runMetrics.fontMetrics));
        if (ANI_OK
            != env->Object_CallMethodByName_Ref(
                mapAniObj, "set", "Lstd/core/Object;Lstd/core/Object;:Lescompat/Map;", &mapRef,
                AniTextUtils::CreateAniDoubleObj(env, static_cast<ani_double>(key)), aniObj)) {
            TEXT_LOGE("Object_CallMethodByName_Ref set failed");
            break;
        };
    }
    return mapAniObj;
}

ani_object AniCommon::ParseLineMetricsToAni(ani_env* env, const LineMetrics& lineMetrics)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_LINEMETRICS_I, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "startIndex", ani_int(lineMetrics.startIndex));
    env->Object_SetPropertyByName_Double(aniObj, "endIndex", ani_int(lineMetrics.endIndex));
    env->Object_SetPropertyByName_Double(aniObj, "ascent", ani_double(lineMetrics.ascender));
    env->Object_SetPropertyByName_Double(aniObj, "descent", ani_double(lineMetrics.descender));
    env->Object_SetPropertyByName_Double(aniObj, "height", ani_double(lineMetrics.height));
    env->Object_SetPropertyByName_Double(aniObj, "width", ani_double(lineMetrics.width));
    env->Object_SetPropertyByName_Double(aniObj, "left", ani_double(lineMetrics.x));
    env->Object_SetPropertyByName_Double(aniObj, "baseline", ani_double(lineMetrics.baseline));
    env->Object_SetPropertyByName_Double(aniObj, "lineNumber", ani_int(lineMetrics.lineNumber));
    env->Object_SetPropertyByName_Double(aniObj, "topHeight", ani_double(lineMetrics.y));
    env->Object_SetPropertyByName_Ref(aniObj, "runMetrics", ParseRunMetricsToAni(env, lineMetrics.runMetrics));
    return aniObj;
}

ani_object AniCommon::ParseTextShadowToAni(ani_env* env, const TextShadow& textShadow)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXTSHADOW_I, ":V");
    // env->Object_SetPropertyByName_Double(aniObj, "color", ani_int(textShadow.color));
    // env->Object_SetPropertyByName_Double(aniObj, "point", ani_int(textShadow.point));
    env->Object_SetPropertyByName_Double(aniObj, "blurRadius", ani_double(textShadow.blurRadius));
    return aniObj;
}

ani_object AniCommon::ParseDecorationToAni(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_DECORATION_I, ":V");
    // env->Object_SetPropertyByName_Double(aniObj, "color", ani_int(textStyle.decorationColor));
    env->Object_SetPropertyByName_Ref(
        aniObj, "textDecoration",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_DECORATION_TYPE, static_cast<int>(textStyle.decoration)));
    env->Object_SetPropertyByName_Ref(
        aniObj, "decorationStyle",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_DECORATION_STYLE, static_cast<int>(textStyle.decorationStyle)));
    env->Object_SetPropertyByName_Double(aniObj, "blurRadius", textStyle.decorationThicknessScale);
    return aniObj;
}

ani_object AniCommon::ParseRectStyleToAni(ani_env* env, const RectStyle& rectStyle)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RECT_STYLE_I, ":V");
    // env->Object_SetPropertyByName_Double(aniObj, "color", ani_int(textStyle.color));
    env->Object_SetPropertyByName_Double(aniObj, "leftTopRadius", rectStyle.leftTopRadius);
    env->Object_SetPropertyByName_Double(aniObj, "rightTopRadius", rectStyle.rightTopRadius);
    env->Object_SetPropertyByName_Double(aniObj, "rightBottomRadius", rectStyle.rightBottomRadius);
    env->Object_SetPropertyByName_Double(aniObj, "leftBottomRadius", rectStyle.leftBottomRadius);
    return aniObj;
}

ani_object AniCommon::ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures)
{
    const std::vector<std::pair<std::string, int>>& featureSet = fontFeatures.GetFontFeatures();
    ani_object arrayObj = AniTextUtils::CreateAniArrayAndInitData(
        env, featureSet, featureSet.size(), [](ani_env* env, const std::pair<std::string, int>& feature) {
            ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_FEATURE_I, ":V");
            env->Object_SetPropertyByName_Ref(aniObj, "name", AniTextUtils::CreateAniStringObj(env, feature.first));
            env->Object_SetPropertyByName_Double(aniObj, "value", feature.second);
            return aniObj;
        });
    return arrayObj;
}

ani_object AniCommon::ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_VARIATION_I, ":V");
    return aniObj;
}
} // namespace OHOS::Text::NAI