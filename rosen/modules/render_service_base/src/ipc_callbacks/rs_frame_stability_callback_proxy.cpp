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

#include "rs_frame_stability_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSFrameStabilityCallbackProxy::RSFrameStabilityCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIFrameStabilityCallback>(impl)
{
}

void RSFrameStabilityCallbackProxy::OnFrameStabilityChanged(bool isStable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIFrameStabilityCallback::GetDescriptor())) {
        ROSEN_LOGE("RSFrameStabilityCallbackProxy::OnFrameStabilityChanged WriteInterfaceToken failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteBool(isStable)) {
        ROSEN_LOGE("RSFrameStabilityCallbackProxy::OnFrameStabilityChanged WriteBool failed");
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIFrameStabilityCallbackInterfaceCode::ON_FRAME_STABILITY_CHANGED);
    
    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("remote is null!");
        return;
    }

    int32_t err = remote->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSFrameStabilityCallbackProxy::OnFrameStabilityChanged error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
