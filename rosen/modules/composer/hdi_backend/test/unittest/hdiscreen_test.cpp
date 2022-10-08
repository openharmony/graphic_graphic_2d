/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hdi_screen.h"
#include "mock_hdi_device.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::unique_ptr<HdiScreen> hdiScreen_;
    static inline Mock::HdiDeviceMock* mockDevice_;
};

void HdiScreenTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiScreen_ = HdiScreen::CreateHdiScreen(screenId);
    mockDevice_ = Mock::HdiDeviceMock::GetInstance();

    EXPECT_CALL(*mockDevice_, GetScreenCapability(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenSupportedModes(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenMode(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenMode(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenPowerStatus(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenPowerStatus(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenBacklight(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenBacklight(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenSupportedColorGamuts(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenColorGamut(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenColorGamut(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, SetScreenGamutMap(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetScreenGamutMap(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetHDRCapabilityInfos(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*mockDevice_, GetSupportedMetaDataKey(_, _)).WillRepeatedly(testing::Return(0));

    hdiScreen_->SetHdiDevice(mockDevice_);
}

void HdiScreenTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: Init001
 * @tc.desc: Verify the Init of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, Init001, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiScreenTest::hdiScreen_->Init(), true);
}

/**
 * @tc.name: GetScreenCapability001
 * @tc.desc: Verify the GetScreenCapability of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, GetScreenCapability001, Function | MediumTest| Level3)
{
    GraphicDisplayCapability displayCapability = {
        .name = "test",
        .type = GraphicInterfaceType::GRAPHIC_DISP_INTF_BT1120,
        .phyWidth = 800,
        .phyHeight = 600,
        .supportLayers = 1,
        .virtualDispCount = 1,
        .supportWriteBack = true,
        .propertyCount = 1,
        .props = nullptr,
    };
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenCapability(displayCapability), 0);
}

/**
 * @tc.name: GetScreenSupportedModes001
 * @tc.desc: Verify the GetScreenSupportedModes of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, GetScreenSupportedModes001, Function | MediumTest| Level3)
{
    GraphicDisplayModeInfo displayModeInfo = {
        .width = 800,
        .height = 600,
        .freshRate = 60,
        .id = 0,
    };
    std::vector<GraphicDisplayModeInfo> modeInfo = { displayModeInfo };
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenSupportedModes(modeInfo), 0);
}

/**
 * @tc.name: GetScreenMode001
 * @tc.desc: Verify the GetScreenMode of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, GetScreenMode001, Function | MediumTest| Level3)
{
    uint32_t modeId = 0;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenMode(modeId), 0);
}

/**
 * @tc.name: SetScreenMode001
 * @tc.desc: Verify the SetScreenMode of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, SetScreenMode001, Function | MediumTest| Level3)
{
    uint32_t modeId = 0;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->SetScreenMode(modeId), 0);
}

/**
 * @tc.name: GetScreenPowerStatus001
 * @tc.desc: Verify the GetScreenPowerStatus of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, GetScreenPowerStatus001, Function | MediumTest| Level3)
{
    GraphicDispPowerStatus dispPowerStatus = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenPowerStatus(dispPowerStatus), 0);
}

/**
 * @tc.name: SetScreenPowerStatus001
 * @tc.desc: Verify the SetScreenPowerStatus of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, SetScreenPowerStatus001, Function | MediumTest| Level3)
{
    GraphicDispPowerStatus dispPowerStatus = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->SetScreenPowerStatus(dispPowerStatus), 0);
}

/**
 * @tc.name: GetScreenBacklight001
 * @tc.desc: Verify the GetScreenBacklight of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, GetScreenBacklight001, Function | MediumTest| Level3)
{
    uint32_t level = 0;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenBacklight(level), 0);
}

/**
 * @tc.name: SetScreenBacklight001
 * @tc.desc: Verify the SetScreenBacklight of hdiscreen
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiScreenTest, SetScreenBacklight001, Function | MediumTest| Level3)
{
    uint32_t level = 0;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->SetScreenBacklight(level), 0);
}

/*
* Function: GetScreenSupportedColorGamuts001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedColorGamuts
*                  2. check ret
 */
HWTEST_F(HdiScreenTest, GetScreenSupportedColorGamuts001, Function | MediumTest | Level2)
{
    GraphicColorGamut colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    std::vector<GraphicColorGamut> gamuts = { colorGamut };
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenSupportedColorGamuts(gamuts), 0);
}

/*
* Function: SetScreenColorGamut001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenColorGamut
*                  2. check ret
 */
HWTEST_F(HdiScreenTest, SetScreenColorGamut001, Function | MediumTest | Level2)
{
    GraphicColorGamut colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->SetScreenColorGamut(colorGamut), 0);
}

/*
* Function: GetScreenColorGamut001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenColorGamut
*                  2. check ret
 */
HWTEST_F(HdiScreenTest, GetScreenColorGamut001, Function | MediumTest | Level2)
{
    GraphicColorGamut colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenColorGamut(colorGamut), 0);
}

/*
* Function: SetScreenGamutMap001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenGamutMap
*                  2. check ret
 */
HWTEST_F(HdiScreenTest, SetScreenGamutMap001, Function | MediumTest | Level2)
{
    GraphicGamutMap gamutMap = GraphicGamutMap::GRAPHIC_GAMUT_MAP_CONSTANT;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->SetScreenGamutMap(gamutMap), 0);
}

/*
* Function: GetScreenGamutMap001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenGamutMap
*                  2. check ret
 */
HWTEST_F(HdiScreenTest, GetScreenGamutMap001, Function | MediumTest | Level2)
{
    GraphicGamutMap gamutMap = GraphicGamutMap::GRAPHIC_GAMUT_MAP_CONSTANT;
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetScreenGamutMap(gamutMap), 0);
}

/*
* Function: GetHDRCapabilityInfos001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetHDRCapabilityInfos
*                  2. check ret
 */
HWTEST_F(HdiScreenTest, GetHDRCapabilityInfos001, Function | MediumTest | Level2)
{
    GraphicHDRCapability capaility = {
        .formatCount = 0,
        .formats = nullptr,
        .maxLum = 0,
        .maxAverageLum = 0,
        .minLum = 0,
    };
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetHDRCapabilityInfos(capaility), 0);
}

/*
* Function: GetSupportedMetaDataKey001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetSupportedMetaDataKey
*                  2. check ret
 */
HWTEST_F(HdiScreenTest, GetSupportedMetaDataKey001, Function | MediumTest | Level2)
{
    GraphicHDRMetadataKey key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    std::vector<GraphicHDRMetadataKey> keys = { key };
    ASSERT_EQ(HdiScreenTest::hdiScreen_->GetSupportedMetaDataKey(keys), 0);
}
} // namespace
} // namespace Rosen
} // namespace OHOS