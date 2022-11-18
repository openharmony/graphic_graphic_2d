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

#include "animation/rs_spring_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSSpringAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: GetTimingCurveTest001
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    springAnimation->SetDuration(100);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest001 end";
}

/**
 * @tc.name: GetTimingCurveTest002
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest002 start";
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
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest002 end";
}

/**
 * @tc.name: GetTimingCurveTest003
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest003 start";
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
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest003 end";
}

/**
 * @tc.name: GetTimingCurveTest004
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest004 start";
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
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest004 end";
}

/**
 * @tc.name: RSNodeAnimateTest001
 * @tc.desc: Verify the RSNodeAnimate of RSSpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, RSNodeAnimateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSNodeAnimateTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto springAnimation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(springAnimation != nullptr);
    if (springAnimation != nullptr) {
        RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    }
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSNodeAnimateTest001 end";
}

/**
 * @tc.name: SetIsCustomTest001
 * @tc.desc: Verify the SetIsCustom of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetIsCustomTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest001 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto black = RgbPalette::Black();
    auto red = RgbPalette::Red();
    auto property = std::make_shared<RSAnimatableProperty<Color>>(black);
    auto modifier = std::make_shared<RSForegroundColorModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Color>>(black);
    auto endProperty = std::make_shared<RSAnimatableProperty<Color>>(red);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(false);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest001 end";
}

/**
 * @tc.name: SetIsCustomTest002
 * @tc.desc: Verify the SetIsCustom of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetIsCustomTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest002 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(true);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest002 end";
}
} // namespace Rosen
} // namespace OHOS