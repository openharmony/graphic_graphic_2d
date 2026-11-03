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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSContentStyleModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: RSContentStyleModifierTest
 * @tc.desc: Test Set/Get functions of RSContentStyleModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, RSContentStyleModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSContentStyleModifier> modifier =
        std::make_shared<ModifierNG::RSContentStyleModifier>();
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::CONTENT_STYLE);
    EXPECT_EQ(modifier->GetInnerPropertyType(), ModifierNG::RSPropertyType::CONTENT_STYLE);
}

#ifdef RS_MODIFIERS_DRAW_ENABLE
/**
 * @tc.name: FlushContentModifierImmediatelyTest001
 * @tc.desc: Test FlushContentModifierImmediately with expired node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<RSContentStyleModifier>();
    modifier->FlushContentModifierImmediately();
    EXPECT_EQ(modifier->node_.lock(), nullptr);

    auto modifier2 = std::make_shared<RSContentStyleModifier>();
    auto node = std::make_shared<RSNode>(0);
    modifier2->node_ = node;
    modifier2->FlushContentModifierImmediately();
    EXPECT_EQ(modifier2->node_.lock()->GetRSUIContext(), nullptr);
}

/**
 * @tc.name: RenderInClientTest001
 * @tc.desc: Test RenderInClient with nullptr node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, RenderInClientTest001, TestSize.Level1)
{
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    bool ret = modifier->RenderInClient(drawCmdList, nullptr);
    EXPECT_FALSE(ret);
}
 
/**
 * @tc.name: RenderInClientTest002
 * @tc.desc: Test RenderInClient with valid node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, RenderInClientTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingNode>(0);
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    bool ret = modifier->RenderInClient(drawCmdList, node);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch001
 * @tc.desc: Test FlushContentModifierImmediately with null node (node_ never set)
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch001, TestSize.Level1)
{
    // Branch: node_.lock() == nullptr -> return.
    auto modifier = std::make_shared<RSContentStyleModifier>();
    EXPECT_EQ(modifier->node_.lock(), nullptr);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch002
 * @tc.desc: Test FlushContentModifierImmediately with non-canvas-drawing node type
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch002, TestSize.Level1)
{
    // Branch: node->GetType() != RSUINodeType::CANVAS_DRAWING_NODE -> return.
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto node = std::make_shared<RSNode>(false);
    modifier->node_ = node;
    EXPECT_NE(modifier->node_.lock(), nullptr);
    EXPECT_NE(node->GetType(), RSUINodeType::CANVAS_DRAWING_NODE);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
    EXPECT_EQ(node->GetRSUIContext(), nullptr);
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch003
 * @tc.desc: Test FlushContentModifierImmediately with expired weak_ptr node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch003, TestSize.Level1)
{
    // Branch: node_.lock() == nullptr after node destruction.
    auto modifier = std::make_shared<RSContentStyleModifier>();
    {
        auto node = std::make_shared<RSCanvasDrawingNode>(false);
        modifier->node_ = node;
        ASSERT_NE(modifier->node_.lock(), nullptr);
    }
    // node destroyed, weak_ptr expired
    EXPECT_EQ(modifier->node_.lock(), nullptr);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch004
 * @tc.desc: Test FlushContentModifierImmediately with canvas drawing node and no RSUIContext
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch004, TestSize.Level1)
{
    // Branch: canvas drawing node, GetRSUIContext() == nullptr -> skip OnCanvasDrawingNodeUpdate.
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto node = RSCanvasDrawingNode::Create(false, false, nullptr);
    ASSERT_NE(node, nullptr);
    modifier->node_ = node;
    EXPECT_EQ(node->GetType(), RSUINodeType::CANVAS_DRAWING_NODE);
    EXPECT_EQ(node->GetRSUIContext(), nullptr);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch005
 * @tc.desc: Test FlushContentModifierImmediately with canvas drawing node and RSUIContext
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch005, TestSize.Level1)
{
    // Branch: canvas drawing node, GetRSUIContext() != nullptr -> OnCanvasDrawingNodeUpdate().
    constexpr uint64_t token = 200;
    auto rsUIContext = std::make_shared<RSUIContext>(token);
    ASSERT_NE(rsUIContext, nullptr);
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto node = RSCanvasDrawingNode::Create(false, false, rsUIContext);
    ASSERT_NE(node, nullptr);
    modifier->node_ = node;
    EXPECT_EQ(node->GetType(), RSUINodeType::CANVAS_DRAWING_NODE);
    ASSERT_NE(node->GetRSUIContext(), nullptr);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch006
 * @tc.desc: Test FlushContentModifierImmediately with render service canvas drawing node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch006, TestSize.Level1)
{
    // Branch: isRenderServiceNode=true canvas drawing node, no RSUIContext.
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto node = RSCanvasDrawingNode::Create(true, false, nullptr);
    ASSERT_NE(node, nullptr);
    modifier->node_ = node;
    EXPECT_EQ(node->GetType(), RSUINodeType::CANVAS_DRAWING_NODE);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch007
 * @tc.desc: Test FlushContentModifierImmediately with texture export canvas drawing node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch007, TestSize.Level1)
{
    // Branch: isTextureExportNode=true canvas drawing node with RSUIContext.
    constexpr uint64_t token = 201;
    auto rsUIContext = std::make_shared<RSUIContext>(token);
    ASSERT_NE(rsUIContext, nullptr);
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto node = RSCanvasDrawingNode::Create(false, true, rsUIContext);
    ASSERT_NE(node, nullptr);
    modifier->node_ = node;
    EXPECT_EQ(node->GetType(), RSUINodeType::CANVAS_DRAWING_NODE);
    ASSERT_NE(node->GetRSUIContext(), nullptr);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch008
 * @tc.desc: Test FlushContentModifierImmediately repeated calls don't crash
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch008, TestSize.Level1)
{
    // Branch: repeated calls on same canvas drawing node with RSUIContext.
    constexpr uint64_t token = 202;
    auto rsUIContext = std::make_shared<RSUIContext>(token);
    ASSERT_NE(rsUIContext, nullptr);
    auto modifier = std::make_shared<RSContentStyleModifier>();
    auto node = RSCanvasDrawingNode::Create(false, false, rsUIContext);
    ASSERT_NE(node, nullptr);
    modifier->node_ = node;
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch009
 * @tc.desc: Test FlushContentModifierImmediately with multiple modifiers on same node
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch009, TestSize.Level1)
{
    // Branch: two modifiers share the same canvas drawing node, both flush independently.
    constexpr uint64_t token = 203;
    auto rsUIContext = std::make_shared<RSUIContext>(token);
    ASSERT_NE(rsUIContext, nullptr);
    auto node = RSCanvasDrawingNode::Create(false, false, rsUIContext);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(node->GetType(), RSUINodeType::CANVAS_DRAWING_NODE);
    ASSERT_NE(node->GetRSUIContext(), nullptr);

    auto modifier1 = std::make_shared<RSContentStyleModifier>();
    modifier1->node_ = node;
    auto modifier2 = std::make_shared<RSContentStyleModifier>();
    modifier2->node_ = node;

    EXPECT_NO_FATAL_FAILURE(modifier1->FlushContentModifierImmediately());
    EXPECT_NO_FATAL_FAILURE(modifier2->FlushContentModifierImmediately());

    // Swap node to a non-canvas node and verify both modifiers hit the type-check early return.
    auto plainNode = std::make_shared<RSNode>(false);
    ASSERT_NE(plainNode->GetType(), RSUINodeType::CANVAS_DRAWING_NODE);
    modifier1->node_ = plainNode;
    modifier2->node_ = plainNode;
    EXPECT_NO_FATAL_FAILURE(modifier1->FlushContentModifierImmediately());
    EXPECT_NO_FATAL_FAILURE(modifier2->FlushContentModifierImmediately());
}

/**
 * @tc.name: FlushContentModifierImmediatelyBranch010
 * @tc.desc: Test FlushContentModifierImmediately after node context cleared
 * @tc.type: FUNC
 */
HWTEST_F(RSContentStyleModifierNGTypeTest, FlushContentModifierImmediatelyBranch010, TestSize.Level1)
{
    // Branch: canvas drawing node whose rsUIContext_ is manually reset to nullptr.
    auto modifier = std::make_shared<RSContentStyleModifier>();
    constexpr uint64_t token = 204;
    auto rsUIContext = std::make_shared<RSUIContext>(token);
    ASSERT_NE(rsUIContext, nullptr);
    auto node = RSCanvasDrawingNode::Create(false, false, rsUIContext);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->GetRSUIContext(), nullptr);
    modifier->node_ = node;

    // First flush with context present -> OnCanvasDrawingNodeUpdate path.
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());

    // Clear context and flush again -> skip OnCanvasDrawingNodeUpdate path.
    node->rsUIContext_ = nullptr;
    EXPECT_EQ(node->GetRSUIContext(), nullptr);
    EXPECT_NO_FATAL_FAILURE(modifier->FlushContentModifierImmediately());
}
#endif
} // namespace OHOS::Rosen