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

#include "backend/rs_surface_ohos_gl.h"
#include "backend/rs_surface_ohos_raster.h"
#include "command/rs_command.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#include "ipc_callbacks/buffer_available_callback_stub.h"
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "ipc_callbacks/rs_render_mode_change_callback_stub.h"
#include "platform/common/rs_log.h"
#include "rs_render_service_connect_hub.h"
#include "rs_surface_ohos.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSIRenderClient> RSIRenderClient::CreateRenderServiceClient()
{
    static std::shared_ptr<RSIRenderClient> client = std::make_shared<RSRenderServiceClient>();
    return client;
}

void RSRenderServiceClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->CommitTransaction(transactionData);
    }
}

void RSRenderServiceClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->ExecuteSynchronousTask(task);
    }
}

class CustomRenderModeChangeCallback : public RSRenderModeChangeCallbackStub {
public:
    explicit CustomRenderModeChangeCallback(const RenderModeChangeCallback& callback) : cb_(callback) {}
    ~CustomRenderModeChangeCallback() override {};

    void OnRenderModeChanged(bool isUniRender) override
    {
        if (cb_ != nullptr) {
            cb_(isUniRender);
        }
    }

private:
    RenderModeChangeCallback cb_;
};

int32_t RSRenderServiceClient::SetRenderModeChangeCallback(const RenderModeChangeCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    renderModeChangeCb_ = new CustomRenderModeChangeCallback(callback);
    return renderService->SetRenderModeChangeCallback(renderModeChangeCb_);
}

void RSRenderServiceClient::UpdateRenderMode(bool isUniRender)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->UpdateRenderMode(isUniRender);
    }
}

bool RSRenderServiceClient::GetUniRenderEnabled()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    return renderService->GetUniRenderEnabled();
}

bool RSRenderServiceClient::QueryIfRTNeedRender()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    return renderService->QueryIfRTNeedRender();
}

bool RSRenderServiceClient::CreateNode(const RSSurfaceRenderNodeConfig& config)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }
    return renderService->CreateNode(config);
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return nullptr;
    }
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    return CreateRSSurface(surface);
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateRSSurface(const sptr<Surface> &surface)
{
#ifdef ACE_ENABLE_GL
    // GPU render
    std::shared_ptr<RSSurface> producer = std::make_shared<RSSurfaceOhosGl>(surface);
#else
    // CPU render
    std::shared_ptr<RSSurface> producer = std::make_shared<RSSurfaceOhosRaster>(surface);
#endif
    return producer;
}

std::shared_ptr<VSyncReceiver> RSRenderServiceClient::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return nullptr;
    }
    sptr<IVSyncConnection> conn = renderService->CreateVSyncConnection(name);
    return std::make_shared<VSyncReceiver>(conn, looper, name);
}

void RSRenderServiceClient::TriggerSurfaceCaptureCallback(NodeId id, Media::PixelMap* pixelmap)
{
    ROSEN_LOGI("RSRenderServiceClient::Into TriggerSurfaceCaptureCallback nodeId:[%" PRIu64 "]", id);
    std::shared_ptr<Media::PixelMap> surfaceCapture(pixelmap);
    std::shared_ptr<SurfaceCaptureCallback> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = surfaceCaptureCbMap_.find(id);
        if (iter != surfaceCaptureCbMap_.end()) {
            callback = iter->second;
            surfaceCaptureCbMap_.erase(iter);
        }
    }
    if (callback == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::TriggerSurfaceCaptureCallback: callback is nullptr!");
        return;
    }
    callback->OnSurfaceCapture(surfaceCapture);
}

class SurfaceCaptureCallbackDirector : public RSSurfaceCaptureCallbackStub
{
public:
    explicit SurfaceCaptureCallbackDirector(RSRenderServiceClient* client) : client_(client) {}
    ~SurfaceCaptureCallbackDirector() override {};
    void OnSurfaceCapture(NodeId id, Media::PixelMap* pixelmap) override
    {
        client_->TriggerSurfaceCaptureCallback(id, pixelmap);
    };

private:
    RSRenderServiceClient* client_;
};

bool RSRenderServiceClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    float scaleX, float scaleY)
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
        if (surfaceCaptureCbMap_.count(id) != 0) {
            ROSEN_LOGW("RSRenderServiceClient::TakeSurfaceCapture surfaceCaptureCbMap_.count(id) != 0");
            return false;
        }
        surfaceCaptureCbMap_.emplace(id, callback);
    }

    if (surfaceCaptureCbDirector_ == nullptr) {
        surfaceCaptureCbDirector_ = new SurfaceCaptureCallbackDirector(this);
    }
    renderService->TakeSurfaceCapture(id, surfaceCaptureCbDirector_, scaleX, scaleY);
    return true;
}

int32_t RSRenderServiceClient::SetFocusAppInfo(
    int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }

    return renderService->SetFocusAppInfo(pid, uid, bundleName, abilityName);
}

ScreenId RSRenderServiceClient::GetDefaultScreenId()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    return renderService->GetDefaultScreenId();
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
    int32_t flags)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    return renderService->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
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

class CustomScreenChangeCallback : public RSScreenChangeCallbackStub
{
public:
    explicit CustomScreenChangeCallback(const ScreenChangeCallback &callback) : cb_(callback) {}
    ~CustomScreenChangeCallback() override {};

    void OnScreenChanged(ScreenId id, ScreenEvent event) override
    {
        if (cb_ != nullptr) {
            cb_(id, event);
        }
    }

private:
    ScreenChangeCallback cb_;
};

int32_t RSRenderServiceClient::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
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

void RSRenderServiceClient::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
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

    return renderService->GetScreenActiveMode(id);
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

    return renderService->GetScreenPowerStatus(id);
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
        return INVALID_BACKLIGHT_VALUE;
    }

    return renderService->GetScreenBacklight(id);
}

void RSRenderServiceClient::SetScreenBacklight(ScreenId id, uint32_t level)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
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

bool RSRenderServiceClient::RegisterBufferAvailableListener(
    NodeId id, const BufferAvailableCallback &callback, bool isFromRenderThread)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return false;
    }

    auto iter = isFromRenderThread ? bufferAvailableCbRTMap_.find(id) : bufferAvailableCbUIMap_.find(id);
    if (isFromRenderThread && iter != bufferAvailableCbRTMap_.end()) {
        ROSEN_LOGW("RSRenderServiceClient::RegisterBufferAvailableListener "
                   "Node %" PRIu64 " already, bufferAvailableCbRTMap_", iter->first);
    }

    if (!isFromRenderThread && iter != bufferAvailableCbUIMap_.end()) {
        ROSEN_LOGW("RSRenderServiceClient::RegisterBufferAvailableListener "
                   "Node %" PRIu64 " already, bufferAvailableCbUIMap_", iter->first);
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

bool RSRenderServiceClient::UnregisterBufferAvailableListener(NodeId id)
{
    auto iter = bufferAvailableCbRTMap_.find(id);
    if (iter != bufferAvailableCbRTMap_.end()) {
        bufferAvailableCbRTMap_.erase(iter);
    } else {
        ROSEN_LOGD("RSRenderServiceClient::UnregisterBufferAvailableListener "
                   "Node %" PRIu64 " has not registered RT callback",
            id);
    }
    iter = bufferAvailableCbUIMap_.find(id);
    if (iter != bufferAvailableCbUIMap_.end()) {
        bufferAvailableCbUIMap_.erase(iter);
    } else {
        ROSEN_LOGD("RSRenderServiceClient::UnregisterBufferAvailableListener "
                   "Node %" PRIu64 " has not registered UI callback",
            id);
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

int32_t RSRenderServiceClient::GetScreenType(ScreenId id, RSScreenType& screenType)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::GetScreenType renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    return renderService->GetScreenType(id, screenType);
}

int32_t RSRenderServiceClient::SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return RENDER_SERVICE_NULL;
    }
    return renderService->SetScreenSkipFrameInterval(id, skipFrameInterval);
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
    return renderService->RegisterOcclusionChangeCallback(cb);
}

int32_t RSRenderServiceClient::UnRegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        ROSEN_LOGE("RSRenderServiceClient::UnRegisterOcclusionChangeCallback renderService == nullptr!");
        return RENDER_SERVICE_NULL;
    }
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    return renderService->UnRegisterOcclusionChangeCallback(cb);
}

void RSRenderServiceClient::SetAppWindowNum(uint32_t num)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService != nullptr) {
        renderService->SetAppWindowNum(num);
    }
}
} // namespace Rosen
} // namespace OHOS
