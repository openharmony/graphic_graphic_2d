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

#include "transaction/rs_render_service_client.h"
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
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#include "ipc_callbacks/rs_surface_occlusion_change_callback_stub.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
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
#include "platform/common/rs_system_properties.h"
#include "render/rs_typeface_cache.h"
#include "rs_render_service_connect_hub.h"
#include "rs_surface_ohos.h"
#include "vsync_iconnection_token.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSIRenderClient> RSIRenderClient::client_ = nullptr;

std::shared_ptr<RSIRenderClient> RSIRenderClient::CreateRenderServiceClient()
{
    static std::once_flag once_flag;
    std::call_once(once_flag, []() { client_ = std::make_shared<RSRenderServiceClient>(); });
    return client_;
}

void RSRenderServiceClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->CommitTransaction(transactionData);
    } else {
        RS_LOGE_LIMIT(__func__, __line__, "RSRenderServiceClient::CommitTransaction failed, renderService is nullptr");
    }
}

void RSRenderServiceClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return;
    }

    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ExecuteSynchronousTask(task);
    }
}

bool RSRenderServiceClient::GetUniRenderEnabled()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    bool enable;
    return renderService->GetUniRenderEnabled(enable) == ERR_OK && enable;
}

MemoryGraphic RSRenderServiceClient::GetMemoryGraphic(int pid)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return MemoryGraphic {};
    }
    MemoryGraphic memoryGraphic;
    renderService->GetMemoryGraphic(pid, memoryGraphic);
    return memoryGraphic;
}

std::vector<MemoryGraphic> RSRenderServiceClient::GetMemoryGraphics()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return {};
    }
    std::vector<MemoryGraphic> memoryGraphics;
    renderService->GetMemoryGraphics(memoryGraphics);
    return memoryGraphics;
}

bool RSRenderServiceClient::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    return renderService->GetTotalAppMemSize(cpuMemSize, gpuMemSize) == ERR_OK;
}

bool RSRenderServiceClient::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    bool success;
    renderService->CreateNode(displayNodeConfig, nodeId, success);
    return success;
}

bool RSRenderServiceClient::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    bool success;
    renderService->CreateNode(config, success);
    return success;
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    bool unobscured)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return nullptr;
    }

    sptr<Surface> surface = nullptr;
    ErrCode err = renderService->CreateNodeAndSurface(config, surface, unobscured);
    if ((err != ERR_OK) || (surface == nullptr)) {
        ROSEN_LOGE("RSRenderServiceClient::CreateNodeAndSurface surface is nullptr.");
        return nullptr;
    }
#ifdef USE_VIDEO_PROCESSING_ENGINE
    surface = RSVpeManager::GetInstance().CheckAndGetSurface(surface, config);
    if (surface == nullptr) {
        ROSEN_LOGE("RSVpeManager::CheckAndGetSurface surface is nullptr.");
        return nullptr;
    }
#endif
    return CreateRSSurface(surface);
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateRSSurface(const sptr<Surface> &surface)
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

std::shared_ptr<VSyncReceiver> RSRenderServiceClient::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper,
    uint64_t id,
    NodeId windowNodeId,
    bool fromXcomponent)
{
    ROSEN_LOGD("RSRenderServiceClient::CreateVSyncReceiver Start");
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return nullptr;
    }
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<IVSyncConnection> conn = nullptr;
    VSyncConnParam vsyncConnParam = {id, windowNodeId, fromXcomponent};
    renderService->CreateVSyncConnection(conn, name, token, vsyncConnParam);
    if (conn == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::CreateVSyncReceiver Failed");
        return nullptr;
    }
    return std::make_shared<VSyncReceiver>(conn, token->AsObject(), looper, name);
}

int32_t RSRenderServiceClient::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    renderService->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    return repCode;
}

std::shared_ptr<Media::PixelMap> RSRenderServiceClient::CreatePixelMapFromSurfaceId(uint64_t surfaceId,
    const Rect &srcRect)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return nullptr;
    }
    sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
    if (surface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    return renderService->CreatePixelMapFromSurface(surface, srcRect, pixelMap) == ERR_OK ? pixelMap : nullptr;
}

void RSRenderServiceClient::TriggerSurfaceCaptureCallback(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
    std::shared_ptr<Media::PixelMap> pixelmap)
{
    ROSEN_LOGD("RSRenderServiceClient::Into TriggerSurfaceCaptureCallback nodeId:[%{public}" PRIu64 "]", id);
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
        ROSEN_LOGE("RSRenderServiceClient::TriggerSurfaceCaptureCallback: callbackVector is empty!");
        return;
    }
    for (decltype(callbackVector.size()) i = 0; i < callbackVector.size(); ++i) {
        if (callbackVector[i] == nullptr) {
            ROSEN_LOGE("RSRenderServiceClient::TriggerSurfaceCaptureCallback: callback is nullptr!");
            continue;
        }
        std::shared_ptr<Media::PixelMap> surfaceCapture = pixelmap;
        if (i != callbackVector.size() - 1) {
            if (pixelmap != nullptr) {
                Media::InitializationOptions options;
                std::unique_ptr<Media::PixelMap> pixelmapCopy = Media::PixelMap::Create(*pixelmap, options);
                surfaceCapture = std::move(pixelmapCopy);
            }
        }
        if (surfaceCapture) {
            surfaceCapture->SetMemoryName("RSSurfaceCaptureForCallback");
        }
        callbackVector[i]->OnSurfaceCapture(surfaceCapture);
    }
}

class SurfaceCaptureCallbackDirector : public RSSurfaceCaptureCallbackStub
{
public:
    explicit SurfaceCaptureCallbackDirector(RSRenderServiceClient* client) : client_(client) {}
    ~SurfaceCaptureCallbackDirector() override {};
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig, Media::PixelMap* pixelmap) override
    {
        std::shared_ptr<Media::PixelMap> surfaceCapture(pixelmap);
        client_->TriggerSurfaceCaptureCallback(id, captureConfig, surfaceCapture);
    };

private:
    RSRenderServiceClient* client_;
};

bool RSRenderServiceClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig, const RSSurfaceCaptureBlurParam& blurParam,
    const Drawing::Rect& specifiedAreaRect)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TakeSurfaceCapture renderService == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TakeSurfaceCapture callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderServiceClient::TakeSurfaceCapture surfaceCaptureCbMap_.count(id) != 0");
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = {callback};
        surfaceCaptureCbMap_.emplace(std::make_pair(id, captureConfig), callbackVector);
    }

    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderService->TakeSurfaceCapture(id, surfaceCaptureCbDirector_, captureConfig, blurParam, specifiedAreaRect);
    return true;
}

std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> RSRenderServiceClient::TakeSurfaceCaptureSoloNode(
    NodeId id, const RSSurfaceCaptureConfig& captureConfig)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector;
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TakeSurfaceCaptureSoloNode renderService == nullptr!");
        return pixelMapIdPairVector;
    }
    pixelMapIdPairVector = renderService->TakeSurfaceCaptureSoloNode(id, captureConfig);
    return pixelMapIdPairVector;
}

bool RSRenderServiceClient::TakeSelfSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    const RSSurfaceCaptureConfig& captureConfig)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TakeSelfSurfaceCapture renderService == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TakeSelfSurfaceCapture callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderServiceClient::TakeSelfSurfaceCapture surfaceCaptureCbMap_.count(id) != 0");
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = {callback};
        surfaceCaptureCbMap_.emplace(std::make_pair(id, captureConfig), callbackVector);
    }

    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderService->TakeSelfSurfaceCapture(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderServiceClient::SetWindowFreezeImmediately(NodeId id, bool isFreeze,
    std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig,
    const RSSurfaceCaptureBlurParam& blurParam)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetWindowFreezeImmediately renderService == nullptr!");
        return false;
    }
    if (!isFreeze) {
        renderService->SetWindowFreezeImmediately(id, isFreeze, nullptr, captureConfig, blurParam);
        return true;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetWindowFreezeImmediately callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderServiceClient::SetWindowFreezeImmediately surfaceCaptureCbMap_.count(id) != 0");
            iter->second.emplace_back(callback);
            return true;
        }
        std::vector<std::shared_ptr<SurfaceCaptureCallback>> callbackVector = {callback};
        surfaceCaptureCbMap_.emplace(std::make_pair(id, captureConfig), callbackVector);
    }

    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderService->SetWindowFreezeImmediately(id, isFreeze, surfaceCaptureCbDirector_, captureConfig, blurParam);
    return true;
}

bool RSRenderServiceClient::TakeUICaptureInRange(
    NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback, const RSSurfaceCaptureConfig& captureConfig)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TakeUICaptureInRange renderService == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TakeUICaptureInRange callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find({ id, captureConfig });
        if (iter != surfaceCaptureCbMap_.end()) {
            ROSEN_LOGD("RSRenderServiceClient::TakeUICaptureInRange surfaceCaptureCbMap_.count(id) != 0");
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
    renderService->TakeUICaptureInRange(id, surfaceCaptureCbDirector_, captureConfig);
    return true;
}

bool RSRenderServiceClient::SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
    float positionZ, float positionW)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        RS_LOGE("RSRenderServiceClient::SetHwcNodeBounds renderService is null!");
        return false;
    }
    renderService->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

int32_t RSRenderServiceClient::SetFocusAppInfo(const FocusAppInfo& info)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    renderService->SetFocusAppInfo(info, repCode);
    return repCode;
}

ScreenId RSRenderServiceClient::GetDefaultScreenId()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    ScreenId screenId{INVALID_SCREEN_ID};
    renderService->GetDefaultScreenId(screenId);
    return screenId;
}

ScreenId RSRenderServiceClient::GetActiveScreenId()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    ScreenId screenId{INVALID_SCREEN_ID};
    renderService->GetActiveScreenId(screenId);
    return screenId;
}

std::vector<ScreenId> RSRenderServiceClient::GetAllScreenIds()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return std::vector<ScreenId>();
    }

    return renderService->GetAllScreenIds();
}

ScreenId RSRenderServiceClient::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s renderService is null!", __func__);
        return INVALID_SCREEN_ID;
    }

    return renderService->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);
}

int32_t RSRenderServiceClient::SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s renderService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetVirtualScreenBlackList(id, blackListVector);
}

int32_t RSRenderServiceClient::SetVirtualScreenTypeBlackList(ScreenId id, std::vector<NodeType>& typeBlackListVector)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    renderService->SetVirtualScreenTypeBlackList(id, typeBlackListVector, repCode);
    return repCode;
}

int32_t RSRenderServiceClient::AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s renderService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    renderService->AddVirtualScreenBlackList(id, blackListVector, repCode);
    return repCode;
}

int32_t RSRenderServiceClient::RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s renderService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    renderService->RemoveVirtualScreenBlackList(id, blackListVector, repCode);
    return repCode;
}

bool RSRenderServiceClient::SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    bool success;
    renderService->SetWatermark(name, watermark, success);
    return success;
}

int32_t RSRenderServiceClient::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

int32_t RSRenderServiceClient::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetScreenSecurityMask(id, std::move(securityMask));
}

int32_t RSRenderServiceClient::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

int32_t RSRenderServiceClient::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetCastScreenEnableSkipWindow(id, enable);
}

int32_t RSRenderServiceClient::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetVirtualScreenSurface(id, surface);
}

void RSRenderServiceClient::RemoveVirtualScreen(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->RemoveVirtualScreen(id);
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
int32_t RSRenderServiceClient::SetPointerColorInversionConfig(float darkBuffer, float brightBuffer,
    int64_t interval, int32_t rangeSize)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
 
    return renderService->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
}
 
int32_t RSRenderServiceClient::SetPointerColorInversionEnabled(bool enable)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
 
    return renderService->SetPointerColorInversionEnabled(enable);
}
 
class CustomPointerLuminanceChangeCallback : public RSPointerLuminanceChangeCallbackStub
{
public:
    explicit CustomPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback) : cb_(callback) {}
    ~CustomPointerLuminanceChangeCallback() override {};
 
    void OnPointerLuminanceChanged(int32_t brightness) override
    {
        if (cb_ != nullptr) {
            cb_(brightness);
        }
    }
 
private:
    PointerLuminanceChangeCallback cb_;
};
 
int32_t RSRenderServiceClient::RegisterPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
 
    sptr<RSIPointerLuminanceChangeCallback> cb = new CustomPointerLuminanceChangeCallback(callback);
    return renderService->RegisterPointerLuminanceChangeCallback(cb);
}
 
int32_t RSRenderServiceClient::UnRegisterPointerLuminanceChangeCallback()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->UnRegisterPointerLuminanceChangeCallback();
}
#endif

class CustomScreenChangeCallback : public RSScreenChangeCallbackStub
{
public:
    explicit CustomScreenChangeCallback(const ScreenChangeCallback &callback) : cb_(callback) {}
    ~CustomScreenChangeCallback() override {};

    void OnScreenChanged(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason) override
    {
        if (cb_ != nullptr) {
            cb_(id, event, reason);
        }
    }

private:
    ScreenChangeCallback cb_;
};

int32_t RSRenderServiceClient::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s renderService is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    screenChangeCb_ = new CustomScreenChangeCallback(callback);
    return renderService->SetScreenChangeCallback(screenChangeCb_);
}

void RSRenderServiceClient::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->SetScreenActiveMode(id, modeId);
}

void RSRenderServiceClient::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return;
    }

    renderService->SetScreenRefreshRate(id, sceneId, rate);
}

void RSRenderServiceClient::SetRefreshRateMode(int32_t refreshRateMode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return;
    }

    renderService->SetRefreshRateMode(refreshRateMode);
}

void RSRenderServiceClient::SyncFrameRateRange(FrameRateLinkerId id,
    const FrameRateRange& range, int32_t animatorExpectedFrameRate)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return;
    }

    return renderService->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
}

void RSRenderServiceClient::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return;
    }
    renderService->UnregisterFrameRateLinker(id);
}

uint32_t RSRenderServiceClient::GetScreenCurrentRefreshRate(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return renderService->GetScreenCurrentRefreshRate(id);
}

int32_t RSRenderServiceClient::GetCurrentRefreshRateMode()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return renderService->GetCurrentRefreshRateMode();
}

std::vector<int32_t> RSRenderServiceClient::GetScreenSupportedRefreshRates(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return {};
    }

    return renderService->GetScreenSupportedRefreshRates(id);
}

bool RSRenderServiceClient::GetShowRefreshRateEnabled()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return false;
    }

    bool enable = false;
    renderService->GetShowRefreshRateEnabled(enable);
    return enable;
}

std::string RSRenderServiceClient::GetRefreshInfo(pid_t pid)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return "";
    }
    std::string enable;
    renderService->GetRefreshInfo(pid, enable);
    return enable;
}

std::string RSRenderServiceClient::GetRefreshInfoToSP(NodeId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return "";
    }
    std::string enable;
    renderService->GetRefreshInfoToSP(id, enable);
    return enable;
}

void RSRenderServiceClient::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return;
    }

    return renderService->SetShowRefreshRateEnabled(enabled, type);
}

uint32_t RSRenderServiceClient::GetRealtimeRefreshRate(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return renderService->GetRealtimeRefreshRate(id);
}

int32_t RSRenderServiceClient::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("%{public}s: render service is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetPhysicalScreenResolution(id, width, height);
}

int32_t RSRenderServiceClient::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualScreenResolution renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetVirtualScreenResolution(id, width, height);
}

RSVirtualScreenResolution RSRenderServiceClient::GetVirtualScreenResolution(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RSVirtualScreenResolution {}; // return empty RSVirtualScreenResolution.
    }

    return renderService->GetVirtualScreenResolution(id);
}

void RSRenderServiceClient::MarkPowerOffNeedProcessOneFrame()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->MarkPowerOffNeedProcessOneFrame();
}

void RSRenderServiceClient::RepaintEverything()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RepaintEverything renderService is null, return");
        return;
    }

    renderService->RepaintEverything();
}

void RSRenderServiceClient::ForceRefreshOneFrameWithNextVSync()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("ForceRefreshOneFrameWithNextVSync renderService is nullptr, return");
        return;
    }

    renderService->ForceRefreshOneFrameWithNextVSync();
}

void RSRenderServiceClient::DisablePowerOffRenderControl(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->DisablePowerOffRenderControl(id);
}

void RSRenderServiceClient::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s renderService is nullptr", __func__);
        return;
    }

    renderService->SetScreenPowerStatus(id, status);
}

RSScreenModeInfo RSRenderServiceClient::GetScreenActiveMode(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RSScreenModeInfo {}; // return empty RSScreenModeInfo.
    }

    RSScreenModeInfo screenModeInfo;
    renderService->GetScreenActiveMode(id, screenModeInfo);
    return screenModeInfo;
}

std::vector<RSScreenModeInfo> RSRenderServiceClient::GetScreenSupportedModes(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return {};
    }

    return renderService->GetScreenSupportedModes(id);
}

RSScreenCapability RSRenderServiceClient::GetScreenCapability(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RSScreenCapability {};
    }

    return renderService->GetScreenCapability(id);
}

ScreenPowerStatus RSRenderServiceClient::GetScreenPowerStatus(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }
    uint32_t status {static_cast<int32_t>(ScreenPowerStatus::INVALID_POWER_STATUS)};
    renderService->GetScreenPowerStatus(id, status);
    return static_cast<ScreenPowerStatus>(status);
}

RSScreenData RSRenderServiceClient::GetScreenData(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RSScreenData {};
    }

    return renderService->GetScreenData(id);
}

int32_t RSRenderServiceClient::GetScreenBacklight(ScreenId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s renderService is nullptr", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    int32_t backLightLevel = INVALID_BACKLIGHT_VALUE;
    renderService->GetScreenBacklight(id, backLightLevel);
    return backLightLevel;
}

void RSRenderServiceClient::SetScreenBacklight(ScreenId id, uint32_t level)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s renderService is nullptr", __func__);
        return;
    }

    renderService->SetScreenBacklight(id, level);
}

class CustomBufferAvailableCallback : public RSBufferAvailableCallbackStub
{
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

class CustomBufferClearCallback : public RSBufferClearCallbackStub
{
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

bool RSRenderServiceClient::RegisterBufferAvailableListener(
    NodeId id, const BufferAvailableCallback &callback, bool isFromRenderThread)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto iter = isFromRenderThread ? bufferAvailableCbRTMap_.find(id) : bufferAvailableCbUIMap_.find(id);
    if (isFromRenderThread && iter != bufferAvailableCbRTMap_.end()) {
        ROSEN_LOGW("RSRenderServiceClient::RegisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " already, bufferAvailableCbRTMap_", iter->first);
    }

    if (!isFromRenderThread && iter != bufferAvailableCbUIMap_.end()) {
        ROSEN_LOGW("RSRenderServiceClient::RegisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " already, bufferAvailableCbUIMap_", iter->first);
        bufferAvailableCbUIMap_.erase(iter);
    }

    sptr<RSIBufferAvailableCallback> bufferAvailableCb = new CustomBufferAvailableCallback(callback);
    renderService->RegisterBufferAvailableListener(id, bufferAvailableCb, isFromRenderThread);
    if (isFromRenderThread) {
        bufferAvailableCbRTMap_.emplace(id, bufferAvailableCb);
    } else {
        bufferAvailableCbUIMap_.emplace(id, bufferAvailableCb);
    }
    return true;
}

bool RSRenderServiceClient::RegisterBufferClearListener(NodeId id, const BufferClearCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    sptr<RSIBufferClearCallback> bufferClearCb = new CustomBufferClearCallback(callback);
    renderService->RegisterBufferClearListener(id, bufferClearCb);
    return true;
}


bool RSRenderServiceClient::UnregisterBufferAvailableListener(NodeId id)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto iter = bufferAvailableCbRTMap_.find(id);
    if (iter != bufferAvailableCbRTMap_.end()) {
        bufferAvailableCbRTMap_.erase(iter);
    } else {
        ROSEN_LOGD("RSRenderServiceClient::UnregisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " has not registered RT callback", id);
    }
    iter = bufferAvailableCbUIMap_.find(id);
    if (iter != bufferAvailableCbUIMap_.end()) {
        bufferAvailableCbUIMap_.erase(iter);
    } else {
        ROSEN_LOGD("RSRenderServiceClient::UnregisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " has not registered UI callback", id);
    }
    return true;
}

int32_t RSRenderServiceClient::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetScreenSupportedColorGamuts(id, mode);
}

int32_t RSRenderServiceClient::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RequestRotation renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetScreenSupportedMetaDataKeys(id, keys);
}

int32_t RSRenderServiceClient::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetScreenColorGamut(id, mode);
}

int32_t RSRenderServiceClient::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->SetScreenColorGamut(id, modeIdx);
}

int32_t RSRenderServiceClient::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->SetScreenGamutMap(id, mode);
}

int32_t RSRenderServiceClient::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->SetScreenCorrection(id, screenRotation);
}

bool RSRenderServiceClient::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualMirrorScreenCanvasRotation: renderService is nullptr");
        return false;
    }
    return renderService->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

int32_t RSRenderServiceClient::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualScreenAutoRotation: renderService is nullptr");
        return RENDER_SERVICE_NULL;
    }
    return renderService->SetVirtualScreenAutoRotation(id, isAutoRotation);
}

bool RSRenderServiceClient::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualMirrorScreenScaleMode: renderService is nullptr");
        return false;
    }
    return renderService->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

bool RSRenderServiceClient::SetGlobalDarkColorMode(bool isDark)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetGlobalDarkColorMode: renderService is nullptr");
        return false;
    }
    return renderService->SetGlobalDarkColorMode(isDark) == ERR_OK;
}

int32_t RSRenderServiceClient::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetScreenGamutMap(id, mode);
}

int32_t RSRenderServiceClient::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetScreenHDRCapability renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSRenderServiceClient::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetPixelFormat renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->GetPixelFormat(id, pixelFormat, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetPixelFormat renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->SetPixelFormat(id, pixelFormat, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->GetScreenSupportedHDRFormats(id, hdrFormats, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->GetScreenHDRFormat(id, hdrFormat, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->SetScreenHDRFormat(id, modeIdx, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->GetScreenSupportedColorSpaces(id, colorSpaces, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->GetScreenColorSpace(id, colorSpace, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode;
    renderService->SetScreenColorSpace(id, colorSpace, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetScreenType renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetScreenType(id, screenType);
}

bool RSRenderServiceClient::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetBitmap renderService == nullptr!");
        return false;
    }
    bool success;
    renderService->GetBitmap(id, bitmap, success);
    return success;
}

bool RSRenderServiceClient::GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetPixelmap: renderService is nullptr");
        return false;
    }
    bool success;
    renderService->GetPixelmap(id, pixelmap, rect, drawCmdList, success);
    return success;
}

bool RSRenderServiceClient::RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterTypeface: renderService is nullptr");
        return false;
    }
    uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
    ROSEN_LOGD("RSRenderServiceClient::RegisterTypeface: pid[%{public}d] register typface[%{public}u]",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    // timer: 3s
    OHOS::Rosen::RSXCollie registerTypefaceXCollie("registerTypefaceXCollie_" + typeface->GetFamilyName(), 3);
    return renderService->RegisterTypeface(globalUniqueId, typeface);
}

bool RSRenderServiceClient::UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnRegisterTypeface: renderService is nullptr");
        return false;
    }
    uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
    ROSEN_LOGD("RSRenderServiceClient::UnRegisterTypeface: pid[%{public}d] unregister typface[%{public}u]",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    return renderService->UnRegisterTypeface(globalUniqueId);
}

int32_t RSRenderServiceClient::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetDisplayIdentificationData(id, outPort, edidData);
}

int32_t RSRenderServiceClient::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t statusCode = SUCCESS;
    renderService->SetScreenSkipFrameInterval(id, skipFrameInterval, statusCode);
    return statusCode;
}

int32_t RSRenderServiceClient::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    int32_t resCode = RENDER_SERVICE_NULL;
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    renderService->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate, resCode);
    return resCode;
}

uint32_t RSRenderServiceClient::SetScreenActiveRect(ScreenId id, const Rect& activeRect)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    uint32_t repCode;
    return renderService->SetScreenActiveRect(id, activeRect, repCode);
    return repCode;
}

void RSRenderServiceClient::SetScreenOffset(ScreenId id, int32_t offSetX, int32_t offSetY)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }
    renderService->SetScreenOffset(id, offSetX, offSetY);
}

void RSRenderServiceClient::SetScreenFrameGravity(ScreenId id, int32_t gravity)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }
    renderService->SetScreenFrameGravity(id, gravity);
}

class CustomOcclusionChangeCallback : public RSOcclusionChangeCallbackStub
{
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

int32_t RSRenderServiceClient::RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterOcclusionChangeCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    int32_t repCode;
    renderService->RegisterOcclusionChangeCallback(cb, repCode);
    return repCode;
}

class CustomSurfaceOcclusionChangeCallback : public RSSurfaceOcclusionChangeCallbackStub
{
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

int32_t RSRenderServiceClient::RegisterSurfaceOcclusionChangeCallback(
    NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSurfaceOcclusionChangeCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomSurfaceOcclusionChangeCallback> cb = new CustomSurfaceOcclusionChangeCallback(callback);
    return renderService->RegisterSurfaceOcclusionChangeCallback(id, cb, partitionPoints);
}

int32_t RSRenderServiceClient::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnRegisterSurfaceOcclusionChangeCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return renderService->UnRegisterSurfaceOcclusionChangeCallback(id);
}

class CustomHgmConfigChangeCallback : public RSHgmConfigChangeCallbackStub
{
public:
    explicit CustomHgmConfigChangeCallback(const HgmConfigChangeCallback& callback) : cb_(callback) {}
    ~CustomHgmConfigChangeCallback() override {};

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
        if (cb_ != nullptr) {
            cb_(configData);
        }
    }

    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {
    }

    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {
    }
private:
    HgmConfigChangeCallback cb_;
};

int32_t RSRenderServiceClient::RegisterHgmConfigChangeCallback(const HgmConfigChangeCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterHgmConfigChangeCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomHgmConfigChangeCallback> cb = new CustomHgmConfigChangeCallback(callback);
    return renderService->RegisterHgmConfigChangeCallback(cb);
}

class CustomHgmRefreshRateModeChangeCallback : public RSHgmConfigChangeCallbackStub
{
public:
    explicit CustomHgmRefreshRateModeChangeCallback(const HgmRefreshRateModeChangeCallback& callback) : cb_(callback) {}
    ~CustomHgmRefreshRateModeChangeCallback() override {};

    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {
        if (cb_ != nullptr) {
            cb_(refreshRateMode);
        }
    }

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
    }

    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {
    }
private:
    HgmRefreshRateModeChangeCallback cb_;
};

int32_t RSRenderServiceClient::RegisterHgmRefreshRateModeChangeCallback(
    const HgmRefreshRateModeChangeCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterHgmRefreshRateModeChangeCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomHgmRefreshRateModeChangeCallback> cb = new CustomHgmRefreshRateModeChangeCallback(callback);
    return renderService->RegisterHgmRefreshRateModeChangeCallback(cb);
}

class CustomHgmRefreshRateUpdateCallback : public RSHgmConfigChangeCallbackStub
{
public:
    explicit CustomHgmRefreshRateUpdateCallback(const HgmRefreshRateModeChangeCallback& callback) : cb_(callback) {}
    ~CustomHgmRefreshRateUpdateCallback() override {};

    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {
    }

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
    }

    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {
        ROSEN_LOGD("CustomHgmRefreshRateUpdateCallback::OnHgmRefreshRateUpdate called");
        if (cb_ != nullptr) {
            cb_(refreshRate);
        }
    }

private:
    HgmRefreshRateUpdateCallback cb_;
};

int32_t RSRenderServiceClient::RegisterHgmRefreshRateUpdateCallback(
    const HgmRefreshRateUpdateCallback& callback)
{
    sptr<CustomHgmRefreshRateUpdateCallback> cb = nullptr;
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterHgmRefreshRateUpdateCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    if (callback) {
        cb = new CustomHgmRefreshRateUpdateCallback(callback);
    }

    ROSEN_LOGD("RSRenderServiceClient::RegisterHgmRefreshRateUpdateCallback called");
    return renderService->RegisterHgmRefreshRateUpdateCallback(cb);
}

class CustomFirstFrameCommitCallback : public RSFirstFrameCommitCallbackStub
{
public:
    explicit CustomFirstFrameCommitCallback(const FirstFrameCommitCallback& callback) : cb_(callback) {}
    ~CustomFirstFrameCommitCallback() override {};

    void OnFirstFrameCommit(uint64_t screenId, int64_t timestamp) override
    {
        ROSEN_LOGD("CustomFirstFrameCommitCallback::OnFirstFrameCommit called");
        if (cb_ != nullptr) {
            cb_(screenId, timestamp);
        }
    }

private:
    FirstFrameCommitCallback cb_;
};

int32_t RSRenderServiceClient::RegisterFirstFrameCommitCallback(
    const FirstFrameCommitCallback& callback)
{
    sptr<CustomFirstFrameCommitCallback> cb = nullptr;
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterFirstFrameCommitCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    if (callback) {
        cb = new CustomFirstFrameCommitCallback(callback);
    }

    ROSEN_LOGD("RSRenderServiceClient::RegisterFirstFrameCommitCallback called");
    return renderService->RegisterFirstFrameCommitCallback(cb);
}

class CustomFrameRateLinkerExpectedFpsUpdateCallback : public RSFrameRateLinkerExpectedFpsUpdateCallbackStub
{
public:
    explicit CustomFrameRateLinkerExpectedFpsUpdateCallback(
        const FrameRateLinkerExpectedFpsUpdateCallback& callback) : cb_(callback) {}
    ~CustomFrameRateLinkerExpectedFpsUpdateCallback() override {};

    void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid, int32_t expectedFps) override
    {
        ROSEN_LOGD("CustomFrameRateLinkerExpectedFpsUpdateCallback::OnFrameRateLinkerExpectedFpsUpdate called,"
            " pid=%{public}d, fps=%{public}d", dstPid, expectedFps);
        if (cb_ != nullptr) {
            cb_(dstPid, expectedFps);
        }
    }

private:
    FrameRateLinkerExpectedFpsUpdateCallback cb_;
};

int32_t RSRenderServiceClient::RegisterFrameRateLinkerExpectedFpsUpdateCallback(
    int32_t dstPid, const FrameRateLinkerExpectedFpsUpdateCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterFrameRateLinkerExpectedFpsUpdateCallback renderService == nullptr");
        return RENDER_SERVICE_NULL;
    }

    sptr<CustomFrameRateLinkerExpectedFpsUpdateCallback> cb = nullptr;
    if (callback) {
        cb = new CustomFrameRateLinkerExpectedFpsUpdateCallback(callback);
    }

    ROSEN_LOGD("RSRenderServiceClient::RegisterFrameRateLinkerExpectedFpsUpdateCallback called");
    return renderService->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, cb);
}

void RSRenderServiceClient::SetAppWindowNum(uint32_t num)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetAppWindowNum(num);
    }
}

bool RSRenderServiceClient::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetSystemAnimatedScenes renderService == nullptr!");
        return false;
    }
    bool success;
    renderService->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation, success);
    return success;
}

void RSRenderServiceClient::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ShowWatermark(watermarkImg, isShow);
    }
}

int32_t RSRenderServiceClient::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::ResizeVirtualScreen renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    ROSEN_LOGI("RSRenderServiceClient::ResizeVirtualScreen, width:%{public}u, height:%{public}u", width, height);
    return renderService->ResizeVirtualScreen(id, width, height);
}

void RSRenderServiceClient::ReportJankStats()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ReportJankStats();
    }
}

void RSRenderServiceClient::ReportEventResponse(DataBaseRs info)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ReportEventResponse(info);
    }
}

void RSRenderServiceClient::ReportEventComplete(DataBaseRs info)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ReportEventComplete(info);
    }
}

void RSRenderServiceClient::ReportEventJankFrame(DataBaseRs info)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ReportEventJankFrame(info);
    }
}

void RSRenderServiceClient::ReportRsSceneJankStart(AppInfo info)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ReportRsSceneJankStart(info);
    }
}

void RSRenderServiceClient::ReportRsSceneJankEnd(AppInfo info)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ReportRsSceneJankEnd(info);
    }
}

void RSRenderServiceClient::ReportGameStateData(GameStateData info)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ReportGameStateData(info);
    }
}

void RSRenderServiceClient::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);
    }
}

uint32_t RSRenderServiceClient::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        uint32_t resCode;
        renderService->SetHidePrivacyContent(id, needHidePrivacyContent, resCode);
        return resCode;
    }
    return static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
}

void RSRenderServiceClient::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyLightFactorStatus(lightFactorStatus);
    }
}

void RSRenderServiceClient::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyPackageEvent(listSize, packageList);
    }
}

void RSRenderServiceClient::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
    }
}

void RSRenderServiceClient::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyRefreshRateEvent(eventInfo);
    }
}

void RSRenderServiceClient::SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetWindowExpectedRefreshRate(eventInfos);
    }
}

void RSRenderServiceClient::SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetWindowExpectedRefreshRate(eventInfos);
    }
}

bool RSRenderServiceClient::NotifySoftVsyncRateDiscountEvent(uint32_t pid,
    const std::string &name, uint32_t rateDiscount)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        return renderService->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
    }
    return false;
}

void RSRenderServiceClient::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyHgmConfigEvent(eventName, state);
    }
}

void RSRenderServiceClient::NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
    }
}

void RSRenderServiceClient::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyTouchEvent(touchStatus, touchCnt);
    }
}

void RSRenderServiceClient::NotifyDynamicModeEvent(bool enableDynamicMode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->NotifyDynamicModeEvent(enableDynamicMode);
    }
}

void RSRenderServiceClient::SetCacheEnabledForRotation(bool isEnabled)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetCacheEnabledForRotation(isEnabled);
    }
}

void RSRenderServiceClient::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetOnRemoteDiedCallback(callback);
    }
}

std::vector<ActiveDirtyRegionInfo> RSRenderServiceClient::GetActiveDirtyRegionInfo()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return {};
    }
    return renderService->GetActiveDirtyRegionInfo();
}

GlobalDirtyRegionInfo RSRenderServiceClient::GetGlobalDirtyRegionInfo()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return GlobalDirtyRegionInfo {};
    }
    return renderService->GetGlobalDirtyRegionInfo();
}

LayerComposeInfo RSRenderServiceClient::GetLayerComposeInfo()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return LayerComposeInfo {};
    }
    return renderService->GetLayerComposeInfo();
}

HwcDisabledReasonInfos RSRenderServiceClient::GetHwcDisabledReasonInfo()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return {};
    }
    return renderService->GetHwcDisabledReasonInfo();
}

int64_t RSRenderServiceClient::GetHdrOnDuration()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return 0;
    }
    int64_t hdrOnDuration = 0;
    auto ret = renderService->GetHdrOnDuration(hdrOnDuration);
    if (ret != ERR_OK) {
        ROSEN_LOGE("Failed to get HdrOnDuration, ret=%{public}d", ret);
    }
    return hdrOnDuration;
}

void RSRenderServiceClient::SetVmaCacheStatus(bool flag)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }
    renderService->SetVmaCacheStatus(flag);
}

#ifdef TP_FEATURE_ENABLE
void RSRenderServiceClient::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }
    renderService->SetTpFeatureConfig(feature, config, tpFeatureConfigType);
}
#endif

void RSRenderServiceClient::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
    }
}

void RSRenderServiceClient::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    }
}

void RSRenderServiceClient::DropFrameByPid(const std::vector<int32_t> pidList)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->DropFrameByPid(pidList);
    }
}

class CustomUIExtensionCallback : public RSUIExtensionCallbackStub
{
public:
    explicit CustomUIExtensionCallback(const UIExtensionCallback &callback) : cb_(callback) {}
    ~CustomUIExtensionCallback() override {};

    void OnUIExtension(std::shared_ptr<RSUIExtensionData> uiExtensionData, uint64_t userId) override
    {
        if (cb_ != nullptr) {
            cb_(uiExtensionData, userId);
        }
    }

private:
    UIExtensionCallback cb_;
};

int32_t RSRenderServiceClient::RegisterUIExtensionCallback(uint64_t userId, const UIExtensionCallback& callback,
    bool unobscured)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterUIExtensionCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomUIExtensionCallback> cb = new CustomUIExtensionCallback(callback);
    return renderService->RegisterUIExtensionCallback(userId, cb, unobscured);
}

bool RSRenderServiceClient::SetAncoForceDoDirect(bool direct)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        bool res;
        renderService->SetAncoForceDoDirect(direct, res);
        return res;
    }
    ROSEN_LOGE("RSRenderServiceClient::SetAncoForceDoDirect renderService is null");
    return false;
}

bool RSRenderServiceClient::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    bool success = false;
    if (renderService != nullptr) {
        renderService->SetVirtualScreenStatus(id, screenStatus, success);
    }
    return success;
}

void RSRenderServiceClient::SetFreeMultiWindowStatus(bool enable)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetFreeMultiWindowStatus renderService == nullptr!");
        return;
    }
    renderService->SetFreeMultiWindowStatus(enable);
}

class SurfaceBufferCallbackDirector : public RSSurfaceBufferCallbackStub {
public:
    explicit SurfaceBufferCallbackDirector(RSRenderServiceClient* client) : client_(client) {}
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
    RSRenderServiceClient* client_;
};

bool RSRenderServiceClient::RegisterSurfaceBufferCallback(
    pid_t pid, uint64_t uid, std::shared_ptr<SurfaceBufferCallback> callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSurfaceBufferCallback renderService == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSurfaceBufferCallback callback == nullptr!");
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
        if (surfaceBufferCallbacks_.find(uid) == std::end(surfaceBufferCallbacks_)) {
            surfaceBufferCallbacks_.emplace(uid, callback);
        } else {
            ROSEN_LOGE("RSRenderServiceClient::RegisterSurfaceBufferCallback callback exists"
                " in uid %{public}s", std::to_string(uid).c_str());
            return false;
        }
        if (surfaceBufferCbDirector_ == nullptr) {
            surfaceBufferCbDirector_ = new SurfaceBufferCallbackDirector(this);
        }
    }
    renderService->RegisterSurfaceBufferCallback(pid, uid, surfaceBufferCbDirector_);
    return true;
}

bool RSRenderServiceClient::UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnregisterSurfaceBufferCallback renderService == nullptr!");
        return false;
    }
    {
        std::unique_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
        auto iter = surfaceBufferCallbacks_.find(uid);
        if (iter == std::end(surfaceBufferCallbacks_)) {
            ROSEN_LOGE("RSRenderServiceClient::UnregisterSurfaceBufferCallback invaild uid.");
            return false;
        }
        surfaceBufferCallbacks_.erase(iter);
    }
    renderService->UnregisterSurfaceBufferCallback(pid, uid);
    return true;
}

void RSRenderServiceClient::TriggerOnFinish(const FinishCallbackRet& ret) const
{
    std::shared_ptr<SurfaceBufferCallback> callback = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock { surfaceBufferCallbackMutex_ };
        if (auto iter = surfaceBufferCallbacks_.find(ret.uid); iter != std::cend(surfaceBufferCallbacks_)) {
            callback = iter->second;
        }
    }
    if (!callback) {
        ROSEN_LOGD("RSRenderServiceClient::TriggerOnFinish callback is null");
        return;
    }
    callback->OnFinish(ret);
}

void RSRenderServiceClient::TriggerOnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) const
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

void RSRenderServiceClient::SetLayerTopForHWC(const std::string &nodeIdStr, bool isTop, uint32_t zOrder)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetLayerTopForHWC(nodeIdStr, isTop, zOrder);
    }
}

void RSRenderServiceClient::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetLayerTop(nodeIdStr, isTop);
    }
}

void RSRenderServiceClient::SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetForceRefresh(nodeIdStr, isForceRefresh);
    }
}

class TransactionDataCallbackDirector : public RSTransactionDataCallbackStub {
public:
    explicit TransactionDataCallbackDirector(RSRenderServiceClient* client) : client_(client) {}
    ~TransactionDataCallbackDirector() noexcept override = default;
    void OnAfterProcess(int32_t pid, uint64_t timeStamp) override
    {
        RS_LOGD("OnAfterProcess: TriggerTransactionDataCallbackAndErase, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
        client_->TriggerTransactionDataCallbackAndErase(pid, timeStamp);
    }

private:
    RSRenderServiceClient* client_;
};

bool RSRenderServiceClient::RegisterTransactionDataCallback(int32_t pid, uint64_t timeStamp, std::function<void()> callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterTransactionDataCallback renderService == nullptr!");
        return false;
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterTransactionDataCallback callback == nullptr!");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock{ transactionDataCallbackMutex_ };
        if (transactionDataCallbacks_.find(std::make_pair(pid, timeStamp)) == std::end(transactionDataCallbacks_)) {
            transactionDataCallbacks_.emplace(std::make_pair(pid, timeStamp), callback);
        } else {
            ROSEN_LOGE("RSRenderServiceClient::RegisterTransactionDataCallback callback exists"
                " in timeStamp %{public}s", std::to_string(timeStamp).c_str());
            return false;
        }
        if (transactionDataCbDirector_ == nullptr) {
            transactionDataCbDirector_ = new TransactionDataCallbackDirector(this);
        }
    }
    RS_LOGD("RSRenderServiceClient::RegisterTransactionDataCallback, timeStamp: %{public}"
        PRIu64 " pid: %{public}d", timeStamp, pid);
    renderService->RegisterTransactionDataCallback(pid, timeStamp, transactionDataCbDirector_);
    return true;
}

void RSRenderServiceClient::TriggerTransactionDataCallbackAndErase(int32_t pid, uint64_t timeStamp)
{
    std::function<void()> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock{ transactionDataCallbackMutex_ };
        auto iter = transactionDataCallbacks_.find(std::make_pair(pid, timeStamp));
        if (iter != std::end(transactionDataCallbacks_)) {
            callback = iter->second;
            transactionDataCallbacks_.erase(iter);
        }
    }
    if (callback) {
        RS_LOGD("TriggerTransactionDataCallbackAndErase: invoke callback, timeStamp: %{public}"
            PRIu64 " pid: %{public}d", timeStamp, pid);
        std::invoke(callback);
    }
}

void RSRenderServiceClient::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetColorFollow(nodeIdStr, isColorFollow);
    }
}

void RSRenderServiceClient::NotifyScreenSwitched()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s renderService is nullptr", __func__);
        return;
    }
    renderService->NotifyScreenSwitched();
}

void RSRenderServiceClient::SetWindowContainer(NodeId nodeId, bool value)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetWindowContainer(nodeId, value);
    }
}

class CustomSelfDrawingNodeRectChangeCallback : public RSSelfDrawingNodeRectChangeCallbackStub
{
public:
    explicit CustomSelfDrawingNodeRectChangeCallback(const SelfDrawingNodeRectChangeCallback& callback) : cb_(callback)
    {}
    ~CustomSelfDrawingNodeRectChangeCallback() override {};

    void OnSelfDrawingNodeRectChange(std::shared_ptr<RSSelfDrawingNodeRectData> rectData) override
    {
        if (cb_ != nullptr) {
            cb_(rectData);
        }
    }

private:
    SelfDrawingNodeRectChangeCallback cb_;
};

int32_t RSRenderServiceClient::RegisterSelfDrawingNodeRectChangeCallback(
    const RectConstraint& constraint, const SelfDrawingNodeRectChangeCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSelfDrawingNodeRectChangeCallback renderService == nullptr");
        return RENDER_SERVICE_NULL;
    }

    sptr<CustomSelfDrawingNodeRectChangeCallback> cb = nullptr;
    if (callback) {
        cb = new CustomSelfDrawingNodeRectChangeCallback(callback);
    }

    return renderService->RegisterSelfDrawingNodeRectChangeCallback(constraint, cb);
}

int32_t RSRenderServiceClient::UnRegisterSelfDrawingNodeRectChangeCallback()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnRegisterSelfDrawingNodeRectChangeCallback renderService == nullptr");
        return RENDER_SERVICE_NULL;
    }
    return renderService->UnRegisterSelfDrawingNodeRectChangeCallback();
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
int32_t RSRenderServiceClient::SetOverlayDisplayMode(int32_t mode)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->SetOverlayDisplayMode(mode);
}
#endif

void RSRenderServiceClient::NotifyPageName(const std::string &packageName,
    const std::string &pageName, bool isEnter)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::NotifyPageName renderService == nullptr!");
        return;
    }
    renderService->NotifyPageName(packageName, pageName, isEnter);
}

bool RSRenderServiceClient::GetHighContrastTextState()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        return renderService->GetHighContrastTextState();
    }
    return false;
}

bool RSRenderServiceClient::SetBehindWindowFilterEnabled(bool enabled)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (!renderService) {
        return false;
    }
    auto ret = renderService->SetBehindWindowFilterEnabled(enabled);
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::SetBehindWindowFilterEnabled fail, ret[%{public}d]", ret);
        return false;
    }
    return true;
}

bool RSRenderServiceClient::GetBehindWindowFilterEnabled(bool& enabled)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (!renderService) {
        return false;
    }
    auto ret = renderService->GetBehindWindowFilterEnabled(enabled);
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::GetBehindWindowFilterEnabled fail, ret[%{public}d]", ret);
        return false;
    }
    return true;
}

int32_t RSRenderServiceClient::GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (!renderService) {
        return ERR_INVALID_DATA;
    }
    auto ret = renderService->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::GetPidGpuMemoryInMB fail, ret[%{public}d]", ret);
    }
    return ret;
}
RetCodeHrpService RSRenderServiceClient::ProfilerServiceOpenFile(const HrpServiceDirInfo& dirInfo,
    const std::string& fileName, int32_t flags, int& fd)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RET_HRP_SERVICE_ERR_UNKNOWN;
    }

    if (!HrpServiceValidDirOrFileName(fileName)
        || !HrpServiceValidDirOrFileName(dirInfo.subDir) || !HrpServiceValidDirOrFileName(dirInfo.subDir2)) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }
    fd = -1;
    return renderService->ProfilerServiceOpenFile(dirInfo, fileName, flags, fd);
}

RetCodeHrpService RSRenderServiceClient::ProfilerServicePopulateFiles(const HrpServiceDirInfo& dirInfo,
    uint32_t firstFileIndex, std::vector<HrpServiceFileInfo>& outFiles)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RET_HRP_SERVICE_ERR_UNKNOWN;
    }
    if (!HrpServiceValidDirOrFileName(dirInfo.subDir) || !HrpServiceValidDirOrFileName(dirInfo.subDir2)) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }
    return renderService->ProfilerServicePopulateFiles(dirInfo, firstFileIndex, outFiles);
}

bool RSRenderServiceClient::ProfilerIsSecureScreen()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        return renderService->ProfilerIsSecureScreen();
    }
    return false;
}

} // namespace Rosen
} // namespace OHOS
