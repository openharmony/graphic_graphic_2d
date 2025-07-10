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

#include <gtest/gtest.h>
#include <limits>
#include <test_header.h>

#include "hgm_frame_rate_manager.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t delay_120Ms = 120;
}

class HgmFrameVoterTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: MergeRangeByPriority
 * @tc.desc: Verify the result of MergeRangeByPriority function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestMergeRangeByPriority, Function | SmallTest | Level0)
{
    VoteRange voteRange0 = { OLED_40_HZ, OLED_120_HZ };
    VoteRange voteRange1 = { OLED_30_HZ, OLED_40_HZ };
    VoteRange voteRange2 = { OLED_60_HZ, OLED_90_HZ };
    VoteRange voteRange3 = { OLED_120_HZ, OLED_144_HZ };
    VoteRange voteRange4 = { OLED_30_HZ, OLED_144_HZ };
    VoteRange voteRangeRes;

    voteRangeRes = voteRange0;
    HgmVoter::MergeRangeByPriority(voteRangeRes, voteRange1);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_40_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_40_HZ);

    voteRangeRes = voteRange0;
    HgmVoter::MergeRangeByPriority(voteRangeRes, voteRange2);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_60_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_90_HZ);

    voteRangeRes = voteRange0;
    HgmVoter::MergeRangeByPriority(voteRangeRes, voteRange3);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_120_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_120_HZ);

    voteRangeRes = voteRange0;
    HgmVoter::MergeRangeByPriority(voteRangeRes, voteRange4);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_40_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_120_HZ);
}

/**
 * @tc.name: TestGetVoteRecord
 * @tc.desc: Verify the result of GetVoteRecord function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestGetVoteRecord, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    const auto& voteRecord = mgr.frameVoter_.GetVoteRecord();
    auto iter = voteRecord.find("VOTER_GAMES");
    EXPECT_NE(iter, voteRecord.end());
    iter = voteRecord.find("NULL");
    EXPECT_EQ(iter, voteRecord.end());
}

/**
 * @tc.name: TestGetVoters
 * @tc.desc: Verify the result of GetVoters function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestGetVoters, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    const auto& voters = mgr.frameVoter_.GetVoters();
    auto iter = std::find(voters.begin(), voters.end(), "VOTER_GAMES");
    EXPECT_NE(iter, voters.end());
    iter = std::find(voters.begin(), voters.end(), "NULL");
    EXPECT_EQ(iter, voters.end());
}

/**
 * @tc.name: TestGetVoterGamesEffective
 * @tc.desc: Verify the result of GetVoterGamesEffective function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestGetVoterGamesEffective, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    mgr.frameVoter_.voterGamesEffective_ = false;
    EXPECT_EQ(mgr.frameVoter_.GetVoterGamesEffective(), false);
    mgr.frameVoter_.voterGamesEffective_ = true;
    EXPECT_EQ(mgr.frameVoter_.GetVoterGamesEffective(), true);
}

/**
 * @tc.name: TestDragScene
 * @tc.desc: Verify the result of DragScene function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestDragScene, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    mgr.frameVoter_.SetDragScene(false);
    EXPECT_EQ(mgr.frameVoter_.IsDragScene(), false);
    mgr.frameVoter_.SetDragScene(true);
    EXPECT_EQ(mgr.frameVoter_.IsDragScene(), true);
}

/**
 * @tc.name: TestCleanVote
 * @tc.desc: Verify the result of CleanVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestCleanVote, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    HgmFrameVoter hgmFrameVoter(HgmFrameVoter(mgr.multiAppStrategy_));

    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_90_HZ, OLED_120_HZ, DEFAULT_PID }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_GAMES"].first.empty(), false);
    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_90_HZ, OLED_120_HZ, 1 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_GAMES"].first.size(), 2);
    hgmFrameVoter.CleanVote(1);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_GAMES"].first.size(), 1);
}

/**
 * @tc.name: TestDeliverVote
 * @tc.desc: Verify the result of DeliverVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestDeliverVote, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    HgmFrameVoter hgmFrameVoter(HgmFrameVoter(mgr.multiAppStrategy_));

    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_120_HZ, OLED_90_HZ, DEFAULT_PID }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_GAMES"].first.empty(), true);

    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_90_HZ, OLED_120_HZ, DEFAULT_PID }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_GAMES"].first.empty(), false);

    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_60_HZ, OLED_120_HZ, DEFAULT_PID }, false);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_GAMES"].first.empty(), true);

    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_90_HZ, OLED_120_HZ, DEFAULT_PID }, true);

    hgmFrameVoter.DeliverVote({ "VOTER_PACKAGES", OLED_30_HZ, OLED_120_HZ, 2 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_PACKAGES"].first.back().min, OLED_30_HZ);

    hgmFrameVoter.DeliverVote({ "VOTER_PACKAGES", OLED_30_HZ, OLED_120_HZ, 2 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_PACKAGES"].first.back().max, OLED_120_HZ);

    hgmFrameVoter.DeliverVote({ "VOTER_LTPO", OLED_30_HZ, OLED_120_HZ, 3 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_LTPO"].first.back().min, OLED_30_HZ);

    hgmFrameVoter.DeliverVote({ "VOTER_LTPO", OLED_60_HZ, OLED_120_HZ, 3 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_LTPO"].first.back().min, OLED_60_HZ);

    hgmFrameVoter.DeliverVote({ "VOTER_LTPO", OLED_60_HZ, OLED_90_HZ, 3 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_LTPO"].first.back().max, OLED_90_HZ);

    hgmFrameVoter.DeliverVote({ "VOTER_LTPO", OLED_60_HZ, OLED_90_HZ, 3 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_LTPO"].first.back().max, OLED_90_HZ);

    std::string voterName = "";
    hgmFrameVoter.SetChangeRangeCallback([&](const std::string& voter) {
        voterName = voter;
    });

    hgmFrameVoter.DeliverVote({ "VOTER_LTPO", OLED_60_HZ, OLED_120_HZ, 4 }, true);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_LTPO"].first.back().max, OLED_120_HZ);
    EXPECT_EQ(voterName, "VOTER_LTPO");

    hgmFrameVoter.DeliverVote({ "VOTER_LTPO", OLED_60_HZ, OLED_120_HZ, 4 }, false);
    EXPECT_EQ(hgmFrameVoter.voteRecord_["VOTER_LTPO"].first.back().max, OLED_90_HZ);
}

/**
 * @tc.name: TestMergeLtpo2IdleVote
 * @tc.desc: Verify the result of MergeLtpo2IdleVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestMergeLtpo2IdleVote, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    HgmFrameVoter hgmFrameVoter(HgmFrameVoter(mgr.multiAppStrategy_));

    hgmFrameVoter.DeliverVote({ "VOTER_VIDEO", OLED_60_HZ, OLED_60_HZ, 3 }, true);
    std::shared_ptr<PolicyConfigData> policyConfigData = std::move(HgmCore::Instance().mPolicyConfigData_);
    HgmCore::Instance().mPolicyConfigData_ = nullptr;
    hgmFrameVoter.isDragScene_ = true;
    auto voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_TOUCH");
    VoteRange range;
    VoteInfo info;
    hgmFrameVoter.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.first, OLED_60_HZ);
    EXPECT_EQ(info.voterName, "VOTER_VIDEO");

    HgmCore::Instance().mPolicyConfigData_ = std::make_unique<PolicyConfigData>();
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_TOUCH");
    range = {0, 0};
    hgmFrameVoter.DeliverVote({ "VOTER_TOUCH", OLED_30_HZ, OLED_90_HZ, 1 }, true);
    hgmFrameVoter.multiAppStrategy_.pkgs_.push_back("testPkg");
    auto result = hgmFrameVoter.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(result, true);
    
    HgmCore::Instance().mPolicyConfigData_->videoFrameRateList_["testPkg"] = "1";
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_TOUCH");
    range = {0, 0};
    hgmFrameVoter.isDragScene_ = false;
    hgmFrameVoter.DeliverVote({ "VOTER_POINTER", OLED_120_HZ, OLED_120_HZ, 4 }, true);
    hgmFrameVoter.multiAppStrategy_.backgroundPid_.Put(4);
    hgmFrameVoter.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.first, OLED_60_HZ);
    EXPECT_EQ(info.voterName, "VOTER_TOUCH");

    hgmFrameVoter.multiAppStrategy_.foregroundPidAppMap_[3] = { 1, "testPkg" };
    hgmFrameVoter.voters_.insert(hgmFrameVoter.voters_.end() - 2, "NULL");
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_TOUCH");
    range = {0, 0};
    hgmFrameVoter.DeliverVote({ "VOTER_SCENE", OLED_60_HZ, OLED_120_HZ, 2 }, true);
    hgmFrameVoter.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_120_HZ);
    EXPECT_EQ(info.voterName, "VOTER_SCENE");

    // checkVote
    hgmFrameVoter.SetCheckVoteCallback([](const std::string&, VoteInfo&) { return true; });
    hgmFrameVoter.MergeLtpo2IdleVote(voterIter, info, range);
    hgmFrameVoter.SetCheckVoteCallback([](const std::string&, VoteInfo&) { return false; });
    hgmFrameVoter.MergeLtpo2IdleVote(voterIter, info, range);
    hgmFrameVoter.SetCheckVoteCallback(nullptr);
    hgmFrameVoter.MergeLtpo2IdleVote(voterIter, info, range);

    HgmCore::Instance().mPolicyConfigData_ = std::move(policyConfigData);
}

/**
 * @tc.name: TestMergeLtpo2IdleVote02
 * @tc.desc: Verify the result of MergeLtpo2IdleVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestMergeLtpo2IdleVote02, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    mgr.InitTouchManager();

    mgr.frameVoter_.DeliverVote({ "VOTER_LTPO", OLED_90_HZ, OLED_90_HZ, 1 }, true);
    mgr.frameVoter_.DeliverVote({ "VOTER_TOUCH", OLED_120_HZ, OLED_120_HZ, 2 }, true);
    VoteRange range;
    VoteInfo info;
    auto voterIter = std::find(mgr.frameVoter_.voters_.begin(), mgr.frameVoter_.voters_.end(), "VOTER_LTPO");
    mgr.touchManager_.ChangeState(TouchState::DOWN_STATE);
    mgr.frameVoter_.SetTouchUpLTPOFirstDynamicMode(DynamicModeType::TOUCH_EXT_ENABLED);
    mgr.frameVoter_.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_120_HZ);

    range = {0, 0};
    voterIter = std::find(mgr.frameVoter_.voters_.begin(), mgr.frameVoter_.voters_.end(), "VOTER_LTPO");
    mgr.touchManager_.ChangeState(TouchState::UP_STATE);
    mgr.frameVoter_.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_120_HZ);

    range = {0, 0};
    voterIter = std::find(mgr.frameVoter_.voters_.begin(), mgr.frameVoter_.voters_.end(), "VOTER_LTPO");
    mgr.frameVoter_.SetTouchUpLTPOFirstDynamicMode(DynamicModeType::TOUCH_EXT_ENABLED_LTPO_FIRST);
    mgr.frameVoter_.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_90_HZ);

    range = {0, 0};
    voterIter = std::find(mgr.frameVoter_.voters_.begin(), mgr.frameVoter_.voters_.end(), "VOTER_LTPO");
    mgr.touchManager_.ChangeState(TouchState::DOWN_STATE);
    mgr.frameVoter_.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_120_HZ);

    range = {0, 0};
    voterIter = std::find(mgr.frameVoter_.voters_.begin(), mgr.frameVoter_.voters_.end(), "VOTER_LTPO");
    mgr.touchManager_.ChangeState(TouchState::UP_STATE);
    mgr.frameVoter_.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_90_HZ);

    range = {0, 0};
    voterIter = std::find(mgr.frameVoter_.voters_.begin(), mgr.frameVoter_.voters_.end(), "VOTER_LTPO");
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_120Ms));
    mgr.frameVoter_.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_120_HZ);

    range = {0, 0};
    voterIter = std::find(mgr.frameVoter_.voters_.begin(), mgr.frameVoter_.voters_.end(), "VOTER_LTPO");
    mgr.touchManager_.ChangeState(TouchState::IDLE_STATE);
    mgr.frameVoter_.MergeLtpo2IdleVote(voterIter, info, range);
    EXPECT_EQ(range.second, OLED_120_HZ);
}

/**
 * @tc.name: TestProcessVoteIter
 * @tc.desc: Verify the result of ProcessVoteIter function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestProcessVoteIter, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    HgmFrameVoter hgmFrameVoter(HgmFrameVoter(mgr.multiAppStrategy_));
    bool voterGamesEffective = false;

    auto voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_PACKAGES");
    VoteRange range { OLED_NULL_HZ, OLED_MAX_HZ };
    VoteInfo info;
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, range, voterGamesEffective), false);

    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_LTPO");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, range, voterGamesEffective), false);

    VoteRange voteRange0 = { OLED_60_HZ, OLED_120_HZ };
    hgmFrameVoter.DeliverVote({ "VOTER_SCENE", OLED_60_HZ, OLED_120_HZ, 7 }, true);
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_LTPO");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, voteRange0, voterGamesEffective), false);
    
    voteRange0 = { OLED_90_HZ, OLED_90_HZ };
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_LTPO");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, voteRange0, voterGamesEffective), true);

    hgmFrameVoter.DeliverVote({ "VOTER_ANCO", OLED_60_HZ, OLED_120_HZ, 1 }, true);
    hgmFrameVoter.multiAppStrategy_.backgroundPid_.Put(1);
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_ANCO");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, range, voterGamesEffective), false);
    
    voteRange0 = { OLED_60_HZ, OLED_120_HZ };
    hgmFrameVoter.DeliverVote({ "VOTER_ANCO", OLED_60_HZ, OLED_120_HZ, 2 }, true);
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_ANCO");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, voteRange0, voterGamesEffective), false);

    voteRange0 = { OLED_90_HZ, OLED_90_HZ };
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_ANCO");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, voteRange0, voterGamesEffective), true);

    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_60_HZ, OLED_120_HZ, 3 }, true);
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_GAMES");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, voteRange0, voterGamesEffective), true);

    hgmFrameVoter.multiAppStrategy_.foregroundPidAppMap_[3] = { 1, "testGame" };
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "VOTER_GAMES");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, voteRange0, voterGamesEffective), true);
    EXPECT_EQ(voterGamesEffective, true);

    hgmFrameVoter.voters_.insert(hgmFrameVoter.voters_.begin(), "NULL");
    voterIter = std::find(hgmFrameVoter.voters_.begin(), hgmFrameVoter.voters_.end(), "NULL");
    EXPECT_EQ(hgmFrameVoter.ProcessVoteIter(voterIter, info, voteRange0, voterGamesEffective), false);
}

/**
 * @tc.name: TestProcessVote
 * @tc.desc: Verify the result of ProcessVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, TestProcessVote, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    HgmFrameVoter hgmFrameVoter(HgmFrameVoter(mgr.multiAppStrategy_));
    std::string screenStrategyId = "LTPO-test";
    ScreenId screenId = 1;
    int32_t mode = 1;
    hgmFrameVoter.voters_.insert(hgmFrameVoter.voters_.end(), "NULL");

    auto [voteInfo, voteRange] = hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);
    EXPECT_EQ(voteRange.first, OLED_MIN_HZ);

    hgmFrameVoter.DeliverVote({ "VOTER_ANCO", OLED_90_HZ, OLED_90_HZ, 1 }, true);
    auto [voteInfo2, voteRange2] = hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);
    EXPECT_EQ(voteRange2.first, OLED_90_HZ);
    hgmFrameVoter.CleanVote(1);

    hgmFrameVoter.DeliverVote({ "VOTER_LTPO", OLED_90_HZ, OLED_90_HZ, 2 }, true);
    auto [voteInfo3, voteRange3] = hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);
    EXPECT_EQ(voteRange3.first, OLED_90_HZ);
    hgmFrameVoter.CleanVote(2);

    hgmFrameVoter.DeliverVote({ "VOTER_ANCO", OLED_60_HZ, OLED_120_HZ, 1 }, true);
    hgmFrameVoter.DeliverVote({ "VOTER_PACKAGES", OLED_90_HZ, OLED_90_HZ, 3 }, true);
    auto [voteInfo4, voteRange4] = hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);
    EXPECT_EQ(voteRange4.first, OLED_90_HZ);
}

/**
 * @tc.name: Callback
 * @tc.desc: Verify the result of Callback function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameVoterTest, Callback, Function | SmallTest | Level0)
{
    HgmFrameRateManager mgr;
    HgmFrameVoter hgmFrameVoter(HgmFrameVoter(mgr.multiAppStrategy_));
    std::string screenStrategyId = "LTPO-test";
    ScreenId screenId = 1;
    int32_t mode = 1;

    hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);

    hgmFrameVoter.DeliverVote({ "VOTER_THERMAL", OLED_60_HZ, OLED_120_HZ, 0 }, true);
    hgmFrameVoter.DeliverVote({ "VOTER_GAMES", OLED_60_HZ, OLED_120_HZ, 0 }, true);
    hgmFrameVoter.DeliverVote({ "VOTER_TOUCH", OLED_90_HZ, OLED_120_HZ, 0 }, true);

    hgmFrameVoter.SetUpdateVoteRuleCallback([](std::vector<std::string>&) {});
    hgmFrameVoter.SetCheckVoteCallback([](const std::string&, VoteInfo&) -> bool { return true; });
    auto [voteInfo, voteRange] = hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);
    hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);
    hgmFrameVoter.SetCheckVoteCallback([](const std::string&, VoteInfo&) -> bool { return false; });
    auto [voteInfo1, voteRange1] = hgmFrameVoter.ProcessVote(screenStrategyId, screenId, mode);
    EXPECT_EQ(voteRange1.first, OLED_MIN_HZ);
}
} // namespace Rosen
} // namespace OHOS