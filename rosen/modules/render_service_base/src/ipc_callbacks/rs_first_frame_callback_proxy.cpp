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

#include "rs_first_frame_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSFirstFrameCallbackProxy::RSFirstFrameCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIFirstFrameCallback>(impl)
{
}

void RSFirstFrameCallbackProxy::OnPowerOnFirstFrame(uint32_t screenId, int64_t timestamp)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIFirstFrameCallback::GetDescriptor())) {
        ROSEN_LOGE("RSFirstFrameCallbackProxy::OnPowerOnFirstFrame WriteInterfaceToken failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    if (!data.WriteUint32(screenId)) {
        ROSEN_LOGE("RSFirstFrameCallbackProxy::OnPowerOnFirstFrame WriteUint32 failed");
        return;
    }
    if (!data.WriteInt64(timestamp)) {
        ROSEN_LOGE("RSFirstFrameCallbackProxy::OnPowerOnFirstFrame WriteInt64 failed");
        return;
    }
    uint32_t code =
        static_cast<uint32_t>(RSIFirstFrameCallbackInterfaceCode::ON_POWER_ON);
    auto remote = Remote();
    if (remote == nullptr) {
        ROSEN_LOGE("RSFirstFrameCallbackProxy::OnPowerOnFirstFrame remote is null!");
        return;
    }

    int32_t err = remote->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSFirstFrameCallbackProxy::OnPowerOnFirstFrame error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
