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

#include "gtest/gtest.h"
#include "include/command/rs_base_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBaseNodeCommandUnitTest::SetUpTestCase() {}
void RSBaseNodeCommandUnitTest::TearDownTestCase() {}
void RSBaseNodeCommandUnitTest::SetUp() {}
void RSBaseNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestRSBaseNodeCommand01
 * @tc.desc: Verify function RSBaseNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRSBaseNodeCommand01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::Destroy(context, nodeId);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand02
 * @tc.desc: Verify function RSBaseNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRSBaseNodeCommand02, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(0);
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(nodeId), nullptr);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(childNodeId), nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand03
 * @tc.desc: Verify function RSBaseNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRSBaseNodeCommand03, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(1);
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(nodeId), nullptr);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(childNodeId), nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand04
 * @tc.desc: Verify function RSBaseNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRSBaseNodeCommand04, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    int32_t index = static_cast<int32_t>(1);
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childNodeId, index);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(nodeId), nullptr);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(childNodeId), nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand05
 * @tc.desc: Verify function RSBaseNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRSBaseNodeCommand05, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    NodeId childNodeId = static_cast<NodeId>(-2);
    NodeId newParentId = static_cast<NodeId>(-3);
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(nodeId), nullptr);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(childNodeId), nullptr);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(newParentId), nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand06
 * @tc.desc: Verify function RSBaseNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRSBaseNodeCommand06, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand07
 * @tc.desc: Verify function RSBaseNodeCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRSBaseNodeCommand07, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode(nodeId), nullptr);
}

/**
 * @tc.name: TestDestroy01
 * @tc.desc: Verify function Destroy
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestDestroy01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    BaseNodeCommandHelper::Destroy(context, nodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::Destroy(context, nodeId);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: TestAddChild01
 * @tc.desc: Verify function AddChild
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestAddChild01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    int32_t index = 1;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);

    childNodeId = 0;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);

    nodeId = 1;
    BaseNodeCommandHelper::AddChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);
}

/**
 * @tc.name: TestMoveChild01
 * @tc.desc: Verify function MoveChil
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestMoveChild01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    int32_t index = 1;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);

    childNodeId = 0;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId == 0);

    nodeId = 1;
    BaseNodeCommandHelper::MoveChild(context, nodeId, childNodeId, index);
    EXPECT_TRUE(nodeId);
}

/**
 * @tc.name: TestRemoveChild01
 * @tc.desc: Verify function RemoveChild
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRemoveChild01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId == 0);

    childNodeId = 0;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId == 0);

    nodeId = 1;
    BaseNodeCommandHelper::RemoveChild(context, nodeId, childNodeId);
    EXPECT_TRUE(nodeId);
}

/**
 * @tc.name: TestAddCrossParentChild01
 * @tc.desc: Verify function AddCrossParentChild
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestAddCrossParentChild01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childId = 1;
    int32_t index = 1;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId == 0);

    childId = 0;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId == 0);

    nodeId = 1;
    BaseNodeCommandHelper::AddCrossParentChild(context, nodeId, childId, index);
    EXPECT_TRUE(nodeId);
}

/**
 * @tc.name: TestRemoveCrossParentChild01
 * @tc.desc: Verify function RemoveCrossParentChild
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRemoveCrossParentChild01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    NodeId childNodeId = 1;
    NodeId newParentId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!nodeId);

    childNodeId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);

    newParentId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);

    childNodeId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(childNodeId);

    nodeId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(nodeId);

    childNodeId = 0;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);

    newParentId = 1;
    BaseNodeCommandHelper::RemoveCrossParentChild(context, nodeId, childNodeId, newParentId);
    EXPECT_TRUE(!childNodeId);
}

/**
 * @tc.name: TestRemoveFromTree01
 * @tc.desc: Verify function RemoveFromTree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestRemoveFromTree01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::RemoveFromTree(context, nodeId);
    EXPECT_TRUE(nodeId == 0);
}

/**
 * @tc.name: TestClearChildren01
 * @tc.desc: Verify function ClearChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseNodeCommandUnitTest, TestClearChildren01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    EXPECT_TRUE(nodeId);

    nodeId = 0;
    BaseNodeCommandHelper::ClearChildren(context, nodeId);
    EXPECT_TRUE(!nodeId);
}
} // namespace OHOS::Rosen
