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
#include "include/command/rs_canvas_node_command.h"
#include "include/command/rs_node_command.h"
#include "params/rs_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeCommandUnitTest::SetUpTestCase() {}
void RSNodeCommandUnitTest::TearDownTestCase() {}
void RSNodeCommandUnitTest::SetUp() {}
void RSNodeCommandUnitTest::TearDown() {}

/**
 * @tc.name: TestRSNodeCommand01
 * @tc.desc: test results of RemoveModifier
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRSNodeCommand01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    PropertyId propertyId = static_cast<PropertyId>(1);
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
    ASSERT_NE(context, nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand02
 * @tc.desc: test results of AddModifier
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRSBaseNodeCommand02, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSNodeCommandHelper::AddModifier(context, nodeId, modifier);
    ASSERT_NE(context, nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand03
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRSBaseNodeCommand03, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestMarkNodeGroup
 * @tc.desc: test results of MarkNodeGroup
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestMarkNodeGroup, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    bool isNodeGroup = false;
    RSNodeCommandHelper::MarkNodeGroup(context, nodeId, isNodeGroup, true, false);
    nodeId = 1;
    RSCanvasNodeCommandHelper::Create(context, nodeId, false);
    isNodeGroup = true;
    RSNodeCommandHelper::MarkNodeGroup(context, nodeId, isNodeGroup, true, false);
    auto canvasNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    ASSERT_NE(canvasNode, nullptr);
    EXPECT_EQ(canvasNode->GetNodeGroupType(), RSRenderNode::GROUPED_BY_USER);
}

/**
 * @tc.name: TestMarkUifirstNode
 * @tc.desc: test results of MarkUifirstNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestMarkUifirstNode, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSNodeCommandHelper::MarkUifirstNode(context, nodeId, true);
    nodeId = 1;
    RSCanvasNodeCommandHelper::Create(context, nodeId, false);
    RSNodeCommandHelper::MarkUifirstNode(context, nodeId, true);
    auto canvasNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    ASSERT_NE(canvasNode, nullptr);
    EXPECT_TRUE(canvasNode->isUifirstNode_);
}

/**
 * @tc.name: TestSetDrawRegion
 * @tc.desc: test results of SetDrawRegion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestSetDrawRegion, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    std::shared_ptr<RectF> rect = nullptr;
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestRegisterGeometryTransitionPair
 * @tc.desc: test results of RegisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRegisterGeometryTransitionPair, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = static_cast<NodeId>(1);
    NodeId outNodeId = static_cast<NodeId>(1);
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId), nullptr);
}

/**
 * @tc.name: TestAddModifier01
 * @tc.desc: test results of AddModifier
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestAddModifier01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    RSNodeCommandHelper::AddModifier(context, nodeId, nullptr);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 0;
    RSNodeCommandHelper::AddModifier(context, nodeId, nullptr);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId)->singleFrameComposer_, nullptr);
}

/**
 * @tc.name: TestRemoveModifier01
 * @tc.desc: test results of RemoveModifier
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRemoveModifier01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    PropertyId propertyId = 0;
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 0;
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestSetFreeze01
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestSetFreeze01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    pid_t pid = ExtractPid(nodeId);
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 0;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    std::shared_ptr<RSBaseRenderNode> renderNode = std::make_shared<RSBaseRenderNode>(0);

    renderNode->stagingRenderParams_ = std::move(stagingRenderParams);
    context.nodeMap.renderNodeMap_[pid][nodeId] = renderNode;
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestSetNodeName01
 * @tc.desc: test results of SetNodeName
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestSetNodeName01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    std::string nodeName = "NODE_NAME";
    RSNodeCommandHelper::SetNodeName(context, nodeId, nodeName);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 0;
    RSNodeCommandHelper::SetNodeName(context, nodeId, nodeName);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestMarkNodeSingleFrameComposer01
 * @tc.desc: test results of MarkNodeSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestMarkNodeSingleFrameComposer01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    pid_t pid = 0;
    RSNodeCommandHelper::MarkNodeSingleFrameComposer(context, nodeId, true, pid);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 0;
    RSNodeCommandHelper::MarkNodeSingleFrameComposer(context, nodeId, true, pid);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestMarkSuggestOpincNode01
 * @tc.desc: test results of MarkSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestMarkSuggestOpincNode01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    RSNodeCommandHelper::MarkSuggestOpincNode(context, nodeId, true, true);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 0;
    RSNodeCommandHelper::MarkSuggestOpincNode(context, nodeId, true, true);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestSetDrawRegion01
 * @tc.desc: test results of SetDrawRegion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestSetDrawRegion01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    auto rect = std::make_shared<RectF>();
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
    EXPECT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 1;
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestSetOutOfParent01
 * @tc.desc: test results of SetOutOfParent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestSetOutOfParent01, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    OutOfParentType outOfParent = OutOfParentType::OUTSIDE;
    RSNodeCommandHelper::SetOutOfParent(context, nodeId, outOfParent);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 1;
    RSNodeCommandHelper::SetOutOfParent(context, nodeId, outOfParent);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestRegisterGeometryTransitionPair01
 * @tc.desc: test results of RegisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRegisterGeometryTransitionPair01, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = 0;
    NodeId outNodeId = 0;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);

    inNodeId = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);

    outNodeId = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);

    inNodeId = 0;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(0), nullptr);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(1), nullptr);
}

/**
 * @tc.name: TestRegisterGeometryTransitionPair02
 * @tc.desc: test results of RegisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRegisterGeometryTransitionPair02, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = 0;
    NodeId outNodeId = 1;

    auto inNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId);
    auto outNode = std::make_shared<RSRenderNode>(outNodeId);
    context.GetMutableNodeMap().RegisterRenderNode(outNode);
    ASSERT_NE(inNode, nullptr);
    ASSERT_NE(inNode, outNode);
    ASSERT_EQ(inNode->sharedTransitionParam_, nullptr);
    ASSERT_EQ(outNode->sharedTransitionParam_, nullptr);

    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_EQ(inNode->sharedTransitionParam_, nullptr);
    ASSERT_EQ(outNode->sharedTransitionParam_, nullptr);

    inNode->instanceRootNodeId_ = 1;
    outNode->instanceRootNodeId_ = 0;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_EQ(inNode->sharedTransitionParam_, nullptr);
    ASSERT_EQ(outNode->sharedTransitionParam_, nullptr);

    inNode->instanceRootNodeId_ = 0;
    outNode->instanceRootNodeId_ = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_EQ(inNode->sharedTransitionParam_, nullptr);
    ASSERT_EQ(outNode->sharedTransitionParam_, nullptr);

    inNode->instanceRootNodeId_ = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_NE(inNode->sharedTransitionParam_, nullptr);
    ASSERT_NE(outNode->sharedTransitionParam_, nullptr);
}

/**
 * @tc.name: TestUnregisterGeometryTransitionPair01
 * @tc.desc: test results of UnregisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestUnregisterGeometryTransitionPair01, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = 0;
    NodeId outNodeId = 0;
    RSNodeCommandHelper::UnregisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId)->sharedTransitionParam_, nullptr);
    RSCanvasNodeCommandHelper::Create(context, inNodeId, false);
    auto inNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId);
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    RSNodeCommandHelper::UnregisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_NE(inNode, nullptr);
    EXPECT_EQ(inNode->sharedTransitionParam_, nullptr);
}

/**
 * @tc.name: TestRemoveAllModifiers
 * @tc.desc: test results of RemoveAllModifiers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestRemoveAllModifiers, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSNodeCommandHelper::RemoveAllModifiers(context, nodeId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
    nodeId = 1;
    RSCanvasNodeCommandHelper::Create(context, nodeId, false);
    RSNodeCommandHelper::RemoveAllModifiers(context, nodeId);
    auto canvasNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    ASSERT_NE(canvasNode, nullptr);
    EXPECT_TRUE(canvasNode->modifiers_.empty());
}

/**
 * @tc.name: TestCommitDumpClientNodeTree
 * @tc.desc: test results of CommitDumpClientNodeTree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestCommitDumpClientNodeTree, TestSize.Level1)
{
    RSContext context;
    bool flag = false;
    auto func = [&flag](NodeId, pid_t, uint32_t, const std::string&) { flag = true; };
    RSNodeCommandHelper::CommitDumpClientNodeTree(context, 0, 0, 0, "test");
    EXPECT_FALSE(flag);
    RSNodeCommandHelper::SetCommitDumpNodeTreeProcessor(func);
    RSNodeCommandHelper::CommitDumpClientNodeTree(context, 0, 0, 0, "test");
    EXPECT_TRUE(flag);
}

/**
 * @tc.name: TestDumpClientNodeTree01
 * @tc.desc: test results of DumpClientNodeTree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandUnitTest, TestDumpClientNodeTree01, TestSize.Level1)
{
    RSContext context;
    bool flag = false;
    auto func = [&flag] (NodeId, pid_t, uint32_t) { flag = true; };
    RSNodeCommandHelper::SetDumpNodeTreeProcessor(func);
    RSNodeCommandHelper::DumpClientNodeTree(context, 0, 0, 0);
    ASSERT_TRUE(flag);

    flag = false;
    RSNodeCommandHelper::SetDumpNodeTreeProcessor(nullptr);
    RSNodeCommandHelper::DumpClientNodeTree(context, 0, 0, 0);
    ASSERT_FALSE(flag);

    SUCCEED();
}
} // namespace OHOS::Rosen
