/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
 
#ifndef ROSEN_RENDER_SERVICE_BASE_IPOINTER_LUMINANCE_CHANGE_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_IPOINTER_LUMINANCE_CHANGE_CALLBACK_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/pointer_render/pointer_luminance_change_callback.h"
#include "ipc_callbacks/pointer_render/pointer_luminance_change_callback_ipc_interface_code.h"
#include "ipc_callbacks/rs_ipc_callbacks_check.h"
namespace OHOS {
namespace Rosen {
class RSPointerLuminanceChangeCallbackProxy : public IRemoteProxy<RSIPointerLuminanceChangeCallback> {
public:
    explicit RSPointerLuminanceChangeCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSPointerLuminanceChangeCallbackProxy() noexcept = default;
 
    void OnPointerLuminanceChanged(int32_t brightness) override;
 
private:
    static inline BrokerDelegator<RSPointerLuminanceChangeCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
 
#endif // ROSEN_RENDER_SERVICE_BASE_IPOINTER_LUMINANCE_CHANGE_CALLBACK_PROXY_H