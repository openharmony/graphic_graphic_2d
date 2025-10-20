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

#ifndef ROSEN_RENDER_SERVICE_BASE_IBRIGHTNESS_INFO_CHANGE_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_IBRIGHTNESS_INFO_CHANGE_CALLBACK_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/brightness_info_change_callback.h"
#include "ipc_callbacks/brightness_info_change_callback_ipc_interface_code.h"
#include "ipc_callbacks/rs_ipc_callbacks_check.h"

namespace OHOS {
namespace Rosen {
class RSBrightnessInfoChangeCallbackProxy : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit RSBrightnessInfoChangeCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSBrightnessInfoChangeCallbackProxy() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override;

private:
    bool WriteBrightnessInfo(const BrightnessInfo& brightnessInfo, MessageParcel& data);
    static inline BrokerDelegator<RSBrightnessInfoChangeCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_IBRIGHTNESS_INFO_CHANGE_CALLBACK_PROXY_H
