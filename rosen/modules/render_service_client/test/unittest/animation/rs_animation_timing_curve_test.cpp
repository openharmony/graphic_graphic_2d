/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_curve_animation.h"
#include "include/animation/rs_steps_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSAnimationTimingCurveTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: CreateSpringCurve001
 * @tc.desc: Verify the CreateSpringCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringCurve001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringTest001 start";
    float velocity = 0.5f;
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass, stiffness, damping);
    auto timingCurve_ptr = std::make_shared<RSAnimationTimingCurve>(timingCurve);
    EXPECT_TRUE(timingCurve_ptr != nullptr);

    float mass1 = 0.0f;
    float stiffness1 = 1.0f;
    auto timingCurve1 = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass1, stiffness1, damping);
    auto timingCurve_ptr1 = std::make_shared<RSAnimationTimingCurve>(timingCurve);
    EXPECT_TRUE(timingCurve_ptr1 != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringCurve001 end";
}

/**
 * @tc.name: CreateInterpolatingSpring001
 * @tc.desc: Verify the CreateInterpolatingSpring of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateInterpolatingSpring001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateInterpolatingSpring001 start";
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    float velocity = 1.0f;
    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(mass, stiffness, damping, velocity);
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    float mass1 = 0.5f;
    float stiffness1 = 0.5f;
    auto timingCurve1 = RSAnimationTimingCurve::CreateInterpolatingSpring(mass1, stiffness1, damping, velocity);
    EXPECT_TRUE(timingCurve1.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    float mass2 = -1.0f;
    float stiffness2 = 0.5f;
    auto timingCurve2 = RSAnimationTimingCurve::CreateInterpolatingSpring(mass2, stiffness2, damping, velocity);
    EXPECT_TRUE(timingCurve2.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateInterpolatingSpring001 end";
}
} // namespace Rosen
} // namespace OHOS