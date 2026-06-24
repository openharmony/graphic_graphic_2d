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
#include "rs_animation_test_utils.h"

#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_animation.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_property_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "render/rs_path.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;

class RSAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSUIContext> rsUIContext;
    std::shared_ptr<RSCanvasNode> canvasNode;
};

void RSAnimationTest::SetUpTestCase() {}
void RSAnimationTest::TearDownTestCase() {}

void RSAnimationTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    canvasNode->SetBounds(ANIMATION_START_BOUNDS);
    canvasNode->SetFrame(ANIMATION_START_BOUNDS);
}

void RSAnimationTest::TearDown() {}

/**
 * @tc.name: AnimationGetId001
 * @tc.desc: Verify the AnimationGetId of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationGetId001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    AnimationId id = animation->GetId();
    EXPECT_TRUE(id != 0);
    animation->Finish();
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    AnimationId id = animation->GetId();
    EXPECT_TRUE(id != 0);
    animation->Finish();
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    auto target = animation->GetTarget();
    EXPECT_TRUE(target.lock() != nullptr);
    animation->Finish();
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    auto target = animation->GetTarget();
    EXPECT_TRUE(target.lock() != nullptr);
    animation->Finish();
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::LINEAR;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

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
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);

    animation->state_ = Rosen::RSAnimation::AnimationState::PAUSED;
    animation->SetFraction(0.5);

    animation->state_ = RSAnimation::AnimationState::RUNNING;
    animation->Pause();

    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    animation->OnReverse();
    animation->Resume();
    animation->Finish();
    animation->Reverse();

    animation->target_.reset();
    animation->uiAnimation_.reset();
    animation->Resume();
    animation->OnFinish();
    animation->OnReverse();
    animation->OnSetFraction(0.5);

    auto propAnimation = std::make_shared<RSPropertyAnimation>(rsUIContext, nullptr);
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
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
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
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::RUNNING;
    animation->InteractiveContinue();
    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveContinue();
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::FINISHED;
    animation->InteractiveReverse();
    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveReverse();
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
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
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<ModifierNG::RSBoundsModifier>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, property);
    canvasNode->AddModifier(modifier);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(rsUIContext, protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });

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
 * @tc.name: IsSupportInteractiveAnimator001
 * @tc.desc: Verify the IsSupportInteractiveAnimator of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, IsSupportInteractiveAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest IsSupportInteractiveAnimator001 start";
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsSupportInteractiveAnimator());
    animation->Finish();
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
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });

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
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::FINISHED;
    animation->InteractiveSetFraction(3.14f);

    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveSetFraction(3.14f);
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
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
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsRunning());
    animation->Pause();
    EXPECT_TRUE(animation->IsPaused());
    animation->Reverse();
    animation->Resume();
    EXPECT_TRUE(animation->IsRunning());
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
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSAnimation>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    animation->state_ = RSAnimation::AnimationState::FINISHED;
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);

    animation->state_ = RSAnimation::AnimationState::PAUSED;
    animation->target_.reset();
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);

    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
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
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    AnimationId id = animation->GetId();
    EXPECT_TRUE(id != 0);
    animation->Finish();
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
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    std::string testString;
    auto lambda = [&testString]() { testString = SUCCESS_STRING; };
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    }, lambda);

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsRunning());
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
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
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto newRsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    auto newCanvasNode = RSCanvasNode::Create(false, false, newRsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(newRsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });

    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    ASSERT_TRUE(animation != nullptr);
    EXPECT_TRUE(animation->IsRunning());
    auto target = animation->GetTarget();
    EXPECT_TRUE(target.lock() != nullptr);
    animation->Finish();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget003 end";
}
} // namespace Rosen
} // namespace OHOS
