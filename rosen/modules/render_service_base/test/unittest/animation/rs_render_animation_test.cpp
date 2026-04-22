/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_animation.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderAnimationMock : public RSRenderAnimation {
public:
    static constexpr uint64_t ANIMATION_ID = 12345;
    RSRenderAnimationMock() : RSRenderAnimationMock(ANIMATION_ID) {}
    ~RSRenderAnimationMock() = default;
    void Pause()
    {
        RSRenderAnimation::Pause();
    }
    void Resume()
    {
        RSRenderAnimation::Resume();
    }
    void FinishOnCurrentPosition()
    {
        RSRenderAnimation::FinishOnCurrentPosition();
    }
    std::string GetTargeName()
    {
        return RSRenderAnimation::GetTargetName();
    }
    void Finish()
    {
        RSRenderAnimation::Finish();
    }
    void FinishOnPosition(RSInteractiveAnimationPosition pos)
    {
        RSRenderAnimation::FinishOnPosition(pos);
    }
    void AnimateOnGroupWaiting(int64_t time, bool isCustom)
    {
        RSRenderAnimation::AnimateOnGroupWaiting(time, isCustom);
    }
    void Start()
    {
        RSRenderAnimation::Start();
    }
    bool IsPaused() const
    {
        return RSRenderAnimation::IsPaused();
    }
    bool IsRunning() const
    {
        return RSRenderAnimation::IsRunning();
    }
    bool IsGroupWaiting() const
    {
        return RSRenderAnimation::IsGroupWaiting();
    }
    AnimationState GetState() const
    {
        return state_;
    }
    bool Animate(int64_t time, int64_t& minLeftDelayTime, bool isCustom)
    {
        return RSRenderAnimation::Animate(time, minLeftDelayTime, isCustom);
    }
    void SetAsGroupAnimationChild()
    {
        isGroupAnimationChild_ = true;
    }

protected:
    explicit RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
};

class RSRenderAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t NODE_ID = 12345;
};

void RSRenderAnimationTest::SetUpTestCase() {}
void RSRenderAnimationTest::TearDownTestCase() {}
void RSRenderAnimationTest::SetUp() {}
void RSRenderAnimationTest::TearDown() {}

/**
 * @tc.name: Pause001
 * @tc.desc: Verify the Pause
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Pause001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Pause001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->Pause();
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Pause001 end";
}

/**
 * @tc.name: Resume001
 * @tc.desc: Verify the Resume
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Resume001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Resume001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->Resume();
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Resume001 end";
}

/**
 * @tc.name: FinishOnCurrentPosition001
 * @tc.desc: Verify the FinishOnCurrentPosition
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnCurrentPosition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPosition001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->FinishOnCurrentPosition();
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPosition001 end";
}

/**
 * @tc.name: IsFinished001
 * @tc.desc: Verify the IsFinished
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, IsFinished001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsFinished001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    auto result = renderAnimation->IsFinished();
    EXPECT_TRUE(result == false);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsFinished001 end";
}

/**
 * @tc.name: SetReversed001
 * @tc.desc: Verify the SetReversed
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, SetReversed001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest SetReversed001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    renderAnimation->SetReversed(true);
    renderAnimation->Start();
    renderAnimation->SetReversed(true);
    EXPECT_TRUE(renderAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderAnimationTest SetReversed001 end";
}

/**
 * @tc.name: GetTargetName001
 * @tc.desc: Verify the GetTargetName
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, GetTargetName001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest GetTargetName001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    std::string nodeName = "1"; // for test
    renderNode->SetNodeName(nodeName);

    EXPECT_TRUE(renderAnimation != nullptr);
    renderAnimation->Attach(renderNode.get());
    auto name = renderAnimation->GetTargeName();
    EXPECT_EQ(name, nodeName);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest GetTargetName001 end";
}

/**
 * @tc.name: FlipDirection001
 * @tc.desc: Verify FlipDirection switches animation direction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FlipDirection001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FlipDirection001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    renderAnimation->FlipDirection();
    EXPECT_TRUE(renderAnimation != nullptr);

    renderAnimation->FlipDirection();
    EXPECT_TRUE(renderAnimation != nullptr);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FlipDirection001 end";
}

/**
 * @tc.name: ResumeGroupWaiting001
 * @tc.desc: Verify ResumeGroupWaiting when animation is not paused
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, ResumeGroupWaiting001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest ResumeGroupWaiting001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: !IsPaused() -> return early
    renderAnimation->ResumeGroupWaiting();
    // Should return without doing anything when not paused

    GTEST_LOG_(INFO) << "RSRenderAnimationTest ResumeGroupWaiting001 end";
}

/**
 * @tc.name: ResumeGroupWaiting002
 * @tc.desc: Verify ResumeGroupWaiting when animation is paused
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, ResumeGroupWaiting002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest ResumeGroupWaiting002 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsPaused() -> state_ = GROUP_WAITING
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest ResumeGroupWaiting002 end";
}

/**
 * @tc.name: Restart001
 * @tc.desc: Verify Restart when animation is not running and not groupWaiting
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Restart001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Restart001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: !IsRunning() && !IsGroupWaiting() -> return early
    renderAnimation->Restart();
    // Should return without doing anything when not running

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Restart001 end";
}

/**
 * @tc.name: Restart002
 * @tc.desc: Verify Restart when animation is running
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Restart002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Restart002 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsRunning() || IsGroupWaiting() -> execute restart logic
    renderAnimation->Start();
    renderAnimation->Restart();
    EXPECT_TRUE(renderAnimation->IsRunning());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Restart002 end";
}

/**
 * @tc.name: Restart003
 * @tc.desc: Verify Restart when animation is in GROUP_WAITING state
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Restart003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Restart003 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsGroupWaiting() -> execute restart logic
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    renderAnimation->Restart();
    EXPECT_TRUE(renderAnimation->IsRunning());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Restart003 end";
}

/**
 * @tc.name: IsGroupWaiting001
 * @tc.desc: Verify IsGroupWaiting returns true when state is GROUP_WAITING
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, IsGroupWaiting001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupWaiting001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: state_ == GROUP_WAITING -> return true
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupWaiting001 end";
}

/**
 * @tc.name: IsGroupWaiting002
 * @tc.desc: Verify IsGroupWaiting returns false when state is not GROUP_WAITING
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, IsGroupWaiting002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupWaiting002 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: state_ != GROUP_WAITING -> return false
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    renderAnimation->Start();
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupWaiting002 end";
}

/**
 * @tc.name: IsGroupAnimationChild001
 * @tc.desc: Verify IsGroupAnimationChild returns true when isGroupAnimationChild_ is true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, IsGroupAnimationChild001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupAnimationChild001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: isGroupAnimationChild_ == true -> return true
    RSContext context;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        10001, context.weak_from_this(), timingProtocol);

    renderAnimation->SetGroupAnimator(groupAnimator);
    EXPECT_TRUE(renderAnimation->IsGroupAnimationChild());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupAnimationChild001 end";
}

/**
 * @tc.name: IsGroupAnimationChild002
 * @tc.desc: Verify IsGroupAnimationChild returns false when isGroupAnimationChild_ is false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, IsGroupAnimationChild002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupAnimationChild002 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: isGroupAnimationChild_ == false -> return false
    EXPECT_FALSE(renderAnimation->IsGroupAnimationChild());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest IsGroupAnimationChild002 end";
}

/**
 * @tc.name: SetGroupAnimator001
 * @tc.desc: Verify SetGroupAnimator sets isGroupAnimationChild_ and groupAnimator_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, SetGroupAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest SetGroupAnimator001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    RSContext context;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        10001, context.weak_from_this(), timingProtocol);

    renderAnimation->SetGroupAnimator(groupAnimator);
    EXPECT_TRUE(renderAnimation->IsGroupAnimationChild());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest SetGroupAnimator001 end";
}

/**
 * @tc.name: RemoveFromGroupAnimator001
 * @tc.desc: Verify RemoveFromGroupAnimator when isGroupAnimationChild_ is false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, RemoveFromGroupAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest RemoveFromGroupAnimator001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: !isGroupAnimationChild_ -> return early
    renderAnimation->RemoveFromGroupAnimator();
    // Should return without doing anything

    GTEST_LOG_(INFO) << "RSRenderAnimationTest RemoveFromGroupAnimator001 end";
}

/**
 * @tc.name: RemoveFromGroupAnimator002
 * @tc.desc: Verify RemoveFromGroupAnimator when groupAnimator is valid
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, RemoveFromGroupAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest RemoveFromGroupAnimator002 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: isGroupAnimationChild_ && groupAnimator_.lock() valid
    RSContext context;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        10001, context.weak_from_this(), timingProtocol);

    renderAnimation->SetGroupAnimator(groupAnimator);
    EXPECT_TRUE(renderAnimation->IsGroupAnimationChild());

    renderAnimation->RemoveFromGroupAnimator();
    // Should call RemoveActiveChildAnimation

    GTEST_LOG_(INFO) << "RSRenderAnimationTest RemoveFromGroupAnimator002 end";
}

/**
 * @tc.name: RemoveFromGroupAnimator003
 * @tc.desc: Verify RemoveFromGroupAnimator when groupAnimator is expired
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, RemoveFromGroupAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest RemoveFromGroupAnimator003 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: isGroupAnimationChild_ && groupAnimator_.lock() == nullptr
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Create a temporary context that will be destroyed
    {
        RSContext tempContext;
        auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
            10001, tempContext.weak_from_this(), timingProtocol);
        renderAnimation->SetGroupAnimator(groupAnimator);
    }
    // groupAnimator is now expired

    renderAnimation->RemoveFromGroupAnimator();
    // Should handle nullptr gracefully

    GTEST_LOG_(INFO) << "RSRenderAnimationTest RemoveFromGroupAnimator003 end";
}

/**
 * @tc.name: AnimateOnGroupWaitingNeedUpdateStartTimeTrue
 * @tc.desc: Verify AnimateOnGroupWaiting with needUpdateStartTime_ = true (initial call)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateOnGroupWaitingNeedUpdateStartTimeTrue, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingNeedUpdateStartTimeTrue start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: needUpdateStartTime_ == true -> SetStartTime
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    int64_t time = 100;
    renderAnimation->AnimateOnGroupWaiting(time, false);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingNeedUpdateStartTimeTrue end";
}

/**
 * @tc.name: AnimateOnGroupWaitingNeedUpdateStartTimeFalse
 * @tc.desc: Verify AnimateOnGroupWaiting with needUpdateStartTime_ = false (second call)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateOnGroupWaitingNeedUpdateStartTimeFalse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingNeedUpdateStartTimeFalse start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: needUpdateStartTime_ = false
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();
    // First call sets needUpdateStartTime_ = false
    renderAnimation->AnimateOnGroupWaiting(100, false);

    // Second call with needUpdateStartTime_ = false
    renderAnimation->AnimateOnGroupWaiting(200, false);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingNeedUpdateStartTimeFalse end";
}

/**
 * @tc.name: AnimateOnGroupWaitingDeltaTimeZero
 * @tc.desc: Verify AnimateOnGroupWaiting with deltaTime <= 0 (stay in GROUP_WAITING)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateOnGroupWaitingDeltaTimeZero, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingDeltaTimeZero start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: deltaTime <= 0 -> stay in GROUP_WAITING
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    int64_t time = 0;
    renderAnimation->AnimateOnGroupWaiting(time, false);

    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingDeltaTimeZero end";
}

/**
 * @tc.name: AnimateOnGroupWaitingUpdateReturnsFalse
 * @tc.desc: Verify AnimateOnGroupWaiting with UpdateGroupWaitingTime returning false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateOnGroupWaitingUpdateReturnsFalse, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingUpdateReturnsFalse start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: deltaTime > 0 && UpdateGroupWaitingTime returns false
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    // Set runningTime_ > 0 and direction_ = NORMAL to make UpdateGroupWaitingTime return false
    renderAnimation->animationFraction_.runningTime_ = 100;
    renderAnimation->animationFraction_.SetDirectionAfterStart(ForwardDirection::NORMAL);

    int64_t time = 100;
    renderAnimation->AnimateOnGroupWaiting(time, false);

    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingUpdateReturnsFalse end";
}

/**
 * @tc.name: AnimateOnGroupWaitingTransitionToRunning
 * @tc.desc: Verify AnimateOnGroupWaiting with UpdateGroupWaitingTime returning true (state -> RUNNING)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateOnGroupWaitingTransitionToRunning, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingTransitionToRunning start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    renderAnimation->needUpdateStartTime_ = false;
    renderAnimation->animationFraction_.SetLastFrameTime(100);
    renderAnimation->animationFraction_.runningTime_ = 0;
    renderAnimation->animationFraction_.groupWaitingTime_ = 0;

    renderAnimation->AnimateOnGroupWaiting(200, false);

    EXPECT_TRUE(renderAnimation->IsRunning());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingTransitionToRunning end";
}

/**
 * @tc.name: AnimateOnGroupWaitingWithIsCustom
 * @tc.desc: Verify AnimateOnGroupWaiting with isCustom = true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateOnGroupWaitingWithIsCustom, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingWithIsCustom start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    int64_t time = 100;
    renderAnimation->AnimateOnGroupWaiting(time, true);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateOnGroupWaitingWithIsCustom end";
}

/**
 * @tc.name: Finish001
 * @tc.desc: Cover branch: !IsPaused() && !IsRunning() && !IsGroupWaiting() (return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Finish001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish001 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: all states are false -> should return early
    EXPECT_FALSE(renderAnimation->IsPaused());
    EXPECT_FALSE(renderAnimation->IsRunning());
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    renderAnimation->Finish();
    // State should remain INITIALIZED (not changed to FINISHED)
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::INITIALIZED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish001 end";
}

/**
 * @tc.name: Finish002
 * @tc.desc: Cover branch: IsPaused() && !IsRunning() && !IsGroupWaiting() (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Finish002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish002 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Set to PAUSED state
    renderAnimation->Start();
    renderAnimation->Pause();

    // Cover branch: IsPaused() = true -> should succeed
    EXPECT_TRUE(renderAnimation->IsPaused());
    EXPECT_FALSE(renderAnimation->IsRunning());
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    renderAnimation->Finish();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish002 end";
}

/**
 * @tc.name: Finish003
 * @tc.desc: Cover branch: !IsPaused() && IsRunning() && !IsGroupWaiting() (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Finish003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish003 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Set to RUNNING state
    renderAnimation->Start();

    // Cover branch: IsRunning() = true -> should succeed
    EXPECT_FALSE(renderAnimation->IsPaused());
    EXPECT_TRUE(renderAnimation->IsRunning());
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    renderAnimation->Finish();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish003 end";
}

/**
 * @tc.name: Finish004
 * @tc.desc: Cover branch: !IsPaused() && !IsRunning() && IsGroupWaiting() (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, Finish004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish004 start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Set to GROUP_WAITING state
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    // Cover branch: IsGroupWaiting() = true -> should succeed
    EXPECT_FALSE(renderAnimation->IsPaused());
    EXPECT_FALSE(renderAnimation->IsRunning());
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    renderAnimation->Finish();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Finish004 end";
}

/**
 * @tc.name: FinishOnPosition001
 * @tc.desc: Cover branch: all states false, return early (pos = START)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnPositionNotRunning, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionNotRunning start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: all states false + pos = START -> should return early
    renderAnimation->FinishOnPosition(RSInteractiveAnimationPosition::START);
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::INITIALIZED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionNotRunning end";
}

/**
 * @tc.name: FinishOnPositionRunningAtStart
 * @tc.desc: Cover branch: RUNNING state, pos = START (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnPositionRunningAtStart, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionRunningAtStart start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsRunning() = true + pos = START -> should succeed
    renderAnimation->Start();
    renderAnimation->FinishOnPosition(RSInteractiveAnimationPosition::START);
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionRunningAtStart end";
}

/**
 * @tc.name: FinishOnPositionRunningAtEnd
 * @tc.desc: Cover branch: RUNNING state, pos = END (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnPositionRunningAtEnd, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionRunningAtEnd start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsRunning() = true + pos = END -> should succeed
    renderAnimation->Start();
    renderAnimation->FinishOnPosition(RSInteractiveAnimationPosition::END);
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionRunningAtEnd end";
}

/**
 * @tc.name: FinishOnPositionPausedAtStart
 * @tc.desc: Cover branch: IsPaused() = true, pos = START (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnPositionPausedAtStart, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionPausedAtStart start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsPaused() = true + pos = START -> should succeed
    renderAnimation->Start();
    renderAnimation->Pause();
    EXPECT_TRUE(renderAnimation->IsPaused());
    renderAnimation->FinishOnPosition(RSInteractiveAnimationPosition::START);
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionPausedAtStart end";
}

/**
 * @tc.name: FinishOnPositionGroupWaitingAtEnd
 * @tc.desc: Cover branch: IsGroupWaiting() = true, pos = END (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnPositionGroupWaitingAtEnd, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPositionGroupWaitingAtEnd start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsGroupWaiting() = true + pos = END -> should succeed
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());
    renderAnimation->FinishOnPosition(RSInteractiveAnimationPosition::END);
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnPosition005 end";
}

/**
 * @tc.name: FinishOnCurrentPositionNotRunning
 * @tc.desc: Cover branch: all states false, return early
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnCurrentPositionNotRunning, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPositionNotRunning start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: all states false -> should return early
    EXPECT_FALSE(renderAnimation->IsPaused());
    EXPECT_FALSE(renderAnimation->IsRunning());
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    renderAnimation->FinishOnCurrentPosition();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::INITIALIZED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPositionNotRunning end";
}

/**
 * @tc.name: FinishOnCurrentPositionRunning
 * @tc.desc: Cover branch: RUNNING state (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnCurrentPositionRunning, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPositionRunning start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsRunning() = true -> should succeed
    renderAnimation->Start();
    EXPECT_TRUE(renderAnimation->IsRunning());

    renderAnimation->FinishOnCurrentPosition();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPositionRunning end";
}

/**
 * @tc.name: FinishOnCurrentPositionPaused
 * @tc.desc: Cover branch: IsPaused() = true (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnCurrentPositionPaused, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPositionPaused start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsPaused() = true -> should succeed
    renderAnimation->Start();
    renderAnimation->Pause();
    EXPECT_TRUE(renderAnimation->IsPaused());

    renderAnimation->FinishOnCurrentPosition();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPosition004 end";
}

/**
 * @tc.name: FinishOnCurrentPositionGroupWaiting
 * @tc.desc: Cover branch: IsGroupWaiting() = true (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, FinishOnCurrentPositionGroupWaiting, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPositionGroupWaiting start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsGroupWaiting() = true -> should succeed
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    renderAnimation->FinishOnCurrentPosition();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::FINISHED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest FinishOnCurrentPositionGroupWaiting end";
}

/**
 * @tc.name: PauseNotRunning
 * @tc.desc: Cover branch: !IsRunning() && !IsGroupWaiting() (return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, PauseNotRunning, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest PauseNotRunning start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: not running and not group waiting -> should return early
    EXPECT_FALSE(renderAnimation->IsRunning());
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    renderAnimation->Pause();
    // State should remain INITIALIZED (not changed to PAUSED)
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::INITIALIZED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest PauseNotRunning end";
}

/**
 * @tc.name: PauseFromRunning
 * @tc.desc: Cover branch: IsRunning() && !IsGroupWaiting() (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, PauseFromRunning, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest PauseFromRunning start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsRunning() = true -> should succeed
    renderAnimation->Start();
    EXPECT_TRUE(renderAnimation->IsRunning());
    EXPECT_FALSE(renderAnimation->IsGroupWaiting());

    renderAnimation->Pause();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::PAUSED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest PauseFromRunning end";
}

/**
 * @tc.name: PauseFromGroupWaiting
 * @tc.desc: Cover branch: IsGroupWaiting() (success)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, PauseFromGroupWaiting, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest PauseFromGroupWaiting start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Cover branch: IsGroupWaiting() = true -> should succeed
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    renderAnimation->Pause();
    EXPECT_EQ(renderAnimation->GetState(), AnimationState::PAUSED);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest PauseFromGroupWaiting end";
}

/**
 * @tc.name: AnimateWithGroupWaiting
 * @tc.desc: Cover branch: IsGroupWaiting() = true (call AnimateOnGroupWaiting, return false)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateWithGroupWaiting, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateWithGroupWaiting start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Set up GROUP_WAITING state
    renderAnimation->Start();
    renderAnimation->Pause();
    renderAnimation->ResumeGroupWaiting();

    // Verify state is GROUP_WAITING
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    // Call Animate - should call AnimateOnGroupWaiting and return false
    int64_t minLeftDelayTime = 100;
    bool result = renderAnimation->Animate(200, minLeftDelayTime, false);

    EXPECT_FALSE(result);
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateWithGroupWaiting end";
}

/**
 * @tc.name: AnimateAlreadyFinished
 * @tc.desc: Cover branch: !IsRunning() with state = FINISHED (return true)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateAlreadyFinished, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateAlreadyFinished start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Set to FINISHED state
    renderAnimation->Start();
    renderAnimation->Finish();

    // Verify state is FINISHED
    EXPECT_TRUE(renderAnimation->IsFinished());

    // Call Animate - should return true because state is FINISHED
    int64_t minLeftDelayTime = 100;
    bool result = renderAnimation->Animate(200, minLeftDelayTime, false);

    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateAlreadyFinished end";
}

/**
 * @tc.name: AnimatePausedState
 * @tc.desc: Cover branch: !IsRunning() with state = PAUSED (return false)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimatePausedState, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimatePausedState start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Set to PAUSED state
    renderAnimation->Start();
    renderAnimation->Pause();

    // Verify state is PAUSED
    EXPECT_TRUE(renderAnimation->IsPaused());

    // Call Animate - should return false because state is PAUSED (not FINISHED)
    int64_t minLeftDelayTime = 100;
    bool result = renderAnimation->Animate(200, minLeftDelayTime, false);

    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimatePausedState end";
}

/**
 * @tc.name: AnimateGroupChildFinished
 * @tc.desc: Cover branch: isFinished = true && isGroupAnimationChild_ = true (set GROUP_WAITING)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateGroupChildFinished, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateGroupChildFinished start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    // Set as group animation child
    renderAnimation->SetAsGroupAnimationChild();
    renderAnimation->Start();

    // Set duration to 0 to trigger isFinished = true
    renderAnimation->SetDuration(0);

    // Call Animate - should trigger isFinished = true
    // then set GROUP_WAITING because isGroupAnimationChild_ = true
    int64_t minLeftDelayTime = 100;
    renderAnimation->needUpdateStartTime_ = false;
    renderAnimation->Animate(1000, minLeftDelayTime, false);

    // Verify: animation should be in GROUP_WAITING state
    EXPECT_TRUE(renderAnimation->IsGroupWaiting());

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateGroupChildFinished end";
}

/**
 * @tc.name: AnimateNonGroupChildFinished
 * @tc.desc: Cover branch: isFinished = true && isGroupAnimationChild_ = false (return true)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateNonGroupChildFinished, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateNonGroupChildFinished start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    renderAnimation->Start();
    renderAnimation->SetDuration(0);
    renderAnimation->needUpdateStartTime_ = false;
    renderAnimation->animationFraction_.SetLastFrameTime(1000);

    int64_t minLeftDelayTime = 100;
    bool result = renderAnimation->Animate(2000, minLeftDelayTime, false);

    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateNonGroupChildFinished end";
}

/**
 * @tc.name: AnimateSameFrameTime
 * @tc.desc: Cover branch: time == lastFrameTime (return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateSameFrameTime, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateSameFrameTime start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    renderAnimation->Start();

    // Call Animate with same time twice
    int64_t minLeftDelayTime = 100;
    renderAnimation->Animate(100, minLeftDelayTime, false);

    // Call again with same time - should hit the time == lastFrameTime branch
    bool result = renderAnimation->Animate(100, minLeftDelayTime, false);

    // Since animation is RUNNING, should return false (not finished)
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateSameFrameTime end";
}

/**
 * @tc.name: AnimateSetStartTime
 * @tc.desc: Cover branch: needUpdateStartTime_ = true (set start time)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderAnimationTest, AnimateSetStartTime, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationTest AnimateSetStartTime start";
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>();

    renderAnimation->Start();

    // Call Animate - should set start time
    int64_t minLeftDelayTime = 100;
    bool result = renderAnimation->Animate(100, minLeftDelayTime, false);

    // Animation is running, not finished
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSRenderAnimationTest Animate006 end";
}

} // namespace Rosen
} // namespace OHOS
