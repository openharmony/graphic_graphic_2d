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

    std::shared_ptr<RSStepsInterpolator> interpolator2;
    result = curveValueEstimator->EstimateFraction(interpolator2);
    EXPECT_TRUE(result == FRACTION_MIN);
    GTEST_LOG_(INFO) << "RSValueEstimatorTest EstimateFraction001 end";
}

/**
 * @tc.name: InitCurveAnimationValueTest
 * @tc.desc: Verify the InitCurveAnimationValue
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValueTest, TestSize.Level1)
{
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<SimpleDrawCmdListPtr>>();

    auto property = std::make_shared<RSRenderAnimatableProperty<SimpleDrawCmdListPtr>>();
    auto startValue = std::make_shared<RSRenderAnimatableProperty<SimpleDrawCmdListPtr>>();
    auto endValue = std::make_shared<RSRenderAnimatableProperty<SimpleDrawCmdListPtr>>();
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<SimpleDrawCmdListPtr>>();

    curveValueEstimator->InitCurveAnimationValue(nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(curveValueEstimator->property_, nullptr);

    curveValueEstimator->InitCurveAnimationValue(property, nullptr, nullptr, nullptr);
    EXPECT_EQ(curveValueEstimator->property_, nullptr);

    curveValueEstimator->InitCurveAnimationValue(nullptr, nullptr, endValue, nullptr);
    EXPECT_EQ(curveValueEstimator->property_, nullptr);

    curveValueEstimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    EXPECT_NE(curveValueEstimator->property_, nullptr);
}

/**
 * @tc.name: UpdateAnimationValueTest
 * @tc.desc: Verify the UpdateAnimationValue
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, UpdateAnimationValueTest, TestSize.Level1)
{
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<SimpleDrawCmdListPtr>>();
    EXPECT_EQ(curveValueEstimator->property_, nullptr);
    curveValueEstimator->UpdateAnimationValue(0.f, true);

    auto property = std::make_shared<RSRenderAnimatableProperty<SimpleDrawCmdListPtr>>();
    curveValueEstimator->property_ = std::make_shared<RSRenderAnimatableProperty<SimpleDrawCmdListPtr>>();

    auto animationValue = curveValueEstimator->property_->Get();
    EXPECT_EQ(animationValue, nullptr);
    curveValueEstimator->UpdateAnimationValue(0.f, true);

    SimpleDrawCmdListPtr value = std::make_shared<RSSimpleDrawCmdList>();
    EXPECT_NE(value->GetType(), Drawing::CmdList::Type::RS_DRAW_CMD_LIST);
    curveValueEstimator->property_->Set(value);
    curveValueEstimator->UpdateAnimationValue(0.f, true);

    auto value1 = std::make_shared<RSDrawCmdList>(nullptr, nullptr);
    EXPECT_EQ(value1->GetType(), Drawing::CmdList::Type::RS_DRAW_CMD_LIST);
    curveValueEstimator->property_->Set(value1);
    curveValueEstimator->UpdateAnimationValue(0.f, true);
}

/**
 * @tc.name: RebuildValue001
 * @tc.desc: Verify RSCurveAnimationValueEstimator RebuildValue with valid parameters
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, RebuildValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSValueEstimatorTest RebuildValue001 start";
    constexpr float PROPERTY_INIT = 5.0f;
    constexpr float START_VAL = 2.0f;
    constexpr float END_VAL = 8.0f;
    constexpr float LAST_VAL = 8.0f;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(PROPERTY_INIT);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(START_VAL);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(END_VAL);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(LAST_VAL);
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<float>>();
    curveValueEstimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    curveValueEstimator->RebuildValue(0.5f);
    constexpr float EXPECTED_BASE = PROPERTY_INIT - (END_VAL - START_VAL);
    constexpr float EXPECTED_RESULT = EXPECTED_BASE * (1.0f - 0.5f) + PROPERTY_INIT * 0.5f;
    EXPECT_FLOAT_EQ(property->Get(), EXPECTED_RESULT);
    GTEST_LOG_(INFO) << "RSValueEstimatorTest RebuildValue001 end";
}

/**
 * @tc.name: RebuildValue002
 * @tc.desc: Verify RSCurveAnimationValueEstimator RebuildValue with fraction 0
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, RebuildValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSValueEstimatorTest RebuildValue002 start";
    constexpr float PROPERTY_INIT = 5.0f;
    constexpr float START_VAL = 2.0f;
    constexpr float END_VAL = 8.0f;
    constexpr float LAST_VAL = 8.0f;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(PROPERTY_INIT);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(START_VAL);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(END_VAL);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(LAST_VAL);
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<float>>();
    curveValueEstimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    curveValueEstimator->RebuildValue(0.0f);
    constexpr float EXPECTED_BASE = PROPERTY_INIT - (END_VAL - START_VAL);
    EXPECT_FLOAT_EQ(property->Get(), EXPECTED_BASE);
    GTEST_LOG_(INFO) << "RSValueEstimatorTest RebuildValue002 end";
}

/**
 * @tc.name: RebuildValue003
 * @tc.desc: Verify RSCurveAnimationValueEstimator RebuildValue with fraction 1
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, RebuildValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSValueEstimatorTest RebuildValue003 start";
    constexpr float PROPERTY_INIT = 5.0f;
    constexpr float START_VAL = 2.0f;
    constexpr float END_VAL = 8.0f;
    constexpr float LAST_VAL = 8.0f;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(PROPERTY_INIT);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(START_VAL);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(END_VAL);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(LAST_VAL);
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<float>>();
    curveValueEstimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    curveValueEstimator->RebuildValue(1.0f);
    EXPECT_FLOAT_EQ(property->Get(), PROPERTY_INIT);
    GTEST_LOG_(INFO) << "RSValueEstimatorTest RebuildValue003 end";
}

} // namespace Rosen
} // namespace OHOS