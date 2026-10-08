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
#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "animation/rs_interactive_implict_animator.h"
#include "animation/rs_animation_timing_curve.h"
#include "platform/common/rs_system_properties.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_node.h"
#include "common/rs_vector2.h"
#include "command/rs_node_showing_command.h"
#include "animation/rs_render_animation.h"
#include "modifier/rs_property.h"
#include "modifier/rs_render_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
// RSRenderAnimation is abstract (RebuildPropertyValue is pure virtual); this mock makes it
// constructable in UT to populate uiAnimation_ and exercise the IsUiAnimation path.
class RSRenderAnimationMock : public RSRenderAnimation {
public:
    RSRenderAnimationMock() : RSRenderAnimation() {}
    explicit RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationMock() override = default;
    void RebuildPropertyValue(float fraction) override {}
};

class RSInteractiveImplictAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<RSUIContext> CreateRSUIContext();
};

void RSInteractiveImplictAnimatorTest::SetUpTestCase() {}
void RSInteractiveImplictAnimatorTest::TearDownTestCase() {}
void RSInteractiveImplictAnimatorTest::SetUp() {}
void RSInteractiveImplictAnimatorTest::TearDown() {}

std::shared_ptr<RSUIContext> RSInteractiveImplictAnimatorTest::CreateRSUIContext()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    return rsUIContext;
}

/**
 * @tc.name: CreateNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CreateNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator->rsUIContext_.lock() == nullptr);
}

/**
 * @tc.name: AddImplictAnimationNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddImplictAnimationNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    std::function<void()> callback = [] (){};
    auto size = animator->AddImplictAnimation(callback);
    EXPECT_TRUE(size == 0);
}

/**
 * @tc.name: AddAnimationNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddAnimationNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    std::function<void()> callback = [] (){};
    auto size = animator->AddAnimation(callback);
    EXPECT_TRUE(size == 0);
}

/**
 * @tc.name: StartAnimationNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, StartAnimationNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    std::function<void()> callback = [] (){};
    auto res = animator->StartAnimation();
    EXPECT_TRUE(static_cast<int>(res) == 1);
}

/**
 * @tc.name: PauseAnimationNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, PauseAnimationNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    animator->PauseAnimation();
}

/**
 * @tc.name: ContinueAnimationNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ContinueAnimationNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    animator->ContinueAnimation();
}

/**
 * @tc.name: FinishAnimationNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishAnimationNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    animator->FinishAnimation(RSInteractiveAnimationPosition::START);
}

/**
 * @tc.name: ReverseAnimationNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ReverseAnimationNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    animator->ReverseAnimation();
}

/**
 * @tc.name: SetFractionNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SetFractionNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    animator->SetFraction(1.1f);
}

/**
 * @tc.name: GetFractionNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetFractionNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    auto fraction = animator->GetFraction();
    EXPECT_NEAR(fraction, 0.0f, 0.000001);
}

/**
 * @tc.name: GetStatusNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetStatusNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    auto state = animator->GetStatus();
    EXPECT_TRUE(state == RSInteractiveAnimationState::INACTIVE);
}

/**
 * @tc.name: SetFinishCallBackNullContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, SetFinishCallBackNullContextTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    EXPECT_TRUE(animator != nullptr);
    std::function<void()> callback = [] (){};
    animator->SetFinishCallBack(callback);
    EXPECT_TRUE(animator->finishCallback_ != nullptr);
}

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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

    auto rsUIContext = CreateRSUIContext();
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

/**
 * @tc.name: AddImplictAnimation001
 * @tc.desc: Test AddImplictAnimation with null callback returns 0
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddImplictAnimation001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    auto result = animator->AddImplictAnimation(nullptr);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: AddAnimation001
 * @tc.desc: Test AddAnimation with null callback returns 0
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddAnimation001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    auto result = animator->AddAnimation(nullptr);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: AddImplictAnimation002
 * @tc.desc: Test AddImplictAnimation with null rsUIContext returns 0
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddImplictAnimation002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    animator->rsUIContext_.reset();
    auto result = animator->AddImplictAnimation([]() {});
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: AddImplictAnimation003
 * @tc.desc: Test AddImplictAnimation with duration <= 0 returns 0
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddImplictAnimation003, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(-100);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    auto result = animator->AddImplictAnimation([]() {});
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: AddAnimation002
 * @tc.desc: Test AddAnimation with null rsUIContext returns 0
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddAnimation002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    animator->rsUIContext_.reset();
    auto result = animator->AddAnimation([]() {});
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: AddAnimation003
 * @tc.desc: Test AddAnimation with invalid state returns 0
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddAnimation003, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    animator->state_ = RSInteractiveAnimationState::RUNNING;
    auto result = animator->AddAnimation([]() {});
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: AddImplictAnimation004
 * @tc.desc: Test AddImplictAnimation with valid callback does not early return at callback check
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddImplictAnimation004, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    bool callbackCalled = false;
    animator->AddImplictAnimation([&callbackCalled]() { callbackCalled = true; });
    EXPECT_TRUE(callbackCalled);
}

/**
 * @tc.name: AddAnimation004
 * @tc.desc: Test AddAnimation with valid callback does not early return at callback check
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, AddAnimation004, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);

    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);

    bool callbackCalled = false;
    animator->AddAnimation([&callbackCalled]() { callbackCalled = true; });
    EXPECT_TRUE(callbackCalled);
}
/**
 * @tc.name: GetClientFinishPositionEndGroupAutoReverseEven001
 * @tc.desc: Verify GetClientFinishPosition returns START for END+group+autoReverse+even count
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetClientFinishPositionEndGroupAutoReverseEven001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest GetClientFinishPositionEndGroupAutoReverseEven001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(2); // even
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());
    auto result = animator.lock()->GetClientFinishPosition(RSInteractiveAnimationPosition::END);
    EXPECT_EQ(result, RSInteractiveAnimationPosition::START);
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest GetClientFinishPositionEndGroupAutoReverseEven001 end";
}

/**
 * @tc.name: GetClientFinishPositionEndGroupAutoReverseOdd001
 * @tc.desc: Verify GetClientFinishPosition returns END for END+group+autoReverse+odd count
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetClientFinishPositionEndGroupAutoReverseOdd001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest GetClientFinishPositionEndGroupAutoReverseOdd001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(3); // odd
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());
    auto result = animator.lock()->GetClientFinishPosition(RSInteractiveAnimationPosition::END);
    EXPECT_EQ(result, RSInteractiveAnimationPosition::END);
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest GetClientFinishPositionEndGroupAutoReverseOdd001 end";
}

/**
 * @tc.name: GetClientFinishPositionEndGroupNoAutoReverse001
 * @tc.desc: Verify GetClientFinishPosition returns END for END+group+no autoReverse
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetClientFinishPositionEndGroupNoAutoReverse001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetAutoReverse(false); // no autoReverse
    timingProtocol.SetRepeatCount(2);
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());
    auto result = animator.lock()->GetClientFinishPosition(RSInteractiveAnimationPosition::END);
    EXPECT_EQ(result, RSInteractiveAnimationPosition::END);
}

/**
 * @tc.name: GetClientFinishPositionEndNonGroup001
 * @tc.desc: Verify GetClientFinishPosition returns END for END+non-group animator
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetClientFinishPositionEndNonGroup001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(2);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator);
    EXPECT_FALSE(animator->isGroupAnimator_);
    auto result = animator->GetClientFinishPosition(RSInteractiveAnimationPosition::END);
    EXPECT_EQ(result, RSInteractiveAnimationPosition::END);
}

/**
 * @tc.name: GetClientFinishPositionStart001
 * @tc.desc: Verify GetClientFinishPosition returns START for START position
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetClientFinishPositionStart001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(2);
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());
    // START position -> always returns START regardless of group settings
    auto result = animator.lock()->GetClientFinishPosition(RSInteractiveAnimationPosition::START);
    EXPECT_EQ(result, RSInteractiveAnimationPosition::START);
}

/**
 * @tc.name: GetClientFinishPositionEndGroupZeroRepeatCount001
 * @tc.desc: Verify GetClientFinishPosition with zero repeatCount (0 % 2 == 0)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetClientFinishPositionEndGroupZeroRepeatCount001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(2); // valid value to pass ValidateTimingProtocol
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());
    // Bypass validation: directly set repeatCount to 0 to test the 0 % 2 == 0 branch
    animator.lock()->timingProtocol_.SetRepeatCount(0);
    auto result = animator.lock()->GetClientFinishPosition(RSInteractiveAnimationPosition::END);
    EXPECT_EQ(result, RSInteractiveAnimationPosition::START);
}

/**
 * @tc.name: GetGroupAnimationNodeIds001
 * @tc.desc: Verify GetGroupAnimationNodeIds returns empty when no animators exist
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetGroupAnimationNodeIds001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto nodeIds = rsUIContext->GetGroupAnimationNodeIds();
    EXPECT_TRUE(nodeIds.empty());
}

/**
 * @tc.name: GetGroupAnimationNodeIds002
 * @tc.desc: Verify GetGroupAnimationNodeIds skips finite-loop group animators
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetGroupAnimationNodeIds002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(2); // finite
    RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    // Finite repeat -> should be skipped
    auto nodeIds = rsUIContext->GetGroupAnimationNodeIds();
    EXPECT_TRUE(nodeIds.empty());
}

/**
 * @tc.name: GetGroupAnimationNodeIds003
 * @tc.desc: Verify GetGroupAnimationNodeIds collects node IDs from infinite group animators
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, GetGroupAnimationNodeIds003, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(-1); // infinite
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto animator = RSInteractiveImplictAnimator::CreateGroup(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator.lock());
    // Directly populate animations_ with a nodeId entry (private member access)
    animator.lock()->animations_.emplace_back(std::weak_ptr<RSAnimation>(), canvasNode->GetId());
    auto nodeIds = rsUIContext->GetGroupAnimationNodeIds();
    EXPECT_FALSE(nodeIds.empty());
    EXPECT_GT(nodeIds.count(canvasNode->GetId()), static_cast<size_t>(0));
}

/**
 * @tc.name: FinishOnCurrentNullContext001
 * @tc.desc: Verify FinishOnCurrent returns early when rsUIContext is null
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishOnCurrentNullContext001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNullContext001 start";
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(nullptr, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);
    animator->state_ = RSInteractiveAnimationState::RUNNING;
    animator->FinishOnCurrent();
    // rsUIContext is null -> early return, state and animations_ untouched
    EXPECT_EQ(animator->state_, RSInteractiveAnimationState::RUNNING);
    EXPECT_TRUE(animator->animations_.empty());
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNullContext001 end";
}

/**
 * @tc.name: FinishOnCurrentNullAnimationOrNode001
 * @tc.desc: Verify FinishOnCurrent skips entries whose animation or node is null
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishOnCurrentNullAnimationOrNode001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNullAnimationOrNode001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);
    animator->state_ = RSInteractiveAnimationState::RUNNING;

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();

    // valid animation but nodeId absent from nodeMap -> node is null (A1)
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animator->animations_.emplace_back(animation, 999999);
    // expired weak_ptr with a valid nodeId -> node non-null but animation null (A2)
    std::weak_ptr<RSAnimation> expired;
    animator->animations_.emplace_back(expired, nodeId);

    animator->FinishOnCurrent();
    // both entries skipped -> propertiesMap empty -> early return, animations_ untouched
    EXPECT_EQ(animator->animations_.size(), 2u);
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNullAnimationOrNode001 end";
}

/**
 * @tc.name: FinishOnCurrentNoPropertyAnimation001
 * @tc.desc: Verify FinishOnCurrent skips animations whose property has no running
 *           property animation on the node, leaving propertiesMap empty
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishOnCurrentNoPropertyAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNoPropertyAnimation001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);
    animator->state_ = RSInteractiveAnimationState::RUNNING;

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animator->animations_.emplace_back(animation, nodeId);

    // RSDummyAnimation.GetPropertyId() returns 0; node has no property-0 animation
    EXPECT_FALSE(node->HasPropertyAnimation(animation->GetPropertyId()));

    animator->FinishOnCurrent();
    // skipped (no property animation) -> propertiesMap empty -> early return
    EXPECT_EQ(animator->animations_.size(), 1u);
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNoPropertyAnimation001 end";
}

/**
 * @tc.name: FinishOnCurrentNonEmptyMap001
 * @tc.desc: Verify FinishOnCurrent proceeds past the empty-map check (propertiesMap
 *           non-empty) to create and execute the sync task when animations are collected,
 *           covering the task-failure early-return path (sync task is a no-op without a
 *           render service connection)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, FinishOnCurrentNonEmptyMap001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNonEmptyMap001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);
    animator->state_ = RSInteractiveAnimationState::RUNNING;

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();
    // mark property 0 as animating so CollectCancelableAnimations does not skip -> non-empty
    // propertiesMap -> FinishOnCurrent proceeds past the empty-map check to the sync task
    node->animatingPropertyNum_[0] = 1;

    auto animation1 = std::make_shared<RSDummyAnimation>(rsUIContext);
    auto animation2 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animator->animations_.emplace_back(animation1, nodeId);
    animator->animations_.emplace_back(animation2, nodeId);

    // precondition: property owned -> entries collected -> propertiesMap non-empty (427 false)
    EXPECT_TRUE(node->HasPropertyAnimation(animation1->GetPropertyId()));

    animator->FinishOnCurrent();
    // sync task is a no-op without a render service connection -> !IsSuccess() -> early
    // return; state and animations_ must stay intact
    EXPECT_EQ(animator->state_, RSInteractiveAnimationState::RUNNING);
    EXPECT_EQ(animator->animations_.size(), 2u);
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest FinishOnCurrentNonEmptyMap001 end";
}

/**
 * @tc.name: CollectCancelableAnimationsMultipleSameProperty001
 * @tc.desc: Verify CollectCancelableAnimations collects every animationId when several
 *           animations share the same (nodeId, propertyId): std::map::emplace is a no-op
 *           for an existing key, so the second id must be appended, not dropped
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CollectCancelableAnimationsMultipleSameProperty001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest"
        << " CollectCancelableAnimationsMultipleSameProperty001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();
    // RSDummyAnimation.GetPropertyId() returns 0; mark property 0 as animating so the
    // entries are not skipped and reach the emplace/append collection.
    node->animatingPropertyNum_[0] = 1;

    auto animation1 = std::make_shared<RSDummyAnimation>(rsUIContext);
    auto animation2 = std::make_shared<RSDummyAnimation>(rsUIContext);
    animator->animations_.emplace_back(animation1, nodeId);
    animator->animations_.emplace_back(animation2, nodeId);

    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap;
    animator->CollectCancelableAnimations(rsUIContext, propertiesMap);

    // both animations share (nodeId, propertyId 0): one map entry, two animation ids
    ASSERT_EQ(propertiesMap.size(), 1u);
    const auto& [key, value] = *propertiesMap.begin();
    EXPECT_EQ(key.first, nodeId);
    EXPECT_EQ(key.second, animation1->GetPropertyId());
    EXPECT_EQ(value.second.size(), 2u);
    EXPECT_EQ(value.second[0], animation1->GetId());
    EXPECT_EQ(value.second[1], animation2->GetId());
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest"
        << " CollectCancelableAnimationsMultipleSameProperty001 end";
}

/**
 * @tc.name: CollectCancelableAnimationsSkipsUiAnimation001
 * @tc.desc: Verify CollectCancelableAnimations skips UI animations (IsUiAnimation true)
 *           even when the node owns the property, so they are not added to propertiesMap
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, CollectCancelableAnimationsSkipsUiAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest"
        << " CollectCancelableAnimationsSkipsUiAnimation001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();
    // mark property 0 as animating so !HasPropertyAnimation is false; the skip must come
    // from IsUiAnimation being true (not from a missing property animation)
    node->animatingPropertyNum_[0] = 1;

    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->uiAnimation_ = std::make_shared<RSRenderAnimationMock>();
    animator->animations_.emplace_back(animation, nodeId);

    // precondition: property owned (B1 false) but animation is UI (B2 true)
    EXPECT_TRUE(node->HasPropertyAnimation(animation->GetPropertyId()));
    EXPECT_TRUE(animation->IsUiAnimation());

    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap propertiesMap;
    animator->CollectCancelableAnimations(rsUIContext, propertiesMap);
    // UI animation is skipped -> nothing collected
    EXPECT_TRUE(propertiesMap.empty());
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest"
        << " CollectCancelableAnimationsSkipsUiAnimation001 end";
}

/**
 * @tc.name: ApplyShowingPropertyValuesSkipsMissingNodeAndProperty001
 * @tc.desc: Verify ApplyShowingPropertyValues skips entries whose node is absent from the
 *           context (node null) and entries whose propertyId is unknown to the node
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ApplyShowingPropertyValuesSkipsMissingNodeAndProperty001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest"
        << " ApplyShowingPropertyValuesSkipsMissingNodeAndProperty001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();
    PropertyId unknownPropId = 9999;
    NodeId absentNodeId = 8888;

    // precondition: absent node not in context, and the node lacks unknownPropId
    EXPECT_TRUE(rsUIContext->GetNodeMap().GetNode<RSNode>(absentNodeId) == nullptr);
    EXPECT_FALSE(node->HasPropertyAnimation(unknownPropId));

    using MapValue = std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>;
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap map;
    map.emplace(std::make_pair(absentNodeId, unknownPropId), MapValue(nullptr, {}));
    map.emplace(std::make_pair(nodeId, unknownPropId), MapValue(nullptr, {}));
    auto task = std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(1e8, std::move(map));

    // rsUIContext valid (D-true); both entries skipped (E node null, F2 property null)
    animator->ApplyShowingPropertyValues(rsUIContext, *task);
    // skipped entries must not mutate node state
    EXPECT_FALSE(node->HasPropertyAnimation(unknownPropId));
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest"
        << " ApplyShowingPropertyValuesSkipsMissingNodeAndProperty001 end";
}

/**
 * @tc.name: ApplyShowingPropertyValuesNullContext001
 * @tc.desc: Verify ApplyShowingPropertyValues falls back to RSNodeMap::Instance() and skips
 *           safely when rsUIContext is null (no node found in the global map)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ApplyShowingPropertyValuesNullContext001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ApplyShowingPropertyValuesNullContext001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);

    NodeId absentNodeId = 8888;
    using MapValue = std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>;
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap map;
    map.emplace(std::make_pair(absentNodeId, PropertyId(0)), MapValue(nullptr, {}));
    auto task = std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(1e8, std::move(map));

    // null rsUIContext -> RSNodeMap::Instance() fallback (D-false); node absent -> skipped
    animator->ApplyShowingPropertyValues(nullptr, *task);
    EXPECT_EQ(animator->state_, RSInteractiveAnimationState::INACTIVE);
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ApplyShowingPropertyValuesNullContext001 end";
}

/**
 * @tc.name: ApplyShowingPropertyValuesAppliesValue001
 * @tc.desc: Verify ApplyShowingPropertyValues applies a non-null showing value via
 *           SetValueFromRender (G-true) and skips a null value (G-false) when the node
 *           owns the property (F-found)
 * @tc.type: FUNC
 */
HWTEST_F(RSInteractiveImplictAnimatorTest, ApplyShowingPropertyValuesAppliesValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ApplyShowingPropertyValuesAppliesValue001 start";
    auto rsUIContext = CreateRSUIContext();
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetDuration(1000);
    auto animator = RSInteractiveImplictAnimator::Create(rsUIContext, timingProtocol, timingCurve);
    ASSERT_TRUE(animator != nullptr);

    auto node = RSCanvasNode::Create(false, false, rsUIContext);
    NodeId nodeId = node->GetId();
    PropertyId propId = 100;
    // register a concrete property so GetPropertyById finds it (F-found)
    // RSAnimatableProperty (not RSProperty) overrides SetValueFromRender; RSProperty uses
    // the base no-op, so the animatable subclass is required to observe the applied value.
    auto clientProp = std::make_shared<RSAnimatableProperty<float>>();
    node->properties_[propId] = clientProp;
    ASSERT_TRUE(node->GetPropertyById(propId) != nullptr);

    using MapValue = std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>;

    // G-true: non-null showing value -> SetValueFromRender applies it
    auto renderProp = std::make_shared<RSRenderAnimatableProperty<float>>(42.0f);
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap map1;
    map1.emplace(std::make_pair(nodeId, propId), MapValue(renderProp, {}));
    auto task1 = std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(1e8, std::move(map1));
    animator->ApplyShowingPropertyValues(rsUIContext, *task1);
    EXPECT_FLOAT_EQ(clientProp->stagingValue_, 42.0f);

    // G-false: null showing value -> skip, staging value untouched
    clientProp->stagingValue_ = 7.0f;
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap map2;
    map2.emplace(std::make_pair(nodeId, propId), MapValue(nullptr, {}));
    auto task2 = std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(1e8, std::move(map2));
    animator->ApplyShowingPropertyValues(rsUIContext, *task2);
    EXPECT_FLOAT_EQ(clientProp->stagingValue_, 7.0f);
    GTEST_LOG_(INFO) << "RSInteractiveImplictAnimatorTest ApplyShowingPropertyValuesAppliesValue001 end";
}

} // namespace Rosen
} // namespace OHOS
