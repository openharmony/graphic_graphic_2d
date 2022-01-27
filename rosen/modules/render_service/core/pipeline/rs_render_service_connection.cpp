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

#include "rs_render_service_connection.h"

#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_service_listener.h"
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_main_thread.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
// we guarantee that when constructing this object,
// all these pointers are valid, so will not check them.
RSRenderServiceConnection::RSRenderServiceConnection(
    wptr<RSRenderService> renderService,
    RSMainThread* mainThread,
    sptr<RSScreenManager> screenManager,
    sptr<IRemoteObject> token)
    : renderService_(renderService),
      mainThread_(mainThread),
      screenManager_(screenManager),
      token_(token),
      connDeathRecipient_(new RSConnectionDeathRecipient(this)),
      ApplicationDeathRecipient_(new RSApplicationRenderThreadDeathRecipient(this))
{
    if (!token_->AddDeathRecipient(connDeathRecipient_)) {
        ROSEN_LOGW("RSRenderServiceConnection: Failed to set death recipient.");
    }
}

RSRenderServiceConnection::~RSRenderServiceConnection() noexcept
{
    CleanAll();
}

void RSRenderServiceConnection::CleanAll(bool toDelete) noexcept
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (cleanDone_) {
            return;
        }
    }

    ROSEN_LOGD("RSRenderServiceConnection::CleanAll() start.");
    mainThread_->ScheduleTask([this]() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto &id : virtualScreenIds_) {
            screenManager_->RemoveVirtualScreen(id);
        }
        virtualScreenIds_.clear();
        if (screenChangeCallback_ != nullptr) {
            screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
            screenChangeCallback_ = nullptr;
        }
    }).wait();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        cleanDone_ = true;
    }

    if (toDelete) {
        auto renderService = renderService_.promote();
        if (renderService == nullptr) {
            ROSEN_LOGW("RSRenderServiceConnection::CleanAll() RenderService is dead.");
        } else {
            renderService->RemoveConnection(GetToken());
        }
    }

    ROSEN_LOGD("RSRenderServiceConnection::CleanAll() end.");
}

RSRenderServiceConnection::RSConnectionDeathRecipient::RSConnectionDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{
}

void RSRenderServiceConnection::RSConnectionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        ROSEN_LOGW("RSConnectionDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        ROSEN_LOGW("RSConnectionDeathRecipient::OnRemoteDied: RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    if (rsConn->GetToken() != tokenSptr) {
        ROSEN_LOGI("RSConnectionDeathRecipient::OnRemoteDied: token doesn't match, ignore it.");
        return;
    }

    ROSEN_LOGI("RSConnectionDeathRecipient::OnRemoteDied: do the clean work.");
    rsConn->CleanAll(true);
}

RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::RSApplicationRenderThreadDeathRecipient(
    wptr<RSRenderServiceConnection> conn) : conn_(conn)
{}

void RSRenderServiceConnection::RSApplicationRenderThreadDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        ROSEN_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: can't promote remote object.");
        return;
    }

    auto rsConn = conn_.promote();
    if (rsConn == nullptr) {
        ROSEN_LOGW("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: RSRenderServiceConnection was dead, do nothing.");
        return;
    }

    ROSEN_LOGI("RSApplicationRenderThreadDeathRecipient::OnRemoteDied: Unregister.");
    auto app = iface_cast<IApplicationRenderThread>(tokenSptr);
    rsConn->UnregisterApplicationRenderThread(app);
}

void RSRenderServiceConnection::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    mainThread_->RecvRSTransactionData(transactionData);
}

void RSRenderServiceConnection::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    if (task == nullptr) {
        return;
    }

    auto& context = mainThread_->GetContext();
    mainThread_->ScheduleTask([task, &context]() {
        task->Process(context);
    }).wait_for(std::chrono::nanoseconds(task->GetTimeout()));
}

sptr<Surface> RSRenderServiceConnection::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    std::shared_ptr<RSSurfaceRenderNode> node =
        std::make_shared<RSSurfaceRenderNode>(config, mainThread_->GetContext().weak_from_this());
    if (node == nullptr) {
        ROSEN_LOGE("RSRenderService::CreateNodeAndSurface CreateNode fail");
        return nullptr;
    }
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer(config.name);
    if (surface == nullptr) {
        ROSEN_LOGE("RSRenderService::CreateNodeAndSurface get consumer surface fail");
        return nullptr;
    }
    node->SetConsumer(surface);
    std::function<void()> registerNode = [node, this]() -> void {
        this->mainThread_->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    };
    mainThread_->PostTask(registerNode);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(node);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    SurfaceError ret = surface->RegisterConsumerListener(listener);
    if (ret != SURFACE_ERROR_OK) {
        ROSEN_LOGE("RSRenderService::CreateNodeAndSurface Register Consumer Listener fail");
        return nullptr;
    }
    return surface;
}

ScreenId RSRenderServiceConnection::GetDefaultScreenId()
{
    return mainThread_->ScheduleTask([this]() {
        return screenManager_->GetDefaultScreenId();
    }).get();
}

ScreenId RSRenderServiceConnection::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto newVirtualScreenId = screenManager_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
    virtualScreenIds_.insert(newVirtualScreenId);
    return newVirtualScreenId;
}

void RSRenderServiceConnection::RemoveVirtualScreen(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    screenManager_->RemoveVirtualScreen(id);
    virtualScreenIds_.erase(id);
}

void RSRenderServiceConnection::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (screenChangeCallback_ == callback) {
        return;
    }

    if (screenChangeCallback_ != nullptr) {
        // remove the old callback
        screenManager_->RemoveScreenChangeCallback(screenChangeCallback_);
    }

    // update
    screenManager_->AddScreenChangeCallback(callback);
    auto tmp = screenChangeCallback_;
    screenChangeCallback_ = callback;
    lock.unlock();
}

void RSRenderServiceConnection::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenActiveMode(id, modeId);
    }).wait();
}

void RSRenderServiceConnection::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenPowerStatus(id, status);
    }).wait();
}

void RSRenderServiceConnection::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback)
{
    std::function<void()> captureTask = [callback, id]() -> void {
        ROSEN_LOGD("RSRenderService::TakeSurfaceCapture callback->OnSurfaceCapture nodeId:[%llu]", id);
        ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSRenderService::TakeSurfaceCapture");
        RSSurfaceCaptureTask task(id);
        std::unique_ptr<Media::PixelMap> pixelmap = task.Run();
        callback->OnSurfaceCapture(id, pixelmap.get());
        ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    };
    mainThread_->PostTask(captureTask);
}

void RSRenderServiceConnection::RegisterApplicationRenderThread(uint32_t pid, sptr<IApplicationRenderThread> app)
{
    auto captureTask = [=]() -> void {
        mainThread_->RegisterApplicationRenderThread(pid, app);
    };
    mainThread_->PostTask(captureTask);

    app->AsObject()->AddDeathRecipient(ApplicationDeathRecipient_);
}

void RSRenderServiceConnection::UnregisterApplicationRenderThread(sptr<IApplicationRenderThread> app)
{
    auto captureTask = [=]() -> void {
        mainThread_->UnregisterApplicationRenderThread(app);
    };
    mainThread_->PostTask(captureTask);
}

RSScreenModeInfo RSRenderServiceConnection::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    mainThread_->ScheduleTask([=, &screenModeInfo]() {
        return screenManager_->GetScreenActiveMode(id, screenModeInfo);
    }).wait();
    return screenModeInfo;
}

std::vector<RSScreenModeInfo> RSRenderServiceConnection::GetScreenSupportedModes(ScreenId id)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenSupportedModes(id);
    }).get();
}

RSScreenCapability RSRenderServiceConnection::GetScreenCapability(ScreenId id)
{
    RSScreenCapability screenCapability;
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenCapability(id);
    }).get();
}

ScreenPowerStatus RSRenderServiceConnection::GetScreenPowerStatus(ScreenId id)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenPowerStatus(id);
    }).get();
}

RSScreenData RSRenderServiceConnection::GetScreenData(ScreenId id)
{
    RSScreenData screenData;
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenData(id);
    }).get();
}

int32_t RSRenderServiceConnection::GetScreenBacklight(ScreenId id)
{
    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenBacklight(id);
    }).get();
}

void RSRenderServiceConnection::SetScreenBacklight(ScreenId id, uint32_t level)
{
    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenBacklight(id, level);
    }).wait();
}

void RSRenderServiceConnection::RegisterBufferAvailableListener(NodeId id, sptr<RSIBufferAvailableCallback> callback)
{
    std::shared_ptr<RSSurfaceRenderNode> node =
        mainThread_->GetContext().GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id);
    if (node != nullptr) {
        node->RegisterBufferAvailableListener(callback);
    }
}
} // namespace Rosen
} // namespace OHOS
