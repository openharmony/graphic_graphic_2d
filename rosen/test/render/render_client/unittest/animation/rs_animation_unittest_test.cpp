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
class RsAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsAnimationTest::SetUpTestCase() {}
void RsAnimationTest::TearDownTestCase() {}
void RsAnimationTest::SetUp() {}
void RsAnimationTest::TearDown() {}

/**
 * @tc.name: SetFinishCallbackTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, SetFinishCallbackTest01, Level1)
{
    auto finishCallback = std::function<void()>();
    RSAnimation rsAnimation;
    rsAnimation.SetFinishCallback(finishCallback);
    ASSERT_EQ(finishCallback, nullptr);
}

/**
 * @tc.name: StartTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, StartTest01, Level1)
{
    auto target = std::shared_ptr<RSNode>();
    RSAnimation rsAnimation;
    rsAnimation.Start(target);
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: StartTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, StartTest02, Level1)
{
    auto per = std::make_shared<RSNode>(true);
    RSAnimation rsAnimation;
    rsAnimation.StartInner(per);
    rsAnimation.Start(per);
    ASSERT_NE(per, nullptr);
}

/**
 * @tc.name: StartInnerTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, StartInnerTest01, Level1)
{
    auto target = std::shared_ptr<RSNode>();
    RSAnimation rsAnimation;
    rsAnimation.StartInner(target);
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: OnPauseTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, OnPauseTest01, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.OnPause();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: ResumeTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, ResumeTest01, Level1)
{
    auto target = std::shared_ptr<RSNode>();
    RSAnimation rsAnimation;
    rsAnimation.Resume();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: ResumeTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, ResumeTest02, Level1)
{
    auto per = std::make_shared<RSNode>(true);
    RSAnimation rsAnimation;
    rsAnimation.StartInner(per);
    rsAnimation.Resume();
    ASSERT_NE(per, nullptr);
}

/**
 * @tc.name: OnResumeTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, OnResumeTest01, Level1)
{
    auto target = std::shared_ptr<RSNode>();
    RSAnimation rsAnimation;
    rsAnimation.OnResume();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: FinishTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, FinishTest01, Level1)
{
    auto target = std::shared_ptr<RSNode>();
    RSAnimation rsAnimation;
    rsAnimation.Finish();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: FinishTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, FinishTest02, Level1)
{
    RSAnimation rsAnimation;
    auto per = std::make_shared<RSNode>(true);
    rsAnimation.CallFinishCallback();
    rsAnimation.Finish();
    ASSERT_NE(per, nullptr);
}

/**
 * @tc.name: OnFinishTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, OnFinishTest01, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.OnFinish();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: ReverseTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, ReverseTest01, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.Reverse();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: ReverseTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, ReverseTest02, Level1)
{
    RSAnimation rsAnimation;
    auto per = std::make_shared<RSNode>(true);
    rsAnimation.CallFinishCallback();
    rsAnimation.Reverse();
    ASSERT_NE(per, nullptr);
}

/**
 * @tc.name: OnReverseTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, OnReverseTest01, Level1)
{
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.OnReverse();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: SetFractionTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, SetFractionTest01, Level1)
{
    float frac = 3.f;
    RSAnimation rsAnimation;
    auto target = std::shared_ptr<RSNode>();
    rsAnimation.SetFraction(frac);
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: SetFractionTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, SetFractionTest02, Level1)
{
    float frac = 0.0f;
    RSAnimation rsAnimation;
    auto per = std::make_shared<RSNode>(true);
    rsAnimation.CallFinishCallback();
    rsAnimation.SetFraction(frac);
    ASSERT_NE(per, nullptr);
}

/**
 * @tc.name: UpdateParamToRenderAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, UpdateParamToRenderAnimationTest01, Level1)
{
    auto animation = std::make_shared<RSRenderAnimation>();
    RSAnimation rsAnimation;
    rsAnimation.UpdateParamToRenderAnimation(animation);
    ASSERT_NE(animation, nullptr);
}

/**
 * @tc.name: StartCustomAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, StartCustomAnimationTest01, Level1)
{
    auto animation = std::make_shared<RSRenderAnimation>();
    RSAnimation rsAnimation;
    rsAnimation.StartCustomAnimation(animation);
    ASSERT_NE(animation, nullptr);
}

/**
 * @tc.name: PauseTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, PauseTest01, Level1)
{
    auto target = std::shared_ptr<RSNode>();
    RSAnimation rsAnimation;
    rsAnimation.Pause();
    ASSERT_EQ(target, nullptr);
}

/**
 * @tc.name: PauseTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTest, PauseTest02, Level1)
{
    auto per = std::make_shared<RSNode>(true);
    RSAnimation rsAnimation;
    rsAnimation.StartInner(per);
    rsAnimation.Pause();
    ASSERT_NE(per, nullptr);
}
}