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

#include "hgm_config_change_callback_stub.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
int RSHgmConfigChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (!securityManager_.IsInterfaceCodeAccessible(code)) {
        RS_LOGE("RSHgmConfigChangeCallbackStub::OnRemoteRequest no permission to access codeID=%{public}u.", code);
        return ERR_INVALID_STATE;
    }

    auto token = data.ReadInterfaceToken();
    if (token != RSIHgmConfigChangeCallback::GetDescriptor()) {
        return ERR_INVALID_STATE;
    }

    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED): {
            std::shared_ptr<RSHgmConfigData> configData(data.ReadParcelable<RSHgmConfigData>());
            OnHgmConfigChanged(configData);
            break;
        }
        case static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::
                                   ON_HGM_REFRESH_RATE_MODE_CHANGED): {
            int32_t refreshRateMode = data.ReadInt32();
            OnHgmRefreshRateModeChanged(refreshRateMode);
            break;
        }
        case static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_CHANGED): {
            int32_t refreshRate = data.ReadInt32();
            OnHgmRefreshRateUpdate(refreshRate);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }

    return ret;
}

const RSInterfaceCodeSecurityManager RSHgmConfigChangeCallbackStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<RSIHgmConfigChangeCallbackInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS
