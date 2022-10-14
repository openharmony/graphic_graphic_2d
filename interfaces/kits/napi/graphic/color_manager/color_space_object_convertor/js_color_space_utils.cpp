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

#include "js_color_space_utils.h"

namespace OHOS {
namespace ColorManager {
NativeValue* CreateJsError(NativeEngine& engine, int32_t errCode, const std::string& message)
{
    return engine.CreateError(CreateJsValue(engine, errCode), CreateJsValue(engine, message));
}

void BindNativeFunction(NativeEngine& engine, NativeObject& object, const char* name,
    const char* moduleName, NativeCallback func)
{
    std::string fullName(moduleName);
    fullName += ".";
    fullName += name;
    object.SetProperty(name, engine.CreateFunction(fullName.c_str(), fullName.length(), func, nullptr));
}

bool CheckParamMinimumValid(NativeEngine& engine, const size_t paramNum, const size_t minNum)
{
    if (paramNum <= minNum) {
        CMLOGE("[NAPI]Argc is invalid: %{public}zu", paramNum);
        engine.Throw(CreateJsError(engine,
            static_cast<int32_t>(JS_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM)),
            "Parameter check fails. The number of parameter(s) must not be less than " + std::to_string(minNum)));
        return false;
    }
    return true;
}

NativeValue* ColorSpaceTypeInit(NativeEngine* engine)
{
    if (engine == nullptr) {
        CMLOGE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        CMLOGE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("UNKNOWN", CreateJsValue(*engine, static_cast<int32_t>(ApiColorSpaceType::UNKNOWN)));
    object->SetProperty("ADOBE_RGB_1998",
        CreateJsValue(*engine, static_cast<int32_t>(ApiColorSpaceType::ADOBE_RGB_1998)));
    object->SetProperty("DCI_P3", CreateJsValue(*engine, static_cast<int32_t>(ApiColorSpaceType::DCI_P3)));
    object->SetProperty("DISPLAY_P3", CreateJsValue(*engine, static_cast<int32_t>(ApiColorSpaceType::DISPLAY_P3)));
    object->SetProperty("SRGB", CreateJsValue(*engine, static_cast<int32_t>(ApiColorSpaceType::SRGB)));
    object->SetProperty("CUSTOM", CreateJsValue(*engine, static_cast<int32_t>(ApiColorSpaceType::CUSTOM)));
    return objValue;
}

NativeValue* CMErrorInit(NativeEngine* engine)
{
    if (engine == nullptr) {
        CMLOGE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        CMLOGE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("CM_ERROR_NULLPTR", CreateJsValue(*engine, static_cast<int32_t>(CMError::CM_ERROR_NULLPTR)));
    object->SetProperty("CM_ERROR_INVALID_PARAM",
        CreateJsValue(*engine, static_cast<int32_t>(CMError::CM_ERROR_INVALID_PARAM)));
    object->SetProperty("CM_ERROR_INVALID_ENUM_USAGE",
        CreateJsValue(*engine, static_cast<int32_t>(CMError::CM_ERROR_INVALID_ENUM_USAGE)));
    return objValue;
}

NativeValue* CMErrorCodeInit(NativeEngine* engine)
{
    if (engine == nullptr) {
        CMLOGE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        CMLOGE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("CM_ERROR_NO_PERMISSION",
        CreateJsValue(*engine, static_cast<int32_t>(CMErrorCode::CM_ERROR_NO_PERMISSION)));
    object->SetProperty("CM_ERROR_INVALID_PARAM",
        CreateJsValue(*engine, static_cast<int32_t>(CMErrorCode::CM_ERROR_INVALID_PARAM)));
    object->SetProperty("CM_ERROR_DEVICE_NOT_SUPPORT",
        CreateJsValue(*engine, static_cast<int32_t>(CMErrorCode::CM_ERROR_DEVICE_NOT_SUPPORT)));
    object->SetProperty("CM_ERROR_ABNORMAL_PARAM_VALUE",
        CreateJsValue(*engine, static_cast<int32_t>(CMErrorCode::CM_ERROR_ABNORMAL_PARAM_VALUE)));
    return objValue;
}

bool ParseJsDoubleValue(NativeObject* jsObject, NativeEngine& engine, const std::string& name, double& data)
{
    NativeValue* value = jsObject->GetProperty(name.c_str());
    if (value->TypeOf() != NATIVE_UNDEFINED) {
        if (!ConvertFromJsValue(engine, value, data)) {
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
