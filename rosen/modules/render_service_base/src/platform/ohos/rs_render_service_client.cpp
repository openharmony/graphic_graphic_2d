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
#include "ipc_callbacks/brightness_info_change_callback_stub.h"
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->CommitTransaction(transactionData);
    } else {
        RS_LOGE_LIMIT(
            __func__, __line__, "RSRenderServiceClient::CommitTransaction failed, clientToService is nullptr");
    }
}

void RSRenderServiceClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return;
    }

    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ExecuteSynchronousTask(task);
    }
}

bool RSRenderServiceClient::GetUniRenderEnabled()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return false;
    }
    bool enable;
    return clientToService->GetUniRenderEnabled(enable) == ERR_OK && enable;
}

MemoryGraphic RSRenderServiceClient::GetMemoryGraphic(int pid)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return MemoryGraphic {};
    }
    MemoryGraphic memoryGraphic;
    clientToService->GetMemoryGraphic(pid, memoryGraphic);
    return memoryGraphic;
}

std::vector<MemoryGraphic> RSRenderServiceClient::GetMemoryGraphics()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return {};
    }
    std::vector<MemoryGraphic> memoryGraphics;
    clientToService->GetMemoryGraphics(memoryGraphics);
    return memoryGraphics;
}

bool RSRenderServiceClient::GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return false;
    }
    return clientToService->GetTotalAppMemSize(cpuMemSize, gpuMemSize) == ERR_OK;
}

bool RSRenderServiceClient::CreateNode(const RSDisplayNodeConfig& displayNodeConfig, NodeId nodeId)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return false;
    }
    bool success;
    clientToService->CreateNode(displayNodeConfig, nodeId, success);
    return success;
}

bool RSRenderServiceClient::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return false;
    }
    bool success;
    clientToService->CreateNode(config, success);
    return success;
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config,
    bool unobscured)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return nullptr;
    }

    sptr<Surface> surface = nullptr;
    ErrCode err = clientToService->CreateNodeAndSurface(config, surface, unobscured);
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
    std::shared_ptr<RSSurface> producer = nullptr;
#if defined(ACE_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        producer = std::make_shared<RSSurfaceOhosVulkan>(surface); // GPU render
        return producer;
    }
#endif

#if defined(ACE_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        producer = std::make_shared<RSSurfaceOhosGl>(surface); // GPU render
        return producer;
    }
#endif

    // CPU render
    producer = std::make_shared<RSSurfaceOhosRaster>(surface);
    return producer;
}

std::shared_ptr<VSyncReceiver> RSRenderServiceClient::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper,
    uint64_t id,
    NodeId windowNodeId,
    bool fromXcomponent)
{
    ROSEN_LOGD("RSRenderServiceClient::CreateVSyncReceiver Start");
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return nullptr;
    }
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<IVSyncConnection> conn = nullptr;
    VSyncConnParam vsyncConnParam = {id, windowNodeId, fromXcomponent};
    clientToService->CreateVSyncConnection(conn, name, token, vsyncConnParam);
    if (conn == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::CreateVSyncReceiver Failed");
        return nullptr;
    }
    return std::make_shared<VSyncReceiver>(conn, token->AsObject(), looper, name);
}

int32_t RSRenderServiceClient::GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToService->GetPixelMapByProcessId(pixelMapInfoVector, pid, repCode);
    return repCode;
}

std::shared_ptr<Media::PixelMap> RSRenderServiceClient::CreatePixelMapFromSurfaceId(uint64_t surfaceId,
    const Rect &srcRect, bool transformEnabled)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return nullptr;
    }
    sptr<Surface> surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
    if (surface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    return clientToService->CreatePixelMapFromSurface(surface, srcRect, pixelMap,
        transformEnabled) == ERR_OK ? pixelMap : nullptr;
}

ScreenId RSRenderServiceClient::GetDefaultScreenId()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    ScreenId screenId{INVALID_SCREEN_ID};
    clientToService->GetDefaultScreenId(screenId);
    return screenId;
}

ScreenId RSRenderServiceClient::GetActiveScreenId()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    ScreenId screenId{INVALID_SCREEN_ID};
    clientToService->GetActiveScreenId(screenId);
    return screenId;
}

std::vector<ScreenId> RSRenderServiceClient::GetAllScreenIds()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return std::vector<ScreenId>();
    }

    return clientToService->GetAllScreenIds();
}

ScreenId RSRenderServiceClient::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId associatedScreenId,
    int32_t flags,
    std::vector<NodeId> whiteList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s clientToService is null!", __func__);
        return INVALID_SCREEN_ID;
    }

    return clientToService->CreateVirtualScreen(name, width, height, surface, associatedScreenId, flags, whiteList);
}

int32_t RSRenderServiceClient::SetVirtualScreenBlackList(ScreenId id, const std::vector<NodeId>& blackList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s clientToService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetVirtualScreenBlackList(id, blackList);
}

int32_t RSRenderServiceClient::SetVirtualScreenTypeBlackList(ScreenId id, std::vector<NodeType>& typeBlackListVector)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToService->SetVirtualScreenTypeBlackList(id, typeBlackListVector, repCode);
    return repCode;
}

int32_t RSRenderServiceClient::AddVirtualScreenBlackList(ScreenId id, const std::vector<NodeId>& blackList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s clientToService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToService->AddVirtualScreenBlackList(id, blackList, repCode);
    return repCode;
}

int32_t RSRenderServiceClient::RemoveVirtualScreenBlackList(ScreenId id, const std::vector<NodeId>& blackList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s clientToService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToService->RemoveVirtualScreenBlackList(id, blackList, repCode);
    return repCode;
}

int32_t RSRenderServiceClient::AddVirtualScreenWhiteList(ScreenId id, const std::vector<NodeId>& whiteList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s clientToService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToService->AddVirtualScreenWhiteList(id, whiteList, repCode);
    return repCode;
}

int32_t RSRenderServiceClient::RemoveVirtualScreenWhiteList(ScreenId id, const std::vector<NodeId>& whiteList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        RS_LOGE("RSRenderServiceClient::%{public}s clientToService is null!", __func__);
        return RENDER_SERVICE_NULL;
    }
    int32_t repCode;
    clientToService->RemoveVirtualScreenWhiteList(id, whiteList, repCode);
    return repCode;
}

bool RSRenderServiceClient::SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return false;
    }
    bool success;
    clientToService->SetWatermark(name, watermark, success);
    return success;
}

uint32_t RSRenderServiceClient::SetSurfaceWatermark(pid_t pid, const std::string &name,
    const std::shared_ptr<Media::PixelMap> &watermark,
    const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return WATER_MARK_RENDER_SERVICE_NULL;
    }
    return clientToService->SetSurfaceWatermark(pid, name, watermark, nodeIdList, watermarkType);
}
    
void RSRenderServiceClient::ClearSurfaceWatermarkForNodes(pid_t pid, const std::string& name,
    const std::vector<NodeId> &nodeIdList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }
    clientToService->ClearSurfaceWatermarkForNodes(pid, name, nodeIdList);
}
    
void RSRenderServiceClient::ClearSurfaceWatermark(pid_t pid, const std::string &name)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }
    clientToService->ClearSurfaceWatermark(pid, name);
}

int32_t RSRenderServiceClient::SetVirtualScreenSecurityExemptionList(
    ScreenId id,
    const std::vector<NodeId>& securityExemptionList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

int32_t RSRenderServiceClient::SetScreenSecurityMask(ScreenId id,
    std::shared_ptr<Media::PixelMap> securityMask)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetScreenSecurityMask(id, std::move(securityMask));
}

int32_t RSRenderServiceClient::SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect,
    bool supportRotation)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

int32_t RSRenderServiceClient::SetCastScreenEnableSkipWindow(ScreenId id, bool enable)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    ROSEN_LOGI("RSRenderServiceClient::SetCastScreenEnableSkipWindow: id:%{public}" PRIu64 ", enable:%{public}d",
        id, enable);

    return clientToService->SetCastScreenEnableSkipWindow(id, enable);
}

int32_t RSRenderServiceClient::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetVirtualScreenSurface(id, surface);
}

void RSRenderServiceClient::RemoveVirtualScreen(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }

    clientToService->RemoveVirtualScreen(id);
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
int32_t RSRenderServiceClient::SetPointerColorInversionConfig(float darkBuffer, float brightBuffer,
    int64_t interval, int32_t rangeSize)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
 
    return clientToService->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
}
 
int32_t RSRenderServiceClient::SetPointerColorInversionEnabled(bool enable)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
 
    return clientToService->SetPointerColorInversionEnabled(enable);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
 
    sptr<RSIPointerLuminanceChangeCallback> cb = new CustomPointerLuminanceChangeCallback(callback);
    return clientToService->RegisterPointerLuminanceChangeCallback(cb);
}
 
int32_t RSRenderServiceClient::UnRegisterPointerLuminanceChangeCallback()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->UnRegisterPointerLuminanceChangeCallback();
}
#endif

class CustomScreenChangeCallback : public RSScreenChangeCallbackStub
{
public:
    explicit CustomScreenChangeCallback(const ScreenChangeCallback &callback) : cb_(callback) {}
    ~CustomScreenChangeCallback() override {};

    void OnScreenChanged(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason, sptr<IRemoteObject> obj = nullptr) override
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    screenChangeCb_ = new CustomScreenChangeCallback(callback);
    return clientToService->SetScreenChangeCallback(screenChangeCb_);
}

class CustomScreenSwitchingNotifyCallback : public RSScreenSwitchingNotifyCallbackStub
{
public:
    explicit CustomScreenSwitchingNotifyCallback(const ScreenSwitchingNotifyCallback &callback) : cb_(callback) {}
    ~CustomScreenSwitchingNotifyCallback() override {};

    void OnScreenSwitchingNotify(bool status) override
    {
        if (cb_ != nullptr) {
            cb_(status);
        }
    }

private:
    ScreenSwitchingNotifyCallback cb_;
};

int32_t RSRenderServiceClient::SetScreenSwitchingNotifyCallback(const ScreenSwitchingNotifyCallback &callback)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    sptr<CustomScreenSwitchingNotifyCallback> cb = nullptr;
    if (callback) {
        cb = new CustomScreenSwitchingNotifyCallback(callback);
    }

    return clientToService->SetScreenSwitchingNotifyCallback(cb);
}

class CustomBrightnessInfoChangeCallback : public RSBrightnessInfoChangeCallbackStub
{
public:
    explicit CustomBrightnessInfoChangeCallback(const BrightnessInfoChangeCallback& callback) : cb_(callback) {}
    ~CustomBrightnessInfoChangeCallback() override {}

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override
    {
        if (cb_ != nullptr) {
            cb_(screenId, brightnessInfo);
        }
    }

private:
    BrightnessInfoChangeCallback cb_;
};

int32_t RSRenderServiceClient::SetBrightnessInfoChangeCallback(const BrightnessInfoChangeCallback& callback)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    sptr<CustomBrightnessInfoChangeCallback> cb = nullptr;
    if (callback) {
        cb = new CustomBrightnessInfoChangeCallback(callback);
    }

    return clientToService->SetBrightnessInfoChangeCallback(cb);
}

int32_t RSRenderServiceClient::GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is null", __func__);
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetBrightnessInfo(screenId, brightnessInfo);
}

uint32_t RSRenderServiceClient::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return StatusCode::RENDER_SERVICE_NULL;
    }

    return clientToService->SetScreenActiveMode(id, modeId);
}

void RSRenderServiceClient::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return;
    }

    clientToService->SetScreenRefreshRate(id, sceneId, rate);
}

void RSRenderServiceClient::SetRefreshRateMode(int32_t refreshRateMode)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return;
    }

    clientToService->SetRefreshRateMode(refreshRateMode);
}

void RSRenderServiceClient::SyncFrameRateRange(FrameRateLinkerId id,
    const FrameRateRange& range, int32_t animatorExpectedFrameRate)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return;
    }

    return clientToService->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
}

void RSRenderServiceClient::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return;
    }
    clientToService->UnregisterFrameRateLinker(id);
}

uint32_t RSRenderServiceClient::GetScreenCurrentRefreshRate(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return clientToService->GetScreenCurrentRefreshRate(id);
}

int32_t RSRenderServiceClient::GetCurrentRefreshRateMode()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return clientToService->GetCurrentRefreshRateMode();
}

std::vector<int32_t> RSRenderServiceClient::GetScreenSupportedRefreshRates(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return {};
    }

    return clientToService->GetScreenSupportedRefreshRates(id);
}

bool RSRenderServiceClient::GetShowRefreshRateEnabled()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return false;
    }

    bool enable = false;
    clientToService->GetShowRefreshRateEnabled(enable);
    return enable;
}

std::string RSRenderServiceClient::GetRefreshInfo(pid_t pid)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return "";
    }
    std::string enable;
    clientToService->GetRefreshInfo(pid, enable);
    return enable;
}

std::string RSRenderServiceClient::GetRefreshInfoToSP(NodeId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return "";
    }
    std::string enable;
    clientToService->GetRefreshInfoToSP(id, enable);
    return enable;
}

std::string RSRenderServiceClient::GetRefreshInfoByPidAndUniqueId(pid_t pid, uint64_t uniqueId)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return "";
    }
    std::string enable;
    clientToService->GetRefreshInfoByPidAndUniqueId(pid, uniqueId, enable);
    return enable;
}

void RSRenderServiceClient::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return;
    }

    return clientToService->SetShowRefreshRateEnabled(enabled, type);
}

uint32_t RSRenderServiceClient::GetRealtimeRefreshRate(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGW("RSRenderServiceClient clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return clientToService->GetRealtimeRefreshRate(id);
}

int32_t RSRenderServiceClient::SetRogScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("%{public}s: render service is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetRogScreenResolution(id, width, height);
}

int32_t RSRenderServiceClient::GetRogScreenResolution(ScreenId id, uint32_t& width, uint32_t& height)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("%{public}s: render service is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    return clientToService->GetRogScreenResolution(id, width, height);
}

int32_t RSRenderServiceClient::SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("%{public}s: render service is null", __func__);
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetPhysicalScreenResolution(id, width, height);
}

int32_t RSRenderServiceClient::SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualScreenResolution clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    return clientToService->SetVirtualScreenResolution(id, width, height);
}

RSVirtualScreenResolution RSRenderServiceClient::GetVirtualScreenResolution(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RSVirtualScreenResolution {}; // return empty RSVirtualScreenResolution.
    }

    return clientToService->GetVirtualScreenResolution(id);
}

void RSRenderServiceClient::MarkPowerOffNeedProcessOneFrame()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }

    clientToService->MarkPowerOffNeedProcessOneFrame();
}

void RSRenderServiceClient::RepaintEverything()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RepaintEverything clientToService is null, return");
        return;
    }

    clientToService->RepaintEverything();
}

void RSRenderServiceClient::ForceRefreshOneFrameWithNextVSync()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("ForceRefreshOneFrameWithNextVSync clientToService is nullptr, return");
        return;
    }

    clientToService->ForceRefreshOneFrameWithNextVSync();
}

void RSRenderServiceClient::DisablePowerOffRenderControl(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }

    clientToService->DisablePowerOffRenderControl(id);
}

void RSRenderServiceClient::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is nullptr", __func__);
        return;
    }

    clientToService->SetScreenPowerStatus(id, status);
}

int32_t RSRenderServiceClient::SetDualScreenState(ScreenId id, DualScreenStatus status)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is nullptr", __func__);
        return StatusCode::RENDER_SERVICE_NULL;
    }

    return clientToService->SetDualScreenState(id, status);
}

RSScreenModeInfo RSRenderServiceClient::GetScreenActiveMode(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RSScreenModeInfo {}; // return empty RSScreenModeInfo.
    }

    RSScreenModeInfo screenModeInfo;
    clientToService->GetScreenActiveMode(id, screenModeInfo);
    return screenModeInfo;
}

std::vector<RSScreenModeInfo> RSRenderServiceClient::GetScreenSupportedModes(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return {};
    }

    return clientToService->GetScreenSupportedModes(id);
}

RSScreenCapability RSRenderServiceClient::GetScreenCapability(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RSScreenCapability {};
    }

    return clientToService->GetScreenCapability(id);
}

ScreenPowerStatus RSRenderServiceClient::GetScreenPowerStatus(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return ScreenPowerStatus::INVALID_POWER_STATUS;
    }
    uint32_t status {static_cast<int32_t>(ScreenPowerStatus::INVALID_POWER_STATUS)};
    clientToService->GetScreenPowerStatus(id, status);
    return static_cast<ScreenPowerStatus>(status);
}

RSScreenData RSRenderServiceClient::GetScreenData(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RSScreenData {};
    }

    return clientToService->GetScreenData(id);
}

int32_t RSRenderServiceClient::GetScreenBacklight(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is nullptr", __func__);
        return INVALID_BACKLIGHT_VALUE;
    }
    int32_t backLightLevel = INVALID_BACKLIGHT_VALUE;
    clientToService->GetScreenBacklight(id, backLightLevel);
    return backLightLevel;
}

void RSRenderServiceClient::SetScreenBacklight(ScreenId id, uint32_t level)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is nullptr", __func__);
        return;
    }

    clientToService->SetScreenBacklight(id, level);
}

PanelPowerStatus RSRenderServiceClient::GetPanelPowerStatus(ScreenId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is nullptr", __func__);
        return PanelPowerStatus::INVALID_PANEL_POWER_STATUS;
    }

    PanelPowerStatus status{PanelPowerStatus::INVALID_PANEL_POWER_STATUS};
    clientToService->GetPanelPowerStatus(id, status);
    return status;
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto iter = isFromRenderThread ? bufferAvailableCbRTMap_.find(id) : bufferAvailableCbUIMap_.find(id);
    if (isFromRenderThread && iter != bufferAvailableCbRTMap_.end()) {
        HILOG_COMM_WARN("RSRenderServiceClient::RegisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " already, bufferAvailableCbRTMap_", iter->first);
    }

    if (!isFromRenderThread && iter != bufferAvailableCbUIMap_.end()) {
        HILOG_COMM_WARN("RSRenderServiceClient::RegisterBufferAvailableListener "
                   "Node %{public}" PRIu64 " already, bufferAvailableCbUIMap_", iter->first);
        bufferAvailableCbUIMap_.erase(iter);
    }

    sptr<RSIBufferAvailableCallback> bufferAvailableCb = new CustomBufferAvailableCallback(callback);
    clientToService->RegisterBufferAvailableListener(id, bufferAvailableCb, isFromRenderThread);
    if (isFromRenderThread) {
        bufferAvailableCbRTMap_.emplace(id, bufferAvailableCb);
    } else {
        bufferAvailableCbUIMap_.emplace(id, bufferAvailableCb);
    }
    return true;
}

bool RSRenderServiceClient::RegisterBufferClearListener(NodeId id, const BufferClearCallback& callback)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return false;
    }
    sptr<RSIBufferClearCallback> bufferClearCb = new CustomBufferClearCallback(callback);
    clientToService->RegisterBufferClearListener(id, bufferClearCb);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetScreenSupportedColorGamuts(id, mode);
}

int32_t RSRenderServiceClient::GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RequestRotation clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetScreenSupportedMetaDataKeys(id, keys);
}

int32_t RSRenderServiceClient::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetScreenColorGamut(id, mode);
}

int32_t RSRenderServiceClient::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->SetScreenColorGamut(id, modeIdx);
}

int32_t RSRenderServiceClient::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->SetScreenGamutMap(id, mode);
}

int32_t RSRenderServiceClient::SetScreenCorrection(ScreenId id, ScreenRotation screenRotation)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->SetScreenCorrection(id, screenRotation);
}

bool RSRenderServiceClient::SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualMirrorScreenCanvasRotation: clientToService is nullptr");
        return false;
    }
    return clientToService->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

int32_t RSRenderServiceClient::SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualScreenAutoRotation: clientToService is nullptr");
        return RENDER_SERVICE_NULL;
    }
    return clientToService->SetVirtualScreenAutoRotation(id, isAutoRotation);
}

bool RSRenderServiceClient::SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetVirtualMirrorScreenScaleMode: clientToService is nullptr");
        return false;
    }
    return clientToService->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

bool RSRenderServiceClient::SetGlobalDarkColorMode(bool isDark)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetGlobalDarkColorMode: clientToService is nullptr");
        return false;
    }
    return clientToService->SetGlobalDarkColorMode(isDark) == ERR_OK;
}

int32_t RSRenderServiceClient::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetScreenGamutMap(id, mode);
}

int32_t RSRenderServiceClient::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetScreenHDRCapability clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetScreenHDRCapability(id, screenHdrCapability);
}

int32_t RSRenderServiceClient::GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetPixelFormat clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->GetPixelFormat(id, pixelFormat, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetPixelFormat clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->SetPixelFormat(id, pixelFormat, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->GetScreenSupportedHDRFormats(id, hdrFormats, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->GetScreenHDRFormat(id, hdrFormat, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::SetScreenHDRFormat(ScreenId id, int32_t modeIdx)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->SetScreenHDRFormat(id, modeIdx, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenSupportedColorSpaces(
    ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->GetScreenSupportedColorSpaces(id, colorSpaces, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->GetScreenColorSpace(id, colorSpace, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t resCode = SUCCESS;
    clientToService->SetScreenColorSpace(id, colorSpace, resCode);
    return resCode;
}

int32_t RSRenderServiceClient::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetScreenType clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetScreenType(id, screenType);
}

bool RSRenderServiceClient::GetBitmap(NodeId id, Drawing::Bitmap& bitmap)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetBitmap clientToService == nullptr!");
        return false;
    }
    bool success;
    clientToService->GetBitmap(id, bitmap, success);
    return success;
}

bool RSRenderServiceClient::GetPixelmap(NodeId id, std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetPixelmap: clientToService is nullptr");
        return false;
    }
    bool success;
    clientToService->GetPixelmap(id, pixelmap, rect, drawCmdList, success);
    return success;
}

bool RSRenderServiceClient::RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterTypeface: clientToService is nullptr");
        return false;
    }
    uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
    ROSEN_LOGD("RSRenderServiceClient::RegisterTypeface: pid[%{public}d] register typface[%{public}u]",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    // timer: 3s
    OHOS::Rosen::RSXCollie registerTypefaceXCollie("registerTypefaceXCollie_" + typeface->GetFamilyName(), 3);
    return clientToService->RegisterTypeface(globalUniqueId, typeface);
}

int32_t RSRenderServiceClient::RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface, uint32_t index)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterTypeface: clientToService is nullptr");
        return -1;
    }
    uint64_t id = RSTypefaceCache::GenGlobalUniqueId(typeface->GetUniqueID());
    ROSEN_LOGD("RSRenderServiceClient::RegisterTypeface: pid[%{public}d] register typface[%{public}u]",
        RSTypefaceCache::GetTypefacePid(id), RSTypefaceCache::GetTypefaceId(id));
    int32_t needUpdate = 0;
    uint32_t size = typeface->GetSize();
    uint32_t originUniqueId = typeface->GetRawUniqueId();
    Drawing::SharedTypeface sharedTypeface(id, typeface);
    sharedTypeface.originId_ = originUniqueId == 0 ? 0 : RSTypefaceCache::GenGlobalUniqueId(originUniqueId);
    int32_t fd = clientToService->RegisterTypeface(sharedTypeface, needUpdate);
    if (fd != typeface->GetFd() && fd >= 0) {
        auto ashmem = std::make_unique<Ashmem>(fd, size);
        bool mapResult = ashmem->MapReadOnlyAshmem();
        const void* ptr = ashmem->ReadFromAshmem(size, 0);
        if (!mapResult || ptr == nullptr) {
            RS_LOGE("Failed to update ashmem: %{public}d -> %{public}d", typeface->GetFd(), fd);
            return fd;
        }
        auto stream = std::make_unique<Drawing::MemoryStream>(
            ptr, size, [](const void* ptr, void* context) { delete reinterpret_cast<Ashmem*>(context); },
            ashmem.release());
        typeface->UpdateStream(std::move(stream));
        typeface->SetFd(fd);
    }
    return fd;
}

bool RSRenderServiceClient::UnRegisterTypeface(uint32_t uniqueId)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnRegisterTypeface: clientToService is nullptr");
        return false;
    }
    uint64_t globalUniqueId = RSTypefaceCache::GenGlobalUniqueId(uniqueId);
    ROSEN_LOGD("RSRenderServiceClient::UnRegisterTypeface: pid[%{public}d] unregister typface[%{public}u]",
        RSTypefaceCache::GetTypefacePid(globalUniqueId), RSTypefaceCache::GetTypefaceId(globalUniqueId));
    return clientToService->UnRegisterTypeface(globalUniqueId);
}

int32_t RSRenderServiceClient::GetDisplayIdentificationData(ScreenId id, uint8_t& outPort,
    std::vector<uint8_t>& edidData)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->GetDisplayIdentificationData(id, outPort, edidData);
}

int32_t RSRenderServiceClient::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    int32_t statusCode = SUCCESS;
    clientToService->SetScreenSkipFrameInterval(id, skipFrameInterval, statusCode);
    return statusCode;
}

int32_t RSRenderServiceClient::SetVirtualScreenRefreshRate(
    ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    int32_t resCode = RENDER_SERVICE_NULL;
    if (clientToService != nullptr) {
        clientToService->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate, resCode);
    }
    return resCode;
}

uint32_t RSRenderServiceClient::SetScreenActiveRect(ScreenId id, const Rect& activeRect)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    uint32_t repCode;
    return clientToService->SetScreenActiveRect(id, activeRect, repCode);
    return repCode;
}

void RSRenderServiceClient::SetScreenOffset(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }
    clientToService->SetScreenOffset(id, offsetX, offsetY);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterOcclusionChangeCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    int32_t repCode;
    clientToService->RegisterOcclusionChangeCallback(cb, repCode);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSurfaceOcclusionChangeCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomSurfaceOcclusionChangeCallback> cb = new CustomSurfaceOcclusionChangeCallback(callback);
    return clientToService->RegisterSurfaceOcclusionChangeCallback(id, cb, partitionPoints);
}

int32_t RSRenderServiceClient::UnRegisterSurfaceOcclusionChangeCallback(NodeId id)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnRegisterSurfaceOcclusionChangeCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return clientToService->UnRegisterSurfaceOcclusionChangeCallback(id);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterHgmConfigChangeCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomHgmConfigChangeCallback> cb = new CustomHgmConfigChangeCallback(callback);
    return clientToService->RegisterHgmConfigChangeCallback(cb);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterHgmRefreshRateModeChangeCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomHgmRefreshRateModeChangeCallback> cb = new CustomHgmRefreshRateModeChangeCallback(callback);
    return clientToService->RegisterHgmRefreshRateModeChangeCallback(cb);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterHgmRefreshRateUpdateCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    if (callback) {
        cb = new CustomHgmRefreshRateUpdateCallback(callback);
    }

    ROSEN_LOGD("RSRenderServiceClient::RegisterHgmRefreshRateUpdateCallback called");
    return clientToService->RegisterHgmRefreshRateUpdateCallback(cb);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterFirstFrameCommitCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    if (callback) {
        cb = new CustomFirstFrameCommitCallback(callback);
    }

    ROSEN_LOGD("RSRenderServiceClient::RegisterFirstFrameCommitCallback called");
    return clientToService->RegisterFirstFrameCommitCallback(cb);
}

class CustomFrameRateLinkerExpectedFpsUpdateCallback : public RSFrameRateLinkerExpectedFpsUpdateCallbackStub
{
public:
    explicit CustomFrameRateLinkerExpectedFpsUpdateCallback(
        const FrameRateLinkerExpectedFpsUpdateCallback& callback) : cb_(callback) {}
    ~CustomFrameRateLinkerExpectedFpsUpdateCallback() override {};

    void OnFrameRateLinkerExpectedFpsUpdate(pid_t dstPid, const std::string& xcomponentId, int32_t expectedFps) override
    {
        ROSEN_LOGD("CustomFrameRateLinkerExpectedFpsUpdateCallback::OnFrameRateLinkerExpectedFpsUpdate called,"
            " pid=%{public}d, fps=%{public}d", dstPid, expectedFps);
        if (cb_ != nullptr) {
            cb_(dstPid, xcomponentId, expectedFps);
        }
    }
private:
    FrameRateLinkerExpectedFpsUpdateCallback cb_;
};

int32_t RSRenderServiceClient::RegisterFrameRateLinkerExpectedFpsUpdateCallback(
    int32_t dstPid, const FrameRateLinkerExpectedFpsUpdateCallback& callback)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterFrameRateLinkerExpectedFpsUpdateCallback "
            "clientToService == nullptr");
        return RENDER_SERVICE_NULL;
    }

    sptr<CustomFrameRateLinkerExpectedFpsUpdateCallback> cb = nullptr;
    if (callback) {
        cb = new CustomFrameRateLinkerExpectedFpsUpdateCallback(callback);
    }

    ROSEN_LOGD("RSRenderServiceClient::RegisterFrameRateLinkerExpectedFpsUpdateCallback called");
    return clientToService->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, cb);
}

bool RSRenderServiceClient::SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetSystemAnimatedScenes clientToService == nullptr!");
        return false;
    }
    bool success;
    clientToService->SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation, success);
    return success;
}

void RSRenderServiceClient::ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ShowWatermark(watermarkImg, isShow);
    }
}

int32_t RSRenderServiceClient::ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::ResizeVirtualScreen clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }

    ROSEN_LOGI("RSRenderServiceClient::ResizeVirtualScreen, width:%{public}u, height:%{public}u", width, height);
    return clientToService->ResizeVirtualScreen(id, width, height);
}

void RSRenderServiceClient::ReportJankStats()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ReportJankStats();
    }
}

void RSRenderServiceClient::ReportEventResponse(DataBaseRs info)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ReportEventResponse(info);
    }
}

void RSRenderServiceClient::ReportEventComplete(DataBaseRs info)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ReportEventComplete(info);
    }
}

void RSRenderServiceClient::ReportEventJankFrame(DataBaseRs info)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ReportEventJankFrame(info);
    }
}

void RSRenderServiceClient::ReportRsSceneJankStart(AppInfo info)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ReportRsSceneJankStart(info);
    }
}

void RSRenderServiceClient::ReportRsSceneJankEnd(AppInfo info)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ReportRsSceneJankEnd(info);
    }
}

void RSRenderServiceClient::ReportGameStateData(GameStateData info)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->ReportGameStateData(info);
    }
}

void RSRenderServiceClient::SetHardwareEnabled(NodeId id, bool isEnabled, SelfDrawingNodeType selfDrawingType,
    bool dynamicHardwareEnable)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);
    }
}

uint32_t RSRenderServiceClient::SetHidePrivacyContent(NodeId id, bool needHidePrivacyContent)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        uint32_t resCode;
        clientToService->SetHidePrivacyContent(id, needHidePrivacyContent, resCode);
        return resCode;
    }
    return static_cast<uint32_t>(RSInterfaceErrorCode::UNKNOWN_ERROR);
}

void RSRenderServiceClient::NotifyLightFactorStatus(int32_t lightFactorStatus)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyLightFactorStatus(lightFactorStatus);
    }
}

void RSRenderServiceClient::NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyPackageEvent(listSize, packageList);
    }
}

void RSRenderServiceClient::NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
    }
}

void RSRenderServiceClient::NotifyRefreshRateEvent(const EventInfo& eventInfo)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyRefreshRateEvent(eventInfo);
    }
}

void RSRenderServiceClient::SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetWindowExpectedRefreshRate(eventInfos);
    }
}

void RSRenderServiceClient::SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetWindowExpectedRefreshRate(eventInfos);
    }
}

bool RSRenderServiceClient::NotifySoftVsyncRateDiscountEvent(uint32_t pid,
    const std::string &name, uint32_t rateDiscount)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        return clientToService->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
    }
    return false;
}

void RSRenderServiceClient::NotifyHgmConfigEvent(const std::string &eventName, bool state)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyHgmConfigEvent(eventName, state);
    }
}

void RSRenderServiceClient::NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
    }
}

void RSRenderServiceClient::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt, int32_t sourceType)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyTouchEvent(touchStatus, touchCnt, sourceType);
    }
}

void RSRenderServiceClient::NotifyDynamicModeEvent(bool enableDynamicMode)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->NotifyDynamicModeEvent(enableDynamicMode);
    }
}

void RSRenderServiceClient::SetCacheEnabledForRotation(bool isEnabled)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetCacheEnabledForRotation(isEnabled);
    }
}

void RSRenderServiceClient::SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetOnRemoteDiedCallback(callback);
    }
}

std::vector<ActiveDirtyRegionInfo> RSRenderServiceClient::GetActiveDirtyRegionInfo()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return {};
    }
    return clientToService->GetActiveDirtyRegionInfo();
}

GlobalDirtyRegionInfo RSRenderServiceClient::GetGlobalDirtyRegionInfo()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return GlobalDirtyRegionInfo {};
    }
    return clientToService->GetGlobalDirtyRegionInfo();
}

LayerComposeInfo RSRenderServiceClient::GetLayerComposeInfo()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return LayerComposeInfo {};
    }
    return clientToService->GetLayerComposeInfo();
}

HwcDisabledReasonInfos RSRenderServiceClient::GetHwcDisabledReasonInfo()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return {};
    }
    return clientToService->GetHwcDisabledReasonInfo();
}

int64_t RSRenderServiceClient::GetHdrOnDuration()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return 0;
    }
    int64_t hdrOnDuration = 0;
    auto ret = clientToService->GetHdrOnDuration(hdrOnDuration);
    if (ret != ERR_OK) {
        ROSEN_LOGE("Failed to get HdrOnDuration, ret=%{public}d", ret);
    }
    return hdrOnDuration;
}

void RSRenderServiceClient::SetVmaCacheStatus(bool flag)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }
    clientToService->SetVmaCacheStatus(flag);
}

#ifdef TP_FEATURE_ENABLE
void RSRenderServiceClient::SetTpFeatureConfig(int32_t feature, const char* config,
    TpFeatureConfigType tpFeatureConfigType)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return;
    }
    clientToService->SetTpFeatureConfig(feature, config, tpFeatureConfigType);
}
#endif

void RSRenderServiceClient::SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
    }
}

void RSRenderServiceClient::SetCurtainScreenUsingStatus(bool isCurtainScreenOn)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetCurtainScreenUsingStatus(isCurtainScreenOn);
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterUIExtensionCallback clientToService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomUIExtensionCallback> cb = new CustomUIExtensionCallback(callback);
    return clientToService->RegisterUIExtensionCallback(userId, cb, unobscured);
}

bool RSRenderServiceClient::SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    bool success = false;
    if (clientToService != nullptr) {
        clientToService->SetVirtualScreenStatus(id, screenStatus, success);
    }
    return success;
}

void RSRenderServiceClient::SetFreeMultiWindowStatus(bool enable)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::SetFreeMultiWindowStatus clientToService == nullptr!");
        return;
    }
    clientToService->SetFreeMultiWindowStatus(enable);
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

void RSRenderServiceClient::SetLayerTop(const std::string &nodeIdStr, bool isTop)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetLayerTop(nodeIdStr, isTop);
    }
}

void RSRenderServiceClient::SetForceRefresh(const std::string& nodeIdStr, bool isForceRefresh)
{
    if (auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection()) {
        clientToService->SetForceRefresh(nodeIdStr, isForceRefresh);
    }
}

class TransactionDataCallbackDirector : public RSTransactionDataCallbackStub {
public:
    explicit TransactionDataCallbackDirector(RSRenderServiceClient* client) : client_(client) {}
    ~TransactionDataCallbackDirector() noexcept override = default;
    void OnAfterProcess(uint64_t token, uint64_t timeStamp) override
    {
        RS_LOGD("OnAfterProcess: TriggerTransactionDataCallbackAndErase, timeStamp: %{public}"
            PRIu64 " token: %{public}" PRIu64, timeStamp, token);
        client_->TriggerTransactionDataCallbackAndErase(token, timeStamp);
    }

private:
    RSRenderServiceClient* client_;
};

void RSRenderServiceClient::TriggerTransactionDataCallbackAndErase(uint64_t token, uint64_t timeStamp)
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

void RSRenderServiceClient::SetColorFollow(const std::string &nodeIdStr, bool isColorFollow)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        clientToService->SetColorFollow(nodeIdStr, isColorFollow);
    }
}

void RSRenderServiceClient::NotifyScreenSwitched()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::%{public}s clientToService is nullptr", __func__);
        return;
    }
    clientToService->NotifyScreenSwitched();
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
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::RegisterSelfDrawingNodeRectChangeCallback clientToService == nullptr");
        return RENDER_SERVICE_NULL;
    }

    sptr<CustomSelfDrawingNodeRectChangeCallback> cb = nullptr;
    if (callback) {
        cb = new CustomSelfDrawingNodeRectChangeCallback(callback);
    }

    return clientToService->RegisterSelfDrawingNodeRectChangeCallback(constraint, cb);
}

int32_t RSRenderServiceClient::UnRegisterSelfDrawingNodeRectChangeCallback()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnRegisterSelfDrawingNodeRectChangeCallback clientToService == nullptr");
        return RENDER_SERVICE_NULL;
    }
    return clientToService->UnRegisterSelfDrawingNodeRectChangeCallback();
}

void RSRenderServiceClient::NotifyPageName(const std::string &packageName,
    const std::string &pageName, bool isEnter)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::NotifyPageName clientToService == nullptr!");
        return;
    }
    clientToService->NotifyPageName(packageName, pageName, isEnter);
}

bool RSRenderServiceClient::GetHighContrastTextState()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        return clientToService->GetHighContrastTextState();
    }
    return false;
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
int32_t RSRenderServiceClient::SetOverlayDisplayMode(int32_t mode)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return clientToService->SetOverlayDisplayMode(mode);
}
#endif

bool RSRenderServiceClient::SetBehindWindowFilterEnabled(bool enabled)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (!clientToService) {
        return false;
    }
    auto ret = clientToService->SetBehindWindowFilterEnabled(enabled);
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::SetBehindWindowFilterEnabled fail, ret[%{public}d]", ret);
        return false;
    }
    return true;
}

bool RSRenderServiceClient::GetBehindWindowFilterEnabled(bool& enabled)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (!clientToService) {
        return false;
    }
    auto ret = clientToService->GetBehindWindowFilterEnabled(enabled);
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::GetBehindWindowFilterEnabled fail, ret[%{public}d]", ret);
        return false;
    }
    return true;
}

int32_t RSRenderServiceClient::GetPidGpuMemoryInMB(pid_t pid, float& gpuMemInMB)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (!clientToService) {
        return ERR_INVALID_DATA;
    }
    auto ret = clientToService->GetPidGpuMemoryInMB(pid, gpuMemInMB);
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::GetPidGpuMemoryInMB fail, ret[%{public}d]", ret);
    }
    return ret;
}
RetCodeHrpService RSRenderServiceClient::ProfilerServiceOpenFile(const HrpServiceDirInfo& dirInfo,
    const std::string& fileName, int32_t flags, int& fd)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RET_HRP_SERVICE_ERR_UNKNOWN;
    }

    if (!HrpServiceValidDirOrFileName(fileName)
        || !HrpServiceValidDirOrFileName(dirInfo.subDir) || !HrpServiceValidDirOrFileName(dirInfo.subDir2)) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }
    fd = -1;
    return clientToService->ProfilerServiceOpenFile(dirInfo, fileName, flags, fd);
}

RetCodeHrpService RSRenderServiceClient::ProfilerServicePopulateFiles(const HrpServiceDirInfo& dirInfo,
    uint32_t firstFileIndex, std::vector<HrpServiceFileInfo>& outFiles)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService == nullptr) {
        return RET_HRP_SERVICE_ERR_UNKNOWN;
    }
    if (!HrpServiceValidDirOrFileName(dirInfo.subDir) || !HrpServiceValidDirOrFileName(dirInfo.subDir2)) {
        return RET_HRP_SERVICE_ERR_INVALID_PARAM;
    }
    return clientToService->ProfilerServicePopulateFiles(dirInfo, firstFileIndex, outFiles);
}

bool RSRenderServiceClient::ProfilerIsSecureScreen()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (clientToService != nullptr) {
        return clientToService->ProfilerIsSecureScreen();
    }
    return false;
}

void RSRenderServiceClient::AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (!clientToService) {
        ROSEN_LOGE("RSRenderServiceClient::AvcodecVideoStart clientToService == nullptr!");
        return;
    }
    clientToService->AvcodecVideoStart(uniqueIdList, surfaceNameList, fps, reportTime);
}

void RSRenderServiceClient::AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
    const std::vector<std::string>& surfaceNameList, uint32_t fps)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (!clientToService) {
        ROSEN_LOGE("RSRenderServiceClient::AvcodecVideoStop clientToService == nullptr!");
        return;
    }
    clientToService->AvcodecVideoStop(uniqueIdList, surfaceNameList, fps);
}

bool RSRenderServiceClient::AvcodecVideoGet(uint64_t uniqueId)
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (!clientToService) {
        ROSEN_LOGE("RSRenderServiceClient::AvcodecVideoGet clientToService == nullptr!");
        return false;
    }
    auto ret = clientToService->AvcodecVideoGet(uniqueId);
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::AvcodecVideoGet fail, ret[%{public}d]", ret);
        return false;
    }
    return true;
}
 
bool RSRenderServiceClient::AvcodecVideoGetRecent()
{
    auto clientToService = RSRenderServiceConnectHub::GetClientToServiceConnection();
    if (!clientToService) {
        ROSEN_LOGE("RSRenderServiceClient::AvcodecVideoGetRecent clientToService == nullptr!");
        return false;
    }
    auto ret = clientToService->AvcodecVideoGetRecent();
    if (ret != ERR_OK) {
        ROSEN_LOGE("RSRenderServiceClient::AvcodecVideoGetRecent fail, ret[%{public}d]", ret);
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
