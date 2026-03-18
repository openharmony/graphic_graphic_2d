/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_render_interface.h"

#include <cstdint>
#include <functional>
#include <iremote_stub.h>

#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "feature/ui_capture/rs_divided_ui_capture.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "rs_interfaces.h"
#include "rs_trace.h"

#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "ipc_callbacks/rs_isurface_occlusion_change_callback.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_typeface_cache.h"
#include "ui/rs_proxy_node.h"

namespace OHOS {
namespace Rosen {
#ifdef ROSEN_OHOS
constexpr uint32_t WATERMARK_NAME_LENGTH_LIMIT = 128;
#endif

RSRenderInterface::RSRenderInterface() : renderPipelineClient_(std::make_unique<RSRenderPipelineClient>()) {}

RSRenderInterface::~RSRenderInterface() noexcept {}

int32_t RSRenderInterface::GetMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_DISABLED) {
        RS_LOGI("GetMaxGpuBufferSize: non-uni render mode, PostSyncTask to render thread");
        bool querySuccess = false;
        auto queryGpuLimits = [&maxWidth, &maxHeight, &querySuccess]() {
            querySuccess = RSRenderThread::Instance().QueryMaxGpuBufferSize(maxWidth, maxHeight);
        };
        RSRenderThread::Instance().PostSyncTask(queryGpuLimits);
        return querySuccess ? 0 : -1;
    } else {
        RS_LOGI("GetMaxGpuBufferSize: uni render mode, using render pipeline client");
        return renderPipelineClient_->GetMaxGpuBufferSize(maxWidth, maxHeight);
    }
}

bool RSRenderInterface::TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPipelineClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCaptureWithBlur(std::shared_ptr<RSSurfaceNode> node,
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
    return renderPipelineClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig, blurParam);
}

bool RSRenderInterface::TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPipelineClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCapture(
    NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    return renderPipelineClient_->TakeSurfaceCapture(id, callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCaptureForUI(std::shared_ptr<RSNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY, bool isSync,
    const Drawing::Rect& specifiedAreaRect)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = scaleX;
    captureConfig.scaleY = scaleY;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = isSync;
    captureConfig.specifiedAreaRect = specifiedAreaRect;
    return TakeSurfaceCaptureForUIWithConfig(node, callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCaptureForUIWithConfig(std::shared_ptr<RSNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGW("RSRenderInterface::TakeSurfaceCaptureForUIWithConfig rsnode is nullpter return");
        return false;
    }
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    // textureExportNode process cmds in renderThread of application, isSync is unnecessary.
    if (node->IsTextureExportNode()) {
        ROSEN_LOGD("RSRenderInterface::TakeSurfaceCaptureForUI rsNode [%{public}" PRIu64
                   "] is textureExportNode, set isSync false",
            node->GetId());
        captureConfig.isSync = false;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) || (node->GetType() == RSUINodeType::CANVAS_NODE) ||
            (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
            (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSRenderInterface::TakeSurfaceCaptureForUI unsupported node type return");
        return false;
    }
    if (RSSystemProperties::GetUniRenderEnabled()) {
        if (captureConfig.isSync) {
            node->SetTakeSurfaceForUIFlag();
        }
        return renderPipelineClient_->TakeSurfaceCapture(
            node->GetId(), callback, captureConfig, {}, captureConfig.specifiedAreaRect);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(node->GetId(), callback, captureConfig.scaleX, captureConfig.scaleY);
    }
}

bool RSRenderInterface::TakeSurfaceCaptureForUIWithoutUni(
    NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    std::function<void()> offscreenRenderTask = [scaleX, scaleY, callback, id, this]() -> void {
        ROSEN_LOGD("RSRenderInterface::TakeSurfaceCaptureForUIWithoutUni callback->OnOffscreenRender nodeId:"
                   "[%{public}" PRIu64 "]",
            id);
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::TakeSurfaceCaptureForUIWithoutUni");
        std::shared_ptr<RSDividedUICapture> rsDividedUICapture =
            std::make_shared<RSDividedUICapture>(id, scaleX, scaleY);
        std::shared_ptr<Media::PixelMap> pixelmap = rsDividedUICapture->TakeLocalCapture();
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        callback->OnSurfaceCapture(pixelmap);
    };
    RSRenderThread::Instance().PostTask(offscreenRenderTask);
    return true;
}

bool RSRenderInterface::TakeUICaptureInRange(std::shared_ptr<RSNode> beginNode, std::shared_ptr<RSNode> endNode,
    bool useBeginNodeSize, std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY, bool isSync)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = scaleX;
    captureConfig.scaleY = scaleY;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    captureConfig.isSync = isSync;
    if (endNode) {
        captureConfig.uiCaptureInRangeParam.endNodeId = endNode->GetId();
    }
    captureConfig.uiCaptureInRangeParam.useBeginNodeSize = useBeginNodeSize;
    return TakeUICaptureInRangeWithConfig(beginNode, endNode, useBeginNodeSize, callback, captureConfig);
}

bool RSRenderInterface::TakeUICaptureInRangeWithConfig(std::shared_ptr<RSNode> beginNode,
    std::shared_ptr<RSNode> endNode, bool useBeginNodeSize, std::shared_ptr<SurfaceCaptureCallback> callback,
    RSSurfaceCaptureConfig captureConfig)
{
    if (!beginNode) {
        ROSEN_LOGW("RSRenderInterface::TakeUICaptureInRangeWithConfig beginNode is nullpter return");
        return false;
    }
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    if (!endNode) {
        return TakeSurfaceCaptureForUI(
            beginNode, callback, captureConfig.scaleX, captureConfig.scaleY, captureConfig.isSync);
    }
    captureConfig.uiCaptureInRangeParam.endNodeId = endNode->GetId();
    captureConfig.uiCaptureInRangeParam.useBeginNodeSize = useBeginNodeSize;
    if (beginNode->IsTextureExportNode()) {
        ROSEN_LOGD("RSRenderInterface::TakeUICaptureInRange beginNode [%{public}" PRIu64
                   "] is textureExportNode, set isSync false",
            beginNode->GetId());
        captureConfig.isSync = false;
    }
    if (!((beginNode->GetType() == RSUINodeType::ROOT_NODE) || (beginNode->GetType() == RSUINodeType::CANVAS_NODE) ||
            (beginNode->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
            (beginNode->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSRenderInterface::TakeUICaptureInRange unsupported node type return");
        return false;
    }
    if (RSSystemProperties::GetUniRenderEnabled()) {
        if (captureConfig.isSync) {
            beginNode->SetTakeSurfaceForUIFlag();
        }
        return renderPipelineClient_->TakeUICaptureInRange(beginNode->GetId(), callback, captureConfig);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(
            beginNode->GetId(), callback, captureConfig.scaleX, captureConfig.scaleY);
    }
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderInterface::TakeSurfaceCaptureSoloNodeList(
    std::shared_ptr<RSNode> node)
{
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    if (!node) {
        ROSEN_LOGW("RSRenderInterface::TakeSurfaceCaptureSoloNodeList rsnode is nullpter return");
        return pixelMapIdPairVector;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) || (node->GetType() == RSUINodeType::CANVAS_NODE) ||
            (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
            (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSRenderInterface::TakeSurfaceCaptureSoloNodeList unsupported node type return");
        return pixelMapIdPairVector;
    }
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isSoloNodeUiCapture = true;
    if (RSSystemProperties::GetUniRenderEnabled()) {
        pixelMapIdPairVector = renderPipelineClient_->TakeSurfaceCaptureSoloNode(node->GetId(), captureConfig);
        return pixelMapIdPairVector;
    } else {
        ROSEN_LOGE("RSRenderInterface::TakeSurfaceCaptureSoloNodeList UniRender is not enabled return");
        return pixelMapIdPairVector;
    }
}

bool RSRenderInterface::TakeSelfSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPipelineClient_->TakeSelfSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCaptureWithAllWindows(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig, bool checkDrmAndSurfaceLock)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPipelineClient_->TakeSurfaceCaptureWithAllWindows(
        node->GetId(), callback, captureConfig, checkDrmAndSurfaceLock);
}

bool RSRenderInterface::FreezeScreen(std::shared_ptr<RSDisplayNode> node, bool isFreeze, bool needSync)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPipelineClient_->FreezeScreen(node->GetId(), isFreeze, needSync);
}

bool RSRenderInterface::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, std::shared_ptr<SurfaceBufferCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderInterface::RegisterSurfaceBufferCallback callback == nullptr.");
        return false;
    }
    RSSurfaceBufferCallbackManager::Instance().RegisterSurfaceBufferCallback(pid, uid,
        new (std::nothrow) RSDefaultSurfaceBufferCallback({
            .OnFinish = [callback](const FinishCallbackRet& ret) { callback->OnFinish(ret); },
            .OnAfterAcquireBuffer = [callback](
                                        const AfterAcquireBufferRet& ret) { callback->OnAfterAcquireBuffer(ret); },
        }));
    return renderPipelineClient_->RegisterSurfaceBufferCallback(pid, uid, callback);
}

bool RSRenderInterface::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid, uid);
    return renderPipelineClient_->UnregisterSurfaceBufferCallback(pid, uid);
}

bool RSRenderInterface::RegisterTransactionDataCallback(
    uint64_t token, uint64_t timeStamp, std::function<void()> callback)
{
    RS_LOGD("RSRenderInterface::RegisterTransactionDataCallback, timeStamp: %{public}" PRIu64
            " token: %{public}" PRIu64,
        timeStamp, token);
    return renderPipelineClient_->RegisterTransactionDataCallback(token, timeStamp, callback);
}

void RSRenderInterface::ClearUifirstCache(NodeId id)
{
    if (renderPipelineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::ClearUifirstCache renderPipelineClient_ nullptr");
        return;
    }
    renderPipelineClient_->ClearUifirstCache(id);
}

void RSRenderInterface::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    if (renderPipelineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::SetLayerTopForHWC nullptr");
        return;
    }
    renderPipelineClient_->SetLayerTopForHWC(nodeId, isTop, zOrder);
}

int32_t RSRenderInterface::SetFocusAppInfo(const FocusAppInfo& info)
{
    if (renderPipelineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::SetFocusAppInfo renderPipelineClient_ nullptr");
        return ERR_INVALID_VALUE;
    }
    return renderPipelineClient_->SetFocusAppInfo(info);
}

bool RSRenderInterface::SetAncoForceDoDirect(bool direct)
{
    if (renderPipelineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::SetAncoForceDoDirect renderPipelineClient_ nullptr");
        return false;
    }
    return renderPipelineClient_->SetAncoForceDoDirect(direct);
}

bool RSRenderInterface::SetHwcNodeBounds(
    int64_t rsNodeId, float positionX, float positionY, float positionZ, float positionW)
{
    if (renderPipelineClient_ == nullptr) {
        RS_LOGE("RSRenderInterface::SetHwcNodeBounds renderPipelineClient_ is null!");
        return false;
    }
    renderPipelineClient_->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

bool RSRenderInterface::SetWindowFreezeImmediately(std::shared_ptr<RSSurfaceNode> node, bool isFreeze,
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
    return renderPipelineClient_->SetWindowFreezeImmediately(
        node->GetId(), isFreeze, callback, captureConfig, blurParam);
}

void RSRenderInterface::DropFrameByPid(const std::vector<int32_t>& pidList, int32_t dropFrameLevel)
{
    if (pidList.empty()) {
        return;
    }
    RS_TRACE_NAME("DropFrameByPid");
    renderPipelineClient_->DropFrameByPid(pidList, dropFrameLevel);
}

void RSRenderInterface::SetWindowContainer(NodeId nodeId, bool value)
{
    renderPipelineClient_->SetWindowContainer(nodeId, value);
}

int32_t RSRenderInterface::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    return renderPipelineClient_->GetBrightnessInfo(screenId, brightnessInfo);
}

int32_t RSRenderInterface::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus)
{
    return renderPipelineClient_->GetScreenHDRStatus(id, hdrStatus);
}

bool RSRenderInterface::GetHighContrastTextState()
{
    return renderPipelineClient_->GetHighContrastTextState();
}

bool RSRenderInterface::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    return renderPipelineClient_->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation);
}

bool RSRenderInterface::SetGlobalDarkColorMode(bool isDark)
{
    return renderPipelineClient_->SetGlobalDarkColorMode(isDark);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSRenderInterface::RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback)
{
    // Canvas callback memory attribution only works in unified render mode
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
    renderPipelineClient_->RegisterCanvasCallback(callback);
}

int32_t RSRenderInterface::SubmitCanvasPreAllocatedBuffer(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    // Canvas pre-allocated buffer only works in unified render mode
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return INVALID_ARGUMENTS;
    }
    return renderPipelineClient_->SubmitCanvasPreAllocatedBuffer(nodeId, buffer, resetSurfaceIndex);
}
#endif

uint32_t RSRenderInterface::SetSurfaceWatermark(pid_t pid, const std::string& name,
    const std::shared_ptr<Media::PixelMap>& watermark, const std::vector<NodeId>& nodeIdList,
    SurfaceWatermarkType watermarkType)
{
#ifdef ROSEN_OHOS
    if (name.length() > WATERMARK_NAME_LENGTH_LIMIT || name.empty()) {
        ROSEN_LOGE("SetSurfaceWatermark failed, name[%{public}s] is error.", name.c_str());
        return SurfaceWatermarkStatusCode::WATER_MARK_NAME_ERROR;
    }
    if (watermark && watermark->IsAstc()) {
        ROSEN_LOGE("SetSurfaceWatermark failed, watermark[%{public}d, %{public}u] is error", watermark->IsAstc(),
            watermark->GetCapacity());
        return SurfaceWatermarkStatusCode::WATER_MARK_IMG_ASTC_ERROR;
    }

    if (watermarkType >= SurfaceWatermarkType::INVALID_WATER_MARK) {
        return SurfaceWatermarkStatusCode::WATER_MARK_INVALID_WATERMARK_TYPE;
    }
    return renderPipelineClient_->SetSurfaceWatermark(pid, name, watermark, nodeIdList, watermarkType);
#else
    return 0;
#endif
}

void RSRenderInterface::ClearSurfaceWatermarkForNodes(
    pid_t pid, const std::string& name, const std::vector<NodeId>& nodeIdList)
{
#ifdef ROSEN_OHOS
    if (name.length() > WATERMARK_NAME_LENGTH_LIMIT || name.empty()) {
        ROSEN_LOGE("ClearSurfaceWatermarkForNodes failed, name[%{public}s] is error.", name.c_str());
        return;
    }
    return renderPipelineClient_->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList);
#endif
}

int32_t RSRenderInterface::GetMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_DISABLED) {
        RS_LOGI("GetMaxGpuBufferSize: non-uni render mode, PostSyncTask to render thread");
        bool querySuccess = false;
        auto queryGpuLimits = [&maxWidth, &maxHeight, &querySuccess]() {
            querySuccess = RSRenderThread::Instance().QueryMaxGpuBufferSize(maxWidth, maxHeight);
        };
        RSRenderThread::Instance().PostSyncTask(queryGpuLimits);
        return querySuccess ? 0 : -1;
    } else {
        RS_LOGI("GetMaxGpuBufferSize: uni render mode, using render pipeline client");
        return renderPiplineClient_->GetMaxGpuBufferSize(maxWidth, maxHeight);
    }
}

void RSRenderInterface::ClearSurfaceWatermark(pid_t pid, const std::string& name)
{
#ifdef ROSEN_OHOS
    if (name.length() > WATERMARK_NAME_LENGTH_LIMIT || name.empty()) {
        ROSEN_LOGE("ClearSurfaceWatermark failed, name[%{public}s] is error.", name.c_str());
        return;
    }
    return renderPipelineClient_->ClearSurfaceWatermark(pid, name);
#endif
}

int32_t RSRenderInterface::RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    return renderPipelineClient_->RegisterOcclusionChangeCallback(callback);
}

int32_t RSRenderInterface::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints)
{
    return renderPipelineClient_->RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
}

int32_t RSRenderInterface::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    return renderPipelineClient_->UnRegisterSurfaceOcclusionChangeCallback(id);
}

int32_t RSRenderInterface::SetLogicalCameraRotationCorrection(ScreenId id, ScreenRotation logicalCorrection)
{
    return renderPipelineClient_->SetLogicalCameraRotationCorrection(id, logicalCorrection);
}
} // namespace Rosen
} // namespace OHOS
} // namespace OHOS