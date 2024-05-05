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
#include "mock_hdi_device.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr int SLEEP_TIME_US = 500;
    static constexpr uint32_t VIRTUAL_SCREEN_WIDTH = 480;
    static constexpr uint32_t VIRTUAL_SCREEN_HEIGHT = 320;
    static constexpr uint64_t SCREEN_ID = 10;
    static inline ScreenId mockScreenId_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void RSScreenManagerTest::SetUpTestCase()
{
    mockScreenId_ = 0xFFFF;
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, SetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SaveArg<1>(&Mock::HdiDeviceMock::powerStatusMock_), testing::Return(0)));
    EXPECT_CALL(*hdiDeviceMock_, GetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::powerStatusMock_)), testing::Return(0)));
}

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
 * @tc.name: SetDefaultScreenId_001
 * @tc.desc: Test SetDefaultScreenId
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, SetDefaultScreenId_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->SetDefaultScreenId(SCREEN_ID);
    ASSERT_EQ(SCREEN_ID, screenManager->GetDefaultScreenId());
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

    auto csurface = IConsumerSurface::Create();
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
 * @tc.name: CreateVirtualScreen_002
 * @tc.desc: Test CreateVirtualScreen while filteredAppVector is empty
 * @tc.type: FUNC
 * @tc.require: issueI8FSLX
 */
HWTEST_F(RSScreenManagerTest, CreateVirtualScreen_002, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<NodeId> filteredAppVector = {};

    auto id = screenManager->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, filteredAppVector);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->RemoveVirtualScreen(id);
    usleep(SLEEP_TIME_US);
}

/*
 * @tc.name: CreateVirtualScreen_003
 * @tc.desc: Test CreateVirtualScreen while filteredAppVector is not empty
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, CreateVirtualScreen_003, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<NodeId> filteredAppVector = {1};

    auto id = screenManager->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, filteredAppVector);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->RemoveVirtualScreen(id);
    usleep(SLEEP_TIME_US);
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

    auto csurface = IConsumerSurface::Create();
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

    auto csurface = IConsumerSurface::Create();
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

    auto csurface = IConsumerSurface::Create();
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
 * @tc.name: SetVirtualScreenResolution_002
 * @tc.desc: Test SetVirtualScreenResolution
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenResolution_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t width = static_cast<uint32_t>(100);
    uint32_t height = static_cast<uint32_t>(100);
    auto result = screenManager->SetVirtualScreenResolution(screenId, width, height);
    ASSERT_EQ(result, SCREEN_NOT_FOUND);
}

/*
 * @tc.name: GetVirtualScreenResolution_001
 * @tc.desc: Test GetVirtualScreenResolution Succeed
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenResolution_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 1344;
    uint32_t height = 2772;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    width = 1088;
    height = 2224;

    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager->SetVirtualScreenResolution(id, width, height)));
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager->GetVirtualScreenResolution(id, virtualScreenResolution);
    ASSERT_EQ(virtualScreenResolution.GetVirtualScreenWidth(), width);
    ASSERT_EQ(virtualScreenResolution.GetVirtualScreenHeight(), height);
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetVirtualScreenResolution_002
 * @tc.desc: Test GetVirtualScreenResolution Failed
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenResolution_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t width = static_cast<uint32_t>(100);
    uint32_t height = static_cast<uint32_t>(100);
    auto result = screenManager->SetVirtualScreenResolution(screenId, width, height);
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager->GetVirtualScreenResolution(screenId, virtualScreenResolution);
    ASSERT_NE(virtualScreenResolution.GetVirtualScreenWidth(), width);
    ASSERT_NE(virtualScreenResolution.GetVirtualScreenHeight(), height);
    ASSERT_EQ(result, SCREEN_NOT_FOUND);
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

    auto csurface = IConsumerSurface::Create();
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
 * @tc.name: QueryScreenInfo_002
 * @tc.desc: Test QueryScreenInfo
 * @tc.type: FUNC
 * @tc.require: issueI8FSLX
 */
HWTEST_F(RSScreenManagerTest, QueryScreenInfo_002, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<NodeId> filteredAppVector = {1};

    auto id = screenManager->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, filteredAppVector);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto info = screenManager->QueryScreenInfo(id);
    ASSERT_NE(info.filteredAppSet.end(), info.filteredAppSet.find(1));

    screenManager->RemoveVirtualScreen(id);
    usleep(500);
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

    auto csurface = IConsumerSurface::Create();
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

    auto csurface = IConsumerSurface::Create();
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

    auto csurface = IConsumerSurface::Create();
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

    auto csurface = IConsumerSurface::Create();
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
 * @tc.name: GetScreenType_001
 * @tc.desc: Test GetScreenType
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetScreenType_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    RSScreenType type;
    ASSERT_EQ(StatusCode::SUCCESS, screenManager->GetScreenType(id, type));

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

    auto csurface = IConsumerSurface::Create();
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
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetScreenCapability_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    RSScreenCapability screenCapability = screenManager->GetScreenCapability(id);
    ASSERT_EQ("virtualScreen01", screenCapability.GetName());

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenHDRCapability_001
 * @tc.desc: Test GetScreenHDRCapability
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetScreenHDRCapability_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    RSScreenHDRCapability screenHdrCapability;
    auto ret = screenManager->GetScreenHDRCapability(OHOS::Rosen::INVALID_SCREEN_ID, screenHdrCapability);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: GetScreenHDRCapabilityLocked_001
 * @tc.desc: Test GetScreenHDRCapabilityLocked
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetScreenHDRCapabilityLocked_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, nullptr);
    RSScreenHDRCapability screenHdrCapability;
    auto ret = screenManager->GetScreenHDRCapability(virtualScreenId, screenHdrCapability);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
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
    auto csurface = IConsumerSurface::Create();
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

/*
 * @tc.name: GetScreenCapabilityLocked_001
 * @tc.desc: Test GetScreenCapabilityLocked
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenCapabilityLocked_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, nullptr);
    RSScreenCapability screenCapability;
    screenCapability = screenManager->GetScreenCapability(virtualScreenId);
    ASSERT_EQ(screenCapability.GetName(), "virtualScreen01");
}

/*
 * @tc.name: SetVirtualScreenSurface_002
 * @tc.desc: Test SetVirtualScreenSurface
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSurface_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_EQ(screenManager->SetVirtualScreenSurface(screenId, psurface), SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenSurface_003
 * @tc.desc: Test SetVirtualScreenSurface
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSurface_003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId newId = 0;
    ASSERT_NE(screenManager->SetVirtualScreenSurface(newId, nullptr), SUCCESS);
    screenManager->Init();
    auto rsScreen = std::make_unique<impl::RSScreen>(newId, false, HdiOutput::CreateHdiOutput(newId), nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(screenManager->SetVirtualScreenSurface(newId, psurface), SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenSurface_004
 * @tc.desc: Test SetVirtualScreenSurface while suface not unique
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSurface_004, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id1 = screenManager->
        CreateVirtualScreen("virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, nullptr);
    ASSERT_NE(id1, INVALID_SCREEN_ID);
    auto id2 = screenManager->
        CreateVirtualScreen("virtualScreen02", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface);
    ASSERT_NE(id2, INVALID_SCREEN_ID);

    ASSERT_EQ(static_cast<StatusCode>(screenManager->SetVirtualScreenSurface(id1, psurface)), SURFACE_NOT_UNIQUE);
    screenManager->RemoveVirtualScreen(id1);
    screenManager->RemoveVirtualScreen(id2);
    usleep(SLEEP_TIME_US);
}

/*
 * @tc.name: RemoveVirtualScreen_001
 * @tc.desc: Test RemoveVirtualScreen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreen_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: RemoveVirtualScreen_002
 * @tc.desc: Test RemoveVirtualScreen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreen_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen("virtualScreen001", 480, 320, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto id_ = screenManager->CreateVirtualScreen("virtualScreen002", 480, 320, psurface);
    ASSERT_EQ(INVALID_SCREEN_ID, id_);

    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: SetScreenPowerStatus_001
 * @tc.desc: Test SetScreenPowerStatus, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenPowerStatus_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_.count(screenId), 0);
}

/*
 * @tc.name: SetScreenPowerStatus_002
 * @tc.desc: Test SetScreenPowerStatus, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenPowerStatus_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], ScreenPowerStatus::POWER_STATUS_ON);
}

/*
 * @tc.name: SetScreenPowerStatus_003
 * @tc.desc: Test SetScreenPowerStatus, test POWER_STATUS_ON_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenPowerStatus_003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager->MockHdiScreenConnected(rsScreen);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_ON);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_ON_ADVANCED);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_OFF);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_OFF_ADVANCED);
}

/*
 * @tc.name: GetScreenPowerStatus_001
 * @tc.desc: Test GetScreenPowerStatus, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatus_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), INVALID_POWER_STATUS);
}

/*
 * @tc.name: GetScreenPowerStatus_002
 * @tc.desc: Test GetScreenPowerStatus, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatus_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), ScreenPowerStatus::INVALID_POWER_STATUS);
}

/*
 * @tc.name: GetScreenPowerStatus_003
 * @tc.desc: Test GetScreenPowerStatus, test POWER_STATUS_OFF_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatus_003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager->MockHdiScreenConnected(rsScreen);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), POWER_STATUS_ON);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), POWER_STATUS_ON_ADVANCED);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), POWER_STATUS_OFF);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), POWER_STATUS_OFF_ADVANCED);
}

/*
 * @tc.name: SetScreenCorrection_001
 * @tc.desc: Test SetScreenCorrection, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, SetScreenCorrection_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_EQ(screenManager->SetScreenCorrection(screenId, ScreenRotation::ROTATION_0), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetScreenCorrection_002
 * @tc.desc: Test SetScreenCorrection, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, SetScreenCorrection_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    ASSERT_EQ(screenManager->SetScreenCorrection(screenId, ScreenRotation::ROTATION_270), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenCorrection_001
 * @tc.desc: Test GetScreenCorrection, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetScreenCorrection_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->GetScreenCorrection(screenId);
    ASSERT_EQ(screenManager->GetScreenCorrection(screenId), ScreenRotation::INVALID_SCREEN_ROTATION);
}

/*
 * @tc.name: GetScreenCorrection_002
 * @tc.desc: Test GetScreenCorrection, with virtual screen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, GetScreenCorrection_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    screenManager->SetScreenCorrection(screenId, ScreenRotation::ROTATION_270);
    ASSERT_EQ(screenManager->GetScreenCorrection(screenId), ScreenRotation::ROTATION_270);
}

/*
 * @tc.name: GetScreenPowerStatusLocked_001
 * @tc.desc: Test GetScreenPowerStatusLocked, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatusLocked_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    ASSERT_EQ(screenManagerImpl.GetScreenPowerStatusLocked(screenId), INVALID_POWER_STATUS);
}

/*
 * @tc.name: GetScreenPowerStatusLocked_002
 * @tc.desc: Test GetScreenPowerStatusLocked, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatusLocked_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    ASSERT_EQ(screenManagerImpl.GetScreenPowerStatusLocked(screenId), INVALID_POWER_STATUS);
}

/*
 * @tc.name: GetScreenPowerStatusLocked_003
 * @tc.desc: Test GetScreenPowerStatusLocked, test POWER_STATUS_OFF_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatusLocked_003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager->MockHdiScreenConnected(rsScreen);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManagerImpl.GetScreenPowerStatusLocked(screenId), POWER_STATUS_ON);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    ASSERT_EQ(screenManagerImpl.GetScreenPowerStatusLocked(screenId), POWER_STATUS_ON_ADVANCED);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(screenManagerImpl.GetScreenPowerStatusLocked(screenId), POWER_STATUS_OFF);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED);
    ASSERT_EQ(screenManagerImpl.GetScreenPowerStatusLocked(screenId), POWER_STATUS_OFF_ADVANCED);
}

/*
 * @tc.name: GetScreenData_002
 * @tc.desc: Test GetScreenData
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenData_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->GetScreenData(screenId);
}

/*
 * @tc.name: GetScreenData_003
 * @tc.desc: Test GetScreenData, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenData_003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    RSScreenData screenData = screenManager->GetScreenData(screenId);
    ASSERT_EQ(screenData.GetCapability().GetType(), DISP_INVALID);  // type_ attribute is INVALID for virtual screen.
    ASSERT_EQ(screenData.GetActivityModeInfo().GetScreenModeId(), -1);  // virtual screen not support active mode.
    ASSERT_EQ(screenData.GetSupportModeInfo().size(), 0);
    ASSERT_EQ(screenData.GetPowerStatus(), INVALID_POWER_STATUS);
}

/*
 * @tc.name: GetScreenData_004
 * @tc.desc: Test GetScreenData, with mocked HDI device
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenData_004, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenId screenId = mockScreenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager->MockHdiScreenConnected(rsScreen);
    screenManager->SetScreenPowerStatus(screenId, POWER_STATUS_ON_ADVANCED);
    RSScreenData screenData = screenManager->GetScreenData(screenId);
    ASSERT_EQ(screenData.GetPowerStatus(), POWER_STATUS_ON_ADVANCED);
}

/*
 * @tc.name: GetScreenBacklight_002
 * @tc.desc: Test GetScreenBacklight
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenBacklight_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    auto result = screenManager->GetScreenBacklight(screenId);
    ASSERT_EQ(result, INVALID_BACKLIGHT_VALUE);
}

/*
 * @tc.name: SetScreenBacklight_001
 * @tc.desc: Test SetScreenBacklight
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenBacklight_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t level = static_cast<uint32_t>(1);
    screenManager->SetScreenBacklight(screenId, level);
}

/*
 * @tc.name: GetProducerSurface_002
 * @tc.desc: Test GetProducerSurface
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetProducerSurface_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    auto result = screenManager->GetProducerSurface(screenId);
    ASSERT_EQ(result, nullptr);
}

/*
 * @tc.name: GetOutput_001
 * @tc.desc: Test GetOutput
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetOutput_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    auto result = screenManager->GetOutput(screenId);
    ASSERT_EQ(result, nullptr);
}

/*
 * @tc.name: SetScreenSkipFrameInterval_001
 * @tc.desc: Test SetScreenSkipFrameInterval
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenSkipFrameInterval_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t interval = static_cast<uint32_t>(1);
    auto result = screenManager->SetScreenSkipFrameInterval(screenId, interval);
    ASSERT_EQ(result, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetPixelFormat_001
 * @tc.desc: Test SetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, SetPixelFormat_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager->SetPixelFormat(id, static_cast<GraphicPixelFormat>(20)));

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetPixelFormat_001
 * @tc.desc: Test GetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenManagerTest, GetPixelFormat_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager->SetPixelFormat(id, static_cast<GraphicPixelFormat>(20)));
    GraphicPixelFormat pixelFormat;
    ASSERT_EQ(SUCCESS, screenManager->GetPixelFormat(id, pixelFormat));
    ASSERT_EQ(GRAPHIC_PIXEL_FMT_BGRA_8888, pixelFormat);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenSupportedHDRFormats_001
 * @tc.desc: Test GetScreenSupportedHDRFormats
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedHDRFormats_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(SUCCESS, screenManager->GetScreenSupportedHDRFormats(id, hdrFormats));
    ASSERT_LT(0, hdrFormats.size());

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenHDRFormat_001
 * @tc.desc: Test GetScreenHDRFormat
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenManagerTest, GetScreenHDRFormat_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager->SetScreenHDRFormat(id, 0));
    ScreenHDRFormat hdrFormat;
    ASSERT_EQ(SUCCESS, screenManager->GetScreenHDRFormat(id, hdrFormat));
    ASSERT_EQ(NOT_SUPPORT_HDR, hdrFormat);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces_001
 * @tc.desc: Test GetScreenSupportedColorSpaces
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedColorSpaces_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ASSERT_EQ(SUCCESS, screenManager->GetScreenSupportedColorSpaces(id, colorSpaces));
    ASSERT_LT(0, colorSpaces.size());

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenColorSpace_001
 * @tc.desc: Test GetScreenColorSpace
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenManagerTest, GetScreenColorSpace_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager->SetScreenColorSpace(
        id, static_cast<GraphicCM_ColorSpaceType>(1 | (2 << 8) | (3 << 16) | (1 << 21))));
    GraphicCM_ColorSpaceType colorSpace;
    ASSERT_EQ(SUCCESS, screenManager->GetScreenColorSpace(id, colorSpace));
    ASSERT_EQ(GRAPHIC_CM_SRGB_FULL, colorSpace);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: ResizeVirtualScreen_001
 * @tc.desc: Test ResizeVirtualScreen
 * @tc.type: FUNC
 * @tc.require: issueI8F2HB
 */
HWTEST_F(RSScreenManagerTest, ResizeVirtualScreen_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen0";
    uint32_t width = 500;
    uint32_t height = 300;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    auto info = screenManager->QueryScreenInfo(id);
    ASSERT_EQ(info.width, width);
    ASSERT_EQ(info.height, height);

    width = 0;
    height = 0;
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager->ResizeVirtualScreen(id, width, height)));
    info = screenManager->QueryScreenInfo(id);
    ASSERT_EQ(info.width, width);
    ASSERT_EQ(info.height, height);

    width = 100;
    height = 200;
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager->ResizeVirtualScreen(id, width, height)));
    info = screenManager->QueryScreenInfo(id);
    ASSERT_EQ(info.width, width);
    ASSERT_EQ(info.height, height);

    width = 10000;
    height = 9000;
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager->ResizeVirtualScreen(id, width, height)));
    info = screenManager->QueryScreenInfo(id);
    ASSERT_EQ(info.width, width);
    ASSERT_EQ(info.height, height);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: SetRogScreenResolution_001
 * @tc.desc: Test SetRogScreenResolution while screen's id doesn't match
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetRogScreenResolution_002, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->
        CreateVirtualScreen("virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(static_cast<StatusCode>(screenManager->
        SetRogScreenResolution(INVALID_SCREEN_ID, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT)), SCREEN_NOT_FOUND);
    screenManager->RemoveVirtualScreen(id);
    usleep(SLEEP_TIME_US);
}

/*
 * @tc.name: SetRogScreenResolution_002
 * @tc.desc: Test SetRogScreenResolution while the screen's id match
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetRogScreenResolution_003, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->
        CreateVirtualScreen("virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(static_cast<StatusCode>(screenManager->
        SetRogScreenResolution(id, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT)), SUCCESS);
    screenManager->RemoveVirtualScreen(id);
    usleep(SLEEP_TIME_US);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation_001
 * @tc.desc: Test SetVirtualMirrorScreenCanvasRotation while don't have any screen
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetVirtualMirrorScreenCanvasRotationn_001, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ASSERT_EQ(static_cast<StatusCode>(
        screenManager->SetVirtualMirrorScreenCanvasRotation(INVALID_SCREEN_ID, true)), false);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation_002
 * @tc.desc: Test SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetVirtualMirrorScreenCanvasRotationn_002, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->
        CreateVirtualScreen("virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(static_cast<StatusCode>(
        screenManager->SetVirtualMirrorScreenCanvasRotation(id, true)), true);
}

/*
 * @tc.name: GetActiveScreenId_001
 * @tc.desc: Test GetActiveScreenId
 * @tc.type: FUNC
 * @tc.require: issueI8ECTE
 */
HWTEST_F(RSScreenManagerTest, GetActiveScreenId_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    id = screenManager->GetActiveScreenId();
    ASSERT_EQ(INVALID_SCREEN_ID, id);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}
} // namespace OHOS::Rosen
