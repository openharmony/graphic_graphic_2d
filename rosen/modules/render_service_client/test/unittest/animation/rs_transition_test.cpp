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

#include "animation/rs_transition.h"
#include "animation/rs_transition_effect.h"
#include "modifier/rs_extended_modifier.h"
#include "common/rs_color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSTransitionTest : public RSAnimationBaseTest {
};

class TransitionModifier : public RSTransitionModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        if (!alpha_ || !rect_ || !backgroundColor_) {
            auto rect = Drawing::Rect(0, 0, 0, 0);
            Drawing::Brush brush;
            context.canvas->AttachBrush(brush);
            context.canvas->DrawRect(rect);
            context.canvas->DetachBrush();
            return;
        }
        auto rect = Drawing::Rect(rect_->Get().x_, rect_->Get().y_, rect_->Get().z_, rect_->Get().w_);
        Drawing::Brush brush;
        brush.SetColor(backgroundColor_->Get().AsArgbInt());
        brush.SetAlphaF(alpha_->Get());
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(rect);
        context.canvas->DetachBrush();
    }
    void SetRect(Vector4f rect)
    {
        if (rect_ == nullptr) {
            rect_ = std::make_shared<RSAnimatableProperty<Vector4f>>(rect);
            AttachProperty(rect_);
        } else {
            rect_->Set(rect);
        }
    }
    void SetBackgroundColor(Color backgroundColor)
    {
        if (backgroundColor_ == nullptr) {
            backgroundColor_ = std::make_shared<RSAnimatableProperty<Color>>(backgroundColor);
            AttachProperty(backgroundColor_);
        } else {
            backgroundColor_->Set(backgroundColor);
        }
    }
    void SetAlpha(float alpha)
    {
        if (alpha_ == nullptr) {
            alpha_ = std::make_shared<RSAnimatableProperty<float>>(alpha);
            AttachProperty(alpha_);
        } else {
            alpha_->Set(alpha);
        }
    }
    void Active() override
    {
        SetRect({ 0, 200, 0, 0 });
        SetBackgroundColor(Color(255, 0, 0));
        SetAlpha(0);
    }
    void Identity() override
    {
        SetRect({ 0, 200, 400, 200 });
        SetBackgroundColor(Color(0, 0, 255));
        SetAlpha(1);
    }
private:
    std::shared_ptr<RSAnimatableProperty<Vector4f>> rect_;
    std::shared_ptr<RSAnimatableProperty<Color>> backgroundColor_;
    std::shared_ptr<RSAnimatableProperty<float>> alpha_;
};

/*
 * @tc.name: CustomTransitionEffectTest001
 * @tc.desc: Verify the custom transition effect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, CustomTransitionEffectTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest CustomTransitionEffectTest001 start";

    /**
     * @tc.steps: step1. init custom transition effect
     */
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetBounds(250, 500, 200, 200);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    auto transitionModifier = std::make_shared<TransitionModifier>();
    newCanvasNode->AddModifier(transitionModifier);
    auto transitionInEffect = RSTransitionEffect::Create()->Custom(transitionModifier);
    auto transitionOutEffect = RSTransitionEffect::Create()->Custom(transitionModifier);
    newCanvasNode->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));

    /**
     * @tc.steps: step2. start transition-in test
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    bool isFinished = false;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        rootNode->AddChild(newCanvasNode, -1);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    /**
     * @tc.steps: step3. start transition-out test
     */
    isFinished = false;
    animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        rootNode->RemoveChild(newCanvasNode);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    GTEST_LOG_(INFO) << "RSTransitionTest CustomTransitionEffectTest001 end";
}

/*
 * @tc.name: CustomTransitionEffectTest002
 * @tc.desc: Verify the custom transition effect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, CustomTransitionEffectTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest CustomTransitionEffectTest002 start";

    /**
     * @tc.steps: step1. init custom transition effect
     */
    auto newCanvasNode = RSCanvasNode::Create();
    rootNode->AddChild(newCanvasNode, -1);
    newCanvasNode->SetBounds(250, 500, 200, 200);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    auto transitionModifier = std::make_shared<TransitionModifier>();
    newCanvasNode->AddModifier(transitionModifier);
    auto transitionInEffect = RSTransitionEffect::Create()->Custom(transitionModifier);
    auto transitionOutEffect = RSTransitionEffect::Create()->Custom(transitionModifier);
    newCanvasNode->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));

    /**
     * @tc.steps: step2. start transition-in test
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    newCanvasNode->SetVisible(false);
    bool isFinished = false;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->SetVisible(true);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    /**
     * @tc.steps: step3. start transition-out test
     */
    isFinished = false;
    animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->SetVisible(false);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    GTEST_LOG_(INFO) << "RSTransitionTest CustomTransitionEffectTest002 end";
}

/*
 * @tc.name: CustomTransitionEffectTest003
 * @tc.desc: Verify the custom transition effect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, CustomTransitionEffectTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest CustomTransitionEffectTest003 start";

    /**
     * @tc.steps: step1. init custom transition effect
     */
    auto newCanvasNode = RSCanvasNode::Create();
    rootNode->AddChild(newCanvasNode, -1);
    newCanvasNode->SetBounds(250, 500, 200, 200);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    auto transitionModifier = std::make_shared<TransitionModifier>();
    newCanvasNode->AddModifier(transitionModifier);
    auto transitionInEffect = RSTransitionEffect::Create()->Custom(transitionModifier);
    auto transitionOutEffect = RSTransitionEffect::Create()->Custom(transitionModifier);
    newCanvasNode->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));

    /**
     * @tc.steps: step2. start transition-in test
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    bool isFinished = false;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->NotifyTransition(transitionInEffect, true);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    /**
     * @tc.steps: step3. start transition-out test
     */
    isFinished = false;
    animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->NotifyTransition(transitionOutEffect, false);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    GTEST_LOG_(INFO) << "RSTransitionTest CustomTransitionEffectTest003 end";
}

/*
 * @tc.name: CombinedTransitionEffectTest001
 * @tc.desc: Verify the custom transition effect with built-in transition effect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, CombinedTransitionEffectTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest CombinedTransitionEffectTest001 start";

    /**
     * @tc.steps: step1. init custom transition effect with built-in transition effect
     */
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetBounds(250, 500, 200, 200);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    auto transitionModifier = std::make_shared<TransitionModifier>();
    newCanvasNode->AddModifier(transitionModifier);
    auto transitionInEffect =
            RSTransitionEffect::Create()
            ->Custom(transitionModifier)
            ->Opacity(0)
            ->Translate({ -250, 0, 0 });
    auto transitionOutEffect =
            RSTransitionEffect::Create()
            ->Custom(transitionModifier)
            ->Opacity(0)
            ->Translate({ 250, 0, 0 });
    newCanvasNode->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));

    /**
     * @tc.steps: step2. start transition-in test
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    bool isFinished = false;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        rootNode->AddChild(newCanvasNode, -1);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    /**
     * @tc.steps: step3. start transition-out test
     */
    isFinished = false;
    animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        rootNode->RemoveChild(newCanvasNode);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    GTEST_LOG_(INFO) << "RSTransitionTest CombinedTransitionEffectTest001 end";
}

/*
 * @tc.name: CombinedTransitionEffectTest002
 * @tc.desc: Verify the custom transition effect with built-in transition effect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, CombinedTransitionEffectTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest CombinedTransitionEffectTest002 start";

    /**
     * @tc.steps: step1. init custom transition effect with built-in transition effect
     */
    auto newCanvasNode = RSCanvasNode::Create();
    rootNode->AddChild(newCanvasNode, -1);
    newCanvasNode->SetBounds(250, 500, 200, 200);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    auto transitionModifier = std::make_shared<TransitionModifier>();
    newCanvasNode->AddModifier(transitionModifier);
    auto transitionInEffect =
            RSTransitionEffect::Create()
            ->Custom(transitionModifier)
            ->Opacity(0)
            ->Translate({ -250, 0, 0 });
    auto transitionOutEffect =
            RSTransitionEffect::Create()
            ->Custom(transitionModifier)
            ->Opacity(0)
            ->Translate({ 250, 0, 0 });
    newCanvasNode->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));

    /**
     * @tc.steps: step2. start transition-in test
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    newCanvasNode->SetVisible(false);
    bool isFinished = false;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->SetVisible(true);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    /**
     * @tc.steps: step3. start transition-out test
     */
    isFinished = false;
    animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->SetVisible(false);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    GTEST_LOG_(INFO) << "RSTransitionTest CombinedTransitionEffectTest002 end";
}

/*
 * @tc.name: CombinedTransitionEffectTest003
 * @tc.desc: Verify the custom transition effect with built-in transition effect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, CombinedTransitionEffectTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest CombinedTransitionEffectTest003 start";

    /**
     * @tc.steps: step1. init custom transition effect with built-in transition effect
     */
    auto newCanvasNode = RSCanvasNode::Create();
    rootNode->AddChild(newCanvasNode, -1);
    newCanvasNode->SetBounds(250, 500, 200, 200);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    auto transitionModifier = std::make_shared<TransitionModifier>();
    newCanvasNode->AddModifier(transitionModifier);
    auto transitionInEffect =
            RSTransitionEffect::Create()
            ->Custom(transitionModifier)
            ->Opacity(0)
            ->Translate({ -250, 0, 0 });
    auto transitionOutEffect =
            RSTransitionEffect::Create()
            ->Custom(transitionModifier)
            ->Opacity(0)
            ->Translate({ 250, 0, 0 });
    newCanvasNode->SetTransitionEffect(RSTransitionEffect::Asymmetric(transitionInEffect, transitionOutEffect));

    /**
     * @tc.steps: step2. start transition-in test
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    bool isFinished = false;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->NotifyTransition(transitionInEffect, true);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    /**
     * @tc.steps: step3. start transition-out test
     */
    isFinished = false;
    animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE, [&]() {
        newCanvasNode->NotifyTransition(transitionOutEffect, false);
    }, [&isFinished]() {
        isFinished = true;
    });
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    EXPECT_TRUE(isFinished == true);

    GTEST_LOG_(INFO) << "RSTransitionTest CombinedTransitionEffectTest003 end";
}

/**
 * @tc.name: SetTransitionEffectTest001
 * @tc.desc: Verify the SetTransitionEffect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, SetTransitionEffectTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest001 start";
    /**
     * @tc.steps: step1. init SetTransitionEffect
     */
    auto transition = std::make_shared<RSTransition>(RSTransitionEffect::EMPTY, true);
    /**
     * @tc.steps: step2. start SetTransitionEffect test
     */
    transition->SetTransitionEffect(RSTransitionEffect::Create()->Rotate(TRANSITION_EFFECT_ROTATE));

    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest001 end";
}

/**
 * @tc.name: SetTransitionEffectTest002
 * @tc.desc: Verify the SetTransitionEffect of TransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, SetTransitionEffectTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest002 start";
    /**
     * @tc.steps: step1. init SetTransitionEffect
     */
    auto transition = std::make_shared<RSTransition>(RSTransitionEffect::EMPTY, true);
    /**
     * @tc.steps: step2. start SetTransitionEffect test
     */
    transition->SetTransitionEffect(RSTransitionEffect::Create()->Scale({ 0.f, 0.f, 0.f }));
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest002 end";
}

/**
 * @tc.name: SetTransitionEffectTest003
 * @tc.desc: Verify the SetTransitionEffect of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, SetTransitionEffectTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest003 start";
    /**
     * @tc.steps: step1. init SetTransitionEffect
     */
    auto transition = std::make_shared<RSTransition>(RSTransitionEffect::EMPTY, true);
    /**
     * @tc.steps: step2. start SetTransitionEffect test
     */
    transition->SetTransitionEffect(RSTransitionEffect::Create()->Opacity(0));
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest003 end";
}

/**
 * @tc.name: SetTimingCurveTest001
 * @tc.desc: Verify the SetTimingCurve of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, SetTimingCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTimingCurveTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto transition = std::make_shared<RSTransition>(RSTransitionEffect::OPACITY, true);
    transition->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest SetTimingCurveTest001 end";
}

/**
 * @tc.name: SetTimingCurveTest002
 * @tc.desc: Verify the SetTimingCurveTest of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, SetTimingCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTimingCurveTest002 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto transition = std::make_shared<RSTransition>(RSTransitionEffect::OPACITY, true);
    transition->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest SetTimingCurveTest002 end";
}

/**
 * @tc.name: SetTimingCurveTest003
 * @tc.desc: Verify the SetTimingCurveTest of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, SetTimingCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTimingCurveTest003 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto transition = std::make_shared<RSTransition>(RSTransitionEffect::SCALE, true);
    transition->SetTimingCurve(RSAnimationTimingCurve::EASE_IN);
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest SetTimingCurveTest003 end";
}

/**
 * @tc.name: RSNodeAnimateTest001
 * @tc.desc: Verify the RSNodeAnimate of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, RSNodeAnimateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest001 start";
    /**
     * @tc.steps: step1. init RSNodeAnimate
     */
    auto effect = RSTransitionEffect::Create()->Translate({500.f, 300.f, 0.f});
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetBoundsWidth(200.f);
    newCanvasNode->SetBoundsHeight(300.f);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    newCanvasNode->SetTransitionEffect(effect);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode]() {
        rootNode->AddChild(newCanvasNode, -1);
    });

    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest001 end";
}

/**
 * @tc.name: RSNodeAnimateTest002
 * @tc.desc: Verify the RSNodeAnimate of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, RSNodeAnimateTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest002 start";
    /**
     * @tc.steps: step1. init RSNodeAnimate
     */
    auto effect = RSTransitionEffect::Create()->Opacity(0.5f);
    auto newCanvasNode = RSCanvasNode::Create();
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    newCanvasNode->SetTransitionEffect(effect);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode]() {
        newCanvasNode->SetVisible(false);
    });

    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest002 end";
}

/**
 * @tc.name: RSNodeAnimateTest003
 * @tc.desc: Verify the RSNodeAnimate of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, RSNodeAnimateTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest003 start";
    /**
     * @tc.steps: step1. init RSNodeAnimate
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
     * @tc.steps: step2. start RSNodeAnimate test
     */
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() != CORRECT_SIZE) {
        return;
    }
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest003 end";
}
} // namespace Rosen
} // namespace OHOS