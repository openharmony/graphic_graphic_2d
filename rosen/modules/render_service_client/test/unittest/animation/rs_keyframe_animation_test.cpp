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
 * @tc.desc: Verify the AddKeyFrame of KeyframeAnimationTest
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
 * @tc.desc: Verify the AddKeyFrame of keyframeAnimationTest
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
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFrameTest002 end";
}

/**
 * @tc.name: AddKeyFrameTest003
 * @tc.desc: Verify the AddKeyFrame of keyframeAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, AddKeyFrameTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFrameTest003 start";
    /**
     * @tc.steps: step1. init AddKeyFrame
     */
    auto black = RgbPalette::Black();
    auto blue = RgbPalette::Blue();
    auto red = RgbPalette::Red();
    auto white = RgbPalette::White();
    auto startProperty = std::make_shared<RSAnimatableProperty<Color>>(black);
    auto endProperty = std::make_shared<RSAnimatableProperty<Color>>(red);
    auto secondProperty = std::make_shared<RSAnimatableProperty<Color>>(white);
    auto testProperty = std::make_shared<RSAnimatableProperty<Color>>(blue);
    auto modifier = std::make_shared<RSBackgroundColorModifier>(startProperty);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(startProperty);
    keyframeAnimation->SetDuration(ANIMATION_DURATION_2);
    keyframeAnimation->AddKeyFrame(FRACTION_MIN - 0.1f, startProperty, RSAnimationTimingCurve::EASE_OUT);
    keyframeAnimation->AddKeyFrame(FRACTION_MAX / 2, secondProperty, RSAnimationTimingCurve::EASE_IN);
    keyframeAnimation->AddKeyFrame(FRACTION_MAX + 0.1f, endProperty, RSAnimationTimingCurve::EASE_IN_OUT);
    keyframeAnimation->Start(canvasNode);
    keyframeAnimation->AddKeyFrame(FRACTION_MAX * 2, testProperty, RSAnimationTimingCurve::EASE_IN);
    /**
     * @tc.steps: step2. start AddKeyFrame test
     */
    EXPECT_TRUE(keyframeAnimation != nullptr);
    EXPECT_TRUE(keyframeAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFrameTest003 end";
}

/**
 * @tc.name: AddKeyFramesTest001
 * @tc.desc: Verify the AddKeyFrames of keyframeAnimationTest
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
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest AddKeyFramesTest002 end";
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
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RSNodeAnimateTest001 end";
}

/**
 * @tc.name: RSNodeAnimateTest002
 * @tc.desc: Verify the RSNodeAnimate of KeyframeAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, RSNodeAnimateTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RSNodeAnimateTest002 start";
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
        RSNode::AddKeyFrame(
            nullptr, 0.1f, RSAnimationTimingCurve::EASE, [&property]() { property->Set(ANIMATION_TENTH_SCALE); });
        RSNode::AddKeyFrame(
            nullptr, 0.5f, RSAnimationTimingCurve::EASE, [&property]() { property->Set(ANIMATION_HALF_SCALE); });
        RSNode::AddKeyFrame(
            nullptr, 1.0f, RSAnimationTimingCurve::EASE, [&property]() { property->Set(ANIMATION_NORMAL_SCALE); });
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
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest RSNodeAnimateTest002 end";
}

/**
 * @tc.name: SetIsCustomTest001
 * @tc.desc: Verify the SetIsCustom of keyframeAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, SetIsCustomTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest SetIsCustomTest001 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto black = Vector4<Color>(RgbPalette::Black());
    auto red = Vector4<Color>(RgbPalette::Red());
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4<Color>>>(black);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4<Color>>>(red);

    auto property = std::make_shared<RSAnimatableProperty<Vector4<Color>>>(black);
    auto modifier = std::make_shared<RSBorderColorModifier>(property);
    canvasNode->SetBorderColor(RgbPalette::White());
    canvasNode->SetBorderWidth(10.0f);
    canvasNode->SetBorderStyle(1);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    auto keyframeAnimation = std::make_shared<RSKeyframeAnimation>(property);
    keyframeAnimation->SetDuration(ANIMATION_DURATION_2);

    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    keyframes.push_back(std::make_tuple(FRACTION_MIN, startProperty, RSAnimationTimingCurve::EASE_IN));
    keyframes.push_back(std::make_tuple(FRACTION_MAX, endProperty, RSAnimationTimingCurve::EASE_IN));
    keyframeAnimation->AddKeyFrames(keyframes);
    keyframeAnimation->SetIsCustom(false);
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_TRUE(keyframeAnimation != nullptr);
    keyframeAnimation->Start(canvasNode);
    EXPECT_TRUE(keyframeAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest SetIsCustomTest001 end";
}

/**
 * @tc.name: SetIsCustomTest002
 * @tc.desc: Verify the SetIsCustom of keyframeAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, SetIsCustomTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest SetIsCustomTest002 start";
    /**
     * @tc.steps: step1. init SetIsCustom
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
    keyframeAnimation->SetIsCustom(true);
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_TRUE(keyframeAnimation != nullptr);
    keyframeAnimation->Start(canvasNode);
    EXPECT_TRUE(keyframeAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest SetIsCustomTest002 end";
}

/**
 * @tc.name: KeyframeAnimationTest001
 * @tc.desc: Verify the SetIsCustom of keyframeAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSKeyframeAnimationTest, KeyframeAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest KeyframeAnimationTest001 start";
    /**
     * @tc.steps: step1. init
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
    /**
     * @tc.steps: step2. test
     */
    keyframeAnimation->SetDurationKeyframe(true);
    keyframeAnimation->InitInterpolationValue();
    keyframeAnimation->SetDurationKeyframe(false);
    keyframeAnimation->AddKeyFrame(FRACTION_MIN + 1, FRACTION_MIN, startProperty, RSAnimationTimingCurve::EASE_OUT);
    keyframeAnimation->AddKeyFrame(1, 2, startProperty, RSAnimationTimingCurve::EASE_IN);
    keyframeAnimation->InitInterpolationValue();
    keyframeAnimation->SetDurationKeyframe(true);
    keyframeAnimation->OnStart();
    EXPECT_TRUE(keyframeAnimation->isDurationKeyframe_);
    GTEST_LOG_(INFO) << "RSKeyframeAnimationTest KeyframeAnimationTest001 end";
}
} // namespace Rosen
} // namespace OHOS