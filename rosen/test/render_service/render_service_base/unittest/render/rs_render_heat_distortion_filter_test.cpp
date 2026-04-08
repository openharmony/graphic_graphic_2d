/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "effect/rs_render_filter_base.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderHeatDistortionFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderHeatDistortionFilterTest::SetUpTestCase() {}
void RSRenderHeatDistortionFilterTest::TearDownTestCase() {}
void RSRenderHeatDistortionFilterTest::SetUp() {}
void RSRenderHeatDistortionFilterTest::TearDown() {}

/**
 * @tc.name: HeatDistortionCreateAndType001
 * @tc.desc: Verify heat distortion filter type and factory create
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderHeatDistortionFilterTest, HeatDistortionCreateAndType001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderHeatDistortionFilter>();
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::HEAT_DISTORTION);

    auto factoryFilter = RSNGRenderFilterBase::Create(RSNGEffectType::HEAT_DISTORTION);
    ASSERT_NE(factoryFilter, nullptr);
    EXPECT_EQ(factoryFilter->GetType(), RSNGEffectType::HEAT_DISTORTION);
}

/**
 * @tc.name: HeatDistortionSetterGetter001
 * @tc.desc: Verify setter/getter for heat distortion properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderHeatDistortionFilterTest, HeatDistortionSetterGetter001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderHeatDistortionFilter>();
    ASSERT_NE(filter, nullptr);

    constexpr float intensity = 1.5f;
    constexpr float riseSpeed = 1.2f;
    constexpr float noiseScale = 2.5f;
    constexpr float noiseSpeed = 0.7f;
    constexpr float riseWeight = 0.35f;
    constexpr float timePhase = 0.45f;

    filter->Setter<HeatDistortionIntensityRenderTag>(intensity);
    filter->Setter<HeatDistortionRiseSpeedRenderTag>(riseSpeed);
    filter->Setter<HeatDistortionNoiseScaleRenderTag>(noiseScale);
    filter->Setter<HeatDistortionNoiseSpeedRenderTag>(noiseSpeed);
    filter->Setter<HeatDistortionRiseWeightRenderTag>(riseWeight);
    filter->Setter<HeatDistortionTimePhaseRenderTag>(timePhase);

    EXPECT_FLOAT_EQ(filter->Getter<HeatDistortionIntensityRenderTag>()->Get(), intensity);
    EXPECT_FLOAT_EQ(filter->Getter<HeatDistortionRiseSpeedRenderTag>()->Get(), riseSpeed);
    EXPECT_FLOAT_EQ(filter->Getter<HeatDistortionNoiseScaleRenderTag>()->Get(), noiseScale);
    EXPECT_FLOAT_EQ(filter->Getter<HeatDistortionNoiseSpeedRenderTag>()->Get(), noiseSpeed);
    EXPECT_FLOAT_EQ(filter->Getter<HeatDistortionRiseWeightRenderTag>()->Get(), riseWeight);
    EXPECT_FLOAT_EQ(filter->Getter<HeatDistortionTimePhaseRenderTag>()->Get(), timePhase);
}

/**
 * @tc.name: HeatDistortionGenerateGEVisualEffect001
 * @tc.desc: Verify heat distortion can generate GE visual effect
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderHeatDistortionFilterTest, HeatDistortionGenerateGEVisualEffect001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderHeatDistortionFilter>();
    ASSERT_NE(filter, nullptr);

    filter->Setter<HeatDistortionIntensityRenderTag>(1.0f);
    filter->Setter<HeatDistortionRiseSpeedRenderTag>(1.0f);
    filter->Setter<HeatDistortionNoiseScaleRenderTag>(1.0f);
    filter->Setter<HeatDistortionNoiseSpeedRenderTag>(0.5f);
    filter->Setter<HeatDistortionRiseWeightRenderTag>(0.3f);
    filter->Setter<HeatDistortionTimePhaseRenderTag>(0.5f);

    std::shared_ptr<RSNGRenderFilterBase> baseFilter = filter;
    RSNGRenderFilterHelper::GenerateGEVisualEffect(baseFilter);

    ASSERT_NE(baseFilter->geFilter_, nullptr);
    ASSERT_NE(baseFilter->geFilter_->GetImpl(), nullptr);
    EXPECT_EQ(baseFilter->geFilter_->GetImpl()->GetFilterType(),
        Drawing::GEVisualEffectImpl::FilterType::HEAT_DISTORTION);
}
} // namespace OHOS::Rosen
