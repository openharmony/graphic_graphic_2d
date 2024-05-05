/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_COLOR_SPACE_UTILS_H
#define OHOS_JS_COLOR_SPACE_UTILS_H

#include <map>
#include <string>
#include <hilog/log.h>

#include "color_space.h"
#include "color_manager_common.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace ColorManager {

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "JsColorSpace"

#define CMLOGE(fmt, args...) HILOG_ERROR(LOG_CORE, fmt, ##args)
#define CMLOGI(fmt, args...) HILOG_INFO(LOG_CORE, fmt, ##args)

enum class ApiColorSpaceType : uint32_t {
    UNKNOWN = 0,
    ADOBE_RGB_1998 = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    BT709 = 6,
    BT601_EBU = 7,
    BT601_SMPTE_C = 8,
    BT2020_HLG = 9,
    BT2020_PQ = 10,
    P3_HLG = 11,
    P3_PQ = 12,
    ADOBE_RGB_1998_LIMIT = 13,
    DISPLAY_P3_LIMIT = 14,
    SRGB_LIMIT = 15,
    BT709_LIMIT = 16,
    BT601_EBU_LIMIT = 17,
    BT601_SMPTE_C_LIMIT = 18,
    BT2020_HLG_LIMIT = 19,
    BT2020_PQ_LIMIT = 20,
    P3_HLG_LIMIT = 21,
    P3_PQ_LIMIT = 22,
    LINEAR_P3 = 23,
    LINEAR_SRGB = 24,
    LINEAR_BT709 = LINEAR_SRGB,
    LINEAR_BT2020 = 25,
    DISPLAY_SRGB = SRGB,
    DISPLAY_P3_SRGB = DISPLAY_P3,
    DISPLAY_P3_HLG = P3_HLG,
    DISPLAY_P3_PQ = P3_PQ,
    CUSTOM = 5,
    TYPE_END = 26
};

const std::map<ColorSpaceName, ApiColorSpaceType> NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP {
    { ColorSpaceName::NONE, ApiColorSpaceType::UNKNOWN },
    { ColorSpaceName::ADOBE_RGB, ApiColorSpaceType::ADOBE_RGB_1998 },
    { ColorSpaceName::DCI_P3, ApiColorSpaceType::DCI_P3 },
    { ColorSpaceName::DISPLAY_P3, ApiColorSpaceType::DISPLAY_P3 },
    { ColorSpaceName::SRGB, ApiColorSpaceType::SRGB },
    { ColorSpaceName::BT709, ApiColorSpaceType::BT709 },
    { ColorSpaceName::BT601_EBU, ApiColorSpaceType::BT601_EBU },
    { ColorSpaceName::BT601_SMPTE_C, ApiColorSpaceType::BT601_SMPTE_C },
    { ColorSpaceName::BT2020_HLG, ApiColorSpaceType::BT2020_HLG },
    { ColorSpaceName::BT2020_PQ, ApiColorSpaceType::BT2020_PQ },
    { ColorSpaceName::P3_HLG, ApiColorSpaceType::P3_HLG },
    { ColorSpaceName::P3_PQ, ApiColorSpaceType::P3_PQ },
    { ColorSpaceName::ADOBE_RGB_LIMIT, ApiColorSpaceType::ADOBE_RGB_1998_LIMIT },
    { ColorSpaceName::DISPLAY_P3_LIMIT, ApiColorSpaceType::DISPLAY_P3_LIMIT },
    { ColorSpaceName::SRGB_LIMIT, ApiColorSpaceType::SRGB_LIMIT },
    { ColorSpaceName::BT709_LIMIT, ApiColorSpaceType::BT709_LIMIT },
    { ColorSpaceName::BT601_EBU_LIMIT, ApiColorSpaceType::BT601_EBU_LIMIT },
    { ColorSpaceName::BT601_SMPTE_C_LIMIT, ApiColorSpaceType::BT601_SMPTE_C_LIMIT },
    { ColorSpaceName::BT2020_HLG_LIMIT, ApiColorSpaceType::BT2020_HLG_LIMIT },
    { ColorSpaceName::BT2020_PQ_LIMIT, ApiColorSpaceType::BT2020_PQ_LIMIT },
    { ColorSpaceName::P3_HLG_LIMIT, ApiColorSpaceType::P3_HLG_LIMIT },
    { ColorSpaceName::P3_PQ_LIMIT, ApiColorSpaceType::P3_PQ_LIMIT },
    { ColorSpaceName::LINEAR_P3, ApiColorSpaceType::LINEAR_P3 },
    { ColorSpaceName::LINEAR_SRGB, ApiColorSpaceType::LINEAR_SRGB },
    { ColorSpaceName::LINEAR_BT709, ApiColorSpaceType::LINEAR_BT709 },
    { ColorSpaceName::LINEAR_BT2020, ApiColorSpaceType::LINEAR_BT2020 },
    { ColorSpaceName::DISPLAY_SRGB, ApiColorSpaceType::DISPLAY_SRGB },
    { ColorSpaceName::DISPLAY_P3_SRGB, ApiColorSpaceType::DISPLAY_P3_SRGB },
    { ColorSpaceName::DISPLAY_P3_HLG, ApiColorSpaceType::DISPLAY_P3_HLG },
    { ColorSpaceName::DISPLAY_P3_PQ, ApiColorSpaceType::DISPLAY_P3_PQ },
    { ColorSpaceName::CUSTOM, ApiColorSpaceType::CUSTOM },
};

const std::map<ApiColorSpaceType, ColorSpaceName> JS_TO_NATIVE_COLOR_SPACE_NAME_MAP {
    { ApiColorSpaceType::UNKNOWN, ColorSpaceName::NONE },
    { ApiColorSpaceType::ADOBE_RGB_1998, ColorSpaceName::ADOBE_RGB },
    { ApiColorSpaceType::DCI_P3, ColorSpaceName::DCI_P3 },
    { ApiColorSpaceType::DISPLAY_P3, ColorSpaceName::DISPLAY_P3 },
    { ApiColorSpaceType::SRGB, ColorSpaceName::SRGB },
    { ApiColorSpaceType::BT709, ColorSpaceName::BT709 },
    { ApiColorSpaceType::BT601_EBU, ColorSpaceName::BT601_EBU },
    { ApiColorSpaceType::BT601_SMPTE_C, ColorSpaceName::BT601_SMPTE_C },
    { ApiColorSpaceType::BT2020_HLG, ColorSpaceName::BT2020_HLG },
    { ApiColorSpaceType::BT2020_PQ, ColorSpaceName::BT2020_PQ },
    { ApiColorSpaceType::P3_HLG, ColorSpaceName::P3_HLG },
    { ApiColorSpaceType::P3_PQ, ColorSpaceName::P3_PQ },
    { ApiColorSpaceType::ADOBE_RGB_1998_LIMIT, ColorSpaceName::ADOBE_RGB_LIMIT },
    { ApiColorSpaceType::DISPLAY_P3_LIMIT, ColorSpaceName::DISPLAY_P3_LIMIT },
    { ApiColorSpaceType::SRGB_LIMIT, ColorSpaceName::SRGB_LIMIT },
    { ApiColorSpaceType::BT709_LIMIT, ColorSpaceName::BT709_LIMIT },
    { ApiColorSpaceType::BT601_EBU_LIMIT, ColorSpaceName::BT601_EBU_LIMIT },
    { ApiColorSpaceType::BT601_SMPTE_C_LIMIT, ColorSpaceName::BT601_SMPTE_C_LIMIT },
    { ApiColorSpaceType::BT2020_HLG_LIMIT, ColorSpaceName::BT2020_HLG_LIMIT },
    { ApiColorSpaceType::BT2020_PQ_LIMIT, ColorSpaceName::BT2020_PQ_LIMIT },
    { ApiColorSpaceType::P3_HLG_LIMIT, ColorSpaceName::P3_HLG_LIMIT },
    { ApiColorSpaceType::P3_PQ_LIMIT, ColorSpaceName::P3_PQ_LIMIT },
    { ApiColorSpaceType::LINEAR_P3, ColorSpaceName::LINEAR_P3 },
    { ApiColorSpaceType::LINEAR_SRGB, ColorSpaceName::LINEAR_SRGB },
    { ApiColorSpaceType::LINEAR_BT709, ColorSpaceName::LINEAR_BT709 },
    { ApiColorSpaceType::LINEAR_BT2020, ColorSpaceName::LINEAR_BT2020 },
    { ApiColorSpaceType::DISPLAY_SRGB, ColorSpaceName::DISPLAY_SRGB },
    { ApiColorSpaceType::DISPLAY_P3_SRGB, ColorSpaceName::DISPLAY_P3_SRGB },
    { ApiColorSpaceType::DISPLAY_P3_HLG, ColorSpaceName::DISPLAY_P3_HLG },
    { ApiColorSpaceType::DISPLAY_P3_PQ, ColorSpaceName::DISPLAY_P3_PQ },
    { ApiColorSpaceType::CUSTOM, ColorSpaceName::CUSTOM },
};

const std::map<std::string, ApiColorSpaceType> STRING_TO_JS_MAP = {
    { "UNKNOWN", ApiColorSpaceType::UNKNOWN },
    { "ADOBE_RGB_1998", ApiColorSpaceType::ADOBE_RGB_1998 },
    { "DCI_P3", ApiColorSpaceType::DCI_P3 },
    { "DISPLAY_P3", ApiColorSpaceType::DISPLAY_P3 },
    { "SRGB", ApiColorSpaceType::SRGB },
    { "BT709", ApiColorSpaceType::BT709 },
    { "BT601_EBU", ApiColorSpaceType::BT601_EBU },
    { "BT601_SMPTE_C", ApiColorSpaceType::BT601_SMPTE_C },
    { "BT2020_HLG", ApiColorSpaceType::BT2020_HLG },
    { "BT2020_PQ", ApiColorSpaceType::BT2020_PQ },
    { "P3_HLG", ApiColorSpaceType::P3_HLG },
    { "P3_PQ", ApiColorSpaceType::P3_PQ },
    { "ADOBE_RGB_1998_LIMIT", ApiColorSpaceType::ADOBE_RGB_1998_LIMIT },
    { "DISPLAY_P3_LIMIT", ApiColorSpaceType::DISPLAY_P3_LIMIT },
    { "SRGB_LIMIT", ApiColorSpaceType::SRGB_LIMIT },
    { "BT709_LIMIT", ApiColorSpaceType::BT709_LIMIT },
    { "BT601_EBU_LIMIT", ApiColorSpaceType::BT601_EBU_LIMIT },
    { "BT601_SMPTE_C_LIMIT", ApiColorSpaceType::BT601_SMPTE_C_LIMIT },
    { "BT2020_HLG_LIMIT", ApiColorSpaceType::BT2020_HLG_LIMIT },
    { "BT2020_PQ_LIMIT", ApiColorSpaceType::BT2020_PQ_LIMIT },
    { "P3_HLG_LIMIT", ApiColorSpaceType::P3_HLG_LIMIT },
    { "P3_PQ_LIMIT", ApiColorSpaceType::P3_PQ_LIMIT },
    { "LINEAR_P3", ApiColorSpaceType::LINEAR_P3 },
    { "LINEAR_SRGB", ApiColorSpaceType::LINEAR_SRGB },
    { "LINEAR_BT709", ApiColorSpaceType::LINEAR_BT709 },
    { "LINEAR_BT2020", ApiColorSpaceType::LINEAR_BT2020 },
    { "DISPLAY_SRGB", ApiColorSpaceType::DISPLAY_SRGB },
    { "DISPLAY_P3_SRGB", ApiColorSpaceType::DISPLAY_P3_SRGB },
    { "DISPLAY_P3_HLG", ApiColorSpaceType::DISPLAY_P3_HLG },
    { "DISPLAY_P3_PQ", ApiColorSpaceType::DISPLAY_P3_PQ },
    { "CUSTOM", ApiColorSpaceType::CUSTOM },
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

napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message = std::string());
void BindNativeFunction(napi_env env, napi_value object, const char* name, const char* moduleName, napi_callback func);
bool CheckParamMinimumValid(napi_env env, const size_t paramNum, const size_t minNum);

napi_value ColorSpaceTypeInit(napi_env env);
napi_value CMErrorInit(napi_env env);
napi_value CMErrorCodeInit(napi_env env);
bool ParseJsDoubleValue(napi_value jsObject, napi_env env, const std::string& name, double& data);
}  // namespace ColorManager
}  // namespace OHOS
#endif // OHOS_JS_COLOR_SPACE_UTILS_H
