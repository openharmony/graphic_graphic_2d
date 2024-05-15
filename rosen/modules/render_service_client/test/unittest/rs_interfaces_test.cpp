/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "ibuffer_consumer_listener.h"
#include "surface_utils.h"
#include "transaction/rs_interfaces.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class BufferConsumerTestListener : public ::OHOS::IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};
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

    uint32_t GenerateVirtualScreenMirrorId()
    {
        virtualScreenMirrorId++;
        return virtualScreenMirrorId;
    }

private:
    static constexpr uint32_t SET_REFRESHRATE_SLEEP_US = 50000;  // wait for refreshrate change
    uint32_t virtualScreenMirrorId = 1;
};

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
    auto csurface = IConsumerSurface::Create();
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
* Function: SetVirtualScreenPixelFormat/GetVirtualScreenPixelFormat
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. Call CreateVirtualScreen, use normal parameters.
*                  2. Use SetVirtualScreenPixelFormat to set pixelFormat of virtualScreen
*                  3. Use GetVirtualScreenPixelFormat to get current pixelFormat of virtualScreen
*                  4. Check current pixelFormat of virtualScreen
*/
HWTEST_F(RSInterfacesTest, SetVirtualScreenPixelFormat001, Function | SmallTest | Level2)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual5", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    GraphicPixelFormat curPixelFormat;
    int32_t ret = rsInterfaces->GetPixelFormat(virtualScreenId, curPixelFormat);
    EXPECT_NE(ret, StatusCode::SCREEN_NOT_FOUND);
    EXPECT_EQ(curPixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);

    rsInterfaces->SetPixelFormat(virtualScreenId, pixelFormat);

    ret = rsInterfaces->GetPixelFormat(virtualScreenId, curPixelFormat);
    EXPECT_NE(ret, StatusCode::SCREEN_NOT_FOUND);
    EXPECT_EQ(curPixelFormat, GRAPHIC_PIXEL_FMT_BGRA_8888);

    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: SetVirtualMirrorScreenCanvasRotation
 * @tc.desc: 1. Call CreateVirtualScreen, use normal parameters.
 *           2. Use SetVirtualMirrorScreenCanvasRotation to change the width and height of virtualScreen
 *           3. Use GetVirtualScreenResolution to get current width and height of virtualScreen
 *           4. Check current width and height of virtualScreen
 * @tc.type: FUNC
 * @tc.require: issueI8FPRE
 */
HWTEST_F(RSInterfacesTest, SetVirtualMirrorScreenCanvasRotation001, Function | SmallTest | Level2)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 1344;
    uint32_t defaultHeight = 2772;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual5", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_EQ(rsInterfaces->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, true), true);
    EXPECT_EQ(rsInterfaces->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, false), true);
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
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual6", defaultWidth, defaultHeight, psurface, GenerateVirtualScreenMirrorId(), -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    std::vector<ScreenId> ids = rsInterfaces->GetAllScreenIds();
    EXPECT_TRUE(find(ids.begin(), ids.end(), virtualScreenId) != ids.end());
}

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
    auto csurface = IConsumerSurface::Create();
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
    auto csurface = IConsumerSurface::Create();
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
    usleep(SET_REFRESHRATE_SLEEP_US); // wait 50000us to ensure SetScreenPowerStatus done.
    auto powerStatus = rsInterfaces->GetScreenPowerStatus(virtualScreenId);
    EXPECT_NE(powerStatus, ScreenPowerStatus::POWER_STATUS_ON);

    auto screenCapability = rsInterfaces->GetScreenCapability(virtualScreenId);
    EXPECT_EQ(screenCapability.GetPhyWidth(), 0);
    EXPECT_EQ(screenCapability.GetName(), "virtual0");

    auto backLight = rsInterfaces->GetScreenBacklight(virtualScreenId);
    EXPECT_EQ(backLight, -1);
    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/*
* Function: CreateVirtualScreen
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: call CreateVirtualScreen with filteredAppVector
*/
HWTEST_F(RSInterfacesTest, CreateVirtualScreen005, Function | SmallTest | Level2)
{
    std::vector<NodeId> filteredAppVector = {};
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual11", 320, 180, nullptr, INVALID_SCREEN_ID, -1, filteredAppVector);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
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
    auto formerModeInfo = rsInterfaces->GetScreenActiveMode(screenId);

    rsInterfaces->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);

    //restore the former mode
    rsInterfaces->SetScreenActiveMode(screenId, formerModeInfo.GetScreenModeId());
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
    auto formerModeInfo = rsInterfaces->GetScreenActiveMode(screenId);

    auto supportedScreenModes = rsInterfaces->GetScreenSupportedModes(screenId);
    EXPECT_GT(supportedScreenModes.size(), 0);

    rsInterfaces->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);

    //restore the former mode
    rsInterfaces->SetScreenActiveMode(screenId, formerModeInfo.GetScreenModeId());
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
    auto formerModeInfo = rsInterfaces->GetScreenActiveMode(screenId);

    rsInterfaces->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
    EXPECT_NE(modeInfo.GetScreenRefreshRate(), 0);
    EXPECT_NE(modeInfo.GetScreenHeight(), -1);
    EXPECT_NE(modeInfo.GetScreenWidth(), -1);

    //restore the former mode
    rsInterfaces->SetScreenActiveMode(screenId, formerModeInfo.GetScreenModeId());
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
    usleep(SET_REFRESHRATE_SLEEP_US); // wait 50000us to ensure SetScreenPowerStatus done.
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
    EXPECT_NE(powerStatus, ScreenPowerStatus::POWER_STATUS_ON);
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
    usleep(SET_REFRESHRATE_SLEEP_US); // wait to check if the callback returned.
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
* Function: GetScreenSupportedHDRFormats
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedHDRFormats with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenSupportedHDRFormats002, Function | SmallTest | Level2)
{
    std::vector<ScreenHDRFormat> hdrFormats;
    int ret = rsInterfaces->GetScreenSupportedHDRFormats(INVALID_SCREEN_ID, hdrFormats);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenHDRFormat
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenHDRFormat with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenHDRFormat002, Function | SmallTest | Level2)
{
    ScreenHDRFormat hdrFormat = ScreenHDRFormat::NOT_SUPPORT_HDR;
    int ret = rsInterfaces->GetScreenHDRFormat(INVALID_SCREEN_ID, hdrFormat);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetScreenHDRFormat
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenHDRFormat with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenHDRFormat002, Function | SmallTest | Level2)
{
    int ret = rsInterfaces->SetScreenHDRFormat(INVALID_SCREEN_ID, 0);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenSupportedColorSpaces
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenSupportedColorSpaces with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenSupportedColorSpaces002, Function | SmallTest | Level2)
{
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    int ret = rsInterfaces->GetScreenSupportedColorSpaces(INVALID_SCREEN_ID, colorSpaces);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: GetScreenColorSpace
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenColorSpace with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, GetScreenColorSpace002, Function | SmallTest | Level2)
{
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    int ret = rsInterfaces->GetScreenColorSpace(INVALID_SCREEN_ID, colorSpace);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetScreenColorSpace
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenColorSpace with INVALID_SCREEN_ID
*                  2. check ret
*/
HWTEST_F(RSInterfacesTest, SetScreenColorSpace002, Function | SmallTest | Level2)
{
    int ret = rsInterfaces->SetScreenColorSpace(
        INVALID_SCREEN_ID, GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE);
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
 * @tc.name: SetVirtualScreenSurface Test a notfound id
 * @tc.desc: SetVirtualScreenSurface Test a notfound id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSurface_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    int32_t ret = rsInterfaces->SetVirtualScreenSurface(123, psurface);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: GetScreenCurrentRefreshRate001
 * @tc.desc: Verify the function of getting the current refreshrate via rs interfaces
 * @tc.type: FUNC
 * @tc.require: I7EM2R
 */
HWTEST_F(RSInterfacesTest, GetScreenCurrentRefreshRate001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t formerRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);

    FrameRateLinkerId id = 0;
    FrameRateRange range;
    rsInterfaces->SyncFrameRateRange(id, range, false);
    auto modeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    rsInterfaces->SetScreenRefreshRate(screenId, 0, modeInfo.GetScreenRefreshRate());
    uint32_t currentRate = rsInterfaces-> GetScreenCurrentRefreshRate(screenId);
    EXPECT_EQ(modeInfo.GetScreenRefreshRate(), currentRate);
    //restore the former rate
    rsInterfaces->SetScreenRefreshRate(screenId, 0, formerRate);
}

/*
 * @tc.name: SetScreenRefreshRate001
 * @tc.desc: Verify the function of setting the refreshrate with 30hz
 * @tc.type: FUNC
 * @tc.require: I7EM2R
 */
HWTEST_F(RSInterfacesTest, SetScreenRefreshRate001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t formerRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
    uint32_t rateToSet = 30;

    rsInterfaces->SetScreenRefreshRate(screenId, 0, rateToSet);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
    bool ifSupported = false;

    if (currentRate == rateToSet) {
        ifSupported = true;
    } else {
        auto supportedRates = rsInterfaces->GetScreenSupportedRefreshRates(screenId);
        for (auto rateIter : supportedRates) {
            if (rateIter < 0) {
                continue;
            }
            if (static_cast<uint32_t>(rateIter) == rateToSet) {
                ifSupported = true;
                currentRate = rateToSet;
                break;
            }
        }
    }
    
    if (ifSupported) {
        EXPECT_GE(currentRate, rateToSet);
    } else {
        EXPECT_NE(currentRate, rateToSet);
    }

    //restore the former rate
    rsInterfaces->SetScreenRefreshRate(screenId, 0, formerRate);
}

/*
 * @tc.name: SetScreenRefreshRate002
 * @tc.desc: Verify the function of setting the refreshrate with a very high value
 * @tc.type: FUNC
 * @tc.require: I7EM2R
 */
HWTEST_F(RSInterfacesTest, SetScreenRefreshRate002, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t formerRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
    uint32_t rateToSet = 990;

    rsInterfaces->SetScreenRefreshRate(screenId, 0, rateToSet);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
    EXPECT_NE(currentRate, rateToSet);

    //restore the former rate
    rsInterfaces->SetScreenRefreshRate(screenId, 0, formerRate);
}

/*
 * @tc.name: SetScreenRefreshRate003
 * @tc.desc: Verify the function of setting the refreshrate with a normal value of 60hz
 * @tc.type: FUNC
 * @tc.require: I7EM2R
 */
HWTEST_F(RSInterfacesTest, SetScreenRefreshRate003, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t formerRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
    uint32_t rateToSet = 60;
    uint32_t standardRate = 60;

    rsInterfaces->SetScreenRefreshRate(screenId, 0, rateToSet);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
    auto supportedRates = rsInterfaces->GetScreenSupportedRefreshRates(screenId);

    bool ifSupported = false;
    for (auto rateIter : supportedRates) {
        if (rateIter < 0) {
            continue;
        }
        if (static_cast<uint32_t>(rateIter) == rateToSet) {
            ifSupported = true;
        }
    }
    if (ifSupported) {
        EXPECT_GE(currentRate, rateToSet);
    } else {
        EXPECT_GE(currentRate, standardRate);
    }

    //restore the former rate
    rsInterfaces->SetScreenRefreshRate(screenId, 0, formerRate);
}

/*
 * @tc.name: SetRefreshRateMode001
 * @tc.desc: Verify the function of setting the refreshrate mode
 * @tc.type: FUNC
 * @tc.require: I7EM2R
 */
HWTEST_F(RSInterfacesTest, SetRefreshRateMode001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    uint32_t formerRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
    uint32_t newRate = 0;
    uint32_t standardRate = 60;

    //find a supported rate which not equal to formerRate
    auto supportedRates = rsInterfaces->GetScreenSupportedRefreshRates(screenId);
    for (auto rateIter : supportedRates) {
        if (rateIter < 0) {
            continue;
        }
        if (static_cast<uint32_t>(rateIter) != formerRate) {
            newRate = static_cast<uint32_t>(rateIter);
            break;
        }
    }

    if (newRate != 0) {
        rsInterfaces->SetScreenRefreshRate(screenId, 0, newRate);
        usleep(SET_REFRESHRATE_SLEEP_US);
        uint32_t currentRate = rsInterfaces->GetScreenCurrentRefreshRate(screenId);
        EXPECT_GE(currentRate, standardRate);

        //restore the former rate
        rsInterfaces->SetScreenRefreshRate(screenId, 0, formerRate);
    }
}

/*
 * @tc.name: RegisterHgmRefreshRateUpdateCallback Test
 * @tc.desc: RegisterHgmRefreshRateUpdateCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, RegisterHgmRefreshRateUpdateCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    HgmRefreshRateUpdateCallback cb = [](int32_t refreshRate){};
    int32_t ret = rsInterfaces->RegisterHgmRefreshRateUpdateCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: UnRegisterHgmRefreshRateUpdateCallback Test
 * @tc.desc: UnRegisterHgmRefreshRateUpdateCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, UnRegisterHgmRefreshRateUpdateCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    int32_t ret = rsInterfaces->UnRegisterHgmRefreshRateUpdateCallback();
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: RegisterHgmConfigChangeCallback Test
 * @tc.desc: RegisterHgmConfigChangeCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, RegisterHgmConfigChangeCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    HgmConfigChangeCallback cb = [](std::shared_ptr<RSHgmConfigData> data){};
    int32_t ret = rsInterfaces->RegisterHgmConfigChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: NotifyLightFactorStatus001
 * @tc.desc: Notify light factor status to hgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyLightFactorStatus001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    bool isSafe = false;
    rsInterfaces->NotifyLightFactorStatus(isSafe);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: NotifyPackageEvent001
 * @tc.desc: Notify current package list to hgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyPackageEvent001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::vector<std::string> packageList;
    packageList.push_back("NotifyPackageEvent001");
    packageList.push_back("NotifyPackageEvent002");
    uint32_t listSize = packageList.size();
    rsInterfaces->NotifyPackageEvent(listSize, packageList);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: NotifyRefreshRateEvent001
 * @tc.desc: Notify refreshRate event to hgm to modify screen refreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyRefreshRateEvent001, Function | SmallTest | Level2)
{
    constexpr int32_t maxFps = 1000;
    constexpr int32_t minFps = 1;
    ASSERT_NE(rsInterfaces, nullptr);
    EventInfo addVote = { "VOTER_VIDEO", true, minFps, maxFps };
    EventInfo delVote = { "VOTER_VIDEO", false};
    rsInterfaces->NotifyRefreshRateEvent(addVote);
    rsInterfaces->NotifyRefreshRateEvent(delVote);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: NotifyTouchEvent001
 * @tc.desc: Notify touch event to hgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyTouchEvent001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    int32_t touchStatus = 0;
    int32_t touchCnt = 0;
    rsInterfaces->NotifyTouchEvent(touchStatus, touchCnt);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: RegisterHgmRefreshRateModeChangeCallback Test
 * @tc.desc: RegisterHgmRefreshRateModeChangeCallback Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, RegisterHgmRefreshRateModeChangeCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    HgmRefreshRateModeChangeCallback cb = [](int32_t refreshRateMode){};
    int32_t ret = rsInterfaces->RegisterHgmRefreshRateModeChangeCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: RegisterHgmRefreshRateModeChangeCallback001
 * @tc.desc: RegisterHgmRefreshRateModeChangeCallback001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, RegisterHgmRefreshRateModeChangeCallback001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    int32_t savedModeName = -2;
    HgmRefreshRateModeChangeCallback cb = [&](int32_t refreshRateModeName){ savedModeName = refreshRateModeName; };
    int32_t ret = rsInterfaces->RegisterHgmRefreshRateModeChangeCallback(cb);
    ASSERT_EQ(ret, 0);

    rsInterfaces->SetRefreshRateMode(-1);
    ASSERT_EQ(savedModeName, -1);

    for (int32_t mode : {1, 2, 3}) {
        rsInterfaces->SetRefreshRateMode(mode);
        ASSERT_NE(savedModeName, mode);
    }
}

/*
 * @tc.name: RegisterSurfaceOcclusionChangeCallback001
 * @tc.desc: RegisterOcclusionChangeCallback interface test.
 * @tc.type: FUNC
 * @tc.require: issueI851VR
 */
HWTEST_F(RSInterfacesTest, RegisterSurfaceOcclusionChangeCallback001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    NodeId id = 0;
    SurfaceOcclusionChangeCallback cb = [](float) {};
    std::vector<float> partitionPoints;
    int32_t ret = rsInterfaces->RegisterSurfaceOcclusionChangeCallback(id, cb, partitionPoints);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: UnRegisterSurfaceOcclusionChangeCallback001
 * @tc.desc: UnRegisterSurfaceOcclusionChangeCallback interface test.
 * @tc.type: FUNC
 * @tc.require: issueI851VR
 */
HWTEST_F(RSInterfacesTest, UnRegisterSurfaceOcclusionChangeCallback001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    NodeId id = 0;
    int32_t ret = rsInterfaces->UnRegisterSurfaceOcclusionChangeCallback(id);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: ResizeVirtualScreen001
 * @tc.desc: ResizeVirtualScreen interface test.
 * @tc.type: FUNC
 * @tc.require: issueI8F2HB
 */
HWTEST_F(RSInterfacesTest, ResizeVirtualScreen001, Function | SmallTest | Level2)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen0", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t ret = rsInterfaces->ResizeVirtualScreen(virtualScreenId, newWidth, newHeight);
    ASSERT_EQ(ret, 0);

    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: SetCurtainScreenUsingStatus001
 * @tc.desc: Test SetCurtainScreenUsingStatus interface while input is true.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetCurtainScreenUsingStatus001, Function | SmallTest | Level2)
{
    rsInterfaces->SetCurtainScreenUsingStatus(true);
}

/*
 * @tc.name: SetCurtainScreenUsingStatus001
 * @tc.desc: Test SetCurtainScreenUsingStatus interface while input is false.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetCurtainScreenUsingStatus002, Function | SmallTest | Level2)
{
    rsInterfaces->SetCurtainScreenUsingStatus(false);
}

/*
 * @tc.name: SetVirtualScreenUsingStatus001
 * @tc.desc: Test SetVirtualScreenUsingStatus interface while input is true.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenUsingStatus001, Function | SmallTest | Level2)
{
    rsInterfaces->SetVirtualScreenUsingStatus(true);
}

/*
 * @tc.name: SetVirtualScreenUsingStatus002
 * @tc.desc: Test SetVirtualScreenUsingStatus interface while input is false.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenUsingStatus002, Function | SmallTest | Level2)
{
    rsInterfaces->SetVirtualScreenUsingStatus(false);
}

/*
 * @tc.name: GetCurrentRefreshRateMode
 * @tc.desc: Test GetCurrentRefreshRateMode.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, GetCurrentRefreshRateMode, Function | SmallTest | Level2)
{
    EXPECT_TRUE(rsInterfaces->GetCurrentRefreshRateMode() >= -1);
}

/*
 * @tc.name: CreatePixelMapFromSurfaceId
 * @tc.desc: Test CreatePixelMapFromSurfaceId.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, CreatePixelMapFromSurfaceId001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    sptr<IBufferConsumerListener> listener = new BufferConsumerTestListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<IBufferProducer> producer = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);

    int releaseFence = -1;
    sptr<SurfaceBuffer> buffer;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig = {
        .damage = {
            .w = 0x100,
            .h = 0x100,
        }
    };
    GSError ret = pSurface->RequestBuffer(buffer, releaseFence, requestConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);
    ASSERT_NE(buffer, nullptr);
    ret = pSurface->FlushBuffer(buffer, releaseFence, flushConfig);
    ASSERT_EQ(ret, OHOS::GSERROR_OK);

    OHOS::Rect rect = {
        .x = 0,
        .y = 0,
        .w = 0x100,
        .h = 0x100,
    };
    uint64_t surfaceId = static_cast<uint64_t>(cSurface->GetUniqueId());
    auto utils = SurfaceUtils::GetInstance();
    utils->Add(surfaceId, cSurface);
    auto pixcelMap = rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId, rect);
#if defined(RS_ENABLE_UNI_RENDER)
    ASSERT_NE(pixcelMap, nullptr);
#endif
}

/*
 * @tc.name: GetActiveScreenId
 * @tc.desc: Test GetActiveScreenId
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, GetActiveScreenId, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetActiveScreenId();
    ASSERT_EQ(screenId, INVALID_SCREEN_ID);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode
 * @tc.desc: Test SetVirtualMirrorScreenScaleMode
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetVirtualMirrorScreenScaleMode, Function | SmallTest | Level2)
{
    ASSERT_FALSE(rsInterfaces->SetVirtualMirrorScreenScaleMode(INVALID_SCREEN_ID, ScreenScaleMode::INVALID_MODE));
}

/*
 * @tc.name: SetScreenCorrection
 * @tc.desc: Test SetScreenCorrection
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetScreenCorrection, Function | SmallTest | Level2)
{
    int32_t ret = rsInterfaces->SetScreenCorrection(INVALID_SCREEN_ID, ScreenRotation::INVALID_SCREEN_ROTATION);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetSystemAnimatedScenes
 * @tc.desc: Test SetSystemAnimatedScenes
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetSystemAnimatedScenes, Function | SmallTest | Level2)
{
    ASSERT_TRUE(rsInterfaces->SetSystemAnimatedScenes(SystemAnimatedScenes::OTHERS));
}
} // namespace Rosen
} // namespace OHOS
