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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSCurveAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: IsSupportInteractiveAnimator001
 * @tc.desc: Verify the IsSupportInteractiveAnimator
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, IsSupportInteractiveAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest IsSupportInteractiveAnimator001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, startProperty, endProperty);
    auto interpolator = std::shared_ptr<RSInterpolator>();
    auto timingCurve = RSAnimationTimingCurve(interpolator);
    curveAnimation->SetTimingCurve(timingCurve);
    EXPECT_EQ(interpolator, nullptr);
    EXPECT_FALSE(curveAnimation->IsSupportInteractiveAnimator());
    GTEST_LOG_(INFO) << "RSCurveAnimationTest IsSupportInteractiveAnimator001 end";
}
} // namespace Rosen
} // namespace OHOS