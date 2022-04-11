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

#include "rs_window_animation_controller.h"

#include <js_runtime_utils.h>
#include <rs_window_animation_log.h>

#include "rs_window_animation_utils.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;

RSWindowAnimationController::RSWindowAnimationController(NativeEngine& engine,
    const std::shared_ptr<AppExecFwk::EventHandler>& handler)
    : engine_(engine),
      handler_(handler)
{
}

void RSWindowAnimationController::SetJsController(NativeValue* jsController)
{
    jsController_ = std::unique_ptr<NativeReference>(engine_.CreateReference(jsController, ARGC_ONE));
}

void RSWindowAnimationController::OnTransition(const sptr<RSWindowAnimationTarget>& from,
                                               const sptr<RSWindowAnimationTarget>& to,
                                               const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGI("Window animation controller on transition.");
    if (handler_ == nullptr) {
        WALOGE("Handler is null!");
        return;
    }

    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, from, to, finishedCallback]() {
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }

        controllerSptr->HandleOnTransition(from, to, finishedCallback);
    };

    handler_->PostTask(task, AppExecFwk::EventHandler::Priority::IMMEDIATE);
}

void RSWindowAnimationController::OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindow,
                                                   const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGI("Window animation controller on minize window.");
    if (handler_ == nullptr) {
        WALOGE("Handler is null!");
        return;
    }

    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, minimizingWindow, finishedCallback]() {
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }

        controllerSptr->HandleOnMinimizeWindow(minimizingWindow, finishedCallback);
    };

    handler_->PostTask(task, AppExecFwk::EventHandler::Priority::IMMEDIATE);
}

void RSWindowAnimationController::OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindow,
                                                const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGI("Window animation controller on close window.");
    if (handler_ == nullptr) {
        WALOGE("Handler is null!");
        return;
    }

    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, closingWindow, finishedCallback]() {
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }

        controllerSptr->HandleOnCloseWindow(closingWindow, finishedCallback);
    };

    handler_->PostTask(task, AppExecFwk::EventHandler::Priority::IMMEDIATE);
}


void RSWindowAnimationController::HandleOnTransition(const sptr<RSWindowAnimationTarget>& from,
                                                     const sptr<RSWindowAnimationTarget>& to,
                                                     const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGI("Handle on transition.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, *from),
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, *to),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onTransition", argv, ARGC_THREE);
}

void RSWindowAnimationController::HandleOnMinimizeWindow(
    const sptr<RSWindowAnimationTarget>& minimizingWindow,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGI("Handle on minimize window.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, *minimizingWindow),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onMinimizeWindow", argv, ARGC_TWO);
}

void RSWindowAnimationController::HandleOnCloseWindow(
    const sptr<RSWindowAnimationTarget>& closingWindow,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGI("Handle on close window.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, *closingWindow),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onCloseWindow", argv, ARGC_TWO);
}

void RSWindowAnimationController::CallJsFunction(const std::string& methodName, NativeValue* const* argv, size_t argc)
{
    WALOGI("Call js function:%{public}s.", methodName.c_str());
    if (jsController_ == nullptr) {
        WALOGE("JsConterller is null!");
        return;
    }

    auto jsControllerValue = jsController_->Get();
    auto jsControllerObj = ConvertNativeValueTo<NativeObject>(jsControllerValue);
    if (jsControllerObj == nullptr) {
        WALOGE("JsControllerObj is null!");
        return;
    }

    auto method = jsControllerObj->GetProperty(methodName.c_str());
    if (method == nullptr) {
        WALOGE("Failed to get method from object!");
        return;
    }

    engine_.CallFunction(jsControllerValue, method, argv, argc);
}
} // namespace Rosen
} // namespace OHOS
