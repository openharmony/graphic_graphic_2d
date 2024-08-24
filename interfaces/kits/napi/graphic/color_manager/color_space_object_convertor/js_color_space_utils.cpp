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

#include "js_color_space_utils.h"

namespace OHOS {
namespace ColorManager {
napi_value CreateJsError(napi_env env, int32_t errCode, const std::string& message)
{
    napi_value result = nullptr;
    NAPI_CALL_DEFAULT(napi_create_error(env, CreateJsValue(env, errCode),
        CreateJsValue(env, message), &result));
    return result;
}

napi_status BindNativeFunction(napi_env env, napi_value object, const char* name,
    const char* moduleName, napi_callback func)
{
    std::string fullName;
    if (moduleName) {
        fullName = moduleName;
        fullName += '.';
    }
    fullName += name;
    napi_value funcValue = nullptr;
    napi_status status = napi_ok;
    status = napi_create_function(env, fullName.c_str(), fullName.size(), func, nullptr, &funcValue);
    if (status != napi_ok) {
        return status;
    }
    status = napi_set_named_property(env, object, fullName.c_str(), funcValue);
    if (status != napi_ok) {
        return status;
    }
    return status;
}

bool CheckParamMinimumValid(napi_env env, const size_t paramNum, const size_t minNum)
{
    if (paramNum <= minNum) {
        CMLOGE("[NAPI]Argc is invalid: %{public}zu", paramNum);
        std::string errMsg = "BusinessError 401: Parameter error, the type of paramNum must be larger than" +
            std::to_string(minNum);
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            errMsg));
        return false;
    }
    return true;
}

napi_value ColorSpaceTypeInit(napi_env env)
{
    if (env == nullptr) {
        CMLOGE("[NAPI]Engine is nullptr");
        return nullptr;
    }
    napi_value object = nullptr;
    NAPI_CALL_DEFAULT(napi_create_object(env, &object));
    if (object == nullptr) {
        CMLOGE("[NAPI]Failed to get object");
        return nullptr;
    }

    for (auto& [colorSpaceName, colorSpace] : STRING_TO_JS_MAP) {
        napi_value value = CreateJsValue(env, static_cast<int32_t>(colorSpace));
        NAPI_CALL_DEFAULT(napi_set_named_property(env, object, colorSpaceName.c_str(), value));
    }
    return object;
}

napi_value CMErrorInit(napi_env env)
{
    if (env == nullptr) {
        CMLOGE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    napi_value object = nullptr;
    NAPI_CALL_DEFAULT(napi_create_object(env, &object));
    if (object == nullptr) {
        CMLOGE("[NAPI]Failed to get object");
        return nullptr;
    }

    napi_value valueErrorNullptr = CreateJsValue(env, static_cast<int32_t>(CMError::CM_ERROR_NULLPTR));
    napi_value valueErrorInvalidPara = CreateJsValue(env, static_cast<int32_t>(CMError::CM_ERROR_INVALID_PARAM));
    napi_value valueErrorInvalidEnum = CreateJsValue(env, static_cast<int32_t>(CMError::CM_ERROR_INVALID_ENUM_USAGE));
    NAPI_CALL_DEFAULT(napi_set_named_property(env, object, "CM_ERROR_NULLPTR", valueErrorNullptr));
    NAPI_CALL_DEFAULT(
        napi_set_named_property(env, object, "CM_ERROR_INVALID_PARAM", valueErrorInvalidPara));
    NAPI_CALL_DEFAULT(
        napi_set_named_property(env, object, "CM_ERROR_INVALID_ENUM_USAGE", valueErrorInvalidEnum));
    return object;
}

napi_value CMErrorCodeInit(napi_env env)
{
    if (env == nullptr) {
        CMLOGE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    napi_value object = nullptr;
    NAPI_CALL_DEFAULT(napi_create_object(env, &object));
    if (object == nullptr) {
        CMLOGE("[NAPI]Failed to get object");
        return nullptr;
    }

    napi_value valueErrorNoPermission = CreateJsValue(env, static_cast<int32_t>(CMErrorCode::CM_ERROR_NO_PERMISSION));
    napi_value valueErrorInvalidPara = CreateJsValue(env, static_cast<int32_t>(CMErrorCode::CM_ERROR_INVALID_PARAM));
    napi_value valueErrorDevNotSupport = CreateJsValue(env,
        static_cast<int32_t>(CMErrorCode::CM_ERROR_DEVICE_NOT_SUPPORT));
    napi_value valueErrorAbnormalpara = CreateJsValue(env,
        static_cast<int32_t>(CMErrorCode::CM_ERROR_ABNORMAL_PARAM_VALUE));
    NAPI_CALL_DEFAULT(
        napi_set_named_property(env, object, "CM_ERROR_NO_PERMISSION", valueErrorNoPermission));
    NAPI_CALL_DEFAULT(
        napi_set_named_property(env, object, "CM_ERROR_INVALID_PARAM", valueErrorInvalidPara));
    NAPI_CALL_DEFAULT(
        napi_set_named_property(env, object, "CM_ERROR_DEVICE_NOT_SUPPORT", valueErrorDevNotSupport));
    NAPI_CALL_DEFAULT(
        napi_set_named_property(env, object, "CM_ERROR_ABNORMAL_PARAM_VALUE", valueErrorAbnormalpara));
    return object;
}

bool ParseJsDoubleValue(napi_value jsObject, napi_env env, const std::string& name, double& data)
{
    napi_value value = nullptr;
    if (napi_get_named_property(env, jsObject, name.c_str(), &value) != napi_ok) {
        CMLOGE("[NAPI]Failed to get property: %{public}s", name.c_str());
        return false;
    };
    napi_valuetype valueType = napi_undefined;
    if (napi_typeof(env, value, &valueType) != napi_ok) {
        CMLOGE("[NAPI]data type is invalied: %{public}s", name.c_str());
        return false;
    };
    if (valueType != napi_undefined) {
        if (napi_get_value_double(env, value, &data) != napi_ok) {
            CMLOGE("[NAPI]Failed to convert parameter to data: %{public}s", name.c_str());
            return false;
        }
    } else {
        CMLOGI("[NAPI]no property with: %{public}s", name.c_str());
        return false;
    }
    return true;
}
}  // namespace ColorManager
}  // namespace OHOS
