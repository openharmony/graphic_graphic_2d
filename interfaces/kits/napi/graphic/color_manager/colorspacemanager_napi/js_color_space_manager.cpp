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

#include "js_color_space_manager.h"

#include <memory>

#include "js_color_space.h"
#include "js_color_space_manager_utils.h"
#include "js_runtime_utils.h"

namespace OHOS {
namespace ColorManager {
using namespace AbilityRuntime;

constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t PRIMARIES_PARAMS_NUM = 8;

void JsColorSpaceManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    std::unique_ptr<JsColorSpaceManager>(static_cast<JsColorSpaceManager*>(data));
}

NativeValue* JsColorSpaceManager::CreateColorSpace(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsColorSpaceManager* me = CheckParamsAndGetThis<JsColorSpaceManager>(engine, info);
    return (me != nullptr) ? me->OnCreateColorSpace(*engine, *info) : nullptr;
}

NativeValue* JsColorSpaceManager::OnCreateColorSpace(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc != ARGC_ONE && info.argc != ARGC_TWO) {
        CMLOGE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        return nullptr;
    }

    std::shared_ptr<ColorSpace> colorSpace;
    if (info.argc == ARGC_ONE) {
        ApiColorSpaceType csType = ApiColorSpaceType::UNKNOWN;
        if (!ConvertFromJsValue(engine, info.argv[0], csType)) {
            CMLOGE("[NAPI]Failed to convert parameter to ColorSpaceType.");
            return nullptr;
        }
        colorSpace = std::make_shared<ColorSpace>(JS_TO_NATIVE_COLOR_SPACE_NAME_MAP.at(csType));
    } else {
        // info.argc must be ARGC_ONE or ARGC_TWO
        ColorSpacePrimaries primaries;
        double gamma = 0.0;
        NativeObject* nativePrimaries = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (!ParseColorSpacePrimaries(engine, nativePrimaries, primaries)) {
            CMLOGE("[NAPI]Failed to convert parameter to ColorSpacePrimaries.");
            return nullptr;
        }
        if (!ConvertFromJsValue(engine, info.argv[1], gamma)) {
            CMLOGE("[NAPI]Failed to convert parameter to gamma.");
            return nullptr;
        }
        colorSpace = std::make_shared<ColorSpace>(primaries, static_cast<float>(gamma));
    }
    
    if (colorSpace != nullptr) {
        CMLOGI("[NAPI]OnCreateColorSpace CreateJsColorSpaceObject is called");
        return CreateJsColorSpaceObject(engine, colorSpace);
    }
    CMLOGE("[NAPI]Create color space failed");
    return nullptr;
}

bool JsColorSpaceManager::ParseColorSpacePrimaries(
    NativeEngine& engine, NativeObject* jsObject, ColorSpacePrimaries& primaries)
{
    double val;
    int parseTimes = 0;
    if (ParseJsDoubleValue(jsObject, engine, "redX", val)) {
        primaries.rX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, engine, "redY", val)) {
        primaries.rY = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, engine, "greenX", val)) {
        primaries.gX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, engine, "greenX", val)) {
        primaries.gY = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, engine, "blueX", val)) {
        primaries.bX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, engine, "blueX", val)) {
        primaries.bY = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, engine, "whitePointX", val)) {
        primaries.wX = static_cast<float>(val);
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, engine, "whitePointY", val)) {
        primaries.wY = static_cast<float>(val);
        parseTimes++;
    }
    return (parseTimes == PRIMARIES_PARAMS_NUM);
}

NativeValue* JsColorSpaceManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    if (engine == nullptr || exportObj == nullptr) {
        CMLOGE("[NAPI]JsColorSpaceManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        CMLOGE("[NAPI]JsColorSpaceManagerInit object is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsColorSpaceManager> jsColorSpaceManager = std::make_unique<JsColorSpaceManager>();
    object->SetNativePointer(jsColorSpaceManager.release(), JsColorSpaceManager::Finalizer, nullptr);
    object->SetProperty("ColorSpace", ColorSpaceTypeInit(engine));
    const char *moduleName = "JsColorSpaceManager";
    BindNativeFunction(*engine, *object, "create", moduleName, JsColorSpaceManager::CreateColorSpace);
    return engine->CreateUndefined();
}
}  // namespace ColorManager
}  // namespace OHOS