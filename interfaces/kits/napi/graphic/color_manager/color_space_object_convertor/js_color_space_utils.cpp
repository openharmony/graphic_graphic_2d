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
void BindNativeFunction(NativeEngine& engine, NativeObject& object, const char* name,
    const char* moduleName, NativeCallback func)
{
    std::string fullName(moduleName);
    fullName += ".";
    fullName += name;
    object.SetProperty(name, engine.CreateFunction(fullName.c_str(), fullName.length(), func, nullptr));
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
