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
#include "multiscreen_param.h"

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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(rsScreen, nullptr);
    std::string dumpString = "";
    int32_t screenIndex = 0;
    rsScreen->DisplayDump(screenIndex, dumpString);
    ASSERT_NE(dumpString.size(), 0);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    virtualScreen->SetResolution(width, height);
    if (virtualScreen->IsVirtual()) {
        ASSERT_EQ(virtualScreen->Width(), width);
    }

    virtualScreen->onPropertyChange_ = [](auto& property) {};
    virtualScreen->SetResolution(width + 1, height + 1);
}

/*
 * @tc.name: SetResolution_002
 * @tc.desc: SetResolution Test, IsVirtual() is false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetResolution_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->SetResolution(0, 0);
}

/*
 * @tc.name: SetResolution003
 * @tc.desc: SetResolution003 Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetResolution003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_unique<RSScreen>(nullptr);
    EXPECT_NE(nullptr, rsScreen);

    uint32_t width = 3;
    uint32_t height = 4;

    rsScreen->property_.SetPhyWidth(10);
    auto res = rsScreen->SetResolution(width, height);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);

    rsScreen->property_.SetPhyWidth(1);
    rsScreen->property_.SetPhyHeight(10);
    res = rsScreen->SetResolution(width, height);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);

    rsScreen->property_.SetPhyWidth(3);
    rsScreen->property_.SetPhyHeight(4);
    res = rsScreen->SetResolution(width, height);
    EXPECT_EQ(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: SetResolution004
 * @tc.desc: SetResolution004 Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetResolution004, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_unique<RSScreen>(nullptr);
    EXPECT_NE(nullptr, rsScreen);

    uint32_t width = 3;
    uint32_t height = 4;

    rsScreen->property_.SetPhyWidth(1);
    rsScreen->property_.SetPhyHeight(1);
    rsScreen->onPropertyChange_ = [](auto& property) {};
    EXPECT_EQ(StatusCode::SUCCESS, rsScreen->SetResolution(width, height));
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    std::string dumpString = "";
    virtualScreen->DisplayDump(config.id, dumpString);
    ASSERT_NE(dumpString.size(), 0);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    std::vector<ScreenColorGamut> mode;
    virtualScreen->GetScreenSupportedColorGamuts(mode);
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(rsScreen, nullptr);
    rsScreen->SetScreenVsyncEnabled(true);
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    virtualScreen->SetScreenVsyncEnabled(true);
}

/*
 * @tc.name: SetPowerStatus_001
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetPowerStatus_001, testing::ext::TestSize.Level2)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);
    rsScreen->property_.SetIsVirtual(true);
    rsScreen->SetPowerStatus(static_cast<uint32_t>(1000));
    ASSERT_EQ(rsScreen->GetPowerStatus(), ScreenPowerStatus::INVALID_POWER_STATUS);
}

/*
 * @tc.name: SetPowerStatus_002
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetPowerStatus_002, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    virtualScreen->SetPowerStatus(status);
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
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
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
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF_ADVANCED;
    rsScreen->SetPowerStatus(status);
    ASSERT_EQ(rsScreen->GetPowerStatus(), status);
}

/*
 * @tc.name: SetPowerStatus_005
 * @tc.desc: SetPowerStatus Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetPowerStatus_005, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(0));
    ASSERT_NE(nullptr, rsScreen);

    uint32_t powerStatus = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    rsScreen->SetPowerStatus(powerStatus);
}

/*
 * @tc.name: SetPowerStatus_006
 * @tc.desc: SetPowerStatus Test, test POWER_STATUS_DOZE with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetPowerStatus_006, testing::ext::TestSize.Level1)
{
    auto hdiOutput = HdiOutput::CreateHdiOutput(mockScreenId_);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_DOZE;
    rsScreen->SetPowerStatus(status);
    ASSERT_EQ(rsScreen->GetPowerStatus(), status);
}

/*
 * @tc.name: SetPowerStatus_007
 * @tc.desc: SetPowerStatus Test, test POWER_STATUS_DOZE_SUSPEND with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueIBB3WT
 */
HWTEST_F(RSScreenTest, SetPowerStatus_007, testing::ext::TestSize.Level1)
{
    auto hdiOutput = HdiOutput::CreateHdiOutput(mockScreenId_);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_DOZE_SUSPEND;
    rsScreen->SetPowerStatus(status);
    ASSERT_EQ(rsScreen->GetPowerStatus(), status);
}

/*
 * @tc.name: GetPowerStatus_001
 * @tc.desc: GetPowerStatus Test with mock HDI device
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenTest, GetPowerStatus_001, testing::ext::TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    ASSERT_NE(rsScreen, nullptr);
    rsScreen->property_.SetPowerStatus(ScreenPowerStatus::INVALID_POWER_STATUS);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    rsScreen->GetPowerStatus();
    rsScreen->property_.SetPowerStatus(ScreenPowerStatus::INVALID_POWER_STATUS);
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->GetPowerStatus();
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    std::vector<ScreenHDRMetadataKey> keys;
    ASSERT_EQ(virtualScreen->GetScreenSupportedMetaDataKeys(keys), INVALID_BACKLIGHT_VALUE);
    ScreenId id = static_cast<uint64_t>(1);
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ScreenGamutMap map1 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ASSERT_EQ(virtualScreen->SetScreenGamutMap(map1), StatusCode::SUCCESS);
    ScreenGamutMap map2 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ASSERT_EQ(virtualScreen->GetScreenGamutMap(map2), StatusCode::SUCCESS);

    virtualScreen->onPropertyChange_ = [](auto&) {};
    virtualScreen->SetScreenGamutMap(map2);
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(nullptr, rsScreen);
    ScreenGamutMap map1 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto result1 = rsScreen->SetScreenGamutMap(map1);
    ASSERT_EQ(result1, StatusCode::HDI_ERROR);
    const auto result2 = rsScreen->GetScreenGamutMap(map1);
    ASSERT_EQ(result2, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenGamutMap_003
 * @tc.desc: SetScreenGamutMap Test, not virtual, expect SUCCESS.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenGamutMap_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenGamutMap(_, _)).Times(1).WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    ScreenGamutMap mode = GAMUT_MAP_CONSTANT;
    ASSERT_EQ(rsScreen->SetScreenGamutMap(mode), StatusCode::SUCCESS);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_EQ(virtualScreen->GetScreenType(), RSScreenType::VIRTUAL_TYPE_SCREEN);
}

/*
 * @tc.name: GetConnectionTypeTest
 * @tc.desc: GetConnectionType Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetConnectionTypeTest, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    virtualScreen->GetConnectionType();
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    uint32_t skipFrameInterval = 0;
    virtualScreen->SetScreenSkipFrameInterval(skipFrameInterval);
    ASSERT_EQ(virtualScreen->GetScreenSkipFrameInterval(), 0);

    virtualScreen->onPropertyChange_ = [](auto&) {};
    virtualScreen->SetScreenSkipFrameInterval(skipFrameInterval);
}

/*
 * @tc.name: SetScreenExpectedRefreshRateTest
 * @tc.desc: SetScreenExpectedRefreshRate Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetScreenExpectedRefreshRateTest, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    config.id = static_cast<uint64_t>(1000);
    auto virtualScreen = std::make_shared<RSScreen>(config);
    virtualScreen->SetScreenExpectedRefreshRate(30);
    ASSERT_EQ(virtualScreen->property_.GetSkipFrameStrategy(), SKIP_FRAME_BY_REFRESH_RATE);

    virtualScreen->onPropertyChange_ = [](auto&) {};
    virtualScreen->SetScreenExpectedRefreshRate(30);
    ASSERT_EQ(virtualScreen->property_.GetSkipFrameStrategy(), SKIP_FRAME_BY_REFRESH_RATE);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(virtualScreen->GetScreenSupportedHDRFormats(hdrFormats), StatusCode::SUCCESS);
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_EQ(rsScreen->SetPixelFormat(static_cast<GraphicPixelFormat>(20)), StatusCode::SUCCESS); //BGRA8888
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetPixelFormat(static_cast<GraphicPixelFormat>(20)), StatusCode::SUCCESS);

    virtualScreen->onPropertyChange_ = [](auto&) {};
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ASSERT_EQ(virtualScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::SUCCESS);
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(nullptr, rsScreen);
    int32_t screenIndex = 0;
    std::string dumpString = "SurfaceDumpTest";
    rsScreen->SurfaceDump(screenIndex, dumpString);
}

/*
 * @tc.name: DumpCurrentFrameLayersTest
 * @tc.desc: DumpCurrentFrameLayers Test
 * @tc.type: FUNC
 * @tc.require: issueIAXTPS
 */
HWTEST_F(RSScreenTest, DumpCurrentFrameLayersTest, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->DumpCurrentFrameLayers();
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
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
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
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
    auto rsScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(rsScreen, nullptr);
    uint32_t status = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    rsScreen->SetPowerStatus(status);
    std::string dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerStatus=INVALID_POWER_STATUS");
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
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    // Set status to GRAPHIC_POWER_STATUS_ON_ADVANCED
    rsScreen->SetPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON_ADVANCED);
    std::string dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerStatus=POWER_STATUS_ON_ADVANCED");
    // Set status to GRAPHIC_POWER_STATUS_OFF_ADVANCED
    rsScreen->SetPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF_ADVANCED);
    dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerStatus=POWER_STATUS_OFF_ADVANCED");
}

/**
 * @tc.name: SetVirtualScreenAutoRotationTest
 * @tc.desc: SetVirtualScreenAutoRotation Test
 * @tc.type: FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSScreenTest, SetVirtualScreenAutoRotationTest, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    ASSERT_FALSE(rsScreen->IsVirtual());
    ASSERT_EQ(rsScreen->SetVirtualScreenAutoRotation(true), StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: GetVirtualScreenAutoRotationTest
 * @tc.desc: GetVirtualScreenAutoRotation Test
 * @tc.type: FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSScreenTest, GetVirtualScreenAutoRotationTest, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);
    ASSERT_TRUE(rsScreen->IsVirtual());
    ASSERT_EQ(rsScreen->SetVirtualScreenAutoRotation(true), StatusCode::SUCCESS);

    ASSERT_TRUE(rsScreen->GetVirtualScreenAutoRotation());

    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_EQ(rsScreen->SetVirtualScreenAutoRotation(true), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetRogResolution_001
 * @tc.desc: SetRogResolution Test
 * @tc.type: FUNC
 * @tc.require: issueI8JJXW
 */
HWTEST_F(RSScreenTest, SetRogResolution_001, testing::ext::TestSize.Level1)
{
    ScreenId id = INVALID_SCREEN_ID;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(rsScreen, nullptr);
    uint32_t newWidth = 100;
    uint32_t newHeight = 100;
    rsScreen->SetRogResolution(newWidth, newHeight);
    ASSERT_NE(nullptr, rsScreen->hdiScreen_);
    ASSERT_EQ(0, rsScreen->Width());
    ASSERT_EQ(0, rsScreen->Height());
}

/**
 * @tc.name: SetRogResolution_002
 * @tc.desc: SetRogResolution Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetRogResolution_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint32_t width = 100;
    uint32_t height = 100;

    rsScreen->property_.SetWidth(width + 1);
    rsScreen->property_.SetHeight(height + 1);

    rsScreen->property_.SetPhyWidth(width + 1);
    rsScreen->property_.SetPhyHeight(height + 1);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // case 1: hdiScreen_->SetScreenOverlayResolution failed
    EXPECT_CALL(*hdiDeviceMock_, SetScreenOverlayResolution(_, _, _)).Times(1).WillOnce(testing::Return(-1));
    rsScreen->SetRogResolution(width, height);

    // case 2:hdiScreen_->SetScreenOverlayResolution success
    EXPECT_CALL(*hdiDeviceMock_, SetScreenOverlayResolution(_, _, _)).Times(1).WillOnce(testing::Return(0));
    rsScreen->SetRogResolution(width, height);
}

/**
 * @tc.name: GetRogResolution_001
 * @tc.desc: test GetRogResolution with mock HDI device
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenTest, GetRogResolution_001, testing::ext::TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    uint32_t width{0};
    uint32_t height{0};
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    
    ASSERT_NE(nullptr, hdiDeviceMock_);
    ASSERT_NE(nullptr, rsScreen);
    ASSERT_NE(nullptr, rsScreen->hdiScreen_);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // case 1: GetRogResolution without prior setup
    ASSERT_EQ(rsScreen->GetRogResolution(width, height), StatusCode::INVALID_ARGUMENTS);

    // case 2: GetRogResolution with prior setup
    rsScreen->isRogResolution_ = true;
    ASSERT_EQ(rsScreen->GetRogResolution(width, height), StatusCode::SUCCESS);
}

/*
 * @tc.name: ScreenCapabilityInit_001
 * @tc.desc: ScreenCapabilityInit Test, isVirtual_ = true
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ScreenCapabilityInit_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    
    rsScreen->property_.SetIsVirtual(true);
    ASSERT_TRUE(rsScreen->IsVirtual());
    rsScreen->ScreenCapabilityInit();
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
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCapability)
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
 * @tc.name: SetActiveMode_001
 * @tc.desc: SetActiveMode Test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSScreenTest, SetActiveMode_001, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(virtualScreen, nullptr);
    uint32_t modeId = static_cast<uint32_t>(1);
    virtualScreen->SetActiveMode(modeId);
}

/*
 * @tc.name: SetActiveMode_002
 * @tc.desc: SetActiveMode Test, IsVirtual() is false, modeId >= supportedModes_.size()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetActiveMode_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

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
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedModes_.resize(6);
    rsScreen->supportedModes_[2] = { .width = 100, .height = 100, .freshRate = 60, .id = 1};
    rsScreen->supportedModes_[3] = { .width = 200, .height = 200, .freshRate = 60, .id = 1};

    EXPECT_CALL(*hdiDeviceMock_, GetScreenMode).WillRepeatedly(testing::Return(0));

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).Times(1).WillOnce(testing::Return(-1));
    rsScreen->SetActiveMode(1);

    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).Times(1).WillOnce(testing::Return(-1));
    rsScreen->SetActiveMode(2);

    rsScreen->onPropertyChange_ = [](auto&) {};
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).Times(1).WillOnce(testing::Return(-1));
    rsScreen->SetActiveMode(3);

    constexpr int32_t HDF_ERR_NOT_SUPPORT = -5;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).Times(2).WillRepeatedly(testing::Return(HDF_ERR_NOT_SUPPORT));
    auto ret = rsScreen->SetActiveMode(1);
    EXPECT_EQ(ret, StatusCode::HDI_ERR_NOT_SUPPORT);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetActiveMode(1);
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
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    // isure GetActiveMode return {}
    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    rsScreen->supportedModes_[0] = { .width = 100, .height = 100, .freshRate = 60, .id = 1 }; // id not 0
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 }; // id not 0

    // hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).Times(2).WillRepeatedly(testing::Return(1));

    // modeId < supportedModes_.size()
    uint32_t modeId = supportedModesSize - 1;
    rsScreen->SetActiveMode(modeId);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetActiveMode(0);
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
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    // isure GetActiveMode return not {}
    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    // default static init 0, into branch
    rsScreen->supportedModes_[0] = { .width = 200, .height = 100, .freshRate = 60, .id = 0 }; // id 0
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 };

    // hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).Times(2).WillRepeatedly(testing::Return(1));

    // modeId < supportedModes_.size()
    uint32_t modeId = supportedModesSize - 1;
    rsScreen->SetActiveMode(modeId);

    rsScreen->onPropertyChange_ = [](auto&) {};
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
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    // isure GetActiveMode return not {}
    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    // default static init 0, not into branch
    rsScreen->supportedModes_[0] = { .width = 0, .height = 0, .freshRate = 0, .id = 0 }; // id 0
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 };

    // hdiScreen_->SetScreenMode(static_cast<uint32_t>(selectModeId)) > 0
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).Times(2).WillRepeatedly(testing::Return(1));

    // modeId < supportedModes_.size()
    uint32_t modeId = supportedModesSize - 1;
    rsScreen->SetActiveMode(modeId);
    
    rsScreen->onPropertyChange_ = [](auto&) {};
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
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    GraphicIRect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(rsScreen->SetScreenActiveRect(activeRect), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: GetActiveModePosByModeId_001
 * @tc.desc: GetActiveModePosByModeId Test, trigger loop, return modeIndex
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetActiveModePosByModeId_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    decltype(rsScreen->supportedModes_.size()) supportedModesSize = 2;
    rsScreen->supportedModes_.resize(supportedModesSize);

    int32_t modeId = 0;
    rsScreen->supportedModes_[0] = { .width = 0, .height = 0, .freshRate = 0, .id = modeId };
    rsScreen->supportedModes_[1] = { .width = 200, .height = 100, .freshRate = 60, .id = 1 };

    ASSERT_EQ(rsScreen->GetActiveModePosByModeId(modeId), 0);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_EQ(virtualScreen->GetActiveMode().has_value(), false);
}

/*
 * @tc.name: GetActiveMode_002
 * @tc.desc: GetActiveMode Test, trigger branch hdiScreen_ == nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetActiveMode_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_ = nullptr;

    EXPECT_EQ(rsScreen->GetActiveMode(), std::nullopt);
}

/*
 * @tc.name: CapabilityTypeDump_001
 * @tc.desc: CapabilityTypeDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, CapabilityTypeDump_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::string dumpString = "";
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_HDMI, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_LCD, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BT1120, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BT656, dumpString);
    rsScreen->CapabilityTypeDump(GRAPHIC_DISP_INTF_BUTT, dumpString);
}

/*
 * @tc.name: PropDump_001
 * @tc.desc: PropDump Test, trigger loop to AppendFormat
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, PropDump_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->capability_.propertyCount = 2;
    rsScreen->capability_.props.resize(2);
    rsScreen->capability_.props[0] = { .name = "0", .propId = 0, .value = 0 };
    rsScreen->capability_.props[1] = { .name = "1", .propId = 1, .value = 1 };

    std::string dumpString = "";
    rsScreen->PropDump(dumpString);
}

/*
 * @tc.name: PowerStatusDump_003
 * @tc.desc: PowerStatusDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, PowerStatusDump_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
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
 * @tc.name: PowerStatusDump_004
 * @tc.desc: PowerStatusDump Test, with mocked HDI device
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenTest, PowerStatusDump_004, testing::ext::TestSize.Level1)
{
    auto hdiOutput = HdiOutput::CreateHdiOutput(mockScreenId_);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    // Set status to GRAPHIC_POWER_STATUS_DOZE
    rsScreen->SetPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_DOZE);
    std::string dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerStatus=POWER_STATUS_DOZE");
    // Set status to GRAPHIC_POWER_STATUS_DOZE_SUSPEND
    rsScreen->SetPowerStatus(GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_DOZE_SUSPEND);
    dumpString = "";
    rsScreen->PowerStatusDump(dumpString);
    ASSERT_TRUE(dumpString=="powerStatus=POWER_STATUS_DOZE_SUSPEND");
}

/*
 * @tc.name: ScreenTypeDump_002
 * @tc.desc: ScreenTypeDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ScreenTypeDump_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::string dumpString = "dumpString";

    rsScreen->property_.SetScreenType(RSScreenType::BUILT_IN_TYPE_SCREEN);
    rsScreen->ScreenTypeDump(dumpString);

    rsScreen->property_.SetScreenType(RSScreenType::EXTERNAL_TYPE_SCREEN);
    rsScreen->ScreenTypeDump(dumpString);

    rsScreen->property_.SetScreenType(RSScreenType::VIRTUAL_TYPE_SCREEN);
    rsScreen->ScreenTypeDump(dumpString);

    rsScreen->property_.SetScreenType(RSScreenType::UNKNOWN_TYPE_SCREEN);
    rsScreen->ScreenTypeDump(dumpString);
}

/*
 * @tc.name: ResizeVirtualScreen_001
 * @tc.desc: ResizeVirtualScreen Test, not virtual, return directly
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ResizeVirtualScreen_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    rsScreen->ResizeVirtualScreen(100, 100);
}

/*
 * @tc.name: ResizeVirtualScreen_002
 * @tc.desc: ResizeVirtualScreen Test, not virtual, return directly
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ResizeVirtualScreen_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);

    rsScreen->ResizeVirtualScreen(100, 100);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->ResizeVirtualScreen(100, 100);
}

/*
 * @tc.name: GetPanelPowerStatus_001
 * @tc.desc: test GetPanelPowerStatus with mock HDI device
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenTest, GetPanelPowerStatus_001, testing::ext::TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // simulate successful calling
    EXPECT_CALL(*hdiDeviceMock_, GetPanelPowerStatus(mockScreenId_, _)).WillOnce(
        DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::panelPowerStatusMock_)), testing::Return(0)));
    EXPECT_EQ(rsScreen->GetPanelPowerStatus(), PanelPowerStatus::PANEL_POWER_STATUS_ON);

    // simulate fail calling
    EXPECT_CALL(*hdiDeviceMock_, GetPanelPowerStatus(mockScreenId_, _)).WillOnce(
        DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::panelPowerStatusMock_)), testing::Return(-1)));
    EXPECT_EQ(rsScreen->GetPanelPowerStatus(), PanelPowerStatus::INVALID_PANEL_POWER_STATUS);
}

/*
 * @tc.name: GetPanelPowerStatus_002
 * @tc.desc: test GetPanelPowerStatus with virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenTest, GetPanelPowerStatus_002, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    EXPECT_EQ(virtualScreen->GetPanelPowerStatus(), PanelPowerStatus::INVALID_PANEL_POWER_STATUS);
}

/*
 * @tc.name: GetPanelPowerStatus_003
 * @tc.desc: test GetPanelPowerStatus with empty HDI device
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenTest, GetPanelPowerStatus_003, testing::ext::TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_ = nullptr;
    EXPECT_EQ(rsScreen->GetPanelPowerStatus(), PanelPowerStatus::INVALID_PANEL_POWER_STATUS);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    virtualScreen->SetScreenBacklight(static_cast<uint32_t>(1000));
    ASSERT_EQ(virtualScreen->GetScreenBacklight(), INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: SetScreenBacklight_002
 * @tc.desc: SetScreenBacklight Test, trigger branch -- SetScreenBacklight(level) < 0, return directly
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenBacklight_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenBacklight(_, _)).Times(1).WillOnce(testing::Return(-1));

    rsScreen->SetScreenBacklight(1);
}

/*
 * @tc.name: SetScreenBacklight_003
 * @tc.desc: SetScreenBacklight Test, trigger branch -- IsVirtual()
 * @tc.type: FUNC
 * @tc.require: issueIC9IVH
 */
HWTEST_F(RSScreenTest, SetScreenBacklight_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);
    rsScreen->SetScreenBacklight(1);
}

/*
 * @tc.name: SetScreenBacklight_004
 * @tc.desc: SetScreenBacklight Test, trigger branch hasLogBackLightAfterPowerStatusChanged_ is true
 * @tc.type: FUNC
 * @tc.require: issueIC9IVH
 */
HWTEST_F(RSScreenTest, SetScreenBacklight_004, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenBacklight(_, _)).Times(2).WillRepeatedly(testing::Return(1));
    rsScreen->hasLogBackLightAfterPowerStatusChanged_ = false;
    rsScreen->SetScreenBacklight(1);
    EXPECT_EQ(true, rsScreen->hasLogBackLightAfterPowerStatusChanged_);
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetScreenBacklight(1);
    EXPECT_EQ(true, rsScreen->hasLogBackLightAfterPowerStatusChanged_);
}

/*
 * @tc.name: GetScreenBacklight_001
 * @tc.desc: GetScreenBacklight Test, hdiScreen_->GetScreenBacklight(level) < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenBacklight_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->backlightLevel_ = INVALID_BACKLIGHT_VALUE;
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenBacklight).Times(1).WillOnce(testing::Return(-1));

    ASSERT_EQ(rsScreen->GetScreenBacklight(), INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts_002
 * @tc.desc: GetScreenSupportedColorGamuts Test, cover conditions: mode.size() =? 0 when not virtual
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenSupportedColorGamuts_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<ScreenColorGamut> mode;

    rsScreen->supportedPhysicalColorGamuts_.resize(0);
    ASSERT_EQ(rsScreen->GetScreenSupportedColorGamuts(mode), StatusCode::HDI_ERROR);

    rsScreen->supportedPhysicalColorGamuts_.resize(1);
    ASSERT_EQ(rsScreen->GetScreenSupportedColorGamuts(mode), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenColorGamut_001
 * @tc.desc: GetScreenColorGamut Test, IsVirtual() return false and if (supportedPhysicalColorGamuts_.size() == 0)
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenColorGamut_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ScreenColorGamut mode;

    rsScreen->supportedPhysicalColorGamuts_.resize(0);
    ASSERT_EQ(rsScreen->GetScreenColorGamut(mode), StatusCode::HDI_ERROR);
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    rsScreen->SetScreenColorGamut(static_cast<int32_t>(1000));
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenColorGamut(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorGamut_002
 * @tc.desc: SetScreenColorGamut Test, when modeIdx < 0, expect INVALID_ARGUMENTS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = -1;
    ASSERT_EQ(rsScreen->SetScreenColorGamut(modeIdx), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorGamut_003
 * @tc.desc: SetScreenColorGamut Test, trigger branch: modeIdx >= hdiMode.size(), expect INVALID_ARGUMENTS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // modeIdx >=0
    int32_t modeIdx = 2;
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
 * @tc.name: SetScreenColorGamut_004
 * @tc.desc: SetScreenColorGamut Test, not virtual, expect SUCCESS.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_004, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // modeIdx >=0
    int32_t modeIdx = 2;
    // hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
    // modeIdx < static_cast<int32_t>(hdiMode.size())
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([modeIdx](uint32_t, std::vector<GraphicColorGamut>& gamuts) {
            gamuts.resize(modeIdx + 1);
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    // result == GRAPHIC_DISPLAY_SUCCESS
    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorGamut).Times(1).WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    rsScreen->supportedPhysicalColorGamuts_.resize(modeIdx + 1);
    ASSERT_EQ(rsScreen->SetScreenColorGamut(modeIdx), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenColorGamut_005
 * @tc.desc: SetScreenColorGamut Test, not virtual, expect HDI_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_005, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // modeIdx >=0
    int32_t modeIdx = 2;
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
 * @tc.name: SetScreenColorGamut_006
 * @tc.desc: SetScreenColorGamut Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_006, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    virtualScreen->supportedVirtualColorGamuts_.resize(2);
    ASSERT_EQ(virtualScreen->SetScreenColorGamut(0), StatusCode::SUCCESS);

    virtualScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_EQ(virtualScreen->SetScreenColorGamut(1), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenColorGamut_007
 * @tc.desc: SetScreenColorGamut Test, not virtual, expect SUCCESS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_007, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // modeIdx >=0
    int32_t modeIdx = 2;
    // hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
    // modeIdx < static_cast<int32_t>(hdiMode.size())
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce([modeIdx](uint32_t, std::vector<GraphicColorGamut>& gamuts) {
            gamuts.resize(modeIdx + 1);
            return GRAPHIC_DISPLAY_SUCCESS;
        });

    // result != GRAPHIC_DISPLAY_SUCCESS
    EXPECT_CALL(*hdiDeviceMock_, SetScreenColorGamut).Times(1).WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));
    rsScreen->supportedPhysicalColorGamuts_.resize(modeIdx + 1);
    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_EQ(rsScreen->SetScreenColorGamut(modeIdx), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetScreenGamutMap_001
 * @tc.desc: GetScreenGamutMap Test, hdiScreen_->GetScreenGamutMap(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenGamutMap_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
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
 * @tc.name: SetVirtualMirrorScreenCanvasRotation_001
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation Test, not virtual, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetVirtualMirrorScreenCanvasRotation_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->SetVirtualMirrorScreenCanvasRotation(false));
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation_002
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetVirtualMirrorScreenCanvasRotation_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);

    ASSERT_TRUE(rsScreen->SetVirtualMirrorScreenCanvasRotation(false));
    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_TRUE(rsScreen->SetVirtualMirrorScreenCanvasRotation(false));
}

/*
 * @tc.name: GetScaleMode_001
 * @tc.desc: GetScaleMode Test, get scaleMode_
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScaleMode_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ScreenScaleMode scaleMode = ScreenScaleMode::UNISCALE_MODE;
    rsScreen->property_.SetScreenScaleMode(scaleMode);
    ASSERT_EQ(rsScreen->GetScaleMode(), scaleMode);
}

/*
 * @tc.name: GetScreenSupportedHDRFormats_002
 * @tc.desc: GetScreenSupportedHDRFormats Test, IsVirtual() return  false, hdrFormats.size() == 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenSupportedHDRFormats_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedPhysicalHDRFormats_.resize(0);
    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(rsScreen->GetScreenSupportedHDRFormats(hdrFormats), StatusCode::HDI_ERROR);
}
/*
 * @tc.name: GetScreenSupportedHDRFormats_003
 * @tc.desc: GetScreenSupportedHDRFormats Test, IsVirtual() return  false, hdrFormats.size() != 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenSupportedHDRFormats_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedPhysicalHDRFormats_.resize(1);
    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(rsScreen->GetScreenSupportedHDRFormats(hdrFormats), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenHDRFormat_001
 * @tc.desc: GetScreenHDRFormat Test, IsVirtual() return false, supportedPhysicalHDRFormats_.size() == 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenHDRFormat_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedPhysicalHDRFormats_.resize(0);
    ScreenHDRFormat hdrFormat;
    ASSERT_EQ(rsScreen->GetScreenHDRFormat(hdrFormat), StatusCode::HDI_ERROR);
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
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_EQ(rsScreen->SetScreenHDRFormat(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
    VirtualScreenConfigs config;
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenHDRFormat(static_cast<int32_t>(1000)), INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenHDRFormat_002
 * @tc.desc: SetScreenHDRFormat Test, modeIdx < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenHDRFormat_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = -1;
    ASSERT_EQ(rsScreen->SetScreenHDRFormat(modeIdx), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenHDRFormat_003
 * @tc.desc: SetScreenHDRFormat Test, modeIdx > 0, IsVirtual() return false,modeIdx <
 * static_cast<int32_t>(hdrCapability_.formats.size())
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenHDRFormat_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = 1;
    size_t formatsSize = modeIdx + 1;
    rsScreen->hdrCapability_.formats.resize(formatsSize);
    rsScreen->supportedPhysicalHDRFormats_.resize(formatsSize);

    ASSERT_EQ(rsScreen->SetScreenHDRFormat(modeIdx), StatusCode::SUCCESS);

    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_EQ(rsScreen->SetScreenHDRFormat(modeIdx), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenHDRFormat_004
 * @tc.desc: SetScreenHDRFormat Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenHDRFormat_004, testing::ext::TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto rsScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = 1;
    rsScreen->supportedVirtualHDRFormats_.resize(modeIdx + 1);

    ASSERT_EQ(rsScreen->SetScreenHDRFormat(modeIdx), StatusCode::SUCCESS);

    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_EQ(rsScreen->SetScreenHDRFormat(modeIdx), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces_002
 * @tc.desc: GetScreenSupportedColorSpaces Test, IsVirtual() return false, colorSpaces.size() == 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenSupportedColorSpaces_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    colorSpaces.resize(0);
    ASSERT_EQ(colorSpaces.size(), 0);

    rsScreen->supportedPhysicalColorGamuts_.resize(0);
    ASSERT_EQ(rsScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces_003
 * @tc.desc: GetScreenSupportedColorSpaces Test, IsVirtual() return false, colorSpaces.size() != 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenSupportedColorSpaces_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    colorSpaces.resize(2);
    rsScreen->supportedPhysicalColorGamuts_.resize(2);
    ASSERT_EQ(rsScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::SUCCESS);
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
    auto virtualScreen = std::make_shared<RSScreen>(config);
    ASSERT_EQ(virtualScreen->SetScreenColorSpace(
        static_cast<GraphicCM_ColorSpaceType>(1 | (2 << 8) | (3 << 16) | (1 << 21))), SUCCESS);
}

/*
 * @tc.name: SetScreenColorSpace_002
 * @tc.desc: SetScreenColorSpace Test, iter == COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_SMPTE_C_LIMIT;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorSpace_003
 * @tc.desc: SetScreenColorSpace Test, iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual true,it ==
 * supportedVirtualColorGamuts_.end()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->property_.SetIsVirtual(true);

    ASSERT_EQ(rsScreen->SetScreenColorSpace(GRAPHIC_CM_SRGB_FULL), StatusCode::SUCCESS);
    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_EQ(rsScreen->SetScreenColorSpace(GRAPHIC_CM_SRGB_FULL), StatusCode::SUCCESS);

    rsScreen->supportedVirtualColorGamuts_.resize(0);
    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetScreenColorSpace_004
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) != GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_004, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedColorGamuts(_, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenColorSpace_005
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * it == hdiMode.end()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_005, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
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
 * @tc.name: SetScreenColorSpace_006
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * it != hdiMode.end()
 * result == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_006, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
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
    rsScreen->supportedPhysicalColorGamuts_.resize(10);

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenColorSpace_007
 * @tc.desc: SetScreenColorSpace Test,iter != COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end(), IsVirtual false,
 * hdiScreen_->GetScreenSupportedColorGamuts(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * it != hdiMode.end()
 * result != GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_007, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
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
 * @tc.name: SetScreenColorSpace_008
 * @tc.desc: SetScreenColorSpace Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_008, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
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

    rsScreen->supportedPhysicalColorGamuts_.resize(10);
    rsScreen->onPropertyChange_ = [](auto&) {};
    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_BT601_EBU_FULL;
    ASSERT_EQ(rsScreen->SetScreenColorSpace(colorSpace), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetBlackList_001
 * @tc.desc: SetBlackList Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetBlackList_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::unordered_set<uint64_t> blackList {};
    rsScreen->SetBlackList(blackList);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetBlackList(blackList);
}

/*
 * @tc.name: SetTypeBlackList_001
 * @tc.desc: SetTypeBlackList Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetTypeBlackList_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::unordered_set<uint8_t> blackList {};
    rsScreen->SetTypeBlackList(blackList);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetTypeBlackList(blackList);
}

/*
 * @tc.name: AddBlackListTest
 * @tc.desc: AddBlackList Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, AddBlackListTest, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<uint64_t> blackList {};
    rsScreen->AddBlackList(blackList);

    blackList.push_back(0);
    rsScreen->AddBlackList(blackList);
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->AddBlackList(blackList);

    EXPECT_FALSE(rsScreen->property_.GetBlackList().empty());
}

/*
 * @tc.name: RemoveBlackListTest
 * @tc.desc: RemoveBlackList Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, RemoveBlackListTest, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<uint64_t> blackList {};
    rsScreen->RemoveBlackList(blackList);

    blackList.push_back(0);
    rsScreen->RemoveBlackList(blackList);
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->RemoveBlackList(blackList);

    EXPECT_TRUE(rsScreen->property_.GetBlackList().empty());
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow_001
 * @tc.desc: SetCastScreenEnableSkipWindow Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetCastScreenEnableSkipWindow_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->SetCastScreenEnableSkipWindow(false);
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetCastScreenEnableSkipWindow(false);
}

/*
 * @tc.name: GetCastScreenEnableSkipWindow_001
 * @tc.desc: GetCastScreenEnableSkipWindow Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetCastScreenEnableSkipWindow_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->property_.SetCastScreenEnableSkipWindow(true);
    ASSERT_TRUE(rsScreen->GetCastScreenEnableSkipWindow());
}

/*
 * @tc.name: GetBlackList_001
 * @tc.desc: GetBlackList Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetBlackList_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::unordered_set<uint64_t> blackList {};
    rsScreen->property_.SetBlackList(blackList);
    ASSERT_TRUE(rsScreen->GetBlackList() == blackList);
}

/*
 * @tc.name: SetScreenConstraint_001
 * @tc.desc: SetScreenConstraint Test, IsVirtual is true
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenConstraint_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenConstraint_002
 * @tc.desc: SetScreenConstraint Test, IsVirtual is  fasle, hdiScreen_ == nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenConstraint_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = nullptr;

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenConstraint_003
 * @tc.desc: SetScreenConstraint Test, IsVirtual is  fasle, hdiScreen_ != nullptr
 * result == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenConstraint_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(0);
    ASSERT_NE(rsScreen->hdiScreen_, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenConstraint(_, _, _, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenConstraint_004
 * @tc.desc: SetScreenConstraint Test, IsVirtual is  fasle, hdiScreen_ != nullptr
 * result != GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenConstraint_004, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(0);
    ASSERT_NE(rsScreen->hdiScreen_, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenConstraint(_, _, _, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetVirtualScreenStatus_001
 * @tc.desc: SetVirtualScreenStatus Test, IsVirtual false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetVirtualScreenStatus_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: SetVirtualScreenStatus_002
 * @tc.desc: SetVirtualScreenStatus Test, IsVirtual false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetVirtualScreenStatus_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);
    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_TRUE(rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: SetVirtualScreenStatus_003
 * @tc.desc: SetVirtualScreenStatus Test, IsVirtual true and diffrent virtual screen status
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetVirtualScreenStatus_003, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);

    ASSERT_TRUE(rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE));
}

/*
 * @tc.name: GetVirtualScreenStatus_001
 * @tc.desc: GetVirtualScreenStatus Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetVirtualScreenStatus_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->property_.SetScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
    ASSERT_EQ(rsScreen->GetVirtualScreenStatus(), VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
}

/*
 * @tc.name: SetSecurityExemptionList_001
 * @tc.desc: SetSecurityExemptionList Test
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenTest, SetSecurityExemptionList_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    rsScreen->SetSecurityExemptionList(securityExemptionList);
    auto list = rsScreen->GetSecurityExemptionList();
    ASSERT_EQ(list.size(), securityExemptionList.size());
    for (auto i = 0; i < securityExemptionList.size(); i++) {
        ASSERT_EQ(list[i], securityExemptionList[i]);
    }
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetSecurityExemptionList(securityExemptionList);
}

/*
 * @tc.name: GetSecurityExemptionList_001
 * @tc.desc: GetSecurityExemptionList Test
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenTest, GetSecurityExemptionList_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    rsScreen->SetSecurityExemptionList(securityExemptionList);
    auto securityExemptionListGet = rsScreen->GetSecurityExemptionList();
    ASSERT_EQ(securityExemptionListGet.size(), securityExemptionList.size());
    for (auto i = 0; i < securityExemptionList.size(); i++) {
        ASSERT_EQ(securityExemptionListGet[i], securityExemptionList[i]);
    }
}

/*
 * @tc.name: SetEnableVisibleRect_001
 * @tc.desc: SetEnableVisibleRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetEnableVisibleRect_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->SetEnableVisibleRect(true);
    ASSERT_EQ(rsScreen->GetEnableVisibleRect(), true);
    rsScreen->onPropertyChange_ = [](auto&) {};
    ASSERT_EQ(rsScreen->GetEnableVisibleRect(), true);
}

/*
 * @tc.name: GetEnableVisibleRect_001
 * @tc.desc: GetEnableVisibleRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, GetEnableVisibleRect_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);

    bool ret = rsScreen->GetEnableVisibleRect();
    ASSERT_EQ(ret, false);

    rsScreen->SetEnableVisibleRect(true);
    ret = rsScreen->GetEnableVisibleRect();
    ASSERT_EQ(ret, true);
}

/*
 * @tc.name: SetMainScreenVisibleRect_001
 * @tc.desc: SetMainScreenVisibleRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetMainScreenVisibleRect_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);

    Rect rect = {};
    rsScreen->SetMainScreenVisibleRect(rect);
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetMainScreenVisibleRect(rect);
}

/*
 * @tc.name: GetMainScreenVisibleRect_001
 * @tc.desc: GetMainScreenVisibleRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, GetMainScreenVisibleRect_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);

    uint32_t width = 720;  // width value for test
    uint32_t height = 1280;  // height value for test
    Rect rectSet = {0, 0, width, height};
    rsScreen->SetMainScreenVisibleRect(rectSet);
    auto rectGet = rsScreen->GetMainScreenVisibleRect();
    ASSERT_EQ(rectSet.x, rectGet.x);
    ASSERT_EQ(rectSet.y, rectGet.y);
    ASSERT_EQ(rectSet.w, rectGet.w);
    ASSERT_EQ(rectSet.h, rectGet.h);
}

/*
 * Function: SetHasProtectedLayer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: create RSScreen
 *                  2. operation: SetHasProtectedLayer and GetHasProtectedLayer
 *                  3. result: return hasProtectedLayer
 */
HWTEST_F(RSScreenTest, SetHasProtectedLayer_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);

    rsScreen->SetHasProtectedLayer(true);
    auto ret = rsScreen->GetHasProtectedLayer();
    ASSERT_EQ(ret, true);
    rsScreen->SetHasProtectedLayer(false);
    ret = rsScreen->GetHasProtectedLayer();
    ASSERT_EQ(ret, false);
}

/*
 * @tc.name: SetSecurityMaskTest
 * @tc.desc: SetSecurityMask Test with virtualScreenId
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenTest, SetSecurityMaskTest, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);
    auto ret = rsScreen->SetSecurityMask(nullptr);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetSecurityMask(nullptr);
}

/*
 * @tc.name: GetScreenSecurityMask001
 * @tc.desc: GetScreenSecurityMask Test
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenTest, GetSecurityMask001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetIsVirtual(true);
    rsScreen->SetSecurityMask(nullptr);
    auto SecurityMaskGet = rsScreen->GetSecurityMask();
    ASSERT_EQ(SecurityMaskGet, nullptr);
}

/*
 * @tc.name: GetVirtualSecLayerOption001
 * @tc.desc: GetVirtualSecLayerOption Test
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSScreenTest, GetVirtualSecLayerOption001, testing::ext::TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    VirtualScreenConfigs configs;
    configs.id = 1;
    configs.associatedScreenId = 0;
    configs.name = "virtualScreen02";
    configs.width = 480;
    configs.height = 320;
    configs.surface = psurface;
    configs.flags = 1;
    auto rsScreen = std::make_shared<RSScreen>(configs);
    ASSERT_NE(nullptr, rsScreen);
    ASSERT_EQ(rsScreen->property_.GetVirtualSecLayerOption(), 1);
}

/*
 * @tc.name: SetScreenLinearMatrix_001
 * @tc.desc: SetScreenLinearMatrix Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetScreenLinearMatrix_001, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_unique<RSScreen>(nullptr);
    EXPECT_NE(nullptr, rsScreen);

    std::vector<float> matrix1 = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    std::vector<float> matrix2(5, 3.14f);

    rsScreen->property_.SetIsVirtual(true);
    auto res = rsScreen->SetScreenLinearMatrix(matrix1);
    EXPECT_EQ(StatusCode::VIRTUAL_SCREEN, res);

    rsScreen->property_.SetIsVirtual(false);
    rsScreen->hdiScreen_ = nullptr;
    res = rsScreen->SetScreenLinearMatrix(matrix1);
    EXPECT_EQ(StatusCode::HDI_ERROR, res);

    rsScreen->hdiScreen_ = std::make_unique<HdiScreen>(100);
    res = rsScreen->SetScreenLinearMatrix(matrix1);
    EXPECT_EQ(StatusCode::SUCCESS, res);
    res = rsScreen->SetScreenLinearMatrix(matrix2);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);
}

/*
 * @tc.name: SetScreenLinearMatrix_002
 * @tc.desc: SetScreenLinearMatrix Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetScreenLinearMatrix_002, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_unique<RSScreen>(nullptr);
    EXPECT_NE(nullptr, rsScreen);

    std::vector<float> matrix1 = {1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    std::vector<float> matrix2 = {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    rsScreen->hdiScreen_ = std::make_unique<HdiScreen>(100);

    ASSERT_NE(hdiDeviceMock_, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetDisplayPerFrameParameterSmq)
        .Times(2)
        .WillRepeatedly(testing::Return(0));
    auto res = rsScreen->SetScreenLinearMatrix(matrix1);

    rsScreen->onPropertyChange_ = [](auto&) {};
    res = rsScreen->SetScreenLinearMatrix(matrix2);
}

/*
 * @tc.name: GetDisplayIdentificationData
 * @tc.desc: GetDisplayIdentificationData Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, GetDisplayIdentificationData, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_unique<RSScreen>(nullptr);
    EXPECT_NE(nullptr, rsScreen);

    uint8_t port = 1;
    std::vector<uint8_t> edid(5, port);

    rsScreen->hdiScreen_ = nullptr;
    auto res = rsScreen->RSScreen::GetDisplayIdentificationData(port, edid);
    EXPECT_EQ(HDI_ERROR, res);

    rsScreen->hdiScreen_ = std::make_unique<HdiScreen>(100);

    res = rsScreen->RSScreen::GetDisplayIdentificationData(port, edid);
    EXPECT_EQ(HDI_ERROR, res);
}

/*
 * @tc.name: SetScreenActiveRect
 * @tc.desc: SetScreenActiveRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetScreenActiveRect, testing::ext::TestSize.Level1)
{
    auto rsScreen = std::make_unique<RSScreen>(nullptr);
    EXPECT_NE(nullptr, rsScreen);

    GraphicIRect actRect = {1, 2, 3, 4};

    rsScreen->property_.SetIsVirtual(true);
    auto res = rsScreen->SetScreenActiveRect(actRect);
    EXPECT_EQ(StatusCode::HDI_ERROR, res);

    rsScreen->property_.SetIsVirtual(false);
    rsScreen->hdiScreen_ = nullptr;
    res = rsScreen->SetScreenActiveRect(actRect);
    EXPECT_EQ(StatusCode::HDI_ERROR, res);

    rsScreen->hdiScreen_ = std::make_unique<HdiScreen>(100);
    actRect.w = 0;
    rsScreen->SetScreenActiveRect(actRect);

    rsScreen->property_.SetWidth(10);
    actRect.w = 13;
    rsScreen->SetScreenActiveRect(actRect);

    actRect.w = 3;
    actRect.h = 0;
    rsScreen->SetScreenActiveRect(actRect);

    rsScreen->property_.SetHeight(10);
    actRect.h = 14;
    rsScreen->SetScreenActiveRect(actRect);

    actRect.h = 4;
    actRect.x = -1;
    res = rsScreen->SetScreenActiveRect(actRect);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);

    actRect.x = 11;
    rsScreen->SetScreenActiveRect(actRect);

    actRect.x = 1;
    actRect.y = -2;
    rsScreen->SetScreenActiveRect(actRect);

    actRect.y = 12;
    res = rsScreen->SetScreenActiveRect(actRect);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);

    actRect.y = 2;
    res = rsScreen->SetScreenActiveRect(actRect);
    EXPECT_EQ(StatusCode::HDI_ERROR, res);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenActiveRect)
        .Times(1)
        .WillOnce(testing::Return(0));
    res = rsScreen->SetScreenActiveRect(actRect);
    EXPECT_EQ(StatusCode::SUCCESS, res);

    rsScreen->onPropertyChange_ = [](auto&) {};
    EXPECT_CALL(*hdiDeviceMock_, SetScreenActiveRect)
        .Times(1)
        .WillOnce(testing::Return(0));
    res = rsScreen->SetScreenActiveRect(actRect);
    EXPECT_EQ(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: PhysicalScreenInit
 * @tc.desc: PhysicalScreenInit Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, PhysicalScreenInit, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    EXPECT_EQ(rsScreen->property_.GetSkipFrameStrategy(), SKIP_FRAME_BY_INTERVAL);

    id = 1;
    rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(id));
    ASSERT_NE(nullptr, rsScreen);
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    EXPECT_EQ(rsScreen->property_.GetSkipFrameStrategy(), SKIP_FRAME_BY_ACTIVE_REFRESH_RATE);
}

/*
 * @tc.name: SetScreenOffset
 * @tc.desc: SetScreenOffset Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenOffset, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(0));
    rsScreen->SetScreenOffset(0, 0);
    EXPECT_NE(rsScreen, nullptr);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetScreenOffset(0, 0);
}

/*
 * @tc.name: SetProducerSurfaceTest
 * @tc.desc: SetProducerSurface Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetProducerSurfaceTest, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    rsScreen->property_.SetId(INVALID_SCREEN_ID);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    rsScreen->SetProducerSurface(psurface);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->property_.SetId(0);
    rsScreen->SetProducerSurface(nullptr);
    EXPECT_EQ(rsScreen->GetProducerSurface(), nullptr);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleModeTest
 * @tc.desc: SetVirtualMirrorScreenScaleMode Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetVirtualMirrorScreenScaleModeTest, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    EXPECT_EQ(rsScreen->SetVirtualMirrorScreenScaleMode(ScreenScaleMode::UNISCALE_MODE), false);

    rsScreen->property_.SetIsVirtual(true);
    EXPECT_EQ(rsScreen->SetVirtualMirrorScreenScaleMode(ScreenScaleMode::UNISCALE_MODE), true);

    rsScreen->onPropertyChange_ = [](auto&) {};
    EXPECT_EQ(rsScreen->SetVirtualMirrorScreenScaleMode(ScreenScaleMode::UNISCALE_MODE), true);
}

/*
 * @tc.name: SetVisibleRectSupportRotationTest
 * @tc.desc: SetVisibleRectSupportRotation Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetVisibleRectSupportRotationTest, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    rsScreen->SetVisibleRectSupportRotation(true);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetVisibleRectSupportRotation(true);

    EXPECT_TRUE(rsScreen->GetVisibleRectSupportRotation());
}

/*
 * @tc.name: InitDisplayPropertyForHardCursorTest
 * @tc.desc: InitDisplayPropertyForHardCursor Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, InitDisplayPropertyForHardCursorTest, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    rsScreen->hdiScreen_ = nullptr;
    rsScreen->InitDisplayPropertyForHardCursor();
    EXPECT_EQ(rsScreen->hdiScreen_, nullptr);
}

/*
 * @tc.name: SetDualScreenStateTest
 * @tc.desc: SetDualScreenState Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetDualScreenStateTest, TestSize.Level1)
{
    // case 1: hdiScreen_ null
    {
        DualScreenStatus status = DualScreenStatus::DUAL_SCREEN_ENTER;
        auto rsScreen = std::make_shared<RSScreen>(nullptr);
        rsScreen->hdiScreen_ = nullptr;
        EXPECT_EQ(rsScreen->SetDualScreenState(status), StatusCode::HDI_ERROR);
    }

    // case 2: virtual screen
    {
        DualScreenStatus status = DualScreenStatus::DUAL_SCREEN_ENTER;
        VirtualScreenConfigs config;
        auto virtualScreen = std::make_shared<RSScreen>(config);
        EXPECT_EQ(virtualScreen->SetDualScreenState(status), StatusCode::VIRTUAL_SCREEN);
    }

    // case 3: mock hdi error
    {
        ScreenId screenId = mockScreenId_;
        DualScreenStatus status = DualScreenStatus::DUAL_SCREEN_STATUS_BUTT;
        auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
        auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
        rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
        EXPECT_CALL(*hdiDeviceMock_, SetDisplayProperty(_, _, _)).WillOnce(testing::Return(-1));
        EXPECT_EQ(rsScreen->SetDualScreenState(status), StatusCode::HDI_ERROR);
    }

    // case 4
    {
        ScreenId screenId = mockScreenId_;
        DualScreenStatus status = DualScreenStatus::DUAL_SCREEN_ENTER;
        auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
        auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
        rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
        EXPECT_CALL(*hdiDeviceMock_, SetDisplayProperty(_, _, _)).WillOnce(testing::Return(0));
        EXPECT_EQ(rsScreen->SetDualScreenState(status), StatusCode::SUCCESS);
    }
}

/*
 * @tc.name: WriteHisyseventEpsLcdInfo_001
 * @tc.desc: Test WriteHisyseventEpsLcdInfo with mode change
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, WriteHisyseventEpsLcdInfo_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    // Test with mode change
    GraphicDisplayModeInfo newMode = { .width = 1920, .height = 1080, .freshRate = 60, .id = 1 };
    rsScreen->WriteHisyseventEpsLcdInfo(newMode);

    // Test with same mode (no event expected)
    rsScreen->WriteHisyseventEpsLcdInfo(newMode);
}

/*
 * @tc.name: GetOffsetX_001
 * @tc.desc: Test GetOffsetX getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetOffsetX_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->SetScreenOffset(100, 200);
    EXPECT_EQ(rsScreen->GetOffsetX(), 100);
}

/*
 * @tc.name: GetOffsetY_001
 * @tc.desc: Test GetOffsetY getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetOffsetY_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->SetScreenOffset(100, 200);
    EXPECT_EQ(rsScreen->GetOffsetY(), 200);
}

/*
 * @tc.name: IsSamplingOn_001
 * @tc.desc: Test IsSamplingOn getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, IsSamplingOn_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetIsSamplingOn(true);
    EXPECT_TRUE(rsScreen->IsSamplingOn());
}

/*
 * @tc.name: GetSamplingTranslateX_001
 * @tc.desc: Test GetSamplingTranslateX getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetSamplingTranslateX_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetSamplingTranslateX(1.5f);
    EXPECT_FLOAT_EQ(rsScreen->GetSamplingTranslateX(), 1.5f);
}

/*
 * @tc.name: GetSamplingTranslateY_001
 * @tc.desc: Test GetSamplingTranslateY getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetSamplingTranslateY_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetSamplingTranslateY(2.5f);
    EXPECT_FLOAT_EQ(rsScreen->GetSamplingTranslateY(), 2.5f);
}

/*
 * @tc.name: GetSamplingScale_001
 * @tc.desc: Test GetSamplingScale getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetSamplingScale_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetSamplingScale(0.8f);
    EXPECT_FLOAT_EQ(rsScreen->GetSamplingScale(), 0.8f);
}

/*
 * @tc.name: GetActiveRect_001
 * @tc.desc: Test GetActiveRect getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetActiveRect_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    RectI testRect(10, 20, 100, 200);
    rsScreen->property_.SetActiveRect(testRect);
    auto rect = rsScreen->GetActiveRect();
    EXPECT_EQ(rect.left_, 10);
    EXPECT_EQ(rect.top_, 20);
    EXPECT_EQ(rect.width_, 100);
    EXPECT_EQ(rect.height_, 200);
}

/*
 * @tc.name: GetMaskRect_001
 * @tc.desc: Test GetMaskRect getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetMaskRect_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    RectI testRect(5, 10, 50, 100);
    rsScreen->property_.SetMaskRect(testRect);
    auto rect = rsScreen->GetMaskRect();
    EXPECT_EQ(rect.left_, 5);
    EXPECT_EQ(rect.top_, 10);
    EXPECT_EQ(rect.width_, 50);
    EXPECT_EQ(rect.height_, 100);
}

/*
 * @tc.name: GetReviseRect_001
 * @tc.desc: Test GetReviseRect getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetReviseRect_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    RectI testRect(15, 25, 150, 250);
    rsScreen->property_.SetReviseRect(testRect);
    auto rect = rsScreen->GetReviseRect();
    EXPECT_EQ(rect.left_, 15);
    EXPECT_EQ(rect.top_, 25);
    EXPECT_EQ(rect.width_, 150);
    EXPECT_EQ(rect.height_, 250);
}

/*
 * @tc.name: GetActiveRefreshRate_001
 * @tc.desc: Test GetActiveRefreshRate getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetActiveRefreshRate_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetRefreshRate(60);
    EXPECT_EQ(rsScreen->GetActiveRefreshRate(), 60);
}

/*
 * @tc.name: GetSupportedModes_001
 * @tc.desc: Test GetSupportedModes getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetSupportedModes_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->supportedModes_.push_back({ .width = 1920, .height = 1080, .freshRate = 60, .id = 1 });
    const auto& modes = rsScreen->GetSupportedModes();
    EXPECT_EQ(modes.size(), 1);
    EXPECT_EQ(modes[0].width, 1920);
}

/*
 * @tc.name: GetCapability_001
 * @tc.desc: Test GetCapability getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetCapability_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->capability_.name = "TestCapability";
    const auto& capability = rsScreen->GetCapability();
    EXPECT_EQ(capability.name, "TestCapability");
}

/*
 * @tc.name: GetOutput_001
 * @tc.desc: Test GetOutput getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetOutput_001, TestSize.Level1)
{
    ScreenId id = 0;
    auto hdiOutput = HdiOutput::CreateHdiOutput(id);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    ASSERT_NE(rsScreen, nullptr);

    auto output = rsScreen->GetOutput();
    EXPECT_NE(output, nullptr);
}

/*
 * @tc.name: GetOutput_002
 * @tc.desc: Test GetOutput with null output
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetOutput_002, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    auto output = rsScreen->GetOutput();
    EXPECT_EQ(output, nullptr);
}

/*
 * @tc.name: GetProducerSurface_001
 * @tc.desc: Test GetProducerSurface getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetProducerSurface_001, TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto rsScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(rsScreen, nullptr);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    rsScreen->SetProducerSurface(psurface);
    auto surface = rsScreen->GetProducerSurface();
    EXPECT_NE(surface, nullptr);
}

/*
 * @tc.name: SetScreenCorrection_001
 * @tc.desc: Test SetScreenCorrection
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenCorrection_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->SetScreenCorrection(ScreenRotation::ROTATION_90);
    EXPECT_EQ(rsScreen->GetScreenCorrection(), ScreenRotation::ROTATION_90);

    rsScreen->onPropertyChange_ = [](auto&) {};
    rsScreen->SetScreenCorrection(ScreenRotation::ROTATION_180);
    EXPECT_EQ(rsScreen->GetScreenCorrection(), ScreenRotation::ROTATION_180);
}

/*
 * @tc.name: GetScreenCorrection_001
 * @tc.desc: Test GetScreenCorrection getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetScreenCorrection_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetScreenCorrection(ScreenRotation::ROTATION_270);
    EXPECT_EQ(rsScreen->GetScreenCorrection(), ScreenRotation::ROTATION_270);
}

/*
 * @tc.name: GetScreenSkipFrameStrategy_001
 * @tc.desc: Test GetScreenSkipFrameStrategy getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetScreenSkipFrameStrategy_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetSkipFrameStrategy(SKIP_FRAME_BY_REFRESH_RATE);
    EXPECT_EQ(rsScreen->GetScreenSkipFrameStrategy(), SKIP_FRAME_BY_REFRESH_RATE);
}

/*
 * @tc.name: GetCanvasRotation_001
 * @tc.desc: Test GetCanvasRotation getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetCanvasRotation_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetIsVirtual(true);
    rsScreen->property_.SetCanvasRotation(true);
    EXPECT_TRUE(rsScreen->GetCanvasRotation());
}

/*
 * @tc.name: GetScaleMode_002
 * @tc.desc: Test GetScaleMode getter (new)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetScaleMode_002, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetScreenScaleMode(ScreenScaleMode::UNISCALE_MODE);
    EXPECT_EQ(rsScreen->GetScaleMode(), ScreenScaleMode::UNISCALE_MODE);
}

/*
 * @tc.name: GetPixelFormat_001
 * @tc.desc: Test GetPixelFormat getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetPixelFormat_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    GraphicPixelFormat pixelFormat;
    rsScreen->property_.SetPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_8888);
    auto ret = rsScreen->GetPixelFormat(pixelFormat);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
}

/*
 * @tc.name: GetWhiteList_001
 * @tc.desc: Test GetWhiteList getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetWhiteList_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    std::unordered_set<NodeId> whiteList = {1, 2, 3};
    rsScreen->SetWhiteList(whiteList);
    auto retrievedList = rsScreen->GetWhiteList();
    EXPECT_EQ(retrievedList.size(), 3);
    EXPECT_TRUE(retrievedList.find(1) != retrievedList.end());
}

/*
 * @tc.name: GetTypeBlackList_001
 * @tc.desc: Test GetTypeBlackList getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetTypeBlackList_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    std::unordered_set<uint8_t> typeBlackList = {1, 2, 3};
    rsScreen->SetTypeBlackList(typeBlackList);
    auto retrievedList = rsScreen->GetTypeBlackList();
    EXPECT_EQ(retrievedList.size(), 3);
    EXPECT_TRUE(retrievedList.find(1) != retrievedList.end());
}

/*
 * @tc.name: GetVisibleRectSupportRotation_001
 * @tc.desc: Test GetVisibleRectSupportRotation getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetVisibleRectSupportRotation_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetVisibleRectSupportRotation(true);
    EXPECT_TRUE(rsScreen->GetVisibleRectSupportRotation());
}

/*
 * @tc.name: GetAndResetWhiteListChange_001
 * @tc.desc: Test GetAndResetWhiteListChange atomic operation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetAndResetWhiteListChange_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->SetWhiteListChange(true);
    EXPECT_TRUE(rsScreen->GetAndResetWhiteListChange());
    EXPECT_FALSE(rsScreen->GetAndResetWhiteListChange());
}

/*
 * @tc.name: SetWhiteListChange_001
 * @tc.desc: Test SetWhiteListChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetWhiteListChange_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->SetWhiteListChange(true);
    EXPECT_TRUE(rsScreen->GetAndResetWhiteListChange());
}

/*
 * @tc.name: GetAndResetPSurfaceChange_001
 * @tc.desc: Test GetAndResetPSurfaceChange atomic operation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetAndResetPSurfaceChange_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->SetPSurfaceChange(true);
    EXPECT_TRUE(rsScreen->GetAndResetPSurfaceChange());
    EXPECT_FALSE(rsScreen->GetAndResetPSurfaceChange());
}

/*
 * @tc.name: SetPSurfaceChange_001
 * @tc.desc: Test SetPSurfaceChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetPSurfaceChange_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->SetPSurfaceChange(true);
    EXPECT_TRUE(rsScreen->GetAndResetPSurfaceChange());
}

/*
 * @tc.name: SetResolution_Sampling_001
 * @tc.desc: Test SetResolution with sampling enabled (width > phyWidth, height > phyHeight)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetResolution_Sampling_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    uint32_t phyWidth = 100;
    uint32_t phyHeight = 100;
    uint32_t width = 200;
    uint32_t height = 200;

    rsScreen->property_.SetPhyWidth(phyWidth);
    rsScreen->property_.SetPhyHeight(phyHeight);
    rsScreen->property_.SetIsVirtual(false);

    auto ret = rsScreen->SetResolution(width, height);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_TRUE(rsScreen->IsSamplingOn());
    EXPECT_FLOAT_EQ(rsScreen->GetSamplingScale(), 0.5f);
    EXPECT_FLOAT_EQ(rsScreen->GetSamplingTranslateX(), 0.0f);
    EXPECT_FLOAT_EQ(rsScreen->GetSamplingTranslateY(), 0.0f);
}

/*
 * @tc.name: SetResolution_Sampling_002
 * @tc.desc: Test SetResolution with sampling enabled (only width > phyWidth)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetResolution_Sampling_002, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    uint32_t phyWidth = 100;
    uint32_t phyHeight = 200;
    uint32_t width = 200;
    uint32_t height = 200;

    rsScreen->property_.SetPhyWidth(phyWidth);
    rsScreen->property_.SetPhyHeight(phyHeight);
    rsScreen->property_.SetIsVirtual(false);

    auto ret = rsScreen->SetResolution(width, height);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_TRUE(rsScreen->IsSamplingOn());
    EXPECT_FLOAT_EQ(rsScreen->GetSamplingScale(), 0.5f); // min(100/200, 200/200) = 0.5
}

/*
 * @tc.name: SetResolution_NoSampling_001
 * @tc.desc: Test SetResolution with sampling disabled (width == phyWidth && height == phyHeight)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetResolution_NoSampling_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    uint32_t phyWidth = 100;
    uint32_t phyHeight = 100;
    uint32_t width = 100;
    uint32_t height = 100;

    rsScreen->property_.SetPhyWidth(phyWidth);
    rsScreen->property_.SetPhyHeight(phyHeight);
    rsScreen->property_.SetIsVirtual(false);

    auto ret = rsScreen->SetResolution(width, height);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_FALSE(rsScreen->IsSamplingOn());
}

/*
 * @tc.name: SetActiveMode_ResolutionChange_001
 * @tc.desc: Test SetActiveMode with resolution changing and HDF_ERR_NOT_SUPPORT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetActiveMode_ResolutionChange_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->supportedModes_.resize(2);
    rsScreen->supportedModes_[0] = { .width = 100, .height = 100, .freshRate = 60, .id = 0 };
    rsScreen->supportedModes_[1] = { .width = 200, .height = 200, .freshRate = 60, .id = 1 };
    rsScreen->property_.SetPhyWidth(100);
    rsScreen->property_.SetPhyHeight(100);
    rsScreen->property_.SetState(ScreenState::HDI_OUTPUT_ENABLE);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    constexpr int32_t HDF_ERR_NOT_SUPPORT = -5;

    EXPECT_CALL(*hdiDeviceMock_, GetScreenMode).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetScreenMode).WillOnce(testing::Return(HDF_ERR_NOT_SUPPORT));

    auto ret = rsScreen->SetActiveMode(1);
    EXPECT_EQ(ret, StatusCode::HDI_ERR_NOT_SUPPORT);
    // Verify state was restored after HDF_ERR_NOT_SUPPORT
    EXPECT_EQ(rsScreen->property_.GetState(), ScreenState::HDI_OUTPUT_ENABLE);
}

/*
 * @tc.name: GetPowerStatus_HDIFailure_001
 * @tc.desc: Test GetPowerStatus with HDI failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetPowerStatus_HDIFailure_001, TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    rsScreen->property_.SetPowerStatus(ScreenPowerStatus::INVALID_POWER_STATUS);

    EXPECT_CALL(*hdiDeviceMock_, GetScreenPowerStatus(mockScreenId_, _)).WillOnce(testing::Return(-1));

    auto status = rsScreen->GetPowerStatus();
    EXPECT_EQ(status, ScreenPowerStatus::INVALID_POWER_STATUS);
}

/*
 * @tc.name: SetScreenActiveRect_MaskCalculation_001
 * @tc.desc: Test SetScreenActiveRect with mask calculation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenActiveRect_MaskCalculation_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetWidth(100);
    rsScreen->property_.SetHeight(100);

    GraphicIRect activeRect = { .x = 0, .y = 0, .w = 50, .h = 50 };

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenActiveRect).WillOnce(testing::Return(0));

    auto ret = rsScreen->SetScreenActiveRect(activeRect);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenBacklight_HDIFailure_001
 * @tc.desc: Test GetScreenBacklight with HDI failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetScreenBacklight_HDIFailure_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->backlightLevel_ = INVALID_BACKLIGHT_VALUE;
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenBacklight).WillOnce(testing::Return(-1));

    auto level = rsScreen->GetScreenBacklight();
    EXPECT_EQ(level, INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: GetPanelPowerStatus_OutOfRange_001
 * @tc.desc: Test GetPanelPowerStatus with out of range status
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetPanelPowerStatus_OutOfRange_001, TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // Return status >= GRAPHIC_PANEL_POWER_STATUS_BUTT
    EXPECT_CALL(*hdiDeviceMock_, GetPanelPowerStatus(mockScreenId_, _))
        .WillOnce(DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::panelPowerStatusMock_)),
            testing::Return(0)));

    auto status = rsScreen->GetPanelPowerStatus();
    EXPECT_NE(status, PanelPowerStatus::INVALID_PANEL_POWER_STATUS);
}

/*
 * @tc.name: SetScreenColorGamut_NullHdiScreen_001
 * @tc.desc: Test SetScreenColorGamut with null hdiScreen_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_NullHdiScreen_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->hdiScreen_ = nullptr;
    rsScreen->property_.SetIsVirtual(false);

    int32_t modeIdx = 0;
    auto ret = rsScreen->SetScreenColorGamut(modeIdx);
    EXPECT_EQ(ret, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenGamutMap_HDIFailure_001
 * @tc.desc: Test SetScreenGamutMap with HDI failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenGamutMap_HDIFailure_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenGamutMap).WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    auto ret = rsScreen->SetScreenGamutMap(mode);
    EXPECT_EQ(ret, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetScreenGamutMap_HDIFailure_001
 * @tc.desc: Test GetScreenGamutMap with HDI failure
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetScreenGamutMap_HDIFailure_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenGamutMap).WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    ScreenGamutMap mode;
    auto ret = rsScreen->GetScreenGamutMap(mode);
    EXPECT_EQ(ret, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetDisplayIdentificationData_Success_001
 * @tc.desc: Test GetDisplayIdentificationData with success
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetDisplayIdentificationData_Success_001, TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    uint8_t outPort = 0;
    std::vector<uint8_t> edidData;

    EXPECT_CALL(*hdiDeviceMock_, GetDisplayIdentificationData)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    auto ret = rsScreen->GetDisplayIdentificationData(outPort, edidData);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenHDRFormat_BoundsCheck_001
 * @tc.desc: Test SetScreenHDRFormat with modeIdx >= hdrCapability_.formats.size()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenHDRFormat_BoundsCheck_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->hdrCapability_.formats.resize(2);
    rsScreen->property_.SetIsVirtual(false);

    int32_t modeIdx = 5; // >= formats.size()
    auto ret = rsScreen->SetScreenHDRFormat(modeIdx);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: GetScreenHDRFormat_ValidFormats_001
 * @tc.desc: Test GetScreenHDRFormat with valid formats
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetScreenHDRFormat_ValidFormats_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->supportedPhysicalHDRFormats_.push_back(VIDEO_HDR10);
    rsScreen->property_.SetIsVirtual(false);

    ScreenHDRFormat hdrFormat;
    auto ret = rsScreen->GetScreenHDRFormat(hdrFormat);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_EQ(hdrFormat, VIDEO_HDR10);
}

/*
 * @tc.name: GetActiveMode_GetScreenModeFailure_001
 * @tc.desc: Test GetActiveMode when GetScreenMode fails
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetActiveMode_GetScreenModeFailure_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenMode).WillOnce(testing::Return(-1));

    auto mode = rsScreen->GetActiveMode();
    EXPECT_FALSE(mode.has_value());
}

/*
 * @tc.name: GetActiveMode_ModeNotFound_001
 * @tc.desc: Test GetActiveMode when mode not found in supportedModes_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetActiveMode_ModeNotFound_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenMode).WillOnce(testing::Return(0));

    // Leave supportedModes_ empty or with different id
    rsScreen->supportedModes_.resize(1);
    rsScreen->supportedModes_[0] = { .width = 100, .height = 100, .freshRate = 60, .id = 999 };

    auto mode = rsScreen->GetActiveMode();
    EXPECT_FALSE(mode.has_value());
}

/*
 * @tc.name: SetVirtualScreenStatus_VirtualScreenPlay_001
 * @tc.desc: Test SetVirtualScreenStatus with VIRTUAL_SCREEN_PLAY
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetVirtualScreenStatus_VirtualScreenPlay_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetIsVirtual(true);

    rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
    EXPECT_TRUE(rsScreen->GetAndResetVirtualScreenPlay());
    EXPECT_FALSE(rsScreen->GetAndResetVirtualScreenPlay());
}

/*
 * @tc.name: PhysicalScreenInit_HDIFailure_001
 * @tc.desc: Test PhysicalScreenInit with various HDI failures
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, PhysicalScreenInit_HDIFailure_001, TestSize.Level1)
{
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<RSScreen>(hdiOutput);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // Test GetScreenSupportedModes failure
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedModes).WillOnce(testing::Return(-1));
    rsScreen->PhysicalScreenInit();

    // Test GetHDRCapabilityInfos failure
    EXPECT_CALL(*hdiDeviceMock_, GetScreenSupportedModes).WillRepeatedly(testing::Return(-1));
    rsScreen->PhysicalScreenInit();
}

/*
 * @tc.name: VirtualScreenInit_HDRSetup_001
 * @tc.desc: Test VirtualScreenInit HDR capability setup
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, VirtualScreenInit_HDRSetup_001, TestSize.Level1)
{
    VirtualScreenConfigs config;
    auto rsScreen = std::make_shared<RSScreen>(config);
    ASSERT_NE(rsScreen, nullptr);

    // Verify HDR capability was initialized
    EXPECT_GT(rsScreen->hdrCapability_.formatCount, 0);
}

/*
 * @tc.name: GetScreenInfo_001
 * @tc.desc: Test GetScreenInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetScreenInfo_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    auto screenInfo = rsScreen->GetScreenInfo();
    EXPECT_EQ(screenInfo.id, rsScreen->property_.GetId());
}

/*
 * @tc.name: GetProperty_001
 * @tc.desc: Test GetProperty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetProperty_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    auto property = rsScreen->GetProperty();
    EXPECT_NE(property, nullptr);
    EXPECT_EQ(property->GetScreenId(), rsScreen->property_.GetId());
}

/*
 * @tc.name: SetOnPropertyChangedCallback_001
 * @tc.desc: Test SetOnPropertyChangedCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetOnPropertyChangedCallback_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    bool callbackInvoked = false;
    rsScreen->SetOnPropertyChangedCallback([&callbackInvoked](const sptr<RSScreenProperty>&) {
        callbackInvoked = true;
    });

    rsScreen->SetScreenCorrection(ScreenRotation::ROTATION_90);
    EXPECT_TRUE(callbackInvoked);
}

/*
 * @tc.name: SetScreenActiveRect_BoundaryTests_001
 * @tc.desc: Test SetScreenActiveRect with various boundary conditions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenActiveRect_BoundaryTests_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    rsScreen->property_.SetWidth(100);
    rsScreen->property_.SetHeight(100);

    // Test x + w > width boundary
    GraphicIRect activeRect1 = { .x = 50, .y = 0, .w = 60, .h = 50 };
    auto ret1 = rsScreen->SetScreenActiveRect(activeRect1);
    EXPECT_EQ(ret1, StatusCode::INVALID_ARGUMENTS);

    // Test y + h > height boundary
    GraphicIRect activeRect2 = { .x = 0, .y = 50, .w = 50, .h = 60 };
    auto ret2 = rsScreen->SetScreenActiveRect(activeRect2);
    EXPECT_EQ(ret2, StatusCode::INVALID_ARGUMENTS);

    // Test valid boundary
    GraphicIRect activeRect3 = { .x = 0, .y = 0, .w = 100, .h = 100 };
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenActiveRect).WillOnce(testing::Return(0));
    auto ret3 = rsScreen->SetScreenActiveRect(activeRect3);
    EXPECT_EQ(ret3, StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenLinearMatrix_EqualityCheck_001
 * @tc.desc: Test SetScreenLinearMatrix equality check
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, SetScreenLinearMatrix_EqualityCheck_001, TestSize.Level1)
{
    auto rsScreen = std::make_unique<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    std::vector<float> matrix1 = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    rsScreen->hdiScreen_ = std::make_unique<HdiScreen>(100);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // First call should invoke HDI
    EXPECT_CALL(*hdiDeviceMock_, SetDisplayPerFrameParameterSmq).WillOnce(testing::Return(0));
    auto ret1 = rsScreen->SetScreenLinearMatrix(matrix1);
    EXPECT_EQ(ret1, StatusCode::SUCCESS);

    // Second call with same matrix should return SUCCESS without HDI call (early return)
    auto ret2 = rsScreen->SetScreenLinearMatrix(matrix1);
    EXPECT_EQ(ret2, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetHDRCapability_001
 * @tc.desc: Test GetHDRCapability with maxLum set
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenTest, GetHDRCapability_001, TestSize.Level1)
{
    auto rsScreen = std::make_shared<RSScreen>(nullptr);
    ASSERT_NE(rsScreen, nullptr);

    const auto& hdrCapability = rsScreen->GetHDRCapability();
    EXPECT_EQ(hdrCapability.maxLum, 1000);
}
} // namespace OHOS::Rosen
