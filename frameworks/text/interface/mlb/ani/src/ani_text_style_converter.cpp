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
    ani_env* env, ani_object obj, bool readOptional, const ani_method getPropertyMethod, Drawing::Color& colorSrc)
{
    ani_ref colorRef = nullptr;
    ani_status result = ANI_ERROR;
    if (readOptional) { // true: read optional field (eg: param?: string)
        result = AniTextUtils::ReadOptionalField(env, obj, getPropertyMethod, colorRef);
    } else {
        result = env->Object_CallMethod_Ref(obj, getPropertyMethod, &colorRef);
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
    ani_namespace ns = AniFindNamespace(env, ANI_NAMESPACE_TEXT);
    if (ns == nullptr) {
        TEXT_LOGE("Failed to find namespace: %{public}s", ANI_NAMESPACE_TEXT);
        return ANI_NOT_FOUND;
    }
    ani_function fc = AniNamespaceFindFunction(env, ANI_NAMESPACE_TEXT, "transToRequired",
        "C{@ohos.graphics.text.text.TextStyle}:C{@ohos.graphics.text.text.TextStyleR}");
    if (fc == nullptr) {
        TEXT_LOGE("Failed to find function: transToRequired");
        return ANI_NOT_FOUND;
    }
    ani_ref result = nullptr;
    ani_status ret = env->Function_Call_Ref(fc, &result, obj);
    if (ret != ANI_OK || result == nullptr) {
        TEXT_LOGE("Failed to call method, ret %{public}d", ret);
    } else {
        objR = static_cast<ani_object>(result);
    }
    return ret;
}

ani_status AniTextStyleConverter::ParseTextStyleToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ani_object objR = nullptr;
    ani_status status = TransTextStyleToRequired(env, obj, objR);
    if (status != ANI_OK) {
        return status;
    }
    ParseDecorationToNative(env, objR, textStyle);

    ParseDrawingColorToNative(env, objR, false,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)),
        textStyle.color);

    AniTextUtils::ReadEnumField(env, objR, AniTextEnum::ENUM_FONT_WEIGHT,
        AniClassFindMethod(
            env, ANI_INTERFACE_TEXT_STYLE_R, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT)),
        textStyle.fontWeight);
    AniTextUtils::ReadEnumField(env, objR, AniTextEnum::ENUM_FONT_STYLE,
        AniClassFindMethod(
            env, ANI_INTERFACE_TEXT_STYLE_R, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE)),
        textStyle.fontStyle);
    if (textStyle.fontStyle == FontStyle::OBLIQUE) {
        textStyle.fontStyle = FontStyle::ITALIC;
    }
    AniTextUtils::ReadEnumField(env, objR, AniTextEnum::ENUM_TEXT_BASE_LINE,
        AniClassFindMethod(
            env, ANI_INTERFACE_TEXT_STYLE_R, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE)),
        textStyle.baseline);

    AniTextUtils::ReadArrayField<std::string>(env, objR,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY)),
        textStyle.fontFamilies, [](ani_env* env, ani_ref ref) {
            std::string utf8Str;
            AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), utf8Str);
            return utf8Str;
        });

    env->Object_CallMethod_Double(
        objR, AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>fontSize", ":d"), &textStyle.fontSize);
    env->Object_CallMethod_Double(objR,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>letterSpacing", ":d"), &textStyle.letterSpacing);
    env->Object_CallMethod_Double(
        objR, AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>wordSpacing", ":d"), &textStyle.wordSpacing);
    env->Object_CallMethod_Double(
        objR, AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>heightScale", ":d"), &textStyle.heightScale);
    textStyle.heightScale = textStyle.heightScale < 0 ? 0 : textStyle.heightScale;
    ani_boolean tempBool;
    status = env->Object_CallMethod_Boolean(
        objR, AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>halfLeading", ":z"), &tempBool);
    textStyle.halfLeading = (status == ANI_OK && tempBool);
    status = env->Object_CallMethod_Boolean(
        objR, AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>heightOnly", ":z"), &tempBool);
    textStyle.heightOnly = (status == ANI_OK && tempBool);
    AniTextUtils::GetPropertyByCache_U16String(env, objR,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsis", ANI_WRAP_RETURN_C(ANI_STRING)),
        textStyle.ellipsis);
    AniTextUtils::ReadEnumField(env, objR, AniTextEnum::ENUM_ELLIPSIS_MODAL,
        AniClassFindMethod(
            env, ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsisMode", ANI_WRAP_RETURN_E(ANI_ENUM_ELLIPSIS_MODE)),
        textStyle.ellipsisModal);
    AniTextUtils::GetPropertyByCache_String(env, objR,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>locale", ANI_WRAP_RETURN_C(ANI_STRING)),
        textStyle.locale);
    env->Object_CallMethod_Double(objR,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>baselineShift", ":d"), &textStyle.baseLineShift);
    ParseTextShadowToNative(env, objR, textStyle.shadows);
    ParseFontFeatureToNative(env, objR, textStyle.fontFeatures);
    ParseFontVariationToNative(env, objR, textStyle.fontVariations);

    ani_ref backgroundRectRef = nullptr;
    status = env->Object_CallMethod_Ref(objR,
        AniClassFindMethod(
            env, ANI_INTERFACE_TEXT_STYLE_R, "<get>backgroundRect", ANI_WRAP_RETURN_C(ANI_INTERFACE_RECT_STYLE)),
        &backgroundRectRef);
    if (status == ANI_OK && backgroundRectRef != nullptr) {
        ParseRectStyleToNative(env, reinterpret_cast<ani_object>(backgroundRectRef), textStyle.backgroundRect);
    }

    return ANI_OK;
}

void AniTextStyleConverter::ParseDecorationToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ani_ref decorationRef = nullptr;
    ani_status status = env->Object_CallMethod_Ref(obj,
        AniClassFindMethod(
            env, ANI_INTERFACE_TEXT_STYLE_R, "<get>decoration", ANI_WRAP_RETURN_C(ANI_INTERFACE_DECORATION)),
        &decorationRef);
    if (status == ANI_OK && decorationRef != nullptr) {
        AniTextUtils::ReadOptionalEnumField(env, reinterpret_cast<ani_object>(decorationRef),
            AniTextEnum::ENUM_TEXT_DECORATION,
            AniClassFindMethod(
                env, ANI_INTERFACE_DECORATION, "<get>textDecoration", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_TYPE)),
            textStyle.decoration);
        AniTextUtils::ReadOptionalEnumField(env, reinterpret_cast<ani_object>(decorationRef),
            AniTextEnum::ENUM_TEXT_DECORATION_STYLE,
            AniClassFindMethod(env, ANI_INTERFACE_DECORATION, "<get>decorationStyle",
                ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_STYLE)),
            textStyle.decorationStyle);
        AniTextUtils::ReadOptionalDoubleField(env, reinterpret_cast<ani_object>(decorationRef),
            AniClassFindMethod(
                env, ANI_INTERFACE_DECORATION, "<get>decorationThicknessScale", ANI_WRAP_RETURN_C(ANI_DOUBLE)),
            textStyle.decorationThicknessScale);
        ParseDrawingColorToNative(env, reinterpret_cast<ani_object>(decorationRef), true,
            AniClassFindMethod(env, ANI_INTERFACE_DECORATION, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)),
            textStyle.decorationColor);
    }
}

inline void GetPointXFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    ani_status ret = env->Object_CallMethod_Double(
        argValue, AniClassFindMethod(env, ANI_INTERFACE_POINT, "<get>x", ":d"), &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param x is invalid, ret %{public}d", ret);
        return;
    }
    point.SetX(objValue);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    ani_status ret = env->Object_CallMethod_Double(
        argValue, AniClassFindMethod(env, ANI_INTERFACE_POINT, "<get>y", ":d"), &objValue);
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
    AniTextUtils::ReadArrayField<std::string>(env, obj,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>textShadows", ANI_WRAP_RETURN_C(ANI_ARRAY)), array,
        [&textShadow](ani_env* env, ani_ref ref) {
            ani_object shadowObj = reinterpret_cast<ani_object>(ref);
            double runTimeRadius;
            AniTextUtils::ReadOptionalDoubleField(env, shadowObj,
                AniClassFindMethod(env, ANI_INTERFACE_TEXTSHADOW, "<get>blurRadius", ANI_WRAP_RETURN_C(ANI_DOUBLE)),
                runTimeRadius);

            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            ParseDrawingColorToNative(env, shadowObj, true,
                AniClassFindMethod(
                    env, ANI_INTERFACE_TEXTSHADOW, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)),
                colorSrc);

            Drawing::Point offset(0, 0);
            ani_ref pointValue = nullptr;
            ani_status ret = AniTextUtils::ReadOptionalField(env, shadowObj,
                AniClassFindMethod(
                    env, ANI_INTERFACE_TEXTSHADOW, "<get>point", ANI_WRAP_RETURN_C(ANI_INTERFACE_POINT)),
                pointValue);
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
    AniTextUtils::ReadArrayField<std::string>(env, obj,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFeatures", ANI_WRAP_RETURN_C(ANI_ARRAY)),
        array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref nameRef = nullptr;
            ani_status ret = env->Object_CallMethod_Ref(obj,
                AniClassFindMethod(env, ANI_INTERFACE_FONT_FEATURE, "<get>name", ANI_WRAP_RETURN_C(ANI_STRING)),
                &nameRef);
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
            env->Object_CallMethod_Int(
                obj, AniClassFindMethod(env, ANI_INTERFACE_FONT_FEATURE, "<get>value", ":i"), &valueInt);
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
    AniTextUtils::ReadArrayField<std::string>(env, obj,
        AniClassFindMethod(env, ANI_INTERFACE_TEXT_STYLE_R, "<get>fontVariations", ANI_WRAP_RETURN_C(ANI_ARRAY)),
        array, [&fontVariations](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref axisRef = nullptr;
            ani_status ret = env->Object_CallMethod_Ref(obj,
                AniClassFindMethod(env, ANI_INTERFACE_FONT_VARIATION, "<get>axis", ANI_WRAP_RETURN_C(ANI_STRING)),
                &axisRef);
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
            ret = env->Object_CallMethod_Double(
                obj, AniClassFindMethod(env, ANI_INTERFACE_FONT_VARIATION, "<get>value", ":d"), &valueDouble);
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
    if (ParseDrawingColorToNative(env, obj, false,
            AniClassFindMethod(env, ANI_INTERFACE_RECT_STYLE, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)),
            color)
        == ANI_OK) {
        rectStyle.color = color.CastToColorQuad();
    }
    env->Object_CallMethod_Double(obj, AniClassFindMethod(env, ANI_INTERFACE_RECT_STYLE, "<get>leftTopRadius", ":d"),
        &rectStyle.leftTopRadius);
    env->Object_CallMethod_Double(obj,
        AniClassFindMethod(env, ANI_INTERFACE_RECT_STYLE, "<get>rightTopRadius", ":d"), &rectStyle.rightTopRadius);
    env->Object_CallMethod_Double(obj,
        AniClassFindMethod(env, ANI_INTERFACE_RECT_STYLE, "<get>rightBottomRadius", ":d"),
        &rectStyle.rightBottomRadius);
    env->Object_CallMethod_Double(obj,
        AniClassFindMethod(env, ANI_INTERFACE_RECT_STYLE, "<get>leftBottomRadius", ":d"),
        &rectStyle.leftBottomRadius);
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

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_TEXT_STYLE),
        AniClassFindMethod(env, ANI_CLASS_TEXT_STYLE, "<ctor>", sign.c_str()),
        AniTextStyleConverter::ParseDecorationToAni(env, textStyle), aniColorObj,
        AniTextUtils::CreateAniEnum(
            env, AniFindEnum(env, ANI_ENUM_FONT_WEIGHT),
            getEnumIndex(AniTextEnum::ENUM_FONT_WEIGHT, static_cast<uint32_t>(textStyle.fontWeight))),
        AniTextUtils::CreateAniEnum(
            env, AniFindEnum(env, ANI_ENUM_FONT_STYLE),
            getEnumIndex(AniTextEnum::ENUM_FONT_STYLE, static_cast<uint32_t>(textStyle.fontStyle))),
        AniTextUtils::CreateAniEnum(
            env, AniFindEnum(env, ANI_ENUM_TEXT_BASELINE),
            getEnumIndex(AniTextEnum::ENUM_TEXT_BASE_LINE, static_cast<uint32_t>(textStyle.baseline))),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.fontFamilies, textStyle.fontFamilies.size(),
            [](ani_env* env, const std::string& item) { return AniTextUtils::CreateAniStringObj(env, item); }),
        textStyle.fontSize, textStyle.letterSpacing, textStyle.wordSpacing, textStyle.heightScale,
        textStyle.halfLeading, textStyle.heightOnly, AniTextUtils::CreateAniStringObj(env, textStyle.ellipsis),
        AniTextUtils::CreateAniEnum(
            env, AniFindEnum(env, ANI_ENUM_ELLIPSIS_MODE),
            getEnumIndex(AniTextEnum::ENUM_ELLIPSIS_MODAL, static_cast<uint32_t>(textStyle.ellipsisModal))),
        AniTextUtils::CreateAniStringObj(env, textStyle.locale), textStyle.baseLineShift,
        ParseFontFeaturesToAni(env, textStyle.fontFeatures),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.shadows, textStyle.shadows.size(),
            [](ani_env* env, const TextShadow& item) {
                return AniTextStyleConverter::ParseTextShadowToAni(env, item);
            }),
        AniTextStyleConverter::ParseRectStyleToAni(env, textStyle.backgroundRect));
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
        "C{" + std::string(ANI_INTERFACE_COLOR) + "}C{" + std::string(ANI_INTERFACE_POINT) + "}d:";

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_TEXTSHADOW),
        AniClassFindMethod(env, ANI_CLASS_TEXTSHADOW, "<ctor>", sign.c_str()), aniColorObj, aniPointObj,
        ani_double(textShadow.blurRadius));
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

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_DECORATION),
        AniClassFindMethod(env, ANI_CLASS_DECORATION, "<ctor>", sign.c_str()),
        AniTextUtils::CreateAniEnum(
            env, AniFindEnum(env, ANI_ENUM_TEXT_DECORATION_TYPE),
            getEnumIndex(AniTextEnum::ENUM_TEXT_DECORATION, static_cast<uint32_t>(textStyle.decoration))),
        aniColorObj,
        AniTextUtils::CreateAniEnum(
            env, AniFindEnum(env, ANI_ENUM_TEXT_DECORATION_STYLE),
            getEnumIndex(AniTextEnum::ENUM_TEXT_DECORATION_STYLE, static_cast<uint32_t>(textStyle.decorationStyle))),
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
    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_RECT_STYLE),
        AniClassFindMethod(env, ANI_CLASS_RECT_STYLE, "<ctor>", sign.c_str()), aniColorObj, rectStyle.leftTopRadius,
        rectStyle.rightTopRadius, rectStyle.rightBottomRadius, rectStyle.leftBottomRadius);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures)
{
    const std::vector<std::pair<std::string, int>> featureSet = fontFeatures.GetFontFeatures();
    ani_object arrayObj = AniTextUtils::CreateAniArrayAndInitData(
        env, featureSet, featureSet.size(), [](ani_env* env, const std::pair<std::string, int>& feature) {
            static std::string sign = "C{" + std::string(ANI_STRING) + "}i:";
            ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_FONT_FEATURE),
                AniClassFindMethod(env, ANI_CLASS_FONT_FEATURE, "<ctor>", sign.c_str()),
                AniTextUtils::CreateAniStringObj(env, feature.first), ani_int(feature.second));
            return aniObj;
        });
    return arrayObj;
}

ani_object AniTextStyleConverter::ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_FONT_VARIATION),
        AniClassFindMethod(env, ANI_CLASS_FONT_VARIATION, "<ctor>", ":"));
    return aniObj;
}
} // namespace OHOS::Text::ANI