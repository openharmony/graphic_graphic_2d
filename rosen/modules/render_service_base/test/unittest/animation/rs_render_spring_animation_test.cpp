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

#include "animation/rs_render_spring_animation.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderSpringAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr uint64_t PROPERTY_ID_2 = 54322;
    static constexpr uint64_t PROPERTY_ID_3 = 0;
};

void RSRenderSpringAnimationTest::SetUpTestCase() {}
void RSRenderSpringAnimationTest::TearDownTestCase() {}
void RSRenderSpringAnimationTest::SetUp() {}
void RSRenderSpringAnimationTest::TearDown() {}

class RSRenderSpringAnimationMock : public RSRenderSpringAnimation {
public:
    explicit RSRenderSpringAnimationMock(AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue,
        const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue)
        : RSRenderSpringAnimation(id, propertyId, originValue, startValue, endValue)
    {}
    void SetSpringParameters(float response, float dampingRatio, float blendDuration = 0.5f)
    {
        RSRenderSpringAnimation::SetSpringParameters(response, dampingRatio, blendDuration);
    }
    void SetInitialVelocity(const std::shared_ptr<RSRenderPropertyBase>& velocity)
    {
        RSRenderSpringAnimation::SetInitialVelocity(velocity);
    }
    void OnSetFraction(float fraction)
    {
        RSRenderSpringAnimation::OnSetFraction(fraction);
    }
    void OnAnimate(float fraction)
    {
        RSRenderSpringAnimation::OnAnimate(fraction);
    }
    void OnAttach()
    {
        RSRenderSpringAnimation::OnAttach();
    }
    void OnDetach()
    {
        RSRenderSpringAnimation::OnDetach();
    }
    void OnInitialize(int64_t time)
    {
        RSRenderSpringAnimation::OnInitialize(time);
    }
    void InitValueEstimator()
    {
        RSRenderSpringAnimation::InitValueEstimator();
    }
};

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Marshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    Parcel parcel;
    renderSpringAnimation->Marshalling(parcel);
    renderSpringAnimation->Attach(renderNode.get());
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Unmarshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    Parcel parcel;
    auto animation = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation == nullptr);
    auto result = renderSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result == true);
    animation = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Unmarshalling001 end";
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    EXPECT_TRUE(renderPropertyAnimation != nullptr);

    Parcel parcel;
    auto result = renderPropertyAnimation->Marshalling(parcel);
    EXPECT_TRUE(result == true);
    auto animation = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation == nullptr);
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    Parcel parcel;
    renderSpringAnimation->initialVelocity_ = nullptr;
    auto result = renderSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result == true);
    auto animation = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: Attach001
 * @tc.desc: Verify the Attach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Attach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->Attach(nullptr);
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach001 end";
}

/**
 * @tc.name: Attach002
 * @tc.desc: Verify the Attach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Attach002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach002 start";
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation1 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property1, startProperty, endProperty);
    auto renderSpringAnimation2 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID_2, property2, endProperty, startProperty);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderSpringAnimation1 != nullptr);
    renderSpringAnimation1->Attach(renderNode.get());
    renderSpringAnimation1->Start();
    EXPECT_TRUE(renderSpringAnimation1->IsRunning());
    EXPECT_TRUE(renderSpringAnimation2 != nullptr);
    renderSpringAnimation2->Attach(renderNode.get());
    renderSpringAnimation2->Start();
    EXPECT_TRUE(renderSpringAnimation2->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach002 end";
}

/**
 * @tc.name: Attach003
 * @tc.desc: Verify the Attach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Attach003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach003 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderSpringAnimation1 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    EXPECT_TRUE(renderSpringAnimation1 != nullptr);
    renderSpringAnimation1->SetSpringParameters(1.0f, 1.0f, 1000);
    renderSpringAnimation1->Attach(renderNode.get());
    renderSpringAnimation1->Start();
    renderSpringAnimation1->Pause();

    auto renderSpringAnimation2 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderSpringAnimation2 != nullptr);
    renderSpringAnimation2->Attach(renderNode.get());
    renderSpringAnimation2->Start();
    EXPECT_TRUE(renderSpringAnimation2->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach003 end";
}

/**
 * @tc.name: SetZeroThreshold001
 * @tc.desc: Verify funciton SetZeroThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetZeroThreshold001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetZeroThreshold001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    float zeroThreshold = 0.5f;
    renderSpringAnimation->SetZeroThreshold(zeroThreshold);
    EXPECT_TRUE(zeroThreshold);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetZeroThreshold001 end";
}

/**
 * @tc.name: SetFraction001
 * @tc.desc: Verify the SetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetFraction001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->Attach(renderNode.get());
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    renderSpringAnimation->SetFraction(0.1f);
    renderSpringAnimation->Pause();
    EXPECT_TRUE(renderSpringAnimation->IsPaused());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetFraction001 end";
}

/**
 * @tc.name: OnInitialize001
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnInitialize001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnInitialize001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    std::shared_ptr<RSRenderPropertyBase> velocity = nullptr;
    renderSpringAnimationMock->SetInitialVelocity(velocity);
    int64_t time = 100;
    renderSpringAnimationMock->OnInitialize(time);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnInitialize001 end";
}

/**
 * @tc.name: OnInitialize002
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnInitialize002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnInitialize002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    renderSpringAnimationMock->InitValueEstimator();
    renderSpringAnimationMock->OnInitialize(0.0f);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnInitialize002 end";
}

/**
 * @tc.name: OnInitialize003
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnInitialize003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->blendDuration_ = true;
    renderSpringAnimation->OnInitialize(0.0f);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
}

/**
 * @tc.name: SetInitialVelocity001
 * @tc.desc: Verify the SetInitialVelocity
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetInitialVelocity001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetInitialVelocity001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    std::shared_ptr<RSRenderPropertyBase> velocity = nullptr;
    renderSpringAnimationMock->SetInitialVelocity(velocity);
    auto velocity1 = std::make_shared<RSRenderProperty<bool>>();
    renderSpringAnimationMock->SetInitialVelocity(velocity1);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetInitialVelocity001 end";
}

/**
 * @tc.name: OnAnimate001
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimate001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID_3, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    float fraction = 0.1f;
    renderSpringAnimationMock->OnAnimate(fraction);
    EXPECT_EQ(false, renderSpringAnimationMock->IsCalculateAniamtionValue());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimate001 end";
}

/**
 * @tc.name: OnAnimate002
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimate002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    float fraction = 0.1f;
    renderSpringAnimation->springValueEstimator_ = nullptr;
    renderSpringAnimation->OnAnimate(fraction);
    EXPECT_EQ(true, renderSpringAnimation->IsCalculateAniamtionValue());
}

/**
 * @tc.name: OnAnimate003
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimate003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    float fraction = 0.1f;
    renderSpringAnimation->needLogicallyFinishCallback_ = true;
    renderSpringAnimation->animationFraction_.repeatCount_ = 1;
    renderSpringAnimation->animationFraction_.currentRepeatCount_ = 0;
    renderSpringAnimation->OnAnimate(fraction);
    EXPECT_EQ(true, renderSpringAnimation->IsCalculateAniamtionValue());
}

/**
 * @tc.name: OnSetFraction001
 * @tc.desc: Verify the OnSetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnSetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnSetFraction001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID_3, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    renderSpringAnimationMock->OnSetFraction(0.0f);
    EXPECT_EQ(true, renderSpringAnimationMock->IsCalculateAniamtionValue());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnSetFraction001 end";
}

/**
 * @tc.name: InitValueEstimatorTest001
 * @tc.desc: Test valueEstimator_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InitValueEstimatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest InitValueEstimatorTest001 start";

    RSRenderSpringAnimation animation;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    animation.property_ = property;
    animation.InitValueEstimator();
    EXPECT_TRUE(animation.valueEstimator_ == nullptr);

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest InitValueEstimatorTest001 end";
}

/**
 * @tc.name: DumpAnimationInfo001
 * @tc.desc: Test case for property is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, DumpAnimationInfo001, TestSize.Level1)
{
    RSRenderSpringAnimation animation;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    animation.property_ = property;
    std::string out;
    animation.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderSpringAnimation, ModifierType: 0, StartValue: , EndValue: ");
}

/**
 * @tc.name: DumpAnimationInfo002
 * @tc.desc: Test case for property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, DumpAnimationInfo002, TestSize.Level1)
{
    RSRenderSpringAnimation animation;
    animation.property_ = nullptr;
    std::string out;
    animation.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderSpringAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
}

/**
 * @tc.name: InheritSpringAnimation001
 * @tc.desc: Verify the InheritSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringAnimation001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    prevAnimation->AttachRenderProperty(property);
    prevAnimation->InitValueEstimator();
    prevAnimation->OnInitialize(0.0f);
    renderSpringAnimation->InheritSpringAnimation(prevAnimation);

    EXPECT_TRUE(prevAnimation->state_ != AnimationState::FINISHED);
}

/**
 * @tc.name: InheritSpringAnimation002
 * @tc.desc: Verify the InheritSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringAnimation002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property2, property2, property1);
    prevAnimation->AttachRenderProperty(property);
    prevAnimation->InitValueEstimator();
    prevAnimation->OnInitialize(0.0f);
    prevAnimation->Start();
    renderSpringAnimation->InheritSpringAnimation(prevAnimation);

    EXPECT_TRUE(prevAnimation->state_ == AnimationState::FINISHED);
}

/**
 * @tc.name: InheritSpringAnimation003
 * @tc.desc: Verify the InheritSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringAnimation003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->InheritSpringAnimation(prevAnimation);

    EXPECT_TRUE(prevAnimation->state_ != AnimationState::FINISHED);
}

/**
 * @tc.name: InheritSpringAnimation004
 * @tc.desc: Verify the InheritSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringAnimation004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    prevAnimation->AttachRenderProperty(property);
    prevAnimation->InitValueEstimator();
    prevAnimation->OnInitialize(0.0f);
    renderSpringAnimation->InheritSpringAnimation(prevAnimation);

    EXPECT_TRUE(prevAnimation->state_ != AnimationState::FINISHED);
}

/**
 * @tc.name: OnDetach001
 * @tc.desc: Verify the OnDetach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnDetach001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    EXPECT_TRUE(renderNode != nullptr);

    renderSpringAnimation->Attach(renderNode.get());
    renderNode->GetAnimationManager().AddAnimation(renderSpringAnimation);
    EXPECT_TRUE(renderNode->GetAnimationManager().QuerySpringAnimation(PROPERTY_ID) != nullptr);
    renderSpringAnimation->OnDetach();
    EXPECT_TRUE(renderNode->GetAnimationManager().QuerySpringAnimation(PROPERTY_ID) == nullptr);
}

/**
 * @tc.name: GetSpringStatus001
 * @tc.desc: Verify the GetSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, GetSpringStatus001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    renderSpringAnimation->prevMappedTime_ = 0.0f;
    auto [startValue, endValue, velocity] = renderSpringAnimation->GetSpringStatus();
    EXPECT_TRUE(startValue == renderSpringAnimation->startValue_);
    EXPECT_TRUE(endValue == renderSpringAnimation->endValue_);
    EXPECT_TRUE(velocity == renderSpringAnimation->initialVelocity_);
}

/**
 * @tc.name: GetSpringStatus002
 * @tc.desc: Verify the GetSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, GetSpringStatus002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    renderSpringAnimation->prevMappedTime_ = 1.0f;
    renderSpringAnimation->springValueEstimator_ = nullptr;
    auto [startValue, endValue, velocity] = renderSpringAnimation->GetSpringStatus();
    EXPECT_TRUE(startValue == renderSpringAnimation->startValue_);
    EXPECT_TRUE(endValue == renderSpringAnimation->endValue_);
    EXPECT_TRUE(velocity == renderSpringAnimation->initialVelocity_);
}

/**
 * @tc.name: GetSpringStatus003
 * @tc.desc: Verify the GetSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, GetSpringStatus003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);
    renderSpringAnimation->prevMappedTime_ = 1.0f;
    auto [startValue, endValue, velocity] = renderSpringAnimation->GetSpringStatus();
    EXPECT_TRUE(startValue != renderSpringAnimation->startValue_);
    EXPECT_TRUE(endValue == renderSpringAnimation->endValue_);
    EXPECT_TRUE(velocity != renderSpringAnimation->initialVelocity_);
}

/**
 * @tc.name: GetSpringStatus004
 * @tc.desc: Verify the GetSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, GetSpringStatus004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    renderSpringAnimation->animationFraction_.SetAnimationScale(0.0f);
    renderSpringAnimation->prevMappedTime_ = 1.0f;
    auto [startValue, endValue, velocity] = renderSpringAnimation->GetSpringStatus();
    EXPECT_TRUE(startValue == renderSpringAnimation->startValue_);
    EXPECT_TRUE(endValue == renderSpringAnimation->endValue_);
    EXPECT_TRUE(velocity == renderSpringAnimation->initialVelocity_);
}

/**
 * @tc.name: InheritSpringStatus001
 * @tc.desc: Verify the InheritSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringStatus001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property2, property2, property1);
    renderSpringAnimationFrom->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationFrom != nullptr);

    auto result = renderSpringAnimation->InheritSpringStatus(nullptr);
    EXPECT_FALSE(result);

    renderSpringAnimation->springValueEstimator_ = nullptr;
    result = renderSpringAnimation->InheritSpringStatus(renderSpringAnimationFrom.get());
    EXPECT_FALSE(result);
}

/**
 * @tc.name: InheritSpringStatus002
 * @tc.desc: Verify the InheritSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringStatus002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property2, nullptr, property1);
    renderSpringAnimationFrom->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationFrom != nullptr);

    auto result = renderSpringAnimation->InheritSpringStatus(renderSpringAnimationFrom.get());
    EXPECT_FALSE(result);
}

/**
 * @tc.name: InheritSpringStatus003
 * @tc.desc: Verify the InheritSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringStatus003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, nullptr, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimationFrom->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationFrom != nullptr);
    renderSpringAnimationFrom->InitValueEstimator();
    renderSpringAnimationFrom->OnInitialize(0.0f);

    auto result = renderSpringAnimation->InheritSpringStatus(renderSpringAnimationFrom.get());
    EXPECT_TRUE(result);
}

/**
 * @tc.name: InheritSpringStatus004
 * @tc.desc: Verify the InheritSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringStatus004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property2, property2, property1);
    renderSpringAnimationFrom->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationFrom != nullptr);
    renderSpringAnimationFrom->InitValueEstimator();
    renderSpringAnimationFrom->OnInitialize(0.0f);

    auto result = renderSpringAnimation->InheritSpringStatus(renderSpringAnimationFrom.get());
    EXPECT_TRUE(result);
}

/**
 * @tc.name: InheritSpringStatus005
 * @tc.desc: Verify the InheritSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringStatus005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimationFrom->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationFrom != nullptr);
    renderSpringAnimationFrom->InitValueEstimator();
    renderSpringAnimationFrom->OnInitialize(0.0f);

    auto result = renderSpringAnimation->InheritSpringStatus(renderSpringAnimationFrom.get());
    EXPECT_FALSE(result);

    result = renderSpringAnimation->InheritSpringStatus(renderSpringAnimation.get());
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CallLogicallyFinishCallback001
 * @tc.desc: Verify the CallLogicallyFinishCallback
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CallLogicallyFinishCallback001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    EXPECT_TRUE(renderNode != nullptr);

    renderSpringAnimation->Attach(renderNode.get());
    renderNode->GetAnimationManager().AddAnimation(renderSpringAnimation);
    EXPECT_TRUE(renderNode->GetAnimationManager().QuerySpringAnimation(PROPERTY_ID) != nullptr);

    renderSpringAnimation->CallLogicallyFinishCallback();
    EXPECT_TRUE(RSMessageProcessor::Instance().HasTransaction());
}
} // namespace Rosen
} // namespace OHOS