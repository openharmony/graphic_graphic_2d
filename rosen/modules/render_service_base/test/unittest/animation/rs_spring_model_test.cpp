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
#include "common/rs_common_def.h"
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

HWTEST_F(RSSpringModelTest, ROSENLEWithEpsilonLessThanThreshold, TestSize.Level1)
{
    EXPECT_TRUE(ROSEN_LE(0.0f, 0.0f, 0.001f));
    EXPECT_TRUE(ROSEN_LE(-0.5f, 0.0f, 0.001f));
    EXPECT_TRUE(ROSEN_LE(0.0005f, 0.0f, 0.001f));
}

HWTEST_F(RSSpringModelTest, ROSENLEWithEpsilonGreaterThanThreshold, TestSize.Level1)
{
    EXPECT_FALSE(ROSEN_LE(0.001f, 0.0f, 0.001f));
    EXPECT_FALSE(ROSEN_LE(0.002f, 0.0f, 0.001f));
    EXPECT_FALSE(ROSEN_LE(1.0f, 0.0f, 0.001f));
}

HWTEST_F(RSSpringModelTest, ROSENLEWithCustomEpsilon, TestSize.Level1)
{
    float springMinDampingRatio = 1e-4f;
    float springMinResponse = 1e-8f;
    EXPECT_TRUE(ROSEN_LE(0.0f, 0.0f, springMinDampingRatio));
    EXPECT_FALSE(ROSEN_LE(springMinDampingRatio, 0.0f, springMinDampingRatio));
    EXPECT_FALSE(ROSEN_LE(0.001f, 0.0f, springMinDampingRatio));
    EXPECT_TRUE(ROSEN_LE(0.0f, 0.0f, springMinResponse));
    EXPECT_FALSE(ROSEN_LE(springMinResponse, 0.0f, springMinResponse));
}

/**
 * @tc.name: ROSENLNEWithEpsilon001
 * @tc.desc: Verify ROSEN_LNE with epsilon (less not equal, new 3-arg overload)
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, ROSENLNEWithEpsilon001, TestSize.Level1)
{
    constexpr float epsilon = 1e-6f;
    // left significantly less than right -> true
    EXPECT_TRUE(ROSEN_LNE(0.5f, 1.0f, epsilon));
    EXPECT_TRUE(ROSEN_LNE(0.0f, 1.0f, epsilon));
    // left equal to right -> false
    EXPECT_FALSE(ROSEN_LNE(1.0f, 1.0f, epsilon));
    // left greater than right -> false
    EXPECT_FALSE(ROSEN_LNE(1.5f, 1.0f, epsilon));
    EXPECT_FALSE(ROSEN_LNE(2.0f, 1.0f, epsilon));
}

/**
 * @tc.name: ROSENLNEWithEpsilon002
 * @tc.desc: Verify ROSEN_LNE 3-arg boundary and zero-epsilon cases
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, ROSENLNEWithEpsilon002, TestSize.Level1)
{
    // boundary: (right - left) == epsilon -> false (strict >)
    EXPECT_FALSE(ROSEN_LNE(0.0f, 1.0f, 1.0f));
    // just above boundary: (right - left) slightly > epsilon -> true
    EXPECT_TRUE(ROSEN_LNE(-0.0001f, 1.0f, 1.0f));
    // just below boundary: (right - left) slightly < epsilon -> false
    EXPECT_FALSE(ROSEN_LNE(0.0001f, 1.0f, 1.0f));

    // zero epsilon: strict comparison
    EXPECT_TRUE(ROSEN_LNE(0.5f, 1.0f, 0.0f));  // right > left -> true
    EXPECT_FALSE(ROSEN_LNE(1.0f, 1.0f, 0.0f)); // equal -> false
    EXPECT_FALSE(ROSEN_LNE(1.5f, 1.0f, 0.0f)); // left > right -> false
}

/**
 * @tc.name: ROSENGEWithEpsilon001
 * @tc.desc: Verify ROSEN_GE with epsilon (great or equal, new 3-arg overload)
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, ROSENGEWithEpsilon001, TestSize.Level1)
{
    constexpr float epsilon = 1e-6f;
    // left greater than right -> true
    EXPECT_TRUE(ROSEN_GE(1.5f, 1.0f, epsilon));
    EXPECT_TRUE(ROSEN_GE(2.0f, 1.0f, epsilon));
    // left equal to right -> true (within epsilon)
    EXPECT_TRUE(ROSEN_GE(1.0f, 1.0f, epsilon));
    // left slightly less than right but within epsilon -> true
    EXPECT_TRUE(ROSEN_GE(0.9999995f, 1.0f, epsilon));
    // left significantly less than right -> false
    EXPECT_FALSE(ROSEN_GE(0.5f, 1.0f, epsilon));
    EXPECT_FALSE(ROSEN_GE(0.0f, 1.0f, epsilon));
}

/**
 * @tc.name: ROSENGEWithEpsilon002
 * @tc.desc: Verify ROSEN_GE 3-arg boundary and zero-epsilon cases
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, ROSENGEWithEpsilon002, TestSize.Level1)
{
    // boundary: (right - left) == epsilon -> false (strict <)
    EXPECT_FALSE(ROSEN_GE(0.0f, 1.0f, 1.0f));
    // just below boundary: (right - left) slightly < epsilon -> true
    EXPECT_TRUE(ROSEN_GE(0.0001f, 1.0f, 1.0f));
    // just above boundary: (right - left) slightly > epsilon -> false
    EXPECT_FALSE(ROSEN_GE(-0.0001f, 1.0f, 1.0f));

    // zero epsilon: left > right -> true; equal -> false; left < right -> false
    EXPECT_TRUE(ROSEN_GE(1.5f, 1.0f, 0.0f));
    EXPECT_FALSE(ROSEN_GE(1.0f, 1.0f, 0.0f));
    EXPECT_FALSE(ROSEN_GE(0.5f, 1.0f, 0.0f));
}

/**
 * @tc.name: WillOverShootFloat001
 * @tc.desc: Verify RSSpringModel<float>::WillOverShoot for underdamped/critical/overdamped
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootFloat001, TestSize.Level1)
{
    // Underdamped: dampingRatio < 1.0 -> always false
    auto underDamped = std::make_shared<RSSpringModel<float>>(1.0f, 0.5f, 1.0f, 10.0f, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());

    // Critical with positive velocity -> extremumTime > 0 -> true
    auto criticalOver = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, 1.0f, 10.0f, 1.0f);
    EXPECT_TRUE(criticalOver->WillOverShoot());

    // Critical with zero velocity -> extremumTime == 0 -> false
    auto criticalNoOver = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    EXPECT_FALSE(criticalNoOver->WillOverShoot());

    // Critical with tmpCoeff near zero (initialVelocity = -nav * initialOffset) -> false
    float nav = 2.0f * FLOAT_PI / 1.0f;
    auto criticalTmpCoeffZero = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, 1.0f, -nav, 1.0f);
    EXPECT_FALSE(criticalTmpCoeffZero->WillOverShoot());

    // Overdamped with positive velocity -> extremumTime > 0 -> true
    auto overDampedOver = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, 1.0f, 10.0f, 1.0f);
    EXPECT_TRUE(overDampedOver->WillOverShoot());

    // Overdamped with zero velocity -> extremumTime == 0 -> false
    auto overDampedNoOver = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, 1.0f, 0.0f, 1.0f);
    EXPECT_FALSE(overDampedNoOver->WillOverShoot());

    // response <= 0 -> false (log error). Set response_ to 0 after construction.
    auto zeroResponse = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, 1.0f, 10.0f, 1.0f);
    zeroResponse->response_ = 0.0f;
    EXPECT_FALSE(zeroResponse->WillOverShoot());
    zeroResponse->response_ = -1.0f;
    EXPECT_FALSE(zeroResponse->WillOverShoot());
}

/**
 * @tc.name: WillOverShootFloatNegativeOffset001
 * @tc.desc: Verify WillOverShoot with negative offset and velocity variations
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootFloatNegativeOffset001, TestSize.Level1)
{
    // Critical with negative velocity and negative tmpCoeff -> extremumTime > 0 -> true
    auto critical1 = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, 1.0f, -10.0f, 1.0f);
    EXPECT_TRUE(critical1->WillOverShoot());

    // Overdamped with negative velocity -> tmpCoeffG is negative -> extremumTime < 0 -> false
    auto overDampedNeg = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, 1.0f, -10.0f, 1.0f);
    EXPECT_FALSE(overDampedNeg->WillOverShoot());
}

/**
 * @tc.name: WillOverShootFloatOverDampedZeroCoeff001
 * @tc.desc: Verify WillOverShoot over-damped zero-coefficient early-return branches
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootFloatOverDampedZeroCoeff001, TestSize.Level1)
{
    // tmpCoeffE ~ 0: very large response makes naturalAngularVelocity ~ 0
    auto tmpCoeffEZero = std::make_shared<RSSpringModel<float>>(1e18f, 2.0f, 1.0f, 10.0f, 1.0f);
    EXPECT_FALSE(tmpCoeffEZero->WillOverShoot());

    // tmpCoeffC ~ 0: initialVelocity = -initialOffset * nav / tmpCoeffA
    // tmpCoeffA = dampingRatio + sqrt(dampingRatio^2 - 1) = 2 + sqrt(3) ~ 3.732
    // nav = 2*PI/1.0 ~ 6.2832, so velocity ~ -6.2832/3.732 ~ -1.6836
    auto tmpCoeffCZero = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, 1.0f, -1.6835744f, 1.0f);
    EXPECT_FALSE(tmpCoeffCZero->WillOverShoot());

    // tmpCoeffD ~ 0: initialVelocity = -initialOffset * nav / tmpCoeffB
    // tmpCoeffB = dampingRatio - sqrt(dampingRatio^2 - 1) = 2 - sqrt(3) ~ 0.2679
    // velocity ~ -6.2832/0.2679 ~ -23.4492
    auto tmpCoeffDZero = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, 1.0f, -23.449167f, 1.0f);
    EXPECT_FALSE(tmpCoeffDZero->WillOverShoot());
}

/**
 * @tc.name: WillOverShootFloatOverDampedExtremumTime001
 * @tc.desc: Verify WillOverShoot overdamped with extremumTime < 0 (0 < tmpCoeffG < 1)
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootFloatOverDampedExtremumTime001, TestSize.Level1)
{
    // Overdamped with small negative velocity: 0 < tmpCoeffG < 1 -> log < 0 -> extremumTime < 0 -> false
    // tmpCoeffG = tmpCoeffC / tmpCoeffD ≈ 0.718, log(0.718) ≈ -0.331, extremumTime ≈ -0.015
    auto overDampedNegTime = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, 1.0f, -0.5f, 1.0f);
    EXPECT_FALSE(overDampedNegTime->WillOverShoot());
}

/**
 * @tc.name: WillOverShootFloatCriticalNavTmpCoeffZero001
 * @tc.desc: Verify WillOverShoot critical with nav*tmpCoeff ≈ 0 (double epsilon guard)
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootFloatCriticalNavTmpCoeffZero001, TestSize.Level1)
{
    // Critical with very large response: nav is tiny, tmpCoeff passes first guard (>1e-6)
    // but nav*tmpCoeff is below double epsilon (2.22e-16) -> second guard triggers -> false
    // nav = 2*PI/1e15 ≈ 6.28e-15, tmpCoeff = 1e-3 (initialVelocity, initialOffset=0)
    // nav*tmpCoeff = 6.28e-18 <= 2.22e-16 -> false
    auto navTmpCoeffZero = std::make_shared<RSSpringModel<float>>(1e15f, 1.0f, 0.0f, 1e-3f, 1.0f);
    EXPECT_FALSE(navTmpCoeffZero->WillOverShoot());
}

/**
 * @tc.name: WillOverShootResponseZero001
 * @tc.desc: Verify WillOverShoot response<=0 guard for Vector3f/Vector4f/Quaternion/Matrix3f/Vector4<Color>
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootResponseZero001, TestSize.Level1)
{
    // Vector3f: response <= 0 -> false
    Vector3f v3Offset(0.0f, 0.0f, 0.0f);
    Vector3f v3Velocity(10.0f, 10.0f, 10.0f);
    auto v3Model = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 2.0f, v3Offset, v3Velocity, 1.0f);
    v3Model->response_ = 0.0f;
    EXPECT_FALSE(v3Model->WillOverShoot());

    // Vector4f: response <= 0 -> false
    Vector4f v4Offset(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4f v4Velocity(10.0f, 10.0f, 10.0f, 10.0f);
    auto v4Model = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 2.0f, v4Offset, v4Velocity, 1.0f);
    v4Model->response_ = 0.0f;
    EXPECT_FALSE(v4Model->WillOverShoot());

    // Quaternion: response <= 0 -> false
    Quaternion qOffset(0.0f, 0.0f, 0.0f, 0.0f);
    Quaternion qVelocity(10.0f, 10.0f, 10.0f, 10.0f);
    auto qModel = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 2.0f, qOffset, qVelocity, 1.0f);
    qModel->response_ = 0.0f;
    EXPECT_FALSE(qModel->WillOverShoot());

    // Matrix3f: response <= 0 -> false
    Matrix3f mOffset = Matrix3f::ZERO;
    Matrix3f mVelocity = Matrix3f::IDENTITY;
    auto mModel = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 2.0f, mOffset, mVelocity, 1.0f);
    mModel->response_ = 0.0f;
    EXPECT_FALSE(mModel->WillOverShoot());

    // Vector4<Color>: response <= 0 -> false
    Color cOffset(0, 0, 0, 0);
    Color cVelocity(100, 100, 100, 100);
    Vector4<Color> v4cOffset(cOffset, cOffset, cOffset, cOffset);
    Vector4<Color> v4cVelocity(cVelocity, cVelocity, cVelocity, cVelocity);
    auto v4cModel = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 2.0f, v4cOffset, v4cVelocity, 1.0f);
    v4cModel->response_ = 0.0f;
    EXPECT_FALSE(v4cModel->WillOverShoot());
}

/**
 * @tc.name: WillOverShootVector2f001
 * @tc.desc: Verify RSSpringModel<Vector2f>::WillOverShoot
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootVector2f001, TestSize.Level1)
{
    Vector2f offset(0.0f, 0.0f);
    Vector2f velocity(10.0f, 10.0f);
    // Underdamped -> false
    auto underDamped = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    // Critical with positive velocity -> true
    auto critical = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    // Critical with zero velocity -> false
    Vector2f zeroV(0.0f, 0.0f);
    auto criticalZero = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
    // Overdamped with positive velocity -> true
    auto overDamped = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    // response <= 0 -> false
    overDamped->response_ = 0.0f;
    EXPECT_FALSE(overDamped->WillOverShoot());
}

/**
 * @tc.name: WillOverShootVector3f001
 * @tc.desc: Verify RSSpringModel<Vector3f>::WillOverShoot
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootVector3f001, TestSize.Level1)
{
    Vector3f offset(0.0f, 0.0f, 0.0f);
    Vector3f velocity(10.0f, 10.0f, 10.0f);
    auto underDamped = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    auto critical = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    auto overDamped = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    Vector3f zeroV(0.0f, 0.0f, 0.0f);
    auto criticalZero = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
}

/**
 * @tc.name: WillOverShootVector4f001
 * @tc.desc: Verify RSSpringModel<Vector4f>::WillOverShoot
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootVector4f001, TestSize.Level1)
{
    Vector4f offset(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4f velocity(10.0f, 10.0f, 10.0f, 10.0f);
    auto underDamped = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    auto critical = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    auto overDamped = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    // Critical with zero velocity -> false
    Vector4f zeroV(0.0f, 0.0f, 0.0f, 0.0f);
    auto criticalZero = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
}

/**
 * @tc.name: WillOverShootQuaternion001
 * @tc.desc: Verify RSSpringModel<Quaternion>::WillOverShoot
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootQuaternion001, TestSize.Level1)
{
    Quaternion offset(0.0f, 0.0f, 0.0f, 0.0f);
    Quaternion velocity(10.0f, 10.0f, 10.0f, 10.0f);
    auto underDamped = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    auto critical = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    auto overDamped = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    // Critical with zero velocity -> false
    Quaternion zeroV(0.0f, 0.0f, 0.0f, 0.0f);
    auto criticalZero = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
}

/**
 * @tc.name: WillOverShootMatrix3f001
 * @tc.desc: Verify RSSpringModel<Matrix3f>::WillOverShoot
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootMatrix3f001, TestSize.Level1)
{
    Matrix3f offset = Matrix3f::ZERO;
    Matrix3f velocity = Matrix3f::IDENTITY;
    auto underDamped = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    auto critical = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    auto overDamped = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    // Critical with zero velocity -> false
    Matrix3f zeroV = Matrix3f::ZERO;
    auto criticalZero = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
}

/**
 * @tc.name: WillOverShootColor001
 * @tc.desc: Verify RSSpringModel<Color>::WillOverShoot for each channel branch
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootColor001, TestSize.Level1)
{
    Color offset(0, 0, 0, 0);
    Color velocity(100, 100, 100, 100);
    // Underdamped -> false
    auto underDamped = std::make_shared<RSSpringModel<Color>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    // Critical with positive velocity -> true
    auto critical = std::make_shared<RSSpringModel<Color>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    // Critical with zero velocity -> false
    Color zeroV(0, 0, 0, 0);
    auto criticalZero = std::make_shared<RSSpringModel<Color>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
    // Overdamped with positive velocity -> true
    auto overDamped = std::make_shared<RSSpringModel<Color>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    // response <= 0 -> false
    overDamped->response_ = 0.0f;
    EXPECT_FALSE(overDamped->WillOverShoot());
}

/**
 * @tc.name: WillOverShootVector4Color001
 * @tc.desc: Verify RSSpringModel<Vector4<Color>>::WillOverShoot
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootVector4Color001, TestSize.Level1)
{
    Vector4<Color> offset(Color(0, 0, 0, 0), Color(0, 0, 0, 0), Color(0, 0, 0, 0), Color(0, 0, 0, 0));
    Vector4<Color> velocity(
        Color(100, 100, 100, 100), Color(100, 100, 100, 100), Color(100, 100, 100, 100), Color(100, 100, 100, 100));
    auto underDamped = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    auto critical = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    auto overDamped = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    // Critical with zero velocity -> false
    Vector4<Color> zeroV(Color(0, 0, 0, 0), Color(0, 0, 0, 0), Color(0, 0, 0, 0), Color(0, 0, 0, 0));
    auto criticalZero = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
}

/**
 * @tc.name: WillOverShootRRect001
 * @tc.desc: Verify RSSpringModel<RRect>::WillOverShoot
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, WillOverShootRRect001, TestSize.Level1)
{
    RectF rect1;
    rect1.SetAll(0.f, 0.f, 0.f, 0.f);
    RectF rect2;
    rect2.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect offset(rect1, 0.f, 0.f);
    RRect velocity(rect2, 1.f, 1.f);
    // Underdamped -> false
    auto underDamped = std::make_shared<RSSpringModel<RRect>>(1.0f, 0.5f, offset, velocity, 1.0f);
    EXPECT_FALSE(underDamped->WillOverShoot());
    // Critical with positive velocity -> true
    auto critical = std::make_shared<RSSpringModel<RRect>>(1.0f, 1.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(critical->WillOverShoot());
    // Overdamped with positive velocity -> true
    auto overDamped = std::make_shared<RSSpringModel<RRect>>(1.0f, 2.0f, offset, velocity, 1.0f);
    EXPECT_TRUE(overDamped->WillOverShoot());
    // Critical with zero velocity -> false
    RRect zeroV(rect1, 0.f, 0.f);
    auto criticalZero = std::make_shared<RSSpringModel<RRect>>(1.0f, 1.0f, offset, zeroV, 1.0f);
    EXPECT_FALSE(criticalZero->WillOverShoot());
    // response <= 0 -> false
    overDamped->response_ = 0.0f;
    EXPECT_FALSE(overDamped->WillOverShoot());
}

/**
 * @tc.name: GetFrameThresholdFloat001
 * @tc.desc: Verify RSSpringModel<float>::GetFrameThreshold for underdamped/critical/overdamped branches
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdFloat001, TestSize.Level1)
{
    // Underdamped: takes the Sqrt path (ROSEN_GE returns false)
    auto underDamped = std::make_shared<RSSpringModel<float>>(1.0f, 0.5f, 1.0f, 1.0f, 1.0f);
    float underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult));

    // Critical: takes the CalculateDisplacement path (ROSEN_GE returns true)
    auto critical = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    float criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(criticalResult));

    // Overdamped: takes the CalculateDisplacement path (ROSEN_GE returns true)
    auto overDamped = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, 1.0f, 1.0f, 1.0f);
    float overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult));

    // time = 0 for underdamped
    float zeroTimeResult = underDamped->GetFrameThreshold(0.0);
    EXPECT_TRUE(std::isfinite(zeroTimeResult));
}

/**
 * @tc.name: GetFrameThresholdVector2f001
 * @tc.desc: Verify RSSpringModel<Vector2f>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdVector2f001, TestSize.Level1)
{
    Vector2f offset(1.0f, 2.0f);
    Vector2f velocity(1.0f, 1.0f);
    // Underdamped -> Sqrt path
    auto underDamped = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult.x_) && std::isfinite(underResult.y_));
    // Critical -> CalculateDisplacement path
    auto critical = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(criticalResult.x_) && std::isfinite(criticalResult.y_));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult.x_) && std::isfinite(overResult.y_));
}

/**
 * @tc.name: GetFrameThresholdVector3f001
 * @tc.desc: Verify RSSpringModel<Vector3f>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdVector3f001, TestSize.Level1)
{
    Vector3f offset(1.0f, 2.0f, 3.0f);
    Vector3f velocity(1.0f, 1.0f, 1.0f);
    auto underDamped = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult.x_) && std::isfinite(underResult.y_) && std::isfinite(underResult.z_));
    auto critical = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(
        std::isfinite(criticalResult.x_) && std::isfinite(criticalResult.y_) && std::isfinite(criticalResult.z_));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<Vector3f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult.x_) && std::isfinite(overResult.y_) && std::isfinite(overResult.z_));
}

/**
 * @tc.name: GetFrameThresholdVector4f001
 * @tc.desc: Verify RSSpringModel<Vector4f>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdVector4f001, TestSize.Level1)
{
    Vector4f offset(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4f velocity(1.0f, 1.0f, 1.0f, 1.0f);
    auto underDamped = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult.x_) && std::isfinite(underResult.y_) && std::isfinite(underResult.z_) &&
                std::isfinite(underResult.w_));
    auto critical = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(criticalResult.x_) && std::isfinite(criticalResult.y_) &&
                std::isfinite(criticalResult.z_) && std::isfinite(criticalResult.w_));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult.x_) && std::isfinite(overResult.y_) && std::isfinite(overResult.z_) &&
                std::isfinite(overResult.w_));
}

/**
 * @tc.name: GetFrameThresholdQuaternion001
 * @tc.desc: Verify RSSpringModel<Quaternion>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdQuaternion001, TestSize.Level1)
{
    Quaternion offset(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion velocity(1.0f, 1.0f, 1.0f, 1.0f);
    auto underDamped = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult.x_) && std::isfinite(underResult.y_));
    auto critical = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(criticalResult.x_) && std::isfinite(criticalResult.y_));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult.x_) && std::isfinite(overResult.y_));
}

/**
 * @tc.name: GetFrameThresholdMatrix3f001
 * @tc.desc: Verify RSSpringModel<Matrix3f>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdMatrix3f001, TestSize.Level1)
{
    Matrix3f offset = Matrix3f::IDENTITY;
    Matrix3f velocity(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    auto underDamped = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    const float* underData = underResult.GetConstData();
    EXPECT_TRUE(std::isfinite(underData[0]));
    auto critical = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    const float* criticalData = criticalResult.GetConstData();
    EXPECT_TRUE(std::isfinite(criticalData[0]));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    const float* overData = overResult.GetConstData();
    EXPECT_TRUE(std::isfinite(overData[0]));
}

/**
 * @tc.name: GetFrameThresholdColor001
 * @tc.desc: Verify RSSpringModel<Color>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdColor001, TestSize.Level1)
{
    Color offset(10, 20, 30, 40);
    Color velocity(5, 5, 5, 5);
    auto underDamped = std::make_shared<RSSpringModel<Color>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult.GetRedF()));
    auto critical = std::make_shared<RSSpringModel<Color>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(criticalResult.GetRedF()));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<Color>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult.GetRedF()));
}

/**
 * @tc.name: GetFrameThresholdVector4Color001
 * @tc.desc: Verify RSSpringModel<Vector4<Color>>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdVector4Color001, TestSize.Level1)
{
    Vector4<Color> offset(Color(10, 20, 30, 40), Color(10, 20, 30, 40), Color(10, 20, 30, 40), Color(10, 20, 30, 40));
    Vector4<Color> velocity(Color(5, 5, 5, 5), Color(5, 5, 5, 5), Color(5, 5, 5, 5), Color(5, 5, 5, 5));
    auto underDamped = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult.data_[0].GetRedF()));
    auto critical = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(criticalResult.data_[0].GetRedF()));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult.data_[0].GetRedF()));
}

/**
 * @tc.name: GetFrameThresholdRRect001
 * @tc.desc: Verify RSSpringModel<RRect>::GetFrameThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, GetFrameThresholdRRect001, TestSize.Level1)
{
    RectF rect1;
    rect1.SetAll(1.f, 2.f, 3.f, 4.f);
    RectF rect2;
    rect2.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect offset(rect1, 1.f, 1.f);
    RRect velocity(rect2, 1.f, 1.f);
    auto underDamped = std::make_shared<RSSpringModel<RRect>>(1.0f, 0.5f, offset, velocity, 1.0f);
    auto underResult = underDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(underResult.rect_.GetLeft()));
    auto critical = std::make_shared<RSSpringModel<RRect>>(1.0f, 1.0f, offset, velocity, 1.0f);
    auto criticalResult = critical->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(criticalResult.rect_.GetLeft()));
    // Overdamped -> CalculateDisplacement path
    auto overDamped = std::make_shared<RSSpringModel<RRect>>(1.0f, 2.0f, offset, velocity, 1.0f);
    auto overResult = overDamped->GetFrameThreshold(1.0);
    EXPECT_TRUE(std::isfinite(overResult.rect_.GetLeft()));
}

/**
 * @tc.name: SqrtFloat001
 * @tc.desc: Verify RSSpringModel<float>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtFloat001, TestSize.Level1)
{
    EXPECT_FLOAT_EQ(RSSpringModel<float>::Sqrt(4.0f), 2.0f);
    EXPECT_FLOAT_EQ(RSSpringModel<float>::Sqrt(9.0f), 3.0f);
    // negative value: sqrt of absolute value
    EXPECT_FLOAT_EQ(RSSpringModel<float>::Sqrt(-4.0f), 2.0f);
    EXPECT_FLOAT_EQ(RSSpringModel<float>::Sqrt(0.0f), 0.0f);
}

/**
 * @tc.name: SqrtVector2f001
 * @tc.desc: Verify RSSpringModel<Vector2f>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtVector2f001, TestSize.Level1)
{
    Vector2f result = RSSpringModel<Vector2f>::Sqrt(Vector2f(4.0f, 9.0f));
    EXPECT_FLOAT_EQ(result.x_, 2.0f);
    EXPECT_FLOAT_EQ(result.y_, 3.0f);
    // negative values
    Vector2f negResult = RSSpringModel<Vector2f>::Sqrt(Vector2f(-4.0f, -9.0f));
    EXPECT_FLOAT_EQ(negResult.x_, 2.0f);
    EXPECT_FLOAT_EQ(negResult.y_, 3.0f);
}

/**
 * @tc.name: SqrtVector3f001
 * @tc.desc: Verify RSSpringModel<Vector3f>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtVector3f001, TestSize.Level1)
{
    Vector3f result = RSSpringModel<Vector3f>::Sqrt(Vector3f(4.0f, 9.0f, 16.0f));
    EXPECT_FLOAT_EQ(result.x_, 2.0f);
    EXPECT_FLOAT_EQ(result.y_, 3.0f);
    EXPECT_FLOAT_EQ(result.z_, 4.0f);
    Vector3f negResult = RSSpringModel<Vector3f>::Sqrt(Vector3f(-4.0f, -9.0f, -16.0f));
    EXPECT_FLOAT_EQ(negResult.x_, 2.0f);
    EXPECT_FLOAT_EQ(negResult.y_, 3.0f);
    EXPECT_FLOAT_EQ(negResult.z_, 4.0f);
}

/**
 * @tc.name: SqrtVector4f001
 * @tc.desc: Verify RSSpringModel<Vector4f>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtVector4f001, TestSize.Level1)
{
    Vector4f result = RSSpringModel<Vector4f>::Sqrt(Vector4f(4.0f, 9.0f, 16.0f, 25.0f));
    EXPECT_FLOAT_EQ(result.x_, 2.0f);
    EXPECT_FLOAT_EQ(result.y_, 3.0f);
    EXPECT_FLOAT_EQ(result.z_, 4.0f);
    EXPECT_FLOAT_EQ(result.w_, 5.0f);
    Vector4f negResult = RSSpringModel<Vector4f>::Sqrt(Vector4f(-4.0f, -9.0f, -16.0f, -25.0f));
    EXPECT_FLOAT_EQ(negResult.x_, 2.0f);
    EXPECT_FLOAT_EQ(negResult.y_, 3.0f);
    EXPECT_FLOAT_EQ(negResult.z_, 4.0f);
    EXPECT_FLOAT_EQ(negResult.w_, 5.0f);
}

/**
 * @tc.name: SqrtQuaternion001
 * @tc.desc: Verify RSSpringModel<Quaternion>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtQuaternion001, TestSize.Level1)
{
    Quaternion result = RSSpringModel<Quaternion>::Sqrt(Quaternion(4.0f, 9.0f, 16.0f, 25.0f));
    EXPECT_FLOAT_EQ(result.x_, 2.0f);
    EXPECT_FLOAT_EQ(result.y_, 3.0f);
    EXPECT_FLOAT_EQ(result.z_, 4.0f);
    EXPECT_FLOAT_EQ(result.w_, 5.0f);
    Quaternion negResult = RSSpringModel<Quaternion>::Sqrt(Quaternion(-4.0f, -9.0f, -16.0f, -25.0f));
    EXPECT_FLOAT_EQ(negResult.x_, 2.0f);
    EXPECT_FLOAT_EQ(negResult.y_, 3.0f);
    EXPECT_FLOAT_EQ(negResult.z_, 4.0f);
    EXPECT_FLOAT_EQ(negResult.w_, 5.0f);
}

/**
 * @tc.name: SqrtMatrix3f001
 * @tc.desc: Verify RSSpringModel<Matrix3f>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtMatrix3f001, TestSize.Level1)
{
    Matrix3f input(4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f, 64.0f, 81.0f, 100.0f);
    Matrix3f result = RSSpringModel<Matrix3f>::Sqrt(input);
    const float* data = result.GetConstData();
    EXPECT_FLOAT_EQ(data[0], 2.0f);
    EXPECT_FLOAT_EQ(data[1], 3.0f);
    EXPECT_FLOAT_EQ(data[2], 4.0f);
    EXPECT_FLOAT_EQ(data[3], 5.0f);
    EXPECT_FLOAT_EQ(data[4], 6.0f);
    EXPECT_FLOAT_EQ(data[5], 7.0f);
    EXPECT_FLOAT_EQ(data[6], 8.0f);
    EXPECT_FLOAT_EQ(data[7], 9.0f);
    EXPECT_FLOAT_EQ(data[8], 10.0f);
    // negative values
    Matrix3f negInput(-4.0f, -9.0f, -16.0f, -25.0f, -36.0f, -49.0f, -64.0f, -81.0f, -100.0f);
    Matrix3f negResult = RSSpringModel<Matrix3f>::Sqrt(negInput);
    const float* negData = negResult.GetConstData();
    EXPECT_FLOAT_EQ(negData[0], 2.0f);
    EXPECT_FLOAT_EQ(negData[8], 10.0f);
}

/**
 * @tc.name: SqrtColor001
 * @tc.desc: Verify RSSpringModel<Color>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtColor001, TestSize.Level1)
{
    Color result = RSSpringModel<Color>::Sqrt(Color(100, 64, 36, 16));
    EXPECT_NEAR(result.GetRedF(), std::sqrt(100.0f / 255.0f), 0.01f);
    EXPECT_NEAR(result.GetGreenF(), std::sqrt(64.0f / 255.0f), 0.01f);
    EXPECT_NEAR(result.GetBlueF(), std::sqrt(36.0f / 255.0f), 0.01f);
    EXPECT_NEAR(result.GetAlphaF(), std::sqrt(16.0f / 255.0f), 0.01f);
    
    Color negResult = RSSpringModel<Color>::Sqrt(Color(-100, -64, -36, -16));
    EXPECT_NEAR(negResult.GetRedF(), std::sqrt(100.0f / 255.0f), 0.01f);
    EXPECT_NEAR(negResult.GetGreenF(), std::sqrt(64.0f / 255.0f), 0.01f);
    EXPECT_NEAR(negResult.GetBlueF(), std::sqrt(36.0f / 255.0f), 0.01f);
    EXPECT_NEAR(negResult.GetAlphaF(), std::sqrt(16.0f / 255.0f), 0.01f);
}

/**
 * @tc.name: SqrtVector4Color001
 * @tc.desc: Verify RSSpringModel<Vector4<Color>>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtVector4Color001, TestSize.Level1)
{
    Vector4<Color> input(
        Color(100, 64, 36, 16), Color(100, 64, 36, 16), Color(100, 64, 36, 16), Color(100, 64, 36, 16));
    Vector4<Color> result = RSSpringModel<Vector4<Color>>::Sqrt(input);
    for (uint32_t i = 0; i < Vector4<Color>::V4SIZE; i++) {
        EXPECT_NEAR(result.data_[i].GetRedF(), std::sqrt(100.0f / 255.0f), 0.01f);
        EXPECT_NEAR(result.data_[i].GetGreenF(), std::sqrt(64.0f / 255.0f), 0.01f);
        EXPECT_NEAR(result.data_[i].GetBlueF(), std::sqrt(36.0f / 255.0f), 0.01f);
        EXPECT_NEAR(result.data_[i].GetAlphaF(), std::sqrt(16.0f / 255.0f), 0.01f);
    }
    Vector4<Color> negInput(
        Color(-100, -64, -36, -16), Color(-100, -64, -36, -16), Color(-100, -64, -36, -16), Color(-100, -64, -36, -16));
    Vector4<Color> negResult = RSSpringModel<Vector4<Color>>::Sqrt(negInput);
    for (uint32_t i = 0; i < Vector4<Color>::V4SIZE; i++) {
        EXPECT_NEAR(negResult.data_[i].GetRedF(), std::sqrt(100.0f / 255.0f), 0.01f);
        EXPECT_NEAR(negResult.data_[i].GetGreenF(), std::sqrt(64.0f / 255.0f), 0.01f);
        EXPECT_NEAR(negResult.data_[i].GetBlueF(), std::sqrt(36.0f / 255.0f), 0.01f);
        EXPECT_NEAR(negResult.data_[i].GetAlphaF(), std::sqrt(16.0f / 255.0f), 0.01f);
    }
}

/**
 * @tc.name: SqrtRRect001
 * @tc.desc: Verify RSSpringModel<RRect>::Sqrt
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, SqrtRRect001, TestSize.Level1)
{
    RectF rect(4.0f, 9.0f, 16.0f, 25.0f);
    RRect input;
    input.rect_ = rect;
    for (int i = 0; i < 4; i++) {
        input.radius_[i] = Vector2f(36.0f, 49.0f);
    }
    RRect result = RSSpringModel<RRect>::Sqrt(input);
    EXPECT_FLOAT_EQ(result.rect_.GetLeft(), 2.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetTop(), 3.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetWidth(), 4.0f);
    EXPECT_FLOAT_EQ(result.rect_.GetHeight(), 5.0f);
    for (int i = 0; i < 4; i++) {
        EXPECT_FLOAT_EQ(result.radius_[i].x_, 6.0f);
        EXPECT_FLOAT_EQ(result.radius_[i].y_, 7.0f);
    }
    // negative values: sqrt of absolute value
    RRect negInput;
    negInput.rect_ = RectF(-4.0f, -9.0f, -16.0f, -25.0f);
    for (int i = 0; i < 4; i++) {
        negInput.radius_[i] = Vector2f(-36.0f, -49.0f);
    }
    RRect negResult = RSSpringModel<RRect>::Sqrt(negInput);
    EXPECT_FLOAT_EQ(negResult.rect_.GetLeft(), 2.0f);
    EXPECT_FLOAT_EQ(negResult.rect_.GetTop(), 3.0f);
    EXPECT_FLOAT_EQ(negResult.rect_.GetWidth(), 4.0f);
    EXPECT_FLOAT_EQ(negResult.rect_.GetHeight(), 5.0f);
    for (int i = 0; i < 4; i++) {
        EXPECT_FLOAT_EQ(negResult.radius_[i].x_, 6.0f);
        EXPECT_FLOAT_EQ(negResult.radius_[i].y_, 7.0f);
    }
}
} // namespace Rosen
} // namespace OHOS
