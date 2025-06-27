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

#ifndef HGM_FRAME_VOTE_H
#define HGM_FRAME_VOTE_H

#include <unordered_map>
#include <unordered_set>

#include "hgm_multi_app_strategy.h"
#include "hgm_voter.h"

namespace OHOS {
namespace Rosen {
class HgmFrameVoter final {
public:
    using ChangeRangeCallbackFunc = std::function<void(const std::string&)>;
    using UpdateVoteRuleFunc = std::function<void(std::vector<std::string>&)>; // voters
    using CheckVoteFunc = std::function<bool(const std::string&, VoteInfo&)>; // vote, voteInfo

    explicit HgmFrameVoter(HgmMultiAppStrategy& multiAppStrategy);
    ~HgmFrameVoter() = default;

    const VoteRecord& GetVoteRecord() const { return voteRecord_; }
    const std::vector<std::string>& GetVoters() const { return voters_; }
    bool GetVoterGamesEffective() const { return voterGamesEffective_.load(); }

    bool IsDragScene() const { return isDragScene_; }
    void SetDragScene(bool isDragScene) { isDragScene_ = isDragScene; }
    void SetTouchUpLTPOFirst(bool isTouchUp);

    void CleanVote(pid_t pid);
    void DeliverVote(const VoteInfo& voteInfo, bool eventStatus);
    std::pair<VoteInfo, VoteRange> ProcessVote(const std::string& curScreenStrategyId,
        ScreenId curScreenId, int32_t curRefreshRateMode);

    void SetChangeRangeCallback(const ChangeRangeCallbackFunc& func) { markVoteChange_ = func; }
    void SetUpdateVoteRuleCallback(const UpdateVoteRuleFunc& func) { updateVoteRule_ = func; }
    void SetCheckVoteCallback(const CheckVoteFunc& func) { checkVote_ = func; }

private:
    void ProcessVoteLog(const VoteInfo& curVoteInfo, bool isSkip);
    bool MergeLtpo2IdleVote(
        std::vector<std::string>::iterator& voterIter, VoteInfo& resultVoteInfo, VoteRange& mergedVoteRange);
    bool ProcessVoteIter(std::vector<std::string>::iterator& voterIter,
        VoteInfo& resultVoteInfo, VoteRange& voteRange, bool& voterGamesEffective);
    void MarkVoteChange(const std::string& voter = "")
    {
        if (markVoteChange_) {
            markVoteChange_(voter);
        }
    }
    bool NeedSkipVoterTouch()
    {
        if (existVoterLTPO_ && isTouchUpLTPOFirst_) {
            return true;
        }
        return false;
    }

    bool isDragScene_ = false;
    bool existVoterLTPO_ = false;
    bool isTouchUpLTPOFirst_ = false;
    std::atomic<bool> voterGamesEffective_ = false;
    std::unordered_set<pid_t> pidRecord_;
    VoteRecord voteRecord_;
    std::vector<std::string> voters_;

    HgmMultiAppStrategy& multiAppStrategy_;

    ChangeRangeCallbackFunc markVoteChange_{ nullptr };
    // param: voters
    UpdateVoteRuleFunc updateVoteRule_{ nullptr };
    // return: false means vote is invalid, should be skip
    CheckVoteFunc checkVote_{ nullptr };
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_VOTE_H