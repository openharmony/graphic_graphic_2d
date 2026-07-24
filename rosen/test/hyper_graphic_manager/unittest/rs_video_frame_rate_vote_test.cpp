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
    rsVideoFrameRateVote->StartVideoFrameRateVote(30);
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
    rsVideoFrameRateVote->VoteVideoFrameRate(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(300);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);
    rsVideoFrameRateVote->VoteVideoFrameRate(60);
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

/**
 * @tc.name: ReSetLastRate001
 * @tc.desc: Verify the result of ReSetLastRate function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, ReSetLastRate001, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->StartVideoFrameRateVote(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);
    rsVideoFrameRateVote->ReSetLastRate();
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 0);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteVideoFrameRate002
 * @tc.desc: Test VoteVideoFrameRate with boundary values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, VoteVideoFrameRate002, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->VoteVideoFrameRate(1);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 1);
    rsVideoFrameRateVote->VoteVideoFrameRate(144);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 144);
    rsVideoFrameRateVote->VoteVideoFrameRate(0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 144);
    rsVideoFrameRateVote->VoteVideoFrameRate(145);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 144);
    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: StartVideoFrameRateVote002
 * @tc.desc: test StartVideoFrameRateVote with different rates
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, StartVideoFrameRateVote002, Function | SmallTest | Level0)
{
    voteCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->voteCallback_ = [this](uint64_t id, uint32_t rate) { this->VoteCallback(id, rate); };
    rsVideoFrameRateVote->surfaceNodeId_ = 2;

    rsVideoFrameRateVote->StartVideoFrameRateVote(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);

    rsVideoFrameRateVote->StartVideoFrameRateVote(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);

    rsVideoFrameRateVote->StartVideoFrameRateVote(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteVideoFrameRate003
 * @tc.desc: test VoteVideoFrameRate with rate 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, VoteVideoFrameRate003, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->lastRate_ = 60;

    rsVideoFrameRateVote->VoteVideoFrameRate(0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteVideoFrameRate004
 * @tc.desc: test VoteVideoFrameRate with rate > NORMAL_RATE_MAX
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, VoteVideoFrameRate004, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->lastRate_ = 60;

    rsVideoFrameRateVote->VoteVideoFrameRate(145);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: SendDelayTask002
 * @tc.desc: test SendDelayTask when ffrtQueue_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, SendDelayTask002, Function | SmallTest | Level0)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->releaseCallback_ = [this](uint64_t id) { this->ReleaseCallback(id); };
    rsVideoFrameRateVote->surfaceNodeId_ = 2;
    rsVideoFrameRateVote->ffrtQueue_ = nullptr;

    rsVideoFrameRateVote->SendDelayTask();

    ASSERT_EQ(rsVideoFrameRateVote->taskHandler_, nullptr);
    sleep(2);
    ASSERT_EQ(releaseCallbackResult_, 0);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: CancelDelayTask002
 * @tc.desc: test CancelDelayTask when ffrtQueue_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, CancelDelayTask002, Function | SmallTest | Level0)
{
    releaseCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->releaseCallback_ = [this](uint64_t id) { this->ReleaseCallback(id); };
    rsVideoFrameRateVote->surfaceNodeId_ = 2;
    rsVideoFrameRateVote->SendDelayTask();
    ASSERT_NE(rsVideoFrameRateVote->taskHandler_, nullptr);

    rsVideoFrameRateVote->ffrtQueue_ = nullptr;
    rsVideoFrameRateVote->CancelDelayTask();

    ASSERT_NE(rsVideoFrameRateVote->taskHandler_, nullptr);
    sleep(2);
    ASSERT_EQ(releaseCallbackResult_, 0);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: CancelDelayTask003
 * @tc.desc: test CancelDelayTask when ffrtQueue_ is nullptr and taskHandler_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, CancelDelayTask003, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->ffrtQueue_ = nullptr;
    rsVideoFrameRateVote->taskHandler_ = nullptr;

    rsVideoFrameRateVote->CancelDelayTask();

    ASSERT_EQ(rsVideoFrameRateVote->taskHandler_, nullptr);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteVideoFrameRate005
 * @tc.desc: test VoteVideoFrameRate when videoRate < NORMAL_RATE_MIN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, VoteVideoFrameRate005, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->lastRate_ = 60;

    rsVideoFrameRateVote->VoteVideoFrameRate(0);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: VoteVideoFrameRate006
 * @tc.desc: test VoteVideoFrameRate when videoRate == lastRate_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, VoteVideoFrameRate006, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->lastRate_ = 60;

    rsVideoFrameRateVote->VoteVideoFrameRate(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: StartVideoFrameRateVote003
 * @tc.desc: test StartVideoFrameRateVote with different rates
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, StartVideoFrameRateVote003, Function | SmallTest | Level0)
{
    voteCallbackResult_ = 0;
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->voteCallback_ = [this](uint64_t id, uint32_t rate) { this->VoteCallback(id, rate); };
    rsVideoFrameRateVote->surfaceNodeId_ = 2;

    rsVideoFrameRateVote->StartVideoFrameRateVote(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);

    rsVideoFrameRateVote->StartVideoFrameRateVote(60);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 60);

    rsVideoFrameRateVote->StartVideoFrameRateVote(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);

    rsVideoFrameRateVote = nullptr;
}

/**
 * @tc.name: StartVideoFrameRateVote004
 * @tc.desc: test StartVideoFrameRateVote when ffrtQueue_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSVideoFrameRateVoteTest, StartVideoFrameRateVote004, Function | SmallTest | Level0)
{
    std::shared_ptr<RSVideoFrameRateVote> rsVideoFrameRateVote = std::make_shared<RSVideoFrameRateVote>(0,
        nullptr, nullptr);
    rsVideoFrameRateVote->ffrtQueue_ = nullptr;

    rsVideoFrameRateVote->StartVideoFrameRateVote(30);
    ASSERT_EQ(rsVideoFrameRateVote->lastRate_, 30);

    rsVideoFrameRateVote = nullptr;
}
} // namespace Rosen
} // namespace OHOS