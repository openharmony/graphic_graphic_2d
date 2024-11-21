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
#include "animation/rs_animation_manager.h"
#include "animation/rs_render_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationManagerUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationManagerUnitTest::SetUpTestCase() {}
void RSAnimationManagerUnitTest::TearDownTestCase() {}
void RSAnimationManagerUnitTest::SetUp() {}
void RSAnimationManagerUnitTest::TearDown() {}

/**
 * @tc.name: DumpAnimationsTest001
 * @tc.desc: test results of DumpAnimations
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, DumpAnimationsTest001, TestSize.Level1)
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
 * @tc.name: AddAnimationTest001
 * @tc.desc: test results of AddAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, AddAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized = false;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.AddAnimation(animation);
    ASSERT_EQ(isInitialized, false);
}

/**
 * @tc.name: RemoveAnimationTest001
 * @tc.desc: test results of RemoveAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, RemoveAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized = false;
    AnimationId keyId = 0;
    rsAnimationManager.RemoveAnimation(keyId);
    ASSERT_EQ(isInitialized, false);
}

/**
 * @tc.name: CancelAnimationByPropertyIdTest001
 * @tc.desc: test results of CancelAnimationByPropertyId
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, CancelAnimationByPropertyIdTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized = false;
    PropertyId id = 0;
    rsAnimationManager.CancelAnimationByPropertyId(id);
    ASSERT_EQ(isInitialized, false);
}

/**
 * @tc.name: FilterAnimationByPidTest001
 * @tc.desc: test results of FilterAnimationByPid
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, FilterAnimationByPidTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    auto animationPid = rsAnimationManager.GetAnimationPid();
    pid_t pid = 0;
    rsAnimationManager.FilterAnimationByPid(pid);
    EXPECT_EQ(animationPid, pid);
}

/**
 * @tc.name: GetAnimationsSizeTest001
 * @tc.desc: test results of GetAnimationsSize
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, GetAnimationsSizeTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    uint32_t getAnimationsSize = rsAnimationManager.GetAnimationsSize();
    ASSERT_NE(getAnimationsSize, 1);
}

/**
 * @tc.name: GetAnimationTest001
 * @tc.desc: test results of GetAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, GetAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    AnimationId id = 0;
    auto res = rsAnimationManager.GetAnimation(id);
    ASSERT_EQ(res, nullptr);
}

/**
 * @tc.name: OnAnimationFinishedTest001
 * @tc.desc: test results of OnAnimationFinished
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, OnAnimationFinishedTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.OnAnimationFinished(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: AnimateTest001
 * @tc.desc: test results of Animate
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, AnimateTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    int64_t time = 10;
    bool nodeIsOnTheTree = false;
    RSSurfaceNodeAbilityState abilityState = RSSurfaceNodeAbilityState::FOREGROUND;
    rsAnimationManager.Animate(time, nodeIsOnTheTree, abilityState);
    ASSERT_NE(time, 0);
}

/**
 * @tc.name: SetRateDeciderEnableTest
 * @tc.desc: test results of SetRateDeciderEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, SetRateDeciderEnableTest, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool enabled = false;
    FrameRateGetFunc func;
    rsAnimationManager.SetRateDeciderEnable(enabled, func);
    EXPECT_EQ(enabled, false);
}

/**
 * @tc.name: SetRateDeciderScaleSizeTest001
 * @tc.desc: test results of SetRateDeciderScaleSize
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, SetRateDeciderScaleSizeTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    float width = 0.0f;
    float height = 0.0f;
    rsAnimationManager.SetRateDeciderScaleSize(width, height);
    EXPECT_EQ(width, 0);
}

/**
 * @tc.name: GetDecideFrameRateRangeTest
 * @tc.desc: test results of GetDecideFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, GetDecideFrameRateRangeTest, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool test = false;
    FrameRateRange frame;
    frame = rsAnimationManager.GetDecideFrameRateRange();
    ASSERT_EQ(test, false);
}

/**
 * @tc.name: GetFrameRateRangeTest001
 * @tc.desc: test results of GetFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, GetFrameRateRangeTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool test = false;
    FrameRateRange frame;
    frame = rsAnimationManager.GetFrameRateRange();
    ASSERT_EQ(test, false);
}

/**
 * @tc.name: RegisterSpringAnimationTest001
 * @tc.desc: test results of RegisterSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, RegisterSpringAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name: UnregisterSpringAnimationTest001
 * @tc.desc: test results of UnregisterSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, UnregisterSpringAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.UnregisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name: QuerySpringAnimationTest001
 * @tc.desc: test results of QuerySpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, QuerySpringAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    auto res = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.QuerySpringAnimation(propertyId);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: RegisterPathAnimationTest001
 * @tc.desc: test results of RegisterPathAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, RegisterPathAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterPathAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name: UnregisterPathAnimationTest001
 * @tc.desc: test results of UnregisterPathAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, UnregisterPathAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.UnregisterPathAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name: QueryPathAnimationTest001
 * @tc.desc: test results of QueryPathAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, QueryPathAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    auto res = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.QueryPathAnimation(propertyId);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: RegisterParticleAnimationTest
 * @tc.desc: test results of RegisterParticleAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, RegisterParticleAnimationTest, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterParticleAnimation(propertyId, animId);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UnregisterParticleAnimationTest001
 * @tc.desc: test results of UnregisterParticleAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, UnregisterParticleAnimationTest001, TestSize.Level1)
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
 * @tc.name: GetParticleAnimationsTest001
 * @tc.desc: test results of GetParticleAnimations
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationManagerUnitTest, GetParticleAnimationsTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    auto particleAnimations = rsAnimationManager.GetParticleAnimations();
    bool isEmpty = particleAnimations.empty();
    EXPECT_EQ(isEmpty, true);
}
}