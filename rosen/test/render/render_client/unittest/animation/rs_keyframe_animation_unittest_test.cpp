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
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "modifier/rs_property.h"
#include <sys/types.h>
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsKeyFrameAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsKeyFrameAnimationTest::SetUpTestCase() {}
void RsKeyFrameAnimationTest::TearDownTestCase() {}
void RsKeyFrameAnimationTest::SetUp() {}
void RsKeyFrameAnimationTest::TearDown() {}

/**
 * @tc.name: AddKeyFrameTest01
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RsKeyFrameAnimationTest, AddKeyFrameTest01, Level1)
{
    auto value = std::make_shared<RSPropertyBase>();
    RSKeyframeAnimation rsKeyframeAnimation(value);
    RSAnimationTimingCurve timingCurve;
    rsKeyframeAnimation.AddKeyFrame(1.f, value, timingCurve);
    ASSERT_NE(value, nullptr);
}

/**
 * @tc.name: StartRenderAnimationTest01
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RsKeyFrameAnimationTest, StartRenderAnimationTest01, Level1)
{
    auto animation = std::make_shared<RSRenderKeyframeAnimation>();
    auto value = std::make_shared<RSPropertyBase>();
    RSKeyframeAnimation rsKeyframeAnimation(value);
    RSAnimationTimingCurve timingCurve;
    rsKeyframeAnimation.StartInner(nullptr);
    rsKeyframeAnimation.StartRenderAnimation(animation);
    ASSERT_NE(value, nullptr);
}

/**
 * @tc.name: StartUIAnimationTest01
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RsKeyFrameAnimationTest, StartUIAnimationTest01, Level1)
{
    auto animation = std::make_shared<RSRenderKeyframeAnimation>();
    auto value = std::make_shared<RSPropertyBase>();
    RSKeyframeAnimation rsKeyframeAnimation(value);
    RSAnimationTimingCurve timingCurve;
    rsKeyframeAnimation.StartUIAnimation(animation);
    ASSERT_NE(value, nullptr);
}
}