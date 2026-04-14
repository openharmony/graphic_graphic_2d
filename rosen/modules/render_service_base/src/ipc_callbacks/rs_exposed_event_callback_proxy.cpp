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

#include "rs_exposed_event_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "ipc_callbacks/rs_iexposed_event_callback_ipc_interface_code.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSExposedEventCallbackProxy::RSExposedEventCallbackProxy(
    const sptr<IRemoteObject>& impl) : IRemoteProxy<RSIExposedEventCallback>(impl)
{
}

void RSExposedEventCallbackProxy::OnDisplayEvent(const std::shared_ptr<RSExposedEventDataBase> data)
{
    MessageParcel parcelData;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIExposedEventCallbackInterfaceCode::ON_EXPOSED_EVENT);

    if (!parcelData.WriteInterfaceToken(RSIExposedEventCallback::GetDescriptor())) {
        ROSEN_LOGE("RSExposedEventCallbackProxy::OnDisplayEvent WriteInterfaceToken failed");
        return;
    }
    if (!parcelData.WriteParcelable(data.get())) {
        ROSEN_LOGE("RSExposedEventCallbackProxy::OnDisplayEvent WriteParcelable failed");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("RSExposedEventCallbackProxy::OnDisplayEvent: remote is null!");
        return;
    }

    int32_t err = remote->SendRequest(code, parcelData, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSExposedEventCallbackProxy::OnDisplayEvent error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS