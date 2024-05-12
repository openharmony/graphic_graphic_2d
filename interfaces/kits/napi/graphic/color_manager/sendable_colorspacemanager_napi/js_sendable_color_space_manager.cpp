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

#include "js_sendable_color_space_manager.h"

#include <memory>

#include "color_space_object_convertor.h"
#include "js_color_space.h"
#include "js_color_space_utils.h"

namespace OHOS {
namespace ColorManager {
constexpr size_t ARGC_ONE = 1;
constexpr size_t PRIMARIES_PARAMS_NUM = 8;

void JsSendableColorSpaceManager::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsSendableColorSpaceManager>(static_cast<JsSendableColorSpaceManager*>(data));
}

napi_value JsSendableColorSpaceManager::CreateSendableColorSpace(napi_env env, napi_callback_info info)
{
    JsSendableColorSpaceManager* me = CheckParamsAndGetThis<JsSendableColorSpaceManager>(env, info);
    return (me != nullptr) ? me->OnCreateColorSpace(env, info) : nullptr;
}

bool CheckColorSpaceTypeRange(napi_env env, const ApiColorSpaceType csType)
{
    if (csType >= ApiColorSpaceType::TYPE_END) {
        CMLOGE("[NAPI]ColorSpaceType is invalid: %{public}u", csType);
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "BusinessError 401: Parameter error, csType value is out of range."));
        return false;
    }
    if (csType == ApiColorSpaceType::UNKNOWN || csType == ApiColorSpaceType::CUSTOM) {
        CMLOGE("[NAPI]ColorSpaceType is invalid: %{public}u", csType);
        std::string errMsg = "Parameter value is abnormal. Cannot create color"
            " manager object using ApiColorSpaceType " +
            std::to_string(static_cast<int32_t>(ApiColorSpaceType::CUSTOM));
        napi_throw(env,
            CreateJsError(env, static_cast<int>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_ENUM_USAGE)),
            errMsg));
        return false;
    }
    return true;
}

napi_value JsSendableColorSpaceManager::OnCreateColorSpace(napi_env env, napi_callback_info info)
{
    size_t argvSize = 2;
    std::vector<napi_value> argvArr(argvSize);
    napi_get_cb_info(env, info, &argvSize, argvArr.data(), nullptr, nullptr);
    if (!CheckParamMinimumValid(env, argvSize, 0)) {
        return nullptr;
    }
    std::shared_ptr<ColorSpace> colorSpace;
    ApiColorSpaceType csType = ApiColorSpaceType::UNKNOWN;
    napi_value object = nullptr;
    napi_get_undefined(env, &object);
    
    if (ConvertFromJsValue(env, argvArr[0], csType)) {
        if (!CheckColorSpaceTypeRange(env, csType)) {
            return object;
        }
        colorSpace = std::make_shared<ColorSpace>(JS_TO_NATIVE_COLOR_SPACE_NAME_MAP.at(csType));
    } else {
        if (argvSize == ARGC_ONE) {
            napi_throw(env,
                CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
                "BusinessError 401: Parameter error, the type of info must be ApiColorSpaceType"));
            return object;
        }
        ColorSpacePrimaries primaries;
        double gamma = 0.0;
        napi_value nativePrimaries = argvArr[0];
        if (!ParseColorSpacePrimaries(env, nativePrimaries, primaries)) {
            napi_throw(env,
                CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
                "BusinessError 401: Parameter error, the first parameter cannot be convert to ColorSpacePrimaries."));
            return object;
        }
        if (!ConvertFromJsValue(env, argvArr[1], gamma)) {
            napi_throw(env,
                CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
                "BusinessError 401: Parameter error, the second parameter cannot be convert to gamma(float)."));
            return object;
        }
        colorSpace = std::make_shared<ColorSpace>(primaries, static_cast<float>(gamma));
    }

    if (colorSpace != nullptr) {
        CMLOGI("[NAPI]OnCreateColorSpace CreateJsColorSpaceObject is called");
        return CreateJsSendableColorSpaceObject(env, colorSpace);
    }
    napi_throw(env,
        CreateJsError(env, static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
        "Parameter value is abnormal. Fail to create JsColorSpaceObject with input parameter(s)."));
    
    return object;
}

bool JsSendableColorSpaceManager::ParseColorSpacePrimaries(
    napi_env env, napi_value jsObject, ColorSpacePrimaries& primaries)
{
    double val;
    int parseTimes = 0;
    if (ParseJsDoubleValue(jsObject, env, "redX", val)) {
        primaries.rX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "redY", val)) {
        primaries.rY = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "greenX", val)) {
        primaries.gX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "greenY", val)) {
        primaries.gY = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "blueX", val)) {
        primaries.bX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "blueY", val)) {
        primaries.bY = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "whitePointX", val)) {
        primaries.wX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "whitePointY", val)) {
        primaries.wY = static_cast<float>(val);
        parseTimes++;
    }
    return (parseTimes == PRIMARIES_PARAMS_NUM);
}

napi_value JsSendableColorSpaceManagerInit(napi_env env, napi_value exportObj)
{
    if (env == nullptr || exportObj == nullptr) {
        CMLOGE("[NAPI]JsSendableColorSpaceManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsSendableColorSpaceManager> jsColorSpaceManager = std::make_unique<JsSendableColorSpaceManager>();
    napi_wrap(env, exportObj, jsColorSpaceManager.release(), JsSendableColorSpaceManager::Finalizer, nullptr, nullptr);
    return exportObj;
}
}  // namespace ColorManager
}  // namespace OHOS