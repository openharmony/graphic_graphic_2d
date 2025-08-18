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

#include "screen_switching_notify_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
namespace Rosen {
RSScreenSwitchingNotifyCallbackProxy::RSScreenSwitchingNotifyCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIScreenSwitchingNotifyCallback>(impl)
{
}

void RSScreenSwitchingNotifyCallbackProxy::OnScreenSwitchingNotify(bool status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIScreenSwitchingNotifyCallback::GetDescriptor())) {
        ROSEN_LOGE("RSScreenSwitchingNotifyCallbackProxy::OnScreenSwitchingNotify WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteBool(status)) {
        ROSEN_LOGE("RSScreenSwitchingNotifyCallbackProxy::OnScreenSwitchingNotify WriteBool failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIScreenSwitchingNotifyCallbackInterfaceCode::ON_SCREEN_SWITCHING_NOTIFY);

    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSScreenSwitchingNotifyCallbackProxy::OnScreenSwitchingNotify error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS