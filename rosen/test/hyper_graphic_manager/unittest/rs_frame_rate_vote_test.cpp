/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <test_header.h>

#include "rs_frame_rate_vote.h"
#include "hgm_test_base.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSFrameRateVoteTest : public HgmTestBase {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RSFrameRateVoteTest::SetUpTestCase()
{
    HgmTestBase::SetUpTestCase();
}
void RSFrameRateVoteTest::TearDownTestCase() {}
void RSFrameRateVoteTest::SetUp() {}
void RSFrameRateVoteTest::TearDown() {}

/**
 * @tc.name: SetTransactionFlags001
 * @tc.desc: Verify the result of SetTransactionFlags function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetTransactionFlags001, Function | SmallTest | Level0)
{
    std::string transactionFlags = "xxx";
    RSFrameRateVote::isVideoApp_.store(false);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetTransactionFlags(transactionFlags);
    ASSERT_NE(DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_, transactionFlags);
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetTransactionFlags(transactionFlags);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_, transactionFlags);
}

HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote001, Function | SmallTest | Level0)
{
    sptr<SurfaceBuffer> nullBuffer = nullptr;
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    RSFrameRateVote::isVideoApp_.store(false);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, buffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, buffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: VideoFrameRateVote002
 * @tc.desc: Verify the result of VideoFrameRateVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote002, Function | SmallTest | Level0) {
    sptr<SurfaceBuffer> nullBuffer = nullptr;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface = false;

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "[500,30]";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, true);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "[0,30]";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, true);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "[500,30]";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, true);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "[0,30]";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, true);
}

/**
 * @tc.name: ReleaseSurfaceMap001
 * @tc.desc: Verify the result of ReleaseSurfaceMap function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, ReleaseSurfaceMap001, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(1001, rsVideoFrameRateVote));
    DelayedSingleton<RSFrameRateVote>::GetInstance()->ReleaseSurfaceMap(1000);
    sleep(1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->ReleaseSurfaceMap(1001);
    sleep(1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: SurfaceVideoVote001
 * @tc.desc: Verify the result of SurfaceVideoVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SurfaceVideoVote001, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(1000, 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_[1000], 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 30);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(1001, 60);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 2);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_[1001], 60);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 60);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(1002, 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 3);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_[1002], 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 60);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(1000, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 2);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 60);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(1001, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 30);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(1002, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 0);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteRate001
 * @tc.desc: Verify the result of VoteRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VoteRate001, Function | SmallTest | Level0)
{
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_ = false;
    ASSERT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VoteRate(DEFAULT_PID, "VOTER_VIDEO", 30);
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
}

/**
 * @tc.name: VoteRate002
 * @tc.desc: test when voterRateFunc_ is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VoteRate002, Function | SmallTest | Level0)
{
    VideoVoterFunc oriVoterRateFunc = DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_;

    DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_ = false;
    ASSERT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
    VideoVoterFunc voterRateFunc = [](const std::string& key, const std::string& value) {};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_ = voterRateFunc;
    ASSERT_NE(DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_, nullptr);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->VoteRate(DEFAULT_PID, "VOTER_VIDEO", 30);
    EXPECT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_ = oriVoterRateFunc;
}

/**
 * @tc.name: CancelVoteRate001
 * @tc.desc: Verify the result of CancelVoteRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CancelVoteRate001, Function | SmallTest | Level0)
{
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_ = true;
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->CancelVoteRate(DEFAULT_PID, "VOTER_VIDEO");
    ASSERT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
}

/**
 * @tc.name: CancelVoteRate002
 * @tc.desc: test when isVoted_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CancelVoteRate002, Function | SmallTest | Level0)
{
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_ = false;
    ASSERT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->CancelVoteRate(DEFAULT_PID, "VOTER_VIDEO");
    ASSERT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
}

/**
 * @tc.name: CancelVoteRate003
 * @tc.desc: test when isVoted_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CancelVoteRate003, Function | SmallTest | Level0)
{
    VideoVoterFunc oriVoterRateFunc = DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_;

    DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_ = true;
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
    VideoVoterFunc voterRateFunc = [](const std::string& key, const std::string& value) {};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_ = voterRateFunc;
    ASSERT_NE(DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_, nullptr);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->CancelVoteRate(DEFAULT_PID, "VOTER_VIDEO");
    EXPECT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->voterRateFunc_ = oriVoterRateFunc;
}
} // namespace Rosen
} // namespace OHOS