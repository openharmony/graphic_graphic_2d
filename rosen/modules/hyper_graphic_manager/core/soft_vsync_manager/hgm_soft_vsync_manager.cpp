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
#include "hgm_soft_vsync_manager.h"

#include "animation/rs_frame_rate_range.h"
#include "common/rs_optional_trace.h"
#include "hgm_core.h"
#include "hgm_voter.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    const std::string VOTER_NAME[] = {
        "VOTER_VRATE",
        "VOTER_VIDEOCALL",
        "VOTER_GAMEFRAMEINSERTION",
        "VOTER_HIGH",
        "VOTER_MID",
        "VOTER_LOW",
    };
}

HgmSoftVSyncManager::HgmSoftVSyncManager()
{
    HGM_LOGI("Construction of HgmSoftVSyncManager");
}

void HgmSoftVSyncManager::InitController(std::weak_ptr<HgmVSyncGeneratorController> controller,
                                         sptr<VSyncDistributor> appDistributor)
{
    controller_ = controller;
    appDistributor_ = appDistributor;
}

void HgmSoftVSyncManager::DeliverSoftVote(FrameRateLinkerId linkerId, const VoteInfo& voteInfo, bool eventStatus)
{
    FrameRateRange frameRateRange(voteInfo.min, voteInfo.max, voteInfo.max);
    if (!frameRateRange.IsValid()) {
        return;
    }

    if (linkerVoteMap_.find(linkerId) == linkerVoteMap_.end()) {
        std::vector<std::string> voters(std::begin(VOTER_NAME), std::end(VOTER_NAME));
        linkerVoteMap_.try_emplace(linkerId, std::make_shared<HgmVoter>(voters));
    }

    auto hgmVoter = linkerVoteMap_[linkerId];
    if (hgmVoter && hgmVoter->DeliverVote(voteInfo, eventStatus)) {
        VoteInfo voteInfo = hgmVoter->ProcessVote();
        appVoteData_[linkerId] = voteInfo.max;
    }
}

void HgmSoftVSyncManager::HandleLinkers()
{
    // Clear votes for non-existent linkers
    for (auto iter = linkerVoteMap_.begin(); iter != linkerVoteMap_.end();) {
        if (appFrameRateLinkers_.count(iter->first) == 0) {
            iter = linkerVoteMap_.erase(iter);
        } else {
            ++iter;
        }
    }

    winLinkerMap_.clear();
    vsyncLinkerMap_.clear();
    for (auto linker : appFrameRateLinkers_) {
        if (linker.second == nullptr) {
            continue;
        }

        winLinkerMap_[linker.second->GetWindowNodeId()] = linker.first;

        vsyncLinkerMap_.try_emplace(linker.second->GetVsyncName(), std::vector<FrameRateLinkerId>{});
        vsyncLinkerMap_[linker.second->GetVsyncName()].emplace_back(linker.first);
    }
}

void HgmSoftVSyncManager::SetWindowExpectedRefreshRate(pid_t pid,
                                                       const std::unordered_map<WindowId, EventInfo>& voters)
{
    for (const auto& voter : voters) {
        WindowId winId = voter.first;
        EventInfo eventInfo = voter.second;

        auto winLinker = winLinkerMap_.find(winId);
        if (winLinker != winLinkerMap_.end()) {
            FrameRateLinkerId linkerId = winLinker->second;
            DeliverSoftVote(
                linkerId,
                {eventInfo.eventName, eventInfo.minRefreshRate, eventInfo.maxRefreshRate, pid},
                eventInfo.eventStatus);
        }
    }
}

void HgmSoftVSyncManager::SetWindowExpectedRefreshRate(pid_t pid,
                                                       const std::unordered_map<VsyncName, EventInfo>& voters)
{
    for (const auto& voter : voters) {
        VsyncName vsyncName = voter.first;
        EventInfo eventInfo = voter.second;

        auto vsyncLinker = vsyncLinkerMap_.find(vsyncName);
        if (vsyncLinker != vsyncLinkerMap_.end()) {
            std::vector<FrameRateLinkerId> linkerIds = vsyncLinker->second;
            for (const auto& linkerId : linkerIds) {
                DeliverSoftVote(
                    linkerId,
                    {eventInfo.eventName, eventInfo.minRefreshRate, eventInfo.maxRefreshRate, pid},
                    eventInfo.eventStatus);
            }
        }
    }
}

bool HgmSoftVSyncManager::CollectFrameRateChange(FrameRateRange finalRange,
                                                 std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
                                                 const FrameRateLinkerMap& appFrameRateLinkers,
                                                 const uint32_t currRefreshRate)
{
    if (controller_ == nullptr) {
        HGM_LOGE("no valid controller, cannot work correctly, maybe Init() wasn't executed correctly.");
        return false;
    }
    auto sharedController = controller_.lock();
    if (sharedController == nullptr) {
        HGM_LOGE("no valid controller, cannot work correctly, maybe Init() wasn't executed correctly.");
        return false;
    }
    Reset();
    bool frameRateChanged = false;
    bool controllerRateChanged = false;
    auto rsFrameRate = HgmSoftVSyncManager::GetDrawingFrameRate(currRefreshRate, finalRange);
    controllerRate_ = rsFrameRate > 0 ? rsFrameRate : sharedController->GetCurrentRate();
    if (controllerRate_ != sharedController->GetCurrentRate()) {
        rsFrameRateLinker->SetFrameRate(controllerRate_);
        controllerRateChanged = true;
        frameRateChanged = true;
    }

    RS_TRACE_NAME_FMT("CollectFrameRateChange rsFrameRate: %d, finalRange = (%d, %d, %d)",
        rsFrameRate, finalRange.min_, finalRange.max_, finalRange.preferred_);
    RS_TRACE_INT("PreferredFrameRate", static_cast<int>(finalRange.preferred_));

    appChangeData_.clear();
    for (auto linker : appFrameRateLinkers) {
        if (linker.second == nullptr) {
            continue;
        }
        auto expectedRange = linker.second->GetExpectedRange();
        CalcAppFrameRate(linker, expectedRange);
        
    }
    return frameRateChanged;
}

void HgmSoftVSyncManager::CalcAppFrameRate(
    const std::pair<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>>& linker,
    FrameRateRange& expectedRange,
    bool& frameRateChanged,
    bool controllerRateChanged,
    const uint32_t currRefreshRate)
{
    bool isChanged = false;
    if (HgmEnergyConsumptionPolicy::Instance().GetVideoCallFrameRate(
        ExtractPid(linker.first), linker.second->GetVsyncName(), expectedRange)) {
        isChanged = true;
    }
    if (CollectVRateChange(linker.first, expectedRange)) {
        isChanged = true;
    }
    if (!isChanged && appVoteData_.count(linker.first)) {
        expectedRange.preferred_ = appVoteData_[linker.first];
    }
    auto appFrameRate = !isPerformanceFirst_ && expectedRange.type_ != NATIVE_VSYNC_FRAME_RATE_TYPE ?
                        OLED_NULL_HZ : HgmSoftVSyncManager::GetDrawingFrameRate(currRefreshRate, expectedRange);
    if (appFrameRate != linker.second->GetFrameRate() || controllerRateChanged) {
        linker.second->SetFrameRate(appFrameRate);
        appChangeData_[linker.second->GetId()] = appFrameRate;
        HGM_LOGD("HgmSoftVSyncManager: appChangeData linkerId = %{public}" PRIu64 ", %{public}d",
            linker.second->GetId(), appFrameRate);
        frameRateChanged = true;
    }
    if (expectedRange.min_ == OLED_NULL_HZ && expectedRange.preferred_ == OLED_NULL_HZ &&
        (expectedRange.max_ == OLED_144_HZ || expectedRange.max_ == OLED_NULL_HZ)) {
        return;
    }
    RS_TRACE_NAME_FMT("HgmSoftVSyncManager::UniProcessData multiAppFrameRate: pid = %d, linkerId = %ld, "\
        "appFrameRate = %d, appRange = (%d, %d, %d)", ExtractPid(linker.first), linker.second->GetId(),
        appFrameRate, expectedRange.min_, expectedRange.max_, expectedRange.preferred_);
}

uint32_t HgmSoftVSyncManager::GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range)
{
    if (refreshRate == 0 || range.preferred_ <= 0) {
        return 0;
    }

    uint32_t preferredFps = static_cast<uint32_t>(range.preferred_);
    if (!range.IsValid() || preferredFps > refreshRate) {
        return refreshRate;
    }

    // find the smallest value in range of [preferredFps, refreshRate], which can exactly divide refreshRate
    uint32_t divisor = refreshRate / preferredFps;
    while (divisor > 1) {
        if (refreshRate % divisor == 0) {
            break;
        }
        divisor--;
    }
    return refreshRate / divisor;
}

bool HgmSoftVSyncManager::CollectVRateChange(uint64_t linkerId, FrameRateRange& finalRange)
{
    auto iter = vRatesMap_.find(linkerId);
    if (iter == vRatesMap_.end()) {
        RS_OPTIONAL_TRACE_NAME_FMT("CollectVRateChange not find pid = %d linkerId = %" PRIu64 " return",
            ExtractPid(linkerId), linkerId);
        HGM_LOGD("CollectVRateChange not find pid = %{public}d linkerId = %{public}" PRIu64 " return",
            ExtractPid(linkerId), linkerId);
        return false;
    }
    if (iter->second == 1 || iter->second == 0) {
        RS_OPTIONAL_TRACE_NAME_FMT("CollectVRateChange pid = %d , linkerId = %" PRIu64 ", vrate = %d return",
            ExtractPid(linkerId), linkerId, iter->second);
        HGM_LOGD("CollectVRateChange linkerId = %{public}" PRIu64 ",vrate = %{public}d return",
                linkerId, iter->second);
        return false;
    }
    int32_t& appFrameRate = finalRange.preferred_;
    // finalRange.preferred_ is 0 means that the appframerate want to be changed by self.
    if (appFrameRate != 0 && (finalRange.type_ != ACE_COMPONENT_FRAME_RATE_TYPE ||
        (finalRange.type_ == ACE_COMPONENT_FRAME_RATE_TYPE && /* ArkUI Vote */
        iter->second != std::numeric_limits<int>::max()))) { /*invisible window*/
        RS_OPTIONAL_TRACE_NAME_FMT("CollectVRateChange pid = %d , linkerId = %" PRIu64 ", vrate = %d return because "
            "changed by self, not arkui vote, not invisble window", ExtractPid(linkerId), linkerId, iter->second);
        HGM_LOGD("CollectVRateChange linkerId = %{public}" PRIu64 ",vrate = %{public}d return because changed by self,"
            " not arkui vote, not invisble window", linkerId, iter->second);
        return false;
    }

    appFrameRate = static_cast<int>(controllerRate_) / iter->second;
    // vrate is int::max means app need not refreshing
    if (appFrameRate == 0) {
        //appFrameRate value is 1  means that not refreshing.
        appFrameRate = 1;
    }

    finalRange.min_ = OLED_NULL_HZ;
    finalRange.max_ = OLED_144_HZ;

    RS_TRACE_NAME_FMT("CollectVRateChange modification pid = %d , linkerIdS = %" PRIu64 ",appFrameRate = %d,"
        " vrate = %d, rsFrameRate = %u", ExtractPid(linkerId), linkerId, appFrameRate, iter->second, controllerRate_);
    HGM_LOGD("CollectVRateChange modification linkerId = %{public}" PRIu64 ",appFrameRate = %{public}d,"
        " vrate = %{public}d, rsFrameRate = %{public}u", linkerId, appFrameRate, iter->second, controllerRate_);
    return true;
}

std::vector<std::pair<FrameRateLinkerId, uint32_t>> HgmSoftVSyncManager::GetSoftAppChangeData()
{
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appData(appChangeData_.begin(), appChangeData_.end());
    return appData;
}

void HgmSoftVSyncManager::Reset()
{
    controllerRate_ = 0;
    appChangeData_.clear();
}

void HgmSoftVSyncManager::UniProcessDataForLtpo(const std::map<uint64_t, int>& vRatesMap,
                                                const FrameRateLinkerMap& appFrameRateLinkers)
{
    vRatesMap_ = vRatesMap;
    appFrameRateLinkers_ = appFrameRateLinkers;
    HandleLinkers();
}

void HgmSoftVSyncManager::SetQosVSyncRate(const uint32_t currRefreshRate,
                                          const FrameRateLinkerMap& appFrameRateLinkers)
{
    for (const auto& data : appChangeData_) {
        auto linkerId = data.first;
        auto prefer = data.second;
        
        auto linker = appFrameRateLinkers.find(linkerId);
        if (linker != appFrameRateLinkers.end()  && appDistributor_ != nullptr) {
            appDistributor_->SetQosVSyncRate(linker->second->GetWindowNodeId(),
                                             currRefreshRate/prefer,
                                             false);
        }
    }
}
}
}