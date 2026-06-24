/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "animation/rs_interpolating_spring_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_spring_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSSpringAnimationTest : public RSAnimationBaseTest {
};

class RSNodeMock : public RSNode {
public:
    explicit RSNodeMock(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr)
        : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext)
    {}
    ~RSNodeMock() = default;

    bool NeedForcedSendToRemote() const override
    {
        return true;
    }
};

/**
 * @tc.name: InterpolateImplTest
 * @tc.desc: Verify the InterpolateImpl
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolateImplTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolateImplTest start";
    RSSpringInterpolator interpolator(1.f, 1.f, 0.f);

    // case1: fraction <= 0
    float fraction = -1.0f;
    ASSERT_EQ(interpolator.InterpolateImpl(fraction), 0);

    // case2: fraction >= 1
    fraction = 1.5f;
    ASSERT_EQ(interpolator.InterpolateImpl(fraction), 1);

    // case3: 0 < fraction < 1
    fraction = 0.5f;
    ASSERT_NE(interpolator.InterpolateImpl(fraction), 0);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolateImplTest end";
}

/**
 * @tc.name: RSSpringSetZeroThresholdTest_001
 * @tc.desc: Verify SetZeroThreshold with positive value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSSpringSetZeroThresholdTest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSSpringAnimation>(nullptr, property, byValue);
    constexpr float positiveThreshold = 0.001f;
    animation->SetZeroThreshold(positiveThreshold);
    EXPECT_EQ(animation->zeroThreshold_, positiveThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_001 end";
}

/**
 * @tc.name: RSSpringSetZeroThresholdTest_002
 * @tc.desc: Verify SetZeroThreshold rejects negative value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSSpringSetZeroThresholdTest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_002 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSSpringAnimation>(nullptr, property, byValue);
    constexpr float negativeThreshold = -1.0f;
    animation->SetZeroThreshold(negativeThreshold);
    EXPECT_EQ(animation->zeroThreshold_, 0.0f);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_002 end";
}

/**
 * @tc.name: RSSpringSetZeroThresholdTest_003
 * @tc.desc: Verify SetZeroThreshold accepts zero value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSSpringSetZeroThresholdTest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_003 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSSpringAnimation>(nullptr, property, byValue);
    constexpr float zeroThreshold = 0.0f;
    animation->SetZeroThreshold(zeroThreshold);
    EXPECT_EQ(animation->zeroThreshold_, zeroThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_003 end";
}

/**
 * @tc.name: RSInterpolatingSpringSetZeroThresholdTest_001
 * @tc.desc: Verify RSInterpolatingSpringAnimation SetZeroThreshold with positive value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSInterpolatingSpringSetZeroThresholdTest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    constexpr float positiveThreshold = 0.001f;
    animation->SetZeroThreshold(positiveThreshold);
    EXPECT_EQ(animation->zeroThreshold_, positiveThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_001 end";
}

/**
 * @tc.name: RSInterpolatingSpringSetZeroThresholdTest_002
 * @tc.desc: Verify RSInterpolatingSpringAnimation SetZeroThreshold rejects negative value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSInterpolatingSpringSetZeroThresholdTest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_002 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    constexpr float negativeThreshold = -1.0f;
    animation->SetZeroThreshold(negativeThreshold);
    EXPECT_EQ(animation->zeroThreshold_, 0.0f);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_002 end";
}

/**
 * @tc.name: RSInterpolatingSpringSetZeroThresholdTest_003
 * @tc.desc: Verify RSInterpolatingSpringAnimation SetZeroThreshold accepts zero value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSInterpolatingSpringSetZeroThresholdTest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_003 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    constexpr float zeroThreshold = 0.0f;
    animation->SetZeroThreshold(zeroThreshold);
    EXPECT_EQ(animation->zeroThreshold_, zeroThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_003 end";
}
} // namespace Rosen
} // namespace OHOS