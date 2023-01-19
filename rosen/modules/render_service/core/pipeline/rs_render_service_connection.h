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

#ifndef RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_CONNECTION_H
#define RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_CONNECTION_H

#include <mutex>
#include <unordered_set>

#include "ipc_callbacks/buffer_available_callback.h"
#include "pipeline/rs_render_service.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "vsync_distributor.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceConnection : public RSRenderServiceConnectionStub {
public:
    RSRenderServiceConnection(
        pid_t remotePid,
        wptr<RSRenderService> renderService,
        RSMainThread* mainThread,
        sptr<RSScreenManager> screenManager,
        sptr<IRemoteObject> token,
        sptr<VSyncDistributor> distributor);
    ~RSRenderServiceConnection() noexcept;
    RSRenderServiceConnection(const RSRenderServiceConnection&) = delete;
    RSRenderServiceConnection& operator=(const RSRenderServiceConnection&) = delete;

    sptr<IRemoteObject> GetToken() const
    {
        return token_;
    }

private:
    void CleanVirtualScreens() noexcept;
    void CleanRenderNodes() noexcept;
    void CleanAll(bool toDelete = false) noexcept;

    // IPC RSIRenderServiceConnection Interfaces
    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    int32_t SetRenderModeChangeCallback(sptr<RSIRenderModeChangeCallback> callback) override;
    void UpdateRenderMode(bool isUniRender) override;
    bool GetUniRenderEnabled() override;
    bool QueryIfRTNeedRender() override;

    bool CreateNode(const RSSurfaceRenderNodeConfig& config) override;
    sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) override;

    sptr<IVSyncConnection> CreateVSyncConnection(const std::string& name) override;

    int32_t SetFocusAppInfo(
        int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName) override;

    ScreenId GetDefaultScreenId() override;

    std::vector<ScreenId> GetAllScreenIds() override;

    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int32_t flags = 0) override;

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) override;

    void RemoveVirtualScreen(ScreenId id) override;

    int32_t SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) override;

    void SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback, float scaleX, float scaleY) override;

    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) override;

    void UnRegisterApplicationAgent(sptr<IApplicationAgent> app);

    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id) override;

    RSScreenModeInfo GetScreenActiveMode(ScreenId id) override;

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) override;

    RSScreenCapability GetScreenCapability(ScreenId id) override;

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) override;

    RSScreenData GetScreenData(ScreenId id) override;

    int32_t GetScreenBacklight(ScreenId id) override;

    void SetScreenBacklight(ScreenId id, uint32_t level) override;

    void RegisterBufferAvailableListener(
        NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread) override;

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) override;

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) override;

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) override;

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) override;

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) override;

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) override;

    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) override;

    int32_t GetScreenType(ScreenId id, RSScreenType& screenType) override;

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) override;

    int32_t RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback) override;

    int32_t UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback) override;

    void SetAppWindowNum(uint32_t num) override;

    pid_t remotePid_;
    wptr<RSRenderService> renderService_;
    RSMainThread* mainThread_ = nullptr;
    sptr<RSScreenManager> screenManager_;
    sptr<IRemoteObject> token_;

    class RSConnectionDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RSConnectionDeathRecipient(wptr<RSRenderServiceConnection> conn);
        virtual ~RSConnectionDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& token) override;

    private:
        wptr<RSRenderServiceConnection> conn_;
    };
    friend class RSConnectionDeathRecipient;
    sptr<RSConnectionDeathRecipient> connDeathRecipient_;

    class RSApplicationRenderThreadDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit RSApplicationRenderThreadDeathRecipient(wptr<RSRenderServiceConnection> conn);
        virtual ~RSApplicationRenderThreadDeathRecipient() = default;

        void OnRemoteDied(const wptr<IRemoteObject>& token) override;

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
    sptr<VSyncDistributor> appVSyncDistributor_;
    std::vector<sptr<VSyncConnection>> vsyncConnections_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_RENDER_SERVICE_CONNECTION_H
