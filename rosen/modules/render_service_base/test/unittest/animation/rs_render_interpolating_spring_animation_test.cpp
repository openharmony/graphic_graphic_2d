/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "recording/draw_cmd_list.h"

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

    bool OnAnimate(float fraction) override
    {
        RSRenderInterpolatingSpringAnimation::OnAnimate(fraction);
        return false;
    }

    void InitValueEstimator() override
    {
        RSRenderInterpolatingSpringAnimation::InitValueEstimator();
    }

    void OnInitialize(int64_t time, bool isCustom = false) override
    {
        RSRenderInterpolatingSpringAnimation::OnInitialize(time, isCustom);
    }
};

class RSRenderInterpolatingSpringAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
};

void RSRenderInterpolatingSpringAnimationTest::SetUpTestCase() {}
void RSRenderInterpolatingSpringAnimationTest::TearDownTestCase() {}
void RSRenderInterpolatingSpringAnimationTest::SetUp() {}
void RSRenderInterpolatingSpringAnimationTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Marshalling001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcel;
    renderInterpolatingSpringAnimation->Marshalling(parcel);
    renderInterpolatingSpringAnimation->Start();
    EXPECT_TRUE(renderInterpolatingSpringAnimation->IsRunning());
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Marshalling002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcel;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Marshalling003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcel;
    renderInterpolatingSpringAnimation->isAdditive_ = true;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Marshalling004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcel;
    renderInterpolatingSpringAnimation->isAdditive_ = false;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Marshalling005
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Marshalling005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcel;
    renderInterpolatingSpringAnimation->isAdditive_ = true;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Marshalling006
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Marshalling006, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcel;
    renderInterpolatingSpringAnimation->isAdditive_ = true;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    Parcel parcel;
    auto renderAnimation = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(renderAnimation == nullptr);
    renderInterpolatingSpringAnimation->Marshalling(parcel);
    renderAnimation = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(renderAnimation != nullptr);
    EXPECT_EQ(renderAnimation->GetAnimationId(), ANIMATION_ID);
    EXPECT_EQ(renderAnimation->GetPropertyId(), PROPERTY_ID);
}

/**
 * @tc.name: SetFraction001
 * @tc.desc: Verify the SetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetFraction001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->InitValueEstimator();

    renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->OnSetFraction(1.0f);
    renderInterpolatingSpringAnimation->Start();
    EXPECT_TRUE(renderInterpolatingSpringAnimation->IsRunning());

    renderInterpolatingSpringAnimation->valueEstimator_ = nullptr;
    renderInterpolatingSpringAnimation->property_ = std::make_shared<RSRenderProperty<bool>>();
    renderInterpolatingSpringAnimation->property_->CreateRSValueEstimator(RSValueEstimatorType::CURVE_VALUE_ESTIMATOR);
    EXPECT_FALSE(renderInterpolatingSpringAnimation->valueEstimator_);

    auto velocity = renderInterpolatingSpringAnimation->CalculateVelocity(0.0);
    EXPECT_TRUE(velocity == nullptr);
}

/**
 * @tc.name: OnInitialize001
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnInitialize001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetZeroThreshold001, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = true;
    renderInterpolatingSpringAnimation->SetZeroThreshold(-0.5f);
    EXPECT_FALSE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = false;
    renderInterpolatingSpringAnimation->SetZeroThreshold(0.5f);
    EXPECT_TRUE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);
}

/**
 * @tc.name: SetZeroThreshold002
 * @tc.desc: Verify SetZeroThreshold rejects NaN and Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetZeroThreshold002, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = false;
    renderInterpolatingSpringAnimation->SetZeroThreshold(NAN);
    EXPECT_FALSE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);

    renderInterpolatingSpringAnimation->needLogicallyFinishCallback_ = false;
    renderInterpolatingSpringAnimation->SetZeroThreshold(INFINITY);
    EXPECT_FALSE(renderInterpolatingSpringAnimation->needLogicallyFinishCallback_);
}

/**
 * @tc.name: SetSpringParameters002
 * @tc.desc: Verify SetSpringParameters rejects NaN and Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetSpringParameters002, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->SetSpringParameters(NAN, 1.0f, 0.0f, 0.001f);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->SetSpringParameters(INFINITY, 1.0f, 0.0f, 0.001f);
    EXPECT_EQ(anim->dampingRatio_, 0.0f);

    anim->SetSpringParameters(1.0f, NAN, 0.0f, 0.001f);
    EXPECT_EQ(anim->normalizedInitialVelocity_, 0.0);

    anim->SetSpringParameters(1.0f, INFINITY, 0.0f, 0.001f);
    EXPECT_EQ(anim->minimumAmplitudeRatio_, 0.001f);

    anim->SetSpringParameters(1.0f, 1.0f, NAN, 0.001f);
    EXPECT_EQ(anim->minimumAmplitudeRatio_, 0.001f);

    anim->SetSpringParameters(1.0f, 1.0f, INFINITY, 0.001f);
    EXPECT_EQ(anim->minimumAmplitudeRatio_, 0.001f);

    anim->SetSpringParameters(1.0f, 1.0f, 0.0f, NAN);
    EXPECT_EQ(anim->minimumAmplitudeRatio_, 0.001f);

    anim->SetSpringParameters(1.0f, 1.0f, 0.0f, INFINITY);
    EXPECT_EQ(anim->minimumAmplitudeRatio_, 0.001f);
}

/**
 * @tc.name: SetZeroThreshold003
 * @tc.desc: Verify SetZeroThreshold rejects negative value
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetZeroThreshold003, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->SetZeroThreshold(-1.0f);
    EXPECT_FALSE(anim->needLogicallyFinishCallback_);
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: Verify Unmarshalling returns null when startValue_ is INVALID type
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling002, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint64(ANIMATION_ID);
    parcel.WriteInt32(1000);
    parcel.WriteInt32(0);
    parcel.WriteFloat(1.0f);
    parcel.WriteInt32(1);
    parcel.WriteBool(false);
    parcel.WriteBool(false);
    parcel.WriteInt32(0);
    parcel.WriteBool(false);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteUint64(0);
    parcel.WriteUint64(PROPERTY_ID);
    parcel.WriteBool(false);
    uint8_t floatType = static_cast<uint8_t>(RSPropertyType::FLOAT);
    parcel.WriteUnpadBuffer(&floatType, sizeof(uint8_t));
    bool animatable = true;
    parcel.WriteUnpadBuffer(&animatable, sizeof(bool));
    parcel.WriteUint64(PROPERTY_ID);
    parcel.WriteFloat(0.0f);
    uint32_t unit = 0;
    parcel.WriteUnpadBuffer(&unit, sizeof(uint32_t));
    uint8_t invalidType = static_cast<uint8_t>(RSPropertyType::INVALID);
    parcel.WriteUnpadBuffer(&invalidType, sizeof(uint8_t));
    parcel.WriteUnpadBuffer(&invalidType, sizeof(uint8_t));

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: Verify Unmarshalling returns null when spring params are NaN or Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling003, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = NAN;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->zeroThreshold_ = 0.0f;
    anim->needLogicallyFinishCallback_ = false;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling004
 * @tc.desc: Verify Unmarshalling returns null when endValue_ is INVALID but startValue_ valid
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling004, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint64(ANIMATION_ID);
    parcel.WriteInt32(1000);
    parcel.WriteInt32(0);
    parcel.WriteFloat(1.0f);
    parcel.WriteInt32(1);
    parcel.WriteBool(false);
    parcel.WriteBool(false);
    parcel.WriteInt32(0);
    parcel.WriteBool(false);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteUint64(0);
    parcel.WriteUint64(PROPERTY_ID);
    parcel.WriteBool(false);
    uint8_t floatType = static_cast<uint8_t>(RSPropertyType::FLOAT);
    parcel.WriteUnpadBuffer(&floatType, sizeof(uint8_t));
    bool animatable = true;
    parcel.WriteUnpadBuffer(&animatable, sizeof(bool));
    parcel.WriteUint64(PROPERTY_ID);
    parcel.WriteFloat(0.0f);
    uint32_t unit = 0;
    parcel.WriteUnpadBuffer(&unit, sizeof(uint32_t));
    parcel.WriteUnpadBuffer(&floatType, sizeof(uint8_t));
    parcel.WriteUnpadBuffer(&animatable, sizeof(bool));
    parcel.WriteUint64(PROPERTY_ID);
    parcel.WriteFloat(1.0f);
    parcel.WriteUnpadBuffer(&unit, sizeof(uint32_t));
    uint8_t invalidType = static_cast<uint8_t>(RSPropertyType::INVALID);
    parcel.WriteUnpadBuffer(&invalidType, sizeof(uint8_t));

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling005
 * @tc.desc: Verify Unmarshalling returns null when zeroThreshold_ is negative
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling005, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = -1.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling006
 * @tc.desc: Verify Unmarshalling returns null when dampingRatio_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling006, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = NAN;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling007
 * @tc.desc: Verify Unmarshalling returns null when normalizedInitialVelocity_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling007, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = INFINITY;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling008
 * @tc.desc: Verify Unmarshalling returns null when minimumAmplitudeRatio_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling008, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = NAN;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling009
 * @tc.desc: Verify Unmarshalling returns null when zeroThreshold_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling009, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = INFINITY;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling010
 * @tc.desc: Verify Unmarshalling returns null when response_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling010, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = INFINITY;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling011
 * @tc.desc: Verify Unmarshalling returns null when dampingRatio_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling011, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = INFINITY;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling012
 * @tc.desc: Verify Unmarshalling returns null when normalizedInitialVelocity_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling012, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = NAN;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling013
 * @tc.desc: Verify Unmarshalling returns null when minimumAmplitudeRatio_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling013, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = INFINITY;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: UpdateFractionAfterContinue001
 * @tc.desc: Verify the UpdateFractionAfterContinue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, UpdateFractionAfterContinue001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, UpdateFractionAfterContinue002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CalculateTimeFraction001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CalculateTimeFraction002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->OnInitialize(100);
    renderInterpolatingSpringAnimation->SetDuration(0);
    float fraction = renderInterpolatingSpringAnimation->CalculateTimeFraction(1.0f);
    EXPECT_EQ(fraction, 0.0f);
}

/**
 * @tc.name: CalculateTimeFraction003
 * @tc.desc: Verify the CalculateTimeFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CalculateTimeFraction003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);

    renderInterpolatingSpringAnimation->SetDuration(1);
    float fraction = renderInterpolatingSpringAnimation->CalculateTimeFraction(1000.0f);
    EXPECT_EQ(fraction, 0.0f);
}

/**
 * @tc.name: CalculateVelocity001
 * @tc.desc: Verify the CalculateVelocity
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CalculateVelocity001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->AttachRenderProperty(property);
    renderInterpolatingSpringAnimation->OnSetFraction(0.0f);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);
    renderInterpolatingSpringAnimation->InitValueEstimator();
    auto velocity = renderInterpolatingSpringAnimation->CalculateVelocity(0.0);
    EXPECT_TRUE(velocity != nullptr);
    renderInterpolatingSpringAnimation->CallLogicallyFinishCallback();
}

/**
 * @tc.name: DumpAnimationInfo001
 * @tc.desc: Test case for property is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, DumpAnimationInfo001, TestSize.Level1)
{
    RSRenderInterpolatingSpringAnimation animation;
    auto property = std::make_shared<RSRenderProperty<bool>>();
    animation.property_ = property;
    std::string out;
    animation.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderInterpolatingSpringAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
}

/**
 * @tc.name: DumpAnimationInfo002
 * @tc.desc: Test case for property is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, DumpAnimationInfo002, TestSize.Level1)
{
    RSRenderInterpolatingSpringAnimation animation;
    animation.property_ = nullptr;
    std::string out;
    animation.DumpAnimationInfo(out);
    EXPECT_EQ(out, "Type:RSRenderInterpolatingSpringAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
}

/**
 * @tc.name: OnAnimate001
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimate001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimate002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimate003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimate004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
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

class MockInterpolatingSpringCmdListProperty : public RSRenderAnimatableProperty<float> {
public:
    explicit MockInterpolatingSpringCmdListProperty(const float& value, const PropertyId& id)
        : RSRenderAnimatableProperty<float>(value, id)
    {}
    ~MockInterpolatingSpringCmdListProperty() = default;

    RSPropertyType typeTest_ = RSPropertyType::DRAW_CMD_LIST;

protected:
    RSPropertyType GetPropertyType() const override
    {
        return typeTest_;
    }
};

/**
 * @tc.name: ParseParamDrawCmdList001
 * @tc.desc: Verify ParseParam with DRAW_CMD_LIST property type for startValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, ParseParamDrawCmdList001, TestSize.Level1)
{
    auto property = std::make_shared<MockInterpolatingSpringCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockInterpolatingSpringCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockInterpolatingSpringCmdListProperty>(1.0f, PROPERTY_ID);
    property->typeTest_ = RSPropertyType::DRAW_CMD_LIST;
    property1->typeTest_ = RSPropertyType::DRAW_CMD_LIST;
    property2->typeTest_ = RSPropertyType::DRAW_CMD_LIST;

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);

    Parcel parcel;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
    auto animation = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation != nullptr);
    EXPECT_EQ(animation->GetAnimationId(), ANIMATION_ID);
    EXPECT_EQ(animation->GetPropertyId(), PROPERTY_ID);
}

/**
 * @tc.name: ParseParamDrawCmdList002
 * @tc.desc: Verify ParseParam with DRAW_CMD_LIST property type for endValue
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, ParseParamDrawCmdList002, TestSize.Level1)
{
    auto property = std::make_shared<MockInterpolatingSpringCmdListProperty>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<MockInterpolatingSpringCmdListProperty>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<MockInterpolatingSpringCmdListProperty>(1.0f, PROPERTY_ID);
    property1->typeTest_ = RSPropertyType::FLOAT;
    property2->typeTest_ = RSPropertyType::FLOAT;

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);

    Parcel parcel;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);
    auto animation = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(animation != nullptr);
    EXPECT_EQ(animation->GetAnimationId(), ANIMATION_ID);
    EXPECT_EQ(animation->GetPropertyId(), PROPERTY_ID);
}

/**
 * @tc.name: ParseParamDrawCmdListRealTypeTest
 * @desc: Verify ParseParam with real RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>
 *        to trigger DRAW_CMD_LIST conversion branch in ParseParam
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, ParseParamDrawCmdListRealTypeTest, TestSize.Level1)
{
    // Create real DrawCmdListPtr property
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(100, 200, Drawing::DrawCmdList::UnmarshalMode::IMMEDIATE);
    auto property = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList, PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(nullptr, PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList, PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);

    Parcel parcel;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);

    // Create new animation and call ParseParam directly
    auto animation = std::make_shared<RSRenderInterpolatingSpringAnimation>();
    result = animation->ParseParam(parcel);
    EXPECT_TRUE(result);
    EXPECT_EQ(animation->GetAnimationId(), ANIMATION_ID);
    EXPECT_EQ(animation->GetPropertyId(), PROPERTY_ID);
}

/**
 * @tc.name: ParseParamDrawCmdListBothValuesTest
 * @desc: Verify ParseParam when both startValue and endValue are non-null DRAW_CMD_LIST
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, ParseParamDrawCmdListBothValuesTest, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(100, 200, Drawing::DrawCmdList::UnmarshalMode::IMMEDIATE);
    auto property = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList, PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList, PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList, PROPERTY_ID);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);

    Parcel parcel;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);

    auto animation = std::make_shared<RSRenderInterpolatingSpringAnimation>();
    result = animation->ParseParam(parcel);
    EXPECT_TRUE(result);
    EXPECT_EQ(animation->GetAnimationId(), ANIMATION_ID);
    EXPECT_EQ(animation->GetPropertyId(), PROPERTY_ID);
}

/**
 * @tc.name: ParseParamDrawCmdListNullEndValueTest
 * @desc: Verify ParseParam when endValue is null (covers !endValue_ branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, ParseParamDrawCmdListNullEndValueTest, TestSize.Level1)
{
    // Create real DrawCmdListPtr property for origin and start
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(100, 200, Drawing::DrawCmdList::UnmarshalMode::IMMEDIATE);
    auto property = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList, PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(drawCmdList, PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<Drawing::DrawCmdListPtr>>(nullptr, PROPERTY_ID); // endValue is null

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetSpringParameters(1.0f, 1.0f, 1000);

    Parcel parcel;
    auto result = renderInterpolatingSpringAnimation->Marshalling(parcel);
    EXPECT_TRUE(result);

    // Create new animation and call ParseParam directly
    auto animation = std::make_shared<RSRenderInterpolatingSpringAnimation>();
    result = animation->ParseParam(parcel);
    EXPECT_TRUE(result);
    EXPECT_EQ(animation->GetAnimationId(), ANIMATION_ID);
    EXPECT_EQ(animation->GetPropertyId(), PROPERTY_ID);
}

/**
 * @tc.name: RebuildPropertyValue001
 * @tc.desc: Verify RebuildPropertyValue with null valueEstimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, RebuildPropertyValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest RebuildPropertyValue001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderInterpolatingSpringAnimation =
        std::make_shared<RSRenderInterpolatingSpringAnimation>(
            ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->valueEstimator_ = nullptr;
    renderInterpolatingSpringAnimation->RebuildPropertyValue(0.5f);
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest RebuildPropertyValue001 end";
}

/**
 * @tc.name: RebuildPropertyValue002
 * @tc.desc: Verify RebuildPropertyValue with fraction equal to 1.0f
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, RebuildPropertyValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest RebuildPropertyValue002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderInterpolatingSpringAnimation =
        std::make_shared<RSRenderInterpolatingSpringAnimation>(
            ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetDuration(300);
    renderInterpolatingSpringAnimation->property_ = property;
    renderInterpolatingSpringAnimation->InitValueEstimator();
    renderInterpolatingSpringAnimation->RebuildPropertyValue(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest RebuildPropertyValue002 end";
}

/**
 * @tc.name: RebuildPropertyValue003
 * @tc.desc: Verify RebuildPropertyValue with fraction not equal to 1.0f
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, RebuildPropertyValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest RebuildPropertyValue003 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderInterpolatingSpringAnimation =
        std::make_shared<RSRenderInterpolatingSpringAnimation>(
            ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetDuration(300);
    renderInterpolatingSpringAnimation->property_ = property;
    renderInterpolatingSpringAnimation->InitValueEstimator();
    renderInterpolatingSpringAnimation->RebuildPropertyValue(0.5f);
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest RebuildPropertyValue003 end";
}

/**
 * @tc.name: Unmarshalling014
 * @tc.desc: Verify Unmarshalling returns null when zeroThreshold_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling014, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->normalizedInitialVelocity_ = 1.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = NAN;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling015
 * @tc.desc: Verify Unmarshalling returns valid animation when all parameters are normal
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling015, TestSize.Level1)
{
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 0.55f;
    anim->dampingRatio_ = 0.825f;
    anim->normalizedInitialVelocity_ = 0.0f;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: SetSpringParametersConverge001
 * @tc.desc: Verify SetSpringParameters with valid convergeParams
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetSpringParametersConverge001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    EXPECT_TRUE(anim->convergeParams_.has_value());
    EXPECT_FLOAT_EQ(anim->convergeParams_->convergeProgressThreshold_, 0.5f);
}

/**
 * @tc.name: SetSpringParametersConverge002
 * @tc.desc: Verify SetSpringParameters rejects invalid convergeParams
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetSpringParametersConverge002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    // NaN progressThreshold
    ConvergeParams params1;
    params1.convergeProgressThreshold_ = NAN;
    params1.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params1);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // progressThreshold < 0
    ConvergeParams params2;
    params2.convergeProgressThreshold_ = -0.5f;
    params2.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params2);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // progressThreshold > 1
    ConvergeParams params3;
    params3.convergeProgressThreshold_ = 1.5f;
    params3.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params3);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // responseFactor < 0
    ConvergeParams params4;
    params4.convergeProgressThreshold_ = 0.5f;
    params4.convergeResponseFactor_ = -0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params4);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // NaN responseFactor
    ConvergeParams params5;
    params5.convergeProgressThreshold_ = 0.5f;
    params5.convergeResponseFactor_ = NAN;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params5);
    EXPECT_FALSE(anim->convergeParams_.has_value());
}

/**
 * @tc.name: SetSpringParametersConverge003
 * @tc.desc: Verify SetSpringParameters skips when both values are 1.0
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetSpringParametersConverge003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    ConvergeParams params;
    params.convergeProgressThreshold_ = 1.0f;
    params.convergeResponseFactor_ = 1.0f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    EXPECT_FALSE(anim->convergeParams_.has_value());
}

/**
 * @tc.name: IsStartConverging001
 * @tc.desc: Verify IsStartConverging for underdamped model (amplitude-based)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, IsStartConverging001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f);
    anim->OnInitialize(0);
    anim->prevMappedTime_ = 0.0f;

    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->convergeParams_ = params;

    // Underdamped, time=0 -> expCoeffDecay=1 -> 1-1=0 < 0.5 -> false
    EXPECT_FALSE(anim->IsStartConverging(0.0f));
    // displacement >= threshold -> true (progress-based check for underdamped also uses amplitude)
    // At large time, 1-expCoeffDecay approaches 1 >= 0.5 -> true
    anim->prevMappedTime_ = 10.0f;
    EXPECT_TRUE(anim->IsStartConverging(0.5f));
}

/**
 * @tc.name: IsStartConverging002
 * @tc.desc: Verify IsStartConverging for critical/overdamped model (progress-based)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, IsStartConverging002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 1.0f, 0.0f); // critical damping
    anim->OnInitialize(0);

    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->convergeParams_ = params;

    // displacement >= 0.5 -> true
    EXPECT_TRUE(anim->IsStartConverging(0.6f));
    // displacement < 0.5 -> false
    EXPECT_FALSE(anim->IsStartConverging(0.3f));
}

/**
 * @tc.name: IsConvergeCloseToTarget001
 * @tc.desc: Verify IsConvergeCloseToTarget checks displacement distance
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, IsConvergeCloseToTarget001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->endThreshold_ = 0.01f;

    // displacement 0.999 -> |0.999-1|=0.001 <= 0.01 -> true
    EXPECT_TRUE(anim->IsConvergeCloseToTarget(0.999f));
    // displacement 0.5 -> |0.5-1|=0.5 > 0.01 -> false
    EXPECT_FALSE(anim->IsConvergeCloseToTarget(0.5f));
}

/**
 * @tc.name: IsConvergeEnd001
 * @tc.desc: Verify IsConvergeEnd combines WillOverShoot and IsConvergeCloseToTarget
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, IsConvergeEnd001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f); // underdamped -> WillOverShoot=false
    anim->OnInitialize(0);
    anim->endThreshold_ = 0.01f;

    // underdamped: WillOverShoot=false, displacement close to 1 -> true
    EXPECT_TRUE(anim->IsConvergeEnd(0.999f));
    // displacement far from 1 -> false
    EXPECT_FALSE(anim->IsConvergeEnd(0.5f));
}

/**
 * @tc.name: IsConvergeEnd002
 * @tc.desc: Verify IsConvergeEnd returns false when WillOverShoot is true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, IsConvergeEnd002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    // critical damping with positive initial velocity -> WillOverShoot=true
    anim->SetSpringParameters(1.0f, 1.0f, 10.0f);
    anim->OnInitialize(0);
    anim->endThreshold_ = 0.01f;

    // WillOverShoot=true -> IsConvergeEnd returns false even when close to target
    EXPECT_FALSE(anim->IsConvergeEnd(0.999f));
    EXPECT_FALSE(anim->IsConvergeEnd(0.5f));
}

/**
 * @tc.name: CheckStartConverge001
 * @tc.desc: Verify CheckStartConverge does nothing without convergeParams
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CheckStartConverge001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f);
    anim->OnInitialize(0);

    anim->CheckStartConverge();
    EXPECT_FALSE(anim->isConverging_);
}

/**
 * @tc.name: CheckStartConverge002
 * @tc.desc: Verify CheckStartConverge returns early when already converging
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CheckStartConverge002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f);
    anim->OnInitialize(0);

    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->convergeParams_ = params;
    anim->isConverging_ = true;
    anim->lastConvergeTime_ = 1.0f;
    anim->CheckStartConverge();
    EXPECT_FLOAT_EQ(anim->lastConvergeTime_, 1.0f);
}

/**
 * @tc.name: CheckStartConverge003
 * @tc.desc: Verify CheckStartConverge starts convergence when IsStartConverging returns true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CheckStartConverge003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    anim->OnInitialize(0);
    // Large prevMappedTime_ → exp(coeffDecay_ * time) ≈ 0 → 1 - 0 >= 0.5 → true
    anim->prevMappedTime_ = 10.0f;
    anim->CheckStartConverge();
    EXPECT_TRUE(anim->isConverging_);
    EXPECT_FLOAT_EQ(anim->lastConvergeTime_, 10.0f);
    EXPECT_GT(anim->endThreshold_, 0.0f);
}

/**
 * @tc.name: CheckStartConverge004
 * @tc.desc: Verify CheckStartConverge does nothing when IsStartConverging returns false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CheckStartConverge004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    anim->OnInitialize(0);
    // Small prevMappedTime_ -> 1 - exp(coeffDecay_ * 0) = 0 < 0.5 -> false
    anim->prevMappedTime_ = 0.0f;
    anim->CheckStartConverge();
    EXPECT_FALSE(anim->isConverging_);
}

/**
 * @tc.name: CheckConvergeStatus001
 * @tc.desc: Verify CheckConvergeStatus returns false when not converging
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CheckConvergeStatus001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->isConverging_ = false;
    EXPECT_FALSE(anim->CheckConvergeStatus(0.5f, 1.0f));
}

/**
 * @tc.name: CheckConvergeStatus002
 * @tc.desc: Verify CheckConvergeStatus updates lastConvergeTime when not converged
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CheckConvergeStatus002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f);
    anim->OnInitialize(0);

    anim->isConverging_ = true;
    anim->lastConvergeTime_ = 0.0f;
    anim->endThreshold_ = 0.01f;
    // displacement 0.5 -> not close to target -> false, but updates lastConvergeTime
    EXPECT_FALSE(anim->CheckConvergeStatus(0.5f, 2.0f));
    EXPECT_FLOAT_EQ(anim->lastConvergeTime_, 2.0f);
}

/**
 * @tc.name: CheckConvergeStatus003
 * @tc.desc: Verify CheckConvergeStatus returns true when IsConvergeEnd is satisfied
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, CheckConvergeStatus003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f); // underdamped → WillOverShoot=false
    anim->OnInitialize(0);
    anim->isConverging_ = true;
    anim->endThreshold_ = 0.5f; // |0.8-1.0|=0.2 <= 0.5 → IsConvergeCloseToTarget true
    EXPECT_TRUE(anim->CheckConvergeStatus(0.8f, 1.0f));
}

/**
 * @tc.name: GetSpringVelocity001
 * @tc.desc: Verify GetSpringVelocity computes numerical derivative
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, GetSpringVelocity001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f);
    anim->OnInitialize(0);

    // Should compute velocity without crashing
    float velocity = anim->GetSpringVelocity(0.5f);
    EXPECT_TRUE(std::isfinite(velocity));
}

/**
 * @tc.name: GetSpringVelocity002
 * @tc.desc: Verify GetSpringVelocity returns correct derivative value matching initial velocity
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, GetSpringVelocity002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    // normalizedInitialVelocity = 1.0 -> initialVelocity_ = initialOffset_ * (-1.0) = 1.0
    anim->SetSpringParameters(1.0f, 0.5f, 1.0f);
    anim->OnInitialize(0);
    // At t=0, the analytical velocity equals initialVelocity_ = 1.0
    // GetSpringVelocity uses float numerical differentiation with TIME_INTERVAL=1e-6,
    // which causes catastrophic cancellation when subtracting two near-equal float values.
    float velocity = anim->GetSpringVelocity(0.0f);
    EXPECT_NEAR(velocity, 1.0f, 0.05f);
}

/**
 * @tc.name: ProcessOnRepeatFinish001
 * @tc.desc: Verify ProcessOnRepeatFinish resets convergence state
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, ProcessOnRepeatFinish001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->isConverging_ = true;
    anim->lastConvergeTime_ = 5.0f;
    anim->ProcessOnRepeatFinish();
    EXPECT_FALSE(anim->isConverging_);
    EXPECT_FLOAT_EQ(anim->lastConvergeTime_, 0.0f);
}

/**
 * @tc.name: OnAnimateConverge001
 * @tc.desc: Verify OnAnimate returns false when valueEstimator is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimateConverge001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    // valueEstimator_ is null by default
    EXPECT_FALSE(anim->OnAnimate(0.5f));
}

/**
 * @tc.name: OnAnimateConverge002
 * @tc.desc: Verify OnAnimate returns false for GetPropertyId == 0
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimateConverge002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderInterpolatingSpringAnimation>(ANIMATION_ID, 0, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f);
    anim->OnInitialize(0);
    EXPECT_FALSE(anim->OnAnimate(0.5f));
}

/**
 * @tc.name: OnAnimateConverge003
 * @tc.desc: Verify OnAnimate returns true when fraction is 1.0
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimateConverge003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    bool result = animMock->RSRenderInterpolatingSpringAnimation::OnAnimate(1.0f);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: OnAnimateConverge004
 * @tc.desc: Verify OnAnimate with isConverging triggers UpdateSpringConvergeParameters
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimateConverge004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // Manually set converging to trigger UpdateSpringConvergeParameters path
    animMock->isConverging_ = true;
    animMock->lastConvergeTime_ = 0.0f;
    bool result = animMock->RSRenderInterpolatingSpringAnimation::OnAnimate(0.5f);
    EXPECT_FALSE(result); // not converged yet
}

/**
 * @tc.name: OnAnimateConverge005
 * @tc.desc: Verify OnAnimate returns true when convergence is complete
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnAnimateConverge005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    animMock->isConverging_ = true;
    animMock->lastConvergeTime_ = 0.0f;
    animMock->endThreshold_ = 10.0f; // large threshold → IsConvergeCloseToTarget always true
    bool result = animMock->RSRenderInterpolatingSpringAnimation::OnAnimate(0.5f);
    EXPECT_TRUE(result);
}
} // namespace Rosen
} // namespace OHOS