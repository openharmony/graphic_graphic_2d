/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "command/rs_spatial_effect_command.h"
#include "command/rs_surface_node_command.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSpatialEffectCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpatialEffectCommandTest::SetUpTestCase() {}
void RSSpatialEffectCommandTest::TearDownTestCase() {}
void RSSpatialEffectCommandTest::SetUp() {}
void RSSpatialEffectCommandTest::TearDown() {}

/**
 * @tc.name: SetIsDepthBackground001
 * @tc.desc: Verify SetIsDepthBackground with invalid node ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthBackground001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);

    // Setting property on invalid node should not crash
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, id, true);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetIsDepthBackground002
 * @tc.desc: Verify SetIsDepthBackground with valid node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthBackground002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 10;

    // Create the surface render node
    SurfaceNodeCommandHelper::Create(context, nodeId);

    // Set isDepthBackground to true
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, true);

    // Verify node exists
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SetIsDepthBackground003
 * @tc.desc: Verify SetIsDepthBackground with false value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthBackground003, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 20;

    SurfaceNodeCommandHelper::Create(context, nodeId);
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, false);

    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SetIsDepthBackground004
 * @tc.desc: Verify SetIsDepthBackground on SurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthBackground004, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 30;

    SurfaceNodeCommandHelper::Create(context, nodeId);
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, true);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SetIsDepthBackground005
 * @tc.desc: Verify multiple SetIsDepthBackground calls
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthBackground005, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 40;

    SurfaceNodeCommandHelper::Create(context, nodeId);
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, true);
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, false);
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, true);

    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SetIsDepthResource001
 * @tc.desc: Verify SetIsDepthResource with invalid node ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthResource001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);

    // Setting property on invalid node should not crash
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, id, true);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(id), nullptr);
}

/**
 * @tc.name: SetIsDepthResource002
 * @tc.desc: Verify SetIsDepthResource with valid SurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthResource002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 50;

    // Create a surface render node
    SurfaceNodeCommandHelper::Create(context, nodeId);

    // Set isDepthResource to true
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, true);

    // Verify node exists
    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SetIsDepthResource003
 * @tc.desc: Verify SetIsDepthResource with false value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthResource003, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 60;

    SurfaceNodeCommandHelper::Create(context, nodeId);
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, false);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SetIsDepthResource004
 * @tc.desc: Verify SetIsDepthResource on SurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthResource004, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 70;

    // Create a surface render node (which also is RSRenderNode)
    SurfaceNodeCommandHelper::Create(context, nodeId);

    // Set the property
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, true);

    // Verify node exists as both types
    auto renderNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    ASSERT_NE(renderNode, nullptr);

    auto surfaceNode = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(surfaceNode, nullptr);
}

/**
 * @tc.name: SetIsDepthResource005
 * @tc.desc: Verify multiple SetIsDepthResource calls
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthResource005, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 80;

    SurfaceNodeCommandHelper::Create(context, nodeId);
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, true);
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, false);
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, true);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: SetIsDepthResource006
 * @tc.desc: Verify SetIsDepthResource with zero node ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, SetIsDepthResource006, TestSize.Level1)
{
    RSContext context;

    RSSpatialEffectCommandHelper::SetIsDepthResource(context, 0, true);
    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(0);
    EXPECT_EQ(node, nullptr);
}

/**
 * @tc.name: CombinedTest001
 * @tc.desc: Verify using both SetIsDepthBackground and SetIsDepthResource on different nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, CombinedTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId1 = 90;
    NodeId nodeId2 = 91;

    // Create two surface render nodes
    SurfaceNodeCommandHelper::Create(context, nodeId1);
    SurfaceNodeCommandHelper::Create(context, nodeId2);

    // Set different properties on different nodes
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId1, true);
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId2, true);

    auto node1 = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId1);
    auto node2 = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId2);

    ASSERT_NE(node1, nullptr);
    ASSERT_NE(node2, nullptr);
    EXPECT_NE(node1, node2);
}

/**
 * @tc.name: CombinedTest002
 * @tc.desc: Verify setting both properties on same SurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, CombinedTest002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 100;

    SurfaceNodeCommandHelper::Create(context, nodeId);

    // Set both properties on same node
    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, true);
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, false);

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: EdgeCaseTest001
 * @tc.desc: Test with very large node ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, EdgeCaseTest001, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId largeNodeId = 4294967294; // Near max uint32_t

    RSSpatialEffectCommandHelper::SetIsDepthBackground(context, largeNodeId, true);
    RSSpatialEffectCommandHelper::SetIsDepthResource(context, largeNodeId, true);

    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(largeNodeId);
    EXPECT_EQ(node, nullptr);
}

/**
 * @tc.name: EdgeCaseTest002
 * @tc.desc: Test alternating values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSpatialEffectCommandTest, EdgeCaseTest002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 110;

    SurfaceNodeCommandHelper::Create(context, nodeId);

    // Alternate between true and false
    for (int i = 0; i < 5; i++) {
        bool value = (i % 2 == 0);
        RSSpatialEffectCommandHelper::SetIsDepthBackground(context, nodeId, value);
        RSSpatialEffectCommandHelper::SetIsDepthResource(context, nodeId, !value);
    }

    auto node = context.GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
}
} // namespace OHOS::Rosen