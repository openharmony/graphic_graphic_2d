/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "display_engine/ipc_callbacks/rs_de_status_change_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>
#include "ipc_callbacks/rs_ipc_callbacks_check.h"

namespace OHOS {
namespace Rosen {
RSDEStatusChangeCallbackProxy::RSDEStatusChangeCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIDEStatusChangeCallback>(impl)
{
}

void RSDEStatusChangeCallbackProxy::OnNotifyDEStatusChangeDone(const uint32_t sceneKey,
    const std::vector<uint8_t>& result)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIDEStatusChangeCallback::GetDescriptor())) {
        ROSEN_LOGE("RSDEStatusChangeCallbackProxy::OnNotifyDEStatusChangeDone WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(sceneKey)) {
        ROSEN_LOGE("RSDEStatusChangeCallbackProxy::OnNotifyDEStatusChangeDone WriteUint32 sceneKey failed");
        return;
    }
    if (!data.WriteUInt8Vector(result)) {
        ROSEN_LOGE("RSDEStatusChangeCallbackProxy::OnNotifyDEStatusChangeDone WriteUInt8Vector result failed");
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIDEStatusChangeCallbackInterfaceCode::ON_DE_STATUS_CHANGE_DONE);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSDEStatusChangeCallbackProxy::OnNotifyDEStatusChangeDone error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS