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

struct FocusAppInfo {
    int32_t pid = -1;
    int32_t uid = -1;
    std::string bundleName;
    std::string abilityName;
    uint64_t focusNodeId;
};

class RSC_EXPORT RSInterfaces {
public:
    static RSInterfaces &GetInstance();
    RSInterfaces(const RSInterfaces &) = delete;
    void operator=(const RSInterfaces &) = delete;

    int32_t SetFocusAppInfo(FocusAppInfo& info);

    ScreenId GetDefaultScreenId();

    // for bootAnimation only
    ScreenId GetActiveScreenId();

    std::vector<ScreenId> GetAllScreenIds();

    // mirrorId: decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
#ifndef ROSEN_CROSS_PLATFORM
    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int flags = 0,
        std::vector<NodeId> filteredAppVector = {});

    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);
#endif

    void RemoveVirtualScreen(ScreenId id);

    int32_t SetScreenChangeCallback(const ScreenChangeCallback &callback);

    bool TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.0f, float scaleY = 1.0f);

    bool TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.0f, float scaleY = 1.0f);

    bool TakeSurfaceCapture(NodeId id,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.0f, float scaleY = 1.0f);

    bool TakeSurfaceCaptureForUI(std::shared_ptr<RSNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.f, float scaleY = 1.f, bool isSync = false);

    bool RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);
    bool UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);
#ifndef ROSEN_ARKUI_X
    void SetScreenActiveMode(ScreenId id, uint32_t modeId);

    MemoryGraphic GetMemoryGraphic(int pid);

    std::vector<MemoryGraphic> GetMemoryGraphics();
#endif // !ROSEN_ARKUI_X
    bool GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize);

#ifndef ROSEN_ARKUI_X
    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height);
#endif // !ROSEN_ARKUI_X

    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation);

    // set scale mode for virtual screen
    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode);
#ifndef ROSEN_ARKUI_X
    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id);

    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);

    RSScreenModeInfo GetScreenActiveMode(ScreenId id);
#endif // !ROSEN_ARKUI_X

    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);

    void SetRefreshRateMode(int32_t refreshRateMode);

    void SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range);

    uint32_t GetScreenCurrentRefreshRate(ScreenId id);

    int32_t GetCurrentRefreshRateMode();

    std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id);

    bool GetShowRefreshRateEnabled();

    void SetShowRefreshRateEnabled(bool enable);

#ifndef ROSEN_ARKUI_X
    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id);

    RSScreenCapability GetScreenCapability(ScreenId id);

    ScreenPowerStatus GetScreenPowerStatus(ScreenId id);

    RSScreenData GetScreenData(ScreenId id);
#endif // !ROSEN_ARKUI_X
    int32_t GetScreenBacklight(ScreenId id);

    void SetScreenBacklight(ScreenId id, uint32_t level);

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode);

    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys);

    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode);

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);

    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode);

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

    /* skipFrameInterval : decide how many frames apart to refresh a frame,
       DEFAULT_SKIP_FRAME_INTERVAL means refresh each frame,
       change screen refresh rate finally */
    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval);

    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr);

    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        uint64_t id,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr,
        NodeId windowNodeId = 0);

    std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurfaceId(uint64_t surfaceId, const Rect &srcRect);

    int32_t RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback);

    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints);

    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id);

    int32_t RegisterHgmConfigChangeCallback(const HgmConfigChangeCallback& callback);

    int32_t RegisterHgmRefreshRateModeChangeCallback(const HgmRefreshRateModeChangeCallback& callback);

    int32_t RegisterHgmRefreshRateUpdateCallback(const HgmRefreshRateUpdateCallback& callback);

    int32_t UnRegisterHgmRefreshRateUpdateCallback();

    void SetAppWindowNum(uint32_t num);

    // Set the system overload Animated Scenes to RS for special load shedding
    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes);

    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);

    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height);

    void ReportJankStats();

    void NotifyLightFactorStatus(bool isSafe);

    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);

    void NotifyRefreshRateEvent(const EventInfo& eventInfo);

    void NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt);

    void ReportEventResponse(DataBaseRs info);

    void ReportEventComplete(DataBaseRs info);

    void ReportEventJankFrame(DataBaseRs info);

    void ReportGameStateData(GameStateData info);

    void EnableCacheForRotation();

    void DisableCacheForRotation();

    void SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback);

    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn);

    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo();

    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo();

    LayerComposeInfo GetLayerComposeInfo();

#ifdef TP_FEATURE_ENABLE
    void SetTpFeatureConfig(int32_t feature, const char* config);
#endif
    void SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus);

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
