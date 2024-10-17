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
#include "animation/rs_animation_fraction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimatFractionSingleTest : public testing::SingleTest {
public:
    static void SetUpSingleTestCase();
    static void TearDownSingleTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimatFractionSingleTest::SetUpSingleTestCase() {}
void RSAnimatFractionSingleTest::TearDownSingleTestCase() {}
void RSAnimatFractionSingleTest::SetUp() {}
void RSAnimatFractionSingleTest::TearDown() {}

/**
 * @tc.name: GetAnimationScaleSingleTest001
 * @tc.desc: test results of GetAnimationScale
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetAnimationScaleSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetAnimationScale();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: InitSingleTest001
 * @tc.desc: test results of Init
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, InitSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    bool isInitialized = false;
    rsAnimationFraction.Init();
    ASSERT_EQ(isInitialized, false);
}

/**
 * @tc.name: OnAnimationScaleChangedCallbackSingleTest001
 * @tc.desc: test results of OnAnimationScaleChangedCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, OnAnimationScaleChangedCallbackSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    rsAnimationFraction.OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "1", nullptr);
    rsAnimationFraction.OnAnimationScaleChangedCallback("persist", "1", nullptr);
    EXPECT_EQ(rsAnimationFraction.GetRepeatCallbackEnable(), false);
}

/**
 * @tc.name: SetAnimationScaleSingleTest001
 * @tc.desc: test results of SetAnimationScale
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, SetAnimationScaleSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    float animationScale = 0.f;
    rsAnimationFraction.SetAnimationScale(animationScale);
    ASSERT_EQ(animationScale, 0);
}

/**
 * @tc.name: SetLastFrameTimeSingleTest001
 * @tc.desc: test results of SetLastFrameTime
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, SetLastFrameTimeSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    int64_t lastFrameTime = 0;
    rsAnimationFraction.SetLastFrameTime(lastFrameTime);
    int64_t res = rsAnimationFraction.GetLastFrameTime();
    EXPECT_EQ(lastFrameTime, res);
}

/**
 * @tc.name: SetDirectionAfterStartSingleTest001
 * @tc.desc: test results of SetDirectionAfterStart
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, SetDirectionAfterStartSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    ForwardDirection direction = ForwardDirection::NORMAL;
    rsAnimationFraction.SetDirectionAfterStart(direction);
    EXPECT_EQ(direction, ForwardDirection::NORMAL);
}

/**
 * @tc.name: IsStartRunningSingleTest001
 * @tc.desc: test results of IsStartRunning
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, IsStartRunningSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
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
 * @tc.name: GetAnimationFractionSingleTest001
 * @tc.desc: test results of GetAnimationFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetAnimationFractionSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    int64_t time = 0;
    rsAnimationFraction.GetAnimationFraction(time);
    ASSERT_EQ(time, 0);
}

/**
 * @tc.name: GetLastFrameTimeSingleTest001
 * @tc.desc: test results of GetLastFrameTime
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetLastFrameTimeSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    int64_t res = rsAnimationFraction.GetLastFrameTime();
    EXPECT_NE(res, 1);
}

/**
 * @tc.name: IsInRepeatSingleTest001
 * @tc.desc: test results of IsInRepeat
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, IsInRepeatSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    rsAnimationFraction.SetRepeatCallbackEnable(true);
    rsAnimationFraction.SetRepeatCount(-1);
    bool isInRepeat = rsAnimationFraction.IsInRepeat();
    EXPECT_EQ(isInRepeat, true);

    rsAnimationFraction.SetRepeatCallbackEnable(false);
    isInRepeat = rsAnimationFraction.IsInRepeat();
    EXPECT_EQ(isInRepeat, false);
}

/**
 * @tc.name: GetStartFractionSingleTest001
 * @tc.desc: test results of GetStartFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetStartFractionSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetStartFraction();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: IsFinishedSingleTest001
 * @tc.desc: test results of IsFinished
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, IsFinishedSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
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
 * @tc.name: GetEndFractionSingleTest001
 * @tc.desc: test results of GetEndFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetEndFractionSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetEndFraction();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: UpdateRemainTimeFractionSingleTest001
 * @tc.desc: test results of UpdateRemainTimeFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, UpdateRemainTimeFractionSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    int64_t time = 0;
    float fraction = 0.f;
    int remainTime = 1;
    rsAnimationFraction.UpdateRemainTimeFraction(fraction, remainTime);
    ASSERT_EQ(time, 0);
}

/**
 * @tc.name: UpdateReverseStateSingleTest001
 * @tc.desc: test results of UpdateReverseState
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, UpdateReverseStateSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
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
 * @tc.name: UpdateRemainTimeFractionSingleTest002
 * @tc.desc: test results of UpdateRemainTimeFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, UpdateRemainTimeFractionSingleTest002, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    int64_t time = 0;
    float fraction = 1.f;
    int remainTime = 1;
    rsAnimationFraction.UpdateRemainTimeFraction(fraction, remainTime);
    ASSERT_EQ(time, 0);
}

/**
 * @tc.name: GetRemainingRepeatCountSingleTest001
 * @tc.desc: test results of GetRemainingRepeatCount
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetRemainingRepeatCountSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
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
 * @tc.name: ResetFractionTimeSingleTest001
 * @tc.desc: test results of ResetFraction
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, ResetFractionSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    int64_t lastFrameTime = 1;
    rsAnimationFraction.ResetFraction();
    ASSERT_NE(lastFrameTime, 0);
}

/**
 * @tc.name: SetRepeatCallbackEnableSingleTest001
 * @tc.desc: test results of SetRepeatCallbackEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, SetRepeatCallbackEnableSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    bool isEnable = false;
    rsAnimationFraction.SetRepeatCallbackEnable(isEnable);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetRepeatCallbackEnableSingleTest001
 * @tc.desc: test results of GetRepeatCallbackEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetRepeatCallbackEnableSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    rsAnimationFraction.SetRepeatCallbackEnable(false);
    bool res = rsAnimationFraction.GetRepeatCallbackEnable();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetCurrentIsReverseCycleSingleTest001
 * @tc.desc: test results of GetCurrentIsReverseCycle
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatFractionSingleTest, GetCurrentIsReverseCycleSingleTest001, TestSize.Level1)
{
    RSAnimatFraction rsAnimationFraction;
    rsAnimationFraction.ResetFraction();
    bool currentIsReverseCycle = rsAnimationFraction.GetCurrentIsReverseCycle();
    EXPECT_TRUE(currentIsReverseCycle == false);
}
}