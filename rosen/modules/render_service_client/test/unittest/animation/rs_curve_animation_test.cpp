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

#include "animation/rs_curve_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSCurveAnimationTest : public RSAnimationBaseTest {
};

#ifndef MODIFIER_NG
/**
 * @tc.name: GetTimingCurveTest001
 * @tc.desc: Verify the GetTimingCurve of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, GetTimingCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest GetTimingCurveTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    curveAnimation->SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    EXPECT_TRUE(curveAnimation->IsSupportInteractiveAnimator());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest GetTimingCurveTest001 end" ;
}

/**
 * @tc.name: GetTimingCurveTest002
 * @tc.desc: Verify the GetTimingCurve of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, GetTimingCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest GetTimingCurveTest002 start";
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
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest GetTimingCurveTest002 end";
}

/**
 * @tc.name: GetTimingCurveTest003
 * @tc.desc: Verify the GetTimingCurve of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, GetTimingCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest GetTimingCurveTest003 start";
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
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();

    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest GetTimingCurveTest003 end" ;
}

/**
 * @tc.name: SetTimingCurveTest001
 * @tc.desc: Verify the GetTimingCurve of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, SetTimingCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE_OUT);
    curveAnimation->SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();

    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest001 end";
}

/**
 * @tc.name: SetTimingCurveTest002
 * @tc.desc: Verify the GetTimingCurve of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, SetTimingCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest002 start";
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
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE_OUT);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();

    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest002 end";
}

/**
 * @tc.name: SetTimingCurveTest003
 * @tc.desc: Verify the GetTimingCurve of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, SetTimingCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest003 start";
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
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    auto spring = RSAnimationTimingCurve::CreateSpring(0.1f, 0.2f, 0.3f);
    curveAnimation->SetTimingCurve(spring);

    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(spring.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest003 end" ;
}

/**
 * @tc.name: SetTimingCurveTest004
 * @tc.desc: Verify the GetTimingCurve of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, SetTimingCurveTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest003 start";
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
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE_OUT);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();

    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest004 end" ;
}

/**
 * @tc.name: RSNodeAnimateTest001
 * @tc.desc: Verify the RSNodeAnimate of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, RSNodeAnimateTest001, TestSize.Level1)
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
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto springAnimation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(springAnimation != nullptr);
    if (springAnimation != nullptr) {
        RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
        NotifyStartAnimation();
    }
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSNodeAnimateTest001 end";
}

/**
 * @tc.name: SetIsCustomTest001
 * @tc.desc: Verify the SetIsCustom of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, SetIsCustomTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetIsCustomTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto value = Quaternion(0.382683, 0, 0, 0.92388);
    auto property = std::make_shared<RSAnimatableProperty<Quaternion>>();
    auto modifier = std::make_shared<RSQuaternionModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Quaternion>>();
    auto endProperty = std::make_shared<RSAnimatableProperty<Quaternion>>(value);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE_OUT);
    curveAnimation->SetIsCustom(false);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();

    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetTimingCurveTest004 end" ;
}

/**
 * @tc.name: SetIsCustomTest002
 * @tc.desc: Verify the SetIsCustom of CurveAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, SetIsCustomTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetIsCustomTest002 start";
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
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    auto renderAnimation = std::make_shared<RSRenderCurveAnimation>();
    curveAnimation->StartRenderAnimation(renderAnimation);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE_OUT);
    curveAnimation->SetIsCustom(true);
    RSAnimationTimingCurve timingCurve = curveAnimation->GetTimingCurve();

    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(curveAnimation == nullptr);
    EXPECT_FALSE(curveAnimation->IsStarted());
    curveAnimation->Start(canvasNode);
    EXPECT_TRUE(curveAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();

    RSAnimationTimingCurve timing;
    timing.customCurveFunc_ = nullptr;
    timing.interpolator_ = std::make_shared<RSCustomInterpolator>(nullptr, 1);
    curveAnimation->SetTimingCurve(timing);
    curveAnimation->IsSupportInteractiveAnimator();
    GTEST_LOG_(INFO) << "RSCurveAnimationTest SetIsCustomTest002 end" ;
}
#endif

/**
 * @tc.name: IsSupportInteractiveAnimator001
 * @tc.desc: Verify the IsSupportInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, IsSupportInteractiveAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest IsSupportInteractiveAnimator001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    auto interpolator = std::shared_ptr<RSInterpolator>();
    auto timingCurve = RSAnimationTimingCurve(interpolator);
    curveAnimation->SetTimingCurve(timingCurve);
    EXPECT_EQ(interpolator, nullptr);
    EXPECT_FALSE(curveAnimation->IsSupportInteractiveAnimator());
    GTEST_LOG_(INFO) << "RSCurveAnimationTest IsSupportInteractiveAnimator001 end";
}
} // namespace Rosen
} // namespace OHOS