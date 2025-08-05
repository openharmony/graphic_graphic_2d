/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_IBUFFER_AVAILABLE_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_IBUFFER_AVAILABLE_CALLBACK_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_available_callback_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSBufferAvailableCallbackProxy : public IRemoteProxy<RSIBufferAvailableCallback> {
public:
    explicit RSBufferAvailableCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSBufferAvailableCallbackProxy() noexcept = default;

    void OnBufferAvailable() override;
    int32_t SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option);

private:
    static inline BrokerDelegator<RSBufferAvailableCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_IBUFFER_AVAILABLE_CALLBACK_PROXY_H
