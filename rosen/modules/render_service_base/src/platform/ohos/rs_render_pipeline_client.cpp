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

#include "app_image_observer_manager.h"
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
#include "ipc_callbacks/rs_frame_stability_callback_stub.h"
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
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSRenderPipelineClient::RSRenderPipelineClient()
{
    tokenMaskId_ = RSRenderServiceConnectHub::GetDefaultTokenMaskId();
}

RSRenderPipelineClient::RSRenderPipelineClient(sptr<IRemoteObject>& connectToRenderRemote)
{
    static bool isUniRender = RSSystemProperties::GetUniRenderEnabled();
    if (isUniRender && RSSystemProperties::IsSceneBoardEnabled()) {
        tokenMaskId_ = RSRenderServiceConnectHub::GetRenderProcessTokenMaskId(connectToRenderRemote);
    } else {
        tokenMaskId_ = RSRenderServiceConnectHub::GetDefaultTokenMaskId();
    }
}

void RSRenderPipelineClient::SetOnRenderProcessDiedCallback(const std::function<void()>& callback)
{
    RSRenderServiceConnectHub::GetInstance()->SetOnRenderProcessDiedCallback(tokenMaskId_, callback);
}

void RSRenderPipelineClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        clientToRenderConnection->CommitTransaction(transactionData);
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
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        clientToRenderConnection->ExecuteSynchronousTask(task);
    } else {
        RS_LOGE_LIMIT(__func__, __line__,
            "RSRenderPipelineClient::ExecuteSynchronousTask failed, clientToRenderConnection_ is nullptr");
    }
}

void RSRenderPipelineClient::RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        clientToRenderConnection->RegisterApplicationAgent(pid, app);
    } else {
        RS_LOGE_LIMIT(__func__, __line__,
            "RSRenderPipelineClient::RegisterApplicationAgent failed, clientToRenderConnection_ is nullptr");
    }
}

bool RSRenderPipelineClient::CreateDisplayNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateNode clientToRenderConnection_ nullptr");
        return false;
    }
    bool success;
    clientToRenderConnection->CreateDisplayNode(displayNodeConfig, nodeId, success);
    return success;
}

bool RSRenderPipelineClient::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateNode clientToRenderConnection_ nullptr");
        return false;
    }
    bool success;
    clientToRenderConnection->CreateNode(config, success);
    return success;
}

std::shared_ptr<RSSurface> RSRenderPipelineClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    bool unobscured)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateNodeAndSurface clientToRenderConnection_ nullptr");
        return nullptr;
    }

    sptr<Surface> surface = nullptr;
    ErrCode err = clientToRenderConnection->CreateNodeAndSurface(config, surface, unobscured);
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
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
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
    clientToRenderConnection->RegisterBufferAvailableListener(id, bufferAvailableCb, isFromRenderThread);
    if (isFromRenderThread) {
        bufferAvailableCbRTMap_.emplace(id, bufferAvailableCb);
    } else {
        bufferAvailableCbUIMap_.emplace(id, bufferAvailableCb);
    }
    return true;
}

bool RSRenderPipelineClient::RegisterBufferClearListener(NodeId id, const BufferClearCallback& callback)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterBufferClearListener clientToRenderConnection_ is nullptr!");
        return false;
    }
    sptr<RSIBufferClearCallback> bufferClearCb = new CustomBufferClearCallback(callback);
    clientToRenderConnection->RegisterBufferClearListener(id, bufferClearCb);
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
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetSystemAnimatedScenes clientToRenderConnection == nullptr!");
        return false;
    }
    bool success;
    clientToRenderConnection->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation, success);
    return success;
}

uint32_t RSRenderPipelineClient::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        uint32_t resCode;
        clientToRenderConnection->SetHidePrivacyContent(id, needHidePrivacyContent, resCode);
        return resCode;
    }
    ROSEN_LOGE("RSRenderPipelineClient::SetHidePrivacyContent clientToRenderConnection_ is nullptr!");
    return static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
}

void RSRenderPipelineClient::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        clientToRenderConnection->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);
    }
}

bool RSRenderPipelineClient::GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::GetPixelmap: clientToRenderConnection_ is nullptr");
        return false;
    }
    bool success;
    clientToRenderConnection->GetPixelmap(id, pixelmap, rect, drawCmdList, success);
    return success;
}

bool RSRenderPipelineClient::SetGlobalDarkColorMode(bool isDark)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetGlobalDarkColorMode: clientToRenderConnection_ is nullptr");
        return false;
    }
    return clientToRenderConnection->SetGlobalDarkColorMode(isDark) == ERR_OK;
}

bool RSRenderPipelineClient::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::GetBitmap clientToRenderConnection == nullptr!");
        return false;
    }
    bool success;
    clientToRenderConnection->GetBitmap(id, bitmap, success);
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
    explicit SurfaceCaptureCallbackDirector(std::weak_ptr<RSRenderPipelineClient> client) : client_(client) {}
    ~SurfaceCaptureCallbackDirector() override = default;
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig, Media::PixelMap* pixelmap,
        CaptureError captureErrorCode = CaptureError::CAPTURE_OK, Media::PixelMap* pixelmapHDR = nullptr) override
    {
        auto client = client_.lock();
        if (!client) {
            ROSEN_LOGE("SurfaceCaptureCallbackDirector::OnSurfaceCapture: client has been destroyed");
            return;
        }
        std::shared_ptr<Media::PixelMap> surfaceCapture(pixelmap);
        std::shared_ptr<Media::PixelMap> surfaceCaptureHDR(pixelmapHDR);
        client->TriggerSurfaceCaptureCallback(id, captureConfig, surfaceCapture, captureErrorCode, surfaceCaptureHDR);
    };

private:
    std::weak_ptr<RSRenderPipelineClient> client_;
};

bool RSRenderPipelineClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSurfaceCapture clientToRenderConnection == nullptr!");
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
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(weak_from_this());
    }
    clientToRenderConnection->TakeSurfaceCapture(
        id, surfaceCaptureCbDirector_, captureConfig, blurParam, specifiedAreaRect);
    return true;
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderPipelineClient::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig)
{
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSurfaceCaptureSoloNode clientToRenderConnection == nullptr!");
        return pixelMapIdPairVector;
    }
    pixelMapIdPairVector = clientToRenderConnection->TakeSurfaceCaptureSoloNode(id, captureConfig);
    return pixelMapIdPairVector;
}

bool RSRenderPipelineClient::TakeSelfSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeSelfSurfaceCapture clientToRenderConnection == nullptr!");
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
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(weak_from_this());
    }
    clientToRenderConnection->TakeSelfSurfaceCapture(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderPipelineClient::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetWindowFreezeImmediately clientToRenderConnection == nullptr!");
        return false;
    }
    if (!isFreeze) {
        clientToRenderConnection->SetWindowFreezeImmediately(id, isFreeze, nullptr, captureConfig, blurParam);
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
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(weak_from_this());
    }
    clientToRenderConnection->SetWindowFreezeImmediately(
        id, isFreeze, surfaceCaptureCbDirector_, captureConfig, blurParam);
    return true;
}

bool RSRenderPipelineClient::TakeSurfaceCaptureWithAllWindows(NodeId id,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    bool checkDrmAndSurfaceLock)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("%{public}s clientToRenderConnection == nullptr!", __func__);
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
            surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(weak_from_this());
        }
    }

    auto ret = clientToRenderConnection->TakeSurfaceCaptureWithAllWindows(
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
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("%{public}s clientToRenderConnection == nullptr!", __func__);
        return false;
    }
    clientToRenderConnection->FreezeScreen(id, isFreeze, needSync);
    return true;
}

bool RSRenderPipelineClient::TakeUICaptureInRange(
    NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::TakeUICaptureInRange clientToRenderConnection == nullptr!");
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
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(weak_from_this());
    }
    clientToRenderConnection->TakeUICaptureInRange(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderPipelineClient::SetHwcNodeBounds(NodeId rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        RS_LOGE("RSRenderPipelineClient::SetHwcNodeBounds clientToRenderConnection_ is null!");
        return false;
    }
    clientToRenderConnection->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

int32_t RSRenderPipelineClient::SetFocusAppInfo(const FocusAppInfo& info)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToRenderConnection->SetFocusAppInfo(info, repCode);
    return repCode;
}

int32_t RSRenderPipelineClient::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    auto err = clientToRenderConnection->GetBrightnessInfo(screenId, brightnessInfo);
    if (err != SUCCESS) {
        ROSEN_LOGE("RSRenderPipelineClient::GetBrightnessInfo err(%{public}d)!", err);
        resCode = err;
    }
    return resCode;
}

int32_t RSRenderPipelineClient::GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    auto err = clientToRenderConnection->GetScreenHDRStatus(id, hdrStatus, resCode);
    if (err != ERR_OK) {
        ROSEN_LOGE("RSRenderPipelineClient::GetScreenHDRStatus err(%{public}d)!", err);
        resCode = err;
    }
    return resCode;
}

void RSRenderPipelineClient::DropFrameByPid(const std::vector<int32_t>& pidList, int32_t dropFrameLevel)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        clientToRenderConnection->DropFrameByPid(pidList, dropFrameLevel);
    }
}

bool RSRenderPipelineClient::SetAncoForceDoDirect(bool direct)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        bool res;
        clientToRenderConnection->SetAncoForceDoDirect(direct, res);
        return res;
    }
    ROSEN_LOGE("RSRenderPipelineClient::SetAncoForceDoDirect clientToRenderConnection_ is null");
    return false;
}

class SurfaceBufferCallbackDirector : public RSSurfaceBufferCallbackStub {
public:
    explicit SurfaceBufferCallbackDirector(std::weak_ptr<RSRenderPipelineClient> client) : client_(client) {}
    ~SurfaceBufferCallbackDirector() noexcept override = default;
    void OnFinish(const FinishCallbackRet& ret) override
    {
        auto client = client_.lock();
        if (!client) {
            ROSEN_LOGE("SurfaceBufferCallbackDirector::OnFinish: client has been destroyed");
            return;
        }
        client->TriggerOnFinish(ret);
    }

    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) override
    {
        auto client = client_.lock();
        if (!client) {
            ROSEN_LOGE("SurfaceBufferCallbackDirector::OnAfterAcquireBuffer: client has been destroyed");
            return;
        }
        client->TriggerOnAfterAcquireBuffer(ret);
    }

private:
    std::weak_ptr<RSRenderPipelineClient> client_;
};


bool RSRenderPipelineClient::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, std::shared_ptr<SurfaceBufferCallback> callback)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSurfaceBufferCallback clientToRenderConnection == nullptr!");
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
            surfaceBufferCbDirector_ = new SurfaceBufferCallbackDirector(weak_from_this());
        }
    }
    clientToRenderConnection->RegisterSurfaceBufferCallback(pid, uid, surfaceBufferCbDirector_);
    return true;
}

bool RSRenderPipelineClient::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::UnregisterSurfaceBufferCallback clientToRenderConnection == nullptr!");
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
    clientToRenderConnection->UnregisterSurfaceBufferCallback(pid, uid);
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
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        clientToRenderConnection->SetLayerTopForHWC(nodeId, isTop, zOrder);
    }
}

class TransactionDataCallbackDirector : public RSTransactionDataCallbackStub {
public:
    explicit TransactionDataCallbackDirector(std::weak_ptr<RSRenderPipelineClient> client) : client_(client) {}
    ~TransactionDataCallbackDirector() noexcept override = default;
    void OnAfterProcess(uint64_t token, uint64_t timeStamp) override
    {
        auto client = client_.lock();
        if (!client) {
            RS_LOGE("TransactionDataCallbackDirector::OnAfterProcess: client has been destroyed");
            return;
        }
        RS_LOGD("OnAfterProcess: TriggerTransactionDataCallbackAndErase, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64, timeStamp, token);
        client->TriggerTransactionDataCallbackAndErase(token, timeStamp);
    }

private:
    std::weak_ptr<RSRenderPipelineClient> client_;
};

bool RSRenderPipelineClient::RegisterTransactionDataCallback(uint64_t token, uint64_t timeStamp,
    std::function<void()> callback)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterTransactionDataCallback clientToRenderConnection == nullptr!");
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
            transactionDataCbDirector_ = new TransactionDataCallbackDirector(weak_from_this());
        }
    }
    RS_LOGD("RSRenderPipelineClient::RegisterTransactionDataCallback, timeStamp: %{public}"
        PRIu64 " token: %{public}" PRIu64, timeStamp, token);
    clientToRenderConnection->RegisterTransactionDataCallback(token, timeStamp, transactionDataCbDirector_);
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
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection != nullptr) {
        clientToRenderConnection->SetWindowContainer(nodeId, value);
    }
}

void RSRenderPipelineClient::ClearUifirstCache(NodeId id)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::%{public}s clientToRenderConnection == nullptr!", __func__);
        return;
    }
    clientToRenderConnection->ClearUifirstCache(id);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSRenderPipelineClient::RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::RegisterCanvasCallback clientToRenderConnection_ is nullptr!");
        return;
    }

    clientToRenderConnection->RegisterCanvasCallback(callback);
}

int32_t RSRenderPipelineClient::SubmitCanvasPreAllocatedBuffer(
    NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SubmitCanvasPreAllocatedBuffer clientToRenderConnection_ is nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection->SubmitCanvasPreAllocatedBuffer(nodeId, buffer, resetSurfaceIndex);
}
#endif // ROSEN_OHOS && RS_ENABLE_VK

uint32_t RSRenderPipelineClient::SetSurfaceWatermark(pid_t pid, const std::string& name,
    const std::shared_ptr<Media::PixelMap>& watermark, const std::vector<NodeId>& nodeIdList,
    SurfaceWatermarkType watermarkType, uint32_t rowCount, uint32_t colCount)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return WATER_MARK_RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection->SetSurfaceWatermark(
        pid, name, watermark, nodeIdList, watermarkType, rowCount, colCount);
}

void RSRenderPipelineClient::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
    const std::vector<NodeId> &nodeIdList)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return;
    }
    clientToRenderConnection->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList);
}

void RSRenderPipelineClient::ClearSurfaceWatermark(pid_t pid, const std::string &name)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return;
    }
    clientToRenderConnection->ClearSurfaceWatermark(pid, name);
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
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    return clientToRenderConnection->RegisterOcclusionChangeCallback(cb);
}

int32_t RSRenderPipelineClient::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomSurfaceOcclusionChangeCallback> cb = new CustomSurfaceOcclusionChangeCallback(callback);
    return clientToRenderConnection->RegisterSurfaceOcclusionChangeCallback(id, cb, partitionPoints);
}

int32_t RSRenderPipelineClient::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection->UnRegisterSurfaceOcclusionChangeCallback(id);
}

int32_t RSRenderPipelineClient::SetLogicalCameraRotationCorrection(ScreenId id, ScreenRotation logicalCorrection)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetLogicalCameraRotationCorrection clientToRenderConnection_ is nullptr!");
        return RENDER_SERVICE_NULL;
    }
    RS_LOGD("RSRenderPipelineClient::SetLogicalCameraRotationCorrection, screenId: %{public}"
        PRIu64 ", logicalCorrection: %{public}u", id, logicalCorrection);
    return clientToRenderConnection->SetLogicalCameraRotationCorrection(id, logicalCorrection);
}

int32_t RSRenderPipelineClient::GetMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::GetMaxGpuBufferSize clientToRenderConnection_ is nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection->GetMaxGpuBufferSize(maxWidth, maxHeight);
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
sptr<Surface> RSRenderPipelineClient::CreateCanvasDrawingNodeSurface(NodeId nodeId)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateCanvasDrawingNodeSurface Vulkan not enabled");
        return nullptr;
    }
    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateCanvasDrawingNodeSurface renderPipeline is nullptr");
        return nullptr;
    }
    sptr<Surface> surface = renderPipeline->CreateCanvasDrawingNodeSurface(nodeId);
    if (surface == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::CreateCanvasDrawingNodeSurface remote surface is nullptr");
        return nullptr;
    }
    return surface;
}

void RSRenderPipelineClient::ReleaseCanvasDrawingNodeSurface(NodeId nodeId)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        ROSEN_LOGE("RSRenderPipelineClient::ReleaseCanvasDrawingNodeSurface Vulkan not enabled");
        return;
    }
    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::ReleaseCanvasDrawingNodeSurface renderPipeline is nullptr");
        return;
    }
    renderPipeline->ReleaseCanvasDrawingNodeSurface(nodeId);
}
#endif // RS_MODIFIERS_DRAW_ENABLE

void RSRenderPipelineClient::SetFreeMultiWindowStatus(bool enable)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::SetFreeMultiWindowStatus clientToRenderConnection == nullptr!");
        return;
    }
    clientToRenderConnection->SetFreeMultiWindowStatus(enable);
}
class CustomFrameStabilityCallback : public RSFrameStabilityCallbackStub {
public:
    explicit CustomFrameStabilityCallback(const FrameStabilityCallback &callback) : cb_(callback) {}
    ~CustomFrameStabilityCallback() override {};

    void OnFrameStabilityChanged(bool isStable) override
    {
        if (cb_ != nullptr) {
            cb_(isStable);
        }
    }

private:
    FrameStabilityCallback cb_;
};

int32_t RSRenderPipelineClient::RegisterFrameStabilityDetection(
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config,
    const FrameStabilityCallback& callback)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RegisterFrameStabilityDetection clientToRenderConnection == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    if (config.stableDuration < MIN_STABLE_DURATION || config.stableDuration > MAX_STABLE_DURATION) {
        ROSEN_LOGE("RegisterFrameStabilityDetection invalid stableDuration: %{public}u", config.stableDuration);
        return INVALID_ARGUMENTS;
    }
    if (ROSEN_LNE(config.changePercent, MIN_CHANGE_PERCENT) ||
        ROSEN_GNE(config.changePercent, MAX_CHANGE_PERCENT)) {
        ROSEN_LOGE("RegisterFrameStabilityDetection invalid changePercent: %{public}f", config.changePercent);
        return INVALID_ARGUMENTS;
    }
    sptr<CustomFrameStabilityCallback> cb = new CustomFrameStabilityCallback(callback);
    return clientToRenderConnection->RegisterFrameStabilityDetection(target, config, cb);
}

int32_t RSRenderPipelineClient::UnregisterFrameStabilityDetection(const FrameStabilityTarget& target)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::UnregisterFrameStabilityDetection clientToRenderConnection == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection->UnregisterFrameStabilityDetection(target);
}

int32_t RSRenderPipelineClient::StartFrameStabilityCollection(
    const FrameStabilityTarget& target,
    const FrameStabilityConfig& config)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::StartFrameStabilityCollection clientToRenderConnection == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    if (config.stableDuration < MIN_STABLE_DURATION || config.stableDuration > MAX_STABLE_DURATION) {
        ROSEN_LOGE("StartFrameStabilityCollection invalid stableDuration: %{public}u", config.stableDuration);
        return INVALID_ARGUMENTS;
    }
    if (ROSEN_LNE(config.changePercent, MIN_CHANGE_PERCENT) ||
        ROSEN_GNE(config.changePercent, MAX_CHANGE_PERCENT)) {
        ROSEN_LOGE("StartFrameStabilityCollection invalid changePercent: %{public}f", config.changePercent);
        return INVALID_ARGUMENTS;
    }
    return clientToRenderConnection->StartFrameStabilityCollection(target, config);
}

int32_t RSRenderPipelineClient::GetFrameStabilityResult(const FrameStabilityTarget& target, bool& result)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("RSRenderPipelineClient::GetFrameStabilityResult clientToRenderConnection == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection->GetFrameStabilityResult(target, result);
}

int32_t RSRenderPipelineClient::UpdateFrameStabilityDetection(
    const FrameStabilityTarget& oldTarget,
    const FrameStabilityTarget& newTarget)
{
    auto clientToRenderConnection = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (clientToRenderConnection == nullptr) {
        ROSEN_LOGE("UpdateFrameStabilityDetection clientToRenderConnection == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToRenderConnection->UpdateFrameStabilityDetection(oldTarget, newTarget);
}

bool RSRenderPipelineClient::SetDelegateMode(NodeId id, bool isSetDelegateMode, pid_t pid)
{
    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("DelegateModeDebugTag:SetDelegateMode renderPipeline == nullptr!");
        return false;
    }
    return renderPipeline->SetDelegateMode(id, isSetDelegateMode, pid);
}

bool RSRenderPipelineClient::RegisterSurfaceTransactionListener(sptr<RSISurfaceTransactionListener> listener,
    uint64_t listenerId)
{
    if (listener == nullptr) {
        RS_TRACE_NAME("RegisterSurfaceTransactionListener fail, listener is nullptr");
        ROSEN_LOGE("DelegateModeDebugTag:RegisterSurfaceTransactionListener fail, listener is nullptr");
        return false;
    }
    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("DelegateModeDebugTag:RegisterSurfaceTransactionListener renderPipeline == nullptr!");
        return false;
    }
    return renderPipeline->RegisterSurfaceTransactionListener(listener, listenerId);
}

bool RSRenderPipelineClient::UnRegisterSurfaceTransactionListener(uint64_t listenerId)
{
    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("DelegateModeDebugTag:UnRegisterSurfaceTransactionListener renderPipeline == nullptr!");
        return false;
    }
    return renderPipeline->UnRegisterSurfaceTransactionListener(listenerId);
}

bool RSRenderPipelineClient::RegisterSurfaceNodeBufferReleaseListener(
    sptr<RSISurfaceNodeBufferReleaseCallback> listener)
{
    if (listener == nullptr) {
        RS_TRACE_NAME("RegisterSurfaceNodeBufferReleaseListener fail, listener is nullptr");
        ROSEN_LOGE("DelegateModeDebugTag:RegisterSurfaceNodeBufferReleaseListener renderPipeline == nullptr!");
        return false;
    }

    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("DelegateModeDebugTag:RegisterSurfaceNodeBufferReleaseListener renderPipeline == nullptr!");
        return false;
    }
    return renderPipeline->RegisterSurfaceNodeBufferReleaseListener(listener);
}

bool RSRenderPipelineClient::UnRegisterSurfaceNodeBufferReleaseListener()
{
    auto renderPipeline = RSRenderServiceConnectHub::GetClientToRenderConnection(tokenMaskId_);
    if (renderPipeline == nullptr) {
        ROSEN_LOGE("DelegateModeDebugTag:UnRegisterSurfaceNodeBufferReleaseListener renderPipeline == nullptr!");
        return false;
    }
    return renderPipeline->UnRegisterSurfaceNodeBufferReleaseListener();
}
} // namespace Rosen
} // namespace OHOS
