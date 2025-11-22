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
constexpr CacheKey TRANS_TO_REQUIRED_KEY{ANI_NAMESPACE_TEXT, "transToRequired",
    "C{@ohos.graphics.text.text.TextStyle}:C{@ohos.graphics.text.text.TextStyleR}"};
constexpr CacheKey TEXT_STYLE_R_COLOR_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey TEXT_STYLE_R_FONT_WEIGHT_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontWeight", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_WEIGHT)};
constexpr CacheKey TEXT_STYLE_R_FONT_STYLE_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontStyle", ANI_WRAP_RETURN_E(ANI_ENUM_FONT_STYLE)};
constexpr CacheKey TEXT_STYLE_R_BASELINE_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>baseline", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BASELINE)};
constexpr CacheKey TEXT_STYLE_R_FONT_FAMILIES_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFamilies", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_R_FONT_SIZE_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>fontSize", ":d"};
constexpr CacheKey TEXT_STYLE_R_LETTER_SPACING_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>letterSpacing", ":d"};
constexpr CacheKey TEXT_STYLE_R_WORD_SPACING_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>wordSpacing", ":d"};
constexpr CacheKey TEXT_STYLE_R_HEIGHT_SCALE_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>heightScale", ":d"};
constexpr CacheKey TEXT_STYLE_R_HALF_LEADING_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>halfLeading", ":z"};
constexpr CacheKey TEXT_STYLE_R_HEIGHT_ONLY_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>heightOnly", ":z"};
constexpr CacheKey TEXT_STYLE_R_ELLIPSIS_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsis", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey TEXT_STYLE_R_ELLIPSIS_MODE_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>ellipsisMode", ANI_WRAP_RETURN_E(ANI_ENUM_ELLIPSIS_MODE)};
constexpr CacheKey TEXT_STYLE_R_LOCALE_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>locale", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey TEXT_STYLE_R_BASELINE_SHIFT_KEY{ANI_INTERFACE_TEXT_STYLE_R, "<get>baselineShift", ":d"};
constexpr CacheKey TEXT_STYLE_R_BACKGROUND_RECT_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>backgroundRect", ANI_WRAP_RETURN_C(ANI_INTERFACE_RECT_STYLE)};
constexpr CacheKey TEXT_STYLE_R_DECORATION_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>decoration", ANI_WRAP_RETURN_C(ANI_INTERFACE_DECORATION)};
constexpr CacheKey TEXT_STYLE_R_TEXT_SHADOWS_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>textShadows", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_R_FONT_FEATURES_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontFeatures", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_R_FONT_VARIATIONS_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>fontVariations", ANI_WRAP_RETURN_C(ANI_ARRAY)};
constexpr CacheKey TEXT_STYLE_R_BADGE_TYPE_KEY{
    ANI_INTERFACE_TEXT_STYLE_R, "<get>badgeType", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_BADGE_TYPE)};

constexpr CacheKey DECORATION_DECORATION_TYPE_KEY{
    ANI_INTERFACE_DECORATION, "<get>textDecoration", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_TYPE)};
constexpr CacheKey DECORATION_DECORATION_STYLE_KEY{
    ANI_INTERFACE_DECORATION, "<get>decorationStyle", ANI_WRAP_RETURN_E(ANI_ENUM_TEXT_DECORATION_STYLE)};
constexpr CacheKey DECORATION_DECORATION_THICKNESS_SCALE_KEY{
    ANI_INTERFACE_DECORATION, "<get>decorationThicknessScale", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey DECORATION_DECORATION_COLOR_KEY{
    ANI_INTERFACE_DECORATION, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};

constexpr CacheKey POINT_X_KEY{ANI_INTERFACE_POINT, "<get>x", ":d"};
constexpr CacheKey POINT_Y_KEY{ANI_INTERFACE_POINT, "<get>y", ":d"};

constexpr CacheKey TEXTSHADOW_BLUR_RADIUS_KEY{
    ANI_INTERFACE_TEXTSHADOW, "<get>blurRadius", ANI_WRAP_RETURN_C(ANI_DOUBLE)};
constexpr CacheKey TEXTSHADOW_COLOR_KEY{ANI_INTERFACE_TEXTSHADOW, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey TEXTSHADOW_POINT_KEY{ANI_INTERFACE_TEXTSHADOW, "<get>point", ANI_WRAP_RETURN_C(ANI_INTERFACE_POINT)};

constexpr CacheKey FONT_FEATURE_NAME_KEY{ANI_INTERFACE_FONT_FEATURE, "<get>name", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_FEATURE_VALUE_KEY{ANI_INTERFACE_FONT_FEATURE, "<get>value", ":i"};

constexpr CacheKey FONT_VARIATION_AXIS_KEY{ANI_INTERFACE_FONT_VARIATION, "<get>axis", ANI_WRAP_RETURN_C(ANI_STRING)};
constexpr CacheKey FONT_VARIATION_VALUE_KEY{ANI_INTERFACE_FONT_VARIATION, "<get>value", ":d"};

constexpr CacheKey RECT_STYLE_COLOR_KEY{ANI_INTERFACE_RECT_STYLE, "<get>color", ANI_WRAP_RETURN_C(ANI_INTERFACE_COLOR)};
constexpr CacheKey RECT_STYLE_LEFT_TOP_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>leftTopRadius", ":d"};
constexpr CacheKey RECT_STYLE_RIGHT_TOP_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>rightTopRadius", ":d"};
constexpr CacheKey RECT_STYLE_RIGHT_BOTTOM_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>rightBottomRadius", ":d"};
constexpr CacheKey RECT_STYLE_LEFT_BOTTOM_RADIUS_KEY{ANI_INTERFACE_RECT_STYLE, "<get>leftBottomRadius", ":d"};

constexpr std::string_view TEXT_STYLE_SIGN =
    "C{" ANI_INTERFACE_DECORATION "}C{" ANI_INTERFACE_COLOR "}E{" ANI_ENUM_FONT_WEIGHT "}E{" ANI_ENUM_FONT_STYLE
    "}E{" ANI_ENUM_TEXT_BASELINE "}C{" ANI_ARRAY "}ddddzzC{" ANI_STRING "}E{" ANI_ENUM_ELLIPSIS_MODE "}C{" ANI_STRING
    "}dC{" ANI_ARRAY "}C{" ANI_ARRAY "}C{" ANI_INTERFACE_RECT_STYLE "}E{" ANI_ENUM_TEXT_BADGE_TYPE "}:";
constexpr CacheKey TEXT_STYLE_KEY{ANI_CLASS_TEXT_STYLE, "<ctor>", TEXT_STYLE_SIGN};

constexpr std::string_view TEXT_SHADOW_SIGN = "C{" ANI_INTERFACE_COLOR "}C{" ANI_INTERFACE_POINT "}d:";
constexpr CacheKey TEXTSHADOW_KEY{ANI_CLASS_TEXTSHADOW, "<ctor>", TEXT_SHADOW_SIGN};

constexpr std::string_view DECORATION_SIGN =
    "E{" ANI_ENUM_TEXT_DECORATION_TYPE "}C{" ANI_INTERFACE_COLOR "}E{" ANI_ENUM_TEXT_DECORATION_STYLE "}d:";
constexpr CacheKey DECORATION_KEY{ANI_CLASS_DECORATION, "<ctor>", DECORATION_SIGN};

constexpr CacheKey RECT_STYLE_KEY{ANI_CLASS_RECT_STYLE, "<ctor>", "C{" ANI_INTERFACE_COLOR "}dddd:"};

constexpr CacheKey FONT_FEATURE_KEY{ANI_CLASS_FONT_FEATURE, "<ctor>", "C{" ANI_STRING "}i:"};
constexpr CacheKey FONT_VARIATION_KEY{ANI_CLASS_FONT_VARIATION, "<ctor>", ":"};

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
    ani_function fc = AniNamespaceFindFunction(env, TRANS_TO_REQUIRED_KEY);
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
    ParseTextStyleDecorationToNative(env, objR, false, textStyle);
    ParseDrawingColorToNative(env, objR, false, AniClassFindMethod(env, TEXT_STYLE_R_COLOR_KEY), textStyle.color);

    AniTextUtils::ReadEnumField(env, objR, AniTextEnum::fontWeight,
        AniClassFindMethod(env, TEXT_STYLE_R_FONT_WEIGHT_KEY), textStyle.fontWeight);
    AniTextUtils::ReadEnumField(
        env, objR, AniTextEnum::fontStyle, AniClassFindMethod(env, TEXT_STYLE_R_FONT_STYLE_KEY), textStyle.fontStyle);
    if (textStyle.fontStyle == FontStyle::OBLIQUE) {
        textStyle.fontStyle = FontStyle::ITALIC;
    }
    AniTextUtils::ReadEnumField(
        env, objR, AniTextEnum::textBaseLine, AniClassFindMethod(env, TEXT_STYLE_R_BASELINE_KEY), textStyle.baseline);

    AniTextUtils::ReadArrayField<std::string>(env, objR, AniClassFindMethod(env, TEXT_STYLE_R_FONT_FAMILIES_KEY),
        textStyle.fontFamilies, [](ani_env* env, ani_ref ref) {
            std::string utf8Str;
            AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), utf8Str);
            return utf8Str;
        });
    env->Object_CallMethod_Double(objR, AniClassFindMethod(env, TEXT_STYLE_R_FONT_SIZE_KEY), &textStyle.fontSize);
    env->Object_CallMethod_Double(
        objR, AniClassFindMethod(env, TEXT_STYLE_R_LETTER_SPACING_KEY), &textStyle.letterSpacing);
    env->Object_CallMethod_Double(objR, AniClassFindMethod(env, TEXT_STYLE_R_WORD_SPACING_KEY), &textStyle.wordSpacing);
    env->Object_CallMethod_Double(objR, AniClassFindMethod(env, TEXT_STYLE_R_HEIGHT_SCALE_KEY), &textStyle.heightScale);
    textStyle.heightScale = textStyle.heightScale < 0 ? 0 : textStyle.heightScale;
    ani_boolean tempBool;
    status = env->Object_CallMethod_Boolean(objR, AniClassFindMethod(env, TEXT_STYLE_R_HALF_LEADING_KEY), &tempBool);
    textStyle.halfLeading = (status == ANI_OK && tempBool);
    status = env->Object_CallMethod_Boolean(objR, AniClassFindMethod(env, TEXT_STYLE_R_HEIGHT_ONLY_KEY), &tempBool);
    textStyle.heightOnly = (status == ANI_OK && tempBool);
    AniTextUtils::GetPropertyByCache_U16String(
        env, objR, AniClassFindMethod(env, TEXT_STYLE_R_ELLIPSIS_KEY), textStyle.ellipsis);
    AniTextUtils::ReadEnumField(env, objR, AniTextEnum::ellipsisModal,
        AniClassFindMethod(env, TEXT_STYLE_R_ELLIPSIS_MODE_KEY), textStyle.ellipsisModal);
    AniTextUtils::GetPropertyByCache_String(
        env, objR, AniClassFindMethod(env, TEXT_STYLE_R_LOCALE_KEY), textStyle.locale);
    env->Object_CallMethod_Double(
        objR, AniClassFindMethod(env, TEXT_STYLE_R_BASELINE_SHIFT_KEY), &textStyle.baseLineShift);
    ParseTextShadowToNative(env, objR, textStyle.shadows);
    ParseFontFeatureToNative(env, objR, textStyle.fontFeatures);
    ParseFontVariationToNative(env, objR, textStyle.fontVariations);
    ParseRectStyleToNative(env, objR, textStyle.backgroundRect);
    AniTextUtils::ReadEnumField(env, objR, AniTextEnum::textBadgeType,
        AniClassFindMethod(env, TEXT_STYLE_R_BADGE_TYPE_KEY), textStyle.badgeType);
    return ANI_OK;
}

void AniTextStyleConverter::ParseTextStyleDecorationToNative(
    ani_env* env, ani_object textStyleObj, bool reLayout, TextStyle& textStyle)
{
    ani_ref decorationRef = nullptr;
    ani_status status =
        env->Object_CallMethod_Ref(textStyleObj, AniClassFindMethod(env, TEXT_STYLE_R_DECORATION_KEY), &decorationRef);
    if (status == ANI_OK && decorationRef != nullptr) {
        ParseDecorationToNative(env, reinterpret_cast<ani_object>(decorationRef), reLayout, textStyle);
    }
}

void AniTextStyleConverter::ParseDecorationToNative(ani_env* env, ani_object obj, bool reLayout, TextStyle& textStyle)
{
    ani_status ret = AniTextUtils::ReadOptionalEnumField(env, obj,
        AniTextEnum::textDecoration, AniClassFindMethod(env, DECORATION_DECORATION_TYPE_KEY), textStyle.decoration);
    if (ret == ANI_OK) {
        if (reLayout) {
            textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION));
        }
    } else {
        TEXT_LOGE("Failed to parse textDecoration, ret %{public}d", ret);
    }
    ret = AniTextUtils::ReadOptionalEnumField(env, obj,
        AniTextEnum::textDecorationStyle, AniClassFindMethod(env, DECORATION_DECORATION_STYLE_KEY),
        textStyle.decorationStyle);
    if (ret == ANI_OK) {
        if (reLayout) {
            textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_STYLE));
        }
    } else {
        TEXT_LOGE("Failed to parse decorationStyle, ret %{public}d", ret);
    }
    ret = AniTextUtils::ReadOptionalDoubleField(env, obj,
        AniClassFindMethod(env, DECORATION_DECORATION_THICKNESS_SCALE_KEY), textStyle.decorationThicknessScale);
    if (ret == ANI_OK) {
    if (reLayout) {
            textStyle.relayoutChangeBitmap.set(
                static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_THICKNESS_SCALE));
        }
    } else {
        TEXT_LOGE("Failed to parse decorationThicknessScale, ret %{public}d", ret);
    }
    ret = ParseDrawingColorToNative(env, obj, true,
        AniClassFindMethod(env, DECORATION_DECORATION_COLOR_KEY), textStyle.decorationColor);
    if (ret == ANI_OK) {
    if (reLayout) {
            textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_COLOR));
        }
    } else {
        TEXT_LOGE("Failed to parse color, ret %{public}d", ret);
    }
}

inline void GetPointXFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    ani_status ret = env->Object_CallMethod_Double(argValue, AniClassFindMethod(env, POINT_X_KEY), &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param x is invalid, ret %{public}d", ret);
        return;
    }
    point.SetX(objValue);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    ani_status ret = env->Object_CallMethod_Double(argValue, AniClassFindMethod(env, POINT_Y_KEY), &objValue);
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
    AniTextUtils::ReadArrayField<std::string>(env, obj, AniClassFindMethod(env, TEXT_STYLE_R_TEXT_SHADOWS_KEY), array,
        [&textShadow](ani_env* env, ani_ref ref) {
            ani_object shadowObj = reinterpret_cast<ani_object>(ref);
            double runTimeRadius;
            AniTextUtils::ReadOptionalDoubleField(
                env, shadowObj, AniClassFindMethod(env, TEXTSHADOW_BLUR_RADIUS_KEY), runTimeRadius);

            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            ParseDrawingColorToNative(env, shadowObj, true, AniClassFindMethod(env, TEXTSHADOW_COLOR_KEY), colorSrc);

            Drawing::Point offset(0, 0);
            ani_ref pointValue = nullptr;
            ani_status ret = AniTextUtils::ReadOptionalField(
                env, shadowObj, AniClassFindMethod(env, TEXTSHADOW_POINT_KEY), pointValue);
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
    AniTextUtils::ReadArrayField<std::string>(env, obj, AniClassFindMethod(env, TEXT_STYLE_R_FONT_FEATURES_KEY), array,
        [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref nameRef = nullptr;
            ani_status ret = env->Object_CallMethod_Ref(obj, AniClassFindMethod(env, FONT_FEATURE_NAME_KEY), &nameRef);
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
            ret = env->Object_CallMethod_Int(obj, AniClassFindMethod(env, FONT_FEATURE_VALUE_KEY), &valueInt);
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
    AniTextUtils::ReadArrayField<std::string>(env, obj, AniClassFindMethod(env, TEXT_STYLE_R_FONT_VARIATIONS_KEY),
        array, [&fontVariations](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref axisRef = nullptr;
            ani_status ret =
                env->Object_CallMethod_Ref(obj, AniClassFindMethod(env, FONT_VARIATION_AXIS_KEY), &axisRef);
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
            ret = env->Object_CallMethod_Double(obj, AniClassFindMethod(env, FONT_VARIATION_VALUE_KEY), &valueDouble);
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
    ani_ref backgroundRectRef = nullptr;
    ani_status status =
        env->Object_CallMethod_Ref(obj, AniClassFindMethod(env, TEXT_STYLE_R_BACKGROUND_RECT_KEY), &backgroundRectRef);
    if (status == ANI_OK && backgroundRectRef != nullptr) {
        ani_object rectObj = reinterpret_cast<ani_object>(backgroundRectRef);
        Drawing::Color color;
        ani_status ret =
            ParseDrawingColorToNative(env, rectObj, false, AniClassFindMethod(env, RECT_STYLE_COLOR_KEY), color);
        if (ret == ANI_OK) {
            rectStyle.color = color.CastToColorQuad();
        }
        env->Object_CallMethod_Double(
            rectObj, AniClassFindMethod(env, RECT_STYLE_LEFT_TOP_RADIUS_KEY), &rectStyle.leftTopRadius);
        env->Object_CallMethod_Double(
            rectObj, AniClassFindMethod(env, RECT_STYLE_RIGHT_TOP_RADIUS_KEY), &rectStyle.rightTopRadius);
        env->Object_CallMethod_Double(
            rectObj, AniClassFindMethod(env, RECT_STYLE_RIGHT_BOTTOM_RADIUS_KEY), &rectStyle.rightBottomRadius);
        env->Object_CallMethod_Double(
            rectObj, AniClassFindMethod(env, RECT_STYLE_LEFT_BOTTOM_RADIUS_KEY), &rectStyle.leftBottomRadius);
    }
}

ani_object AniTextStyleConverter::ParseTextStyleToAni(ani_env* env, const TextStyle& textStyle)
{
    ani_object aniColorObj = nullptr;
    ani_status status = OHOS::Rosen::Drawing::CreateColorObj(env, textStyle.color, aniColorObj);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to parse color, ret %{public}d", status);
        aniColorObj = AniTextUtils::CreateAniUndefined(env);
    }

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_TEXT_STYLE),
        AniClassFindMethod(env, TEXT_STYLE_KEY), AniTextStyleConverter::ParseDecorationToAni(env, textStyle),
        aniColorObj,
        AniTextUtils::CreateAniOptionalEnum(env, AniFindEnum(env, ANI_ENUM_FONT_WEIGHT),
            aniGetEnumIndex(AniTextEnum::fontWeight, static_cast<ani_size>(textStyle.fontWeight))),
        AniTextUtils::CreateAniOptionalEnum(env, AniFindEnum(env, ANI_ENUM_FONT_STYLE),
            aniGetEnumIndex(AniTextEnum::fontStyle, static_cast<ani_size>(textStyle.fontStyle))),
        AniTextUtils::CreateAniOptionalEnum(env, AniFindEnum(env, ANI_ENUM_TEXT_BASELINE),
            aniGetEnumIndex(AniTextEnum::textBaseLine, static_cast<ani_size>(textStyle.baseline))),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.fontFamilies, textStyle.fontFamilies.size(),
            [](ani_env* env, const std::string& item) { return AniTextUtils::CreateAniStringObj(env, item); }),
        textStyle.fontSize, textStyle.letterSpacing, textStyle.wordSpacing, textStyle.heightScale,
        textStyle.halfLeading, textStyle.heightOnly, AniTextUtils::CreateAniStringObj(env, textStyle.ellipsis),
        AniTextUtils::CreateAniOptionalEnum(env, AniFindEnum(env, ANI_ENUM_ELLIPSIS_MODE),
            aniGetEnumIndex(AniTextEnum::ellipsisModal, static_cast<ani_size>(textStyle.ellipsisModal))),
        AniTextUtils::CreateAniStringObj(env, textStyle.locale), textStyle.baseLineShift,
        ParseFontFeaturesToAni(env, textStyle.fontFeatures),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.shadows, textStyle.shadows.size(),
            [](ani_env* env, const TextShadow& item) {
                return AniTextStyleConverter::ParseTextShadowToAni(env, item);
            }),
        AniTextStyleConverter::ParseRectStyleToAni(env, textStyle.backgroundRect),
        AniTextUtils::CreateAniOptionalEnum(env, AniFindEnum(env, ANI_ENUM_TEXT_BADGE_TYPE),
            static_cast<ani_size>(textStyle.badgeType))
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

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_TEXTSHADOW),
        AniClassFindMethod(env, TEXTSHADOW_KEY), aniColorObj, aniPointObj, ani_double(textShadow.blurRadius));
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
    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_DECORATION),
        AniClassFindMethod(env, DECORATION_KEY),
        AniTextUtils::CreateAniOptionalEnum(env, AniFindEnum(env, ANI_ENUM_TEXT_DECORATION_TYPE),
            aniGetEnumIndex(AniTextEnum::textDecoration, static_cast<ani_size>(textStyle.decoration))),
        aniColorObj,
        AniTextUtils::CreateAniOptionalEnum(env, AniFindEnum(env, ANI_ENUM_TEXT_DECORATION_STYLE),
            aniGetEnumIndex(AniTextEnum::textDecorationStyle, static_cast<ani_size>(textStyle.decorationStyle))),
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
    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_RECT_STYLE),
        AniClassFindMethod(env, RECT_STYLE_KEY), aniColorObj, rectStyle.leftTopRadius, rectStyle.rightTopRadius,
        rectStyle.rightBottomRadius, rectStyle.leftBottomRadius);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures)
{
    const std::vector<std::pair<std::string, int>> featureSet = fontFeatures.GetFontFeatures();
    ani_object arrayObj = AniTextUtils::CreateAniArrayAndInitData(
        env, featureSet, featureSet.size(), [](ani_env* env, const std::pair<std::string, int>& feature) {
            ani_object aniObj = AniTextUtils::CreateAniObject(env, AniFindClass(env, ANI_CLASS_FONT_FEATURE),
                AniClassFindMethod(env, FONT_FEATURE_KEY), AniTextUtils::CreateAniStringObj(env, feature.first),
                ani_int(feature.second));
            return aniObj;
        });
    return arrayObj;
}

ani_object AniTextStyleConverter::ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(
        env, AniFindClass(env, ANI_CLASS_FONT_VARIATION), AniClassFindMethod(env, FONT_VARIATION_KEY));
    return aniObj;
}
} // namespace OHOS::Text::ANI