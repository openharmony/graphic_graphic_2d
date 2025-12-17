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

#include "rs_render_single_process_manager.h"

#include "dfx/rs_service_dump_manager.h"
#include "dfx/rs_process_dump_manager.h"
#include "rs_render_pipeline.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "render_server/transaction/rs_render_to_service_connection.h"
#include "rs_composer_to_render_connection.h"
#include "rs_trace.h"

#include "rs_render_to_composer_connection.h"
#include "rs_render_composer_agent.h"
#include "rs_render_composer_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "screen_manager/screen_types.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"

#undef LOG_TAG
#define LOG_TAG "RSSingleRenderProcessManager"

namespace OHOS {
namespace Rosen {
RSSingleRenderProcessManager::RSSingleRenderProcessManager(RSRenderService& renderService) :
    RSRenderProcessManager(renderService)
{
    // step1: Create Vsync Connection and Receiver
    sptr<VSyncIConnectionToken> vsyncToken = new IRemoteStub<VSyncIConnectionToken>();
    sptr<VSyncConnection> conn = new VSyncConnection(renderService.rsVSyncDistributor_, "rs", vsyncToken->AsObject());
    renderService.rsVSyncDistributor_->AddConnection(conn);
    auto receiver = std::make_shared<VSyncReceiver>(conn, vsyncToken->AsObject(), renderService.handler_, "rs");
    receiver->Init();

    // step2: Create renderPipeline and Following Connections
    auto renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(renderService);
    auto renderProcessManagerAgent =
        sptr<RSRenderProcessManagerAgent>::MakeSptr(renderService.renderProcessManager_);
    auto screenManagerAgent = sptr<RSScreenManagerAgent>::MakeSptr(renderService.screenManager_);
    renderToServiceConnection_ =
        sptr<RSRenderToServiceConnection>::MakeSptr(renderServiceAgent, renderProcessManagerAgent, screenManagerAgent);
    renderService.renderPipeline_ = RSRenderPipeline::Create(renderService.handler_,
        receiver, renderToServiceConnection_);
    renderService.renderPipeline_->InitRsVsyncManagerAgent(renderService_.rsVsyncManagerAgent_);
    renderService.renderPipeline_->RegisterScreenSwitchFinishCallback(renderToServiceConnection_);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderService_.renderPipeline_);
    serviceToRenderConnection_ = sptr<RSServiceToRenderConnection>::MakeSptr(renderServiceAgent, renderPipelineAgent);
    composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

    // step3:
    connectToRenderConnection_ = sptr<RSIConnectToRenderProcess>(
        new RSConnectToRenderProcess(renderService.mainThread_, renderPipelineAgent));
}

sptr<IRemoteObject> RSSingleRenderProcessManager::OnScreenConnected(ScreenId screenId, const ScreenEventData& data,
    const sptr<RSScreenProperty>& property)
{
    RSRenderProcessManager::OnScreenConnected(screenId, data, property);

    auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(property->GetScreenId());
    renderService_.rsRenderComposerManager_->SetComposerToRenderConnection(screenId, composerToRenderConnection_);
    std::shared_ptr<RSRenderComposerClient> composerClient = RSRenderComposerClient::Create(false, composerConn);
    composerClient->InitRsVsyncManagerAgent(renderService_.rsVsyncManagerAgent_);
    RSProcessDumpManager::GetInstance().SetRenderToServiceConnection(renderToServiceConnection_);
    renderService_.renderPipeline_->OnScreenConnected(property, composerClient);
    return connectToRenderConnection_->AsObject();
}

void RSSingleRenderProcessManager::OnScreenDisconnected(ScreenId id)
{
    RSRenderProcessManager::OnScreenDisconnected(id);

    renderService_.renderPipeline_->OnScreenDisconnected(id);
}

void RSSingleRenderProcessManager::OnScreenPropertyChanged(ScreenId id, const sptr<RSScreenProperty>& property)
{
    RSRenderProcessManager::OnScreenPropertyChanged(id, property);

    renderService_.renderPipeline_->OnScreenPropertyChanged(property);
}
 
void RSSingleRenderProcessManager::OnScreenRefresh(ScreenId id)
{
    renderService_.renderPipeline_->OnScreenRefresh(id);
}

void RSSingleRenderProcessManager::OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
    const sptr<RSScreenProperty>& property)
{
    renderService_.renderPipeline_->OnScreenConnected(property, nullptr);
}

void RSSingleRenderProcessManager::OnVirtualScreenDisconnected(ScreenId id)
{
    renderService_.renderPipeline_->OnScreenDisconnected(id);
}

sptr<RSIServiceToRenderConnection> RSSingleRenderProcessManager::GetServiceToRenderConn(ScreenId screenId) const
{
    return serviceToRenderConnection_;
}

std::vector<sptr<RSIServiceToRenderConnection>> RSSingleRenderProcessManager::GetServiceToRenderConns() const
{
    return { serviceToRenderConnection_ };
}

sptr<RSIConnectToRenderProcess> RSSingleRenderProcessManager::GetConnectToRenderConnection(ScreenId screenId) const
{
    return connectToRenderConnection_;
}
} // namespace Rosen
} // namespace OHOS
