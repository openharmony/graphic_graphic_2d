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
#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
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
RSRenderPipelineClient::RSRenderPipelineClient(sptr<IRemoteObject>& connectToRenderRemote)
{
    auto conn = iface_cast<RSIConnectToRenderProcess>(connectToRenderRemote);
    if (conn == nullptr) {
        RS_LOGE("RSRenderPipelineClient::%{public}s, iface_cast failed", __func__);
        return;
    }
    sptr<RSIConnectionToken> token = new IRemoteStub<RSIConnectionToken>();
    clientToRenderConnection_ = conn->CreateRenderConnection(token);
}

void RSRenderPipelineClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    if (clientToRenderConnection_ != nullptr) {
        clientToRenderConnection_->CommitTransaction(transactionData);
    } else {
        RS_LOGE_LIMIT(__func__, __line__,
            "RSRenderPipelineClient::CommitTransaction failed, clientToRenderConnection_ is nullptr");
    }
}

void RSRenderPipelineClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return;
    }
    if (clientToRenderConnection_ != nullptr) {
        clientToRenderConnection_->ExecuteSynchronousTask(task);
    } else {
        RS_LOGE_LIMIT(__func__, __line__,
            "RSRenderPipelineClient::ExecuteSynchronousTask failed, clientToRenderConnection_ is nullptr");
    }
}

void RSRenderPipelineClient::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    if (clientToRenderConnection_ != nullptr) {
        clientToRenderConnection_->RegisterApplicationAgent(pid, app);
    } else {
        RS_LOGE_LIMIT(__func__, __line__,
            "RSRenderPipelineClient::RegisterApplicationAgent failed, clientToRenderConnection_ is nullptr");
    }
}

bool RSRenderPipelineClient::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateNode clientToRenderConnection_ nullptr");
        return false;
    }
    bool success;
    clientToRenderConnection_->CreateNode(displayNodeConfig, nodeId, success);
    return success;
}

bool RSRenderPipelineClient::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateNode clientToRenderConnection_ nullptr");
        return false;
    }
    bool success;
    clientToRenderConnection_->CreateNode(config, success);
    return success;
}

std::shared_ptr<RSSurface> RSRenderPipelineClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    bool unobscured)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateNodeAndSurface clientToRenderConnection_ nullptr");
        return nullptr;
    }

    sptr<Surface> surface = nullptr;
    ErrCode err = clientToRenderConnection_->CreateNodeAndSurface(config, surface, unobscured);
    if ((err != ERR_OK) || (surface == nullptr)) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateNodeAndSurface surface is nullptr.");
        return nullptr;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    surface = RSVpeManager::GetInstance().CheckAndGetSurface(surface, config);
    if (surface == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CheckAndGetSurface surface is nullptr.");
        return nullptr;
    }
#endif
    return CreateRSSurface(surface);
}

std::shared_ptr<RSSurface> RSRenderPipelineClient::CreateRSSurface(const sptr<Surface> &surface)
{
#if defined (RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        return std::make_shared<RSSurfaceOhosVulkan>(surface); // GPU render
    }
#endif

#if defined (RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == Rosen::GpuApiType::OPENGL) {
        return std::make_shared<RSSurfaceOhosGl>(surface); // GPU render
    }
#endif
    return std::make_shared<RSSurfaceOhosRaster>(surface); // CPU render
}

class CustomBufferAvailableCallback : public RSBufferAvailableCallbackStub {
public:
    explicit CustomBufferAvailableCallback(const BufferAvailableCallback &callback) : cb_(callback) {}
    ~CustomBufferAvailableCallback() override {};

    void OnBufferAvailable() override
    {
        if (cb_ != nullptr) {
            cb_();
        }
    }

private:
    BufferAvailableCallback cb_;
};

class CustomBufferClearCallback : public RSBufferClearCallbackStub {
public:
    explicit CustomBufferClearCallback(const BufferClearCallback &callback) : cb_(callback) {}
    ~CustomBufferClearCallback() override {};

    void OnBufferClear() override
    {
        if (cb_ != nullptr) {
            cb_();
        }
    }

private:
    BufferClearCallback cb_;
};

bool RSRenderPipelineClient::RegisterBufferAvailableListener(
    NodeId id, const BufferAvailableCallback &callback, bool isFromRenderThread)
{
    if (clientToRenderConnection_ == nullptr) {
        RS_TRACE_NAME_FMT("RSRenderPipelineClient::RegisterBufferAvailableListener nullptr id is %lu", id);
        return false;
    }
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto iter = isFromRenderThread ? bufferAvailableCbRTMap_.find(id) : bufferAvailableCbUIMap_.find(id);
    if (isFromRenderThread && iter != bufferAvailableCbRTMap_.end()) {
        HILOG_COMM_WARN("RSRenderPipelineClient::RegisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " already, bufferAvailableCbRTMap_", iter->first);
    }

    if (!isFromRenderThread && iter != bufferAvailableCbUIMap_.end()) {
        HILOG_COMM_WARN("RSRenderPipelineClient::RegisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " already, bufferAvailableCbUIMap_", iter->first);
        bufferAvailableCbUIMap_.erase(iter);
    }

    sptr<RSIBufferAvailableCallback> bufferAvailableCb = new CustomBufferAvailableCallback(callback);
    clientToRenderConnection_->RegisterBufferAvailableListener(id, bufferAvailableCb, isFromRenderThread);
    if (isFromRenderThread) {
        bufferAvailableCbRTMap_.emplace(id, bufferAvailableCb);
    } else {
        bufferAvailableCbUIMap_.emplace(id, bufferAvailableCb);
    }
    return true;
}

bool RSRenderPipelineClient::RegisterBufferClearListener(NodeId id, const BufferClearCallback& callback)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterBufferClearListener clientToRenderConnection_ is nullptr!");
        return false;
    }
    sptr<RSIBufferClearCallback> bufferClearCb = new CustomBufferClearCallback(callback);
    clientToRenderConnection_->RegisterBufferClearListener(id, bufferClearCb);
    return true;
}


bool RSRenderPipelineClient::UnregisterBufferAvailableListener(NodeId id)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto iter = bufferAvailableCbRTMap_.find(id);
    if (iter != bufferAvailableCbRTMap_.end()) {
        bufferAvailableCbRTMap_.erase(iter);
    } else {
        ROSEN_LOGD("RSRenderPipelineClient::UnregisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " has not registered RT callback", id);
    }
    iter = bufferAvailableCbUIMap_.find(id);
    if (iter != bufferAvailableCbUIMap_.end()) {
        bufferAvailableCbUIMap_.erase(iter);
    } else {
        ROSEN_LOGD("RSRenderPipelineClient::UnregisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " has not registered UI callback", id);
    }
    return true;
}

bool RSRenderPipelineClient::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetSystemAnimatedScenes clientToRenderConnection == nullptr!");
        return false;
    }
    bool success;
    clientToRenderConnection_->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation, success);
    return success;
}

uint32_t RSRenderPipelineClient::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent)
{
    if (clientToRenderConnection_ != nullptr) {
        uint32_t resCode;
        clientToRenderConnection_->SetHidePrivacyContent(id, needHidePrivacyContent, resCode);
        return resCode;
    }
    ROSEN_LOGE("RSRenderPipelineClient::SetHidePrivacyContent clientToRenderConnection_ is nullptr!");
    return static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
}

void RSRenderPipelineClient::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    if (clientToRenderConnection_ != nullptr) {
        clientToRenderConnection_->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);
    }
}

bool RSRenderPipelineClient::GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::GetPixelmap: clientToRenderConnection_ is nullptr");
        return false;
    }
    bool success;
    clientToRenderConnection_->GetPixelmap(id, pixelmap, rect, drawCmdList, success);
    return success;
}

bool RSRenderPipelineClient::SetGlobalDarkColorMode(bool isDark)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetGlobalDarkColorMode: clientToRenderConnection_ is nullptr");
        return false;
    }
    return clientToRenderConnection_->SetGlobalDarkColorMode(isDark) == ERR_OK;
}

bool RSRenderPipelineClient::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::GetBitmap clientToRenderConnection_ == nullptr!");
        return false;
    }
    bool success;
    clientToRenderConnection_->GetBitmap(id, bitmap, success);
    return success;
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
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSurfaceCapture clientToRenderConnection_ == nullptr!");
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
    clientToRenderConnection_->TakeSurfaceCapture(id, surfaceCaptureCbDirector_, captureConfig, blurParam, specifiedAreaRect);
    return true;
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderPipelineClient::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig)
{
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSurfaceCaptureSoloNode clientToRenderConnection_ == nullptr!");
        return pixelMapIdPairVector;
    }
    pixelMapIdPairVector = clientToRenderConnection_->TakeSurfaceCaptureSoloNode(id, captureConfig);
    return pixelMapIdPairVector;
}

bool RSRenderPipelineClient::TakeSelfSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSelfSurfaceCapture clientToRenderConnection_ == nullptr!");
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
    clientToRenderConnection_->TakeSelfSurfaceCapture(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderPipelineClient::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetWindowFreezeImmediately clientToRenderConnection_ == nullptr!");
        return false;
    }
    if (!isFreeze) {
        clientToRenderConnection_->SetWindowFreezeImmediately(id, isFreeze, nullptr, captureConfig, blurParam);
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
    clientToRenderConnection_->SetWindowFreezeImmediately(id, isFreeze, surfaceCaptureCbDirector_, captureConfig, blurParam);
    return true;
}

bool RSRenderPipelineClient::TakeSurfaceCaptureWithAllWindows(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("%{public}s clientToRenderConnection_ == nullptr!", __func__);
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

    auto ret = clientToRenderConnection_->TakeSurfaceCaptureWithAllWindows(
        id, surfaceCaptureCbDirector_, captureConfig, checkDrmAndSurfaceLock);
    if (ret != ERR_OK) {
        ROSEN_LOGE("%{public}s fail, ret[%{public}d]", __func__, ret);
        std::lock_guard<std::mutex> lock(mutex_);
        surfaceCaptureCbMap_.erase(key);
        return false;
    }
    return true;
}

bool RSRenderPipelineClient::FreezeScreen(NodeId id, bool isFreeze, bool needSync)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("%{public}s clientToRenderConnection_ == nullptr!", __func__);
        return false;
    }
    clientToRenderConnection_->FreezeScreen(id, isFreeze, needSync);
    return true;
}

bool RSRenderPipelineClient::TakeUICaptureInRange(
    NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeUICaptureInRange clientToRenderConnection_ == nullptr!");
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
    clientToRenderConnection_->TakeUICaptureInRange(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderPipelineClient::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    if (clientToRenderConnection_ == nullptr) {
        RS_LOGE("RSRenderPipelineClient::SetHwcNodeBounds clientToRenderConnection_ is null!");
        return false;
    }
    clientToRenderConnection_->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

int32_t RSRenderPipelineClient::SetFocusAppInfo(const FocusAppInfo& info)
{
    if (clientToRenderConnection_ == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToRenderConnection_->SetFocusAppInfo(info, repCode);
    return repCode;
}

int32_t RSRenderPipelineClient::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    if (clientToRenderConnection_ == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    auto err = clientToRenderConnection_->GetBrightnessInfo(screenId, brightnessInfo);
    if (err != SUCCESS) {
        ROSEN_LOGE("RSRenderPipelineClient::GetBrightnessInfo err(%{public}d)!", err);
        resCode = err;
    }
    return resCode;
}

int32_t RSRenderPipelineClient::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus)
{
    if (clientToRenderConnection_ == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    auto err = clientToRenderConnection_->GetScreenHDRStatus(id, hdrStatus, resCode);
    if (err != ERR_OK) {
        ROSEN_LOGE("RSRenderPipelineClient::GetScreenHDRStatus err(%{public}d)!", err);
        resCode = err;
    }
    return resCode;
}

void RSRenderPipelineClient::DropFrameByPid(const std::vector<int32_t>& pidList, int32_t dropFrameLevel)
{
    if (clientToRenderConnection_ != nullptr) {
        clientToRenderConnection_->DropFrameByPid(pidList, dropFrameLevel);
    }
}

bool RSRenderPipelineClient::SetAncoForceDoDirect(bool direct)
{
    if (clientToRenderConnection_ != nullptr) {
        bool res;
        clientToRenderConnection_->SetAncoForceDoDirect(direct, res);
        return res;
    }
    ROSEN_LOGE("RSRenderPipelineClient::SetAncoForceDoDirect clientToRenderConnection_ is null");
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
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSurfaceBufferCallback clientToRenderConnection_ == nullptr!");
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
    clientToRenderConnection_->RegisterSurfaceBufferCallback(pid, uid, surfaceBufferCbDirector_);
    return true;
}

bool RSRenderPipelineClient::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::UnregisterSurfaceBufferCallback clientToRenderConnection_ == nullptr!");
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
    clientToRenderConnection_->UnregisterSurfaceBufferCallback(pid, uid);
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
    if (clientToRenderConnection_ != nullptr) {
        clientToRenderConnection_->SetLayerTopForHWC(nodeId, isTop, zOrder);
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
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterTransactionDataCallback clientToRenderConnection_ == nullptr!");
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
    clientToRenderConnection_->RegisterTransactionDataCallback(token, timeStamp, transactionDataCbDirector_);
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
    if (clientToRenderConnection_ != nullptr) {
        clientToRenderConnection_->SetWindowContainer(nodeId, value);
    }
}

void RSRenderPipelineClient::ClearUifirstCache(NodeId id)
{
    if (!clientToRenderConnection_) {
        ROSEN_LOGE("RSRenderPipelineClient::%{public}s clientToRenderConnection_ == nullptr!", __func__);
        return;
    }
    clientToRenderConnection_->ClearUifirstCache(id);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSRenderPipelineClient::RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterCanvasCallback clientToRenderConnection_ is nullptr!");
        return;
    }

    clientToRenderConnection_->RegisterCanvasCallback(callback);
}

int32_t RSRenderPipelineClient::SubmitCanvasPreAllocatedBuffer(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    if (clientToRenderConnection_ == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SubmitCanvasPreAllocatedBuffer clientToRenderConnection_ is nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection_->SubmitCanvasPreAllocatedBuffer(nodeId, buffer, resetSurfaceIndex);
}
#endif // ROSEN_OHOS && RS_ENABLE_VK

uint32_t RSRenderPipelineClient::SetSurfaceWatermark(pid_t pid, const std::string &name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType)
{
    if (clientToRenderConnection_ == nullptr) {
        return WATER_MARK_RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection_->SetSurfaceWatermark(pid, name, watermark, nodeIdList, watermarkType);
}
    
void RSRenderPipelineClient::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
    const std::vector<NodeId> &nodeIdList)
{
    auto clientToRenderConnection_ = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (clientToRenderConnection_ == nullptr) {
        return;
    }
    clientToRenderConnection_->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList);
}
    
void RSRenderPipelineClient::ClearSurfaceWatermark(pid_t pid, const std::string &name)
{
    if (clientToRenderConnection_ == nullptr) {
        return;
    }
    clientToRenderConnection_->ClearSurfaceWatermark(pid, name);
}

class CustomOcclusionChangeCallback : public RSOcclusionChangeCallbackStub {
public:
    explicit CustomOcclusionChangeCallback(const OcclusionChangeCallback &callback) : cb_(callback) {}
    ~CustomOcclusionChangeCallback() override {};

    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override
    {
        if (cb_ != nullptr) {
            cb_(occlusionData);
        }
    }

private:
    OcclusionChangeCallback cb_;
};

class CustomSurfaceOcclusionChangeCallback : public RSSurfaceOcclusionChangeCallbackStub {
public:
    explicit CustomSurfaceOcclusionChangeCallback(const SurfaceOcclusionChangeCallback &callback) : cb_(callback) {}
    ~CustomSurfaceOcclusionChangeCallback() override {};

    void OnSurfaceOcclusionVisibleChanged(float visibleAreaRatio) override
    {
        if (cb_ != nullptr) {
            cb_(visibleAreaRatio);
        }
    }

private:
    SurfaceOcclusionChangeCallback cb_;
};

ErrCode RSRenderPipelineClient::RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    if (clientToRenderConnection_ == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    return clientToRenderConnection_->RegisterOcclusionChangeCallback(cb);
}

int32_t RSRenderPipelineClient::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints)
{
    if (clientToRenderConnection_ == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomSurfaceOcclusionChangeCallback> cb = new CustomSurfaceOcclusionChangeCallback(callback);
    return clientToRenderConnection_->RegisterSurfaceOcclusionChangeCallback(id, cb, partitionPoints);
}

int32_t RSRenderPipelineClient::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    if (clientToRenderConnection_ == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection_->UnRegisterSurfaceOcclusionChangeCallback(id);
}

int32_t RSRenderPipelineClient::SetLogicalCameraRotationCorrection(ScreenId id, ScreenRotation logicalCorrection)
{
    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection();
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetLogicalCameraRotationCorrection renderPipeline is nullptr!");
        return RENDER_SERVICE_NULL;
    }
    RS_LOGD("RSRenderPipelineClient::SetLogicalCameraRotationCorrection, screenId: %{public}"
        PRIu64 ", logicalCorrection: %{public}u", id, logicalCorrection);
    return renderPipeline->SetLogicalCameraRotationCorrection(id, logicalCorrection);
}
} // namespace Rosen
} // namespace OHOS
