/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "screen_change_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
namespace Rosen {
RSScreenChangeCallbackProxy::RSScreenChangeCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIScreenChangeCallback>(impl)
{
}

void RSScreenChangeCallbackProxy::OnScreenChanged(ScreenId id, ScreenEvent event,
    ScreenChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if(!data.WriteInterfaceToken(RSIScreenChangeCallback::GetDescriptor())) {
        ROSEN_LOGE("RSScreenChangeCallbackProxy::OnScreenChanged WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint64(id)) {
        ROSEN_LOGE("RSScreenChangeCallbackProxy::OnScreenChanged WriteUint64 failed");
        return;
    }
    if (!data.WriteUint8(ECast(event))) {
        ROSEN_LOGE("RSScreenChangeCallbackProxy::OnScreenChanged WriteUint8 failed");
        return;
    }
    if (!data.WriteUint8(ECast(reason))) {
        ROSEN_LOGE("RSScreenChangeCallbackProxy::OnScreenChanged WriteUint8 reason failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIScreenChangeCallbackInterfaceCode::ON_SCREEN_CHANGED);
    
    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("RSScreenChangeCallbackProxy::OnScreenChanged: remote is null!");
        return;
    }

    int32_t err = remote->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSScreenChangeCallbackProxy::OnScreenChanged error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
