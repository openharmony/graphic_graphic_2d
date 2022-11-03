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

#include "animation/rs_keyframe_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSKeyframeAnimationTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: AddKeyFrameTest001
 * @tc.desc: Verify the GetTimingCurve of KeyframeAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, AddKeyFrameTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFrameTest001 start";

    /**
     * @tc.steps: step1. init AddKeyFrame
     */
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);

    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(property);
    keyframeAnimation->SetDuration(ANIMATION_DURATION_2);
    keyframeAnimation->AddKeyFrame(FRACTION_MIN, startProperty, RSAnimationTimingCurve::EASE_OUT);
    keyframeAnimation->AddKeyFrame(FRACTION_MAX, endProperty, RSAnimationTimingCurve::EASE_IN_OUT);
    /**
     * @tc.steps: step2. start AddKeyFrame test
     */
    EXPECT_TRUE(keyframeAnimation != nullptr);
    keyframeAnimation->Start(canvasNode);
    EXPECT_TRUE(keyframeAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFrameTest001 end";
}

/**
 * @tc.name: AddKeyFrameTest002
 * @tc.desc: Verify the GetTimingCurve of keyframeAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, AddKeyFrameTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFrameTest002 start";
    /**
     * @tc.steps: step1. init AddKeyFrame
     */
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto secondProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_SECOND_BOUNDS);

    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(property);
    keyframeAnimation->SetDuration(ANIMATION_DURATION_2);
    keyframeAnimation->AddKeyFrame(FRACTION_MIN, startProperty, RSAnimationTimingCurve::EASE_OUT);
    keyframeAnimation->AddKeyFrame(FRACTION_MAX / 2, secondProperty, RSAnimationTimingCurve::EASE_IN);
    keyframeAnimation->AddKeyFrame(FRACTION_MAX, endProperty, RSAnimationTimingCurve::EASE_IN_OUT);

    /**
     * @tc.steps: step2. start AddKeyFrame test
     */
    EXPECT_TRUE(keyframeAnimation != nullptr);
    keyframeAnimation->Start(canvasNode);
    EXPECT_TRUE(keyframeAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFrameTest001 end";
}

/**
 * @tc.name: AddKeyFramesTest001
 * @tc.desc: Verify the GetTimingCurve of keyframeAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, AddKeyFramesTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFramesTest001 start";
    /**
     * @tc.steps: step1. init AddKeyFrames
     */
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto secondProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_SECOND_BOUNDS);

    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(property);
    keyframeAnimation->SetDuration(ANIMATION_DURATION_2);

    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    keyframes.push_back(std::make_tuple(FRACTION_MIN, startProperty, RSAnimationTimingCurve::EASE_IN));
    keyframes.push_back(std::make_tuple(FRACTION_MAX / 2, secondProperty, RSAnimationTimingCurve::EASE_IN));
    keyframes.push_back(std::make_tuple(FRACTION_MAX, endProperty, RSAnimationTimingCurve::EASE_IN));
    keyframeAnimation->AddKeyFrames(keyframes);
    /**
     * @tc.steps: step2. start AddKeyFrames test
     */
    EXPECT_TRUE(keyframeAnimation != nullptr);
    keyframeAnimation->Start(canvasNode);
    EXPECT_TRUE(keyframeAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFramesTest001 end";
}

/**
 * @tc.name: AddKeyFramesTest002
 * @tc.desc: Verify the AddKeyFrames of KeyframeAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, AddKeyFramesTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFramesTest002 start";
    /**
     * @tc.steps: step1. init AddKeyFrames
     */
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto secondProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_SECOND_BOUNDS);
    auto thirdProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_THIRD_BOUNDS);

    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(property);
    keyframeAnimation->SetDuration(ANIMATION_DURATION_2);

    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    keyframes.push_back(std::make_tuple(FRACTION_MIN, startProperty, RSAnimationTimingCurve::EASE_IN));
    keyframes.push_back(std::make_tuple(FRACTION_MAX / 3, secondProperty, RSAnimationTimingCurve::EASE_IN));
    keyframes.push_back(std::make_tuple(FRACTION_MAX * 2 / 3, thirdProperty, RSAnimationTimingCurve::EASE_IN));
    keyframes.push_back(std::make_tuple(FRACTION_MAX, endProperty, RSAnimationTimingCurve::EASE_IN));
    keyframeAnimation->AddKeyFrames(keyframes);
    /**
     * @tc.steps: step2. start AddKeyFrames test
     */
    EXPECT_TRUE(keyframeAnimation != nullptr);
    keyframeAnimation->Start(canvasNode);
    EXPECT_TRUE(keyframeAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFramesTest001 end";
}

/**
 * @tc.name: RSNodeAnimateTest001
 * @tc.desc: Verify the RSNodeAnimate of KeyframeAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, RSNodeAnimateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RSNodeAnimateTest001 start";
    /**
     * @tc.steps: step1. init RSNodeAnimate
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto modifier = std::make_shared<RSScaleModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        RSNode::AddKeyFrame(0.1f, RSAnimationTimingCurve::EASE, [&property]() {
            property->Set(ANIMATION_TENTH_SCALE);
        });
        RSNode::AddKeyFrame(0.5f, RSAnimationTimingCurve::EASE, [&property]() {
            property->Set(ANIMATION_HALF_SCALE);
        });
        RSNode::AddKeyFrame(1.0f, RSAnimationTimingCurve::EASE, [&property]() {
            property->Set(ANIMATION_NORMAL_SCALE);
        });
    });
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    if (animations.size() == CORRECT_SIZE) {
        auto animation = std::static_pointer_cast<RSKeyframeAnimation>(animations[FIRST_ANIMATION]);
        EXPECT_TRUE(animation != nullptr);
        EXPECT_TRUE(animation->IsRunning());
        NotifyStartAnimation();
    }
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSNodeAnimateTest001 end";
}

} // namespace Rosen
} // namespace OHOS