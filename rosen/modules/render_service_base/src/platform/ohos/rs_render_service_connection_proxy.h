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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H

#include <iremote_proxy.h>
#include <platform/ohos/rs_irender_service_connection.h>
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceConnectionProxy : public IRemoteProxy<RSIRenderServiceConnection> {
public:
    explicit RSRenderServiceConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSRenderServiceConnectionProxy() noexcept = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    bool FillParcelWithTransactionData(
        std::unique_ptr<RSTransactionData>& transactionData, std::shared_ptr<MessageParcel>& data);

    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    int32_t SetRenderModeChangeCallback(sptr<RSIRenderModeChangeCallback> callback) override;
    void UpdateRenderMode(bool isUniRender) override;
    bool GetUniRenderEnabled() override;
    bool QueryIfRTNeedRender() override;
    bool CreateNode(const RSSurfaceRenderNodeConfig& config) override;
    sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) override;

    virtual sptr<IVSyncConnection> CreateVSyncConnection(const std::string& name) override;

    int32_t SetFocusAppInfo(
        int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName) override;

    ScreenId GetDefaultScreenId() override;

    std::vector<ScreenId> GetAllScreenIds() override;

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

    int32_t SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) override;

    void SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) override;

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback, float scaleX, float scaleY) override;

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
private:
    static inline BrokerDelegator<RSRenderServiceConnectionProxy> delegator_;

    pid_t pid_ = GetRealPid();
    uint32_t transactionDataIndex_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H
