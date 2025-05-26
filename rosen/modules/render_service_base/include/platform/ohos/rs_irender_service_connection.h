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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_H

#include <iremote_broker.h>
#include <string>
#include <surface.h>

#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "feature/capture/rs_ui_capture.h"
#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/buffer_clear_callback.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_callbacks/rs_iself_drawing_node_rect_change_callback.h"
#include "ipc_callbacks/rs_isurface_occlusion_change_callback.h"
#include "ipc_callbacks/rs_surface_buffer_callback.h"
#include "ipc_callbacks/rs_iframe_rate_linker_expected_fps_update_callback.h"
#include "ipc_callbacks/screen_change_callback.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "ipc_callbacks/rs_transaction_data_callback.h"
#include "memory/rs_memory_graphic.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"
#include "screen_manager/rs_screen_hdr_capability.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_virtual_screen_resolution.h"
#include "transaction/rs_transaction_data.h"
#include "transaction/rs_render_service_client.h"
#include "ivsync_connection.h"
#include "ipc_callbacks/rs_ihgm_config_change_callback.h"
#include "ipc_callbacks/rs_ifirst_frame_commit_callback.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "ipc_callbacks/rs_iuiextension_callback.h"
#include "vsync_iconnection_token.h"

namespace OHOS {
namespace Rosen {

class RSIRenderServiceConnection : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");

    RSIRenderServiceConnection() = default;
    virtual ~RSIRenderServiceConnection() noexcept = default;

    virtual ErrCode CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) = 0;
    virtual ErrCode ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) = 0;

    virtual ErrCode GetUniRenderEnabled(bool& enable) = 0;

    virtual ErrCode CreateNode(const RSSurfaceRenderNodeConfig& config, bool& success) = 0;
    virtual ErrCode CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId, bool& success) = 0;
    virtual ErrCode CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, sptr<Surface>& sfc,
        bool unobscured = false) = 0;

    virtual ErrCode CreateVSyncConnection(sptr<IVSyncConnection>& vsyncConn,
                                          const std::string& name,
                                          const sptr<VSyncIConnectionToken>& token = nullptr,
                                          VSyncConnParam vsyncConnParam = {0, 0, false}) = 0;

    virtual ErrCode GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid,
        int32_t& repCode) = 0;

    virtual ErrCode CreatePixelMapFromSurface(sptr<Surface> surface,
        const Rect &srcRect, std::shared_ptr<Media::PixelMap> &pixelMap) = 0;

    virtual ErrCode SetFocusAppInfo(const FocusAppInfo& info, int32_t& repCode) = 0;

    virtual ErrCode GetDefaultScreenId(uint64_t& screenId) = 0;

    virtual ErrCode GetActiveScreenId(uint64_t& screenId) = 0;

    virtual std::vector<ScreenId> GetAllScreenIds() = 0;

    // mirrorId: decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
    virtual ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int32_t flags = 0,
        std::vector<NodeId> whiteList = {}) = 0;

    virtual int32_t SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector) = 0;

    virtual ErrCode SetVirtualScreenTypeBlackList(
        ScreenId id, std::vector<NodeType>& typeBlackListVector, int32_t& repCode) = 0;

    virtual ErrCode AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector, int32_t& repCode) = 0;
    
    virtual ErrCode RemoveVirtualScreenBlackList(
        ScreenId id, std::vector<NodeId>& blackListVector, int32_t& repCode) = 0;

    virtual ErrCode SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark,
        bool& success) = 0;

    virtual int32_t SetVirtualScreenSecurityExemptionList(
        ScreenId id, const std::vector<NodeId>& securityExemptionList) = 0;

    virtual int32_t SetScreenSecurityMask(ScreenId id,
        std::shared_ptr<Media::PixelMap> securityMask) = 0;

    virtual int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
        bool supportRotation = false) = 0;

    virtual int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable) = 0;

    virtual int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface) = 0;

    virtual void RemoveVirtualScreen(ScreenId id) = 0;

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    virtual int32_t SetPointerColorInversionConfig(float darkBuffer, float brightBuffer,
        int64_t interval, int32_t rangeSize) = 0;
 
    virtual int32_t SetPointerColorInversionEnabled(bool enable) = 0;
 
    virtual int32_t RegisterPointerLuminanceChangeCallback(sptr<RSIPointerLuminanceChangeCallback> callback) = 0;
 
    virtual int32_t UnRegisterPointerLuminanceChangeCallback() = 0;
#endif

    virtual int32_t SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback) = 0;

    virtual void SetScreenActiveMode(ScreenId id, uint32_t modeId) = 0;

    virtual void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate) = 0;

    virtual void SetRefreshRateMode(int32_t refreshRateMode) = 0;

    virtual void SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range,
        int32_t animatorExpectedFrameRate) = 0;

    virtual void UnregisterFrameRateLinker(FrameRateLinkerId id) = 0;

    virtual uint32_t GetScreenCurrentRefreshRate(ScreenId id) = 0;

    virtual int32_t GetCurrentRefreshRateMode() = 0;

    virtual std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id) = 0;

    virtual ErrCode GetShowRefreshRateEnabled(bool& enable) = 0;

    virtual void SetShowRefreshRateEnabled(bool enabled, int32_t type) = 0;

    virtual uint32_t GetRealtimeRefreshRate(ScreenId screenId) = 0;

    virtual ErrCode GetRefreshInfo(pid_t pid, std::string& enable) = 0;
    virtual ErrCode GetRefreshInfoToSP(NodeId id, std::string& enable) = 0;

    virtual int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual ErrCode MarkPowerOffNeedProcessOneFrame() = 0;

    virtual ErrCode RepaintEverything() = 0;

    virtual ErrCode ForceRefreshOneFrameWithNextVSync() = 0;

    virtual void DisablePowerOffRenderControl(ScreenId id) = 0;

    virtual void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status) = 0;

    virtual void TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam = {},
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f),
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) = 0;

    virtual std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> TakeSurfaceCaptureSoloNode(
        NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        RSSurfaceCapturePermissions permissions = RSSurfaceCapturePermissions()) = 0;

    virtual void TakeSelfSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig) = 0;

    virtual ErrCode SetWindowFreezeImmediately(NodeId id, bool isFreeze, sptr<RSISurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam = {}) = 0;

    virtual void TakeUICaptureInRange(
        NodeId id, sptr<RSISurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig) = 0;

    virtual ErrCode SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW) = 0;

    virtual ErrCode RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app) = 0;

    virtual RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id) = 0;

    virtual ErrCode GetScreenActiveMode(uint64_t id, RSScreenModeInfo& screenModeInfo) = 0;

    virtual std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id) = 0;

    virtual RSScreenCapability GetScreenCapability(ScreenId id) = 0;

    virtual ErrCode GetScreenPowerStatus(uint64_t screenId, uint32_t& status) = 0;

    virtual RSScreenData GetScreenData(ScreenId id) = 0;

    virtual ErrCode GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic) = 0;

    virtual ErrCode GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize) = 0;

    virtual ErrCode GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics) = 0;

    virtual ErrCode GetScreenBacklight(uint64_t id, int32_t& level) = 0;

    virtual void SetScreenBacklight(ScreenId id, uint32_t level) = 0;

    virtual ErrCode RegisterBufferAvailableListener(
        NodeId id, sptr<RSIBufferAvailableCallback> callback, bool isFromRenderThread) = 0;

    virtual ErrCode RegisterBufferClearListener(
        NodeId id, sptr<RSIBufferClearCallback> callback) = 0;

    virtual int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode) = 0;

    virtual int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys) = 0;

    virtual int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode) = 0;

    virtual int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx) = 0;

    virtual int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode) = 0;

    virtual int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation) = 0;

    virtual bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation) = 0;

    virtual bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode) = 0;

    virtual ErrCode SetGlobalDarkColorMode(bool isDark) = 0;

    virtual int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode) = 0;

    virtual int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability) = 0;

    virtual ErrCode GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat, int32_t& resCode) = 0;

    virtual ErrCode SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat, int32_t& resCode) = 0;

    virtual ErrCode GetScreenSupportedHDRFormats(
        ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats, int32_t& resCode) = 0;

    virtual ErrCode GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat, int32_t& resCode) = 0;

    virtual ErrCode SetScreenHDRFormat(ScreenId id, int32_t modeIdx, int32_t& resCode) = 0;

    virtual ErrCode GetScreenSupportedColorSpaces(
        ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces, int32_t& resCode) = 0;

    virtual ErrCode GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace, int32_t& resCode) = 0;

    virtual ErrCode SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace, int32_t& resCode) = 0;

    virtual int32_t GetScreenType(ScreenId id, RSScreenType& screenType) = 0;

    virtual ErrCode GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success) = 0;
    virtual ErrCode GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
        const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList, bool& success) = 0;
    virtual bool RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface) = 0;
    virtual bool UnRegisterTypeface(uint64_t globalUniqueId) = 0;

    virtual ErrCode SetScreenSkipFrameInterval(uint64_t id, uint32_t skipFrameInterval, int32_t& resCode) = 0;

    virtual ErrCode SetVirtualScreenRefreshRate(
        ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate, int32_t& retVal) = 0;

    virtual ErrCode SetScreenActiveRect(ScreenId id, const Rect& activeRect, uint32_t& repCode) = 0;

    virtual ErrCode RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback, int32_t& repCode) = 0;

    virtual int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints) = 0;

    virtual int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id) = 0;

    virtual int32_t RegisterHgmConfigChangeCallback(sptr<RSIHgmConfigChangeCallback> callback) = 0;

    virtual int32_t RegisterHgmRefreshRateModeChangeCallback(sptr<RSIHgmConfigChangeCallback> callback) = 0;

    virtual ErrCode SetAppWindowNum(uint32_t num) = 0;

    virtual int32_t RegisterHgmRefreshRateUpdateCallback(sptr<RSIHgmConfigChangeCallback> callback) = 0;

    virtual int32_t RegisterFirstFrameCommitCallback(sptr<RSIFirstFrameCommitCallback> callback) = 0;

    virtual int32_t RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t pid,
        sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> callback) = 0;

    virtual ErrCode SetSystemAnimatedScenes(
        SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation, bool& success) = 0;

    virtual void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow) = 0;

    virtual int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height) = 0;

    virtual ErrCode ReportJankStats() = 0;

    virtual ErrCode NotifyLightFactorStatus(int32_t lightFactorStatus) = 0;

    virtual void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList) = 0;

    virtual void NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
        const std::vector<std::pair<std::string, std::string>>& newConfig) = 0;

    virtual void NotifyRefreshRateEvent(const EventInfo& eventInfo) = 0;

    virtual void SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos) = 0;

    virtual void SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos) = 0;

    virtual ErrCode NotifySoftVsyncEvent(uint32_t pid, uint32_t rateDiscount) = 0;

    virtual bool NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name, uint32_t rateDiscount) = 0;

    virtual ErrCode NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt) = 0;

    virtual void NotifyDynamicModeEvent(bool enableDynamicMode) = 0;

    virtual ErrCode NotifyHgmConfigEvent(const std::string &eventName, bool state) = 0;

    virtual ErrCode NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate) = 0;

    virtual ErrCode ReportEventResponse(DataBaseRs info) = 0;

    virtual ErrCode ReportEventComplete(DataBaseRs info) = 0;

    virtual ErrCode ReportEventJankFrame(DataBaseRs info) = 0;

    virtual ErrCode ReportGameStateData(GameStateData info) = 0;

    virtual void ReportRsSceneJankStart(AppInfo info) = 0;

    virtual void ReportRsSceneJankEnd(AppInfo info) = 0;

    virtual ErrCode SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
        bool dynamicHardwareEnable) = 0;

    virtual ErrCode SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent, uint32_t& resCode) = 0;

    virtual ErrCode SetCacheEnabledForRotation(bool isEnabled) = 0;

    virtual void SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback) = 0;

    virtual void RunOnRemoteDiedCallback() = 0;

    virtual void SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus) = 0;

    virtual ErrCode SetCurtainScreenUsingStatus(bool isCurtainScreenOn) = 0;

    virtual ErrCode DropFrameByPid(const std::vector<int32_t> pidList) = 0;

    virtual std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo() = 0;

    virtual GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo() = 0;

    virtual LayerComposeInfo GetLayerComposeInfo() = 0;

    virtual HwcDisabledReasonInfos GetHwcDisabledReasonInfo() = 0;

    virtual ErrCode GetHdrOnDuration(int64_t& hdrOnDuration) = 0;

    virtual ErrCode SetVmaCacheStatus(bool flag) = 0;

    virtual int32_t RegisterUIExtensionCallback(uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false) = 0;

    virtual ErrCode SetAncoForceDoDirect(bool direct, bool& res) = 0;

    virtual ErrCode SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus, bool& success) = 0;

    virtual void SetFreeMultiWindowStatus(bool enable) = 0;

    virtual int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData) = 0;

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    virtual ErrCode SetOverlayDisplayMode(int32_t mode) = 0;
#endif

    virtual ErrCode SetLayerTop(const std::string &nodeIdStr, bool isTop) = 0;

    virtual void SetColorFollow(const std::string &nodeIdStr, bool isColorFollow) = 0;
#ifdef TP_FEATURE_ENABLE
    virtual ErrCode SetTpFeatureConfig(
        int32_t feature, const char* config, TpFeatureConfigType tpFeatureConfigType) = 0;
#endif

    virtual ErrCode RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        sptr<RSISurfaceBufferCallback> callback) = 0;

    virtual ErrCode UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid) = 0;

    virtual void RegisterTransactionDataCallback(int32_t pid, uint64_t timeStamp, sptr<RSITransactionDataCallback> callback) = 0;

    virtual ErrCode NotifyScreenSwitched() = 0;

    virtual ErrCode SetWindowContainer(NodeId nodeId, bool value) = 0;

    virtual int32_t RegisterSelfDrawingNodeRectChangeCallback(sptr<RSISelfDrawingNodeRectChangeCallback> callback) = 0;

    virtual ErrCode NotifyPageName(const std::string &packageName, const std::string &pageName, bool isEnter) = 0;

    virtual bool GetHighContrastTextState() = 0;

    virtual ErrCode SetBehindWindowFilterEnabled(bool enabled) = 0;

    virtual ErrCode GetBehindWindowFilterEnabled(bool& enabled) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IRENDER_SERVICE_CONNECTION_H
