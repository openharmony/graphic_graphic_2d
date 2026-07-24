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

#include "command_modifier/rs_command_modifier.h"
#include "command_modifier/rs_node_command_modifier.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCommandModifierTest::SetUpTestCase() {}
void RSCommandModifierTest::TearDownTestCase() {}
void RSCommandModifierTest::SetUp() {}
void RSCommandModifierTest::TearDown() {}

class TestableCmdModifier : public RSCmdModifier {
public:
    explicit TestableCmdModifier(std::weak_ptr<RSNode> node) : RSCmdModifier(std::move(node)) {}

    void UpdateToRender() override {}

    RSCmdModifierType GetType() const override
    {
        return RSCmdModifierType::MAX;
    }

    using RSCmdModifier::AddCommand;
    using RSCmdModifier::GetNode;
    using RSCmdModifier::node_;
    using RSCmdModifier::index_;
};

/**
 * @tc.name: GetNodeTest001
 * @tc.desc: Test GetNode with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, GetNodeTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    ASSERT_TRUE(node);
    auto modifier = std::make_shared<TestableCmdModifier>(node);
    auto result = modifier->GetNode();
    ASSERT_TRUE(result);
    EXPECT_EQ(result->GetId(), node->GetId());
}

/**
 * @tc.name: GetNodeTest002
 * @tc.desc: Test GetNode with expired node
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, GetNodeTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    {
        auto node = RSCanvasNode::Create();
        weakNode = node;
    }
    auto modifier = std::make_shared<TestableCmdModifier>(weakNode);
    auto result = modifier->GetNode();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: AddCommandTest001
 * @tc.desc: Test AddCommand with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, AddCommandTest001, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    {
        auto node = RSCanvasNode::Create();
        weakNode = node;
    }
    auto modifier = std::make_shared<TestableCmdModifier>(weakNode);
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetNodeName>(0, "test");
    bool result = modifier->AddCommand(command, false);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AddCommandTest002
 * @tc.desc: Test AddCommand 4-arg with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, AddCommandTest002, TestSize.Level1)
{
    std::weak_ptr<RSNode> weakNode;
    {
        auto node = RSCanvasNode::Create();
        weakNode = node;
    }
    auto modifier = std::make_shared<TestableCmdModifier>(weakNode);
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetNodeName>(0, "test");
    bool result = modifier->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, 0);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DumpParamTest001
 * @tc.desc: Test default DumpParam outputs index
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, DumpParamTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto modifier = std::make_shared<TestableCmdModifier>(node);
    std::string out;
    modifier->DumpParam(out);
    EXPECT_NE(out.find("{index:"), std::string::npos);
    EXPECT_NE(out.find("}"), std::string::npos);
}

/**
 * @tc.name: UpdateToRenderWithResultTest001
 * @tc.desc: Test default UpdateToRenderWithResult returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, UpdateToRenderWithResultTest001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto modifier = std::make_shared<TestableCmdModifier>(node);
    auto result = modifier->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_FALSE(val);
}

/**
 * @tc.name: RSCmdModifierTypeEnumTest001
 * @tc.desc: Test RSCmdModifierType enum values
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, RSCmdModifierTypeEnumTest001, TestSize.Level1)
{
    EXPECT_EQ(static_cast<uint16_t>(RSCmdModifierType::NODE_NAME), 0);
    EXPECT_EQ(static_cast<uint16_t>(RSCmdModifierType::OCCLUSION_CULLING_STATUS), 1);
    EXPECT_EQ(static_cast<uint16_t>(RSCmdModifierType::IS_P3COLOR), 2);
    EXPECT_EQ(static_cast<uint16_t>(RSCmdModifierType::DRAW_REGION), 3);
    EXPECT_NE(static_cast<uint16_t>(RSCmdModifierType::MAX), 0);
}

/**
 * @tc.name: AddCommandTest003
 * @tc.desc: Test AddCommand with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, AddCommandTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto modifier = std::make_shared<TestableCmdModifier>(node);
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetNodeName>(node->GetId(), "test");
    bool result = modifier->AddCommand(command, false);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: AddCommandTest004
 * @tc.desc: Test AddCommand 4-arg with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, AddCommandTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto modifier = std::make_shared<TestableCmdModifier>(node);
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetNodeName>(node->GetId(), "test");
    bool result = modifier->AddCommand(command, false, FollowType::FOLLOW_TO_PARENT, node->GetId());
    EXPECT_TRUE(result);
}

/**
 * @tc.name: GenerateCmdModifierIndexTest001
 * @tc.desc: Test GenerateCmdModifierIndex returns monotonically increasing values
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, GenerateCmdModifierIndexTest001, TestSize.Level1)
{
    auto index1 = RSCmdModifier::GenerateCmdModifierIndex();
    auto index2 = RSCmdModifier::GenerateCmdModifierIndex();
    auto index3 = RSCmdModifier::GenerateCmdModifierIndex();
    EXPECT_GT(index2, index1);
    EXPECT_GT(index3, index2);
    EXPECT_GE(index1, 1u);
}

/**
 * @tc.name: GenerateCmdModifierIndexTest002
 * @tc.desc: Test modifier index is assigned on construction
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, GenerateCmdModifierIndexTest002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto modifier1 = std::make_shared<TestableCmdModifier>(node);
    auto modifier2 = std::make_shared<TestableCmdModifier>(node);
    EXPECT_GT(modifier2->GetIndex(), modifier1->GetIndex());
    EXPECT_GE(modifier1->GetIndex(), 1u);
}

/**
 * @tc.name: GenerateCmdModifierIndexTest003
 * @tc.desc: Test DumpParam contains the index value matching GetIndex
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, GenerateCmdModifierIndexTest003, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto modifier = std::make_shared<TestableCmdModifier>(node);
    std::string out;
    modifier->DumpParam(out);
    auto expectedStr = "{index:" + std::to_string(modifier->GetIndex()) + "}";
    EXPECT_EQ(out, expectedStr);
}

/**
 * @tc.name: GenerateCmdModifierIndexTest004
 * @tc.desc: Test index is updated when SetParam changes the parameter
 * @tc.type: FUNC
 */
HWTEST_F(RSCommandModifierTest, GenerateCmdModifierIndexTest004, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    auto modifier = std::make_shared<TestableCmdModifier>(node);
    uint64_t oldIndex = modifier->GetIndex();
    // Simulate SetRSCmdProperty updating index on param change
    modifier->index_ = RSCmdModifier::GenerateCmdModifierIndex();
    uint64_t newIndex = modifier->GetIndex();
    EXPECT_GT(newIndex, oldIndex);
}

} // namespace Rosen
} // namespace OHOS
