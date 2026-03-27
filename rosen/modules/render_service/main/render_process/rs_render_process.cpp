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
           !result->serviceConnection_ ||
           !result->composeConnection_ ||
           !result->rsScreenProperty_ ||
           !result->vsyncConn_;
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

    // TODO: 需要删除
    mainThread_ = RSMainThread::Instance();

    // register the child process to the parent process
    RS_LOGI("%{public}s: Subprocess Registration", __func__);
    auto composerToRenderConn = sptr<RSComposerToRenderConnection>::MakeSptr();
    auto [rsScreenProperty, renderToComposerConn, receiver] = ConnectToRenderService(composerToRenderConn);
    if (!renderToServiceConnection_ || !renderToComposerConn) {
        RS_LOGE("%{public}s: renderToServiceConnection_ not exist", __func__);
        return false;
    }

    // create render pipeline
    RS_LOGI("%{public}s: %{public}p", __func__, mainThread_);
    renderPipeline_ = RSRenderPipeline::Create(handler_, receiver, renderToServiceConnection_, nullptr);
    renderPipeline_->OnScreenConnected(rsScreenProperty, renderToComposerConn, composerToRenderConn, nullptr);

    // child process is initialized
    if (!renderToServiceConnection_->NotifyRenderProcessInitFinished()) {
        RS_LOGE("%{public}s: NotifyRenderProcessInitFinished is failed", __func__);
        return false;
    }

    // TODO: dfx适配
    // RSProcessDumpManager::GetInstance().SetRenderToServiceConnection(renderToServiceConnection_);
    // RSProcessDumpManager::GetInstance().SetPid(getpid());

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

std::tuple<sptr<RSScreenProperty>, sptr<IRSRenderToComposerConnection>, std::shared_ptr<VSyncReceiver>>
RSRenderProcess::ConnectToRenderService(const sptr<IRSComposerToRenderConnection>& composerToRenderConnection)
{
    auto renderServer = GetRenderServer();
    if (UNLIKELY(!renderServer)) {
        RS_LOGE("%{public}s: renderServer is null", __func__);
        return { nullptr, nullptr, nullptr };
    }

    auto renderProcessAgent { sptr<RSRenderProcessAgent>::MakeSptr(*this) };
    auto renderPipelineAgent { sptr<RSRenderPipelineAgent>::MakeSptr(renderPipeline_) };
    // called from service
    auto serviceToRenderConnection =
        sptr<RSServiceToRenderConnection>::MakeSptr(renderProcessAgent, renderPipelineAgent);
    // called from others
    auto connectToRender = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    auto connectToServiceInfo = sptr<ConnectToServiceInfo>::MakeSptr(serviceToRenderConnection->AsObject(),
        composerToRenderConnection->AsObject(), connectToRender->AsObject(), token->AsObject());
    // make first connection to service
    auto replyToRenderInfo = renderServer->RegisterRenderProcessConnection(connectToServiceInfo);
    if (IsInvalidReplyInfo(replyToRenderInfo)) {
        RS_LOGE("%{public}s: replyToRenderInfo is nullptr", __func__);
        return { nullptr, nullptr, nullptr };
    }

    renderToServiceConnection_ = iface_cast<RSIRenderToServiceConnection>(replyToRenderInfo->serviceConnection_);
    auto composerConn = iface_cast<IRSRenderToComposerConnection>(replyToRenderInfo->composeConnection_);
    auto& rsScreenProperty = replyToRenderInfo->rsScreenProperty_;
    // RSProcessDumpManager::GetInstance().SetScreenId(rsScreenProperty->GetScreenId());
    auto vsyncConn = iface_cast<IVSyncConnection>(replyToRenderInfo->vsyncConn_);
    auto receiver = std::make_shared<VSyncReceiver>(vsyncConn, token->AsObject(), handler_, "render_process");
    receiver->Init();

    return { rsScreenProperty, composerConn, receiver };
}
} // namespace Rosen
} // namespace OHOS
