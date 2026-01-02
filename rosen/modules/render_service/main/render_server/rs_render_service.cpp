/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "rs_render_service.h"

#include <iservice_registry.h>
#include <malloc.h>
#include <memory>
#include <parameters.h>
#include <platform/common/rs_log.h>
#include <string>
#include <system_ability_definition.h>
#include <unistd.h>

#include "dfx/rs_service_dump_manager.h"
#include "feature/param_manager/rs_param_manager.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "hgm_core.h"
#include "parameter.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "rs_profiler.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "transaction/rs_client_to_render_connection.h"

#include "graphic_feature_param_manager.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "rs_render_composer_manager.h"

#include "rs_render_process_manager_agent.h"
#include "transaction/rs_client_to_service_connection.h"
#include "vsync_generator.h"
#include "xcollie/watchdog.h"

#ifdef RS_ENABLE_RDO
#include "feature/rdo/rs_rdo.h"
#endif
#ifdef TP_FEATURE_ENABLE
#include "screen_manager/touch_screen.h"
#endif

#undef LOG_TAG
#define LOG_TAG "RSRenderService"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t UNI_RENDER_VSYNC_OFFSET = 5000000; // ns
constexpr int64_t UNI_RENDER_VSYNC_OFFSET_DELAY_MODE = -3300000; // ns
const std::string BOOTEVENT_RENDER_SERVICE_READY = "bootevent.renderservice.ready";
constexpr uint32_t WATCHDOG_TIMEVAL = 5000;
}

bool RSRenderService::Init()
{
    // Set boot voting to false
    RS_LOGD("dmulti_process %{public}s: renderService init", __func__);
    system::SetParameter(BOOTEVENT_RENDER_SERVICE_READY.c_str(), "false");

    // Initialize runner_, handler_, and watchdog
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    uint32_t timeForWatchDog = WATCHDOG_TIMEVAL;
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread("RenderService", handler_, timeForWatchDog);
    if (ret != 0) {
        RS_LOGW("dmulti_process Add watchdog thread failed");
    }

    // Enable tcache
    if (RSSystemParameters::GetTcacheEnabled()) {
        mallopt(M_OHOS_CONFIG, M_TCACHE_NORMAL_MODE);
        mallopt(M_OHOS_CONFIG, M_ENABLE_OPT_TCACHE);
        mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
        mallopt(M_DELAYED_FREE, M_DELAYED_FREE_ENABLE);
    }

    // CCM config parsing
    InitCCMConfig();

    // Create core components
    CoreComponentsInit();

    // Create multi-process/single-process mode
    RenderProcessManagerInit();

    // Register with system ability manager
    if (!SAMgrRegister()) {
        RS_LOGE("dmulti_process %{public}s: samgr registration failed", __func__);
        return false;
    }

    // Set boot voting to true
    RS_LOGD("dmulti_process %{public}s: Set boot render service started true", __func__);
    system::SetParameter(BOOTEVENT_RENDER_SERVICE_READY.c_str(), "true");

    RS_LOGD("dmulti_process %{public}s: render service init successfully", __func__);
    return true;
}

void RSRenderService::InitCCMConfig()
{
    // feature param parse
    GraphicFeatureParamManager::GetInstance().Init();
}

void RSRenderService::CoreComponentsInit()
{
    RS_LOGD("dmulti_process %{public}s: CoreComponentsInit", __func__);
    // vsyncManager init
    VsyncComponentInit();
    
    // composerManager init
    rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(handler_, rsVsyncManagerAgent_);
    
    // screenManager init
    screenManager_ = sptr<RSScreenManager>::MakeSptr();
    
    // hgm init
    HgmInit();
    
    // reature init
    FeatureComponentInit();
}

void RSRenderService::HgmInit()
{
    HgmCore::Instance().SetScreenManager(screenManager_.GetRefPtr());
    if (auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr()) {
        auto callbackFunc = [this](bool forceUpdate, ScreenId activeScreenId) {
            if (renderProcessManager_) {
                if (auto serviceToRenderConn = renderProcessManager_->GetServiceToRenderConn(activeScreenId)) {
                    serviceToRenderConn ->HgmForceUpdateTask(forceUpdate, "ltpoForceUpdate");
                }
            }
        };
        hgmContext_ = std::make_shared<HgmContext>(handler_, frameRateMgr, callbackFunc,
            appVSyncDistributor_, rsVSyncDistributor_);
        hgmContext_->InitHgmTaskHandleThread(rsVSyncController_, appVSyncController_, vsyncGenerator_);
    }
}

void RSRenderService::FeatureComponentInit()
{
    // vk init
#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::IsUseVulkan()) {
        RsVulkanContext::SetRecyclable(false);
    }
#endif

    // touch screen init
#ifdef TP_FEATURE_ENABLE
    TOUCH_SCREEN->InitTouchScreen();
#endif

    // dump init
    rsDumper_ = std::make_shared<RSServiceDumper>(handler_, screenManager_, rsRenderComposerManager_);
    rsDumper_->RsDumpInit();

    // rdo init
#ifdef RS_ENABLE_RDO
    EnableRSCodeCache();
#endif
}

void RSRenderService::VsyncComponentInit()
{
    vsyncGenerator_ = CreateVSyncGenerator();
    // The offset needs to be set
    int64_t offset = 0;
    if (!HgmCore::Instance().GetLtpoEnabled()) {
        if (RSUniRenderJudgement::GetUniRenderEnabledType() == UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            offset = HgmCore::Instance().IsDelayMode() ? UNI_RENDER_VSYNC_OFFSET_DELAY_MODE : UNI_RENDER_VSYNC_OFFSET;
        }
        rsVSyncController_ = new VSyncController(vsyncGenerator_, offset);
        appVSyncController_ = new VSyncController(vsyncGenerator_, offset);
    } else {
        rsVSyncController_ = new VSyncController(vsyncGenerator_, 0);
        appVSyncController_ = new VSyncController(vsyncGenerator_, 0);
        vsyncGenerator_->SetVSyncMode(VSYNC_MODE_LTPO);
    }
    vsyncSampler_ = CreateVSyncSampler();
    vsyncSampler_->RegUpdateVsyncEnabledScreenIdCallback([this](uint64_t vsyncEnabledScreenId) -> bool {
       return this->screenManager_->UpdateVsyncEnabledScreenId(vsyncEnabledScreenId);
    });

    vsyncSampler_->RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback([this](
        uint64_t screenId, ScreenPowerStatus status, uint64_t enabledScreenId) -> uint64_t {
        return this->screenManager_->JudgeVSyncEnabledScreenWhilePowerStatusChanged(screenId, status, enabledScreenId);
    });

    vsyncSampler_->RegUpdateFoldScreenConnectStatusLockedCallback([this](uint64_t screenId, bool connected) {
        return this->screenManager_->UpdateFoldScreenConnectStatusLocked(screenId, connected);
    });

    vsyncSampler_->RegSetScreenVsyncEnableByIdCallback([this](
        uint64_t vsyncEnabledScreenId, uint64_t screenId, bool enabled) {
        return this->screenManager_->SetScreenVsyncEnableById(vsyncEnabledScreenId, screenId, enabled);
    });

    vsyncSampler_->RegGetScreenVsyncEnableByIdCallback([this](uint64_t vsyncEnabledScreenId) {
        return this->screenManager_->GetScreenVsyncEnableById(vsyncEnabledScreenId);
    });
    
    auto dvsyncParam = InitDVSyncParams();
    rsVSyncDistributor_ = new VSyncDistributor(rsVSyncController_, "rs", dvsyncParam);
    appVSyncDistributor_ = new VSyncDistributor(appVSyncController_, "app", dvsyncParam);
    vsyncGenerator_->SetRSDistributor(rsVSyncDistributor_);
    vsyncGenerator_->SetAppDistributor(appVSyncDistributor_);
    rsVsyncManagerAgent_ = new RSVsyncManagerAgent(vsyncGenerator_, rsVSyncDistributor_);
}

void RSRenderService::RenderProcessManagerInit()
{
    RS_LOGD("dmulti_process %{public}s: renderProcessManager_ init", __func__);
    // TODO: mainThread_ need to be removed in render_service asap
    mainThread_ = RSMainThread::Instance();
    renderProcessManager_ = RSRenderProcessManager::Create(*this);
    auto screenManagerListener = sptr<ScreenManagerListener>::MakeSptr(*this);
    screenManager_->RegisterCoreListener(screenManagerListener);
    if (screenManager_->Init(handler_)) {
        RS_LOGE("ScreenManager initV2 Success");
    }
}

bool RSRenderService::SAMgrRegister()
{
    RS_LOGD("dmulti_process samgr registration start");
    // Wait samgr ready for up to 5 seconds to ensure adding service to samgr
    int status = WaitParameter("bootevent.samgr.ready", "true", 5);
    if (status != 0) {
        RS_LOGE("dmulti_process %{public}s: wait SAMGR error, return value [%{public}d]", __func__, status);
        return false;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        RS_LOGE("dmulti_process %{public}s: GetSystemAbilityManager fail", __func__);
        return false;
    }
    
    samgr->AddSystemAbility(RENDER_SERVICE, this);
    return true;
}

DVSyncFeatureParam RSRenderService::InitDVSyncParams()
{
    std::vector<bool> switchParams = {
        DVSyncParam::IsDVSyncEnable(),
        DVSyncParam::IsUiDVSyncEnable(),
        DVSyncParam::IsNativeDVSyncEnable(),
        DVSyncParam::IsAdaptiveDVSyncEnable(),
    };
    std::vector<uint32_t> bufferCountParams = {
        DVSyncParam::GetUiBufferCount(),
        DVSyncParam::GetRsBufferCount(),
        DVSyncParam::GetNativeBufferCount(),
        DVSyncParam::GetWebBufferCount(),
    };
    auto adaptiveConfigs = DVSyncParam::GetAdaptiveConfig();
    DVSyncFeatureParam dvsyncParam = {
        std::move(switchParams),
        std::move(bufferCountParams),
        std::move(adaptiveConfigs)
    };
    return dvsyncParam;
}

void RSRenderService::Run()
{
    if (runner_) {
        RS_LOGD("dmulti_process MainProcess Run");
        runner_->Run();
    }
}

std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> RSRenderService::GetConnection(
    const sptr<RSIConnectionToken>& token)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto tokenObj = token->AsObject();
    auto iter = connections_.find(tokenObj);
    if (iter == connections_.end()) {
        RS_LOGE("GetConnection: connections_ cannot find token");
        return {nullptr, nullptr};
    }
    return iter->second;
}

std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> RSRenderService::CreateConnection(const sptr<RSIConnectionToken>& token)
{
    if (!token) {
        RS_LOGE("CreateConnection failed, mainThread or token is nullptr");
        return std::make_pair(nullptr, nullptr);
    }
    pid_t remotePid = GetCallingPid();
    RS_PROFILER_ON_CREATE_CONNECTION(remotePid);

    auto tokenObj = token->AsObject();
    sptr<RSScreenManagerAgent> screenManagerAgent = new RSScreenManagerAgent(screenManager_);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*this);
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent =
        sptr<RSRenderProcessManagerAgent>::MakeSptr(renderProcessManager_);
    sptr<RSIClientToServiceConnection> newConn(new RSClientToServiceConnection(remotePid, renderServiceAgent,
        renderProcessManagerAgent, screenManagerAgent, tokenObj, appVSyncDistributor_));
    sptr<RSRenderPipelineAgent> renderPipelineAgent = new RSRenderPipelineAgent(renderPipeline_);
    sptr<RSIClientToRenderConnection> newRenderConn(
        new RSClientToRenderConnection(remotePid, renderPipelineAgent, tokenObj));
    std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> tmp;
    std::unique_lock<std::mutex> lock(mutex_);
    // if connections_ has the same token one, replace it.
    auto it = connections_.find(tokenObj);
    if (it != connections_.end()) {
        tmp = it->second;
    }
    connections_[tokenObj] = { newConn, newRenderConn };
    lock.unlock();
    // TODO: 这个AddTransactionDataPidInfo需要移到renderPipeline里面去 transaction
    mainThread_->AddTransactionDataPidInfo(remotePid);
    return std::make_pair(newConn, newRenderConn);
}

bool RSRenderService::RemoveConnection(const sptr<RSIConnectionToken>& token)
{
    if (token == nullptr) {
        RS_LOGE("RemoveConnection: token is nullptr");
        return false;
    }
    // temporarily extending the life cycle
    auto tokenObj = token->AsObject();
    std::unique_lock<std::mutex> lock(mutex_);
    auto iter = connections_.find(tokenObj);
    if (iter == connections_.end()) {
        RS_LOGE("RemoveConnection: connections_ cannot find token");
        return false;
    }
    connections_.erase(iter);
    lock.unlock();
    return true;
}

int RSRenderService::Dump(int fd, const std::vector<std::u16string>& args)
{
    std::string dumpString;
    RSServiceDumpManager::GetInstance().DoDump(args, dumpString, renderProcessManager_->GetServiceToRenderConns());

    if (dumpString.size() == 0) {
        return OHOS::INVALID_OPERATION;
    }
    if (write(fd, dumpString.c_str(), dumpString.size()) < 0) {
        RS_LOGE("DumpNodesNotOnTheTree write failed, string size: %{public}zu", dumpString.size());
        return UNKNOWN_ERROR;
    }
    return OHOS::NO_ERROR;
}

void RSRenderService::HandleTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
    rsVSyncDistributor_->HandleTouchEvent(touchStatus, touchCnt);
}

void RSRenderService::GetRefreshInfoToSP(std::string& dumpString, NodeId nodeId)
{
    rsRenderComposerManager_->GetRefreshInfoToSP(dumpString, nodeId);
}

void RSRenderService::FpsDump(std::string& dumpString, const std::string& arg)
{
    rsRenderComposerManager_->FpsDump(dumpString, arg);
}

sptr<IRemoteObject> RSRenderService::ScreenManagerListener::OnScreenConnected(ScreenId screenId,
    const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property)
{
    RS_LOGD("%{public}s: rsScreenProperty.id[%{public}" PRIu64 "] .width[%{public}d] .height[%{public}d]",
        __func__, property->GetScreenId(), property->GetWidth(), property->GetHeight());
    renderService_.rsRenderComposerManager_->OnScreenConnected(output, property);
    if (const auto& hgmContext = renderService_.GetHgmContext()) {
        hgmContext->AddScreenToHgm(property);
    }
    uint64_t vsyncEnabledScreenId = renderService_.vsyncSampler_->JudgeVSyncEnabledScreenWhileHotPlug(screenId, true);
    renderService_.vsyncSampler_->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId,
        renderService_.handler_);
    renderService_.screenManager_->SetScreenVsyncEnableById(vsyncEnabledScreenId, screenId, false);
    return renderService_.renderProcessManager_->OnScreenConnected(screenId, output, property);
}

void RSRenderService::ScreenManagerListener::OnScreenDisconnected(ScreenId id)
{
    RS_LOGD("%{public}s: ScreenId[%{public}" PRIu64 "]", __func__, id);
    renderService_.rsRenderComposerManager_->OnScreenDisconnected(id);
    if (const auto& hgmContext = renderService_.GetHgmContext()) {
        hgmContext->RemoveScreenFromHgm(id);
    }
    uint64_t vsyncEnabledScreenId = renderService_.vsyncSampler_->JudgeVSyncEnabledScreenWhileHotPlug(id, false);
    renderService_.vsyncSampler_->RegSetScreenVsyncEnabledCallbackForRenderService(vsyncEnabledScreenId,
        renderService_.handler_);
    renderService_.renderProcessManager_->OnScreenDisconnected(id);
}

void RSRenderService::ScreenManagerListener::OnScreenPropertyChanged(ScreenId id,
    const sptr<RSScreenProperty>& property)
{
    RS_LOGD("%{public}s: ScreenId[%{public}" PRIu64 "]", __func__, id);
    if (!property->IsVirtual()) {
        auto status = property->GetScreenPowerStatus();
        renderService_.vsyncSampler_->ProcessVSyncScreenIdWhilePowerStatusChanged(id, status,
            renderService_.handler_, renderService_.screenManager_->GetIsFoldScreenFlag());
    }
    renderService_.renderProcessManager_->OnScreenPropertyChanged(id, property);
}

void RSRenderService::ScreenManagerListener::OnScreenRefresh(ScreenId id)
{
    renderService_.renderProcessManager_->OnScreenRefresh(id);
}

void RSRenderService::ScreenManagerListener::OnVBlankIdle(ScreenId id, uint64_t ns)
{
    if (auto composerConn = renderService_.rsRenderComposerManager_->GetRSComposerConnection(id)) {
        composerConn->OnScreenVBlankIdleCallback(id, ns);
    }
}

void RSRenderService::ScreenManagerListener::OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
    const sptr<RSScreenProperty>& property)
{
    renderService_.renderProcessManager_->OnVirtualScreenConnected(id, associatedScreenId, property);
}

void RSRenderService::ScreenManagerListener::OnVirtualScreenDisconnected(ScreenId id)
{
    renderService_.renderProcessManager_->OnVirtualScreenDisconnected(id);
}

void RSRenderService::ScreenManagerListener::OnHwcEvent(uint32_t deviceId, uint32_t eventId,
    const std::vector<int32_t>& eventData)
{
    renderService_.renderProcessManager_->OnHwcEvent(deviceId, eventId, eventData);
}

void RSRenderService::ScreenManagerListener::OnHwcRestored(ScreenId id, const std::shared_ptr<HdiOutput>& output,
    const sptr<RSScreenProperty>& property)
{
    RS_LOGI("%{public}s: ScreenId[%{public}" PRIu64 "]", __func__, id);
    renderService_.rsRenderComposerManager_->OnHwcRestored(output, property);
}

void RSRenderService::ScreenManagerListener::OnHwcDead(ScreenId id)
{
    RS_LOGI("%{public}s: ScreenId[%{public}" PRIu64 "]", __func__, id);
    renderService_.rsRenderComposerManager_->OnHwcDead(id);
}

void RSRenderService::ScreenManagerListener::OnActiveScreenIdChanged(ScreenId activeScreenId)
{
    HgmCore::Instance().SetActiveScreenId(activeScreenId);
}

void RSRenderService::ScreenManagerListener::OnScreenBacklightChanged(ScreenId id, uint32_t level)
{
    renderService_.renderProcessManager_->OnScreenBacklightChanged(id, level);
}
} // namespace Rosen
} // namespace OHOS
