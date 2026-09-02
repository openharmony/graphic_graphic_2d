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

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_property_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderPropertyAnimationMock : public RSRenderPropertyAnimation {
public:
    RSRenderPropertyAnimationMock(
        AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue)
        : RSRenderPropertyAnimation(id, propertyId, originValue)
    {}
    void RebuildPropertyValue(float fraction) override {}
};

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
    bool OnAnimate(float fraction)
    {
        RSRenderSpringAnimation::OnAnimate(fraction);
        return false;
    }
    void OnAttach()
    {
        RSRenderSpringAnimation::OnAttach();
    }
    void OnDetach()
    {
        RSRenderSpringAnimation::OnDetach();
    }
    void OnInitialize(int64_t time, bool isCustom = false)
    {
        RSRenderSpringAnimation::OnInitialize(time, isCustom);
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
    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimationMock>(
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
 * @tc.name: Unmarshalling004
 * @tc.desc: Verify Unmarshalling returns null when response_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = NAN;
    anim->dampingRatio_ = 1.0f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling005
 * @tc.desc: Verify Unmarshalling returns null when zeroThreshold_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = INFINITY;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling006
 * @tc.desc: Verify Unmarshalling returns null when startValue_ is INVALID type
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling006, TestSize.Level1)
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

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling007
 * @tc.desc: Verify Unmarshalling returns null when endValue_ is INVALID but startValue_ valid
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling007, TestSize.Level1)
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

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling008
 * @tc.desc: Verify Unmarshalling returns null when zeroThreshold_ is negative
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling008, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = -1.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling009
 * @tc.desc: Verify Unmarshalling returns null when dampingRatio_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling009, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = NAN;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling010
 * @tc.desc: Verify Unmarshalling returns null when minimumAmplitudeRatio_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling010, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = INFINITY;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling011
 * @tc.desc: Verify Unmarshalling returns null when response_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling011, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = INFINITY;
    anim->dampingRatio_ = 1.0f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling012
 * @tc.desc: Verify Unmarshalling returns null when dampingRatio_ is Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling012, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = INFINITY;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling013
 * @tc.desc: Verify Unmarshalling returns null when minimumAmplitudeRatio_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling013, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = NAN;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Attach001
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
 * @tc.name: SetZeroThreshold002
 * @tc.desc: Verify SetZeroThreshold rejects NaN and Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetZeroThreshold002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation =
        std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    renderSpringAnimation->SetZeroThreshold(NAN);
    EXPECT_FALSE(renderSpringAnimation->needLogicallyFinishCallback_);

    renderSpringAnimation->needLogicallyFinishCallback_ = false;
    renderSpringAnimation->SetZeroThreshold(INFINITY);
    EXPECT_FALSE(renderSpringAnimation->needLogicallyFinishCallback_);

    renderSpringAnimation->needLogicallyFinishCallback_ = false;
    renderSpringAnimation->SetZeroThreshold(-1.0f);
    EXPECT_FALSE(renderSpringAnimation->needLogicallyFinishCallback_);
}

/**
 * @tc.name: SetSpringParameters002
 * @tc.desc: Verify SetSpringParameters rejects NaN and Inf
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetSpringParameters002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->SetSpringParameters(NAN, 1.0f, 1.0f);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->SetSpringParameters(INFINITY, 1.0f, 1.0f);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->SetSpringParameters(1.0f, NAN, 1.0f);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->SetSpringParameters(1.0f, INFINITY, 1.0f);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->SetSpringParameters(1.0f, 1.0f, NAN);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->SetSpringParameters(1.0f, 1.0f, INFINITY);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->RSRenderSpringAnimation::SetSpringParameters(1.0f, 1.0f, 1.0f, NAN);
    EXPECT_EQ(anim->response_, 0.0f);

    anim->RSRenderSpringAnimation::SetSpringParameters(1.0f, 1.0f, 1.0f, INFINITY);
    EXPECT_EQ(anim->response_, 0.0f);
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
    EXPECT_EQ(out, "Type:RSRenderSpringAnimation, ModifierType: INVALID, StartValue: , EndValue: ");
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
    renderNode->AddAnimation(renderSpringAnimation);
    renderSpringAnimation->OnAttach();
    auto animationManager = renderNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    EXPECT_TRUE(animationManager->QuerySpringAnimation(PROPERTY_ID) != nullptr);
    renderSpringAnimation->OnDetach();
    EXPECT_TRUE(animationManager->QuerySpringAnimation(PROPERTY_ID) == nullptr);
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

    // Restore animationScale to default value
    renderSpringAnimation->animationFraction_.SetAnimationScale(1.0f);
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
    renderNode->AddAnimation(renderSpringAnimation);
    renderSpringAnimation->OnAttach();
    auto animationManager = renderNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    EXPECT_TRUE(animationManager->QuerySpringAnimation(PROPERTY_ID) != nullptr);

    renderSpringAnimation->CallLogicallyFinishCallback();
    EXPECT_TRUE(RSMessageProcessor::Instance().HasTransaction());
}

/**
 * @tc.name: OnAttach002
 * @tc.desc: Verify OnAttach with non-null animationManager registers spring animation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAttach002, TestSize.Level1)
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
    // Make animationManager_ non-null by adding an animation
    renderNode->AddAnimation(renderSpringAnimation);
    auto animationManager = renderNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    // OnAttach with non-null animationManager: will register spring animation
    renderSpringAnimation->OnAttach();
    EXPECT_TRUE(animationManager->QuerySpringAnimation(PROPERTY_ID) != nullptr);
}

/**
 * @tc.name: OnDetach002
 * @tc.desc: Verify OnDetach with non-null animationManager unregisters spring animation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnDetach002, TestSize.Level1)
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
    renderNode->AddAnimation(renderSpringAnimation);
    auto animationManager = renderNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    renderSpringAnimation->OnAttach();
    EXPECT_TRUE(animationManager->QuerySpringAnimation(PROPERTY_ID) != nullptr);
    // OnDetach with non-null animationManager: will unregister spring animation
    renderSpringAnimation->OnDetach();
    EXPECT_TRUE(animationManager->QuerySpringAnimation(PROPERTY_ID) == nullptr);
}

/**
 * @tc.name: RebuildPropertyValue001
 * @tc.desc: Verify RebuildPropertyValue with null property
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, RebuildPropertyValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->property_ = nullptr;
    renderSpringAnimation->RebuildPropertyValue(0.5f);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue001 end";
}

/**
 * @tc.name: RebuildPropertyValue002
 * @tc.desc: Verify RebuildPropertyValue with null startValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, RebuildPropertyValue002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->startValue_ = nullptr;
    renderSpringAnimation->RebuildPropertyValue(0.5f);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue002 end";
}

/**
 * @tc.name: RebuildPropertyValue003
 * @tc.desc: Verify RebuildPropertyValue with null endValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, RebuildPropertyValue003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue003 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->endValue_ = nullptr;
    renderSpringAnimation->RebuildPropertyValue(0.5f);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue003 end";
}

/**
 * @tc.name: RebuildPropertyValue004
 * @tc.desc: Verify RebuildPropertyValue with null springValueEstimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, RebuildPropertyValue004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue004 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->springValueEstimator_ = nullptr;
    renderSpringAnimation->RebuildPropertyValue(0.5f);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue004 end";
}

/**
 * @tc.name: RebuildPropertyValue005
 * @tc.desc: Verify RebuildPropertyValue with all valid parameters
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, RebuildPropertyValue005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue005 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->SetDuration(300);
    renderSpringAnimation->property_ = property;
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->RebuildPropertyValue(0.5f);
    EXPECT_NE(renderSpringAnimation->springValueEstimator_, nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest RebuildPropertyValue005 end";
}

/**
 * @tc.name: OnAnimateVelocityNull001
 * @tc.desc: Verify OnAnimate handles GetPropertyVelocity returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateVelocityNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimateVelocityNull001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->SetDuration(300);
    renderSpringAnimation->property_ = property;
    renderSpringAnimation->lastValue_ = property;
    renderSpringAnimation->needLogicallyFinishCallback_ = true;
    renderSpringAnimation->SetRepeatCount(1);
    renderSpringAnimation->animationFraction_.currentRepeatCount_ = 0;
    renderSpringAnimation->InitValueEstimator();
    auto springEstimator =
        std::static_pointer_cast<RSSpringValueEstimator<float>>(renderSpringAnimation->springValueEstimator_);
    springEstimator->springModel_ = nullptr;
    renderSpringAnimation->OnAnimate(0.5f);
    EXPECT_TRUE(renderSpringAnimation->needLogicallyFinishCallback_);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimateVelocityNull001 end";
}

/**
 * @tc.name: OnAnimateVelocityNotNull001
 * @tc.desc: Verify OnAnimate proceeds when velocity is not nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateVelocityNotNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimateVelocityNotNull001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderSpringAnimation->SetDuration(300);
    renderSpringAnimation->property_ = property;
    renderSpringAnimation->lastValue_ = property;
    renderSpringAnimation->needLogicallyFinishCallback_ = true;
    renderSpringAnimation->SetRepeatCount(1);
    renderSpringAnimation->animationFraction_.currentRepeatCount_ = 0;
    renderSpringAnimation->SetSpringParameters(1.0f, 1.0f);
    renderSpringAnimation->InitValueEstimator();
    renderSpringAnimation->OnAnimate(0.5f);
    EXPECT_FALSE(renderSpringAnimation->needLogicallyFinishCallback_);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimateVelocityNotNull001 end";
}

/**
 * @tc.name: Unmarshalling014
 * @tc.desc: Verify Unmarshalling returns null when zeroThreshold_ is NaN
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling014, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 1.0f;
    anim->dampingRatio_ = 1.0f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = NAN;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: Unmarshalling015
 * @tc.desc: Verify Unmarshalling returns valid animation with haveInitialVelocity=false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling015, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID);

    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->response_ = 0.55f;
    anim->dampingRatio_ = 0.825f;
    anim->blendDuration_ = 1000;
    anim->minimumAmplitudeRatio_ = 0.001f;
    anim->needLogicallyFinishCallback_ = false;
    anim->zeroThreshold_ = 0.0f;
    anim->initialVelocity_ = nullptr;

    Parcel parcel;
    anim->Marshalling(parcel);

    auto* result = RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: SetSpringParametersConverge001
 * @tc.desc: Verify SetSpringParameters with valid convergeParams
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetSpringParametersConverge001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params);
    EXPECT_TRUE(anim->convergeParams_.has_value());
    EXPECT_FLOAT_EQ(anim->convergeParams_->convergeProgressThreshold_, 0.5f);
}

/**
 * @tc.name: SetSpringParametersConverge002
 * @tc.desc: Verify SetSpringParameters rejects invalid convergeParams
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetSpringParametersConverge002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    // NaN progressThreshold
    ConvergeParams params1;
    params1.convergeProgressThreshold_ = NAN;
    params1.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params1);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // progressThreshold < 0
    ConvergeParams params2;
    params2.convergeProgressThreshold_ = -0.5f;
    params2.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params2);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // progressThreshold > 1
    ConvergeParams params3;
    params3.convergeProgressThreshold_ = 1.5f;
    params3.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params3);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // NaN responseFactor
    ConvergeParams params4;
    params4.convergeProgressThreshold_ = 0.5f;
    params4.convergeResponseFactor_ = NAN;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params4);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // responseFactor < 0
    ConvergeParams params5;
    params5.convergeProgressThreshold_ = 0.5f;
    params5.convergeResponseFactor_ = -0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params5);
    EXPECT_FALSE(anim->convergeParams_.has_value());
}

/**
 * @tc.name: SetSpringParametersConverge003
 * @tc.desc: Verify SetSpringParameters skips when both converge values are 1.0 (default)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetSpringParametersConverge003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    // Both being 1.0 is the default value -> should not set convergeParams_
    ConvergeParams params;
    params.convergeProgressThreshold_ = 1.0f;
    params.convergeResponseFactor_ = 1.0f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params);
    EXPECT_FALSE(anim->convergeParams_.has_value());

    // Either being 1.0 is treated as default and should not set convergeParams_
    ConvergeParams params2;
    params2.convergeProgressThreshold_ = 1.0f;
    params2.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, params2);
    EXPECT_FALSE(anim->convergeParams_.has_value());
}

/**
 * @tc.name: SetSpringParametersConverge004
 * @tc.desc: Verify SetSpringParameters with nullopt convergeParams
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetSpringParametersConverge004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->SetSpringParameters(1.0f, 0.5f, 0.5f, 0.001f, std::nullopt);
    EXPECT_FALSE(anim->convergeParams_.has_value());
}

/**
 * @tc.name: ProcessOnRepeatFinish001
 * @tc.desc: Verify ProcessOnRepeatFinish resets convergence state
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, ProcessOnRepeatFinish001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    // Set convergence state manually
    anim->isConverging_ = true;
    anim->lastConvergeTime_ = 5.0f;
    anim->ProcessOnRepeatFinish();
    EXPECT_FALSE(anim->isConverging_);
    EXPECT_FLOAT_EQ(anim->lastConvergeTime_, 0.0f);
}

/**
 * @tc.name: IsConvergeCloseToTarget001
 * @tc.desc: Verify IsConvergeCloseToTarget with null endThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, IsConvergeCloseToTarget001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    // endThreshold_ is null by default
    EXPECT_FALSE(anim->IsConvergeCloseToTarget());
}

/**
 * @tc.name: IsConvergeCloseToTarget002
 * @tc.desc: Verify IsConvergeCloseToTarget with null endValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, IsConvergeCloseToTarget002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);

    // endThreshold_ is null by default after init
    EXPECT_FALSE(animMock->IsConvergeCloseToTarget());

    // Set endThreshold_ to valid, but endValue_ to null
    animMock->endThreshold_ = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f);
    animMock->endValue_ = nullptr;
    EXPECT_FALSE(animMock->IsConvergeCloseToTarget());
}

/**
 * @tc.name: IsConvergeCloseToTarget003
 * @tc.desc: Verify IsConvergeCloseToTarget with valid endThreshold_ and normal comparison
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, IsConvergeCloseToTarget003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // GetAnimationProperty returns lastValue_ (0.0f, the origin value)
    // IsAbsNearEqual: |0.0 - 1.0| = 1.0 <= |1.0| → true
    animMock->endThreshold_ = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    EXPECT_TRUE(animMock->IsConvergeCloseToTarget());
}

/**
 * @tc.name: IsConvergeCloseToTarget004
 * @tc.desc: Verify IsConvergeCloseToTarget returns false when value is genuinely far from target
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, IsConvergeCloseToTarget004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // GetAnimationProperty returns lastValue_ (0.0f), endValue_ is 1.0f
    // Small endThreshold_ -> |0.0 - 1.0| = 1.0 > 0.001 -> false
    animMock->endThreshold_ = std::make_shared<RSRenderAnimatableProperty<float>>(0.001f);
    EXPECT_FALSE(animMock->IsConvergeCloseToTarget());
}

/**
 * @tc.name: CheckStartConverge001
 * @tc.desc: Verify CheckStartConverge does nothing without convergeParams
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckStartConverge001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    // No convergeParams -> should return immediately, isConverging_ stays false
    anim->CheckStartConverge();
    EXPECT_FALSE(anim->isConverging_);
}

/**
 * @tc.name: CheckStartConverge002
 * @tc.desc: Verify CheckStartConverge returns early when already converging
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckStartConverge002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    anim->isConverging_ = true; // already converging
    anim->lastConvergeTime_ = 1.0f;
    anim->CheckStartConverge();
    // Should return early, lastConvergeTime_ unchanged
    EXPECT_FLOAT_EQ(anim->lastConvergeTime_, 1.0f);
}

/**
 * @tc.name: CheckStartConverge003
 * @tc.desc: Verify CheckStartConverge starts convergence when IsStartConverging returns true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckStartConverge003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    animMock->RSRenderSpringAnimation::SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // Large prevMappedTime_ → IsStartConverging returns true
    animMock->prevMappedTime_ = 10.0f;
    animMock->CheckStartConverge();
    EXPECT_TRUE(animMock->isConverging_);
    EXPECT_FLOAT_EQ(animMock->lastConvergeTime_, 10.0f);
    EXPECT_NE(animMock->endThreshold_, nullptr);
}

/**
 * @tc.name: CheckStartConverge004
 * @tc.desc: Verify CheckStartConverge does nothing when IsStartConverging returns false
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckStartConverge004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    animMock->RSRenderSpringAnimation::SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // Small prevMappedTime_ -> IsStartConverging returns false
    animMock->prevMappedTime_ = 0.0f;
    animMock->CheckStartConverge();
    EXPECT_FALSE(animMock->isConverging_);
}

/**
 * @tc.name: CheckStartConverge005
 * @tc.desc: Verify CheckStartConverge enters blendDuration_ branch when converging starts during blend
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckStartConverge005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    // Non-zero blendDuration -> blendDuration_ != 0 after OnInitialize
    animMock->RSRenderSpringAnimation::SetSpringParameters(1.0f, 0.5f, 1.0f, 0.001f, params);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // Large prevMappedTime_ -> IsStartConverging returns true
    animMock->prevMappedTime_ = 10.0f;
    animMock->CheckStartConverge();
    // isConverging_ set to true, blendDuration_ branch calls SetDuration + OnInitialize
    EXPECT_TRUE(animMock->isConverging_);
    EXPECT_FLOAT_EQ(animMock->lastConvergeTime_, 10.0f);
    EXPECT_NE(animMock->endThreshold_, nullptr);
}

/**
 * @tc.name: CheckConvergeStatus001
 * @tc.desc: Verify CheckConvergeStatus returns false when not converging
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckConvergeStatus001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->isConverging_ = false;
    EXPECT_FALSE(anim->CheckConvergeStatus(1.0f));
}

/**
 * @tc.name: CheckConvergeStatus002
 * @tc.desc: Verify CheckConvergeStatus updates lastConvergeTime when not converged
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckConvergeStatus002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);

    animMock->isConverging_ = true;
    animMock->lastConvergeTime_ = 0.0f;
    // endThreshold_ is null -> IsConvergeCloseToTarget returns false -> IsConvergeEnd returns false
    EXPECT_FALSE(animMock->CheckConvergeStatus(2.0f));
    // lastConvergeTime_ should be updated
    EXPECT_FLOAT_EQ(animMock->lastConvergeTime_, 2.0f);
}

/**
 * @tc.name: CheckConvergeStatus003
 * @tc.desc: Verify CheckConvergeStatus returns true when IsConvergeEnd is satisfied
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, CheckConvergeStatus003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    animMock->isConverging_ = true;
    animMock->endThreshold_ = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    // GetAnimationProperty returns lastValue_ (0.0f, the origin value)
    // IsAbsNearEqual: |0.0 - 1.0| = 1.0 <= |1.0| → true
    // WillOverShoot: underdamped → false → !false = true
    // IsConvergeEnd: true && true → true
    EXPECT_TRUE(animMock->CheckConvergeStatus(1.0f));
}

/**
 * @tc.name: OnAnimateConverge001
 * @tc.desc: Verify OnAnimate returns false for GetPropertyId == 0
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateConverge001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID_3, property, property1, property2);
    anim->SetSpringParameters(1.0f, 0.5f, 0.0f);
    EXPECT_FALSE(anim->OnAnimate(0.5f));
}

/**
 * @tc.name: OnAnimateConverge002
 * @tc.desc: Verify OnAnimate returns false when springValueEstimator is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateConverge002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    // springValueEstimator_ is null by default (not initialized)
    EXPECT_FALSE(anim->OnAnimate(0.5f));
}

/**
 * @tc.name: OnAnimateConverge003
 * @tc.desc: Verify OnAnimate returns true when fraction is 1.0 and not converging
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateConverge003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // fraction == 1.0 without converging -> should return true
    bool result = animMock->RSRenderSpringAnimation::OnAnimate(1.0f);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: OnAnimateConverge004
 * @tc.desc: Verify OnAnimate returns true when fraction is 1.0 and converging
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateConverge004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    animMock->isConverging_ = true;
    animMock->lastConvergeTime_ = 0.1f;
    // fraction == 1.0 with converging -> should return true
    bool result = animMock->RSRenderSpringAnimation::OnAnimate(1.0f);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: OnAnimateConverge005
 * @tc.desc: Verify OnAnimate with isConverging triggers UpdateSpringConvergeParameters
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateConverge005, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    animMock->RSRenderSpringAnimation::SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    // Manually set converging to trigger UpdateSpringConvergeParameters path
    animMock->isConverging_ = true;
    animMock->lastConvergeTime_ = 0.0f;
    // Non-1.0 fraction with isConverging_ -> calls UpdateSpringConvergeParameters
    bool result = animMock->RSRenderSpringAnimation::OnAnimate(0.5f);
    EXPECT_FALSE(result); // not converged yet
}

/**
 * @tc.name: OnAnimateConverge006
 * @tc.desc: Verify OnAnimate returns true when convergence is complete
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnAnimateConverge006, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    ConvergeParams params;
    params.convergeProgressThreshold_ = 0.5f;
    params.convergeResponseFactor_ = 0.5f;
    animMock->RSRenderSpringAnimation::SetSpringParameters(1.0f, 0.5f, 0.0f, 0.001f, params);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);
    animMock->isConverging_ = true;
    animMock->lastConvergeTime_ = 0.0f;
    animMock->endThreshold_ = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    bool result = animMock->RSRenderSpringAnimation::OnAnimate(0.5f);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: OnInitializeConverge001
 * @tc.desc: Verify OnInitialize skips blend when isConverging_ is true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnInitializeConverge001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 1.0f); // blendDuration = 1.0 → blendDuration_ != 0
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->isConverging_ = true; // blendDuration_ && !isConverging_ → false → blend skipped
    animMock->OnInitialize(0);
    // blend skipped → RSRenderPropertyAnimation::OnInitialize called → needInitialize_ = false
    EXPECT_FALSE(animMock->needInitialize_);
}

/**
 * @tc.name: OnInitializeConverge002
 * @tc.desc: Verify OnInitialize with isCustom=true sets isCustom_ and uses animationScale=1.0
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, OnInitializeConverge002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 1.0f); // blendDuration = 1.0 → blendDuration_ != 0
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    // Call OnInitialize with isCustom=true → isCustom_ set, blend branch uses animationScale=1.0
    animMock->OnInitialize(0, true);
    EXPECT_TRUE(animMock->isCustom_);
}

/**
 * @tc.name: IsConvergeEnd001
 * @tc.desc: Verify IsConvergeEnd with underdamped model (WillOverShoot=false)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, IsConvergeEnd001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 0.5f, 0.0f); // underdamped → WillOverShoot=false
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    animMock->OnInitialize(0);

    // endThreshold_ is null → IsConvergeCloseToTarget returns false → IsConvergeEnd false
    EXPECT_FALSE(animMock->IsConvergeEnd());

    // large endThreshold_ → IsConvergeCloseToTarget true, WillOverShoot false → IsConvergeEnd true
    animMock->endThreshold_ = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    EXPECT_TRUE(animMock->IsConvergeEnd());
}

/**
 * @tc.name: IsConvergeEnd002
 * @tc.desc: Verify IsConvergeEnd returns false when WillOverShoot is true (critical damping with velocity)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, IsConvergeEnd002, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animMock =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    animMock->SetSpringParameters(1.0f, 1.0f, 0.0f); // critical damping
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    animMock->Attach(renderNode.get());
    animMock->Start();
    animMock->AttachRenderProperty(property);
    // Set non-zero initial velocity so that WillOverShoot returns true for critical damping.
    // initialVelocity_ must be set before OnInitialize, otherwise OnInitialize overrides it to zero.
    animMock->initialVelocity_ = std::make_shared<RSRenderAnimatableProperty<float>>(10.0f);
    animMock->OnInitialize(0);
    // WillOverShoot=true → IsConvergeEnd returns false even with large threshold
    animMock->endThreshold_ = std::make_shared<RSRenderAnimatableProperty<float>>(100.0f);
    EXPECT_FALSE(animMock->IsConvergeEnd());
}

/**
 * @tc.name: SetSpringParameters003
 * @tc.desc: Verify SetSpringParameters handles blendDuration overflow by setting blendDuration_ to 0
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetSpringParameters003, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->SetSpringParameters(1.0f, 0.5f, 1e30f);
    EXPECT_EQ(anim->blendDuration_, 0u);
}

/**
 * @tc.name: SetSpringParameters004
 * @tc.desc: Verify SetSpringParameters handles negative blendDuration by setting blendDuration_ to 0
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetSpringParameters004, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    anim->SetSpringParameters(1.0f, 0.5f, -1e10f);
    EXPECT_EQ(anim->blendDuration_, 0u);
}

/**
 * @tc.name: InheritSpringAnimationTypeCheck001
 * @tc.desc: Verify InheritSpringAnimation rejects non-spring prevAnimation type
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringAnimationTypeCheck001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    anim->SetSpringParameters(0.5f, 0.5f, 0.0f);

    auto curveProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto curveProperty1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto curveProperty2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto curveAnim = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID + 1, PROPERTY_ID, curveProperty, curveProperty1, curveProperty2);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    anim->Attach(renderNode.get());
    anim->Start();
    anim->AttachRenderProperty(property);
    anim->InitValueEstimator();
    anim->OnInitialize(0);

    curveAnim->Attach(renderNode.get());
    curveAnim->Start();

    anim->blendDuration_ = 1000000000;
    int64_t delayTime = 0;
    bool isCustom = false;
    anim->Animate(0, delayTime, isCustom);
    auto prevAnim = std::static_pointer_cast<RSRenderAnimation>(curveAnim);
    anim->InheritSpringAnimation(prevAnim, isCustom);
    EXPECT_EQ(curveAnim->GetType(), RSRenderAnimationType::CURVE_ANIMATION);
    EXPECT_EQ(anim->blendDuration_, 1000000000u);
}

/**
 * @tc.name: InheritSpringBlendDurationCap001
 * @tc.desc: Verify InheritSpringAnimation caps blendDuration_ to prevAnimation duration
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringBlendDurationCap001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    // Non-zero blendDuration, response differs from prev so none of the 3 branches at 228-237 fire
    anim->SetSpringParameters(0.55f, 0.825f, 100.0f);
    anim->AttachRenderProperty(property);
    anim->InitValueEstimator();
    anim->OnInitialize(0);

    auto prevAnim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID + 1, PROPERTY_ID, property2, property2, property1);
    prevAnim->SetSpringParameters(0.3f, 0.825f, 0.0f);
    prevAnim->AttachRenderProperty(property2);
    prevAnim->InitValueEstimator();
    prevAnim->OnInitialize(0);
    prevAnim->Start();

    uint64_t blendBefore = anim->blendDuration_;
    ASSERT_GT(blendBefore, 0u);

    auto prevAnimBase = std::static_pointer_cast<RSRenderAnimation>(prevAnim);
    anim->InheritSpringAnimation(prevAnimBase, false);

    int64_t prevDurationMs = prevAnim->GetDuration();
    ASSERT_GT(prevDurationMs, 0);
    // 1ms = 1,000,000ns (1e9 * 1e-3)
    uint64_t prevDurationNs = static_cast<uint64_t>(prevDurationMs) * 1000000u;
    ASSERT_GT(prevDurationNs, 0u);
    // Cap should have triggered: blendDuration_ reduced to prevDurationNs
    EXPECT_LT(anim->blendDuration_, blendBefore);
    EXPECT_EQ(anim->blendDuration_, prevDurationNs);
}

/**
 * @tc.name: InheritSpringBlendDurationNoCap001
 * @tc.desc: Verify InheritSpringAnimation does not cap when blendDuration_ <= prevDuration
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, InheritSpringBlendDurationNoCap001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    // Small blendDuration (0.001s = 1ms in ns), response differs from prev
    anim->SetSpringParameters(0.55f, 0.825f, 0.001f);
    anim->AttachRenderProperty(property);
    anim->InitValueEstimator();
    anim->OnInitialize(0);

    auto prevAnim =
        std::make_shared<RSRenderSpringAnimationMock>(ANIMATION_ID + 1, PROPERTY_ID, property2, property2, property1);
    prevAnim->SetSpringParameters(0.3f, 0.825f, 0.0f);
    prevAnim->AttachRenderProperty(property2);
    prevAnim->InitValueEstimator();
    prevAnim->OnInitialize(0);
    prevAnim->Start();

    uint64_t blendBefore = anim->blendDuration_;
    ASSERT_GT(blendBefore, 0u);

    auto prevAnimBase = std::static_pointer_cast<RSRenderAnimation>(prevAnim);
    anim->InheritSpringAnimation(prevAnimBase, false);

    // prevDuration is seconds-level, blendDuration_ is 1ms → no cap
    int64_t prevDurationMs = prevAnim->GetDuration();
    ASSERT_GT(prevDurationMs, 0);
    uint64_t prevDurationNs = static_cast<uint64_t>(prevDurationMs) * 1000000u;
    EXPECT_GT(prevDurationNs, blendBefore);
    // blendDuration_ unchanged: cap not applied
    EXPECT_EQ(anim->blendDuration_, blendBefore);
}

/**
 * @tc.name: GetType001
 * @tc.desc: Verify GetType returns SPRING_ANIMATION
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, GetType001, TestSize.Level1)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto anim = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, startProperty, endProperty);
    EXPECT_EQ(anim->GetType(), RSRenderAnimationType::SPRING_ANIMATION);
}

} // namespace Rosen
} // namespace OHOS
