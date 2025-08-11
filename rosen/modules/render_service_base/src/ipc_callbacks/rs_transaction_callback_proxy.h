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

#ifndef ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_PROXY_H

#include <iremote_proxy.h>

#include "ipc_callbacks/rs_transaction_callback.h"
#include "ipc_callbacks/rs_transaction_callback_ipc_interface_code.h"
#include "ipc_callbacks/rs_ipc_callbacks_check.h"
namespace OHOS {
namespace Rosen {
class RSTransactionCallbackProxy : public IRemoteProxy<RSITransactionCallback> {
public:
    explicit RSTransactionCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSTransactionCallbackProxy() noexcept = default;

    GSError OnCompleted(const OnCompletedRet& ret) override;
    GSError GoBackground(uint64_t queueId) override;
    GSError OnDropBuffers(const OnDropBuffersRet& ret) override;
    GSError SetDefaultWidthAndHeight(const OnSetDefaultWidthAndHeightRet& ret) override;
    GSError OnSurfaceDump(OnSurfaceDumpRet& ret) override;
private:
    static inline BrokerDelegator<RSTransactionCallbackProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_ITRANSACTION_CALLBACK_PROXY_H