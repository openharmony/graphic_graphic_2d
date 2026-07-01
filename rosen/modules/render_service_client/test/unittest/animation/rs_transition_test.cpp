/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "animation/rs_transition.h"
#include "animation/rs_transition_effect.h"
#include "common/rs_color.h"
#include "rs_animation_test_utils.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSTransitionTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<RSUIContext> rsUIContext;
    std::shared_ptr<RSCanvasNode> canvasNode;
};
void RSTransitionTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
}
void RSTransitionTest::TearDown()
{
    canvasNode->RemoveAllAnimations();
}
/**
 * @tc.name: SetTransitionEffectTest
 * @tc.desc: Verify the SetTransitionEffect
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionTest, SetTransitionEffectTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest start";
    RSTransition transition(rsUIContext, nullptr, false);
    ASSERT_FALSE(transition.effect_);
    auto effect = RSTransitionEffect::Create();
    transition.SetTransitionEffect(effect);
    ASSERT_TRUE(transition.effect_);
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest end";
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
    auto transition = std::make_shared<RSTransition>(rsUIContext, RSTransitionEffect::EMPTY, true);
    /**
     * @tc.steps: step2. start SetTransitionEffect test
     */
    transition->SetTransitionEffect(RSTransitionEffect::Create()->Rotate(TRANSITION_EFFECT_ROTATE));
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
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
    auto transition = std::make_shared<RSTransition>(rsUIContext, RSTransitionEffect::EMPTY, true);
    /**
     * @tc.steps: step2. start SetTransitionEffect test
     */
    transition->SetTransitionEffect(RSTransitionEffect::Create()->Scale({ 0.f, 0.f, 0.f }));
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
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
    auto transition = std::make_shared<RSTransition>(rsUIContext, RSTransitionEffect::EMPTY, true);
    /**
     * @tc.steps: step2. start SetTransitionEffect test
     */
    transition->SetTransitionEffect(RSTransitionEffect::Create()->Opacity(0));
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
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
    auto transition = std::make_shared<RSTransition>(rsUIContext, RSTransitionEffect::OPACITY, true);
    transition->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
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
    auto transition = std::make_shared<RSTransition>(rsUIContext, RSTransitionEffect::OPACITY, true);
    transition->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
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
    auto transition = std::make_shared<RSTransition>(rsUIContext, RSTransitionEffect::SCALE, true);
    transition->SetTimingCurve(RSAnimationTimingCurve::EASE_IN);
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(transition == nullptr);
    EXPECT_FALSE(transition->IsStarted());
    transition->Start(canvasNode);
    EXPECT_TRUE(transition->IsRunning());
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
    auto rootNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto effect = RSTransitionEffect::Create()->Translate({500.f, 300.f, 0.f});
    auto newCanvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    newCanvasNode->SetBoundsWidth(200.f);
    newCanvasNode->SetBoundsHeight(300.f);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    newCanvasNode->SetTransitionEffect(effect);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &rootNode]() {
        rootNode->AddChild(newCanvasNode, -1);
    });
    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
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
    auto rootNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto effect = RSTransitionEffect::Create()->Opacity(0.5f);
    auto newCanvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    newCanvasNode->SetTransitionEffect(effect);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode]() {
        newCanvasNode->SetVisible(false);
    });
    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
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
    auto rootNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    auto newCanvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    newCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    newCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(newCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&newCanvasNode, &effect]() {
        newCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest003 end";
}
/**
 * @tc.name: RSNodeAnimateTest004
 * @tc.desc: Verify the RSNodeAnimate of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, RSNodeAnimateTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest004 start";
    /**
     * @tc.steps: step1. init RSNodeAnimate
     */
    auto rootNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto animationCanvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto effect = RSTransitionEffect::Create()->Translate({500.f, 300.f, 0.f});
    animationCanvasNode->SetBoundsWidth(200.f);
    animationCanvasNode->SetBoundsHeight(300.f);
    animationCanvasNode->SetBackgroundColor(SK_ColorRED);
    animationCanvasNode->SetTransitionEffect(effect);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&animationCanvasNode, &rootNode]() {
        rootNode->AddChild(animationCanvasNode, -1);
    });
    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest004 end";
}
/**
 * @tc.name: RSNodeAnimateTest005
 * @tc.desc: Verify the RSNodeAnimate of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, RSNodeAnimateTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest005 start";
    /**
     * @tc.steps: step1. init RSNodeAnimate
     */
    auto rootNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto animationCanvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto effect = RSTransitionEffect::Create()->Opacity(0.5f);
    animationCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    animationCanvasNode->SetBackgroundColor(SK_ColorRED);
    animationCanvasNode->SetTransitionEffect(effect);
    rootNode->AddChild(animationCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&animationCanvasNode]() {
        animationCanvasNode->SetVisible(false);
    });
    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest005 end";
}
/**
 * @tc.name: RSNodeAnimateTest006
 * @tc.desc: Verify the RSNodeAnimate of RSTransitionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSTransitionTest, RSNodeAnimateTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest006 start";
    /**
     * @tc.steps: step1. init RSNodeAnimate
     */
    auto rootNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto animationCanvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto effect = RSTransitionEffect::Create()->Scale({0.1f, 0.4f, 0.5f});
    animationCanvasNode->SetFrame(ANIMATION_START_BOUNDS);
    animationCanvasNode->SetBackgroundColor(SK_ColorRED);
    rootNode->AddChild(animationCanvasNode, -1);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(rsUIContext, protocol, RSAnimationTimingCurve::EASE,
        [&animationCanvasNode, &effect]() {
        animationCanvasNode->NotifyTransition(effect, true);
    });
    /**
     * @tc.steps: step2. start RSNodeAnimate test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    GTEST_LOG_(INFO) << "RSTransitionTest RSNodeAnimateTest006 end";
}
} // namespace Rosen
} // namespace OHOS
