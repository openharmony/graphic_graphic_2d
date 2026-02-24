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
#include "text/font_types.h"
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

void AniTextStyleConverter::ParseEnumTextStyleToNative(ani_env* env, ani_object obj, OHOS::Rosen::TextStyle& textStyle)
{
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::fontWeight, AniGlobalMethod::GetInstance().textStyleFontWeight, textStyle.fontWeight);
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::fontStyle, AniGlobalMethod::GetInstance().textStyleFontStyle, textStyle.fontStyle);
    if (textStyle.fontStyle == FontStyle::OBLIQUE) {
        textStyle.fontStyle = FontStyle::ITALIC;
    }
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::textBaseLine, AniGlobalMethod::GetInstance().textStyleBaseline, textStyle.baseline);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::ellipsisModal,
        AniGlobalMethod::GetInstance().textStyleEllipsisMode, textStyle.ellipsisModal);
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::textBadgeType, AniGlobalMethod::GetInstance().textStyleBadgeType, textStyle.badgeType);
    AniTextUtils::ReadOptionalEnumField(env, obj, AniTextEnum::lineHeightStyle,
        AniGlobalMethod::GetInstance().textStyleLineHeightStyle, textStyle.lineHeightStyle);
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::fontWidth, AniGlobalMethod::GetInstance().textStyleFontWidth, textStyle.fontWidth);
    AniTextUtils::ReadOptionalEnumField(
        env, obj, AniTextEnum::fontEdging, AniGlobalMethod::GetInstance().textStyleFontEdging, textStyle.fontEdging);
}

void AniTextStyleConverter::ParseDoubleTextStyleToNative(
    ani_env* env, ani_object obj, OHOS::Rosen::TextStyle& textStyle)
{
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().textStyleFontSize, textStyle.fontSize);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().textStyleLetterSpacing, textStyle.letterSpacing);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().textStyleWordSpacing, textStyle.wordSpacing);
    textStyle.heightScale = textStyle.heightScale < 0 ? 0 : textStyle.heightScale;
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().textStyleHeightScale, textStyle.heightScale);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().textStyleBaselineShift, textStyle.baseLineShift);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().textStyleMaxLineHeight, textStyle.maxLineHeight);
    AniTextUtils::ReadOptionalDoubleField(
        env, obj, AniGlobalMethod::GetInstance().textStyleMinLineHeight, textStyle.minLineHeight);
}

void AniTextStyleConverter::ParseBoolTextStyleToNative(ani_env* env, ani_object obj, OHOS::Rosen::TextStyle& textStyle)
{
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::GetInstance().textStyleHalfLeading, textStyle.halfLeading);
    AniTextUtils::ReadOptionalBoolField(
        env, obj, AniGlobalMethod::GetInstance().textStyleHeightOnly, textStyle.heightOnly);
}

ani_status AniTextStyleConverter::ParseTextStyleToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ParseTextStyleDecorationToNative(env, obj, false, textStyle);
    ParseDrawingColorToNative(env, obj, true, AniGlobalMethod::GetInstance().textStyleColor, textStyle.color);
    AniTextUtils::ReadOptionalArrayField<std::string>(env, obj, AniGlobalMethod::GetInstance().textStyleFontFamilies,
        textStyle.fontFamilies, [](ani_env* env, ani_ref ref) {
            std::string utf8Str;
            AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), utf8Str);
            return utf8Str;
        });
    AniTextUtils::ReadOptionalU16StringField(
        env, obj, AniGlobalMethod::GetInstance().textStyleEllipsis, textStyle.ellipsis);
    AniTextUtils::ReadOptionalStringField(env, obj, AniGlobalMethod::GetInstance().textStyleLocale, textStyle.locale);
    ParseTextShadowToNative(env, obj, textStyle.shadows);
    ParseFontFeatureToNative(env, obj, textStyle.fontFeatures);
    ParseFontVariationToNative(env, obj, textStyle.fontVariations);
    ParseRectStyleToNative(env, obj, textStyle.backgroundRect);

    ParseBoolTextStyleToNative(env, obj, textStyle);
    ParseDoubleTextStyleToNative(env, obj, textStyle);
    ParseEnumTextStyleToNative(env, obj, textStyle);
    return ANI_OK;
}

void AniTextStyleConverter::ParseTextStyleDecorationToNative(
    ani_env* env, ani_object textStyleObj, bool reLayout, TextStyle& textStyle)
{
    ani_ref decorationRef = nullptr;
    ani_status status = AniTextUtils::ReadOptionalField(
        env, textStyleObj, AniGlobalMethod::GetInstance().textStyleDecoration, decorationRef);
    if (status == ANI_OK && decorationRef != nullptr) {
        ParseDecorationToNative(env, reinterpret_cast<ani_object>(decorationRef), reLayout, textStyle);
    }
}

void AniTextStyleConverter::ParseDecorationToNative(ani_env* env, ani_object obj, bool reLayout, TextStyle& textStyle)
{
    ani_status ret = AniTextUtils::ReadOptionalEnumField(env, obj,
        AniTextEnum::textDecoration, AniGlobalMethod::GetInstance().decorationDecorationType, textStyle.decoration);
    if (ret == ANI_OK) {
        if (reLayout) {
            textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION));
        }
    } else {
        TEXT_LOGE("Failed to parse textDecoration, ret %{public}d", ret);
    }
    ret = AniTextUtils::ReadOptionalEnumField(env, obj,
        AniTextEnum::textDecorationStyle, AniGlobalMethod::GetInstance().decorationDecorationStyle,
        textStyle.decorationStyle);
    if (ret == ANI_OK) {
        if (reLayout) {
            textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_STYLE));
        }
    } else {
        TEXT_LOGE("Failed to parse decorationStyle, ret %{public}d", ret);
    }
    ret = AniTextUtils::ReadOptionalDoubleField(env, obj,
        AniGlobalMethod::GetInstance().decorationDecorationThicknessScale, textStyle.decorationThicknessScale);
    if (ret == ANI_OK) {
    if (reLayout) {
            textStyle.relayoutChangeBitmap.set(
                static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_THICKNESS_SCALE));
        }
    } else {
        TEXT_LOGE("Failed to parse decorationThicknessScale, ret %{public}d", ret);
    }
    ret = ParseDrawingColorToNative(env, obj, true,
        AniGlobalMethod::GetInstance().decorationDecorationColor, textStyle.decorationColor);
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
    ani_status ret = env->Object_CallMethod_Double(argValue, AniGlobalMethod::GetInstance().pointX, &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param x is invalid, ret %{public}d", ret);
        return;
    }
    point.SetX(objValue);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    ani_status ret = env->Object_CallMethod_Double(argValue, AniGlobalMethod::GetInstance().pointY, &objValue);
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
        env, obj, AniGlobalMethod::GetInstance().textStyleTextShadows, array, [&textShadow](ani_env* env, ani_ref ref) {
            ani_object shadowObj = reinterpret_cast<ani_object>(ref);
            double runTimeRadius;
            AniTextUtils::ReadOptionalDoubleField(
                env, shadowObj, AniGlobalMethod::GetInstance().textShadowBlurRadius, runTimeRadius);

            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            ParseDrawingColorToNative(env, shadowObj, true, AniGlobalMethod::GetInstance().textShadowColor, colorSrc);

            Drawing::Point offset(0, 0);
            ani_ref pointValue = nullptr;
            ani_status ret = AniTextUtils::ReadOptionalField(
                env, shadowObj, AniGlobalMethod::GetInstance().textShadowPoint, pointValue);
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
    AniTextUtils::ReadOptionalArrayField<std::string>(env, obj, AniGlobalMethod::GetInstance().textStyleFontFeatures,
        array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref nameRef = nullptr;
            ani_status ret = env->Object_CallMethod_Ref(obj, AniGlobalMethod::GetInstance().fontFeatureName, &nameRef);
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
            ret = env->Object_CallMethod_Int(obj, AniGlobalMethod::GetInstance().fontFeatureValue, &valueInt);
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
    AniTextUtils::ReadOptionalArrayField<std::string>(env, obj, AniGlobalMethod::GetInstance().textStyleFontVariations,
        array, [&fontVariations](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref axisRef = nullptr;
            ani_status ret =
                env->Object_CallMethod_Ref(obj, AniGlobalMethod::GetInstance().fontVariationAxis, &axisRef);
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
            ret = env->Object_CallMethod_Double(obj, AniGlobalMethod::GetInstance().fontVariationValue, &valueDouble);
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
    ani_status status = AniTextUtils::ReadOptionalField(
        env, obj, AniGlobalMethod::GetInstance().textStyleBackgroundRect, backgroundRectRef);
    if (status == ANI_OK && backgroundRectRef != nullptr) {
        ani_object rectObj = reinterpret_cast<ani_object>(backgroundRectRef);
        Drawing::Color color;
        ani_status ret =
            ParseDrawingColorToNative(env, rectObj, false, AniGlobalMethod::GetInstance().rectStyleColor, color);
        if (ret == ANI_OK) {
            rectStyle.color = color.CastToColorQuad();
        }
        env->Object_CallMethod_Double(
            rectObj, AniGlobalMethod::GetInstance().rectStyleLeftTopRadius, &rectStyle.leftTopRadius);
        env->Object_CallMethod_Double(
            rectObj, AniGlobalMethod::GetInstance().rectStyleRightTopRadius, &rectStyle.rightTopRadius);
        env->Object_CallMethod_Double(
            rectObj, AniGlobalMethod::GetInstance().rectStyleRightBottomRadius, &rectStyle.rightBottomRadius);
        env->Object_CallMethod_Double(
            rectObj, AniGlobalMethod::GetInstance().rectStyleLeftBottomRadius, &rectStyle.leftBottomRadius);
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

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().textStyle,
        AniGlobalMethod::GetInstance().textStyleCtor, AniTextStyleConverter::ParseDecorationToAni(env, textStyle),
        aniColorObj,
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().fontWeight,
            aniGetEnumIndex(AniTextEnum::fontWeight, static_cast<uint32_t>(textStyle.fontWeight))),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().fontStyle,
            aniGetEnumIndex(AniTextEnum::fontStyle, static_cast<uint32_t>(textStyle.fontStyle))),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().textBaseline,
            aniGetEnumIndex(AniTextEnum::textBaseLine, static_cast<uint32_t>(textStyle.baseline))),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.fontFamilies, textStyle.fontFamilies.size(),
            [](ani_env* env, const std::string& item) { return AniTextUtils::CreateAniStringObj(env, item); }),
        textStyle.fontSize, textStyle.letterSpacing, textStyle.wordSpacing, textStyle.heightScale,
        textStyle.halfLeading, textStyle.heightOnly, AniTextUtils::CreateAniStringObj(env, textStyle.ellipsis),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().ellipsisMode,
            aniGetEnumIndex(AniTextEnum::ellipsisModal, static_cast<uint32_t>(textStyle.ellipsisModal))),
        AniTextUtils::CreateAniStringObj(env, textStyle.locale), textStyle.baseLineShift,
        ParseFontFeaturesToAni(env, textStyle.fontFeatures),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.shadows, textStyle.shadows.size(),
            [](ani_env* env, const TextShadow& item) {
                return AniTextStyleConverter::ParseTextShadowToAni(env, item);
            }),
        AniTextStyleConverter::ParseRectStyleToAni(env, textStyle.backgroundRect),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().textBadgeType,
            aniGetEnumIndex(AniTextEnum::textBadgeType, static_cast<uint32_t>(textStyle.badgeType))),
        textStyle.maxLineHeight, textStyle.minLineHeight,
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().lineHeightStyle,
            aniGetEnumIndex(AniTextEnum::lineHeightStyle, static_cast<uint32_t>(textStyle.lineHeightStyle))),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().fontWidth,
            aniGetEnumIndex(AniTextEnum::fontWidth, static_cast<uint32_t>(textStyle.fontWidth))),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().fontEdging,
            aniGetEnumIndex(AniTextEnum::fontEdging, static_cast<uint32_t>(textStyle.fontEdging)))
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

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().textShadow,
        AniGlobalMethod::GetInstance().textShadowCtor, aniColorObj, aniPointObj, ani_double(textShadow.blurRadius));
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
    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().decoration,
        AniGlobalMethod::GetInstance().decorationCtor,
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().textDecorationType,
            aniGetEnumIndex(AniTextEnum::textDecoration, static_cast<uint32_t>(textStyle.decoration))),
        aniColorObj,
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::GetInstance().textDecorationStyle,
            aniGetEnumIndex(AniTextEnum::textDecorationStyle, static_cast<uint32_t>(textStyle.decorationStyle))),
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
    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().rectStyle,
        AniGlobalMethod::GetInstance().rectStyleCtor, aniColorObj, rectStyle.leftTopRadius, rectStyle.rightTopRadius,
        rectStyle.rightBottomRadius, rectStyle.leftBottomRadius);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures)
{
    const std::vector<std::pair<std::string, int>> featureSet = fontFeatures.GetFontFeatures();
    ani_object arrayObj = AniTextUtils::CreateAniArrayAndInitData(
        env, featureSet, featureSet.size(), [](ani_env* env, const std::pair<std::string, int>& feature) {
            ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::GetInstance().fontFeature,
                AniGlobalMethod::GetInstance().fontFeatureCtor, AniTextUtils::CreateAniStringObj(env, feature.first),
                ani_int(feature.second));
            return aniObj;
        });
    return arrayObj;
}

ani_object AniTextStyleConverter::ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations)
{
    ani_object aniObj = AniTextUtils::CreateAniObject(
        env, AniGlobalClass::GetInstance().fontVariation, AniGlobalMethod::GetInstance().fontVariationCtor);
    return aniObj;
}
} // namespace OHOS::Text::ANI