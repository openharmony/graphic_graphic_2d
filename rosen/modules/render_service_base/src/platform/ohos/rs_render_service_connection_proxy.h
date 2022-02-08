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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H

#include <iremote_proxy.h>
#include <platform/ohos/rs_irender_service_connection.h>

namespace OHOS {
namespace Rosen {
class RSRenderServiceConnectionProxy : public IRemoteProxy<RSIRenderServiceConnection> {
public:
    explicit RSRenderServiceConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSRenderServiceConnectionProxy() noexcept = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;

    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) override;

    virtual sptr<IVSyncConnection> CreateVSyncConnection(const std::string& name) override;

    ScreenId GetDefaultScreenId() override;

    // mirrorId: decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int32_t flags = 0) override;

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) override;

    void RemoveVirtualScreen(ScreenId id) override;

    void SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) override;

    void SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    void RegisterApplicationRenderThread(uint32_t pid, sptr<IApplicationRenderThread> app) override;

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback) override;

    RSScreenModeInfo GetScreenActiveMode(ScreenId id) override;

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) override;

    RSScreenCapability GetScreenCapability(ScreenId id) override;

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id) override;

    RSScreenData GetScreenData(ScreenId id) override;

    int32_t GetScreenBacklight(ScreenId id) override;

    void SetScreenBacklight(ScreenId id, uint32_t level) override;

    void RegisterBufferAvailableListener(NodeId id, sptr<RSIBufferAvailableCallback> callback) override;

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) override;

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) override;

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) override;

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) override;

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) override;
    
    bool RequestRotation(ScreenId id, ScreenRotation rotation) override;

    ScreenRotation GetRotation(ScreenId id) override;

private:
    static inline BrokerDelegator<RSRenderServiceConnectionProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H
