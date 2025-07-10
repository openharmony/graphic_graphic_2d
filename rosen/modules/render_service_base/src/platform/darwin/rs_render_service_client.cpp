/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "transaction/rs_render_service_client.h"

#include "platform/common/rs_log.h"
#include "rs_surface_darwin.h"
#include "rs_vsync_client_darwin.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSIRenderClient> RSIRenderClient::CreateRenderServiceClient()
{
    static std::shared_ptr<RSIRenderClient> client = std::make_shared<RSRenderServiceClient>();
    return client;
}

void RSRenderServiceClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
}

void RSRenderServiceClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task) {}

MemoryGraphic RSRenderServiceClient::GetMemoryGraphic(int pid)
{
    return {};
}

std::vector<MemoryGraphic> RSRenderServiceClient::GetMemoryGraphics()
{
    return {};
}

bool RSRenderServiceClient::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    return {};
}

bool RSRenderServiceClient::SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark)
{
    return false;
}

bool RSRenderServiceClient::GetUniRenderEnabled()
{
    return {};
}

bool RSRenderServiceClient::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId)
{
    return {};
}

bool RSRenderServiceClient::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    return {};
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    bool unobscured)
{
    return std::make_shared<RSSurfaceDarwin>(reinterpret_cast<OnRenderFunc>(config.additionalData));
}

class VSyncReceiverDarwin : public VSyncReceiver {
    std::unique_ptr<RSVsyncClient> client_ = nullptr;

public:
    VsyncError Init() override
    {
        client_ = RSVsyncClient::Create();
        if (client_ == nullptr) {
            return GSERROR_NO_MEM;
        }

        return GSERROR_OK;
    }

    VsyncError RequestNextVSync(FrameCallback callback) override
    {
        if (client_ == nullptr) {
            return GSERROR_NOT_INIT;
        }

        auto func = [callback](int64_t time, int64_t frameCount) {
            callback.callbackWithId_(time, frameCount, callback.userData_);
        };
        client_->SetVsyncCallback(func);
        client_->RequestNextVsync();
        return GSERROR_OK;
    }

    VsyncError SetVSyncRate(FrameCallback callback, int32_t rate) override
    {
        if (client_ == nullptr) {
            return GSERROR_NOT_INIT;
        }

        return GSERROR_OK;
    }
};

std::shared_ptr<VSyncReceiver> RSRenderServiceClient::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper,
    uint64_t id,
    NodeId windowNodeId,
    bool fromXcomponent)
{
    return std::make_shared<VSyncReceiverDarwin>();
}

int32_t RSRenderServiceClient::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid)
{
    return 0;
}

std::shared_ptr<Media::PixelMap> RSRenderServiceClient::CreatePixelMapFromSurfaceId(uint64_t surfaceId,
    const Rect &srcRect)
{
    return nullptr;
}

bool RSRenderServiceClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect)
{
    return false;
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderServiceClient::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig)
{
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> result;
    return result;
}

bool RSRenderServiceClient::TakeSelfSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
    return false;
}

bool RSRenderServiceClient::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    return false;
}

bool RSRenderServiceClient::TakeUICaptureInRange(
    NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    return false;
}

bool RSRenderServiceClient::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    return false;
}

int32_t RSRenderServiceClient::SetFocusAppInfo(const FocusAppInfo& info)
{
    return false;
}

ScreenId RSRenderServiceClient::GetDefaultScreenId()
{
    return 0;
}

ScreenId RSRenderServiceClient::GetActiveScreenId()
{
    return 0;
}

std::vector<ScreenId> RSRenderServiceClient::GetAllScreenIds()
{
    return {0};
}

void RSRenderServiceClient::RemoveVirtualScreen(ScreenId id)
{
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
int32_t RSRenderServiceClient::SetPointerColorInversionConfig(float darkBuffer, float brightBuffer,
    int64_t interval, int32_t rangeSize)
{
    return 0;
}
 
int32_t RSRenderServiceClient::SetPointerColorInversionEnabled(bool enable)
{
    return 0;
}
 
int32_t RSRenderServiceClient::RegisterPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback)
{
    return 0;
}
 
int32_t RSRenderServiceClient::UnRegisterPointerLuminanceChangeCallback()
{
    return 0;
}
#endif

int32_t RSRenderServiceClient::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    return 0;
}

void RSRenderServiceClient::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
}

void RSRenderServiceClient::SetScreenOffset(ScreenId id, int32_t offSetX, int32_t offSetY)
{
}

void RSRenderServiceClient::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
}

void RSRenderServiceClient::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
}

void RSRenderServiceClient::SetRefreshRateMode(int32_t refreshRateMode)
{
}

void RSRenderServiceClient::SyncFrameRateRange(FrameRateLinkerId id,
    const FrameRateRange& range, int32_t animatorExpectedFrameRate)
{
}

void RSRenderServiceClient::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
}

uint32_t RSRenderServiceClient::GetScreenCurrentRefreshRate(ScreenId id)
{
    return {};
}

int32_t RSRenderServiceClient::GetCurrentRefreshRateMode()
{
    return {};
}

std::vector<int32_t> RSRenderServiceClient::GetScreenSupportedRefreshRates(ScreenId id)
{
    return {};
}

bool RSRenderServiceClient::GetShowRefreshRateEnabled()
{
    return false;
}

void RSRenderServiceClient::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
}

uint32_t RSRenderServiceClient::GetRealtimeRefreshRate(ScreenId id)
{
    return {};
}

std::string RSRenderServiceClient::GetRefreshInfo(pid_t pid)
{
    return "";
}

std::string RSRenderServiceClient::GetRefreshInfoToSP(NodeId id)
{
    return "";
}


int32_t RSRenderServiceClient::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return 0;
}

int32_t RSRenderServiceClient::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return {};
}

RSVirtualScreenResolution RSRenderServiceClient::GetVirtualScreenResolution(ScreenId id)
{
    return {};
}

void RSRenderServiceClient::MarkPowerOffNeedProcessOneFrame()
{
}

void RSRenderServiceClient::RepaintEverything()
{
}

void RSRenderServiceClient::ForceRefreshOneFrameWithNextVSync()
{
}

void RSRenderServiceClient::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
}

void RSRenderServiceClient::DisablePowerOffRenderControl(ScreenId id)
{
}

RSScreenModeInfo RSRenderServiceClient::GetScreenActiveMode(ScreenId id)
{
    return {};
}

std::vector<RSScreenModeInfo> RSRenderServiceClient::GetScreenSupportedModes(ScreenId id)
{
    return {};
}

RSScreenCapability RSRenderServiceClient::GetScreenCapability(ScreenId id)
{
    return {};
}

ScreenPowerStatus RSRenderServiceClient::GetScreenPowerStatus(ScreenId id)
{
    return {};
}

RSScreenData RSRenderServiceClient::GetScreenData(ScreenId id)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenBacklight(ScreenId id)
{
    return {};
}

void RSRenderServiceClient::SetScreenBacklight(ScreenId id, uint32_t level)
{
}

bool RSRenderServiceClient::RegisterBufferAvailableListener(
    NodeId id, const BufferAvailableCallback &callback, bool isFromRenderThread)
{
    return {};
}

bool RSRenderServiceClient::RegisterBufferClearListener(
        NodeId id, const BufferClearCallback& callback)
{
    return {};
}

bool RSRenderServiceClient::UnregisterBufferAvailableListener(NodeId id)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    return {};
}

bool RSRenderServiceClient::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    return {};
}

int32_t RSRenderServiceClient::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    return {};
}

bool RSRenderServiceClient::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    return {};
}

bool RSRenderServiceClient::SetGlobalDarkColorMode(bool isDark)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    return {};
}

int32_t RSRenderServiceClient::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat)
{
    return {};
}

int32_t RSRenderServiceClient::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    return {};
}

bool RSRenderServiceClient::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
{
    return {};
}

bool RSRenderServiceClient::GetPixelmap(NodeId id, const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    return {};
}

bool RSRenderServiceClient::RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    return {};
}

bool RSRenderServiceClient::UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    return {};
}

int32_t RSRenderServiceClient::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    return 0;
}

int32_t RSRenderServiceClient::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    return 0;
}

int32_t RSRenderServiceClient::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
    bool supportRotation)
{
    return {};
}

int32_t RSRenderServiceClient::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    return {};
}

int32_t RSRenderServiceClient::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    return {};
}

uint32_t RSRenderServiceClient::SetScreenActiveRect(ScreenId id, const Rect& activeRect)
{
    return {};
}

int32_t RSRenderServiceClient::RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    return {};
}

int32_t RSRenderServiceClient::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints)
{
    return {};
}

int32_t RSRenderServiceClient::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    return {};
}

int32_t RSRenderServiceClient::RegisterHgmConfigChangeCallback(const HgmConfigChangeCallback& callback)
{
    return {};
}

int32_t RSRenderServiceClient::RegisterHgmRefreshRateModeChangeCallback(
    const HgmRefreshRateModeChangeCallback& callback)
{
    return {};
}

int32_t RSRenderServiceClient::RegisterHgmRefreshRateUpdateCallback(
    const HgmRefreshRateUpdateCallback& callback)
{
    return {};
}

int32_t RSRenderServiceClient::RegisterFirstFrameCommitCallback(
    const FirstFrameCommitCallback& callback)
{
    return {};
}

int32_t RSRenderServiceClient::RegisterFrameRateLinkerExpectedFpsUpdateCallback(
    int32_t dstPid, const FrameRateLinkerExpectedFpsUpdateCallback& callback)
{
    return {};
}

void RSRenderServiceClient::SetAppWindowNum(uint32_t num)
{
}

bool RSRenderServiceClient::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    return {};
}

void RSRenderServiceClient::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
}

int32_t RSRenderServiceClient::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    return {};
}

void RSRenderServiceClient::ReportJankStats()
{
}

void RSRenderServiceClient::ReportEventResponse(DataBaseRs info)
{
}

void RSRenderServiceClient::ReportEventComplete(DataBaseRs info)
{
}

void RSRenderServiceClient::ReportEventJankFrame(DataBaseRs info)
{
}

void RSRenderServiceClient::ReportRsSceneJankStart(AppInfo info)
{
}

void RSRenderServiceClient::ReportRsSceneJankEnd(AppInfo info)
{
}

void RSRenderServiceClient::ReportGameStateData(GameStateData info)
{
}

void RSRenderServiceClient::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
}

void RSRenderServiceClient::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
}

void RSRenderServiceClient::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
}

void RSRenderServiceClient::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
}

void RSRenderServiceClient::SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos)
{
}

void RSRenderServiceClient::SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos)
{
}

bool RSRenderServiceClient::NotifySoftVsyncRateDiscountEvent(uint32_t pid,
    const std::string &name, uint32_t rateDiscount)
{
    return {};
}

void RSRenderServiceClient::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
}

void RSRenderServiceClient::NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate)
{
}

void RSRenderServiceClient::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
}

void RSRenderServiceClient::NotifyDynamicModeEvent(bool enableDynamicMode)
{
}

void RSRenderServiceClient::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
}

uint32_t RSRenderServiceClient::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent)
{
    return {};
}

void RSRenderServiceClient::SetCacheEnabledForRotation(bool isEnabled)
{
}

void RSRenderServiceClient::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
}

std::vector<ActiveDirtyRegionInfo> RSRenderServiceClient::GetActiveDirtyRegionInfo()
{
    return {};
}

GlobalDirtyRegionInfo RSRenderServiceClient::GetGlobalDirtyRegionInfo()
{
    return GlobalDirtyRegionInfo {};
}

LayerComposeInfo RSRenderServiceClient::GetLayerComposeInfo()
{
    return LayerComposeInfo {};
}

HwcDisabledReasonInfos RSRenderServiceClient::GetHwcDisabledReasonInfo()
{
    return {};
}

int64_t RSRenderServiceClient::GetHdrOnDuration()
{
    return 0;
}

void RSRenderServiceClient::SetVmaCacheStatus(bool flag)
{
}

#ifdef TP_FEATURE_ENABLE
void RSRenderServiceClient::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
}
#endif

void RSRenderServiceClient::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
}

void RSRenderServiceClient::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
}

void RSRenderServiceClient::DropFrameByPid(const std::vector<int32_t> pidList)
{
}

int32_t RSRenderServiceClient::RegisterUIExtensionCallback(uint64_t userId, const UIExtensionCallback& callback,
    bool unobscured)
{
    return {};
}

bool RSRenderServiceClient::SetAncoForceDoDirect(bool direct)
{
    return false;
}

bool RSRenderServiceClient::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    return false;
}

void RSRenderServiceClient::SetFreeMultiWindowStatus(bool enable)
{
}

bool RSRenderServiceClient::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    std::shared_ptr<SurfaceBufferCallback> callback)
{
    return false;
}

bool RSRenderServiceClient::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    return false;
}

void RSRenderServiceClient::SetLayerTopForHWC(const std::string &nodeIdStr, bool isTop, uint32_t zOrder)
{
}

void RSRenderServiceClient::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
}

void RSRenderServiceClient::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
}

bool RSRenderServiceClient::RegisterTransactionDataCallback(uint64_t token, uint64_t timeStamp,
    std::function<void()> callback)
{
    return false;
}

void RSRenderServiceClient::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
}

void RSRenderServiceClient::NotifyScreenSwitched()
{
}

void RSRenderServiceClient::SetWindowContainer(NodeId nodeId, bool value)
{
}

int32_t RSRenderServiceClient::RegisterSelfDrawingNodeRectChangeCallback(
    const RectConstraint& constraint, const SelfDrawingNodeRectChangeCallback& callback)
{
    return {};
}

int32_t RSRenderServiceClient::UnRegisterSelfDrawingNodeRectChangeCallback()
{
    return {};
}

void RSRenderServiceClient::NotifyPageName(const std::string &packageName,
    const std::string &pageName, bool isEnter)
{
}

bool RSRenderServiceClient::GetHighContrastTextState()
{
    return false;
}

bool RSRenderServiceClient::SetBehindWindowFilterEnabled(bool enabled)
{
    return false;
}

bool RSRenderServiceClient::GetBehindWindowFilterEnabled(bool& enabled)
{
    return false;
}

int32_t RSRenderServiceClient::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    return {};
}

void RSRenderServiceClient::ClearUifirstCache(NodeId id)
{
}
} // namespace Rosen
} // namespace OHOS
