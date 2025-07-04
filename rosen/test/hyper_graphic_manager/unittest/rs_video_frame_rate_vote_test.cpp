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

#include "hgm_test_base.h"
#include "rs_frame_rate_vote.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSVideoFrameRateVoteTest : public HgmTestBase {
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

void RSVideoFrameRateVoteTest::SetUpTestCase()
{
    HgmTestBase::SetUpTestCase();
}
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
HWTEST_F(RSVideoFrameRateVoteTest, RSVideoFrameRateVote001, Function | SmallTest | Level0)
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
HWTEST_F(RSVideoFrameRateVoteTest, StartVideoFrameRateVote001, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->StartVideoFrameRateVote(30.0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteVideoFrameRate001
 * @tc.desc: Verify the result of VoteVideoFrameRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, VoteVideoFrameRate001, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->VoteVideoFrameRate(30.0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(0.0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(300.0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(30.0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(60.0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: SendDelayTask001
 * @tc.desc: Verify the result of SendDelayTask function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, SendDelayTask001, Function | SmallTest | Level0)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->releaseCallback_ = [this](uint64_t id) { this->ReleaseCallback(id); };
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
HWTEST_F(RSVideoFrameRateVoteTest, CancelDelayTask001, Function | SmallTest | Level0)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->releaseCallback_ = [this](uint64_t id) { this->ReleaseCallback(id); };
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
HWTEST_F(RSVideoFrameRateVoteTest, DoVoteCallback001, Function | SmallTest | Level0)
{
    voteCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    ASSERT_EQ(rsVideoFrameRateVote->voteCallback_, nullptr);
    rsVideoFrameRateVote->DoVoteCallback(0);
    rsVideoFrameRateVote->voteCallback_ = [this](uint64_t id, uint32_t rate) { this->VoteCallback(id, rate); };
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
HWTEST_F(RSVideoFrameRateVoteTest, DoReleaseCallback001, Function | SmallTest | Level0)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    ASSERT_EQ(rsVideoFrameRateVote->releaseCallback_, nullptr);
    rsVideoFrameRateVote->DoReleaseCallback();
    rsVideoFrameRateVote->releaseCallback_ = [this](uint64_t id) { this->ReleaseCallback(id); };
    rsVideoFrameRateVote->surfaceNodeId_ = 2;
    rsVideoFrameRateVote->DoReleaseCallback();
    ASSERT_EQ(releaseCallbackResult_, 2);
    rsVideoFrameRateVote = nullptr;
}
} // namespace Rosen
} // namespace OHOS