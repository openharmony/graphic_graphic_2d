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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_path_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSPathAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: GetTimingCurveTest001
 * @tc.desc: Verify the GetTimingCurve of PathAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, GetTimingCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetTimingCurveTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto pathAnimation = std::make_shared<RSPathAnimation>(property, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
    RSAnimationTimingCurve timingCurve = pathAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(pathAnimation == nullptr);
    EXPECT_FALSE(pathAnimation->IsStarted());
    pathAnimation->Start(canvasNode);
    EXPECT_TRUE(pathAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetTimingCurveTest001 end";
}


/**
 * @tc.name: GetRotationModeTest001
 * @tc.desc: Verify the GetRotationMode of PathAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, GetRotationModeTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetRotationModeTest001 start";
    /**
     * @tc.steps: step1. init GetRotationMode
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto pathAnimation = std::make_shared<RSPathAnimation>(property, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
    pathAnimation->SetRotationMode(RotationMode::ROTATE_AUTO);
    /**
     * @tc.steps: step2. start GetRotationMode test
     */
    EXPECT_FALSE(pathAnimation == nullptr);
    EXPECT_FALSE(pathAnimation->IsStarted());
    pathAnimation->Start(canvasNode);
    EXPECT_TRUE(pathAnimation->IsRunning());
    EXPECT_EQ(RotationMode::ROTATE_AUTO, pathAnimation->GetRotationMode());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetRotationModeTest001 end";
}

/**
 * @tc.name: GetBeginFractionTest001
 * @tc.desc: Verify the GetBeginFraction of PathAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, GetBeginFractionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetBeginFractionTest001 start";
    /**
     * @tc.steps: step1. init GetBeginFraction
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto pathAnimation = std::make_shared<RSPathAnimation>(property, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
    pathAnimation->SetBeginFraction(0.1f);
    /**
     * @tc.steps: step2. start GetBeginFraction test
     */
    EXPECT_FALSE(pathAnimation == nullptr);
    EXPECT_FALSE(pathAnimation->IsStarted());
    pathAnimation->Start(canvasNode);
    EXPECT_TRUE(pathAnimation->IsRunning());
    EXPECT_EQ(0.1f, pathAnimation->GetBeginFraction());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetBeginFractionTest001 end";
}

/**
 * @tc.name: GetEndFractionTest001
 * @tc.desc: Verify the GetEndFraction of PathAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSPathAnimationTest, GetEndFractionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetEndFractionTest001 start";
    /**
     * @tc.steps: step1. init GetEndFraction
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto pathAnimation = std::make_shared<RSPathAnimation>(property, ANIMATION_PATH, startProperty, endProperty);
    pathAnimation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
    pathAnimation->SetEndFraction(0.6f);
    /**
     * @tc.steps: step2. start GetEndFraction test
     */
    EXPECT_FALSE(pathAnimation == nullptr);
    EXPECT_FALSE(pathAnimation->IsStarted());
    pathAnimation->Start(canvasNode);
    EXPECT_TRUE(pathAnimation->IsRunning());
    EXPECT_EQ(0.6f, pathAnimation->GetEndFraction());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSPathAnimationTest GetEndFractionTest001 end";
}
} // namespace Rosen
} // namespace OHOS