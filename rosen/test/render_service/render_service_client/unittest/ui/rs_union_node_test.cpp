/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_union_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUnionNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUnionNodeTest::SetUpTestCase() {}
void RSUnionNodeTest::TearDownTestCase() {}
void RSUnionNodeTest::SetUp() {}
void RSUnionNodeTest::TearDown() {}

/**
 * @tc.name: CreateTest001
 * @tc.desc: Test whether the Create function can correctly return a node when rsUIContext is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeTest, CreateTest001, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    bool isTextureExportNode = false;
    std::shared_ptr<RSUIContext> rsUIContext = nullptr;

    auto result = RSUnionNode::Create(isRenderServiceNode, isTextureExportNode, rsUIContext);

    EXPECT_NE(result, nullptr);
    EXPECT_TRUE(result->IsRenderServiceNode());
    EXPECT_FALSE(result->IsTextureExportNode());
}

/**
 * @tc.name: CreateTest002
 * @tc.desc: Test whether the Create function can correctly return a node when rsUIContext is not null.
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeTest, CreateTest002, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    bool isTextureExportNode = true;
    auto rsUIContext = std::make_shared<RSUIContext>();

    auto result = RSUnionNode::Create(isRenderServiceNode, isTextureExportNode, rsUIContext);

    EXPECT_NE(result, nullptr);
    EXPECT_FALSE(result->IsRenderServiceNode());
    EXPECT_TRUE(result->IsTextureExportNode());
}

/**
 * @tc.name: CreateTest003
 * @tc.desc: Test whether the Create function can correctly add commands when the transaction is empty.
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeTest, CreateTest003, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    bool isTextureExportNode = false;
    auto rsUIContext = std::make_shared<RSUIContext>();

    auto result = RSUnionNode::Create(isRenderServiceNode, isTextureExportNode, rsUIContext);

    EXPECT_NE(result, nullptr);
    EXPECT_TRUE(result->IsRenderServiceNode());
    EXPECT_FALSE(result->IsTextureExportNode());
}

/**
 * @tc.name: CreateTest004
 * @tc.desc: Test whether the Create function can correctly add commands when the transaction is not empty.
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeTest, CreateTest004, TestSize.Level1)
{
    bool isRenderServiceNode = false;
    bool isTextureExportNode = true;
    auto rsUIContext = std::make_shared<RSUIContext>();

    auto result = RSUnionNode::Create(isRenderServiceNode, isTextureExportNode, rsUIContext);

    EXPECT_NE(result, nullptr);
    EXPECT_FALSE(result->IsRenderServiceNode());
    EXPECT_TRUE(result->IsTextureExportNode());
}

/**
 * @tc.name: SetUnionSpacingTest
 * @tc.desc: test results of RSUnionNode::SetUnionSpacing
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeTest, SetUnionSpacingTest, TestSize.Level1)
{
    auto modifierType = ModifierNG::RSModifierType::BOUNDS;
    auto rsUnionNode = RSUnionNode::Create();

    EXPECT_EQ(rsUnionNode->GetModifierCreatedBySetter(modifierType), nullptr);

    float spacing = 0.5;
    rsUnionNode->SetUnionSpacing(spacing);
    auto& properties = rsUnionNode->GetModifierCreatedBySetter(modifierType)->properties_;

    EXPECT_NE(rsUnionNode->GetModifierCreatedBySetter(modifierType), nullptr);
    EXPECT_NE(properties.find(ModifierNG::RSPropertyType::UNION_SPACING), properties.end());
}

/**
 * @tc.name: RegisterNodeMap001
 * @tc.desc: test results of RegisterNodeMap
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionNodeTest, RegisterNodeMap001, TestSize.Level1)
{
    bool isRenderServiceNode = true;
    bool isTextureExportNode = false;
    std::shared_ptr<RSUIContext> rsUIContext = nullptr;
    auto node = RSUnionNode::Create(isRenderServiceNode, isTextureExportNode, rsUIContext);
    EXPECT_NE(node, nullptr);
    node->id_ = 1;
    node->RegisterNodeMap();

    auto rsUIContext1 = std::make_shared<RSUIContext>();
    node->rsUIContext_ = rsUIContext1;
    node->RegisterNodeMap();
    ASSERT_NE(rsUIContext1->GetMutableNodeMap().GetNode(1), nullptr);
}
} // namespace OHOS::Rosen
