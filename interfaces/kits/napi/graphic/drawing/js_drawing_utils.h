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

#ifndef OHOS_JS_DRAWING_UTILS_H
#define OHOS_JS_DRAWING_UTILS_H

#include <map>
#ifdef ROSEN_OHOS
#include "hilog/log.h"
#endif

#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "text/font_metrics.h"
#include "text/font_types.h"
#include "utils/rect.h"

namespace OHOS::Rosen {
namespace Drawing {
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr size_t ARGC_FOUR = 4;
constexpr size_t ARGC_FIVE = 5;
constexpr size_t ARGC_SIX = 6;

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

inline napi_value GetFontMetricsAndConvertToJsValue(napi_env env, FontMetrics* metrics)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (metrics != nullptr && objValue != nullptr) {
        napi_set_named_property(env, objValue, "top", CreateJsNumber(env, metrics->fTop));
        napi_set_named_property(env, objValue, "ascent", CreateJsNumber(env, metrics->fAscent));
        napi_set_named_property(env, objValue, "descent", CreateJsNumber(env, metrics->fDescent));
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, metrics->fBottom));
        napi_set_named_property(env, objValue, "leading", CreateJsNumber(env, metrics->fLeading));
    }
    return objValue;
}

inline napi_value GetRectAndConvertToJsValue(napi_env env, std::shared_ptr<Rect> rect)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (rect != nullptr && objValue != nullptr) {
        napi_set_named_property(env, objValue, "left", CreateJsNumber(env, rect->GetLeft()));
        napi_set_named_property(env, objValue, "top", CreateJsNumber(env, rect->GetTop()));
        napi_set_named_property(env, objValue, "right", CreateJsNumber(env, rect->GetRight()));
        napi_set_named_property(env, objValue, "bottom", CreateJsNumber(env, rect->GetBottom()));
    }
    return objValue;
}

inline napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func);
napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message);

bool ConvertFromJsTextEncoding(napi_env env, TextEncoding& textEncoding, napi_value nativeType);

napi_value NapiThrowError(napi_env env, DrawingErrorCode err, const std::string& message);
} // namespace Drawing
} // namespace OHOS::Rosen

#ifdef ROSEN_OHOS
constexpr OHOS::HiviewDFX::HiLogLabel LABEL_ROSEN = {LOG_CORE, 0xD001400, "JsDrawing"};
#define ROSEN_LOGI(format, ...) \
    OHOS::HiviewDFX::HiLog::Info(LABEL_ROSEN, format, ##__VA_ARGS__)
#define ROSEN_LOGD(format, ...) \
    OHOS::HiviewDFX::HiLog::Debug(LABEL_ROSEN, format, ##__VA_ARGS__)
#define ROSEN_LOGE(format, ...) \
    OHOS::HiviewDFX::HiLog::Error(LABEL_ROSEN, format, ##__VA_ARGS__)
#else
#define ROSEN_LOGI(format, ...)
#define ROSEN_LOGD(format, ...)
#define ROSEN_LOGE(format, ...)
#endif

#endif // OHOS_JS_DRAWING_UTILS_H
