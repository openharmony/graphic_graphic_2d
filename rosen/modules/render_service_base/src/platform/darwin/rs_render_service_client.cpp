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

bool RSRenderServiceClient::GetUniRenderEnabled()
{
    return {};
}

bool RSRenderServiceClient::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    return {};
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
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

        auto func = [callback](int64_t time) {
            callback.callback_(time, callback.userData_);
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
    NodeId windowNodeId)
{
    return std::make_shared<VSyncReceiverDarwin>();
}

std::shared_ptr<Media::PixelMap> RSRenderServiceClient::CreatePixelMapFromSurfaceId(uint64_t surfaceId,
    const Rect &srcRect)
{
    return nullptr;
}

bool RSRenderServiceClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    float scaleX, float scaleY, SurfaceCaptureType surfaceCaptureType, bool isSync)
{
    return false;
}

int32_t RSRenderServiceClient::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName, uint64_t focusNodeId)
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

int32_t RSRenderServiceClient::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    return 0;
}

void RSRenderServiceClient::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
}

void RSRenderServiceClient::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
}

void RSRenderServiceClient::SetRefreshRateMode(int32_t refreshRateMode)
{
}

void RSRenderServiceClient::SyncFrameRateRange(FrameRateLinkerId id,
    const FrameRateRange& range, bool isAnimatorStopped)
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

void RSRenderServiceClient::SetShowRefreshRateEnabled(bool enable)
{
}

int32_t RSRenderServiceClient::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return {};
}

RSVirtualScreenResolution RSRenderServiceClient::GetVirtualScreenResolution(ScreenId id)
{
    return {};
}

void RSRenderServiceClient::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
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

bool RSRenderServiceClient::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
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

int32_t RSRenderServiceClient::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
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

void RSRenderServiceClient::SetAppWindowNum(uint32_t num)
{
}

bool RSRenderServiceClient::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes)
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

void RSRenderServiceClient::ReportGameStateData(GameStateData info)
{
}

void RSRenderServiceClient::NotifyLightFactorStatus(bool isSafe)
{
}

void RSRenderServiceClient::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
}

void RSRenderServiceClient::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
}

void RSRenderServiceClient::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
}

void RSRenderServiceClient::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType)
{
}

void RSRenderServiceClient::SetCacheEnabledForRotation(bool isEnabled)
{
}

void RSRenderServiceClient::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
}

GpuDirtyRegionInfo RSRenderServiceClient::GetCurrentDirtyRegionInfo(ScreenId id)
{
    return GpuDirtyRegionInfo {};
}

#ifdef TP_FEATURE_ENABLE
void RSRenderServiceClient::SetTpFeatureConfig(int32_t feature, const char* config)
{
}
#endif

void RSRenderServiceClient::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
}

void RSRenderServiceClient::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
}
} // namespace Rosen
} // namespace OHOS
