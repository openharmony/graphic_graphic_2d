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
 
#include "hgm_test_base.h"
#include "hgm_voter.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace Rosen {
namespace {
    std::vector<std::string> voters = {"voter1", "voter2"};
}
class DeliverVoteTest : public HgmTestBase {
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
 * @tc.name  : DeliverVote
 * @tc.number: DeliverVoteTest_001
 * @tc.desc  : When there are no voters, DeliverVote should return false
 */
HWTEST_F(DeliverVoteTest, DeliverVoteTest_001, Function | SmallTest | Level1) {
    auto voter = std::make_shared<HgmVoter>(voters);
    VoteInfo voteInfo;
    voteInfo.voterName = "nonexistentVoter";
    voteInfo.pid = 123;
    voteInfo.min = 1;
    voteInfo.max = 100;
 
    bool result = voter->DeliverVote(voteInfo, true);
    EXPECT_FALSE(result);
}
 
/**
 * @tc.name  : DeliverVote
 * @tc.number: DeliverVoteTest_002
 * @tc.desc  : When pid is 0 and eventStatus is false,
DeliverVote should clear vec and return true
 */
HWTEST_F(DeliverVoteTest, DeliverVoteTest_002, Function | SmallTest | Level1) {
    auto voter = std::make_shared<HgmVoter>(voters);
    VoteInfo voteInfo;
    voteInfo.voterName = "voter1";
    voteInfo.pid = 0;
    voteInfo.min = 1;
    voteInfo.max = 100;
 
    voter->voteRecord_["voter1"].first.push_back(voteInfo);
 
    bool result = voter->DeliverVote(voteInfo, false);
    EXPECT_TRUE(result);
    EXPECT_TRUE(voter->voteRecord_["voter1"].first.empty());
}
 
/**
 * @tc.name  : DeliverVote
 * @tc.number: DeliverVoteTest_003
 * @tc.desc  : When eventStatus is false and pid matches,
DeliverVote should delete the matching record and return true
 */
HWTEST_F(DeliverVoteTest, DeliverVoteTest_003, Function | SmallTest | Level1) {
    auto voter = std::make_shared<HgmVoter>(voters);
    VoteInfo voteInfo;
    voteInfo.voterName = "voter1";
    voteInfo.pid = 123;
    voteInfo.min = 1;
    voteInfo.max = 100;
 
    voter->voteRecord_["voter1"].first.push_back(voteInfo);
 
    bool result = voter->DeliverVote(voteInfo, false);
    EXPECT_TRUE(result);
    EXPECT_TRUE(voter->voteRecord_["voter1"].first.empty());
}
 
/**
 * @tc.name  : DeliverVote
 * @tc.number: DeliverVoteTest_004
 * @tc.desc  : When eventStatus is true and pid matches,
DeliverVote should modify the matched record and return true.
 */
HWTEST_F(DeliverVoteTest, DeliverVoteTest_004, Function | SmallTest | Level1) {
    auto voter = std::make_shared<HgmVoter>(voters);
    VoteInfo voteInfo;
    voteInfo.voterName = "voter1";
    voteInfo.pid = 123;
    voteInfo.min = 1;
    voteInfo.max = 100;
 
    voter->voteRecord_["voter1"].first.push_back(voteInfo);
    bool result = voter->DeliverVote(voteInfo, true);
    EXPECT_FALSE(result);
 
    voteInfo.min = 1;
    voteInfo.max = 120;
    result = voter->DeliverVote(voteInfo, true);
    EXPECT_TRUE(result);
    EXPECT_EQ(voter->voteRecord_["voter1"].first.size(), 1);
    EXPECT_EQ(voter->voteRecord_["voter1"].first[0].min, 1);
    EXPECT_EQ(voter->voteRecord_["voter1"].first[0].max, 120);
 
    voteInfo.min = 20;
    voteInfo.max = 100;
    result = voter->DeliverVote(voteInfo, true);
    EXPECT_TRUE(result);
    EXPECT_EQ(voter->voteRecord_["voter1"].first.size(), 1);
    EXPECT_EQ(voter->voteRecord_["voter1"].first[0].min, 20);
    EXPECT_EQ(voter->voteRecord_["voter1"].first[0].max, 100);
 
    voteInfo.min = 20;
    voteInfo.max = 120;
    result = voter->DeliverVote(voteInfo, true);
    EXPECT_TRUE(result);
    EXPECT_EQ(voter->voteRecord_["voter1"].first.size(), 1);
    EXPECT_EQ(voter->voteRecord_["voter1"].first[0].min, 20);
    EXPECT_EQ(voter->voteRecord_["voter1"].first[0].max, 120);
}
 
/**
 * @tc.name  : DeliverVote
 * @tc.number: DeliverVoteTest_005
 * @tc.desc  : When eventStatus is true and pid does not match,
DeliverVote should add a new record and return true
 */
HWTEST_F(DeliverVoteTest, DeliverVoteTest_005, Function | SmallTest | Level1) {
    auto voter = std::make_shared<HgmVoter>(voters);
    VoteInfo voteInfo;
    voteInfo.voterName = "voter1";
    voteInfo.pid = 456;
    voteInfo.min = 1;
    voteInfo.max = 100;
 
    bool result = voter->DeliverVote(voteInfo, true);
    EXPECT_TRUE(result);
    EXPECT_EQ(voter->voteRecord_["voter1"].first.size(), 1);
    EXPECT_EQ(voter->voteRecord_["voter1"].first[0].pid, 456);
}
} // namespace Rosen
} // namespace OHOS