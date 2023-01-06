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
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenManagerTest::SetUpTestCase() {}
void RSScreenManagerTest::TearDownTestCase() {}
void RSScreenManagerTest::SetUp() {}
void RSScreenManagerTest::TearDown() {}

/*
 * @tc.name: CreateOrGetScreenManager_001
 * @tc.desc: Test CreateOrGetScreenManager
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, CreateOrGetScreenManager_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
}

/*
 * @tc.name: GetDefaultScreenId_001
 * @tc.desc: Test GetDefaultScreenId
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetDefaultScreenId_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->SetDefaultScreenId(0);
    ASSERT_NE(INVALID_SCREEN_ID, screenManager->GetDefaultScreenId());
}

/*
 * @tc.name: CreateVirtualScreen_001
 * @tc.desc: Test CreateVirtualScreen
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, CreateVirtualScreen_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetAllScreenIds_001
 * @tc.desc: Test GetAllScreenIds
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetAllScreenIds_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    ASSERT_LT(0, static_cast<int32_t>(screenManager->GetAllScreenIds().size()));
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: SetVirtualScreenSurface_001
 * @tc.desc: Test SetVirtualScreenSurface
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSurface_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto id = screenManager->CreateVirtualScreen(name, width, height, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager->SetVirtualScreenSurface(id, psurface)));
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: SetVirtualScreenResolution_001
 * @tc.desc: Test SetVirtualScreenResolution
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenResolution_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    width = 960;
    height = 640;

    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager->SetVirtualScreenResolution(id, width, height)));
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: QueryScreenInfo_001
 * @tc.desc: Test QueryScreenInfo
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, QueryScreenInfo_001, TestSize.Level1)
{
    auto screenManager =  CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto info = screenManager->QueryScreenInfo(id);

    ASSERT_EQ(id, info.id);
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetProducerSurface_001
 * @tc.desc: Test GetProducerSurface
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetProducerSurface_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_NE(nullptr, screenManager->GetProducerSurface(id));
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: ProcessScreenHotPlugEvents_001
 * @tc.desc: Test ProcessScreenHotPlugEvents
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenHotPlugEvents_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->ProcessScreenHotPlugEvents();
}

/*
 * @tc.name: GetScreenBacklight_001
 * @tc.desc: Test GetScreenBacklight
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenBacklight_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(INVALID_BACKLIGHT_VALUE, screenManager->GetScreenBacklight(id));

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: AddScreenChangeCallback_001
 * @tc.desc: Test AddScreenChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, AddScreenChangeCallback_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ASSERT_EQ(INVALID_ARGUMENTS, screenManager->AddScreenChangeCallback(nullptr));
}

/*
 * @tc.name: GetScreenSupportedColorGamuts_001
 * @tc.desc: Test GetScreenSupportedColorGamuts
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedColorGamuts_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    std::vector<ScreenColorGamut> mode;
    ASSERT_EQ(SUCCESS, screenManager->GetScreenSupportedColorGamuts(id, mode));
    ASSERT_LT(0, mode.size());

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenColorGamut_001
 * @tc.desc: Test GetScreenColorGamut
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenColorGamut_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager->SetScreenColorGamut(id, 1));
    ScreenColorGamut mode;
    ASSERT_EQ(SUCCESS, screenManager->GetScreenColorGamut(id, mode));
    ASSERT_EQ(COLOR_GAMUT_DCI_P3, mode);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: SetScreenMirror_001
 * @tc.desc: Test SetScreenMirror
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetScreenMirror_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenId id = screenManager->GetDefaultScreenId();

    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto mirrorId = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, mirrorId);
    screenManager->SetScreenMirror(id, mirrorId);
}

/*
 * @tc.name: SetScreenActiveMode_001
 * @tc.desc: Test SetScreenActiveMode
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetScreenActiveMode_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->SetDefaultScreenId(0);
    ScreenId screenId = screenManager->GetDefaultScreenId();
    ASSERT_NE(INVALID_SCREEN_ID, screenId);
    // [PLANNING]: ASSERT_NE
    RSScreenModeInfo screenModeInfo0;
    std::vector<RSScreenModeInfo> screenSupportedModes;
    screenSupportedModes = screenManager->GetScreenSupportedModes(screenId);
    screenManager->SetScreenActiveMode(screenId, screenSupportedModes.size() + 3);
    // [PLANNING]: ASSERT_EQ
}

/*
 * @tc.name: GetScreenActiveMode_001
 * @tc.desc: Test GetScreenActiveMode
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenActiveMode_001, testing::ext::TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    RSScreenModeInfo screenModeInfo1;
    screenManager->GetScreenActiveMode(OHOS::Rosen::INVALID_SCREEN_ID, screenModeInfo1);
    ASSERT_EQ(screenModeInfo1.GetScreenWidth(), -1);
    ASSERT_EQ(screenModeInfo1.GetScreenHeight(), -1);
    ASSERT_EQ(screenModeInfo1.GetScreenRefreshRate(), 0);
    ASSERT_EQ(screenModeInfo1.GetScreenModeId(), -1);
}

/*
 * @tc.name: GetScreenSupportedModes_001
 * @tc.desc: Test GetScreenSupportedModes
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedModes_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::vector<RSScreenModeInfo> screenSupportedModes1;
    screenSupportedModes1 = screenManager->GetScreenSupportedModes(OHOS::Rosen::INVALID_SCREEN_ID);
    ASSERT_EQ(screenSupportedModes1.size(), 0);
}

/*
 * @tc.name: GetScreenCapability_001
 * @tc.desc: Test GetScreenCapability
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenCapability_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    RSScreenCapability screenCapability1;
    screenCapability1 = screenManager->GetScreenCapability(OHOS::Rosen::INVALID_SCREEN_ID);
    ASSERT_EQ(screenCapability1.GetType(),  OHOS::Rosen::DISP_INVALID);
}

/*
 * @tc.name: GetScreenData_001
 * @tc.desc: Test GetScreenData
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenData_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    RSScreenData screenData1;
    screenData1 = screenManager->GetScreenData(OHOS::Rosen::INVALID_SCREEN_ID);
    ASSERT_EQ(screenData1.GetSupportModeInfo().size(), 0);
}

/*
 * @tc.name: RSDump_001
 * @tc.desc: Test RS Dump
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, RSDump_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string dumpString = "";
    std::string arg = "";
    std::string empty = "";
    auto csurface = Surface::CreateSurfaceAsConsumer();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(csurface, nullptr);
    auto id = screenManager->CreateVirtualScreen("VirtualScreen", 200, 200, psurface, 0, 0);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->DisplayDump(dumpString);
    ASSERT_STRNE(dumpString.c_str(), empty.c_str());
    dumpString = "";
    screenManager->SurfaceDump(dumpString);
    dumpString = "";
    screenManager->FpsDump(dumpString, arg);
}

/*
 * @tc.name: ScreenGamutMap_001
 * @tc.desc: Test SetScreenGamutMap And GetScreenGamutMap Successed
 * @tc.type: FUNC
 * @tc.require: issueI60RFZ
 */
HWTEST_F(RSScreenManagerTest, ScreenGamutMap_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, nullptr);
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);
    int32_t setStatusCode = screenManager->SetScreenGamutMap(virtualScreenId, mode);
    ASSERT_EQ(setStatusCode, StatusCode::SUCCESS);
    int32_t getStatusCode = screenManager->GetScreenGamutMap(virtualScreenId, mode);
    ASSERT_EQ(getStatusCode, StatusCode::SUCCESS);
}
} // namespace OHOS::Rosen
