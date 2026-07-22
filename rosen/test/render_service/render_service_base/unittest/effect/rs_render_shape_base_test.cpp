
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
#include "ge_visual_effect_container.h"
#include "gtest/gtest.h"
#include "parcel.h" // Assumed definition of Parcel

#include "effect/rs_render_shape_base.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderShapeBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderShapeBaseTest::SetUpTestCase() {}
void RSRenderShapeBaseTest::TearDownTestCase() {}
void RSRenderShapeBaseTest::SetUp() {}
void RSRenderShapeBaseTest::TearDown() {}

/**
 * @tc.name: RSNGRenderShapeBaseUnmarshallingTest010
 * @tc.desc: Test marshalling and unmarshalling with SDF_UNION_OP_SHAPE, SDF_SMOOTH_UNION_OP_SHAPE
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShapeBaseTest, RSNGRenderShapeBaseUnmarshallingTest010, TestSize.Level1)
{
    auto head = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    auto next = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);

    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;

    Parcel parcel;
    head->Marshalling(parcel);

    RSNGEffectTypeUnderlying sdfUnionOpShape =
        static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::SDF_UNION_OP_SHAPE);
    parcel.WriteUint32(sdfUnionOpShape);

    std::shared_ptr<RSNGRenderShapeBase> result;
    EXPECT_TRUE(RSNGRenderShapeBase::Unmarshalling(parcel, result));
}

/**
 * @tc.name: RSNGRenderShapeBaseUnmarshallingTest011
 * @tc.desc: Test marshalling and unmarshalling with SDF_UNION_OP_SHAPE, SDF_RRECT_SHAPE
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderShapeBaseTest, RSNGRenderShapeBaseUnmarshallingTest011, TestSize.Level1)
{
    auto head = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    auto next = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);

    EXPECT_NE(head, nullptr);
    EXPECT_NE(next, nullptr);
    head->nextEffect_ = next;

    Parcel parcel;
    head->Marshalling(parcel);

    RSNGEffectTypeUnderlying sdfUnionOpShape =
        static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::SDF_UNION_OP_SHAPE);
    parcel.WriteUint32(sdfUnionOpShape);

    std::shared_ptr<RSNGRenderShapeBase> result;
    EXPECT_TRUE(RSNGRenderShapeBase::Unmarshalling(parcel, result));
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_NullShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(10.0f, 20.0f, 100.0f, 200.0f);
    std::shared_ptr<RSNGRenderShapeBase> shape = nullptr;

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFUnionOpShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(10.0f, 20.0f, 100.0f, 200.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFSmoothUnionOpShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(50.0f, 60.0f, 150.0f, 250.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_SMOOTH_UNION_OP_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFSubOpShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(15.0f, 25.0f, 120.0f, 220.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_SUB_OP_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFSmoothSubOpShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(30.0f, 40.0f, 140.0f, 240.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_SMOOTH_SUB_OP_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFRRectShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(0.0f, 0.0f, 200.0f, 300.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFTriangleShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(100.0f, 100.0f, 300.0f, 400.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_TRIANGLE_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFPixelmapShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(25.0f, 35.0f, 125.0f, 235.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_PIXELMAP_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFDFTransformShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(10.0f, 20.0f, 110.0f, 220.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_TRANSFORM_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFEmptyShape_ReturnsBound, TestSize.Level1)
{
    RectF bound(0.0f, 0.0f, 0.0f, 0.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_EMPTY_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_SDFDistortOpShape_WithNullBase_ReturnsBound, TestSize.Level1)
{
    RectF bound(10.0f, 20.0f, 100.0f, 200.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_TransformDrawRectIsUpdated, TestSize.Level1)
{
    RectF bound(10.0f, 20.0f, 100.0f, 200.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);

    ASSERT_NE(shape, nullptr);

    RSNGRenderShapeHelper::CalcRect(shape, bound);
    const RectF& transformDrawRect = shape->GetTransformDrawRect();

    EXPECT_FLOAT_EQ(transformDrawRect.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(transformDrawRect.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(transformDrawRect.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(transformDrawRect.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_NegativeBoundValues, TestSize.Level1)
{
    RectF bound(-10.0f, -20.0f, 100.0f, 200.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_LargeBoundValues, TestSize.Level1)
{
    RectF bound(0.0f, 0.0f, 10000.0f, 10000.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_TRIANGLE_SHAPE);

    ASSERT_NE(shape, nullptr);

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
}

HWTEST_F(RSRenderShapeBaseTest, CalcRect_NeedUpdateFalse, TestSize.Level1)
{
    RectF bound(10.0f, 20.0f, 100.0f, 200.0f);
    auto shape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);

    ASSERT_NE(shape, nullptr);
    EXPECT_TRUE(shape->GetTransformDrawRect().IsEmpty());

    RectF result = RSNGRenderShapeHelper::CalcRect(shape, bound, false);

    EXPECT_FLOAT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_FLOAT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_FLOAT_EQ(result.GetWidth(), bound.GetWidth());
    EXPECT_FLOAT_EQ(result.GetHeight(), bound.GetHeight());
    EXPECT_TRUE(shape->GetTransformDrawRect().IsEmpty());
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_NullShape, TestSize.Level1)
{
    std::shared_ptr<RSNGRenderShapeBase> sdfShape = nullptr;
    RRect sdfRRect(RectF(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f);
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, sdfRRect, nodeId);
    EXPECT_EQ(sdfShape, nullptr);
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_NotDistortOpShape, TestSize.Level1)
{
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    RRect sdfRRect(RectF(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f);
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, sdfRRect, nodeId);
    EXPECT_EQ(sdfShape->GetType(), RSNGEffectType::SDF_RRECT_SHAPE);
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_AlreadyHasInnerShape, TestSize.Level1)
{
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(sdfShape);
    auto existingShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    distortOpShape->Setter<SDFDistortOpShapeShapeRenderTag>(existingShape);
    RRect sdfRRect(RectF(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f);
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, sdfRRect, nodeId);
    auto innerShape = distortOpShape->Getter<SDFDistortOpShapeShapeRenderTag>()->Get();
    EXPECT_EQ(innerShape, existingShape);
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_SyncTrueWithExistingShape, TestSize.Level1)
{
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(sdfShape);
    auto existingShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    distortOpShape->Setter<SDFDistortOpShapeShapeRenderTag>(existingShape);
    distortOpShape->Setter<SDFDistortOpShapeSyncRenderTag>(true);
    RRect sdfRRect(RectF(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f);
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, sdfRRect, nodeId);
    auto innerShape = distortOpShape->Getter<SDFDistortOpShapeShapeRenderTag>()->Get();
    EXPECT_EQ(innerShape, existingShape);
    auto rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(innerShape);
    auto actualRRect = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetLeft(), sdfRRect.rect_.GetLeft());
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetTop(), sdfRRect.rect_.GetTop());
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetWidth(), sdfRRect.rect_.GetWidth());
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetHeight(), sdfRRect.rect_.GetHeight());
    EXPECT_FLOAT_EQ(actualRRect.radius_[0][0], sdfRRect.radius_[0][0]);
    EXPECT_FLOAT_EQ(actualRRect.radius_[0][1], sdfRRect.radius_[0][1]);
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_Success, TestSize.Level1)
{
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(sdfShape);
    RRect sdfRRect(RectF(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f);
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, sdfRRect, nodeId);
    auto innerShape = distortOpShape->Getter<SDFDistortOpShapeShapeRenderTag>()->Get();
    ASSERT_NE(innerShape, nullptr);
    EXPECT_EQ(innerShape->GetType(), RSNGEffectType::SDF_RRECT_SHAPE);
    auto rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(innerShape);
    auto actualRRect = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetLeft(), sdfRRect.rect_.GetLeft());
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetTop(), sdfRRect.rect_.GetTop());
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetWidth(), sdfRRect.rect_.GetWidth());
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetHeight(), sdfRRect.rect_.GetHeight());
    EXPECT_FLOAT_EQ(actualRRect.radius_[0][0], sdfRRect.radius_[0][0]);
    EXPECT_FLOAT_EQ(actualRRect.radius_[0][1], sdfRRect.radius_[0][1]);
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_SyncFalseWithExistingShape, TestSize.Level1)
{
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(sdfShape);

    // Set existing inner shape
    auto existingShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_RRECT_SHAPE);
    ASSERT_NE(existingShape, nullptr);
    auto existingRRectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(existingShape);

    // Set initial RRect values
    RRect initialRRect(RectF(10.0f, 10.0f, 50.0f, 50.0f), 5.0f, 5.0f);
    existingRRectShape->Setter<SDFRRectShapeRRectRenderTag>(initialRRect);
    distortOpShape->Setter<SDFDistortOpShapeShapeRenderTag>(existingShape);

    // Set sync to false - should not update RRect
    distortOpShape->Setter<SDFDistortOpShapeSyncRenderTag>(false);

    RRect newSdfRRect(RectF(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f);
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, newSdfRRect, nodeId);

    // Verify inner shape remains unchanged
    auto innerShape = distortOpShape->Getter<SDFDistortOpShapeShapeRenderTag>()->Get();
    EXPECT_EQ(innerShape, existingShape);

    // Verify RRect was not updated (sync is false)
    auto actualRRect = existingRRectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetLeft(), initialRRect.rect_.GetLeft());
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetTop(), initialRRect.rect_.GetTop());
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_NullSdfRRect, TestSize.Level1)
{
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(sdfShape);

    // Use default zero RRect
    RRect emptySdfRRect;
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, emptySdfRRect, nodeId);

    auto innerShape = distortOpShape->Getter<SDFDistortOpShapeShapeRenderTag>()->Get();
    ASSERT_NE(innerShape, nullptr);
    EXPECT_EQ(innerShape->GetType(), RSNGEffectType::SDF_RRECT_SHAPE);

    auto rrectShape = std::static_pointer_cast<RSNGRenderSDFRRectShape>(innerShape);
    auto actualRRect = rrectShape->Getter<SDFRRectShapeRRectRenderTag>()->stagingValue_;
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetLeft(), 0.f);
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetTop(), 0.f);
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetWidth(), 0.f);
    EXPECT_FLOAT_EQ(actualRRect.rect_.GetHeight(), 0.f);
}

HWTEST_F(RSRenderShapeBaseTest, FillEmptyDistortOpShape_SkipNonRRectInner, TestSize.Level1)
{
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_DISTORT_OP_SHAPE);
    ASSERT_NE(sdfShape, nullptr);
    auto distortOpShape = std::static_pointer_cast<RSNGRenderSDFDistortOpShape>(sdfShape);
    auto existingShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_TRIANGLE_SHAPE);
    ASSERT_NE(existingShape, nullptr);
    distortOpShape->Setter<SDFDistortOpShapeShapeRenderTag>(existingShape);
    distortOpShape->Setter<SDFDistortOpShapeSyncRenderTag>(true, PropertyUpdateType::UPDATE_TYPE_ONLY_VALUE);

    RRect sdfRRect(RectF(0.0f, 0.0f, 100.0f, 100.0f), 10.0f, 10.0f);
    NodeId nodeId = 1;

    RSNGRenderShapeHelper::FillEmptyDistortOpShape(sdfShape, sdfRRect, nodeId);

    // Inner is kept as-is; no RRect sync attempted on a non-RRect shape.
    auto innerShape = distortOpShape->Getter<SDFDistortOpShapeShapeRenderTag>()->Get();
    EXPECT_EQ(innerShape, existingShape);
    EXPECT_EQ(innerShape->GetType(), RSNGEffectType::SDF_TRIANGLE_SHAPE);
}

} // namespace OHOS::Rosen