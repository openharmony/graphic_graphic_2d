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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_SERVICE_CONNECT_HUB_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_SERVICE_CONNECT_HUB_H

#include <mutex>
#include <iremote_object.h>
#include "platform/ohos/transaction/rs_irender_service_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSIConnectionToken;
class RSIClientToServiceConnection;
class RSClientToServiceConnectHub : public RefBase {
public:
    static sptr<RSIClientToServiceConnection> GetClientToServiceConnection();
    static sptr<RSClientToServiceConnectHub> GetInstance();
    RSIConnectionToken* GetToken()
    {
        return token_.GetRefPtr();
    }
    void ConnectDied();

private:
    RSClientToServiceConnectHub();
    ~RSClientToServiceConnectHub() noexcept;
    static void Init();
    static void Destroy();
    bool Connect();

    class RenderServiceDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        explicit RenderServiceDeathRecipient(wptr<RSClientToServiceConnectHub> hub)
            : hub_(hub) {}
        ~RenderServiceDeathRecipient() noexcept final = default;
        DISALLOW_COPY_AND_MOVE(RenderServiceDeathRecipient);
        void OnRemoteDied(const wptr<IRemoteObject>& remote) final override;
    private:
        wptr<RSClientToServiceConnectHub> hub_;
    };

    std::mutex mutex_;
    sptr<IRemoteObject> renderService_;
    sptr<RSIConnectionToken> token_;
    sptr<RSIClientToServiceConnection> conn_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;

    static std::once_flag flag_;
    static sptr<RSClientToServiceConnectHub> instance_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_SERVICE_CONNECT_HUB_H
