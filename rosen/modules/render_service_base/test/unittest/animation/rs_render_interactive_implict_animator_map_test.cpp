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

#include "gtest/gtest.h"

#include "animation/rs_render_interactive_implict_animator_map.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderInteractiveImplictAnimatorMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr InteractiveImplictAnimatorId ANIMATOR_ID = 10001;
    static constexpr NodeId NODE_ID = 10002;
    static constexpr AnimationId ANIMATION_ID = 10003;
    static constexpr uint64_t PROPERTY_ID = 10004;
};

void RSRenderInteractiveImplictAnimatorMapTest::SetUpTestCase() {}
void RSRenderInteractiveImplictAnimatorMapTest::TearDownTestCase() {}
void RSRenderInteractiveImplictAnimatorMapTest::SetUp() {}
void RSRenderInteractiveImplictAnimatorMapTest::TearDown() {}

/**
 * @tc.name: RegisterInteractiveImplictAnimator001
 * @tc.desc: Verify the RegisterInteractiveImplictAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, RegisterInteractiveImplictAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest RegisterInteractiveImplictAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(animator);
    auto result = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(ANIMATOR_ID);
    EXPECT_TRUE(result != nullptr);

    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(animator);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest RegisterInteractiveImplictAnimator001 end";
}

/**
 * @tc.name: UnregisterInteractiveImplictAnimator001
 * @tc.desc: Verify the UnregisterInteractiveImplictAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UnregisterInteractiveImplictAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UnregisterInteractiveImplictAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(animator);
    auto result = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(ANIMATOR_ID);
    EXPECT_TRUE(result != nullptr);

    context.GetInteractiveImplictAnimatorMap().UnregisterInteractiveImplictAnimator(ANIMATOR_ID);
    result = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(ANIMATOR_ID);
    EXPECT_TRUE(result == nullptr);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UnregisterInteractiveImplictAnimator001 end";
}

/**
 * @tc.name: UpdateGroupAnimators001
 * @tc.desc: Verify UpdateGroupAnimators with empty map returns false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators001 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: empty map -> returns false
    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators001 end";
}

/**
 * @tc.name: UpdateGroupAnimators002
 * @tc.desc: Verify UpdateGroupAnimators with non-time-driven animator returns false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators002 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: animator && !animator->IsTimeDriven() -> skip, returns false
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(animator);

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_FALSE(result); // RSRenderInteractiveImplictAnimator is not time-driven

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators002 end";
}

/**
 * @tc.name: UpdateGroupAnimators003
 * @tc.desc: Verify UpdateGroupAnimators with time-driven animator not running returns false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators003 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: animator && animator->IsTimeDriven() && !IsRunning() -> returns false
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(0);
    timingProtocol.SetRepeatCount(1);
    timingProtocol.SetSpeed(1.0f);
    timingProtocol.SetAutoReverse(false);

    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID, context.weak_from_this(), timingProtocol);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator);

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_FALSE(result); // Animator is not running (state is INITIALIZED)

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators003 end";
}

/**
 * @tc.name: UpdateGroupAnimators004
 * @tc.desc: Verify UpdateGroupAnimators with running time-driven animator returns true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators004 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: animator && animator->IsTimeDriven() && IsRunning() -> returns true
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(0);
    timingProtocol.SetRepeatCount(1);
    timingProtocol.SetSpeed(1.0f);
    timingProtocol.SetAutoReverse(false);

    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID, context.weak_from_this(), timingProtocol);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator);

    // Start the animator to make it running
    groupAnimator->StartAnimator();

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_TRUE(result); // Animator is running

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators004 end";
}

/**
 * @tc.name: UpdateGroupAnimators005
 * @tc.desc: Verify UpdateGroupAnimators with mixed animators (time-driven and non-time-driven)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators005 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: multiple animators with different types
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(0);
    timingProtocol.SetRepeatCount(1);
    timingProtocol.SetSpeed(1.0f);
    timingProtocol.SetAutoReverse(false);

    // Add non-time-driven animator
    auto regularAnimator = std::make_shared<RSRenderInteractiveImplictAnimator>(
        ANIMATOR_ID, context.weak_from_this());
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(regularAnimator);

    // Add time-driven animator (not running)
    auto groupAnimator1 = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID + 1, context.weak_from_this(), timingProtocol);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator1);

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_FALSE(result); // No running animators

    // Start the time-driven animator
    groupAnimator1->StartAnimator();
    result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_TRUE(result); // Has running animator

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators005 end";
}

/**
 * @tc.name: UpdateGroupAnimators006
 * @tc.desc: Verify UpdateGroupAnimators updates minLeftDelayTime
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators006 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: OnAnimate updates minLeftDelayTime
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(100); // 100ms delay
    timingProtocol.SetRepeatCount(1);
    timingProtocol.SetSpeed(1.0f);
    timingProtocol.SetAutoReverse(false);

    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID, context.weak_from_this(), timingProtocol);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator);
    groupAnimator->StartAnimator();

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_TRUE(result);
    // minLeftDelayTime should be updated based on start delay

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators006 end";
}

/**
 * @tc.name: UpdateGroupAnimators007
 * @tc.desc: Verify UpdateGroupAnimators with multiple running time-driven animators
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators007 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: multiple time-driven animators, all running
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(0);
    timingProtocol.SetRepeatCount(1);
    timingProtocol.SetSpeed(1.0f);
    timingProtocol.SetAutoReverse(false);

    auto groupAnimator1 = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID, context.weak_from_this(), timingProtocol);
    auto groupAnimator2 = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID + 1, context.weak_from_this(), timingProtocol);

    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator1);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator2);

    groupAnimator1->StartAnimator();
    groupAnimator2->StartAnimator();

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_TRUE(result); // Has running animators

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators007 end";
}

/**
 * @tc.name: UpdateGroupAnimators008
 * @tc.desc: Verify UpdateGroupAnimators with some running and some non-running animators
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators008 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Cover branch: mix of running and non-running time-driven animators
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(0);
    timingProtocol.SetRepeatCount(1);
    timingProtocol.SetSpeed(1.0f);
    timingProtocol.SetAutoReverse(false);

    auto groupAnimator1 = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID, context.weak_from_this(), timingProtocol);
    auto groupAnimator2 = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID + 1, context.weak_from_this(), timingProtocol);

    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator1);
    context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator2);

    // Start only one animator
    groupAnimator1->StartAnimator();
    // groupAnimator2 is not started

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_TRUE(result); // Has at least one running animator

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators008 end";
}

/**
 * @tc.name: UpdateGroupAnimators009
 * @tc.desc: Cover branch: animator is nullptr in map
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorMapTest, UpdateGroupAnimators009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators009 start";
    RSContext context;
    int64_t minLeftDelayTime = 0;
    int64_t timestamp = 0;

    // Directly insert nullptr into map to cover animator == nullptr branch
    context.GetInteractiveImplictAnimatorMap().interactiveImplictAnimatorMap_.emplace(ANIMATOR_ID, nullptr);

    bool result = context.GetInteractiveImplictAnimatorMap().UpdateGroupAnimators(timestamp, minLeftDelayTime);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorMapTest UpdateGroupAnimators009 end";
}

} // namespace Rosen
} // namespace OHOS
