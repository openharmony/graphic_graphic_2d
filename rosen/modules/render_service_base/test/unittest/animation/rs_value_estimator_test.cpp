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
 * @tc.type: FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValueTest, TestSize.Level1)
{
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<Drawing::DrawCmdListPtr>>();

    auto property = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>();
    auto startValue = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>();
    auto endValue = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>();
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>();

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
 * @tc.type: FUNC
 */
HWTEST_F(RSValueEstimatorTest, UpdateAnimationValueTest, TestSize.Level1)
{
    auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<Drawing::DrawCmdListPtr>>();
    EXPECT_EQ(curveValueEstimator->property_, nullptr);
    curveValueEstimator->UpdateAnimationValue(0.f, true);

    auto property = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>();
    curveValueEstimator->property_ = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>();

    auto animationValue = curveValueEstimator->property_->Get();
    EXPECT_EQ(animationValue, nullptr);
    curveValueEstimator->UpdateAnimationValue(0.f, true);

    Drawing::DrawCmdListPtr value = std::make_shared<Drawing::DrawCmdList>();
    EXPECT_NE(value->GetType(), Drawing::CmdList::Type::RS_DRAW_CMD_LIST);
    curveValueEstimator->property_->Set(value);
    curveValueEstimator->UpdateAnimationValue(0.f, true);

    Drawing::DrawCmdListPtr value1 = std::make_shared<RSDrawCmdList>(nullptr, nullptr);
    EXPECT_EQ(value1->GetType(), Drawing::CmdList::Type::RS_DRAW_CMD_LIST);
    curveValueEstimator->property_->Set(value1);
    curveValueEstimator->UpdateAnimationValue(0.f, true);
}
} // namespace Rosen
} // namespace OHOS