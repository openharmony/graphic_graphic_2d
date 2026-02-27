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
#include "animation/rs_render_animation.h"
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
    auto animation = std::make_shared<RSDummyAnimation>();
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
    auto animation = std::make_shared<RSDummyAnimation>();
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
    auto animation = std::make_shared<RSDummyAnimation>();
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
    auto animation1 = std::make_shared<RSDummyAnimation>();
    animation1->SetRepeatCount(1);

    AnimationId id2 = 2;
    auto animation2 = std::make_shared<RSDummyAnimation>();
    animation2->SetRepeatCount(-1);
    animation2->uiAnimation_ = std::make_shared<RSRenderAnimation>();

    AnimationId id3 = 3;
    auto animation3 = std::make_shared<RSDummyAnimation>();
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
    auto animation = std::make_shared<RSDummyAnimation>();
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
    auto animation = std::make_shared<RSDummyAnimation>();
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

    bool isRenderServiceNode = true;
    auto target = std::make_shared<RSNode>(isRenderServiceNode);
    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>();
    rsNode->FallbackAnimationsToRoot();
    EXPECT_NE(RSNodeMap::Instance().animationFallbackNode_, nullptr);

    animation = std::make_shared<RSDummyAnimation>();
    animation->SetRepeatCount(1);
    rsNode->animations_.insert({ id, animation });
    rsNode->FallbackAnimationsToRoot();
    // Animation should be moved to target node
    EXPECT_TRUE(rsNode->animations_.empty());

    animation = std::make_shared<RSDummyAnimation>();
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
    
    // Create multiple animations
    AnimationId id1 = 1;
    auto animation1 = std::make_shared<RSDummyAnimation>();
    animation1->SetRepeatCount(1);
    
    AnimationId id2 = 2;
    auto animation2 = std::make_shared<RSDummyAnimation>();
    animation2->SetRepeatCount(-1);
    animation2->uiAnimation_ = std::make_shared<RSRenderAnimation>();
    
    AnimationId id3 = 3;
    auto animation3 = std::make_shared<RSDummyAnimation>();
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

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>();
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

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>();
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
} // namespace OHOS::Rosen
