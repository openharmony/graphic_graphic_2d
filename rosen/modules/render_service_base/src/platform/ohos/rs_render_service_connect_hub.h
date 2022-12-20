/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECT_HUB_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECT_HUB_H

#include <mutex>
#include <platform/ohos/rs_irender_service.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
using OnConnectCallback = std::function<void(sptr<RSIRenderServiceConnection>&)>;
class RSRenderServiceConnectHub : public RefBase {
public:
    static sptr<RSIRenderServiceConnection> GetRenderService();
    static void SetOnConnectCallback(OnConnectCallback cb)
    {
        onConnectCallback_ = cb;
        // if already connected, call the callback immediately
        if (instance_ && instance_->conn_ && onConnectCallback_) {
            onConnectCallback_(instance_->conn_);
        }
    }

private:
    static RSRenderServiceConnectHub* GetInstance();
    static void Init();
    static void Destroy();

    RSRenderServiceConnectHub();
    ~RSRenderServiceConnectHub() noexcept;

    class RenderServiceDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        RenderServiceDeathRecipient(wptr<RSRenderServiceConnectHub> rsConnHub)
            : rsConnHub_(std::move(rsConnHub)) {}
        ~RenderServiceDeathRecipient() noexcept final = default;

        DISALLOW_COPY_AND_MOVE(RenderServiceDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final override;

    private:
        wptr<RSRenderServiceConnectHub> rsConnHub_;
    };

    sptr<RSIRenderServiceConnection> GetRenderServiceConnection();
    bool Connect();
    void ConnectDied();

    mutable std::mutex mutex_;
    sptr<RSIRenderService> renderService_;
    sptr<RSIConnectionToken> token_;
    sptr<RSIRenderServiceConnection> conn_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;

    static std::once_flag flag_;
    static RSRenderServiceConnectHub* instance_;
    static OnConnectCallback onConnectCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECT_HUB_H
