/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "animation/rs_spring_model.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSpringModelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpringModelTest::SetUpTestCase() {}
void RSSpringModelTest::TearDownTestCase() {}
void RSSpringModelTest::SetUp() {}
void RSSpringModelTest::TearDown() {}

/**
 * @tc.name: RSSpringModelFloatTest001
 * @tc.desc: Verify the RSSpringModelFloat
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelFloatTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelFloatTest001 start";
    float initialOffset = 1.0f;
    float initialVelocity = 1.0f;

    auto model1 = std::make_shared<RSSpringModel<float>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == 0.0f);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != 0.0f);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != 0.0f);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model4 = std::make_shared<RSSpringModel<float>>();
    duration = model4->EstimateDuration();
    EXPECT_TRUE(duration == 0.0f);
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelFloatTest001 end";
}

/**
 * @tc.name: RSSpringModelColorTest001
 * @tc.desc: Verify the RSSpringModelColor
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelColorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelColorTest001 start";
    Color initialOffset = Color(0, 0, 0);
    Color initialVelocity = Color(255, 255, 255);

    auto model1 = std::make_shared<RSSpringModel<Color>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Color>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Color>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelColorTest001 end";
}

/**
 * @tc.name: RSSpringModelMatrix3fTest001
 * @tc.desc: Verify the RSSpringModelMatrix3f
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelMatrix3fTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelMatrix3fTest001 start";
    Matrix3f initialOffset = Matrix3f::ZERO;
    Matrix3f initialVelocity = Matrix3f::IDENTITY;

    auto model1 = std::make_shared<RSSpringModel<Matrix3f>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelMatrix3fTest001 end";
}

/**
 * @tc.name: RSSpringModelVector2fTest001
 * @tc.desc: Verify the RSSpringModelVector2f
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelVector2fTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelVector2fTest001 start";
    Vector2f initialOffset = Vector2f(0.0f, 0.0f);
    Vector2f initialVelocity = Vector2f(1.0f, 1.0f);

    auto model1 = std::make_shared<RSSpringModel<Vector2f>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelVector2fTest001 end";
}

/**
 * @tc.name: RSSpringModelVector4fTest001
 * @tc.desc: Verify the RSSpringModelVector4f
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelVector4fTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelVector4fTest001 start";
    Vector4f initialOffset = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4f initialVelocity = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

    auto model1 = std::make_shared<RSSpringModel<Vector4f>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelVector4fTest001 end";
}

/**
 * @tc.name: RSSpringModelQuaternionTest001
 * @tc.desc: Verify the RSSpringModelQuaternion
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelQuaternionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelQuaternionTest001 start";
    Quaternion initialOffset = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
    Quaternion initialVelocity = Quaternion(1.0f, 1.0f, 1.0f, 1.0f);

    auto model1 = std::make_shared<RSSpringModel<Quaternion>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelQuaternionTest001 end";
}

/**
 * @tc.name: RSSpringModelVector4ColorTest001
 * @tc.desc: Verify the RSSpringModelVector4Color
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelVector4ColorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelVector4ColorTest001 start";
    Vector4<Color> initialOffset =
        Vector4<Color>(Color(0, 0, 0), Color(0, 0, 0), Color(0, 0, 0), Color(0, 0, 0));
    Vector4<Color> initialVelocity =
        Vector4<Color>(Color(255, 255, 255), Color(255, 255, 255), Color(255, 255, 255), Color(255, 255, 255));

    auto model1 = std::make_shared<RSSpringModel<Vector4<Color>>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelVector4ColorTest001 end";
}

/**
 * @tc.name: RSSpringModelRSRSRenderPropertyBaseTest001
 * @tc.desc: Verify the RSSpringModelRSRenderPropertyBase
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelRSRSRenderPropertyBaseTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRSRSRenderPropertyBaseTest001 start";

    auto initialOffset = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(0.0f, 0.0f,
        initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(1.0f, 1.0f,
        initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(1.0f, 2.0f,
        initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model4 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>();
    model4->dampingRatio_ = -1.0;
    duration = model4->EstimateDuration();
    EXPECT_TRUE(duration == 0.0f);
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRSRSRenderPropertyBaseTest001 end";
}

/**
 * @tc.name: RSSpringModelRRectTest001
 * @tc.desc: Verify the RSSpringModelRRect
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelRRectTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRRectTest001 start";

    RectF rect1;
    rect1.SetAll(0.f, 0.f, 0.f, 0.f);
    RectF rect2;
    rect2.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect initialOffset(rect1, 0.f, 0.f);
    RRect initialVelocity(rect2, 1.f, 1.f);

    auto model1 = std::make_shared<RSSpringModel<RRect>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<RRect>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<RRect>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->EstimateDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRRectTest001 end";
}

/**
 * @tc.name: RSSpringModelRRectTest002
 * @tc.desc: Verify the RSSpringModelRRectTest
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelRRectTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRRectTest002 start";

    RectF rect1;
    rect1.SetAll(0.f, 0.f, 0.f, 0.f);
    RectF rect2;
    rect2.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect initialOffset(rect1, 0.f, 0.f);
    RRect initialVelocity(rect2, 1.f, 1.f);

    auto model1 = std::make_shared<RSSpringModel<RRect>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    model1->dampingRatio_ = -1.0f;
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == initialOffset);
    auto duration = model1->EstimateDuration();
    EXPECT_TRUE(duration == 0.0f);

    auto model2 = std::make_shared<RSSpringModel<RRect>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    model2->CalculateSpringParameters();
    duration = model2->EstimateDuration();
    EXPECT_FALSE(duration == 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRRectTest002 end";
}

/**
 * @tc.name: RSSpringModelRRectTest003
 * @tc.desc: Verify the RSSpringModelRRectTest
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelRRectTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRRectTest003 start";

    RectF rect1;
    rect1.SetAll(0.f, 0.f, 0.f, 0.f);
    RectF rect2;
    rect2.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect initialOffset(rect1, 0.f, 0.f);
    RRect initialVelocity(rect2, 1.f, 1.f);

    auto model1 = std::make_shared<RSSpringModel<RRect>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    model1->response_ = -1.0f;
    auto duration = model1->EstimateDurationForUnderDampedModel();
    EXPECT_TRUE(duration == 0.0f);
    duration = model1->EstimateDurationForCriticalDampedModel();
    EXPECT_TRUE(duration == 0.0f);
    duration = model1->EstimateDurationForOverDampedModel();
    EXPECT_TRUE(duration == 0.0f);

    auto model2 = std::make_shared<RSSpringModel<RRect>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    model2->dampingRatio_ = 1.0f;
    auto duration2 = model2->EstimateDurationForUnderDampedModel();
    EXPECT_TRUE(duration2 == 0.0f);
    duration2 = model2->EstimateDurationForOverDampedModel();
    EXPECT_TRUE(duration2 == 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRRectTest003 end";
}

/**
 * @tc.name: CalculateSpringParameters001
 * @tc.desc: Verify the CalculateSpringParameters
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, CalculateSpringParameters001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest CalculateSpringParameters001 start";

    auto initialOffset = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(0.0f, 0.0f,
        initialOffset, initialVelocity, 0.0f);
    model1->response_ = INFINITY;
    model1->CalculateSpringParameters();
    auto duration = model1->EstimateDuration();
    EXPECT_EQ(duration, 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest CalculateSpringParameters001 end";
}

/**
 * @tc.name: EstimateDuration001
 * @tc.desc: Verify the EstimateDuration
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, EstimateDuration001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest EstimateDuration001 start";

    auto initialOffset = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(0.0f, 0.0f,
        initialOffset, initialVelocity, 0.0f);
    model1->dampingRatio_ = -1.0f;
    model1->coeffDecay_ = 0.0f;
    auto duration = model1->EstimateDuration();
    EXPECT_EQ(duration, 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest EstimateDuration001 end";
}

/**
 * @tc.name: EstimateDurationForUnderDampedModel001
 * @tc.desc: Verify the EstimateDurationForUnderDampedModel
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, EstimateDurationForUnderDampedModel001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest EstimateDurationForUnderDampedModel001 start";

    auto initialOffset = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(0.0f, 0.0f,
        initialOffset, initialVelocity, 0.0f);
    model1->response_ = 1.0f;
    model1->dampingRatio_ = 0.0f;
    auto duration1 = model1->EstimateDurationForUnderDampedModel();
    EXPECT_EQ(duration1, 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest EstimateDurationForUnderDampedModel001 end";
}

/**
 * @tc.name: EstimateDurationForOverDampedModel001
 * @tc.desc: Verify the EstimateDurationForOverDampedModel
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, EstimateDurationForOverDampedModel001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest EstimateDurationForOverDampedModel001 start";

    auto initialOffset = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(0.0f, 0.0f,
        initialOffset, initialVelocity, 0.0f);
    model1->dampingRatio_ = 1.0f;
    auto duration = model1->EstimateDurationForOverDampedModel();
    EXPECT_EQ(duration, 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest EstimateDurationForOverDampedModel001 end";
}
} // namespace Rosen
} // namespace OHOS