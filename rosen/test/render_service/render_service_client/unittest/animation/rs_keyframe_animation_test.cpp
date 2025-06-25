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
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "modifier/rs_property.h"
#include <sys/types.h>
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSKeyframeAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSKeyframeAnimationTest::SetUpTestCase() {}
void RSKeyframeAnimationTest::TearDownTestCase() {}
void RSKeyframeAnimationTest::SetUp() {}
void RSKeyframeAnimationTest::TearDown() {}

/**
 * @tc.name: AddKeyFrameTest001
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, AddKeyFrameTest001, Level1)
{
    auto value = std::make_shared<RSProperty<float>>();
    RSKeyframeAnimation rsKeyframeAnimation(value);
    RSAnimationTimingCurve timingCurve;
    rsKeyframeAnimation.AddKeyFrame(1.f, value, timingCurve);
    ASSERT_NE(value, nullptr);
}

/**
 * @tc.name: StartRenderAnimationTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, StartRenderAnimationTest, Level1)
{
    auto value = std::make_shared<RSProperty<float>>();
    auto animation = std::make_shared<RSRenderKeyframeAnimation>();
    RSKeyframeAnimation rsKeyframeAnimation(value);
    RSAnimationTimingCurve timingCurve;
    rsKeyframeAnimation.StartInner(nullptr);
    rsKeyframeAnimation.StartRenderAnimation(animation);
    ASSERT_NE(value, nullptr);
}

/**
 * @tc.name: StartUIAnimationTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, StartUIAnimationTest, Level1)
{
    auto value = std::make_shared<RSProperty<float>>();
    auto animation = std::make_shared<RSRenderKeyframeAnimation>();
    RSKeyframeAnimation rsKeyframeAnimation(value);
    RSAnimationTimingCurve timingCurve;
    rsKeyframeAnimation.StartUIAnimation(animation);
    ASSERT_NE(value, nullptr);
}
}