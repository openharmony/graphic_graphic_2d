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

// #include "dfx/rs_process_dump_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/transaction/rs_render_connect_parcel_info.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/rs_connect_to_render_process.h"
#include "transaction/rs_service_to_render_connection.h"
#include "xcollie/watchdog.h"
#include "transaction/rs_connect_to_render_process.h"
#include "rs_composer_to_render_connection.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderProcess"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t WATCHDOG_TIMEVAL = 5000;

sptr<RSIRenderService> ConnectToServer()
{
    int tryCnt = 0;
    sptr<RSIRenderService> renderService = nullptr;
    do {
        // sleep move time (1000us * tryCnt) when tryCnt++
        usleep(1000 * tryCnt);
        ++tryCnt;
        // try most 5 times to get render service.
        if (tryCnt == 5) {
            RS_LOGE("%{public}s: tried 5 times.", __func__);
            break;
        }

        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            RS_LOGE("%{public}s: samgr is null", __func__);
            continue;
        }
        auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE); // chenke fix
        if (remoteObject == nullptr) {
            RS_LOGE("%{public}s: remoteObject is null", __func__);
            continue;
        }
        renderService = iface_cast<RSIRenderService>(remoteObject);
        if (renderService != nullptr) {
            RS_LOGE("%{public}s: renderService is null", __func__);
            break;
        }
    } while (true);

    if (renderService == nullptr) {
        RS_LOGE("%{public}s: failed to get render service proxy", __func__);
        return nullptr;
    }
    return renderService;
}
}

bool RSRenderProcess::Init()
{
    RS_LOGI("dmulti_process %{public}s: start Init", __func__);
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    uint32_t timeForWatchDog = WATCHDOG_TIMEVAL;
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread("RenderProcess", handler_, timeForWatchDog);
    if (ret != 0) {
        RS_LOGW("dmulti_process Add watchdog thread failed");
    }

    // enable tcache
    if (RSSystemParameters::GetTcacheEnabled()) {
        mallopt(M_OHOS_CONFIG, M_TCACHE_NORMAL_MODE);
        mallopt(M_OHOS_CONFIG, M_ENABLE_OPT_TCACHE);
        mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
        mallopt(M_DELAYED_FREE, M_DELAYED_FREE_ENABLE);
    }

    // 核心组件初始化
    mainThread_ = RSMainThread::Instance();

    // 注册子进程到主进程
    RS_LOGD("dmulti_process %{public}s: Subprocess Registration", __func__);
    auto [rsScreenProperty, composeConn, receiver] = ConnectToRenderService();
    if (!renderToServiceConnection_) {
        RS_LOGI("dmulti_process %{public}s: renderToServiceConnection_ not exist", __func__);
        return false;
    }
    mainThread_->RegisterScreenSwitchFinishCallback(renderToServiceConnection_);

    // 渲染管线拉起
    RS_LOGE("dmulti_process liweiiii RSRenderProcess::init %{public}p", mainThread_);
    // TODO: 需要适配
    // renderPipeline_ = RSRenderPipeline::Create(handler_, receiver);
    // renderPipeline_->OnScreenConnected(rsScreenProperty, composerClient, hgmClient);
    renderPipeline_ = RSRenderPipeline::Create(handler_, receiver, renderToServiceConnection_, nullptr);
    renderPipeline_->OnScreenConnected(rsScreenProperty, nullptr, nullptr, nullptr);

    // 子进程初始化完毕
    RS_LOGI("dmulti_process %{public}s: notify render process init successful", __func__);
    if (!renderToServiceConnection_->NotifyRenderProcessInitFinished()) {
        RS_LOGE("%{public}s: NotifyRenderProcessInitFinished is failed", __func__);
        return false;
    }

    // TODO: dfx适配
    // RSProcessDumpManager::GetInstance().SetRenderToServiceConnection(renderToServiceConnection_);
    // RSProcessDumpManager::GetInstance().SetPid(getpid());

    RS_LOGI("dmulti_process %{public}s: subprocess init successful", __func__);
    return true;
}

void RSRenderProcess::Run()
{
    if (runner_) {
        RS_LOGI("dmulti_process %{public}s: Subprocess Run", __func__);
        runner_->Run();
    }
}

sptr<RSIRenderService> RSRenderProcess::GetRenderServer()
{
    // get sa proxy
    if (renderServer_ == nullptr) {
        renderServer_ = ConnectToServer();
    }
    return renderServer_;
}

static bool IsInvalidReplyInfo(const sptr<ReplyToRenderInfo>& result)
{
    return !result ||
           !result->serviceConnection_ ||
           !result->composeConnection_ ||
           !result->rsScreenProperty_ ||
           !result->vsyncConn_;
}

std::tuple<sptr<RSScreenProperty>, sptr<IRSRenderToComposerConnection>, std::shared_ptr<VSyncReceiver>> RSRenderProcess::ConnectToRenderService()
{
    auto renderServer = GetRenderServer();
    if (UNLIKELY(renderServer == nullptr)) {
        RS_LOGE("dmulti_process %{public}s: renderServer is null", __func__);
        return {nullptr, nullptr, nullptr};
    }

    sptr<RSRenderProcessAgent> renderProcessAgent = sptr<RSRenderProcessAgent>::MakeSptr(*this);
    sptr<RSRenderPipelineAgent> renderPipelineAgent = new RSRenderPipelineAgent(renderPipeline_);
    // called from service
    auto serviceToRenderConnection = sptr<RSServiceToRenderConnection>::MakeSptr(renderProcessAgent, renderPipelineAgent);
    auto composerToRenderConnection = sptr<RSComposerToRenderConnection>::MakeSptr();
    // called from others
    renderPipelineAgent = new RSRenderPipelineAgent(renderPipeline_);
    auto connectToRender = sptr<RSConnectToRenderProcess>::MakeSptr(renderPipelineAgent);
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    auto connectToServiceInfo = sptr<ConnectToServiceInfo>::MakeSptr(serviceToRenderConnection->AsObject(),
        composerToRenderConnection->AsObject(), connectToRender->AsObject(), token->AsObject());
    // make first connection to service
    auto replyToRenderInfo = renderServer->RegisterRenderProcessConnection(connectToServiceInfo);
    if (IsInvalidReplyInfo(replyToRenderInfo)) {
        RS_LOGE("dmulti_process replyToRenderInfo is nullptr");
        return { nullptr, nullptr, nullptr };
    }

    renderToServiceConnection_ = iface_cast<RSIRenderToServiceConnection>(replyToRenderInfo->serviceConnection_);
    auto composerConn = iface_cast<IRSRenderToComposerConnection>(replyToRenderInfo->composeConnection_);
    auto& rsScreenProperty = replyToRenderInfo->rsScreenProperty_;
    // TODO: dfx适配
    // RSProcessDumpManager::GetInstance().SetScreenId(rsScreenProperty->GetScreenId());
    auto vsyncConn = iface_cast<IVSyncConnection>(replyToRenderInfo->vsyncConn_);
    RS_LOGI("dmulti_process %{public}s: vsync conn create successfully", __func__);
    auto receiver = std::make_shared<VSyncReceiver>(vsyncConn, token->AsObject(), handler_, "render_process");
    receiver->Init();

    return {rsScreenProperty, composerConn, receiver};
}

} // namespace Rosen
} // namespace OHOS
