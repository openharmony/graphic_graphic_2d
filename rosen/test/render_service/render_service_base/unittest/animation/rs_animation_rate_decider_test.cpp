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

#include "animation/rs_animation_rate_decider.h"
#include "modifier/rs_render_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationRateDeciderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationRateDeciderTest::SetUpTestCase() {}
void RSAnimationRateDeciderTest::TearDownTestCase() {}
void RSAnimationRateDeciderTest::SetUp() {}
void RSAnimationRateDeciderTest::TearDown() {}

/**
 * @tc.name: SetEnable
 * @tc.desc: Test SetEnable
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, SetEnable, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    bool enabled = true;
    rsAnimationRateDecider.SetEnable(enabled);
    EXPECT_EQ(rsAnimationRateDecider.isEnabled_, true);
}

/**
 * @tc.name: SetScaleReferenceSize
 * @tc.desc: Test SetScaleReferenceSize
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, SetScaleReferenceSize, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    float width = 0.0f;
    float height = 0.0f;
    rsAnimationRateDecider.SetScaleReferenceSize(width, height);
    EXPECT_EQ(width, 0);
}

/**
 * @tc.name: Reset
 * @tc.desc: Test Reset
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, Reset, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    rsAnimationRateDecider.Reset();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: AddDecisionElement
 * @tc.desc: Test AddDecisionElement
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, AddDecisionElement, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    PropertyId id = 0;
    PropertyValue velocity;
    FrameRateRange range;

    rsAnimationRateDecider.SetEnable(false);
    rsAnimationRateDecider.AddDecisionElement(id, velocity, range);
    EXPECT_EQ(rsAnimationRateDecider.isEnabled_, false);

    rsAnimationRateDecider.SetEnable(true);
    rsAnimationRateDecider.AddDecisionElement(id, velocity, range);
    EXPECT_EQ(rsAnimationRateDecider.isEnabled_, true);

    rsAnimationRateDecider.SetEnable(true);
    velocity = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    range.Set(0, 0, 1);
    rsAnimationRateDecider.AddDecisionElement(id, velocity, range);
    EXPECT_EQ(rsAnimationRateDecider.isEnabled_, true);

    rsAnimationRateDecider.SetEnable(true);
    velocity = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    range.Set(0, 0, 1);
    rsAnimationRateDecider.AddDecisionElement(id, velocity, range);
    EXPECT_EQ(rsAnimationRateDecider.isEnabled_, true);
}

/**
 * @tc.name: MakeDecision
 * @tc.desc: Test MakeDecision
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, MakeDecision, TestSize.Level1)
{
    auto rateDecider_ = std::make_shared<RSAnimationRateDecider>();
    ASSERT_NE(rateDecider_, nullptr);

    rateDecider_->Reset();
    PropertyValue value1 = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    rateDecider_->AddDecisionElement(1, value1, {0, 120, 60});
    PropertyValue value2 = std::make_shared<RSRenderAnimatableProperty<float>>(
        10.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    rateDecider_->AddDecisionElement(1, value2, {0, 120, 60});
    PropertyValue value3 = std::make_shared<RSRenderAnimatableProperty<float>>(
        10.0, 2, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    rateDecider_->AddDecisionElement(2, value3, {0, 120, 120});
    PropertyValue value4 = std::make_shared<RSRenderAnimatableProperty<float>>(
        10.0, 2, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    rateDecider_->AddDecisionElement(2, value4, {0, 0, 0});
    rateDecider_->AddDecisionElement(2, nullptr, {0, 120, 120});
    rateDecider_->AddDecisionElement(3, nullptr, {0, 0, 0});

    rateDecider_->MakeDecision([](const RSPropertyUnit unit, float velocity) -> int32_t { return 90; });
    auto range = rateDecider_->GetFrameRateRange();
    EXPECT_EQ(range.preferred_, 90);
}

/**
 * @tc.name: GetFrameRateRange
 * @tc.desc: Test GetFrameRateRange
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, GetFrameRateRange, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    FrameRateRange frame;
    frame = rsAnimationRateDecider.GetFrameRateRange();
    EXPECT_EQ(frame, frame);
}

/**
 * @tc.name: CalculatePreferredRate
 * @tc.desc: Test CalculatePreferredRate
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, CalculatePreferredRate, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    auto frameRateGetFunc = [this](const RSPropertyUnit unit, float velocity) -> int32_t{
        return 0;
    };
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    RSRenderPropertyType type = RSRenderPropertyType::PROPERTY_VECTOR4F;
    property->SetPropertyType(type);
    int32_t res = rsAnimationRateDecider.CalculatePreferredRate(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);

    type = RSRenderPropertyType::PROPERTY_VECTOR2F;
    property->SetPropertyType(type);
    res = rsAnimationRateDecider.CalculatePreferredRate(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);

    type = RSRenderPropertyType::PROPERTY_FLOAT;
    property->SetPropertyType(type);
    res = rsAnimationRateDecider.CalculatePreferredRate(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);

    type = RSRenderPropertyType::INVALID;
    property->SetPropertyType(type);
    res = rsAnimationRateDecider.CalculatePreferredRate(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: ProcessVector4f
 * @tc.desc: Test ProcessVector4f
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, ProcessVector4f, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    auto frameRateGetFunc = [this](const RSPropertyUnit unit, float velocity) -> int32_t{
        return 0;
    };
    int32_t res = rsAnimationRateDecider.ProcessVector4f(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);

    RSPropertyUnit unit = RSPropertyUnit::PIXEL_POSITION;
    property->SetPropertyUnit(unit);
    res = rsAnimationRateDecider.ProcessVector4f(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: ProcessVector2f
 * @tc.desc: Test ProcessVector2f
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationRateDeciderTest, ProcessVector2f, TestSize.Level1)
{
    RSAnimationRateDecider rsAnimationRateDecider;
    PropertyValue property = std::make_shared<RSRenderAnimatableProperty<float>>(
        0.0, 1, RSRenderPropertyType::PROPERTY_FLOAT, RSPropertyUnit::PIXEL_POSITION);
    auto frameRateGetFunc = [this](const RSPropertyUnit unit, float velocity) -> int32_t{
        return 0;
    };
    RSPropertyUnit unit = RSPropertyUnit::RATIO_SCALE;
    property->SetPropertyUnit(unit);
    int32_t res = rsAnimationRateDecider.ProcessVector2f(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);

    unit = RSPropertyUnit::PIXEL_SIZE;
    property->SetPropertyUnit(unit);
    res = rsAnimationRateDecider.ProcessVector2f(property, frameRateGetFunc);
    EXPECT_EQ(res, 0);
}
} // namespace OHOS::Rosen