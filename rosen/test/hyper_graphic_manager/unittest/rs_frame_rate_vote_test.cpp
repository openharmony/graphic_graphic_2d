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

#define private public
#include "rs_frame_rate_vote.h"
#undef private
#include "hgm_core.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
}
class RSFrameRateVoteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RSFrameRateVoteTest::SetUpTestCase() {}
void RSFrameRateVoteTest::TearDownTestCase() {}
void RSFrameRateVoteTest::SetUp() {}
void RSFrameRateVoteTest::TearDown() {}

/**
 * @tc.name: VideoFrameRateVote001
 * @tc.desc: Verify the result of VideoFrameRateVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, VideoFrameRateVote001, Function | SmallTest | Level1)
{
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, 0);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, 0);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, 0);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, 100000);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = false;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, 100000);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, 100000);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, 0);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 0);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isSwitchOn_ = true;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VideoFrameRateVote(
        1000, OHSurfaceSource::OH_SURFACE_SOURCE_VIDEO, 100000);
    usleep(1000);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->surfaceVideoFrameRateVote_.size(), 1);
}

/**
 * @tc.name: ReleaseSurfaceMap001
 * @tc.desc: Verify the result of ReleaseSurfaceMap function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, ReleaseSurfaceMap001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_ = nullptr;
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
HWTEST_F(RSFrameRateVoteTest, SurfaceVideoVote001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_ = nullptr;
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
HWTEST_F(RSFrameRateVoteTest, VoteRate001, Function | SmallTest | Level1)
{
    DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_ = nullptr;
    ASSERT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->VoteRate("VOTER_VIDEO", 30);
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
}

/**
 * @tc.name: CancelVoteRate001
 * @tc.desc: Verify the result of CancelVoteRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, CancelVoteRate001, Function | SmallTest | Level1)
{
    DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_ = nullptr;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_ = true;
    ASSERT_TRUE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->CancelVoteRate("VOTER_VIDEO");
    ASSERT_FALSE(DelayedSingleton<RSFrameRateVote>::GetInstance()->isVoted_);
}

/**
 * @tc.name: NotifyRefreshRateEvent001
 * @tc.desc: Verify the result of NotifyRefreshRateEvent function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSFrameRateVoteTest, NotifyRefreshRateEvent001, Function | SmallTest | Level1)
{
    DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_ =
        OHOS::Rosen::HgmCore::Instance().GetFrameRateMgr();
    ASSERT_NE(DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_, nullptr);
    EventInfo eventInfo1 = {
        .eventName = "VOTER_VIDEO",
        .eventStatus = true,
        .minRefreshRate = 60,
        .maxRefreshRate = 60,
    };
    DelayedSingleton<RSFrameRateVote>::GetInstance()->NotifyRefreshRateEvent(eventInfo1);
    EventInfo eventInfo2 = {
        .eventName = "VOTER_VIDEO",
        .eventStatus = false,
    };
    DelayedSingleton<RSFrameRateVote>::GetInstance()->NotifyRefreshRateEvent(eventInfo2);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_ = nullptr;
    DelayedSingleton<RSFrameRateVote>::GetInstance()->NotifyRefreshRateEvent(eventInfo2);
    ASSERT_EQ(DelayedSingleton<RSFrameRateVote>::GetInstance()->frameRateMgr_, nullptr);
}
} // namespace Rosen
} // namespace OHOS