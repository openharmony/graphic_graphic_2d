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
#include "ui_effect/property/include/rs_ui_filter_base.h"
#include "ui_effect/filter/include/filter_heat_distortion_para.h"
#include "ui_effect/filter/include/filter_blur_bubbles_rise_para.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIFilterBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFilterBaseTest::SetUpTestCase() {}
void RSUIFilterBaseTest::TearDownTestCase() {}
void RSUIFilterBaseTest::SetUp() {}
void RSUIFilterBaseTest::TearDown() {}

/**
 * @tc.name: CreateNGBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGBlurFilter, TestSize.Level1)
{
    auto blurFilter = std::static_pointer_cast<RSNGBlurFilter>(
        RSNGFilterHelper::CreateNGBlurFilter(10.f, 10.f));
    EXPECT_NE(blurFilter, nullptr);
    float blurRadiusX = blurFilter->Getter<BlurRadiusXTag>()->Get();
    float blurRadiusY = blurFilter->Getter<BlurRadiusYTag>()->Get();
    EXPECT_FLOAT_EQ(blurRadiusX, 10.f);
    EXPECT_FLOAT_EQ(blurRadiusY, 10.f);
}

/**
 * @tc.name: CreateNGMaterialBlurFilter
 * @tc.desc: test for RSNGFilterHelper::CreateNGMaterialBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateNGMaterialBlurFilter, TestSize.Level1)
{
    MaterialParam materialParam = { -50, -0.5, -0.5, Color(0x9fff0000), false };
    auto materialBlurFilter = std::static_pointer_cast<RSNGMaterialBlurFilter>(
        RSNGFilterHelper::CreateNGMaterialBlurFilter(materialParam));
    EXPECT_NE(materialBlurFilter, nullptr);
    float radius = materialBlurFilter->Getter<MaterialBlurRadiusTag>()->Get();
    float saturation = materialBlurFilter->Getter<MaterialBlurSaturationTag>()->Get();
    float brightness = materialBlurFilter->Getter<MaterialBlurBrightnessTag>()->Get();
    EXPECT_FLOAT_EQ(radius, -50.f);
    EXPECT_FLOAT_EQ(saturation, -0.5);
    EXPECT_FLOAT_EQ(brightness, -0.5);
}

/**
 * @tc.name: CreateHeatDistortionFilter001
 * @tc.desc: test creating HEAT_DISTORTION filter via RSNGFilterBase::Create
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateHeatDistortionFilter001, TestSize.Level1)
{
    auto heatDistortionFilter = RSNGFilterBase::Create(RSNGEffectType::HEAT_DISTORTION);
    EXPECT_NE(heatDistortionFilter, nullptr);
    EXPECT_EQ(heatDistortionFilter->GetType(), RSNGEffectType::HEAT_DISTORTION);
}

/**
 * @tc.name: CreateBlurBubblesRiseFilter001
 * @tc.desc: test creating BLUR_BUBBLES_RISE filter via RSNGFilterBase::Create
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateBlurBubblesRiseFilter001, TestSize.Level1)
{
    auto blurBubblesRiseFilter = RSNGFilterBase::Create(RSNGEffectType::BLUR_BUBBLES_RISE);
    EXPECT_NE(blurBubblesRiseFilter, nullptr);
    EXPECT_EQ(blurBubblesRiseFilter->GetType(), RSNGEffectType::BLUR_BUBBLES_RISE);
}

/**
 * @tc.name: CreateHeatDistortionFilterFromPara001
 * @tc.desc: test creating HEAT_DISTORTION filter from FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateHeatDistortionFilterFromPara001, TestSize.Level1)
{
    auto heatDistortionPara = std::make_shared<HeatDistortionPara>();
    heatDistortionPara->SetIntensity(0.8f);
    heatDistortionPara->SetNoiseScale(1.5f);
    heatDistortionPara->SetRiseWeight(0.7f);
    heatDistortionPara->SetProgress(0.5f);

    auto heatDistortionFilter = RSNGFilterBase::Create(heatDistortionPara);
    EXPECT_NE(heatDistortionFilter, nullptr);
    EXPECT_EQ(heatDistortionFilter->GetType(), RSNGEffectType::HEAT_DISTORTION);
}

/**
 * @tc.name: CreateHeatDistortionFilterFromPara002
 * @tc.desc: test creating HEAT_DISTORTION filter with nullptr FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateHeatDistortionFilterFromPara002, TestSize.Level1)
{
    std::shared_ptr<FilterPara> nullptrPara = nullptr;
    auto heatDistortionFilter = RSNGFilterBase::Create(nullptrPara);
    EXPECT_EQ(heatDistortionFilter, nullptr);
}

/**
 * @tc.name: CreateBlurBubblesRiseFilterFromPara001
 * @tc.desc: test creating BLUR_BUBBLES_RISE filter from FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateBlurBubblesRiseFilterFromPara001, TestSize.Level1)
{
    auto blurBubblesRisePara = std::make_shared<BlurBubblesRisePara>();
    blurBubblesRisePara->SetBlurIntensity(0.6f);
    blurBubblesRisePara->SetMixStrength(1.0f);
    blurBubblesRisePara->SetProgress(0.3f);

    auto blurBubblesRiseFilter = RSNGFilterBase::Create(blurBubblesRisePara);
    EXPECT_NE(blurBubblesRiseFilter, nullptr);
    EXPECT_EQ(blurBubblesRiseFilter->GetType(), RSNGEffectType::BLUR_BUBBLES_RISE);
}

/**
 * @tc.name: CreateBlurBubblesRiseFilterFromPara002
 * @tc.desc: test creating BLUR_BUBBLES_RISE filter with nullptr FilterPara
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, CreateBlurBubblesRiseFilterFromPara002, TestSize.Level1)
{
    std::shared_ptr<FilterPara> nullptrPara = nullptr;
    auto blurBubblesRiseFilter = RSNGFilterBase::Create(nullptrPara);
    EXPECT_EQ(blurBubblesRiseFilter, nullptr);
}

/**
 * @tc.name: HeatDistortionFilterParameterVerification
 * @tc.desc: test that HeatDistortionPara parameters are correctly set
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, HeatDistortionFilterParameterVerification, TestSize.Level1)
{
    auto heatDistortionPara = std::make_shared<HeatDistortionPara>();
    float testIntensity = 0.9f;
    float testNoiseScale = 2.0f;
    float testRiseWeight = 0.8f;
    float testProgress = 0.6f;

    heatDistortionPara->SetIntensity(testIntensity);
    heatDistortionPara->SetNoiseScale(testNoiseScale);
    heatDistortionPara->SetRiseWeight(testRiseWeight);
    heatDistortionPara->SetProgress(testProgress);

    auto heatDistortionFilter = RSNGFilterBase::Create(heatDistortionPara);
    EXPECT_NE(heatDistortionFilter, nullptr);

    auto retrievedIntensity = heatDistortionPara->GetIntensity();
    auto retrievedNoiseScale = heatDistortionPara->GetNoiseScale();
    auto retrievedRiseWeight = heatDistortionPara->GetRiseWeight();
    auto retrievedProgress = heatDistortionPara->GetProgress();

    EXPECT_FLOAT_EQ(retrievedIntensity, testIntensity);
    EXPECT_FLOAT_EQ(retrievedNoiseScale, testNoiseScale);
    EXPECT_FLOAT_EQ(retrievedRiseWeight, testRiseWeight);
    EXPECT_FLOAT_EQ(retrievedProgress, testProgress);
}

/**
 * @tc.name: BlurBubblesRiseFilterParameterVerification
 * @tc.desc: test that BlurBubblesRisePara parameters are correctly set
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, BlurBubblesRiseFilterParameterVerification, TestSize.Level1)
{
    auto blurBubblesRisePara = std::make_shared<BlurBubblesRisePara>();
    float testBlurIntensity = 0.7f;
    float testMixStrength = 0.9f;
    float testProgress = 0.4f;

    blurBubblesRisePara->SetBlurIntensity(testBlurIntensity);
    blurBubblesRisePara->SetMixStrength(testMixStrength);
    blurBubblesRisePara->SetProgress(testProgress);

    auto blurBubblesRiseFilter = RSNGFilterBase::Create(blurBubblesRisePara);
    EXPECT_NE(blurBubblesRiseFilter, nullptr);

    auto retrievedBlurIntensity = blurBubblesRisePara->GetBlurIntensity();
    auto retrievedMixStrength = blurBubblesRisePara->GetMixStrength();
    auto retrievedProgress = blurBubblesRisePara->GetProgress();

    EXPECT_FLOAT_EQ(retrievedBlurIntensity, testBlurIntensity);
    EXPECT_FLOAT_EQ(retrievedMixStrength, testMixStrength);
    EXPECT_FLOAT_EQ(retrievedProgress, testProgress);
}

/**
 * @tc.name: HeatDistortionParaDefaultValues
 * @tc.desc: test HeatDistortionPara default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, HeatDistortionParaDefaultValues, TestSize.Level1)
{
    auto heatDistortionPara = std::make_shared<HeatDistortionPara>();

    EXPECT_FLOAT_EQ(heatDistortionPara->GetIntensity(), 1.0f);
    EXPECT_FLOAT_EQ(heatDistortionPara->GetNoiseScale(), 1.0f);
    EXPECT_FLOAT_EQ(heatDistortionPara->GetRiseWeight(), 0.2f);
    EXPECT_FLOAT_EQ(heatDistortionPara->GetProgress(), 0.0f);

    EXPECT_EQ(heatDistortionPara->GetParaType(), FilterPara::ParaType::HEAT_DISTORTION);
}

/**
 * @tc.name: BlurBubblesRiseParaDefaultValues
 * @tc.desc: test BlurBubblesRisePara default values
 * @tc.type: FUNC
 */
HWTEST_F(RSUIFilterBaseTest, BlurBubblesRiseParaDefaultValues, TestSize.Level1)
{
    auto blurBubblesRisePara = std::make_shared<BlurBubblesRisePara>();

    EXPECT_FLOAT_EQ(blurBubblesRisePara->GetBlurIntensity(), 0.3f);
    EXPECT_FLOAT_EQ(blurBubblesRisePara->GetMixStrength(), 1.0f);
    EXPECT_FLOAT_EQ(blurBubblesRisePara->GetProgress(), 0.0f);
    EXPECT_EQ(blurBubblesRisePara->GetMaskImage(), nullptr);

    EXPECT_EQ(blurBubblesRisePara->GetParaType(), FilterPara::ParaType::BLUR_BUBBLES_RISE);
}

} // namespace OHOS::Rosen
