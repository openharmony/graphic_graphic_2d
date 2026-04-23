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

#include <memory>
#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"
#include "animation/rs_animation.h"
#include "animation/rs_interactive_implict_animator.h"
#include "animation/rs_animation_timing_curve.h"
#include "platform/common/rs_system_properties.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"
#include "ui/rs_node.h"
#include "common/rs_vector2.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSInteractiveImplictAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSInteractiveImplictAnimatorTest::SetUpTestCase() {}
void RSInteractiveImplictAnimatorTest::TearDownTestCase() {}
void RSInteractiveImplictAnimatorTest::SetUp() {}
void RSInteractiveImplictAnimatorTest::TearDown() {}

/**
 * @tc.name: CreateGroup001
 * @tc.desc: Test CreateGroup with null rsUIContext
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CreateGroup001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CreateGroup001 start";

    RSAnimationTimingCurve timingCurve;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Cover branch: rsUIContext == nullptr
    auto animator = RSInteractiveImplictAnimator::CreateGroup(nullptr, timingProtocol, timingCurve);

    // Should return empty weak_ptr
    EXPECT_TRUE(animator.expired());

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CreateGroup001 end";
}

/**
 * @tc.name: CreateGroup002
 * @tc.desc: Test CreateGroup with invalid timingProtocol
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CreateGroup002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CreateGroup002 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(-100); // Invalid duration

    // Cover branch: ValidateTimingProtocol returns false
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);

    // Should return empty weak_ptr
    EXPECT_TRUE(animator.expired());

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CreateGroup002 end";
}

/**
 * @tc.name: CreateGroup003
 * @tc.desc: Test CreateGroup with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CreateGroup003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CreateGroup003 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);

    // Should succeed
    ASSERT_TRUE(animator.lock());
    EXPECT_TRUE(animator.lock()->isGroupAnimator_);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CreateGroup003 end";
}

/**
 * @tc.name: ValidateTimingProtocol001
 * @tc.desc: Test ValidateTimingProtocol with invalid duration (<= 0)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol001 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(-100); // Invalid duration

    // Should return false when duration is invalid
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol001 end";
}

/**
 * @tc.name: ValidateTimingProtocol002
 * @tc.desc: Test ValidateTimingProtocol with negative startDelay
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol002 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(-50); // Negative startDelay

    // Should succeed and reset startDelay to 0
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_TRUE(result);
    EXPECT_EQ(timingProtocol.GetStartDelay(), 0);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol002 end";
}

/**
 * @tc.name: ValidateTimingProtocol003
 * @tc.desc: Test ValidateTimingProtocol with invalid speed (<= 0)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol003 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetSpeed(-1.0f); // Invalid speed

    // Should succeed and reset speed to 1.0f
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(timingProtocol.GetSpeed(), 1.0f);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol003 end";
}

/**
 * @tc.name: ValidateTimingProtocol004
 * @tc.desc: Test ValidateTimingProtocol with invalid speed (isinf)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol004 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetSpeed(std::numeric_limits<float>::infinity()); // Invalid speed

    // Should succeed and reset speed to 1.0f
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(timingProtocol.GetSpeed(), 1.0f);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol004 end";
}

/**
 * @tc.name: ValidateTimingProtocol005
 * @tc.desc: Test ValidateTimingProtocol with invalid speed (isnan)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol005 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetSpeed(std::numeric_limits<float>::quiet_NaN()); // Invalid speed

    // Should succeed and reset speed to 1.0f
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(timingProtocol.GetSpeed(), 1.0f);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol005 end";
}

/**
 * @tc.name: ValidateTimingProtocol006
 * @tc.desc: Test ValidateTimingProtocol with invalid repeatCount (< -1)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol006 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(-2); // Invalid repeatCount (< -1)

    // Should succeed and reset repeatCount to 1
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_TRUE(result);
    EXPECT_EQ(timingProtocol.GetRepeatCount(), 1);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol006 end";
}

/**
 * @tc.name: ValidateTimingProtocol007
 * @tc.desc: Test ValidateTimingProtocol with invalid repeatCount (== 0)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol007 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(0); // Invalid repeatCount (== 0)

    // Should succeed and reset repeatCount to 1
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_TRUE(result);
    EXPECT_EQ(timingProtocol.GetRepeatCount(), 1);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol007 end";
}

/**
 * @tc.name: ValidateTimingProtocol008
 * @tc.desc: Test ValidateTimingProtocol with all valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ValidateTimingProtocol008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol008 start";

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(100);
    timingProtocol.SetSpeed(1.5f);
    timingProtocol.SetRepeatCount(2);

    // Should succeed with all valid parameters
    auto result = RSInteractiveImplictAnimator::ValidateTimingProtocol(timingProtocol);
    EXPECT_TRUE(result);
    EXPECT_EQ(timingProtocol.GetDuration(), 1000);
    EXPECT_EQ(timingProtocol.GetStartDelay(), 100);
    EXPECT_FLOAT_EQ(timingProtocol.GetSpeed(), 1.5f);
    EXPECT_EQ(timingProtocol.GetRepeatCount(), 2);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ValidateTimingProtocol008 end";
}

/**
 * @tc.name: Destructor001
 * @tc.desc: Test destructor with isGroupAnimator_ = true (skip sending destroy command)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, Destructor001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest Destructor001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    // Create group animator (isGroupAnimator_ = true)
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());

    // Destroy group animator - should not send destroy command
    animator = std::weak_ptr<RSInteractiveImplictAnimator>();

    // Verify no crash
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest Destructor001 end";
}

/**
 * @tc.name: Destructor002
 * @tc.desc: Test destructor with isGroupAnimator_ = false (send destroy command)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, Destructor002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest Destructor002 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    // Create regular animator (isGroupAnimator_ = false)
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    EXPECT_TRUE(animator);

    // Destroy regular animator - should send destroy command
    animator.reset();

    // Verify no crash
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest Destructor002 end";
}

/**
 * @tc.name: CallFinishCallback001
 * @tc.desc: Test CallFinishCallback with isGroupAnimator_ = true
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CallFinishCallback001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());

    // Add animator to UI context
    rsUIContext->AddInteractiveImplictAnimator(animator.lock());

    // Call finish callback - should remove animator from UI context
    animator.lock()->CallFinishCallback();

    // Verify animator was removed
    // (Note: We can't directly verify this without accessing private members)

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback001 end";
}

/**
 * @tc.name: CallFinishCallback002
 * @tc.desc: Test CallFinishCallback with isGroupAnimator_ = false
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CallFinishCallback002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback002 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    EXPECT_TRUE(animator);

    // Call finish callback - should NOT remove from UI context (isGroupAnimator_ = false)
    animator->CallFinishCallback();

    // Verify no crash
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback002 end";
}

/**
 * @tc.name: CallFinishCallback003
 * @tc.desc: Test CallFinishCallback with finishCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CallFinishCallback003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback003 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    EXPECT_TRUE(animator);

    bool callbackCalled = false;
    animator->SetFinishCallBack([&callbackCalled]() {
        callbackCalled = true;
    });

    // Call finish callback - should invoke the callback
    animator->CallFinishCallback();

    EXPECT_TRUE(callbackCalled);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback003 end";
}

/**
 * @tc.name: CallFinishCallback004
 * @tc.desc: Test CallFinishCallback with empty rsUIContext_ (lock() returns nullptr)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CallFinishCallback004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback004 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto groupAnimator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(groupAnimator.lock());

    // Clear rsUIContext_ to make it an empty weak_ptr
    groupAnimator.lock()->rsUIContext_.reset();

    // CallFinishCallback - rsUIContext_.lock() will return nullptr
    // Should handle gracefully without crash
    groupAnimator.lock()->CallFinishCallback();

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback004 end";
}

/**
 * @tc.name: CallFinishCallback005
 * @tc.desc: Test CallFinishCallback with isGroupAnimator_ = true and valid callback
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CallFinishCallback005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback005 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto groupAnimator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(groupAnimator.lock());

    bool callbackCalled = false;
    groupAnimator.lock()->SetFinishCallBack([&callbackCalled]() {
        callbackCalled = true;
    });

    // Call finish callback - should execute callback and try to remove from context
    groupAnimator.lock()->CallFinishCallback();

    EXPECT_TRUE(callbackCalled);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest CallFinishCallback005 end";
}

/**
 * @tc.name: StartAnimation001
 * @tc.desc: Test StartAnimation with isGroupAnimator_ = true covers speed multiplier branch
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, StartAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetSpeed(2.0f);

    auto groupAnimator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(groupAnimator.lock());

    // Verify isGroupAnimator_ is true for group animator
    EXPECT_TRUE(groupAnimator.lock()->isGroupAnimator_);

    // Create node and add it to nodeMap
    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();

    // Create animation and directly add to animations_ (UT can access private members)
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetSpeed(1.0f);  // Child animation speed
    groupAnimator.lock()->animations_.emplace_back(animation, nodeId);

    // Directly set state to ACTIVE
    groupAnimator.lock()->state_ = RSInteractiveAnimationState::ACTIVE;

    // Now animations_ is not empty, state is ACTIVE
    EXPECT_FALSE(groupAnimator.lock()->animations_.empty());

    // StartAnimation will execute the for loop and isGroupAnimator_ branch (speed multiplication)
    auto result = groupAnimator.lock()->StartAnimation();
    GTEST_LOG_(INFO) << "StartAnimation result: " << result;

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation001 end";
}

/**
 * @tc.name: StartAnimation002
 * @tc.desc: Test StartAnimation with isGroupAnimator_ = false (no speed multiplication)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, StartAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation002 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetSpeed(2.0f);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    // Verify isGroupAnimator_ is false for regular animator
    EXPECT_FALSE(animator->isGroupAnimator_);

    // Create node and add it to nodeMap
    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();

    // Create animation and directly add to animations_ (UT can access private members)
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetSpeed(1.0f);
    animator->animations_.emplace_back(animation, nodeId);

    // Directly set state to ACTIVE
    animator->state_ = RSInteractiveAnimationState::ACTIVE;

    // Now animations_ is not empty, state is ACTIVE
    EXPECT_FALSE(animator->animations_.empty());

    // StartAnimation will execute the for loop without speed multiplication (isGroupAnimator_ = false)
    auto result = animator->StartAnimation();
    GTEST_LOG_(INFO) << "StartAnimation result: " << result;

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation002 end";
}

/**
 * @tc.name: StartAnimation003
 * @tc.desc: Test StartAnimation with invalid state
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, StartAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation003 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    EXPECT_TRUE(animator);

    // Try to start without adding animations - should fail
    auto result = animator->StartAnimation();
    EXPECT_TRUE(result > 0);

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest StartAnimation003 end";
}

/**
 * @tc.name: SendCreateAnimatorCommand001
 * @tc.desc: Test SendCreateAnimatorCommand with isGroupAnimator_ = true
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SendCreateAnimatorCommand001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand001 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto groupAnimator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(groupAnimator.lock());

    // Prepare test data - directly call private method with render animations
    std::vector<std::pair<NodeId, AnimationId>> renderAnimations;
    renderAnimations.emplace_back(1001, 2001);
    renderAnimations.emplace_back(1002, 2002);

    groupAnimator.lock()->SendCreateAnimatorCommand(renderAnimations);

    // Verify: should send RSInteractiveAnimatorCreateGroup command (no assertion needed, just ensure no crash)
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand001 end";
}

/**
 * @tc.name: SendCreateAnimatorCommand002
 * @tc.desc: Test SendCreateAnimatorCommand with isGroupAnimator_ = false
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SendCreateAnimatorCommand002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand002 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    EXPECT_TRUE(animator);

    // Prepare test data
    std::vector<std::pair<NodeId, AnimationId>> renderAnimations;
    renderAnimations.emplace_back(1003, 2003);

    animator->SendCreateAnimatorCommand(renderAnimations);

    // Verify: should send RSInteractiveAnimatorCreate command (no assertion needed, just ensure no crash)
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand002 end";
}

/**
 * @tc.name: SendCreateAnimatorCommand003
 * @tc.desc: Test SendCreateAnimatorCommand with isGroupAnimator_ = true and IsUniRenderEnabled = false
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SendCreateAnimatorCommand003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand003 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto groupAnimator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(groupAnimator.lock());

    // Directly modify private static member to control IsUniRenderEnabled() return value
    RSSystemProperties::isUniRenderEnabled_ = false;

    std::vector<std::pair<NodeId, AnimationId>> renderAnimations;
    renderAnimations.emplace_back(1004, 2004);

    // Should trigger: if (isGroupAnimator_) && if (!IsUniRenderEnabled())
    groupAnimator.lock()->SendCreateAnimatorCommand(renderAnimations);

    // Restore to default state
    RSSystemProperties::isUniRenderEnabled_ = true;

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand003 end";
}

/**
 * @tc.name: SendCreateAnimatorCommand004
 * @tc.desc: Test SendCreateAnimatorCommand with isGroupAnimator_ = false and IsUniRenderEnabled = false
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SendCreateAnimatorCommand004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand004 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    EXPECT_TRUE(animator);

    // Directly modify private static member to control IsUniRenderEnabled() return value
    RSSystemProperties::isUniRenderEnabled_ = false;

    std::vector<std::pair<NodeId, AnimationId>> renderAnimations;
    renderAnimations.emplace_back(1005, 2005);

    // Should trigger: else (not group) && if (!IsUniRenderEnabled())
    animator->SendCreateAnimatorCommand(renderAnimations);

    // Restore to default state
    RSSystemProperties::isUniRenderEnabled_ = true;

    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand004 end";
}

/**
 * @tc.name: SendCreateAnimatorCommand005
 * @tc.desc: Test SendCreateAnimatorCommand with empty animations
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SendCreateAnimatorCommand005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand005 start";

    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    EXPECT_TRUE(animator);

    // Prepare empty test data
    std::vector<std::pair<NodeId, AnimationId>> renderAnimations;  // Empty vector

    animator->SendCreateAnimatorCommand(renderAnimations);

    // Verify: should handle empty animations gracefully (no assertion needed, just ensure no crash)
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest SendCreateAnimatorCommand005 end";
}

} // namespace Rosen
} // namespace OHOS
