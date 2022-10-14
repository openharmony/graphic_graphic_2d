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

#include <memory>

#include <js_runtime_utils.h>
#include <rs_window_animation_log.h>

#include "rs_window_animation_utils.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;

RSWindowAnimationController::RSWindowAnimationController(NativeEngine& engine)
    : engine_(engine)
{
}

void RSWindowAnimationController::SetJsController(NativeValue* jsController)
{
    WALOGD("SetJsController.");
    jsController_ = std::unique_ptr<NativeReference>(engine_.CreateReference(jsController, ARGC_ONE));
}

void RSWindowAnimationController::OnStartApp(StartingAppType type,
    const sptr<RSWindowAnimationTarget>& startingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on start app.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, type, startingWindowTarget, finishedCallback](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnStartApp(type, startingWindowTarget, finishedCallback);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnStartApp", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::OnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on app transition.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, fromWindowTarget, toWindowTarget, finishedCallback](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnAppTransition(fromWindowTarget, toWindowTarget, finishedCallback);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnAppTransition", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::OnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on app back transition.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, fromWindowTarget, toWindowTarget, finishedCallback](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnAppBackTransition(fromWindowTarget, toWindowTarget, finishedCallback);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnAppBackTransition", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on minimize window.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, minimizingWindowTarget, finishedCallback](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnMinimizeWindow(minimizingWindowTarget, finishedCallback);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnMinimizeWindow", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::OnMinimizeAllWindow(
    std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on minimize all windows.");
    if (minimizingWindowsTarget.empty()) {
        finishedCallback->OnAnimationFinished();
        WALOGE("The minimizing Windows vector is empty!");
        return;
    }
    for (auto target : minimizingWindowsTarget) {
        sptr<RSIWindowAnimationFinishedCallback> animationCallback =
            new(std::nothrow) RSWindowAnimationCallback(finishedCallback);
        OnMinimizeWindow(target, animationCallback);
    }
}

void RSWindowAnimationController::OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on close window.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, closingWindowTarget, finishedCallback](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnCloseWindow(closingWindowTarget, finishedCallback);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnCloseWindow", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on screen unlock.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, finishedCallback](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnScreenUnlock(finishedCallback);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnScreenUnlock", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::OnWindowAnimationTargetsUpdate(
    const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
    const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets)
{
    WALOGD("Window animation controller on window animation targets update.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, fullScreenWindowTarget, floatingWindowTargets](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnWindowAnimationTargetsUpdate(fullScreenWindowTarget, floatingWindowTargets);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnWindowAnimationTargetsUpdate", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget)
{
    WALOGD("Window animation controller on wallpaper update.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [controllerWptr, wallpaperTarget](NativeEngine&, AsyncTask&, int32_t) {
            auto controllerSptr = controllerWptr.promote();
            if (controllerSptr == nullptr) {
                WALOGE("Controller is null!");
                return;
            }

            controllerSptr->HandleOnWallpaperUpdate(wallpaperTarget);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("RSWindowAnimationController::OnWallpaperUpdate", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void RSWindowAnimationController::HandleOnStartApp(StartingAppType type,
    const sptr<RSWindowAnimationTarget>& startingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on start app.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, startingWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };

    switch (type) {
        case StartingAppType::FROM_LAUNCHER:
            CallJsFunction("onStartAppFromLauncher", argv, ARGC_TWO);
            break;
        case StartingAppType::FROM_RECENT:
            CallJsFunction("onStartAppFromRecent", argv, ARGC_TWO);
            break;
        case StartingAppType::FROM_OTHER:
            CallJsFunction("onStartAppFromOther", argv, ARGC_TWO);
            break;
        default:
            WALOGE("Unknow starting app type.");
            break;
    }
}

void RSWindowAnimationController::HandleOnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on app transition.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, fromWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, toWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onAppTransition", argv, ARGC_THREE);
}

void RSWindowAnimationController::HandleOnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on app back transition.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, fromWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, toWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onAppBackTransition", argv, ARGC_THREE);
}

void RSWindowAnimationController::HandleOnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on minimize window.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, minimizingWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onMinimizeWindow", argv, ARGC_TWO);
}

void RSWindowAnimationController::HandleOnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on close window.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, closingWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onCloseWindow", argv, ARGC_TWO);
}

void RSWindowAnimationController::HandleOnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on screen unlock.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(engine_, finishedCallback),
    };
    CallJsFunction("onScreenUnlock", argv, ARGC_ONE);
}


void RSWindowAnimationController::HandleOnWindowAnimationTargetsUpdate(
    const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
    const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets)
{
    WALOGD("Handle on window animation targets update.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, fullScreenWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationTargetArray(engine_, floatingWindowTargets),
    };
    CallJsFunction("onWindowAnimationTargetsUpdate", argv, ARGC_TWO);
}

void RSWindowAnimationController::HandleOnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget)
{
    WALOGD("Handle on wallpaper target update.");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, wallpaperTarget),
    };
    CallJsFunction("onWallpaperUpdate", argv, ARGC_ONE);
}

void RSWindowAnimationController::CallJsFunction(const std::string& methodName, NativeValue* const* argv, size_t argc)
{
    WALOGD("Call js function:%{public}s.", methodName.c_str());
    if (jsController_ == nullptr) {
        WALOGE("JsController is null!");
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
