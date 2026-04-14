/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <sys/types.h>
#include <unistd.h>

#include "gtest/gtest.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_interactive_implict_animator.h"
#include "animation/rs_render_animation.h"
#include "common/rs_vector2.h"
#include "modifier/rs_property.h"
#include "pipeline/rs_node_map.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSNodeAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeAnimationTest::SetUpTestCase() {}
void RSNodeAnimationTest::TearDownTestCase() {}
void RSNodeAnimationTest::SetUp() {}
void RSNodeAnimationTest::TearDown() {}

/**
 * @tc.name: FallbackAnimationsToContext001
 * @tc.desc: test results of FallbackAnimationsToContext when rsUIContext is null
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToContext001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->rsUIContext_ = nullptr;

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsNode->rsUIContext_);
    rsNode->animations_.insert({ id, animation });

    // When rsUIContext is null, FallbackAnimationsToContext should return false
    bool result = rsNode->FallbackAnimationsToContext();
    EXPECT_FALSE(result);
    // Animations should still exist in the node
    EXPECT_FALSE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToContext002
 * @tc.desc: test results of FallbackAnimationsToContext with valid rsUIContext
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToContext002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto rsUIContext = std::make_shared<RSUIContext>(0);
    rsNode->rsUIContext_ = rsUIContext;

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(1);
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToContext should move animations to context
    bool result = rsNode->FallbackAnimationsToContext();
    EXPECT_TRUE(result);
    // Animations should be moved to context
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToContext003
 * @tc.desc: test results of FallbackAnimationsToContext with infinite repeat UI animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToContext003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto rsUIContext = std::make_shared<RSUIContext>(0);
    rsNode->rsUIContext_ = rsUIContext;

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(-1);
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToContext should process the animation
    // Note: Due to std::move in loop, all animations are cleared after the loop
    bool result = rsNode->FallbackAnimationsToContext();
    EXPECT_TRUE(result);
    // Animations container should be cleared after FallbackAnimationsToContext
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToContext004
 * @tc.desc: test results of FallbackAnimationsToContext with mixed animations
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToContext004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto rsUIContext = std::make_shared<RSUIContext>(0);
    rsNode->rsUIContext_ = rsUIContext;

    // Create multiple animations
    AnimationId id1 = 1;
    auto animation1 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation1->SetRepeatCount(1);

    AnimationId id2 = 2;
    auto animation2 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation2->SetRepeatCount(-1);
    animation2->uiAnimation_ = std::make_shared<RSRenderAnimation>();

    AnimationId id3 = 3;
    auto animation3 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation3->SetRepeatCount(2);

    rsNode->animations_.insert({ id1, animation1 });
    rsNode->animations_.insert({ id2, animation2 });
    rsNode->animations_.insert({ id3, animation3 });

    // FallbackAnimationsToContext should move non-infinite UI animations
    // Note: Due to std::move in loop and clear() at end, all animations are cleared
    bool result = rsNode->FallbackAnimationsToContext();
    EXPECT_TRUE(result);
    // Animations container should be cleared after FallbackAnimationsToContext
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToContext005
 * @tc.desc: test results of FallbackAnimationsToContext with infinite repeat non-UI animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToContext005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto rsUIContext = std::make_shared<RSUIContext>(0);
    rsNode->rsUIContext_ = rsUIContext;

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(-1);
    // No uiAnimation_ set, so IsUiAnimation() returns false
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToContext should move animation to context
    // Condition: animation (true) && GetRepeatCount() == -1 (true) && IsUiAnimation() (false)
    // Result: NOT skipped, should be moved
    bool result = rsNode->FallbackAnimationsToContext();
    EXPECT_TRUE(result);
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToContext006
 * @tc.desc: test results of FallbackAnimationsToContext with finite repeat UI animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToContext006, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto rsUIContext = std::make_shared<RSUIContext>(0);
    rsNode->rsUIContext_ = rsUIContext;

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(1);
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToContext should move animation to context
    // Condition: animation (true) && GetRepeatCount() == -1 (false) && IsUiAnimation() (true)
    // Result: NOT skipped, should be moved
    bool result = rsNode->FallbackAnimationsToContext();
    EXPECT_TRUE(result);
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToContext007
 * @tc.desc: test results of FallbackAnimationsToContext with null animation pointer
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToContext007, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto rsUIContext = std::make_shared<RSUIContext>(0);
    rsNode->rsUIContext_ = rsUIContext;

    AnimationId id = 1;
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToContext should skip null animation
    // Condition: animation (false) && ...
    // Result: Skipped, not added to context
    bool result = rsNode->FallbackAnimationsToContext();
    EXPECT_TRUE(result);
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToRoot
 * @tc.desc: test results of FallbackAnimationsToRoot
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToRoot, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->FallbackAnimationsToRoot();
    EXPECT_EQ(rsNode->motionPathOption_, nullptr);

    auto rsUIContext = std::make_shared<RSUIContext>(0);
    bool isRenderServiceNode = true;
    auto target = std::make_shared<RSNode>(isRenderServiceNode);
    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    rsNode->FallbackAnimationsToRoot();
    EXPECT_NE(RSNodeMap::Instance().animationFallbackNode_, nullptr);

    animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(1);
    rsNode->animations_.insert({ id, animation });
    rsNode->FallbackAnimationsToRoot();
    // Animation should be moved to target node
    EXPECT_TRUE(rsNode->animations_.empty());

    animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(-1);
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    rsNode->animations_.insert({ id, animation });
    rsNode->FallbackAnimationsToRoot();
    // Infinite UI animation should be cleared (destroyed)
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToRoot002
 * @tc.desc: test results of FallbackAnimationsToRoot with mixed animations
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToRoot002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool isRenderServiceNode = true;
    auto target = std::make_shared<RSNode>(isRenderServiceNode);
    auto rsUIContext = std::make_shared<RSUIContext>(0);

    // Create multiple animations
    AnimationId id1 = 1;
    auto animation1 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation1->SetRepeatCount(1);

    AnimationId id2 = 2;
    auto animation2 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation2->SetRepeatCount(-1);
    animation2->uiAnimation_ = std::make_shared<RSRenderAnimation>();

    AnimationId id3 = 3;
    auto animation3 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation3->SetRepeatCount(2);

    rsNode->animations_.insert({ id1, animation1 });
    rsNode->animations_.insert({ id2, animation2 });
    rsNode->animations_.insert({ id3, animation3 });

    // FallbackAnimationsToRoot should move non-infinite UI animations to target
    // Note: Due to std::move in loop and clear() at end, all animations are cleared
    rsNode->FallbackAnimationsToRoot();
    // Animations container should be cleared after FallbackAnimationsToRoot
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToRoot003
 * @tc.desc: test results of FallbackAnimationsToRoot with infinite repeat non-UI animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToRoot003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool isRenderServiceNode = true;
    auto target = std::make_shared<RSNode>(isRenderServiceNode);
    auto rsUIContext = std::make_shared<RSUIContext>(0);

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(-1);
    // No uiAnimation_ set, so IsUiAnimation() returns false
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToRoot should move animation to target
    // Condition: animation (true) && GetRepeatCount() == -1 (true) && IsUiAnimation() (false)
    // Result: NOT skipped, should be moved
    rsNode->FallbackAnimationsToRoot();
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToRoot004
 * @tc.desc: test results of FallbackAnimationsToRoot with finite repeat UI animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToRoot004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool isRenderServiceNode = true;
    auto target = std::make_shared<RSNode>(isRenderServiceNode);
    auto rsUIContext = std::make_shared<RSUIContext>(0);

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(1);
    animation->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToRoot should move animation to target
    // Condition: animation (true) && GetRepeatCount() == -1 (false) && IsUiAnimation() (true)
    // Result: NOT skipped, should be moved
    rsNode->FallbackAnimationsToRoot();
    EXPECT_TRUE(rsNode->animations_.empty());
}

/**
 * @tc.name: FallbackAnimationsToRoot005
 * @tc.desc: test results of FallbackAnimationsToRoot with null animation pointer
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, FallbackAnimationsToRoot005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool isRenderServiceNode = true;
    auto target = std::make_shared<RSNode>(isRenderServiceNode);

    AnimationId id = 1;
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsNode->animations_.insert({ id, animation });

    // FallbackAnimationsToRoot should skip null animation
    // Condition: animation (false) && ...
    // Result: Skipped, not added to target
    rsNode->FallbackAnimationsToRoot();
    EXPECT_TRUE(rsNode->animations_.empty());
}
/**
 * @tc.name: OpenImplicitAnimationWithGroupAnimator001
 * @tc.desc: Test OpenImplicitAnimation with GROUP animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, OpenImplicitAnimationWithGroupAnimator001, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    RSNode::OpenImplicitAnimation(rsUIContext, timingProtocol, timingCurve, finishCallback);

    const auto& params = implicitAnimator->globalImplicitParams_;
    if (!params.empty()) {
        const auto& [protocol, curve, callback, repeatCallback] = params.top();
        EXPECT_EQ(callback, nullptr);
    }

    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: OpenImplicitAnimationWithNoneAnimator001
 * @tc.desc: Test OpenImplicitAnimation with NONE animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, OpenImplicitAnimationWithNoneAnimator001, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    RSNode::OpenImplicitAnimation(rsUIContext, timingProtocol, timingCurve, finishCallback);

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    EXPECT_EQ(implicitAnimator->GetInteractiveAnimatorType(), InteractiveAnimatorType::NONE);

    const auto& params = implicitAnimator->globalImplicitParams_;
    ASSERT_FALSE(params.empty());
    const auto& [protocol, curve, callback, repeatCallback] = params.top();
    EXPECT_NE(callback, nullptr);

    implicitAnimator->CloseImplicitAnimation();
}

/**
 * @tc.name: OpenImplicitAnimationWithGroupAnimator002
 * @tc.desc: Test OpenImplicitAnimation with GROUP animator type and null finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, OpenImplicitAnimationWithGroupAnimator002, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    std::function<void()> nullCallback = nullptr;
    RSNode::OpenImplicitAnimation(rsUIContext, timingProtocol, timingCurve, nullCallback);

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: AnimateWithGroupAnimator001
 * @tc.desc: Test Animate with GROUP animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithGroupAnimator001, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::Animate(rsUIContext, timingProtocol, timingCurve,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback);

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: AnimateWithGroupAnimator002
 * @tc.desc: Test Animate with GROUP animator type and null finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithGroupAnimator002, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE;

    std::function<void()> nullCallback = nullptr;

    auto animations = RSNode::Animate(rsUIContext, timingProtocol, timingCurve,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, nullCallback);

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: AnimateWithCurrentOptionsGroupAnimator001
 * @tc.desc: Test AnimateWithCurrentOptions with GROUP animator type
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithCurrentOptionsGroupAnimator001, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, nullptr);

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::AnimateWithCurrentOptions(rsUIContext,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback, true);

    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: AnimateWithCurrentOptionsWithoutGroup001
 * @tc.desc: Test AnimateWithCurrentOptions without GROUP animator (NONE type)
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithCurrentOptionsWithoutGroup001, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, nullptr);

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::AnimateWithCurrentOptions(rsUIContext,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback, true);

    implicitAnimator->CloseImplicitAnimation();
}

/**
 * @tc.name: AnimateWithCurrentCallbackGroupAnimator001
 * @tc.desc: Test AnimateWithCurrentCallback with GROUP animator type
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithCurrentCallbackGroupAnimator001, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    auto implicitAnimator = rsUIContext->GetRSImplicitAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::EASE;

    auto animations = RSNode::AnimateWithCurrentCallback(rsUIContext, timingProtocol, timingCurve,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); });

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: OpenImplicitAnimationWithoutContextGroup001
 * @tc.desc: Test OpenImplicitAnimation without rsUIContext param with GROUP animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, OpenImplicitAnimationWithoutContextGroup001, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    RSNode::OpenImplicitAnimation(timingProtocol, timingCurve, finishCallback);

    const auto& params = implicitAnimator->globalImplicitParams_;
    ASSERT_FALSE(params.empty());
    const auto& [protocol, curve, callback, repeatCallback] = params.top();
    EXPECT_EQ(callback, nullptr);

    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: OpenImplicitAnimationWithoutContextGroup002
 * @tc.desc: Test OpenImplicitAnimation without rsUIContext param with GROUP animator type and null callback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, OpenImplicitAnimationWithoutContextGroup002, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    std::function<void()> nullCallback = nullptr;
    RSNode::OpenImplicitAnimation(timingProtocol, timingCurve, nullCallback);

    const auto& params = implicitAnimator->globalImplicitParams_;
    ASSERT_FALSE(params.empty());
    const auto& [protocol, curve, callback, repeatCallback] = params.top();
    EXPECT_EQ(callback, nullptr);

    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: OpenImplicitAnimationWithoutContextNone001
 * @tc.desc: Test OpenImplicitAnimation without rsUIContext param with NONE animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, OpenImplicitAnimationWithoutContextNone001, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    RSNode::OpenImplicitAnimation(timingProtocol, timingCurve, finishCallback);

    const auto& params = implicitAnimator->globalImplicitParams_;
    ASSERT_FALSE(params.empty());
    const auto& [protocol, curve, callback, repeatCallback] = params.top();
    EXPECT_NE(callback, nullptr);

    implicitAnimator->CloseImplicitAnimation();
}

/**
 * @tc.name: AnimateWithoutContextGroup001
 * @tc.desc: Test Animate without rsUIContext param with GROUP animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithoutContextGroup001, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    auto node = RSCanvasNode::Create();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::Animate(timingProtocol, timingCurve,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback);

    const auto& params = implicitAnimator->globalImplicitParams_;
    if (!params.empty()) {
        const auto& [protocol, curve, callback, repeatCallback] = params.top();
        EXPECT_EQ(callback, nullptr);
    }

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: AnimateWithoutContextGroup002
 * @tc.desc: Test Animate without rsUIContext param with GROUP animator type and null finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithoutContextGroup002, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    auto node = RSCanvasNode::Create();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    std::function<void()> nullCallback = nullptr;

    auto animations = RSNode::Animate(timingProtocol, timingCurve,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, nullCallback);

    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: AnimateWithoutContextNone001
 * @tc.desc: Test Animate without rsUIContext param with NONE animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithoutContextNone001, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;

    auto node = RSCanvasNode::Create();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::Animate(timingProtocol, timingCurve,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback);
}

/**
 * @tc.name: AnimateWithNoneAnimator001
 * @tc.desc: Test Animate with rsUIContext param with NONE animator type and finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithNoneAnimator001, TestSize.Level1)
{
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto node = RSCanvasNode::Create(false, false, rsUIContext);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;

    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::Animate(rsUIContext, timingProtocol, timingCurve,
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback);
}

/**
 * @tc.name: AnimateWithCurrentOptionsWithoutContextGroup001
 * @tc.desc: Test AnimateWithCurrentOptions without rsUIContext param with GROUP animator type
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithCurrentOptionsWithoutContextGroup001, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::GROUP;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, nullptr);

    auto node = RSCanvasNode::Create();
    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::AnimateWithCurrentOptions(
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback, true);

    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;
}

/**
 * @tc.name: AnimateWithCurrentOptionsWithoutContextNone001
 * @tc.desc: Test AnimateWithCurrentOptions without rsUIContext param with NONE animator type
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimationTest, AnimateWithCurrentOptionsWithoutContextNone001, TestSize.Level1)
{
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator();
    ASSERT_NE(implicitAnimator, nullptr);
    implicitAnimator->interactiveAnimatorType_ = InteractiveAnimatorType::NONE;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::LINEAR;
    implicitAnimator->OpenImplicitAnimation(timingProtocol, timingCurve, nullptr);

    auto node = RSCanvasNode::Create();
    bool callbackCalled = false;
    auto finishCallback = [&callbackCalled]() { callbackCalled = true; };

    auto animations = RSNode::AnimateWithCurrentOptions(
        [&node]() { node->SetTranslate(Vector2f(100.0f, 0.0f)); }, finishCallback, true);

    implicitAnimator->CloseImplicitAnimation();
}
} // namespace OHOS::Rosen
