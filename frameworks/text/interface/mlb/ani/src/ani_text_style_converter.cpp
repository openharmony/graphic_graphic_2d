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

#include "ani_text_style_converter.h"

#include "ani_common.h"
#include "ani_drawing_utils.h"
#include "ani_text_utils.h"
#include "draw/color.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
ani_status ParseDrawingColorToNative(
    ani_env* env, ani_object obj, bool readOptional, const AniCacheParam& param, Drawing::Color& colorSrc)
{
    ani_ref colorRef = nullptr;
    ani_status result = ANI_ERROR;
    if (readOptional) { // true: read optional field (eg: param?: string)
        result = AniTextUtils::ReadOptionalField(env, obj, param, colorRef);
    } else {
        result = AniTextUtils::GetPropertyByCache_Ref(env, obj, param, colorRef);
    }
    if (result != ANI_OK || colorRef == nullptr) {
        TEXT_LOGD("Failed to find param color, ret %{public}d", result);
        return result;
    }
    Drawing::ColorQuad color;
    if (OHOS::Rosen::Drawing::GetColorQuadFromColorObj(env, reinterpret_cast<ani_object>(colorRef), color)) {
        colorSrc = Drawing::Color(color);
    }
    return ANI_OK;
}
} // namespace

ani_status AniTextStyleConverter::TransTextStyleToRequired(ani_env* env, ani_object obj, ani_object& objR)
{
    ani_namespace ns = nullptr;
    ani_status ret = AniTextUtils::FindNamespaceWithCache(env, ANI_NAMESPACE_TEXT, ns);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find namespace %{public}s, ret %{public}d", ANI_NAMESPACE_TEXT, ret);
        return ret;
    }
    ani_function fc = nullptr;
    static AniCacheParam param = { ANI_NAMESPACE_TEXT,
        "transToRequired", "C{@ohos.graphics.text.text.TextStyle}:C{@ohos.graphics.text.text.TextStyleR}" };
    ret = AniTextUtils::FindFunctionWithCache(env, param, ns, fc);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_ref result = nullptr;
    ret = env->Function_Call_Ref(fc, &result, obj);
    if (ret != ANI_OK || result == nullptr) {
        TEXT_LOGE("Failed to call method, ret %{public}d", ret);
    } else {
        objR = static_cast<ani_object>(result);
    }
    return ret;
}

ani_status AniTextStyleConverter::ParseTextStyleToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ani_object objR;
    ani_status status = TransTextStyleToRequired(env, obj, objR);
    if (status != ANI_OK) {
        return status;
    }
    ParseDecorationToNative(env, objR, textStyle);
    static AniCacheParam paramColor =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR) };
    ParseDrawingColorToNative(env, objR, false, paramColor, textStyle.color);

    static AniCacheParam fontWeightParam =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT) };
    AniTextUtils::ReadEnumField(env, objR, fontWeightParam, textStyle.fontWeight);
    static AniCacheParam fontStyleParam =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE) };
    AniTextUtils::ReadEnumField(env, objR, fontStyleParam, textStyle.fontStyle);
    if (textStyle.fontStyle == FontStyle::OBLIQUE) {
        textStyle.fontStyle = FontStyle::ITALIC;
    }
    static AniCacheParam baselineParam =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE) };
    AniTextUtils::ReadEnumField(env, objR, baselineParam, textStyle.baseline);

    static AniCacheParam fontFamiliesParams =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY) };
    AniTextUtils::ReadArrayField<std::string>(
        env, objR, fontFamiliesParams, textStyle.fontFamilies, [](ani_env* env, ani_ref ref) {
            std::string utf8Str;
            AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), utf8Str);
            return utf8Str;
        });
    static AniCacheParam fontSizeParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>fontSize", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, objR, fontSizeParam, textStyle.fontSize);
    static AniCacheParam letterSpacingParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>letterSpacing", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, objR, letterSpacingParam, textStyle.letterSpacing);
    static AniCacheParam wordSpacingParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>wordSpacing", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, objR, wordSpacingParam, textStyle.wordSpacing);
    static AniCacheParam heightScaleParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>heightScale", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, objR, heightScaleParam, textStyle.heightScale);
    textStyle.heightScale = textStyle.heightScale < 0 ? 0 : textStyle.heightScale;
    static AniCacheParam halfLeadingParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>halfLeading", ":z" };
    AniTextUtils::GetPropertyByCache_Bool(env, objR, halfLeadingParam, textStyle.halfLeading);
    static AniCacheParam heightOnlyParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>heightOnly", ":z" };
    AniTextUtils::GetPropertyByCache_Bool(env, objR, heightOnlyParam, textStyle.heightOnly);
    static AniCacheParam ellipsisParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsis", ANI_WRAP_RETURN_C(ANI_STRING) };
    AniTextUtils::GetPropertyByCache_U16String(env, objR, ellipsisParam, textStyle.ellipsis);
    static AniCacheParam ellipsisModeParam =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsisMode", ANI_WRAP_RETURN_E(ANI_ENUM_ELLIPSIS_MODE) };
    AniTextUtils::ReadEnumField(env, objR, ellipsisModeParam, textStyle.ellipsisModal);
    static AniCacheParam localeParam = { ANI_INTERFACE_TEXT_STYLE_R, "<get>locale", ANI_WRAP_RETURN_C(ANI_STRING) };
    AniTextUtils::GetPropertyByCache_String(env, objR, localeParam, textStyle.locale);
    static AniCacheParam baselineShiftParam =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>baselineShift", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, objR, baselineShiftParam, textStyle.baseLineShift);
    ParseTextShadowToNative(env, objR, textStyle.shadows);
    ParseFontFeatureToNative(env, objR, textStyle.fontFeatures);
    ParseFontVariationToNative(env, objR, textStyle.fontVariations);

    ani_ref backgroundRectRef = nullptr;
    static AniCacheParam backgroundRectParam =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>backgroundRect", ANI_WRAP_RETURN_C(ANI_INTERFACE_RECT_STYLE) };
    if (AniTextUtils::GetPropertyByCache_Ref(env, objR, backgroundRectParam, backgroundRectRef) == ANI_OK
        && backgroundRectRef != nullptr) {
        ParseRectStyleToNative(env, reinterpret_cast<ani_object>(backgroundRectRef), textStyle.backgroundRect);
    }

    return ANI_OK;
}

void AniTextStyleConverter::ParseDecorationToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ani_ref decorationRef = nullptr;
    static AniCacheParam decorationParam =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>decoration", ANI_WRAP_RETURN_C(ANI_INTERFACE_DECORATION) };
    if (AniTextUtils::GetPropertyByCache_Ref(env, obj, decorationParam, decorationRef) == ANI_OK &&
        decorationRef != nullptr) {
        static AniCacheParam textDecorationParam =
            { ANI_INTERFACE_DECORATION, "<get>textDecoration", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_TYPE) };
        AniTextUtils::ReadOptionalEnumField(
            env, reinterpret_cast<ani_object>(decorationRef), textDecorationParam, textStyle.decoration);
        static AniCacheParam decorationStyleParam =
            { ANI_INTERFACE_DECORATION, "<get>decorationStyle", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_STYLE) };
        AniTextUtils::ReadOptionalEnumField(
            env, reinterpret_cast<ani_object>(decorationRef), decorationStyleParam, textStyle.decorationStyle);
        static AniCacheParam decorationThicknessScaleParam =
            { ANI_INTERFACE_DECORATION, "<get>decorationThicknessScale", ANI_WRAP_RETURN_C(ANI_DOUBLE) };
        AniTextUtils::ReadOptionalDoubleField(env, reinterpret_cast<ani_object>(decorationRef),
            decorationThicknessScaleParam, textStyle.decorationThicknessScale);
        static AniCacheParam paramColor =
            { ANI_INTERFACE_DECORATION, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR) };
        ParseDrawingColorToNative(
            env, reinterpret_cast<ani_object>(decorationRef), true, paramColor, textStyle.decorationColor);
    }
}

inline void GetPointXFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    static AniCacheParam paramX = { ANI_INTERFACE_POINT, "<get>x", ":d" };
    ani_status ret = AniTextUtils::GetPropertyByCache_Double(env, argValue, paramX, objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param x is invalid, ret %{public}d", ret);
        return;
    }
    point.SetX(objValue);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    static AniCacheParam paramY = { ANI_INTERFACE_POINT, "<get>y", ":d" };
    ani_status ret = AniTextUtils::GetPropertyByCache_Double(env, argValue, paramY, objValue);
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
    static AniCacheParam params = { ANI_INTERFACE_TEXT_STYLE_R, "<get>textShadows", ANI_WRAP_RETURN_C(ANI_ARRAY) };
    AniTextUtils::ReadArrayField<std::string>(
        env, obj, params, array, [&textShadow](ani_env* env, ani_ref ref) {
            ani_object shadowObj = reinterpret_cast<ani_object>(ref);
            double runTimeRadius;
            static AniCacheParam blurRadiusParam =
                { ANI_INTERFACE_TEXTSHADOW, "<get>blurRadius", ANI_WRAP_RETURN_C(ANI_DOUBLE) };
            AniTextUtils::ReadOptionalDoubleField(env, shadowObj, blurRadiusParam, runTimeRadius);

            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            static AniCacheParam paramColor =
                { ANI_INTERFACE_TEXTSHADOW, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR) };
            ParseDrawingColorToNative(env, shadowObj, true, paramColor, colorSrc);

            Drawing::Point offset(0, 0);
            ani_ref pointValue = nullptr;
            static AniCacheParam pointParam =
                { ANI_INTERFACE_TEXTSHADOW, "<get>point", ANI_WRAP_RETURN_C(ANI_INTERFACE_POINT) };
            ani_status ret = AniTextUtils::ReadOptionalField(env, shadowObj, pointParam, pointValue);
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
    static AniCacheParam params =
        { ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFeatures", ANI_WRAP_RETURN_C(ANI_ARRAY) };
    AniTextUtils::ReadArrayField<std::string>(
        env, obj, params, array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref nameRef = nullptr;
            static AniCacheParam paramName =
                { ANI_INTERFACE_FONT_FEATURE, "<get>name", ANI_WRAP_RETURN_C(ANI_STRING) };
            ani_status ret = AniTextUtils::GetPropertyByCache_Ref(env, obj, paramName, nameRef);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to get name, ret %{public}d", ret);
                return "";
            }
            std::string name;
            ret = AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(nameRef), name);
            if (ret != ANI_OK) {
                return "";
            }

            ani_int valueInt;
            static AniCacheParam paramValue = { ANI_INTERFACE_FONT_FEATURE, "<get>value", ":i" };
            AniTextUtils::GetPropertyByCache_Int(env, obj, paramValue, valueInt);
            if (ret != ANI_OK) {
                TEXT_LOGE("Failed to get value, ret %{public}d", ret);
                return "";
            }
            fontFeatures.SetFeature(name, static_cast<int>(valueInt));
            return "";
        });
}

void AniTextStyleConverter::ParseFontVariationToNative(ani_env* env, ani_object obj, FontVariations& fontVariations)
{
    std::vector<std::string> array;
    static AniCacheParam params = { ANI_INTERFACE_TEXT_STYLE_R, "<get>fontVariations", ANI_WRAP_RETURN_C(ANI_ARRAY) };
    AniTextUtils::ReadArrayField<std::string>(
        env, obj, params, array, [&fontVariations](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref axisRef = nullptr;
            static AniCacheParam paramAxis =
                { ANI_INTERFACE_FONT_VARIATION, "<get>axis", ANI_WRAP_RETURN_C(ANI_STRING) };
            ani_status ret = AniTextUtils::GetPropertyByCache_Ref(env, obj, paramAxis, axisRef);
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
            static AniCacheParam paramValue = { ANI_INTERFACE_FONT_VARIATION, "<get>value", ":d" };
            ret = AniTextUtils::GetPropertyByCache_Double(env, obj, paramValue, valueDouble);
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
    Drawing::Color color;
    static AniCacheParam paramColor =
        { ANI_INTERFACE_RECT_STYLE, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR) };
    if (ParseDrawingColorToNative(env, obj, false, paramColor, color) == ANI_OK) {
        rectStyle.color = color.CastToColorQuad();
    }
    static AniCacheParam paramLeftTopRadius = { ANI_INTERFACE_RECT_STYLE, "<get>leftTopRadius", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, obj, paramLeftTopRadius, rectStyle.leftTopRadius);
    static AniCacheParam paramRightTopRadius = { ANI_INTERFACE_RECT_STYLE, "<get>rightTopRadius", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, obj, paramRightTopRadius, rectStyle.rightTopRadius);
    static AniCacheParam paramRightBottomRadius = { ANI_INTERFACE_RECT_STYLE, "<get>rightBottomRadius", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, obj, paramRightBottomRadius, rectStyle.rightBottomRadius);
    static AniCacheParam paramLeftBottomRadius = { ANI_INTERFACE_RECT_STYLE, "<get>leftBottomRadius", ":d" };
    AniTextUtils::GetPropertyByCache_Double(env, obj, paramLeftBottomRadius, rectStyle.leftBottomRadius);
}

ani_object AniTextStyleConverter::ParseTextStyleToAni(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniColorObj = nullptr;
    ani_status status = OHOS::Rosen::Drawing::CreateColorObj(env, textStyle.color, aniColorObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to parse color, ret %{public}d", status);
        aniColorObj = AniTextUtils::CreateAniUndefined(env);
    }

    static std::string sign = "C{" + std::string(ANI_INTERFACE_DECORATION) + "}C{" +
        std::string(ANI_INTERFACE_COLOR) + "}E{" + std::string(ANI_ENUM_FONT_WEIGHT) + "}E{" +
        std::string(ANI_ENUM_FONT_STYLE) + "}E{" + std::string(ANI_ENUM_TEXT_BASELINE) + "}C{" +
        std::string(ANI_ARRAY) + "}ddddzzC{" + std::string(ANI_STRING) + "}E{" +
        std::string(ANI_ENUM_ELLIPSIS_MODE) + "}C{" + std::string(ANI_STRING) +
        "}dC{" + std::string(ANI_ARRAY) + "}C{" +
        std::string(ANI_ARRAY) + "}C{" + std::string(ANI_INTERFACE_RECT_STYLE) + "}:";

    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXT_STYLE, sign.c_str(),
        AniTextStyleConverter::ParseDecorationToAni(env, textStyle),
        aniColorObj,
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_FONT_WEIGHT, static_cast<int>(textStyle.fontWeight)),
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_FONT_STYLE, static_cast<int>(textStyle.fontStyle)),
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_BASELINE, static_cast<int>(textStyle.baseline)),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.fontFamilies, textStyle.fontFamilies.size(),
            [](ani_env* env, const std::string& item) { return AniTextUtils::CreateAniStringObj(env, item); }),
        textStyle.fontSize,
        textStyle.letterSpacing,
        textStyle.wordSpacing,
        textStyle.heightScale,
        textStyle.halfLeading,
        textStyle.heightOnly,
        AniTextUtils::CreateAniStringObj(env, textStyle.ellipsis),
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_ELLIPSIS_MODE, static_cast<int>(textStyle.ellipsisModal)),
        AniTextUtils::CreateAniStringObj(env, textStyle.locale),
        textStyle.baseLineShift,
        ParseFontFeaturesToAni(env, textStyle.fontFeatures),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.shadows, textStyle.shadows.size(),
            [](ani_env* env, const TextShadow& item) {
                return AniTextStyleConverter::ParseTextShadowToAni(env, item);
            }),
        AniTextStyleConverter::ParseRectStyleToAni(env, textStyle.backgroundRect)
    );
    return aniObj;
}

ani_object AniTextStyleConverter::ParseTextShadowToAni(ani_env* env, const TextShadow& textShadow)
{
    ani_object aniColorObj = nullptr;
    ani_status status = OHOS::Rosen::Drawing::CreateColorObj(env, textShadow.color, aniColorObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to parse color, ret %{public}d", status);
        aniColorObj = AniTextUtils::CreateAniUndefined(env);
    }

    ani_object aniPointObj = nullptr;
    status = OHOS::Rosen::Drawing::CreatePointObj(env, textShadow.offset, aniPointObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to parse point, ret %{public}d", status);
        aniPointObj = AniTextUtils::CreateAniUndefined(env);
    }

    static std::string sign =
        "C{" + std::string(ANI_INTERFACE_COLOR) + "}C{" + std::string(ANI_INTERFACE_POINT) +
        "}d:";

    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_TEXTSHADOW, sign.c_str(),
        aniColorObj,
        aniPointObj,
        ani_double(textShadow.blurRadius)
    );
    return aniObj;
}

ani_object AniTextStyleConverter::ParseDecorationToAni(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniColorObj = nullptr;
    ani_status status = OHOS::Rosen::Drawing::CreateColorObj(env, textStyle.decorationColor, aniColorObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to parse color, ret %{public}d", status);
        aniColorObj = AniTextUtils::CreateAniUndefined(env);
    }

    static std::string sign = "E{" + std::string(ANI_ENUM_TEXT_DECORATION_TYPE) + "}C{" +
        std::string(ANI_INTERFACE_COLOR) + "}E{" + std::string(ANI_ENUM_TEXT_DECORATION_STYLE) + "}d:";

    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_DECORATION, sign.c_str(),
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_DECORATION_TYPE, static_cast<int>(textStyle.decoration)),
        aniColorObj,
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_TEXT_DECORATION_STYLE, static_cast<int>(textStyle.decorationStyle)),
        textStyle.decorationThicknessScale);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseRectStyleToAni(ani_env* env, const RectStyle& rectStyle)
{
    OHOS::Rosen::Drawing::Color color = OHOS::Rosen::Drawing::Color(rectStyle.color);
    ani_object aniColorObj = nullptr;
    ani_status status = OHOS::Rosen::Drawing::CreateColorObj(env, color, aniColorObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to parse color, ret %{public}d", status);
        aniColorObj = AniTextUtils::CreateAniUndefined(env);
    }
    static std::string sign = "C{" + std::string(ANI_INTERFACE_COLOR) + "}dddd:";
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_RECT_STYLE, sign.c_str(),
        aniColorObj,
        rectStyle.leftTopRadius,
        rectStyle.rightTopRadius,
        rectStyle.rightBottomRadius,
        rectStyle.leftBottomRadius);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures)
{
    const std::vector<std::pair<std::string, int>> featureSet = fontFeatures.GetFontFeatures();
    ani_object arrayObj = AniTextUtils::CreateAniArrayAndInitData(
        env, featureSet, featureSet.size(), [](ani_env* env, const std::pair<std::string, int>& feature) {
            static std::string sign = "C{" + std::string(ANI_STRING) + "}i:";
            ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_FEATURE, sign.c_str(),
                AniTextUtils::CreateAniStringObj(env, feature.first),
                ani_int(feature.second));
            return aniObj;
        });
    return arrayObj;
}

ani_object AniTextStyleConverter::ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_FONT_VARIATION, ":");
    return aniObj;
}
} // namespace OHOS::Text::ANI