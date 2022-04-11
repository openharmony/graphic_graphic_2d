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

void RSWindowAnimationProxy::OnTransition(const sptr<RSWindowAnimationTarget>& from,
                                          const sptr<RSWindowAnimationTarget>& to,
                                          const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on transition!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(from.GetRefPtr())) {
        WALOGE("Failed to write animation target from!");
        return;
    }

    if (!data.WriteParcelable(to.GetRefPtr())) {
        WALOGE("Failed to write animation target to!");
        return;
    }

    if (!data.WriteParcelable(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSIWindowAnimationController::ON_TRANSITION, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send transition request, error code:%d", ret);
    }
}

void RSWindowAnimationProxy::OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindow,
                                              const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on minimize window!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(minimizingWindow.GetRefPtr())) {
        WALOGE("Failed to write minimizing window!");
        return;
    }

    if (!data.WriteParcelable(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
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

void RSWindowAnimationProxy::OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindow,
                                           const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation proxy on close window!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(closingWindow.GetRefPtr())) {
        WALOGE("Failed to write closing window!");
        return;
    }

    if (!data.WriteParcelable(finishedCallback->AsObject())) {
        WALOGE("Failed to write finished callback!");
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
} // namespace Rosen
} // namespace OHOS
