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
#include "animation/rs_curve_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "render/rs_path.h"
#include "ui/rs_canvas_node.h"

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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
        AnimationId id = animation->GetId();
        EXPECT_TRUE(id != 0);
        NotifyStartAnimation();
    }
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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
        AnimationId id = animation->GetId();
        EXPECT_TRUE(id != 0);
        NotifyStartAnimation();
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId002 end";
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        AnimationId id = animation->GetId();
        EXPECT_TRUE(id != 0);
        NotifyStartAnimation();
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetId003 end";
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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
        NotifyStartAnimation();
        sleep(DELAY_TIME_ONE);
        animation->Finish();
        EXPECT_TRUE(animation->IsFinished());
        EXPECT_STREQ(SUCCESS_STRING.c_str(), testString.c_str());
    }
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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
        NotifyStartAnimation();
        sleep(DELAY_TIME_ONE);
        animation->Finish();
        EXPECT_TRUE(animation->IsFinished());
        EXPECT_STREQ(SUCCESS_STRING.c_str(), testString.c_str());
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback002 end";
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        EXPECT_TRUE(animation->IsRunning());
        NotifyStartAnimation();
        sleep(DELAY_TIME_ONE);
        animation->Finish();
        EXPECT_TRUE(animation->IsFinished());
        EXPECT_STREQ(SUCCESS_STRING.c_str(), testString.c_str());
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSetFinishCallback003 end";
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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
        auto target = animation->GetTarget();
        EXPECT_TRUE(target.lock() != nullptr);
        NotifyStartAnimation();
    }
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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
        auto target = animation->GetTarget();
        EXPECT_TRUE(target.lock() != nullptr);
        NotifyStartAnimation();
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget002 end";
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
        EXPECT_TRUE(animation->IsRunning());
        auto target = animation->GetTarget();
        EXPECT_TRUE(target.lock() != nullptr);
        NotifyStartAnimation();
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGetTarget003 end";
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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
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
    }
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
    auto modifier = std::make_shared<RSBoundsModifier>(property);
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSCurveAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
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
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus002 end";
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
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    auto animation = std::static_pointer_cast<RSTransition>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animation != nullptr);
    if (animation != nullptr) {
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
    }
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatus003 end";
}
} // namespace Rosen
} // namespace OHOS