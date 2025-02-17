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

#include "animation/rs_render_interpolating_spring_animation.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderInterpolatingSpringAnimationMock : public RSRenderInterpolatingSpringAnimation {
public:
    explicit RSRenderInterpolatingSpringAnimationMock(AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue,
        const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& endValue) : RSRenderInterpolatingSpringAnimation(id,
        propertyId, originValue, startValue, endValue) {}
    ~RSRenderInterpolatingSpringAnimationMock() = default;

    void OnSetFraction(float fraction) override
    {
        RSRenderInterpolatingSpringAnimation::OnSetFraction(fraction);
    }

    void OnAnimate(float fraction) override
    {
        RSRenderInterpolatingSpringAnimation::OnAnimate(fraction);
    }

    void InitValueEstimator() override
    {
        RSRenderInterpolatingSpringAnimation::InitValueEstimator();
    }

    void OnInitialize(int64_t time) override
    {
        RSRenderInterpolatingSpringAnimation::OnInitialize(time);
    }
};

class RSRenderInterpolatingSpringAnimationUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_TEST_ID = 465412;
    static constexpr uint64_t PROPERTY_TEST_ID = 78998;
};

void RSRenderInterpolatingSpringAnimationUnitTest::SetUpTestCase() {}
void RSRenderInterpolatingSpringAnimationUnitTest::TearDownTestCase() {}
void RSRenderInterpolatingSpringAnimationUnitTest::SetUp() {}
void RSRenderInterpolatingSpringAnimationUnitTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, Marshalling001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.3f);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcelUt;
    renderInterpolatingSpringAnimation->Marshalling(parcelUt);
    renderInterpolatingSpringAnimation->Start();
    EXPECT_TRUE(renderInterpolatingSpringAnimation->IsRunning());
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, Marshalling002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.5f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcelUt;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcelUt);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, Marshalling003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::INVALID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcelUt;
    renderInterpolatingSpringAnimation->isAdditive_ = true;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcelUt);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, Marshalling004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.43f,
        PROPERTY_TEST_ID, RSRenderPropertyType::INVALID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcelUt;
    renderInterpolatingSpringAnimation->isAdditive_ = false;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcelUt);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Marshalling005
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, Marshalling005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_TEST_ID, RSRenderPropertyType::INVALID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcelUt;
    renderInterpolatingSpringAnimation->isAdditive_ = true;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcelUt);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Marshalling006
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, Marshalling006, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::INVALID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcelUt;
    renderInterpolatingSpringAnimation->isAdditive_ = true;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcelUt);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, Unmarshalling001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.5f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcelUt;
    auto renderAnimation = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcelUt);
    EXPECT_TRUE(renderAnimation == nullptr);
    renderInterpolatingSpringAnimation->Marshalling(parcelUt);
    renderAnimation = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcelUt);
    EXPECT_TRUE(renderAnimation != nullptr);
}

/**
 * @tc.name: SetFraction001
 * @tc.desc: Verify the SetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, SetFraction001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.6f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->InitValueEstimator();

    renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->OnSetFraction(1.0f);
    renderInterpolatingSpringAnimation->Start();
    EXPECT_TRUE(renderInterpolatingSpringAnimation->IsRunning());

    renderInterpolatingSpringAnimation->valueEstimator_ = nullptr;
    renderInterpolatingSpringAnimation->property_ = std::make_shared<RSRenderPropertyBase>();
    renderInterpolatingSpringAnimation->property_->CreateRSValueEstimator(RSValueEstimatorType::CURVE_VALUE_ESTIMATOR);
    EXPECT_TRUE(renderInterpolatingSpringAnimation->valueEstimator_ == nullptr);

    auto velocityUt = renderInterpolatingSpringAnimation->CalculateVelocity(0.0);
    EXPECT_TRUE(velocityUt == nullptr);
}

/**
 * @tc.name: OnInitialize001
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, OnInitialize001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->OnInitialize(100);
    renderInterpolatingSpringAnimation->Start();
    EXPECT_TRUE(renderInterpolatingSpringAnimation->IsRunning());
}

/**
 * @tc.name: SetZeroThreshold001
 * @tc.desc: Verify funciton SetZeroThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, SetZeroThreshold001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.5f, PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.4f, PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(
        1.3f, PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = true;
    renderInterpolatingSpringAnimation->SetZeroThreshold(-0.5f);
    EXPECT_FALSE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = false;
    renderInterpolatingSpringAnimation->SetZeroThreshold(0.5f);
    EXPECT_TRUE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);
}

/**
 * @tc.name: UpdateFractionAfterContinue001
 * @tc.desc: Verify the UpdateFractionAfterContinue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, UpdateFractionAfterContinue001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->OnInitialize(100);
    renderInterpolatingSpringAnimation->valueEstimator_ = nullptr;
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->UpdateFractionAfterContinue();
    renderInterpolatingSpringAnimation->OnAnimate(1.0f);
}

/**
 * @tc.name: UpdateFractionAfterContinue002
 * @tc.desc: Verify the UpdateFractionAfterContinue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, UpdateFractionAfterContinue002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.5f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->OnInitialize(100);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->OnSetFraction(1.0f);
    renderInterpolatingSpringAnimation->UpdateFractionAfterContinue();
    renderInterpolatingSpringAnimation->OnAnimate(1.0f);
    renderInterpolatingSpringAnimation->OnAnimate(0.5f);
    renderInterpolatingSpringAnimation->Start();
    EXPECT_TRUE(renderInterpolatingSpringAnimation->IsRunning());
}

/**
 * @tc.name: CalculateTimeFraction001
 * @tc.desc: Verify the CalculateTimeFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, CalculateTimeFraction001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.22f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->OnInitialize(0);
    EXPECT_NE(renderInterpolatingSpringAnimation->CalculateTimeFraction(0.0f), 1.0f);
    renderInterpolatingSpringAnimation->OnInitialize(100);
    EXPECT_NE(renderInterpolatingSpringAnimation->CalculateTimeFraction(0.0f), 1.0f);
    EXPECT_NE(renderInterpolatingSpringAnimation->CalculateTimeFraction(0.5f), 1.0f);
    EXPECT_NE(renderInterpolatingSpringAnimation->CalculateTimeFraction(1.0f), 1.0f);
}

/**
 * @tc.name: CalculateTimeFraction002
 * @tc.desc: Verify the CalculateTimeFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, CalculateTimeFraction002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.033f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->OnInitialize(100);
    renderInterpolatingSpringAnimation->SetDuration(0);
    float fractionUt = renderInterpolatingSpringAnimation->CalculateTimeFraction(1.0f);
    EXPECT_EQ(fractionUt, 0.0f);
}

/**
 * @tc.name: CalculateTimeFraction003
 * @tc.desc: Verify the CalculateTimeFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, CalculateTimeFraction003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.232f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.324f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    renderInterpolatingSpringAnimation->SetDuration(1);
    float fractionUt = renderInterpolatingSpringAnimation->CalculateTimeFraction(1000.0f);
    EXPECT_EQ(fractionUt, 0.0f);
}

/**
 * @tc.name: CalculateVelocity001
 * @tc.desc: Verify the CalculateVelocity
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, CalculateVelocity001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.12f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.32f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.32f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();
    auto velocityUt = renderInterpolatingSpringAnimation->CalculateVelocity(0.0);
    EXPECT_TRUE(velocityUt != nullptr);
    renderInterpolatingSpringAnimation->CallLogicallyFinishCallback();
}

/**
 * @tc.name: DumpAnimationInfo001
 * @tc.desc: Test case for property is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, DumpAnimationInfo001, TestSize.Level1)
{
    RSRenderInterpolatingSpringAnimation animationUt;
    auto property = std::make_shared<RSRenderPropertyBase>();
    animationUt.property_ = property;
    std::string out;
    animationUt.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderInterpolatingSpringAnimation, ModifierType: 0, StartValue: , EndValue: ");
}

/**
 * @tc.name: DumpAnimationInfo002
 * @tc.desc: Test case for property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, DumpAnimationInfo002, TestSize.Level1)
{
    RSRenderInterpolatingSpringAnimation animationUt;
    animationUt.property_ = nullptr;
    std::string out;
    animationUt.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderInterpolatingSpringAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
}

/**
 * @tc.name: OnAnimate001
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, OnAnimate001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->valueEstimator_ = nullptr;

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = true;
    renderInterpolatingSpringAnimation->animationFraction_.repeatCount_ = 1;
    renderInterpolatingSpringAnimation->animationFraction_.currentRepeatCount_ = 0;
    renderInterpolatingSpringAnimation->OnAnimate(0.1f);

    EXPECT_TRUE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);
}

/**
 * @tc.name: OnAnimate002
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, OnAnimate002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.03f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = true;
    renderInterpolatingSpringAnimation->animationFraction_.repeatCount_ = 1;
    renderInterpolatingSpringAnimation->animationFraction_.currentRepeatCount_ = 0;
    renderInterpolatingSpringAnimation->OnAnimate(1.0f);

    EXPECT_TRUE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);
}

/**
 * @tc.name: OnAnimate003
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, OnAnimate003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = true;
    renderInterpolatingSpringAnimation->animationFraction_.repeatCount_ = 1;
    renderInterpolatingSpringAnimation->animationFraction_.currentRepeatCount_ = 1;
    renderInterpolatingSpringAnimation->OnAnimate(0.1f);

    EXPECT_TRUE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);
}

/**
 * @tc.name: OnAnimate004
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationUnitTest, OnAnimate004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.23f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.5f,
        PROPERTY_TEST_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = true;
    renderInterpolatingSpringAnimation->animationFraction_.repeatCount_ = 1;
    renderInterpolatingSpringAnimation->animationFraction_.currentRepeatCount_ = 0;
    renderInterpolatingSpringAnimation->OnAnimate(0.1f);

    EXPECT_FALSE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);
}
} // namespace Rosen
} // namespace OHOS