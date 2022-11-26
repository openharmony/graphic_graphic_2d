/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "transaction/rs_interfaces.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSInterfacesTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSInterfacesTest" };

    static void SetUpTestCase()
    {
        rsInterfaces = &(RSInterfaces::GetInstance());
    }

    static void TearDownTestCase()
    {
        rsInterfaces = nullptr;
    }

    static inline RSInterfaces* rsInterfaces = nullptr;
};

/*
* Function: GetDefaultScreenId
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetDefaultScreenId
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetDefaultScreenId, Function | SmallTest | Level2)
{
    ScreenId defaultScreenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(defaultScreenId, INVALID_SCREEN_ID);
}

/*
* Function: CreateVirtualScreen
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateVirtualScreen, use normal parameters.
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, CreateVirtualScreen001, Function | SmallTest | Level2)
{
    auto csurface = Surface::CreateSurfaceAsConsumer();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(csurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual0", 320, 180, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/*
* Function: CreateVirtualScreen
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateVirtualScreen, use nullptr surface
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, CreateVirtualScreen002, Function | SmallTest | Level2)
{
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual0", 320, 180, nullptr, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/*
* Function: CreateVirtualScreen
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateVirtualScreen twice with the same surface
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, CreateVirtualScreen003, Function | SmallTest | Level2)
{
    auto csurface = Surface::CreateSurfaceAsConsumer();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(csurface, nullptr);

    ScreenId virtualScreenId1 = rsInterfaces->CreateVirtualScreen(
        "virtual1", 320, 180, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId1, INVALID_SCREEN_ID);

    ScreenId virtualScreenId2 = rsInterfaces->CreateVirtualScreen(
        "virtual2", 320, 180, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_EQ(virtualScreenId2, INVALID_SCREEN_ID);

    rsInterfaces->RemoveVirtualScreen(virtualScreenId1);
}

/*
* Function: CreateVirtualScreen
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call CreateVirtualScreen with other get/set funcs
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, CreateVirtualScreen004, Function | SmallTest | Level2)
{
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual0", 320, 180, nullptr, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    auto supportedScreenModes = rsInterfaces->GetScreenSupportedModes(virtualScreenId);
    EXPECT_EQ(supportedScreenModes.size(), 0);

    rsInterfaces->SetScreenActiveMode(virtualScreenId, 0);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(virtualScreenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), -1);

    rsInterfaces->SetScreenPowerStatus(virtualScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    usleep(50000); // wait 50000us to ensure SetScreenPowerStatus done.
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(virtualScreenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::INVALID_POWER_STATUS);

    auto screenCapability = rsInterfaces->GetScreenCapability(virtualScreenId);
    EXPECT_EQ(screenCapability.GetPhyWidth(), 0);
    EXPECT_EQ(screenCapability.GetName(), "virtual0");

    auto backLight = rsInterfaces->GetScreenBacklight(virtualScreenId);
    EXPECT_EQ(backLight, -1);
    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/*
* Function: GetScreenSupportedModes
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedModes
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenSupportedModes001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    auto supportedScreenModes = rsInterfaces->GetScreenSupportedModes(screenId);
    EXPECT_GT(supportedScreenModes.size(), 0);
}

/*
* Function: GetScreenSupportedModes
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedModes with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenSupportedModes002, Function | SmallTest | Level2)
{
    auto supportedScreenModes = rsInterfaces->GetScreenSupportedModes(INVALID_SCREEN_ID);
    EXPECT_EQ(supportedScreenModes.size(), 0);
}

/*
* Function: SetScreenActiveMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveMode
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenActiveMode001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
}

/*
* Function: SetScreenActiveMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveMode
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenActiveMode002, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    auto supportedScreenModes = rsInterfaces->GetScreenSupportedModes(screenId);
    EXPECT_GT(supportedScreenModes.size(), 0);

    rsInterfaces->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
}

/*
* Function: GetScreenActiveMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveMode
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenActiveMode001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
    EXPECT_NE(modeInfo.GetScreenRefreshRate(), 0);
    EXPECT_NE(modeInfo.GetScreenHeight(), -1);
    EXPECT_NE(modeInfo.GetScreenWidth(), -1);
}

/*
* Function: GetScreenActiveMode
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveMode with INVALID_SCREEN_ID
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenActiveMode002, Function | SmallTest | Level2)
{
    auto modeInfo = rsInterfaces->GetScreenActiveMode(INVALID_SCREEN_ID);
    EXPECT_EQ(modeInfo.GetScreenHeight(), -1);
    EXPECT_EQ(modeInfo.GetScreenWidth(), -1);
}

/*
* Function: SetScreenPowerStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenPowerStatus with INVALID_SCREEN_ID
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenPowerStatus001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenPowerStatus(INVALID_SCREEN_ID, ScreenPowerStatus::POWER_STATUS_STANDBY);
}

/*
* Function: SetScreenPowerStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenPowerStatus with value of POWER_STATUS_ON
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenPowerStatus002, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    usleep(50000); // wait 50000us to ensure SetScreenPowerStatus done.
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(screenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::POWER_STATUS_ON);
}

/*
* Function: GetScreenPowerStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenPowerStatus when set POWER_STATUS_ON
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenPowerStatus001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    usleep(50000); // wait 50000us to ensure SetScreenPowerStatus done.
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(screenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::POWER_STATUS_ON);
}

/*
* Function: GetScreenPowerStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenPowerStatus when INVALID screenID
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenPowerStatus002, Function | SmallTest | Level2)
{
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(INVALID_SCREEN_ID);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::INVALID_POWER_STATUS);
}

/*
* Function: GetScreenCapability
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenCapability
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenCapability001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    auto screenCapability = rsInterfaces->GetScreenCapability(screenId);
    EXPECT_NE(screenCapability.GetType(), DISP_INVALID);
}

/*
* Function: GetScreenCapability
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenCapability with INVALID_SCREEN_ID
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenCapability002, Function | SmallTest | Level2)
{
    auto screenCapability = rsInterfaces->GetScreenCapability(INVALID_SCREEN_ID);
    EXPECT_EQ(screenCapability.GetPhyWidth(), 0);
    EXPECT_EQ(screenCapability.GetPhyHeight(), 0);
}

/*
* Function: GetScreenData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenData
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenData001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    auto screenData = rsInterfaces->GetScreenData(screenId);
    EXPECT_GT(screenData.GetSupportModeInfo().size(), 0);

    auto screenCapability = screenData.GetCapability();
    EXPECT_NE(screenCapability.GetType(), DISP_INVALID);

    auto modeInfo = screenData.GetActivityModeInfo();
    EXPECT_NE(modeInfo.GetScreenModeId(), -1);
    EXPECT_NE(modeInfo.GetScreenRefreshRate(), 0);
    EXPECT_NE(modeInfo.GetScreenHeight(), -1);
    EXPECT_NE(modeInfo.GetScreenWidth(), -1);
}

/*
* Function: GetScreenData
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenData with INVALID_SCREEN_ID
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenData002, Function | SmallTest | Level2)
{
    auto screenData = rsInterfaces->GetScreenData(INVALID_SCREEN_ID);
    EXPECT_EQ(screenData.GetSupportModeInfo().size(), 0);

    auto screenCapability = screenData.GetCapability();
    EXPECT_EQ(screenCapability.GetPhyWidth(), 0);
    EXPECT_EQ(screenCapability.GetPhyHeight(), 0);

    auto modeInfo = screenData.GetActivityModeInfo();
    EXPECT_EQ(modeInfo.GetScreenHeight(), -1);
    EXPECT_EQ(modeInfo.GetScreenWidth(), -1);
}

/*
* Function: SetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenBacklight with value:50
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenBacklight001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 50);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 50);
}

/*
* Function: SetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenBacklight with value:100
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenBacklight002, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 100);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 100);
}

/*
* Function: GetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenBacklight with value: 50
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenBacklight001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 50);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 50);
}

/*
* Function: GetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenBacklight INVALID_SCREEN_ID
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenBacklight002, Function | SmallTest | Level2)
{
    auto backLight = rsInterfaces->GetScreenBacklight(INVALID_SCREEN_ID);
    EXPECT_EQ(backLight, -1);
}

/*
* Function: SetScreenChangeCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenChangeCallback
*                  2. wait 2s and check the ret
*/
HWTEST_F(RSInterfacesTest, SetScreenChangeCallback, Function | SmallTest | Level2)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    bool callbacked = false;
    auto callback = [&screenId, &screenEvent, &callbacked](ScreenId id, ScreenEvent event) {
        screenId = id;
        screenEvent = event;
        callbacked = true;
    };
    int32_t status = rsInterfaces->SetScreenChangeCallback(callback);
    EXPECT_EQ(status, StatusCode::SUCCESS);
    sleep(2); // wait 2s to check if the callback returned.
    if (status == StatusCode::SUCCESS) {
        EXPECT_NE(screenId, INVALID_SCREEN_ID);
        EXPECT_NE(screenEvent, ScreenEvent::UNKNOWN);
    } else {
        EXPECT_EQ(screenId, INVALID_SCREEN_ID);
        EXPECT_EQ(screenEvent, ScreenEvent::UNKNOWN);
    }
}

/*
* Function: GetScreenSupportedColorGamuts
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedColorGamuts with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenSupportedColorGamuts002, Function | SmallTest | Level2)
{
    std::vector<ScreenColorGamut> modes;
    int ret = rsInterfaces->GetScreenSupportedColorGamuts(INVALID_SCREEN_ID, modes);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenSupportedMetaDataKeys
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedMetaDataKeys
*                  2. check ret
* @tc.require: IssueI5KGK4
*/
HWTEST_F(RSInterfacesTest, GetScreenSupportedMetaDataKeys001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    std::vector<ScreenHDRMetadataKey> keys;
    int ret = rsInterfaces->GetScreenSupportedMetaDataKeys(screenId, keys);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_EQ(keys[0], ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X); // ScreenHDRMetadataKey now is mock data.
}

/*
* Function: GetScreenSupportedMetaDataKeys
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedMetaDataKeys with INVALID_SCREEN_ID
*                  2. check ret
* @tc.require: IssueI5KGK4
*/
HWTEST_F(RSInterfacesTest, GetScreenSupportedMetaDataKeys002, Function | SmallTest | Level2)
{
    std::vector<ScreenHDRMetadataKey> keys;
    int ret = rsInterfaces->GetScreenSupportedMetaDataKeys(INVALID_SCREEN_ID, keys);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenColorGamut
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenColorGamut with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenColorGamut002, Function | SmallTest | Level2)
{
    ScreenColorGamut mode = ScreenColorGamut::COLOR_GAMUT_INVALID;
    int ret = rsInterfaces->GetScreenColorGamut(INVALID_SCREEN_ID, mode);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetScreenColorGamut
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenColorGamut with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenColorGamut002, Function | SmallTest | Level2)
{
    int ret = rsInterfaces->SetScreenColorGamut(INVALID_SCREEN_ID, 0);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetScreenGamutMap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenGamutMap with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenGamutMap002, Function | SmallTest | Level2)
{
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    int ret = rsInterfaces->SetScreenGamutMap(INVALID_SCREEN_ID, gamutMap);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenGamutMap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenGamutMap with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenGamutMap002, Function | SmallTest | Level2)
{
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    int ret = rsInterfaces->GetScreenGamutMap(INVALID_SCREEN_ID, gamutMap);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenHDRCapability
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenHDRCapability
*                  2. check ret
* @tc.require: IssueI5KGK4
*/
HWTEST_F(RSInterfacesTest, GetScreenHDRCapability001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    RSScreenHDRCapability hdrCapability;
    int ret = rsInterfaces->GetScreenHDRCapability(screenId, hdrCapability);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    EXPECT_EQ(hdrCapability.GetMaxLum(), 1000); // maxLum now is mock data
}

/*
* Function: GetScreenHDRCapability
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenHDRCapability with INVALID_SCREEN_ID
*                  2. check ret
* @tc.require: IssueI5KGK4
*/
HWTEST_F(RSInterfacesTest, GetScreenHDRCapability002, Function | SmallTest | Level2)
{
    RSScreenHDRCapability hdrCapability;
    int ret = rsInterfaces->GetScreenHDRCapability(INVALID_SCREEN_ID, hdrCapability);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenType
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenType
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenType001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    RSScreenType type;
    int ret = rsInterfaces->GetScreenType(screenId, type);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: GetScreenType
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenType with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenType002, Function | SmallTest | Level2)
{
    RSScreenType type;
    int ret = rsInterfaces->GetScreenType(INVALID_SCREEN_ID, type);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetVirtualScreenResolution/GetVirtualScreenResolution
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Call CreateVirtualScreen, use normal parameters.
*                  2. Use SetVirtualScreenResolution to change the width and height of virtualScreen
*                  3. Use GetVirtualScreenResolution to get current width and height of virtualScreen
*                  4. Check current width and height of virtualScreen
*/
HWTEST_F(RSInterfacesTest, SetVirtualScreenResolution001, Function | SmallTest | Level2)
{
    auto csurface = Surface::CreateSurfaceAsConsumer();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual5", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    auto curVirtualScreenResolution = rsInterfaces->GetVirtualScreenResolution(virtualScreenId);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenWidth(), defaultWidth);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenHeight(), defaultHeight);

    rsInterfaces->SetVirtualScreenResolution(virtualScreenId, newWidth, newHeight);

    curVirtualScreenResolution = rsInterfaces->GetVirtualScreenResolution(virtualScreenId);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenWidth(), newWidth);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenHeight(), newHeight);

    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/*
* Function: GetAllScreenIds
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetAllScreenIds
*                  2. check vector size
*/
HWTEST_F(RSInterfacesTest, GetAllScreenIds, Function | SmallTest | Level2)
{
    std::vector<ScreenId> ids = rsInterfaces->GetAllScreenIds();
    int32_t size = ids.size();
    EXPECT_GT(ids.size(), 0);
    auto csurface = Surface::CreateSurfaceAsConsumer();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual6", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    ids = rsInterfaces->GetAllScreenIds();
    EXPECT_EQ(size + 1, ids.size());
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenSkipFrameInterval with invalid parameters and check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenSkipFrameInterval001, Function | SmallTest | Level2)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t skipFrameInterval = 1;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenSkipFrameInterval with invalid parameters and check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenSkipFrameInterval002, Function | SmallTest | Level2)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 0;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenSkipFrameInterval with invalid parameters and check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenSkipFrameInterval003, Function | SmallTest | Level2)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 100;  // for test
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenSkipFrameInterval with valid parameters and check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenSkipFrameInterval004, Function | SmallTest | Level1)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    float skipFrameInterval = 2.1;  // for test
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenSkipFrameInterval with valid parameters and check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenSkipFrameInterval005, Function | SmallTest | Level1)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 1;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: ScreenGamutMap_001
 * @tc.desc: Test SetScreenGamutMap And GetScreenGamutMap
 * @tc.type: FUNC
 * @tc.require: issueI60RFZ
 */
HWTEST_F(RSInterfacesTest, ScreenGamutMap_001, Function | SmallTest | Level2)
{
    ScreenId defaultScreenId = rsInterfaces->GetDefaultScreenId();
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    uint32_t statusMode = rsInterfaces->SetScreenGamutMap(defaultScreenId, mode);
    ASSERT_EQ(statusMode, StatusCode::HDI_ERROR);
    statusMode = rsInterfaces->GetScreenGamutMap(defaultScreenId, mode);
    ASSERT_EQ(statusMode, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: RegisterOcclusionChangeCallback Test
 * @tc.desc: RegisterOcclusionChangeCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, RegisterOcclusionChangeCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    OcclusionChangeCallback cb = [](std::shared_ptr<RSOcclusionData> data){};
    int32_t ret = rsInterfaces->RegisterOcclusionChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: UnRegisterOcclusionChangeCallback Test
 * @tc.desc: UnRegisterOcclusionChangeCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, UnRegisterOcclusionChangeCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    OcclusionChangeCallback cb = [](std::shared_ptr<RSOcclusionData> data){};
    int32_t ret = rsInterfaces->UnRegisterOcclusionChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: SetRenderModeChangeCallback Test
 * @tc.desc: SetRenderModeChangeCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, SetRenderModeChangeCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    RenderModeChangeCallback cb = [](bool flag){};
    int32_t ret = rsInterfaces->SetRenderModeChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: UpdateRenderMode True Test
 * @tc.desc: UpdateRenderMode True Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, UpdateRenderMode_True, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->UpdateRenderMode(true);
}

/*
 * @tc.name: UpdateRenderMode False Test
 * @tc.desc: UpdateRenderMode False Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, UpdateRenderMode_False, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->UpdateRenderMode(false);
}

/*
 * @tc.name: SetVirtualScreenSurface Test a notfound id
 * @tc.desc: SetVirtualScreenSurface Test a notfound id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSurface_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    int32_t ret = rsInterfaces->SetVirtualScreenSurface(123, psurface);
    ASSERT_EQ(ret, 0);
}
} // namespace Rosen
} // namespace OHOS
