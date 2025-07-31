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
#include "parameters.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"

namespace OHOS {
namespace Rosen {

HgmContext::HgmContext()
{
    rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>(
        [](const RSRenderFrameRateLinker& linker) { HgmCore::Instance().SetHgmTaskFlag(true); });
}

void HgmContext::InitHgmTaskHandleThread(
    sptr<VSyncController> rsVSyncController, sptr<VSyncController> appVSyncController,
    sptr<VSyncGenerator> vsyncGenerator, sptr<VSyncDistributor> appVSyncDistributor)
{
    RS_LOGI("HgmTaskHandleThread init");
    auto forceUpdateTask = [this](bool idleTimerExpired, bool forceUpdate) {
        RSMainThread::Instance()->PostTask([this, idleTimerExpired, forceUpdate]() {
            ForceUpdateRenderService(idleTimerExpired, forceUpdate);
        });
    };
    HgmTaskHandleThread::Instance().PostSyncTask([
        forceUpdateTask, rsVSyncController,
        appVSyncController, vsyncGenerator, appVSyncDistributor]() {
            auto frameRateMgr = OHOS::Rosen::HgmCore::Instance().GetFrameRateMgr();
            if (frameRateMgr == nullptr) {
                return;
            }
            frameRateMgr->SetForceUpdateCallback(forceUpdateTask);
            frameRateMgr->Init(rsVSyncController, appVSyncController, vsyncGenerator, appVSyncDistributor);
        });
}

void HgmContext::ForceUpdateRenderService(bool idleTimerExpired, bool forceUpdate)
{
    RS_TRACE_NAME_FMT("HgmContext::TimerExpiredCallback Run idleTimerExpiredFlag: %s forceUpdateFlag: %s",
        idleTimerExpired ? "True" : "False", forceUpdate ? "True" : "False");
    if(lastForceUpdateVsyncId_ != currVsyncId_) {
        lastForceUpdateVsyncId_ = currVsyncId_;
        RSMainThread::Instance()->SetForceUpdateUniRenderFlag(forceUpdate);
        RSMainThread::Instance()->RequestNextVSync("ltpoForceUpdate");
    }
}

int32_t HgmContext::FrameRateGetFunc(
    const RSPropertyUnit unit, float velocity, int32_t area, int32_t length)
{
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr != nullptr) {
        return frameRateMgr->GetExpectedFrameRate(unit, velocity, area, length);
    }
    return 0;
}

void HgmContext::ProcessHgmFrameRate(
    uint64_t timestamp, sptr<VSyncDistributor> rsVSyncDistributor, uint64_t vsyncId)
{
    currVsyncId_ = vsyncId;
    int changed = 0;
    if (bool enable = RSSystemParameters::GetShowRefreshRateEnabled(&changed); changed != 0) {
        RSRealtimeRefreshRateManager::Instance().SetShowRefreshRateEnabled(enable, 1);
    }
    bool isUiDvsyncOn = rsVSyncDistributor != nullptr ? rsVSyncDistributor->IsUiDvsyncOn() : false;
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr || rsVSyncDistributor == nullptr) {
        return;
    }

    static std::once_flag initUIFwkTableFlag;
    std::call_once(initUIFwkTableFlag, [this]() {
        if (auto config = HgmCore::Instance().GetPolicyConfigData(); config != nullptr) {
            RSMainThread::Instance()->GetContext().SetUiFrameworkTypeTable(config->appBufferList_);
        }
    });

    // Check and processing refresh rate task.
    frameRateMgr->ProcessPendingRefreshRate(
        timestamp, vsyncId, rsVSyncDistributor->GetRefreshRate(), isUiDvsyncOn);
    if (rsFrameRateLinker_ != nullptr) {
        auto rsCurrRange = rsCurrRange_;
        rsCurrRange.type_ = RS_ANIMATION_FRAME_RATE_TYPE;
        HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsCurrRange);
        rsFrameRateLinker_->SetExpectedRange(rsCurrRange);
        RS_TRACE_NAME_FMT("rsCurrRange = (%d, %d, %d)", rsCurrRange.min_, rsCurrRange.max_, rsCurrRange.preferred_);
    }
    rsCurrRange_.IsValid() ? frameRateMgr->GetRsFrameRateTimer().Start() : frameRateMgr->GetRsFrameRateTimer().Stop();
    
    bool needRefresh = frameRateMgr->UpdateUIFrameworkDirtyNodes(
        RSMainThread::Instance()->GetContext().GetUiFrameworkDirtyNodes(), timestamp);
    bool setHgmTaskFlag = HgmCore::Instance().SetHgmTaskFlag(false);
    auto& rsVsyncRateReduceManager = RSMainThread::Instance()->GetRSVsyncRateReduceManager();
    bool vrateStatusChange = rsVsyncRateReduceManager.SetVSyncRatesChangeStatus(false);
    bool isVideoCallVsyncChange = HgmEnergyConsumptionPolicy::Instance().GetVideoCallVsyncChange();
    if (!vrateStatusChange && !setHgmTaskFlag && !needRefresh && !isVideoCallVsyncChange &&
        HgmCore::Instance().GetPendingScreenRefreshRate() == frameRateMgr->GetCurrRefreshRate()) {
        return;
    }
    HgmTaskHandleThread::Instance().PostTask([timestamp, rsFrameRateLinker = rsFrameRateLinker_,
        appFrameRateLinkers = RSMainThread::Instance()->GetContext().GetFrameRateLinkerMap().Get(),
        linkers = rsVsyncRateReduceManager.GetVrateMap()]() mutable {
            RS_TRACE_NAME("ProcessHgmFrameRate");
            if (auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr(); frameRateMgr != nullptr) {
                frameRateMgr->UniProcessDataForLtpo(timestamp, rsFrameRateLinker, appFrameRateLinkers, linkers);
            }
        });
}
} // namespace Rosen
} // namespace OHOS