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

#include "gtest/gtest.h"

#include <cmath>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_curve_animation.h"
#include "include/animation/rs_steps_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAnimationTimingCurveTest : public testing::Test {
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
/**
 * @tc.name: CreateCubicCurveNaNInf001
 * @tc.desc: Verify CreateCubicCurve NaN/Inf fallback to defaults
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveNaNInf001, TestSize.Level1)
{
    auto curve1 = RSAnimationTimingCurve::CreateCubicCurve(NAN, 0.0f, 1.0f, 1.0f);
    EXPECT_TRUE(curve1.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve2 = RSAnimationTimingCurve::CreateCubicCurve(INFINITY, 0.0f, 1.0f, 1.0f);
    EXPECT_TRUE(curve2.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve3 = RSAnimationTimingCurve::CreateCubicCurve(0.0f, NAN, 1.0f, 1.0f);
    EXPECT_TRUE(curve3.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve4 = RSAnimationTimingCurve::CreateCubicCurve(0.0f, INFINITY, 1.0f, 1.0f);
    EXPECT_TRUE(curve4.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve5 = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, NAN, 1.0f);
    EXPECT_TRUE(curve5.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve6 = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, INFINITY, 1.0f);
    EXPECT_TRUE(curve6.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve7 = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 1.0f, NAN);
    EXPECT_TRUE(curve7.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve8 = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 1.0f, INFINITY);
    EXPECT_TRUE(curve8.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
}

/**
 * @tc.name: CreateSpringCurveNaNInf001
 * @tc.desc: Verify CreateSpringCurve NaN/Inf fallback to defaults
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringCurveNaNInf001, TestSize.Level1)
{
    auto curve1 = RSAnimationTimingCurve::CreateSpringCurve(NAN, 1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(curve1.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve2 = RSAnimationTimingCurve::CreateSpringCurve(INFINITY, 1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(curve2.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve3 = RSAnimationTimingCurve::CreateSpringCurve(0.0f, NAN, 1.0f, 1.0f);
    EXPECT_TRUE(curve3.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve4 = RSAnimationTimingCurve::CreateSpringCurve(0.0f, INFINITY, 1.0f, 1.0f);
    EXPECT_TRUE(curve4.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve5 = RSAnimationTimingCurve::CreateSpringCurve(0.0f, 1.0f, NAN, 1.0f);
    EXPECT_TRUE(curve5.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve6 = RSAnimationTimingCurve::CreateSpringCurve(0.0f, 1.0f, INFINITY, 1.0f);
    EXPECT_TRUE(curve6.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve7 = RSAnimationTimingCurve::CreateSpringCurve(0.0f, 1.0f, 1.0f, NAN);
    EXPECT_TRUE(curve7.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve8 = RSAnimationTimingCurve::CreateSpringCurve(0.0f, 1.0f, 1.0f, INFINITY);
    EXPECT_TRUE(curve8.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
}

/**
 * @tc.name: CreateInterpolatingSpringNaNInf001
 * @tc.desc: Verify CreateInterpolatingSpring NaN/Inf fallback to defaults
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateInterpolatingSpringNaNInf001, TestSize.Level1)
{
    auto curve1 = RSAnimationTimingCurve::CreateInterpolatingSpring(NAN, 1.0f, 1.0f, 0.0f);
    EXPECT_TRUE(curve1.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve2 = RSAnimationTimingCurve::CreateInterpolatingSpring(INFINITY, 1.0f, 1.0f, 0.0f);
    EXPECT_TRUE(curve2.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve3 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, NAN, 1.0f, 0.0f);
    EXPECT_TRUE(curve3.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve4 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, INFINITY, 1.0f, 0.0f);
    EXPECT_TRUE(curve4.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve5 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 1.0f, NAN, 0.0f);
    EXPECT_TRUE(curve5.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve6 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 1.0f, INFINITY, 0.0f);
    EXPECT_TRUE(curve6.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve7 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 1.0f, 1.0f, NAN);
    EXPECT_TRUE(curve7.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve8 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 1.0f, 1.0f, INFINITY);
    EXPECT_TRUE(curve8.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve9 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 1.0f, 1.0f, 0.0f, NAN);
    EXPECT_TRUE(curve9.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto curve10 = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 1.0f, 1.0f, 0.0f, INFINITY);
    EXPECT_TRUE(curve10.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);
}

/**
 * @tc.name: CreateSpringNaNInf001
 * @tc.desc: Verify CreateSpring NaN/Inf fallback to defaults
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringNaNInf001, TestSize.Level1)
{
    auto curve1 = RSAnimationTimingCurve::CreateSpring(NAN, 0.825f);
    EXPECT_TRUE(curve1.type_ == RSAnimationTimingCurve::CurveType::SPRING);

    auto curve2 = RSAnimationTimingCurve::CreateSpring(INFINITY, 0.825f);
    EXPECT_TRUE(curve2.type_ == RSAnimationTimingCurve::CurveType::SPRING);

    auto curve3 = RSAnimationTimingCurve::CreateSpring(0.55f, NAN);
    EXPECT_TRUE(curve3.type_ == RSAnimationTimingCurve::CurveType::SPRING);

    auto curve4 = RSAnimationTimingCurve::CreateSpring(0.55f, INFINITY);
    EXPECT_TRUE(curve4.type_ == RSAnimationTimingCurve::CurveType::SPRING);

    auto curve5 = RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, NAN);
    EXPECT_TRUE(curve5.type_ == RSAnimationTimingCurve::CurveType::SPRING);

    auto curve6 = RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, INFINITY);
    EXPECT_TRUE(curve6.type_ == RSAnimationTimingCurve::CurveType::SPRING);

    auto curve7 = RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, 0.0f, NAN);
    EXPECT_TRUE(curve7.type_ == RSAnimationTimingCurve::CurveType::SPRING);

    auto curve8 = RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, 0.0f, INFINITY);
    EXPECT_TRUE(curve8.type_ == RSAnimationTimingCurve::CurveType::SPRING);
}
/**
 * @tc.name: CreateSpringCurveStiffnessLEZero001
 * @tc.desc: Verify CreateSpringCurve with stiffness <= 0 or mass*stiffness <= 0 fallback to defaults
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringCurveStiffnessLEZero001, TestSize.Level1)
{
    auto curve1 = RSAnimationTimingCurve::CreateSpringCurve(0.5f, 1.0f, -1.0f, 0.5f);
    EXPECT_TRUE(curve1.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve2 = RSAnimationTimingCurve::CreateSpringCurve(0.5f, 0.0f, 1.0f, 0.5f);
    EXPECT_TRUE(curve2.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve3 = RSAnimationTimingCurve::CreateSpringCurve(0.5f, -1.0f, 1.0f, 0.5f);
    EXPECT_TRUE(curve3.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto curve4 = RSAnimationTimingCurve::CreateSpringCurve(0.5f, 1.0f, 0.0f, 0.5f);
    EXPECT_TRUE(curve4.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
}

/**
 * @tc.name: CreateCurveNormal001
 * @tc.desc: Verify Create* methods with valid parameters return normal curves
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCurveNormal001, TestSize.Level1)
{
    auto cubic = RSAnimationTimingCurve::CreateCubicCurve(0.25f, 0.1f, 0.25f, 1.0f);
    EXPECT_TRUE(cubic.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto springCurve = RSAnimationTimingCurve::CreateSpringCurve(0.5f, 1.0f, 100.0f, 10.0f);
    EXPECT_TRUE(springCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);

    auto interpSpring =
        RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 100.0f, 10.0f, 0.0f, 0.001f);
    EXPECT_TRUE(interpSpring.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    auto spring = RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, 0.0f, 0.001f);
    EXPECT_TRUE(spring.type_ == RSAnimationTimingCurve::CurveType::SPRING);
}
} // namespace Rosen
} // namespace OHOS