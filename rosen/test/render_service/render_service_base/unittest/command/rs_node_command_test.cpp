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
    EXPECT_TRUE(nodeId == -1);
    PropertyId propertyId = static_cast<PropertyId>(1);
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
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
    EXPECT_TRUE(nodeId == -1);
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSNodeCommandHelper::AddModifier(context, nodeId, modifier);
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
    EXPECT_TRUE(nodeId == -1);
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
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
    EXPECT_TRUE(nodeId == -1);
    bool isNodeGroup = false;
    RSNodeCommandHelper::MarkNodeGroup(context, nodeId, isNodeGroup, true, false);
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
    EXPECT_TRUE(nodeId == 1);
    std::shared_ptr<RectF> rect = nullptr;
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
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
    EXPECT_TRUE(inNodeId == 1);
    NodeId outNodeId = static_cast<NodeId>(1);
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
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
    EXPECT_EQ(1, nodeId);

    nodeId = 0;
    RSNodeCommandHelper::AddModifier(context, nodeId, nullptr);
    EXPECT_EQ(0, nodeId);
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
    EXPECT_EQ(1, nodeId);

    nodeId = 0;
    RSNodeCommandHelper::RemoveModifier(context, nodeId, propertyId);
    EXPECT_EQ(0, nodeId);
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
    EXPECT_EQ(1, nodeId);

    nodeId = 0;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    std::shared_ptr<RSBaseRenderNode> renderNode = std::make_shared<RSBaseRenderNode>(0);
    EXPECT_NE(renderNode, nullptr);

    renderNode->stagingRenderParams_ = std::move(stagingRenderParams);
    context.nodeMap.renderNodeMap_[pid][nodeId] = renderNode;
    RSNodeCommandHelper::SetFreeze(context, nodeId, true);
    EXPECT_EQ(0, nodeId);
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
    std::string nodeName = "";
    RSNodeCommandHelper::SetNodeName(context, nodeId, nodeName);
    EXPECT_EQ(1, nodeId);

    nodeId = 0;
    RSNodeCommandHelper::SetNodeName(context, nodeId, nodeName);
    EXPECT_EQ(0, nodeId);
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
    EXPECT_EQ(1, nodeId);

    nodeId = 0;
    RSNodeCommandHelper::MarkNodeSingleFrameComposer(context, nodeId, true, pid);
    EXPECT_EQ(0, nodeId);
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
    EXPECT_EQ(1, nodeId);

    nodeId = 0;
    RSNodeCommandHelper::MarkSuggestOpincNode(context, nodeId, true, true);
    EXPECT_EQ(0, nodeId);
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
    EXPECT_EQ(0, nodeId);

    nodeId = 1;
    RSNodeCommandHelper::SetDrawRegion(context, nodeId, rect);
    EXPECT_EQ(1, nodeId);
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
    EXPECT_EQ(0, nodeId);

    nodeId = 1;
    RSNodeCommandHelper::SetOutOfParent(context, nodeId, outOfParent);
    EXPECT_EQ(1, nodeId);
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
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    EXPECT_EQ(0, inNodeId);

    inNodeId = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    EXPECT_EQ(1, inNodeId);

    outNodeId = 1;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    EXPECT_EQ(1, inNodeId);

    inNodeId = 0;
    RSNodeCommandHelper::RegisterGeometryTransitionPair(context, inNodeId, outNodeId);
    EXPECT_EQ(0, inNodeId);
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
    EXPECT_EQ(0, inNodeId);
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
    NodeId nodeId = 0;
    auto func = [] (NodeId, pid_t, uint32_t) {};
    RSNodeCommandHelper::SetDumpNodeTreeProcessor(func);
    RSNodeCommandHelper::DumpClientNodeTree(context, nodeId, 0, 0);
    RSNodeCommandHelper::SetDumpNodeTreeProcessor(nullptr);
    RSNodeCommandHelper::DumpClientNodeTree(context, nodeId, 0, 0);
    EXPECT_NE(nodeId, -3);
    SUCCEED();
}
} // namespace OHOS::Rosen
