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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <refbase.h>
#include <surface_type.h>
#ifndef ROSEN_CROSS_PLATFORM
#include <surface.h>
#include <utility>
#endif

#include "ipc_callbacks/buffer_available_callback.h"
#include "ipc_callbacks/iapplication_agent.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_change_callback.h"
#endif
#include "ipc_callbacks/rs_surface_buffer_callback.h"
#include "ipc_callbacks/screen_change_callback.h"
#include "ipc_callbacks/surface_capture_callback.h"
#include "ipc_callbacks/rs_transaction_data_callback.h"
#include "memory/rs_memory_graphic.h"
#include "platform/drawing/rs_surface.h"
#include "rs_irender_client.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "screen_manager/rs_screen_capability.h"
#include "screen_manager/rs_screen_data.h"
#include "screen_manager/rs_screen_hdr_capability.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_virtual_screen_resolution.h"
#include "vsync_receiver.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "rs_hgm_config_data.h"
#include "rs_occlusion_data.h"
#include "rs_self_drawing_node_rect_data.h"
#include "rs_uiextension_data.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "info_collection/rs_hardware_compose_disabled_reason_collection.h"
#include "info_collection/rs_layer_compose_collection.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
// normal callback functor for client users.
using ScreenChangeCallback = std::function<void(ScreenId, ScreenEvent, ScreenChangeReason)>;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
using PointerLuminanceChangeCallback = std::function<void(int32_t)>;
#endif
using BufferAvailableCallback = std::function<void()>;
using BufferClearCallback = std::function<void()>;
using OcclusionChangeCallback = std::function<void(std::shared_ptr<RSOcclusionData>)>;
using SurfaceOcclusionChangeCallback = std::function<void(float)>;
using HgmConfigChangeCallback = std::function<void(std::shared_ptr<RSHgmConfigData>)>;
using OnRemoteDiedCallback = std::function<void()>;
using HgmRefreshRateModeChangeCallback = std::function<void(int32_t)>;
using HgmRefreshRateUpdateCallback = std::function<void(int32_t)>;
using FrameRateLinkerExpectedFpsUpdateCallback = std::function<void(int32_t, int32_t)>;
using UIExtensionCallback = std::function<void(std::shared_ptr<RSUIExtensionData>, uint64_t)>;
using SelfDrawingNodeRectChangeCallback = std::function<void(std::shared_ptr<RSSelfDrawingNodeRectData>)>;
using FirstFrameCommitCallback = std::function<void(uint64_t, int64_t)>;
struct DataBaseRs {
    int32_t appPid = -1;
    int32_t eventType = -1;
    int32_t versionCode = -1;
    int64_t uniqueId = 0;
    int64_t inputTime = 0;
    int64_t beginVsyncTime = 0;
    int64_t endVsyncTime = 0;
    bool isDisplayAnimator = false;
    std::string sceneId;
    std::string versionName;
    std::string bundleName;
    std::string processName;
    std::string abilityName;
    std::string pageUrl;
    std::string sourceType;
    std::string note;
};

struct AppInfo {
    int64_t startTime = 0;
    int64_t endTime = 0;
    int32_t pid = 0;
    std::string versionName;
    int32_t versionCode = -1;
    std::string bundleName;
    std::string processName;
};
struct GameStateData {
    int32_t pid = -1;
    int32_t uid = 0;
    int32_t state = 0;
    int32_t renderTid = -1;
    std::string bundleName;
};

class SurfaceCaptureCallback {
public:
    SurfaceCaptureCallback() {}
    virtual ~SurfaceCaptureCallback() {}
    virtual void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) = 0;
};

class SurfaceBufferCallback {
public:
    SurfaceBufferCallback() = default;
    virtual ~SurfaceBufferCallback() noexcept = default;
    virtual void OnFinish(const FinishCallbackRet& ret) = 0;
    virtual void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) = 0;
};

class RSB_EXPORT RSRenderServiceClient : public RSIRenderClient {
public:
    RSRenderServiceClient() = default;
    virtual ~RSRenderServiceClient() = default;

    RSRenderServiceClient(const RSRenderServiceClient&) = delete;
    void operator=(const RSRenderServiceClient&) = delete;

    void CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData) override;
    void ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) override;

    bool GetUniRenderEnabled();

    bool CreateNode(const RSSurfaceRenderNodeConfig& config);
    bool CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId);
    std::shared_ptr<RSSurface> CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, bool unobscured = false);
    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr,
        uint64_t id = 0,
        NodeId windowNodeId = 0,
        bool fromXcomponent = false);

    int32_t GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid);

    std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurfaceId(uint64_t surfaceid, const Rect &srcRect);

    bool TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig,
        std::unique_ptr<Media::PixelMap> clientCapturePixelMap = nullptr,
        const RSSurfaceCaptureBlurParam& blurParam = {},
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f));

    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> TakeSurfaceCaptureSoloNode(
        NodeId id, const RSSurfaceCaptureConfig& captureConfig);

    bool TakeSelfSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig);

    bool SetWindowFreezeImmediately(NodeId id, bool isFreeze, std::shared_ptr<SurfaceCaptureCallback> callback,
        const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam = {});

    bool TakeUICaptureInRange(
        NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig);

    bool SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW);

    int32_t SetFocusAppInfo(const FocusAppInfo& info);

    ScreenId GetDefaultScreenId();
    ScreenId GetActiveScreenId();

    std::vector<ScreenId> GetAllScreenIds();

#ifndef ROSEN_CROSS_PLATFORM
    std::shared_ptr<RSSurface> CreateRSSurface(const sptr<Surface> &surface);
    ScreenId CreateVirtualScreen(const std::string& name, uint32_t width, uint32_t height, sptr<Surface> surface,
        ScreenId mirrorId, int32_t flags, std::vector<NodeId> whiteList = {});

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);

    int32_t SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);

    int32_t SetVirtualScreenTypeBlackList(ScreenId id, std::vector<NodeType>& typeBlackListVector);

    int32_t AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);

    int32_t RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);
#endif

    int32_t SetVirtualScreenSecurityExemptionList(ScreenId id, const std::vector<NodeId>& securityExemptionList);

    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask);

    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false);

    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable);

    bool SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark);

    void RemoveVirtualScreen(ScreenId id);

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int32_t SetPointerColorInversionConfig(float darkBuffer, float brightBuffer, int64_t interval, int32_t rangeSize);
 
    int32_t SetPointerColorInversionEnabled(bool enable);
 
    int32_t RegisterPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback);
 
    int32_t UnRegisterPointerLuminanceChangeCallback();
#endif

    int32_t SetScreenChangeCallback(const ScreenChangeCallback& callback);

#ifndef ROSEN_ARKUI_X
    void SetScreenActiveMode(ScreenId id, uint32_t modeId);
#endif // !ROSEN_ARKUI_X
    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);

    void SetRefreshRateMode(int32_t refreshRateMode);

    void SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range,
        int32_t animatorExpectedFrameRate);

    void UnregisterFrameRateLinker(FrameRateLinkerId id);

    uint32_t GetScreenCurrentRefreshRate(ScreenId id);

    int32_t GetCurrentRefreshRateMode();

    std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id);

    bool GetShowRefreshRateEnabled();

    void SetShowRefreshRateEnabled(bool enabled, int32_t type);

    uint32_t GetRealtimeRefreshRate(ScreenId screenId);

    std::string GetRefreshInfo(pid_t pid);
    std::string GetRefreshInfoToSP(NodeId id);

#ifndef ROSEN_ARKUI_X
    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height);

    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height);

    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id);

    void MarkPowerOffNeedProcessOneFrame();

    void RepaintEverything();

    void DisablePowerOffRenderControl(ScreenId id);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    RSScreenModeInfo GetScreenActiveMode(ScreenId id);

    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id);

    RSScreenCapability GetScreenCapability(ScreenId id);

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id);

    RSScreenData GetScreenData(ScreenId id);

    MemoryGraphic GetMemoryGraphic(int pid);

    std::vector<MemoryGraphic> GetMemoryGraphics();
#endif // !ROSEN_ARKUI_X
    bool GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize);

    int32_t GetScreenBacklight(ScreenId id);

    void SetScreenBacklight(ScreenId id, uint32_t level);

    bool RegisterBufferAvailableListener(
        NodeId id, const BufferAvailableCallback &callback, bool isFromRenderThread = false);

    bool RegisterBufferClearListener(
        NodeId id, const BufferClearCallback &callback);

    bool UnregisterBufferAvailableListener(NodeId id);

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode);

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys);

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode);

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode);

    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation);

    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation);

    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode);

    bool SetGlobalDarkColorMode(bool isDark);

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

    bool GetBitmap(NodeId id, Drawing::Bitmap& bitmap);
    bool GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
        const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList);
    bool RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);
    bool UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);

    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData);

    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval);

    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate);

    uint32_t SetScreenActiveRect(ScreenId id, const Rect& activeRect);

    int32_t RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback);

    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints);

    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id);

    int32_t RegisterHgmConfigChangeCallback(const HgmConfigChangeCallback& callback);

    int32_t RegisterHgmRefreshRateModeChangeCallback(const HgmRefreshRateModeChangeCallback& callback);

    int32_t RegisterHgmRefreshRateUpdateCallback(const HgmRefreshRateUpdateCallback& callback);

    int32_t RegisterFirstFrameCommitCallback(const FirstFrameCommitCallback& callback);

    int32_t RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
        const FrameRateLinkerExpectedFpsUpdateCallback& callback);

    void SetAppWindowNum(uint32_t num);

    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation = false);

    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);

    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height);

    void ReportJankStats();

    void NotifyLightFactorStatus(int32_t lightFactorStatus);

    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);

    void NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
        const std::vector<std::pair<std::string, std::string>>& newConfig);

    void NotifyRefreshRateEvent(const EventInfo& eventInfo);

    void SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos);

    void SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos);

    bool NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name, uint32_t rateDiscount);

    void NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt);

    void NotifyDynamicModeEvent(bool enableDynamicMode);

    void NotifyHgmConfigEvent(const std::string &eventName, bool state);

    void NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate);

    void ReportEventResponse(DataBaseRs info);

    void ReportEventComplete(DataBaseRs info);

    void ReportEventJankFrame(DataBaseRs info);

    void ReportRsSceneJankStart(AppInfo info);

    void ReportRsSceneJankEnd(AppInfo info);

    void ReportGameStateData(GameStateData info);

    void SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType, bool dynamicHardwareEnable);

    uint32_t SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent);

    void SetCacheEnabledForRotation(bool isEnabled);

    void SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback);

    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo();
 
    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo();

    LayerComposeInfo GetLayerComposeInfo();

    HwcDisabledReasonInfos GetHwcDisabledReasonInfo();

    int64_t GetHdrOnDuration();

    void SetVmaCacheStatus(bool flag);

    int32_t RegisterUIExtensionCallback(uint64_t userId, const UIExtensionCallback& callback, bool unobscured = false);

    bool SetAncoForceDoDirect(bool direct);

    void SetLayerTop(const std::string &nodeIdStr, bool isTop);

    void SetColorFollow(const std::string &nodeIdStr, bool isColorFollow);

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    int32_t SetOverlayDisplayMode(int32_t mode);
#endif

#ifdef TP_FEATURE_ENABLE
    void SetTpFeatureConfig(int32_t feature, const char* config,
        TpFeatureConfigType tpFeatureConfigType = TpFeatureConfigType::DEFAULT_TP_FEATURE);
#endif
    void SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus);
    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn);

    void DropFrameByPid(const std::vector<int32_t> pidList);
    
    bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus);

    void SetFreeMultiWindowStatus(bool enable);

    bool RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        std::shared_ptr<SurfaceBufferCallback> callback);

    bool UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid);

    void NotifyScreenSwitched();

    void ForceRefreshOneFrameWithNextVSync();

    void SetWindowContainer(NodeId nodeId, bool value);

    int32_t RegisterSelfDrawingNodeRectChangeCallback(const SelfDrawingNodeRectChangeCallback& callback);

    void NotifyPageName(const std::string &packageName, const std::string &pageName, bool isEnter);

    bool GetHighContrastTextState();

    bool RegisterTransactionDataCallback(int32_t pid, uint64_t timeStamp, std::function<void()> callback);

    bool SetBehindWindowFilterEnabled(bool enabled);

    bool GetBehindWindowFilterEnabled(bool& enabled);

    int32_t GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB);
private:
    void TriggerSurfaceCaptureCallback(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        std::shared_ptr<Media::PixelMap> pixelmap);
    void TriggerOnFinish(const FinishCallbackRet& ret) const;
    void TriggerOnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) const;
    void TriggerTransactionDataCallbackAndErase(int32_t pid, uint64_t timeStamp);
    struct RectHash {
        std::size_t operator()(const Drawing::Rect& rect) const {
            std::size_t h1 = std::hash<Drawing::scalar>()(rect.left_);
            std::size_t h2 = std::hash<Drawing::scalar>()(rect.top_);
            std::size_t h3 = std::hash<Drawing::scalar>()(rect.right_);
            std::size_t h4 = std::hash<Drawing::scalar>()(rect.bottom_);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    // Hash for solo node capture
    struct UICaptureParamHash {
        std::size_t operator()(const RSUICaptureInRangeParam& param) const {
            std::size_t h1 = std::hash<NodeId>()(param.endNodeId);
            std::size_t h2 = std::hash<bool>()(param.useBeginNodeSize);
            return h1 ^ (h2 << 1);
        }
    };

    struct PairHash {
        std::size_t operator()(const std::pair<NodeId, RSSurfaceCaptureConfig>& p) const {
            std::size_t h1 = std::hash<NodeId>()(p.first);
            std::size_t h2 = RectHash()(p.second.mainScreenRect);
            std::size_t h3 = UICaptureParamHash()(p.second.uiCaptureInRangeParam);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    std::mutex mutex_;
    std::mutex surfaceCaptureCbDirectorMutex_;
    std::map<NodeId, sptr<RSIBufferAvailableCallback>> bufferAvailableCbRTMap_;
    std::mutex mapMutex_;
    std::map<NodeId, sptr<RSIBufferAvailableCallback>> bufferAvailableCbUIMap_;
    sptr<RSIScreenChangeCallback> screenChangeCb_ = nullptr;
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCbDirector_ = nullptr;
    std::unordered_map<std::pair<NodeId, RSSurfaceCaptureConfig>,
        std::vector<std::shared_ptr<SurfaceCaptureCallback>>, PairHash> surfaceCaptureCbMap_;

    sptr<RSISurfaceBufferCallback> surfaceBufferCbDirector_;
    std::map<uint64_t, std::shared_ptr<SurfaceBufferCallback>> surfaceBufferCallbacks_;
    mutable std::shared_mutex surfaceBufferCallbackMutex_;

    sptr<RSITransactionDataCallback> transactionDataCbDirector_;
    std::map<std::pair<int32_t, uint64_t>, std::function<void()>> transactionDataCallbacks_;
    std::mutex transactionDataCallbackMutex_;

    friend class SurfaceCaptureCallbackDirector;
    friend class SurfaceBufferCallbackDirector;
    friend class TransactionDataCallbackDirector;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_RENDER_SERVICE_CLIENT_H
