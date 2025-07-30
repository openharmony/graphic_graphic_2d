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
#include "ui_effect/property/include/rs_ui_sound_wave_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUISoundWaveFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUISoundWaveFilterTest::SetUpTestCase() {}
void RSUISoundWaveFilterTest::TearDownTestCase() {}
void RSUISoundWaveFilterTest::SetUp() {}
void RSUISoundWaveFilterTest::TearDown() {}

/**
 * @tc.name: Equal001
 * @tc.desc: Test Equal
 * @tc.type:FUNC
 */
HWTEST_F(RSUISoundWaveFilterTest, Equal001, TestSize.Level1)
{
    auto rsUISoundWaveFilterPara1 = std::make_shared<RSUISoundWaveFilterPara>();
    auto rsUISoundWaveFilterPara2 = std::make_shared<RSUISoundWaveFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUISoundWaveFilterPara2);
    
    EXPECT_TRUE(rsUISoundWaveFilterPara1->Equals(rsUIFilterParaBase));
    EXPECT_FALSE(rsUISoundWaveFilterPara1->Equals(nullptr));
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test Dump
 * @tc.type:FUNC
 */
HWTEST_F(RSUISoundWaveFilterTest, Dump001, TestSize.Level1)
{
    auto rsUISoundWaveFilterPara = std::make_shared<RSUISoundWaveFilterPara>();

    std::string temp = "RSUISoundWaveFilterPara: [";
    std::string out;
    rsUISoundWaveFilterPara->Dump(out);
    EXPECT_EQ(temp, out);

    std::string temp1 = "RSUISoundWaveFilterPara: [[soundProgress: 1.500000]";
    rsUISoundWaveFilterPara->SetColorProgress(1.5);
    std::string out1;
    rsUISoundWaveFilterPara->Dump(out1);
    EXPECT_EQ(temp1, out1);
}

/**
 * @tc.name: SetProperty001
 * @tc.desc: Test SetProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSUISoundWaveFilterTest, SetProperty001, TestSize.Level1)
{
    auto rsUISoundWaveFilterPara1 = std::make_shared<RSUISoundWaveFilterPara>();
    auto rsUISoundWaveFilterPara2 = std::make_shared<RSUISoundWaveFilterPara>();
    auto rsUIFilterParaBase = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUISoundWaveFilterPara2);

    rsUISoundWaveFilterPara1->SetProperty(nullptr);
    rsUISoundWaveFilterPara1->SetProperty(rsUIFilterParaBase);

    Vector4f colorA = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f colorB = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f colorC = { 1.0f, 1.0f, 1.0f, 1.0f };
    float colorProgress = 1.0f;
    float soundIntensity = 1.5f;
    float shockWaveAlphaA = 0.6f;
    float shockWaveAlphaB = 0.8f;
    float shockWaveProgressA = 0.7f;
    float shockWaveProgressB = 1.0f;
    float shockWaveTotalAlpha = 0.5f;
    rsUISoundWaveFilterPara2->SetColors(colorA, colorB, colorC);
    rsUISoundWaveFilterPara2->SetColorProgress(colorProgress);
    rsUISoundWaveFilterPara2->SetSoundIntensity(soundIntensity);
    rsUISoundWaveFilterPara2->SetShockWaveAlphaA(shockWaveAlphaA);
    rsUISoundWaveFilterPara2->SetShockWaveAlphaB(shockWaveAlphaB);
    rsUISoundWaveFilterPara2->SetShockWaveProgressA(shockWaveProgressA);
    rsUISoundWaveFilterPara2->SetShockWaveProgressB(shockWaveProgressB);
    rsUISoundWaveFilterPara2->SetShockWaveTotalAlpha(shockWaveTotalAlpha);
    auto rsUIFilterParaBase2 = static_cast<std::shared_ptr<RSUIFilterParaBase>>(rsUISoundWaveFilterPara2);
    rsUISoundWaveFilterPara1->SetProperty(rsUIFilterParaBase2);

    auto out1 = rsUISoundWaveFilterPara1->GetLeafProperties();
    auto out2 = rsUISoundWaveFilterPara2->GetLeafProperties();
    EXPECT_EQ(out1.size(), out2.size());
}

/**
 * @tc.name: CreateRSRenderFilter001
 * @tc.desc: Test CreateRSRenderFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSUISoundWaveFilterTest, CreateRSRenderFilter001, TestSize.Level1)
{
    auto rsUISoundWaveFilterPara1 = std::make_shared<RSUISoundWaveFilterPara>();
    
    Vector4f colorA = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f colorB = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4f colorC = { 1.0f, 1.0f, 1.0f, 1.0f };
    float colorProgress = 1.0f;
    float soundIntensity = 1.5f;
    float shockWaveAlphaA = 0.6f;
    float shockWaveAlphaB = 0.8f;
    float shockWaveProgressA = 0.7f;
    float shockWaveProgressB = 1.0f;
    float shockWaveTotalAlpha = 0.5f;
    rsUISoundWaveFilterPara1->SetColors(colorA, colorB, colorC);
    rsUISoundWaveFilterPara1->SetColorProgress(colorProgress);
    rsUISoundWaveFilterPara1->SetSoundIntensity(soundIntensity);
    rsUISoundWaveFilterPara1->SetShockWaveAlphaA(shockWaveAlphaA);
    rsUISoundWaveFilterPara1->SetShockWaveAlphaB(shockWaveAlphaB);
    rsUISoundWaveFilterPara1->SetShockWaveProgressA(shockWaveProgressA);
    rsUISoundWaveFilterPara1->SetShockWaveProgressB(shockWaveProgressB);
    rsUISoundWaveFilterPara1->SetShockWaveTotalAlpha(shockWaveTotalAlpha);

    auto rsRenderFilterParaBase = rsUISoundWaveFilterPara1->CreateRSRenderFilter();
    EXPECT_NE(rsRenderFilterParaBase, nullptr);
}
} // namespace OHOS::Rosen