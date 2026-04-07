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
#include "params/rs_render_params.h"
#include "pipeline/rs_union_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "render_thread/rs_render_thread_visitor.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class RSUnionRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSUnionRenderNodeTest::SetUpTestCase() {}
void RSUnionRenderNodeTest::TearDownTestCase() {}
void RSUnionRenderNodeTest::SetUp() {}
void RSUnionRenderNodeTest::TearDown() {}

/**
 * @tc.name: CreateSDFOpShapeWithBaseInitialization001
 * @tc.desc: test CreateSDFOpShapeWithBaseInitialization
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, CreateSDFOpShapeWithBaseInitialization001, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto ret = unionNode->CreateSDFOpShapeWithBaseInitialization(RSNGEffectType::CONTENT_LIGHT);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateSDFOpShapeWithBaseInitialization002
 * @tc.desc: test CreateSDFOpShapeWithBaseInitialization
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, CreateSDFOpShapeWithBaseInitialization002, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto ret = unionNode->CreateSDFOpShapeWithBaseInitialization(RSNGEffectType::SDF_UNION_OP_SHAPE);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: CreateSDFOpShapeWithBaseInitialization003
 * @tc.desc: test CreateSDFOpShapeWithBaseInitialization
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, CreateSDFOpShapeWithBaseInitialization003, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto ret = unionNode->CreateSDFOpShapeWithBaseInitialization(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: CreateSDFOpShapeWithBaseInitialization004
 * @tc.desc: test CreateSDFOpShapeWithBaseInitialization
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, CreateSDFOpShapeWithBaseInitialization004, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto ret = unionNode->CreateSDFOpShapeWithBaseInitialization(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetOrCreateChildSDFShape001
 * @tc.desc: test GetOrCreateChildSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetOrCreateChildSDFShape001, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);
    child->renderProperties_.renderSDFShape_ = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);

    auto ret = unionNode->GetOrCreateChildSDFShape(child);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: GetOrCreateChildSDFShape002
 * @tc.desc: test GetOrCreateChildSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetOrCreateChildSDFShape002, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);
    child->renderProperties_.clipRRect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);

    auto ret = unionNode->GetOrCreateChildSDFShape(child);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: GetOrCreateChildSDFShape003
 * @tc.desc: test GetOrCreateChildSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetOrCreateChildSDFShape003, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);
    child->renderProperties_.cornerRadius_ = Vector4f(3.f);
    child->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);

    auto ret = unionNode->GetOrCreateChildSDFShape(child);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: GetOrCreateChildSDFShape004
 * @tc.desc: test GetOrCreateChildSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetOrCreateChildSDFShape004, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);

    auto ret = unionNode->GetOrCreateChildSDFShape(child);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: GetChildRelativeMatrixToUnionNode001
 * @tc.desc: test GetChildRelativeMatrixToUnionNode
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetChildRelativeMatrixToUnionNode001, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);
    unionNode->renderProperties_.boundsGeo_ = nullptr;
    Drawing::Matrix matrix;

    auto ret = unionNode->GetChildRelativeMatrixToUnionNode(matrix, child);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: GetChildRelativeMatrixToUnionNode002
 * @tc.desc: test GetChildRelativeMatrixToUnionNode
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetChildRelativeMatrixToUnionNode002, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    EXPECT_TRUE(unionNode->renderProperties_.boundsGeo_->absMatrix_.has_value());
    unionNode->renderProperties_.boundsGeo_->absMatrix_->SetMatrix(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    
    Drawing::Matrix matrix;
    auto ret = unionNode->GetChildRelativeMatrixToUnionNode(matrix, child);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: GetChildRelativeMatrixToUnionNode003
 * @tc.desc: test GetChildRelativeMatrixToUnionNode
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetChildRelativeMatrixToUnionNode003, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child->parent_ = unionNode;
    
    Drawing::Matrix matrix;
    auto ret = unionNode->GetChildRelativeMatrixToUnionNode(matrix, child);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: GetChildRelativeMatrixToUnionNode004
 * @tc.desc: test GetChildRelativeMatrixToUnionNode
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GetChildRelativeMatrixToUnionNode004, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    auto child = std::make_shared<RSRenderNode>(id + 1, context);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child->renderProperties_.boundsGeo_ = nullptr;
    
    Drawing::Matrix matrix;
    auto ret = unionNode->GetChildRelativeMatrixToUnionNode(matrix, child);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: GenerateSDFNonLeaf001
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFNonLeaf001, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    unionNode->unionChildren_.emplace(id + 1);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, RSNGRenderSDFSmoothUnionOpShape,
        SDFSmoothUnionOpShapeShapeXRenderTag, SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GenerateSDFNonLeaf002
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFNonLeaf002, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    unionNode->unionChildren_.emplace(id + 1);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
        SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GenerateSDFNonLeaf003
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFNonLeaf003, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->unionChildren_.emplace(id + 2);
    unionNode->unionChildren_.emplace(id + 3);
    unionNode->unionChildren_.emplace(id + 4);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, RSNGRenderSDFSmoothUnionOpShape,
        SDFSmoothUnionOpShapeShapeXRenderTag, SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GenerateSDFNonLeaf004
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFNonLeaf004, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->unionChildren_.emplace(id + 2);
    unionNode->unionChildren_.emplace(id + 3);
    unionNode->unionChildren_.emplace(id + 4);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
        SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GenerateSDFLeaf001
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf001, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    EXPECT_EQ(unionNode->GetContext().lock(), nullptr);
    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    unionNode->unionChildren_.emplace(id + 1);
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, RSNGRenderSDFSmoothUnionOpShape,
        SDFSmoothUnionOpShapeShapeXRenderTag, SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    unionNode->GenerateSDFLeaf<RSNGRenderSDFSmoothUnionOpShape, SDFSmoothUnionOpShapeShapeXRenderTag,
        SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf002
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf002, TestSize.Level1)
{
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, context);
    EXPECT_EQ(unionNode->GetContext().lock(), nullptr);
    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    unionNode->unionChildren_.emplace(id + 1);
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
        SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    unionNode->GenerateSDFLeaf<RSNGRenderSDFUnionOpShape, SDFUnionOpShapeShapeXRenderTag,
        SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf003
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf003, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    EXPECT_EQ(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, RSNGRenderSDFSmoothUnionOpShape,
        SDFSmoothUnionOpShapeShapeXRenderTag, SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFSmoothUnionOpShape, SDFSmoothUnionOpShapeShapeXRenderTag,
        SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf004
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf004, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    EXPECT_EQ(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
        SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFUnionOpShape, SDFUnionOpShapeShapeXRenderTag,
        SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf005
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf005, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->renderProperties_.boundsGeo_ = nullptr;
    Drawing::Matrix matrix;
    EXPECT_FALSE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, RSNGRenderSDFSmoothUnionOpShape,
        SDFSmoothUnionOpShapeShapeXRenderTag, SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFSmoothUnionOpShape, SDFSmoothUnionOpShapeShapeXRenderTag,
        SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf006
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf006, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->renderProperties_.boundsGeo_ = nullptr;
    Drawing::Matrix matrix;
    EXPECT_FALSE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
        SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFUnionOpShape, SDFUnionOpShapeShapeXRenderTag,
        SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf007
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf007, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->parent_ = unionNode;
    Drawing::Matrix matrix;
    EXPECT_TRUE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));
    child1->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);
    auto ret = unionNode->GetOrCreateChildSDFShape(child1);
    EXPECT_NE(ret, nullptr);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    ASSERT_TRUE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFSmoothUnionOpShape, SDFSmoothUnionOpShapeShapeXRenderTag,
        SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf008
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf008, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->parent_ = unionNode;
    Drawing::Matrix matrix;
    EXPECT_TRUE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));
    child1->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);
    auto ret = unionNode->GetOrCreateChildSDFShape(child1);
    EXPECT_NE(ret, nullptr);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    ASSERT_TRUE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFUnionOpShape, SDFUnionOpShapeShapeXRenderTag,
        SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf009
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf009, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    auto child2 = std::make_shared<RSRenderNode>(id + 2, context);
    sContext->nodeMap.RegisterRenderNode(child2);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 2), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->unionChildren_.emplace(id + 2);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child2->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->parent_ = unionNode;
    child2->parent_ = unionNode;
    Drawing::Matrix matrix;
    EXPECT_TRUE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));
    child1->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);
    child2->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 20.f, 20.f), 3.f, 3.f);
    auto shape = unionNode->GetOrCreateChildSDFShape(child1);
    EXPECT_NE(shape, nullptr);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE, RSNGRenderSDFSmoothUnionOpShape,
        SDFSmoothUnionOpShapeShapeXRenderTag, SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFSmoothUnionOpShape, SDFSmoothUnionOpShapeShapeXRenderTag,
        SDFSmoothUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(shapeQueue.empty());
}

/**
 * @tc.name: GenerateSDFLeaf010
 * @tc.desc: test GenerateSDFNonLeaf
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, GenerateSDFLeaf010, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    auto child2 = std::make_shared<RSRenderNode>(id + 2, context);
    sContext->nodeMap.RegisterRenderNode(child2);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 2), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->unionChildren_.emplace(id + 2);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child2->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->parent_ = unionNode;
    child2->parent_ = unionNode;
    Drawing::Matrix matrix;
    EXPECT_TRUE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));
    child1->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);
    child2->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 20.f, 20.f), 3.f, 3.f);
    auto shape = unionNode->GetOrCreateChildSDFShape(child1);
    EXPECT_NE(shape, nullptr);

    std::queue<std::shared_ptr<RSNGRenderShapeBase>> shapeQueue;
    auto ret = unionNode->GenerateSDFNonLeaf<RSNGEffectType::SDF_UNION_OP_SHAPE, RSNGRenderSDFUnionOpShape,
        SDFUnionOpShapeShapeXRenderTag, SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_FALSE(shapeQueue.empty());
    unionNode->GenerateSDFLeaf<RSNGRenderSDFUnionOpShape, SDFUnionOpShapeShapeXRenderTag,
        SDFUnionOpShapeShapeYRenderTag>(shapeQueue);
    ASSERT_TRUE(shapeQueue.empty());
}

/**
 * @tc.name: ProcessSDFShape001
 * @tc.desc: test ProcessSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessSDFShape001, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);

    RSDirtyRegionManager dirtyManager;
    unionNode->ProcessSDFShape(dirtyManager);
    ASSERT_TRUE(unionNode->unionChildren_.empty());
}

/**
 * @tc.name: ProcessSDFShape002
 * @tc.desc: test ProcessSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessSDFShape002, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    auto child2 = std::make_shared<RSRenderNode>(id + 2, context);
    sContext->nodeMap.RegisterRenderNode(child2);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 2), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->unionChildren_.emplace(id + 2);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child2->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->parent_ = unionNode;
    child2->parent_ = unionNode;
    Drawing::Matrix matrix;
    EXPECT_TRUE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));
    child1->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);
    child2->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 20.f, 20.f), 3.f, 3.f);
    auto shape = unionNode->GetOrCreateChildSDFShape(child1);
    EXPECT_NE(shape, nullptr);
    unionNode->renderProperties_.unionSpacing_ = 0.5f;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(id);
    ASSERT_NE(stagingRenderParams, nullptr);
    unionNode->stagingRenderParams_ = std::move(stagingRenderParams);

    RSDirtyRegionManager dirtyManager;
    unionNode->ProcessSDFShape(dirtyManager);
    ASSERT_TRUE(unionNode->renderProperties_.renderSDFShape_ != nullptr);
}

/**
 * @tc.name: ProcessSDFShape003
 * @tc.desc: test ProcessSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessSDFShape003, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto child1 = std::make_shared<RSRenderNode>(id + 1, context);
    sContext->nodeMap.RegisterRenderNode(child1);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 1), nullptr);
    auto child2 = std::make_shared<RSRenderNode>(id + 2, context);
    sContext->nodeMap.RegisterRenderNode(child2);
    EXPECT_NE(sContext->GetNodeMap().GetRenderNode<RSRenderNode>(id + 2), nullptr);
    unionNode->unionChildren_.emplace(id + 1);
    unionNode->unionChildren_.emplace(id + 2);
    unionNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    unionNode->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    unionNode->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child1->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child2->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    child2->renderProperties_.boundsGeo_->matrix_ = Drawing::Matrix();
    child1->parent_ = unionNode;
    child2->parent_ = unionNode;
    Drawing::Matrix matrix;
    EXPECT_TRUE(unionNode->GetChildRelativeMatrixToUnionNode(matrix, child1));
    child1->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 10.f, 10.f), 3.f, 3.f);
    child2->renderProperties_.rrect_ = RRect(RectF(0.f, 0.f, 20.f, 20.f), 3.f, 3.f);
    auto shape = unionNode->GetOrCreateChildSDFShape(child1);
    EXPECT_NE(shape, nullptr);
    unionNode->renderProperties_.unionSpacing_ = 0.f;
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(id);
    ASSERT_NE(stagingRenderParams, nullptr);
    unionNode->stagingRenderParams_ = std::move(stagingRenderParams);

    RSDirtyRegionManager dirtyManager;
    unionNode->ProcessSDFShape(dirtyManager);
    ASSERT_TRUE(unionNode->renderProperties_.renderSDFShape_ != nullptr);
}

/**
 * @tc.name: ProcessSDFShape004
 * @tc.desc: test ProcessSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessSDFShape004, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_EMPTY_SHAPE);
    unionNode->renderProperties_.SetSDFShape(shape);

    RSDirtyRegionManager dirtyManager;
    unionNode->ProcessSDFShape(dirtyManager);
    ASSERT_TRUE(unionNode->renderProperties_.renderSDFShape_->GetType() == RSNGEffectType::SDF_EMPTY_SHAPE);
}

/**
 * @tc.name: ProcessSDFShape005
 * @tc.desc: test ProcessSDFShape
 * @tc.type:FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessSDFShape005, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    EXPECT_NE(unionNode->GetContext().lock(), nullptr);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    unionNode->renderProperties_.SetSDFShape(shape);

    RSDirtyRegionManager dirtyManager;
    unionNode->ProcessSDFShape(dirtyManager);
    ASSERT_TRUE(unionNode->renderProperties_.renderSDFShape_->GetType() == RSNGEffectType::SDF_EMPTY_SHAPE);
}

/**
 * @tc.name: QuickPrepare001
 * @tc.desc: test QuickPrepare
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, QuickPrepare001, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);

    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    unionNode->QuickPrepare(visitor);
    visitor = std::make_shared<RSRenderThreadVisitor>();
    unionNode->isFullChildrenListValid_ = true;
    unionNode->isChildrenSorted_ = true;
    unionNode->childrenHasSharedTransition_ = false;
    unionNode->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    unionNode->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::USE_EFFECT), true);
    unionNode->QuickPrepare(visitor);
    ASSERT_FALSE(unionNode->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::USE_EFFECT)));
}

/**
 * @tc.name: AddUnionChild
 * @tc.desc: test AddUnionChild
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, AddUnionChild, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    NodeId childId = 1;
    unionNode->AddUnionChild(childId);

    ASSERT_FALSE(unionNode->unionChildren_.empty());
}

/**
 * @tc.name: RemoveUnionChild
 * @tc.desc: test RemoveUnionChild
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, RemoveUnionChild, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    NodeId childId = 1;
    unionNode->AddUnionChild(childId);
    EXPECT_FALSE(unionNode->unionChildren_.empty());

    unionNode->RemoveUnionChild(childId);
    ASSERT_TRUE(unionNode->unionChildren_.empty());
}

/**
 * @tc.name: ResetUnionChildren
 * @tc.desc: test ResetUnionChildren
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ResetUnionChildren, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    auto unionNode = std::make_shared<RSUnionRenderNode>(id, sContext);
    NodeId childId = 1;
    unionNode->AddUnionChild(childId);
    EXPECT_FALSE(unionNode->unionChildren_.empty());

    unionNode->ResetUnionChildren();
    ASSERT_TRUE(unionNode->unionChildren_.empty());
}

/**
 * @tc.name: FindClosestUnionAncestor001
 * @tc.desc: parent == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, FindClosestUnionAncestor001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);

    ASSERT_EQ(RSUnionRenderNode::FindClosestUnionAncestor(node), nullptr);
}

/**
 * @tc.name: FindClosestUnionAncestor002
 * @tc.desc: find until unionNode
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, FindClosestUnionAncestor002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSUnionRenderNode> unionNode = std::make_shared<RSUnionRenderNode>(1);
    node->parent_ = parent;
    parent->parent_ = unionNode;

    ASSERT_NE(RSUnionRenderNode::FindClosestUnionAncestor(node), nullptr);
}

/**
 * @tc.name: ProcessUnionInfoOnTreeStateChanged001
 * @tc.desc: GetUseUnion == false
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessUnionInfoOnTreeStateChanged001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    node->renderProperties_.useUnion_ = false;

    RSUnionRenderNode::ProcessUnionInfoOnTreeStateChanged(node);
    ASSERT_FALSE(node->renderProperties_.useUnion_);
}

/**
 * @tc.name: ProcessUnionInfoOnTreeStateChanged002
 * @tc.desc: cannot find unionNode
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessUnionInfoOnTreeStateChanged002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    node->renderProperties_.useUnion_ = true;

    RSUnionRenderNode::ProcessUnionInfoOnTreeStateChanged(node);
    ASSERT_TRUE(node->renderProperties_.useUnion_);
}

/**
 * @tc.name: ProcessUnionInfoOnTreeStateChanged003
 * @tc.desc: can find unionNode
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessUnionInfoOnTreeStateChanged003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSUnionRenderNode> unionNode = std::make_shared<RSUnionRenderNode>(2);
    node->parent_ = parent;
    parent->parent_ = unionNode;
    node->renderProperties_.useUnion_ = true;
    node->isOnTheTree_ = true;

    RSUnionRenderNode::ProcessUnionInfoOnTreeStateChanged(node);
    ASSERT_FALSE(unionNode->unionChildren_.empty());
}

/**
 * @tc.name: ProcessUnionInfoAfterApplyModifiers001
 * @tc.desc: dirtyTypesNG_ has no BOUNDS type
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessUnionInfoAfterApplyModifiers001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    node->isOnTheTree_ = false;

    RSUnionRenderNode::ProcessUnionInfoAfterApplyModifiers(node);
    ASSERT_FALSE(node->isOnTheTree_);
}

/**
 * @tc.name: ProcessUnionInfoAfterApplyModifiers002
 * @tc.desc: dirtyTypesNG_ has no BOUNDS type
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessUnionInfoAfterApplyModifiers002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    node->isOnTheTree_ = true;

    RSUnionRenderNode::ProcessUnionInfoAfterApplyModifiers(node);
    ASSERT_TRUE(node->isOnTheTree_);
}

/**
 * @tc.name: ProcessUnionInfoAfterApplyModifiers003
 * @tc.desc: dirtyTypesNG_ has no BOUNDS type
 * @tc.type: FUNC
 */
HWTEST_F(RSUnionRenderNodeTest, ProcessUnionInfoAfterApplyModifiers003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(1);
    std::shared_ptr<RSUnionRenderNode> unionNode = std::make_shared<RSUnionRenderNode>(2);
    node->parent_ = parent;
    parent->parent_ = unionNode;
    node->AddDirtyType(ModifierNG::RSModifierType::BOUNDS);
    node->renderProperties_.useUnion_ = true;
    node->isOnTheTree_ = true;

    RSUnionRenderNode::ProcessUnionInfoAfterApplyModifiers(node);
    ASSERT_FALSE(unionNode->unionChildren_.empty());
}
} // namespace Rosen
} // namespace OHOS
