/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
class RSNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeCommandTest::SetUpTestCase() {}
void RSNodeCommandTest::TearDownTestCase() {}
void RSNodeCommandTest::SetUp() {}
void RSNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSBaseNodeCommand001
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, TestRSNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    PropertyId propertyId = static_cast<PropertyId>(1);
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
    EXPECT_TRUE(nodeId != -2);
}

/**
 * @tc.name: TestRSBaseNodeCommand002
 * @tc.desc: AddModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, TestRSBaseNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSNodeCommandHelper::AddModifier(context, nodeId, modifier);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: TestRSBaseNodeCommand003
 * @tc.desc: SetFreeze test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, TestRSBaseNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: MarkNodeGroupTest
 * @tc.desc: MarkNodeGroup test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, MarkNodeGroupTest, TestSize.Level1)
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
 * @tc.name: MarkUifirstNodeTest
 * @tc.desc: MarkUifirstNode test.
 * @tc.type: FUNC
 * @tc.require: issueIB209E
 */
HWTEST_F(RSNodeCommandTest, MarkUifirstNodeTest, TestSize.Level1)
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
 * @tc.name: SetDrawRegionTest
 * @tc.desc: SetDrawRegion test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, SetDrawRegionTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    std::shared_ptr<RectF> rect = nullptr;
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId) == nullptr);
}

/**
 * @tc.name: SetDrawNodeType
 * @tc.desc: SetDrawNodeType test.
 * @tc.type: FUNC
 * @tc.require: IC8BLE
 */
HWTEST_F(RSNodeCommandTest, SetDrawNodeType, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    std::shared_ptr<RectF> rect = nullptr;
    RSNodeCommandHelper::SetDrawNodeType(context, nodeId, DrawNodeType::PureContainerType);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId) == nullptr);
}

/**
 * @tc.name: RegisterGeometryTransitionPairTest
 * @tc.desc: RegisterGeometryTransitionPair test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, RegisterGeometryTransitionPairTest, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = static_cast<NodeId>(1);
    NodeId outNodeId = static_cast<NodeId>(1);
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId, true);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId) == nullptr);
}

/**
 * @tc.name: AddModifier001
 * @tc.desc: test results of AddModifier
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, AddModifier001, TestSize.Level1)
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
 * @tc.name: RemoveModifier001
 * @tc.desc: test results of RemoveModifier
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, RemoveModifier001, TestSize.Level1)
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
 * @tc.name: SetFreeze001
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSNodeCommandTest, SetFreeze001, TestSize.Level1)
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
 * @tc.name: SetNodeName001
 * @tc.desc: test results of SetNodeName
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, SetNodeName001, TestSize.Level1)
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
 * @tc.name: MarkRepaintBoundary
 * @tc.desc: test results of MarkRepaintBoundary
 * @tc.type: FUNC
 * @tc.require: issuesIC50OX
 */
HWTEST_F(RSNodeCommandTest, MarkRepaintBoundary, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 0;
    RSNodeCommandHelper::MarkRepaintBoundary(context, nodeId, true);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId)->isRepaintBoundary_, true);

    RSNodeCommandHelper::MarkRepaintBoundary(context, nodeId, false);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId)->isRepaintBoundary_, false);
}

/**
 * @tc.name: MarkNodeSingleFrameComposer001
 * @tc.desc: test results of MarkNodeSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, MarkNodeSingleFrameComposer001, TestSize.Level1)
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
 * @tc.name: MarkSuggestOpincNode001
 * @tc.desc: test results of MarkSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSNodeCommandTest, MarkSuggestOpincNode001, TestSize.Level1)
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
 * @tc.name: SetDrawRegion001
 * @tc.desc: test results of SetDrawRegion
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, SetDrawRegion001, TestSize.Level1)
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
 * @tc.name: SetOutOfParent001
 * @tc.desc: test results of SetOutOfParent
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, SetOutOfParent001, TestSize.Level1)
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
 * @tc.name: RegisterGeometryTransitionPair001
 * @tc.desc: test results of RegisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, RegisterGeometryTransitionPair001, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = 0;
    NodeId outNodeId = 0;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId, true);

    inNodeId = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId, true);

    outNodeId = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId, true);

    inNodeId = 0;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId, true);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(0), nullptr);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(1), nullptr);
}

/**
 * @tc.name: UnregisterGeometryTransitionPair001
 * @tc.desc: test results of UnregisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCommandTest, UnregisterGeometryTransitionPair001, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = 0;
    NodeId outNodeId = 0;
    RSNodeCommandHelper::UnregisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId)->sharedTransitionParam_, nullptr);
    RSCanvasNodeCommandHelper::Create(context, inNodeId, false);
    auto inNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId);
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId, true);
    RSNodeCommandHelper::UnregisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_NE(inNode, nullptr);
    EXPECT_EQ(inNode->sharedTransitionParam_, nullptr);
}

/**
 * @tc.name: RemoveAllModifiers
 * @tc.desc: test results of RemoveAllModifiers
 * @tc.type: FUNC
 * @tc.require: issueIB209E
 */
HWTEST_F(RSNodeCommandTest, RemoveAllModifiersTest, TestSize.Level1)
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
 * @tc.name: CommitDumpClientNodeTree
 * @tc.desc: test results of CommitDumpClientNodeTree
 * @tc.type: FUNC
 * @tc.require: issueIB209E
 */
HWTEST_F(RSNodeCommandTest, CommitDumpClientNodeTreeTest, TestSize.Level1)
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
 * @tc.name: DumpClientNodeTree001
 * @tc.desc: test results of DumpClientNodeTree
 * @tc.type: FUNC
 * @tc.require: issueIAKME2
 */
HWTEST_F(RSNodeCommandTest, DumpClientNodeTree001, TestSize.Level1)
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

/**
 * @tc.name: SetUITokenTest001
 * @tc.desc: test results of SetUIToken
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNodeCommandTest, SetUITokenTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    uint64_t token = 1001;
    RSNodeCommandHelper::SetUIToken(context, nodeId, token);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 1;
    RSCanvasNodeCommandHelper::Create(context, nodeId, false);
    auto canvasNode = context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
    RSNodeCommandHelper::SetUIToken(context, nodeId, token);
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_EQ(canvasNode->uiContextToken_, token);
}

/**
 * @tc.name: SetEnableHDREffectTest
 * @tc.desc: SetEnableHDREffect test
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, SetEnableHDREffectTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    RSNodeCommandHelper::SetEnableHDREffect(context, nodeId, true);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId) == nullptr);
    auto node = std::make_shared<RSRenderNode>(nodeId);
    context.nodeMap.renderNodeMap_[ExtractPid(nodeId)][nodeId] = node;
    RSNodeCommandHelper::SetEnableHDREffect(context, nodeId, true);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId)->enableHdrEffect_);
}

/**
 * @tc.name: SetNeedUseCmdlistDrawRegion
 * @tc.desc: SetNeedUseCmdlistDrawRegion test
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandTest, SetNeedUseCmdlistDrawRegion, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    bool needUseCmdlistDrawRegion = false;
    RSNodeCommandHelper::SetNeedUseCmdlistDrawRegion(context, nodeId, needUseCmdlistDrawRegion);
    EXPECT_TRUE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId) == nullptr);
}
} // namespace OHOS::Rosen
