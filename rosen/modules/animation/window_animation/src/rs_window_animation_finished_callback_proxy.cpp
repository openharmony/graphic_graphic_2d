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

#include "rs_window_animation_finished_callback_proxy.h"

#include "rs_window_animation_log.h"

namespace OHOS {
namespace Rosen {
RSWindowAnimationFinishedCallbackProxy::RSWindowAnimationFinishedCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIWindowAnimationFinishedCallback>(impl)
{
}

bool RSWindowAnimationFinishedCallbackProxy::WriteInterfaceToken(MessageParcel& data)
{
    if (!data.WriteInterfaceToken(RSWindowAnimationFinishedCallbackProxy::GetDescriptor())) {
        WALOGE("Failed to write interface token!");
        return false;
    }

    return true;
}

void RSWindowAnimationFinishedCallbackProxy::OnAnimationFinished()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    WALOGD("Window animation finished callback proxy on transition!");
    if (!WriteInterfaceToken(data)) {
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        WALOGE("remote is null!");
        return;
    }

    auto ret = remote->SendRequest(RSWindowAnimationFinishedCallbackProxy::ON_ANIMATION_FINISHED, data, reply, option);
    if (ret != NO_ERROR) {
        WALOGE("Failed to send animation finished callback request, error code:%d", ret);
    }
}
} // namespace Rosen
} // namespace OHOS
