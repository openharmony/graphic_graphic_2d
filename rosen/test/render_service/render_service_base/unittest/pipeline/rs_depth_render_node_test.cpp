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

#include "modifier_ng/appearance/rs_depth_space_render_modifier.h"
#include "params/rs_depth_render_params.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_depth_render_node.h"
#include "property/rs_spatial_effect_manager.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "visitor/rs_node_visitor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDepthRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId testNodeId = 1;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSDepthRenderNodeTest::SetUpTestCase() {}
void RSDepthRenderNodeTest::TearDownTestCase() {}
void RSDepthRenderNodeTest::SetUp() {}
void RSDepthRenderNodeTest::TearDown() {}

class TestDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit TestDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

/**
 * @tc.name: GetType001
 * @tc.desc: Verify GetType returns DEPTH_NODE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, GetType001, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetType(), RSRenderNodeType::DEPTH_NODE);
}

/**
 * @tc.name: GetType002
 * @tc.desc: Verify static Type constant matches GetType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, GetType002, TestSize.Level1)
{
    EXPECT_EQ(RSDepthRenderNode::Type, RSRenderNodeType::DEPTH_NODE);
}

/**
 * @tc.name: SetAndGetDepthSpaceType001
 * @tc.desc: Test SetDepthSpaceType and GetDepthSpaceType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, SetAndGetDepthSpaceType001, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    EXPECT_EQ(node->GetDepthSpaceType(), DepthSpaceType::INSTANCE);
    node->SetDepthSpaceType(DepthSpaceType::GLOBAL);
    EXPECT_EQ(node->GetDepthSpaceType(), DepthSpaceType::GLOBAL);
}

/**
 * @tc.name: Prepare001
 * @tc.desc: Verify Prepare does not crash with null visitor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, Prepare001, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->Prepare(nullptr));
}

/**
 * @tc.name: Prepare002
 * @tc.desc: Verify Prepare does not crash with valid visitor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, Prepare002, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    auto visitor = std::make_shared<RSRenderThreadVisitor>();
    EXPECT_NO_FATAL_FAILURE(node->Prepare(visitor));
}

/**
 * @tc.name: Process001
 * @tc.desc: Verify Process does not crash with null visitor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, Process001, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->Process(nullptr));
}

/**
 * @tc.name: Process002
 * @tc.desc: Verify Process does not crash with valid visitor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, Process002, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    auto visitor = std::make_shared<RSRenderThreadVisitor>();
    EXPECT_NO_FATAL_FAILURE(node->Process(visitor));
}

/**
 * @tc.name: QuickPrepare001
 * @tc.desc: Verify QuickPrepare does not crash with null visitor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, QuickPrepare001, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->QuickPrepare(nullptr));
}

/**
 * @tc.name: QuickPrepare002
 * @tc.desc: Verify QuickPrepare does not crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, QuickPrepare002, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    auto visitor = std::make_shared<RSRenderThreadVisitor>();
    ASSERT_NE(visitor, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->QuickPrepare(visitor));
}

/**
 * @tc.name: InitRenderParams001
 * @tc.desc: Verify InitRenderParams does not crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, InitRenderParams001, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    ASSERT_NE(node, nullptr);
    EXPECT_NO_FATAL_FAILURE(node->InitRenderParams());
}

/**
 * @tc.name: UpdateRenderParams
 * @tc.desc: Verify UpdateRenderParams method
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, UpdateRenderParams, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    node->InitRenderParams();
    EXPECT_NO_FATAL_FAILURE(node->UpdateRenderParams());
    node->stagingRenderParams_ = nullptr;
    EXPECT_NO_FATAL_FAILURE(node->UpdateRenderParams());
}

/**
 * @tc.name: UpdateRenderParams_NoDepthResourceNode
 * @tc.desc: Verify UpdateRenderParams method when no depth resource node
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, UpdateRenderParams_NoDepthResourceNode, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    node->InitRenderParams();
    ASSERT_NE(node->stagingRenderParams_, nullptr);

    node->UpdateRenderParams();
    auto depthParams = static_cast<RSDepthRenderParams*>(node->stagingRenderParams_.get());
    EXPECT_TRUE(depthParams->depthSrcSurfaceDrawable_.expired());
}

/**
 * @tc.name: UpdateRenderParams_DepthResourceNodeNoDrawable
 * @tc.desc: Verify UpdateRenderParams method when depth resource node has no drawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, UpdateRenderParams_DepthResourceNodeNoDrawable, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    node->InitRenderParams();
    ASSERT_NE(node->stagingRenderParams_, nullptr);

    auto depthResourceNode = std::make_shared<RSRenderNode>(2, context);
    auto instance = RSSpatialEffectManager::Instance();
    instance->depthDepthResourceNodeMap_.emplace(testNodeId, depthResourceNode->weak_from_this());
    depthResourceNode->renderDrawable_ = nullptr;

    node->UpdateRenderParams();
    auto depthParams = static_cast<RSDepthRenderParams*>(node->stagingRenderParams_.get());
    EXPECT_TRUE(depthParams->depthSrcSurfaceDrawable_.expired());
    instance->depthDepthResourceNodeMap_.clear();
}

/**
 * @tc.name: UpdateRenderParams_DepthResourceNodeValid
 * @tc.desc: Verify UpdateRenderParams method when depth resource node is valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, UpdateRenderParams_DepthResourceNodeValid, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(testNodeId, context);
    node->InitRenderParams();
    ASSERT_NE(node->stagingRenderParams_, nullptr);

    auto depthResourceNode = std::make_shared<RSSurfaceRenderNode>(2, context);
    auto instance = RSSpatialEffectManager::Instance();
    instance->depthDepthResourceNodeMap_.emplace(testNodeId, depthResourceNode->weak_from_this());
    depthResourceNode->renderDrawable_ = std::make_shared<TestDrawableAdapter>(depthResourceNode);
    EXPECT_NE(depthResourceNode->renderDrawable_, nullptr);

    node->UpdateRenderParams();
    auto depthParams = static_cast<RSDepthRenderParams*>(node->stagingRenderParams_.get());
    EXPECT_FALSE(depthParams->depthSrcSurfaceDrawable_.expired());
    instance->depthDepthResourceNodeMap_.clear();
}

/**
 * @tc.name: Constructor001
 * @tc.desc: Verify constructor with isTextureExportNode = false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, Constructor001, TestSize.Level1)
{
    constexpr NodeId id = 42;
    auto node = std::make_shared<RSDepthRenderNode>(id, context, false);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetId(), id);
}

/**
 * @tc.name: Constructor002
 * @tc.desc: Verify constructor with isTextureExportNode = true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, Constructor002, TestSize.Level1)
{
    constexpr NodeId id = 43;
    auto node = std::make_shared<RSDepthRenderNode>(id, context, true);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->GetId(), id);
}

/**
 * @tc.name: Destructor001
 * @tc.desc: Verify destructor runs without crash
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, Destructor001, TestSize.Level1)
{
    constexpr NodeId id = 44;
    {
        auto node = std::make_shared<RSDepthRenderNode>(id, context);
        EXPECT_NE(node, nullptr);
    }
    // Node should be destroyed without crash
}

/**
 * @tc.name: ApplyDepthSpaceModifier_NormalCase
 * @tc.desc: Verify ApplyDepthSpaceModifier normal case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, ApplyDepthSpaceModifier_NormalCase, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(1, std::weak_ptr<RSContext>(), false);
    node->dirtyTypesNG_.set(static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE));
    auto modifier = std::make_shared<ModifierNG::RSDepthSpaceRenderModifier>();
    node->AddModifier(modifier);
    node->ApplyDepthSpaceModifier();
    EXPECT_FALSE(node->dirtyTypesNG_.test(static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE)));
}

/**
 * @tc.name: ApplyDepthSpaceModifier_NoDirtyFlag
 * @tc.desc: Verify ApplyDepthSpaceModifier no dirty flag case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, ApplyDepthSpaceModifier_NoDirtyFlag, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(1, std::weak_ptr<RSContext>(), false);

    auto modifier = std::make_shared<ModifierNG::RSDepthSpaceRenderModifier>();
    node->AddModifier(modifier);

    node->ApplyDepthSpaceModifier();

    EXPECT_FALSE(node->dirtyTypesNG_.test(static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE)));
}

/**
 * @tc.name: ApplyDepthSpaceModifier_NoModifiers
 * @tc.desc: Verify ApplyDepthSpaceModifier no modifiers case
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDepthRenderNodeTest, ApplyDepthSpaceModifier_NoModifiers, TestSize.Level1)
{
    auto node = std::make_shared<RSDepthRenderNode>(1, std::weak_ptr<RSContext>(), false);
    node->dirtyTypesNG_.set(static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE));
    node->ApplyDepthSpaceModifier();
    EXPECT_TRUE(node->dirtyTypesNG_.test(static_cast<uint16_t>(ModifierNG::RSModifierType::DEPTH_SPACE)));
}
} // namespace OHOS::Rosen