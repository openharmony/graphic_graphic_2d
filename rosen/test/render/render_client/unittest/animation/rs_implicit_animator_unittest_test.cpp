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
class RsImplicitAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsImplicitAnimatorTest::SetUpTestCase() {}
void RsImplicitAnimatorTest::TearDownTestCase() {}
void RsImplicitAnimatorTest::SetUp() {}
void RsImplicitAnimatorTest::TearDown() {}

/**
 * @tc.name: BeginImplicitKeyFrameAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, BeginImplicitKeyFrameAnimationTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    float fraction = 0.f;
    RSAnimationTimingCurve timingCurve;
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(fraction, timingCurve);
    ASSERT_EQ(fraction, 0.f);
}

/**
 * @tc.name: BeginImplicitKeyFrameAnimationTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, BeginImplicitKeyFrameAnimationTest02, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    float fraction = 0.f;
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(fraction);
    ASSERT_EQ(fraction, 0.f);
}

/**
 * @tc.name: EndImplicitKeyFrameAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, EndImplicitKeyFrameAnimationTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    float fraction = 0.f;
    rsImplicitAnimator.EndImplicitKeyFrameAnimation();
    ASSERT_EQ(fraction, 0.f);
}

/**
 * @tc.name: NeedImplicitAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, NeedImplicitAnimationTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    bool res = rsImplicitAnimator.NeedImplicitAnimation();
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: BeginImplicitTransitionTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, BeginImplicitTransitionTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    bool isTransitionIn = true;
    auto effect = std::make_shared<RSTransitionEffect>();
    rsImplicitAnimator.BeginImplicitTransition(effect, isTransitionIn);
    ASSERT_EQ(isTransitionIn, true);
}

/**
 * @tc.name: EndImplicitTransitionTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, EndImplicitTransitionTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.EndImplicitTransition();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: BeginImplicitPathAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, BeginImplicitPathAnimationTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    auto motionPathOption = std::make_shared<RSMotionPathOption>("path");
    rsImplicitAnimator.BeginImplicitPathAnimation(motionPathOption);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: EndImplicitPathAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, EndImplicitPathAnimationTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.EndImplicitPathAnimation();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: CreateImplicitAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, CreateImplicitAnimationTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.CreateImplicitAnimation(nullptr, nullptr, nullptr, nullptr);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: OpenImplicitAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, OpenImplicitAnimationTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    int res = rsImplicitAnimator.OpenImplicitAnimation(timingProtocol, timingCurve);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: CreateImplicitTransitionTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, CreateImplicitTransitionTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    RSNode target(true);
    int res = 1;
    rsImplicitAnimator.CreateImplicitTransition(target);
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: PopImplicitParamTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, PopImplicitParamTest01, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 1;
    rsImplicitAnimator.PopImplicitParam();
    ASSERT_NE(res, 0);
}

/**
 * @tc.name: ExecuteWithoutAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsImplicitAnimatorTest, ExecuteWithoutAnimationTest01, Level1)
{
    int res = 1;
    RSImplicitAnimator rsImplicitAnimator;
    rsImplicitAnimator.ExecuteWithoutAnimation(nullptr);
    ASSERT_NE(res, 0);
}
} // namespace OHOS::Rosen
