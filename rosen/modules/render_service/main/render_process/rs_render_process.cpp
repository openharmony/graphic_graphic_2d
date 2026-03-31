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

#include "rs_render_process.h"

#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <malloc.h>
#include <memory>
#include <system_ability_definition.h>

#include "irs_render_to_composer_connection.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/transaction/rs_render_connect_parcel_info.h"
#include "rs_composer_to_render_connection.h"
#include "rs_ipc_replayer.h"
#include "transaction/rs_connect_to_render_process.h"
#include "transaction/rs_service_to_render_connection.h"
#include "xcollie/watchdog.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcess"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t WATCHDOG_TIMEVAL = 5000;
constexpr int32_t MAX_TRY_GET_SA = 5;

sptr<RSIRenderService> ConnectToServer()
{
    sptr<RSIRenderService> renderService = nullptr;
    int tryCnt = 0;
    for (; tryCnt < MAX_TRY_GET_SA; tryCnt++) {
        // sleep move time (1000us * tryCnt) when tryCnt++
        usleep(1000 * tryCnt);

        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!samgr) {
            RS_LOGE("%{public}s: samgr is null", __func__);
            continue;
        }
        auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE); // chenke fix
        if (!remoteObject) {
            RS_LOGE("%{public}s: remoteObject is null", __func__);
            continue;
        }
        renderService = iface_cast<RSIRenderService>(remoteObject);
        if (renderService) {
            break;
        }
    }
    // try most 5 times to get render service.
    if (tryCnt >= MAX_TRY_GET_SA) {
        RS_LOGE("%{public}s: tried 5 times.", __func__);
    }
    if (!renderService) {
        RS_LOGE("%{public}s: failed to get render service proxy", __func__);
    }
    return renderService;
}

bool IsInvalidReplyInfo(const sptr<ReplyToRenderInfo>& result)
{
    return !result ||
           !result->composeConnection_ ||
           !result->rsScreenProperty_ ||
           !result->vsyncConn_ ||
           !result->replayData_;
}
}

bool RSRenderProcess::Init()
{
    RS_LOGI("%{public}s", __func__);
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    if (HiviewDFX::Watchdog::GetInstance().AddThread("RenderProcess", handler_, WATCHDOG_TIMEVAL) != 0) {
        RS_LOGW("%{public}s: Add watchdog thread failed", __func__);
    }

    // enable tcache
    if (RSSystemParameters::GetTcacheEnabled()) {
        mallopt(M_OHOS_CONFIG, M_TCACHE_NORMAL_MODE);
        mallopt(M_OHOS_CONFIG, M_ENABLE_OPT_TCACHE);
        mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
        mallopt(M_DELAYED_FREE, M_DELAYED_FREE_ENABLE);
    }

    // register the child process to the parent process
    RS_LOGI("%{public}s: Subprocess Registration", __func__);
    auto composerToRenderConn = sptr<RSComposerToRenderConnection>::MakeSptr();
    auto renderPipelineAgent = sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline_);
    sptr<VSyncIConnectionToken> vsyncToken = new IRemoteStub<VSyncIConnectionToken>();
    auto replyToRenderInfo = ConnectToRenderService(composerToRenderConn, renderPipelineAgent, vsyncToken);
    if (!replyToRenderInfo) {
        RS_LOGE("%{public}s: replyToRenderInfo is nullptr", __func__);
        return false;
    }
    renderToServiceConnection_ = iface_cast<RSIRenderToServiceConnection>(replyToRenderInfo->serviceConnection_);
    auto renderToComposerConn = iface_cast<IRSRenderToComposerConnection>(replyToRenderInfo->composeConnection_);
    auto& rsScreenProperty = replyToRenderInfo->rsScreenProperty_;
    auto vsyncConn = iface_cast<IVSyncConnection>(replyToRenderInfo->vsyncConn_);
    auto receiver = std::make_shared<VSyncReceiver>(vsyncConn, vsyncToken->AsObject(), handler_, "render_process");
    receiver->Init();

    // create render pipeline
    renderPipeline_ = RSRenderPipeline::Create(handler_, receiver, renderToServiceConnection_, nullptr);
    renderPipeline_->OnScreenConnected(rsScreenProperty, renderToComposerConn, composerToRenderConn, nullptr);

    // replay global ipc
    RSIpcReplayer::ReplayIpcData(renderPipelineAgent, replyToRenderInfo->replayData_);

    auto renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*this);
    // called from service
    auto serviceToRenderConnection =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderProcessAgent, renderPipelineAgent);
    auto connectToRenderConnection = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    // child process is initialized
    if (!renderToServiceConnection_->NotifyRenderProcessInitFinished(
        serviceToRenderConnection->AsObject(), connectToRenderConnection->AsObject())) {
        RS_LOGE("%{public}s: NotifyRenderProcessInitFinished is failed", __func__);
        return false;
    }

    RS_LOGI("%{public}s: subprocess init successful", __func__);
    return true;
}

void RSRenderProcess::Run()
{
    if (runner_) {
        RS_LOGI("%{public}s: Subprocess Run", __func__);
        runner_->Run();
    }
}

sptr<RSIRenderService> RSRenderProcess::GetRenderServer()
{
    // get sa proxy
    if (!renderServer_) {
        renderServer_ = ConnectToServer();
    }
    return renderServer_;
}

sptr<ReplyToRenderInfo> RSRenderProcess::ConnectToRenderService(
    const sptr<IRSComposerToRenderConnection>& composerToRenderConnection,
    const sptr<RSRenderPipelineAgent>& renderPipelineAgent, const sptr<VSyncIConnectionToken>& vsyncToken)
{
    auto renderServer = GetRenderServer();
    if (UNLIKELY(!renderServer)) {
        RS_LOGE("%{public}s: renderServer is null", __func__);
        return nullptr;
    }

    // called from others
    auto connectToServiceInfo =
        sptr<ConnectToServiceInfo>::MakeSptr(composerToRenderConnection->AsObject(), vsyncToken->AsObject());
    // make first connection to service
    auto replyToRenderInfo = renderServer->RegisterRenderProcessConnection(connectToServiceInfo);
    if (IsInvalidReplyInfo(replyToRenderInfo)) {
        RS_LOGE("%{public}s: replyToRenderInfo is nullptr", __func__);
        return nullptr;
    }
    return replyToRenderInfo;
}
} // namespace Rosen
} // namespace OHOS
