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
#include "include/animation/rs_steps_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSStepsInterpolatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSStepsInterpolatorTest::SetUpTestCase() {}
void RSStepsInterpolatorTest::TearDownTestCase() {}
void RSStepsInterpolatorTest::SetUp() {}
void RSStepsInterpolatorTest::TearDown() {}

/**
 * @tc.name: StepsCurveTest001
 * @tc.desc: test results of StepsCurve
 * @tc.type: FUNC
 */
HWTEST_F(RSStepsInterpolatorTest, StepsCurveTest001, TestSize.Level1)
{
    int32_t steps = 4;
    float stepValue = 1.0f / steps;
    RSStepsInterpolator stepsCurveStart(steps, StepsCurvePosition::START);
    /**
     * @tc.steps: step1. set the testValue of time to test steps Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue of time to test steps Curve.
     */
    float targetValueFirst = stepValue;
    float targetValueSecond = stepValue * 3;
    float targetValueThird = stepValue * 4;

    /**
     * @tc.steps: step3. verify the result of steps Curve.
     * @tc.expected: step3. the result is right.
     */
    ASSERT_EQ(targetValueFirst, stepsCurveStart.Interpolate(testValueFirst));
    ASSERT_EQ(targetValueSecond, stepsCurveStart.Interpolate(testValueSecond));
    ASSERT_EQ(targetValueThird, stepsCurveStart.Interpolate(testValueThird));
}

/**
 * @tc.name: StepsCurveTest002
 * @tc.desc: test results of StepsCurve
 * @tc.type: FUNC
 */
HWTEST_F(RSStepsInterpolatorTest, StepsCurveTest002, TestSize.Level1)
{
    int32_t steps = 4;
    float stepValue = 1.0f / steps;
    RSStepsInterpolator stepsCurveEnd(steps, StepsCurvePosition::END);
    /**
     * @tc.steps: step1. set the testValue of time to test steps Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.5f;
    float testValueThird = 0.8f;

    /**
     * @tc.steps: step2. set the targetValue of time to test steps Curve.
     */
    float targetValueFirst = 0;
    float targetValueSecond = stepValue * 2;
    float targetValueThird = stepValue * 3;

    /**
     * @tc.steps: step3. verify the result of steps Curve.
     * @tc.expected: step3. the result is right.
     */
    ASSERT_EQ(targetValueFirst, stepsCurveEnd.Interpolate(testValueFirst));
    ASSERT_EQ(targetValueSecond, stepsCurveEnd.Interpolate(testValueSecond));
    ASSERT_EQ(targetValueThird, stepsCurveEnd.Interpolate(testValueThird));
}

/**
 * @tc.name: StepsCurveTest003
 * @tc.desc: test results of Interpolate
 * @tc.type: FUNC
 */
HWTEST_F(RSStepsInterpolatorTest, StepsCurveTest003, TestSize.Level1)
{
    RSStepsInterpolator stepsCurveStart(0);
    /**
     * @tc.steps: step1. set the testValue of time to test steps Curve.
     */
    float testValueFirst = 0.2f;
    float testValueSecond = 0.8f;

    /**
     * @tc.steps: step2. verify the result of steps Curve.
     * @tc.expected: step2. the result is right.
     */
    ASSERT_EQ(1.0f, stepsCurveStart.Interpolate(testValueFirst));
    ASSERT_EQ(1.0f, stepsCurveStart.Interpolate(testValueSecond));
}

/**
 * @tc.name: InterpolateImplTest
 * @tc.desc: test results of InterpolateImpl
 * @tc.type: FUNC
 */
HWTEST_F(RSStepsInterpolatorTest, InterpolateImplTest, TestSize.Level1)
{
    RSStepsInterpolator stepsCurveStart(0, 0);
    RSStepsInterpolator stepsCurveStart2(0, 1);
    float fraction = -1.0f;
    float res = stepsCurveStart.InterpolateImpl(fraction);
    EXPECT_EQ(res, 1.0f);

    fraction = 3.0f;
    res = stepsCurveStart.InterpolateImpl(fraction);
    EXPECT_EQ(res, 1.0f);

    fraction = 0.0f;
    res = stepsCurveStart.InterpolateImpl(fraction);
    EXPECT_EQ(res, 1.0f);

    res = stepsCurveStart2.InterpolateImpl(fraction);
    EXPECT_EQ(res, 1.0f);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSStepsInterpolatorTest, MarshallingTest, TestSize.Level1)
{
    RSStepsInterpolator stepsCurveStart(0);
    Parcel parcel;
    bool res = stepsCurveStart.Marshalling(parcel);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSStepsInterpolatorTest, UnmarshallingTest, TestSize.Level1)
{
    RSStepsInterpolator stepsCurveStart(0);
    Parcel parcel;
    RSStepsInterpolator* step;
    step = stepsCurveStart.Unmarshalling(parcel);
    ASSERT_EQ(step, nullptr);
}
} // namespace OHOS::Rosen