/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest Marshalling001 start";
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
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest Unmarshalling001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

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
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest Unmarshalling001 end";
}

/**
 * @tc.name: SetFraction001
 * @tc.desc: Verify the SetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest SetFraction001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

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
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest SetFraction001 end";
}

/**
 * @tc.name: OnInitialize001
 * @tc.desc: Verify the OnInitialize
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, OnInitialize001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest OnInitialize001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f,
        PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    EXPECT_TRUE(renderInterpolatingSpringAnimation != nullptr);
    renderInterpolatingSpringAnimation->OnInitialize(100);
    renderInterpolatingSpringAnimation->Start();
    EXPECT_TRUE(renderInterpolatingSpringAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest OnInitialize001 end";
}

/**
 * @tc.name: SetZeroThreshold001
 * @tc.desc: Verify funciton SetZeroThreshold
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInterpolatingSpringAnimationTest, SetZeroThreshold001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest SetZeroThreshold001 start";
    auto property =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property1 =
        std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);
    auto property2 =
        std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, PROPERTY_ID, RSRenderPropertyType::PROPERTY_FLOAT);

    auto renderInterpolatingSpringAnimation = std::make_shared<RSRenderInterpolatingSpringAnimationMock>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderInterpolatingSpringAnimation->SetZeroThreshold(-0.5f);
    renderInterpolatingSpringAnimation->SetZeroThreshold(0.5f);
    GTEST_LOG_(INFO) << "RSRenderInterpolatingSpringAnimationTest SetZeroThreshold001 end";
}
} // namespace Rosen
} // namespace OHOS