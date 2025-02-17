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
 * @tc.name: PowerStatusDump_003
 * @tc.desc: PowerStatusDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, PowerStatusDump_003, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: ScreenTypeDump_002
 * @tc.desc: ScreenTypeDump Test, trigger all cases of switch
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ScreenTypeDump_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: ResizeVirtualScreen_001
 * @tc.desc: ResizeVirtualScreen Test, not virtual, return directly
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, ResizeVirtualScreen_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->IsVirtual());

    rsScreen->ResizeVirtualScreen(100, 100);
}

/*
 * @tc.name: SetScreenBacklight_002
 * @tc.desc: SetScreenBacklight Test, trigger branch -- SetScreenBacklight(level) < 0, return directly
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenBacklight_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenBacklight(_, _)).Times(1).WillOnce(testing::Return(-1));

    rsScreen->SetScreenBacklight(1);
}

/*
 * @tc.name: GetScreenBacklight_001
 * @tc.desc: GetScreenBacklight Test, hdiScreen_->GetScreenBacklight(level) < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenBacklight_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->screenBacklightLevel_ = INVALID_BACKLIGHT_VALUE;
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetScreenBacklight(_, _)).Times(1).WillOnce(testing::Return(-1));

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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->isVirtual_ = false;
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ScreenColorGamut mode;

    rsScreen->supportedPhysicalColorGamuts_.resize(0);
    ASSERT_EQ(rsScreen->GetScreenColorGamut(mode), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenColorGamut_002
 * @tc.desc: SetScreenColorGamut Test, when modeIdx < 0, expect INVALID_ARGUMENTS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: SetScreenColorGamut_004
 * @tc.desc: SetScreenColorGamut Test, not virtual, expect SUCCESS.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_004, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: SetScreenColorGamut_005
 * @tc.desc: SetScreenColorGamut Test, not virtual, expect HDI_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorGamut_005, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: SetScreenGamutMap_003
 * @tc.desc: SetScreenGamutMap Test, not virtual, expect SUCCESS.
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenGamutMap_003, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenGamutMap(_, _)).Times(1).WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    ScreenGamutMap mode = GAMUT_MAP_CONSTANT;
    ASSERT_EQ(rsScreen->SetScreenGamutMap(mode), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenGamutMap_001
 * @tc.desc: GetScreenGamutMap Test, hdiScreen_->GetScreenGamutMap(hdiMode) == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScreenGamutMap_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->SetVirtualMirrorScreenCanvasRotation(false));
}

/*
 * @tc.name: GetScaleMode_001
 * @tc.desc: GetScaleMode Test, get scaleMode_
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetScaleMode_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ScreenScaleMode scaleMode = ScreenScaleMode::UNISCALE_MODE;
    rsScreen->scaleMode_ = scaleMode;
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->supportedPhysicalHDRFormats_.resize(0);
    ScreenHDRFormat hdrFormat;
    ASSERT_EQ(rsScreen->GetScreenHDRFormat(hdrFormat), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetScreenHDRFormat_002
 * @tc.desc: SetScreenHDRFormat Test, modeIdx < 0
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenHDRFormat_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    int32_t modeIdx = 1;
    decltype(rsScreen->hdrCapability_.formats.size()) formatsSize = modeIdx + 1;
    rsScreen->hdrCapability_.formats.resize(formatsSize);

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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    colorSpaces.resize(2);

    ASSERT_EQ(rsScreen->GetScreenSupportedColorSpaces(colorSpaces), StatusCode::SUCCESS);
}
/*
 * @tc.name: SetScreenColorSpace_002
 * @tc.desc: SetScreenColorSpace Test, iter == COMMON_COLOR_SPACE_TYPE_TO_RS_MAP.end()
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenColorSpace_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    GraphicCM_ColorSpaceType colorSpace = GRAPHIC_CM_COLORSPACE_NONE;
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
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
 * @tc.name: SetCastScreenEnableSkipWindow_001
 * @tc.desc: SetCastScreenEnableSkipWindow Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetCastScreenEnableSkipWindow_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->skipWindow_ = true;
    ASSERT_TRUE(rsScreen->GetCastScreenEnableSkipWindow());
}

/*
 * @tc.name: SetScreenConstraint_003
 * @tc.desc: SetScreenConstraint Test, IsVirtual is  fasle, hdiScreen_ != nullptr
 * result == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueIB7PXQ
 */
HWTEST_F(RSScreenTest, SetScreenConstraint_003, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(id));
    ASSERT_NE(rsScreen->hdiScreen_, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenConstraint(_, _, _, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenConstraint_001
 * @tc.desc: SetScreenConstraint Test, IsVirtual is true
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetScreenConstraint_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = nullptr;

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::HDI_ERROR);
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    uint64_t frameId = 0;
    uint64_t timestamp = 0;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_NONE;
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(ScreenPhysicalId(id));
    ASSERT_NE(rsScreen->hdiScreen_, nullptr);

    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, SetScreenConstraint(_, _, _, _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_PARAM_ERR));

    ASSERT_EQ(rsScreen->SetScreenConstraint(frameId, timestamp, type), StatusCode::HDI_ERROR);
}

/*
 * @tc.name: SetVirtualScreenStatus_002
 * @tc.desc: SetVirtualScreenStatus Test, IsVirtual false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetVirtualScreenStatus_002, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_TRUE(rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: SetVirtualScreenStatus_001
 * @tc.desc: SetVirtualScreenStatus Test, IsVirtual false
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, SetVirtualScreenStatus_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    ASSERT_FALSE(rsScreen->SetVirtualScreenStatus(VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: GetVirtualScreenStatus_001
 * @tc.desc: GetVirtualScreenStatus Test
 * @tc.type: FUNC
 * @tc.require: issueIAIRAN
 */
HWTEST_F(RSScreenTest, GetVirtualScreenStatus_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->screenStatus_ = VirtualScreenStatus::VIRTUAL_SCREEN_PLAY;
    ASSERT_EQ(rsScreen->GetVirtualScreenStatus(), VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);
}

/*
 * @tc.name: GetEnableVisibleRect_001
 * @tc.desc: GetEnableVisibleRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, GetEnableVisibleRect_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    bool ret = rsScreen->GetEnableVisibleRect();
    ASSERT_EQ(ret, false);

    rsScreen->enableVisibleRect_ = true;
    ret = rsScreen->GetEnableVisibleRect();
    ASSERT_EQ(ret, true);
}

/*
 * @tc.name: SetSecurityExemptionList_001
 * @tc.desc: SetSecurityExemptionList Test
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenTest, SetSecurityExemptionList_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    rsScreen->SetSecurityExemptionList(securityExemptionList);
    ASSERT_EQ(rsScreen->securityExemptionList_.size(), securityExemptionList.size());
    for (auto i = 0; i < securityExemptionList.size(); i++) {
        ASSERT_EQ(rsScreen->securityExemptionList_[i], securityExemptionList[i]);
    }
}

/*
 * @tc.name: GetSecurityExemptionList_001
 * @tc.desc: GetSecurityExemptionList Test
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenTest, GetSecurityExemptionList_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    rsScreen->securityExemptionList_ = securityExemptionList;
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
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    rsScreen->SetEnableVisibleRect(true);
    ASSERT_EQ(rsScreen->enableVisibleRect_, true);
}

/*
 * @tc.name: GetMainScreenVisibleRect_001
 * @tc.desc: GetMainScreenVisibleRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, GetMainScreenVisibleRect_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

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
 * @tc.name: SetMainScreenVisibleRect_001
 * @tc.desc: SetMainScreenVisibleRect Test
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenTest, SetMainScreenVisibleRect_001, testing::ext::TestSize.Level1)
{
    ScreenId id = 0;
    auto rsScreen = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_NE(nullptr, rsScreen);

    Rect rect = {};
    rsScreen->SetMainScreenVisibleRect(rect);
}
}