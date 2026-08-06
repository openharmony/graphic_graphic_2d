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
#include "command/rs_depth_node_command.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_depth_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDepthNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDepthNodeCommandTest::SetUpTestCase() {}
void RSDepthNodeCommandTest::TearDownTestCase() {}
void RSDepthNodeCommandTest::SetUp() {}
void RSDepthNodeCommandTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: Verify RSDepthNodeCommandHelper::Create creates and registers a depth node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, Create001, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId = 100;
    RSDepthNodeCommandHelper::Create(context, testNodeId, false);
    auto node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetType(), RSRenderNodeType::DEPTH_NODE);
}

/**
 * @tc.name: Create002
 * @tc.desc: Verify Create with isTextureExportNode = true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, Create002, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId = 101;
    RSDepthNodeCommandHelper::Create(context, testNodeId, true);
    auto node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId);
    ASSERT_NE(node, nullptr);
}

/**
 * @tc.name: Create003
 * @tc.desc: Verify creating multiple depth nodes with different IDs
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, Create003, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId1 = 200;
    constexpr NodeId testNodeId2 = 201;
    RSDepthNodeCommandHelper::Create(context, testNodeId1, false);
    RSDepthNodeCommandHelper::Create(context, testNodeId2, false);
    auto node1 = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId1);
    auto node2 = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId2);
    ASSERT_NE(node1, nullptr);
    ASSERT_NE(node2, nullptr);
    EXPECT_NE(node1, node2);
}

/**
 * @tc.name: Create004
 * @tc.desc: Verify node ID is correctly set after creation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, Create004, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId = 300;
    RSDepthNodeCommandHelper::Create(context, testNodeId, false);
    auto node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetId(), testNodeId);
}

/**
 * @tc.name: SetType001
 * @tc.desc: Test SetType node not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, SetType001, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId = 114;
    RSDepthNodeCommandHelper::Create(context, testNodeId, false);
    auto node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId);
    ASSERT_NE(node, nullptr);
    RSDepthNodeCommandHelper::SetType(context, testNodeId, DepthSpaceType::INSTANCE);
    EXPECT_EQ(node->GetDepthSpaceType(), DepthSpaceType::INSTANCE);
}

/**
 * @tc.name: SetType002
 * @tc.desc: Test SetType node is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, SetType002, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId = 514;
    RSDepthNodeCommandHelper::SetType(context, testNodeId, DepthSpaceType::INSTANCE);
    auto node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId);
    ASSERT_EQ(node, nullptr);
}

/**
 * @tc.name: SetType003
 * @tc.desc: Test SetType invalid type less than min
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, SetType003, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId = 1919;
    RSDepthNodeCommandHelper::Create(context, testNodeId, false);
    auto node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId);
    ASSERT_NE(node, nullptr);
    RSDepthNodeCommandHelper::SetType(context, testNodeId, static_cast<DepthSpaceType>(-810));
    EXPECT_EQ(node->GetDepthSpaceType(), DepthSpaceType::INSTANCE);
}

/**
 * @tc.name: SetType004
 * @tc.desc: Test SetType invalid type greater than max
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthNodeCommandTest, SetType004, TestSize.Level1)
{
    RSContext context;
    constexpr NodeId testNodeId = 1919;
    RSDepthNodeCommandHelper::Create(context, testNodeId, false);
    auto node = context.GetNodeMap().GetRenderNode<RSDepthRenderNode>(testNodeId);
    ASSERT_NE(node, nullptr);
    RSDepthNodeCommandHelper::SetType(context, testNodeId, static_cast<DepthSpaceType>(810));
    EXPECT_EQ(node->GetDepthSpaceType(), DepthSpaceType::INSTANCE);
}
} // namespace OHOS::Rosen