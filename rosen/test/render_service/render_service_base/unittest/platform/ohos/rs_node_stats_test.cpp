/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "platform/ohos/rs_node_stats.h"

#include <chrono>
#include <sys/time.h>
#include <unistd.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSNodeStatsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeStatsTest::SetUpTestCase() {}
void RSNodeStatsTest::TearDownTestCase() {}
void RSNodeStatsTest::SetUp() {}
void RSNodeStatsTest::TearDown() {}

/**
 * @tc.name: AddNodeStatsTest1
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeStatsTest, AddNodeStatsTest1, TestSize.Level1)
{
    auto& rsNodeStats = RSNodeStats::GetInstance();
    RSNodeStatsType nodeStats1 = CreateRSNodeStatsItem(100, 1000, "Node1");
    RSNodeStatsType nodeStats2 = CreateRSNodeStatsItem(200, 2000, "");
    RSNodeStatsType nodeStats3 = CreateRSNodeStatsItem(300, 3000, "Node3");
    rsNodeStats.AddNodeStats(nodeStats1, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats3, RSNodeStatsUpdateMode::SIMPLE_APPEND);
}

/**
 * @tc.name: AddNodeStatsTest2
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeStatsTest, AddNodeStatsTest2, TestSize.Level1)
{
    auto& rsNodeStats = RSNodeStats::GetInstance();
    RSNodeStatsType nodeStats1 = CreateRSNodeStatsItem(400, 1000, "");
    RSNodeStatsType nodeStats2 = CreateRSNodeStatsItem(500, 1000, "Node2");
    RSNodeStatsType nodeStats3 = CreateRSNodeStatsItem(600, 3000, "");
    rsNodeStats.AddNodeStats(nodeStats1, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.AddNodeStats(nodeStats3, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
}

/**
 * @tc.name: ClearNodeStatsTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeStatsTest, ClearNodeStatsTest, TestSize.Level1)
{
    auto& rsNodeStats = RSNodeStats::GetInstance();
    rsNodeStats.ClearNodeStats();
    RSNodeStatsType nodeStats1 = CreateRSNodeStatsItem(700, 1000, "Node1");
    RSNodeStatsType nodeStats2 = CreateRSNodeStatsItem(800, 2000, "Node2");
    rsNodeStats.AddNodeStats(nodeStats1, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.ClearNodeStats();
}

/**
 * @tc.name: ReportRSNodeLimitExceededTest1
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeStatsTest, ReportRSNodeLimitExceededTest1, TestSize.Level1)
{
    auto& rsNodeStats = RSNodeStats::GetInstance();
    RSNodeStatsType nodeStats1 = CreateRSNodeStatsItem(350, 1000, "Node1");
    RSNodeStatsType nodeStats2 = CreateRSNodeStatsItem(450, 2000, "Node2");
    rsNodeStats.AddNodeStats(nodeStats1, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.ReportRSNodeLimitExceeded();
}

/**
 * @tc.name: ReportRSNodeLimitExceededTest2
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeStatsTest, ReportRSNodeLimitExceededTest2, TestSize.Level1)
{
    auto& rsNodeStats = RSNodeStats::GetInstance();
    RSNodeStatsType nodeStats1 = CreateRSNodeStatsItem(100, 1000, "Node1");
    RSNodeStatsType nodeStats2 = CreateRSNodeStatsItem(200, 2000, "Node2");
    RSNodeStatsType nodeStats3 = CreateRSNodeStatsItem(300, 3000, "");
    RSNodeStatsType nodeStats4 = CreateRSNodeStatsItem(200, 4000, "Node4");
    RSNodeStatsType nodeStats5 = CreateRSNodeStatsItem(100, 5000, "Node5");
    rsNodeStats.AddNodeStats(nodeStats1, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats3, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats4, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats5, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.ReportRSNodeLimitExceeded();
}

/**
 * @tc.name: ReportRSNodeLimitExceededTest3
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeStatsTest, ReportRSNodeLimitExceededTest3, TestSize.Level1)
{
    auto& rsNodeStats = RSNodeStats::GetInstance();
    RSNodeStatsType nodeStats1 = CreateRSNodeStatsItem(100, 1000, "Node1");
    RSNodeStatsType nodeStats2 = CreateRSNodeStatsItem(200, 1000, "");
    RSNodeStatsType nodeStats3 = CreateRSNodeStatsItem(300, 3000, "Node3");
    RSNodeStatsType nodeStats4 = CreateRSNodeStatsItem(200, 4000, "");
    RSNodeStatsType nodeStats5 = CreateRSNodeStatsItem(100, 4000, "Node5");
    rsNodeStats.AddNodeStats(nodeStats1, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.AddNodeStats(nodeStats3, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.AddNodeStats(nodeStats4, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.AddNodeStats(nodeStats5, RSNodeStatsUpdateMode::DISCARD_DUPLICATE);
    rsNodeStats.ReportRSNodeLimitExceeded();
    usleep(100000);
    rsNodeStats.ReportRSNodeLimitExceeded();
    rsNodeStats.ClearNodeStats();
    rsNodeStats.ReportRSNodeLimitExceeded();
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats2, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats3, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats3, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats4, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.AddNodeStats(nodeStats4, RSNodeStatsUpdateMode::SIMPLE_APPEND);
    rsNodeStats.ReportRSNodeLimitExceeded();
}

} // namespace Rosen
} // namespace OHOS
