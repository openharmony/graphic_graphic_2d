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

/**
 * @tc.name: GetAnimationValueEmptyKeyframes001
 * @tc.desc: Verify GetAnimationValue returns property value when keyframes_ is empty
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, GetAnimationValueEmptyKeyframes001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSValueEstimatorTest GetAnimationValueEmptyKeyframes001 start";
    auto estimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(10.0f);
    estimator->property_ = property;
    estimator->durationKeyframes_.clear();
    estimator->keyframes_.clear();
    auto result = estimator->GetAnimationValue(0.5f, false);
    EXPECT_FLOAT_EQ(result, 10.0f);
    GTEST_LOG_(INFO) << "RSValueEstimatorTest GetAnimationValueEmptyKeyframes001 end";
}

/**
 * @tc.name: GetAnimationValueEmptyKeyframesNoProperty001
 * @tc.desc: Verify GetAnimationValue returns default T() when both keyframes_ and property_ are empty
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, GetAnimationValueEmptyKeyframesNoProperty001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSValueEstimatorTest GetAnimationValueEmptyKeyframesNoProperty001 start";
    auto estimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    estimator->property_ = nullptr;
    estimator->durationKeyframes_.clear();
    estimator->keyframes_.clear();
    auto result = estimator->GetAnimationValue(0.5f, false);
    EXPECT_FLOAT_EQ(result, 0.0f);
    GTEST_LOG_(INFO) << "RSValueEstimatorTest GetAnimationValueEmptyKeyframesNoProperty001 end";
}

/**
 * @tc.name: InitCurveAnimationValuePropertyNull001
 * @tc.desc: Verify InitCurveAnimationValue returns early when property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValuePropertyNull001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitCurveAnimationValue(nullptr, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
    EXPECT_FLOAT_EQ(estimator->startValue_, 0.0f);
    EXPECT_FLOAT_EQ(estimator->endValue_, 0.0f);
    EXPECT_FLOAT_EQ(estimator->lastValue_, 0.0f);
}

/**
 * @tc.name: InitCurveAnimationValuePropertyTypeMismatch001
 * @tc.desc: Verify InitCurveAnimationValue returns early when property type does not match T
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValuePropertyTypeMismatch001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitCurveAnimationValueTypeInconsistent001
 * @tc.desc: Verify InitCurveAnimationValue skips when startValue/endValue/lastValue type
 *           does not match property
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValueTypeInconsistent001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitCurveAnimationValueTypeInconsistent002
 * @tc.desc: Verify InitCurveAnimationValue skips when endValue type does not match property
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValueTypeInconsistent002, TestSize.Level1)
{
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitCurveAnimationValueTypeInconsistent003
 * @tc.desc: Verify InitCurveAnimationValue skips when lastValue type does not match property
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValueTypeInconsistent003, TestSize.Level1)
{
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    estimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitCurveAnimationValueAllTypeMatch001
 * @tc.desc: Verify InitCurveAnimationValue succeeds when all types match
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValueAllTypeMatch001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(2.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    estimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
    EXPECT_NE(estimator->property_, nullptr);
    EXPECT_FLOAT_EQ(estimator->startValue_, 2.0f);
    EXPECT_FLOAT_EQ(estimator->endValue_, 8.0f);
    EXPECT_FLOAT_EQ(estimator->lastValue_, 8.0f);
}

/**
 * @tc.name: InitKeyframeAnimationValuePropertyNull001
 * @tc.desc: Verify InitKeyframeAnimationValue returns early when property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitKeyframeAnimationValuePropertyNull001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>> keyframes;
    estimator->InitKeyframeAnimationValue(nullptr, keyframes, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitKeyframeAnimationValuePropertyTypeMismatch001
 * @tc.desc: Verify InitKeyframeAnimationValue returns early when property type does not match T
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitKeyframeAnimationValuePropertyTypeMismatch001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>> keyframes;
    estimator->InitKeyframeAnimationValue(property, keyframes, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitKeyframeAnimationValueKeyframeTypeInconsistent001
 * @tc.desc: Verify InitKeyframeAnimationValue skips keyframe whose type does not match property
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitKeyframeAnimationValueKeyframeTypeInconsistent001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto mismatchedKeyframe = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto matchedKeyframe = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    auto interpolator = std::make_shared<RSStepsInterpolator>(1, StepsCurvePosition::START);
    std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>> keyframes;
    keyframes.push_back({0.0f, mismatchedKeyframe, interpolator});
    keyframes.push_back({1.0f, matchedKeyframe, interpolator});
    estimator->InitKeyframeAnimationValue(property, keyframes, lastValue);
    EXPECT_NE(estimator->property_, nullptr);
    EXPECT_EQ(estimator->keyframes_.size(), 1u);
}

/**
 * @tc.name: InitDurationKeyframeAnimationValuePropertyNull001
 * @tc.desc: Verify InitDurationKeyframeAnimationValue returns early when property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitDurationKeyframeAnimationValuePropertyNull001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    std::vector<std::tuple<float, float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>> keyframes;
    estimator->InitDurationKeyframeAnimationValue(nullptr, keyframes, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitDurationKeyframeAnimationValueTypeInconsistent001
 * @tc.desc: Verify InitDurationKeyframeAnimationValue skips keyframe whose type does not match
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitDurationKeyframeAnimationValueTypeInconsistent001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSKeyframeValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto mismatchedKeyframe = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto matchedKeyframe = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    auto interpolator = std::make_shared<RSStepsInterpolator>(1, StepsCurvePosition::START);
    std::vector<std::tuple<float, float, std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSInterpolator>>> keyframes;
    keyframes.push_back({0, 100, mismatchedKeyframe, interpolator});
    keyframes.push_back({100, 200, matchedKeyframe, interpolator});
    estimator->InitDurationKeyframeAnimationValue(property, keyframes, lastValue);
    EXPECT_NE(estimator->property_, nullptr);
    EXPECT_EQ(estimator->durationKeyframes_.size(), 1u);
}

/**
 * @tc.name: InitRSSpringValueEstimatorPropertyNull001
 * @tc.desc: Verify InitRSSpringValueEstimator returns early when property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitRSSpringValueEstimatorPropertyNull001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitRSSpringValueEstimator(nullptr, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitRSSpringValueEstimatorPropertyTypeMismatch001
 * @tc.desc: Verify InitRSSpringValueEstimator returns early when property type does not match T
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitRSSpringValueEstimatorPropertyTypeMismatch001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitRSSpringValueEstimatorTypeInconsistent001
 * @tc.desc: Verify InitRSSpringValueEstimator skips when startValue type does not match property
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitRSSpringValueEstimatorTypeInconsistent001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitRSSpringValueEstimatorTypeInconsistent002
 * @tc.desc: Verify InitRSSpringValueEstimator skips when endValue type does not match property
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitRSSpringValueEstimatorTypeInconsistent002, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitRSSpringValueEstimatorTypeInconsistent003
 * @tc.desc: Verify InitRSSpringValueEstimator skips when lastValue type does not match property
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitRSSpringValueEstimatorTypeInconsistent003, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    estimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

/**
 * @tc.name: InitRSSpringValueEstimatorAllTypeMatch001
 * @tc.desc: Verify InitRSSpringValueEstimator succeeds when all types match
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitRSSpringValueEstimatorAllTypeMatch001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(2.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    estimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
    EXPECT_NE(estimator->property_, nullptr);
    EXPECT_FLOAT_EQ(estimator->startValue_, 2.0f);
    EXPECT_FLOAT_EQ(estimator->endValue_, 8.0f);
    EXPECT_FLOAT_EQ(estimator->lastValue_, 8.0f);
}

/**
 * @tc.name: UpdateStartValueAndLastValuePropertyNull001
 * @tc.desc: Verify UpdateStartValueAndLastValue returns early when property_ is null
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, UpdateStartValueAndLastValuePropertyNull001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    estimator->property_ = nullptr;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(2.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    estimator->UpdateStartValueAndLastValue(startValue, lastValue);
    EXPECT_FLOAT_EQ(estimator->startValue_, 0.0f);
    EXPECT_FLOAT_EQ(estimator->lastValue_, 0.0f);
}

/**
 * @tc.name: UpdateStartValueAndLastValueTypeInconsistent001
 * @tc.desc: Verify UpdateStartValueAndLastValue skips when startValue type does not match
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, UpdateStartValueAndLastValueTypeInconsistent001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    estimator->property_ = property;
    estimator->endValue_ = 8.0f;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(8.0f);
    estimator->UpdateStartValueAndLastValue(startValue, lastValue);
    EXPECT_FLOAT_EQ(estimator->startValue_, 0.0f);
}

/**
 * @tc.name: UpdateStartValueAndLastValueTypeInconsistent002
 * @tc.desc: Verify UpdateStartValueAndLastValue skips when lastValue type does not match
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, UpdateStartValueAndLastValueTypeInconsistent002, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(5.0f);
    estimator->property_ = property;
    estimator->endValue_ = 8.0f;
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(2.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0.f));
    estimator->UpdateStartValueAndLastValue(startValue, lastValue);
    EXPECT_FLOAT_EQ(estimator->startValue_, 0.0f);
    EXPECT_FLOAT_EQ(estimator->lastValue_, 0.0f);
}

/**
 * @tc.name: SetInitialVelocityNull001
 * @tc.desc: Verify SetInitialVelocity does nothing when initialVelocity is null
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, SetInitialVelocityNull001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    estimator->InitSpringModel();
    EXPECT_NE(estimator->springModel_, nullptr);
    auto prevVelocity = estimator->springModel_->initialVelocity_;
    estimator->SetInitialVelocity(nullptr);
    EXPECT_FLOAT_EQ(estimator->springModel_->initialVelocity_, prevVelocity);
}

/**
 * @tc.name: SetInitialVelocityTypeMismatch001
 * @tc.desc: Verify SetInitialVelocity does nothing when initialVelocity type does not match T
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, SetInitialVelocityTypeMismatch001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    estimator->InitSpringModel();
    EXPECT_NE(estimator->springModel_, nullptr);
    auto prevVelocity = estimator->springModel_->initialVelocity_;
    auto velocity = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(1.f));
    estimator->SetInitialVelocity(velocity);
    EXPECT_FLOAT_EQ(estimator->springModel_->initialVelocity_, prevVelocity);
}

/**
 * @tc.name: SetInitialVelocityTypeMatch001
 * @tc.desc: Verify SetInitialVelocity succeeds when type matches T
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, SetInitialVelocityTypeMatch001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSSpringValueEstimator<float>>();
    estimator->InitSpringModel();
    EXPECT_NE(estimator->springModel_, nullptr);
    auto velocity = std::make_shared<RSRenderAnimatableProperty<float>>(3.0f);
    estimator->SetInitialVelocity(velocity);
    EXPECT_FLOAT_EQ(estimator->springModel_->initialVelocity_, 3.0f);
}

/**
 * @tc.name: CastToAnimatablePropertyOfNotAnimatable001
 * @tc.desc: Verify CastToAnimatablePropertyOf returns nullptr for non-animatable RSRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, CastToAnimatablePropertyOfNotAnimatable001, TestSize.Level1)
{
    auto nonAnimatable = std::make_shared<RSRenderProperty<float>>();
    EXPECT_FALSE(nonAnimatable->IsAnimatable());
    auto result = nonAnimatable->CastToAnimatablePropertyOf<float>(__func__);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CastToAnimatablePropertyOfAnimatable001
 * @tc.desc: Verify CastToAnimatablePropertyOf succeeds for RSRenderAnimatableProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, CastToAnimatablePropertyOfAnimatable001, TestSize.Level1)
{
    auto animatable = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    EXPECT_TRUE(animatable->IsAnimatable());
    auto result = animatable->CastToAnimatablePropertyOf<float>(__func__);
    EXPECT_NE(result, nullptr);
    EXPECT_FLOAT_EQ(result->Get(), 1.0f);
}

/**
 * @tc.name: CastToAnimatablePropertyOfTypeMismatch001
 * @tc.desc: Verify CastToAnimatablePropertyOf returns nullptr when T does not match property type
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, CastToAnimatablePropertyOfTypeMismatch001, TestSize.Level1)
{
    auto animatable = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto result = animatable->CastToAnimatablePropertyOf<Vector4f>(__func__);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: IsAnimatablePropertyBase001
 * @tc.desc: Verify RSRenderPropertyBase::IsAnimatable returns false for RSRenderProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, IsAnimatablePropertyBase001, TestSize.Level1)
{
    auto nonAnimatable = std::make_shared<RSRenderProperty<float>>();
    EXPECT_FALSE(nonAnimatable->IsAnimatable());
    auto animatable = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    EXPECT_TRUE(animatable->IsAnimatable());
}

/**
 * @tc.name: InitCurveAnimationValueNotAnimatableProperty001
 * @tc.desc: Verify InitCurveAnimationValue returns early when property is not animatable
 * @tc.type:FUNC
 */
HWTEST_F(RSValueEstimatorTest, InitCurveAnimationValueNotAnimatableProperty001, TestSize.Level1)
{
    auto estimator = std::make_shared<RSCurveValueEstimator<float>>();
    auto nonAnimatableProperty = std::make_shared<RSRenderProperty<float>>();
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    estimator->InitCurveAnimationValue(nonAnimatableProperty, startValue, endValue, lastValue);
    EXPECT_EQ(estimator->property_, nullptr);
}

} // namespace Rosen
} // namespace OHOS