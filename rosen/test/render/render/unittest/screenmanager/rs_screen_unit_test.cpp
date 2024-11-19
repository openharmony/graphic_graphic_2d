/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "limit_number.h"
#include "screen_manager/rs_screen.h"
#include "mock_hdi_device.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline ScreenId mockScreenId_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void RsScreenTest::SetUpTestCase()
{
    mockScreenId_ = 0xFFFF;
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, SetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SaveArg<1>(&Mock::HdiDeviceMock::powerStatusMock_), testing::Return(0)));
    EXPECT_CALL(*hdiDeviceMock_, GetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::powerStatusMock_)), testing::Return(0)));
}
void RsScreenTest::TearDownTestCase() {}
void RsScreenTest::SetUp() {}
void RsScreenTest::TearDown() {}

/*
 * @tc.name: IsEnable001
 * @tc.desc: IsEnable Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, IsEnable001, testing::ext::TestSize.Level1)
{
    ScreenId idx = INVALID_SCREEN_ID;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_EQ(rsScreen->IsEnable(), false);
}

/*
 * @tc.name: DisplayDump001
 * @tc.desc: Test PhysicalScreen DisplayDump
 * @tc.type: FUNC
 * @tc.require: issueI60RFZ
 */
HWTEST_F(RsScreenTest, DisplayDump001, testing::ext::TestSize.Level2)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    std::string dumpString = "";
    int32_t screenIndex = 0;
    rsScreen->DisplayDump(screenIndex, dumpString);
    ASSERT_NE(dumpString.size(), 0);
}

/*
 * @tc.name: SetResolution001
 * @tc.desc: SetResolution Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetResolution001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    uint32_t width = 100;
    uint32_t height = 100;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->SetResolution(width, height);
    if (virtualScreen->IsVirtual()) {
        ASSERT_EQ(virtualScreen->Width(), width);
    }
}

/*
 * @tc.name: ScreenTypeDump001
 * @tc.desc: ScreenTypeDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, ScreenTypeDump001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    std::string dumpString = "";
    virtualScreen->DisplayDump(config.id, dumpString);
    ASSERT_NE(dumpString.size(), 0);
}

/*
 * @tc.name: SetPowerStatus001
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetPowerStatus001, testing::ext::TestSize.Level2)
{
    ScreenId idx = INVALID_SCREEN_ID;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, true, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    rsScreen->SetPowerStatus(static_cast<uint32_t>(1000));
    ASSERT_EQ(rsScreen->GetPowerStatus(), ScreenPowerStatus::INVALID_POWER_STATUS);
}

/*
 * @tc.name: SetScreenBacklight001
 * @tc.desc: SetScreenBacklight Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetScreenBacklight001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->SetScreenBacklight(static_cast<uint32_t>(1000));
    ASSERT_EQ(virtualScreen->GetScreenBacklight(), INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts001
 * @tc.desc: GetScreenSupportedColorGamuts Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, GetScreenSupportedColorGamuts001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    std::vector<ScreenColorGamut> mode;
    virtualScreen->GetScreenSupportedColorGamuts(mode);
}

/*
 * @tc.name: GetAndResetVirtualSurfaceUpdateFlag001
 * @tc.desc: Test get virtualSurface update flag correctly
 * @tc.type: FUNC
 * @tc.require: issueIA9QG0
 */
HWTEST_F(RsScreenTest, GetAndResetVirtualSurfaceUpdateFlag001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->isVirtualSurfaceUpdateFlag_ = true;
    ASSERT_EQ(virtualScreen->GetAndResetVirtualSurfaceUpdateFlag(), true);
    virtualScreen->isVirtualSurfaceUpdateFlag_ = false;
    ASSERT_EQ(virtualScreen->GetAndResetVirtualSurfaceUpdateFlag(), false);
}

/*
 * @tc.name: SetScreenColorGamut001
 * @tc.desc: SetScreenColorGamut Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetScreenColorGamut001, testing::ext::TestSize.Level1)
{
    ScreenId idx = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    rsScreen->SetScreenColorGamut(static_cast<int32_t>(1000));
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenColorGamut(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetActiveMode001
 * @tc.desc: SetActiveMode Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetActiveMode001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    uint32_t modeId = static_cast<uint32_t>(1);
    virtualScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetScreenVsyncEnabled001
 * @tc.desc: SetScreenVsyncEnabled Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetScreenVsyncEnabled001, testing::ext::TestSize.Level1)
{
    ScreenId idx = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    rsScreen->SetScreenVsyncEnabled(true);
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    virtualScreen->SetScreenVsyncEnabled(true);
}

/*
 * @tc.name: SetPowerStatus002
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetPowerStatus002, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    virtualScreen->SetPowerStatus(status);
}

/*
 * @tc.name: SetPowerStatus004
 * @tc.desc: SetPowerStatus Test, test POWER_STATUS_OFF_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetPowerStatus004, testing::ext::TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF_ADVANCED;
    rsScreen->SetPowerStatus(status);
    ASSERT_EQ(rsScreen->GetPowerStatus(), status);
}

/*
 * @tc.name: SetPowerStatus003
 * @tc.desc: SetPowerStatus Test, test POWER_STATUS_ON_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetPowerStatus003, testing::ext::TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON_ADVANCED;
    rsScreen->SetPowerStatus(status);
    ASSERT_EQ(rsScreen->GetPowerStatus(), status);
}

/*
 * @tc.name: GetActiveMode001
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, GetActiveMode001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->GetActiveMode().has_value(), false);
}

/*
 * @tc.name: SetScreenGamutMap001
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetScreenGamutMap001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ScreenGamutMap map1 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto result1 = virtualScreen->SetScreenGamutMap(map1);
    ASSERT_EQ(result1, StatusCode::SUCCESS);
    ScreenGamutMap map2 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    const auto result2 = virtualScreen->GetScreenGamutMap(map2);
    ASSERT_EQ(result2, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys001
 * @tc.desc: GetScreenSupportedMetaDataKeys Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, GetScreenSupportedMetaDataKeys001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<ScreenHDRMetadataKey> keys;
    ASSERT_EQ(virtualScreen->GetScreenSupportedMetaDataKeys(keys), INVALID_BACKLIGHT_VALUE);
    ScreenId idx = static_cast<uint64_t>(1);
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_EQ(rsScreen->GetScreenSupportedMetaDataKeys(keys), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenGamutMap002
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetScreenGamutMap002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_NE(nullptr, rsScreen);
    ScreenGamutMap map1 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto result1 = rsScreen->SetScreenGamutMap(map1);
    ASSERT_EQ(result1, StatusCode::HDI_ERROR);
    const auto result2 = rsScreen->GetScreenGamutMap(map1);
    ASSERT_EQ(result2, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetScreenTypeTest
 * @tc.desc: GetScreenType Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, GetScreenTypeTest, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    auto res = virtualScreen->GetScreenType();
    auto type = RSScreenType::VIRTUAL_TYPE_SCREEN;
    ASSERT_EQ(res, type);
}

/*
 * @tc.name: GetActiveModePosByModeIdTest
 * @tc.desc: GetActiveModePosByModeId Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, GetActiveModePosByModeIdTest001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    int32_t modeId = 0;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    auto res = virtualScreen->GetActiveModePosByModeId(modeId);
    ASSERT_EQ(res, -1);
}

/*
 * @tc.name: SetScreenSkipFrameIntervalTest
 * @tc.desc: SetScreenSkipFrameInterval Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SetScreenSkipFrameIntervalTest, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    uint32_t skipFrameInterval = 0;
    virtualScreen->SetScreenSkipFrameInterval(skipFrameInterval);
    auto res = virtualScreen->GetScreenSkipFrameInterval();
    ASSERT_EQ(res, 0);
}

/*
 * @tc.name: SetScreenHDRFormat001
 * @tc.desc: SetScreenHDRFormat Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RsScreenTest, SetScreenHDRFormat001, testing::ext::TestSize.Level1)
{
    ScreenId idx = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_EQ(rsScreen->SetScreenHDRFormat(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenHDRFormat(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
}

/*
 * @tc.name: GetScreenSupportedHDRFormats001
 * @tc.desc: GetScreenSupportedHDRFormats Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RsScreenTest, GetScreenSupportedHDRFormats001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(virtualScreen->GetScreenSupportedHDRFormats(hdrFormats), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetPixelFormat001
 * @tc.desc: SetPixelFormat Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RsScreenTest, SetPixelFormat001, testing::ext::TestSize.Level1)
{
    ScreenId idx = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_EQ(rsScreen->SetPixelFormat(static_cast<GraphicPixelFormat>(20)), StatusCode::SUCCESS); //BGRA8888
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetPixelFormat(static_cast<GraphicPixelFormat>(20)), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenColorSpace001
 * @tc.desc: SetScreenColorSpace Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RsScreenTest, SetScreenColorSpace001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenColorSpace(
        static_cast<GraphicCM_ColorSpaceType>(1 | (2 << 8) | (3 << 16) | (1 << 21))), SUCCESS);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces001
 * @tc.desc: GetScreenSupportedColorSpaces Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RsScreenTest, GetScreenSupportedColorSpaces001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ASSERT_EQ(virtualScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::SUCCESS);
}

/*
 * @tc.name: SurfaceDumpTest
 * @tc.desc: SurfaceDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, SurfaceDumpTest, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "SurfaceDumpTest";
    rsScreen->SurfaceDump(screenIndex, dumpString);
}

/*
 * @tc.name: ClearFpsDumpTest
 * @tc.desc: ClearFpsDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, ClearFpsDumpTest001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "ClearFpsDumpTest";
    std::string arg = "ClearFpsDumpTest";
    rsScreen->ClearFpsDump(screenIndex, dumpString, arg);
}

/*
 * @tc.name: FpsDumpTest
 * @tc.desc: FpsDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, FpsDumpTest, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "FpsDumpTest";
    std::string arg = "FpsDumpTest";
    rsScreen->FpsDump(screenIndex, dumpString, arg);
}

/*
 * @tc.name: ClearFpsDumpTest
 * @tc.desc: ClearFpsDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, ClearFpsDumpTest002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "ClearFpsDumpTest";
    std::string arg = "ClearFpsDumpTest";
    rsScreen->ClearFpsDump(screenIndex, dumpString, arg);
}

/*
 * @tc.name: PowerStatusDump002
 * @tc.desc: PowerStatusDump Test, with mocked HDI device
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, PowerStatusDump002, testing::ext::TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    // Set status to GRAPHIC_POWER_STATUS_ON_ADVANCED
    rsScreen->SetPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON_ADVANCED);
    std::string dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerstatus=POWER_STATUS_ON_ADVANCED");
    // Set status to GRAPHIC_POWER_STATUS_OFF_ADVANCED
    rsScreen->SetPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF_ADVANCED);
    dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerstatus=POWER_STATUS_OFF_ADVANCED");
}

/*
 * @tc.name: PowerStatusDump001
 * @tc.desc: PowerStatusDump Test, virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RsScreenTest, PowerStatusDump001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = 1;
    auto rsScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(rsScreen, nullptr);
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    rsScreen->SetPowerStatus(status);
    std::string dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerstatus=INVALID_POWER_STATUS");
}

/*
 * @tc.name: SetRogResolution002
 * @tc.desc: SetRogResolution Test, trigger branch -- hdiScreen_->SetScreenOverlayResolution(width, height) < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetRogResolution002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint32_t width = 100;
    uint32_t height = 100;

    rsScreen->width_ = width + 1;
    rsScreen->height_ = height + 1;

    rsScreen->phyWidth_ = width + 1;
    rsScreen->phyHeight_ = height + 1;

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenOverlayResolution(_, _, _)).Times(1).WillOnce(testing::Return(-1));

    rsScreen->SetRogResolution(width, height);
}

/*
 * @tc.name: ScreenCapabilityInit002
 * @tc.desc: ScreenCapabilityInit Test, isVirtual_ is false, hdiScreen_->GetScreenCapability return not
 * GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, ScreenCapabilityInit002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCapability(idx, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    rsScreen->ScreenCapabilityInit();
    ASSERT_EQ(rsScreen->capability_.name, "test1");
    ASSERT_EQ(rsScreen->capability_.type, GRAPHIC_DISP_INTF_HDMI);
    ASSERT_EQ(rsScreen->capability_.phyWidth, 1921);
    ASSERT_EQ(rsScreen->capability_.phyHeight, 1081);
    ASSERT_EQ(rsScreen->capability_.supportLayers, 0);
    ASSERT_EQ(rsScreen->capability_.virtualDispCount, 0);
    ASSERT_EQ(rsScreen->capability_.supportWriteBack, true);
    ASSERT_EQ(rsScreen->capability_.propertyCount, 0);
}

/*
 * @tc.name: ScreenCapabilityInit001
 * @tc.desc: ScreenCapabilityInit Test, isVirtual_ = true
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, ScreenCapabilityInit001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_TRUE(rsScreen->IsVirtual());
    rsScreen->ScreenCapabilityInit();
}

/*
 * @tc.name: IsEnable002
 * @tc.desc: IsEnable Test, idx is valid and 4 conditions of hdiOutput_ nad producerSurface_
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, IsEnable002, testing::ext::TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);

    ScreenId idx = 0;

    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);
    ASSERT_NE(rsScreen->id_, INVALID_SCREEN_ID);

    ASSERT_FALSE(rsScreen->IsEnable());

    rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), nullptr);
    ASSERT_TRUE(rsScreen->IsEnable());

    rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, psurface);
    ASSERT_TRUE(rsScreen->IsEnable());

    rsScreen = std::make_unique<impl::RSScreen>(idx, false, HdiOutput::CreateHdiOutput(idx), psurface);
    ASSERT_TRUE(rsScreen->IsEnable());
}

/*
 * @tc.name: SetActiveMode003
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetActiveMode003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;

    rsScreen->supportedModes_.resize(6);
    uint32_t modeId = 1;
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode(_, _)).Times(1).WillOnce(testing::Return(-1));

    rsScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetActiveMode002
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId >= supportedModes_.size()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetActiveMode002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;

    rsScreen->supportedModes_.resize(1);
    uint32_t modeId = 5;

    rsScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetActiveMode004
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0,
 * GetActiveMode() return {}
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetActiveMode004, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    //  IsVirtual() is false
    rsScreen->isVirtual_ = false;

    // isure GetActiveMode return {}
    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    rsScreen->supportedModes_[0] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 }; // id not 0
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 }; // id not 0

    // hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode(_, _)).Times(1).WillOnce(testing::Return(1));

    // modeId < supportedModes_.size()
    uint32_t modeId = supportedModesSize - 1;
    rsScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetActiveMode006
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0,
 * GetActiveMode() return not {}
 * only if one !=, not into branch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetActiveMode006, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    //  IsVirtual() is false
    rsScreen->isVirtual_ = false;

    // isure GetActiveMode return not {}
    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    // default static init 0, not into branch
    rsScreen->supportedModes_[0] = { .width = 0, .height = 0, .freshRate = 0, .id = 0 }; // id 0
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 };

    // hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode(_, _)).Times(1).WillOnce(testing::Return(1));

    // modeId < supportedModes_.size()
    uint32_t modeId = supportedModesSize - 1;
    rsScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetActiveMode005
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0,
 * GetActiveMode() return not {}
 * only if one !=, into branch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetActiveMode005, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    //  IsVirtual() is false
    rsScreen->isVirtual_ = false;

    // isure GetActiveMode return not {}
    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    // default static init 0, into branch
    rsScreen->supportedModes_[0] = { .width = 200, .height = 100, .freshRate = 60, .id = 0 }; // id 0
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 };

    // hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode(_, _)).Times(1).WillOnce(testing::Return(1));

    // modeId < supportedModes_.size()
    uint32_t modeId = supportedModesSize - 1;
    rsScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetScreenActiveRect001
 * @tc.desc: SetScreenActiveRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB3986
 */
HWTEST_F(RsScreenTest, SetScreenActiveRect001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;
    GraphicIRect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(rsScreen->SetScreenActiveRect(activeRect), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetResolution002
 * @tc.desc: SetResolution Test, IsVirtual() is false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetResolution002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;
    rsScreen->SetResolution(0, 0);
}

/*
 * @tc.name: SetPowerStatus005
 * @tc.desc: SetPowerStatus Test, hdiScreen_->SetScreenVsyncEnabled(true) != GRAPHIC_DISPLAY_SUCCESS,
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetPowerStatus005, testing::ext::TestSize.Level1)
{
    ScreenId idx = mockScreenId_;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenVsyncEnabled(_, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    rsScreen->isVirtual_ = false;
    uint32_t powerStatus = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    rsScreen->SetPowerStatus(powerStatus);
}

/*
 * @tc.name: GetActiveModePosByModeId001
 * @tc.desc: GetActiveModePosByModeId Test, trigger loop, return modeIndex
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetActiveModePosByModeId001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    int32_t modeId = 0;
    rsScreen->supportedModes_[0] = { .width = 0, .height = 0, .freshRate = 0, .id = modeId };
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 };

    ASSERT_EQ(rsScreen->GetActiveModePosByModeId(modeId), 0);
}

/*
 * @tc.name: SetPowerStatus006
 * @tc.desc: SetPowerStatus Test, hdiScreen_->SetScreenVsyncEnabled(true) == GRAPHIC_DISPLAY_SUCCESS,
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetPowerStatus006, testing::ext::TestSize.Level1)
{
    ScreenId idx = mockScreenId_;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenVsyncEnabled(_, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    rsScreen->isVirtual_ = false;
    uint32_t powerStatus = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    rsScreen->SetPowerStatus(powerStatus);
}

/*
 * @tc.name: GetActiveMode002
 * @tc.desc: GetActiveMode Test, trigger branch hdiScreen_ == nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetActiveMode002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;
    rsScreen->hdiScreen_ = nullptr;

    EXPECT_EQ(rsScreen->GetActiveMode(), std::nullopt);
}

/*
 * @tc.name: PropDump001
 * @tc.desc: PropDump Test, trigger loop to AppendFormat
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, PropDump001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->capability_.propertyCount = 2;
    rsScreen->capability_.props.resize(2);
    rsScreen->capability_.props[0] = { .name = "0", .propId = 0, .value = 0 };
    rsScreen->capability_.props[1] = { .name = "1", .propId = 1, .value = 1 };

    std::string dumpString = "";
    rsScreen->PropDump(dumpString);
}

/*
 * @tc.name: CapabilityTypeDump001
 * @tc.desc: CapabilityTypeDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, CapabilityTypeDump001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::string dumpString = "";
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_HDMI, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_LCD, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BT1120, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BT656, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BUTT, dumpString);
}

/*
 * @tc.name: PowerStatusDump003
 * @tc.desc: PowerStatusDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, PowerStatusDump003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::string dumpString = "dumpString";
    uint32_t status;

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_STANDBY;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_SUSPEND;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF_FAKE;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_BUTT;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON_ADVANCED;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF_ADVANCED;
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);

    status = static_cast<GraphicDispPowerStatus>(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF_ADVANCED + 1);
    rsScreen->SetPowerStatus(status);
    rsScreen->PowerStatusDump(dumpString);
}

/*
 * @tc.name: ResizeVirtualScreen001
 * @tc.desc: ResizeVirtualScreen Test, not virtual, return directly
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, ResizeVirtualScreen001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    rsScreen->ResizeVirtualScreen(100, 100);
}

/*
 * @tc.name: SetScreenBacklight002
 * @tc.desc: SetScreenBacklight Test, trigger branch -- SetScreenBacklight(level) < 0, return directly
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenBacklight002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenBacklight(_, _)).Times(1).WillOnce(testing::Return(-1));

    rsScreen->SetScreenBacklight(1);
}

/*
 * @tc.name: GetScreenBacklight001
 * @tc.desc: GetScreenBacklight Test, hdiScreen_->GetScreenBacklight(level) < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenBacklight001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->screenBacklightLevel_ = INVALID_BACKLIGHT_VALUE;
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenBacklight(_, _)).Times(1).WillOnce(testing::Return(-1));

    ASSERT_EQ(rsScreen->GetScreenBacklight(), INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: ScreenTypeDump002
 * @tc.desc: ScreenTypeDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, ScreenTypeDump002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::string dumpString = "dumpString";

    rsScreen->screenType_ = RSScreenType::BUILT_IN_TYPE_SCREEN;
    rsScreen->ScreenTypeDump(dumpString);

    rsScreen->screenType_ = RSScreenType::EXTERNAL_TYPE_SCREEN;
    rsScreen->ScreenTypeDump(dumpString);

    rsScreen->screenType_ = RSScreenType::VIRTUAL_TYPE_SCREEN;
    rsScreen->ScreenTypeDump(dumpString);

    rsScreen->screenType_ = RSScreenType::UNKNOWN_TYPE_SCREEN;
    rsScreen->ScreenTypeDump(dumpString);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts002
 * @tc.desc: GetScreenSupportedColorGamuts Test, cover conditions: mode.size() =? 0 when not virtual
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenSupportedColorGamuts002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;
    std::vector<ScreenColorGamut> mode;

    rsScreen->supportedPhysicalColorGamuts_.resize(0);
    ASSERT_EQ(rsScreen->GetScreenSupportedColorGamuts(mode), StatusCode::HDI_ERROR);

    rsScreen->supportedPhysicalColorGamuts_.resize(1);
    ASSERT_EQ(rsScreen->GetScreenSupportedColorGamuts(mode), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenColorGamut001
 * @tc.desc: GetScreenColorGamut Test, IsVirtual() return false and if (supportedPhysicalColorGamuts_.size() == 0)
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenColorGamut001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ScreenColorGamut mode;

    rsScreen->supportedPhysicalColorGamuts_.resize(0);
    ASSERT_EQ(rsScreen->GetScreenColorGamut(mode), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenColorGamut002
 * @tc.desc: SetScreenColorGamut Test, when modeIdx < 0, expect INVALID_ARGUMENTS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorGamut002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = -1;
    ASSERT_EQ(rsScreen->SetScreenColorGamut(modeIdx), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorGamut003
 * @tc.desc: SetScreenColorGamut Test, trigger branch: modeIdx >= hdiMode.size(), expect INVALID_ARGUMENTS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorGamut003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // modeIdx >=0
    int32_t modeIdx = 2;
    // IsVirtual() return false
    rsScreen->isVirtual_ = false;
    // hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
    // modeIdx >= static_cast<int32_t>(hdiMode.size())
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([modeIdx](uint32_t, std::vector<GraphicColorGamut>& gamuts) {
            gamuts.resize(modeIdx - 1);
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    ASSERT_EQ(rsScreen->SetScreenColorGamut(modeIdx), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorGamut004
 * @tc.desc: SetScreenColorGamut Test, not virtual, expect SUCCESS.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorGamut004, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // modeIdx >=0
    int32_t modeIdx = 2;
    // IsVirtual() return false
    rsScreen->isVirtual_ = false;
    // hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
    // modeIdx < static_cast<int32_t>(hdiMode.size())
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([modeIdx](uint32_t, std::vector<GraphicColorGamut>& gamuts) {
            gamuts.resize(modeIdx + 1);
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    // result == GRAPHIC_DISPLAY_SUCCESS
    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorGamut(_, _)).Times(1).WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    ASSERT_EQ(rsScreen->SetScreenColorGamut(modeIdx), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenColorGamut005
 * @tc.desc: SetScreenColorGamut Test, not virtual, expect HDI_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorGamut005, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // modeIdx >=0
    int32_t modeIdx = 2;
    // IsVirtual() return false
    rsScreen->isVirtual_ = false;
    // hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
    // modeIdx < static_cast<int32_t>(hdiMode.size())
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([modeIdx](uint32_t, std::vector<GraphicColorGamut>& gamuts) {
            gamuts.resize(modeIdx + 1);
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    // result != GRAPHIC_DISPLAY_SUCCESS
    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorGamut(_, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    ASSERT_EQ(rsScreen->SetScreenColorGamut(modeIdx), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenGamutMap003
 * @tc.desc: SetScreenGamutMap Test, not virtual, expect SUCCESS.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenGamutMap003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenGamutMap(_, _)).Times(1).WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    ScreenGamutMap mode = GAMUT_MAP_CONSTANT;
    ASSERT_EQ(rsScreen->SetScreenGamutMap(mode), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenGamutMap001
 * @tc.desc: GetScreenGamutMap Test, hdiScreen_->GetScreenGamutMap(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenGamutMap001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenGamutMap(_, _)).Times(1).WillOnce([](uint32_t, GraphicGamutMap& gamutMap) {
        GraphicGamutMap hdiMode = GRAPHIC_GAMUT_MAP_CONSTANT;
        gamutMap = hdiMode;
        return GRAPHIC_DISPLAY_SUCCESS;
    });

    ScreenGamutMap mode = GAMUT_MAP_CONSTANT;
    ASSERT_EQ(rsScreen->GetScreenGamutMap(mode), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation001
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation Test, not virtual, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetVirtualMirrorScreenCanvasRotation001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->SetVirtualMirrorScreenCanvasRotation(false));
}

/*
 * @tc.name: GetScaleMode001
 * @tc.desc: GetScaleMode Test, get scaleMode_
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScaleMode001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ScreenScaleMode scaleMode = ScreenScaleMode::UNISCALE_MODE;
    rsScreen->scaleMode_ = scaleMode;
    ASSERT_EQ(rsScreen->GetScaleMode(), scaleMode);
}

/*
 * @tc.name: GetScreenSupportedHDRFormats002
 * @tc.desc: GetScreenSupportedHDRFormats Test, IsVirtual() return  false, hdrFormats.size() == 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenSupportedHDRFormats002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedPhysicalHDRFormats_.resize(0);
    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(rsScreen->GetScreenSupportedHDRFormats(hdrFormats), StatusCode::HDI_ERROR);
}
/*
 * @tc.name: GetScreenSupportedHDRFormats003
 * @tc.desc: GetScreenSupportedHDRFormats Test, IsVirtual() return  false, hdrFormats.size() != 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenSupportedHDRFormats003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedPhysicalHDRFormats_.resize(1);
    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(rsScreen->GetScreenSupportedHDRFormats(hdrFormats), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenHDRFormat001
 * @tc.desc: GetScreenHDRFormat Test, IsVirtual() return false, supportedPhysicalHDRFormats_.size() == 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenHDRFormat001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedPhysicalHDRFormats_.resize(0);
    ScreenHDRFormat hdrFormat;
    ASSERT_EQ(rsScreen->GetScreenHDRFormat(hdrFormat), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenHDRFormat002
 * @tc.desc: SetScreenHDRFormat Test, modeIdx < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenHDRFormat002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = -1;
    ASSERT_EQ(rsScreen->SetScreenHDRFormat(modeIdx), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenHDRFormat003
 * @tc.desc: SetScreenHDRFormat Test, modeIdx > 0, IsVirtual() return false,modeIdx <
 * static_cast<int32_t>(hdrCapability_.formats.size())
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenHDRFormat003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = 1;
    decltype(rsScreen->hdrCapability_.formats.size()) formatsSize = modeIdx + 1;
    rsScreen->hdrCapability_.formats.resize(formatsSize);

    ASSERT_EQ(rsScreen->SetScreenHDRFormat(modeIdx), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces002
 * @tc.desc: GetScreenSupportedColorSpaces Test, IsVirtual() return false, colorSpaces.size() == 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenSupportedColorSpaces002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    colorSpaces.resize(0);
    ASSERT_EQ(colorSpaces.size(), 0);

    rsScreen->supportedPhysicalColorGamuts_.resize(0);
    ASSERT_EQ(rsScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces003
 * @tc.desc: GetScreenSupportedColorSpaces Test, IsVirtual() return false, colorSpaces.size() != 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetScreenSupportedColorSpaces003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    colorSpaces.resize(2);

    ASSERT_EQ(rsScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::SUCCESS);
}
/*
 * @tc.name: SetScreenColorSpace002
 * @tc.desc: SetScreenColorSpace Test, iter == COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorSpace002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_COLORSPACE_NONE;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorSpace003
 * @tc.desc: SetScreenColorSpace Test, iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual true,it ==
 * supportedVirtualColorGamuts_.end()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorSpace003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedVirtualColorGamuts_.resize(0);

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorSpace004
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) != GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorSpace004, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenColorSpace005
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * it == hdiMode.end()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorSpace005, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([](uint32_t, std::vector<GraphicColorGamut>& hdiMode) {
            hdiMode.resize(0);
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorSpace006
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * it != hdiMode.end()
 * result == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorSpace006, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([](uint32_t, std::vector<GraphicColorGamut>& hdiMode) {
            hdiMode.resize(1);
            hdiMode[0] = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_STANDARD_BT601; // it != hdiMode.end(),curIdx=0
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorGamut(_, _)).Times(1).WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenColorSpace007
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * it != hdiMode.end()
 * result != GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenColorSpace007, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([](uint32_t, std::vector<GraphicColorGamut>& hdiMode) {
            hdiMode.resize(1);
            hdiMode[0] = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_STANDARD_BT601; // it != hdiMode.end(),curIdx=0
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorGamut(_, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow001
 * @tc.desc: SetCastScreenEnableSkipWindow Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetCastScreenEnableSkipWindow001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->SetCastScreenEnableSkipWindow(false);
}

/*
 * @tc.name: GetCastScreenEnableSkipWindow001
 * @tc.desc: GetCastScreenEnableSkipWindow Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetCastScreenEnableSkipWindow001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->skipWindow_ = true;
    ASSERT_TRUE(rsScreen->GetCastScreenEnableSkipWindow());
}

/*
 * @tc.name: SetScreenConstraint001
 * @tc.desc: SetScreenConstraint Test, IsVirtual is true
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenConstraint001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenConstraint002
 * @tc.desc: SetScreenConstraint Test, IsVirtual is  fasle, hdiScreen_ == nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenConstraint002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = nullptr;

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenConstraint003
 * @tc.desc: SetScreenConstraint Test, IsVirtual is  fasle, hdiScreen_ != nullptr
 * result == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenConstraint003, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(idx));
    ASSERT_NE(rsScreen->hdiScreen_, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenConstraint(_, _, _, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenConstraint004
 * @tc.desc: SetScreenConstraint Test, IsVirtual is  fasle, hdiScreen_ != nullptr
 * result != GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetScreenConstraint004, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(idx));
    ASSERT_NE(rsScreen->hdiScreen_, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenConstraint(_, _, _, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetVirtualScreenStatus001
 * @tc.desc: SetVirtualScreenStatus Test, IsVirtual false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetVirtualScreenStatus001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: SetVirtualScreenStatus002
 * @tc.desc: SetVirtualScreenStatus Test, IsVirtual false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, SetVirtualScreenStatus002, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_TRUE(rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: GetVirtualScreenStatus001
 * @tc.desc: GetVirtualScreenStatus Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RsScreenTest, GetVirtualScreenStatus001, testing::ext::TestSize.Level1)
{
    ScreenId idx = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(idx, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->screenStatus_ = VirtualScreenStatus::VIRTUAL_SCREEN_PLAY;
    ASSERT_EQ(rsScreen->GetVirtualScreenStatus(), VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
}

} // namespace OHOS::Rosen