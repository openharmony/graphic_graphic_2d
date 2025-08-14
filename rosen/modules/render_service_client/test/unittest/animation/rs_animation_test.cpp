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

#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_animation.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "render/rs_path.h"
#include "ui/rs_canvas_node.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: AnimationGetId001
 * @tc.desc: Verify the AnimationGetId of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationGetId001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId001 start";
    /**
     * @tc.steps: step1. init AnimationGetId
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start AnimationGetId test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    AnimationId id = animation->GetId();
    EXPECT_TRUE(id != 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId001 end";
}

/**
 * @tc.name: AnimationGetId002
 * @tc.desc: Verify the AnimationGetId of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationGetId002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId002 start";
    /**
     * @tc.steps: step1. init AnimationGetId
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });
    /**
     * @tc.steps: step2. start AnimationGetId test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    AnimationId id = animation->GetId();
    EXPECT_TRUE(id != 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId002 end";
}

/**
 * @tc.name: AnimationSetFinishCallback001
 * @tc.desc: Verify the AnimationSetFinishCallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSetFinishCallback001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback001 start";
    /**
     * @tc.steps: step1. init AnimationSetFinishCallback
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    std::string testString;
    auto lambda = [&testString]() { testString = SUCCESS_STRING; };
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    }, lambda);

    /**
     * @tc.steps: step2. start AnimationSetFinishCallback test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
    EXPECT_STREQ(SUCCESS_STRING.c_str(), testString.c_str());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback001 end";
}

/**
 * @tc.name: AnimationSetFinishCallback002
 * @tc.desc: Verify the AnimationSetFinishCallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSetFinishCallback002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback002 start";
    /**
     * @tc.steps: step1. init AnimationSetFinishCallback
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    std::string testString;
    auto lambda = [&testString]() { testString = SUCCESS_STRING; };
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    }, lambda);
    /**
     * @tc.steps: step2. start AnimationSetFinishCallback test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
    EXPECT_STREQ(SUCCESS_STRING.c_str(), testString.c_str());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback002 end";
}

/**
 * @tc.name: AnimationGetTarget001
 * @tc.desc: Verify the AnimationGetTarget of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationGetTarget001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget001 start";
    /**
     * @tc.steps: step1. init AnimationGetTarget
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start AnimationGetTarget test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    auto target = animation->GetTarget();
    EXPECT_TRUE(target.lock() != nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget001 end";
}

/**
 * @tc.name: AnimationGetTarget002
 * @tc.desc: Verify the AnimationGetTarget of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationGetTarget002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget002 start";
    /**
     * @tc.steps: step1. init AnimationGetTarget
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start AnimationGetTarget test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    auto target = animation->GetTarget();
    EXPECT_TRUE(target.lock() != nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget002 end";
}

/**
 * @tc.name: AnimationStatus001
 * @tc.desc: Verify the AnimationStatus of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationStatus001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus001 start";
    /**
     * @tc.steps: step1. init AnimationStatus
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start AnimationStatus test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    EXPECT_TRUE(animation->IsRunning());
    animation->Pause();
    EXPECT_TRUE(animation->IsPaused());
    animation->Reverse();
    animation->Resume();
    EXPECT_TRUE(animation->IsRunning());
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus001 end";
}

/**
 * @tc.name: AnimationStatus002
 * @tc.desc: Verify the AnimationStatus of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationStatus002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus002 start";
    /**
     * @tc.steps: step1. init AnimationStatus
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start AnimationStatus test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    EXPECT_TRUE(animation->IsRunning());
    animation->Pause();
    EXPECT_TRUE(animation->IsPaused());
    animation->Reverse();
    animation->Resume();
    EXPECT_TRUE(animation->IsRunning());
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus002 end";
}

/**
 * @tc.name: AnimationStatus004
 * @tc.desc: Verify the AnimationStatus of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationStatus004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus004 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);

    animation->state_ = Rosen::RSAnimation::AnimationState::PAUSED;
    animation->SetFraction(0.5);

    animation->state_ = Rosen::RSAnimation::AnimationState::RUNNING;
    animation->Pause();

    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    animation->OnReverse();
    animation->Resume();
    animation->Finish();
    animation->Reverse();

    animation->target_ = RSCanvasNode::Create(true, false);
    animation->uiAnimation_.reset();
    animation->Resume();
    animation->OnFinish();
    animation->OnReverse();
    animation->OnSetFraction(0.5);

    auto propAnimation = std::make_shared<RSPropertyAnimation>(nullptr);
    auto propId = propAnimation->GetPropertyId();
    EXPECT_TRUE(propId == 0);
    propAnimation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    propAnimation->property_ = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    propAnimation->UpdateStagingValueOnInteractiveFinish(RSInteractiveAnimationPosition::CURRENT);
    std::string dumpInfo = "";
    propAnimation->DumpAnimationInfo(dumpInfo);
    EXPECT_TRUE(propAnimation != nullptr);
    
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus004 end";
}

/**
 * @tc.name: InteractivePause
 * @tc.desc: Verify the InteractivePause of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, InteractivePause, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest InteractivePause start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);

    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->InteractivePause();
    animation->state_ = RSAnimation::AnimationState::RUNNING;
    animation->target_.reset();
    animation->InteractivePause();
    std::shared_ptr<RSNode> target = std::make_shared<RSNode>(true, true);
    animation->target_ = target;
    std::shared_ptr<RSRenderAnimation> uiAnimation =
        std::make_unique<RSRenderAnimation>();
    animation->uiAnimation_ = uiAnimation;
    animation->InteractivePause();
    EXPECT_NE(nullptr, animation->uiAnimation_);
    GTEST_LOG_(INFO) << "RSAnimationTest InteractivePause end";
}

/**
 * @tc.name: InteractiveContinue
 * @tc.desc: Verify the InteractiveContinue of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, InteractiveContinue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveContinue start";
    /**
     * @tc.steps: step1. init InteractiveContinue
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start InteractiveContinue test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::RUNNING;
    animation->InteractiveContinue();
    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveContinue();
    std::shared_ptr<RSNode> target = std::make_shared<RSNode>(true, true);
    animation->target_ = target;
    std::shared_ptr<RSRenderAnimation> uiAnimation =
        std::make_unique<RSRenderAnimation>();
    animation->uiAnimation_ = uiAnimation;
    animation->InteractiveContinue();
    EXPECT_NE(nullptr, animation->uiAnimation_);
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveContinue end";
}

/**
 * @tc.name: InteractiveReverse
 * @tc.desc: Verify the InteractiveReverse of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, InteractiveReverse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveReverse start";
    /**
     * @tc.steps: step1. init InteractiveReverse
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start InteractiveReverse test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::FINISHED;
    animation->InteractiveReverse();
    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveReverse();
    std::shared_ptr<RSNode> target = std::make_shared<RSNode>(true, true);
    animation->target_ = target;
    std::shared_ptr<RSRenderAnimation> uiAnimation =
        std::make_unique<RSRenderAnimation>();
    animation->uiAnimation_ = uiAnimation;
    animation->InteractiveReverse();
    EXPECT_NE(nullptr, animation->uiAnimation_);
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveReverse end";
}

/**
 * @tc.name: Finish
 * @tc.desc: Verify the Finish of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, Finish, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest Finish start";
    /**
     * @tc.steps: step1. init Finish
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    /**
     * @tc.steps: step2. start Finish test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->Finish();
    animation->state_ = RSAnimation::AnimationState::RUNNING;
    animation->Finish();
    std::weak_ptr<RSNode> rsNode = animation->GetTarget();
    EXPECT_FALSE(rsNode.lock() != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest Finish end";
}

/**
 * @tc.name: PropertyAnimationGetModifierTypeTest001
 * @tc.desc: Verify the GetModifierType of PropertyAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, PropertyAnimationGetModifierTypeTest001, TestSize.Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSCurveAnimation>(property, endProperty);
    ASSERT_NE(animation, nullptr);
    EXPECT_EQ(animation->GetModifierType(), RSModifierType::INVALID);
}

/**
 * @tc.name: IsSupportInteractiveAnimator001
 * @tc.desc: Verify the IsSupportInteractiveAnimator of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, IsSupportInteractiveAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest IsSupportInteractiveAnimator001 start";
    /**
     * @tc.steps: step1. init IsSupportInteractiveAnimator
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start IsSupportInteractiveAnimator test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsSupportInteractiveAnimator());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest IsSupportInteractiveAnimator001 end";
}

/**
 * @tc.name: Pause
 * @tc.desc: Verify the Pause of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, Pause, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest Pause start";
    /**
     * @tc.steps: step1. init Pause
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start Pause test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::RUNNING;
    animation->target_.reset();
    animation->Pause();
    std::weak_ptr<RSNode> rsNode = animation->GetTarget();
    EXPECT_FALSE(rsNode.lock() != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest Pause end";
}

/**
 * @tc.name: InteractiveSetFraction
 * @tc.desc: Verify the InteractiveSetFraction of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, InteractiveSetFraction, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveSetFraction start";
    /**
     * @tc.steps: step1. init InteractiveSetFraction
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start InteractiveSetFraction test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::FINISHED;
    animation->InteractiveSetFraction(3.14f);

    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveSetFraction(3.14f);
    std::shared_ptr<RSNode> target = std::make_shared<RSNode>(true, true);
    animation->target_ = target;
    std::shared_ptr<RSRenderAnimation> uiAnimation =
        std::make_unique<RSRenderAnimation>();
    animation->uiAnimation_ = uiAnimation;
    animation->InteractiveSetFraction(3.14f);
    EXPECT_NE(nullptr, animation->uiAnimation_);
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveSetFraction end";
}

/**
 * @tc.name: AnimationStatus003
 * @tc.desc: Verify the AnimationStatus of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationStatus003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus003 start";
    /**
     * @tc.steps: step1. init AnimationStatus
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start AnimationStatus test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsRunning());
    animation->Pause();
    EXPECT_TRUE(animation->IsPaused());
    animation->Reverse();
    animation->Resume();
    EXPECT_TRUE(animation->IsRunning());
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus003 end";
}

/**
 * @tc.name: InteractiveFinish
 * @tc.desc: Verify the InteractiveFinish of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, InteractiveFinish, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveFinish start";
    /**
     * @tc.steps: step1. init InteractiveFinish
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start InteractiveFinish test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::FINISHED;
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);

    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);

    std::shared_ptr<RSNode> target = std::make_shared<RSNode>(true, true);
    animation->target_ = target;
    std::shared_ptr<RSRenderAnimation> uiAnimation =
        std::make_unique<RSRenderAnimation>();
    animation->uiAnimation_ = uiAnimation;
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);
    EXPECT_NE(nullptr, animation->uiAnimation_);
    GTEST_LOG_(INFO) << "RSAnimationTest InteractiveFinish end";
}

/**
 * @tc.name: AnimationGetId003
 * @tc.desc: Verify the AnimationGetId of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationGetId003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId003 start";
    /**
     * @tc.steps: step1. init AnimationGetId
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start AnimationGetId test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    AnimationId id = animation->GetId();
    EXPECT_TRUE(id != 0);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId003 end";
}

/**
 * @tc.name: AnimationSetFinishCallback003
 * @tc.desc: Verify the AnimationSetFinishCallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSetFinishCallback003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback003 start";
    /**
     * @tc.steps: step1. init AnimationSetFinishCallback
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    std::string testString;
    auto lambda = [&testString]() { testString = SUCCESS_STRING; };
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    }, lambda);
    /**
     * @tc.steps: step2. start AnimationSetFinishCallback test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsRunning());
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
    EXPECT_STREQ(SUCCESS_STRING.c_str(), testString.c_str());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback003 end";
}

/**
 * @tc.name: AnimationGetTarget003
 * @tc.desc: Verify the AnimationGetTarget of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationGetTarget003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget003 start";
    /**
     * @tc.steps: step1. init AnimationGetTarget
     */
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start AnimationGetTarget test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsRunning());
    auto target = animation->GetTarget();
    EXPECT_TRUE(target.lock() != nullptr);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget003 end";
}
} // namespace Rosen
} // namespace OHOS