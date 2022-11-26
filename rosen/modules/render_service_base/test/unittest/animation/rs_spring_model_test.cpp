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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<float>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != 0.0f);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<float>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != 0.0f);
    duration = model3->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Color>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Color>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Matrix3f>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Vector2f>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Vector4f>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Quaternion>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<Vector4<Color>>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelVector4ColorTest001 end";
}

/**
 * @tc.name: RSSpringModelRSFilterTest001
 * @tc.desc: Verify the RSSpringModelRSFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringModelTest, RSSpringModelRSFilterTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRSFilterTest001 start";

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(1.0f, 2.0f, nullptr, nullptr, 1.0f);
    auto result = model1->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result == nullptr);
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto initialOffset = RSFilter::CreateBlurFilter(0.f, 0.f);
    auto initialVelocity = RSFilter::CreateBlurFilter(10.f, 10.f);

    auto model2 =
        std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(0.0f, 0.0f, initialOffset, initialVelocity, 0.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 =
        std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(1.0f, 1.0f, initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model4 =
        std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(1.0f, 2.0f, initialOffset, initialVelocity, 1.0f);
    result = model4->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model4->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRSFilterTest001 end";
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
    auto duration = model1->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model2 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(1.0f, 1.0f,
        initialOffset, initialVelocity, 1.0f);
    result = model2->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model2->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    auto model3 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(1.0f, 2.0f,
        initialOffset, initialVelocity, 1.0f);
    result = model3->CalculateDisplacement(1.0f);
    EXPECT_TRUE(result != initialOffset);
    duration = model3->GetEstimatedDuration();
    EXPECT_TRUE(duration != 0.0f);

    GTEST_LOG_(INFO) << "RSSpringModelTest RSSpringModelRSRSRenderPropertyBaseTest001 end";
}
} // namespace Rosen
} // namespace OHOS