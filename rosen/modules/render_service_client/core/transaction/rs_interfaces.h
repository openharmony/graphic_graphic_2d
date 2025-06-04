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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H

#include <memory>
#include <mutex>

#include "memory/rs_memory_graphic.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSInterfaces {
public:
    static RSInterfaces &GetInstance();
    RSInterfaces(const RSInterfaces &) = delete;
    void operator=(const RSInterfaces &) = delete;

    int32_t SetFocusAppInfo(const FocusAppInfo& info);

    ScreenId GetDefaultScreenId();

    // for bootAnimation only
    ScreenId GetActiveScreenId();

    std::vector<ScreenId> GetAllScreenIds();

#ifndef ROSEN_CROSS_PLATFORM
    /**
     * @brief Create virtual screen with params.
     * @param name Virtual screen name.
     * @param width Virtual screen width, max: MAX_VIRTUAL_SCREEN_WIDTH.
     * @param height Virtual screen height, max: MAX_VIRTUAL_SCREEN_HEIGHT.
     * @param surface Virtual screen surface, if not nullptr, vote for 60Hz.
     * @param mirrorId Decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
     * @param flags Reserved for future use.
     * @param whiteList List of surface node id, only these nodes can be drawn on this screen.
     * @return Virtual screen id, INVALID_SCREEN_ID means failed.
     */
    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int flags = 0,
        std::vector<NodeId> whiteList = {});

    /**
     * @brief Set list of surface node id, these nodes will be excluded from this screen.
     * @param id Valid screen id: set screen record black list; INVALID_SCREEN_ID: set screen cast black list.
     * @param blackListVector List of surface node id. If the screen id is INVALID_SCREEN_ID, the blackListVector will
     * apply to all virtual screens.
     * @return 0 means success.
     */
    int32_t SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);

    // use surfaceNodeType to set black list
    int32_t SetVirtualScreenTypeBlackList(ScreenId id, std::vector<NodeType>& typeBlackListVector);

    int32_t AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);

    int32_t RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);

    int32_t SetVirtualScreenSecurityExemptionList(ScreenId id, const std::vector<NodeId>& securityExemptionList);

    /**
     * @brief Set mask image displayed on virtual mirror screen when security layer is present.
     * @param id Virtual screen id.
     * @param securityMask Mask image to be set, nullptr means no mask.
     * @return 0 means success.
     */
    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask);

    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false);

    /**
     * @brief Set if cast screen black list is enabled, surface node in black list will not be drawn.
     * @param id Virtual screen id.
     * @param enable True means enable, false means disable.
     * @return 0 means success.
     */
    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable);

    /**
     * @brief Set producer surface for virtual screen.
     * @param id Virtual screen id.
     * @param surface Producer surface.
     * @return 0 means success.
     */
    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);
#endif

    /**
     * @brief Remove virtual screen.
     * @param id Virtual screen id.
     */
    void RemoveVirtualScreen(ScreenId id);

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t SetPointerColorInversionConfig(float darkBuffer, float brightBuffer, int64_t interval, int32_t rangeSize);

    int32_t SetPointerColorInversionEnabled(bool enable);

    int32_t RegisterPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback);

    int32_t UnRegisterPointerLuminanceChangeCallback();
#endif

    /**
     * @brief Set screen connection status change callback.
     * on the screen connection status is changed.
     * @param callback Callback of the screen connection status changed.
     * @return Returns int32_t, return value == 0 success, otherwise, failed.
     */
    int32_t SetScreenChangeCallback(const ScreenChangeCallback &callback);

    // if return true, the setting is successful. otherwise failed. The function is setted watermark for SurfaceNode
    bool SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark);

    int32_t GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid);

    bool TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    bool TakeSurfaceCaptureWithBlur(std::shared_ptr<RSSurfaceNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig = {},
        float blurRadius = 1E-6);

    /**
     * @brief Take display node capture.
     * @param node Display node.
     * @param callback Callback to be called when capture is done.
     * @param captureConfig Capture config, see RSSurfaceCaptureConfig.
     * @return True if capture task is successfully created, false if failed.
     */
    bool TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    bool TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    bool TakeSurfaceCaptureForUI(std::shared_ptr<RSNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.f, float scaleY = 1.f,
        bool isSync = false, const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f));

    bool TakeSurfaceCaptureForUIWithConfig(std::shared_ptr<RSNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig = {},
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f));

    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
        TakeSurfaceCaptureSoloNodeList(std::shared_ptr<RSNode> node);
        
    bool TakeSelfSurfaceCapture(std::shared_ptr<RSSurfaceNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    bool SetWindowFreezeImmediately(std::shared_ptr<RSSurfaceNode> node, bool isFreeze,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig = {},
        float blurRadius = 1E-6);

    bool TakeUICaptureInRange(std::shared_ptr<RSNode> beginNode, std::shared_ptr<RSNode> endNode, bool useBeginNodeSize,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY, bool isSync);

    bool SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY, float positionZ, float positionW);

    bool RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);
    bool UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);
#ifndef ROSEN_ARKUI_X
    void SetScreenActiveMode(ScreenId id, uint32_t modeId);

    MemoryGraphic GetMemoryGraphic(int pid);

    std::vector<MemoryGraphic> GetMemoryGraphics();
#endif // !ROSEN_ARKUI_X
    bool GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize);

#ifndef ROSEN_ARKUI_X
    /**
     * @brief Set render resolution of physical screen.
     * @param id Id of the physical screen.
     * @param width Width to set, and should be greater than physical width.
     * @param height Height to set, and should be greater than physical height.
     * @return StatusCode. Returns value, return value == 0 success, otherwise, failed.
     */
    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height);

    /**
     * @brief Set virtual screen resolution.
     * @param id Virtual screen id.
     * @param width Virtual screen width, max: MAX_VIRTUAL_SCREEN_WIDTH.
     * @param height Virtual screen height, max: MAX_VIRTUAL_SCREEN_HEIGHT.
     * @return 0 means success.
     */
    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height);
#endif // !ROSEN_ARKUI_X

    /**
     * @brief Set if auto rotation is enabled for virtual mirror screen, keep content always horizontal.
     * @param id Virtual screen id.
     * @param canvasRotation True means enable, false means disable.
     * @return True if success, false if failed.
     */
    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation);

    /**
     * @brief Set scale mode for virtual mirror screen.
     * @param id Virtual screen id.
     * @param scaleMode Scale mode, see ScreenScaleMode.
     * @return True if success, false if failed.
     */
    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode);

    // WMS set dark color display mode to RS
    bool SetGlobalDarkColorMode(bool isDark);
#ifndef ROSEN_ARKUI_X
    /**
     * @brief Get virtual screen resolution.
     * @param id Virtual screen id.
     * @return Virtual screen resolution, see RSVirtualScreenResolution.
     */
    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id);

    void MarkPowerOffNeedProcessOneFrame();

    void RepaintEverything();

    void DisablePowerOffRenderControl(ScreenId id);

    /**
     * @brief Set power of the screen.
     * @param id Id of the screen to set power status.
     * @param status The status to set to the screen.
     */
    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    RSScreenModeInfo GetScreenActiveMode(ScreenId id);
#endif // !ROSEN_ARKUI_X

    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);

    void SetRefreshRateMode(int32_t refreshRateMode);

    void SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range, int32_t animatorExpectedFrameRate);

    uint32_t GetScreenCurrentRefreshRate(ScreenId id);

    int32_t GetCurrentRefreshRateMode();

    std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id);

    bool GetShowRefreshRateEnabled();

    void SetShowRefreshRateEnabled(bool enabled, int32_t type = 1);

    uint32_t GetRealtimeRefreshRate(ScreenId id);

    std::string GetRefreshInfo(pid_t pid);
    std::string GetRefreshInfoToSP(NodeId id);

#ifndef ROSEN_ARKUI_X
    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id);

    RSScreenCapability GetScreenCapability(ScreenId id);

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id);

    RSScreenData GetScreenData(ScreenId id);
#endif // !ROSEN_ARKUI_X
    int32_t GetScreenBacklight(ScreenId id);

    /**
     * @brief Set backlight value of the screen.
     * 
     * @param id Id of the screen to set its backlight value.
     * @param level The value of backlight.
     */
    void SetScreenBacklight(ScreenId id, uint32_t level);

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode);

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys);

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode);

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode);

    /**
     * @brief Set screen correction, used to correct screen rotation.
     * @param id Screen id.
     * @param screenRotation Screen correction, see ScreenRotation.
     * @return 0 means success.
     */
    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation);

    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode);

    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability);

    int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat);

    int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat);

    int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats);

    int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat);

    int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx);

    int32_t GetScreenSupportedColorSpaces(ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces);

    int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace);

    int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace);

    int32_t GetScreenType(ScreenId id, RSScreenType& screenType);

    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData);

    /* skipFrameInterval : decide how many frames apart to refresh a frame,
       DEFAULT_SKIP_FRAME_INTERVAL means refresh each frame,
       change screen refresh rate finally */
    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval);

    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate);

    /**
     * @brief Set screen active rect, part of screen that can be drawn.
     * @param id Screen id.
     * @param activeRect Screen active rect.
     * @return 0 means success.
     */
    uint32_t SetScreenActiveRect(ScreenId id, const Rect& activeRect);

    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr);

    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        uint64_t id,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr,
        NodeId windowNodeId = 0,
        bool fromXcomponent = false);

    std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurfaceId(uint64_t surfaceId, const Rect &srcRect);

    int32_t RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback);

    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints);

    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id);

    int32_t RegisterHgmConfigChangeCallback(const HgmConfigChangeCallback& callback);

    int32_t RegisterHgmRefreshRateModeChangeCallback(const HgmRefreshRateModeChangeCallback& callback);

    int32_t RegisterHgmRefreshRateUpdateCallback(const HgmRefreshRateUpdateCallback& callback);

    int32_t UnRegisterHgmRefreshRateUpdateCallback();

    int32_t RegisterFirstFrameCommitCallback(const FirstFrameCommitCallback& callback);

    int32_t UnRegisterFirstFrameCommitCallback();

    int32_t RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
        const FrameRateLinkerExpectedFpsUpdateCallback& callback);

    int32_t UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid);

    void SetAppWindowNum(uint32_t num);

    /*
    * @brief Set the system overload Animated Scenes to RS for special load shedding
    * @param systemAnimatedScenes indicates the system animation scene
    * @param isRegularAnimation indicates irregular windows in the animation scene
    * @return true if succeed, otherwise false
    */
    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation = false);

    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);

    /**
     * @brief Resize virtual screen.
     * @param id Virtual screen id.
     * @param width Virtual screen width, max: MAX_VIRTUAL_SCREEN_WIDTH.
     * @param height Virtual screen height, max: MAX_VIRTUAL_SCREEN_HEIGHT.
     * @return 0 means success.
     */
    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height);

    void ReportJankStats();

    void NotifyLightFactorStatus(int32_t lightFactorStatus);

    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);

    void NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
        const std::vector<std::pair<std::string, std::string>>& newConfig);

    void NotifyRefreshRateEvent(const EventInfo& eventInfo);

    /*
    * @brief Support setting softVsync frame rate through windowId
    * @param eventInfos a map,key is windowId, value is eventInfo
    */
    void SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos);

    /*
    * @brief Support setting softVsync frame rate through vsyncName
    * @param eventInfos a map,key is vsyncName, value is eventInfo
    */
    void SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos);

    bool NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name, uint32_t rateDiscount);

    void NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt);

    void NotifyDynamicModeEvent(bool enableDynamicMode);

    void NotifyHgmConfigEvent(const std::string &eventName, bool state);

    void NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate);

    void ReportEventResponse(DataBaseRs info);

    void ReportEventComplete(DataBaseRs info);

    void ReportEventJankFrame(DataBaseRs info);

    void ReportGameStateData(GameStateData info);

    void ReportRsSceneJankStart(AppInfo info);

    void ReportRsSceneJankEnd(AppInfo info);

    void EnableCacheForRotation();

    void DisableCacheForRotation();

    void SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback);

    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn);

    void DropFrameByPid(const std::vector<int32_t> pidList);

    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo() const;

    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo() const;

    LayerComposeInfo GetLayerComposeInfo() const;

    HwcDisabledReasonInfos GetHwcDisabledReasonInfo() const;

    int64_t GetHdrOnDuration() const;

    void SetVmaCacheStatus(bool flag);

#ifdef TP_FEATURE_ENABLE
    void SetTpFeatureConfig(int32_t feature, const char* config,
        TpFeatureConfigType tpFeatureConfigType = TpFeatureConfigType::DEFAULT_TP_FEATURE);
#endif

    /**
     * @brief Set virtual screen using status, vote for 60Hz if being used.
     * @param isVirtualScreenUsingStatus True means using virtual screen, false means not using.
     */
    void SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus);

    int32_t RegisterUIExtensionCallback(uint64_t userId, const UIExtensionCallback& callback, bool unobscured = false);

    /**
     * @brief Set virtual screen status.
     * @param id Virtual screen id.
     * @param screenStatus Virtual screen status, see VirtualScreenStatus.
     * @return True if success, false if failed.
     */
    bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus);

    bool SetAncoForceDoDirect(bool direct);

    void SetFreeMultiWindowStatus(bool enable);

    bool RegisterTransactionDataCallback(int32_t pid, uint64_t timeStamp, std::function<void()> callback);

    bool RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        std::shared_ptr<SurfaceBufferCallback> callback);

    bool UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid);

    // Make this node(nodeIdStr) should do DSS composition and set the layer to top. otherwise do GPU composition.
    void SetLayerTop(const std::string &nodeIdStr, bool isTop);

    void SetColorFollow(const std::string &nodeIdStr, bool isColorFollow);

    void NotifyScreenSwitched();

    void ForceRefreshOneFrameWithNextVSync();

    void SetWindowContainer(NodeId nodeId, bool value);

    int32_t RegisterSelfDrawingNodeRectChangeCallback(const SelfDrawingNodeRectChangeCallback& callback);

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    int32_t SetOverlayDisplayMode(int32_t mode);
#endif

    void NotifyPageName(const std::string &packageName, const std::string &pageName, bool isEnter);

    bool GetHighContrastTextState();

    bool SetBehindWindowFilterEnabled(bool enabled);

    bool GetBehindWindowFilterEnabled(bool& enabled);
private:
    RSInterfaces();
    ~RSInterfaces() noexcept;

    bool TakeSurfaceCaptureForUIWithoutUni(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        float scaleX, float scaleY);

    std::unique_ptr<RSRenderServiceClient> renderServiceClient_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H
