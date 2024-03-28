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

#include <cstdint>
#include <functional>

#include "rs_interfaces.h"
#include "rs_trace.h"

#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_divided_ui_capture.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "ui/rs_frame_rate_policy.h"
#include "ui/rs_proxy_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSInterfaces &RSInterfaces::GetInstance()
{
    static RSInterfaces instance;
    return instance;
}

RSInterfaces::RSInterfaces() : renderServiceClient_(std::make_unique<RSRenderServiceClient>())
{
}

RSInterfaces::~RSInterfaces() noexcept
{
}

int32_t RSInterfaces::SetFocusAppInfo(FocusAppInfo& info)
{
    int32_t pid = info.pid;
    int32_t uid = info.uid;
    const std::string bundleName = info.bundleName;
    const std::string abilityName = info.abilityName;
    uint64_t focusNodeId = info.focusNodeId;
    return renderServiceClient_->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
}

ScreenId RSInterfaces::GetDefaultScreenId()
{
    return renderServiceClient_->GetDefaultScreenId();
}

ScreenId RSInterfaces::GetActiveScreenId()
{
    return renderServiceClient_->GetActiveScreenId();
}

std::vector<ScreenId> RSInterfaces::GetAllScreenIds()
{
    return renderServiceClient_->GetAllScreenIds();
}

#ifndef ROSEN_CROSS_PLATFORM
ScreenId RSInterfaces::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int flags,
    std::vector<NodeId> filteredAppVector)
{
    return renderServiceClient_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, filteredAppVector);
}

int32_t RSInterfaces::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    return renderServiceClient_->SetVirtualScreenSurface(id, surface);
}
#endif

void RSInterfaces::RemoveVirtualScreen(ScreenId id)
{
    renderServiceClient_->RemoveVirtualScreen(id);
}

int32_t RSInterfaces::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    return renderServiceClient_->SetScreenChangeCallback(callback);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    if (!node) {
        ROSEN_LOGW("node is nullptr");
        return false;
    }
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    if (!node) {
        ROSEN_LOGW("node is nullptr");
        return false;
    }
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY);
}

bool RSInterfaces::TakeSurfaceCapture(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    return renderServiceClient_->TakeSurfaceCapture(id, callback, scaleX, scaleY);
}

#ifndef ROSEN_ARKUI_X
void RSInterfaces::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    renderServiceClient_->SetScreenActiveMode(id, modeId);
}
#endif // !ROSEN_ARKUI_X
void RSInterfaces::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    renderServiceClient_->SetScreenRefreshRate(id, sceneId, rate);
}

void RSInterfaces::SetRefreshRateMode(int32_t refreshRateMode)
{
    renderServiceClient_->SetRefreshRateMode(refreshRateMode);
}

void RSInterfaces::SyncFrameRateRange(const FrameRateRange& range)
{
    renderServiceClient_->SyncFrameRateRange(range);
}

uint32_t RSInterfaces::GetScreenCurrentRefreshRate(ScreenId id)
{
    return renderServiceClient_->GetScreenCurrentRefreshRate(id);
}

int32_t RSInterfaces::GetCurrentRefreshRateMode()
{
    return renderServiceClient_->GetCurrentRefreshRateMode();
}

std::vector<int32_t> RSInterfaces::GetScreenSupportedRefreshRates(ScreenId id)
{
    return renderServiceClient_->GetScreenSupportedRefreshRates(id);
}

bool RSInterfaces::GetShowRefreshRateEnabled()
{
    return renderServiceClient_->GetShowRefreshRateEnabled();
}

void RSInterfaces::SetShowRefreshRateEnabled(bool enable)
{
    return renderServiceClient_->SetShowRefreshRateEnabled(enable);
}

bool RSInterfaces::TakeSurfaceCaptureForUI(std::shared_ptr<RSNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY, bool isSync)
{
    if (!node) {
        ROSEN_LOGW("RSInterfaces::TakeSurfaceCaptureForUI rsnode is nullpter return");
        return false;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSInterfaces::TakeSurfaceCaptureForUI unsupported node type return");
        return false;
    }
    if (RSSystemProperties::GetUniRenderEnabled()) {
        if (isSync) {
            node->SetTakeSurfaceForUIFlag();
        }
        return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, scaleX, scaleY,
            SurfaceCaptureType::UICAPTURE, isSync);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(node->GetId(), callback, scaleX, scaleY);
    }
}

bool RSInterfaces::RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    if (RSSystemProperties::GetUniRenderEnabled()) {
        return renderServiceClient_->RegisterTypeface(typeface);
    }
    return true;
}

bool RSInterfaces::UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    if (RSSystemProperties::GetUniRenderEnabled()) {
        return renderServiceClient_->UnRegisterTypeface(typeface);
    }
    return true;
}

#ifndef ROSEN_ARKUI_X
int32_t RSInterfaces::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return renderServiceClient_->SetVirtualScreenResolution(id, width, height);
}
#endif // !ROSEN_ARKUI_X
bool RSInterfaces::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    return renderServiceClient_->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

bool RSInterfaces::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    return renderServiceClient_->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

#ifndef ROSEN_ARKUI_X
RSVirtualScreenResolution RSInterfaces::GetVirtualScreenResolution(ScreenId id)
{
    return renderServiceClient_->GetVirtualScreenResolution(id);
}

void RSInterfaces::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    RS_LOGI("[UL_POWER]RSInterfaces::SetScreenPowerStatus: ScreenId: %{public}" PRIu64
            ", ScreenPowerStatus: %{public}u",
        id, static_cast<uint32_t>(status));
    renderServiceClient_->SetScreenPowerStatus(id, status);
}
#endif // !ROSEN_ARKUI_X
bool RSInterfaces::TakeSurfaceCaptureForUIWithoutUni(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    std::function<void()> offscreenRenderTask = [scaleX, scaleY, callback, id, this]() -> void {
        ROSEN_LOGD(
            "RSInterfaces::TakeSurfaceCaptureForUIWithoutUni callback->OnOffscreenRender nodeId:"
            "[%{public}" PRIu64 "]", id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::TakeSurfaceCaptureForUIWithoutUni");
        std::shared_ptr<RSDividedUICapture> rsDividedUICapture =
            std::make_shared<RSDividedUICapture>(id, scaleX, scaleY);
        std::shared_ptr<Media::PixelMap> pixelmap = rsDividedUICapture->TakeLocalCapture();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        callback->OnSurfaceCapture(pixelmap);
    };
    RSOffscreenRenderThread::Instance().PostTask(offscreenRenderTask);
    return true;
}

#ifndef ROSEN_ARKUI_X
RSScreenModeInfo RSInterfaces::GetScreenActiveMode(ScreenId id)
{
    return renderServiceClient_->GetScreenActiveMode(id);
}

std::vector<RSScreenModeInfo> RSInterfaces::GetScreenSupportedModes(ScreenId id)
{
    return renderServiceClient_->GetScreenSupportedModes(id);
}

RSScreenCapability RSInterfaces::GetScreenCapability(ScreenId id)
{
    return renderServiceClient_->GetScreenCapability(id);
}

ScreenPowerStatus RSInterfaces::GetScreenPowerStatus(ScreenId id)
{
    return renderServiceClient_->GetScreenPowerStatus(id);
}

RSScreenData RSInterfaces::GetScreenData(ScreenId id)
{
    return renderServiceClient_->GetScreenData(id);
}
#endif // !ROSEN_ARKUI_X
int32_t RSInterfaces::GetScreenBacklight(ScreenId id)
{
    return renderServiceClient_->GetScreenBacklight(id);
}

void RSInterfaces::SetScreenBacklight(ScreenId id, uint32_t level)
{
    RS_LOGD("RSInterfaces::SetScreenBacklight: ScreenId: %{public}" PRIu64 ", level: %{public}u", id, level);
    renderServiceClient_->SetScreenBacklight(id, level);
}

int32_t RSInterfaces::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    return renderServiceClient_->GetScreenSupportedColorGamuts(id, mode);
}

int32_t RSInterfaces::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    return renderServiceClient_->GetScreenSupportedMetaDataKeys(id, keys);
}

int32_t RSInterfaces::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    return renderServiceClient_->GetScreenColorGamut(id, mode);
}

int32_t RSInterfaces::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    return renderServiceClient_->SetScreenColorGamut(id, modeIdx);
}

int32_t RSInterfaces::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    return renderServiceClient_->SetScreenGamutMap(id, mode);
}

int32_t RSInterfaces::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    return renderServiceClient_->SetScreenCorrection(id, screenRotation);
}

int32_t RSInterfaces::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    return renderServiceClient_->GetScreenGamutMap(id, mode);
}

std::shared_ptr<VSyncReceiver> RSInterfaces::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper)
{
    return renderServiceClient_->CreateVSyncReceiver(name, looper);
}

std::shared_ptr<VSyncReceiver> RSInterfaces::CreateVSyncReceiver(
    const std::string& name,
    uint64_t id,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper)
{
    return renderServiceClient_->CreateVSyncReceiver(name, looper, id);
}

int32_t RSInterfaces::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    return renderServiceClient_->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSInterfaces::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat)
{
    return renderServiceClient_->GetPixelFormat(id, pixelFormat);
}

int32_t RSInterfaces::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    return renderServiceClient_->SetPixelFormat(id, pixelFormat);
}

int32_t RSInterfaces::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats)
{
    return renderServiceClient_->GetScreenSupportedHDRFormats(id, hdrFormats);
}

int32_t RSInterfaces::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat)
{
    return renderServiceClient_->GetScreenHDRFormat(id, hdrFormat);
}

int32_t RSInterfaces::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    return renderServiceClient_->SetScreenHDRFormat(id, modeIdx);
}

int32_t RSInterfaces::GetScreenSupportedColorSpaces(ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    return renderServiceClient_->GetScreenSupportedColorSpaces(id, colorSpaces);
}

int32_t RSInterfaces::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace)
{
    return renderServiceClient_->GetScreenColorSpace(id, colorSpace);
}

int32_t RSInterfaces::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    return renderServiceClient_->SetScreenColorSpace(id, colorSpace);
}

int32_t RSInterfaces::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    return renderServiceClient_->GetScreenType(id, screenType);
}

int32_t RSInterfaces::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    return renderServiceClient_->SetScreenSkipFrameInterval(id, skipFrameInterval);
}

bool RSInterfaces::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes)
{
    return renderServiceClient_->SetSystemAnimatedScenes(systemAnimatedScenes);
}

int32_t RSInterfaces::RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    return renderServiceClient_->RegisterOcclusionChangeCallback(callback);
}

int32_t RSInterfaces::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints)
{
    return renderServiceClient_->RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
}

int32_t RSInterfaces::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    return renderServiceClient_->UnRegisterSurfaceOcclusionChangeCallback(id);
}

int32_t RSInterfaces::RegisterHgmConfigChangeCallback(const HgmConfigChangeCallback& callback)
{
    return renderServiceClient_->RegisterHgmConfigChangeCallback(callback);
}

int32_t RSInterfaces::RegisterHgmRefreshRateModeChangeCallback(const HgmRefreshRateModeChangeCallback& callback)
{
    return renderServiceClient_->RegisterHgmRefreshRateModeChangeCallback(callback);
}

void RSInterfaces::SetAppWindowNum(uint32_t num)
{
    renderServiceClient_->SetAppWindowNum(num);
}

void RSInterfaces::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    renderServiceClient_->ShowWatermark(watermarkImg, isShow);
}

int32_t RSInterfaces::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    return renderServiceClient_->ResizeVirtualScreen(id, width, height);
}

#ifndef ROSEN_ARKUI_X
MemoryGraphic RSInterfaces::GetMemoryGraphic(int pid)
{
    return renderServiceClient_->GetMemoryGraphic(pid);
}

std::vector<MemoryGraphic> RSInterfaces::GetMemoryGraphics()
{
    return renderServiceClient_->GetMemoryGraphics();
}
#endif // !ROSEN_ARKUI_X
bool RSInterfaces::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    return renderServiceClient_->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
}

void RSInterfaces::ReportJankStats()
{
    renderServiceClient_->ReportJankStats();
}

void RSInterfaces::ReportEventResponse(DataBaseRs info)
{
    renderServiceClient_->ReportEventResponse(info);
}

void RSInterfaces::ReportEventComplete(DataBaseRs info)
{
    renderServiceClient_->ReportEventComplete(info);
}

void RSInterfaces::ReportEventJankFrame(DataBaseRs info)
{
    renderServiceClient_->ReportEventJankFrame(info);
}

void RSInterfaces::ReportGameStateData(GameStateData info)
{
    renderServiceClient_->ReportGameStateData(info);
}

void RSInterfaces::EnableCacheForRotation()
{
    renderServiceClient_->SetCacheEnabledForRotation(true);
}

void RSInterfaces::NotifyLightFactorStatus(bool isSafe)
{
    renderServiceClient_->NotifyLightFactorStatus(isSafe);
}

void RSInterfaces::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    renderServiceClient_->NotifyPackageEvent(listSize, packageList);
}

void RSInterfaces::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    renderServiceClient_->NotifyRefreshRateEvent(eventInfo);
}

void RSInterfaces::NotifyTouchEvent(int32_t touchStatus)
{
    renderServiceClient_->NotifyTouchEvent(touchStatus);
}

void RSInterfaces::DisableCacheForRotation()
{
    renderServiceClient_->SetCacheEnabledForRotation(false);
}

void RSInterfaces::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
    renderServiceClient_->SetOnRemoteDiedCallback(callback);
}

GpuDirtyRegionInfo RSInterfaces::GetCurrentDirtyRegionInfo(ScreenId id)
{
    return renderServiceClient_->GetCurrentDirtyRegionInfo(id);
}

#ifdef TP_FEATURE_ENABLE
void RSInterfaces::SetTpFeatureConfig(int32_t feature, const char* config)
{
    renderServiceClient_->SetTpFeatureConfig(feature, config);
}
#endif

void RSInterfaces::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
    renderServiceClient_->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
}

void RSInterfaces::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    renderServiceClient_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
}
} // namespace Rosen
} // namespace OHOS
