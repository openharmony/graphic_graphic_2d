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

#include "js_text_utils.h"

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

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message)
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
    napi_value tempValueChild = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        return false;
    }
    int32_t alpha = 0;
    int32_t red = 0;
    int32_t green = 0;
    int32_t blue = 0;
    napi_get_named_property(env, tempValue, "alpha", &tempValueChild);
    bool isAlphaOk = ConvertClampFromJsValue(env, tempValueChild, alpha, 0, Drawing::Color::RGB_MAX);
    napi_get_named_property(env, tempValue, "red", &tempValueChild);
    bool isRedOk = ConvertClampFromJsValue(env, tempValueChild, red, 0, Drawing::Color::RGB_MAX);
    napi_get_named_property(env, tempValue, "green", &tempValueChild);
    bool isGreenOk = ConvertClampFromJsValue(env, tempValueChild, green, 0, Drawing::Color::RGB_MAX);
    napi_get_named_property(env, tempValue, "blue", &tempValueChild);
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

    napi_get_named_property(env, tempValue, "decorationStyle", &tempValueChild);
    uint32_t decorationStyle = 0;
    if (tempValueChild != nullptr && napi_get_value_uint32(env, tempValueChild, &decorationStyle) == napi_ok) {
        textStyle.decorationStyle = TextDecorationStyle(decorationStyle);
    }
    SetDoubleValueFromJS(env, tempValue, "decorationThicknessScale", textStyle.decorationThicknessScale);
    return true;
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
        ROSEN_LOGE("The parameter of font features is unvaild");
        return;
    }

    for (uint32_t further = 0; further < arrayLength; further++) {
        napi_value singleElementValue;
        if (napi_get_element(env, allFeatureValue, further, &singleElementValue) != napi_ok) {
            ROSEN_LOGE("This parameter of the font features is unvaild");
            break;
        }
        napi_value featureElement;
        std::string name;
        if (napi_get_named_property(env, singleElementValue, "name", &featureElement) != napi_ok ||
            !ConvertFromJsValue(env, featureElement, name)) {
            ROSEN_LOGE("This time that the name of parameter in font features is unvaild");
            break;
        }

        int value = 0;
        if (napi_get_named_property(env, singleElementValue, "value", &featureElement) != napi_ok ||
            !ConvertFromJsValue(env, featureElement, value)) {
            ROSEN_LOGE("This time that the value of parameter in font features is unvaild");
            break;
        }
        textStyle.fontFeatures.SetFeature(name, value);
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
}

void ScanShadowValue(napi_env env, napi_value allShadowValue, uint32_t arrayLength, TextStyle& textStyle)
{
    textStyle.shadows.clear();
    for (uint32_t further = 0; further < arrayLength; further++) {
        napi_value element;
        Drawing::Color colorSrc;
        Drawing::Point offset;
        double runTimeRadius = 0;
        if (napi_get_element(env, allShadowValue, further, &element) != napi_ok) {
            ROSEN_LOGE("The parameter of as private text-shadow is unvaild");
            return;
        }
        SetColorFromJS(env, element, "color", colorSrc);

        napi_value pointValue = nullptr;
        if (napi_get_named_property(env, element, "point", &pointValue) != napi_ok) {
            ROSEN_LOGE("The parameter of as private point is unvaild");
            return;
        }
        GetPointFromJsValue(env, pointValue, offset);

        napi_value radius = nullptr;
        if (napi_get_named_property(env, element, "blurRadius", &radius) != napi_ok ||
            napi_get_value_double(env, radius, &runTimeRadius) != napi_ok) {
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
    if (napi_get_array_length(env, allShadowValue, &arrayLength) != napi_ok) {
        ROSEN_LOGE("The parameter of text shadow is not array");
        return;
    }
    ScanShadowValue(env, allShadowValue, arrayLength, textStyle);
    return;
}

bool GetTextStyleFromJS(napi_env env, napi_value argValue, TextStyle& textStyle)
{
    if (argValue == nullptr) {
        return false;
    }
    SetColorFromJS(env, argValue, "color", textStyle.color);
    ParsePartTextStyle(env, argValue, textStyle);
    SetTextShadowProperty(env, argValue, textStyle);
    SetRectStyleFromJS(env, argValue, textStyle.backgroundRect);
    return true;
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

    napi_get_named_property(env, argValue, "textDirection", &tempValue);
    uint32_t textDirection = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &textDirection) == napi_ok) {
        pographyStyle.textDirection = TextDirection(textDirection);
    }

    napi_get_named_property(env, argValue, "align", &tempValue);
    uint32_t align = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &align) == napi_ok) {
        pographyStyle.textAlign = TextAlign(align);
    }

    napi_get_named_property(env, argValue, "wordBreak", &tempValue);
    uint32_t wordBreak = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &wordBreak) == napi_ok) {
        pographyStyle.wordBreakType = WordBreakType(wordBreak);
    }

    napi_get_named_property(env, argValue, "maxLines", &tempValue);
    uint32_t maxLines = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &maxLines) == napi_ok) {
        pographyStyle.maxLines = maxLines;
    }

    napi_get_named_property(env, argValue, "breakStrategy", &tempValue);
    uint32_t breakStrategy = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &breakStrategy) == napi_ok) {
        pographyStyle.breakStrategy = BreakStrategy(breakStrategy);
    }

    SetStrutStyleFromJS(env, argValue, pographyStyle);
    SetEnumValueFromJS(env, argValue, "textHeightBehavior", pographyStyle.textHeightBehavior);

    return true;
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
    }

    napi_get_named_property(env, argValue, "height", &tempValue);
    double height = 0;
    if (tempValue != nullptr && napi_get_value_double(env, tempValue, &height) == napi_ok) {
        placeholderSpan.height = height;
    }

    napi_get_named_property(env, argValue, "align", &tempValue);
    uint32_t align = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &align) == napi_ok) {
        placeholderSpan.alignment = PlaceholderVerticalAlignment(align);
    }

    napi_get_named_property(env, argValue, "baseline", &tempValue);
    uint32_t baseline = 0;
    if (tempValue != nullptr && napi_get_value_uint32(env, tempValue, &baseline) == napi_ok) {
        placeholderSpan.baseline = TextBaseline(baseline);
    }

    napi_get_named_property(env, argValue, "baselineOffset", &tempValue);
    double baselineOffset = 0;
    if (tempValue != nullptr && napi_get_value_double(env, tempValue, &baselineOffset) == napi_ok) {
        placeholderSpan.baselineOffset = baselineOffset;
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

void SetStrutStyleFromJS(napi_env env, napi_value argValue, TypographyStyle& pographyStyle)
{
    if (!argValue) {
        return;
    }

    napi_value strutStyleValue = nullptr;
    if (!GetNamePropertyFromJS(env, argValue, "strutStyle", strutStyleValue)) {
        return;
    }

    napi_value tempValue = nullptr;
    if (GetNamePropertyFromJS(env, strutStyleValue, "fontFamilies", tempValue)) {
        std::vector<std::string> fontFamilies;
        if (tempValue != nullptr && OnMakeFontFamilies(env, tempValue, fontFamilies)) {
            pographyStyle.lineStyleFontFamilies = fontFamilies;
        }
    }

    SetEnumValueFromJS(env, strutStyleValue, "fontStyle", pographyStyle.lineStyleFontStyle);
    SetEnumValueFromJS(env, strutStyleValue, "fontWidth", pographyStyle.lineStyleFontWidth);
    SetEnumValueFromJS(env, strutStyleValue, "fontWeight", pographyStyle.lineStyleFontWeight);

    SetDoubleValueFromJS(env, strutStyleValue, "fontSize", pographyStyle.lineStyleFontSize);
    SetDoubleValueFromJS(env, strutStyleValue, "height", pographyStyle.lineStyleHeightScale);
    SetDoubleValueFromJS(env, strutStyleValue, "leading", pographyStyle.lineStyleSpacingScale);

    SetBoolValueFromJS(env, strutStyleValue, "forceHeight", pographyStyle.lineStyleOnly);
    SetBoolValueFromJS(env, strutStyleValue, "enabled", pographyStyle.useLineStyle);
    SetBoolValueFromJS(env, strutStyleValue, "heightOverride", pographyStyle.lineStyleHeightOnly);
    SetBoolValueFromJS(env, strutStyleValue, "halfLeading", pographyStyle.lineStyleHalfLeading);
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
} // namespace OHOS::Rosen
