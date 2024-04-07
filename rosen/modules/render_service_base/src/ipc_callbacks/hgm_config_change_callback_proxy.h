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

#ifndef ROSEN_RENDER_SERVICE_BASE_IHGM_CONFIG_CHANGE_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_IHGM_CONFIG_CHANGE_CALLBACK_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/rs_ihgm_config_change_callback.h"
#include "ipc_callbacks/rs_ihgm_config_change_callback_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSHgmConfigChangeCallbackProxy : public IRemoteProxy<RSIHgmConfigChangeCallback> {
public:
    explicit RSHgmConfigChangeCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSHgmConfigChangeCallbackProxy() noexcept = default;

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override;
    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override;
    void OnHgmRefreshRateUpdate(int32_t refreshRate) override;

private:
    static inline BrokerDelegator<RSHgmConfigChangeCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_IHGM_CONFIG_CHANGE_CALLBACK_PROXY_H