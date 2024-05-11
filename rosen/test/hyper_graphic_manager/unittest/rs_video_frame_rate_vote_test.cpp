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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
}
class RSVideoFrameRateVoteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    void VoteCallback(uint64_t avg1, uint32_t avg2);
    void ReleaseCallback(int64_t avg);
public:
    int64_t voteCallbackResult_ {0};
    int64_t releaseCallbackResult_ {0};
};

void RSVideoFrameRateVoteTest::SetUpTestCase() {}
void RSVideoFrameRateVoteTest::TearDownTestCase() {}
void RSVideoFrameRateVoteTest::SetUp() {}
void RSVideoFrameRateVoteTest::TearDown() {}

void RSVideoFrameRateVoteTest::VoteCallback(uint64_t avg1, uint32_t avg2)
{
    voteCallbackResult_ = static_cast<int64_t>(avg1) + static_cast<int64_t>(avg2);
}

void RSVideoFrameRateVoteTest::ReleaseCallback(int64_t avg)
{
    releaseCallbackResult_ = avg;
}

/**
 * @tc.name: RSVideoFrameRateVote001
 * @tc.desc: Verify the result of RSVideoFrameRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, RSVideoFrameRateVote001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    ASSERT_EQ(rsVideoFrameRateVote->surfaceNodeId_, 0);
    ASSERT_EQ(rsVideoFrameRateVote->voteCallback_, nullptr);
    ASSERT_EQ(rsVideoFrameRateVote->releaseCallback_, nullptr);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: StartVideoFrameRateVote001
 * @tc.desc: Verify the result of StartVideoFrameRateVote function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, StartVideoFrameRateVote001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->StartVideoFrameRateVote(1);
    for (unsigned int i = 0; i < 28; i++) {
        rsVideoFrameRateVote->StartVideoFrameRateVote(0);
    }
    rsVideoFrameRateVote->StartVideoFrameRateVote(966667);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    ASSERT_EQ(rsVideoFrameRateVote->lastTimestamp_, 966667);
    rsVideoFrameRateVote->timestampType_ = RSVideoFrameRateVote::TimestampType::MILLI_SECOND_TYPE;
    rsVideoFrameRateVote->StartVideoFrameRateVote(1);
    for (unsigned int i = 0; i < 28; i++) {
        rsVideoFrameRateVote->StartVideoFrameRateVote(0);
    }
    rsVideoFrameRateVote->StartVideoFrameRateVote(967);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    ASSERT_EQ(rsVideoFrameRateVote->lastTimestamp_, 967);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: CalculateVideoFrameRate001
 * @tc.desc: Verify the result of CalculateVideoFrameRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, CalculateVideoFrameRate001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    std::vector<int64_t> timestampVector(30);
    rsVideoFrameRateVote->videoTimestamp_ = timestampVector;
    ASSERT_EQ(rsVideoFrameRateVote->CalculateVideoFrameRate(), 0);
    timestampVector[0] = 1;
    timestampVector[29] = 3;
    rsVideoFrameRateVote->videoTimestamp_ = timestampVector;
    ASSERT_EQ(rsVideoFrameRateVote->CalculateVideoFrameRate(), 14500);
    timestampVector[0] = 1;
    timestampVector[29] = 2;
    rsVideoFrameRateVote->videoTimestamp_ = timestampVector;
    rsVideoFrameRateVote->timestampType_ = RSVideoFrameRateVote::TimestampType::MILLI_SECOND_TYPE;
    ASSERT_EQ(rsVideoFrameRateVote->CalculateVideoFrameRate(), 29000);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: Calculate001
 * @tc.desc: Verify the result of Calculate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, Calculate001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    std::vector<int64_t> timestampVector(30);
    rsVideoFrameRateVote->videoTimestamp_ = timestampVector;
    ASSERT_EQ(rsVideoFrameRateVote->Calculate(1000), 0);
    timestampVector[0] = 1;
    timestampVector[29] = 3;
    rsVideoFrameRateVote->videoTimestamp_ = timestampVector;
    ASSERT_EQ(rsVideoFrameRateVote->Calculate(1000), 14500);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteVideoFrameRate001
 * @tc.desc: Verify the result of VoteVideoFrameRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, VoteVideoFrameRate001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->VoteVideoFrameRate(0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 0);
    rsVideoFrameRateVote->VoteVideoFrameRate(150);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 0);
    rsVideoFrameRateVote->VoteVideoFrameRate(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);
    rsVideoFrameRateVote->VoteVideoFrameRate(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);
    rsVideoFrameRateVote->VoteVideoFrameRate(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);
    rsVideoFrameRateVote->VoteVideoFrameRate(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: SendDelayTask001
 * @tc.desc: Verify the result of SendDelayTask function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, SendDelayTask001, Function | SmallTest | Level1)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->releaseCallback_ = std::bind(&RSVideoFrameRateVoteTest::ReleaseCallback, this,
        std::placeholders::_1);
    rsVideoFrameRateVote->surfaceNodeId_ = 2;
    rsVideoFrameRateVote->SendDelayTask();
    ASSERT_NE(rsVideoFrameRateVote->taskHandler_, nullptr);
    sleep(2);
    ASSERT_EQ(releaseCallbackResult_, 2);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: CancelDelayTask001
 * @tc.desc: Verify the result of CancelDelayTask function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, CancelDelayTask001, Function | SmallTest | Level1)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->releaseCallback_ = std::bind(&RSVideoFrameRateVoteTest::ReleaseCallback, this,
        std::placeholders::_1);
    rsVideoFrameRateVote->surfaceNodeId_ = 2;
    rsVideoFrameRateVote->SendDelayTask();
    ASSERT_NE(rsVideoFrameRateVote->taskHandler_, nullptr);
    rsVideoFrameRateVote->CancelDelayTask();
    ASSERT_EQ(rsVideoFrameRateVote->taskHandler_, nullptr);
    sleep(2);
    ASSERT_EQ(releaseCallbackResult_, 0);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: DoVoteCallback001
 * @tc.desc: Verify the result of DoVoteCallback function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, DoVoteCallback001, Function | SmallTest | Level1)
{
    voteCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    ASSERT_EQ(rsVideoFrameRateVote->voteCallback_, nullptr);
    rsVideoFrameRateVote->DoVoteCallback(0);
    rsVideoFrameRateVote->voteCallback_ = std::bind(&RSVideoFrameRateVoteTest::VoteCallback, this,
        std::placeholders::_1, std::placeholders::_2);
    rsVideoFrameRateVote->surfaceNodeId_ = 2;
    rsVideoFrameRateVote->DoVoteCallback(2);
    ASSERT_EQ(voteCallbackResult_, 4);
    rsVideoFrameRateVote->DoVoteCallback(3);
    ASSERT_EQ(voteCallbackResult_, 5);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: DoReleaseCallback001
 * @tc.desc: Verify the result of DoReleaseCallback function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, DoReleaseCallback001, Function | SmallTest | Level1)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    ASSERT_EQ(rsVideoFrameRateVote->releaseCallback_, nullptr);
    rsVideoFrameRateVote->DoReleaseCallback();
    rsVideoFrameRateVote->releaseCallback_ = std::bind(&RSVideoFrameRateVoteTest::ReleaseCallback, this,
        std::placeholders::_1);
    rsVideoFrameRateVote->surfaceNodeId_ = 2;
    rsVideoFrameRateVote->DoReleaseCallback();
    ASSERT_EQ(releaseCallbackResult_, 2);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: GetCurrentTimeMillis001
 * @tc.desc: Verify the result of GetCurrentTimeMillis function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, GetCurrentTimeMillis001, Function | SmallTest | Level1)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    int64_t time1 = rsVideoFrameRateVote->GetCurrentTimeMillis();
    usleep(1000);
    int64_t time2 = rsVideoFrameRateVote->GetCurrentTimeMillis();
    ASSERT_NE(time1, time2);
    rsVideoFrameRateVote = nullptr;
}
} // namespace Rosen
} // namespace OHOS