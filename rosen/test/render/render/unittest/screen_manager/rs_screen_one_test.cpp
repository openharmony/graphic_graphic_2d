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
#include "limit_number.h"
#include "screen_manager/rs_screen.h"
#include "mock_hdi_device.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline ScreenId mockScreenId_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void RSScreenTest::SetUpTestCase()
{
    mockScreenId_ = 0xFFFF;
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, SetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SaveArg<1>(&Mock::HdiDeviceMock::powerStatusMock_), testing::Return(0)));
    EXPECT_CALL(*hdiDeviceMock_, GetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::powerStatusMock_)), testing::Return(0)));
}
void RSScreenTest::TearDownTestCase() {}
void RSScreenTest::SetUp() {}
void RSScreenTest::TearDown() {}

/*
 * @tc.name: DisplayDump_001
 * @tc.desc: Test PhysicalScreen DisplayDump
 * @tc.type: FUNC
 * @tc.require: issueI60RFZ
 */
HWTEST_F(RSScreenTest, DisplayDump_001, testing::ext::TestSize.Level2)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    std::string dumpString = "";
    int32_t screenIndex = 0;
    rsScreen->DisplayDump(screenIndex, dumpString);
    ASSERT_NE(dumpString.size(), 0);
}

/*
 * @tc.name: IsEnable_001
 * @tc.desc: IsEnable Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, IsEnable_001, testing::ext::TestSize.Level1)
{
    ScreenId id = INVALID_SCREEN_ID;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_EQ(rsScreen->IsEnable(), false);
}

/*
 * @tc.name: SetResolution_001
 * @tc.desc: SetResolution Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetResolution_001, testing::ext::TestSize.Level1)
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
 * @tc.name: SetPowerStatus_001
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetPowerStatus_001, testing::ext::TestSize.Level2)
{
    ScreenId id = INVALID_SCREEN_ID;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    rsScreen->SetPowerStatus(static_cast<uint32_t>(1000));
    ASSERT_EQ(rsScreen->GetPowerStatus(), ScreenPowerStatus::INVALID_POWER_STATUS);
}

/*
 * @tc.name: ScreenTypeDump_001
 * @tc.desc: ScreenTypeDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, ScreenTypeDump_001, testing::ext::TestSize.Level1)
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
 * @tc.name: SetScreenBacklight_001
 * @tc.desc: SetScreenBacklight Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenBacklight_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->SetScreenBacklight(static_cast<uint32_t>(1000));
    ASSERT_EQ(virtualScreen->GetScreenBacklight(), INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: GetAndResetVirtualSurfaceUpdateFlag_001
 * @tc.desc: Test get virtualSurface update flag correctly
 * @tc.type: FUNC
 * @tc.require: issueIA9QG0
 */
HWTEST_F(RSScreenTest, GetAndResetVirtualSurfaceUpdateFlag_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    virtualScreen->isVirtualSurfaceUpdateFlag_ = true;
    ASSERT_EQ(virtualScreen->GetAndResetVirtualSurfaceUpdateFlag(), true);
    virtualScreen->isVirtualSurfaceUpdateFlag_ = false;
    ASSERT_EQ(virtualScreen->GetAndResetVirtualSurfaceUpdateFlag(), false);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts_001
 * @tc.desc: GetScreenSupportedColorGamuts Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetScreenSupportedColorGamuts_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    std::vector<ScreenColorGamut> mode;
    virtualScreen->GetScreenSupportedColorGamuts(mode);
}

/*
 * @tc.name: SetScreenColorGamut_001
 * @tc.desc: SetScreenColorGamut Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_001, testing::ext::TestSize.Level1)
{
    ScreenId id = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    rsScreen->SetScreenColorGamut(static_cast<int32_t>(1000));
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenColorGamut(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenVsyncEnabled_001
 * @tc.desc: SetScreenVsyncEnabled Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenVsyncEnabled_001, testing::ext::TestSize.Level1)
{
    ScreenId id = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    rsScreen->SetScreenVsyncEnabled(true);
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    virtualScreen->SetScreenVsyncEnabled(true);
}

/*
 * @tc.name: SetActiveMode_001
 * @tc.desc: SetActiveMode Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetActiveMode_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    uint32_t modeId = static_cast<uint32_t>(1);
    virtualScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetPowerStatus_003
 * @tc.desc: SetPowerStatus Test, test POWER_STATUS_ON_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetPowerStatus_003, testing::ext::TestSize.Level1)
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
 * @tc.name: SetPowerStatus_004
 * @tc.desc: SetPowerStatus Test, test POWER_STATUS_OFF_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetPowerStatus_004, testing::ext::TestSize.Level1)
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
 * @tc.name: GetActiveMode_001
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetActiveMode_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->GetActiveMode().has_value(), false);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys_001
 * @tc.desc: GetScreenSupportedMetaDataKeys Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetScreenSupportedMetaDataKeys_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<ScreenHDRMetadataKey> keys;
    ASSERT_EQ(virtualScreen->GetScreenSupportedMetaDataKeys(keys), INVALID_BACKLIGHT_VALUE);
    ScreenId id = static_cast<uint64_t>(1);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_EQ(rsScreen->GetScreenSupportedMetaDataKeys(keys), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenGamutMap_001
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenGamutMap_001, testing::ext::TestSize.Level1)
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
 * @tc.name: SetScreenGamutMap_002
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenGamutMap_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_NE(nullptr, rsScreen);
    ScreenGamutMap map1 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto result1 = rsScreen->SetScreenGamutMap(map1);
    ASSERT_EQ(result1, StatusCode::HDI_ERROR);
    const auto result2 = rsScreen->GetScreenGamutMap(map1);
    ASSERT_EQ(result2, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetActiveModePosByModeIdTest
 * @tc.desc: GetActiveModePosByModeId Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetActiveModePosByModeIdTest001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    int32_t modeId = 0;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    auto res = virtualScreen->GetActiveModePosByModeId(modeId);
    ASSERT_EQ(res, -1);
}

/*
 * @tc.name: GetScreenTypeTest
 * @tc.desc: GetScreenType Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, GetScreenTypeTest, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    auto res = virtualScreen->GetScreenType();
    auto type = RSScreenType::VIRTUAL_TYPE_SCREEN;
    ASSERT_EQ(res, type);
}

/*
 * @tc.name: SetScreenSkipFrameIntervalTest
 * @tc.desc: SetScreenSkipFrameInterval Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenSkipFrameIntervalTest, testing::ext::TestSize.Level1)
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
 * @tc.name: GetScreenSupportedHDRFormats_001
 * @tc.desc: GetScreenSupportedHDRFormats Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenTest, GetScreenSupportedHDRFormats_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(virtualScreen->GetScreenSupportedHDRFormats(hdrFormats), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenHDRFormat_001
 * @tc.desc: SetScreenHDRFormat Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenTest, SetScreenHDRFormat_001, testing::ext::TestSize.Level1)
{
    ScreenId id = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_EQ(rsScreen->SetScreenHDRFormat(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenHDRFormat(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetPixelFormat_001
 * @tc.desc: SetPixelFormat Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenTest, SetPixelFormat_001, testing::ext::TestSize.Level1)
{
    ScreenId id = static_cast<uint64_t>(1000);
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_EQ(rsScreen->SetPixelFormat(static_cast<GraphicPixelFormat>(20)), StatusCode::SUCCESS); //BGRA8888
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetPixelFormat(static_cast<GraphicPixelFormat>(20)), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces_001
 * @tc.desc: GetScreenSupportedColorSpaces Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenTest, GetScreenSupportedColorSpaces_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ASSERT_EQ(virtualScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenColorSpace_001
 * @tc.desc: SetScreenColorSpace Test
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_unique<impl::RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenColorSpace(
        static_cast<GraphicCM_ColorSpaceType>(1 | (2 << 8) | (3 << 16) | (1 << 21))), SUCCESS);
}

/*
 * @tc.name: SurfaceDumpTest
 * @tc.desc: SurfaceDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SurfaceDumpTest, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "SurfaceDumpTest";
    rsScreen->SurfaceDump(screenIndex, dumpString);
}

/*
 * @tc.name: FpsDumpTest
 * @tc.desc: FpsDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, FpsDumpTest, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
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
HWTEST_F(RSScreenTest, ClearFpsDumpTest_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "ClearFpsDumpTest";
    std::string arg = "ClearFpsDumpTest";
    rsScreen->ClearFpsDump(screenIndex, dumpString, arg);
}

/*
 * @tc.name: ClearFpsDumpTest
 * @tc.desc: ClearFpsDump Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, ClearFpsDumpTest_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "ClearFpsDumpTest";
    std::string arg = "ClearFpsDumpTest";
    rsScreen->ClearFpsDump(screenIndex, dumpString, arg);
}

/*
 * @tc.name: PowerStatusDump_001
 * @tc.desc: PowerStatusDump Test, virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, PowerStatusDump_001, testing::ext::TestSize.Level1)
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
 * @tc.name: PowerStatusDump_002
 * @tc.desc: PowerStatusDump Test, with mocked HDI device
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, PowerStatusDump_002, testing::ext::TestSize.Level1)
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
 * @tc.name: ScreenCapabilityInit_001
 * @tc.desc: ScreenCapabilityInit Test, isVirtual_ = true
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ScreenCapabilityInit_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_TRUE(rsScreen->IsVirtual());
    rsScreen->ScreenCapabilityInit();
}

/*
 * @tc.name: SetRogResolution_002
 * @tc.desc: SetRogResolution Test, trigger branch -- hdiScreen_->SetScreenOverlayResolution(width, height) < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetRogResolution_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: ScreenCapabilityInit_002
 * @tc.desc: ScreenCapabilityInit Test, isVirtual_ is false, hdiScreen_->GetScreenCapability return not
 * GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ScreenCapabilityInit_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCapability(id, _))
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
 * @tc.name: IsEnable_002
 * @tc.desc: IsEnable Test, id is valid and 4 conditions of hdiOutput_ nad producerSurface_
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, IsEnable_002, testing::ext::TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);

    ScreenId id = 0;

    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);
    ASSERT_NE(rsScreen->id_, INVALID_SCREEN_ID);

    ASSERT_FALSE(rsScreen->IsEnable());

    rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_TRUE(rsScreen->IsEnable());

    rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, psurface);
    ASSERT_TRUE(rsScreen->IsEnable());

    rsScreen = std::make_unique<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), psurface);
    ASSERT_TRUE(rsScreen->IsEnable());
}

/*
 * @tc.name: SetActiveMode_002
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId >= supportedModes_.size()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetActiveMode_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;

    rsScreen->supportedModes_.resize(1);
    uint32_t modeId = 5;

    rsScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetActiveMode_003
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetActiveMode_003, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;

    rsScreen->supportedModes_.resize(6);
    uint32_t modeId = 1;
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode(_, _)).Times(1).WillOnce(testing::Return(-1));

    rsScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetActiveMode_006
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0,
 * GetActiveMode() return not {}
 * only if one !=, not into branch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetActiveMode_006, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: SetActiveMode_004
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0,
 * GetActiveMode() return {}
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetActiveMode_004, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: SetActiveMode_005
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId < supportedModes_.size(),
 * hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0,
 * GetActiveMode() return not {}
 * only if one !=, into branch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetActiveMode_005, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
HWTEST_F(RSScreenTest, SetScreenActiveRect001, testing::ext::TestSize.Level1)
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
}

/*
 * @tc.name: SetResolution_002
 * @tc.desc: SetResolution Test, IsVirtual() is false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetResolution_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;
    rsScreen->SetResolution(0, 0);
}

/*
 * @tc.name: GetActiveModePosByModeId_001
 * @tc.desc: GetActiveModePosByModeId Test, trigger loop, return modeIndex
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetActiveModePosByModeId_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    int32_t modeId = 0;
    rsScreen->supportedModes_[0] = { .width = 0, .height = 0, .freshRate = 0, .id = modeId };
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 };

    ASSERT_EQ(rsScreen->GetActiveModePosByModeId(modeId), 0);
}

/*
 * @tc.name: PropDump_001
 * @tc.desc: PropDump Test, trigger loop to AppendFormat
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, PropDump_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->capability_.propertyCount = 2;
    rsScreen->capability_.props.resize(2);
    rsScreen->capability_.props[0] = { .name = "0", .propId = 0, .value = 0 };
    rsScreen->capability_.props[1] = { .name = "1", .propId = 1, .value = 1 };

    std::string dumpString = "";
    rsScreen->PropDump(dumpString);
}

/*
 * @tc.name: SetPowerStatus_005
 * @tc.desc: SetPowerStatus Test, hdiScreen_->SetScreenVsyncEnabled(true) != GRAPHIC_DISPLAY_SUCCESS,
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetPowerStatus_005, testing::ext::TestSize.Level1)
{
    ScreenId id = mockScreenId_;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: SetPowerStatus_006
 * @tc.desc: SetPowerStatus Test, hdiScreen_->SetScreenVsyncEnabled(true) == GRAPHIC_DISPLAY_SUCCESS,
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetPowerStatus_006, testing::ext::TestSize.Level1)
{
    ScreenId id = mockScreenId_;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: GetActiveMode_002
 * @tc.desc: GetActiveMode Test, trigger branch hdiScreen_ == nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetActiveMode_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;
    rsScreen->hdiScreen_ = nullptr;
}

/*
 * @tc.name: CapabilityTypeDump_001
 * @tc.desc: CapabilityTypeDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, CapabilityTypeDump_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::string dumpString = "";
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_HDMI, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_LCD, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BT1120, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BT656, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BUTT, dumpString);
}
} // namespace OHOS::Rosen