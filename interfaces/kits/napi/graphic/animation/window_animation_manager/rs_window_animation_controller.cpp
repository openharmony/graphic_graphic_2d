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

#include "rs_trace.h"
#include "rs_window_animation_utils.h"
#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
thread_local std::unique_ptr<NativeReference> jsController_ = nullptr;

RSWindowAnimationController::RSWindowAnimationController(napi_env env)
    : env_(env),
    handler_(std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner()))
{}

void RSWindowAnimationController::SetJsController(napi_value jsController)
{
    WALOGD("SetJsController.");
    napi_ref ref = nullptr;
    napi_create_reference(env_, jsController, ARGC_ONE, &ref);
    jsController_ = std::unique_ptr<NativeReference>(reinterpret_cast<NativeReference*>(ref));
}

void RSWindowAnimationController::OnStartApp(StartingAppType type,
    const sptr<RSWindowAnimationTarget>& startingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on start app.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, type, startingWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnStartApp");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }
        if (!RSWindowAnimationUtils::IsSystemApp()) {
            WALOGE("OnStartApp is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnStartApp(type, startingWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        WALOGE("Window animation controller on start app error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void RSWindowAnimationController::OnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on app transition.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, fromWindowTarget, toWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnAppTransition");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }
        if (!RSWindowAnimationUtils::IsSystemApp()) {
            WALOGE("OnAppTransition is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnAppTransition(fromWindowTarget, toWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        WALOGE("Window animation controller on app transition error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void RSWindowAnimationController::OnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on app back transition.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, fromWindowTarget, toWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnAppBackTransition");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }

        controllerSptr->HandleOnAppBackTransition(fromWindowTarget, toWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        WALOGE("Window animation controller on app back transition error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void RSWindowAnimationController::OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on minimize window.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, minimizingWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnMinimizeWindow");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }
        if (!RSWindowAnimationUtils::IsSystemApp()) {
            WALOGE("OnMinimizeWindow is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnMinimizeWindow(minimizingWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        WALOGE("Window animation controller on minimize window error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
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
    auto task = [controllerWptr, closingWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnCloseWindow");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }
        if (!RSWindowAnimationUtils::IsSystemApp()) {
            WALOGE("OnCloseWindow is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnCloseWindow(closingWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        WALOGE("Window animation controller on close window error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void RSWindowAnimationController::OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Window animation controller on screen unlock.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, finishedCallback]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnScreenUnlock");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }
        if (!RSWindowAnimationUtils::IsSystemApp()) {
            WALOGE("OnScreenUnlock is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnScreenUnlock(finishedCallback);
    };
#ifdef SOC_PERF_ENABLE
    constexpr int32_t ACTION_TYPE_CPU_BOOST_CMDID = 10050;
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(ACTION_TYPE_CPU_BOOST_CMDID, true, "");
#endif
    if (handler_ == nullptr) {
        WALOGE("Window animation controller on screen unlock error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void RSWindowAnimationController::OnWindowAnimationTargetsUpdate(
    const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
    const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets)
{
    WALOGD("Window animation controller on window animation targets update.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, fullScreenWindowTarget, floatingWindowTargets]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnWindowAnimationTargetsUpdate");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }
        if (!RSWindowAnimationUtils::IsSystemApp()) {
            WALOGE("OnWindowAnimationTargetsUpdate is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnWindowAnimationTargetsUpdate(fullScreenWindowTarget, floatingWindowTargets);
    };

    if (handler_ == nullptr) {
        WALOGE("Window animation controller on window animation targets update error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void RSWindowAnimationController::OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget)
{
    WALOGD("Window animation controller on wallpaper update.");
    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, wallpaperTarget]() {
        RS_TRACE_NAME("RSWindowAnimationController::OnWallpaperUpdate");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            WALOGE("Controller is null!");
            return;
        }

        controllerSptr->HandleOnWallpaperUpdate(wallpaperTarget);
    };

    if (handler_ == nullptr) {
        WALOGE("Window animation controller on app transition error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void RSWindowAnimationController::HandleOnStartApp(StartingAppType type,
    const sptr<RSWindowAnimationTarget>& startingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on start app.");
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, startingWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(env_, finishedCallback),
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
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, fromWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, toWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(env_, finishedCallback),
    };
    CallJsFunction("onAppTransition", argv, ARGC_THREE);
}

void RSWindowAnimationController::HandleOnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on app back transition.");
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, fromWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, toWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(env_, finishedCallback),
    };
    CallJsFunction("onAppBackTransition", argv, ARGC_THREE);
}

void RSWindowAnimationController::HandleOnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on minimize window.");
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, minimizingWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(env_, finishedCallback),
    };
    CallJsFunction("onMinimizeWindow", argv, ARGC_TWO);
}

void RSWindowAnimationController::HandleOnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on close window.");
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, closingWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(env_, finishedCallback),
    };
    CallJsFunction("onCloseWindow", argv, ARGC_TWO);
}

void RSWindowAnimationController::HandleOnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    WALOGD("Handle on screen unlock.");
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(env_, finishedCallback),
    };
    CallJsFunction("onScreenUnlock", argv, ARGC_ONE);
}


void RSWindowAnimationController::HandleOnWindowAnimationTargetsUpdate(
    const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
    const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets)
{
    WALOGD("Handle on window animation targets update.");
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, fullScreenWindowTarget),
        RSWindowAnimationUtils::CreateJsWindowAnimationTargetArray(env_, floatingWindowTargets),
    };
    CallJsFunction("onWindowAnimationTargetsUpdate", argv, ARGC_TWO);
}

void RSWindowAnimationController::HandleOnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget)
{
    WALOGD("Handle on wallpaper target update.");
    napi_value argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(env_, wallpaperTarget),
    };
    CallJsFunction("onWallpaperUpdate", argv, ARGC_ONE);
}

void RSWindowAnimationController::CallJsFunction(const std::string& methodName, napi_value const* argv, size_t argc)
{
    WALOGD("Call js function:%{public}s.", methodName.c_str());
    if (jsController_ == nullptr) {
        WALOGE("JsController is null!");
        return;
    }
    
    auto jsController_get = jsController_->Get();
    napi_ref ref = reinterpret_cast<napi_ref>(jsController_get);
    napi_value jsControllerValue = nullptr;
    napi_get_reference_value(env_, ref, &jsControllerValue);

    if (jsControllerValue == nullptr) {
        WALOGE("jsControllerValue is null!");
        return;
    }

    napi_value method = nullptr;
    napi_get_named_property(env_, jsControllerValue, methodName.c_str(), &method);
    if (method == nullptr) {
        WALOGE("Failed to get method from object!");
        return;
    }

    napi_call_function(env_, jsControllerValue, method, argc, argv, nullptr);
}
} // namespace Rosen
} // namespace OHOS
