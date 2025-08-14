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
#include "hgm_command.h"
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
constexpr uint32_t SOFT_NATIVE_VSYNC_FRAME_RATE_TYPE = 6;
const std::string VRATE_CONTROL_MINIFPS = "minifps";
}

// LCOV_EXCL_START
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

bool HgmSoftVSyncManager::CollectFrameRateChange(FrameRateRange finalRange,
                                                 std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
                                                 const FrameRateLinkerMap& appFrameRateLinkers,
                                                 const uint32_t currRefreshRate)
{
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
        CalcAppFrameRate(linker, expectedRange, frameRateChanged, controllerRateChanged, currRefreshRate);
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
        expectedRange.preferred_ = static_cast<int32_t>(appVoteData_[linker.first]);
    }
    auto appFrameRate = isPerformanceFirst_ && expectedRange.type_ != SOFT_NATIVE_VSYNC_FRAME_RATE_TYPE ?
                        OLED_NULL_HZ : HgmSoftVSyncManager::GetDrawingFrameRate(currRefreshRate, expectedRange);
    if (CollectGameRateDiscountChange(linker.first, expectedRange, currRefreshRate)) {
        appFrameRate = static_cast<uint32_t>(expectedRange.preferred_);
    }
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

void HgmSoftVSyncManager::GetVRateMiniFPS(const std::shared_ptr<PolicyConfigData>& configData)
{
    if (configData == nullptr) {
        HGM_LOGE("GetVRateMiniFPS configData is nullptr use dafault value");
        return;
    }
    if (configData->vRateControlList_.find(VRATE_CONTROL_MINIFPS) == configData->vRateControlList_.end()) {
        HGM_LOGE("GetVRateMiniFPS VRATE_CONTROL_MINIFPS config is invalid use dafault value");
        return;
    }
    if (!XMLParser::IsNumber(configData->vRateControlList_[VRATE_CONTROL_MINIFPS])) {
        HGM_LOGE("GetVRateMiniFPS VRATE_CONTROL_MINIFPS config is Is Not Number use dafault value");
        return;
    }

    vrateControlMinifpsValue_ = static_cast<int32_t>(std::stoi(configData->vRateControlList_[VRATE_CONTROL_MINIFPS]));
    HGM_LOGI("GetVRateMiniFPS vrateControlMinifpsValue:%{public}d", vrateControlMinifpsValue_);
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
    if (appFrameRate != 0 && (finalRange.type_ != DRAG_SCENE_FRAME_RATE_TYPE ||
        (finalRange.type_ == DRAG_SCENE_FRAME_RATE_TYPE && /* ArkUI Vote */
        iter->second != std::numeric_limits<int>::max()))) { /* invisible window */
        RS_OPTIONAL_TRACE_NAME_FMT("CollectVRateChange pid = %d , linkerId = %" PRIu64 ", vrate = %d return because "
            "appFrameRate[%d] changed by self, not arkui vote[%u], not invisble window", ExtractPid(linkerId),
            linkerId, iter->second, appFrameRate, finalRange.type_);
        HGM_LOGD("CollectVRateChange linkerId = %{public}" PRIu64 ",vrate = %{public}d return "
            "appFrameRate[%{public}d]  because changed by self, not arkui vote[%{public}u], not invisble window",
            linkerId, iter->second, appFrameRate, finalRange.type_);
        return false;
    }

    appFrameRate = static_cast<int>(controllerRate_) / iter->second;
    if (iter->second == std::numeric_limits<int>::max()) {
        // invisible window 1 means app need not refreshing
        appFrameRate = 1;
    }

    // vrate is int::max means app need not refreshing
    if (appFrameRate == 0 && vrateControlMinifpsValue_ >= 0) {
        appFrameRate = vrateControlMinifpsValue_;
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
        if (linker != appFrameRateLinkers.end()  && appDistributor_ != nullptr && linker->second != nullptr) {
            // perfer equals 0 means keeping the original rhythm without skipping, just pass 1
            appDistributor_->SetQosVSyncRateByConnId(linker->second->GetId(),
                prefer == 0 ? 1 : currRefreshRate / prefer);
        }
    }
}

bool HgmSoftVSyncManager::CollectGameRateDiscountChange(uint64_t linkerId, FrameRateRange& expectedRange,
    const uint32_t currRefreshRate)
{
    auto iter = gameRateDiscountMap_.find(linkerId);
    if (iter == gameRateDiscountMap_.end()) {
        return false;
    }
    if (iter->second == 1 || iter->second == 0) {
        return false;
    }
    if (expectedRange.preferred_ == 0 && currRefreshRate == 0) {
        return false;
    }
    int32_t tmpPreferred = expectedRange.preferred_;
    int32_t tmpMin = expectedRange.min_;
    int32_t tmpMax = expectedRange.max_;
    if (expectedRange.preferred_ == 0) {
        expectedRange.preferred_ = static_cast<int32_t>(currRefreshRate) / static_cast<int32_t>(iter->second);
    } else {
        expectedRange.preferred_ = std::min(expectedRange.preferred_, static_cast<int32_t>(currRefreshRate)) /
            static_cast<int32_t>(iter->second);
    }
    expectedRange.min_ = expectedRange.preferred_;
    expectedRange.max_ = expectedRange.preferred_;

    int32_t drawingFrameRate = static_cast<int32_t>(GetDrawingFrameRate(currRefreshRate, expectedRange));
    if (drawingFrameRate != expectedRange.preferred_) {
        RS_TRACE_NAME_FMT("CollectGameRateDiscountChange failed, linkerId=%" PRIu64
            ", rateDiscount=%d, preferred=%d drawingFrameRate=%d currRefreshRate=%d",
            linkerId, iter->second, expectedRange.preferred_, drawingFrameRate, static_cast<int32_t>(currRefreshRate));
        HGM_LOGD("CollectGameRateDiscountChange failed, linkerId=%{public}" PRIu64
            ", rateDiscount=%{public}d, preferred=%{public}d drawingFrameRate=%{public}d currRefreshRate=%{public}d",
            linkerId, iter->second, expectedRange.preferred_, drawingFrameRate, static_cast<int32_t>(currRefreshRate));
        expectedRange.preferred_ = tmpPreferred;
        expectedRange.min_ = tmpMin;
        expectedRange.max_ = tmpMax;
        return false;
    }
    RS_TRACE_NAME_FMT("CollectGameRateDiscountChange linkerId=%" PRIu64
        ", rateDiscount=%d, preferred=%d currRefreshRate=%d",
        linkerId, iter->second, expectedRange.preferred_, static_cast<int32_t>(currRefreshRate));
    HGM_LOGD("CollectGameRateDiscountChange succeed, linkerId=%{public}" PRIu64
        ", rateDiscount=%{public}d, preferred=%{public}d currRefreshRate=%{public}d",
        linkerId, iter->second, expectedRange.preferred_, static_cast<int32_t>(currRefreshRate));
    return true;
}

void HgmSoftVSyncManager::EraseGameRateDiscountMap(pid_t pid)
{
    for (auto iter = gameRateDiscountMap_.begin(); iter != gameRateDiscountMap_.end(); ++iter) {
        if (ExtractPid(iter->first) == pid) {
            gameRateDiscountMap_.erase(iter);
            break;
        }
    }
}

void HgmSoftVSyncManager::SetVsyncRateDiscountLTPO(const std::vector<uint64_t>& linkerIds, uint32_t rateDiscount)
{
    for (auto linkerId : linkerIds) {
        gameRateDiscountMap_[linkerId] = rateDiscount;
    }
}
// LCOV_EXCL_STOP

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
}
}