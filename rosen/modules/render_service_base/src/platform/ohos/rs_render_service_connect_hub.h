/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <unordered_map>
#include <functional>
#include "platform/common/rs_log.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
namespace OHOS {
namespace Rosen {
using OnConnectCallback = std::function<void(sptr<RSIClientToRenderConnection>&)>;
class RSRenderServiceConnectHub;

class RSConnectRenderProcessDeathRecipient final : public IRemoteObject::DeathRecipient {
public:
    explicit RSConnectRenderProcessDeathRecipient(wptr<RSRenderServiceConnectHub> rsConnHub, uint64_t tokenMaskId)
        : rsConnHub_(rsConnHub), tokenMaskId_(tokenMaskId) {}
    ~RSConnectRenderProcessDeathRecipient() noexcept final = default;

    DISALLOW_COPY_AND_MOVE(RSConnectRenderProcessDeathRecipient);

    void OnRemoteDied(const wptr<IRemoteObject> &remote) final override;
    void SetOnRenderProcessDiedCallback(std::function<void()> callback)
    {
        callback_ = callback;
    }

private:
    wptr<RSRenderServiceConnectHub> rsConnHub_;
    std::function<void()> callback_;
    uint64_t tokenMaskId_;
};

struct RenderProcessInfo {
    sptr<RSIConnectionToken> token;
    sptr<IRemoteObject> connectToRenderRemote;
    sptr<RSIConnectToRenderProcess> renderProcess;
    sptr<RSConnectRenderProcessDeathRecipient> deathRecipient;
    sptr<RSIClientToRenderConnection> clientToRenderConnection;
};

enum class RSOnDiedCallbackCode : int32_t {
    APPLICATION_AGENT = 0,
};

class RSRenderServiceConnectHub : public RefBase {
public:
    static std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> GetRenderService();
    static sptr<RSIClientToServiceConnection> GetClientToServiceConnection();
    static sptr<RSIClientToRenderConnection> GetClientToRenderConnection(uint64_t tokenMaskId);
    static uint64_t GetDefaultTokenMaskId();
    static uint64_t GetRenderProcessTokenMaskId(sptr<IRemoteObject>& connectToRenderRemote);
    static void SetOnConnectCallback(OnConnectCallback cb)
    {
        onConnectCallback_ = cb;
        // if already connected, call the callback immediately
        if (instance_ && instance_->renderConn_ && onConnectCallback_) {
            onConnectCallback_(instance_->renderConn_);
        }
    }

    static void SetOnDiedCallback(RSOnDiedCallbackCode code, std::function<void()> cb);
    static void RemoveOnDiedCallback(RSOnDiedCallbackCode code, bool isDestreuctionProcess);

    static sptr<RSRenderServiceConnectHub> GetConnectHubInstance()
    {
        return instance_;
    }

    uint64_t FindTokenMaskIdByRenderRemote(sptr<IRemoteObject>& connectToRenderRemote);
    void SetOnRenderProcessDiedCallback(uint64_t tokenMaskId, std::function<void()> callback);

    void AddRenderProcessConnectionToken(uint64_t tokenMaskId, sptr<RSIConnectionToken> token,
        sptr<IRemoteObject> connectToRenderRemote,
        sptr<RSIConnectToRenderProcess> renderPrecess,
        sptr<RSIClientToRenderConnection> clientToRenderConnection);
    RSIConnectionToken* GetToken()
    {
        return token_.GetRefPtr();
    }
    void ConnectRenderProcessDied(uint64_t tokenMaskId);
    // Temporary interface for same-layer rendering and non-multi-instance-adapted interfaces
    uint64_t GetDefaultTokenMaskIdInner();
    sptr<RSIClientToRenderConnection> FindClientToRenderConnection(uint64_t tokenMaskId);
    
private:
    static sptr<RSRenderServiceConnectHub> GetInstance();
    static void Init();
    static void Destroy();

    RSRenderServiceConnectHub();
    ~RSRenderServiceConnectHub() noexcept;

    class RenderServiceDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        explicit RenderServiceDeathRecipient(wptr<RSRenderServiceConnectHub> rsConnHub)
            : rsConnHub_(rsConnHub) {}
        ~RenderServiceDeathRecipient() noexcept final = default;

        DISALLOW_COPY_AND_MOVE(RenderServiceDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final override;

    private:
        wptr<RSRenderServiceConnectHub> rsConnHub_;
    };

    std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> GetRenderServiceConnection();
    void CleanConnectRenderProcess();
    void RemoveRenderProcessDeathRecipient(uint64_t tokenMaskId, sptr<RSIConnectToRenderProcess> renderProcess);
    void ExecuteAndClearDiedCallbacks();
    bool Connect();
    void ConnectDied();

    mutable std::mutex mutex_;
    sptr<RSIRenderService> renderService_;
    sptr<RSIConnectionToken> token_;
    sptr<RSIClientToServiceConnection> conn_;
    sptr<RSIClientToRenderConnection> renderConn_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    std::mutex renderPipelineClientMutex_;
    std::map<std::uint64_t, RenderProcessInfo> connRenderProcesses_;

    static std::once_flag flag_;
    static sptr<RSRenderServiceConnectHub> instance_;
    static OnConnectCallback onConnectCallback_;
    std::mutex onDiedCallbacksMutex_;
    std::unordered_map<int32_t, std::function<void()>> OnDiedCallbacks_;
    friend class RSRenderPipelineClient;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECT_HUB_H
