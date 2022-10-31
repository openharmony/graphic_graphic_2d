/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <hilog/log.h>

#include "color_space.h"
#include "color_manager_common.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace ColorManager {
#ifndef TITLE
#define TITLE __func__
#endif

constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0xD001400, "JsColorSpace"};
#define CMLOGE(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}s: " fmt, TITLE, ##args)
#define CMLOGI(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}s: " fmt, TITLE, ##args)

enum class ApiColorSpaceType : uint32_t {
    UNKNOWN = 0,
    ADOBE_RGB_1998 = 1,
    DCI_P3 = 2,
    DISPLAY_P3 = 3,
    SRGB = 4,
    CUSTOM = 5,
    TYPE_END
};

const std::map<ColorSpaceName, ApiColorSpaceType> NATIVE_TO_JS_COLOR_SPACE_TYPE_MAP {
    { ColorSpaceName::NONE, ApiColorSpaceType::UNKNOWN },
    { ColorSpaceName::ADOBE_RGB, ApiColorSpaceType::ADOBE_RGB_1998 },
    { ColorSpaceName::DCI_P3, ApiColorSpaceType::DCI_P3 },
    { ColorSpaceName::DISPLAY_P3, ApiColorSpaceType::DISPLAY_P3 },
    { ColorSpaceName::SRGB, ApiColorSpaceType::SRGB },
    { ColorSpaceName::CUSTOM, ApiColorSpaceType::CUSTOM },
};

const std::map<ApiColorSpaceType, ColorSpaceName> JS_TO_NATIVE_COLOR_SPACE_NAME_MAP {
    { ApiColorSpaceType::UNKNOWN, ColorSpaceName::NONE },
    { ApiColorSpaceType::ADOBE_RGB_1998, ColorSpaceName::ADOBE_RGB },
    { ApiColorSpaceType::DCI_P3, ColorSpaceName::DCI_P3 },
    { ApiColorSpaceType::DISPLAY_P3, ColorSpaceName::DISPLAY_P3 },
    { ApiColorSpaceType::SRGB, ColorSpaceName::SRGB },
    { ApiColorSpaceType::CUSTOM, ColorSpaceName::CUSTOM },
};

template<class T>
inline T* ConvertNativeValueTo(NativeValue* value)
{
    return (value != nullptr) ? static_cast<T*>(value->GetInterface(T::INTERFACE_ID)) : nullptr;
}

template<class T>
inline T* CheckParamsAndGetThis(const NativeEngine* engine, NativeCallbackInfo* info, const char* name = nullptr)
{
    if (engine == nullptr || info == nullptr) {
        return nullptr;
    }
    NativeObject* object = ConvertNativeValueTo<NativeObject>(info->thisVar);
    if (object != nullptr && name != nullptr) {
        object = ConvertNativeValueTo<NativeObject>(object->GetProperty(name));
    }
    return (object != nullptr) ? static_cast<T*>(object->GetNativePointer()) : nullptr;
}

template<class T>
NativeValue* CreateJsValue(NativeEngine& engine, const T& value)
{
    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_same_v<ValueType, bool>) {
        return engine.CreateBoolean(value);
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        return engine.CreateNumber(value);
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        return engine.CreateString(value.c_str(), value.length());
    } else if constexpr (std::is_enum_v<ValueType>) {
        return engine.CreateNumber(static_cast<std::make_signed_t<ValueType>>(value));
    } else if constexpr (std::is_same_v<ValueType, const char*>) {
        return (value != nullptr) ? engine.CreateString(value, strlen(value)) : engine.CreateUndefined();
    }
    return engine.CreateUndefined();
}

template<class T>
bool ConvertFromJsValue(NativeEngine& engine, NativeValue* jsValue, T& value)
{
    if (jsValue == nullptr) {
        return false;
    }

    using ValueType = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_same_v<ValueType, bool>) {
        auto boolValue = ConvertNativeValueTo<NativeBoolean>(jsValue);
        if (boolValue == nullptr) {
            return false;
        }
        value = *boolValue;
        return true;
    } else if constexpr (std::is_arithmetic_v<ValueType>) {
        auto numberValue = ConvertNativeValueTo<NativeNumber>(jsValue);
        if (numberValue == nullptr) {
            return false;
        }
        value = *numberValue;
        return true;
    } else if constexpr (std::is_same_v<ValueType, std::string>) {
        auto stringValue = ConvertNativeValueTo<NativeString>(jsValue);
        if (stringValue == nullptr) {
            return false;
        }
        size_t len = stringValue->GetLength() + 1;
        auto buffer = std::make_unique<char[]>(len);
        size_t strLength = 0;
        stringValue->GetCString(buffer.get(), len, &strLength);
        value = buffer.get();
        return true;
    } else if constexpr (std::is_enum_v<ValueType>) {
        auto numberValue = ConvertNativeValueTo<NativeNumber>(jsValue);
        if (numberValue == nullptr) {
            return false;
        }
        value = static_cast<ValueType>(static_cast<std::make_signed_t<ValueType>>(*numberValue));
        return true;
    }
    CMLOGE("[NAPI]Failed to ConvertFromJsValue");
    return false;
}

NativeValue* CreateJsError(NativeEngine& engine, int32_t errCode, const std::string& message = std::string());
void BindNativeFunction(NativeEngine& engine, NativeObject& object, const char* name,
    const char* moduleName, NativeCallback func);
bool CheckParamMinimumValid(NativeEngine& engine, const size_t paramNum, const size_t minNum);

NativeValue* ColorSpaceTypeInit(NativeEngine* engine);
NativeValue* CMErrorInit(NativeEngine* engine);
NativeValue* CMErrorCodeInit(NativeEngine* engine);
bool ParseJsDoubleValue(NativeObject* jsObject, NativeEngine& engine, const std::string& name, double& data);
}  // namespace ColorManager
}  // namespace OHOS
#endif // OHOS_JS_COLOR_SPACE_UTILS_H
