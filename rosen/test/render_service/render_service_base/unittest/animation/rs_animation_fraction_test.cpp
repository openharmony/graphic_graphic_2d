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
 * @tc.desc:
 * @tc.type:FUNC
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
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetAnimationScaleTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetAnimationScale();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: SetAnimationScaleTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, SetAnimationScaleTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float animationScale = 0.f;
    rsAnimationFraction.SetAnimationScale(animationScale);
    ASSERT_EQ(animationScale, 0);
}

/**
 * @tc.name: GetAnimationFractionTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetAnimationFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t time = 0;
    rsAnimationFraction.GetAnimationFraction(time);
    ASSERT_EQ(time, 0);
}

/**
 * @tc.name: UpdateRemainTimeFractionTest001
 * @tc.desc:
 * @tc.type:FUNC
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
 * @tc.desc:
 * @tc.type:FUNC
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
 * @tc.name: GetStartFractionTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetStartFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetStartFraction();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: GetEndFractionTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetEndFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    float res = rsAnimationFraction.GetEndFraction();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: SetDirectionAfterStartTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, SetDirectionAfterStartTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    ForwardDirection direction = ForwardDirection::NORMAL;
    int res = 1;
    rsAnimationFraction.SetDirectionAfterStart(direction);
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: SetLastFrameTimeTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, SetLastFrameTimeTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t lastFrameTime = 1;
    rsAnimationFraction.SetLastFrameTime(lastFrameTime);
    ASSERT_NE(lastFrameTime, 0);
}

/**
 * @tc.name: GetLastFrameTimeTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, GetLastFrameTimeTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t res = rsAnimationFraction.GetLastFrameTime();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: ResetFractionTimeTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationFractionTest, ResetFractionTest001, TestSize.Level1)
{
    RSAnimationFraction rsAnimationFraction;
    int64_t lastFrameTime = 1;
    rsAnimationFraction.ResetFraction();
    ASSERT_NE(lastFrameTime, 0);
}
}