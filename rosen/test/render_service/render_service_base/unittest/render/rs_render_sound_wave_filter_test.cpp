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

} // namespace OHOS::Rosen