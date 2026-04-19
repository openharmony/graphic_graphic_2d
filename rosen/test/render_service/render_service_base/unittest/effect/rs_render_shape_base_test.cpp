
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
#include "ge_visual_effect_container.h"
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

    RSNGEffectTypeUnderlying sdfUnionOpShape = static_cast<RSNGEffectTypeUnderlying>(
        RSNGEffectType::SDF_UNION_OP_SHAPE);
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

    RSNGEffectTypeUnderlying sdfUnionOpShape = static_cast<RSNGEffectTypeUnderlying>(
        RSNGEffectType::SDF_UNION_OP_SHAPE);
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

} // namespace OHOS::Rosen