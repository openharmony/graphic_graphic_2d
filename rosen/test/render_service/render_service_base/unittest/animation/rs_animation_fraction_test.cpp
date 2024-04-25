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
#include "animation/rs_animation_fraction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationFractionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationFractionTest::SetUpTestCase() {}
void RSAnimationFractionTest::TearDownTestCase() {}
void RSAnimationFractionTest::SetUp() {}
void RSAnimationFractionTest::TearDown() {}

/**
 * @tc.name: InitTest001
 * @tc.desc: test results of Init
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, InitTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    bool isInitialized_ = false;
    rsAnimationFraction.Init();
    ASSERT_EQ(isInitialized_, false);
}

/**
 * @tc.name: GetAnimationScaleTest001
 * @tc.desc: test results of GetAnimationScale
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetAnimationScaleTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetAnimationScale();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: SetAnimationScaleTest001
 * @tc.desc: test results of SetAnimationScale
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, SetAnimationScaleTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float animationScale = 0.f;
    rsAnimationFraction.SetAnimationScale(animationScale);
    ASSERT_EQ(animationScale, 0);
}

/**
 * @tc.name: OnAnimationScaleChangedCallbackTest001
 * @tc.desc: test results of OnAnimationScaleChangedCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, OnAnimationScaleChangedCallbackTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    rsAnimationFraction.OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "1", nullptr);
    rsAnimationFraction.OnAnimationScaleChangedCallback("persist", "1", nullptr);
    EXPECT_EQ(rsAnimationFraction.GetRepeatCallbackEnable(), false);
}

/**
 * @tc.name: SetDirectionAfterStartTest001
 * @tc.desc: test results of SetDirectionAfterStart
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, SetDirectionAfterStartTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    ForwardDirection direction = ForwardDirection::NORMAL;
    rsAnimationFraction.SetDirectionAfterStart(direction);
    EXPECT_EQ(direction, ForwardDirection::NORMAL);
}

/**
 * @tc.name: SetLastFrameTimeTest001
 * @tc.desc: test results of SetLastFrameTime
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, SetLastFrameTimeTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t lastFrameTime = 0;
    rsAnimationFraction.SetLastFrameTime(lastFrameTime);
    EXPECT_EQ(lastFrameTime, 0);
}

/**
 * @tc.name: GetLastFrameTimeTest001
 * @tc.desc: test results of GetLastFrameTime
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetLastFrameTimeTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t res = rsAnimationFraction.GetLastFrameTime();
    EXPECT_NE(res, 1);
}

/**
 * @tc.name: IsStartRunningTest001
 * @tc.desc: test results of IsStartRunning
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, IsStartRunningTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t deltaTime = 1;
    int64_t startDelayNs = 0;
    rsAnimationFraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    rsAnimationFraction.SetAnimationScale(-0.1f);
    bool res = rsAnimationFraction.IsStartRunning(deltaTime, startDelayNs);
    EXPECT_EQ(res, true);

    rsAnimationFraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    rsAnimationFraction.SetAnimationScale(0.1f);
    res = rsAnimationFraction.IsStartRunning(deltaTime, startDelayNs);
    EXPECT_EQ(res, true);

    rsAnimationFraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    rsAnimationFraction.SetAnimationScale(-0.1f);
    res = rsAnimationFraction.IsStartRunning(deltaTime, startDelayNs);
    EXPECT_EQ(res, true);

    rsAnimationFraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    rsAnimationFraction.SetAnimationScale(0.1f);
    res = rsAnimationFraction.IsStartRunning(deltaTime, startDelayNs);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetAnimationFractionTest001
 * @tc.desc: test results of GetAnimationFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetAnimationFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t time = 0;
    rsAnimationFraction.GetAnimationFraction(time);
    ASSERT_EQ(time, 0);
}

/**
 * @tc.name: IsInRepeatTest001
 * @tc.desc: test results of IsInRepeat
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, IsInRepeatTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    rsAnimationFraction.SetRepeatCallbackEnable(true);
    rsAnimationFraction.SetRepeatCount(-1);
    bool isInRepeat = rsAnimationFraction.IsInRepeat();
    EXPECT_EQ(isInRepeat, true);

    rsAnimationFraction.SetRepeatCallbackEnable(false);
    isInRepeat = rsAnimationFraction.IsInRepeat();
    EXPECT_EQ(isInRepeat, false);
}

/**
 * @tc.name: IsFinishedTest001
 * @tc.desc: test results of IsFinished
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, IsFinishedTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    rsAnimationFraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    rsAnimationFraction.SetRepeatCount(-1);
    bool ifFinished = rsAnimationFraction.IsFinished();
    EXPECT_EQ(ifFinished, false);

    rsAnimationFraction.SetDirectionAfterStart(ForwardDirection::NORMAL);
    rsAnimationFraction.SetRepeatCount(0);
    ifFinished = rsAnimationFraction.IsFinished();
    EXPECT_EQ(ifFinished, true);

    rsAnimationFraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    ifFinished = rsAnimationFraction.IsFinished();
    EXPECT_EQ(ifFinished, true);
}

/**
 * @tc.name: GetStartFractionTest001
 * @tc.desc: test results of GetStartFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetStartFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetStartFraction();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: GetEndFractionTest001
 * @tc.desc: test results of GetEndFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetEndFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetEndFraction();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: UpdateReverseStateTest001
 * @tc.desc: test results of UpdateReverseState
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateReverseStateTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    bool finish = true;
    rsAnimationFraction.UpdateReverseState(finish);
    EXPECT_EQ(rsAnimationFraction.currentIsReverseCycle_, false);

    rsAnimationFraction.SetAutoReverse(false);
    rsAnimationFraction.UpdateReverseState(finish);
    EXPECT_EQ(rsAnimationFraction.autoReverse_, false);

    finish = false;
    rsAnimationFraction.SetAutoReverse(false);
    rsAnimationFraction.UpdateReverseState(finish);
    EXPECT_EQ(finish, false);

    finish = true;
    rsAnimationFraction.SetDirection(false);
    rsAnimationFraction.SetAutoReverse(false);
    rsAnimationFraction.UpdateReverseState(finish);
    EXPECT_EQ(rsAnimationFraction.isForward_, false);

    rsAnimationFraction.SetDirection(false);
    rsAnimationFraction.SetAutoReverse(false);
    rsAnimationFraction.UpdateReverseState(finish);
    EXPECT_EQ(finish, true);

    finish = false;
    rsAnimationFraction.SetDirection(false);
    rsAnimationFraction.SetAutoReverse(false);
    rsAnimationFraction.UpdateReverseState(finish);
    EXPECT_EQ(finish, false);
}

/**
 * @tc.name: UpdateRemainTimeFractionTest001
 * @tc.desc: test results of UpdateRemainTimeFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateRemainTimeFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t time = 0;
    float fraction = 0.f;
    int remainTime = 1;
    rsAnimationFraction.UpdateRemainTimeFraction(fraction, remainTime);
    ASSERT_EQ(time, 0);
}

/**
 * @tc.name: UpdateRemainTimeFractionTest002
 * @tc.desc: test results of UpdateRemainTimeFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, UpdateRemainTimeFractionTest002, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t time = 0;
    float fraction = 1.f;
    int remainTime = 1;
    rsAnimationFraction.UpdateRemainTimeFraction(fraction, remainTime);
    ASSERT_EQ(time, 0);
}

/**
 * @tc.name: ResetFractionTimeTest001
 * @tc.desc: test results of ResetFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, ResetFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t lastFrameTime = 1;
    rsAnimationFraction.ResetFraction();
    ASSERT_NE(lastFrameTime, 0);
}

/**
 * @tc.name: GetRemainingRepeatCountTest001
 * @tc.desc: test results of GetRemainingRepeatCount
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetRemainingRepeatCountTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    rsAnimationFraction.SetRepeatCount(-1);
    int remainingRepeatCount = rsAnimationFraction.GetRemainingRepeatCount();
    EXPECT_EQ(remainingRepeatCount, -1);

    rsAnimationFraction.SetRepeatCount(0);
    rsAnimationFraction.ResetFraction();
    remainingRepeatCount = rsAnimationFraction.GetRemainingRepeatCount();
    EXPECT_EQ(remainingRepeatCount, 0);

    rsAnimationFraction.SetRepeatCount(1);
    rsAnimationFraction.ResetFraction();
    remainingRepeatCount = rsAnimationFraction.GetRemainingRepeatCount();
    EXPECT_EQ(remainingRepeatCount, 1);
}

/**
 * @tc.name: GetCurrentIsReverseCycleTest001
 * @tc.desc: test results of GetCurrentIsReverseCycle
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetCurrentIsReverseCycleTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    rsAnimationFraction.ResetFraction();
    bool currentIsReverseCycle = rsAnimationFraction.GetCurrentIsReverseCycle();
    EXPECT_TRUE(currentIsReverseCycle == false);
}

/**
 * @tc.name: SetRepeatCallbackEnableTest001
 * @tc.desc: test results of SetRepeatCallbackEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, SetRepeatCallbackEnableTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    bool isEnable = false;
    rsAnimationFraction.SetRepeatCallbackEnable(isEnable);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetRepeatCallbackEnableTest001
 * @tc.desc: test results of GetRepeatCallbackEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetRepeatCallbackEnableTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    rsAnimationFraction.SetRepeatCallbackEnable(false);
    bool res = rsAnimationFraction.GetRepeatCallbackEnable();
    EXPECT_EQ(res, false);
}
}