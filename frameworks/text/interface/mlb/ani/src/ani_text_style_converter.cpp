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
    ani_ref result = nullptr;
    ani_status ret = env->Function_Call_Ref(AniGlobalFunction::transToRequired, &result, obj);
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
    ParseDrawingColorToNative(env, objR, false, AniGlobalMethod::textStyleRColor, textStyle.color);

    AniTextUtils::ReadEnumField(
        env, objR, AniTextEnum::fontWeight, AniGlobalMethod::textStyleRFontWeight, textStyle.fontWeight);
    AniTextUtils::ReadEnumField(
        env, objR, AniTextEnum::fontStyle, AniGlobalMethod::textStyleRFontStyle, textStyle.fontStyle);
    if (textStyle.fontStyle == FontStyle::OBLIQUE) {
        textStyle.fontStyle = FontStyle::ITALIC;
    }
    AniTextUtils::ReadEnumField(
        env, objR, AniTextEnum::textBaseLine, AniGlobalMethod::textStyleRBaseline, textStyle.baseline);

    AniTextUtils::ReadArrayField<std::string>(
        env, objR, AniGlobalMethod::textStyleRFontFamilies, textStyle.fontFamilies, [](ani_env* env, ani_ref ref) {
            std::string utf8Str;
            AniTextUtils::AniToStdStringUtf8(env, reinterpret_cast<ani_string>(ref), utf8Str);
            return utf8Str;
        });

    env->Object_CallMethod_Double(objR, AniGlobalMethod::textStyleRFontSize, &textStyle.fontSize);
    env->Object_CallMethod_Double(objR, AniGlobalMethod::textStyleRLetterSpacing, &textStyle.letterSpacing);
    env->Object_CallMethod_Double(objR, AniGlobalMethod::textStyleRWordSpacing, &textStyle.wordSpacing);
    env->Object_CallMethod_Double(objR, AniGlobalMethod::textStyleRHeightScale, &textStyle.heightScale);
    textStyle.heightScale = textStyle.heightScale < 0 ? 0 : textStyle.heightScale;
    ani_boolean tempBool;
    status = env->Object_CallMethod_Boolean(objR, AniGlobalMethod::textStyleRHalfLeading, &tempBool);
    textStyle.halfLeading = (status == ANI_OK && tempBool);
    status = env->Object_CallMethod_Boolean(objR, AniGlobalMethod::textStyleRHeightOnly, &tempBool);
    textStyle.heightOnly = (status == ANI_OK && tempBool);
    AniTextUtils::GetPropertyByCache_U16String(env, objR, AniGlobalMethod::textStyleREllipsis, textStyle.ellipsis);
    AniTextUtils::ReadEnumField(
        env, objR, AniTextEnum::ellipsisModal, AniGlobalMethod::textStyleREllipsisMode, textStyle.ellipsisModal);
    AniTextUtils::GetPropertyByCache_String(env, objR, AniGlobalMethod::textStyleRLocale, textStyle.locale);
    env->Object_CallMethod_Double(objR, AniGlobalMethod::textStyleRBaselineShift, &textStyle.baseLineShift);
    ParseTextShadowToNative(env, objR, textStyle.shadows);
    ParseFontFeatureToNative(env, objR, textStyle.fontFeatures);
    ParseFontVariationToNative(env, objR, textStyle.fontVariations);
    ParseRectStyleToNative(env, objR, textStyle.backgroundRect);
    return ANI_OK;
}

void AniTextStyleConverter::ParseDecorationToNative(ani_env* env, ani_object obj, TextStyle& textStyle)
{
    ani_ref decorationRef = nullptr;
    ani_status status = env->Object_CallMethod_Ref(obj, AniGlobalMethod::textStyleRDecoration, &decorationRef);
    if (status == ANI_OK && decorationRef != nullptr) {
        AniTextUtils::ReadOptionalEnumField(env, reinterpret_cast<ani_object>(decorationRef),
            AniTextEnum::textDecoration, AniGlobalMethod::decorationDecorationType, textStyle.decoration);
        AniTextUtils::ReadOptionalEnumField(env, reinterpret_cast<ani_object>(decorationRef),
            AniTextEnum::textDecorationStyle, AniGlobalMethod::decorationDecorationStyle, textStyle.decorationStyle);
        AniTextUtils::ReadOptionalDoubleField(env, reinterpret_cast<ani_object>(decorationRef),
            AniGlobalMethod::decorationDecorationThicknessScale, textStyle.decorationThicknessScale);
        ParseDrawingColorToNative(env, reinterpret_cast<ani_object>(decorationRef), true,
            AniGlobalMethod::decorationDecorationColor, textStyle.decorationColor);
    }
}

inline void GetPointXFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    ani_status ret = env->Object_CallMethod_Double(argValue, AniGlobalMethod::pointX, &objValue);
    if (ret != ANI_OK) {
        TEXT_LOGE("Param x is invalid, ret %{public}d", ret);
        return;
    }
    point.SetX(objValue);
}

inline void GetPointYFromJsBumber(ani_env* env, ani_object argValue, Drawing::Point& point)
{
    ani_double objValue = 0;
    ani_status ret = env->Object_CallMethod_Double(argValue, AniGlobalMethod::pointY, &objValue);
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
    AniTextUtils::ReadArrayField<std::string>(
        env, obj, AniGlobalMethod::textStyleRTextShadows, array, [&textShadow](ani_env* env, ani_ref ref) {
            ani_object shadowObj = reinterpret_cast<ani_object>(ref);
            double runTimeRadius;
            AniTextUtils::ReadOptionalDoubleField(env, shadowObj, AniGlobalMethod::textShadowBlurRadius, runTimeRadius);

            Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
            ParseDrawingColorToNative(env, shadowObj, true, AniGlobalMethod::textShadowColor, colorSrc);

            Drawing::Point offset(0, 0);
            ani_ref pointValue = nullptr;
            ani_status ret =
                AniTextUtils::ReadOptionalField(env, shadowObj, AniGlobalMethod::textShadowPoint, pointValue);
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
    AniTextUtils::ReadArrayField<std::string>(
        env, obj, AniGlobalMethod::textStyleRFontFeatures, array, [&fontFeatures](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref nameRef = nullptr;
            ani_status ret = env->Object_CallMethod_Ref(obj, AniGlobalMethod::fontFeatureName, &nameRef);
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
            ret = env->Object_CallMethod_Int(obj, AniGlobalMethod::fontFeatureValue, &valueInt);
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
    AniTextUtils::ReadArrayField<std::string>(
        env, obj, AniGlobalMethod::textStyleRFontVariations, array, [&fontVariations](ani_env* env, ani_ref ref) {
            ani_object obj = reinterpret_cast<ani_object>(ref);
            ani_ref axisRef = nullptr;
            ani_status ret = env->Object_CallMethod_Ref(obj, AniGlobalMethod::fontVariationAxis, &axisRef);
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
            ret = env->Object_CallMethod_Double(obj, AniGlobalMethod::fontVariationValue, &valueDouble);
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
    ani_status status = env->Object_CallMethod_Ref(obj, AniGlobalMethod::textStyleRBackgroundRect, &backgroundRectRef);
    if (status == ANI_OK && backgroundRectRef != nullptr) {
        ani_object rectObj = reinterpret_cast<ani_object>(backgroundRectRef);
        Drawing::Color color;
        ani_status ret = ParseDrawingColorToNative(env, rectObj, false, AniGlobalMethod::rectStyleColor, color);
        if (ret == ANI_OK) {
            rectStyle.color = color.CastToColorQuad();
        }
        env->Object_CallMethod_Double(rectObj, AniGlobalMethod::rectStyleLeftTopRadius, &rectStyle.leftTopRadius);
        env->Object_CallMethod_Double(rectObj, AniGlobalMethod::rectStyleRightTopRadius, &rectStyle.rightTopRadius);
        env->Object_CallMethod_Double(
            rectObj, AniGlobalMethod::rectStyleRightBottomRadius, &rectStyle.rightBottomRadius);
        env->Object_CallMethod_Double(rectObj, AniGlobalMethod::rectStyleLeftBottomRadius, &rectStyle.leftBottomRadius);
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

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::textStyle, AniGlobalMethod::textStyleCtor,
        AniTextStyleConverter::ParseDecorationToAni(env, textStyle), aniColorObj,
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::fontWeight,
            aniGetEnumIndex(AniTextEnum::fontWeight, static_cast<uint32_t>(textStyle.fontWeight))),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::fontStyle,
            aniGetEnumIndex(AniTextEnum::fontStyle, static_cast<uint32_t>(textStyle.fontStyle))),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::textBaseline,
            aniGetEnumIndex(AniTextEnum::textBaseLine, static_cast<uint32_t>(textStyle.baseline))),
        AniTextUtils::CreateAniArrayAndInitData(env, textStyle.fontFamilies, textStyle.fontFamilies.size(),
            [](ani_env* env, const std::string& item) { return AniTextUtils::CreateAniStringObj(env, item); }),
        textStyle.fontSize, textStyle.letterSpacing, textStyle.wordSpacing, textStyle.heightScale,
        textStyle.halfLeading, textStyle.heightOnly, AniTextUtils::CreateAniStringObj(env, textStyle.ellipsis),
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::ellipsisMode,
            aniGetEnumIndex(AniTextEnum::ellipsisModal, static_cast<uint32_t>(textStyle.ellipsisModal))),
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

    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::textShadow, AniGlobalMethod::textShadowCtor,
        aniColorObj, aniPointObj, ani_double(textShadow.blurRadius));
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
    ani_object aniObj = AniTextUtils::CreateAniObject(env, AniGlobalClass::decoration, AniGlobalMethod::decorationCtor,
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::textDecorationType,
            aniGetEnumIndex(AniTextEnum::textDecoration, static_cast<uint32_t>(textStyle.decoration))),
        aniColorObj,
        AniTextUtils::CreateAniOptionalEnum(env, AniGlobalEnum::textDecorationStyle,
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
    ani_object aniObj =
        AniTextUtils::CreateAniObject(env, AniGlobalClass::rectStyle, AniGlobalMethod::rectStyleCtor, aniColorObj,
            rectStyle.leftTopRadius, rectStyle.rightTopRadius, rectStyle.rightBottomRadius, rectStyle.leftBottomRadius);
    return aniObj;
}

ani_object AniTextStyleConverter::ParseFontFeaturesToAni(ani_env* env, const FontFeatures& fontFeatures)
{
    const std::vector<std::pair<std::string, int>> featureSet = fontFeatures.GetFontFeatures();
    ani_object arrayObj = AniTextUtils::CreateAniArrayAndInitData(
        env, featureSet, featureSet.size(), [](ani_env* env, const std::pair<std::string, int>& feature) {
            ani_object aniObj =
                AniTextUtils::CreateAniObject(env, AniGlobalClass::fontFeature, AniGlobalMethod::fontFeatureCtor,
                    AniTextUtils::CreateAniStringObj(env, feature.first), ani_int(feature.second));
            return aniObj;
        });
    return arrayObj;
}

ani_object AniTextStyleConverter::ParseFontVariationsToAni(ani_env* env, const FontVariations& fontVariations)
{
    ani_object aniObj =
        AniTextUtils::CreateAniObject(env, AniGlobalClass::fontVariation, AniGlobalMethod::fontVariationCtor);
    return aniObj;
}
} // namespace OHOS::Text::ANI