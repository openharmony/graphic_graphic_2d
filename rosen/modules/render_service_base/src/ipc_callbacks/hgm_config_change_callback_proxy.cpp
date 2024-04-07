/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hgm_config_change_callback_proxy.h"

#include <message_option.h>
#include <message_parcel.h>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSHgmConfigChangeCallbackProxy::RSHgmConfigChangeCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<RSIHgmConfigChangeCallback>(impl)
{
}

void RSHgmConfigChangeCallbackProxy::OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIHgmConfigChangeCallback::GetDescriptor())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    data.WriteParcelable(configData.get());
    uint32_t code = static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSHgmConfigChangeCallbackProxy::OnHgmConfigChanged error = %{public}d", err);
    }
}

void RSHgmConfigChangeCallbackProxy::OnHgmRefreshRateModeChanged(int32_t refreshRateMode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(RSIHgmConfigChangeCallback::GetDescriptor())) {
        return;
    }

    option.SetFlags(MessageOption::TF_ASYNC);
    data.WriteInt32(refreshRateMode);
    uint32_t code =
        static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_MODE_CHANGED);
    int32_t err = Remote()->SendRequest(code, data, reply, option);
    if (err != NO_ERROR) {
        ROSEN_LOGE("RSHgmRefreshRateModeChangeCallbackProxy::OnHgmRefreshRateModeChanged error = %{public}d", err);
    }
}
} // namespace Rosen
} // namespace OHOS
