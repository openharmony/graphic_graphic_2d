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
class RsAnimationTimingCurTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsAnimationTimingCurTest::SetUpTestCase() {}
void RsAnimationTimingCurTest::TearDownTestCase() {}
void RsAnimationTimingCurTest::SetUp() {}
void RsAnimationTimingCurTest::TearDown() {}

/**
 * @tc.name: CreateSpringCurveTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTimingCurTest, CreateSpringCurveTest01, Level1)
{
    auto customCurveFunc = std::function<float(float)>();
    RSAnimationTimingCurve rsAnimationTimingCurve(customCurveFunc);
    float mass = -1.0f;
    float stiffness = -1.0f;
    float velocity = 0.0f;
    float damping = 0.0f;
    rsAnimationTimingCurve.CreateSpringCurve(velocity, mass, stiffness, damping);
    ASSERT_EQ(velocity, 0.0f);
}

/**
 * @tc.name: CreateInterpolatingSpringTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationTimingCurTest, CreateInterpolatingSpringTest01, Level1)
{
    auto customCurveFunc = std::function<float(float)>();
    RSAnimationTimingCurve rsAnimationTimingCurve(customCurveFunc);
    float stiffness = 0.0f;
    float damping = 0.0f;
    float velocity = -1.0f;
    float mass = -1.0f;
    rsAnimationTimingCurve.CreateInterpolatingSpring(velocity, mass, stiffness, damping);
    ASSERT_EQ(velocity, -1.0f);
}
}