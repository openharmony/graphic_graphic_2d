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

#include "brightness_info_change_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
namespace Rosen {
RSBrightnessInfoChangeCallbackProxy::RSBrightnessInfoChangeCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIBrightnessInfoChangeCallback>(impl)
{
}

bool RSBrightnessInfoChangeCallbackProxy::WriteBrightnessInfo(const BrightnessInfo& brightnessInfo, MessageParcel& data)
{
    if (!data.WriteFloat(brightnessInfo.currentHeadroom) ||
        !data.WriteFloat(brightnessInfo.maxHeadroom) ||
        !data.WriteFloat(brightnessInfo.sdrNits)) {
        ROSEN_LOGE("RSScreenChangeCallbackProxy::WriteBrightnessInfo write brightnessInfo failed");
        return false;
    }
    return true;
}

void RSBrightnessInfoChangeCallbackProxy::OnBrightnessInfoChange(ScreenId screenId,
    const BrightnessInfo& brightnessInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ROSEN_LOGE("RSBrightnessInfoChangeCallbackProxy::OnBrightnessInfoChange WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        ROSEN_LOGE("RSBrightnessInfoChangeCallbackProxy::OnBrightnessInfoChange WriteUint64 failed");
        return;
    }
    if (!WriteBrightnessInfo(brightnessInfo, data)) {
        ROSEN_LOGE("RSBrightnessInfoChangeCallbackProxy::OnBrightnessInfoChange WriteBrightnessInfo failed");
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIBrightnessInfoChangeCallbackInterfaceCode::ON_BRIGHTNESS_INFO_CHANGE);
    int32_t err = SendRequestRemote::SendRequest(Remote(), code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSBrightnessInfoChangeCallbackProxy::OnBrightnessInfoChange error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
