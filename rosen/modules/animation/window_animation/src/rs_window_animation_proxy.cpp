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

#include "rs_window_animation_proxy.h"

#include "rs_iwindow_animation_finished_callback.h"
#include "rs_window_animation_log.h"
#include "rs_window_animation_target.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr int MAX_FLOATING_WINDOW_NUMBER = 100;
}
RSWindowAnimationProxy::RSWindowAnimationProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIWindowAnimationController>(impl)
{
}

bool RSWindowAnimationProxy::WriteInterfaceToken(MessageParcel& data)
{
    if (!data.WriteInterfaceToken(RSWindowAnimationProxy::GetDescriptor())) {
        WALOGE("Failed to write interface token!");
        return false;
    }

    return true;
}

bool RSWindowAnimationProxy::WriteTargetAndCallback(MessageParcel& data,
    const sptr<RSWindowAnimationTarget>& windowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    if (!data.WriteParcelable(windowTarget.GetRefPtr())) {
        WALOGE("Failed to write window animation target!");
        return false;
    }

    if (!data.WriteRemoteObject(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
        return false;
    }

    return true;
}

void RSWindowAnimationProxy::OnStartApp(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on start app!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteInt32(type)) {
        WALOGE("Failed to write starting app type!");
        return;
    }

    if (!WriteTargetAndCallback(data, startingWindowTarget, finishedCallback)) {
        WALOGE("Failed to write window animation target or callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_START_APP, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send start app request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on app transition!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(fromWindowTarget.GetRefPtr())) {
        WALOGE("Failed to write from animation target!");
        return;
    }

    if (!data.WriteParcelable(toWindowTarget.GetRefPtr())) {
        WALOGE("Failed to write to animation target!");
        return;
    }

    if (!data.WriteRemoteObject(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_APP_TRANSITION, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send app transition request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
    const sptr<RSWindowAnimationTarget>& toWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on app back transition!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(fromWindowTarget.GetRefPtr())) {
        WALOGE("Failed to write from animation target!");
        return;
    }

    if (!data.WriteParcelable(toWindowTarget.GetRefPtr())) {
        WALOGE("Failed to write to animation target!");
        return;
    }

    if (!data.WriteRemoteObject(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send app back transition request, error code:%d", ret);
    }
}


void RSWindowAnimationProxy::OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on minimize window!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!WriteTargetAndCallback(data, minimizingWindowTarget, finishedCallback)) {
        WALOGE("Failed to write window animation target or callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_MINIMIZE_WINDOW, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send minimize window request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnMinimizeAllWindow(std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    if (minimizingWindowsTarget.empty()) {
        if (finishedCallback != nullptr) {
            finishedCallback->OnAnimationFinished();
        }
        WALOGE("The minimizing Windows vector is empty!");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on minimize all windows!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteUint32(minimizingWindowsTarget.size())) {
        WALOGE("Failed to write minimizing animation target size!");
        return;
    }

    for (auto& target : minimizingWindowsTarget) {
        if (!data.WriteParcelable(target.GetRefPtr())) {
            WALOGE("Failed to write minimizing animation target!");
            return;
        }
    }

    if (!data.WriteRemoteObject(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send minimize all windows request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
    const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on close window!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!WriteTargetAndCallback(data, closingWindowTarget, finishedCallback)) {
        WALOGE("Failed to write window animation target or callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_CLOSE_WINDOW, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send close window request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on screen unlock!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteRemoteObject(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_SCREEN_UNLOCK, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send screen unlock request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnWindowAnimationTargetsUpdate(
    const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
    const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets)
{
    if (floatingWindowTargets.size() > MAX_FLOATING_WINDOW_NUMBER) {
        WALOGE("Floating windows are too much!");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on window animation targets update!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (fullScreenWindowTarget.GetRefPtr() == nullptr) {
        if (!data.WriteBool(false)) {
            WALOGE("Failed to write null full Screen Window Target!");
            return;
        }
    } else {
        if (!data.WriteBool(true) || !data.WriteParcelable(fullScreenWindowTarget.GetRefPtr())) {
            WALOGE("Failed to write full screen animation target!");
            return;
        }
    }

    if (!data.WriteUint32(floatingWindowTargets.size())) {
        WALOGE("Failed to write floating animation target size!");
        return;
    }

    for (auto& target : floatingWindowTargets) {
        if (!data.WriteParcelable(target.GetRefPtr())) {
            WALOGE("Failed to write floating animation target!");
            return;
        }
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE,
        data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send window animation targets update request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on wallpaper update!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(wallpaperTarget.GetRefPtr())) {
        WALOGE("Failed to write wallpaper update target!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_WALLPAPER_UPDATE, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send wallpaper update request, error code:%d", ret);
    }
}
} // namespace Rosen
} // namespace OHOS
