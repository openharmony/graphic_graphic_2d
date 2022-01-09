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

#include "pipeline/rs_render_service.h"

#include <unordered_set>
#include <unistd.h>

#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "command/rs_command.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_service_listener.h"
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
RSRenderService& RSRenderService::GetInstance()
{
    static RSRenderService instance;
    return instance;
}

RSRenderService::RSRenderService() {}

RSRenderService::~RSRenderService() noexcept {}

bool RSRenderService::Init()
{
    mainThread_ = RSMainThread::Instance();

    screenManager_ = CreateOrGetScreenManager();
    if (screenManager_ == nullptr || !screenManager_->Init()) {
        ROSEN_LOGI("RSRenderService CreateOrGetScreenManager fail");
        return false;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    samgr->AddSystemAbility(RENDER_SERVICE, &GetInstance());
    return true;
}

void RSRenderService::Run()
{
    ROSEN_LOGE("RSRenderService::Run");
    mainThread_->Start();
}

int RSRenderService::Dump(int fd, const std::vector<std::u16string>& args)
{
    std::unordered_set<std::u16string> argSets;
    std::u16string arg1(u"display");
    std::u16string arg2(u"surface");
    for (decltype(args.size()) index = 0; index < args.size(); ++index) {
        argSets.insert(args[index]);
    }
    std::string dumpString;
    if (screenManager_ == nullptr) {
        return OHOS::INVALID_OPERATION;
    }
    if (args.size() == 0 || argSets.count(arg1) != 0) {
        screenManager_->DisplayDump(dumpString);
    }
    if (args.size() == 0 || argSets.count(arg2) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            return screenManager_->SurfaceDump(dumpString);
        }).wait();
    }
    if (dumpString.size() == 0) {
        return OHOS::INVALID_OPERATION;
    }
    write(fd, dumpString.c_str(), dumpString.size());
    return OHOS::NO_ERROR;
}

void RSRenderService::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
    mainThread_->RecvRSTransactionData(transactionData);
}

void RSRenderService::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    std::condition_variable cv;
    auto& mainThread = mainThread_;
    mainThread->PostTask([task, &cv, &mainThread] {
        if (task == nullptr) {
            return;
        }
        task->Process(mainThread->GetContext());
        cv.notify_all();
    });
    cv.wait_for(lock, std::chrono::nanoseconds(task->GetTimeout()));
}

sptr<Surface> RSRenderService::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(config);
    if (node == nullptr) {
        ROSEN_LOGE("RSRenderService::CreateNodeAndSurface CreateNode fail");
        return nullptr;
    }
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
    if (surface == nullptr) {
        ROSEN_LOGE("RSRenderService::CreateNodeAndSurface get consumer surface fail");
        return nullptr;
    }
    node->SetConsumer(surface);
    std::function<void()> registerNode = [node, this]() -> void {
        this->mainThread_->GetContext().GetNodeMap().RegisterRenderNode(node);
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

ScreenId RSRenderService::GetDefaultScreenId()
{
    if (screenManager_ == nullptr) {
        return INVALID_SCREEN_ID;
    }

    return mainThread_->ScheduleTask([this]() {
        return screenManager_->GetDefaultScreenId();
    }).get();
}

ScreenId RSRenderService::CreateVirtualScreen(
    const std::string &name,
    uint32_t width,
    uint32_t height,
    sptr<Surface> surface,
    ScreenId mirrorId,
    int32_t flags)
{
    if (screenManager_ == nullptr) {
        return INVALID_SCREEN_ID;
    }

    return mainThread_->ScheduleTask([=]() {
        return screenManager_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
    }).get();
}

void RSRenderService::RemoveVirtualScreen(ScreenId id)
{
    if (screenManager_ == nullptr) {
        return;
    }

    mainThread_->ScheduleTask([=]() {
        screenManager_->RemoveVirtualScreen(id);
    }).wait();
}

void RSRenderService::SetScreenChangeCallback(sptr<RSIScreenChangeCallback> callback)
{
    if (screenManager_ == nullptr) {
        return;
    }

    mainThread_->ScheduleTask([=]() {
        screenManager_->AddScreenChangeCallback(callback);
    }).wait();
}

void RSRenderService::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
    if (screenManager_ == nullptr) {
        return;
    }

    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenActiveMode(id, modeId);
    }).wait();
}

void RSRenderService::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    if (screenManager_ == nullptr) {
        return;
    }

    mainThread_->ScheduleTask([=]() {
        screenManager_->SetScreenPowerStatus(id, status);
    }).wait();
}

void RSRenderService::TakeSurfaceCapture(NodeId id, sptr<RSISurfaceCaptureCallback> callback)
{
    std::function<void()> captureTask = [callback, id]() -> void {
        RSSurfaceCaptureTask task(id);
        std::unique_ptr<Media::PixelMap> pixelmap = task.Run();
        callback->OnSurfaceCapture(id, pixelmap.get());
    };
    mainThread_->PostTask(captureTask);
}

RSScreenModeInfo RSRenderService::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    if (screenManager_ == nullptr) {
        return screenModeInfo;
    }

    mainThread_->ScheduleTask([=, &screenModeInfo]() {
        return screenManager_->GetScreenActiveMode(id, screenModeInfo);
    }).wait();
    return screenModeInfo;
}

std::vector<RSScreenModeInfo> RSRenderService::GetScreenSupportedModes(ScreenId id)
{
    if (screenManager_ == nullptr) {
        return {};
    }

    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenSupportedModes(id);
    }).get();
}

RSScreenCapability RSRenderService::GetScreenCapability(ScreenId id)
{
    RSScreenCapability screenCapability;
    if (screenManager_ == nullptr) {
        return screenCapability;
    }

    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenCapability(id);
    }).get();
}

ScreenPowerStatus RSRenderService::GetScreenPowerStatus(ScreenId id)
{
    if (screenManager_ == nullptr) {
        return INVALID_POWER_STATUS;
    }

    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenPowerStatus(id);
    }).get();
}

RSScreenData RSRenderService::GetScreenData(ScreenId id)
{
    RSScreenData screenData;
    if (screenManager_ == nullptr) {
        return screenData;
    }

    return mainThread_->ScheduleTask([=]() {
        return screenManager_->GetScreenData(id);
    }).get();
}
} // namespace Rosen
} // namespace OHOS
