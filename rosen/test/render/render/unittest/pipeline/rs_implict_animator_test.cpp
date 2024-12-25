/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "animation/rs_interactive_implict_animator.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSImplicAnimationTest : public RSAnimationBaseTest {
public:
    static constexpr float FRACTION_1 = 0.5f;
};

/**
 * @tc.name: AddAnimation
 * @tc.desc: Verify the AddAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, AddAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest AddAnimation001 start";

    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->AddAnimation([&]() {
        animationProperty->Set(ANIMATION_START_BOUNDS);
    });
    NotifyStartAnimation();

    GTEST_LOG_(INFO) << "RSImplicAnimationTest AddAnimation001 end";
}

/**
 * @tc.name: AddAnimation
 * @tc.desc: Verify the AddAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, AddAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest AddAnimation002 start";

    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(0);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    NotifyStartAnimation();

    GTEST_LOG_(INFO) << "RSImplicAnimationTest AddAnimation002 end";
}

/**
 * @tc.name: StartAnimation
 * @tc.desc: Verify the StartAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, StartAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest StartAnimation001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->StartAnimation();
    interactiveAnimator->AddAnimation([&]() {});
    interactiveAnimator->StartAnimation();
    NotifyStartAnimation();

    GTEST_LOG_(INFO) << "RSImplicAnimationTest StartAnimation001 end";
}

/**
 * @tc.name: PauseAnimation
 * @tc.desc: Verify the PauseAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, PauseAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest PauseAnimation001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->PauseAnimation();
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->PauseAnimation();
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest PauseAnimation001 end";
}

/**
 * @tc.name: ContinueAnimation
 * @tc.desc: Verify the ContinueAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, ContinueAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest ContinueAnimation001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->ContinueAnimation();
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->PauseAnimation();
    interactiveAnimator->ContinueAnimation();
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest ContinueAnimation001 end";
}

/**
 * @tc.name: FinishAnimation
 * @tc.desc: Verify the FinishAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, FinishAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest FinishAnimation001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->FinishAnimation(RSInteractiveAnimationPosition::END);
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->FinishAnimation(RSInteractiveAnimationPosition::END);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest FinishAnimation001 end";
}

/**
 * @tc.name: FinishAnimation
 * @tc.desc: Verify the FinishAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, FinishAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest FinishAnimation002 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->FinishAnimation(RSInteractiveAnimationPosition::START);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest FinishAnimation002 end";
}

/**
 * @tc.name: FinishAnimation
 * @tc.desc: Verify the FinishAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, FinishAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest FinishAnimation003 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->FinishAnimation(RSInteractiveAnimationPosition::CURRENT);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest FinishAnimation003 end";
}

/**
 * @tc.name: ReverseAnimation
 * @tc.desc: Verify the ReverseAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, ReverseAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest ReverseAnimation001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->ReverseAnimation();
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->PauseAnimation();
    interactiveAnimator->ReverseAnimation();
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest ReverseAnimation001 end";
}

/**
 * @tc.name: SetFraction
 * @tc.desc: Verify the SetFraction of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, SetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest SetFraction001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    interactiveAnimator->SetFraction(FRACTION_1);
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    sleep(DELAY_TIME_ONE);
    interactiveAnimator->PauseAnimation();
    interactiveAnimator->SetFraction(FRACTION_1);
    sleep(DELAY_TIME_ONE);
    auto fraction = interactiveAnimator->GetFraction();
    EXPECT_TRUE(fraction == 0.0f);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest SetFraction001 end";
}

/**
 * @tc.name: GetFraction
 * @tc.desc: Verify the GetFraction of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, GetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest GetFraction001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    auto fraction = interactiveAnimator->GetFraction();
    EXPECT_TRUE(fraction == 0.0f);
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    interactiveAnimator->StartAnimation();
    interactiveAnimator->PauseAnimation();
    interactiveAnimator->SetFraction(FRACTION_1);
    fraction = interactiveAnimator->GetFraction();
    EXPECT_TRUE(fraction == 0.0f);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSImplicAnimationTest GetFraction001 end";
}

/**
 * @tc.name: SetFinishCallBack
 * @tc.desc: Verify the SetFinishCallBack of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicAnimationTest, SetFinishCallBack001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicAnimationTest SetFinishCallBack001 start";
    auto animationProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto newModifier = std::make_shared<RSBoundsModifier>(animationProperty);
    canvasNode->AddModifier(newModifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto interactiveAnimator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(interactiveAnimator != nullptr);
    int testNum = 0;
    interactiveAnimator->SetFinishCallBack([&]() {
        testNum = 1;
    });
    interactiveAnimator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });

    auto callBack = interactiveAnimator->GetAnimatorFinishCallback();
    EXPECT_TRUE(callBack != nullptr);

    interactiveAnimator->StartAnimation();
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);

    protocol.SetDuration(0);
    auto interactiveAnimator2 = RSInteractiveImplictAnimator::Create(protocol, curve);
    // state
    auto res = 1;
    interactiveAnimator2->state_ = RSInteractiveAnimationState::RUNNING;
    res = interactiveAnimator2->AddImplictAnimation(nullptr);
    interactiveAnimator2->state_ = RSInteractiveAnimationState::INACTIVE;
    EXPECT_TRUE(res == 0);
    // protocol
    res = 1;
    res = interactiveAnimator2->AddImplictAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS);
        });
    });
    EXPECT_TRUE(res == 0);

    protocol.SetDuration(ANIMATION_DURATION);
    auto interactiveAnimator3 = RSInteractiveImplictAnimator::Create(protocol, curve);
    interactiveAnimator3->AddImplictAnimation([&]()
        { RSNode::Animate(protocol, curve, [&]() {
            animationProperty->Set(ANIMATION_END_BOUNDS * 2);
        });
    });
    GTEST_LOG_(INFO) << "RSImplicAnimationTest SetFinishCallBack001 end";
}
} // namespace Rosen
} // namespace OHOS