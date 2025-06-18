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

#include "rs_animation_test_utils.h"
#include "rs_animation_base_test.h"

#include "animation/rs_animation_group.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_spring_animation.h"
#include "modifier/rs_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSAnimationGroupTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: AnimationGroupTest001
 * @tc.desc: Verify the AddAnimation of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, AddAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AddAnimationTest001 start";
    /**
     * @tc.steps: step1. init AddAnimationTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSSpringAnimation>(boundsProperty,
        boundsStartProperty, boundsEndProperty);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSSpringAnimation>(alphaProperty,
        alphaStartProperty, alphaEndProperty);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    /**
     * @tc.steps: step2. start AddAnimationTest test
     */
    animationGroup->AddAnimation(nullptr);
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(animationGroup->IsStarted());
    animationGroup->AddAnimation(alphaAnimation);
    animationGroup->Finish();
    EXPECT_FALSE(animationGroup->IsRunning());
    alphaAnimation->Start(canvasNode);
    EXPECT_TRUE(alphaAnimation->IsStarted());
    animationGroup->AddAnimation(alphaAnimation);
    alphaAnimation->Finish();
    animationGroup->AddAnimation(boundsAnimation);
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AddAnimationTest001 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: AnimationGroupTest002
 * @tc.desc: Verify the AddAnimation of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, AddAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AddAnimationTest002 start";
    /**
     * @tc.steps: step1. init AddAnimationTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSSpringAnimation>(boundsProperty,
        boundsStartProperty, boundsEndProperty);
    boundsAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    boundsAnimation->SetDuration(ANIMATION_DURATION);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSSpringAnimation>(alphaProperty,
        alphaStartProperty, alphaEndProperty);
    alphaAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    alphaAnimation->SetDuration(ANIMATION_DURATION);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->AddAnimation(alphaAnimation);
    /**
     * @tc.steps: step2. start AddAnimationTest test
     */
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(boundsAnimation->IsRunning());
    EXPECT_TRUE(alphaAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AddAnimationTest002 end";
}

/**
 * @tc.name: AnimationGroupTest003
 * @tc.desc: Verify the AddAnimation of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, AddAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AddAnimationTest003 start";
    /**
     * @tc.steps: step1. init AddAnimationTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSCurveAnimation>(boundsProperty,
        boundsStartProperty, boundsEndProperty);
    boundsAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    boundsAnimation->SetDuration(ANIMATION_DURATION);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSCurveAnimation>(alphaProperty,
        alphaStartProperty, alphaEndProperty);
    alphaAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    alphaAnimation->SetDuration(ANIMATION_DURATION);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->AddAnimation(alphaAnimation);
    /**
     * @tc.steps: step2. start AddAnimationTest test
     */
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(boundsAnimation->IsRunning());
    EXPECT_TRUE(alphaAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AddAnimationTest003 end";
}

/**
 * @tc.name: AnimationGroupTest004
 * @tc.desc: Verify the AddAnimation of animationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, AnimationGroupTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AnimationGroupTest004 start";
    /**
     * @tc.steps: step1. init AddAnimation group
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsMiddleProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_MIDDLE_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSKeyframeAnimation>(boundsProperty);
    boundsAnimation->AddKeyFrame(0.f, boundsStartProperty, RSAnimationTimingCurve::DEFAULT);
    boundsAnimation->AddKeyFrame(0.5f, boundsMiddleProperty, RSAnimationTimingCurve::DEFAULT);
    boundsAnimation->AddKeyFrame(1.f, boundsEndProperty, RSAnimationTimingCurve::DEFAULT);
    boundsAnimation->SetDuration(ANIMATION_DURATION);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaMiddleProperty = std::make_shared<RSAnimatableProperty<float>>(0.5f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSKeyframeAnimation>(alphaProperty);
    alphaAnimation->SetDuration(ANIMATION_DURATION);
    alphaAnimation->AddKeyFrame(0.f, boundsStartProperty, RSAnimationTimingCurve::DEFAULT);
    alphaAnimation->AddKeyFrame(0.5f, alphaMiddleProperty, RSAnimationTimingCurve::DEFAULT);
    alphaAnimation->AddKeyFrame(1.f, boundsEndProperty, RSAnimationTimingCurve::DEFAULT);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->AddAnimation(alphaAnimation);
    /**
     * @tc.steps: step2. start AddAnimationTest test
     */
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(boundsAnimation->IsRunning());
    EXPECT_TRUE(alphaAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationGroupTest AnimationGroupTest004 end";
}
#endif

/**
 * @tc.name: RemoveAnimationTest001
 * @tc.desc: Verify the RemoveAnimation of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest001 start";
    /**
     * @tc.steps: step1. init RemoveAnimationTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSSpringAnimation>(boundsProperty,
        boundsStartProperty, boundsEndProperty);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSSpringAnimation>(alphaProperty,
        alphaStartProperty, alphaEndProperty);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    /**
     * @tc.steps: step2. start RemoveAnimationTest test
     */
    animationGroup->RemoveAnimation(nullptr);
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(animationGroup->IsStarted());
    animationGroup->RemoveAnimation(boundsAnimation);
    animationGroup->Finish();
    EXPECT_FALSE(animationGroup->IsRunning());
    boundsAnimation->Start(canvasNode);
    EXPECT_TRUE(boundsAnimation->IsStarted());
    animationGroup->RemoveAnimation(boundsAnimation);
    boundsAnimation->Finish();
    animationGroup->RemoveAnimation(alphaAnimation);
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest001 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RemoveAnimationTest002
 * @tc.desc: Verify the RemoveAnimation of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest002 start";
    /**
     * @tc.steps: step1. init RemoveAnimationTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSSpringAnimation>(boundsProperty,
        boundsStartProperty, boundsEndProperty);
    boundsAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    boundsAnimation->SetDuration(ANIMATION_DURATION);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSSpringAnimation>(alphaProperty,
        alphaStartProperty, alphaEndProperty);
    alphaAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    alphaAnimation->SetDuration(ANIMATION_DURATION);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->AddAnimation(alphaAnimation);
    animationGroup->RemoveAnimation(boundsAnimation);
    /**
     * @tc.steps: step2. start RemoveAnimationTest test
     */
    animationGroup->Start(canvasNode);
    EXPECT_FALSE(boundsAnimation->IsRunning());
    EXPECT_TRUE(alphaAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest002 end";
}

/**
 * @tc.name: RemoveAnimationTest003
 * @tc.desc: Verify the RemoveAnimation of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest003 start";
    /**
     * @tc.steps: step1. init RemoveAnimationTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSCurveAnimation>(boundsProperty,
        boundsStartProperty, boundsEndProperty);
    boundsAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    boundsAnimation->SetDuration(ANIMATION_DURATION);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSCurveAnimation>(alphaProperty,
        alphaStartProperty, alphaEndProperty);
    alphaAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    alphaAnimation->SetDuration(ANIMATION_DURATION);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->AddAnimation(alphaAnimation);
    animationGroup->RemoveAnimation(alphaAnimation);

    /**
     * @tc.steps: step2. start RemoveAnimationTest test
     */
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(boundsAnimation->IsRunning());
    EXPECT_FALSE(alphaAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest003 end";
}

/**
 * @tc.name: RemoveAnimationTest004
 * @tc.desc: Verify the RemoveAnimation of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest003 start";
    /**
     * @tc.steps: step1. init RemoveAnimationTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsMiddleProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_MIDDLE_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSKeyframeAnimation>(boundsProperty);
    boundsAnimation->AddKeyFrame(0.f, boundsStartProperty, RSAnimationTimingCurve::DEFAULT);
    boundsAnimation->AddKeyFrame(0.5f, boundsMiddleProperty, RSAnimationTimingCurve::DEFAULT);
    boundsAnimation->AddKeyFrame(1.f, boundsEndProperty, RSAnimationTimingCurve::DEFAULT);
    boundsAnimation->SetDuration(ANIMATION_DURATION);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaMiddleProperty = std::make_shared<RSAnimatableProperty<float>>(0.5f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSKeyframeAnimation>(alphaProperty);
    alphaAnimation->SetDuration(ANIMATION_DURATION);
    alphaAnimation->AddKeyFrame(0.f, boundsStartProperty, RSAnimationTimingCurve::DEFAULT);
    alphaAnimation->AddKeyFrame(0.5f, alphaMiddleProperty, RSAnimationTimingCurve::DEFAULT);
    alphaAnimation->AddKeyFrame(1.f, boundsEndProperty, RSAnimationTimingCurve::DEFAULT);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->AddAnimation(alphaAnimation);
    animationGroup->RemoveAnimation(boundsAnimation);
    animationGroup->RemoveAnimation(alphaAnimation);
    /**
     * @tc.steps: step2. start RemoveAnimationTest test
     */
    animationGroup->Start(canvasNode);
    EXPECT_FALSE(boundsAnimation->IsRunning());
    EXPECT_FALSE(alphaAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RemoveAnimationTest004 end";
}

/**
 * @tc.name: RSNodeAnimateTest001
 * @tc.desc: Verify the RSNodeAnimate of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, RSNodeAnimateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RSNodeAnimateTest001 start";
    /**
     * @tc.steps: step1. init RSAnimateTest
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::SPRING,
        [&alphaProperty, &boundsProperty]() {
        boundsProperty->Set(ANIMATION_END_BOUNDS);
        alphaProperty->Set(0.f);
    });
    /**
     * @tc.steps: step2. start RSAnimate test
     */
    ASSERT_TRUE(animations.size() == CORRECT_GROUP_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_FALSE(animations[SECOND_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[SECOND_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RSNodeAnimateTest001 end";
}
#endif

/**
 * @tc.name: RSAnimationGroupStatusTest001
 * @tc.desc: Verify the RSAnimationGroupStatus of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, RSAnimationGroupStatusTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RSAnimationGroupStatusTest001 start";
    /**
     * @tc.steps: step1. init RSAnimationGroupStatus
     */
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    /**
     * @tc.steps: step2. start RSAnimationGroupStatus test
     */
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(animationGroup->IsRunning());
    animationGroup->Pause();
    EXPECT_TRUE(animationGroup->IsPaused());
    animationGroup->Reverse();
    animationGroup->Resume();
    EXPECT_TRUE(animationGroup->IsRunning());
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animationGroup->Finish();
    EXPECT_TRUE(animationGroup->IsFinished());
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RSAnimationGroupStatusTest001 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RSAnimationGroupStatusTest002
 * @tc.desc: Verify the RSAnimationGroupStatus of AnimationGroup
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationGroupTest, RSAnimationGroupStatusTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RSAnimationGroupStatusTest002 start";
    /**
     * @tc.steps: step1. init RSAnimationGroupStatus
     */
    auto boundsProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsModifier = std::make_shared<RSBoundsModifier>(boundsProperty);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(boundsModifier);
    canvasNode->AddModifier(alphaModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto boundsStartProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto boundsEndProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto boundsAnimation = std::make_shared<RSSpringAnimation>(boundsProperty,
        boundsStartProperty, boundsEndProperty);
    boundsAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    boundsAnimation->SetDuration(ANIMATION_DURATION);

    auto alphaStartProperty = std::make_shared<RSAnimatableProperty<float>>(0.f);
    auto alphaEndProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto alphaAnimation = std::make_shared<RSSpringAnimation>(alphaProperty,
        alphaStartProperty, alphaEndProperty);
    alphaAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    alphaAnimation->SetDuration(ANIMATION_DURATION);
    auto animationGroup = std::make_shared<RSAnimationGroup>();
    animationGroup->AddAnimation(boundsAnimation);
    animationGroup->AddAnimation(alphaAnimation);
    /**
     * @tc.steps: step2. start RSAnimationGroupStatus test
     */
    animationGroup->Start(canvasNode);
    EXPECT_TRUE(animationGroup->IsRunning());
    animationGroup->Pause();
    EXPECT_TRUE(animationGroup->IsPaused());
    animationGroup->Reverse();
    animationGroup->Resume();
    EXPECT_TRUE(animationGroup->IsRunning());
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animationGroup->Finish();
    EXPECT_TRUE(animationGroup->IsFinished());
    GTEST_LOG_(INFO) << "RSAnimationGroupTest RSAnimationGroupStatusTest002 end";
}
#endif
} // namespace Rosen
} // namespace OHOS