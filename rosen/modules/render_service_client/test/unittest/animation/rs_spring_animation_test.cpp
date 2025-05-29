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
 * @tc.name: GetTimingCurveTest001
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    springAnimation->SetDuration(100);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest001 end";
}

/**
 * @tc.name: GetTimingCurveTest002
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest002 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::SPRING);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest002 end";
}

/**
 * @tc.name: GetTimingCurveTest003
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest003 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest003 end";
}

/**
 * @tc.name: GetTimingCurveTest004
 * @tc.desc: Verify the GetTimingCurve of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, GetTimingCurveTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest004 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest GetTimingCurveTest004 end";
}

/**
 * @tc.name: RSNodeAnimateTest001
 * @tc.desc: Verify the RSNodeAnimate of RSSpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, RSNodeAnimateTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSNodeAnimateTest001 start";
    /**
     * @tc.steps: step1. init GetTimingCurve
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::SPRING;
    auto animations = RSNode::Animate(protocol, curve, [&property]() {
        property->Set(ANIMATION_END_BOUNDS);
    });
    /**
     * @tc.steps: step2. start GetTimingCurve test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    auto springAnimation = std::static_pointer_cast<RSSpringAnimation>(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(springAnimation != nullptr);
    if (springAnimation != nullptr) {
        RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
        EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    }
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSNodeAnimateTest001 end";
}

/**
 * @tc.name: SetIsCustomTest001
 * @tc.desc: Verify the SetIsCustom of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetIsCustomTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest001 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto black = RgbPalette::Black();
    auto red = RgbPalette::Red();
    auto property = std::make_shared<RSAnimatableProperty<Color>>(black);
    auto modifier = std::make_shared<RSForegroundColorModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Color>>(black);
    auto endProperty = std::make_shared<RSAnimatableProperty<Color>>(red);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(false);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest001 end";
}

/**
 * @tc.name: SetIsCustomTest002
 * @tc.desc: Verify the SetIsCustom of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetIsCustomTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest002 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(true);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest002 end";
}

/**
 * @tc.name: SetZeroThreshold001
 * @tc.desc: Verify the SetZeroThreshold of SpringAnimationTest
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetZeroThreshold001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold001 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetZeroThreshold(0.1f);
    springAnimation->SetZeroThreshold(-0.1f);
    springAnimation->SetInitialVelocity(nullptr);
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold001 end";
}

/**
 * @tc.name: SetZeroThreshold002
 * @tc.desc: Verify the SetZeroThreshold of RSInterpolatingSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetZeroThreshold002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold002 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSInterpolatingSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetZeroThreshold(0.1f);
    springAnimation->SetZeroThreshold(-0.1f);
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold002 end";
}

/**
 * @tc.name: SetZeroThreshold003
 * @tc.desc: Verify the SetZeroThreshold of RSInterpolatingSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetZeroThreshold003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold003 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto byProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSInterpolatingSpringAnimation>(property, byProperty);
    springAnimation->SetZeroThreshold(0.01f);
    springAnimation->SetZeroThreshold(-0.01f);
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    ASSERT_TRUE(springAnimation != nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold003 end";
}

/**
 * @tc.name: SetZeroThreshold004
 * @tc.desc: Verify the SetZeroThreshold of RSInterpolatingSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetZeroThreshold004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold004 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto byProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSInterpolatingSpringAnimation>(property, byProperty);
    springAnimation->SetZeroThreshold(0.001f);
    springAnimation->SetZeroThreshold(-0.001f);
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    ASSERT_TRUE(springAnimation != nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetZeroThreshold004 end";
}

/**
 * @tc.name: SetIsCustomTest003
 * @tc.desc: Verify the SetIsCustom of RSInterpolatingSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, SetIsCustomTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest003 start";
    /**
     * @tc.steps: step1. init SetIsCustom
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSInterpolatingSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(true);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. start SetIsCustom test
     */
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest003 end";
}

/**
 * @tc.name: TargetTest001
 * @tc.desc: Verify the SetIsCustom of RSInterpolatingSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, TargetTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest003 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSInterpolatingSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(false);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. settarget
     */
    springAnimation->target_.reset();
    springAnimation->OnStart();
    EXPECT_FALSE(springAnimation == nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    canvasNode->isRenderServiceNode_ = true;
    canvasNode->isTextureExportNode_ = false;
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SetIsCustomTest002 end";
}

/**
 * @tc.name: TargetTest002
 * @tc.desc: Verify the SetIsCustom of RSInterpolatingSpringAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, TargetTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest TargetTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    canvasNode->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSInterpolatingSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(false);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. settarget
     */
    ASSERT_TRUE(springAnimation != nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->StartInner(canvasNode);
    auto target = springAnimation->target_.lock();
    ASSERT_TRUE(target != nullptr);
    springAnimation->OnStart();
    EXPECT_FALSE(target->NeedForcedSendToRemote());
    canvasNode->isRenderServiceNode_ = true;
    canvasNode->isTextureExportNode_ = false;
    springAnimation->Start(canvasNode);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest TargetTest002 end";
}

HWTEST_F(RSSpringAnimationTest, TargetTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest TargetTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto nodeMock = std::make_shared<RSNodeMock>(true);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    nodeMock->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(DELAY_TIME_ONE);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    auto springAnimation = std::make_shared<RSInterpolatingSpringAnimation>(property, startProperty, endProperty);
    springAnimation->SetIsCustom(false);
    RSAnimationTimingCurve timingCurve = springAnimation->GetTimingCurve();
    /**
     * @tc.steps: step2. settarget
     */
    ASSERT_TRUE(springAnimation != nullptr);
    EXPECT_FALSE(springAnimation->IsStarted());
    springAnimation->StartInner(nodeMock);
    auto target = springAnimation->target_.lock();
    ASSERT_TRUE(target != nullptr);
    springAnimation->OnStart();
    EXPECT_TRUE(target->NeedForcedSendToRemote());
    animationSurfaceNode->isRenderServiceNode_ = true;
    animationSurfaceNode->isTextureExportNode_ = false;
    springAnimation->Start(nodeMock);
    EXPECT_TRUE(springAnimation->IsRunning());
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSSpringAnimationTest TargetTest003 end";
}
} // namespace Rosen
} // namespace OHOS