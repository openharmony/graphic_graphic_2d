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

#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSInterpolatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t CUSTOM_DURATION = 1000;
};

void RSInterpolatorTest::SetUpTestCase() {}
void RSInterpolatorTest::TearDownTestCase() {}
void RSInterpolatorTest::SetUp() {}
void RSInterpolatorTest::TearDown() {}

/**
 * @tc.name: RSStepInterpolatorTest001
 * @tc.desc: Verify the RSStepInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSStepInterpolatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSStepInterpolatorTest001 start";

    Parcel parcel1;
    std::shared_ptr<RSInterpolator> interpolator(RSStepsInterpolator::Unmarshalling(parcel1));
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel2;
    parcel2.WriteInt32(1);
    interpolator.reset(RSStepsInterpolator::Unmarshalling(parcel2));
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel3;
    parcel3.WriteUint16(InterpolatorType::STEPS);
    parcel3.WriteUint64(123123);
    parcel3.WriteInt32(1);
    parcel3.WriteInt32(0);
    interpolator.reset(RSStepsInterpolator::Unmarshalling(parcel3));
    EXPECT_TRUE(interpolator != nullptr);

    GTEST_LOG_(INFO) << "RSInterpolatorTest RSStepInterpolatorTest001 end";
}

/**
 * @tc.name: RSSpringInterpolatorTest001
 * @tc.desc: Verify the RSSpringInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSSpringInterpolatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest001 start";

    Parcel parcel1;
    std::shared_ptr<RSInterpolator> interpolator(RSSpringInterpolator::Unmarshalling(parcel1));
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    interpolator.reset(RSSpringInterpolator::Unmarshalling(parcel2));
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    interpolator.reset(RSSpringInterpolator::Unmarshalling(parcel3));
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel4;
    parcel4.WriteUint16(InterpolatorType::SPRING);
    parcel4.WriteUint64(123124);
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    interpolator.reset(RSSpringInterpolator::Unmarshalling(parcel4));
    EXPECT_TRUE(interpolator != nullptr);

    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest001 end";
}

/**
 * @tc.name: RSSpringInterpolatorTest002
 * @tc.desc: Verify the RSSpringInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSSpringInterpolatorTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest002 start";

    RSSpringInterpolator interpolator(0.1f, 1.0f, 1.0f);
    auto result = interpolator.Interpolate(0.0f);
    EXPECT_EQ(result, 0.0f);

    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest002 end";
}

/**
 * @tc.name: RSCustomInterpolatorTest001
 * @tc.desc: Verify the RSCustomInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSCustomInterpolatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSCustomInterpolatorTest001 start";

    auto lambda = [](float value) -> bool { return value; };
    RSCustomInterpolator interpolator2(lambda, CUSTOM_DURATION);
    auto result = interpolator2.Interpolate(CUSTOM_DURATION);
    EXPECT_EQ(result, 1.0f);
    result = interpolator2.Interpolate(1.0f);
    EXPECT_EQ(result, 1.0f);

    GTEST_LOG_(INFO) << "RSInterpolatorTest RSCustomInterpolatorTest001 end";
}

/**
 * @tc.name: Convert001
 * @tc.desc: Verify the Convert of RSCustomInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, Convert001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest Convert001 start";
    std::function<float(float)> func = nullptr;
    auto interpolator = std::make_shared<RSCustomInterpolator>(func, CUSTOM_DURATION);
    EXPECT_TRUE(interpolator != nullptr);
    GTEST_LOG_(INFO) << "RSInterpolatorTest Convert001 end";
}

/**
 * @tc.name: Interpolate001
 * @tc.desc: Verify the Interpolate of RSCustomInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, Interpolate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest Interpolate001 start";
    std::vector<float> times = { 1.0f, 3.0f };
    std::vector<float> values = { 1.0f, 3.0f };
    Parcel parcel;
    parcel.WriteUint64(123125);
    parcel.WriteFloatVector(times);
    parcel.WriteFloatVector(values);
    std::shared_ptr<RSInterpolator> interpolator(RSCustomInterpolator::Unmarshalling(parcel));
    EXPECT_TRUE(interpolator != nullptr);
    if (interpolator != nullptr) {
        float input = -1.0f;
        float result = interpolator->Interpolate(input);
        EXPECT_EQ(result, -1.0f);
        input = 2.0f;
        result = interpolator->Interpolate(input);
        EXPECT_EQ(result, 2.0f);
        input = 4.0f;
        result = interpolator->Interpolate(input);
        EXPECT_EQ(result, 3.0f);
    }
    GTEST_LOG_(INFO) << "RSInterpolatorTest Interpolate001 end";
}
} // namespace Rosen
} // namespace OHOS