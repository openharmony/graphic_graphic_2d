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

#ifndef RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_CONNECTION_H
#define RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_CONNECTION_H

#include <mutex>
#include <unordered_set>

#include "pipeline/rs_render_service.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_render_service_connection_stub.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceConnection : public RSRenderServiceConnectionStub {
public:
    RSRenderServiceConnection(
        wptr<RSRenderService> renderService,
        RSMainThread* mainThread,
        sptr<RSScreenManager> screenManager,
        sptr<IRemoteObject> token);
    ~RSRenderServiceConnection() noexcept;
    RSRenderServiceConnection(const RSRenderServiceConnection&) = delete;
    RSRenderServiceConnection& operator=(const RSRenderServiceConnection&) = delete;

    sptr<IRemoteObject> GetToken() const
    {
        return token_;
    }

private:
    void CleanAll(bool toDelete = false) noexcept;

    // IPC RSIRenderServiceConnection Interfaces
    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) override;

    ScreenId GetDefaultScreenId() override;

    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int32_t flags = 0) override;

    void RemoveVirtualScreen(ScreenId id) override;

    void SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) override;

    void SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback) override;

    void RegisterApplicationRenderThread(uint32_t pid, sptr<IApplicationRenderThread> app) override;

    void UnregisterApplicationRenderThread(sptr<IApplicationRenderThread> app);

    RSScreenModeInfo GetScreenActiveMode(ScreenId id) override;

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) override;

    RSScreenCapability GetScreenCapability(ScreenId id) override;

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) override;

    RSScreenData GetScreenData(ScreenId id) override;

    int32_t GetScreenBacklight(ScreenId id) override;

    void SetScreenBacklight(ScreenId id, uint32_t level) override;

    wptr<RSRenderService> renderService_;
    RSMainThread* mainThread_ = nullptr;
    sptr<RSScreenManager> screenManager_;
    sptr<IRemoteObject> token_;

    class RSConnectionDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RSConnectionDeathRecipient(wptr<RSRenderServiceConnection> conn);
        virtual ~RSConnectionDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;

    private:
        wptr<RSRenderServiceConnection> conn_;
    };
    friend class RSConnectionDeathRecipient;
    sptr<RSConnectionDeathRecipient> connDeathRecipient_;

    class RSApplicationRenderThreadDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RSApplicationRenderThreadDeathRecipient(wptr<RSRenderServiceConnection> conn);
        virtual ~RSApplicationRenderThreadDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;

    private:
        wptr<RSRenderServiceConnection> conn_;
    };
    friend class RSApplicationRenderThreadDeathRecipient;
    sptr<RSApplicationRenderThreadDeathRecipient> ApplicationDeathRecipient_;

    mutable std::mutex mutex_;
    bool cleanDone_ = false;

    // save all virtual screenIds created by this connection.
    std::unordered_set<ScreenId> virtualScreenIds_;
    sptr<RSIScreenChangeCallback> screenChangeCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_CONNECTION_H
