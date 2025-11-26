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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_SERVICE_CONNECTION_PROXY_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_SERVICE_CONNECTION_PROXY_H

#include "command/rs_node_showing_command.h"
#include <iremote_proxy.h>
#include <memory>
#include <platform/ohos/rs_iclient_to_service_connection.h>
#include <platform/ohos/rs_irender_service_connection_ipc_interface_code.h>
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
class RSClientToServiceConnectionProxy : public IRemoteProxy<RSIClientToServiceConnection> {
public:
    explicit RSClientToServiceConnectionProxy(const sptr<IRemoteObject>& impl);
    virtual ~RSClientToServiceConnectionProxy() noexcept = default;

    ErrCode CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    ErrCode ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    ErrCode GetUniRenderEnabled(bool& enable) override;

    ErrCode CreateNode(const RSSurfaceRenderNodeConfig& configg, bool& success) override;
    ErrCode CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeIdg, bool& success) override;
    ErrCode CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, sptr<Surface>& sfc,
        bool unobscured = false) override;

    virtual ErrCode CreateVSyncConnection(sptr<IVSyncConnection>& vsyncConn,
                                          const std::string& name,
                                          const sptr<VSyncIConnectionToken>& token,
                                          VSyncConnParam vsyncConnParam = {0, 0, false}) override;

    ErrCode GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid,
        int32_t& repCode) override;
    
    ErrCode CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
        std::shared_ptr<Media::PixelMap> &pixelMap) override;

    ErrCode GetDefaultScreenId(uint64_t& screenId) override;
    ErrCode GetActiveScreenId(uint64_t& screenId) override;

    std::vector<ScreenId> GetAllScreenIds() override;

    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId associatedScreenId = 0,
        int32_t flags = 0,
        std::vector<NodeId> whiteList = {}) override;

    int32_t SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector) override;

    ErrCode SetVirtualScreenTypeBlackList(
        ScreenId id, std::vector<NodeType>& typeBlackListVector, int32_t& repCode) override;

    ErrCode AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector, int32_t& repCode) override;
    
    ErrCode RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector, int32_t& repCode) override;

    ErrCode SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success) override;

    uint32_t SetSurfaceWatermark(pid_t pid, const std::string &name,
        const std::shared_ptr<Media::PixelMap> &watermark,
        const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType) override;
        
    void ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
        const std::vector<NodeId>& nodeIdList) override;
        
    void ClearSurfaceWatermark(pid_t pid, const std::string &name) override;

    int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<NodeId>& securityExemptionList) override;

    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask) override;

    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false) override;

    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable) override;

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) override;
    void RemoveVirtualScreen(ScreenId id) override;

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t SetPointerColorInversionConfig(float darkBuffer, float brightBuffer,
        int64_t interval, int32_t rangeSize) override;
 
    int32_t SetPointerColorInversionEnabled(bool enable) override;
 
    int32_t RegisterPointerLuminanceChangeCallback(sptr<RSIPointerLuminanceChangeCallback> callback) override;
 
    int32_t UnRegisterPointerLuminanceChangeCallback() override;
#endif

    int32_t SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) override;

    int32_t SetScreenSwitchingNotifyCallback(sptr<RSIScreenSwitchingNotifyCallback> callback) override;

    int32_t SetBrightnessInfoChangeCallback(sptr<RSIBrightnessInfoChangeCallback> callback) override;

    int32_t GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo) override;

    bool ReadBrightnessInfo(BrightnessInfo& brightnessInfo, MessageParcel& data);

    uint32_t SetScreenActiveMode(ScreenId id, uint32_t modeId) override;

    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate) override;

    void SetRefreshRateMode(int32_t refreshRateMode) override;

    void SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range,
        int32_t animatorExpectedFrameRate) override;

    void UnregisterFrameRateLinker(FrameRateLinkerId id) override;

    uint32_t GetScreenCurrentRefreshRate(ScreenId id) override;

    int32_t GetCurrentRefreshRateMode() override;

    std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id) override;

    ErrCode GetShowRefreshRateEnabled(bool& enable) override;

    void SetShowRefreshRateEnabled(bool enabled, int32_t type) override;

    uint32_t GetRealtimeRefreshRate(ScreenId id) override;

    ErrCode GetRefreshInfo(pid_t pid, std::string& enable) override;
    ErrCode GetRefreshInfoToSP(NodeId id, std::string& enable) override;

    int32_t SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    int32_t GetRogScreenResolution(ScreenId id, uint32_t& width, uint32_t& height) override;

    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) override;

    ErrCode MarkPowerOffNeedProcessOneFrame() override;

    ErrCode RepaintEverything() override;

    ErrCode ForceRefreshOneFrameWithNextVSync() override;

    void DisablePowerOffRenderControl(ScreenId id) override;

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) override;

    ErrCode RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) override;
    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id) override;

    ErrCode GetScreenActiveMode(uint64_t id, RSScreenModeInfo& screenModeInfo) override;

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) override;

    RSScreenCapability GetScreenCapability(ScreenId id) override;

    ErrCode GetScreenPowerStatus(uint64_t screenId, uint32_t& status) override;

    RSScreenData GetScreenData(ScreenId id) override;

    ErrCode GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic) override;
    ErrCode GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize) override;
    ErrCode GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics) override;
    ErrCode GetScreenBacklight(uint64_t id, int32_t& level) override;

    void SetScreenBacklight(ScreenId id, uint32_t level) override;

    ErrCode RegisterBufferAvailableListener(
        NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread) override;

    ErrCode RegisterBufferClearListener(
        NodeId id, sptr<RSIBufferClearCallback> callback) override;

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) override;

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) override;

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) override;

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) override;

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) override;

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation) override;

    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation) override;

    int32_t SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation) override;

    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode) override;

    ErrCode SetGlobalDarkColorMode(bool isDark) override;

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) override;

    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) override;

    ErrCode GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat, int32_t& resCode) override;

    ErrCode SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat, int32_t& resCode) override;

    ErrCode GetScreenSupportedHDRFormats(
        ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats, int32_t& resCode) override;

    ErrCode GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat, int32_t& resCode) override;

    ErrCode SetScreenHDRFormat(ScreenId id, int32_t modeIdx, int32_t& resCode) override;

    ErrCode GetScreenSupportedColorSpaces(
        ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces, int32_t& resCode) override;

    ErrCode GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace, int32_t& resCode) override;

    ErrCode SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace, int32_t& resCode) override;

    int32_t GetScreenType(ScreenId id, RSScreenType& screenType) override;

    ErrCode GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success) override;
    ErrCode GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
        const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success) override;
    bool RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface) override;
    int32_t RegisterTypeface(uint64_t id, uint32_t size, int32_t fd, int32_t& needUpdate) override;
    bool UnRegisterTypeface(uint64_t globalUniqueId) override;

    ErrCode SetScreenSkipFrameInterval(uint64_t id, uint32_t skipFrameInterval, int32_t& resCode) override;

    ErrCode SetVirtualScreenRefreshRate(
        ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate, int32_t& retVal) override;

    ErrCode SetScreenActiveRect(ScreenId id, const Rect& activeRect, uint32_t& repCode) override;

    void SetScreenOffset(ScreenId id, int32_t offSetX, int32_t offSetY) override;

    ErrCode RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback, int32_t& repCode) override;

    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) override;

    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) override;

    int32_t RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback) override;

    int32_t RegisterHgmRefreshRateModeChangeCallback(sptr<RSIHgmConfigChangeCallback> callback) override;

    ErrCode SetAppWindowNum(uint32_t num) override;
    int32_t RegisterHgmRefreshRateUpdateCallback(sptr<RSIHgmConfigChangeCallback> callback) override;

    int32_t RegisterFirstFrameCommitCallback(sptr<RSIFirstFrameCommitCallback> callback) override;

    int32_t RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
        sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback) override;

    ErrCode SetSystemAnimatedScenes(
        SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success) override;

    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) override;

    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height) override;

    ErrCode ReportJankStats() override;

    ErrCode NotifyLightFactorStatus(int32_t lightFactorStatus) override;

    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList) override;

    void NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
        const std::vector<std::pair<std::string, std::string>>& newConfig) override;

    void NotifyRefreshRateEvent(const EventInfo& eventInfo) override;

    void SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos) override;

    void SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos) override;

    ErrCode NotifySoftVsyncEvent(uint32_t pid, uint32_t rateDiscount) override;

    bool NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name, uint32_t rateDiscount) override;

    ErrCode NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt) override;

    void NotifyDynamicModeEvent(bool enableDynamicMode) override;

    ErrCode NotifyHgmConfigEvent(const std::string &eventName, bool state) override;

    ErrCode NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate) override;

    ErrCode ReportEventResponse(DataBaseRs info) override;

    ErrCode ReportEventComplete(DataBaseRs info) override;

    ErrCode ReportEventJankFrame(DataBaseRs info) override;

    ErrCode ReportGameStateData(GameStateData info) override;
    void ReportRsSceneJankStart(AppInfo info) override;

    void ReportRsSceneJankEnd(AppInfo info) override;

    ErrCode SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
        bool dynamicHardwareEnable) override;

    ErrCode SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode) override;

    ErrCode SetCacheEnabledForRotation(bool isEnabled) override;

    void SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback) override;

    void RunOnRemoteDiedCallback() override;

    void SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus) override;
    ErrCode SetCurtainScreenUsingStatus(bool isCurtainScreenOn) override;
    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo() override;

    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo() override;

    LayerComposeInfo GetLayerComposeInfo() override;

    HwcDisabledReasonInfos GetHwcDisabledReasonInfo() override;

    ErrCode GetHdrOnDuration(int64_t& hdrOnDuration) override;

    ErrCode SetVmaCacheStatus(bool flag) override;

    int32_t RegisterUIExtensionCallback(uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false) override;

    ErrCode SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus, bool& success) override;

    void SetFreeMultiWindowStatus(bool enable) override;

    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) override;
#ifdef TP_FEATURE_ENABLE
    ErrCode SetTpFeatureConfig(int32_t feature, const char* config,
        TpFeatureConfigType tpFeatureConfigType = TpFeatureConfigType::DEFAULT_TP_FEATURE) override;
#endif

    ErrCode NotifyScreenSwitched() override;

    int32_t RegisterSelfDrawingNodeRectChangeCallback(
        const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback) override;

    int32_t UnRegisterSelfDrawingNodeRectChangeCallback() override;

    ErrCode NotifyPageName(const std::string &packageName, const std::string &pageName, bool isEnter) override;

    ErrCode AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime) override;

    ErrCode AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps) override;

    bool GetHighContrastTextState() override;

    ErrCode SetBehindWindowFilterEnabled(bool enabled) override;

    ErrCode GetBehindWindowFilterEnabled(bool& enabled) override;

    int32_t GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB) override;

    RetCodeHrpService ProfilerServiceOpenFile(const HrpServiceDirInfo& dirInfo,
        const std::string& fileName, int32_t flags, int& outFd) override;
    RetCodeHrpService ProfilerServicePopulateFiles(const HrpServiceDirInfo& dirInfo,
        uint32_t firstFileIndex, std::vector<HrpServiceFileInfo>& outFiles) override;
    bool ProfilerIsSecureScreen() override;

    ErrCode SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t> pidList) override;

    ErrCode SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList) override;

    bool WriteSurfaceCaptureConfig(const RSSurfaceCaptureConfig& captureConfig, MessageParcel& data);

    bool WriteSurfaceCaptureBlurParam(const RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data);

    bool WriteSurfaceCaptureAreaRect(const Drawing::Rect& specifiedAreaRect, MessageParcel& data);
private:
    bool FillParcelWithTransactionData(
        std::unique_ptr<RSTransactionData>& transactionData, std::shared_ptr<MessageParcel>& data);

    void ReportDataBaseRs(MessageParcel& data, MessageParcel& reply, MessageOption& option, DataBaseRs info);

    void WriteAppInfo(MessageParcel& data, MessageParcel& reply, MessageOption& option, AppInfo info);

    void ReportGameStateDataRs(MessageParcel& data, MessageParcel& reply, MessageOption& option, GameStateData info);

    int32_t SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);

    ErrCode SetLayerTop(const std::string &nodeIdStr, bool isTop) override;

    ErrCode SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh) override;

    void SetColorFollow(const std::string &nodeIdStr, bool isColorFollow) override;

    void RemoveToken() override {};

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    ErrCode SetOverlayDisplayMode(int32_t mode) override;
#endif

    static inline BrokerDelegator<RSClientToServiceConnectionProxy> delegator_;

    pid_t pid_ = GetRealPid();
    std::atomic<uint32_t> transactionDataIndex_ = 0;
    OnRemoteDiedCallback OnRemoteDiedCallback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_CLIENT_TO_SERVICE_CONNECTION_PROXY_H
