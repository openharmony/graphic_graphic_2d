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

#include "rs_window_animation_manager.h"

#include <memory>

#include <js_runtime_utils.h>
#include <rs_window_animation_log.h>
#include <singleton_container.h>
#include <window_adapter.h>

#include "rs_window_animation_utils.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr int32_t ERR_NOT_OK = -1;
constexpr int32_t ERR_OK = 0;

NativeValue* RSWindowAnimationManager::Init(NativeEngine* engine, NativeValue* exportObj)
{
    WALOGD("Init");
    if (engine == nullptr || exportObj == nullptr) {
        WALOGE("Engine or exportObj is null!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WALOGE("Object is null");
        return nullptr;
    }

    auto windowAnimationManager = std::make_unique<RSWindowAnimationManager>();
    object->SetNativePointer(windowAnimationManager.release(), RSWindowAnimationManager::Finalizer, nullptr);

    const char *moduleName = "RSWindowAnimationManager";
    BindNativeFunction(*engine, *object, "setController", moduleName, RSWindowAnimationManager::SetController);
    BindNativeFunction(*engine, *object, "minimizeWindowWithAnimation", moduleName,
        RSWindowAnimationManager::MinimizeWindowWithAnimation);
    return nullptr;
}

void RSWindowAnimationManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    std::unique_ptr<RSWindowAnimationManager>(static_cast<RSWindowAnimationManager*>(data));
}

NativeValue* RSWindowAnimationManager::SetController(NativeEngine* engine, NativeCallbackInfo* info)
{
    WALOGD("SetController");
    auto me = CheckParamsAndGetThis<RSWindowAnimationManager>(engine, info);
    return (me != nullptr) ? me->OnSetController(*engine, *info) : nullptr;
}

NativeValue* RSWindowAnimationManager::MinimizeWindowWithAnimation(NativeEngine* engine, NativeCallbackInfo* info)
{
    WALOGD("MinimizeWindowWithAnimation");
    auto me = CheckParamsAndGetThis<RSWindowAnimationManager>(engine, info);
    return (me != nullptr) ? me->OnMinimizeWindowWithAnimation(*engine, *info) : nullptr;
}

NativeValue* RSWindowAnimationManager::OnSetController(NativeEngine& engine, NativeCallbackInfo& info)
{
    WALOGD("OnSetController");
    // only support one param
    if (info.argc != ARGC_ONE) {
        WALOGE("No enough params!");
        return nullptr;
    }

    sptr<RSWindowAnimationController> controller = new RSWindowAnimationController(engine);
    controller->SetJsController(info.argv[0]);
    SingletonContainer::Get<WindowAdapter>().SetWindowAnimationController(controller);
    return nullptr;
}

NativeValue* RSWindowAnimationManager::OnMinimizeWindowWithAnimation(NativeEngine& engine, NativeCallbackInfo& info)
{
    int32_t errCode = ERR_OK;
    if (info.argc < ARGC_ONE || info.argc > ARGC_TWO) {
        WALOGE("No enough params!");
        errCode = ERR_NOT_OK;
    }

    auto targetObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
    RSWindowAnimationTarget* target = nullptr;
    
    if (targetObj == nullptr) {
        WALOGE("Window animation target object is null!");
        errCode = ERR_NOT_OK;
    } else {
        target = static_cast<RSWindowAnimationTarget*>(targetObj->GetNativePointer());
    }
    if (target == nullptr) {
        WALOGE("Window animation target is null!");
        errCode = ERR_NOT_OK;
    }

    AsyncTask::CompleteCallback complete =
        [target, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != ERR_OK) {
                task.Reject(engine, CreateJsError(engine, errCode, "Invalid params."));
                return;
            }

            std::vector<uint32_t> windowIds = {target->windowId_};
            sptr<RSIWindowAnimationFinishedCallback> finishedCallback;
            SingletonContainer::Get<WindowAdapter>().MinimizeWindowsByLauncher(windowIds, true, finishedCallback);
            if (finishedCallback == nullptr) {
                WALOGE("Window animation finished callback is null!");
                task.Reject(engine, CreateJsError(engine, errCode, "Animation finished callback is null!"));
                return;
            }

            WALOGD("Resolve minimize window with animation!");
            task.Resolve(engine,
                RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine, finishedCallback));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("RSWindowAnimationManager::OnMinimizeWindowWithAnimation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}
} // namespace Rosen
} // namespace OHOS
