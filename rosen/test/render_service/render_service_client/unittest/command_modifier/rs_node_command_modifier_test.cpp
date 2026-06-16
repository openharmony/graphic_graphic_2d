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
#include "gtest/hwext/gtest-tag.h"

#include "command_modifier/rs_node_command_modifier.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSNodeCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeCommandModifierTest::SetUpTestCase() {}
void RSNodeCommandModifierTest::TearDownTestCase() {}
void RSNodeCommandModifierTest::SetUp() {}
void RSNodeCommandModifierTest::TearDown() {}

/**
 * @tc.name: OcclusionCullingStatusTest001
 * @tc.desc: Test GetType and SetParam same/different + GetParam + DumpParam
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, OcclusionCullingStatusTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    OcclusionCullingStatusCmdParam param{true, 100};
    auto mod = std::make_shared<OcclusionCullingStatusCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::OCCLUSION_CULLING_STATUS);
    EXPECT_EQ(mod->GetParam().enablekeyOcclusion_, true);
    EXPECT_EQ(mod->GetParam().keyOcclusionNodeId_, 100);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().keyOcclusionNodeId_, 100);

    OcclusionCullingStatusCmdParam param2{false, 200};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().enablekeyOcclusion_, false);
    EXPECT_EQ(mod->GetParam().keyOcclusionNodeId_, 200);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("enablekeyOcclusion"), std::string::npos);
    EXPECT_NE(out.find("200"), std::string::npos);
}

/**
 * @tc.name: OcclusionCullingStatusTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, OcclusionCullingStatusTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    OcclusionCullingStatusCmdParam param{true, 100};
    auto mod = std::make_shared<OcclusionCullingStatusCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: NodeNameTest001
 * @tc.desc: Test all methods of NodeNameCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, NodeNameTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    NodeNameCmdParam param{"testNode"};
    auto mod = std::make_shared<NodeNameCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::NODE_NAME);
    EXPECT_EQ(mod->GetParam().nodeName_, "testNode");

    NodeNameCmdParam param2{"otherNode"};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().nodeName_, "otherNode");

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("otherNode"), std::string::npos);
}

/**
 * @tc.name: NodeNameTest002
 * @tc.desc: Test NodeNameCmdModifier UpdateToRender with null
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, NodeNameTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    NodeNameCmdParam param{"test"};
    auto mod = std::make_shared<NodeNameCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsP3ColorTest001
 * @tc.desc: Test all methods of IsP3ColorCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsP3ColorTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsP3ColorCmdParam param{1};
    auto mod = std::make_shared<IsP3ColorCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_P3COLOR);
    EXPECT_EQ(mod->GetParam().collectColorSpace_, 1);

    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().collectColorSpace_, 1);

    IsP3ColorCmdParam param2{2};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().collectColorSpace_, 2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("collectColorSpace"), std::string::npos);
}

/**
 * @tc.name: IsP3ColorTest002
 * @tc.desc: Test IsP3ColorCmdModifier UpdateToRender with null
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsP3ColorTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    IsP3ColorCmdParam param{0};
    auto mod = std::make_shared<IsP3ColorCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DrawRegionTest001
 * @tc.desc: Test DrawRegionCmdModifier with valid and null drawRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, DrawRegionTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawRegionCmdParam param{std::make_shared<RectF>(0.f, 0.f, 100.f, 100.f)};
    auto mod = std::make_shared<DrawRegionCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::DRAW_REGION);
    ASSERT_TRUE(mod->GetParam().drawRegion_);
    EXPECT_EQ(mod->GetParam().drawRegion_->width_, 100.f);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("drawRegion"), std::string::npos);
}

/**
 * @tc.name: DrawRegionTest002
 * @tc.desc: Test DrawRegionCmdModifier DumpParam with null drawRegion
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, DrawRegionTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawRegionCmdParam param{nullptr};
    auto mod = std::make_shared<DrawRegionCmdModifier>(node, param);
    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("null"), std::string::npos);
}

/**
 * @tc.name: DrawRegionTest003
 * @tc.desc: Test DrawRegionCmdModifier UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, DrawRegionTest003, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    DrawRegionCmdParam param{std::make_shared<RectF>(0, 0, 10, 10)};
    auto mod = std::make_shared<DrawRegionCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: UseCmdlistDrawRegionTest001
 * @tc.desc: Test all methods of UseCmdlistDrawRegionCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, UseCmdlistDrawRegionTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    UseCmdlistDrawRegionCmdParam param{true};
    auto mod = std::make_shared<UseCmdlistDrawRegionCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::USE_CMDLIST_DRAWREGION);
    EXPECT_TRUE(mod->GetParam().needUseCmdlistDrawRegion_);

    UseCmdlistDrawRegionCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().needUseCmdlistDrawRegion_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("needUseCmdlistDrawRegion"), std::string::npos);
}

/**
 * @tc.name: UseCmdlistDrawRegionTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, UseCmdlistDrawRegionTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    UseCmdlistDrawRegionCmdParam param{false};
    auto mod = std::make_shared<UseCmdlistDrawRegionCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ExcludeNodeGroupTest001
 * @tc.desc: Test all methods of ExcludeNodeGroupCmdModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, ExcludeNodeGroupTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ExcludeNodeGroupCmdParam param{true};
    auto mod = std::make_shared<ExcludeNodeGroupCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::EXCLUDE_NODE_GROUP);
    EXPECT_TRUE(mod->GetParam().isExcludedFromNodeGroup_);

    ExcludeNodeGroupCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isExcludedFromNodeGroup_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isExcludedFromNodeGroup"), std::string::npos);
}

/**
 * @tc.name: ExcludeNodeGroupTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, ExcludeNodeGroupTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    ExcludeNodeGroupCmdParam param{true};
    auto mod = std::make_shared<ExcludeNodeGroupCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: MarkNodeSingleFrameComposerTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, MarkNodeSingleFrameComposerTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    MarkNodeSingleFrameComposerCmdParam param{true, 1234};
    auto mod = std::make_shared<MarkNodeSingleFrameComposerCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::MARK_NODE_SINGLE_FRAME_COMPOSER);
    EXPECT_TRUE(mod->GetParam().isNodeSingleFrameComposer_);
    EXPECT_EQ(mod->GetParam().realPid_, 1234);

    MarkNodeSingleFrameComposerCmdParam param2{false, 5678};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isNodeSingleFrameComposer_);
    EXPECT_EQ(mod->GetParam().realPid_, 5678);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isNodeSingleFrameComposer"), std::string::npos);
    EXPECT_NE(out.find("5678"), std::string::npos);
}

/**
 * @tc.name: MarkNodeSingleFrameComposerTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, MarkNodeSingleFrameComposerTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    MarkNodeSingleFrameComposerCmdParam param{true, 0};
    auto mod = std::make_shared<MarkNodeSingleFrameComposerCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsRepaintBoundaryTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsRepaintBoundaryTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsRepaintBoundaryCmdParam param{true};
    auto mod = std::make_shared<IsRepaintBoundaryCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_REPAINT_BOUNDARY);
    EXPECT_TRUE(mod->GetParam().isRepaintBoundary_);

    IsRepaintBoundaryCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isRepaintBoundary_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isRepaintBoundary"), std::string::npos);
}

/**
 * @tc.name: IsRepaintBoundaryTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsRepaintBoundaryTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    IsRepaintBoundaryCmdParam param{true};
    auto mod = std::make_shared<IsRepaintBoundaryCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: MarkOpincNodeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, MarkOpincNodeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    MarkOpincNodeCmdParam param{true, false};
    auto mod = std::make_shared<MarkOpincNodeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::MARK_OPINC_NODE);
    EXPECT_TRUE(mod->GetParam().isSuggestOpincNode_);
    EXPECT_FALSE(mod->GetParam().isOpincNeedCalculate_);

    MarkOpincNodeCmdParam param2{false, true};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isSuggestOpincNode_);
    EXPECT_TRUE(mod->GetParam().isOpincNeedCalculate_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isSuggestOpincNode"), std::string::npos);
}

/**
 * @tc.name: MarkOpincNodeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, MarkOpincNodeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    MarkOpincNodeCmdParam param{true, true};
    auto mod = std::make_shared<MarkOpincNodeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsUifirstNodeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsUifirstNodeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsUifirstNodeCmdParam param{true};
    auto mod = std::make_shared<IsUifirstNodeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_UIFIRST_NODE);
    EXPECT_TRUE(mod->GetParam().isUifirstNode_);

    IsUifirstNodeCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isUifirstNode_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isUifirstNode"), std::string::npos);
}

/**
 * @tc.name: IsUifirstNodeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsUifirstNodeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    IsUifirstNodeCmdParam param{true};
    auto mod = std::make_shared<IsUifirstNodeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsForceUifirstNodeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsForceUifirstNodeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsForceUifirstNodeCmdParam param{true, false};
    auto mod = std::make_shared<IsForceUifirstNodeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_FORCE_UIFIRST_NODE);
    EXPECT_TRUE(mod->GetParam().isForceFlag_);
    EXPECT_FALSE(mod->GetParam().isUifirstEnable_);

    IsForceUifirstNodeCmdParam param2{false, true};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isForceFlag_);
    EXPECT_TRUE(mod->GetParam().isUifirstEnable_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isForceFlag"), std::string::npos);
}

/**
 * @tc.name: IsForceUifirstNodeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsForceUifirstNodeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    IsForceUifirstNodeCmdParam param{true, true};
    auto mod = std::make_shared<IsForceUifirstNodeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SyncDrawNodeTypeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, SyncDrawNodeTypeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    SyncDrawNodeTypeCmdParam param{DrawNodeType::PureContainerType};
    auto mod = std::make_shared<SyncDrawNodeTypeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SYNC_DRAW_NODE_TYPE);
    EXPECT_EQ(mod->GetParam().nodeType_, DrawNodeType::PureContainerType);

    SyncDrawNodeTypeCmdParam param2{DrawNodeType::MergeableType};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().nodeType_, DrawNodeType::MergeableType);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("nodeType"), std::string::npos);
}

/**
 * @tc.name: SyncDrawNodeTypeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, SyncDrawNodeTypeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    SyncDrawNodeTypeCmdParam param{DrawNodeType::PureContainerType};
    auto mod = std::make_shared<SyncDrawNodeTypeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: UIFirstSwitchTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, UIFirstSwitchTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    UIFirstSwitchCmdParam param{RSUIFirstSwitch::FORCE_ENABLE};
    auto mod = std::make_shared<UIFirstSwitchCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::UI_FIRST_SWITCH);
    EXPECT_EQ(mod->GetParam().uifirstSwitch_, RSUIFirstSwitch::FORCE_ENABLE);

    UIFirstSwitchCmdParam param2{RSUIFirstSwitch::NONE};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().uifirstSwitch_, RSUIFirstSwitch::NONE);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("uifirstSwitch"), std::string::npos);
}

/**
 * @tc.name: UIFirstSwitchTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, UIFirstSwitchTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    UIFirstSwitchCmdParam param{RSUIFirstSwitch::NONE};
    auto mod = std::make_shared<UIFirstSwitchCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: OutOfParentTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, OutOfParentTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    OutOfParentCmdParam param{OutOfParentType::OUTSIDE};
    auto mod = std::make_shared<OutOfParentCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::OUT_OF_PARENT);
    EXPECT_EQ(mod->GetParam().outOfParent_, OutOfParentType::OUTSIDE);

    OutOfParentCmdParam param2{OutOfParentType::WITHIN};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().outOfParent_, OutOfParentType::WITHIN);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("outOfParent"), std::string::npos);
}

/**
 * @tc.name: OutOfParentTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, OutOfParentTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    OutOfParentCmdParam param{OutOfParentType::WITHIN};
    auto mod = std::make_shared<OutOfParentCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsCrossNodeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsCrossNodeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsCrossNodeCmdParam param{true};
    auto mod = std::make_shared<IsCrossNodeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_CROSS_NODE);
    EXPECT_TRUE(mod->GetParam().isCrossNode_);

    IsCrossNodeCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isCrossNode_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isCrossNode"), std::string::npos);
}

/**
 * @tc.name: IsCrossNodeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsCrossNodeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    IsCrossNodeCmdParam param{true};
    auto mod = std::make_shared<IsCrossNodeCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: NodeGroupTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, NodeGroupTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    NodeGroupCmdParam param{true, false, true};
    auto mod = std::make_shared<NodeGroupCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::MARK_NODE_GROUP);
    EXPECT_TRUE(mod->GetParam().isNodeGroup_);
    EXPECT_FALSE(mod->GetParam().nodeGroupIsForced_);
    EXPECT_TRUE(mod->GetParam().nodeGroupIncludeProperty_);

    NodeGroupCmdParam param2{false, true, false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isNodeGroup_);
    EXPECT_TRUE(mod->GetParam().nodeGroupIsForced_);
    EXPECT_FALSE(mod->GetParam().nodeGroupIncludeProperty_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isNodeGroup"), std::string::npos);
}

/**
 * @tc.name: NodeGroupTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, NodeGroupTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    { auto n = RSCanvasNode::Create(); weakNode = n; }
    NodeGroupCmdParam param{true, true, true};
    auto mod = std::make_shared<NodeGroupCmdModifier>(weakNode, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

HWTEST_F(RSNodeCommandModifierTest, OcclusionCullingStatusTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    OcclusionCullingStatusCmdParam param{true, 100};
    auto mod = std::make_shared<OcclusionCullingStatusCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: NodeNameTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, NodeNameTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    NodeNameCmdParam param{"testNode"};
    auto mod = std::make_shared<NodeNameCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().nodeName_, "testNode");
    mod->UpdateToRender();
}

/**
 * @tc.name: IsP3ColorTest003
 * @tc.desc: Test UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsP3ColorTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsP3ColorCmdParam param{1};
    auto mod = std::make_shared<IsP3ColorCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DrawRegionTest004
 * @tc.desc: Test SetParam same/different and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, DrawRegionTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    DrawRegionCmdParam param{std::make_shared<RectF>(0.f, 0.f, 100.f, 100.f)};
    auto mod = std::make_shared<DrawRegionCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    DrawRegionCmdParam param2{nullptr};
    ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().drawRegion_);
    mod->UpdateToRender();
}

/**
 * @tc.name: UseCmdlistDrawRegionTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, UseCmdlistDrawRegionTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    UseCmdlistDrawRegionCmdParam param{true};
    auto mod = std::make_shared<UseCmdlistDrawRegionCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: ExcludeNodeGroupTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, ExcludeNodeGroupTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ExcludeNodeGroupCmdParam param{true};
    auto mod = std::make_shared<ExcludeNodeGroupCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: MarkNodeSingleFrameComposerTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, MarkNodeSingleFrameComposerTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    MarkNodeSingleFrameComposerCmdParam param{true, 1234};
    auto mod = std::make_shared<MarkNodeSingleFrameComposerCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsRepaintBoundaryTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsRepaintBoundaryTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsRepaintBoundaryCmdParam param{true};
    auto mod = std::make_shared<IsRepaintBoundaryCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: MarkOpincNodeTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, MarkOpincNodeTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    MarkOpincNodeCmdParam param{true, false};
    auto mod = std::make_shared<MarkOpincNodeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsUifirstNodeTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsUifirstNodeTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsUifirstNodeCmdParam param{true};
    auto mod = std::make_shared<IsUifirstNodeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsForceUifirstNodeTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsForceUifirstNodeTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsForceUifirstNodeCmdParam param{true, false};
    auto mod = std::make_shared<IsForceUifirstNodeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: SyncDrawNodeTypeTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, SyncDrawNodeTypeTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    SyncDrawNodeTypeCmdParam param{DrawNodeType::PureContainerType};
    auto mod = std::make_shared<SyncDrawNodeTypeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: UIFirstSwitchTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, UIFirstSwitchTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    UIFirstSwitchCmdParam param{RSUIFirstSwitch::FORCE_ENABLE};
    auto mod = std::make_shared<UIFirstSwitchCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: OutOfParentTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, OutOfParentTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    OutOfParentCmdParam param{OutOfParentType::OUTSIDE};
    auto mod = std::make_shared<OutOfParentCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: IsCrossNodeTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, IsCrossNodeTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    IsCrossNodeCmdParam param{true};
    auto mod = std::make_shared<IsCrossNodeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: NodeGroupTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, NodeGroupTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    NodeGroupCmdParam param{true, false, true};
    auto mod = std::make_shared<NodeGroupCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

/**
 * @tc.name: LayerPartRenderTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, LayerPartRenderTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    LayerPartRenderCmdParam param{true};
    auto mod = std::make_shared<LayerPartRenderCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::MARK_LAYER_PART_RENDER);
    EXPECT_TRUE(mod->GetParam().isLayerPartRender_);

    LayerPartRenderCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isLayerPartRender_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isLayerPartRender"), std::string::npos);
}

/**
 * @tc.name: LayerPartRenderTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, LayerPartRenderTest002, TestSize.Level1)
{
    LayerPartRenderCmdParam param{true};
    std::weak_ptr<RSNode> expired;
    {
        auto n = RSCanvasNode::Create();
        expired = n;
    }
    auto mod = std::make_shared<LayerPartRenderCmdModifier>(expired, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: LayerPartRenderTest003
 * @tc.desc: Test SetParam same param and UpdateToRender with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeCommandModifierTest, LayerPartRenderTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    LayerPartRenderCmdParam param{true};
    auto mod = std::make_shared<LayerPartRenderCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    mod->UpdateToRender();
}

} // namespace Rosen
} // namespace OHOS
