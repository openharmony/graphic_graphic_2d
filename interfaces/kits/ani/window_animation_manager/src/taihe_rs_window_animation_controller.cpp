/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <memory>

#include "rs_trace.h"
#include "taihe_rs_window_animation_controller.h"
#include "taihe_rs_window_animation_log.h"
#include "taihe_window_animation_finished_callback.h"
#include "taihe_window_animation_utils.h"
#include "window_animation_finished_callback_impl.h"
#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif

using namespace taihe;
using namespace ohos::animation::windowAnimationManager;

namespace OHOS {
namespace Rosen {
thread_local std::unique_ptr<WindowAnimationController> jsController_ = nullptr;

TaiheRSWindowAnimationController::TaiheRSWindowAnimationController()
    : handler_(std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner()))
{}

void TaiheRSWindowAnimationController::SetJsController(WindowAnimationController jsController)
{
    TWA_LOGD("SetJsController");
    jsController_ = std::make_unique<WindowAnimationController>(jsController);
}

void TaiheRSWindowAnimationController::OnStartApp(StartingAppType type,
    const sptr<RSWindowAnimationTarget>& startingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Window animation controller on start app.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, type, startingWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnStartApp");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }
        if (!TaiheWindowAnimationUtils::IsSystemApp()) {
            TWA_LOGE("OnStartApp is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnStartApp(type, startingWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on start app error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::OnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Window animation controller on app transition.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, fromWindowTarget, toWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnAppTransition");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }
        if (!TaiheWindowAnimationUtils::IsSystemApp()) {
            TWA_LOGE("OnAppTransition is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnAppTransition(fromWindowTarget, toWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on app transition error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::OnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Window animation controller on app back transition.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, fromWindowTarget, toWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnAppBackTransition");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }

        controllerSptr->HandleOnAppBackTransition(fromWindowTarget, toWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on app back transition error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Window animation controller on minimize window.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, minimizingWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnMinimizeWindow");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }
        if (!TaiheWindowAnimationUtils::IsSystemApp()) {
            TWA_LOGE("OnMinimizeWindow is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnMinimizeWindow(minimizingWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on minimize window error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::OnMinimizeAllWindow(
    std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Window animation controller on minimize all windows.");
    if (minimizingWindowsTarget.empty()) {
        finishedCallback->OnAnimationFinished();
        TWA_LOGE("The minimizing Windows vector is empty!");
        return;
    }
    for (auto target : minimizingWindowsTarget) {
        sptr<RSIWindowAnimationFinishedCallback> animationCallback =
            new(std::nothrow) TaiheWindowAnimationFinishedCallback(finishedCallback);
        OnMinimizeWindow(target, animationCallback);
    }
}

void TaiheRSWindowAnimationController::OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Window animation controller on close window.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, closingWindowTarget, finishedCallback]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnCloseWindow");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }
        if (!TaiheWindowAnimationUtils::IsSystemApp()) {
            TWA_LOGE("OnCloseWindow is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnCloseWindow(closingWindowTarget, finishedCallback);
    };

    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on close window error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Window animation controller on screen unlock.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, finishedCallback]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnScreenUnlock");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }
        if (!TaiheWindowAnimationUtils::IsSystemApp()) {
            TWA_LOGE("OnScreenUnlock is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnScreenUnlock(finishedCallback);
    };
#ifdef SOC_PERF_ENABLE
    constexpr int32_t ACTION_TYPE_CPU_BOOST_CMDID = 10050;
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(ACTION_TYPE_CPU_BOOST_CMDID, true, "");
#endif
    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on screen unlock error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::OnWindowAnimationTargetsUpdate(
    const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
    const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets)
{
    TWA_LOGD("Window animation controller on window animation targets update.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, fullScreenWindowTarget, floatingWindowTargets]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnWindowAnimationTargetsUpdate");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }
        if (!TaiheWindowAnimationUtils::IsSystemApp()) {
            TWA_LOGE("OnWindowAnimationTargetsUpdate is not system app, not call js function.");
            return;
        }
        controllerSptr->HandleOnWindowAnimationTargetsUpdate(fullScreenWindowTarget, floatingWindowTargets);
    };

    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on window animation targets update error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget)
{
    TWA_LOGD("Window animation controller on wallpaper update.");
    wptr<TaiheRSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, wallpaperTarget]() {
        RS_TRACE_NAME("TaiheRSWindowAnimationController::OnWallpaperUpdate");
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            TWA_LOGE("Controller is null!");
            return;
        }

        controllerSptr->HandleOnWallpaperUpdate(wallpaperTarget);
    };

    if (handler_ == nullptr) {
        TWA_LOGE("Window animation controller on app transition error, handler is nullptr.");
        return;
    }
    handler_->PostTask(task);
}

void TaiheRSWindowAnimationController::HandleOnStartApp(StartingAppType type,
    const sptr<RSWindowAnimationTarget>& startingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Handle on start app.");
    auto jsWindowTarget = TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(startingWindowTarget);
    auto jsFinishedCb = TaiheWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(finishedCallback);

    switch (type) {
        case StartingAppType::FROM_LAUNCHER:
            jsController_->onStartAppFromLauncher(jsWindowTarget, jsFinishedCb);
            break;
        case StartingAppType::FROM_RECENT:
            jsController_->onStartAppFromRecent(jsWindowTarget, jsFinishedCb);
            break;
        case StartingAppType::FROM_OTHER:
            jsController_->onStartAppFromOther(jsWindowTarget, jsFinishedCb);
            break;
        default:
            TWA_LOGE("Unknow starting app type.");
            break;
    }
}

void TaiheRSWindowAnimationController::HandleOnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Handle on app transition.");
    auto jsFromWindowTarget = TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(fromWindowTarget);
    auto jsToWindowTarget = TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(toWindowTarget);
    auto jsFinishedCb = TaiheWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(finishedCallback);
    jsController_->onAppTransition(jsFromWindowTarget, jsToWindowTarget, jsFinishedCb);
}

void TaiheRSWindowAnimationController::HandleOnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Handle on app back transition.");
}

void TaiheRSWindowAnimationController::HandleOnMinimizeWindow(
    const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Handle on minimize window.");
    auto jsMinimizingWindowTarget = TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(minimizingWindowTarget);
    auto jsFinishedCb = TaiheWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(finishedCallback);
    jsController_->onMinimizeWindow(jsMinimizingWindowTarget, jsFinishedCb);
}

void TaiheRSWindowAnimationController::HandleOnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Handle on close window.");
    auto jsClosingWindowTarget = TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(closingWindowTarget);
    auto jsFinishedCb = TaiheWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(finishedCallback);
    jsController_->onCloseWindow(jsClosingWindowTarget, jsFinishedCb);
}

void TaiheRSWindowAnimationController::HandleOnScreenUnlock(
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    TWA_LOGD("Handle on screen unlock.");
    auto jsFinishedCb = TaiheWindowAnimationUtils::CreateJsWindowAnimationFinishedCallback(finishedCallback);
    jsController_->onScreenUnlock(jsFinishedCb);
}

void TaiheRSWindowAnimationController::HandleOnWindowAnimationTargetsUpdate(
    const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
    const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets)
{
    TWA_LOGD("Handle on window animation targets update.");
    auto jsFullScreenWindowTarget = TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(fullScreenWindowTarget);

    std::vector<WindowAnimationTarget> results;
    for (const auto& rsWindoeAnimationTarget : floatingWindowTargets) {
        results.emplace_back(TaiheWindowAnimationUtils::CreateJsWindowAnimationTarget(rsWindoeAnimationTarget));
    }
    auto jsFloatingWindowTargets = array<WindowAnimationTarget>(
        copy_data_t{}, results.data(), results.size());
    jsController_->onWindowAnimationTargetsUpdate(jsFullScreenWindowTarget, jsFloatingWindowTargets);
}

void TaiheRSWindowAnimationController::HandleOnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget)
{
    TWA_LOGD("Handle on wallpaper target update.");
}
} // namespace Rosen
} // namespace OHOS