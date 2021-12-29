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

#include "ipc_callbacks/screen_change_callback_stub.h"
#include "rs_render_service_connect.h"
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
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService != nullptr) {
        renderService->CommitTransaction(transactionData);
    }
}

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return nullptr;
    }
    sptr<Surface> surface = renderService->CreateNodeAndSurface(config);
    std::shared_ptr<RSSurface> producer = std::make_shared<RSSurfaceOhos>(surface);
    return producer;
}

ScreenId RSRenderServiceClient::GetDefaultScreenId()
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
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
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return INVALID_SCREEN_ID;
    }

    return renderService->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
}

void RSRenderServiceClient::RemoveVirtualScreen(ScreenId id)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->RemoveVirtualScreen(id);
}

class CustomScreenChangeCallback : public RSScreenChangeCallbackStub
{
public:
    CustomScreenChangeCallback(const ScreenChangeCallback &callback) : cb_(callback) {}
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
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    screenChangeCb_ = new CustomScreenChangeCallback(callback);
    renderService->SetScreenChangeCallback(screenChangeCb_);
}

void RSRenderServiceClient::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->SetScreenActiveMode(id, modeId);
}

void RSRenderServiceClient::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return;
    }

    renderService->SetScreenPowerStatus(id, status);
}

RSScreenModeInfo RSRenderServiceClient::GetScreenActiveMode(ScreenId id)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return RSScreenModeInfo {}; // return empty RSScreenModeInfo.
    }

    return renderService->GetScreenActiveMode(id);
}

std::vector<RSScreenModeInfo> RSRenderServiceClient::GetScreenSupportedModes(ScreenId id)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return {};
    }

    return renderService->GetScreenSupportedModes(id);
}

RSScreenCapability RSRenderServiceClient::GetScreenCapability(ScreenId id)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return RSScreenCapability {};
    }

    return renderService->GetScreenCapability(id);
}

ScreenPowerStatus RSRenderServiceClient::GetScreenPowerStatus(ScreenId id)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return ScreenPowerStatus::INVAILD_POWER_STATUS;
    }

    return renderService->GetScreenPowerStatus(id);
}

RSScreenData RSRenderServiceClient::GetScreenData(ScreenId id)
{
    auto renderService = RSRenderServiceConnect::GetRenderService();
    if (renderService == nullptr) {
        return RSScreenData {};
    }

    return renderService->GetScreenData(id);
}
} // namespace Rosen
} // namespace OHOS
