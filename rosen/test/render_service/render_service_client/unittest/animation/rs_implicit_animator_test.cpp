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
#include "animation/rs_implicit_animator.h"
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
 * @tc.name: BeginImplicitKeyFrameAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimatorTest, BeginImplicitKeyFrameAnimationTest, Level1)
{
    RSImplicitAnimator rsImplicitAnimator;
    int res = 0;
    rsImplicitAnimator.BeginImplicitKeyFrameAnimation(1.0);
    ASSERT_EQ(res, 0);
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
    ASSERT_EQ(res, 0);
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
    ASSERT_EQ(res, 0);
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
    rsImplicitAnimator.BeginImplicitPathAnimation(nullptr);
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
}