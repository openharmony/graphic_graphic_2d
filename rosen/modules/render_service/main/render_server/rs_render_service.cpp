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

#include "feature/param_manager/rs_param_manager.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "ge_mesa_blur_shader_filter.h"
#include "hgm_core.h"
#include "parameter.h"
#include "render_process/transaction/rs_service_to_render_connection.h"
#include "rs_profiler.h"
#include "rs_trace.h"
#include "vsync_generator.h"

#include "common/rs_singleton.h"
#include "memory/rs_memory_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/rs_render_to_service_connection.h"

#include "rs_render_to_composer_connection.h"
#include "rs_render_composer_manager.h"
#include "dfx/rs_service_dump_manager.h" // todo dfx or dfx ?
#include "gfx/fps_info/rs_surface_fps_manager.h"
#include "graphic_feature_param_manager.h"
#include "system/rs_system_parameters.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "render/rs_render_kawase_blur_filter.h"

#include "text/font_mgr.h"
#include "transaction/rs_client_to_service_connection.h"
#include "xcollie/watchdog.h"
#include "rs_render_process_manager_agent.h"
#include "feature/hyper_graphic_manager/hgm_context.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
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

    // need called after GraphicFeatureParamManager::GetInstance().Init();
    FilterCCMInit();
}

void RSRenderService::FilterCCMInit()
{
    RSFilterCacheManager::isCCMFilterCacheEnable_ = FilterParam::IsFilterCacheEnable();
    RSFilterCacheManager::isCCMEffectMergeEnable_ = FilterParam::IsEffectMergeEnable();
    RSProperties::SetFilterCacheEnabledByCCM(RSFilterCacheManager::isCCMFilterCacheEnable_);
    RSProperties::SetBlurAdaptiveAdjustEnabledByCCM(FilterParam::IsBlurAdaptiveAdjust());
    RSKawaseBlurShaderFilter::SetMesablurAllEnabledByCCM(FilterParam::IsMesablurAllEnable());
    GEMESABlurShaderFilter::SetMesaModeByCCM(FilterParam::GetSimplifiedMesaMode());
}

void RSRenderService::CoreComponentsInit()
{
#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::IsUseVulkan()) {
        RsVulkanContext::SetRecyclable(false);
    }
#endif

    RS_LOGD("dmulti_process %{public}s: rsRenderComposerManager_ init", __func__);
    rsRenderComposerManager_ = std::make_shared<RSRenderComposerManager>(handler_);

    RS_LOGD("dmulti_process %{public}s: screenManager_ init", __func__);
    screenManager_ = CreateOrGetScreenManager();
    HgmCore::Instance().SetScreenManager(screenManager_.GetRefPtr());

#ifdef TP_FEATURE_ENABLE
    RS_LOGD("dmulti_process %{public}s: touchScreen init", __func__);
    TOUCH_SCREEN->InitTouchScreen();
#endif

    RS_LOGD("dmulti_process %{public}s: vsync generate init", __func__);
    VsyncComponentInit();
    rsRenderComposerManager_->InitRsVsyncManagerAgent(rsVsyncManagerAgent_);

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

    rsDumper_ = std::make_shared<RSServiceDumper>(handler_, screenManager_, rsRenderComposerManager_);
    rsDumper_->RsDumpInit();

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
    vsyncSampler_->RegUpdateVsyncEnabledScreenIdCallback([this](uint64_t vsyncEnabledScreenId)->bool{
       return this->screenManager_->UpdateVsyncEnabledScreenId(vsyncEnabledScreenId);
    });

    vsyncSampler_->RegJudgeVSyncEnabledScreenWhilePowerStatusChangedCallback([this](uint64_t screenId, ScreenPowerStatus status, uint64_t enabledScreenId)->uint64_t{
        return this->screenManager_->JudgeVSyncEnabledScreenWhilePowerStatusChanged(screenId, status, enabledScreenId);
    });

    vsyncSampler_->RegUpdateFoldScreenConnectStatusLockedCallback([this](uint64_t screenId, bool connected){
        return this->screenManager_->UpdateFoldScreenConnectStatusLocked(screenId, connected);
    });

    vsyncSampler_->RegSetScreenVsyncEnableByIdCallback([this](uint64_t vsyncEnabledScreenId, uint64_t screenId, bool enabled){
        return this->screenManager_->SetScreenVsyncEnableById(vsyncEnabledScreenId, screenId, enabled);
    });

    vsyncSampler_->RegGetScreenVsyncEnableByIdCallback([this](uint64_t vsyncEnabledScreenId){
        return this->screenManager_->GetScreenVsyncEnableById(vsyncEnabledScreenId);
    });
    
    DVSyncFeatureParam dvsyncParam;
    InitDVSyncParams(dvsyncParam);
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
    if (screenManager_->Init_V2(handler_)) {
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

void RSRenderService::InitDVSyncParams(DVSyncFeatureParam &dvsyncParam)
{
    std::vector<bool> switchParams = {};
    std::vector<uint32_t> bufferCountParams = {};
    std::unordered_map<std::string, std::string> adaptiveConfigs;
    switchParams = {
        DVSyncParam::IsDVSyncEnable(),
        DVSyncParam::IsUiDVSyncEnable(),
        DVSyncParam::IsNativeDVSyncEnable(),
        DVSyncParam::IsAdaptiveDVSyncEnable(),
    };
    bufferCountParams = {
        DVSyncParam::GetUiBufferCount(),
        DVSyncParam::GetRsBufferCount(),
        DVSyncParam::GetNativeBufferCount(),
        DVSyncParam::GetWebBufferCount(),
    };
    adaptiveConfigs = DVSyncParam::GetAdaptiveConfig();
    dvsyncParam = { switchParams, bufferCountParams, adaptiveConfigs };
}

void RSRenderService::Run()
{
    if (runner_) {
        RS_LOGD("dmulti_process MainProcess Run");
        runner_->Run();
    }
}

std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> RSRenderService::CreateConnection(const sptr<RSIConnectionToken>& token)
{
    if (!mainThread_ || !token) {
        RS_LOGE("CreateConnection failed, mainThread or token is nullptr");
        return std::make_pair(nullptr, nullptr);
    }
    pid_t remotePid = GetCallingPid();
    RS_PROFILER_ON_CREATE_CONNECTION(remotePid);

    auto tokenObj = token->AsObject();
    sptr<RSScreenManagerAgent> screenManagerAgent = new RSScreenManagerAgent(screenManager_);
    sptr<RSRenderServiceAgent> renderServiceAgent = sptr<RSRenderServiceAgent>::MakeSptr(*this);
    sptr<RSRenderProcessManagerAgent> renderProcessManagerAgent = sptr<RSRenderProcessManagerAgent>::MakeSptr(renderProcessManager_);
    sptr<RSIClientToServiceConnection> newConn(
        new RSClientToServiceConnection(remotePid, this, renderServiceAgent, renderProcessManagerAgent, mainThread_, screenManagerAgent, tokenObj, appVSyncDistributor_));
    sptr<RSRenderPipelineAgent> renderPipelineAgent = new RSRenderPipelineAgent(renderPipeline_);
    sptr<RSIClientToRenderConnection> newRenderConn(
        new RSClientToRenderConnection(remotePid, mainThread_, renderPipelineAgent, tokenObj));
    std::pair<sptr<RSIClientToServiceConnection>, sptr<RSIClientToRenderConnection>> tmp;
    std::unique_lock<std::mutex> lock(mutex_);
    // if connections_ has the same token one, replace it.
    auto it = connections_.find(tokenObj);
    if (it != connections_.end()) {
        tmp = it->second;
    }
    connections_[tokenObj] = { newConn, newRenderConn };
    lock.unlock();
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
    if (screenManager_ == nullptr) {
        RS_LOGE("DoDump failed, mainThread, screenManager is nullptr");
        return OHOS::INVALID_OPERATION;
    }
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

void RSRenderService::GetRefreshInfoToSP(std::string& dumpString, NodeId& nodeId)
{
    if (rsRenderComposerManager_) {
        rsRenderComposerManager_->GetRefreshInfoToSP(dumpString, nodeId);
    }
}

void RSRenderService::FpsDump(std::string& dumpString, std::string& arg)
{
    if (rsRenderComposerManager_) {
        rsRenderComposerManager_->FpsDump(dumpString, arg);
    }
}

sptr<IRemoteObject> RSRenderService::ScreenManagerListener::OnScreenConnected(ScreenId screenId,
    const ScreenEventData& data, const sptr<RSScreenProperty>& property)
{
    return renderService_.renderProcessManager_->OnScreenConnected(screenId, data, property);
}

void RSRenderService::ScreenManagerListener::OnScreenDisconnected(ScreenId id)
{
    renderService_.renderProcessManager_->OnScreenDisconnected(id);
}

void RSRenderService::ScreenManagerListener::OnScreenPropertyChanged(ScreenId id,
    const sptr<RSScreenProperty>& property)
{
    renderService_.renderProcessManager_->OnScreenPropertyChanged(id, property);
}

void RSRenderService::ScreenManagerListener::OnScreenRefresh(ScreenId id)
{
    renderService_.renderProcessManager_->OnScreenRefresh(id);
}

void RSRenderService::ScreenManagerListener::OnVBlankIdle(ScreenId id, uint64_t ns)
{
    renderService_.renderProcessManager_->OnVBlankIdle(id, ns);
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

void RSRenderService::ScreenManagerListener::OnActiveScreenIdChanged(ScreenId activeScreenId)
{
    renderService_.renderProcessManager_->OnActiveScreenIdChanged(activeScreenId);
}

void RSRenderService::ScreenManagerListener::OnScreenBacklightChanged(ScreenId id, uint32_t level)
{
    renderService_.renderProcessManager_->OnScreenBacklightChanged(id, level);
}
} // namespace Rosen
} // namespace OHOS
