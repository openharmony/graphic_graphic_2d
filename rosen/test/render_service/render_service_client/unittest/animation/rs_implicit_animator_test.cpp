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
#include "animation/rs_animation_callback.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_transition_effect.h"
#include "ui/rs_node.h"
#include <sys/types.h>
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
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
 * @tc.name: BeginImplicitKeyFrameAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitKeyFrameAnimationTest001, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    float fraction = 0.f;
    RSAnimationTimingCurve timingCurve;
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(fraction, timingCurve);
    ASSERT_EQ(fraction, 0.f);
}

/**
 * @tc.name: BeginImplicitKeyFrameAnimationTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitKeyFrameAnimationTest002, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    float fraction = 0.f;
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(fraction);
    ASSERT_EQ(fraction, 0.f);
}

/**
 * @tc.name: EndImplicitKeyFrameAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, EndImplicitKeyFrameAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    float fraction = 0.f;
    rsImplicitAnimator.EndImplicitKeyFrameAnimation();
    ASSERT_EQ(fraction, 0.f);
}

/**
 * @tc.name: NeedImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, NeedImplicitAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    bool res = rsImplicitAnimator.NeedImplicitAnimation();
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: BeginImplicitTransitionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitTransitionTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    bool isTransitionIn = true;
    auto effect = std::make_shared<RSTransitionEffect>();
    rsImplicitAnimator.BeginImplicitTransition(effect, isTransitionIn);
    ASSERT_EQ(isTransitionIn, true);
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
    ASSERT_EQ(res, 0);
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
    auto motionPathOption = std::make_shared<RSMotionPathOption>("path");
    rsImplicitAnimator.BeginImplicitPathAnimation(motionPathOption);
    ASSERT_EQ(res, 0);
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
    ASSERT_EQ(res, 0);
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
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: OpenImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, OpenImplicitAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    int res = rsImplicitAnimator.OpenImplicitAnimation(timingProtocol, timingCurve);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: CreateImplicitTransitionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, CreateImplicitTransitionTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    RSNode target(true);
    int res = 1;
    rsImplicitAnimator.CreateImplicitTransition(target);
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: PopImplicitParamTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, PopImplicitParamTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 1;
    rsImplicitAnimator.PopImplicitParam();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: ExecuteWithoutAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, ExecuteWithoutAnimationTest, Level1)
{
    int res = 1;
    RSImplicitAnimator rsImplicitAnimator;
    rsImplicitAnimator.ExecuteWithoutAnimation(nullptr);
    ASSERT_NE(res, 0);
}
} // namespace OHOS::Rosen
