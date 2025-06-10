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

#include "hgm_frame_voter.h"

#include "hgm_core.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t DEFAULT_PRIORITY = 0;
    constexpr uint32_t VOTER_SCENE_PRIORITY_BEFORE_PACKAGES = 1;
    constexpr uint32_t VOTER_LTPO_PRIORITY_BEFORE_PACKAGES = 2;
    // CAUTION: with priority
    const std::string VOTER_NAME[] = {
        "VOTER_THERMAL",
        "VOTER_VIRTUALDISPLAY_FOR_CAR",
        "VOTER_VIRTUALDISPLAY",
        "VOTER_MUTIPHYSICALSCREEN",
        "VOTER_MULTISELFOWNEDSCREEN",
        "VOTER_POWER_MODE",
        "VOTER_DISPLAY_ENGINE",
        "VOTER_GAMES",
        "VOTER_ANCO",

        "VOTER_PAGE_URL",
        "VOTER_PACKAGES",
        "VOTER_LTPO",
        "VOTER_TOUCH",
        "VOTER_POINTER",
        "VOTER_SCENE",
        "VOTER_VIDEO",
        "VOTER_IDLE"
    };
}
HgmFrameVoter::HgmFrameVoter(HgmMultiAppStrategy& multiAppStrategy)
    : voters_(std::begin(VOTER_NAME), std::end(VOTER_NAME)), multiAppStrategy_(multiAppStrategy)
{
    HGM_LOGI("Construction of HgmFrameVoter");
    for (auto &voter : voters_) {
        voteRecord_[voter] = {{}, true};
    }
}

void HgmFrameVoter::CleanVote(pid_t pid)
{
    if (pidRecord_.count(pid) == 0) {
        return;
    }
    HGM_LOGW("CleanVote: i am [%{public}d], i died, clean my votes please.", pid);
    pidRecord_.erase(pid);

    for (auto& [voterName, voterInfo] : voteRecord_) {
        for (auto iter = voterInfo.first.begin(); iter != voterInfo.first.end();) {
            if (iter->pid == pid) {
                auto voter = iter->voterName;
                iter = voterInfo.first.erase(iter);
                MarkVoteChange(voter);
                break;
            }
            ++iter;
        }
    }
}

void HgmFrameVoter::DeliverVote(const VoteInfo& voteInfo, bool eventStatus)
{
    RS_TRACE_NAME_FMT("Deliver voter:%s(pid:%d extInfo:%s), status:%u, value:[%d-%d]",
        voteInfo.voterName.c_str(), voteInfo.pid, voteInfo.extInfo.c_str(),
        eventStatus, voteInfo.min, voteInfo.max);
    if (voteInfo.min > voteInfo.max) {
        HGM_LOGW("HgmFrameRateManager:invalid vote %{public}s(%{public}d %{public}s):[%{public}d, %{public}d]",
            voteInfo.voterName.c_str(), voteInfo.pid, voteInfo.extInfo.c_str(), voteInfo.min, voteInfo.max);
        return;
    }

    voteRecord_.try_emplace(voteInfo.voterName, std::pair<std::vector<VoteInfo>, bool>({{}, true}));
    auto& vec = voteRecord_[voteInfo.voterName].first;

    auto voter = voteInfo.voterName != "VOTER_PACKAGES" ? voteInfo.voterName : "";

    // clear
    if ((voteInfo.pid == 0) && (eventStatus == REMOVE_VOTE)) {
        if (!vec.empty()) {
            vec.clear();
            MarkVoteChange(voter);
        }
        return;
    }

    for (auto it = vec.begin(); it != vec.end(); it++) {
        if ((*it).pid != voteInfo.pid) {
            continue;
        }

        if (eventStatus == REMOVE_VOTE) {
            // remove
            it = vec.erase(it);
            MarkVoteChange(voter);
            return;
        }

        if ((*it).min != voteInfo.min || (*it).max != voteInfo.max) {
            // modify
            vec.erase(it);
            vec.push_back(voteInfo);
            MarkVoteChange(voter);
        } else if (voteInfo.voterName == "VOTER_PACKAGES") {
            // force update cause VOTER_PACKAGES is flag of safe_voter
            MarkVoteChange(voter);
        }
        return;
    }

    // add
    if (eventStatus == ADD_VOTE) {
        pidRecord_.insert(voteInfo.pid);
        vec.push_back(voteInfo);
        MarkVoteChange(voter);
    }
}

void HgmFrameVoter::ProcessVoteLog(const VoteInfo& curVoteInfo, bool isSkip)
{
    RS_TRACE_NAME_FMT("Process voter:%s(pid:%d), value:[%d-%d]%s",
        curVoteInfo.voterName.c_str(), curVoteInfo.pid, curVoteInfo.min, curVoteInfo.max, isSkip ? " skip" : "");
    HGM_LOGD("Process: %{public}s(%{public}d):[%{public}d, %{public}d]%{public}s",
        curVoteInfo.voterName.c_str(), curVoteInfo.pid, curVoteInfo.min, curVoteInfo.max, isSkip ? " skip" : "");
}

bool HgmFrameVoter::MergeLtpo2IdleVote(
    std::vector<std::string>::iterator &voterIter, VoteInfo& resultVoteInfo, VoteRange &mergedVoteRange)
{
    bool mergeSuccess = false;
    // [VOTER_LTPO, VOTER_IDLE)
    for (; voterIter != voters_.end() - 1; voterIter++) {
        if (voteRecord_.find(*voterIter) == voteRecord_.end()) {
            continue;
        }
        voteRecord_[*voterIter].second = true;
        auto vec = voteRecord_[*voterIter].first;
        if (vec.empty()) {
            continue;
        }

        VoteInfo curVoteInfo = vec.back();
        if (!multiAppStrategy_.CheckPidValid(curVoteInfo.pid)) {
            ProcessVoteLog(curVoteInfo, true);
            continue;
        }
        if (checkVote_ != nullptr && !checkVote_(*voterIter, curVoteInfo)) {
            continue;
        }
        if (isDragScene_ && curVoteInfo.voterName == "VOTER_TOUCH") {
            continue;
        }
        ProcessVoteLog(curVoteInfo, false);
        if (mergeSuccess) {
            mergedVoteRange.first = mergedVoteRange.first > curVoteInfo.min ? mergedVoteRange.first : curVoteInfo.min;
            if (curVoteInfo.max >= mergedVoteRange.second) {
                mergedVoteRange.second = curVoteInfo.max;
                resultVoteInfo.Merge(curVoteInfo);
            }
        } else {
            resultVoteInfo.Merge(curVoteInfo);
            mergedVoteRange = {curVoteInfo.min, curVoteInfo.max};
        }
        mergeSuccess = true;
    }
    return mergeSuccess;
}

bool HgmFrameVoter::ProcessVoteIter(std::vector<std::string>::iterator& voterIter,
    VoteInfo& resultVoteInfo, VoteRange& voteRange, bool &voterGamesEffective)
{
    VoteRange range;
    VoteInfo info;
    if (*voterIter == "VOTER_LTPO" && MergeLtpo2IdleVote(voterIter, info, range)) {
        auto [mergeVoteRange, mergeVoteInfo] = HgmVoter::MergeRangeByPriority(voteRange, range);
        if (mergeVoteInfo) {
            resultVoteInfo.Merge(info);
        }
        if (mergeVoteRange) {
            return true;
        }
    }

    auto &voter = *voterIter;
    if (voteRecord_.find(voter) == voteRecord_.end()) {
        return false;
    }
    voteRecord_[voter].second = true;
    auto& voteInfos = voteRecord_[voter].first;
    auto firstValidVoteInfoIter = std::find_if(voteInfos.begin(), voteInfos.end(), [this] (auto& voteInfo) {
        if (!multiAppStrategy_.CheckPidValid(voteInfo.pid)) {
            ProcessVoteLog(voteInfo, true);
            return false;
        }
        return true;
    });
    if (firstValidVoteInfoIter == voteInfos.end()) {
        return false;
    }
    auto curVoteInfo = *firstValidVoteInfoIter;
    if (checkVote_ != nullptr && !checkVote_(voter, curVoteInfo)) {
            return false;
    }
    if (voter == "VOTER_GAMES") {
        voterGamesEffective = true;
    }
    ProcessVoteLog(curVoteInfo, false);
    auto [mergeVoteRange, mergeVoteInfo] =
        HgmVoter::MergeRangeByPriority(voteRange, {curVoteInfo.min, curVoteInfo.max});
    if (mergeVoteInfo) {
        resultVoteInfo.Merge(curVoteInfo);
    }
    if (mergeVoteRange) {
        return true;
    }
    return false;
}

std::pair<VoteInfo, VoteRange> HgmFrameVoter::ProcessVote(const std::string& curScreenStrategyId,
    ScreenId curScreenId, int32_t curRefreshRateMode)
{
    if (updateVoteRule_ != nullptr) {
        voters_ = std::vector<std::string>(std::begin(VOTER_NAME), std::end(VOTER_NAME));
        updateVoteRule_(voters_);
    }

    VoteInfo resultVoteInfo;
    VoteRange voteRange = { OLED_MIN_HZ, OLED_MAX_HZ };
    auto &[min, max] = voteRange;

    bool voterGamesEffective = false;
    auto voterIter = voters_.begin();
    for (; voterIter != voters_.end(); ++voterIter) {
        if (ProcessVoteIter(voterIter, resultVoteInfo, voteRange, voterGamesEffective)) {
            break;
        }
    }
    voterGamesEffective_ = voterGamesEffective;
    // update effective status
    if (voterIter != voters_.end()) {
        ++voterIter;
        for (; voterIter != voters_.end(); ++voterIter) {
            if (auto iter = voteRecord_.find(*voterIter); iter != voteRecord_.end()) {
                iter->second.second = false;
            }
        }
    }
    if (voteRecord_["VOTER_PACKAGES"].second || voteRecord_["VOTER_LTPO"].second) {
        voteRecord_["VOTER_SCENE"].second = true;
    }
    HGM_LOGD("Process: Strategy:%{public}s Screen:%{public}d Mode:%{public}d -- VoteResult:{%{public}d-%{public}d}",
        curScreenStrategyId.c_str(), static_cast<int>(curScreenId), curRefreshRateMode, min, max);
    return {resultVoteInfo, voteRange};
}
} // namespace Rosen
} // namespace OHOS
