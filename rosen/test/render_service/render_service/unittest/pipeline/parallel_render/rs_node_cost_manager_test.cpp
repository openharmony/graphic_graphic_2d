/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "pipeline/rs_context.h"
#include "pipeline/parallel_render/rs_node_cost_manager.h"
#include "limit_number.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsNodeCostManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsNodeCostManagerTest::SetUpTestCase() {}
void RsNodeCostManagerTest::TearDownTestCase() {}
void RsNodeCostManagerTest::SetUp() {}
void RsNodeCostManagerTest::TearDown() {}

/**
 * @tc.name: GetDirtyNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.GetDirtyNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, GetDirtyNodeCostTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
    rsNodeCostManager->AddNodeCost(5);
    result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(5, result);
}

/**
 * @tc.name: IsSkipProcessingTest
 * @tc.desc: Test RsNodeCostManagerTest.IsSkipProcessingTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, IsSkipProcessingTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->SetSecurityLayer(1);
    auto result = rsNodeCostManager->IsSkipProcessing(*rsSurfaceRenderNode);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CalcBaseRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcBaseRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcBaseRenderNodeCostTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsNodeCostManager->CalcBaseRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}

/**
 * @tc.name: CalcCanvasRenderNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.CalcCanvasRenderNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcCanvasRenderNodeCostTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    rsNodeCostManager->CalcBaseRenderNodeCost(node);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}

/**
 * @tc.name: AddNodeCostTest
 * @tc.desc: Test RsNodeCostManagerTest.AddNodeCostTest
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, AddNodeCostTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    rsNodeCostManager->AddNodeCost(5);
    auto result = rsNodeCostManager->dirtyNodeCost_;
    ASSERT_EQ(5, result);
}

/**
 * @tc.name: CalcSurfaceRenderNodeCost
 * @tc.desc: Test RsNodeCostManagerTest.CalcSurfaceRenderNodeCost
 * @tc.type: FUNC
 * @tc.require: issueI6FZHQ
 */
HWTEST_F(RsNodeCostManagerTest, CalcSurfaceRenderNodeCostTest, TestSize.Level1)
{
    auto rsNodeCostManager = std::make_shared<RSNodeCostManager>(1, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsNodeCostManager->CalcSurfaceRenderNodeCost(*rsSurfaceRenderNode);
    auto result = rsNodeCostManager->GetDirtyNodeCost();
    ASSERT_EQ(0, result);
}
} // namespace OHOS::Rosen