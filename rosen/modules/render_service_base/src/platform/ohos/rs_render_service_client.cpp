/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return nullptr;
    }
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);

#ifdef ACE_ENABLE_GL
    // GPU render
    std::shared_ptr<RSSurface> producer = std::make_shared<RSSurfaceOhosGl>(surface);
#else
    // CPU render
    std::shared_ptr<RSSurface> producer = std::make_shared<RSSurfaceOhosRaster>(surface);
#endif
    return producer;
}

void RSRenderServiceClient::TriggerSurfaceCaptureCallback(NodeId id, Media::PixelMap* pixelmap)
{
    ROSEN_LOGI("RSRenderServiceClient::Into TriggerSurfaceCaptureCallback nodeId:[%llu]", id);
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

bool RSRenderServiceClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback)
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
    renderService->TakeSurfaceCapture(id, surfaceCaptureCbDirector_);
    return true;
}

ScreenId RSRenderServiceClient::GetDefaultScreenId()
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    return renderService->GetDefaultScreenId();
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

void RSRenderServiceClient::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    screenChangeCb_ = new CustomScreenChangeCallback(callback);
    renderService->SetScreenChangeCallback(screenChangeCb_);
}

void RSRenderServiceClient::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->SetScreenActiveMode(id, modeId);
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
} // namespace Rosen
} // namespace OHOS
