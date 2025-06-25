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
#include "transaction/rs_marshalling_helper.h"

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
    RSMarshallingHelper::Marshalling(parcel, times);
    RSMarshallingHelper::Marshalling(parcel, values);
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

/**
 * @tc.name: Interpolate002
 * @tc.desc: Verify the Interpolate of RSCustomInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, Interpolate002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest Interpolate002 start";
    std::vector<float> times = {};
    std::vector<float> values = { 1.0f, 3.0f };
    Parcel parcel;
    parcel.WriteUint64(123125);
    RSMarshallingHelper::Marshalling(parcel, times);
    RSMarshallingHelper::Marshalling(parcel, values);
    std::shared_ptr<RSInterpolator> interpolator(RSCustomInterpolator::Unmarshalling(parcel));
    EXPECT_TRUE(interpolator != nullptr);
    if (interpolator != nullptr) {
        float input = -1.0f;
        float result = interpolator->Interpolate(input);
        EXPECT_EQ(result, -1.0f);
        input = 2.0f;
        result = interpolator->Interpolate(input);
        EXPECT_EQ(result, 0.0f);
        input = 4.0f;
        result = interpolator->Interpolate(input);
        EXPECT_EQ(result, 0.0f);
    }

    std::vector<float> times1 = { 1.0f };
    std::vector<float> values1 = { 1.0f, 3.0f };
    Parcel parcel1;
    parcel1.WriteUint64(123125);
    RSMarshallingHelper::Marshalling(parcel1, times1);
    RSMarshallingHelper::Marshalling(parcel1, values1);
    std::shared_ptr<RSInterpolator> interpolator1(RSCustomInterpolator::Unmarshalling(parcel1));
    EXPECT_TRUE(interpolator1 != nullptr);
    if (interpolator1 != nullptr) {
        float input = -1.0f;
        float result = interpolator1->Interpolate(input);
        EXPECT_EQ(result, -1.0f);
        input = 2.0f;
        result = interpolator1->Interpolate(input);
        EXPECT_EQ(result, 0.0f);
        input = 4.0f;
        result = interpolator1->Interpolate(input);
        EXPECT_EQ(result, 0.0f);
    }
    GTEST_LOG_(INFO) << "RSInterpolatorTest Interpolate002 end";
}

/**
 * @tc.name: Interpolate003
 * @tc.desc: Verify the Interpolate of RSCustomInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, Interpolate003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest Interpolate003 start";
    std::vector<float> times = { 1.0f, 3.0f };
    std::vector<float> values = { 1.0f, 8.0f };
    Parcel parcel;
    parcel.WriteUint64(123125);
    RSMarshallingHelper::Marshalling(parcel, times);
    RSMarshallingHelper::Marshalling(parcel, values);
    std::shared_ptr<RSInterpolator> interpolator(RSCustomInterpolator::Unmarshalling(parcel));
    EXPECT_TRUE(interpolator != nullptr);
    if (interpolator != nullptr) {
        float input = -1.0f;
        float result = interpolator->Interpolate(input);
        EXPECT_EQ(result, -1.0f);
        input = 0.5f;
        result = interpolator->Interpolate(input);
        EXPECT_EQ(result, 1.0f);
        input = 5.0f;
        result = interpolator->Interpolate(input);
        EXPECT_EQ(result, 8.0f);
    }
    GTEST_LOG_(INFO) << "RSInterpolatorTest Interpolate003 end";
}

/**
 * @tc.name: RSCustomInterpolatorUnmarshallingTest001
 * @tc.desc: Verify the RSCustomInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSCustomInterpolatorUnmarshallingTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSCustomInterpolatorUnmarshallingTest001 start";
    std::vector<float> times;
    std::vector<float> values;
    for (int i = 0; i < 300; i++) {
        times.push_back(1.0f);
        values.push_back(1.0f);
    }
    Parcel parcel1;
    parcel1.WriteUint64(123125);
    RSMarshallingHelper::Marshalling(parcel1, times);
    RSMarshallingHelper::Marshalling(parcel1, values);
    std::shared_ptr<RSInterpolator> interpolator1(RSCustomInterpolator::Unmarshalling(parcel1));
    EXPECT_TRUE(interpolator1 != nullptr);

    times.push_back(1.0f);
    values.push_back(1.0f);
    Parcel parcel2;
    parcel2.WriteUint64(123125);
    RSMarshallingHelper::Marshalling(parcel2, times);
    RSMarshallingHelper::Marshalling(parcel2, values);
    std::shared_ptr<RSInterpolator> interpolator2(RSCustomInterpolator::Unmarshalling(parcel2));
    EXPECT_TRUE(interpolator2 == nullptr);
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSCustomInterpolatorUnmarshallingTest001 end";
}

/**
 * @tc.name: RSCustomInterpolatorMarshallingTest001
 * @tc.desc: Verify the Interpolate of RSCustomInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSCustomInterpolatorMarshallingTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSCustomInterpolatorMarshallingTest001 start";
    std::vector<float> times = { 1.0f, 3.0f };
    std::vector<float> values = { 1.0f, 3.0f };
    auto interpolator = std::make_shared<RSCustomInterpolator>(123125, std::move(times), std::move(values));
    Parcel parcel;
    bool suc = interpolator->Marshalling(parcel);
    EXPECT_TRUE(suc);
    uint16_t interpolatorType = 0;
    parcel.ReadUint16(interpolatorType);
    std::shared_ptr<RSCustomInterpolator> copyInterpolator(RSCustomInterpolator::Unmarshalling(parcel));
    EXPECT_TRUE(copyInterpolator != nullptr);
    if (copyInterpolator != nullptr) {
        float input = -1.0f;
        float result = copyInterpolator->Interpolate(input);
        EXPECT_EQ(result, -1.0f);
        input = 2.0f;
        result = copyInterpolator->Interpolate(input);
        EXPECT_EQ(result, 2.0f);
        input = 4.0f;
        result = copyInterpolator->Interpolate(input);
        EXPECT_EQ(result, 3.0f);

        EXPECT_EQ(copyInterpolator->times_.size(), times.size());
        EXPECT_EQ(copyInterpolator->values_.size(), values.size());
        EXPECT_EQ(copyInterpolator->times_[0], times[0]);
        EXPECT_EQ(copyInterpolator->times_[1], times[1]);
        EXPECT_EQ(copyInterpolator->values_[0], values[0]);
        EXPECT_EQ(copyInterpolator->values_[1], values[1]);
    }
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSCustomInterpolatorMarshallingTest001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Interpolate of RSInterpolator Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest Unmarshalling001 start";
    Parcel parcel1;
    parcel1.WriteUint16(InterpolatorType::LINEAR);
    std::shared_ptr<RSInterpolator> interpolator1(RSInterpolator::Unmarshalling(parcel1));
    EXPECT_EQ(interpolator1, nullptr);

    Parcel parcel2;
    parcel2.WriteUint16(InterpolatorType::CUSTOM);
    std::shared_ptr<RSInterpolator> interpolator2(RSInterpolator::Unmarshalling(parcel2));
    EXPECT_FALSE(interpolator2 != nullptr);

    Parcel parcel3;
    parcel3.WriteUint16(InterpolatorType::CUBIC_BEZIER);
    std::shared_ptr<RSInterpolator> interpolator3(RSInterpolator::Unmarshalling(parcel3));
    EXPECT_EQ(interpolator3, nullptr);

    Parcel parcel4;
    parcel4.WriteUint16(InterpolatorType::SPRING);
    std::shared_ptr<RSInterpolator> interpolator4(RSInterpolator::Unmarshalling(parcel4));
    EXPECT_EQ(interpolator4, nullptr);

    Parcel parcel5;
    parcel5.WriteUint16(InterpolatorType::STEPS);
    std::shared_ptr<RSInterpolator> interpolator5(RSInterpolator::Unmarshalling(parcel5));
    EXPECT_EQ(interpolator5, nullptr);

    GTEST_LOG_(INFO) << "RSInterpolatorTest Unmarshalling001 end";
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Verify the Interpolate of RSInterpolator Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, Unmarshalling002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest Unmarshalling002 start";
    Parcel parcel;
    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel1;
    parcel1.WriteUint16(InterpolatorType::LINEAR);
    std::shared_ptr<RSInterpolator> interpolator1(RSInterpolator::UnmarshallingFromParcel(parcel1));
    EXPECT_EQ(interpolator1, nullptr);

    Parcel parcel2;
    parcel2.WriteUint16(InterpolatorType::CUSTOM);
    std::shared_ptr<RSInterpolator> interpolator2(RSInterpolator::UnmarshallingFromParcel(parcel2));
    EXPECT_FALSE(interpolator2 != nullptr);

    Parcel parcel3;
    parcel3.WriteUint16(InterpolatorType::CUBIC_BEZIER);
    std::shared_ptr<RSInterpolator> interpolator3(RSInterpolator::UnmarshallingFromParcel(parcel3));
    EXPECT_EQ(interpolator3, nullptr);

    Parcel parcel4;
    parcel4.WriteUint16(InterpolatorType::SPRING);
    std::shared_ptr<RSInterpolator> interpolator4(RSInterpolator::UnmarshallingFromParcel(parcel4));
    EXPECT_EQ(interpolator4, nullptr);

    Parcel parcel5;
    parcel5.WriteUint16(InterpolatorType::STEPS);
    std::shared_ptr<RSInterpolator> interpolator5(RSInterpolator::UnmarshallingFromParcel(parcel5));
    EXPECT_EQ(interpolator5, nullptr);

    Parcel parcel6;
    std::shared_ptr<RSInterpolator> interpolator6(RSInterpolator::UnmarshallingFromParcel(parcel6));
    EXPECT_EQ(interpolator6, nullptr);

    GTEST_LOG_(INFO) << "RSInterpolatorTest Unmarshalling002 end";
}
} // namespace Rosen
} // namespace OHOS