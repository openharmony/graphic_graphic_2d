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
#include "ui/rs_node.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationTest::SetUpTestCase() {}
void RSAnimationTest::TearDownTestCase() {}
void RSAnimationTest::SetUp() {}
void RSAnimationTest::TearDown() {}

/**
 * @tc.name: SetFinishCallbackTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, SetFinishCallbackTest, Level1)
{
    RSAnimation rsAnimation;
    auto finishCallback = std::function<void()>();
    rsAnimation.SetFinishCallback(finishCallback);
    ASSERT_EQ(finishCallback, nullptr);
}

/**
 * @tc.name: StartTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, StartTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.Start(target);
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: StartInnerTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, StartInnerTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.StartInner(target);
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: OnPauseTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, OnPauseTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.OnPause();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: ResumeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, ResumeTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.Resume();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: OnResumeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, OnResumeTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.OnResume();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: FinishTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, FinishTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.Finish();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: OnFinishTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, OnFinishTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.OnFinish();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: ReverseTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, ReverseTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.Reverse();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: OnReverseTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, OnReverseTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.OnReverse();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: SetFractionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, SetFractionTest, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    float fraction = 0.f;
    rsAnimation.SetFraction(fraction);
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: UpdateParamToRenderAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, UpdateParamToRenderAnimationTest, Level1)
{
    RSAnimation rsAnimation;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimation.UpdateParamToRenderAnimation(animation);
    ASSERT_NE(animation, nullptr);
}

/**
 * @tc.name: StartCustomAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationTest, StartCustomAnimationTest, Level1)
{
    RSAnimation rsAnimation;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimation.StartCustomAnimation(animation);
    ASSERT_NE(animation, nullptr);
}
}