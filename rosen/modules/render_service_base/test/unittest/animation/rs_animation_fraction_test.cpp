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

#include "gtest/gtest.h"

#include "animation/rs_animation_fraction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAnimationFractionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationFractionTest::SetUpTestCase() {}
void RSAnimationFractionTest::TearDownTestCase() {}

void RSAnimationFractionTest::SetUp()
{
    RSAnimationFraction::Init();
}

void RSAnimationFractionTest::TearDown() {}

/**
 * @tc.name: GetAnimationFraction001
 * @tc.desc: Verify the GetAnimationFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetAnimationFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetAnimationFraction001 start";
    bool isDelay = false;
    int64_t leftDelayTime = 0;
    bool isFinished = false;
    bool isRepeatFinished = false;
    float result = 0.0f;
    float resultNegative = 0.0f;
    RSAnimationFraction fraction;
    fraction.SetDuration(0);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(0, leftDelayTime, false);
    EXPECT_EQ(result, 1.0f);

    fraction.SetDuration(300);
    fraction.SetRepeatCount(0);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(0, leftDelayTime, false);
    EXPECT_EQ(result, 1.0f);

    fraction.SetRepeatCount(1);
    RSAnimationFraction::OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "0", nullptr);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);

    fraction.SetRepeatCount(1);
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);

    leftDelayTime = 100;
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    std::tie(resultNegative, isDelay, isFinished, isRepeatFinished) =
        fraction.GetAnimationFraction(90, leftDelayTime, false);
    EXPECT_TRUE(resultNegative < result);

    fraction.startDelay_ = 300;
    leftDelayTime = 100;
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(0, leftDelayTime, false);
    EXPECT_FALSE(result);

    // Restore animationScale to default value
    RSAnimationFraction::OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "1", nullptr);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetAnimationFraction001 end";
}

/**
 * @tc.name: GetAnimationFraction002
 * @tc.desc: Verify the GetAnimationFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetAnimationFraction002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetAnimationFraction001 start";
    RSAnimationFraction::OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "1", nullptr);
    RSAnimationFraction fraction;
    bool isDelay = false;
    int64_t leftDelayTime = 0;
    bool isFinishedFalse = false;
    bool isFinishedTrue = true;
    bool isRepeatFinished = false;
    float result = 0.0f;
    fraction.SetDuration(300);
    fraction.SetRepeatCount(1);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);

    fraction.SetDirection(false);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);

    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.SetDirection(true);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);

    leftDelayTime = 100;
    fraction.SetDirection(false);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_NE(result, 0.0f);

    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.SetDirection(true);
    fraction.runningTime_ = 0;
    leftDelayTime = 10000;
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) =
        fraction.GetAnimationFraction(100, leftDelayTime, false);
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetAnimationFraction002 end";
}

/**
 * @tc.name: GetRemainingRepeatCount001
 * @tc.desc: Verify the GetRemainingRepeatCount
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetRemainingRepeatCount001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetRemainingRepeatCount001 start";
    RSAnimationFraction fraction;
    fraction.SetRepeatCount(-1);
    int remainingRepeatCount = fraction.GetRemainingRepeatCount();
    EXPECT_EQ(remainingRepeatCount, -1);

    fraction.SetRepeatCount(0);
    remainingRepeatCount = fraction.GetRemainingRepeatCount();
    EXPECT_EQ(remainingRepeatCount, 0);

    fraction.SetRepeatCount(1);
    remainingRepeatCount = fraction.GetRemainingRepeatCount();
    EXPECT_EQ(remainingRepeatCount, 1);
    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetRemainingRepeatCount001 end";
}

/**
 * @tc.name: GetCurrentIsReverseCycle001
 * @tc.desc: Verify funciton GetCurrentIsReverseCycle
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetCurrentIsReverseCycle001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetCurrentIsReverseCycle001 start";
    RSAnimationFraction fraction;
    fraction.ResetFraction();
    bool currentIsReverseCycle = fraction.GetCurrentIsReverseCycle();
    EXPECT_TRUE(currentIsReverseCycle == false);
    GTEST_LOG_(INFO) << "RSAnimationFractionTest GetCurrentIsReverseCycle001 end";
}

/**
 * @tc.name: AnimationScaleChangedCallback001
 * @tc.desc: Verify the AnimationScaleChangedCallback
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, AnimationScaleChangedCallback001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest AnimationScaleChangedCallback001 start";
    RSAnimationFraction::OnAnimationScaleChangedCallback("", "1", nullptr);
    RSAnimationFraction::OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "1", nullptr);
    EXPECT_TRUE(RSAnimationFraction::animationScale_);
    GTEST_LOG_(INFO) << "RSAnimationFractionTest AnimationScaleChangedCallback001 end";
}

/**
 * @tc.name: FlipDirection001
 * @tc.desc: Verify FlipDirection switches from NORMAL to REVERSE
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, FlipDirection001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest FlipDirection001 start";
    RSAnimationFraction fraction;

    // Cover branch: direction_ == ForwardDirection::NORMAL
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    EXPECT_EQ(fraction.direction_, ForwardDirection::NORMAL);

    fraction.FlipDirection();
    // After flip: direction_ should be ForwardDirection::REVERSE
    EXPECT_EQ(fraction.direction_, ForwardDirection::REVERSE);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest FlipDirection001 end";
}

/**
 * @tc.name: FlipDirection002
 * @tc.desc: Verify FlipDirection switches from REVERSE to NORMAL
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, FlipDirection002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest FlipDirection002 start";
    RSAnimationFraction fraction;

    // Cover branch: direction_ != ForwardDirection::NORMAL (REVERSE)
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    EXPECT_EQ(fraction.direction_, ForwardDirection::REVERSE);

    fraction.FlipDirection();
    // After flip: direction_ should be ForwardDirection::NORMAL
    EXPECT_EQ(fraction.direction_, ForwardDirection::NORMAL);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest FlipDirection002 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime001
 * @tc.desc: Cover branch: direction=NORMAL, runningTime>0, normal addition
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime001 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover branch: direction_ == NORMAL && runningTime_ > 0
    // Cover branch: normal addition (no overflow)
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    fraction.runningTime_ = 100; // Directly set runningTime_ > 0

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    EXPECT_FALSE(result); // Returns false in NORMAL direction

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime001 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime002
 * @tc.desc: Cover branch: runningTime_ <= 0 returns true directly
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime002 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover branch: runningTime_ <= 0 returns true directly
    // runningTime_ default is 0, so returns true immediately
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // runningTime_ = 0 <= 0, returns true directly
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime002 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime003
 * @tc.desc: Cover branch: isCustom=true (use scale=1.0)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime003 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);

    // Make runningTime_ > 0 to enter the NORMAL direction branch
    fraction.runningTime_ = 100;

    // Cover branch: isCustom ? 1.0f : GetAnimationScale() (true branch)
    // When isCustom=true, animationScale=1.0f, deltaWithSpeed = deltaTime * speed_ / 1.0f = 100
    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, true); // isCustom = true
    EXPECT_FALSE(result); // Returns false when direction is NORMAL and runningTime_ > 0

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime003 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime004
 * @tc.desc: Cover branch: isCustom=false (use system scale)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime004 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);

    // Make runningTime_ > 0 to enter the NORMAL direction branch
    fraction.runningTime_ = 100;

    // Cover branch: isCustom ? 1.0f : GetAnimationScale() (false branch)
    // When isCustom=false, animationScale=2.0f, deltaWithSpeed = deltaTime * speed_ / 2.0f = 50
    RSAnimationFraction::SetAnimationScale(2.0f);

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false); // isCustom = false
    EXPECT_FALSE(result); // Returns false when direction is NORMAL and runningTime_ > 0

    // Restore default animation scale
    RSAnimationFraction::SetAnimationScale(1.0f);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime004 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime005
 * @tc.desc: Cover branch: animationScale == 0.0f (use MAX_SPEED)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime005 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);

    // Make runningTime_ > 0 to enter the NORMAL direction branch
    fraction.runningTime_ = 100;

    // Cover branch: ROSEN_EQ(animationScale, 0.0f)
    // When animationScale=0.0f, deltaWithSpeed = deltaTime * MAX_SPEED
    RSAnimationFraction::SetAnimationScale(0.0f);

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    EXPECT_FALSE(result); // Returns false when direction is NORMAL and runningTime_ > 0

    // Restore default animation scale
    RSAnimationFraction::SetAnimationScale(1.0f);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime005 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime006
 * @tc.desc: Cover branch: animationScale != 0.0f (use speed/scale calculation)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime006 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(2.0f);
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);

    // Make runningTime_ > 0 to enter the NORMAL direction branch
    fraction.runningTime_ = 100;

    // Cover branch: !ROSEN_EQ(animationScale, 0.0f)
    // When animationScale=0.5f, speed=2.0f, deltaWithSpeed = 100 * 2.0 / 0.5 = 400
    RSAnimationFraction::SetAnimationScale(0.5f);

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    EXPECT_FALSE(result); // Returns false when direction is NORMAL and runningTime_ > 0

    // Restore default animation scale
    RSAnimationFraction::SetAnimationScale(1.0f);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime006 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime007
 * @tc.desc: Cover branch: direction=REVERSE, runningTime_>0, groupWaitingTime_>0 returns false
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime007 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover REVERSE branch: runningTime_ > 0, groupWaitingTime_ > 0 returns false
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.runningTime_ = 100; // Need runningTime_ > 0 to enter REVERSE branch
    fraction.groupWaitingTime_ = 200;

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // groupWaitingTime_ = 200 - 100 = 100 > 0, so return false
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime007 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime008
 * @tc.desc: Cover branch: direction=REVERSE, runningTime_>0, groupWaitingTime_<=0 returns true
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime008 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover REVERSE branch: runningTime_ > 0, groupWaitingTime_ <= 0 returns true
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.runningTime_ = 100; // Need runningTime_ > 0 to enter REVERSE branch
    fraction.groupWaitingTime_ = 50;

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // groupWaitingTime_ = 50 - 100 = -50 <= 0, so return true
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime008 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime009
 * @tc.desc: Cover if branch: direction NORMAL with runningTime > 0 returns false
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime009 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover if branch: direction NORMAL with runningTime_ > 0
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    fraction.runningTime_ = 100; // Directly set runningTime_ > 0

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // Should return false when direction is NORMAL and runningTime_ > 0
    EXPECT_FALSE(result);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime009 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime010
 * @tc.desc: Cover branch: runningTime_ <= 0 returns true directly
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime010 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover branch: runningTime_ <= 0 returns true directly
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    fraction.runningTime_ = 0; // runningTime_ <= 0

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // runningTime_ <= 0, returns true directly without entering direction branch
    EXPECT_TRUE(result);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime010 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime011
 * @tc.desc: Cover branch: overflow check in NORMAL direction
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime011 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover if branch: direction is NORMAL, runningTime_ > 0
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    fraction.runningTime_ = 100;

    // Set groupWaitingTime_ near INT64_MAX to test overflow
    fraction.groupWaitingTime_ = INT64_MAX - 50;

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // Should detect overflow and clamp to INT64_MAX
    EXPECT_FALSE(result); // NORMAL direction always returns false
    EXPECT_EQ(fraction.groupWaitingTime_, INT64_MAX);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime011 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime012
 * @tc.desc: Cover branch: underflow check in REVERSE direction
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime012 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Cover else branch: direction is REVERSE
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.runningTime_ = 100;

    // Set groupWaitingTime_ near INT64_MIN to test underflow
    fraction.groupWaitingTime_ = INT64_MIN + 50;

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // Should detect underflow and clamp to INT64_MIN
    EXPECT_TRUE(result); // groupWaitingTime_ <= 0 after subtraction

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime012 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime013
 * @tc.desc: Cover branch: zero or negative deltaTime
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime013 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Test with deltaTime = 0
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    fraction.runningTime_ = 100;
    fraction.groupWaitingTime_ = 100;

    bool result = fraction.UpdateGroupWaitingTime(0, false);
    EXPECT_FALSE(result); // deltaWithSpeed = 0, no change to groupWaitingTime_
    EXPECT_EQ(fraction.groupWaitingTime_, 100);

    // Test with negative deltaTime (edge case)
    fraction.groupWaitingTime_ = 200;
    result = fraction.UpdateGroupWaitingTime(-50, false);
    EXPECT_FALSE(result); // deltaWithSpeed would be negative, but calculation handles it

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime013 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime014
 * @tc.desc: Cover branch: speed=0 (deltaWithSpeed=0)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime014 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(0.0f); // speed = 0

    // Test NORMAL direction with speed=0
    fraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    fraction.runningTime_ = 100;
    fraction.groupWaitingTime_ = 100;

    int64_t deltaTime = 100;
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // deltaWithSpeed = deltaTime * speed / animationScale = 0
    // groupWaitingTime_ should remain unchanged
    EXPECT_FALSE(result);
    EXPECT_EQ(fraction.groupWaitingTime_, 100);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime014 end";
}

/**
 * @tc.name: UpdateGroupWaitingTime015
 * @tc.desc: Cover branch: deltaWithSpeed <= 0 in REVERSE direction (condition: deltaWithSpeed > 0 && ...)
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateGroupWaitingTime015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime015 start";
    RSAnimationFraction fraction;
    fraction.SetDuration(1000);
    fraction.SetSpeed(1.0f);

    // Enter REVERSE branch (where the condition deltaWithSpeed > 0 && groupWaitingTime_ < INT64_MIN + deltaWithSpeed)
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.runningTime_ = 100;
    fraction.groupWaitingTime_ = 100;

    int64_t deltaTime = -100; // negative deltaTime, so deltaWithSpeed < 0
    bool result = fraction.UpdateGroupWaitingTime(deltaTime, false);
    // deltaWithSpeed < 0, so condition (deltaWithSpeed > 0 && ...) is false due to short-circuit evaluation
    // Should execute: groupWaitingTime_ -= (-100) = groupWaitingTime_ + 100
    EXPECT_FALSE(result); // 100 - (-100) = 200 > 0, returns false
    EXPECT_EQ(fraction.groupWaitingTime_, 200);

    GTEST_LOG_(INFO) << "RSAnimationFractionTest UpdateGroupWaitingTime015 end";
}

} // namespace Rosen
} // namespace OHOS
