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

#include "rs_window_animation_utils.h"

#include <js_runtime_utils.h>

#include "rs_window_animation_log.h"

#include "ui/rs_proxy_node.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

NativeValue* RSWindowAnimationUtils::CreateJsWindowAnimationTarget(NativeEngine& engine,
    const sptr<RSWindowAnimationTarget>& target)
{
    WALOGD("Create!");
    if (target == nullptr) {
        WALOGD("Target is null!");
        return engine.CreateNull();
    }

    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WALOGE("Failed to create object!");
        return engine.CreateUndefined();
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WALOGE("Failed to convert object!");
        return engine.CreateUndefined();
    }

    NativeFinalize finalizeCallback = [](NativeEngine* engine, void* data, void* hint) {
        auto target = sptr<RSWindowAnimationTarget>(static_cast<RSWindowAnimationTarget*>(hint));
        target.GetRefPtr()->DecStrongRef(target.GetRefPtr());
    };
    target.GetRefPtr()->IncStrongRef(target.GetRefPtr());
    object->SetNativePointer(&(target->surfaceNode_), finalizeCallback, target.GetRefPtr());
    if (auto proxyNode = RSBaseNode::ReinterpretCast<RSProxyNode>(target->surfaceNode_)) {
        // force proxy node to flush correct context alpha on next visit
        proxyNode->ResetContextVariableCache();
    }

    object->SetProperty("bundleName", CreateJsValue(engine, target->bundleName_));
    object->SetProperty("abilityName", CreateJsValue(engine, target->abilityName_));
    object->SetProperty("windowBounds", CreateJsRRect(engine, target->windowBounds_));
    object->SetProperty("missionId", CreateJsValue(engine, target->missionId_));
    object->SetProperty("windowId", CreateJsValue(engine, target->windowId_));

    return objValue;
}

NativeValue* RSWindowAnimationUtils::CreateJsWindowAnimationTargetArray(NativeEngine& engine,
    const std::vector<sptr<RSWindowAnimationTarget>>& targets)
{
    WALOGD("Create!");
    NativeValue* arrayValue = engine.CreateArray(targets.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    uint32_t index = 0;
    for (const auto& item : targets) {
        array->SetElement(index++, CreateJsWindowAnimationTarget(engine, item));
    }
    return arrayValue;
}

NativeValue* RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(
    NativeEngine& engine, const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Create!");
    if (finishedCallback == nullptr) {
        WALOGE("Finished callback is null!");
        return engine.CreateUndefined();
    }

    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WALOGE("Failed to create object!");
        return engine.CreateUndefined();
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WALOGE("Failed to convert object!");
        return engine.CreateUndefined();
    }

    NativeFinalize finalizeCallback = [](NativeEngine* engine, void* data, void* hint) {
        auto finishedCallback =
            sptr<RSIWindowAnimationFinishedCallback>(static_cast<RSIWindowAnimationFinishedCallback*>(data));
        finishedCallback.GetRefPtr()->DecStrongRef(finishedCallback.GetRefPtr());
    };
    finishedCallback.GetRefPtr()->IncStrongRef(finishedCallback.GetRefPtr());
    object->SetNativePointer(finishedCallback.GetRefPtr(), finalizeCallback, nullptr);

    NativeCallback jsFinishedCallback = [](NativeEngine* engine, NativeCallbackInfo* info) -> NativeValue* {
        WALOGD("Native finished callback is called!");
        auto nativeFinishedCallback = CheckParamsAndGetThis<RSIWindowAnimationFinishedCallback>(engine, info);
        if (nativeFinishedCallback == nullptr) {
            WALOGE("Finished callback is null!");
            return engine->CreateUndefined();
        }

        nativeFinishedCallback->OnAnimationFinished();
        return engine->CreateUndefined();
    };
    const char *moduleName = "RSWindowAnimationUtils";
    BindNativeFunction(engine, *object, "onAnimationFinish", moduleName, jsFinishedCallback);
    return objValue;
}

NativeValue* RSWindowAnimationUtils::CreateJsRRect(NativeEngine& engine, const RRect& rrect)
{
    WALOGD("Create!");
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WALOGE("Failed to create object!");
        return engine.CreateUndefined();
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WALOGE("Failed to convert object!");
        return engine.CreateUndefined();
    }

    object->SetProperty("left", CreateJsValue(engine, rrect.rect_.left_));
    object->SetProperty("top", CreateJsValue(engine, rrect.rect_.top_));
    object->SetProperty("width", CreateJsValue(engine, rrect.rect_.width_));
    object->SetProperty("height", CreateJsValue(engine, rrect.rect_.height_));
    object->SetProperty("radius", CreateJsValue(engine, rrect.radius_[0].x_));
    return objValue;
}
} // namespace Rosen
} // namespace OHOS
