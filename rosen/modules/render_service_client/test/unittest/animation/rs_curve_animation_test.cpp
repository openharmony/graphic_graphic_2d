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
#include "animation/rs_animation_timing_protocol.h"
#include "ui/rs_ui_context_manager.h"

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
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(rsUIContext, property, startProperty, endProperty);
    auto interpolator = std::shared_ptr<RSInterpolator>();
    auto timingCurve = RSAnimationTimingCurve(interpolator);
    curveAnimation->SetTimingCurve(timingCurve);
    EXPECT_EQ(interpolator, nullptr);
    EXPECT_FALSE(curveAnimation->IsSupportInteractiveAnimator());
    GTEST_LOG_(INFO) << "RSCurveAnimationTest IsSupportInteractiveAnimator001 end";
}

/**
 * @tc.name: InvertStagingValue001
 * @tc.desc: Verify the InvertStagingValue with non-group animator
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, InvertStagingValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(rsUIContext, property, startProperty, endProperty);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(0);

    Vector4f originalValue = property->Get();

    // Call InvertStagingValue with isGroupAnimator = false
    curveAnimation->InvertStagingValue(false, timingProtocol);

    // Property value should not change when isGroupAnimator is false
    EXPECT_EQ(property->Get(), originalValue);

    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue001 end";
}

/**
 * @tc.name: InvertStagingValue002
 * @tc.desc: Verify the InvertStagingValue with group animator but AutoReverse is false
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, InvertStagingValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue002 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(rsUIContext, property, startProperty, endProperty);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetAutoReverse(false);  // AutoReverse is false
    timingProtocol.SetRepeatCount(0);

    // Set property to a different value first
    property->Set(ANIMATION_MIDDLE_BOUNDS);
    Vector4f currentValue = property->Get();

    // Call InvertStagingValue with isGroupAnimator = true but AutoReverse = false
    curveAnimation->InvertStagingValue(true, timingProtocol);

    // Property value should not change when AutoReverse is false
    EXPECT_EQ(property->Get(), currentValue);

    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue002 end";
}

/**
 * @tc.name: InvertStagingValue003
 * @tc.desc: Verify the InvertStagingValue with group animator, AutoReverse true, but odd repeat count
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, InvertStagingValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue003 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(rsUIContext, property, startProperty, endProperty);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(1);  // Odd repeat count

    // Set property to a different value first
    property->Set(ANIMATION_MIDDLE_BOUNDS);
    Vector4f currentValue = property->Get();

    // Call InvertStagingValue with isGroupAnimator = true, AutoReverse = true, but odd repeat count
    curveAnimation->InvertStagingValue(true, timingProtocol);

    // Property value should not change when repeat count is odd
    EXPECT_EQ(property->Get(), currentValue);

    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue003 end";
}

/**
 * @tc.name: InvertStagingValue004
 * @tc.desc: Verify the InvertStagingValue with group animator, AutoReverse true, and even repeat count (0)
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, InvertStagingValue004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue004 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(rsUIContext, property, startProperty, endProperty);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(0);  // Even repeat count (0 % 2 == 0)

    // Set property to a different value first
    property->Set(ANIMATION_MIDDLE_BOUNDS);

    // Call InvertStagingValue with isGroupAnimator = true, AutoReverse = true, and even repeat count
    curveAnimation->InvertStagingValue(true, timingProtocol);

    // Property value should be set to startValue_
    EXPECT_EQ(property->Get(), startProperty->Get());

    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue004 end";
}

/**
 * @tc.name: InvertStagingValue005
 * @tc.desc: Verify the InvertStagingValue with group animator, AutoReverse true, and even repeat count (2)
 * @tc.type: FUNC
 */
HWTEST_F(RSCurveAnimationTest, InvertStagingValue005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue005 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(rsUIContext, property, startProperty, endProperty);

    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetAutoReverse(true);
    timingProtocol.SetRepeatCount(2);  // Even repeat count (2 % 2 == 0)

    // Set property to a different value first
    property->Set(ANIMATION_MIDDLE_BOUNDS);

    // Call InvertStagingValue with isGroupAnimator = true, AutoReverse = true, and even repeat count
    curveAnimation->InvertStagingValue(true, timingProtocol);

    // Property value should be set to startValue_
    EXPECT_EQ(property->Get(), startProperty->Get());

    GTEST_LOG_(INFO) << "RSCurveAnimationTest InvertStagingValue005 end";
}
} // namespace Rosen
} // namespace OHOS
