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

#include "transaction/rs_render_pipeline_client.h"

#include <iremote_stub.h>
#include "rs_render_service_connect_hub.h"
#include "surface_type.h"
#include "rs_trace.h"
#include "surface_utils.h"
#ifdef RS_ENABLE_GL
#include "backend/rs_surface_ohos_gl.h"
#endif
#include "backend/rs_surface_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "backend/rs_surface_ohos_vulkan.h"
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "display_engine/rs_vpe_manager.h"
#endif
#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "common/rs_xcollie.h"
#include "ipc_callbacks/brightness_info_change_callback_stub.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif
#include "ipc_callbacks/rs_surface_occlusion_change_callback_stub.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/screen_switching_notify_callback_stub.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#include "ipc_callbacks/buffer_available_callback_stub.h"
#include "ipc_callbacks/buffer_clear_callback_stub.h"
#include "ipc_callbacks/hgm_config_change_callback_stub.h"
#include "ipc_callbacks/rs_first_frame_commit_callback_stub.h"
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "ipc_callbacks/rs_self_drawing_node_rect_change_callback_stub.h"
#include "ipc_callbacks/rs_surface_buffer_callback_stub.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "ipc_callbacks/rs_uiextension_callback_stub.h"
#include "platform/common/rs_log.h"
#include "render/rs_typeface_cache.h"
#include "rs_render_service_connect_hub.h"
#include "rs_surface_ohos.h"
#include "vsync_iconnection_token.h"

namespace OHOS {
namespace Rosen {


void RSRenderPipelineClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
}

void RSRenderPipelineClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
}

void RSRenderPipelineClient::TriggerSurfaceCaptureCallback(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
    std::shared_ptr<Media::PixelMap> pixelmap, CaptureError captureErrorCode,
    std::shared_ptr<Media::PixelMap> pixelmapHDR)
{
    ROSEN_LOGD("RSRenderPipelineClient::Into TriggerSurfaceCaptureCallback nodeId:[%{public}" PRIu64 "]", id);
    std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            callbackVector = iter->second;
            surfaceCaptureCbMap_.erase(iter);
        }
    }
    if (callbackVector.empty()) {
        ROSEN_LOGE("RSRenderPipelineClient::TriggerSurfaceCaptureCallback: callbackVector is empty!");
        return;
    }
    for (decltype(callbackVector.size()) i = 0; i < callbackVector.size(); ++i) {
        if (callbackVector[i] == nullptr) {
            ROSEN_LOGE("RSRenderPipelineClient::TriggerSurfaceCaptureCallback: callback is nullptr!");
            continue;
        }
        std::shared_ptr<Media::PixelMap> surfaceCapture = pixelmap;
        std::shared_ptr<Media::PixelMap> surfaceCaptureHDR = pixelmapHDR;
        if (i != callbackVector.size() - 1) {
            if (pixelmap != nullptr) {
                Media::InitializationOptions options;
                std::unique_ptr<Media::PixelMap> pixelmapCopy = Media::PixelMap::Create(*pixelmap, options);
                surfaceCapture = std::move(pixelmapCopy);
            }
            if (pixelmapHDR != nullptr) {
                Media::InitializationOptions options;
                std::unique_ptr<Media::PixelMap> pixelmapCopyHDR = Media::PixelMap::Create(*pixelmapHDR, options);
                surfaceCaptureHDR = std::move(pixelmapCopyHDR);
            }
        }
        if (surfaceCapture) {
            surfaceCapture->SetMemoryName("RSSurfaceCaptureForCallback");
        }
        if (surfaceCaptureHDR) {
            surfaceCaptureHDR->SetMemoryName("RSSurfaceCaptureForCallbackHDR");
        }
        if (captureConfig.needErrorCode) {
            callbackVector[i]->OnSurfaceCaptureWithErrorCode(surfaceCapture, surfaceCaptureHDR, captureErrorCode);
        } else if (captureConfig.isHdrCapture) {
            callbackVector[i]->OnSurfaceCaptureHDR(surfaceCapture, surfaceCaptureHDR);
        } else {
            callbackVector[i]->OnSurfaceCapture(surfaceCapture);
        }
    }
}

class SurfaceCaptureCallbackDirector : public RSSurfaceCaptureCallbackStub {
public:
    explicit SurfaceCaptureCallbackDirector(RSRenderPipelineClient* client) : client_(client) {}
    ~SurfaceCaptureCallbackDirector() override {};
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig, Media::PixelMap* pixelmap,
        CaptureError captureErrorCode = CaptureError::CAPTURE_OK, Media::PixelMap* pixelmapHDR = nullptr) override
    {
        std::shared_ptr<Media::PixelMap> surfaceCapture(pixelmap);
        std::shared_ptr<Media::PixelMap> surfaceCaptureHDR(pixelmapHDR);
        client_->TriggerSurfaceCaptureCallback(id, captureConfig, surfaceCapture, captureErrorCode, surfaceCaptureHDR);
    };

private:
    RSRenderPipelineClient* client_;
};

bool RSRenderPipelineClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSurfaceCapture renderPipleine == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSurfaceCapture callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderPipelineClient::TakeSurfaceCapture surfaceCaptureCbMap_.count(id) != 0");
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = {callback};
        surfaceCaptureCbMap_.emplace(std::make_pair(id, captureConfig), callbackVector);
    }

    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderPipleine->TakeSurfaceCapture(id, surfaceCaptureCbDirector_, captureConfig, blurParam, specifiedAreaRect);
    return true;
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderPipelineClient::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSurfaceCaptureSoloNode renderPipleine == nullptr!");
        return pixelMapIdPairVector;
    }
    pixelMapIdPairVector = renderPipleine->TakeSurfaceCaptureSoloNode(id, captureConfig);
    return pixelMapIdPairVector;
}

bool RSRenderPipelineClient::TakeSelfSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSelfSurfaceCapture renderPipleine == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSelfSurfaceCapture callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderPipelineClient::TakeSelfSurfaceCapture surfaceCaptureCbMap_.count(id) != 0");
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = {callback};
        surfaceCaptureCbMap_.emplace(std::make_pair(id, captureConfig), callbackVector);
    }

    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderPipleine->TakeSelfSurfaceCapture(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderPipelineClient::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetWindowFreezeImmediately renderPipleine == nullptr!");
        return false;
    }
    if (!isFreeze) {
        renderPipleine->SetWindowFreezeImmediately(id, isFreeze, nullptr, captureConfig, blurParam);
        return true;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetWindowFreezeImmediately callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderPipelineClient::SetWindowFreezeImmediately surfaceCaptureCbMap_.count(id) != 0");
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = {callback};
        surfaceCaptureCbMap_.emplace(std::make_pair(id, captureConfig), callbackVector);
    }

    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderPipleine->SetWindowFreezeImmediately(id, isFreeze, surfaceCaptureCbDirector_, captureConfig, blurParam);
    return true;
}

bool RSRenderPipelineClient::TakeSurfaceCaptureWithAllWindows(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("%{public}s renderPipleine == nullptr!", __func__);
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("%{public}s callback == nullptr!", __func__);
        return false;
    }
    std::pair<NodeId, RSSurfaceCaptureConfig> key = std::make_pair(id, captureConfig);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find(key);
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("%{public}s surfaceCaptureCbMap_.count(id) != 0", __func__);
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = { callback };
        surfaceCaptureCbMap_.emplace(key, callbackVector);

        if (surfaceCaptureCbDirector_ == nullptr) {
            surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
        }
    }

    auto ret = renderPipleine->TakeSurfaceCaptureWithAllWindows(
        id, surfaceCaptureCbDirector_, captureConfig, checkDrmAndSurfaceLock);
    if (ret != ERR_OK) {
        ROSEN_LOGE("%{public}s fail, ret[%{public}d]", __func__, ret);
        std::lock_guard<std::mutex> lock(mutex_);
        surfaceCaptureCbMap_.erase(key);
        return false;
    }
    return true;
}

bool RSRenderPipelineClient::FreezeScreen(NodeId id, bool isFreeze)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("%{public}s renderPipleine == nullptr!", __func__);
        return false;
    }
    renderPipleine->FreezeScreen(id, isFreeze);
    return true;
}

bool RSRenderPipelineClient::TakeUICaptureInRange(
    NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeUICaptureInRange renderPipleine == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeUICaptureInRange callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderPipelineClient::TakeUICaptureInRange surfaceCaptureCbMap_.count(id) != 0");
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = {callback};
        surfaceCaptureCbMap_.emplace(std::make_pair(id, captureConfig), callbackVector);
    }

    std::lock_guard<std::mutex> lock(surfaceCaptureCbDirectorMutex_);
    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderPipleine->TakeUICaptureInRange(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderPipelineClient::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        RS_LOGE("RSRenderPipelineClient::SetHwcNodeBounds renderPipleine is null!");
        return false;
    }
    renderPipleine->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

int32_t RSRenderPipelineClient::SetFocusAppInfo(const FocusAppInfo& info)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    renderPipleine->SetFocusAppInfo(info, repCode);
    return repCode;
}

int32_t RSRenderPipelineClient::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    auto err = renderPipleine->GetScreenHDRStatus(id, hdrStatus, resCode);
    if (err != ERR_OK) {
        ROSEN_LOGE("RSRenderPipelineClient::GetScreenHDRStatus err(%{public}d)!", err);
        resCode = err;
    }
    return resCode;
}

void RSRenderPipelineClient::DropFrameByPid(const std::vector<int32_t>& pidList, int32_t dropFrameLevel)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine != nullptr) {
        renderPipleine->DropFrameByPid(pidList, dropFrameLevel);
    }
}

bool RSRenderPipelineClient::SetAncoForceDoDirect(bool direct)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine != nullptr) {
        bool res;
        renderPipleine->SetAncoForceDoDirect(direct, res);
        return res;
    }
    ROSEN_LOGE("RSRenderPipelineClient::SetAncoForceDoDirect renderPipleine is null");
    return false;
}

class SurfaceBufferCallbackDirector : public RSSurfaceBufferCallbackStub {
public:
    explicit SurfaceBufferCallbackDirector(RSRenderPipelineClient* client) : client_(client) {}
    ~SurfaceBufferCallbackDirector() noexcept override = default;
    void OnFinish(const FinishCallbackRet& ret) override
    {
        client_->TriggerOnFinish(ret);
    }

    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) override
    {
        client_->TriggerOnAfterAcquireBuffer(ret);
    }

private:
    RSRenderPipelineClient* client_;
};

bool RSRenderPipelineClient::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, std::shared_ptr<SurfaceBufferCallback> callback)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterSurfaceBufferCallback renderPipleine == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterSurfaceBufferCallback callback == nullptr!");
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
        if (surfaceBufferCallbacks_.find(uid) == std::end(surfaceBufferCallbacks_)) {
            surfaceBufferCallbacks_.emplace(uid, callback);
        } else {
            ROSEN_LOGE("RSRenderPipelineClient::RegisterSurfaceBufferCallback callback exists"
                " in uid %{public}s", std::to_string(uid).c_str());
            return false;
        }
        if (surfaceBufferCbDirector_ == nullptr) {
            surfaceBufferCbDirector_ = new SurfaceBufferCallbackDirector(this);
        }
    }
    renderPipleine->RegisterSurfaceBufferCallback(pid, uid, surfaceBufferCbDirector_);
    return true;
}

bool RSRenderPipelineClient::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::UnregisterSurfaceBufferCallback renderPipleine == nullptr!");
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
        auto iter = surfaceBufferCallbacks_.find(uid);
        if (iter == std::end(surfaceBufferCallbacks_)) {
            ROSEN_LOGE("RSRenderPipelineClient::UnregisterSurfaceBufferCallback invaild uid.");
            return false;
        }
        surfaceBufferCallbacks_.erase(iter);
    }
    renderPipleine->UnregisterSurfaceBufferCallback(pid, uid);
    return true;
}

void RSRenderPipelineClient::TriggerOnFinish(const FinishCallbackRet& ret) const
{
    std::shared_ptr<SurfaceBufferCallback> callback = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
        if (auto iter = surfaceBufferCallbacks_.find(ret.uid); iter != std::cend(surfaceBufferCallbacks_)) {
            callback = iter->second;
        }
    }
    if (!callback) {
        ROSEN_LOGD("RSRenderPipelineClient::TriggerOnFinish callback is null");
        return;
    }
    callback->OnFinish(ret);
}

void RSRenderPipelineClient::TriggerOnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) const
{
    std::shared_ptr<SurfaceBufferCallback> callback = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
        if (auto iter = surfaceBufferCallbacks_.find(ret.uid); iter != std::cend(surfaceBufferCallbacks_)) {
            callback = iter->second;
        }
    }
    if (callback) {
        callback->OnAfterAcquireBuffer(ret);
    }
}

void RSRenderPipelineClient::SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine != nullptr) {
        renderPipleine->SetLayerTopForHWC(nodeId, isTop, zOrder);
    }
}

class TransactionDataCallbackDirector : public RSTransactionDataCallbackStub {
public:
    explicit TransactionDataCallbackDirector(RSRenderPipelineClient* client) : client_(client) {}
    ~TransactionDataCallbackDirector() noexcept override = default;
    void OnAfterProcess(uint64_t token, uint64_t timeStamp) override
    {
        RS_LOGD("OnAfterProcess: TriggerTransactionDataCallbackAndErase, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64, timeStamp, token);
        client_->TriggerTransactionDataCallbackAndErase(token, timeStamp);
    }

private:
    RSRenderPipelineClient* client_;
};

bool RSRenderPipelineClient::RegisterTransactionDataCallback(uint64_t token, uint64_t timeStamp,
    std::function<void()> callback)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterTransactionDataCallback renderPipleine == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterTransactionDataCallback callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock{ transactionDataCallbackMutex_ };
        if (transactionDataCallbacks_.find(std::make_pair(token, timeStamp)) == std::end(transactionDataCallbacks_)) {
            transactionDataCallbacks_.emplace(std::make_pair(token, timeStamp), callback);
        } else {
            ROSEN_LOGE("RSRenderPipelineClient::RegisterTransactionDataCallback callback exists"
                " in timeStamp %{public}s", std::to_string(timeStamp).c_str());
            return false;
        }
        if (transactionDataCbDirector_ == nullptr) {
            transactionDataCbDirector_ = new TransactionDataCallbackDirector(this);
        }
    }
    RS_LOGD("RSRenderPipelineClient::RegisterTransactionDataCallback, timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    renderPipleine->RegisterTransactionDataCallback(token, timeStamp, transactionDataCbDirector_);
    return true;
}

void RSRenderPipelineClient::TriggerTransactionDataCallbackAndErase(uint64_t token, uint64_t timeStamp)
{
    std::function<void()> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock{ transactionDataCallbackMutex_ };
        auto iter = transactionDataCallbacks_.find(std::make_pair(token, timeStamp));
        if (iter != std::end(transactionDataCallbacks_)) {
            callback = iter->second;
            transactionDataCallbacks_.erase(iter);
        }
    }
    if (callback) {
        RS_LOGD("TriggerTransactionDataCallbackAndErase: invoke callback, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64, timeStamp, token);
        std::invoke(callback);
    }
}

void RSRenderPipelineClient::SetWindowContainer(NodeId nodeId, bool value)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine != nullptr) {
        renderPipleine->SetWindowContainer(nodeId, value);
    }
}

void RSRenderPipelineClient::ClearUifirstCache(NodeId id)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (!renderPipleine) {
        return;
    }
    renderPipleine->ClearUifirstCache(id);
}

void RSRenderPipelineClient::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        return;
    }
    renderPipleine->SetScreenFrameGravity(id, gravity);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSRenderPipelineClient::RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterCanvasCallback renderPipleine is nullptr!");
        return;
    }

    renderPipleine->RegisterCanvasCallback(callback);
}

int32_t RSRenderPipelineClient::SubmitCanvasPreAllocatedBuffer(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SubmitCanvasPreAllocatedBuffer renderPipleine is nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return renderPipleine->SubmitCanvasPreAllocatedBuffer(nodeId, buffer, resetSurfaceIndex);
}
#endif // ROSEN_OHOS && RS_ENABLE_VK

int32_t RSRenderPipelineClient::SetLogicalCameraRotationCorrection(ScreenId id, ScreenRotation logicalCorrection)
{
    auto renderPipleine = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipleine == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetLogicalCameraRotationCorrection renderPipleine is nullptr!");
        return RENDER_SERVICE_NULL;
    }
    RS_LOGD("RSRenderPipelineClient::SetLogicalCameraRotationCorrection, screenId: %{public}"
        PRIu64 ", logicalCorrection: %{public}u", id, logicalCorrection);
    return renderPipleine->SetLogicalCameraRotationCorrection(id, logicalCorrection);
}
} // namespace Rosen
} // namespace OHOS
