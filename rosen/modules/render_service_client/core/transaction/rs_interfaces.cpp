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
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "feature/ui_capture/rs_divided_ui_capture.h"
#include "ui/rs_proxy_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_typeface_cache.h"
namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
namespace {
constexpr uint32_t WATERMARK_PIXELMAP_SIZE_LIMIT = 500 * 1024;
constexpr uint32_t WATERMARK_NAME_LENGTH_LIMIT = 128;
constexpr int32_t SECURITYMASK_IMAGE_WIDTH_LIMIT = 4096;
constexpr int32_t SECURITYMASK_IMAGE_HEIGHT_LIMIT = 4096;
}
#endif
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

int32_t RSInterfaces::SetFocusAppInfo(const FocusAppInfo& info)
{
    return renderServiceClient_->SetFocusAppInfo(info);
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
    std::vector<NodeId> whiteList)
{
    return renderServiceClient_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);
}

int32_t RSInterfaces::SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    return renderServiceClient_->SetVirtualScreenBlackList(id, blackListVector);
}

int32_t RSInterfaces::SetVirtualScreenTypeBlackList(ScreenId id, std::vector<NodeType>& typeBlackListVector)
{
    return renderServiceClient_->SetVirtualScreenTypeBlackList(id, typeBlackListVector);
}

int32_t RSInterfaces::AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    return renderServiceClient_->AddVirtualScreenBlackList(id, blackListVector);
}

int32_t RSInterfaces::RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    return renderServiceClient_->RemoveVirtualScreenBlackList(id, blackListVector);
}

int32_t RSInterfaces::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    return renderServiceClient_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

int32_t RSInterfaces::SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask)
{
    Media::ImageInfo imageInfo;
    if (securityMask) {
        securityMask->GetImageInfo(imageInfo);
    }
    if (securityMask && (imageInfo.size.width > SECURITYMASK_IMAGE_WIDTH_LIMIT ||
        imageInfo.size.height > SECURITYMASK_IMAGE_HEIGHT_LIMIT)) {
        ROSEN_LOGE("SetScreenSecurityMask failed, securityMask width: %{public}d, height: %{public}d is error",
            imageInfo.size.width, imageInfo.size.height);
        return RS_CONNECTION_ERROR;
    }
    return renderServiceClient_->SetScreenSecurityMask(id, std::move(securityMask));
}

int32_t RSInterfaces::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation)
{
    return renderServiceClient_->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

int32_t RSInterfaces::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    return renderServiceClient_->SetCastScreenEnableSkipWindow(id, enable);
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

bool RSInterfaces::SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark)
{
#ifdef ROSEN_OHOS
    if (renderServiceClient_ == nullptr) {
        return false;
    }
    if (name.length() > WATERMARK_NAME_LENGTH_LIMIT || name.empty()) {
        ROSEN_LOGE("SetWatermark failed, name[%{public}s] is error.", name.c_str());
        return false;
    }
    if (watermark && (watermark->IsAstc() || watermark->GetCapacity() > WATERMARK_PIXELMAP_SIZE_LIMIT)) {
        ROSEN_LOGE("SetWatermark failed, watermark[%{public}d, %{public}d] is error",
            watermark->IsAstc(), watermark->GetCapacity());
        return false;
    }
    return renderServiceClient_->SetWatermark(name, watermark);
#else
    return false;
#endif
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
int32_t RSInterfaces::SetPointerColorInversionConfig(float darkBuffer, float brightBuffer,
    int64_t interval, int32_t rangeSize)
{
    if (renderServiceClient_ == nullptr) {
        return StatusCode::RENDER_SERVICE_NULL;
    }
    return renderServiceClient_->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
}

int32_t RSInterfaces::SetPointerColorInversionEnabled(bool enable)
{
    if (renderServiceClient_ == nullptr) {
        return StatusCode::RENDER_SERVICE_NULL;
    }
    return renderServiceClient_->SetPointerColorInversionEnabled(enable);
}

int32_t RSInterfaces::RegisterPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback)
{
    if (renderServiceClient_ == nullptr) {
        return StatusCode::RENDER_SERVICE_NULL;
    }
    return renderServiceClient_->RegisterPointerLuminanceChangeCallback(callback);
}

int32_t RSInterfaces::UnRegisterPointerLuminanceChangeCallback()
{
    if (renderServiceClient_ == nullptr) {
        return StatusCode::RENDER_SERVICE_NULL;
    }
    return renderServiceClient_->UnRegisterPointerLuminanceChangeCallback();
}
#endif

int32_t RSInterfaces::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    ROSEN_LOGI("RSInterfaces::%{public}s", __func__);
    return renderServiceClient_->SetScreenChangeCallback(callback);
}

int32_t RSInterfaces::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid)
{
    return renderServiceClient_->GetPixelMapByProcessId(pixelMapInfoVector, pid);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSInterfaces::TakeSurfaceCaptureWithBlur(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig, float blurRadius)
{
    if (blurRadius < 1) {
        ROSEN_LOGW("%{public}s no blur effect", __func__);
        return TakeSurfaceCapture(node, callback, captureConfig);
    }
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    RSSurfaceCaptureBlurParam blurParam;
    blurParam.isNeedBlur = true;
    blurParam.blurRadius = blurRadius;
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig, blurParam);
}

bool RSInterfaces::TakeSelfSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderServiceClient_->TakeSelfSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSInterfaces::SetWindowFreezeImmediately(std::shared_ptr<RSSurfaceNode> node, bool isFreeze,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig, float blurRadius)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    RSSurfaceCaptureBlurParam blurParam;
    if (ROSEN_GE(blurRadius, 1.f)) {
        blurParam.isNeedBlur = true;
        blurParam.blurRadius = blurRadius;
    }
    return renderServiceClient_->SetWindowFreezeImmediately(
        node->GetId(), isFreeze, callback, captureConfig, blurParam);
}

bool RSInterfaces::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    return renderServiceClient_->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
}

bool RSInterfaces::TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSInterfaces::TakeSurfaceCapture(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    return renderServiceClient_->TakeSurfaceCapture(id, callback, captureConfig);
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

void RSInterfaces::SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range,
    int32_t animatorExpectedFrameRate)
{
    renderServiceClient_->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
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

void RSInterfaces::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    return renderServiceClient_->SetShowRefreshRateEnabled(enabled, type);
}

uint32_t RSInterfaces::GetRealtimeRefreshRate(ScreenId id)
{
    RS_LOGD("GetRealtimeRefreshRate: screenId[%{public}" PRIu64"]", id);
    return renderServiceClient_->GetRealtimeRefreshRate(id);
}

std::string RSInterfaces::GetRefreshInfo(pid_t pid)
{
    return renderServiceClient_->GetRefreshInfo(pid);
}

std::string RSInterfaces::GetRefreshInfoToSP(NodeId id)
{
    return renderServiceClient_->GetRefreshInfoToSP(id);
}

bool RSInterfaces::TakeSurfaceCaptureForUI(std::shared_ptr<RSNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY,
    bool isSync, const Drawing::Rect& specifiedAreaRect)
{
    if (!node) {
        ROSEN_LOGW("RSInterfaces::TakeSurfaceCaptureForUI rsnode is nullpter return");
        return false;
    }
    // textureExportNode process cmds in renderThread of application, isSync is unnecessary.
    if (node->IsTextureExportNode()) {
        ROSEN_LOGD("RSInterfaces::TakeSurfaceCaptureForUI rsNode [%{public}" PRIu64
            "] is textureExportNode, set isSync false", node->GetId());
        isSync = false;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSInterfaces::TakeSurfaceCaptureForUI unsupported node type return");
        return false;
    }
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = scaleX;
    captureConfig.scaleY = scaleY;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = isSync;
    captureConfig.specifiedAreaRect = specifiedAreaRect;
    if (RSSystemProperties::GetUniRenderEnabled()) {
        if (isSync) {
            node->SetTakeSurfaceForUIFlag();
        }
        return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig, {}, specifiedAreaRect);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(node->GetId(), callback, scaleX, scaleY);
    }
}

bool RSInterfaces::TakeSurfaceCaptureForUIWithConfig(std::shared_ptr<RSNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig,
    const Drawing::Rect& specifiedAreaRect)
{
    if (!node) {
        ROSEN_LOGW("RSInterfaces::TakeSurfaceCaptureForUIWithConfig rsnode is nullpter return");
        return false;
    }
    // textureExportNode process cmds in renderThread of application, isSync is unnecessary.
    if (node->IsTextureExportNode()) {
        ROSEN_LOGD("RSInterfaces::TakeSurfaceCaptureForUI rsNode [%{public}" PRIu64
            "] is textureExportNode, set isSync false", node->GetId());
        captureConfig.isSync = false;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSInterfaces::TakeSurfaceCaptureForUIWithConfig unsupported node type return");
        return false;
    }
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    if (RSSystemProperties::GetUniRenderEnabled()) {
        if (captureConfig.isSync) {
            node->SetTakeSurfaceForUIFlag();
        }
        RSSurfaceCaptureBlurParam blurParam = {};
        return renderServiceClient_->TakeSurfaceCapture(node->GetId(), callback,
            captureConfig, blurParam, specifiedAreaRect);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(node->GetId(), callback, captureConfig.scaleX, captureConfig.scaleY);
    }
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
    RSInterfaces::TakeSurfaceCaptureSoloNodeList(std::shared_ptr<RSNode> node)
{
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    if (!node) {
        ROSEN_LOGW("RSInterfaces::TakeSurfaceCaptureSoloNodeList rsnode is nullpter return");
        return pixelMapIdPairVector;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSInterfaces::TakeSurfaceCaptureSoloNodeList unsupported node type return");
        return pixelMapIdPairVector;
    }
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isSoloNodeUiCapture = true;
    if (RSSystemProperties::GetUniRenderEnabled()) {
        pixelMapIdPairVector = renderServiceClient_->TakeSurfaceCaptureSoloNode(node->GetId(), captureConfig);
        return pixelMapIdPairVector;
    } else {
        ROSEN_LOGE("RSInterfaces::TakeSurfaceCaptureSoloNodeList UniRender is not enabled return");
        return pixelMapIdPairVector;
    }
}

bool RSInterfaces::TakeUICaptureInRange(std::shared_ptr<RSNode> beginNode, std::shared_ptr<RSNode> endNode,
    bool useBeginNodeSize, std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY, bool isSync)
{
    if (!beginNode) {
        ROSEN_LOGW("RSInterfaces::TakeUICaptureInRange beginNode is nullpter return");
        return false;
    }
    if (!endNode) {
        return TakeSurfaceCaptureForUI(beginNode, callback, scaleX, scaleY, isSync);
    }
    // textureExportNode process cmds in renderThread of application, isSync is unnecessary.
    if (beginNode->IsTextureExportNode()) {
        ROSEN_LOGD("RSInterfaces::TakeUICaptureInRange beginNode [%{public}" PRIu64
            "] is textureExportNode, set isSync false", beginNode->GetId());
        isSync = false;
    }
    if (!((beginNode->GetType() == RSUINodeType::ROOT_NODE) ||
          (beginNode->GetType() == RSUINodeType::CANVAS_NODE) ||
          (beginNode->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (beginNode->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSInterfaces::TakeUICaptureInRange unsupported node type return");
        return false;
    }
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = scaleX;
    captureConfig.scaleY = scaleY;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = isSync;
    captureConfig.uiCaptureInRangeParam.endNodeId = endNode->GetId();
    captureConfig.uiCaptureInRangeParam.useBeginNodeSize = useBeginNodeSize;
    if (RSSystemProperties::GetUniRenderEnabled()) {
        if (isSync) {
            beginNode->SetTakeSurfaceForUIFlag();
        }
        return renderServiceClient_->TakeUICaptureInRange(beginNode->GetId(), callback, captureConfig);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(beginNode->GetId(), callback, scaleX, scaleY);
    }
}

bool RSInterfaces::RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    static std::function<std::shared_ptr<Drawing::Typeface> (uint64_t)> customTypefaceQueryfunc =
        [](uint64_t globalUniqueId) -> std::shared_ptr<Drawing::Typeface> {
        return RSTypefaceCache::Instance().GetDrawingTypefaceCache(globalUniqueId);
    };

    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {
        Drawing::DrawOpItem::SetTypefaceQueryCallBack(customTypefaceQueryfunc);
    });

    if (RSSystemProperties::GetUniRenderEnabled()) {
        bool result = renderServiceClient_->RegisterTypeface(typeface);
        if (result) {
            RS_LOGI("RSInterfaces:Succeed in reg typeface, family name:%{public}s, uniqueid:%{public}u",
                typeface->GetFamilyName().c_str(), typeface->GetUniqueID());
            uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
            RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
        } else {
            RS_LOGE("RSInterfaces:Failed to reg typeface, family name:%{public}s, uniqueid:%{public}u",
                typeface->GetFamilyName().c_str(), typeface->GetUniqueID());
        }
        return result;
    }

    RS_LOGI("RSInterfaces:Succeed in reg typeface, family name:%{public}s, uniqueid:%{public}u",
        typeface->GetFamilyName().c_str(), typeface->GetUniqueID());
    uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
    RSTypefaceCache::Instance().CacheDrawingTypeface(globalUniqueId, typeface);
    return true;
}

bool RSInterfaces::UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    RS_LOGW("RSInterfaces:Unreg typeface: family name:%{public}s, uniqueid:%{public}u",
        typeface->GetFamilyName().c_str(), typeface->GetUniqueID());
    if (RSSystemProperties::GetUniRenderEnabled()) {
        bool result = renderServiceClient_->UnRegisterTypeface(typeface);
        if (result) {
            uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
            RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(globalUniqueId);
        }
        return result;
    }

    uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
    RSTypefaceCache::Instance().AddDelayDestroyQueue(globalUniqueId);
    return true;
}

bool RSInterfaces::SetGlobalDarkColorMode(bool isDark)
{
    return renderServiceClient_->SetGlobalDarkColorMode(isDark);
}

#ifndef ROSEN_ARKUI_X
int32_t RSInterfaces::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    RS_LOGI("RSInterfaces:%{public}s, screenId:%{public}" PRIu64 ", width:%{public}u, height:%{public}u", __func__, id,
            width, height);
    return renderServiceClient_->SetPhysicalScreenResolution(id, width, height);
}

int32_t RSInterfaces::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    return renderServiceClient_->SetVirtualScreenResolution(id, width, height);
}
#endif // !ROSEN_ARKUI_X

bool RSInterfaces::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    return renderServiceClient_->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

int32_t RSInterfaces::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    return renderServiceClient_->SetVirtualScreenAutoRotation(id, isAutoRotation);
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

void RSInterfaces::MarkPowerOffNeedProcessOneFrame()
{
    RS_LOGD("[UL_POWER]RSInterfaces::MarkPowerOffNeedProcessOneFrame.");
    renderServiceClient_->MarkPowerOffNeedProcessOneFrame();
}

void RSInterfaces::RepaintEverything()
{
    renderServiceClient_->RepaintEverything();
}

void RSInterfaces::ForceRefreshOneFrameWithNextVSync()
{
    renderServiceClient_->ForceRefreshOneFrameWithNextVSync();
}

void RSInterfaces::DisablePowerOffRenderControl(ScreenId id)
{
    RS_LOGD("RSInterfaces::DisablePowerOffRenderControl.");
    renderServiceClient_->DisablePowerOffRenderControl(id);
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
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper,
    NodeId windowNodeId,
    bool fromXcomponent)
{
    return renderServiceClient_->CreateVSyncReceiver(name, looper, id, windowNodeId, fromXcomponent);
}

std::shared_ptr<Media::PixelMap> RSInterfaces::CreatePixelMapFromSurfaceId(uint64_t surfaceId, const Rect &srcRect)
{
    return renderServiceClient_->CreatePixelMapFromSurfaceId(surfaceId, srcRect);
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

int32_t RSInterfaces::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData)
{
    return renderServiceClient_->GetDisplayIdentificationData(id, outPort, edidData);
}

int32_t RSInterfaces::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    return renderServiceClient_->SetScreenSkipFrameInterval(id, skipFrameInterval);
}

uint32_t RSInterfaces::SetScreenActiveRect(ScreenId id, const Rect& activeRect)
{
    return renderServiceClient_->SetScreenActiveRect(id, activeRect);
}

void RSInterfaces::SetScreenOffset(ScreenId id, int32_t offSetX, int32_t offSetY)
{
    return renderServiceClient_->SetScreenOffset(id, offSetX, offSetY);
}

void RSInterfaces::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    return renderServiceClient_->SetScreenFrameGravity(id, gravity);
}

int32_t RSInterfaces::SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    return renderServiceClient_->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
}

bool RSInterfaces::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    return renderServiceClient_->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation);
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

int32_t RSInterfaces::RegisterHgmRefreshRateUpdateCallback(const HgmRefreshRateUpdateCallback& callback)
{
    return renderServiceClient_->RegisterHgmRefreshRateUpdateCallback(callback);
}

int32_t RSInterfaces::UnRegisterHgmRefreshRateUpdateCallback()
{
    return renderServiceClient_->RegisterHgmRefreshRateUpdateCallback(nullptr);
}

int32_t RSInterfaces::RegisterFirstFrameCommitCallback(const FirstFrameCommitCallback& callback)
{
    return renderServiceClient_->RegisterFirstFrameCommitCallback(callback);
}

int32_t RSInterfaces::UnRegisterFirstFrameCommitCallback()
{
    return renderServiceClient_->RegisterFirstFrameCommitCallback(nullptr);
}

int32_t RSInterfaces::RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
    const FrameRateLinkerExpectedFpsUpdateCallback& callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSInterfaces::RegisterFrameRateLinkerExpectedFpsUpdateCallback callback == nullptr.");
        return INVALID_ARGUMENTS;
    }
    return renderServiceClient_->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback);
}

int32_t RSInterfaces::UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid)
{
    return renderServiceClient_->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, nullptr);
}

void RSInterfaces::SetAppWindowNum(uint32_t num)
{
    renderServiceClient_->SetAppWindowNum(num);
}

/**
 * @brief Display safe Watermark
 * @param watermarkImg, The image width and height are less than twice the screen size
 * @param isShow, flag indicating whether to display the watermark identifier(true) or hide it(false)
 */
void RSInterfaces::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    if (watermarkImg == nullptr) {
        ROSEN_LOGE("RSInterfaces::ShowWatermark watermarkImg is nullptr");
    }
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

void RSInterfaces::ReportRsSceneJankStart(AppInfo info)
{
    renderServiceClient_->ReportRsSceneJankStart(info);
}

void RSInterfaces::ReportRsSceneJankEnd(AppInfo info)
{
    renderServiceClient_->ReportRsSceneJankEnd(info);
}

void RSInterfaces::EnableCacheForRotation()
{
    renderServiceClient_->SetCacheEnabledForRotation(true);
}

void RSInterfaces::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    renderServiceClient_->NotifyLightFactorStatus(lightFactorStatus);
}

void RSInterfaces::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    renderServiceClient_->NotifyPackageEvent(listSize, packageList);
}

void RSInterfaces::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    renderServiceClient_->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
}

void RSInterfaces::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    renderServiceClient_->NotifyRefreshRateEvent(eventInfo);
}

void RSInterfaces::SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos)
{
    renderServiceClient_->SetWindowExpectedRefreshRate(eventInfos);
}

void RSInterfaces::SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos)
{
    renderServiceClient_->SetWindowExpectedRefreshRate(eventInfos);
}

bool RSInterfaces::NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name, uint32_t rateDiscount)
{
    return renderServiceClient_->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
}

void RSInterfaces::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    if (!RSFrameRatePolicy::GetInstance()->GetTouchOrPointerAction(touchStatus)) {
        return;
    }
    renderServiceClient_->NotifyTouchEvent(touchStatus, touchCnt);
}

void RSInterfaces::NotifyDynamicModeEvent(bool enableDynamicMode)
{
    renderServiceClient_->NotifyDynamicModeEvent(enableDynamicMode);
}

void RSInterfaces::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
    renderServiceClient_->NotifyHgmConfigEvent(eventName, state);
}

void RSInterfaces::NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate)
{
    renderServiceClient_->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
}

void RSInterfaces::DisableCacheForRotation()
{
    renderServiceClient_->SetCacheEnabledForRotation(false);
}

void RSInterfaces::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
    renderServiceClient_->SetOnRemoteDiedCallback(callback);
}

std::vector<ActiveDirtyRegionInfo> RSInterfaces::GetActiveDirtyRegionInfo() const
{
    const auto& activeDirtyRegionInfo = renderServiceClient_->GetActiveDirtyRegionInfo();
    return activeDirtyRegionInfo;
}

GlobalDirtyRegionInfo RSInterfaces::GetGlobalDirtyRegionInfo() const
{
    const auto& globalDirtyRegionInfo = renderServiceClient_->GetGlobalDirtyRegionInfo();
    return globalDirtyRegionInfo;
}

LayerComposeInfo RSInterfaces::GetLayerComposeInfo() const
{
    const auto& layerComposeInfo = renderServiceClient_->GetLayerComposeInfo();
    return layerComposeInfo;
}

HwcDisabledReasonInfos RSInterfaces::GetHwcDisabledReasonInfo() const
{
    const auto& hwcDisabledReasonInfo = renderServiceClient_->GetHwcDisabledReasonInfo();
    return hwcDisabledReasonInfo;
}

int64_t RSInterfaces::GetHdrOnDuration() const
{
    return renderServiceClient_->GetHdrOnDuration();
}

void RSInterfaces::SetVmaCacheStatus(bool flag)
{
    renderServiceClient_->SetVmaCacheStatus(flag);
}

#ifdef TP_FEATURE_ENABLE
void RSInterfaces::SetTpFeatureConfig(int32_t feature, const char* config, TpFeatureConfigType tpFeatureConfigType)
{
    renderServiceClient_->SetTpFeatureConfig(feature, config, tpFeatureConfigType);
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

void RSInterfaces::DropFrameByPid(const std::vector<int32_t> pidList)
{
    if (pidList.empty()) {
        return;
    }
    RS_TRACE_NAME("DropFrameByPid");
    renderServiceClient_->DropFrameByPid(pidList);
}

int32_t RSInterfaces::RegisterUIExtensionCallback(uint64_t userId, const UIExtensionCallback& callback, bool unobscured)
{
    return renderServiceClient_->RegisterUIExtensionCallback(userId, callback, unobscured);
}

bool RSInterfaces::SetAncoForceDoDirect(bool direct)
{
    return renderServiceClient_->SetAncoForceDoDirect(direct);
}

bool RSInterfaces::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    return renderServiceClient_->SetVirtualScreenStatus(id, screenStatus);
}

void RSInterfaces::SetFreeMultiWindowStatus(bool enable)
{
    renderServiceClient_->SetFreeMultiWindowStatus(enable);
}

bool RSInterfaces::RegisterTransactionDataCallback(uint64_t token, uint64_t timeStamp, std::function<void()> callback)
{
    RS_LOGD("interface::RegisterTransactionDataCallback, timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    return renderServiceClient_->RegisterTransactionDataCallback(token, timeStamp, callback);
}

bool RSInterfaces::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    std::shared_ptr<SurfaceBufferCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSInterfaces::RegisterSurfaceBufferCallback callback == nullptr.");
        return false;
    }
    RSSurfaceBufferCallbackManager::Instance().RegisterSurfaceBufferCallback(pid, uid,
        new (std::nothrow) RSDefaultSurfaceBufferCallback ({
            .OnFinish = [callback](const FinishCallbackRet& ret) {
                callback->OnFinish(ret);
            },
            .OnAfterAcquireBuffer = [callback](const AfterAcquireBufferRet& ret) {
                callback->OnAfterAcquireBuffer(ret);
            },
        })
    );
    return renderServiceClient_->RegisterSurfaceBufferCallback(pid, uid, callback);
}

bool RSInterfaces::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid, uid);
    return renderServiceClient_->UnregisterSurfaceBufferCallback(pid, uid);
}

void RSInterfaces::SetLayerTopForHWC(const std::string &nodeIdStr, bool isTop, uint32_t zOrder)
{
    renderServiceClient_->SetLayerTopForHWC(nodeIdStr, isTop, zOrder);
}

void RSInterfaces::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    renderServiceClient_->SetLayerTop(nodeIdStr, isTop);
}

void RSInterfaces::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    renderServiceClient_->SetForceRefresh(nodeIdStr, isForceRefresh);
}

void RSInterfaces::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    renderServiceClient_->SetColorFollow(nodeIdStr, isColorFollow);
}

void RSInterfaces::NotifyScreenSwitched()
{
    RS_TRACE_NAME("NotifyScreenSwitched");
    ROSEN_LOGI("RSInterfaces::%{public}s", __func__);
    renderServiceClient_->NotifyScreenSwitched();
}

void RSInterfaces::SetWindowContainer(NodeId nodeId, bool value)
{
    renderServiceClient_->SetWindowContainer(nodeId, value);
}

int32_t RSInterfaces::RegisterSelfDrawingNodeRectChangeCallback(
    const RectConstraint& constraint, const SelfDrawingNodeRectChangeCallback& callback)
{
    RS_LOGD("RSInterfaces::RegisterSelfDrawingNodeRectChangeCallback lowLimit_width: %{public}d lowLimit_height: "
            "%{public}d highLimit_width: %{public}d highLimit_height: %{public}d",
            constraint.range.lowLimit.width, constraint.range.lowLimit.height, constraint.range.highLimit.width,
            constraint.range.highLimit.height);
    return renderServiceClient_->RegisterSelfDrawingNodeRectChangeCallback(constraint, callback);
}

int32_t RSInterfaces::UnRegisterSelfDrawingNodeRectChangeCallback()
{
    return renderServiceClient_->UnRegisterSelfDrawingNodeRectChangeCallback();
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
int32_t RSInterfaces::SetOverlayDisplayMode(int32_t mode)
{
    ROSEN_LOGI("RSInterfaces::SetOverlayDisplayMode enter.");
    return renderServiceClient_->SetOverlayDisplayMode(mode);
}
#endif

void RSInterfaces::NotifyPageName(const std::string &packageName, const std::string &pageName, bool isEnter)
{
    auto pageNameList = RSFrameRatePolicy::GetInstance()->GetPageNameList();
    auto item = pageNameList.find(pageName);
    if (item != pageNameList.end()) {
        ROSEN_LOGI("RSInterfaces packageName = %{public}s pageName = %{public}s isEnter = %{public}d",
            packageName.c_str(), pageName.c_str(), isEnter);
        renderServiceClient_->NotifyPageName(packageName, pageName, isEnter);
    }
}

int32_t RSInterfaces::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    auto ret = renderServiceClient_->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    ROSEN_LOGD("RSInterfaces::GetpidGpuMemoryInMB called!");
    return ret;
}

bool RSInterfaces::GetHighContrastTextState()
{
    return renderServiceClient_->GetHighContrastTextState();
}

bool RSInterfaces::SetBehindWindowFilterEnabled(bool enabled)
{
    return renderServiceClient_->SetBehindWindowFilterEnabled(enabled);
}

bool RSInterfaces::GetBehindWindowFilterEnabled(bool& enabled)
{
    return renderServiceClient_->GetBehindWindowFilterEnabled(enabled);
}
} // namespace Rosen
} // namespace OHOS
