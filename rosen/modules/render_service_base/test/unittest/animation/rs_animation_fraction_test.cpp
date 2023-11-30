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
    bool isFinished = false;
    bool isRepeatFinished = false;
    float result = 0.0f;
    RSAnimationFraction fraction;
    fraction.SetDuration(0);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(0);
    EXPECT_EQ(result, 1.0f);

    fraction.SetDuration(300);
    fraction.SetRepeatCount(0);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(0);
    EXPECT_EQ(result, 1.0f);

    fraction.SetRepeatCount(1);
    RSAnimationFraction::OnAnimationScaleChangedCallback("persist.sys.graphic.animationscale", "0", nullptr);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);

    fraction.SetRepeatCount(1);
    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    std::tie(result, isDelay, isFinished, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);

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
    bool isFinishedFalse = false;
    bool isFinishedTrue = true;
    bool isRepeatFinished = false;
    float result = 0.0f;
    fraction.SetDuration(300);
    fraction.SetRepeatCount(1);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);

    fraction.SetDirection(false);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);

    fraction.SetDirectionAfterStart(ForwardDirection::REVERSE);
    fraction.SetDirection(true);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);

    fraction.SetDirection(false);
    std::tie(result, isDelay, isFinishedFalse, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);
    std::tie(result, isDelay, isFinishedTrue, isRepeatFinished) = fraction.GetAnimationFraction(100);
    EXPECT_TRUE(result != 0.0f);

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
    GTEST_LOG_(INFO) << "RSAnimationFractionTest AnimationScaleChangedCallback001 end";
}
} // namespace Rosen
} // namespace OHOS