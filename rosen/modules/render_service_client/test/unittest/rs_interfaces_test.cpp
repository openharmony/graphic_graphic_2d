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

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

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
        uint64_t tokenId;
        const char* perms[1];
        perms[0] = "ohos.permission.CAPTURE_SCREEN";
        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = 1,
            .aclsNum = 0,
            .dcaps = NULL,
            .perms = perms,
            .acls = NULL,
            .processName = "foundation",
            .aplStr = "system_basic",
        };
        tokenId = GetAccessTokenId(&infoInstance);
        SetSelfTokenID(tokenId);
        OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
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
 * @tc.name: SetPhysicalScreenResolution001
 * @tc.desc: Test SetPhysicalScreenResolution
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfacesTest, SetPhysicalScreenResolution001, Function | SmallTest | Level2)
{
    ScreenId id = INVALID_SCREEN_ID;
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    auto ret = rsInterfaces->SetPhysicalScreenResolution(id, newWidth, newHeight);
    EXPECT_EQ(ret, StatusCode::RS_CONNECTION_ERROR);
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
* CaseDescription: call CreateVirtualScreen with whiteList
*/
HWTEST_F(RSInterfacesTest, CreateVirtualScreen005, Function | SmallTest | Level2)
{
    std::vector<NodeId> whiteList = {};
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtual11", 320, 180, nullptr, INVALID_SCREEN_ID, -1, whiteList);
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

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
/**
 * @tc.name: SetPointerColorInversionConfig001
 * @tc.desc: set pointer color inversion config function.
 * @tc.type: FUNC
 * @tc.require: Issue #IA8DQ6
 */
HWTEST_F(RSInterfacesTest, SetPointerColorInversionConfig001, TestSize.Level1)
{
    float darkBuffer = 0.5f;
    float brightBuffer = 0.5f;
    int64_t interval = 50;
    int32_t rangeSize = 20;
    int32_t ret = rsInterfaces->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/**
 * @tc.name: SetPointerColorInversionEnabled001
 * @tc.desc: set pointer color inversion enabled function.
 * @tc.type: FUNC
 * @tc.require: Issue #IA8DQ6
 */
HWTEST_F(RSInterfacesTest, SetPointerColorInversionEnabled001, TestSize.Level1)
{
    int32_t ret = rsInterfaces->SetPointerColorInversionEnabled(false);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/**
 * @tc.name: RegisterPointerLuminanceChangeCallback001
 * @tc.desc: register pointer luminance change callback function.
 * @tc.type: FUNC
 * @tc.require: Issue #IA8DQ6
 */
HWTEST_F(RSInterfacesTest, RegisterPointerLuminanceChangeCallback001, TestSize.Level1)
{
    int32_t ret = rsInterfaces->RegisterPointerLuminanceChangeCallback([](int32_t brightness) -> void {});
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/**
 * @tc.name: UnRegisterPointerLuminanceChangeCallback001
 * @tc.desc: unregister pointer luminance change callback function.
 * @tc.type: FUNC
 * @tc.require: Issue #IA8DQ6
 */
HWTEST_F(RSInterfacesTest, UnRegisterPointerLuminanceChangeCallback001, TestSize.Level1)
{
    int32_t ret = rsInterfaces->UnRegisterPointerLuminanceChangeCallback();
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}
#endif

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
* Function: SetScreenActiveRect
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenActiveRect
*                  2. check
*/
HWTEST_F(RSInterfacesTest, SetScreenActiveRect001, Function | SmallTest | Level2)
{
    auto screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    Rect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(rsInterfaces->SetScreenActiveRect(screenId, activeRect), StatusCode::SUCCESS);
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
    ScreenChangeReason errorReason = ScreenChangeReason::DEFAULT;
    bool callbacked = false;
    auto callback = [&screenId, &screenEvent, &errorReason, &callbacked]
        (ScreenId id, ScreenEvent event, ScreenChangeReason reason) {
        screenId = id;
        screenEvent = event;
        errorReason = reason;
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
    rsInterfaces->SyncFrameRateRange(id, range, 0);
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
 * @tc.name: SetShowRefreshRateEnabled
 * @tc.desc: Verify the function of SetShowRefreshRateEnabled
 * @tc.type: FUNC
 * @tc.require: I7EM2R
 */
HWTEST_F(RSInterfacesTest, SetShowRefreshRateEnabled, Function | SmallTest | Level2)
{
    bool enabled = true;
    int32_t type = 0;
    rsInterfaces->SetShowRefreshRateEnabled(enabled, type);
    EXPECT_GE(rsInterfaces->GetShowRefreshRateEnabled(), 0);
}

/*
 * @tc.name: GetRealtimeRefreshRate
 * @tc.desc: Verify the function of GetRealtimeRefreshRate
 * @tc.type: FUNC
 * @tc.require: I7EM2R
 */
HWTEST_F(RSInterfacesTest, GetRealtimeRefreshRate, Function | SmallTest | Level2)
{
    ScreenId id = 0;
    EXPECT_GE(rsInterfaces->GetRealtimeRefreshRate(id), 0);
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
 * @tc.name: RegisterFirstFrameCommitCallback Test
 * @tc.desc: RegisterFirstFrameCommitCallback Test
 * @tc.type: FUNC
 * @tc.require: issueIBTF2E
 */
HWTEST_F(RSInterfacesTest, RegisterFirstFrameCommitCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    FirstFrameCommitCallback cb = [](uint64_t screenId, int64_t timestamp){};
    int32_t ret = rsInterfaces->RegisterFirstFrameCommitCallback(cb);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: UnRegisterFirstFrameCommitCallback Test
 * @tc.desc: UnRegisterFirstFrameCommitCallback Test
 * @tc.type: FUNC
 * @tc.require: issueIBTF2E
 */
HWTEST_F(RSInterfacesTest, UnRegisterFirstFrameCommitCallback_Test, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    int32_t ret = rsInterfaces->UnRegisterFirstFrameCommitCallback();
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
    int32_t lightFactorStatus = 0;
    rsInterfaces->NotifyLightFactorStatus(lightFactorStatus);
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
 * @tc.name: NotifyAppStrategyConfigChangeEvent001
 * @tc.desc: Notify current package list to hgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyAppStrategyConfigChangeEvent001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::string pkgName = "test";
    std::vector<std::pair<std::string, std::string>> newConfig;
    newConfig.push_back(std::make_pair("min", "60"));
    uint32_t listSize = newConfig.size();
    rsInterfaces->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
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
 * @tc.name: NotifySoftVsyncRateDiscountEvent001
 * @tc.desc: Notify rateDiscount event to hgm to modify rateDiscount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifySoftVsyncRateDiscountEvent001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    uint32_t pid = 1;
    std::string name = "test";
    uint32_t rateDiscount = 1;
    rsInterfaces->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: SetWindowExpectedRefreshRate001
 * @tc.desc: Set window expected soft refresh rate by windowId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, SetWindowExpectedRefreshRate001, Function | SmallTest | Level2)
{
    constexpr int32_t maxFps = 1000;
    constexpr int32_t minFps = 1;
    constexpr uint64_t windowId = 1;
    ASSERT_NE(rsInterfaces, nullptr);
    EventInfo addVote = { "VOTER_VRATE", true, minFps, maxFps };
    EventInfo delVote = { "VOTER_VRATE", false };
    std::unordered_map<uint64_t, EventInfo> addVotes;
    addVotes.insert({ windowId, addVote });
    std::unordered_map<uint64_t, EventInfo> delVotes;
    delVotes.insert({ windowId, delVote });
    rsInterfaces->SetWindowExpectedRefreshRate(addVotes);
    rsInterfaces->SetWindowExpectedRefreshRate(delVotes);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: SetWindowExpectedRefreshRate002
 * @tc.desc: Set window expected soft refresh rate by vsyncName
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, SetWindowExpectedRefreshRate002, Function | SmallTest | Level2)
{
    constexpr int32_t maxFps = 1000;
    constexpr int32_t minFps = 1;
    ASSERT_NE(rsInterfaces, nullptr);
    EventInfo addVote = { "VOTER_VRATE", true, minFps, maxFps };
    EventInfo delVote = { "VOTER_VRATE", false };
    std::unordered_map<std::string, EventInfo> addVotes;
    addVotes.insert({ "vsync1", addVote });
    std::unordered_map<std::string, EventInfo> delVotes;
    delVotes.insert({ "vsync1", delVote });
    rsInterfaces->SetWindowExpectedRefreshRate(addVotes);
    rsInterfaces->SetWindowExpectedRefreshRate(delVotes);
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
 * @tc.name: NotifyDynamicModeEvent001
 * @tc.desc: Notify touch event to hgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyDynamicModeEvent001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    bool enableDynamicMode = false;
    rsInterfaces->NotifyDynamicModeEvent(enableDynamicMode);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: NotifyHgmConfigEvent001
 * @tc.desc: Notify hgm config event to hgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyHgmConfigEvent001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::string eventName = "HGMCONFIG_HIGH_TEMP";
    bool state = false;
    rsInterfaces->NotifyHgmConfigEvent(eventName, state);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: NotifyXComponentExpectedFrameRate
 * @tc.desc: Notify hgm config event to hgm
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, NotifyXComponentExpectedFrameRate, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::string id = "xcomponent";
    int32_t expectedFrameRate = 5;
    rsInterfaces->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
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
}

/*
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest
 * @tc.desc: test RegisterFrameRateLinkerExpectedFpsUpdateCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    FrameRateLinkerExpectedFpsUpdateCallback cb = [](int32_t pid, int32_t fps){};
    int32_t ret = rsInterfaces->RegisterFrameRateLinkerExpectedFpsUpdateCallback(1, cb);
    ASSERT_EQ(ret, 0);
    ret = rsInterfaces->UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(1);
    ASSERT_EQ(ret, 0);
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
    EXPECT_EQ(ret, RS_CONNECTION_ERROR); // Unable to access IPC due to lack of permissions.
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
    EXPECT_EQ(ret, RS_CONNECTION_ERROR); // Unable to access IPC due to lack of permissions.
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
    ASSERT_TRUE(rsInterfaces != nullptr);
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
    ASSERT_TRUE(rsInterfaces != nullptr);
    rsInterfaces->SetCurtainScreenUsingStatus(false);
}

/*
 * @tc.name: DropFrameByPid001
 * @tc.desc: Test DropFrameByPid interface
 * @tc.type: FUNC
 * @tc.require: issueIB612L
 */
HWTEST_F(RSInterfacesTest, DropFrameByPid001, Function | SmallTest | Level2)
{
    ASSERT_TRUE(rsInterfaces != nullptr);
    rsInterfaces->DropFrameByPid({0});
}

/*
 * @tc.name: DropFrameByPid002
 * @tc.desc: Test DropFrameByPid interface while pidList empty
 * @tc.type: FUNC
 * @tc.require: issueIB612L
 */
HWTEST_F(RSInterfacesTest, DropFrameByPid002, Function | SmallTest | Level2)
{
    ASSERT_TRUE(rsInterfaces != nullptr);
    rsInterfaces->DropFrameByPid({});
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
    EXPECT_TRUE(rsInterfaces != nullptr);
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
    EXPECT_TRUE(rsInterfaces != nullptr);
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
    int32_t width = 0x100;
    int32_t height = 0x100;
    sptr<SurfaceBuffer> buffer;
    BufferRequestConfig requestConfig = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    BufferFlushConfig flushConfig = {
        .damage = {
            .w = width,
            .h = height,
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
    EXPECT_EQ(rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId,
        {0, 0, 0, 0}), nullptr);
    EXPECT_EQ(rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId,
        {0, 0, width + 1, height + 1}), nullptr);
    EXPECT_EQ(rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId,
        {width - 1, height - 1, 2, 2}), nullptr);
    EXPECT_NE(rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId,
        {0, 0, width, height}), nullptr);
    EXPECT_NE(rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId,
        {width - 50, height - 50, 50, 50}), nullptr);
    EXPECT_NE(rsInterfaces->CreatePixelMapFromSurfaceId(surfaceId,
        {width / 2, height/ 2, width / 2, height/ 2}), nullptr);
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
 * @tc.name: SetGlobalDarkColorMode
 * @tc.desc: Test SetGlobalDarkColorMode
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesTest, SetGlobalDarkColorMode, Function | SmallTest | Level2)
{
    ASSERT_TRUE(rsInterfaces->SetGlobalDarkColorMode(true));
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

/*
 * @tc.name: SetVirtualScreenBlackList
 * @tc.desc: Test SetVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require:issueI9P2VD
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenBlackList_Test, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeId> blackList = {1, 2, 3};
    int32_t ret = rsInterfaces->SetVirtualScreenBlackList(virtualScreenId, blackList);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: SetVirtualScreenTypeBlackList
 * @tc.desc: Test SetVirtualScreenTypeBlackList
 * @tc.type: FUNC
 * @tc.require:issueI9P2VD
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenTypeBlackList_Test, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeType> typeBlackList = {1, 2, 3};
    int32_t ret = rsInterfaces->SetVirtualScreenTypeBlackList(virtualScreenId, typeBlackList);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: AddVirtualScreenBlackList
 * @tc.desc: Test AddVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require:issueIB3TS6
 */
HWTEST_F(RSInterfacesTest, AddVirtualScreenBlackList_Test, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeId> blackList = {1, 2, 3};
    int32_t ret = rsInterfaces->AddVirtualScreenBlackList(virtualScreenId, blackList);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList
 * @tc.desc: Test RemoveVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require:issueIB3TS6
 */
HWTEST_F(RSInterfacesTest, RemoveVirtualScreenBlackList_Test, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeId> blackList = {1, 2, 3};
    int32_t ret = rsInterfaces->RemoveVirtualScreenBlackList(virtualScreenId, blackList);
    ASSERT_EQ(ret, 0);
}

/*
 * @tc.name: MarkPowerOffNeedProcessOneFrame
 * @tc.desc: Test MarkPowerOffNeedProcessOneFrame
 * @tc.type: FUNC
 * @tc.require:issueI9P2VD
 */
HWTEST_F(RSInterfacesTest, MarkPowerOffNeedProcessOneFrame, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->MarkPowerOffNeedProcessOneFrame();
}

/*
 * @tc.name: DisablePowerOffRenderControl
 * @tc.desc: Test DisablePowerOffRenderControl
 * @tc.type: FUNC
 * @tc.require:issueI9P2VD
 */
HWTEST_F(RSInterfacesTest, DisablePowerOffRenderControl, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->DisablePowerOffRenderControl(INVALID_SCREEN_ID);
}

/*
 * @tc.name: ForceRefreshOneFrameWithNextVSync
 * @tc.desc: Test ForceRefreshOneFrameWithNextVSync
 * @tc.type: FUNC
 * @tc.require:issueIBHG7Q
 */
HWTEST_F(RSInterfacesTest, ForceRefreshOneFrameWithNextVSync, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->ForceRefreshOneFrameWithNextVSync();
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow
 * @tc.desc: Test SetCastScreenEnableSkipWindow
 * @tc.type: FUNC
 * @tc.require: issueI9VAB2
 */
HWTEST_F(RSInterfacesTest, SetCastScreenEnableSkipWindow_Test, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    bool enable = true;
    auto res = rsInterfaces->SetCastScreenEnableSkipWindow(virtualScreenId, enable);
    EXPECT_EQ(res, SUCCESS);
}

/*
 * @tc.name: RegisterUIExtensionCallback_001
 * @tc.desc: Test if UIExtensionCallback can be sucessifully registered.
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSInterfacesTest, RegisterUIExtensionCallback_001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    UIExtensionCallback callback = [](std::shared_ptr<RSUIExtensionData>, uint64_t) {};
    uint64_t userId = 0;
    auto res = rsInterfaces->RegisterUIExtensionCallback(userId, callback);
    EXPECT_EQ(res, SUCCESS);
}

/*
 * @tc.name: RegisterUIExtensionCallback_002
 * @tc.desc: Test if UIExtensionCallback is null, registration failed.
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSInterfacesTest, RegisterUIExtensionCallback_002, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    UIExtensionCallback callback = nullptr;
    uint64_t userId = 0;
    auto res = rsInterfaces->RegisterUIExtensionCallback(userId, callback);
    EXPECT_EQ(res, SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenStatus
 * @tc.desc: Test SetVirtualScreenStatus
 * @tc.type: FUNC
 * @tc.require: issueIAF42F
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenStatus, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    EXPECT_EQ(rsInterfaces->SetVirtualScreenStatus(virtualScreenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY), true);
    EXPECT_EQ(rsInterfaces->SetVirtualScreenStatus(virtualScreenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE), true);
    rsInterfaces->RemoveVirtualScreen(virtualScreenId);
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList_001
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with abnormal params, screenId is INVALID_SCREEN_ID.
 * @tc.type: FUNC
 * @tc.require: issueIAJ2YU
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSecurityExemptionList_001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::vector<NodeId> securityExemptionList = {};
    int32_t res = rsInterfaces->SetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID, securityExemptionList);
    EXPECT_EQ(res, RS_CONNECTION_ERROR); // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList_002
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with normal params.
 * @tc.type: FUNC
 * @tc.require: issueIAJ2YU
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSecurityExemptionList_002, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;  // width value for test
    uint32_t defaultHeight = 1280;  // height value for test
    ASSERT_NE(rsInterfaces, nullptr);
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    std::vector<NodeId> securityExemptionList = {1, 2, 3};  // 1,2,3 NodeId for test
    int32_t res = rsInterfaces->SetVirtualScreenSecurityExemptionList(virtualScreenId, securityExemptionList);
    EXPECT_EQ(res, RS_CONNECTION_ERROR); // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList_003
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with normal params, list is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJ2YU
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSecurityExemptionList_003, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;  // width value for test
    uint32_t defaultHeight = 1280;  // height value for test
    ASSERT_NE(rsInterfaces, nullptr);
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    std::vector<NodeId> securityExemptionList = {};
    int32_t res = rsInterfaces->SetVirtualScreenSecurityExemptionList(virtualScreenId, securityExemptionList);
    EXPECT_EQ(res, RS_CONNECTION_ERROR); // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList_004
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with abnormal params, screen not found by screenId.
 * @tc.type: FUNC
 * @tc.require: issueIAJ8DJ
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSecurityExemptionList_004, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;  // width value for test
    uint32_t defaultHeight = 1280;  // height value for test
    ASSERT_NE(rsInterfaces, nullptr);
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", defaultWidth, defaultHeight, pSurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    ScreenId id = virtualScreenId + 1;
    std::vector<NodeId> securityExemptionList = {};
    int32_t res = rsInterfaces->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    EXPECT_EQ(res, RS_CONNECTION_ERROR); // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList_005
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with abnormal params, screen is not virtual by screenId.
 * @tc.type: FUNC
 * @tc.require: issueIAJ8DJ
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSecurityExemptionList_005, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::vector<NodeId> securityExemptionList = {};
    int32_t res = rsInterfaces->SetVirtualScreenSecurityExemptionList(0, securityExemptionList);
    EXPECT_EQ(res, RS_CONNECTION_ERROR); // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList_006
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with securityExemptionList size more than 1024.
 * @tc.type: FUNC
 * @tc.require: issueIB8NPD
 */
HWTEST_F(RSInterfacesTest, SetVirtualScreenSecurityExemptionList_006, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::vector<NodeId> securityExemptionList = {};
    for (NodeId i = 0; i <= 1024; i++) {  // 1024 is max securityExemptionList number
        securityExemptionList.emplace_back(i);
    }
    int32_t res = rsInterfaces->SetVirtualScreenSecurityExemptionList(0, securityExemptionList);
    EXPECT_EQ(res, INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetMirrorScreenVisibleRect_001
 * @tc.desc: Test SetMirrorScreenVisibleRect with abnormal params, screenId is INVALID_SCREEN_ID.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSInterfacesTest, SetMirrorScreenVisibleRect_001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    uint32_t defaultWidth = 720;  // width value for test
    uint32_t defaultHeight = 1280;  // height value for test
    Rect rect = {0, 0, defaultWidth, defaultHeight};

    int32_t res = rsInterfaces->SetMirrorScreenVisibleRect(INVALID_SCREEN_ID, rect);
    EXPECT_EQ(res, RS_CONNECTION_ERROR);  // Unable to access IPC due to lack of permissions.
    res = rsInterfaces->SetMirrorScreenVisibleRect(0, rect);
    EXPECT_EQ(res, RS_CONNECTION_ERROR);  // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetMirrorScreenVisibleRect_002
 * @tc.desc: Test SetMirrorScreenVisibleRect with abnormal params, rect is abnormal.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSInterfacesTest, SetMirrorScreenVisibleRect_002, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    int32_t x = -10;  // x value for test
    int32_t y = -10;  // y value for test
    int32_t defaultWidth = -720;  // width value for test
    int32_t defaultHeight = -1280;  // height value for test
    Rect rect = {x, y, defaultWidth, defaultHeight};

    int32_t res = rsInterfaces->SetMirrorScreenVisibleRect(0, rect);
    EXPECT_EQ(res, RS_CONNECTION_ERROR);  // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetMirrorScreenVisibleRect_003
 * @tc.desc: Test SetMirrorScreenVisibleRect with normal params.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSInterfacesTest, SetMirrorScreenVisibleRect_003, Function | SmallTest | Level2)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    ASSERT_NE(rsInterfaces, nullptr);
    uint32_t defaultWidth = 720;  // width value for test
    uint32_t defaultHeight = 1280;  // height value for test
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", defaultWidth, defaultHeight, pSurface, 0, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    Rect rect = {0, 0, defaultWidth, defaultHeight};
    int32_t res = rsInterfaces->SetMirrorScreenVisibleRect(virtualScreenId, rect);
    EXPECT_EQ(res, RS_CONNECTION_ERROR);  // Unable to access IPC due to lack of permissions.
}

/*
 * @tc.name: SetFreeMultiWindowStatus_001
 * @tc.desc: Test SetFreeMultiWindowStatus with false.
 * @tc.type: FUNC
 * @tc.require: issueIANPC2
 */
HWTEST_F(RSInterfacesTest, SetFreeMultiWindowStatus_001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->SetFreeMultiWindowStatus(false);
}

/*
 * @tc.name: SetFreeMultiWindowStatus_002
 * @tc.desc: Test SetFreeMultiWindowStatus with true.
 * @tc.type: FUNC
 * @tc.require: issueIANPC2
 */
HWTEST_F(RSInterfacesTest, SetFreeMultiWindowStatus_002, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->SetFreeMultiWindowStatus(true);
}

/*
 * @tc.name: SetLayerTop_001
 * @tc.desc: Test SetLayerTop with false.
 * @tc.type: FUNC
 * @tc.require: issueIAT8HK
 */
HWTEST_F(RSInterfacesTest, SetLayerTop_001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::string nodeIdStr = "123456";
    rsInterfaces->SetLayerTop(nodeIdStr, false);
}

/*
 * @tc.name: SetLayerTop_002
 * @tc.desc: Test SetLayerTop with true.
 * @tc.type: FUNC
 * @tc.require: issueIAT8HK
 */
HWTEST_F(RSInterfacesTest, SetLayerTop_002, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::string nodeIdStr = "123456";
    rsInterfaces->SetLayerTop(nodeIdStr, true);
}

/*
 * @tc.name: SetScreenSecurityMask_001
 * @tc.desc: Test SetScreenSecurityMask with normal params, securityMask is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSInterfacesTest, SetScreenSecurityMask_001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", sizeWidth, sizeHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t ret = rsInterfaces->SetScreenSecurityMask(virtualScreenId, nullptr);
    EXPECT_EQ(ret, SUCCESS);
}

/*
 * @tc.name: SetScreenSecurityMask_002
 * @tc.desc: Test SetScreenSecurityMask with normal params.
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSInterfacesTest, SetScreenSecurityMask_002, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", sizeWidth, sizeHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    const uint32_t color[1] = { 0x6f0000ff };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(color, colorLength, opts);
    int32_t ret = rsInterfaces->SetScreenSecurityMask(virtualScreenId, std::move(pixelMap));
    EXPECT_EQ(ret, SUCCESS);
}

/*
 * @tc.name: SetScreenSecurityMask_003
 * @tc.desc: Test SetScreenSecurityMask with too large pixelMap.
 * @tc.type: FUNC
 * @tc.require: issueIBCH1W
 */
HWTEST_F(RSInterfacesTest, SetScreenSecurityMask_003, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "VirtualScreenStatus0", sizeWidth, sizeHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    constexpr uint32_t colorWidth = 5000;
    constexpr uint32_t colorHeight = 5000;
    uint32_t *color;
    color = (uint32_t *)malloc(colorWidth * colorHeight * sizeof(uint32_t));
    memset_s(color, colorWidth * colorHeight, 0xffffffff, colorWidth * colorHeight * 2);
    uint32_t colorLength = colorWidth * colorHeight;
    Media::InitializationOptions opts;
    opts.size.width = colorWidth;
    opts.size.height = colorHeight;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(color, colorLength, opts);
    if (pixelMap) {
        int32_t ret = rsInterfaces->SetScreenSecurityMask(virtualScreenId, std::move(pixelMap));
        EXPECT_EQ(ret, RS_CONNECTION_ERROR);
    }
}

/*
 * @tc.name: TakeSurfaceCaptureTest
 * @tc.desc: Test TakeSurfaceCapture.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(RSInterfacesTest, TakeSurfaceCaptureTest, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::shared_ptr<RSDisplayNode> displayNode;
    std::shared_ptr<SurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig

    int32_t ret = rsInterfaces->TakeSurfaceCapture(displayNode, callback, captureConfig);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: RegisterSelfDrawingNodeRectChangeCallbackTest
 * @tc.desc: Test RegisterSelfDrawingNodeRectChangeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, RegisterSelfDrawingNodeRectChangeCallbackTest, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    SelfDrawingNodeRectChangeCallback callback =
        [](std::shared_ptr<RSSelfDrawingNodeRectData> SelfDrawingNodeRectData) {};
    ASSERT_EQ(rsInterfaces->RegisterSelfDrawingNodeRectChangeCallback(callback), SUCCESS);
}

/*
 * @tc.name: NotifyScreenSwitched
 * @tc.desc: Test NotifyScreenSwitched.
 * @tc.type: FUNC
 * @tc.require: issueIBH4PQ
 */
HWTEST_F(RSInterfacesTest, NotifyScreenSwitched, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->NotifyScreenSwitched();
}

/*
 * @tc.name: NotifyPageName
 * @tc.desc: Test NotifyPageName
 * @tc.type: FUNC
 * @tc.require: issueIBPH63
 */
HWTEST_F(RSInterfacesTest, NotifyPageName, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->NotifyPageName("com.package.other", "page", true);
    rsInterfaces->NotifyPageName("com.package.other", "page", false);
    ASSERT_NE(rsInterfaces, nullptr);
}

/*
 * @tc.name: GetMemoryGraphics
 * @tc.desc: Test GetMemoryGraphics
 * @tc.type: FUNC
 * @tc.require: issueIBPH63
 */
HWTEST_F(RSInterfacesTest, GetMemoryGraphics, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    rsInterfaces->GetMemoryGraphics();
}

/*
 * @tc.name: SetAncoForceDoDirect
 * @tc.desc: Test SetAncoForceDoDirect
 * @tc.type: FUNC
 * @tc.require: issueIBYASC
 */
HWTEST_F(RSInterfacesTest, SetAncoForceDoDirect, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    bool res = rsInterfaces->SetAncoForceDoDirect(false);
    ASSERT_EQ(res, false);
}

/*
 * @tc.name: SetColorFollow001
 * @tc.desc: Test SetColorFollow with false.
 * @tc.type: FUNC
 * @tc.require: issueIBZALS
 */
HWTEST_F(RSInterfacesTest, SetColorFollow001, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::string nodeIdStr = "1";
    rsInterfaces->SetColorFollow(nodeIdStr, false);
}

/*
 * @tc.name: SetColorFollow002
 * @tc.desc: Test SetColorFollow with true.
 * @tc.type: FUNC
 * @tc.require: issueIBZALS
 */
HWTEST_F(RSInterfacesTest, SetColorFollow002, Function | SmallTest | Level2)
{
    ASSERT_NE(rsInterfaces, nullptr);
    std::string nodeIdStr = "1";
    rsInterfaces->SetColorFollow(nodeIdStr, true);
}
} // namespace Rosen
} // namespace OHOS
