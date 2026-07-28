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
#include <chrono>

#include "rs_frame_rate_vote.h"
#include "hgm_test_base.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace {
    inline constexpr pid_t ExtractPid(uint64_t id)
    {
        return static_cast<pid_t>(id >> 32);
    }

    inline constexpr uint64_t MakeNodeId(pid_t pid, uint32_t uid)
    {
        return (static_cast<uint64_t>(pid) << 32) | uid;
    }

    constexpr pid_t TEST_PID = 12345;
    constexpr uint64_t TEST_SURFACE_NODE_ID = MakeNodeId(TEST_PID, 1000);
    constexpr uint64_t TEST_SURFACE_NODE_ID_2 = MakeNodeId(TEST_PID, 1001);
    constexpr uint64_t TEST_SURFACE_NODE_ID_3 = MakeNodeId(TEST_PID, 1002);
    constexpr uint64_t TEST_SURFACE_NODE_ID_4 = MakeNodeId(TEST_PID, 2000);
}

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
    DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.clear();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    RSFrameRateVote::isVideoApp_.store(false);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer, 0);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer, 0);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer, 0);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer, 0);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, buffer, 0);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, buffer, 0);
    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer, 0);
    usleep(500000);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.clear();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);
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
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "[0,30]";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "[500,30]";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, nullBuffer, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->transactionFlags_ = "[0,30]";
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 0;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, nullBuffer, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->hasUiOrSurface, false);
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
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID_2, rsVideoFrameRateVote));
    DelayedSingleton<RSFrameRateVote>::GetInstance()->ReleaseSurfaceMap(TEST_SURFACE_NODE_ID);
    sleep(1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->ReleaseSurfaceMap(TEST_SURFACE_NODE_ID_2);
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
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(TEST_SURFACE_NODE_ID, 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_[TEST_SURFACE_NODE_ID], 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 30);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(TEST_SURFACE_NODE_ID_2, 60);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 2);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_[TEST_SURFACE_NODE_ID_2], 60);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 60);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(TEST_SURFACE_NODE_ID_3, 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 3);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_[TEST_SURFACE_NODE_ID_3], 30);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 60);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(TEST_SURFACE_NODE_ID, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 2);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 60);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(TEST_SURFACE_NODE_ID_2, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 30);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SurfaceVideoVote(TEST_SURFACE_NODE_ID_3, 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoRate_.size(), 0);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedRate_, 0);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: SurfaceVideoVote002
 * @tc.desc: test SurfaceVideoVote when maxRate == lastVotedRate_ && maxPid == lastVotedPid_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SurfaceVideoVote002, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->surfaceVideoRate_.clear();
    instance->lastVotedPid_ = 12345;
    instance->lastVotedRate_ = 60;

    instance->SurfaceVideoVote(TEST_SURFACE_NODE_ID, 60);

    ASSERT_EQ(instance->surfaceVideoRate_.size(), 1);
    ASSERT_EQ(instance->lastVotedPid_, 12345);
    ASSERT_EQ(instance->lastVotedRate_, 60);
}

/**
 * @tc.name: SurfaceVideoVote003
 * @tc.desc: test SurfaceVideoVote when ffrtQueue_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SurfaceVideoVote003, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    auto originalQueue = instance->ffrtQueue_;
    instance->ffrtQueue_ = nullptr;

    instance->surfaceVideoRate_.clear();
    instance->lastVotedPid_ = 0;
    instance->lastVotedRate_ = OLED_NULL_HZ;
    instance->taskHandler_ = nullptr;

    instance->SurfaceVideoVote(TEST_SURFACE_NODE_ID, 60);

    ASSERT_EQ(instance->surfaceVideoRate_.size(), 1);
    ASSERT_EQ(instance->lastVotedPid_, 12345);
    ASSERT_EQ(instance->lastVotedRate_, 60);
    ASSERT_EQ(instance->taskHandler_, nullptr);

    instance->ffrtQueue_ = originalQueue;
}

/**
 * @tc.name: SurfaceVideoVote004
 * @tc.desc: test SurfaceVideoVote when ffrtQueue_ is nullptr but taskHandler is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SurfaceVideoVote004, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    auto originalQueue = instance->ffrtQueue_;
    auto originalHandler = instance->taskHandler_;

    instance->surfaceVideoRate_.clear();
    instance->lastVotedPid_ = 0;
    instance->lastVotedRate_ = OLED_NULL_HZ;

    instance->SurfaceVideoVote(TEST_SURFACE_NODE_ID, 60);

    ASSERT_EQ(instance->surfaceVideoRate_.size(), 1);
    ASSERT_EQ(instance->lastVotedPid_, 12345);
    ASSERT_EQ(instance->lastVotedRate_, 60);

    instance->ffrtQueue_ = originalQueue;
    instance->taskHandler_ = originalHandler;
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
    EXPECT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
}

/**
 * @tc.name: CancelVoteRate003
 * @tc.desc: test when voterRateFunc_ is not nullptr
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

/**
 * @tc.name: SetVideoRateInfo001
 * @tc.desc: test when isVideoApp_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo001, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(false);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo002
 * @tc.desc: test when SetVideoRateInfo when pid is missing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo002, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo003
 * @tc.desc: test when SetVideoRateInfo when decRate is missing
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo003, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo004
 * @tc.desc: test when pid is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo004, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "invalid"}, {"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo005
 * @tc.desc: test when decRate is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo005, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "invalid"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo006
 * @tc.desc: test SetVideoRateInfo with valid input
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo006, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.size(), 1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_[12345], 60);
}

/**
 * @tc.name: SetVideoRateInfo007
 * @tc.desc: test SetVideoRateInfo with multiple entries
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo007, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo1 = {{"pid", "12345"}, {"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo1);

    std::unordered_map<std::string, std::string> videoRateInfo2 = {{"pid", "67890"}, {"decRate", "30"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo2);

    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.size(), 2);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_[12345], 60);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_[67890], 30);
}

/**
 * @tc.name: VideoFrameRateVote003
 * @tc.desc: test VideoFrameRateVote with availableBufferCount > 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote003, Function | SmallTest | Level0)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(1000,
        nullptr, nullptr);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID, rsVideoFrameRateVote));

    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 1);

    sleep(1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VideoFrameRateVote004
 * @tc.desc: test VideoFrameRateVote with availableBufferCount = 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote004, Function | SmallTest | Level0)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: VideoFrameRateVote005
 * @tc.desc: test VideoFrameRateVote uses videoRateInfo_ to get videoRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote005, Function | SmallTest | Level0)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    pid_t testPid = 12345;
    uint32_t testRate = 60;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_[testPid] = testRate;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = testPid;

    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: VideoFrameRateVote006
 * @tc.desc: test VideoFrameRateVote when lastVotedPid_ not in videoRateInfo_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote006, Function | SmallTest | Level0)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    DelayedSingleton<RSFrameRateVote>::GetInstance()->lastVotedPid_ = 12345;

    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: CheckSurfaceNodeIdChange001
 * @tc.desc: test CheckSurfaceNodeIdChange when surfaceNodeId changes within 50ms
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceNodeIdChange001, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->lastSurfaceNodeIdUpdateTime_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());

    usleep(10000);

    bool result = instance->CheckSurfaceNodeIdChange(TEST_SURFACE_NODE_ID_2);
    ASSERT_TRUE(result);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: CheckSurfaceNodeIdChange002
 * @tc.desc: test CheckSurfaceNodeIdChange when surfaceNodeId does not change
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceNodeIdChange002, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->lastSurfaceNodeIdUpdateTime_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());

    usleep(10000);

    bool result = instance->CheckSurfaceNodeIdChange(TEST_SURFACE_NODE_ID);
    ASSERT_FALSE(result);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: CheckSurfaceNodeIdChange003
 * @tc.desc: test CheckSurfaceNodeIdChange when time exceeds 50ms
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceNodeIdChange003, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->lastSurfaceNodeIdUpdateTime_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());

    usleep(60000);

    bool result = instance->CheckSurfaceNodeIdChange(TEST_SURFACE_NODE_ID_2);
    ASSERT_FALSE(result);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: CheckSurfaceNodeIdChange004
 * @tc.desc: test CheckSurfaceNodeIdChange when lastSurfaceNodeId_ is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceNodeIdChange004, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());

    usleep(10000);

    bool result = instance->CheckSurfaceNodeIdChange(TEST_SURFACE_NODE_ID);
    ASSERT_TRUE(result);

    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: CheckAvailableBufferCount001
 * @tc.desc: test CheckAvailableBufferCount when bufferCount > 1 for 4 times in 7 consecutive checks
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckAvailableBufferCount001, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);

    bool result1 = instance->CheckAvailableBufferCount(2);
    ASSERT_FALSE(result1);

    bool result2 = instance->CheckAvailableBufferCount(2);
    ASSERT_FALSE(result2);

    bool result3 = instance->CheckAvailableBufferCount(2);
    ASSERT_FALSE(result3);

    bool result4 = instance->CheckAvailableBufferCount(2);
    ASSERT_TRUE(result4);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
}

/**
 * @tc.name: CheckAvailableBufferCount002
 * @tc.desc: test CheckAvailableBufferCount when bufferCount is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckAvailableBufferCount002, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);

    bool result = instance->CheckAvailableBufferCount(0);
    ASSERT_FALSE(result);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
}

/**
 * @tc.name: CheckAvailableBufferCount003
 * @tc.desc: test CheckAvailableBufferCount when bufferCount > 1 for less than 4 times in 7 checks
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckAvailableBufferCount003, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);

    bool result1 = instance->CheckAvailableBufferCount(2);
    ASSERT_FALSE(result1);

    bool result2 = instance->CheckAvailableBufferCount(2);
    ASSERT_FALSE(result2);

    bool result3 = instance->CheckAvailableBufferCount(2);
    ASSERT_FALSE(result3);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
}

/**
 * @tc.name: CheckAvailableBufferCount004
 * @tc.desc: test CheckAvailableBufferCount when bufferCount > 1 for more than 4 times in 7 checks
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckAvailableBufferCount004, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);

    instance->CheckAvailableBufferCount(2);
    instance->CheckAvailableBufferCount(2);
    instance->CheckAvailableBufferCount(2);
    instance->CheckAvailableBufferCount(2);

    bool result4 = instance->CheckAvailableBufferCount(2);
    ASSERT_TRUE(result4);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
}

/**
 * @tc.name: CheckAvailableBufferCount005
 * @tc.desc: test CheckAvailableBufferCount when bufferCount > 1 then 0 then >1
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckAvailableBufferCount005, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);

    instance->CheckAvailableBufferCount(2);
    instance->CheckAvailableBufferCount(2);
    instance->CheckAvailableBufferCount(0);

    bool result4 = instance->CheckAvailableBufferCount(2);
    ASSERT_FALSE(result4);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
}

/**
 * @tc.name: CheckAvailableBufferCount006
 * @tc.desc: test CheckAvailableBufferCount when bufferCount is negative
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckAvailableBufferCount006, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);

    bool result = instance->CheckAvailableBufferCount(-1);
    ASSERT_FALSE(result);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
}

/**
 * @tc.name: CheckSurfaceNodeIdChange005
 * @tc.desc: test CheckSurfaceNodeIdChange when duration is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceNodeIdChange005, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);

    bool result = instance->CheckSurfaceNodeIdChange(TEST_SURFACE_NODE_ID_2);
    ASSERT_FALSE(result);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: CheckSurfaceNodeIdChange006
 * @tc.desc: test CheckSurfaceNodeIdChange when duration is negative (time goes backwards)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceNodeIdChange006, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->lastSurfaceNodeIdUpdateTime_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count() + 1000);

    usleep(10000);

    bool result = instance->CheckSurfaceNodeIdChange(TEST_SURFACE_NODE_ID_2);
    ASSERT_TRUE(result);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: CheckSurfaceNodeIdChange007
 * @tc.desc: test CheckSurfaceNodeIdChange when surfaceNodeId == lastId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceNodeIdChange007, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);

    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->lastSurfaceNodeIdUpdateTime_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());

    usleep(10000);

    bool result = instance->CheckSurfaceNodeIdChange(TEST_SURFACE_NODE_ID);
    ASSERT_FALSE(result);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: SetVideoRateInfo008
 * @tc.desc: test SetVideoRateInfo when decRate is 0 (should delete entry)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo008, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    pid_t testPid = 12345;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_[testPid] = 60;
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.size(), 1);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo009
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and pid not in map
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo009, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo010
 * @tc.desc: test SetVideoRateInfo when decRate is not 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo010, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.size(), 1);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_[12345], 60);
}

/**
 * @tc.name: SetVideoRateInfo011
 * @tc.desc: test SetVideoRateInfo when decRate is invalid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo011, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "invalid"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo012
 * @tc.desc: test SetVideoRateInfo when pid is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo012, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "0"}, {"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: ReleaseSurfaceMap002
 * @tc.desc: test ReleaseSurfaceMap when ffrtQueue_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, ReleaseSurfaceMap002, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    auto originalQueue = instance->ffrtQueue_;
    instance->ffrtQueue_ = nullptr;

    instance->surfaceVideoFrameRateVote_.clear();

    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    instance->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID_2, rsVideoFrameRateVote));

    instance->ReleaseSurfaceMap(TEST_SURFACE_NODE_ID_2);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);

    instance->ffrtQueue_ = originalQueue;
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: ReleaseSurfaceMap003
 * @tc.desc: test ReleaseSurfaceMap when surfaceNodeId not found
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, ReleaseSurfaceMap003, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();

    instance->surfaceVideoFrameRateVote_.clear();

    instance->ReleaseSurfaceMap(TEST_SURFACE_NODE_ID_2);
    sleep(1);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);
}

/**
 * @tc.name: SetVideoRateInfo013
 * @tc.desc: test SetVideoRateInfo when pid is negative
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo013, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "-1"}, {"decRate", "60"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo014
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and pid is in map
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo014, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.clear();

    pid_t testPid = 12345;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_[testPid] = 60;
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.size(), 1);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->videoRateInfo_.empty());
}

/**
 * @tc.name: VideoFrameRateVote007
 * @tc.desc: test VideoFrameRateVote when CheckSurfaceNodeIdChange returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote007, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->isVoted_ = true;
    instance->videoRateInfo_.clear();
    instance->surfaceVideoRate_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->lastSurfaceNodeIdUpdateTime_.store(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());

    usleep(10000);

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID_4, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    sleep(1);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);

    instance->lastSurfaceNodeIdForCheck_.store(0);
    instance->lastSurfaceNodeIdUpdateTime_.store(0);
}

/**
 * @tc.name: VideoFrameRateVote008
 * @tc.desc: test VideoFrameRateVote when CheckAvailableBufferCount returns true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote008, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->isVoted_ = true;
    instance->videoRateInfo_.clear();
    instance->surfaceVideoRate_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(2);
    instance->bufferCountHistory_[0] = 2;
    instance->bufferCountHistory_[1] = 2;
    instance->bufferCountHistory_[2] = 2;
    instance->bufferCountHistory_[3] = 2;

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 2);

    sleep(1);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);

    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
}

/**
 * @tc.name: VideoFrameRateVote009
 * @tc.desc: test VideoFrameRateVote when surfaceVideoFrameRateVote_ already has entry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote009, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->videoRateInfo_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
    instance->surfaceVideoFrameRateVote_.clear();

    pid_t testPid = 12345;
    uint32_t testRate = 60;
    instance->videoRateInfo_[testPid] = testRate;
    instance->lastVotedPid_ = testPid;

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: VideoFrameRateVote010
 * @tc.desc: test VideoFrameRateVote when videoRate is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote010, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->videoRateInfo_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
    instance->surfaceVideoFrameRateVote_.clear();

    pid_t testPid = 12345;
    instance->videoRateInfo_[testPid] = 0;
    instance->lastVotedPid_ = testPid;

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: VideoFrameRateVote011
 * @tc.desc: test VideoFrameRateVote when transactionFlags doesn't match
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote011, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->videoRateInfo_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
    instance->surfaceVideoFrameRateVote_.clear();
    instance->transactionFlags_ = "[999, 30]";
    instance->lastVotedPid_ = 12345;

    pid_t testPid = 12345;
    uint32_t testRate = 60;
    instance->videoRateInfo_[testPid] = testRate;

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: VideoFrameRateVote012
 * @tc.desc: test VideoFrameRateVote when pid not in videoRateInfo_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote012, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->videoRateInfo_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
    instance->surfaceVideoFrameRateVote_.clear();
    instance->transactionFlags_ = "";
    instance->lastVotedPid_ = 0;

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: VideoFrameRateVote013
 * @tc.desc: test VideoFrameRateVote when ffrtQueue_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote013, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    auto originalQueue = instance->ffrtQueue_;
    instance->ffrtQueue_ = nullptr;

    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->videoRateInfo_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
    instance->surfaceVideoFrameRateVote_.clear();
    instance->transactionFlags_ = "";
    instance->lastVotedPid_ = 0;

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);

    instance->ffrtQueue_ = originalQueue;
}

/**
 * @tc.name: CheckSurfaceAndUi001
 * @tc.desc: test CheckSurfaceAndUi when isVideoApp_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi001, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(false);
    instance->hasUiOrSurface = true;

    instance->CheckSurfaceAndUi(TEST_SURFACE_NODE_ID);

    ASSERT_TRUE(instance->hasUiOrSurface);
}

/**
 * @tc.name: CheckSurfaceAndUi002
 * @tc.desc: test CheckSurfaceAndUi when hasUiOrSurface is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi002, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = false;

    instance->CheckSurfaceAndUi(TEST_SURFACE_NODE_ID);

    ASSERT_FALSE(instance->hasUiOrSurface);
}

/**
 * @tc.name: CheckSurfaceAndUi003
 * @tc.desc: test CheckSurfaceAndUi when duration >= DANMU_MAX_INTERVAL_TIME
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi003, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = true;
    instance->currentUpdateTime_ = 0;

    uint64_t timestamp = 60 * NS_PER_MS;
    instance->CheckSurfaceAndUi(timestamp);

    ASSERT_FALSE(instance->hasUiOrSurface);
}

/**
 * @tc.name: CheckSurfaceAndUi004
 * @tc.desc: test CheckSurfaceAndUi when lastVotedRate_ is OLED_NULL_HZ
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi004, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = true;
    instance->currentUpdateTime_ = 0;
    instance->lastVotedRate_ = OLED_NULL_HZ;

    uint64_t timestamp = 30 * NS_PER_MS;
    instance->CheckSurfaceAndUi(timestamp);

    ASSERT_FALSE(instance->hasUiOrSurface);
}

/**
 * @tc.name: CheckSurfaceAndUi005
 * @tc.desc: test CheckSurfaceAndUi when surfaceVideoFrameRateVote_ is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi005, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = true;
    instance->currentUpdateTime_ = 0;
    instance->lastVotedRate_ = 60;
    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->surfaceVideoFrameRateVote_.clear();

    uint64_t timestamp = 30 * NS_PER_MS;
    instance->CheckSurfaceAndUi(timestamp);

    ASSERT_FALSE(instance->hasUiOrSurface);
}

/**
 * @tc.name: CheckSurfaceAndUi006
 * @tc.desc: test CheckSurfaceAndUi when surfaceVideoFrameRateVote_ has entry
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi006, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = true;
    instance->currentUpdateTime_ = 0;
    instance->lastVotedRate_ = 60;
    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->surfaceVideoFrameRateVote_.clear();

    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(1000,
        nullptr, nullptr);
    instance->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID, rsVideoFrameRateVote));

    uint64_t timestamp = 30 * NS_PER_MS;
    instance->CheckSurfaceAndUi(timestamp);

    ASSERT_FALSE(instance->hasUiOrSurface);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 0);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: CheckSurfaceAndUi007
 * @tc.desc: test CheckSurfaceAndUi when duration is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi007, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = true;
    instance->currentUpdateTime_ = 0;
    instance->lastVotedRate_ = 60;
    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->surfaceVideoFrameRateVote_.clear();

    uint64_t timestamp = 0;
    instance->CheckSurfaceAndUi(timestamp);

    ASSERT_FALSE(instance->hasUiOrSurface);
}

/**
 * @tc.name: CheckSurfaceAndUi008
 * @tc.desc: test CheckSurfaceAndUi when currentUpdateTime_ <= lastUpdateTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi008, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = true;
    instance->currentUpdateTime_ = 100;
    instance->lastVotedRate_ = 60;
    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->surfaceVideoFrameRateVote_.clear();

    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(1000,
        nullptr, nullptr);
    instance->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID, rsVideoFrameRateVote));

    uint64_t timestamp = 30 * NS_PER_MS;
    instance->CheckSurfaceAndUi(timestamp);

    ASSERT_FALSE(instance->hasUiOrSurface);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 0);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: CheckSurfaceAndUi009
 * @tc.desc: test CheckSurfaceAndUi when votingAddress->second is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CheckSurfaceAndUi009, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    RSFrameRateVote::isVideoApp_.store(true);
    instance->hasUiOrSurface = true;
    instance->currentUpdateTime_ = 0;
    instance->lastVotedRate_ = 60;
    instance->lastSurfaceNodeIdForCheck_.store(TEST_SURFACE_NODE_ID);
    instance->surfaceVideoFrameRateVote_.clear();

    instance->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID, nullptr));

    uint64_t timestamp = 30 * NS_PER_MS;
    instance->CheckSurfaceAndUi(timestamp);

    ASSERT_FALSE(instance->hasUiOrSurface);
}

/**
 * @tc.name: VideoFrameRateVote014
 * @tc.desc: test VideoFrameRateVote when ExtractPid(surfaceNodeId) returns a pid in videoRateInfo_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote014, Function | SmallTest | Level0)
{
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->isSwitchOn_ = true;
    RSFrameRateVote::isVideoApp_.store(true);
    instance->videoRateInfo_.clear();
    instance->bufferCountIndex_ = 0;
    instance->bufferCountHistory_.fill(0);
    instance->surfaceVideoFrameRateVote_.clear();
    instance->transactionFlags_ = "";
    instance->lastVotedPid_ = 0;

    pid_t testPid = 12345;
    uint32_t testRate = 60;
    instance->videoRateInfo_[testPid] = testRate;
    instance->lastVotedPid_ = testPid;

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();

    instance->VideoFrameRateVote(TEST_SURFACE_NODE_ID, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, buffer, 0);

    usleep(500000);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: SetVideoRateInfo015
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and cancelVoteRate should be called
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo015, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();
    instance->isVoted_ = true;
    instance->lastVotedPid_ = 12345;
    instance->lastSurfaceNodeId_ = TEST_SURFACE_NODE_ID;

    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(1000,
        nullptr, nullptr);
    instance->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID, rsVideoFrameRateVote));

    ASSERT_TRUE(instance->isVoted_);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_FALSE(instance->isVoted_);
    ASSERT_TRUE(instance->videoRateInfo_.empty());
    sleep(1);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: SetVideoRateInfo016
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and pid not in videoRateInfo_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo016, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();
    instance->isVoted_ = true;
    instance->lastVotedPid_ = 12345;
    instance->lastSurfaceNodeId_ = TEST_SURFACE_NODE_ID;

    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(1000,
        nullptr, nullptr);
    instance->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID, rsVideoFrameRateVote));

    ASSERT_TRUE(instance->isVoted_);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "67890"}, {"decRate", "0"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_FALSE(instance->isVoted_);
    ASSERT_TRUE(instance->videoRateInfo_.empty());
    sleep(1);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: SetVideoRateInfo017
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and lastSurfaceNodeId_ is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo017, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();
    instance->isVoted_ = true;
    instance->lastVotedPid_ = 12345;
    instance->lastSurfaceNodeId_ = 0;

    ASSERT_TRUE(instance->isVoted_);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_FALSE(instance->isVoted_);
    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo018
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and surfaceVideoFrameRateVote_ is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo018, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();
    instance->isVoted_ = true;
    instance->lastVotedPid_ = 12345;
    instance->lastSurfaceNodeId_ = TEST_SURFACE_NODE_ID;
    instance->surfaceVideoFrameRateVote_.clear();

    ASSERT_TRUE(instance->isVoted_);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_FALSE(instance->isVoted_);
    ASSERT_TRUE(instance->videoRateInfo_.empty());
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);
}

/**
 * @tc.name: SetVideoRateInfo019
 * @tc.desc: test SetVideoRateInfo when pid has trailing characters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo019, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345abc"}, {"decRate", "60"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo020
 * @tc.desc: test SetVideoRateInfo when decRate has trailing characters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo020, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "60abc"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo021
 * @tc.desc: test SetVideoRateInfo when pid has leading spaces
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo021, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", " 12345"}, {"decRate", "60"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo022
 * @tc.desc: test SetVideoRateInfo when decRate has leading spaces
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo022, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", " 60"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo023
 * @tc.desc: test SetVideoRateInfo when both pid and decRate have trailing characters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo023, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345abc"}, {"decRate", "60xyz"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo024
 * @tc.desc: test SetVideoRateInfo when pid is valid but decRate has trailing characters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo024, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "60xyz"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo025
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and CancelVoteRate is called
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo025, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();
    instance->isVoted_ = true;
    instance->lastVotedPid_ = 12345;
    instance->lastSurfaceNodeId_ = TEST_SURFACE_NODE_ID;

    ASSERT_TRUE(instance->isVoted_);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    instance->SetVideoRateInfo(videoRateInfo);

    ASSERT_FALSE(instance->isVoted_);
    ASSERT_TRUE(instance->videoRateInfo_.empty());
}

/**
 * @tc.name: SetVideoRateInfo026
 * @tc.desc: test SetVideoRateInfo when decRate is 0 and ReleaseSurfaceMap is called
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, SetVideoRateInfo026, Function | SmallTest | Level0)
{
    RSFrameRateVote::isVideoApp_.store(true);
    auto instance = DelayedSingleton<RSFrameRateVote>::GetInstance();
    instance->videoRateInfo_.clear();
    instance->isVoted_ = true;
    instance->lastVotedPid_ = 12345;
    instance->lastSurfaceNodeId_ = TEST_SURFACE_NODE_ID;

    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(1000,
        nullptr, nullptr);
    instance->surfaceVideoFrameRateVote_.insert(
        std::pair<uint64_t, std::shared_ptr<RSVideoFrameRateVote>>(TEST_SURFACE_NODE_ID, rsVideoFrameRateVote));

    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 1);

    std::unordered_map<std::string, std::string> videoRateInfo = {{"pid", "12345"}, {"decRate", "0"}};
    instance->SetVideoRateInfo(videoRateInfo);

    sleep(1);
    ASSERT_EQ(instance->surfaceVideoFrameRateVote_.size(), 0);
    rsVideoFrameRateVote = nullptr;
}
} // namespace Rosen
} // namespace OHOS