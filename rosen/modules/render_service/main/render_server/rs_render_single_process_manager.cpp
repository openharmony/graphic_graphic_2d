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
#include "dfx/rs_pipline_dump_manager.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "render_server/transaction/rs_render_to_service_connection.h"
#include "rs_composer_to_render_connection.h"
#include "rs_render_pipeline.h"

#include "rs_render_composer_manager.h"
#include "screen_manager/screen_types.h"
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
        receiver, renderToServiceConnection_, renderService_.rsVsyncManagerAgent_);
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderService_.renderPipeline_);
    serviceToRenderConnection_ = sptr<RSServiceToRenderConnection>::MakeSptr(renderPipelineAgent);
    composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

    // step3:
    connectToRenderConnection_ = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
}

sptr<IRemoteObject> RSSingleRenderProcessManager::OnScreenConnected(ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(property->GetScreenId());
    renderService_.renderPipeline_->OnScreenConnected(property, composerConn, composerToRenderConnection_,
        renderService_.rsVsyncManagerAgent_, output);
    return connectToRenderConnection_->AsObject();
}

void RSSingleRenderProcessManager::OnScreenDisconnected(ScreenId id)
{
    renderService_.renderPipeline_->OnScreenDisconnected(id);
}

void RSSingleRenderProcessManager::OnScreenPropertyChanged(
    ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property)
{
    renderService_.renderPipeline_->OnScreenPropertyChanged(id, type, property);
}
 
void RSSingleRenderProcessManager::OnScreenRefresh(ScreenId id)
{
    renderService_.renderPipeline_->OnScreenRefresh(id);
}

void RSSingleRenderProcessManager::OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
    const sptr<RSScreenProperty>& property)
{
    renderService_.renderPipeline_->OnScreenConnected(property, nullptr, nullptr, nullptr, nullptr);
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
