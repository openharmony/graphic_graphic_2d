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
class RSInteractiveImplictAnimatorTest : public RSAnimationBaseTest {
public:
    static constexpr float FRACTION_1 = 0.5f;
};
#if defined(MODIFIER_NG)
#else
/**
 * @tc.name: AddAnimation
 * @tc.desc: Verify the AddAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest AddAnimation001 start";

    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->AddAnimation([&]() {
        property->Set(ANIMATION_START_BOUNDS);
    });
    NotifyStartAnimation();

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest AddAnimation001 end";
}

/**
 * @tc.name: AddAnimation
 * @tc.desc: Verify the AddAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest AddAnimation002 start";

    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(0);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    NotifyStartAnimation();

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest AddAnimation002 end";
}

/**
 * @tc.name: StartAnimation
 * @tc.desc: Verify the StartAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, StartAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->StartAnimation();
    animator->AddAnimation([&]() {});
    animator->StartAnimation();
    NotifyStartAnimation();

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation001 end";
}

/**
 * @tc.name: PauseAnimation
 * @tc.desc: Verify the PauseAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, PauseAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest PauseAnimation001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->PauseAnimation();
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->PauseAnimation();
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest PauseAnimation001 end";
}

/**
 * @tc.name: ContinueAnimation
 * @tc.desc: Verify the ContinueAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ContinueAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ContinueAnimation001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->ContinueAnimation();
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->PauseAnimation();
    animator->ContinueAnimation();
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ContinueAnimation001 end";
}

/**
 * @tc.name: FinishAnimation
 * @tc.desc: Verify the FinishAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishAnimation001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->FinishAnimation(RSInteractiveAnimationPosition::END);
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->FinishAnimation(RSInteractiveAnimationPosition::END);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishAnimation001 end";
}

/**
 * @tc.name: FinishAnimation
 * @tc.desc: Verify the FinishAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishAnimation002 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->FinishAnimation(RSInteractiveAnimationPosition::START);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishAnimation002 end";
}

/**
 * @tc.name: FinishAnimation
 * @tc.desc: Verify the FinishAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishAnimation003 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->FinishAnimation(RSInteractiveAnimationPosition::CURRENT);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishAnimation003 end";
}

/**
 * @tc.name: ReverseAnimation
 * @tc.desc: Verify the ReverseAnimation of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ReverseAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ReverseAnimation001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->ReverseAnimation();
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->PauseAnimation();
    animator->ReverseAnimation();
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ReverseAnimation001 end";
}

/**
 * @tc.name: SetFraction
 * @tc.desc: Verify the SetFraction of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SetFraction001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    animator->SetFraction(FRACTION_1);
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    sleep(DELAY_TIME_ONE);
    animator->PauseAnimation();
    animator->SetFraction(FRACTION_1);
    sleep(DELAY_TIME_ONE);
    auto fraction = animator->GetFraction();
    EXPECT_TRUE(fraction == 0.0f);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SetFraction001 end";
}

/**
 * @tc.name: GetFraction
 * @tc.desc: Verify the GetFraction of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest GetFraction001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    auto fraction = animator->GetFraction();
    EXPECT_TRUE(fraction == 0.0f);
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    animator->StartAnimation();
    animator->PauseAnimation();
    animator->SetFraction(FRACTION_1);
    fraction = animator->GetFraction();
    EXPECT_TRUE(fraction == 0.0f);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest GetFraction001 end";
}

/**
 * @tc.name: SetFinishCallBack
 * @tc.desc: Verify the SetFinishCallBack of RSInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SetFinishCallBack001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SetFinishCallBack001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animator = RSInteractiveImplictAnimator::Create(protocol, curve);
    EXPECT_TRUE(animator != nullptr);
    int testNum = 0;
    animator->SetFinishCallBack([&]() {
        testNum = 1;
    });
    animator->AddAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });

    auto callBack = animator->GetAnimatorFinishCallback();
    EXPECT_TRUE(callBack != nullptr);

    animator->StartAnimation();
    NotifyStartAnimation();
    sleep(DELAY_TIME_ONE);

    protocol.SetDuration(0);
    auto animator2 = RSInteractiveImplictAnimator::Create(protocol, curve);
    // state
    auto res = 1;
    animator2->state_ = RSInteractiveAnimationState::RUNNING;
    res = animator2->AddImplictAnimation(nullptr);
    animator2->state_ = RSInteractiveAnimationState::INACTIVE;
    EXPECT_TRUE(res == 0);
    // protocol
    res = 1;
    res = animator2->AddImplictAnimation([&]() {
        RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS);
        });
    });
    EXPECT_TRUE(res == 0);

    protocol.SetDuration(ANIMATION_DURATION);
    auto animator3 = RSInteractiveImplictAnimator::Create(protocol, curve);
    animator3->AddImplictAnimation([&]()
        { RSNode::Animate(protocol, curve, [&]() {
            property->Set(ANIMATION_END_BOUNDS * 2);
        });
    });
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SetFinishCallBack001 end";
}
#endif
} // namespace Rosen
} // namespace OHOS