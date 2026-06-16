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

#include <memory>
#include <string>

#include "gtest/gtest.h"

#include "command_modifier/rs_node_command_modifier.h"
#include "command_modifier/rs_canvas_node_command_modifier.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSNodeDumpTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeDumpTest::SetUpTestCase() {}
void RSNodeDumpTest::TearDownTestCase() {}
void RSNodeDumpTest::SetUp() {}
void RSNodeDumpTest::TearDown() {}

class TestableRSNode : public RSNode {
public:
    static std::shared_ptr<TestableRSNode> Create(bool isRenderServiceNode = false)
    {
        return std::make_shared<TestableRSNode>(isRenderServiceNode);
    }

    static std::shared_ptr<TestableRSNode> CreateWithId(NodeId id, bool isRenderServiceNode = false)
    {
        return std::make_shared<TestableRSNode>(isRenderServiceNode, id);
    }

    explicit TestableRSNode(bool isRenderServiceNode) : RSNode(isRenderServiceNode) {}
    explicit TestableRSNode(bool isRenderServiceNode, NodeId id) : RSNode(isRenderServiceNode, id) {}
};

/**
 * @tc.name: GetNodeStateDefault
 * @tc.desc: Test default node state is ACTIVE
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, GetNodeStateDefault, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetNodeState(), RSNodeState::ACTIVE);
}

/**
 * @tc.name: SetNodeStateSameState
 * @tc.desc: Test SetNodeState with same state returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, SetNodeStateSameState, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->SetNodeState(RSNodeState::ACTIVE));
    EXPECT_EQ(node->GetNodeState(), RSNodeState::ACTIVE);
}

/**
 * @tc.name: SetNodeStateActiveToInactive
 * @tc.desc: Test valid state transition from ACTIVE to INACTIVE
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, SetNodeStateActiveToInactive, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->SetNodeState(RSNodeState::INACTIVE));
    EXPECT_EQ(node->GetNodeState(), RSNodeState::INACTIVE);
}

/**
 * @tc.name: SetNodeStateInactiveToActive
 * @tc.desc: Test valid state transition from INACTIVE to ACTIVE
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, SetNodeStateInactiveToActive, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->SetNodeState(RSNodeState::INACTIVE);
    EXPECT_TRUE(node->SetNodeState(RSNodeState::ACTIVE));
    EXPECT_EQ(node->GetNodeState(), RSNodeState::ACTIVE);
}

/**
 * @tc.name: SetNodeStateLazyLoadToActive
 * @tc.desc: Test valid state transition from LAZY_LOAD to ACTIVE
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, SetNodeStateLazyLoadToActive, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->nodeState_ = RSNodeState::LAZY_LOAD;
    EXPECT_TRUE(node->SetNodeState(RSNodeState::ACTIVE));
    EXPECT_EQ(node->GetNodeState(), RSNodeState::ACTIVE);
}

/**
 * @tc.name: SetNodeStateActiveToLazyLoadInvalid
 * @tc.desc: Test invalid state transition from ACTIVE to LAZY_LOAD
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, SetNodeStateActiveToLazyLoadInvalid, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    EXPECT_FALSE(node->SetNodeState(RSNodeState::LAZY_LOAD));
    EXPECT_EQ(node->GetNodeState(), RSNodeState::ACTIVE);
}

/**
 * @tc.name: SetNodeStateInactiveToLazyLoadInvalid
 * @tc.desc: Test invalid state transition from INACTIVE to LAZY_LOAD
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, SetNodeStateInactiveToLazyLoadInvalid, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->SetNodeState(RSNodeState::INACTIVE);
    EXPECT_FALSE(node->SetNodeState(RSNodeState::LAZY_LOAD));
    EXPECT_EQ(node->GetNodeState(), RSNodeState::INACTIVE);
}

/**
 * @tc.name: SetNodeStateLazyLoadToInactiveInvalid
 * @tc.desc: Test invalid state transition from LAZY_LOAD to INACTIVE
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, SetNodeStateLazyLoadToInactiveInvalid, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->nodeState_ = RSNodeState::LAZY_LOAD;
    EXPECT_FALSE(node->SetNodeState(RSNodeState::INACTIVE));
    EXPECT_EQ(node->GetNodeState(), RSNodeState::LAZY_LOAD);
}

/**
 * @tc.name: GetRSCmdModifierEmpty
 * @tc.desc: Test GetRSCmdModifier returns nullptr when no modifiers exist
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, GetRSCmdModifierEmpty, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    auto result = node->GetRSCmdModifier(RSCmdModifierType::NODE_NAME);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetRSCmdModifierFound
 * @tc.desc: Test GetRSCmdModifier returns modifier when found
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, GetRSCmdModifierFound, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    auto modifier = std::make_shared<NodeNameCmdModifier>(node, NodeNameCmdParam{"test"});
    node->rsCmdModifiers_.emplace(RSCmdModifierType::NODE_NAME, modifier);
    auto result = node->GetRSCmdModifier(RSCmdModifierType::NODE_NAME);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetType(), RSCmdModifierType::NODE_NAME);
}

/**
 * @tc.name: GetRSCmdModifierNotFoundWithOtherEntry
 * @tc.desc: Test GetRSCmdModifier returns nullptr when searching different type
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, GetRSCmdModifierNotFoundWithOtherEntry, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    auto modifier = std::make_shared<NodeNameCmdModifier>(node, NodeNameCmdParam{"test"});
    node->rsCmdModifiers_.emplace(RSCmdModifierType::NODE_NAME, modifier);
    auto result = node->GetRSCmdModifier(RSCmdModifierType::IS_P3COLOR);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: DumpRSCmdModifiersEmpty
 * @tc.desc: Test DumpRSCmdModifiers output when no modifiers exist
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpRSCmdModifiersEmpty, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    std::string out;
    node->DumpRSCmdModifiers(out);
    EXPECT_EQ(out, "RSCmdModifiers: [empty]");
}

/**
 * @tc.name: DumpRSCmdModifiersWithNullModifier
 * @tc.desc: Test DumpRSCmdModifiers output with null modifier entry
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpRSCmdModifiersWithNullModifier, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->rsCmdModifiers_.emplace(RSCmdModifierType::NODE_NAME, nullptr);
    std::string out;
    node->DumpRSCmdModifiers(out);
    EXPECT_NE(out.find("RSCmdModifiers:"), std::string::npos);
}

/**
 * @tc.name: DumpRSCmdModifiersWithSingleModifier
 * @tc.desc: Test DumpRSCmdModifiers output with a single modifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpRSCmdModifiersWithSingleModifier, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    auto modifier = std::make_shared<NodeNameCmdModifier>(node, NodeNameCmdParam{"testNode"});
    node->rsCmdModifiers_.emplace(RSCmdModifierType::NODE_NAME, modifier);
    std::string out;
    node->DumpRSCmdModifiers(out);
    EXPECT_NE(out.find("RSCmdModifiers: ["), std::string::npos);
    EXPECT_NE(out.find("type=NODE_NAME"), std::string::npos);
    EXPECT_NE(out.find("param="), std::string::npos);
}

/**
 * @tc.name: DumpRSCmdModifiersWithMultipleModifiers
 * @tc.desc: Test DumpRSCmdModifiers output with multiple modifiers
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpRSCmdModifiersWithMultipleModifiers, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    auto modifier1 = std::make_shared<NodeNameCmdModifier>(node, NodeNameCmdParam{"node1"});
    auto modifier2 = std::make_shared<IsP3ColorCmdModifier>(node, IsP3ColorCmdParam{1});
    node->rsCmdModifiers_.emplace(RSCmdModifierType::NODE_NAME, modifier1);
    node->rsCmdModifiers_.emplace(RSCmdModifierType::IS_P3COLOR, modifier2);
    std::string out;
    node->DumpRSCmdModifiers(out);
    EXPECT_NE(out.find("RSCmdModifiers: ["), std::string::npos);
    EXPECT_NE(out.find("type=NODE_NAME"), std::string::npos);
    EXPECT_NE(out.find("type=IS_P3COLOR"), std::string::npos);
    EXPECT_NE(out.find(", "), std::string::npos);
}

/**
 * @tc.name: DumpRSCmdModifiersWithNullModifierSkipped
 * @tc.desc: Test DumpRSCmdModifiers skips null modifiers in mixed case
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpRSCmdModifiersWithNullModifierSkipped, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    auto modifier = std::make_shared<NodeNameCmdModifier>(node, NodeNameCmdParam{"testNode"});
    node->rsCmdModifiers_.emplace(RSCmdModifierType::NODE_NAME, nullptr);
    node->rsCmdModifiers_.emplace(RSCmdModifierType::IS_P3COLOR, modifier);
    std::string out;
    node->DumpRSCmdModifiers(out);
    EXPECT_NE(out.find("type=IS_P3COLOR"), std::string::npos);
    size_t nodeNamePos = out.find("type=NODE_NAME");
    size_t firstComma = out.find(", type=");
    if (firstComma != std::string::npos) {
        EXPECT_NE(nodeNamePos, firstComma + 2);
    }
}

/**
 * @tc.name: DumpModifiersEmpty
 * @tc.desc: Test DumpModifiers output when no modifiers exist
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpModifiersEmpty, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    std::string out;
    node->DumpModifiers(out);
    EXPECT_TRUE(out.empty());
}

/**
 * @tc.name: DumpNodeStateActive
 * @tc.desc: Test DumpNodeState returns ACTIVE string
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpNodeStateActive, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->DumpNodeState(), "ACTIVE");
}

/**
 * @tc.name: DumpNodeStateInactive
 * @tc.desc: Test DumpNodeState returns INACTIVE string
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpNodeStateInactive, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->nodeState_ = RSNodeState::INACTIVE;
    EXPECT_EQ(node->DumpNodeState(), "INACTIVE");
}

/**
 * @tc.name: DumpNodeStateLazyLoad
 * @tc.desc: Test DumpNodeState returns LAZY_LOAD string
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpNodeStateLazyLoad, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->nodeState_ = RSNodeState::LAZY_LOAD;
    EXPECT_EQ(node->DumpNodeState(), "LAZY_LOAD");
}

/**
 * @tc.name: DumpRSCmdModifiersWithCanvasNodeModifier
 * @tc.desc: Test DumpRSCmdModifiers with canvas node modifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpRSCmdModifiersWithCanvasNodeModifier, TestSize.Level1)
{
    auto canvasNode = RSCanvasNode::Create();
    ASSERT_NE(canvasNode, nullptr);
    auto modifier = std::make_shared<HdrPresentCmdModifier>(
        canvasNode, HdrPresentCmdParam{true});
    canvasNode->rsCmdModifiers_.emplace(RSCmdModifierType::HDR_PRESENT, modifier);
    std::string out;
    canvasNode->DumpRSCmdModifiers(out);
    EXPECT_NE(out.find("RSCmdModifiers: ["), std::string::npos);
    EXPECT_NE(out.find("type=HDR_PRESENT"), std::string::npos);
}

/**
 * @tc.name: DumpRSCmdModifiersAllNullModifiers
 * @tc.desc: Test DumpRSCmdModifiers when all modifier entries are null
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeDumpTest, DumpRSCmdModifiersAllNullModifiers, TestSize.Level1)
{
    auto node = TestableRSNode::Create();
    ASSERT_NE(node, nullptr);
    node->rsCmdModifiers_.emplace(RSCmdModifierType::NODE_NAME, nullptr);
    node->rsCmdModifiers_.emplace(RSCmdModifierType::IS_P3COLOR, nullptr);
    std::string out;
    node->DumpRSCmdModifiers(out);
    EXPECT_NE(out.find("RSCmdModifiers: ["), std::string::npos);
    EXPECT_EQ(out.find("type="), std::string::npos);
}

} // namespace OHOS::Rosen
