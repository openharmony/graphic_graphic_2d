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
#include "ani_drawing_converter.h"
#include "ani_text_style_converter.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
ani_status AniTextStyleConverter::ParseTextStyleToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_INTERFACE_TEXT_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return ret;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("Object mismatch, ret %{public}d", ret);
        return ret;
    }

    ParseDecorationToNative(env, obj, textStyle);
    AniDrawingConverter::ParseDrawingColorToNative(env, obj, "color", textStyle.color);

    AniTextUtils::ReadOptionalEnumField(env, obj, "fontWeight", textStyle.fontWeight);
    AniTextUtils::ReadOptionalEnumField(env, obj, "fontStyle", textStyle.fontStyle);
    if (textStyle.fontStyle == FontStyle::OBLIQUE) {
        textStyle.fontStyle = FontStyle::ITALIC;
    }
    AniTextUtils::ReadOptionalEnumField(env, obj, "baseline", textStyle.baseline);

    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontFamilies", textStyle.fontFamilies, [](ani_env* env, ani_ref ref) {
            std::string utf8Str;
            AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), utf8Str);
            return utf8Str;
        });
    AniTextUtils::ReadOptionalDoubleField(env, obj, "fontSize", textStyle.fontSize);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "letterSpacing", textStyle.letterSpacing);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "wordSpacing", textStyle.wordSpacing);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "heightScale", textStyle.heightScale);
    AniTextUtils::ReadOptionalBoolField(env, obj, "halfLeading", textStyle.halfLeading);
    AniTextUtils::ReadOptionalBoolField(env, obj, "heightOnly", textStyle.heightOnly);
    AniTextUtils::ReadOptionalU16StringField(env, obj, "ellipsis", textStyle.ellipsis);
    AniTextUtils::ReadOptionalEnumField(env, obj, "ellipsisMode", textStyle.ellipsisModal);
    AniTextUtils::ReadOptionalStringField(env, obj, "locale", textStyle.locale);
    AniTextUtils::ReadOptionalDoubleField(env, obj, "baselineShift", textStyle.baseLineShift);
    ParseTextShadowToNative(env, obj, textStyle.shadows);
    ParseFontFeatureToNative(env, obj, textStyle.fontFeatures);
    ParseFontVariationToNative(env, obj, textStyle.fontVariations);

    ani_ref backgroundRectRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "backgroundRect", backgroundRectRef) == ANI_OK
        && backgroundRectRef != nullptr) {
        ParseRectStyleToNative(env, reinterpret_cast<ani_object>(backgroundRectRef), textStyle.backgroundRect);
    }

    return ANI_OK;
}

void AniTextStyleConverter::ParseDecorationToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ani_ref decorationRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "decoration", decorationRef) == ANI_OK && decorationRef != nullptr) {
        AniTextUtils::ReadOptionalEnumField(
            env, reinterpret_cast<ani_object>(decorationRef), "textDecoration", textStyle.decoration);
        AniTextUtils::ReadOptionalEnumField(
            env, reinterpret_cast<ani_object>(decorationRef), "decorationStyle", textStyle.decorationStyle);
        AniTextUtils::ReadOptionalDoubleField(env, reinterpret_cast<ani_object>(decorationRef),
            "decorationThicknessScale", textStyle.decorationThicknessScale);
        AniDrawingConverter::ParseDrawingColorToNative(
            env, reinterpret_cast<ani_object>(decorationRef), "color", textStyle.decorationColor);
    }
}

inline void GetPointXFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue{0};
    ani_status ret = env->Object_GetPropertyByName_Double(argValue, "x", &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param x is invalid, ret %{public}d", ret);
        return;
    }
    point.SetX(objValue);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue{0};
    ani_status ret = env->Object_GetPropertyByName_Double(argValue, "y", &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param y is invalid, ret %{public}d", ret);
        return;
    }
    point.SetY(objValue);
}

inline void GetTextShadowPoint(ani_env* env, ani_object obj, Drawing::Point& point)
{
    GetPointXFromJsBumber(env, obj, point);
    GetPointYFromJsBumber(env, obj, point);
}

void AniTextStyleConverter::ParseTextShadowToNative(ani_env* env, ani_object obj, std::vector<TextShadow>& textShadow)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "textShadows", array, [&textShadow](ani_env* env, ani_ref ref) {
            ani_object shadowObj = reinterpret_cast<ani_object>(ref);
            ani_class cls = nullptr;
            ani_status ret = env->FindClass(ANI_INTERFACE_TEXTSHADOW, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to find class, ret %{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(shadowObj, cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("Object mismatch, ret %{public}d", ret);
                return "";
            }

            double runTimeRadius;
            AniTextUtils::ReadOptionalDoubleField(env, shadowObj, "blurRadius", runTimeRadius);

            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            AniDrawingConverter::ParseDrawingColorToNative(env, shadowObj, "color", colorSrc);

            Drawing::Point offset(0, 0);
            ani_ref pointValue = nullptr;
            ret = AniTextUtils::ReadOptionalField(env, shadowObj, "point", pointValue);
            if (ret == ANI_OK && pointValue != nullptr) {
                GetTextShadowPoint(env, reinterpret_cast<ani_object>(pointValue), offset);
            }

            textShadow.emplace_back(TextShadow(colorSrc, offset, runTimeRadius));
            return "";
        });
}

void AniTextStyleConverter::ParseFontFeatureToNative(ani_env* env, ani_object obj, FontFeatures& fontFeatures)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontFeatures", array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_class cls = nullptr;
            ani_status ret = env->FindClass(ANI_INTERFACE_FONT_FEATURE, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to find class, ret %{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(obj, cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("Object mismatch, ret %{public}d", ret);
                return "";
            }
            ani_ref nameRef = nullptr;
            ret = env->Object_GetPropertyByName_Ref(obj, "name", &nameRef);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to get name, ret %{public}d", ret);
                return "";
            }
            std::string name;
            ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(nameRef), name);
            if (ret != ANI_OK) {
                return "";
            }

            ani_double valueDouble;
            ret = env->Object_GetPropertyByName_Double(obj, "value", &valueDouble);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to get value, ret %{public}d", ret);
                return "";
            }
            fontFeatures.SetFeature(name, static_cast<int>(valueDouble));
            return "";
        });
}

void AniTextStyleConverter::ParseFontVariationToNative(ani_env* env, ani_object obj, FontVariations& fontVariations)
{
    std::vector<std::string> array;
    AniTextUtils::ReadOptionalArrayField<std::string>(
        env, obj, "fontVariations", array, [&fontVariations](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_class cls = nullptr;
            ani_status ret = env->FindClass(ANI_INTERFACE_FONT_VARIATION, &cls);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to find class, ret %{public}d", ret);
                return "";
            }
            ani_boolean isObj = false;
            ret = env->Object_InstanceOf(obj, cls, &isObj);
            if (!isObj) {
                TEXT_LOGE("Object mismatch, ret %{public}d", ret);
                return "";
            }
            ani_ref axisRef = nullptr;
            ret = env->Object_GetPropertyByName_Ref(obj, "axis", &axisRef);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to get filed axis, ret %{public}d", ret);
                return "";
            }
            std::string axis;
            ret = AniTextUtils::AniToStdStringUtf8(env, static_cast<ani_string>(axisRef), axis);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to parse string filed axis, ret %{public}d", ret);
                return "";
            }
            ani_double valueDouble;
            ret = env->Object_GetPropertyByName_Double(obj, "value", &valueDouble);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to get filed value, ret %{public}d", ret);
                return "";
            }
            fontVariations.SetAxisValue(axis, static_cast<int>(valueDouble));
            return "";
        });
}

void AniTextStyleConverter::ParseRectStyleToNative(ani_env* env, ani_object obj, RectStyle& rectStyle)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_INTERFACE_RECT_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find class, ret %{public}d", ret);
        return;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("Object mismatch, ret %{public}d", ret);
        return;
    }
    env->Object_GetPropertyByName_Double(obj, "leftTopRadius", &rectStyle.leftTopRadius);
    env->Object_GetPropertyByName_Double(obj, "rightTopRadius", &rectStyle.rightTopRadius);
    env->Object_GetPropertyByName_Double(obj, "rightBottomRadius", &rectStyle.rightBottomRadius);
    env->Object_GetPropertyByName_Double(obj, "leftBottomRadius", &rectStyle.leftBottomRadius);
}

ani_object AniTextStyleConverter::ParseTextStyleToAni(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXT_STYLE, ":V");
    env->Object_SetPropertyByName_Ref(
        aniObj, "decoration", AniTextStyleConverter::ParseDecorationToAni(env, textStyle));
    env->Object_SetPropertyByName_Ref(aniObj, "fontWeight",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_FONT_WEIGHT, static_cast<int>(textStyle.fontWeight)));
    env->Object_SetPropertyByName_Ref(aniObj, "fontStyle",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_FONT_STYLE, static_cast<int>(textStyle.fontStyle)));
    env->Object_SetPropertyByName_Ref(aniObj, "baseline",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_BASELINE, static_cast<int>(textStyle.baseline)));
    ani_object fontFamiliesAniObj =
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.fontFamilies, textStyle.fontFamilies.size(),
            [](ani_env* env, const std::string& item) { return AniTextUtils::CreateAniStringObj(env, item); });
    env->Object_SetPropertyByName_Ref(aniObj, "fontFamilies", fontFamiliesAniObj);
    env->Object_SetPropertyByName_Ref(aniObj, "fontSize", AniTextUtils::CreateAniDoubleObj(env, textStyle.fontSize));
    env->Object_SetPropertyByName_Ref(
        aniObj, "letterSpacing", AniTextUtils::CreateAniDoubleObj(env, textStyle.letterSpacing));
    env->Object_SetPropertyByName_Ref(
        aniObj, "wordSpacing", AniTextUtils::CreateAniDoubleObj(env, textStyle.wordSpacing));
    env->Object_SetPropertyByName_Ref(
        aniObj, "heightScale", AniTextUtils::CreateAniDoubleObj(env, textStyle.heightScale));
    env->Object_SetPropertyByName_Ref(
        aniObj, "halfLeading", AniTextUtils::CreateAniBooleanObj(env, textStyle.halfLeading));
    env->Object_SetPropertyByName_Ref(
        aniObj, "heightOnly", AniTextUtils::CreateAniBooleanObj(env, textStyle.heightOnly));
    env->Object_SetPropertyByName_Ref(aniObj, "ellipsis", AniTextUtils::CreateAniStringObj(env, textStyle.ellipsis));
    env->Object_SetPropertyByName_Ref(aniObj, "ellipsisMode",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_ELLIPSIS_MODE, static_cast<int>(textStyle.ellipsisModal)));
    env->Object_SetPropertyByName_Ref(aniObj, "locale", AniTextUtils::CreateAniStringObj(env, textStyle.locale));
    env->Object_SetPropertyByName_Ref(
        aniObj, "baselineShift", AniTextUtils::CreateAniDoubleObj(env, textStyle.baseLineShift));
    env->Object_SetPropertyByName_Ref(
        aniObj, "backgroundRect", AniTextStyleConverter::ParseRectStyleToAni(env, textStyle.backgroundRect));
    ani_object shadowsAniObj = AniTextUtils::CreateAniArrayAndInitData(env, textStyle.shadows, textStyle.shadows.size(),
        [](ani_env* env, const TextShadow& item) { return AniTextStyleConverter::ParseTextShadowToAni(env, item); });
    env->Object_SetPropertyByName_Ref(aniObj, "textShadows", shadowsAniObj);
    env->Object_SetPropertyByName_Ref(aniObj, "fontFeatures", ParseFontFeaturesToAni(env, textStyle.fontFeatures));
    return aniObj;
}

ani_object AniTextStyleConverter::ParseTextShadowToAni(ani_env* env, const TextShadow& textShadow)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXTSHADOW, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "blurRadius", ani_double(textShadow.blurRadius));
    return aniObj;
}

ani_object AniTextStyleConverter::ParseDecorationToAni(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_DECORATION, ":V");
    env->Object_SetPropertyByName_Ref(aniObj, "textDecoration",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_DECORATION_TYPE, static_cast<int>(textStyle.decoration)));
    env->Object_SetPropertyByName_Ref(aniObj, "decorationStyle",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_DECORATION_STYLE, static_cast<int>(textStyle.decorationStyle)));
    env->Object_SetPropertyByName_Double(aniObj, "blurRadius", textStyle.decorationThicknessScale);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseRectStyleToAni(ani_env* env, const RectStyle& rectStyle)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RECT_STYLE, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "leftTopRadius", rectStyle.leftTopRadius);
    env->Object_SetPropertyByName_Double(aniObj, "rightTopRadius", rectStyle.rightTopRadius);
    env->Object_SetPropertyByName_Double(aniObj, "rightBottomRadius", rectStyle.rightBottomRadius);
    env->Object_SetPropertyByName_Double(aniObj, "leftBottomRadius", rectStyle.leftBottomRadius);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures)
{
    const std::vector<std::pair<std::string, int>> featureSet = fontFeatures.GetFontFeatures();
    ani_object arrayObj = AniTextUtils::CreateAniArrayAndInitData(
        env, featureSet, featureSet.size(), [](ani_env* env, const std::pair<std::string, int>& feature) {
            ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_FEATURE, ":V");
            env->Object_SetPropertyByName_Ref(aniObj, "name", AniTextUtils::CreateAniStringObj(env, feature.first));
            env->Object_SetPropertyByName_Double(aniObj, "value", feature.second);
            return aniObj;
        });
    return arrayObj;
}

ani_object AniTextStyleConverter::ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_VARIATION, ":V");
    return aniObj;
}
} // namespace OHOS::Text::ANI