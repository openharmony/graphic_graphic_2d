/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_sound_wave_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderSoundWaveFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderSoundWaveFilterTest::SetUpTestCase() {}
void RSRenderSoundWaveFilterTest::TearDownTestCase() {}
void RSRenderSoundWaveFilterTest::SetUp() {}
void RSRenderSoundWaveFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, GetDescription001, TestSize.Level1)
{
    auto rsRenderSoundWaveFilterPara = std::make_shared<RSRenderSoundWaveFilterPara>(0);
    std::string out;
    rsRenderSoundWaveFilterPara->GetDescription(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: WriteToParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, WriteToParcel001, TestSize.Level1)
{
    auto rsRenderSoundWaveFilterPara = std::make_shared<RSRenderSoundWaveFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderSoundWaveFilterPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ReadFromParcel001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, ReadFromParcel001, TestSize.Level1)
{
    auto rsRenderSoundWaveFilterPara = std::make_shared<RSRenderSoundWaveFilterPara>(0);
    Parcel parcel;
    auto ret = rsRenderSoundWaveFilterPara->ReadFromParcel(parcel);
    EXPECT_FALSE(ret);

    ret = rsRenderSoundWaveFilterPara->WriteToParcel(parcel);
    EXPECT_TRUE(ret);

    ret = rsRenderSoundWaveFilterPara->ReadFromParcel(parcel);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CreateRenderPropert001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, CreateRenderPropert001, TestSize.Level1)
{
    auto rsRenderSoundWaveFilterPara = std::make_shared<RSRenderSoundWaveFilterPara>(0);

    auto renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::RIPPLE_MASK);
    EXPECT_EQ(renderPropert, nullptr);

    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_A);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_B);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_C);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SOUND_INTENSITY);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_A);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SHOCK_WAVE_ALPHA_B);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_A);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SHOCK_WAVE_PROGRESS_B);
    EXPECT_NE(renderPropert, nullptr);

    renderPropert = nullptr;
    renderPropert = rsRenderSoundWaveFilterPara->CreateRenderProperty(RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA);
    EXPECT_NE(renderPropert, nullptr);
}

/**
 * @tc.name: GetLeafRenderProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, GetLeafRenderProperties001, TestSize.Level1)
{
    auto rsRenderSoundWaveFilterPara = std::make_shared<RSRenderSoundWaveFilterPara>(0);
    auto rsRenderPropertyBaseVec = rsRenderSoundWaveFilterPara->GetLeafRenderProperties();
    EXPECT_TRUE(rsRenderPropertyBaseVec.empty());
}

/**
 * @tc.name: RSRenderSoundWaveFilter001
 * @tc.desc: Verify function RSRenderSoundWaveFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, RSRenderSoundWaveFilter001, TestSize.Level1)
{
    float colorProgress = 1.0f;
    float soundIntensity = 1.5f;
    float shockWaveAlphaA = 0.6f;
    float shockWaveAlphaB = 0.8f;
    float shockWaveProgressA = 0.7f;
    float shockWaveProgressB = 1.0f;
    float shockWaveTotalAlpha = 1.0f;
    auto filter = std::make_shared<RSRenderSoundWaveFilterPara>(0);
    EXPECT_NE(filter, nullptr);
    filter->colorProgress_ = colorProgress;
    filter->soundIntensity_ = soundIntensity;
    filter->shockWaveAlphaA_ = shockWaveAlphaA;
    filter->shockWaveAlphaB_ = shockWaveAlphaB;
    filter->shockWaveProgressA_ = shockWaveProgressA;
    filter->shockWaveProgressB_ = shockWaveProgressB;
    filter->shockWaveTotalAlpha_ = shockWaveTotalAlpha;
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetColorProgress(), colorProgress));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetSoundIntensity(), soundIntensity));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveAlphaA(), shockWaveAlphaA));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveAlphaB(), shockWaveAlphaB));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveProgressA(), shockWaveProgressA));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveProgressB(), shockWaveProgressB));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveTotalAlpha(), shockWaveTotalAlpha));
}

/**
 * @tc.name: ParseFilterValuesTest001
 * @tc.desc: Verify function ParseFilterValues
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, ParseFilterValuesTest001, TestSize.Level1)
{
    auto filter = std::make_shared<RSRenderSoundWaveFilterPara>(0);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto colorAProperty = std::make_shared<RSRenderProperty<Vector4f>>(Vector4f(1.f, 1.f, 1.f, 1.f), 0);
    filter->Setter(RSUIFilterType::SOUND_WAVE_COLOR_A, colorAProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto colorBProperty = std::make_shared<RSRenderProperty<Vector4f>>(Vector4f(1.f, 1.f, 1.f, 1.f), 0);
    filter->Setter(RSUIFilterType::SOUND_WAVE_COLOR_B, colorBProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto colorCProperty = std::make_shared<RSRenderProperty<Vector4f>>(Vector4f(1.f, 1.f, 1.f, 1.f), 0);
    filter->Setter(RSUIFilterType::SOUND_WAVE_COLOR_C, colorCProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto colorProgressProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::SOUND_WAVE_COLOR_PROGRESS, colorProgressProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto soundIntensityProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::SOUND_INTENSITY, soundIntensityProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto shockWaveAlphaAProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::SHOCK_WAVE_ALPHA_A, shockWaveAlphaAProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto shockWaveAlphaBProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::SHOCK_WAVE_ALPHA_B, shockWaveAlphaBProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto shockWaveProgressAProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::SHOCK_WAVE_PROGRESS_A, shockWaveProgressAProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto shockWaveTotalProperty = std::make_shared<RSRenderProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::SHOCK_WAVE_TOTAL_ALPHA, shockWaveTotalProperty);
    EXPECT_FALSE(filter->ParseFilterValues());

    auto shockWaveProgressBProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f, 0);
    filter->Setter(RSUIFilterType::SHOCK_WAVE_PROGRESS_B, shockWaveProgressBProperty);
    EXPECT_TRUE(filter->ParseFilterValues());

    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    filter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    auto filterCopy = filter->DeepCopy();
    EXPECT_NE(filterCopy, nullptr);
}

/**
 * @tc.name: GenerateGEVisualEffect001
 * @tc.desc: Test hdr support of GenerateGEVisualEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderSoundWaveFilterTest, GenerateGEVisualEffect001, TestSize.Level1)
{
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    auto rsSoundWaveLightFilter = std::make_shared<RSRenderSoundWaveFilterPara>(0);

    rsSoundWaveLightFilter->colorA_ = { 0.5f, 0.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->colorB_ = { 0.5f, 0.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->colorC_ = { 0.5f, 0.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    rsSoundWaveLightFilter->colorA_ = { 1.5f, 0.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->colorB_ = { 1.5f, 0.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->colorC_ = { 1.5f, 0.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    rsSoundWaveLightFilter->colorA_ = { 0.5f, 1.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->colorB_ = { 0.5f, 1.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->colorC_ = { 0.5f, 1.5f, 0.5f, 1.0f };
    rsSoundWaveLightFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());

    rsSoundWaveLightFilter->colorA_ = { 0.5f, 0.5f, 1.5f, 1.0f };
    rsSoundWaveLightFilter->colorB_ = { 0.5f, 0.5f, 1.5f, 1.0f };
    rsSoundWaveLightFilter->colorC_ = { 0.5f, 0.5f, 1.5f, 1.0f };
    rsSoundWaveLightFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace OHOS::Rosen