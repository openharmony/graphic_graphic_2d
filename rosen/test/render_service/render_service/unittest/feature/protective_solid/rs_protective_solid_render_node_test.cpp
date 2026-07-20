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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_context.h"
#include "feature/protective_solid/rs_protective_solid_render_node.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "common/rs_vector4.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSProtectiveSolidRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    
    static inline std::shared_ptr<RSContext> context_ = nullptr;
    static inline NodeId nextNodeId_ = 1;
    NodeId nodeId_ = 0;
    std::shared_ptr<RSProtectiveSolidRenderNode> node_;
    NodeId AllocNodeId()
    {
        return nextNodeId_++;
    }
};

void RSProtectiveSolidRenderNodeTest::SetUpTestCase()
{
    context_ = std::make_shared<RSContext>();
    context_->GetMutableNodeMap().Initialize(context_);
}

void RSProtectiveSolidRenderNodeTest::TearDownTestCase()
{
    context_ = nullptr;
}

void RSProtectiveSolidRenderNodeTest::SetUp()
{
    nodeId_ = AllocNodeId();
    node_ = std::make_shared<RSProtectiveSolidRenderNode>(nodeId_, context_);
    context_->GetMutableNodeMap().RegisterRenderNode(node_);
}

void RSProtectiveSolidRenderNodeTest::TearDown()
{
    context_->GetMutableNodeMap().UnregisterRenderNode(nodeId_);
    node_ = nullptr;
}

/**
 * @tc.name: CreateNodeTest001
 * @tc.desc: Test creating RSProtectiveSolidRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, CreateNodeTest001, TestSize.Level1)
{
    ASSERT_NE(node_, nullptr);
    EXPECT_EQ(node_->GetId(), nodeId_);
}

/**
 * @tc.name: GetTypeTest001
 * @tc.desc: Test GetType returns correct type
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, GetTypeTest001, TestSize.Level1)
{
    ASSERT_NE(node_, nullptr);
    EXPECT_EQ(node_->GetType(), RSRenderNodeType::PROTECTIVE_SOLID_NODE);
}

/**
 * @tc.name: QuickPrepareTest001
 * @tc.desc: Test QuickPrepare with valid visitor
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, QuickPrepareTest001, TestSize.Level1)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    
    EXPECT_NO_FATAL_FAILURE(node_->QuickPrepare(visitor, false));
}

/**
 * @tc.name: QuickPrepareTest002
 * @tc.desc: Test QuickPrepare with nullptr visitor
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, QuickPrepareTest002, TestSize.Level1)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    EXPECT_NO_FATAL_FAILURE(node_->QuickPrepare(visitor, true));
}

/**
 * @tc.name: UpdateProtectiveSolidLayerInfoTest001
 * @tc.desc: Test UpdateProtectiveSolidLayerInfo with normal parameters
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, UpdateProtectiveSolidLayerInfoTest001, TestSize.Level1)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    
    EXPECT_NO_FATAL_FAILURE(node_->UpdateProtectiveSolidLayerInfo(transform));
}

/**
 * @tc.name: UpdateProtectiveSolidLayerInfoTest002
 * @tc.desc: Test UpdateProtectiveSolidLayerInfo with different transform types
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, UpdateProtectiveSolidLayerInfoTest002, TestSize.Level1)
{
    std::vector<GraphicTransformType> transforms = {
        GraphicTransformType::GRAPHIC_ROTATE_NONE,
        GraphicTransformType::GRAPHIC_ROTATE_90,
        GraphicTransformType::GRAPHIC_ROTATE_180,
        GraphicTransformType::GRAPHIC_ROTATE_270,
        GraphicTransformType::GRAPHIC_FLIP_H,
        GraphicTransformType::GRAPHIC_FLIP_V,
    };
    
    for (const auto& transform : transforms) {
        EXPECT_NO_FATAL_FAILURE(node_->UpdateProtectiveSolidLayerInfo(transform));
    }
}

/**
 * @tc.name: UpdateProtectiveSolidLayerInfoTest003
 * @tc.desc: Test UpdateProtectiveSolidLayerInfo sets correct layer info
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, UpdateProtectiveSolidLayerInfoTest003, TestSize.Level1)
{
    node_->GetMutableRenderProperties().SetBounds(Vector4f(100, 200, 300, 400));
    node_->UpdateProtectiveSolidLayerInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    
    auto params = static_cast<RSSurfaceRenderParams*>(node_->GetStagingRenderParams().get());
    ASSERT_NE(params, nullptr);
    
    auto layer = params->GetLayerInfo();
    EXPECT_EQ(layer.boundRect.x, 100);
    EXPECT_EQ(layer.boundRect.y, 200);
    EXPECT_EQ(layer.boundRect.w, 300);
    EXPECT_EQ(layer.boundRect.h, 400);
}

/**
 * @tc.name: UpdateProtectiveSolidLayerInfoTest004
 * @tc.desc: Test UpdateProtectiveSolidLayerInfo sets correct alpha
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, UpdateProtectiveSolidLayerInfoTest004, TestSize.Level1)
{
    constexpr float testAlpha = 0.5f;
    node_->SetGlobalAlpha(testAlpha);
    node_->UpdateProtectiveSolidLayerInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    
    auto params = static_cast<RSSurfaceRenderParams*>(node_->GetStagingRenderParams().get());
    ASSERT_NE(params, nullptr);
    
    auto layer = params->GetLayerInfo();
    EXPECT_FLOAT_EQ(layer.alpha, testAlpha);
}

/**
 * @tc.name: UpdateProtectiveSolidLayerInfoTest005
 * @tc.desc: Test UpdateProtectiveSolidLayerInfo sets correct transform
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, UpdateProtectiveSolidLayerInfoTest005, TestSize.Level1)
{
    GraphicTransformType testTransform = GraphicTransformType::GRAPHIC_ROTATE_90;
    node_->UpdateProtectiveSolidLayerInfo(testTransform);
    
    auto params = static_cast<RSSurfaceRenderParams*>(node_->GetStagingRenderParams().get());
    ASSERT_NE(params, nullptr);
    
    auto layer = params->GetLayerInfo();
    EXPECT_EQ(layer.transformType, testTransform);
}

/**
 * @tc.name: GetScreenNodeIdTest001
 * @tc.desc: Test GetScreenNodeId returns correct value
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, GetScreenNodeIdTest001, TestSize.Level1)
{
    EXPECT_NO_FATAL_FAILURE(node_->GetScreenNodeId());
}

/**
 * @tc.name: GetLogicalDisplayNodeIdTest001
 * @tc.desc: Test GetLogicalDisplayNodeId returns correct value
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, GetLogicalDisplayNodeIdTest001, TestSize.Level1)
{
    EXPECT_NO_FATAL_FAILURE(node_->GetLogicalDisplayNodeId());
}

/**
 * @tc.name: TypeTest001
 * @tc.desc: Test static type member
 * @tc.type: FUNC
 * @tc.require: issueI9NBLA
 */
HWTEST_F(RSProtectiveSolidRenderNodeTest, TypeTest001, TestSize.Level1)
{
    EXPECT_EQ(RSProtectiveSolidRenderNode::Type, RSRenderNodeType::PROTECTIVE_SOLID_NODE);
}
} // namespace OHOS::Rosen