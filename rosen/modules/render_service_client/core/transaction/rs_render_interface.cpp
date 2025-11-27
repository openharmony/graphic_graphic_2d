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

#include <iremote_stub.h>

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
#include "pipeline/rs_render_thread.h"
#include "ui/rs_proxy_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_typeface_cache.h"

namespace OHOS {
namespace Rosen {

RSRenderInterface &RSRenderInterface::GetInstance()
{
    static RSRenderInterface instance;
    return instance;
}

RSRenderInterface::RSRenderInterface() : renderPiplineClient_(std::make_unique<RSRenderPipelineClient>())
{
}

RSRenderInterface::~RSRenderInterface() noexcept
{
}

bool RSRenderInterface::TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPiplineClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig);
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
    return renderPiplineClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig, blurParam);
}

bool RSRenderInterface::TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPiplineClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCapture(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig)
{
    return renderPiplineClient_->TakeSurfaceCapture(id, callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCaptureForUI(std::shared_ptr<RSNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY,
    bool isSync, const Drawing::Rect& specifiedAreaRect)
{
    if (!node) {
        ROSEN_LOGW("RSRenderInterface::TakeSurfaceCaptureForUI rsnode is nullpter return");
        return false;
    }
    // textureExportNode process cmds in renderThread of application, isSync is unnecessary.
    if (node->IsTextureExportNode()) {
        ROSEN_LOGD("RSRenderInterface::TakeSurfaceCaptureForUI rsNode [%{public}" PRIu64
            "] is textureExportNode, set isSync false", node->GetId());
        isSync = false;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSRenderInterface::TakeSurfaceCaptureForUI unsupported node type return");
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
        return renderPiplineClient_->TakeSurfaceCapture(node->GetId(), callback, captureConfig, {}, specifiedAreaRect);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(node->GetId(), callback, scaleX, scaleY);
    }
}

bool RSRenderInterface::TakeSurfaceCaptureForUIWithoutUni(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY)
{
    std::function<void()> offscreenRenderTask = [scaleX, scaleY, callback, id, this]() -> void {
        ROSEN_LOGD(
            "RSRenderInterface::TakeSurfaceCaptureForUIWithoutUni callback->OnOffscreenRender nodeId:"
            "[%{public}" PRIu64 "]", id);
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
    if (!beginNode) {
        ROSEN_LOGW("RSRenderInterface::TakeUICaptureInRange beginNode is nullpter return");
        return false;
    }
    if (!endNode) {
        return TakeSurfaceCaptureForUI(beginNode, callback, scaleX, scaleY, isSync);
    }
    // textureExportNode process cmds in renderThread of application, isSync is unnecessary.
    if (beginNode->IsTextureExportNode()) {
        ROSEN_LOGD("RSRenderInterface::TakeUICaptureInRange beginNode [%{public}" PRIu64
            "] is textureExportNode, set isSync false", beginNode->GetId());
        isSync = false;
    }
    if (!((beginNode->GetType() == RSUINodeType::ROOT_NODE) ||
          (beginNode->GetType() == RSUINodeType::CANVAS_NODE) ||
          (beginNode->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (beginNode->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSRenderInterface::TakeUICaptureInRange unsupported node type return");
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
        return renderPiplineClient_->TakeUICaptureInRange(beginNode->GetId(), callback, captureConfig);
    } else {
        return TakeSurfaceCaptureForUIWithoutUni(beginNode->GetId(), callback, scaleX, scaleY);
    }
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
    RSRenderInterface::TakeSurfaceCaptureSoloNodeList(std::shared_ptr<RSNode> node)
{
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    if (!node) {
        ROSEN_LOGW("RSRenderInterface::TakeSurfaceCaptureSoloNodeList rsnode is nullpter return");
        return pixelMapIdPairVector;
    }
    if (!((node->GetType() == RSUINodeType::ROOT_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_NODE) ||
          (node->GetType() == RSUINodeType::CANVAS_DRAWING_NODE) ||
          (node->GetType() == RSUINodeType::SURFACE_NODE))) {
        ROSEN_LOGE("RSRenderInterface::TakeSurfaceCaptureSoloNodeList unsupported node type return");
        return pixelMapIdPairVector;
    }
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isSoloNodeUiCapture = true;
    if (RSSystemProperties::GetUniRenderEnabled()) {
        pixelMapIdPairVector = renderPiplineClient_->TakeSurfaceCaptureSoloNode(node->GetId(), captureConfig);
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
    return renderPiplineClient_->TakeSelfSurfaceCapture(node->GetId(), callback, captureConfig);
}

bool RSRenderInterface::TakeSurfaceCaptureWithAllWindows(std::shared_ptr<RSDisplayNode> node,
    std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig,
    bool checkDrmAndSurfaceLock)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPiplineClient_->TakeSurfaceCaptureWithAllWindows(
        node->GetId(), callback, captureConfig, checkDrmAndSurfaceLock);
}

bool RSRenderInterface::FreezeScreen(std::shared_ptr<RSDisplayNode> node, bool isFreeze)
{
    if (!node) {
        ROSEN_LOGE("%{public}s node is nullptr", __func__);
        return false;
    }
    return renderPiplineClient_->FreezeScreen(node->GetId(), isFreeze);
}

bool RSRenderInterface::RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
    std::shared_ptr<SurfaceBufferCallback> callback)
{
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderInterface::RegisterSurfaceBufferCallback callback == nullptr.");
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
    return renderPiplineClient_->RegisterSurfaceBufferCallback(pid, uid, callback);
}

bool RSRenderInterface::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    RSSurfaceBufferCallbackManager::Instance().UnregisterSurfaceBufferCallback(pid, uid);
    return renderPiplineClient_->UnregisterSurfaceBufferCallback(pid, uid);
}

bool RSRenderInterface::RegisterTransactionDataCallback(
    uint64_t token, uint64_t timeStamp, std::function<void()> callback)
{
    RS_LOGD("RSRenderInterface::RegisterTransactionDataCallback, timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    return renderPiplineClient_->RegisterTransactionDataCallback(token, timeStamp, callback);
}

void RSRenderInterface::ClearUifirstCache(NodeId id)
{
    if (renderPiplineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::ClearUifirstCache renderPiplineClient_ nullptr");
        return;
    }
    renderPiplineClient_->ClearUifirstCache(id);
}

void RSRenderInterface::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    if (renderPiplineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::SetLayerTopForHWC nullptr");
        return;
    }
    renderPiplineClient_->SetLayerTopForHWC(nodeId, isTop, zOrder);
}

int32_t RSRenderInterface::SetFocusAppInfo(const FocusAppInfo& info)
{
    if (renderPiplineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::SetFocusAppInfo renderPiplineClient_ nullptr");
        return ERR_INVALID_VALUE;
    }
    return renderPiplineClient_->SetFocusAppInfo(info);
}

bool RSRenderInterface::SetAncoForceDoDirect(bool direct)
{
    if (renderPiplineClient_ == nullptr) {
        ROSEN_LOGE("RSRenderInterface::SetAncoForceDoDirect renderPiplineClient_ nullptr");
        return false;
    }
    return renderPiplineClient_->SetAncoForceDoDirect(direct);
}

bool RSRenderInterface::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    if (renderPiplineClient_ == nullptr) {
        RS_LOGE("RSRenderInterface::SetHwcNodeBounds renderPiplineClient_ is null!");
        return false;
    }
    renderPiplineClient_->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
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
    return renderPiplineClient_->SetWindowFreezeImmediately(
        node->GetId(), isFreeze, callback, captureConfig, blurParam);
}

void RSRenderInterface::DropFrameByPid(const std::vector<int32_t> pidList)
{
    if (pidList.empty()) {
        return;
    }
    RS_TRACE_NAME("DropFrameByPid");
    renderPiplineClient_->DropFrameByPid(pidList);
}

void RSRenderInterface::SetWindowContainer(NodeId nodeId, bool value)
{
    renderPiplineClient_->SetWindowContainer(nodeId, value);
}

int32_t RSRenderInterface::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus)
{
    return renderPiplineClient_->GetScreenHDRStatus(id, hdrStatus);
}

void RSRenderInterface::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    return renderPiplineClient_->SetScreenFrameGravity(id, gravity);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSRenderInterface::RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback)
{
    // Canvas callback memory attribution only works in unified render mode
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return;
    }
    renderPiplineClient_->RegisterCanvasCallback(callback);
}

int32_t RSRenderInterface::SubmitCanvasPreAllocatedBuffer(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    // Canvas pre-allocated buffer only works in unified render mode
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        return INVALID_ARGUMENTS;
    }
    return renderPiplineClient_->SubmitCanvasPreAllocatedBuffer(nodeId, buffer, resetSurfaceIndex);
}
#endif
}
}