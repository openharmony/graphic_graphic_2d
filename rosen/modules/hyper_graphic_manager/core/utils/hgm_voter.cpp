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

#include "hgm_voter.h"

namespace OHOS {
namespace Rosen {
HgmVoter::HgmVoter(const std::vector<std::string>& voters): voters_(voters)
{
    HGM_LOGI("Construction of HgmVoter");
}

VoteInfo HgmVoter::ProcessVote()
{
    VoteInfo resultVoteInfo;
    VoteRange voteRange = { OLED_MIN_HZ, OLED_MAX_HZ };
    auto &[min, max] = voteRange;

    auto voterIter = voters_.begin();
    for (; voterIter != voters_.end(); ++voterIter) {
        if (ProcessVote(voterIter, resultVoteInfo, voteRange)) {
            break;
        }
    }

    // update effective status
    if (voterIter != voters_.end()) {
        ++voterIter;
        for (; voterIter != voters_.end(); ++voterIter) {
            if (auto iter = voteRecord_.find(*voterIter); iter != voteRecord_.end()) {
                iter->second.second = false;
            }
        }
    }

    resultVoteInfo.min = min;
    resultVoteInfo.max = max;

    return resultVoteInfo;
}

bool HgmVoter::ProcessVote(std::vector<std::string>::iterator& voterIter,
    VoteInfo& resultVoteInfo, VoteRange& voteRange)
{
    auto &voter = *voterIter;
    if (voteRecord_.find(voter) == voteRecord_.end()) {
        return false;
    }

    voteRecord_[voter].second = true;
    auto& voteInfos = voteRecord_[voter].first;
    if (voteInfos.empty()) {
        return false;
    }
    VoteInfo curVoteInfo = voteInfos.back();

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

std::pair<bool, bool> HgmVoter::MergeRangeByPriority(VoteRange& rangeRes, const VoteRange& curVoteRange)
{
    auto &[min, max] = rangeRes;
    auto &[minTemp, maxTemp] = curVoteRange;
    bool needMergeVoteInfo = false;
    if (minTemp > min) {
        min = minTemp;
        if (min >= max) {
            min = max;
            return {true, needMergeVoteInfo};
        }
    }
    if (maxTemp < max) {
        max = maxTemp;
        needMergeVoteInfo = true;
        if (min >= max) {
            max = min;
            return {true, needMergeVoteInfo};
        }
    }
    if (min == max) {
        return {true, needMergeVoteInfo};
    }
    return {false, needMergeVoteInfo};
}

bool HgmVoter::DeliverVote(const VoteInfo& voteInfo, bool eventStatus)
{
    if (std::find(voters_.begin(), voters_.end(), voteInfo.voterName) == voters_.end()) {
        return false;
    }
    voteRecord_.try_emplace(voteInfo.voterName, std::pair<std::vector<VoteInfo>, bool>({{}, true}));
    auto& vec = voteRecord_[voteInfo.voterName].first;

    // clear
    if (voteInfo.pid == 0 && !eventStatus) {
        if (!vec.empty()) {
            vec.clear();
            return true;
        }
        return false;
    }

    for (auto it = vec.begin(); it != vec.end(); it++) {
        if ((*it).pid != voteInfo.pid) {
            continue;
        }

        if (!eventStatus) {
            // remove
            it = vec.erase(it);
            return true;
        }
        if ((*it).min != voteInfo.min || (*it).max != voteInfo.max) {
            // modify
            vec.erase(it);
            vec.push_back(voteInfo);
            return true;
        }
        return false;
    }

    // add
    if (eventStatus) {
        vec.push_back(voteInfo);
        return true;
    }
    return false;
}
}
}