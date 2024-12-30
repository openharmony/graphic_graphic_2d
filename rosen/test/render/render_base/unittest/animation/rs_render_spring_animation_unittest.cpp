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

#include "animation/rs_render_spring_animation.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderSpringAnimationUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_UT_ID = 51564;
    static constexpr uint64_t PROPERTY_UT_ID = 564654;
    static constexpr uint64_t PROPERTY_UT_ID_2 = 243245;
    static constexpr uint64_t PROPERTY_UT_ID_3 = 0;
};

void RSRenderSpringAnimationUnitTest::SetUpTestCase() {}
void RSRenderSpringAnimationUnitTest::TearDownTestCase() {}
void RSRenderSpringAnimationUnitTest::SetUp() {}
void RSRenderSpringAnimationUnitTest::TearDown() {}

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
HWTEST_F(RSRenderSpringAnimationUnitTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Marshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.323f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.044f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_UT_ID);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    Parcel parcel;
    renderSpringAnimation->Marshalling(parcel);
    renderSpringAnimation->Attach(renderNode.get());
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Unmarshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.55f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    Parcel parcel;
    auto animation = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation == nullptr);
    auto result = renderSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result == true);
    animation = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Unmarshalling001 end";
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, Unmarshalling002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.33f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, Unmarshalling003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.22f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.33f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.12f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, Attach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Attach001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.33f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.55f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->Attach(nullptr);
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Attach001 end";
}

/**
 * @tc.name: Attach002
 * @tc.desc: Verify the Attach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, Attach002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Attach002 start";
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation1 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property1, startProperty, endProperty);
    auto renderSpringAnimation2 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID_2, property2, endProperty, startProperty);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_UT_ID);

    EXPECT_TRUE(renderSpringAnimation1 != nullptr);
    renderSpringAnimation1->Attach(renderNode.get());
    renderSpringAnimation1->Start();
    EXPECT_TRUE(renderSpringAnimation1->IsRunning());
    EXPECT_TRUE(renderSpringAnimation2 != nullptr);
    renderSpringAnimation2->Attach(renderNode.get());
    renderSpringAnimation2->Start();
    EXPECT_TRUE(renderSpringAnimation2->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Attach002 end";
}

/**
 * @tc.name: Attach003
 * @tc.desc: Verify the Attach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, Attach003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Attach003 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.323f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.22f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.33f,
        PROPERTY_UT_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderSpringAnimation1 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_UT_ID);
    EXPECT_TRUE(renderSpringAnimation1 != nullptr);
    renderSpringAnimation1->SetSpringParameters(1.0f, 1.0f, 1000);
    renderSpringAnimation1->Attach(renderNode.get());
    renderSpringAnimation1->Start();
    renderSpringAnimation1->Pause();

    auto renderSpringAnimation2 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    EXPECT_TRUE(renderSpringAnimation2 != nullptr);
    renderSpringAnimation2->Attach(renderNode.get());
    renderSpringAnimation2->Start();
    EXPECT_TRUE(renderSpringAnimation2->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest Attach003 end";
}

/**
 * @tc.name: SetZeroThreshold001
 * @tc.desc: Verify funciton SetZeroThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, SetZeroThreshold001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest SetZeroThreshold001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.32f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.33f);

    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_UT_ID);
    float zeroThreshold = 0.5f;
    renderSpringAnimation->SetZeroThreshold(zeroThreshold);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest SetZeroThreshold001 end";
}

/**
 * @tc.name: SetFraction001
 * @tc.desc: Verify the SetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, SetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest SetFraction001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.434f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.32f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.32f);

    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_UT_ID);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->Attach(renderNode.get());
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    renderSpringAnimation->SetFraction(0.1f);
    renderSpringAnimation->Pause();
    EXPECT_TRUE(renderSpringAnimation->IsPaused());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest SetFraction001 end";
}

/**
 * @tc.name: OnInitialize001
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, OnInitialize001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnInitialize001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.04f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.23f);

    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    std::shared_ptr<RSRenderPropertyBase> velocity = nullptr;
    renderSpringAnimationMock->SetInitialVelocity(velocity);
    int64_t time = 100;
    renderSpringAnimationMock->OnInitialize(time);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnInitialize001 end";
}

/**
 * @tc.name: OnInitialize002
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, OnInitialize002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnInitialize002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.232f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.232f);

    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    renderSpringAnimationMock->InitValueEstimator();
    renderSpringAnimationMock->OnInitialize(0.0f);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnInitialize002 end";
}

/**
 * @tc.name: OnInitialize003
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, OnInitialize003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.434f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.22f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.344f);

    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, SetInitialVelocity001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest SetInitialVelocity001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.32f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.323f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.42f);

    auto renderSpringAnimationMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    std::shared_ptr<RSRenderPropertyBase> velocity = nullptr;
    renderSpringAnimationMock->SetInitialVelocity(velocity);
    auto velocity1 = std::make_shared<RSRenderPropertyBase>();
    renderSpringAnimationMock->SetInitialVelocity(velocity1);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest SetInitialVelocity001 end";
}

/**
 * @tc.name: OnAnimate001
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, OnAnimate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnAnimate001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.232f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.43f);
    auto renderSpringAnimationMock = std::make_shared<RSRenderSpringAnimationMock>(
        ANIMATION_UT_ID, PROPERTY_UT_ID_3, property, property1, property2);
    float response = 0.5f;
    float dampingRatio = 0.5f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    float fraction = 0.1f;
    renderSpringAnimationMock->OnAnimate(fraction);
    EXPECT_EQ(false, renderSpringAnimationMock->IsCalculateAniamtionValue());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnAnimate001 end";
}

/**
 * @tc.name: OnAnimate002
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, OnAnimate002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.03f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.32f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, OnAnimate003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.53f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.32f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, OnSetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnSetFraction001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.344f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.343f);
    auto renderSpringAnimationMock = std::make_shared<RSRenderSpringAnimationMock>(
        ANIMATION_UT_ID, PROPERTY_UT_ID_3, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimationMock->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimationMock != nullptr);
    renderSpringAnimationMock->OnSetFraction(0.0f);
    EXPECT_EQ(true, renderSpringAnimationMock->IsCalculateAniamtionValue());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest OnSetFraction001 end";
}

/**
 * @tc.name: InitValueEstimatorTest001
 * @tc.desc: Test valueEstimator_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, InitValueEstimatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest InitValueEstimatorTest001 start";

    RSRenderSpringAnimation animation;
    auto property = std::make_shared<RSRenderPropertyBase>();
    animation.property_ = property;
    animation.InitValueEstimator();
    EXPECT_TRUE(animation.valueEstimator_ == nullptr);

    GTEST_LOG_(INFO) << "RSRenderSpringAnimationUnitTest InitValueEstimatorTest001 end";
}

/**
 * @tc.name: DumpAnimationInfo001
 * @tc.desc: Test case for property is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, DumpAnimationInfo001, TestSize.Level1)
{
    RSRenderSpringAnimation animation;
    auto property = std::make_shared<RSRenderPropertyBase>();
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
HWTEST_F(RSRenderSpringAnimationUnitTest, DumpAnimationInfo002, TestSize.Level1)
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringAnimation001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.343f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringAnimation002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.232f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.434f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property2, property2, property1);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringAnimation003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.35f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.55f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    renderSpringAnimation->InheritSpringAnimation(prevAnimation);

    EXPECT_TRUE(prevAnimation->state_ != AnimationState::FINISHED);
}

/**
 * @tc.name: InheritSpringAnimation004
 * @tc.desc: Verify the InheritSpringAnimation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringAnimation004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.345f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.434f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    auto prevAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, OnDetach001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3453f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4345f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_UT_ID);
    EXPECT_TRUE(renderNode != nullptr);

    renderSpringAnimation->Attach(renderNode.get());
    renderNode->GetAnimationManager().AddAnimation(renderSpringAnimation);
    EXPECT_TRUE(renderNode->GetAnimationManager().QuerySpringAnimation(PROPERTY_UT_ID) != nullptr);
    renderSpringAnimation->OnDetach();
    EXPECT_TRUE(renderNode->GetAnimationManager().QuerySpringAnimation(PROPERTY_UT_ID) == nullptr);
}

/**
 * @tc.name: GetSpringStatus001
 * @tc.desc: Verify the GetSpringStatus
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationUnitTest, GetSpringStatus001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.433f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.543f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.32f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, GetSpringStatus002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.34f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, GetSpringStatus003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.34f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, GetSpringStatus004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.53f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.32f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringStatus001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.034f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.343f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property2, property2, property1);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringStatus002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.34f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property2, nullptr, property1);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringStatus003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.34f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, nullptr, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringStatus004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.32f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.34f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property2, property2, property1);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, InheritSpringStatus005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.43f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.34f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->AttachRenderProperty(property);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnInitialize(0.0f);

    auto renderSpringAnimationFrom =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
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
HWTEST_F(RSRenderSpringAnimationUnitTest, CallLogicallyFinishCallback001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.343f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.343f);
    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_UT_ID, PROPERTY_UT_ID, property, property1, property2);
    float response = 1.0f;
    float dampingRatio = 1.0f;
    float blendDuration = 1.0f;
    renderSpringAnimation->SetSpringParameters(response, dampingRatio, blendDuration);
    EXPECT_TRUE(renderSpringAnimation != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_UT_ID);
    EXPECT_TRUE(renderNode != nullptr);

    renderSpringAnimation->Attach(renderNode.get());
    renderNode->GetAnimationManager().AddAnimation(renderSpringAnimation);
    EXPECT_TRUE(renderNode->GetAnimationManager().QuerySpringAnimation(PROPERTY_UT_ID) != nullptr);

    renderSpringAnimation->CallLogicallyFinishCallback();
    EXPECT_TRUE(RSMessageProcessor::Instance().HasTransaction());
}
} // namespace Rosen
} // namespace OHOS