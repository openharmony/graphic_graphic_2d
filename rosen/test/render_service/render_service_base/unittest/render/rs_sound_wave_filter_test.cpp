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

#include "render/rs_sound_wave_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSoundWaveFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSoundWaveFilterTest::SetUpTestCase() {}
void RSSoundWaveFilterTest::TearDownTestCase() {}
void RSSoundWaveFilterTest::SetUp() {}
void RSSoundWaveFilterTest::TearDown() {}

/**
 * @tc.name: RSSoundWaveFilter001
 * @tc.desc: Verify function RSSoundWaveFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSSoundWaveFilterTest, RSSoundWaveFilter001, TestSize.Level1)
{
    RSColor colorA = RSColor(0XFFFF0000);
    RSColor colorB = RSColor(0XFF00FF00);
    RSColor colorC = RSColor(0XFF0000FF);
    float colorProgress = 1.0f;
    float centerBrightness = 0.5f;
    float soundIntensity = 1.5f;
    float shockWaveAlphaA = 0.6f;
    float shockWaveAlphaB = 0.8f;
    float shockWaveProgressA = 0.7f;
    float shockWaveProgressB = 1.0f;
    auto filter = std::make_shared<RSSoundWaveFilter>(colorA, colorB, colorC,
        colorProgress, centerBrightness, soundIntensity, shockWaveAlphaA,
        shockWaveAlphaB, shockWaveProgressA, shockWaveProgressB);

    EXPECT_NE(filter, nullptr);
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetColorProgress(), colorProgress));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetCenterBrightness(), centerBrightness));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetSoundIntensity(), soundIntensity));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveAlphaA(), shockWaveAlphaA));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveAlphaB(), shockWaveAlphaB));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveProgressA(), shockWaveProgressA));
    EXPECT_TRUE(ROSEN_EQ<float>(filter->GetShockWaveProgressB(), shockWaveProgressB));
}

/**
 * @tc.name: GenerateGEVisualEffect001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSoundWaveFilterTest, GenerateGEVisualEffect001, TestSize.Level1)
{
    RSColor colorA = RSColor(0XFFFF0000);
    RSColor colorB = RSColor(0XFF00FF00);
    RSColor colorC = RSColor(0XFF0000FF);
    float colorProgress = 1.0f;
    float centerBrightness = 0.5f;
    float soundIntensity = 1.5f;
    float shockWaveAlphaA = 0.6f;
    float shockWaveAlphaB = 0.8f;
    float shockWaveProgressA = 0.7f;
    float shockWaveProgressB = 1.0f;
    auto filter = std::make_shared<RSSoundWaveFilter>(colorA, colorB, colorC,
        colorProgress, centerBrightness, soundIntensity, shockWaveAlphaA,
        shockWaveAlphaB, shockWaveProgressA, shockWaveProgressB);

    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    filter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace OHOS::Rosen
