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

#include "rs_window_animation_stub.h"

#include "rs_iwindow_animation_finished_callback.h"
#include "rs_window_animation_log.h"
#include "rs_window_animation_target.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr int MAX_FLOATING_WINDOW_NUMBER = 100;
static constexpr int MAX_WINDOW_NUMBER = 100;
}
int RSWindowAnimationStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption &option)
{
    WALOGD("Window animation on remote request!");
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WALOGE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    switch (code) {
        case RSIWindowAnimationController::ON_START_APP:
            return StartApp(data, reply);
        case RSIWindowAnimationController::ON_APP_TRANSITION:
            return AppTransition(data, reply);
        case RSIWindowAnimationController::ON_APP_BACK_TRANSITION:
            return AppBackTransition(data, reply);
        case RSIWindowAnimationController::ON_MINIMIZE_WINDOW:
            return MinimizeWindow(data, reply);
        case RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW:
            return MinimizeAllWindow(data, reply);
        case RSIWindowAnimationController::ON_CLOSE_WINDOW:
            return CloseWindow(data, reply);
        case RSIWindowAnimationController::ON_SCREEN_UNLOCK:
            return ScreenUnlock(data, reply);
        case RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE:
            return WindowAnimationTargetsUpdate(data, reply);
        case RSIWindowAnimationController::ON_WALLPAPER_UPDATE:
            return WallpaperUpdate(data, reply);
        default:
            break;
    }
    WALOGE("Failed to find function handler!");
    return ERR_UNKNOWN_TRANSACTION;
}

int RSWindowAnimationStub::StartApp(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation start app!");
    StartingAppType type = static_cast<StartingAppType>(data.ReadInt32());
    sptr<RSWindowAnimationTarget> startingWindowTarget(data.ReadParcelable<RSWindowAnimationTarget>());
    if (startingWindowTarget == nullptr) {
        WALOGE("Failed to read starting window target!");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> finishcallbackObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback =
        iface_cast<RSIWindowAnimationFinishedCallback>(finishcallbackObject);
    if (finishedCallback == nullptr) {
        WALOGE("Failed to read animation finished callback!");
        return ERR_INVALID_DATA;
    }

    OnStartApp(type, startingWindowTarget, finishedCallback);
    return ERR_NONE;
}

int RSWindowAnimationStub::AppTransition(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation transition!");
    sptr<RSWindowAnimationTarget> fromWindowTarget(data.ReadParcelable<RSWindowAnimationTarget>());
    if (fromWindowTarget == nullptr) {
        WALOGE("AppTransition failed to read animation target from!");
        return ERR_INVALID_DATA;
    }

    sptr<RSWindowAnimationTarget> toWindowTarget(data.ReadParcelable<RSWindowAnimationTarget>());
    if (toWindowTarget == nullptr) {
        WALOGE("AppTransition failed to read animation target to!");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> finishcallbackObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback =
        iface_cast<RSIWindowAnimationFinishedCallback>(finishcallbackObject);
    if (finishedCallback == nullptr) {
        WALOGE("AppTransition failed to read animation finished callback!");
        return ERR_INVALID_DATA;
    }

    OnAppTransition(fromWindowTarget, toWindowTarget, finishedCallback);
    return ERR_NONE;
}

int RSWindowAnimationStub::AppBackTransition(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation back transition!");
    sptr<RSWindowAnimationTarget> fromWindowTarget(data.ReadParcelable<RSWindowAnimationTarget>());
    if (fromWindowTarget == nullptr) {
        WALOGE("AppBackTransition failed to read animation target from!");
        return ERR_INVALID_DATA;
    }

    sptr<RSWindowAnimationTarget> toWindowTarget(data.ReadParcelable<RSWindowAnimationTarget>());
    if (toWindowTarget == nullptr) {
        WALOGE("AppBackTransition failed to read animation target to!");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> finishcallbackObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback =
        iface_cast<RSIWindowAnimationFinishedCallback>(finishcallbackObject);
    if (finishedCallback == nullptr) {
        WALOGE("AppBackTransition failed to read animation finished callback!");
        return ERR_INVALID_DATA;
    }

    OnAppBackTransition(fromWindowTarget, toWindowTarget, finishedCallback);
    return ERR_NONE;
}

int RSWindowAnimationStub::MinimizeWindow(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation minimize window!");
    sptr<RSWindowAnimationTarget> minimizingWindow(data.ReadParcelable<RSWindowAnimationTarget>());
    if (minimizingWindow == nullptr) {
        WALOGE("Failed to read minimizing window!");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> finishcallbackObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback =
        iface_cast<RSIWindowAnimationFinishedCallback>(finishcallbackObject);
    if (finishedCallback == nullptr) {
        WALOGE("MinimizeWindow failed to read animation finished callback!");
        return ERR_INVALID_DATA;
    }

    OnMinimizeWindow(minimizingWindow, finishedCallback);
    return ERR_NONE;
}

int RSWindowAnimationStub::MinimizeAllWindow(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation minimize all window!");
    size_t dataCount = data.ReadUint32();
    if (dataCount > MAX_WINDOW_NUMBER) {
        WALOGE("Windows are too much!");
        return ERR_INVALID_DATA;
    }
    std::vector<sptr<RSWindowAnimationTarget>> minimizingWindows;
    for (size_t i = 0; i < dataCount; i++) {
        sptr<RSWindowAnimationTarget> minimizingWindow(data.ReadParcelable<RSWindowAnimationTarget>());
        if (minimizingWindow == nullptr) {
            WALOGE("Failed to read minimizing window!");
            return ERR_INVALID_DATA;
        }
        minimizingWindows.push_back(minimizingWindow);
    }

    sptr<IRemoteObject> finishcallbackObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback =
        iface_cast<RSIWindowAnimationFinishedCallback>(finishcallbackObject);
    if (finishedCallback == nullptr) {
        WALOGE("MinimizeAllWindow failed to read animation finished callback!");
        return ERR_INVALID_DATA;
    }

    OnMinimizeAllWindow(minimizingWindows, finishedCallback);
    return ERR_NONE;
}

int RSWindowAnimationStub::CloseWindow(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation close window!");
    sptr<RSWindowAnimationTarget> closingWindow(data.ReadParcelable<RSWindowAnimationTarget>());
    if (closingWindow == nullptr) {
        WALOGE("Failed to read closing window!");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> finishcallbackObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback =
        iface_cast<RSIWindowAnimationFinishedCallback>(finishcallbackObject);
    if (finishedCallback == nullptr) {
        WALOGE("CloseWindow failed to read animation finished callback!");
        return ERR_INVALID_DATA;
    }

    OnCloseWindow(closingWindow, finishedCallback);
    return ERR_NONE;
}

int RSWindowAnimationStub::ScreenUnlock(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation screen unlock!");
    sptr<IRemoteObject> finishcallbackObject = data.ReadRemoteObject();
    sptr<RSIWindowAnimationFinishedCallback> finishedCallback =
        iface_cast<RSIWindowAnimationFinishedCallback>(finishcallbackObject);
    if (finishedCallback == nullptr) {
        WALOGE("ScreenUnlock failed to read animation finished callback!");
        return ERR_INVALID_DATA;
    }

    OnScreenUnlock(finishedCallback);
    return ERR_NONE;
}

int RSWindowAnimationStub::WindowAnimationTargetsUpdate(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation targets update!");
    sptr<RSWindowAnimationTarget> fullScreenWindowTarget = nullptr;
    bool isFullScreenWindowTarget;
    if (!data.ReadBool(isFullScreenWindowTarget)) {
        WALOGE("Failed to read whether there is full screen window target!");
        return ERR_INVALID_DATA;
    }
    
    if (isFullScreenWindowTarget) {
        fullScreenWindowTarget = data.ReadParcelable<RSWindowAnimationTarget>();
        if (fullScreenWindowTarget == nullptr) {
            WALOGE("Failed to read full screen window animation target!");
            return ERR_INVALID_DATA;
        }
    }

    size_t floatWindowSize = data.ReadUint32();
    if (floatWindowSize > MAX_FLOATING_WINDOW_NUMBER) {
        WALOGE("Floating windows are too much!");
        return ERR_INVALID_DATA;
    }
    std::vector<sptr<RSWindowAnimationTarget>> floatingWindowTargets;
    for (size_t i = 0; i < floatWindowSize; i++) {
        sptr<RSWindowAnimationTarget> floatingWindowTarget(data.ReadParcelable<RSWindowAnimationTarget>());
        if (floatingWindowTarget == nullptr) {
            WALOGE("Failed to read floating window animation window!");
            return ERR_INVALID_DATA;
        }
        floatingWindowTargets.push_back(floatingWindowTarget);
    }

    OnWindowAnimationTargetsUpdate(fullScreenWindowTarget, floatingWindowTargets);
    return ERR_NONE;
}

int RSWindowAnimationStub::WallpaperUpdate(MessageParcel& data, MessageParcel& reply)
{
    WALOGD("Window animation wallpaper update!");
    sptr<RSWindowAnimationTarget> wallpaperTarget(data.ReadParcelable<RSWindowAnimationTarget>());
    OnWallpaperUpdate(wallpaperTarget);
    return ERR_NONE;
}
} // namespace Rosen
} // namespace OHOS
