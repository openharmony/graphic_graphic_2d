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
    ASSERT_NE(defaultScreenId, INVALID_SCREEN_ID);
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
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(csurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual0", 320, 180, psurface, INVALID_SCREEN_ID, -1);
    ASSERT_NE(virtualScreenId, INVALID_SCREEN_ID);

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
    ASSERT_EQ(virtualScreenId, INVALID_SCREEN_ID);
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
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(csurface, nullptr);

    ScreenId virtualScreenId1 = rsInterfaces->CreateVirtualScreen(
        "virtual1", 320, 180, psurface, INVALID_SCREEN_ID, -1);
    ASSERT_NE(virtualScreenId1, INVALID_SCREEN_ID);

    ScreenId virtualScreenId2 = rsInterfaces->CreateVirtualScreen(
        "virtual2", 320, 180, psurface, INVALID_SCREEN_ID, -1);
    ASSERT_EQ(virtualScreenId2, INVALID_SCREEN_ID);
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
    EXPECT_NE(modeInfo.GetScreenFreshRate(), 0);
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
* CaseDescription: 1. call SetScreenPowerStatus with value of POWER_STATUS_STANDBY
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenPowerStatus001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_STANDBY);
    usleep(50000); // wait 50000us to ensure SetScreenPowerStatus done.
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(screenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::POWER_STATUS_STANDBY);
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
* CaseDescription: 1. call GetScreenPowerStatus when set POWER_STATUS_STANDBY
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenPowerStatus001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_STANDBY);
    usleep(50000); // wait 50000us to ensure SetScreenPowerStatus done.
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(screenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::POWER_STATUS_STANDBY);
}

/*
* Function: GetScreenPowerStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenPowerStatus when set POWER_STATUS_OFF
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenPowerStatus002, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    usleep(50000); // wait 50000us to ensure SetScreenPowerStatus done.
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(screenId);
    EXPECT_EQ(powerStatus, ScreenPowerStatus::POWER_STATUS_OFF);
}

/*
* Function: GetScreenPowerStatus
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenPowerStatus when set POWER_STATUS_ON
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenPowerStatus003, Function | SmallTest | Level2)
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
HWTEST_F(RSInterfacesTest, GetScreenPowerStatus004, Function | SmallTest | Level2)
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
    std::string emptyName;
    EXPECT_GT(screenCapability.GetPhyWidth(), 0);
    EXPECT_GT(screenCapability.GetPhyHeight(), 0);
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
    std::string emptyName;
    EXPECT_GT(screenCapability.GetPhyWidth(), 0);
    EXPECT_GT(screenCapability.GetPhyHeight(), 0);
    EXPECT_NE(screenCapability.GetType(), DISP_INVALID);

    auto modeInfo = screenData.GetActivityModeInfo();
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
    EXPECT_NE(modeInfo.GetScreenFreshRate(), 0);
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
* CaseDescription: 1. call SetScreenBacklight with value:0
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenBacklight001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 0);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 0);
}

/*
* Function: SetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenBacklight with value:50
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenBacklight002, Function | SmallTest | Level2)
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
HWTEST_F(RSInterfacesTest, SetScreenBacklight003, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 100);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 100);
}

/*
* Function: SetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenBacklight with value:200
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenBacklight004, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 200);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 100);
}

/*
* Function: SetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenBacklight with value:UINT32_MAX
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenBacklight005, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, UINT32_MAX);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 100);
}

/*
* Function: GetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenBacklight with value: 0
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenBacklight001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 0);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 0);
}

/*
* Function: GetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenBacklight with value: 50
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenBacklight002, Function | SmallTest | Level2)
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
* CaseDescription: 1. call GetScreenBacklight with value: 100
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenBacklight003, Function | SmallTest | Level2)
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
* CaseDescription: 1. call GetScreenBacklight with value: 200
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenBacklight004, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, 200);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 100);
}

/*
* Function: GetScreenBacklight
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenBacklight with value: UINT32_MAX
*                  2. check
*/
HWTEST_F(RSInterfacesTest, GetScreenBacklight005, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    rsInterfaces->SetScreenBacklight(screenId, UINT32_MAX);
    auto backLight = rsInterfaces->GetScreenBacklight(screenId);
    EXPECT_EQ(backLight, 100);
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
    rsInterfaces->SetScreenChangeCallback(callback);
    sleep(2); // wait 2s to check if the callback returned.
    if (callbacked) {
        EXPECT_NE(screenId, INVALID_SCREEN_ID);
        EXPECT_NE(screenEvent, ScreenEvent::UNKNOWN);
    } else {
        EXPECT_EQ(screenId, INVALID_SCREEN_ID);
        EXPECT_EQ(screenEvent, ScreenEvent::UNKNOWN);
    }
}
} // namespace Rosen
} // namespace OHOS
