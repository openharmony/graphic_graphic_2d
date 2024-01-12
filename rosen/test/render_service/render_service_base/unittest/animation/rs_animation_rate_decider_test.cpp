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
 * @tc.name: MakeDecision
 * @tc.desc: Test MakeDecision
 * @tc.type:FUNC
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
} // namespace OHOS::Rosen