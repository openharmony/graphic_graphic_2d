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
#include "include/command/rs_node_command.h"
#include "params/rs_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeCmdUniTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeCmdUniTest::SetUpTestCase() {}
void RSNodeCmdUniTest::TearDownTestCase() {}
void RSNodeCmdUniTest::SetUp() {}
void RSNodeCmdUniTest::TearDown() {}

/**
 * @tc.name: MarkNodeGroupUniTest
 * @tc.desc: MarkNodeGroup test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCmdUniTest, MarkNodeGroupUniTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    bool isNodeGroup = false;
    RSNodeCommandHelper::MarkNodeGroup(context, nodeId, isNodeGroup, true, false);
}

/**
 * @tc.name: TestRSBaseNodeCmdUniTestUniTest001
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCmdUniTest, TestRSNodeCmdUniTestUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    PropertyId propertyId = static_cast<PropertyId>(1);
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
}

/**
 * @tc.name: TestRSBaseNodeCmdUniTestUniTest002
 * @tc.desc: AddModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCmdUniTest, TestRSBaseNodeCmdUniTestUniTest002, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSNodeCommandHelper::AddModifier(context, nodeId, modifier);
}

/**
 * @tc.name: TestRSBaseNodeCmdUniTestUniTest003
 * @tc.desc: SetFreeze test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCmdUniTest, TestRSBaseNodeCmdUniTestUniTest003, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(-1);
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    EXPECT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: RegisterGeometryTransitionPairUniTest
 * @tc.desc: RegisterGeometryTransitionPair test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCmdUniTest, RegisterGeometryTransitionPairUniTest, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = static_cast<NodeId>(1);
    NodeId outNodeId = static_cast<NodeId>(1);
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
}

/**
 * @tc.name: SetDrawRegionUniTest
 * @tc.desc: SetDrawRegion test.
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCmdUniTest, SetDrawRegionUniTest, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = static_cast<NodeId>(1);
    std::shared_ptr<RectF> rect = nullptr;
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
}

/**
 * @tc.name: RemoveModifierUniTest001
 * @tc.desc: test results of RemoveModifier
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, RemoveModifierUniTest001, TestSize.Level1)
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
 * @tc.name: AddModifierUniTest001
 * @tc.desc: test results of AddModifier
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, AddModifierUniTest001, TestSize.Level1)
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
 * @tc.name: SetNodeNameUniTest001
 * @tc.desc: test results of SetNodeName
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, SetNodeNameUniTest001, TestSize.Level1)
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
 * @tc.name: MarkNodeSingleFrameComposerUniTest001
 * @tc.desc: test results of MarkNodeSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, MarkNodeSingleFrameComposerUniTest001, TestSize.Level1)
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
 * @tc.name: SetFreezeUniTest001
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSNodeCmdUniTest, SetFreezeUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = 1;
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);

    nodeId = 0;
    pid_t pid = ExtractPid(nodeId);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    std::shared_ptr<RSBaseRenderNode> renderNode = std::make_shared<RSBaseRenderNode>(0);

    renderNode->stagingRenderParams_ = std::move(stagingRenderParams);
    context.nodeMap.renderNodeMap_[pid][nodeId] = renderNode;
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    ASSERT_NE(context.GetNodeMap().GetRenderNode<RSRenderNode>(nodeId), nullptr);
}

/**
 * @tc.name: SetDrawRegionUniTest001
 * @tc.desc: test results of SetDrawRegion
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, SetDrawRegionUniTest001, TestSize.Level1)
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
 * @tc.name: MarkSuggestOpincNodeUniTest001
 * @tc.desc: test results of MarkSuggestOpincNode
 * @tc.type: FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSNodeCmdUniTest, MarkSuggestOpincNodeUniTest001, TestSize.Level1)
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
 * @tc.name: RegisterGeometryTransitionPairUniTest001
 * @tc.desc: test results of RegisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, RegisterGeometryTransitionPairUniTest001, TestSize.Level1)
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
 * @tc.name: SetOutOfParentUniTest001
 * @tc.desc: test results of SetOutOfParent
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, SetOutOfParentUniTest001, TestSize.Level1)
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
 * @tc.name: DumpClientNodeTreeUniTest001
 * @tc.desc: test results of DumpClientNodeTree
 * @tc.type: FUNC
 * @tc.require: issueIAKME2
 */
HWTEST_F(RSNodeCmdUniTest, DumpClientNodeTreeUniTest001, TestSize.Level1)
{
    RSContext context;
    bool flag = false;
    auto func = [&flag] (NodeId, pid_t, uint32_t, const std::string&) { flag = true; };
    RSNodeCommandHelper::SetDumpNodeTreeProcessor(func);
    RSNodeCommandHelper::DumpClientNodeTree(context, 0, 0, 0, "");
    ASSERT_TRUE(flag);

    flag = false;
    RSNodeCommandHelper::SetDumpNodeTreeProcessor(nullptr);
    RSNodeCommandHelper::DumpClientNodeTree(context, 0, 0, 0, "");
    ASSERT_FALSE(flag);

    SUCCEED();
}

/**
 * @tc.name: UnregisterGeometryTransitionPairUniTest001
 * @tc.desc: test results of UnregisterGeometryTransitionPair
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSNodeCmdUniTest, UnregisterGeometryTransitionPairUniTest001, TestSize.Level1)
{
    RSContext context;
    NodeId inNodeId = 0;
    NodeId outNodeId = 0;
    RSNodeCommandHelper::UnregisterGeometryTransitionPair(context, inNodeId, outNodeId);
    ASSERT_EQ(context.GetNodeMap().GetRenderNode<RSRenderNode>(inNodeId)->sharedTransitionParam_, nullptr);
}
} // namespace OHOS::Rosen
