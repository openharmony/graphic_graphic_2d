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

#include "animation/rs_cubic_bezier_interpolator.h"
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

/**
 * @tc.name: RSSpringInterpolatorUnmarshallingNaNInf001
 * @tc.desc: Verify RSSpringInterpolator Unmarshalling rejects NaN and Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSSpringInterpolatorUnmarshallingNaNInf001, TestSize.Level1)
{
    Parcel parcelResponseNaN;
    parcelResponseNaN.WriteUint64(123124);
    parcelResponseNaN.WriteFloat(NAN);
    parcelResponseNaN.WriteFloat(1.0f);
    parcelResponseNaN.WriteFloat(1.0f);
    EXPECT_EQ(RSSpringInterpolator::Unmarshalling(parcelResponseNaN), nullptr);

    Parcel parcelResponseInf;
    parcelResponseInf.WriteUint64(123124);
    parcelResponseInf.WriteFloat(INFINITY);
    parcelResponseInf.WriteFloat(1.0f);
    parcelResponseInf.WriteFloat(1.0f);
    EXPECT_EQ(RSSpringInterpolator::Unmarshalling(parcelResponseInf), nullptr);

    Parcel parcelDampingNaN;
    parcelDampingNaN.WriteUint64(123124);
    parcelDampingNaN.WriteFloat(1.0f);
    parcelDampingNaN.WriteFloat(NAN);
    parcelDampingNaN.WriteFloat(1.0f);
    EXPECT_EQ(RSSpringInterpolator::Unmarshalling(parcelDampingNaN), nullptr);

    Parcel parcelDampingInf;
    parcelDampingInf.WriteUint64(123124);
    parcelDampingInf.WriteFloat(1.0f);
    parcelDampingInf.WriteFloat(INFINITY);
    parcelDampingInf.WriteFloat(1.0f);
    EXPECT_EQ(RSSpringInterpolator::Unmarshalling(parcelDampingInf), nullptr);

    Parcel parcelVelocityNaN;
    parcelVelocityNaN.WriteUint64(123124);
    parcelVelocityNaN.WriteFloat(1.0f);
    parcelVelocityNaN.WriteFloat(1.0f);
    parcelVelocityNaN.WriteFloat(NAN);
    EXPECT_EQ(RSSpringInterpolator::Unmarshalling(parcelVelocityNaN), nullptr);

    Parcel parcelVelocityInf;
    parcelVelocityInf.WriteUint64(123124);
    parcelVelocityInf.WriteFloat(1.0f);
    parcelVelocityInf.WriteFloat(1.0f);
    parcelVelocityInf.WriteFloat(INFINITY);
    EXPECT_EQ(RSSpringInterpolator::Unmarshalling(parcelVelocityInf), nullptr);
}

/**
 * @tc.name: RSCubicBezierInterpolatorUnmarshallingNaNInf001
 * @tc.desc: Verify RSCubicBezierInterpolator Unmarshalling rejects NaN/Inf for x1/y1
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSCubicBezierInterpolatorUnmarshallingNaNInf001, TestSize.Level1)
{
    Parcel parcelX1NaN;
    parcelX1NaN.WriteUint64(123123);
    parcelX1NaN.WriteFloat(NAN);
    parcelX1NaN.WriteFloat(0.0f);
    parcelX1NaN.WriteFloat(1.0f);
    parcelX1NaN.WriteFloat(1.0f);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelX1NaN), nullptr);

    Parcel parcelX1Inf;
    parcelX1Inf.WriteUint64(123123);
    parcelX1Inf.WriteFloat(INFINITY);
    parcelX1Inf.WriteFloat(0.0f);
    parcelX1Inf.WriteFloat(1.0f);
    parcelX1Inf.WriteFloat(1.0f);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelX1Inf), nullptr);

    Parcel parcelY1NaN;
    parcelY1NaN.WriteUint64(123123);
    parcelY1NaN.WriteFloat(0.0f);
    parcelY1NaN.WriteFloat(NAN);
    parcelY1NaN.WriteFloat(1.0f);
    parcelY1NaN.WriteFloat(1.0f);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelY1NaN), nullptr);

    Parcel parcelY1Inf;
    parcelY1Inf.WriteUint64(123123);
    parcelY1Inf.WriteFloat(0.0f);
    parcelY1Inf.WriteFloat(INFINITY);
    parcelY1Inf.WriteFloat(1.0f);
    parcelY1Inf.WriteFloat(1.0f);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelY1Inf), nullptr);
}

/**
 * @tc.name: RSCubicBezierInterpolatorUnmarshallingNaNInf002
 * @tc.desc: Verify RSCubicBezierInterpolator Unmarshalling rejects NaN/Inf for x2/y2
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSCubicBezierInterpolatorUnmarshallingNaNInf002, TestSize.Level1)
{
    Parcel parcelX2NaN;
    parcelX2NaN.WriteUint64(123123);
    parcelX2NaN.WriteFloat(0.0f);
    parcelX2NaN.WriteFloat(0.0f);
    parcelX2NaN.WriteFloat(NAN);
    parcelX2NaN.WriteFloat(1.0f);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelX2NaN), nullptr);

    Parcel parcelX2Inf;
    parcelX2Inf.WriteUint64(123123);
    parcelX2Inf.WriteFloat(0.0f);
    parcelX2Inf.WriteFloat(0.0f);
    parcelX2Inf.WriteFloat(INFINITY);
    parcelX2Inf.WriteFloat(1.0f);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelX2Inf), nullptr);

    Parcel parcelY2NaN;
    parcelY2NaN.WriteUint64(123123);
    parcelY2NaN.WriteFloat(0.0f);
    parcelY2NaN.WriteFloat(0.0f);
    parcelY2NaN.WriteFloat(1.0f);
    parcelY2NaN.WriteFloat(NAN);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelY2NaN), nullptr);

    Parcel parcelY2Inf;
    parcelY2Inf.WriteUint64(123123);
    parcelY2Inf.WriteFloat(0.0f);
    parcelY2Inf.WriteFloat(0.0f);
    parcelY2Inf.WriteFloat(1.0f);
    parcelY2Inf.WriteFloat(INFINITY);
    EXPECT_EQ(RSCubicBezierInterpolator::Unmarshalling(parcelY2Inf), nullptr);
}

/**
 * @tc.name: RSSpringInterpolatorConstructorNaNInf001
 * @tc.desc: Verify RSSpringInterpolator constructor NaN/Inf fallback to defaults
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSSpringInterpolatorConstructorNaNInf001, TestSize.Level1)
{
    auto interp1 = RSSpringInterpolator(NAN, 1.0f, 0.0f);
    EXPECT_TRUE(interp1.estimatedDuration_ > 0.0f);

    auto interp2 = RSSpringInterpolator(INFINITY, 1.0f, 0.0f);
    EXPECT_TRUE(interp2.estimatedDuration_ > 0.0f);

    auto interp3 = RSSpringInterpolator(1.0f, NAN, 0.0f);
    EXPECT_TRUE(interp3.estimatedDuration_ > 0.0f);

    auto interp4 = RSSpringInterpolator(1.0f, INFINITY, 0.0f);
    EXPECT_TRUE(interp4.estimatedDuration_ > 0.0f);

    auto interp5 = RSSpringInterpolator(1.0f, 1.0f, NAN);
    EXPECT_TRUE(interp5.estimatedDuration_ > 0.0f);

    auto interp6 = RSSpringInterpolator(1.0f, 1.0f, INFINITY);
    EXPECT_TRUE(interp6.estimatedDuration_ > 0.0f);

    auto interp7 = RSSpringInterpolator(1.0f, 1.0f, 0.0f);
    EXPECT_TRUE(interp7.estimatedDuration_ > 0.0f);
}

/**
 * @tc.name: RSCubicBezierInterpolatorConstructorNaNInf001
 * @tc.desc: Verify RSCubicBezierInterpolator constructor NaN/Inf fallback to defaults
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSCubicBezierInterpolatorConstructorNaNInf001, TestSize.Level1)
{
    auto interp1 = RSCubicBezierInterpolator(NAN, 0.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(interp1.controlX1_, 0.42f);

    auto interp2 = RSCubicBezierInterpolator(INFINITY, 0.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(interp2.controlX1_, 0.42f);

    auto interp3 = RSCubicBezierInterpolator(0.0f, NAN, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(interp3.controlY1_, 0.0f);

    auto interp4 = RSCubicBezierInterpolator(0.0f, INFINITY, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(interp4.controlY1_, 0.0f);

    auto interp5 = RSCubicBezierInterpolator(0.0f, 0.0f, NAN, 1.0f);
    EXPECT_FLOAT_EQ(interp5.controlX2_, 0.58f);

    auto interp6 = RSCubicBezierInterpolator(0.0f, 0.0f, INFINITY, 1.0f);
    EXPECT_FLOAT_EQ(interp6.controlX2_, 0.58f);

    auto interp7 = RSCubicBezierInterpolator(0.0f, 0.0f, 1.0f, NAN);
    EXPECT_FLOAT_EQ(interp7.controlY2_, 1.0f);

    auto interp8 = RSCubicBezierInterpolator(0.0f, 0.0f, 1.0f, INFINITY);
    EXPECT_FLOAT_EQ(interp8.controlY2_, 1.0f);

    auto interp9 = RSCubicBezierInterpolator(0.0f, 0.0f, 1.0f, 1.0f);
    EXPECT_FLOAT_EQ(interp9.controlX1_, 0.0f);
    EXPECT_FLOAT_EQ(interp9.controlY1_, 0.0f);
    EXPECT_FLOAT_EQ(interp9.controlX2_, 1.0f);
    EXPECT_FLOAT_EQ(interp9.controlY2_, 1.0f);
}
} // namespace Rosen
} // namespace OHOS