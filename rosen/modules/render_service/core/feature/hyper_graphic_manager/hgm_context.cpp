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

#include "feature/hyper_graphic_manager/hgm_context.h"

#include "common/rs_optional_trace.h"
#include "hfbc_param.h"
#include "hgm_config_callback_manager.h"
#include "hgm_core.h"
#include "parameters.h"
#include "rs_frame_report.h"
#include "rs_frame_blur_predict.h"
#include "screen_manager/rs_screen_manager.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string VOTER_SCENE_BLUR = "VOTER_SCENE_BLUR";
const std::string VOTER_SCENE_GPU = "VOTER_SCENE_GPU";
}

HgmContext::HgmContext(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<HgmFrameRateManager>& frameRateMgr,
    std::function<void(bool, ScreenId)> requestRSNextVsyncFunc,
    const sptr<VSyncDistributor>& appVSyncDistributor,
    const sptr<VSyncDistributor>& rsVSyncDistributor)
    : renderServiceHandler_(handler),
      frameRateManager_(frameRateMgr),
      hgmCore_(HgmCore::Instance()),
      requestRSNextVsyncFunc_(std::move(requestRSNextVsyncFunc)),
      appVSyncDistributor_(appVSyncDistributor),
      rsVSyncDistributor_(rsVSyncDistributor)
{
    rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>([this] { hgmCore_.SetHgmTaskFlag(true); });
}

void HgmContext::InitHgmTaskHandleThread(
    const sptr<VSyncController>& rsVSyncController, const sptr<VSyncController>& appVSyncController,
    const sptr<VSyncGenerator>& vsyncGenerator)
{
    auto forceUpdateTask = [this](bool idleTimerExpired, bool forceUpdate) {
        renderServiceHandler_->PostTask([this, idleTimerExpired, forceUpdate] {
            RS_TRACE_NAME_FMT("HgmContext::TimerExpiredCallback Run idleTimerExpiredFlag: %s forceUpdateFlag: %s",
                idleTimerExpired ? "True" : "False", forceUpdate ? "True" : "False");
            if (GetLastForceUpdateVsyncId() != GetCurrVsyncId()) {
                SetLastForceUpdateVsyncId(GetCurrVsyncId());
                requestRSNextVsyncFunc_(forceUpdate, hgmCore_.GetActiveScreenId());
            }
        });
    };
    HgmTaskHandleThread::Instance().PostSyncTask([frameRateManager = frameRateManager_,
        task = std::move(forceUpdateTask), rsVSyncController, appVSyncController, vsyncGenerator,
        appVSyncDistributor = appVSyncDistributor_] {
        frameRateManager->SetForceUpdateCallback(task);
        frameRateManager->Init(rsVSyncController, appVSyncController, vsyncGenerator, appVSyncDistributor);
    });

    InitHgmUpdateCallback();
}

void HgmContext::InitHgmUpdateCallback()
{
    auto hgmConfigUpdateCallbackTask = [this](std::shared_ptr<RPHgmConfigData> configData,
        bool ltpoEnabled, bool isDelayMode, int32_t pipelineOffsetPulseNum) {
        renderServiceHandler_->PostTask([this, configData, ltpoEnabled, isDelayMode, pipelineOffsetPulseNum] {
            hgmDataChangeTypes_.set(HgmDataChangeType::HGM_CONFIG_DATA);
            rpHgmConfigData_ = std::move(configData);
            ltpoEnabled_ = ltpoEnabled;
            isDelayMode_ = isDelayMode;
            pipelineOffsetPulseNum_ = pipelineOffsetPulseNum;
        });
    };

    auto adaptiveVsyncUpdateCallbackTask = [this](bool isAdaptive, const std::string& gameNodeName) {
        renderServiceHandler_->PostTask([this, isAdaptive, gameNodeName] {
            hgmDataChangeTypes_.set(HgmDataChangeType::HGM_CONFIG_DATA);
            isAdaptive_ = isAdaptive;
            gameNodeName_ = gameNodeName;
        });
    };

    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_,
        hgmConfigUpdateCallbackTask, adaptiveVsyncUpdateCallbackTask] {
        frameRateManager->SetHgmConfigUpdateCallback(hgmConfigUpdateCallbackTask);
        frameRateManager->SetAdaptiveVsyncUpdateCallback(adaptiveVsyncUpdateCallbackTask);
    });
}

void HgmContext::HandleHgmProcessInfo(const sptr<HgmProcessToServiceInfo>& info)
{
    if (info == nullptr) {
        return;
    }

    HgmEnergyConsumptionPolicy::Instance().HandleEnergyCommonData(info->energyCommonData);
    frameRateLinkerMap_.UnregisterFrameRateLinker(info->frameRateLinkerDestroyIds);
    frameRateLinkerMap_.UpdateFrameRateLinker(info->frameRateLinkerUpdateInfoMap);
    TransformNodeToLinkersRateMap(info);

    rsCurrRange_ = info->rsCurrRange;
    for (const auto& [surfaceName, nodePid] : info->surfaceData) {
        frameRateManager_->UpdateSurfaceTime(surfaceName, nodePid, UIFWKType::FROM_SURFACE);
    }
    frameRateManager_->SetIsGameNodeOnTree(info->isGameNodeOnTree);
}

void HgmContext::TransformNodeToLinkersRateMap(const sptr<HgmProcessToServiceInfo>& info)
{
    if (info->vRateMap.empty()) {
        return;
    }
    linkersRateMap_.clear();
    for (const auto& [nodeId, rate] : info->vRateMap) {
        std::vector<uint64_t> linkerIds = appVSyncDistributor_->GetSurfaceNodeLinkerIds(nodeId);
        if (rate != 0 && RSSystemParameters::GetVRateControlEnabled()) {
            for (auto& linkerId : linkerIds) {
                linkersRateMap_.emplace(linkerId, rate);
                RS_OPTIONAL_TRACE_NAME_FMT("NotifyVRates linkerid = %" PRIu64 " nodeId=%" PRIu64
                    " rate=%d", linkerId, nodeId, rate);
                RS_LOGD("TransformNodeToLinkersRateMap linkerid = %{public}" PRIu64 " nodeId=%{public}"
                    PRIu64 " rate=%{public}d", linkerId, nodeId, rate);
            }
        }
        auto iter = lastVSyncRateMap_.find(nodeId);
        if (iter != lastVSyncRateMap_.end() && iter->second == rate) {
            continue;
        }
        if (RSSystemParameters::GetVRateControlEnabled()) {
            needPostTask_.exchange(true);
        }
    }
    lastVSyncRateMap_ = info->vRateMap;
}

void HgmContext::SetServiceToProcessInfo(sptr<HgmServiceToProcessInfo> serviceToProcessInfo)
{
    if (serviceToProcessInfo == nullptr) {
        RS_LOGE("HgmRPContext serviceToProcessInfo is null");
        return;
    }
    serviceToProcessInfo->pendingScreenRefreshRate = hgmCore_.GetPendingScreenRefreshRate();
    serviceToProcessInfo->pendingConstraintRelativeTime = hgmCore_.GetPendingConstraintRelativeTime();
    if (hgmDataChangeTypes_.test(HgmDataChangeType::ADAPTIVE_VSYNC)) {
        serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::ADAPTIVE_VSYNC);
        serviceToProcessInfo->isAdaptive = isAdaptive_;
        serviceToProcessInfo->gameNodeName = gameNodeName_;
    }
    if (hgmDataChangeTypes_.test(HgmDataChangeType::HGM_CONFIG_DATA)) {
        serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::HGM_CONFIG_DATA);
        serviceToProcessInfo->rpHgmConfigData = std::move(rpHgmConfigData_);
        serviceToProcessInfo->ltpoEnabled = ltpoEnabled_;
        serviceToProcessInfo->isDelayMode = isDelayMode_;
        serviceToProcessInfo->pipelineOffsetPulseNum = pipelineOffsetPulseNum_;
    }
    serviceToProcessInfo->isPowerIdle = HgmEnergyConsumptionPolicy::Instance().GetPowerIdle();
    hgmDataChangeTypes_.reset();
    RS_TRACE_NAME_FMT("HgmContext::SetServiceToProcessInfo %d, %s",
        serviceToProcessInfo->pendingScreenRefreshRate, serviceToProcessInfo->hgmDataChangeTypes.to_string().c_str());
}

void HgmContext::ProcessHgmFrameRate(
    uint64_t timestamp, uint64_t vsyncId,
    const sptr<HgmProcessToServiceInfo>& processToServiceInfo, sptr<HgmServiceToProcessInfo> serviceToProcessInfo)
{
    HandleHgmProcessInfo(processToServiceInfo);
    currVsyncId_ = vsyncId;

    // Check and processing refresh rate task.
    frameRateManager_->ProcessPendingRefreshRate(
        timestamp, vsyncId, rsVSyncDistributor_->GetRefreshRate(), rsVSyncDistributor_->IsUiDvsyncOn());

    SetServiceToProcessInfo(serviceToProcessInfo);

    if (rsFrameRateLinker_ != nullptr) {
        auto rsCurrRange = rsCurrRange_;
        rsCurrRange.type_ = RS_ANIMATION_FRAME_RATE_TYPE;
        HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsCurrRange);
        rsFrameRateLinker_->SetExpectedRange(rsCurrRange);
        RS_TRACE_NAME_FMT("rsCurrRange = (%d, %d, %d)", rsCurrRange.min_, rsCurrRange.max_, rsCurrRange.preferred_);
    }
    rsCurrRange_.IsValid() ?
        frameRateManager_->GetRsFrameRateTimer().Start() : frameRateManager_->GetRsFrameRateTimer().Stop();
    
    bool needRefresh = frameRateManager_->UpdateUIFrameworkDirtyNodes(
        processToServiceInfo->uiFrameworkDirtyNodeNameMap, timestamp);
    bool setHgmTaskFlag = hgmCore_.SetHgmTaskFlag(false);

    bool vrateStatusChange = SetVSyncRatesChangeStatus(false);
    bool isVideoCallVsyncChange = HgmEnergyConsumptionPolicy::Instance().GetVideoCallVsyncChange();
    if (!vrateStatusChange && !setHgmTaskFlag && !needRefresh && !isVideoCallVsyncChange &&
        hgmCore_.GetPendingScreenRefreshRate() == frameRateManager_->GetCurrRefreshRate()) {
        return;
    }

    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, timestamp,
        rsFrameRateLinker = rsFrameRateLinker_, appFrameRateLinkers = frameRateLinkerMap_.Get(),
        linkersRateMap = linkersRateMap_] {
        RS_TRACE_NAME("ProcessHgmFrameRate");
        frameRateManager->UniProcessDataForLtpo(timestamp, rsFrameRateLinker, appFrameRateLinkers, linkersRateMap);
    });
}

void HgmContext::AddScreenToHgm(ScreenId screenId)
{
    RS_LOGI("%{public}s in screenId:%{public}" PRIu64, __func__, screenId);
    HgmTaskHandleThread::Instance().PostSyncTask([this, screenId] {
        RSScreenManager* scmFromHgm = hgmCore_.GetScreenManager();
        RSScreenModeInfo rsScreenModeInfo;
        scmFromHgm->GetScreenActiveMode(screenId, rsScreenModeInfo);
        int32_t initModeId = rsScreenModeInfo.GetScreenModeId();
        const auto& capability = scmFromHgm->GetScreenCapability(screenId);
        ScreenInfo curScreenInfo = scmFromHgm->QueryScreenInfo(screenId);
        ScreenSize screenSize =
            { curScreenInfo.width, curScreenInfo.height, capability.GetPhyWidth(), capability.GetPhyHeight() };
        RS_LOGI("%{public}s: add screen: w * h: [%{public}u * %{public}u], capability w * h: "
            "[%{public}u * %{public}u]", __func__, curScreenInfo.width, curScreenInfo.height,
            capability.GetPhyWidth(), capability.GetPhyHeight());
        if (hgmCore_.AddScreen(screenId, initModeId, screenSize, scmFromHgm->GetScreenSupportedModes(screenId))) {
            RS_LOGE("%{public}s failed to add screen : %{public}" PRIu64, __func__, screenId);
            return;
        }
    });
}

void HgmContext::RemoveScreenFromHgm(ScreenId screenId)
{
    RS_LOGI("%{public}s in screenId:%{public}" PRIu64, __func__, screenId);
    HgmTaskHandleThread::Instance().PostTask([this, screenId] {
        RS_LOGI("remove screen, screenId: %{public}" PRIu64, screenId);
        if (hgmCore_.RemoveScreen(screenId)) {
            RS_LOGW("failed to remove screen : %{public}" PRIu64, screenId);
        }
    });
}

void HgmContext::CleanAllWhenServiceConnectionDie(pid_t remotePid)
{
    renderServiceHandler_->PostSyncTask([this, remotePid] {
        frameRateLinkerMap_.FilterFrameRateLinkerByPid(remotePid);
    });

    HgmTaskHandleThread::Instance().ScheduleTask([frameRateManager = frameRateManager_, remotePid] {
        RS_TRACE_NAME_FMT("CleanHgmEvent %d", remotePid);
        HgmConfigCallbackManager::GetInstance()->UnRegisterHgmConfigChangeCallback(remotePid);
        frameRateManager->CleanVote(remotePid);
    }).wait();
}

void HgmContext::CreateFrameRateLinker(const std::string& name, FrameRateLinkerId id, NodeId windowNodeId)
{
    auto linker = std::make_shared<RSRenderFrameRateLinker>(id);
    linker->SetVsyncName(name);
    linker->SetWindowNodeId(windowNodeId);
    frameRateLinkerMap_.RegisterFrameRateLinker(linker);
}

void HgmContext::NotifyDynamicModeEvent(bool enableDynamicModeEvent)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, enableDynamicModeEvent] {
        frameRateManager->HandleDynamicModeEvent(enableDynamicModeEvent);
    });
}

void HgmContext::NotifyRefreshRateEvent(pid_t pid, const EventInfo& eventInfo)
{
    if (VOTER_SCENE_BLUR == eventInfo.eventName) {
        RsFrameBlurPredict::GetInstance().TakeEffectBlurScene(eventInfo);
        return;
    }

    if (VOTER_SCENE_GPU == eventInfo.eventName) {
        RsFrameReport::GetInstance().ReportScbSceneInfo(eventInfo.description, eventInfo.eventStatus);
        return;
    }

    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, pid, eventInfo] {
        frameRateManager->HandleRefreshRateEvent(pid, eventInfo);
    });
}

ErrCode HgmContext::NotifyLightFactorStatus(pid_t pid, int32_t lightFactorStatus)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, pid, lightFactorStatus] {
        frameRateManager->HandleLightFactorStatus(pid, lightFactorStatus);
    });
    return ERR_OK;
}

ErrCode HgmContext::NotifyAppStrategyConfigChangeEvent(pid_t pid, const std::string& pkgName,
    const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, pid, pkgName, newConfig] {
        frameRateManager->HandleAppStrategyConfigEvent(pid, pkgName, newConfig);
    });
    return ERR_OK;
}

uint32_t HgmContext::GetScreenCurrentRefreshRate(ScreenId id)
{
    uint32_t rate = HgmTaskHandleThread::Instance().ScheduleTask([this, id] {
        return hgmCore_.GetScreenCurrentRefreshRate(id);
    }).get();
    if (rate == 0) {
        RS_LOGW("GetScreenCurrentRefreshRate failed to get current refreshrate of screen : %{public}" PRIu64, id);
    }
    return rate;
}

void HgmContext::SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range,
    int32_t animatorExpectedFrameRate)
{
    auto linker = frameRateLinkerMap_.GetFrameRateLinker(id);
    if (linker == nullptr) {
        RS_LOGW("SyncFrameRateRange there is no frameRateLinker for id %{public}" PRIu64, id);
        return;
    }
    linker->SetExpectedRange(range);
    linker->SetAnimatorExpectedFrameRate(animatorExpectedFrameRate);
    if (range.type_ != NATIVE_VSYNC_FRAME_RATE_TYPE) {
        return;
    }
    auto conn = appVSyncDistributor_->GetVSyncConnection(id);
    if (conn == nullptr) {
        return;
    }
    std::weak_ptr<RSRenderFrameRateLinker> weakPtr = linker;
    conn->RegisterRequestNativeVSyncCallback([weakPtr] {
        RS_TRACE_NAME("NativeVSync request frame, update timepoint");
        auto linker = weakPtr.lock();
        if (linker == nullptr) {
            return;
        }
        linker->UpdateNativeVSyncTimePoint();
    });
}

int32_t HgmContext::RegisterHgmRefreshRateUpdateCallback(pid_t pid, sptr<RSIHgmConfigChangeCallback> callback)
{
    HgmTaskHandleThread::Instance().PostSyncTask([pid, callbackFunc = std::move(callback)] {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmRefreshRateUpdateCallback(pid, callbackFunc);
    });
    return StatusCode::SUCCESS;
}

int32_t HgmContext::RegisterHgmConfigChangeCallback(pid_t pid, sptr<RSIHgmConfigChangeCallback> callback)
{
    if (!callback) {
        RS_LOGD("RegisterHgmConfigChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmTaskHandleThread::Instance().PostSyncTask([pid, callbackFunc = std::move(callback)] {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmConfigChangeCallback(pid, callbackFunc);
    });
    return StatusCode::SUCCESS;
}

int32_t HgmContext::RegisterHgmRefreshRateModeChangeCallback(pid_t pid, sptr<RSIHgmConfigChangeCallback> callback)
{
    if (!callback) {
        RS_LOGD("RegisterHgmRefreshRateModeChangeCallback: callback is nullptr");
        return StatusCode::INVALID_ARGUMENTS;
    }

    HgmTaskHandleThread::Instance().PostSyncTask([pid, callbackFunc = std::move(callback)] {
        HgmConfigCallbackManager::GetInstance()->RegisterHgmRefreshRateModeChangeCallback(pid, callbackFunc);
    });
    return StatusCode::SUCCESS;
}

int32_t HgmContext::GetCurrentRefreshRateMode()
{
    return HgmTaskHandleThread::Instance().ScheduleTask([this] {
        return hgmCore_.GetCurrentRefreshRateMode();
    }).get();
}

std::vector<int32_t> HgmContext::GetScreenSupportedRefreshRates(ScreenId id)
{
    return HgmTaskHandleThread::Instance().ScheduleTask([this, id] {
        return hgmCore_.GetScreenComponentRefreshRates(id);
    }).get();
}

void HgmContext::SetWindowExpectedRefreshRate(pid_t pid, const std::unordered_map<uint64_t, EventInfo>& eventInfos)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, pid, eventInfos] {
        auto& softVsyncMgr = frameRateManager->SoftVSyncMgrRef();
        softVsyncMgr.SetWindowExpectedRefreshRate(pid, eventInfos);
    });
}

void HgmContext::SetWindowExpectedRefreshRate(pid_t pid, const std::unordered_map<std::string, EventInfo>& eventInfos)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, pid, eventInfos] {
        auto& softVsyncMgr = frameRateManager->SoftVSyncMgrRef();
        softVsyncMgr.SetWindowExpectedRefreshRate(pid, eventInfos);
    });
}

void HgmContext::SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate)
{
    RS_TRACE_NAME_FMT("HgmContext::SetScreenRefreshRate");
    HgmTaskHandleThread::Instance().PostTask([this, id, sceneId, rate] {
        int32_t setResult = hgmCore_.SetScreenRefreshRate(id, sceneId, rate);
        if (setResult != 0) {
            RS_LOGW("SetScreenRefreshRate request of screen %{public}" PRIu64 " of rate %{public}d is refused",
                id, rate);
            return;
        }
    });
}

void HgmContext::SetRefreshRateMode(int32_t refreshRateMode)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "HgmContext::SetRefreshRateMode");
    HgmTaskHandleThread::Instance().PostTask([this, refreshRateMode] {
        int32_t setResult = hgmCore_.SetRefreshRateMode(refreshRateMode);
        RSSystemProperties::SetHgmRefreshRateModesEnabled(std::to_string(refreshRateMode));
        if (setResult != 0) {
            RS_LOGW("SetRefreshRateMode mode %{public}d is not supported", refreshRateMode);
        }
    });
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void HgmContext::HandleTouchEvent(pid_t pid, int32_t touchStatus, int32_t touchCnt)
{
    frameRateManager_->HandleTouchEvent(pid, touchStatus, touchCnt);
}

void HgmContext::NotifyPackageEvent(pid_t pid, const std::vector<std::string>& packageList)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, pid, packageList] {
        frameRateManager->HandlePackageEvent(pid, packageList);
    });
}

void HgmContext::NotifyXComponentExpectedFrameRate(pid_t remotePid, const std::string& id, int32_t expectedFrameRate)
{
    HgmTaskHandleThread::Instance().PostTask([remotePid, id, expectedFrameRate] {
        HgmConfigCallbackManager::GetInstance()->SyncXComponentExpectedFrameRateCallback(
            remotePid, id, expectedFrameRate);
    });
}

void HgmContext::NotifyHgmConfigEvent(const std::string& eventName, bool state)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, eventName, state] {
        RS_LOGI("NotifyHgmConfigEvent: recive notify %{public}s, %{public}d", eventName.c_str(), state);
        if (eventName == "HGMCONFIG_HIGH_TEMP") {
            frameRateManager->HandleScreenExtStrategyChange(state, HGM_CONFIG_TYPE_THERMAL_SUFFIX);
        } else if (eventName == "IA_DRAG_SLIDE") {
            frameRateManager->HandleScreenExtStrategyChange(state, HGM_CONFIG_TYPE_DRAGSLIDE_SUFFIX);
        } else if (eventName == "IA_THROW_SLIDE") {
            frameRateManager->HandleScreenExtStrategyChange(state, HGM_CONFIG_TYPE_THROWSLIDE_SUFFIX);
        }
    });
}

void HgmContext::UnregisterFrameRateLinker(FrameRateLinkerId id)
{
    frameRateLinkerMap_.UnregisterFrameRateLinker(id);
}

bool HgmContext::NotifySoftVsyncRateDiscountEvent(
    uint32_t pid, const std::string& name, uint32_t rateDiscount, sptr<VSyncDistributor> appVSyncDistributor)
{
    if (!appVSyncDistributor) {
        return false;
    }

    std::vector<uint64_t> linkerIds = appVSyncDistributor_->GetVsyncNameLinkerIds(pid, name);
    if (linkerIds.empty()) {
        RS_LOGW("NotifySoftVsyncRateDiscountEvent: pid=%{public}d linkerIds is nullptr.", pid);
        return false;
    }

    if (!frameRateManager_->SetVsyncRateDiscountLTPO(linkerIds, rateDiscount)) {
        RS_LOGW("NotifySoftVsyncRateDiscountEvent: pid=%{public}d Set LTPO fail.", pid);
        return false;
    }

    VsyncError ret = appVSyncDistributor_->SetVsyncRateDiscountLTPS(pid, name, rateDiscount);
    if (ret != VSYNC_ERROR_OK) {
        RS_LOGW("NotifySoftVsyncRateDiscountEvent: pid=%{public}d Set LTPS fail.", pid);
        return false;
    }

    return true;
}

void HgmContext::UpdateRenderProcessPid(ScreenId screenId, pid_t pid)
{
    HgmTaskHandleThread::Instance().PostTask([frameRateManager = frameRateManager_, screenId, pid] {
        frameRateManager->UpdateRenderProcessPid(screenId, pid);
    });
}

void HgmContext::InitHfbcConfig()
{
    if (!HFBCParam::GetHfbcConfigMap().empty()) {
        HgmTaskHandleThread::Instance().PostTask([this] {
            HgmHfbcConfig& hfbcConfig = hgmCore_.GetHfbcConfig();
            RS_LOGI("InitHfbcCCM: postTask about hfbcConfig");
            hfbcConfig.SetHfbcConfigMap(HFBCParam::GetHfbcConfigMap());
            hfbcConfig.SetHfbcControlMode(HFBCParam::GetHfbcControlMode());
        });
    }
}
} // namespace Rosen
} // namespace OHOS