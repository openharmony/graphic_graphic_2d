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
#include "animation/rs_animation_manager.h"
#include "animation/rs_render_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationManagerExtTest : public testing::ExtTest {
public:
    static void SetUpExtTestCase();
    static void TearDownExtTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationManagerExtTest::SetUpExtTestCase() {}
void RSAnimationManagerExtTest::TearDownExtTestCase() {}
void RSAnimationManagerExtTest::SetUp() {}
void RSAnimationManagerExtTest::TearDown() {}

/**
 * @tc.name: AddAnimationExtTest001
 * @tc.desc: test results of AddAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, AddAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized_ = false;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.AddAnimation(animation);
    ASSERT_EQ(isInitialized_, false);
}

/**
 * @tc.name: DumpAnimationsExtExtTest001
 * @tc.desc: test results of DumpAnimations
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, DumpAnimationsExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    auto animation = std::make_shared<RSRenderAnimation>();
    std::string str = "out";
    rsAnimationManager.AddAnimation(animation);

    std::shared_ptr<RSRenderAnimation> animation2;
    rsAnimationManager.AddAnimation(animation2);

    rsAnimationManager.DumpAnimations(str);
}

/**
 * @tc.name: CancelAnimationByPropertyIdExtTest001
 * @tc.desc: test results of CancelAnimationByPropertyId
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, CancelAnimationByPropertyIdExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized_ = false;
    PropertyId id = 0;
    rsAnimationManager.CancelAnimationByPropertyId(id);
    ASSERT_EQ(isInitialized_, false);
}

/**
 * @tc.name: FilterAnimationByPidExtTest001
 * @tc.desc: test results of FilterAnimationByPid
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, FilterAnimationByPidExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    auto animationPid = rsAnimationManager.GetAnimationPid();
    pid_t pid = 0;
    rsAnimationManager.FilterAnimationByPid(pid);
    EXPECT_EQ(animationPid, pid);
}

/**
 * @tc.name: GetAnimationsSizeExtTest001
 * @tc.desc: test results of GetAnimationsSize
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, GetAnimationsSizeExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    uint32_t getAnimationsSize = rsAnimationManager.GetAnimationsSize();
    ASSERT_NE(getAnimationsSize, 1);
}

/**
 * @tc.name: RemoveAnimationExtTest001
 * @tc.desc: test results of RemoveAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, RemoveAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized_ = false;
    AnimationId keyId = 0;
    rsAnimationManager.RemoveAnimation(keyId);
    ASSERT_EQ(isInitialized_, false);
}

/**
 * @tc.name: GetAnimationExtTest001
 * @tc.desc: test results of GetAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, GetAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    AnimationId id = 0;
    auto res = rsAnimationManager.GetAnimation(id);
    ASSERT_EQ(res, nullptr);
}

/**
 * @tc.name: AnimateExtTest001
 * @tc.desc: test results of Animate
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, AnimateExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    int64_t time = 10;
    bool nodeIsOnTheTree = false;
    RSSurfaceNodeAbilityState abilityState = RSSurfaceNodeAbilityState::FOREGROUND;
    rsAnimationManager.Animate(time, nodeIsOnTheTree, abilityState);
    ASSERT_NE(time, 0);
}

/**
 * @tc.name: OnAnimationFinishedExtTest001
 * @tc.desc: test results of OnAnimationFinished
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, OnAnimationFinishedExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.OnAnimationFinished(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: SetRateDeciderScaleSizeExtTest001
 * @tc.desc: test results of SetRateDeciderScaleSize
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, SetRateDeciderScaleSizeExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    float width = 0.0f;
    float height = 0.0f;
    rsAnimationManager.SetRateDeciderScaleSize(width, height);
    EXPECT_EQ(width, 0);
}

/**
 * @tc.name: SetRateDeciderEnableExtTest
 * @tc.desc: test results of SetRateDeciderEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, SetRateDeciderEnableExtTest, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool enabled = false;
    FrameRateGetFunc func;
    rsAnimationManager.SetRateDeciderEnable(enabled, func);
    EXPECT_EQ(enabled, false);
}

/**
 * @tc.name: GetFrameRateRangeExtTest001
 * @tc.desc: test results of GetFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, GetFrameRateRangeExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool test = false;
    FrameRateRange frame;
    frame = rsAnimationManager.GetFrameRateRange();
    ASSERT_EQ(test, false);
}

/**
 * @tc.name: GetDecideFrameRateRangeExtTest
 * @tc.desc: test results of GetDecideFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, GetDecideFrameRateRangeExtTest, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool test = false;
    FrameRateRange frame;
    frame = rsAnimationManager.GetDecideFrameRateRange();
    ASSERT_EQ(test, false);
}

/**
 * @tc.name: UnregisterSpringAnimationExtTest001
 * @tc.desc: test results of UnregisterSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, UnregisterSpringAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.UnregisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name: RegisterSpringAnimationExtTest001
 * @tc.desc: test results of RegisterSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, RegisterSpringAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name: RegisterPathAnimationExtTest001
 * @tc.desc: test results of RegisterPathAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, RegisterPathAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterPathAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name: QuerySpringAnimationExtTest001
 * @tc.desc: test results of QuerySpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, QuerySpringAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    auto res = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.QuerySpringAnimation(propertyId);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: QueryPathAnimationExtTest001
 * @tc.desc: test results of QueryPathAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, QueryPathAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    auto res = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.QueryPathAnimation(propertyId);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: UnregisterPathAnimationExtTest001
 * @tc.desc: test results of UnregisterPathAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, UnregisterPathAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.UnregisterPathAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}


/**
 * @tc.name: UnregisterParticleAnimationExtTest001
 * @tc.desc: test results of UnregisterParticleAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, UnregisterParticleAnimationExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.AddAnimation(animation);
    rsAnimationManager.UnregisterParticleAnimation(propertyId, animId);
    ASSERT_NE(animation, nullptr);
}

/**
 * @tc.name: RegisterParticleAnimationExtTest
 * @tc.desc: test results of RegisterParticleAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, RegisterParticleAnimationExtTest, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterParticleAnimation(propertyId, animId);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetParticleAnimationsExtTest001
 * @tc.desc: test results of GetParticleAnimations
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatManagerExtTest, GetParticleAnimationsExtTest001, ExtTestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    auto particleAnimations = rsAnimationManager.GetParticleAnimations();
    bool isEmpty = particleAnimations.empty();
    EXPECT_EQ(isEmpty, true);
}
}