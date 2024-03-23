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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H

#include "command/rs_node_showing_command.h"
#include <iremote_proxy.h>
#include <platform/ohos/rs_irender_service_connection.h>
#include <platform/ohos/rs_irender_service_connection_ipc_interface_code.h>
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
class RSRenderServiceConnectionProxy : public IRemoteProxy<RSIRenderServiceConnection> {
public:
    explicit RSRenderServiceConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSRenderServiceConnectionProxy() noexcept = default;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    MemoryGraphic GetMemoryGraphic(int pid) override;
    std::vector<MemoryGraphic> GetMemoryGraphics() override;
    bool GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize) override;

    bool GetUniRenderEnabled() override;

    bool CreateNode(const RSSurfaceRenderNodeConfig& config) override;
    sptr<Surface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config) override;

    virtual sptr<IVSyncConnection> CreateVSyncConnection(const std::string& name,
                                                         const sptr<VSyncIConnectionToken>& token,
                                                         uint64_t id = 0) override;

    int32_t SetFocusAppInfo(
        int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName,
        uint64_t focusNodeId) override;

    ScreenId GetDefaultScreenId() override;
    ScreenId GetActiveScreenId() override;

    std::vector<ScreenId> GetAllScreenIds() override;

    // mirrorId: decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int32_t flags = 0,
        std::vector<NodeId> filteredAppVector = {}) override;

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) override;

    void RemoveVirtualScreen(ScreenId id) override;

    int32_t SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) override;

    void SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate) override;

    void SetRefreshRateMode(int32_t refreshRateMode) override;

    void SyncFrameRateRange(const FrameRateRange& range) override;

    uint32_t GetScreenCurrentRefreshRate(ScreenId id) override;

    int32_t GetCurrentRefreshRateMode() override;

    std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id) override;

    bool GetShowRefreshRateEnabled() override;

    void SetShowRefreshRateEnabled(bool enable) override;

    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) override;

    void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback, float scaleX, float scaleY,
        SurfaceCaptureType surfaceCaptureType) override;

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

    void RegisterBufferClearListener(
        NodeId id, sptr<RSIBufferClearCallback> callback) override;

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) override;

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) override;

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) override;

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) override;

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) override;

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation) override;

    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation) override;

    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode) override;

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) override;

    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) override;

    int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat) override;

    int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat) override;

    int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats) override;

    int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat) override;

    int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx) override;

    int32_t GetScreenSupportedColorSpaces(ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces) override;

    int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace) override;

    int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace) override;

    int32_t GetScreenType(ScreenId id, RSScreenType& screenType) override;

    bool GetBitmap(NodeId id, Drawing::Bitmap& bitmap) override;
    bool GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
        const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList) override;

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval) override;

    int32_t RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback) override;

    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) override;

    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) override;

    int32_t RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback) override;

    int32_t RegisterHgmRefreshRateModeChangeCallback(sptr<RSIHgmConfigChangeCallback> callback) override;

    void SetAppWindowNum(uint32_t num) override;

    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes) override;

    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) override;

    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height) override;

    void ReportJankStats() override;

    void NotifyLightFactorStatus(bool isSafe) override;

    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList) override;

    void NotifyRefreshRateEvent(const EventInfo& eventInfo) override;

    void NotifyTouchEvent(int32_t touchStatus) override;

    void ReportEventResponse(DataBaseRs info) override;

    void ReportEventComplete(DataBaseRs info) override;

    void ReportEventJankFrame(DataBaseRs info) override;

    void ReportGameStateData(GameStateData info) override;

    void SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType) override;

    void SetCacheEnabledForRotation(bool isEnabled) override;

    void SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback) override;

    void RunOnRemoteDiedCallback() override;

    GpuDirtyRegionInfo GetCurrentDirtyRegionInfo(ScreenId id) override;

#ifdef TP_FEATURE_ENABLE
    void SetTpFeatureConfig(int32_t feature, const char* config) override;
#endif
    void SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus) override;
    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn) override;
private:
    bool FillParcelWithTransactionData(
        std::unique_ptr<RSTransactionData>& transactionData, std::shared_ptr<MessageParcel>& data);

    void ReportDataBaseRs(MessageParcel& data, MessageParcel& reply, MessageOption& option, DataBaseRs info);

    void ReportGameStateDataRs(MessageParcel& data, MessageParcel& reply, MessageOption& option, GameStateData info);

    static inline BrokerDelegator<RSRenderServiceConnectionProxy> delegator_;

    pid_t pid_ = GetRealPid();
    uint32_t transactionDataIndex_ = 0;
    OnRemoteDiedCallback OnRemoteDiedCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CONNECTION_PROXY_H
