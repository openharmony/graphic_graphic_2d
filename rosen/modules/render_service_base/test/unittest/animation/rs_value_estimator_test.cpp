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

#include "animation/rs_steps_interpolator.h"
#include "animation/rs_value_estimator.h"
#include "modifier/rs_render_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSValueEstimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSValueEstimatorTest::SetUpTestCase() {}
void RSValueEstimatorTest::TearDownTestCase() {}
void RSValueEstimatorTest::SetUp() {}
void RSValueEstimatorTest::TearDown() {}

/**
 * @tc.name: EstimateTest001
 * @tc.desc: Verify the Estimate
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, EstimateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSValueEstimatorTest EstimateTest001 start";
    auto startValue = RSFilter::CreateBlurFilter(1.f, 1.f);
    auto endValue = RSFilter::CreateBlurFilter(10.f, 10.f);
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<float>>();

    auto filter1 = curveValueEstimator->Estimate(0.0f, nullptr, endValue);
    EXPECT_TRUE(filter1 != nullptr);

    auto filter2 = curveValueEstimator->Estimate(0.0f, startValue, nullptr);
    EXPECT_TRUE(filter2 != nullptr);

    auto filter3 = curveValueEstimator->Estimate(0.1f, startValue, endValue);
    EXPECT_TRUE(filter3 != nullptr);

    auto endValue2 = RSFilter::CreateMaterialFilter(1, 0.1f);
    auto filter4 = curveValueEstimator->Estimate(0.1f, startValue, endValue2);
    EXPECT_TRUE(filter4 != nullptr);

    auto endValue3 = RSFilter::CreateMaterialFilter(1, 0.1f);
    auto filter5 = curveValueEstimator->Estimate(0.1f, nullptr, endValue3);
    EXPECT_TRUE(filter5 != nullptr);
    GTEST_LOG_(INFO) << "RSValueEstimatorTest EstimateTest001 end";
}

/**
 * @tc.name: EstimateFraction001
 * @tc.desc: Verify the EstimateFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, EstimateFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSValueEstimatorTest EstimateFraction001 start";

    auto interpolator = std::make_shared<RSStepsInterpolator>(1, StepsCurvePosition::START);
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<float>>();

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    curveValueEstimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    auto result = curveValueEstimator->EstimateFraction(interpolator);
    EXPECT_TRUE(result != 0.0f);

    GTEST_LOG_(INFO) << "RSValueEstimatorTest EstimateFraction001 end";
}

} // namespace Rosen
} // namespace OHOS