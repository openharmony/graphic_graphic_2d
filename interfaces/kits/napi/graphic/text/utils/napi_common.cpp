/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "napi_common.h"

namespace OHOS::Rosen {
void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func)
{
    std::string fullName;
    if (moduleName) {
        fullName = moduleName;
        fullName += '.';
    }
    fullName += name;
    napi_value funcValue = nullptr;
    napi_create_function(env, fullName.c_str(), fullName.size(), func, nullptr, &funcValue);
    napi_set_named_property(env, object, fullName.c_str(), funcValue);
}

napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message)
{
    napi_value result = nullptr;
    napi_create_error(env, CreateJsValue(env, errCode), CreateJsValue(env, message), &result);
    return result;
}

napi_value NapiThrowError(napi_env env, TextErrorCode err, const std::string& message)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), message));
    return NapiGetUndefined(env);
}

bool OnMakeFontFamilies(napi_env& env, napi_value jsValue, std::vector<std::string> &fontFamilies)
{
    if (jsValue == nullptr) {
        return false;
    }
    uint32_t size = 0;
    napi_get_array_length(env, jsValue, &size);
    if (size == 0) {
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        napi_value tempStr = nullptr;
        napi_get_element(env, jsValue, i, &tempStr);
        std::string text = "";
        if (ConvertFromJsValue(env, tempStr, text)) {
            fontFamilies.push_back(text);
        }
    }
    return true;
}

bool SetColorFromJS(napi_env env, napi_value argValue, const std::string& str, Drawing::Color& colorSrc)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    return SetColorFromJS(env, tempValue, colorSrc);
}

bool SetColorFromJS(napi_env env, napi_value argValue, Drawing::Color& colorSrc)
{
    napi_value tempValueChild = nullptr;
    if (argValue == nullptr) {
        return false;
    }

    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, argValue, "alpha", &tempValueChild);
    bool isAlphaOk = ConvertClampFromJsValue(env, tempValueChild, alpha, 0, Drawing::Color::RGB_MAX);
    napi_get_named_property(env, argValue, "red", &tempValueChild);
    bool isRedOk = ConvertClampFromJsValue(env, tempValueChild, red, 0, Drawing::Color::RGB_MAX);
    napi_get_named_property(env, argValue, "green", &tempValueChild);
    bool isGreenOk = ConvertClampFromJsValue(env, tempValueChild, green, 0, Drawing::Color::RGB_MAX);
    napi_get_named_property(env, argValue, "blue", &tempValueChild);
    bool isBlueOk = ConvertClampFromJsValue(env, tempValueChild, blue, 0, Drawing::Color::RGB_MAX);
    if (isAlphaOk && isRedOk && isGreenOk && isBlueOk) {
        Drawing::Color color(Drawing::Color::ColorQuadSetARGB(alpha, red, green, blue));
        colorSrc = color;
        return true;
    }
    return false;
}

bool GetDecorationFromJS(napi_env env, napi_value argValue, const std::string& str, TextStyle& textStyle)
{
    if (argValue == nullptr) {
        return false;
    }
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        return false;
    }

    napi_value tempValueChild = nullptr;
    napi_get_named_property(env, tempValue, "textDecoration", &tempValueChild);
    uint32_t textDecoration = 0;
    if (tempValueChild != nullptr && napi_get_value_uint32(env, tempValueChild, &textDecoration) == napi_ok) {
        textStyle.decoration = TextDecoration(textDecoration);
    }

    SetColorFromJS(env, tempValue, "color", textStyle.decorationColor);

    tempValueChild = nullptr;
    napi_get_named_property(env, tempValue, "decorationStyle", &tempValueChild);
    uint32_t decorationStyle = 0;
    if (tempValueChild != nullptr && napi_get_value_uint32(env, tempValueChild, &decorationStyle) == napi_ok) {
        textStyle.decorationStyle = TextDecorationStyle(decorationStyle);
    }
    SetDoubleValueFromJS(env, tempValue, "decorationThicknessScale", textStyle.decorationThicknessScale);
    return true;
}

bool GetDecorationFromJSForUpdate(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    if (argValue == nullptr) {
        return false;
    }

    napi_value tempValueChild = nullptr;
    napi_get_named_property(env, argValue, "textDecoration", &tempValueChild);
    uint32_t textDecoration = 0;
    if (tempValueChild != nullptr && napi_get_value_uint32(env, tempValueChild, &textDecoration) == napi_ok) {
        textStyle.decoration = TextDecoration(textDecoration);
        textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION));
    }

    if (SetColorFromJS(env, argValue, "color", textStyle.decorationColor)) {
        textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_COLOR));
    }

    tempValueChild = nullptr;
    napi_get_named_property(env, argValue, "decorationStyle", &tempValueChild);
    uint32_t decorationStyle = 0;
    if (tempValueChild != nullptr && napi_get_value_uint32(env, tempValueChild, &decorationStyle) == napi_ok) {
        textStyle.decorationStyle = TextDecorationStyle(decorationStyle);
        textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_STYLE));
    }

    tempValueChild = nullptr;
    napi_get_named_property(env, argValue, "decorationThicknessScale", &tempValueChild);
    double decorationThicknessScale = 1.0;
    if (tempValueChild != nullptr && napi_get_value_double(env, tempValueChild, &decorationThicknessScale) == napi_ok) {
        textStyle.decorationThicknessScale = decorationThicknessScale;
        textStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_THICKNESS_SCALE));
    }
    return true;
}

bool GetNamePropertyFromJS(napi_env env, napi_value argValue, const std::string& str, napi_value& propertyValue)
{
    bool result = false;
    if (napi_has_named_property(env, argValue, str.c_str(), &result) != napi_ok || (!result)) {
        return false;
    }

    if (napi_get_named_property(env, argValue, str.c_str(), &propertyValue) != napi_ok) {
        return false;
    }

    return true;
}

void ReceiveFontFeature(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    napi_value allFeatureValue = nullptr;
    napi_get_named_property(env, argValue, "fontFeatures", &allFeatureValue);
    uint32_t arrayLength = 0;
    if (napi_get_array_length(env, allFeatureValue, &arrayLength) != napi_ok ||
        !arrayLength) {
        TEXT_LOGD("Failed to get font feature");
        return;
    }

    for (uint32_t further = 0; further < arrayLength; further++) {
        napi_value singleElementValue;
        if (napi_get_element(env, allFeatureValue, further, &singleElementValue) != napi_ok) {
            TEXT_LOGE("Failed to get font feature");
            break;
        }
        napi_value featureElement;
        std::string name;
        if (napi_get_named_property(env, singleElementValue, "name", &featureElement) != napi_ok ||
            !ConvertFromJsValue(env, featureElement, name)) {
            TEXT_LOGE("Failed to get name");
            break;
        }

        int value = 0;
        if (napi_get_named_property(env, singleElementValue, "value", &featureElement) != napi_ok ||
            !ConvertFromJsValue(env, featureElement, value)) {
            TEXT_LOGE("Failed to get value");
            break;
        }
        textStyle.fontFeatures.SetFeature(name, value);
    }
    return;
}

void ReceiveFontVariation(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    napi_value allVariationValue = nullptr;
    napi_get_named_property(env, argValue, "fontVariations", &allVariationValue);
    uint32_t arrayLength = 0;
    auto status = napi_get_array_length(env, allVariationValue, &arrayLength);
    if ((status != napi_ok) || (arrayLength == 0)) {
        TEXT_LOGD("Failed to get variations, ret %{public}d", static_cast<int>(status));
        return;
    }

    for (uint32_t further = 0; further < arrayLength; further++) {
        napi_value singleElementValue;
        status = napi_get_element(env, allVariationValue, further, &singleElementValue);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to get variation, ret %{public}d", static_cast<int>(status));
            break;
        }
        napi_value variationElement;
        std::string axis;
        status = napi_get_named_property(env, singleElementValue, "axis", &variationElement);
        if ((status != napi_ok) || !ConvertFromJsValue(env, variationElement, axis)) {
            TEXT_LOGE("Failed to get axis, ret %{public}d", static_cast<int>(status));
            break;
        }

        int value = 0;
        status = napi_get_named_property(env, singleElementValue, "value", &variationElement);
        if ((status != napi_ok) || !ConvertFromJsValue(env, variationElement, value)) {
            TEXT_LOGE("Failed to get value, ret %{public}d", static_cast<int>(status));
            break;
        }
        textStyle.fontVariations.SetAxisValue(axis, value);
    }
    return;
}

void SetTextStyleBaseType(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    SetDoubleValueFromJS(env, argValue, "letterSpacing", textStyle.letterSpacing);
    SetDoubleValueFromJS(env, argValue, "wordSpacing", textStyle.wordSpacing);
    SetDoubleValueFromJS(env, argValue, "baselineShift", textStyle.baseLineShift);
    SetDoubleValueFromJS(env, argValue, "heightScale", textStyle.heightScale);
    SetBoolValueFromJS(env, argValue, "halfLeading", textStyle.halfLeading);
    SetBoolValueFromJS(env, argValue, "heightOnly", textStyle.heightOnly);

    textStyle.heightScale = textStyle.heightScale < 0 ? 0 : textStyle.heightScale;
}

void ScanShadowValue(napi_env env, napi_value allShadowValue, uint32_t arrayLength, TextStyle& textStyle)
{
    textStyle.shadows.clear();
    for (uint32_t further = 0; further < arrayLength; further++) {
        napi_value element;
        Drawing::Color colorSrc = OHOS::Rosen::Drawing::Color::COLOR_BLACK;
        Drawing::Point offset(0, 0);
        double runTimeRadius = 0;
        auto status = napi_get_element(env, allShadowValue, further, &element);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to get shadow, ret %{public}d", static_cast<int>(status));
            return;
        }
        SetColorFromJS(env, element, "color", colorSrc);

        napi_value pointValue = nullptr;
        status = napi_get_named_property(env, element, "point", &pointValue);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to get point, ret %{public}d", static_cast<int>(status));
            return;
        }
        GetPointFromJsValue(env, pointValue, offset);

        napi_value radius = nullptr;
        status = napi_get_named_property(env, element, "blurRadius", &radius);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to get blur radius, ret %{public}d", static_cast<int>(status));
            return;
        }
        status = napi_get_value_double(env, radius, &runTimeRadius);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to get radius, ret %{public}d", static_cast<int>(status));
            return;
        }
        textStyle.shadows.emplace_back(TextShadow(colorSrc, offset, runTimeRadius));
    }
    return;
}

void SetTextShadowProperty(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    napi_value allShadowValue = nullptr;
    if (!GetNamePropertyFromJS(env, argValue, "textShadows", allShadowValue)) {
        return;
    }

    uint32_t arrayLength = 0;
    auto status = napi_get_array_length(env, allShadowValue, &arrayLength);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get shadow array length, ret %{public}d", static_cast<int>(status));
        return;
    }
    ScanShadowValue(env, allShadowValue, arrayLength, textStyle);
    return;
}

void ParsePartTextStyle(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, "fontWeight", &tempValue);
    uint32_t fontWeight = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &fontWeight) == napi_ok) {
        textStyle.fontWeight = FontWeight(fontWeight);
    }
    napi_get_named_property(env, argValue, "fontStyle", &tempValue);
    uint32_t fontStyle = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &fontStyle) == napi_ok) {
        textStyle.fontStyle = FontStyle(fontStyle);

        // Let OBLIQUE be equal to ITALIC, it's a temp modify.
        if (textStyle.fontStyle == FontStyle::OBLIQUE) {
            textStyle.fontStyle = FontStyle::ITALIC;
        }
    }
    napi_get_named_property(env, argValue, "baseline", &tempValue);
    uint32_t baseline = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &baseline) == napi_ok) {
        textStyle.baseline = TextBaseline(baseline);
    }
    SetDoubleValueFromJS(env, argValue, "fontSize", textStyle.fontSize);

    std::vector<std::string> fontFamilies;
    napi_get_named_property(env, argValue, "fontFamilies", &tempValue);
    if (tempValue != nullptr && OnMakeFontFamilies(env, tempValue, fontFamilies)) {
        textStyle.fontFamilies = fontFamilies;
    }
    GetDecorationFromJS(env, argValue, "decoration", textStyle);
    SetTextStyleBaseType(env, argValue, textStyle);
    ReceiveFontFeature(env, argValue, textStyle);
    ReceiveFontVariation(env, argValue, textStyle);
    napi_get_named_property(env, argValue, "ellipsis", &tempValue);
    std::string text = "";
    if (tempValue != nullptr && ConvertFromJsValue(env, tempValue, text)) {
        textStyle.ellipsis = Str8ToStr16(text);
    }
    napi_get_named_property(env, argValue, "ellipsisMode", &tempValue);
    uint32_t ellipsisModal = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &ellipsisModal)== napi_ok) {
        textStyle.ellipsisModal = EllipsisModal(ellipsisModal);
    }
    napi_get_named_property(env, argValue, "locale", &tempValue);
    std::string textLocale = "";
    if (tempValue != nullptr && ConvertFromJsValue(env, tempValue, textLocale)) {
        textStyle.locale = textLocale;
    }
}

bool GetTextStyleFromJS(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    napi_valuetype valueType = napi_undefined;
    if (argValue == nullptr || napi_typeof(env, argValue, &valueType) != napi_ok || valueType != napi_object) {
        return false;
    }
    SetColorFromJS(env, argValue, "color", textStyle.color);
    ParsePartTextStyle(env, argValue, textStyle);
    SetTextShadowProperty(env, argValue, textStyle);
    SetRectStyleFromJS(env, argValue, textStyle.backgroundRect);
    return true;
}

static void SetAlignValueForParagraphStyle(napi_env env, napi_value argValue, TypographyStyle& pographyStyle)
{
    if (argValue == nullptr) {
        return;
    }
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, "align", &tempValue);
    uint32_t align = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &align) == napi_ok) {
        pographyStyle.textAlign = TextAlign(align);
    }
}

bool GetParagraphStyleFromJS(napi_env env, napi_value argValue, TypographyStyle& pographyStyle)
{
    if (argValue == nullptr) {
        return false;
    }
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, "textStyle", &tempValue);
    TextStyle textStyle;
    if (tempValue != nullptr && GetTextStyleFromJS(env, tempValue, textStyle)) {
        pographyStyle.SetTextStyle(textStyle);
    }
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "textDirection", &tempValue);
    uint32_t textDirection = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &textDirection) == napi_ok) {
        pographyStyle.textDirection = TextDirection(textDirection);
    }
    SetAlignValueForParagraphStyle(env, argValue, pographyStyle);
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "wordBreak", &tempValue);
    uint32_t wordBreak = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &wordBreak) == napi_ok) {
        pographyStyle.wordBreakType = WordBreakType(wordBreak);
    }
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "maxLines", &tempValue);
    int64_t maxLines = 0;
    if (tempValue != nullptr && napi_get_value_int64(env, tempValue, &maxLines) == napi_ok) {
        pographyStyle.maxLines = maxLines < 0 ? 0 : maxLines;
    }
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "breakStrategy", &tempValue);
    uint32_t breakStrategy = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &breakStrategy) == napi_ok) {
        pographyStyle.breakStrategy = BreakStrategy(breakStrategy);
    }

    napi_value strutStyleValue = nullptr;
    if (GetNamePropertyFromJS(env, argValue, "strutStyle", strutStyleValue)) {
        SetStrutStyleFromJS(env, strutStyleValue, pographyStyle);
    }

    pographyStyle.ellipsis = textStyle.ellipsis;
    pographyStyle.ellipsisModal = textStyle.ellipsisModal;

    napi_get_named_property(env, argValue, "tab", &tempValue);
    TextTab textTab;
    if (tempValue != nullptr && GetTextTabFromJS(env, tempValue, textTab)) {
        pographyStyle.tab = textTab;
    }
    HandleExtentParagraphStyleProperties(env, argValue, pographyStyle);
    return true;
}

void HandleExtentParagraphStyleProperties(napi_env env, napi_value argValue, TypographyStyle& pographyStyle)
{
    SetEnumValueFromJS(env, argValue, "textHeightBehavior", pographyStyle.textHeightBehavior);
    SetBoolValueFromJS(env, argValue, "trailingSpaceOptimized", pographyStyle.isTrailingSpaceOptimized);
    SetBoolValueFromJS(env, argValue, "autoSpace", pographyStyle.enableAutoSpace);
}

bool GetPlaceholderSpanFromJS(napi_env env, napi_value argValue, PlaceholderSpan& placeholderSpan)
{
    if (argValue == nullptr) {
        return false;
    }
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, "width", &tempValue);
    double width = 0;
    if (tempValue != nullptr && napi_get_value_double(env, tempValue, &width) == napi_ok) {
        placeholderSpan.width = width;
    } else {
        return false;
    }
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "height", &tempValue);
    double height = 0;
    if (tempValue != nullptr && napi_get_value_double(env, tempValue, &height) == napi_ok) {
        placeholderSpan.height = height;
    } else {
        return false;
    }
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "align", &tempValue);
    uint32_t align = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &align) == napi_ok) {
        placeholderSpan.alignment = PlaceholderVerticalAlignment(align);
    } else {
        return false;
    }
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "baseline", &tempValue);
    uint32_t baseline = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &baseline) == napi_ok) {
        placeholderSpan.baseline = TextBaseline(baseline);
    } else {
        return false;
    }
    tempValue = nullptr;
    napi_get_named_property(env, argValue, "baselineOffset", &tempValue);
    double baselineOffset = 0;
    if (tempValue != nullptr && napi_get_value_double(env, tempValue, &baselineOffset) == napi_ok) {
        placeholderSpan.baselineOffset = baselineOffset;
    } else {
        return false;
    }
    return true;
}

size_t GetParamLen(napi_env env, napi_value param)
{
    size_t buffSize = 0;
    napi_status status = napi_get_value_string_utf8(env, param, nullptr, 0, &buffSize);
    if (status != napi_ok || buffSize == 0) {
        return 0;
    }
    return buffSize;
}

bool GetFontMetricsFromJS(napi_env env, napi_value argValue, Drawing::FontMetrics& fontMetrics)
{
    if (argValue == nullptr) {
        return false;
    }
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, "flags", &tempValue);
    uint32_t flags = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &flags) == napi_ok) {
        fontMetrics.fFlags = Drawing::FontMetrics::FontMetricsFlags(flags);
    }
    SetFontMetricsFloatValueFromJS(env, argValue, "top", fontMetrics.fTop);
    SetFontMetricsFloatValueFromJS(env, argValue, "ascent", fontMetrics.fAscent);
    SetFontMetricsFloatValueFromJS(env, argValue, "descent", fontMetrics.fDescent);
    SetFontMetricsFloatValueFromJS(env, argValue, "bottom", fontMetrics.fBottom);
    SetFontMetricsFloatValueFromJS(env, argValue, "leading", fontMetrics.fLeading);
    SetFontMetricsFloatValueFromJS(env, argValue, "avgCharWidth", fontMetrics.fAvgCharWidth);
    SetFontMetricsFloatValueFromJS(env, argValue, "maxCharWidth", fontMetrics.fMaxCharWidth);
    SetFontMetricsFloatValueFromJS(env, argValue, "xMin", fontMetrics.fXMin);
    SetFontMetricsFloatValueFromJS(env, argValue, "xMax", fontMetrics.fXMax);
    SetFontMetricsFloatValueFromJS(env, argValue, "xHeight", fontMetrics.fXHeight);
    SetFontMetricsFloatValueFromJS(env, argValue, "capHeight", fontMetrics.fCapHeight);
    SetFontMetricsFloatValueFromJS(env, argValue, "underlineThickness", fontMetrics.fUnderlineThickness);
    SetFontMetricsFloatValueFromJS(env, argValue, "underlinePosition", fontMetrics.fUnderlinePosition);
    SetFontMetricsFloatValueFromJS(env, argValue, "strikethroughThickness", fontMetrics.fStrikeoutThickness);
    SetFontMetricsFloatValueFromJS(env, argValue, "strikethroughPosition", fontMetrics.fStrikeoutPosition);
    return true;
}

bool SetStrutStyleFromJS(napi_env env, napi_value strutStyleValue, TypographyStyle& typographyStyle)
{
    napi_valuetype valueType = napi_undefined;
    if (strutStyleValue == nullptr || napi_typeof(env, strutStyleValue, &valueType) != napi_ok ||
        valueType != napi_object) {
        TEXT_LOGE("Invalid strut style value");
        return false;
    }

    napi_value tempValue = nullptr;
    if (GetNamePropertyFromJS(env, strutStyleValue, "fontFamilies", tempValue)) {
        std::vector<std::string> fontFamilies;
        if (tempValue != nullptr && OnMakeFontFamilies(env, tempValue, fontFamilies)) {
            typographyStyle.lineStyleFontFamilies = fontFamilies;
        }
    }
 
    SetEnumValueFromJS(env, strutStyleValue, "fontStyle", typographyStyle.lineStyleFontStyle);
    SetEnumValueFromJS(env, strutStyleValue, "fontWidth", typographyStyle.lineStyleFontWidth);
    SetEnumValueFromJS(env, strutStyleValue, "fontWeight", typographyStyle.lineStyleFontWeight);
 
    SetDoubleValueFromJS(env, strutStyleValue, "fontSize", typographyStyle.lineStyleFontSize);
    SetDoubleValueFromJS(env, strutStyleValue, "height", typographyStyle.lineStyleHeightScale);
    SetDoubleValueFromJS(env, strutStyleValue, "leading", typographyStyle.lineStyleSpacingScale);
 
    SetBoolValueFromJS(env, strutStyleValue, "forceHeight", typographyStyle.lineStyleOnly);
    SetBoolValueFromJS(env, strutStyleValue, "enabled", typographyStyle.useLineStyle);
    SetBoolValueFromJS(env, strutStyleValue, "heightOverride", typographyStyle.lineStyleHeightOnly);
    SetBoolValueFromJS(env, strutStyleValue, "halfLeading", typographyStyle.lineStyleHalfLeading);
    return true;
}

void SetRectStyleFromJS(napi_env env, napi_value argValue, RectStyle& rectStyle)
{
    if (!argValue) {
        return;
    }

    napi_value tempValue = nullptr;
    if (!GetNamePropertyFromJS(env, argValue, "backgroundRect", tempValue)) {
        return;
    }

    Drawing::Color color;
    SetColorFromJS(env, tempValue, "color", color);
    rectStyle.color = color.CastToColorQuad();
    SetDoubleValueFromJS(env, tempValue, "leftTopRadius", rectStyle.leftTopRadius);
    SetDoubleValueFromJS(env, tempValue, "rightTopRadius", rectStyle.rightTopRadius);
    SetDoubleValueFromJS(env, tempValue, "rightBottomRadius", rectStyle.rightBottomRadius);
    SetDoubleValueFromJS(env, tempValue, "leftBottomRadius", rectStyle.leftBottomRadius);
}

napi_value CreateLineMetricsJsValue(napi_env env, OHOS::Rosen::LineMetrics& lineMetrics)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "startIndex", CreateJsNumber(env, (uint32_t)lineMetrics.startIndex));
        napi_set_named_property(env, objValue, "endIndex", CreateJsNumber(env, (uint32_t)lineMetrics.endIndex));
        napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, lineMetrics.ascender));
        napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, lineMetrics.descender));
        napi_set_named_property(env, objValue, "height", CreateJsNumber(env, lineMetrics.height));
        napi_set_named_property(env, objValue, "width", CreateJsNumber(env, lineMetrics.width));
        napi_set_named_property(env, objValue, "left", CreateJsNumber(env, lineMetrics.x));
        napi_set_named_property(env, objValue, "baseline", CreateJsNumber(env, lineMetrics.baseline));
        napi_set_named_property(env, objValue, "lineNumber", CreateJsNumber(env, lineMetrics.lineNumber));
        napi_set_named_property(env, objValue, "topHeight", CreateJsNumber(env, lineMetrics.y));
        napi_set_named_property(env, objValue, "runMetrics", ConvertMapToNapiMap(env, lineMetrics.runMetrics));
    }
    return objValue;
}

napi_value CreateShadowArrayJsValue(napi_env env, const std::vector<TextShadow>& textShadows)
{
    napi_value jsArray = nullptr;
    napi_status arrayStatus = napi_create_array_with_length(env, textShadows.size(), &jsArray);
    if (arrayStatus != napi_ok) {
        TEXT_LOGE("Failed to create text Shadows array, ret %{public}d", arrayStatus);
        return nullptr;
    }
    size_t index = 0;
    for (const auto& shadow : textShadows) {
        if (!shadow.HasShadow()) {
            continue;
        }
        napi_value shadowObj = nullptr;
        napi_status status = napi_create_object(env, &shadowObj);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to create shadow object, ret %{public}d", status);
            continue;
        }
        napi_set_named_property(
            env, shadowObj, "color", CreateJsNumber(env, (uint32_t)shadow.color.CastToColorQuad()));
        napi_set_named_property(
            env, shadowObj, "point", CreatePointJsValue(env, (OHOS::Rosen::Drawing::PointF)shadow.offset));
        napi_set_named_property(env, shadowObj, "blurRadius", CreateJsNumber(env, shadow.blurRadius));
        status = napi_set_element(env, jsArray, index, shadowObj);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set shadow in textShadows, ret %{public}d", status);
            continue;
        }
        index++;
    }
    
    return jsArray;
}

napi_value CreatePointJsValue(napi_env env, const OHOS::Rosen::Drawing::PointF& point)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "x", CreateJsNumber(env, point.GetX()));
        napi_set_named_property(env, objValue, "y", CreateJsNumber(env, point.GetY()));
    }
    return objValue;
}

napi_value CreateRectStyleJsValue(napi_env env, RectStyle& rectStyle)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "color", CreateJsNumber(env, (uint32_t)rectStyle.color));
        napi_set_named_property(env, objValue, "leftTopRadius", CreateJsNumber(env, rectStyle.leftTopRadius));
        napi_set_named_property(env, objValue, "rightTopRadius", CreateJsNumber(env, rectStyle.rightTopRadius));
        napi_set_named_property(env, objValue, "rightBottomRadius", CreateJsNumber(env, rectStyle.rightBottomRadius));
        napi_set_named_property(env, objValue, "leftBottomRadius", CreateJsNumber(env, rectStyle.leftBottomRadius));
    }
    return objValue;
}

napi_value CreateFontFeatureArrayJsValue(napi_env env, const FontFeatures& fontFeatures)
{
    napi_value jsArray;
    napi_status arrayStatus = napi_create_array(env, &jsArray);
    if (arrayStatus != napi_ok) {
        TEXT_LOGE("Failed to create fontFeature array, ret %{public}d", arrayStatus);
        return nullptr;
    }
    const std::vector<std::pair<std::string, int>>& featureSet = fontFeatures.GetFontFeatures();
    for (size_t i = 0; i < featureSet.size(); ++i) {
        const auto& feature = featureSet[i];
        napi_value jsObject;
        napi_status status = napi_create_object(env, &jsObject);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to create fontFeature, ret %{public}d", status);
            continue;
        }
        napi_set_named_property(env, jsObject, "name", CreateStringJsValue(env, Str8ToStr16(feature.first)));
        napi_set_named_property(env, jsObject, "value", CreateJsNumber(env, feature.second));
        napi_set_element(env, jsArray, i, jsObject);
        status = napi_set_element(env, jsArray, i, jsObject);
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set fontFeature, ret %{public}d", status);
            continue;
        }
    }

    return jsArray;
}

napi_value CreateDecrationJsValue(napi_env env, TextStyle textStyle)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(
            env, objValue, "textDecoration", CreateJsNumber(env, static_cast<uint32_t>(textStyle.decoration)));
        napi_set_named_property(
            env, objValue, "color", CreateJsNumber(env, (uint32_t)textStyle.decorationColor.CastToColorQuad()));
        napi_set_named_property(
            env, objValue, "decorationStyle", CreateJsNumber(env, static_cast<uint32_t>(textStyle.decorationStyle)));
        napi_set_named_property(
            env, objValue, "decorationThicknessScale", CreateJsNumber(env, textStyle.decorationThicknessScale));
    }

    return objValue;
}

napi_value CreateTextStyleJsValue(napi_env env, TextStyle textStyle)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "decoration", CreateDecrationJsValue(env, textStyle));
        napi_set_named_property(env, objValue, "color", CreateJsNumber(env,
            (uint32_t)textStyle.color.CastToColorQuad()));
        napi_set_named_property(env, objValue, "fontWeight", CreateJsNumber(
            env, static_cast<uint32_t>(textStyle.fontWeight)));
        napi_set_named_property(env, objValue, "fontStyle", CreateJsNumber(
            env, static_cast<uint32_t>(textStyle.fontStyle)));
        napi_set_named_property(env, objValue, "baseline", CreateJsNumber(
            env, static_cast<uint32_t>(textStyle.baseline)));
        napi_set_named_property(env, objValue, "fontFamilies", CreateArrayStringJsValue(env, textStyle.fontFamilies));
        napi_set_named_property(env, objValue, "fontSize", CreateJsNumber(env, textStyle.fontSize));
        napi_set_named_property(env, objValue, "letterSpacing", CreateJsNumber(env, textStyle.letterSpacing));
        napi_set_named_property(env, objValue, "wordSpacing", CreateJsNumber(env, textStyle.wordSpacing));
        napi_set_named_property(env, objValue, "heightScale", CreateJsNumber(env, textStyle.heightScale));
        napi_set_named_property(env, objValue, "halfLeading", CreateJsValue(env, textStyle.halfLeading));
        napi_set_named_property(env, objValue, "heightOnly", CreateJsValue(env, textStyle.heightOnly));
        napi_set_named_property(env, objValue, "ellipsis", CreateStringJsValue(env, textStyle.ellipsis));
        napi_set_named_property(env, objValue, "ellipsisMode", CreateJsNumber(
            env, static_cast<uint32_t>(textStyle.ellipsisModal)));
        napi_set_named_property(env, objValue, "locale", CreateJsValue(env, textStyle.locale));
        napi_set_named_property(env, objValue, "baselineShift", CreateJsNumber(env, textStyle.baseLineShift));
        napi_set_named_property(env, objValue, "backgroundRect", CreateRectStyleJsValue(env, textStyle.backgroundRect));
        napi_set_named_property(env, objValue, "textShadows", CreateShadowArrayJsValue(env, textStyle.shadows));
        napi_set_named_property(
            env, objValue, "fontFeatures", CreateFontFeatureArrayJsValue(env, textStyle.fontFeatures));
    }
    return objValue;
}

struct NapiMap {
    napi_value instance;
    napi_value set_function;
};

static NapiMap CreateNapiMap(napi_env env)
{
    NapiMap res = {nullptr, nullptr};
    napi_valuetype value_type;

    napi_value global = nullptr;
    if (napi_get_global(env, &global) != napi_ok || !global) {
        return res;
    }

    napi_value constructor = nullptr;
    if (napi_get_named_property(env, global, "Map", &constructor) != napi_ok || !constructor) {
        return res;
    }

    if (napi_typeof(env, constructor, &value_type) != napi_ok || value_type != napi_valuetype::napi_function) {
        return res;
    }

    napi_value map_instance = nullptr;
    if (napi_new_instance(env, constructor, 0, nullptr, &map_instance) != napi_ok || !map_instance) {
        return res;
    }

    napi_value map_set = nullptr;
    if (napi_get_named_property(env, map_instance, "set", &map_set) != napi_ok || !map_set) {
        return res;
    }
    if (napi_typeof(env, map_set, &value_type) != napi_ok || value_type != napi_valuetype::napi_function) {
        return res;
    }

    res.instance = map_instance;
    res.set_function = map_set;

    return res;
}

static bool NapiMapSet(napi_env env, NapiMap& map, uint32_t key, const RunMetrics& runMetrics)
{
    napi_value keyValue = nullptr;
    keyValue = CreateJsNumber(env, key);
    napi_value runMetricsValue = nullptr;
    runMetricsValue = CreateRunMetricsJsValue(env, runMetrics);
    if (!keyValue || !runMetricsValue) {
        return false;
    }
    napi_value args[2] = {keyValue, runMetricsValue};
    napi_status status = napi_call_function(env, map.instance, map.set_function, 2, args, nullptr);
    if (status != napi_ok) {
        return false;
    }
    return true;
}

napi_value ConvertMapToNapiMap(napi_env env, const std::map<size_t, RunMetrics>& map)
{
    auto mapReturn = CreateNapiMap(env);
    for (const auto &[key, val] : map) {
        NapiMapSet(env, mapReturn, static_cast<uint32_t>(key), val);
    }
    return mapReturn.instance;
}
 
napi_value CreateFontMetricsJsValue(napi_env env, Drawing::FontMetrics& fontMetrics)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "flags", CreateJsNumber(env, fontMetrics.fFlags));
        napi_set_named_property(env, objValue, "top", CreateJsNumber(env, fontMetrics.fTop)); // float type
        napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, fontMetrics.fAscent));
        napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, fontMetrics.fDescent));
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, fontMetrics.fBottom));
        napi_set_named_property(env, objValue, "leading", CreateJsNumber(env, fontMetrics.fLeading));
        napi_set_named_property(env, objValue, "avgCharWidth", CreateJsNumber(env, fontMetrics.fAvgCharWidth));
        napi_set_named_property(env, objValue, "maxCharWidth", CreateJsNumber(env, fontMetrics.fMaxCharWidth));
        napi_set_named_property(env, objValue, "xMin", CreateJsNumber(env, fontMetrics.fXMin));
        napi_set_named_property(env, objValue, "xMax", CreateJsNumber(env, fontMetrics.fXMax));
        napi_set_named_property(env, objValue, "xHeight", CreateJsNumber(env, fontMetrics.fXHeight));
        napi_set_named_property(env, objValue, "capHeight", CreateJsNumber(env, fontMetrics.fCapHeight));
        napi_set_named_property(env, objValue, "underlineThickness", CreateJsNumber(env,
            fontMetrics.fUnderlineThickness));
        napi_set_named_property(env, objValue, "underlinePosition", CreateJsNumber(env,
            fontMetrics.fUnderlinePosition));
        napi_set_named_property(env, objValue, "strikethroughThickness", CreateJsNumber(env,
            fontMetrics.fStrikeoutThickness));
        napi_set_named_property(env, objValue, "strikethroughPosition", CreateJsNumber(env,
            fontMetrics.fStrikeoutPosition));
    }
    return objValue;
}

napi_value GetFontMetricsAndConvertToJsValue(napi_env env, Drawing::FontMetrics* metrics)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (metrics != nullptr && objValue != nullptr) {
        napi_set_named_property(env, objValue, "top", CreateJsNumber(env, metrics->fTop));
        napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, metrics->fAscent));
        napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, metrics->fDescent));
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, metrics->fBottom));
        napi_set_named_property(env, objValue, "leading", CreateJsNumber(env, metrics->fLeading));
        napi_set_named_property(env, objValue, "flags", CreateJsNumber(env, metrics->fFlags));
        napi_set_named_property(env, objValue, "avgCharWidth", CreateJsNumber(env, metrics->fAvgCharWidth));
        napi_set_named_property(env, objValue, "maxCharWidth", CreateJsNumber(env, metrics->fMaxCharWidth));
        napi_set_named_property(env, objValue, "xMin", CreateJsNumber(env, metrics->fXMin));
        napi_set_named_property(env, objValue, "xMax", CreateJsNumber(env, metrics->fXMax));
        napi_set_named_property(env, objValue, "xHeight", CreateJsNumber(env, metrics->fXHeight));
        napi_set_named_property(env, objValue, "capHeight", CreateJsNumber(env, metrics->fCapHeight));
        napi_set_named_property(env, objValue, "underlineThickness", CreateJsNumber(env,
            metrics->fUnderlineThickness));
        napi_set_named_property(env, objValue, "underlinePosition", CreateJsNumber(env,
            metrics->fUnderlinePosition));
        napi_set_named_property(env, objValue, "strikethroughThickness", CreateJsNumber(env,
            metrics->fStrikeoutThickness));
        napi_set_named_property(env, objValue, "strikethroughPosition", CreateJsNumber(env,
            metrics->fStrikeoutPosition));
    }
    return objValue;
}

bool GetTextTabFromJS(napi_env env, napi_value argValue, TextTab& tab)
{
    if (argValue == nullptr) {
        return false;
    }
    napi_value tempValue = nullptr;
    if (napi_get_named_property(env, argValue, "alignment", &tempValue) != napi_ok) {
        TEXT_LOGE("Failed to get alignment");
        return false;
    }
    uint32_t align = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &align) == napi_ok) {
        TextAlign textAlign;
        switch (TextAlign(align)) {
            case TextAlign::LEFT: {
                textAlign = TextAlign::LEFT;
                break;
            }
            case TextAlign::RIGHT: {
                textAlign = TextAlign::RIGHT;
                break;
            }
            case TextAlign::CENTER: {
                textAlign = TextAlign::CENTER;
                break;
            }
            default: {
                textAlign = TextAlign::LEFT;
                break;
            }
        }
        tab.alignment = textAlign;
    } else {
        TEXT_LOGE("Invalid alignment");
        return false;
    }

    double location = 0;
    if (napi_get_named_property(env, argValue, "location", &tempValue) != napi_ok) {
        return false;
    }
    if (tempValue != nullptr && napi_get_value_double(env, tempValue, &location) == napi_ok) {
        tab.location = location;
    } else {
        TEXT_LOGE("Invalid location");
        return false;
    }
    return true;
}

bool NapiValueTypeIsValid(napi_env env, napi_value argValue)
{
    napi_valuetype valueType;
    if (napi_typeof(env, argValue, &valueType) != napi_ok || valueType == napi_null || valueType == napi_undefined) {
        TEXT_LOGE("Invalid value type %{public}d", static_cast<int32_t>(valueType));
        return false;
    }
    return true;
}

napi_value GetTypographicBoundsAndConvertToJsValue(napi_env env, float ascent,
    float descent, float leading, float width)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_status status = napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, ascent));
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set named property, ret %{public}d", status);
            return nullptr;
        }
        status = napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, descent));
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set named property, ret %{public}d", status);
            return nullptr;
        }
        status = napi_set_named_property(env, objValue, "leading", CreateJsNumber(env, leading));
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set named property, ret %{public}d", status);
            return nullptr;
        }
        status = napi_set_named_property(env, objValue, "width", CreateJsNumber(env, width));
        if (status != napi_ok) {
            TEXT_LOGE("Failed to set named property, ret %{public}d", status);
            return nullptr;
        }
    }
    return objValue;
}

bool GetStartEndParams(napi_env env, napi_value arg, int64_t &start, int64_t &end)
{
    napi_valuetype valueType = napi_undefined;
    if (arg == nullptr || napi_typeof(env, arg, &valueType) != napi_ok || valueType != napi_object) {
        TEXT_LOGE("Invalid arg");
        return false;
    }

    napi_value tempValue = nullptr;
    auto status = napi_get_named_property(env, arg, "start", &tempValue);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get start, ret %{public}d", static_cast<int>(status));
        return false;
    }
    bool isStartOk = ConvertFromJsValue(env, tempValue, start);

    status = napi_get_named_property(env, arg, "end", &tempValue);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get end, ret %{public}d", static_cast<int>(status));
        return false;
    }
    bool isEndOk = ConvertFromJsValue(env, tempValue, end);
    if (!isStartOk || !isEndOk || start < 0 || end < 0) {
        TEXT_LOGE("Invalid parameter, is start %{public}d, is end %{public}d, start %{public}" PRId64
        ", end %{public}" PRId64,
            isStartOk, isEndOk, start, end);
        return false;
    }

    return true;
}
} // namespace OHOS::Rosen
