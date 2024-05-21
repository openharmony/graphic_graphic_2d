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

#ifndef OHOS_JS_TEXT_UTILS_H
#define OHOS_JS_TEXT_UTILS_H

#include <map>
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "utils/point.h"

#include "draw/color.h"
#include "js_drawing_utils.h"
#include "line_metrics.h"
#include "text_style.h"
#include "typography.h"
#include "typography_create.h"
#include "typography_style.h"
#include <codecvt>

namespace OHOS::Rosen {
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr size_t ARGC_FOUR = 4;
constexpr size_t ARGC_FIVE = 5;
constexpr size_t ARGC_SIX = 6;

struct ResourceInfo {
    int32_t resId = 0;
    int32_t type = 0;
    std::vector<std::string> params;
    std::string bundleName;
    std::string moduleName;
};

enum class ResourceType {
    COLOR = 10001,
    FLOAT,
    STRING,
    PLURAL,
    BOOLEAN,
    INTARRAY,
    INTEGER,
    PATTERN,
    STRARRAY,
    MEDIA = 20000,
    RAWFILE = 30000
};

enum class DrawingErrorCode : int32_t {
    OK = 0,
    ERROR_NO_PERMISSION = 201, // the value do not change. It is defined on all system
    ERROR_INVALID_PARAM = 401, // the value do not change. It is defined on all system
    ERROR_DEVICE_NOT_SUPPORT = 801, // the value do not change. It is defined on all system
    ERROR_ABNORMAL_PARAM_VALUE = 18600001, // the value do not change. It is defined on color manager system
};

template<class T>
T* CheckParamsAndGetThis(const napi_env env, napi_callback_info info, const char* name = nullptr)
{
    if (env == nullptr || info == nullptr) {
        return nullptr;
    }
    napi_value object = nullptr;
    napi_value propertyNameValue = nullptr;
    napi_value pointerValue = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &object, nullptr);
    if (object != nullptr && name != nullptr) {
        napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &propertyNameValue);
    }
    napi_value& resObject = propertyNameValue ? propertyNameValue : object;
    if (resObject) {
        return napi_unwrap(env, resObject, (void **)(&pointerValue)) == napi_ok ?
            reinterpret_cast<T*>(pointerValue) : nullptr;
    }
    return nullptr;
}

template<typename T, size_t N>
inline constexpr size_t ArraySize(T (&)[N]) noexcept
{
    return N;
}

inline napi_value CreateJsUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

inline napi_value CreateJsNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, int32_t value)
{
    napi_value result = nullptr;
    napi_create_int32(env, value, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, uint32_t value)
{
    napi_value result = nullptr;
    napi_create_uint32(env, value, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, int64_t value)
{
    napi_value result = nullptr;
    napi_create_int64(env, value, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, uint64_t value)
{
    napi_value result = nullptr;
    napi_create_int64(env, value, &result);
    return result;
}

inline napi_value CreateJsNumber(napi_env env, double value)
{
    napi_value result = nullptr;
    napi_create_double(env, value, &result);
    return result;
}

template<class T>
napi_value CreateJsValue(napi_env env, const T& value)
{
    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    napi_value result = nullptr;
    if constexpr (std::is_same_v<ValueType, bool>) {
        napi_get_boolean(env, value, &result);
        return result;
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        return CreateJsNumber(env, value);
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        napi_create_string_utf8(env, value.c_str(), value.length(), &result);
        return result;
    } else if constexpr (std::is_enum_v<ValueType>) {
        return CreateJsNumber(env, static_cast<std::make_signed_t<ValueType>>(value));
    } else if constexpr (std::is_same_v<ValueType, const char*>) {
        (value != nullptr) ? napi_create_string_utf8(env, value, strlen(value), &result) :
            napi_get_undefined(env, &result);
        return result;
    }
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, int32_t& value)
{
    return napi_get_value_int32(env, jsValue, &value) == napi_ok;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, uint32_t& value)
{
    return napi_get_value_uint32(env, jsValue, &value) == napi_ok;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, int64_t& value)
{
    return napi_get_value_int64(env, jsValue, &value) == napi_ok;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, uint64_t& value)
{
    int64_t num;
    auto res = napi_get_value_int64(env, jsValue, &num);
    if (res == napi_ok) {
        value = static_cast<uint64_t>(num);
    }
    return res == napi_ok;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, double& value)
{
    return napi_get_value_double(env, jsValue, &value) == napi_ok;
}

inline bool ConvertFromJsNumber(napi_env env, napi_value jsValue, bool& value)
{
    return napi_get_value_bool(env, jsValue, &value) == napi_ok;
}

template<class T>
bool ConvertFromJsValue(napi_env env, napi_value jsValue, T& value)
{
    if (jsValue == nullptr) {
        return false;
    }

    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_same_v<ValueType, bool>) {
        return napi_get_value_bool(env, jsValue, &value) == napi_ok;
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        return ConvertFromJsNumber(env, jsValue, value);
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        size_t len = 0;
        if (napi_get_value_string_utf8(env, jsValue, nullptr, 0, &len) != napi_ok) {
            return false;
        }
        auto buffer = std::make_unique<char[]>(len + 1);
        size_t strLength = 0;
        if (napi_get_value_string_utf8(env, jsValue, buffer.get(), len + 1, &strLength) == napi_ok) {
            value = buffer.get();
            return true;
        }
        return false;
    } else if constexpr (std::is_enum_v<ValueType>) {
        std::make_signed_t<ValueType> numberValue = 0;
        if (!ConvertFromJsNumber(env, jsValue, numberValue)) {
            return false;
        }
        value = static_cast<ValueType>(numberValue);
        return true;
    }
    return false;
}

inline bool ConvertClampFromJsValue(napi_env env, napi_value jsValue, int32_t& value, int32_t lo, int32_t hi)
{
    if (jsValue == nullptr) {
        return false;
    }
    bool ret = napi_get_value_int32(env, jsValue, &value) == napi_ok;
    value = std::clamp(value, lo, hi);
    return ret;
}

inline napi_value GetDoubleAndConvertToJsValue(napi_env env, double d)
{
    napi_value value = nullptr;
    (void)napi_create_double(env, d, &value);
    return value;
}

inline napi_value GetStringAndConvertToJsValue(napi_env env, std::string str)
{
    napi_value objValue = nullptr;
    napi_create_string_utf8(env, str.c_str(), str.length(), &objValue);
    return objValue;
}

inline napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

inline napi_value GetPointAndConvertToJsValue(napi_env env, Drawing::Point& point)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "x", CreateJsNumber(env, point.GetX()));
        napi_set_named_property(env, objValue, "y", CreateJsNumber(env, point.GetY()));
    }
    return objValue;
}

inline void GetPointXFromJsNumber(napi_env env, napi_value argValue, Drawing::Point& point)
{
    napi_value objValue = nullptr;
    double targetX = 0;
    if (napi_get_named_property(env, argValue, "x", &objValue) != napi_ok ||
        napi_get_value_double(env, objValue, &targetX) != napi_ok) {
        ROSEN_LOGE("The Parameter of number x about JsPoint is unvaild");
        return;
    }
    point.SetX(targetX);
    return;
}

inline void GetPointYFromJsNumber(napi_env env, napi_value argValue, Drawing::Point& point)
{
    napi_value objValue = nullptr;
    double targetY = 0;
    if (napi_get_named_property(env, argValue, "y", &objValue) != napi_ok ||
        napi_get_value_double(env, objValue, &targetY) != napi_ok) {
        ROSEN_LOGE("The Parameter of number y about JsPoint is unvaild");
        return;
    }
    point.SetY(targetY);
    return;
}

inline void GetPointFromJsValue(napi_env env, napi_value argValue, Drawing::Point& point)
{
    GetPointXFromJsNumber(env, argValue, point);
    GetPointYFromJsNumber(env, argValue, point);
    return;
}

void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func);
napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message);

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message);

inline std::u16string Str8ToStr16(const std::string &str)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.from_bytes(str);
}

inline void SetTextStyleDoubleValueFromJS(napi_env env, napi_value argValue, const std::string str, double& cValue)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        return;
    }
    ConvertFromJsValue(env, tempValue, cValue);
}

inline void SetTextStyleBooleValueFromJS(napi_env env, napi_value argValue, const std::string str, bool& cValue)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        return;
    }
    ConvertFromJsValue(env, tempValue, cValue);
}

inline napi_value GetPositionWithAffinityAndConvertToJsValue(napi_env env,
    IndexAndAffinity* positionWithAffinity)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (positionWithAffinity != nullptr && objValue != nullptr) {
        napi_set_named_property(env, objValue, "position", CreateJsNumber(env, positionWithAffinity->index));
        napi_set_named_property(env, objValue, "affinity", CreateJsNumber(env, (int)positionWithAffinity->affinity));
    }
    return objValue;
}

inline napi_value GetRangeAndConvertToJsValue(napi_env env, Boundary* range)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (range != nullptr && objValue != nullptr) {
        napi_set_named_property(env, objValue, "start", CreateJsNumber(env, range->leftIndex));
        napi_set_named_property(env, objValue, "end", CreateJsNumber(env, range->rightIndex));
    }
    return objValue;
}

inline napi_value GetRectAndConvertToJsValue(napi_env env, Drawing::Rect rect)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "left", CreateJsNumber(env, rect.GetLeft()));
        napi_set_named_property(env, objValue, "top", CreateJsNumber(env, rect.GetTop()));
        napi_set_named_property(env, objValue, "right", CreateJsNumber(env, rect.GetRight()));
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, rect.GetBottom()));
    }
    return objValue;
}

inline napi_value CreateTextRectJsValue(napi_env env, TextRect textrect)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "rect", GetRectAndConvertToJsValue(env, textrect.rect));
        napi_set_named_property(env, objValue, "direction", CreateJsNumber(env, (int)textrect.direction));
    }
    return objValue;
}

inline napi_value CreateArrayStringJsValue(napi_env env, std::vector<std::string> vectorString)
{
    napi_value jsArray = nullptr;
    if (napi_create_array_with_length(env, vectorString.size(), &jsArray) == napi_ok) {
    size_t index = 0;
    for (const auto& family : vectorString) {
        napi_value jsString;
        napi_create_string_utf8(env, family.c_str(), family.length(), &jsString);
        napi_set_element(env, jsArray, index++, jsString);
    }
    }
    return jsArray;
}

inline napi_value CreateStringJsValue(napi_env env, std::u16string u16String)
{
    napi_value jsStr = nullptr;
    napi_create_string_utf16(env, reinterpret_cast<const char16_t*>(u16String.c_str()), u16String.length(), &jsStr);
    return jsStr;
}

inline napi_value CreateTextStyleJsValue(napi_env env, TextStyle textStyle)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "decoration", CreateJsNumber(
            env, static_cast<uint32_t>(textStyle.decoration)));
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
        napi_set_named_property(env, objValue, "wordSpacing", CreateJsNumber(env, textStyle.wordSpacing));
        napi_set_named_property(env, objValue, "heightScale", CreateJsNumber(env, textStyle.heightScale));
        napi_set_named_property(env, objValue, "halfLeading", CreateJsNumber(env, textStyle.halfLeading));
        napi_set_named_property(env, objValue, "heightOnly", CreateJsNumber(env, textStyle.heightOnly));
        napi_set_named_property(env, objValue, "ellipsis", CreateStringJsValue(env, textStyle.ellipsis));
        napi_set_named_property(env, objValue, "ellipsisMode", CreateJsNumber(
            env, static_cast<uint32_t>(textStyle.ellipsisModal)));
        napi_set_named_property(env, objValue, "locale", CreateJsValue(env, textStyle.locale));
    }
    return objValue;
}

inline napi_value CreateFontMetricsJsValue(napi_env env, Drawing::FontMetrics fontMetrics)
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
        napi_set_named_property(env, objValue, "strikeoutThickness", CreateJsNumber(env,
            fontMetrics.fStrikeoutThickness));
        napi_set_named_property(env, objValue, "strikeoutPosition", CreateJsNumber(env,
            fontMetrics.fStrikeoutPosition));
    }
    return objValue;
}

inline napi_value CreateRunMetricsJsValue(napi_env env, RunMetrics runMetrics)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "textStyle", CreateTextStyleJsValue(env, *(runMetrics.textStyle)));
        napi_set_named_property(env, objValue, "fontMetrics", CreateFontMetricsJsValue(env, runMetrics.fontMetrics));
    }
    return objValue;
}

inline napi_value ConvertMapToNapiMap(napi_env env, const std::map<size_t, RunMetrics>& map) {
    napi_value result;
    napi_status status = napi_create_object(env, &result);
    if (status != napi_ok) {
        ROSEN_LOGE("yqf-Napi: ConvertMapToNapiMap napi_create_object failed");
        return nullptr;
    }
    napi_value jsSize;
    napi_create_uint32(env, map.size(), &jsSize);
    napi_set_named_property(env, result, "size", jsSize);
    for (const auto &[key, val] : map) {
        const std::string &name = std::to_string(key);
        status = napi_set_property(env, result, CreateJsValue(env,name), CreateRunMetricsJsValue(env, val));
        if (status != napi_ok) {
            return nullptr;
        }
    }
    return result;
}

inline napi_value CreateLineMetricsJsValue(napi_env env, OHOS::Rosen::LineMetrics lineMetrics)
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
        napi_set_named_property(env, objValue, "topHeight", CreateJsNumber(env, lineMetrics.capHeight));
        napi_set_named_property(env, objValue, "runMetrics", ConvertMapToNapiMap(env, lineMetrics.runMetrics));
    }
    return objValue;
}

inline void SetFontMetricsFloatValueFromJS(napi_env env, napi_value argValue, const std::string str, float& cValue)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        return;
    }
    double tempValuechild = 0.0;

    ConvertFromJsValue(env, tempValue, tempValuechild);
    cValue = static_cast<float>(tempValuechild);
}

inline void SetLineMetricsDoubleValueFromJS(napi_env env, napi_value argValue, const std::string str, double& cValue)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        return;
    }
    ConvertFromJsValue(env, tempValue, cValue);
}

inline void SetLineMetricsSizeTValueFromJS(napi_env env, napi_value argValue, const std::string str, uint32_t& cValue)
{
    napi_value tempValue = nullptr;
    napi_get_named_property(env, argValue, str.c_str(), &tempValue);
    if (tempValue == nullptr) {
        return;
    }

    uint32_t tempValuechild = 0;
    ConvertFromJsValue(env, tempValue, tempValuechild);
    cValue = static_cast<size_t>(tempValuechild);
}

bool OnMakeFontFamilies(napi_env& env, napi_value jsValue, std::vector<std::string> &fontFamilies);

bool SetTextStyleColor(napi_env env, napi_value argValue, const std::string& str, Drawing::Color& colorSrc);

bool GetDecorationFromJS(napi_env env, napi_value argValue, TextStyle& textStyle);

bool GetTextStyleFromJS(napi_env env, napi_value argValue, TextStyle& textStyle);

bool GetParagraphStyleFromJS(napi_env env, napi_value argValue, TypographyStyle& pographyStyle);

bool GetPlaceholderSpanFromJS(napi_env env, napi_value argValue, PlaceholderSpan& placeholderSpan);

void ParsePartTextStyle(napi_env env, napi_value argValue, TextStyle& textStyle);

size_t GetParamLen(napi_env env, napi_value param);

bool GetFontMetricsFromJS(napi_env env, napi_value argValue, Drawing::FontMetrics& fontMetrics);

bool GetRunMetricsFromJS(napi_env env, napi_value argValue, RunMetrics& runMetrics);

bool GetLineMetricsFromJS(napi_env env, napi_value argValue, LineMetrics& runMetrics);

void ScanShadowValue(napi_env env, napi_value allShadowValue, uint32_t arrayLength, TextStyle& textStyle);

void SetTextShadowProperty(napi_env env, napi_value argValue, TextStyle& textStyle);
} // namespace OHOS::Rosen
#endif // OHOS_JS_TEXT_UTILS_H