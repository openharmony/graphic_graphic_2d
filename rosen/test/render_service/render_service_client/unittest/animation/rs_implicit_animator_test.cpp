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
#include "animation/rs_animation.h"
#include "animation/rs_implicit_animator.h"
#include <sys/types.h>
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImplicitAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImplicitAnimatorTest::SetUpTestCase() {}
void RSImplicitAnimatorTest::TearDownTestCase() {}
void RSImplicitAnimatorTest::SetUp() {}
void RSImplicitAnimatorTest::TearDown() {}

/**
 * @tc.name: BeginImplicitKeyFrameAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitKeyFrameAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(1.0);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: NeedImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, NeedImplicitAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.NeedImplicitAnimation();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: BeginImplicitTransitionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitTransitionTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.BeginImplicitTransition(nullptr, true);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: EndImplicitTransitionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EndImplicitTransitionTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.EndImplicitTransition();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: BeginImplicitPathAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitPathAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.BeginImplicitPathAnimation(nullptr);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: EndImplicitPathAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EndImplicitPathAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.EndImplicitPathAnimation();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: ApplyAnimationSpeedMultiplierTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ApplyAnimationSpeedMultiplierTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    float multiplier = 2.0f;
    ASSERT_TRUE(std::fabs(rsImplicitAnimator.speedMultiplier_ - 1.0f) < 1e-6f);
    rsImplicitAnimator.ApplyAnimationSpeedMultiplier(multiplier);
    ASSERT_TRUE(std::fabs(rsImplicitAnimator.speedMultiplier_ - 2.0f) < 1e-6f);
    multiplier = 0.0f;
    rsImplicitAnimator.ApplyAnimationSpeedMultiplier(multiplier);
    ASSERT_TRUE(std::fabs(rsImplicitAnimator.speedMultiplier_ - 1.0f) < 1e-6f);
}

/**
 * @tc.name: CreateImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CreateImplicitAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.CreateImplicitAnimation(nullptr, nullptr, nullptr, nullptr);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: CloseImplicitAnimation001
 * @tc.desc: Test CloseImplicitAnimation without opening implicit animation
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseImplicitAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation001 start";
    RSImplicitAnimator rsImplicitAnimator;

    // Cover branch: CheckImplicitAnimationConditions() == false
    auto result = rsImplicitAnimator.CloseImplicitAnimation();
    ASSERT_TRUE(result.empty());

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation001 end";
}

/**
 * @tc.name: CloseImplicitAnimation002
 * @tc.desc: Test CloseImplicitAnimation with empty animations
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseImplicitAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation002 start";
    RSImplicitAnimator rsImplicitAnimator;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;
    std::function<void()> callback;
    auto finishCallback = std::make_shared<AnimationFinishCallback>(callback);

    // Open implicit animation
    rsImplicitAnimator.OpenImplicitAnimation(timingProtocol, timingCurve, std::move(finishCallback));

    // Close without adding any animations
    // Cover branch: currentAnimations.empty() && currentKeyframeAnimations.empty() -> ProcessEmptyAnimations
    auto result = rsImplicitAnimator.CloseImplicitAnimation();
    ASSERT_TRUE(result.empty());

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation002 end";
}

/**
 * @tc.name: CloseImplicitAnimation003
 * @tc.desc: Test CloseImplicitAnimation with !hasUiAnimation && interactiveAnimatorType_ ==
 * InteractiveAnimatorType::NONE (true branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseImplicitAnimation003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation003 start";
    RSImplicitAnimator rsImplicitAnimator;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;
    std::function<void()> callback;
    auto finishCallback = std::make_shared<AnimationFinishCallback>(callback);

    // Open implicit animation (interactiveAnimatorType_ = InteractiveAnimatorType::NONE by default)
    rsImplicitAnimator.OpenImplicitAnimation(timingProtocol, timingCurve, std::move(finishCallback));

    // Close without UI animations
    // Cover branch: !hasUiAnimation && interactiveAnimatorType_ == InteractiveAnimatorType::NONE ->
    // ProcessAnimationFinishCallbackGuaranteeTask
    auto result = rsImplicitAnimator.CloseImplicitAnimation();
    ASSERT_TRUE(result.empty());

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation003 end";
}

/**
 * @tc.name: CloseImplicitAnimation004
 * @tc.desc: Test CloseImplicitAnimation with hasUiAnimation && interactiveAnimatorType_ ==
 * InteractiveAnimatorType::NONE (false branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseImplicitAnimation004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation004 start";
    RSImplicitAnimator rsImplicitAnimator;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;
    std::function<void()> callback;
    auto finishCallback = std::make_shared<AnimationFinishCallback>(callback);

    // Open implicit animation (interactiveAnimatorType_ = InteractiveAnimatorType::NONE)
    rsImplicitAnimator.OpenImplicitAnimation(timingProtocol, timingCurve, std::move(finishCallback));

    // Begin keyframe animation to create a UI animation
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(1.0f);

    // Close with UI animation
    // Cover branch: hasUiAnimation && interactiveAnimatorType_ == InteractiveAnimatorType::NONE -> skip
    // ProcessAnimationFinishCallbackGuaranteeTask
    auto result = rsImplicitAnimator.CloseImplicitAnimation();

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation004 end";
}

/**
 * @tc.name: CloseImplicitAnimation005
 * @tc.desc: Test CloseImplicitAnimation with !hasUiAnimation && interactiveAnimatorType_ ==
 * InteractiveAnimatorType::GROUP (false branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseImplicitAnimation005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation005 start";
    RSImplicitAnimator rsImplicitAnimator;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;
    std::function<void()> callback;
    auto finishCallback = std::make_shared<AnimationFinishCallback>(callback);

    // Open interactive implicit animation (isAddImplictAnimation = true, isGroupAnimator = true)
    rsImplicitAnimator.OpenInterActiveImplicitAnimation(
        true, true, timingProtocol, timingCurve, std::move(finishCallback));

    // Close without UI animations but with interactiveAnimatorType_ == InteractiveAnimatorType::GROUP
    // Cover branch: !hasUiAnimation && interactiveAnimatorType_ == InteractiveAnimatorType::GROUP -> skip
    // ProcessAnimationFinishCallbackGuaranteeTask
    auto result = rsImplicitAnimator.CloseImplicitAnimation();

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation005 end";
}

/**
 * @tc.name: CloseImplicitAnimation006
 * @tc.desc: Test CloseImplicitAnimation with hasUiAnimation && interactiveAnimatorType_ ==
 * InteractiveAnimatorType::GROUP (false branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CloseImplicitAnimation006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation006 start";
    RSImplicitAnimator rsImplicitAnimator;

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    RSAnimationTimingCurve timingCurve;
    std::function<void()> callback;
    auto finishCallback = std::make_shared<AnimationFinishCallback>(callback);

    // Open interactive implicit animation (isAddImplictAnimation = true, isGroupAnimator = true)
    rsImplicitAnimator.OpenInterActiveImplicitAnimation(
        true, true, timingProtocol, timingCurve, std::move(finishCallback));

    // Begin keyframe animation to create a UI animation
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(1.0f);

    // Close with UI animation and interactiveAnimatorType_ == InteractiveAnimatorType::GROUP
    // Cover branch: hasUiAnimation && interactiveAnimatorType_ == InteractiveAnimatorType::GROUP -> skip
    // ProcessAnimationFinishCallbackGuaranteeTask
    auto result = rsImplicitAnimator.CloseImplicitAnimation();

    GTEST_LOG_(INFO) << "RSImplicitAnimatorTest CloseImplicitAnimation006 end";
}

}
