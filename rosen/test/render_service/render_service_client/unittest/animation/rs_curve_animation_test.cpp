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
#include "animation/rs_curve_animation.h"
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
class RSCurveAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCurveAnimationTest::SetUpTestCase() {}
void RSCurveAnimationTest::TearDownTestCase() {}
void RSCurveAnimationTest::SetUp() {}
void RSCurveAnimationTest::TearDown() {}

/**
 * @tc.name: StartRenderAnimationTest
 * @tc.desc: RemoveModifier test.
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, StartRenderAnimationTest, Level1)
{
    auto property = std::make_shared<RSProperty<float>>();
    auto byValue = std::make_shared<RSProperty<float>>();
    RSCurveAnimation rsCurveAnimation(property, byValue);
    auto animation = std::make_shared<RSRenderCurveAnimation>();
    rsCurveAnimation.StartInner(nullptr);
    rsCurveAnimation.StartRenderAnimation(animation);
    ASSERT_NE(animation, nullptr);
}
}