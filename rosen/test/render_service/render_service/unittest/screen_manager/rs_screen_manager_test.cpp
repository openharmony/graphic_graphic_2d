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
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
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
    static constexpr uint32_t LIGHT_LEVEL = 1;
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
 * @tc.name: QueryDefaultScreenInfo001
 * @tc.desc: Test QueryDefaultScreenInfo
 * @tc.type: FUNC
 * @tc.require: issueIAKFUL
 */
HWTEST_F(RSScreenManagerTest, QueryDefaultScreenInfo001, TestSize.Level1)
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

    screenManager->SetDefaultScreenId(id);
    auto defaultScreenInfo = screenManager->QueryDefaultScreenInfo();
    EXPECT_EQ(defaultScreenInfo.id, id);
    EXPECT_EQ(defaultScreenInfo.width, width);
    EXPECT_EQ(defaultScreenInfo.height, height);
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
 * @tc.desc: Test CreateVirtualScreen while whiteList is empty
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

    std::vector<NodeId> whiteList = {};

    auto id = screenManager->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, whiteList);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->RemoveVirtualScreen(id);
    usleep(SLEEP_TIME_US);
}

/*
 * @tc.name: CreateVirtualScreen_003
 * @tc.desc: Test CreateVirtualScreen while whiteList is not empty
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

    std::vector<NodeId> whiteList = {1};

    auto id = screenManager->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, whiteList);
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

    std::vector<NodeId> whiteList = {1};

    auto id = screenManager->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, whiteList);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto info = screenManager->QueryScreenInfo(id);
    ASSERT_NE(info.whiteList.end(), info.whiteList.find(1));

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
 * @tc.name: GetAndResetVirtualSurfaceUpdateFlag_001
 * @tc.desc: Test get virtualSurface update flag correctly
 * @tc.type: FUNC
 * @tc.require: issueIA9QG0
 */
HWTEST_F(RSScreenManagerTest, GetAndResetVirtualSurfaceUpdateFlag_001, TestSize.Level1)
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

    ASSERT_EQ(screenManager->GetAndResetVirtualSurfaceUpdateFlag(id), false);
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
 * @tc.desc: Test GetScreenSupportedColorGamuts false.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedColorGamuts_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::vector<ScreenColorGamut> mode;
    ASSERT_EQ(SCREEN_NOT_FOUND, screenManager->GetScreenSupportedColorGamuts(SCREEN_ID, mode));
}

/*
 * @tc.name: GetScreenSupportedColorGamuts_002
 * @tc.desc: Test GetScreenSupportedColorGamuts succeed.
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedColorGamuts_002, TestSize.Level1)
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
 * @tc.desc: Test GetScreenColorGamut false.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, GetScreenColorGamut_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenColorGamut mode;
    ASSERT_EQ(SCREEN_NOT_FOUND, screenManager->GetScreenColorGamut(SCREEN_ID, mode));
}

/*
 * @tc.name: GetScreenColorGamut_002
 * @tc.desc: Test GetScreenColorGamut succeed
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenColorGamut_002, TestSize.Level1)
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

    ASSERT_EQ(SCREEN_NOT_FOUND, screenManager->SetScreenColorGamut(INVALID_SCREEN_ID, 1));
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
 * @tc.name: SetScreenConstraint_001
 * @tc.desc: Test SetScreenConstraint
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, SetScreenConstraint_001, testing::ext::TestSize.Level2)
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

    auto mirrorId = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, mirrorId);
    // timestamp is 10000000 ns
    screenManager->SetScreenConstraint(mirrorId, 10000000, ScreenConstraintType::CONSTRAINT_NONE);
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
 * @tc.name: SetScreenActiveRect001
 * @tc.desc: Test SetScreenActiveRect
 * @tc.type: FUNC
 * @tc.require: issueIB3986
 */
HWTEST_F(RSScreenManagerTest, SetScreenActiveRect001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenId screenId = 1;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);

    GraphicIRect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(screenManager->SetScreenActiveRect(screenId, activeRect), StatusCode::HDI_ERROR);
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
 * @tc.desc: Test GetScreenData is empty.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
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
 * @tc.name: GetScreenData_002
 * @tc.desc: Test GetScreenData, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenData_002, TestSize.Level1)
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
 * @tc.name: GetScreenData_003
 * @tc.desc: Test GetScreenData, with mocked HDI device
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenData_003, TestSize.Level1)
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
    ASSERT_STRNE(dumpString.c_str(), empty.c_str());
    dumpString = "";
    screenManager->FpsDump(dumpString, arg);
    ASSERT_STRNE(dumpString.c_str(), empty.c_str());
    dumpString = "";
    screenManager->HitchsDump(dumpString, arg);
    ASSERT_STRNE(dumpString.c_str(), empty.c_str());
    dumpString = "";
    screenManager->ClearFpsDump(dumpString, arg);
    screenManager->ClearFrameBufferIfNeed();
    ASSERT_STRNE(dumpString.c_str(), empty.c_str());
}

/*
 * @tc.name: ScreenGamutMap_001
 * @tc.desc: Test SetScreenGamutMap And GetScreenGamutMap False.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, ScreenGamutMap_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    int32_t setStatusCode = screenManager->SetScreenGamutMap(SCREEN_ID, mode);
    ASSERT_EQ(setStatusCode, StatusCode::SCREEN_NOT_FOUND);
    int32_t getStatusCode = screenManager->GetScreenGamutMap(SCREEN_ID, mode);
    ASSERT_EQ(getStatusCode, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: ScreenGamutMap_002
 * @tc.desc: Test SetScreenGamutMap And GetScreenGamutMap Successed
 * @tc.type: FUNC
 * @tc.require: issueI60RFZ
 */
HWTEST_F(RSScreenManagerTest, ScreenGamutMap_002, TestSize.Level1)
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
    ASSERT_NE(nullptr, screenManager);
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->RemoveVirtualScreen(screenId);
    ASSERT_EQ(screenManager->GetProducerSurface(screenId), nullptr);
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
    auto surface = screenManager->GetProducerSurface(id);
    ASSERT_NE(surface, nullptr);
    screenManager->RemoveVirtualScreen(id);
    surface = screenManager->GetProducerSurface(id);
    ASSERT_EQ(surface, nullptr);
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
 * @tc.require: issueI9S56D
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
 * @tc.desc: Test SetScreenBacklight false.
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenBacklight_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager->SetScreenBacklight(screenId, LIGHT_LEVEL);
}

/*
 * @tc.name: SetScreenBacklight_002
 * @tc.desc: Test SetScreenBacklight succeed.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, SetScreenBacklight_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto id = screenManager->GetDefaultScreenId();
    screenManager->SetScreenBacklight(id, LIGHT_LEVEL);
    auto ret = screenManager->GetScreenBacklight(id);
    ASSERT_EQ(ret, LIGHT_LEVEL);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
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
 * @tc.desc: Test ResizeVirtualScreen false.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, ResizeVirtualScreen_001, testing::ext::TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto ret = screenManager->ResizeVirtualScreen(OHOS::Rosen::INVALID_SCREEN_ID, VIRTUAL_SCREEN_WIDTH,
        VIRTUAL_SCREEN_HEIGHT);
    ASSERT_EQ(SCREEN_NOT_FOUND, ret);
}

/*
 * @tc.name: ResizeVirtualScreen_002
 * @tc.desc: Test ResizeVirtualScreen succeed.
 * @tc.type: FUNC
 * @tc.require: issueI8F2HB
 */
HWTEST_F(RSScreenManagerTest, ResizeVirtualScreen_002, TestSize.Level1)
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
    auto activeScreenId = screenManager->GetActiveScreenId();
    bool isFoldScreenFlag = false;
    isFoldScreenFlag = system::GetParameter("const.window.foldscreen.type", "") != "";
    if (isFoldScreenFlag) {
        ASSERT_NE(INVALID_SCREEN_ID, activeScreenId);
    } else {
        ASSERT_EQ(INVALID_SCREEN_ID, activeScreenId);
    }
}

/*
 * @tc.name: IsAllScreensPowerOff_001
 * @tc.desc: Test ScreenPowerStatus are empty.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, IsAllScreensPowerOff_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto powerStatus = screenManager->IsAllScreensPowerOff();
    ASSERT_EQ(false, powerStatus);
}

/*
 * @tc.name: IsAllScreensPowerOff_002
 * @tc.desc: There is screen power on.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, IsAllScreensPowerOff_002, TestSize.Level1)
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

    screenManager->SetScreenPowerStatus(id, ScreenPowerStatus::POWER_STATUS_ON);
    auto powerStatus = screenManager->IsAllScreensPowerOff();
    ASSERT_EQ(false, powerStatus);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: ForceRefreshOneFrameIfNoRNV_001
 * @tc.desc: Test ForceRefreshOneFrameIfNoRNV.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, ForceRefreshOneFrameIfNoRNV_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->ForceRefreshOneFrameIfNoRNV();
}

/*
 * @tc.name: SetVirtualScreenStatus_001
 * @tc.desc: Test SetVirtualScreenStatus, input invalid id, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenStatus_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_FALSE(screenManager->SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: SetVirtualScreenStatus_002
 * @tc.desc: Test SetVirtualScreenStatus, input screens_ with nullptr, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenStatus_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId screenId = SCREEN_ID;
    screenManagerImpl.screens_[screenId] = nullptr;
    ASSERT_FALSE(screenManager->SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
}

/*
 * @tc.name: GetCanvasRotation_001
 * @tc.desc: Test GetCanvasRotation, input invalid id, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, GetCanvasRotation_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_FALSE(screenManager->GetCanvasRotation(screenId));
}

/*
 * @tc.name: GetCanvasRotation_002
 * @tc.desc: Test GetCanvasRotation, input screens_ with nullptr, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, GetCanvasRotation_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId screenId = SCREEN_ID;
    screenManagerImpl.screens_[screenId] = nullptr;
    ASSERT_FALSE(screenManager->GetCanvasRotation(screenId));
}

/*
 * @tc.name: GetScaleMode_001
 * @tc.desc: Test GetScaleMode, input invalid id, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, GetScaleMode_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_EQ(screenManager->GetScaleMode(screenId), ScreenScaleMode::INVALID_MODE);
}

/*
 * @tc.name: GetScaleMode_002
 * @tc.desc: Test GetScaleMode, input screens_ with nullptr, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, GetScaleMode_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId screenId = SCREEN_ID;
    screenManagerImpl.screens_[screenId] = nullptr;
    ASSERT_EQ(screenManager->GetScaleMode(screenId), ScreenScaleMode::INVALID_MODE);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode_001
 * @tc.desc: Test SetVirtualMirrorScreenScaleMode false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, SetVirtualMirrorScreenScaleMode_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto scaleMode = screenManager->SetVirtualMirrorScreenScaleMode(SCREEN_ID, ScreenScaleMode::INVALID_MODE);
    ASSERT_EQ(false, scaleMode);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode_002
 * @tc.desc: Test SetVirtualMirrorScreenScaleMode succeed.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, SetVirtualMirrorScreenScaleMode_002, TestSize.Level1)
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

    auto scaleMode = screenManager->SetVirtualMirrorScreenScaleMode(id, ScreenScaleMode::FILL_MODE);
    ASSERT_EQ(true, scaleMode);

    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: IsScreenPowerOffTest001
 * @tc.desc: Test IsScreenPowerOff, input invalid id, expect correctly get power status.
 * @tc.type: FUNC
 * @tc.require: issueI9UNQP
 */
HWTEST_F(RSScreenManagerTest, IsScreenPowerOffTest001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    ScreenId id = INVALID_SCREEN_ID;
    ASSERT_FALSE(screenManager->IsScreenPowerOff(id));
}

/*
 * @tc.name: IsScreenPowerOffTest002
 * @tc.desc: Test IsScreenPowerOff, input invalid id, expect false.
 * @tc.type: FUNC
 * @tc.require: issueI9UNQP
 */
HWTEST_F(RSScreenManagerTest, IsScreenPowerOffTest002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
        
    ScreenId id = 1;
    screenManagerImpl.screenPowerStatus_[id] = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON;
    ASSERT_FALSE(screenManager->IsScreenPowerOff(id));
    screenManagerImpl.screenPowerStatus_[id] = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_ON_ADVANCED;
    ASSERT_FALSE(screenManager->IsScreenPowerOff(id));
    screenManagerImpl.screenPowerStatus_[id] = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_OFF;
    ASSERT_TRUE(screenManager->IsScreenPowerOff(id));
    screenManagerImpl.screenPowerStatus_[id] = GraphicDispPowerStatus::GRAPHIC_POWER_STATUS_SUSPEND;
    ASSERT_TRUE(screenManager->IsScreenPowerOff(id));
}

/*
 * @tc.name: SetVirtualScreenBlackList001
 * @tc.desc: Test SetVirtualScreenBlackList, input invalid id
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager->SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenBlackList002
 * @tc.desc: Test SetVirtualScreenBlackList, input id  which not found in screens_
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ScreenId id = 1;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager->SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenBlackList003
 * @tc.desc: Test SetVirtualScreenBlackList, input id in screens_, and screens_[id] = null,
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenBlackList004
 * @tc.desc: Test SetVirtualScreenBlackList, screens_[id]!=nullptr, id==mainId
 * screens_[]!=null id==DefaultScreenId
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList004, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = id;
    screenManagerImpl.SetDefaultScreenId(mainId);

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenBlackList005
 * @tc.desc: Test SetVirtualScreenBlackList, screens_[id]!=nullptr, id!=mainId, mainId is not one of key of screens_
 * screens_[]!=null id==DefaultScreenId
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList005, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManagerImpl.SetDefaultScreenId(mainId);

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenBlackList006
 * @tc.desc: Test SetVirtualScreenBlackList, screens_[id]!=nullptr, id!=mainId, mainId is one of key of screens_ but
 * screens_[mainId]==nullptr screens_[]!=null id==DefaultScreenId
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList006, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManagerImpl.SetDefaultScreenId(mainId);

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    screenManagerImpl.screens_[mainId] = nullptr;
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: AddVirtualScreenBlackList001
 * @tc.desc: Test AddVirtualScreenBlackList, when id is INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager->AddVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: AddVirtualScreenBlackList002
 * @tc.desc: Test AddVirtualScreenBlackList, input id in screens_, and screens_[id] is multiple
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManagerImpl.AddVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);

    ScreenId mainId = id;
    screenManagerImpl.SetDefaultScreenId(mainId);
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    ASSERT_EQ(screenManagerImpl.AddVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: AddVirtualScreenBlackList003
 * @tc.desc: Test AddVirtualScreenBlackList, screens_[id]!=nullptr, id!=mainId, screens_[mainId]==null
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManagerImpl.SetDefaultScreenId(mainId);
    std::vector<uint64_t> blackList = {};

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    ASSERT_EQ(screenManagerImpl.AddVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);

    screenManagerImpl.screens_[mainId] = nullptr;
    ASSERT_EQ(screenManagerImpl.AddVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: AddVirtualScreenBlackList004
 * @tc.desc: Test AddVirtualScreenBlackList, screens_[id]!=nullptr, id!=mainId,  screens_[mainId]!=nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList004, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManagerImpl.SetDefaultScreenId(mainId);

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    screenManagerImpl.screens_[mainId] = std::make_unique<impl::RSScreen>(mainId, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[mainId], nullptr);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManagerImpl.AddVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList001
 * @tc.desc: Test RemoveVirtualScreenBlackList, when id is INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager->RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList002
 * @tc.desc: Test RemoveVirtualScreenBlackList, input id in screens_, and screens_[id] is multiple
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManagerImpl.RemoveVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);

    ScreenId mainId = id;
    screenManagerImpl.SetDefaultScreenId(mainId);
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    ASSERT_EQ(screenManagerImpl.RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList003
 * @tc.desc: Test RemoveVirtualScreenBlackList, id!=mainId, screens_[id]!=nullptr,screens_[mainId]==nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManagerImpl.SetDefaultScreenId(mainId);
    std::vector<uint64_t> blackList = {};

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    ASSERT_EQ(screenManagerImpl.RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);

    screenManagerImpl.screens_[mainId] = nullptr;
    ASSERT_EQ(screenManagerImpl.RemoveVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList004
 * @tc.desc: Test RemoveVirtualScreenBlackList, id!=mainId, screens_[id]!=nullptr, screens_[mainId]!=nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB3TS6
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList004, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManagerImpl.SetDefaultScreenId(mainId);

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    screenManagerImpl.screens_[mainId] = std::make_unique<impl::RSScreen>(mainId, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[mainId], nullptr);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManagerImpl.RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenBlackList007
 * @tc.desc: Test SetVirtualScreenBlackList, screens_[id]!=nullptr, id!=mainId, mainId is one of key of screens_ but
 * screens_[mainId]!=nullptr screens_[]!=null id==DefaultScreenId
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList007, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManagerImpl.SetDefaultScreenId(mainId);

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    screenManagerImpl.screens_[mainId] = std::make_unique<impl::RSScreen>(mainId, false, nullptr, nullptr);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList001
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with abnormal param
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSecurityExemptionList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    auto ret = screenManagerImpl.SetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);

    ScreenId id = 11;  // screenId for test
    ret = screenManagerImpl.SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    screenManagerImpl.screens_[id] = nullptr;
    ret = screenManagerImpl.SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    ret = screenManagerImpl.SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
    screenManagerImpl.screens_.erase(id);
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList002
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with normal param
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSecurityExemptionList002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    auto ret = screenManagerImpl.SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetVirtualScreenSecurityExemptionList001
 * @tc.desc: Test GetVirtualScreenSecurityExemptionList with abnormal param
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenSecurityExemptionList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    auto securityExemptionListGet = screenManagerImpl.GetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID);
    ASSERT_EQ(securityExemptionListGet.size(), 0);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    securityExemptionListGet = screenManagerImpl.GetVirtualScreenSecurityExemptionList(id);
    ASSERT_EQ(securityExemptionListGet.size(), 0);
}

/*
 * @tc.name: GetVirtualScreenSecurityExemptionList002
 * @tc.desc: Test GetVirtualScreenSecurityExemptionList with normal param
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenSecurityExemptionList002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    auto ret = screenManagerImpl.SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
    auto securityExemptionListGet = screenManagerImpl.GetVirtualScreenSecurityExemptionList(id);
    ASSERT_EQ(securityExemptionListGet.size(), securityExemptionList.size());
    for (auto i = 0; i < securityExemptionList.size(); i++) {
        ASSERT_EQ(securityExemptionListGet[i], securityExemptionList[i]);
    }
}

/*
 * @tc.name: SetMirrorScreenVisibleRect001
 * @tc.desc: Test SetMirrorScreenVisibleRect with abnormal params.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenManagerTest, SetMirrorScreenVisibleRect001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    uint32_t width = 720;  // width value for test
    uint32_t height = 1280;  // height value for test
    Rect rect = {0, 0, width, height};
    int32_t ret = screenManagerImpl.SetMirrorScreenVisibleRect(INVALID_SCREEN_ID, rect);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);

    screenManagerImpl.screens_.erase(id);
    ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    screenManagerImpl.screens_[id] = nullptr;
    ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetMirrorScreenVisibleRect002
 * @tc.desc: Test SetMirrorScreenVisibleRect with normal params.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenManagerTest, SetMirrorScreenVisibleRect002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    uint32_t width = 720;  // width value for test
    uint32_t height = 1280;  // height value for test
    Rect rect = {0, 0, width, height};
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    int32_t ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SUCCESS);

    rect = {-10, -10, -100, -100};  // test rect value
    ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetMirrorScreenVisibleRect001
 * @tc.desc: Test GetMirrorScreenVisibleRect with abnormal params.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenManagerTest, GetMirrorScreenVisibleRect001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    auto rect = screenManagerImpl.GetMirrorScreenVisibleRect(INVALID_SCREEN_ID);
    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    rect = screenManagerImpl.GetMirrorScreenVisibleRect(id);
}

/*
 * @tc.name: GetMirrorScreenVisibleRect002
 * @tc.desc: Test GetMirrorScreenVisibleRect with normal params.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenManagerTest, GetMirrorScreenVisibleRect002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    uint32_t width = 720;  // width value for test
    uint32_t height = 1280;  // height value for test
    Rect rectSet = {0, 0, width, height};
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, true, nullptr, nullptr);
    int32_t ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rectSet);
    ASSERT_EQ(ret, StatusCode::SUCCESS);

    auto rectGet = screenManagerImpl.GetMirrorScreenVisibleRect(id);
    ASSERT_EQ(rectSet.x, rectGet.x);
    ASSERT_EQ(rectSet.y, rectGet.y);
    ASSERT_EQ(rectSet.w, rectGet.w);
    ASSERT_EQ(rectSet.h, rectGet.h);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow001
 * @tc.desc: Test SetCastScreenEnableSkipWindow, input id not in keys of screens_
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetCastScreenEnableSkipWindow001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = INVALID_SCREEN_ID;
    bool enable = false;
    ASSERT_EQ(screenManagerImpl.SetCastScreenEnableSkipWindow(id, enable), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow002
 * @tc.desc: Test SetCastScreenEnableSkipWindow, input id in keys of screens_, and screens_[id] = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetCastScreenEnableSkipWindow002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    bool enable = false;
    ASSERT_EQ(screenManagerImpl.SetCastScreenEnableSkipWindow(id, enable), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow003
 * @tc.desc: Test SetCastScreenEnableSkipWindow, input id in keys of screens_, and screens_[id] != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetCastScreenEnableSkipWindow003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    bool enable = false;
    ASSERT_EQ(screenManagerImpl.SetCastScreenEnableSkipWindow(id, enable), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetCastScreenEnableSkipWindow001
 * @tc.desc: Test GetCastScreenEnableSkipWindow, input id not in keys of screens_
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, GetCastScreenEnableSkipWindow001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ASSERT_EQ(screenManagerImpl.GetCastScreenEnableSkipWindow(id), false);
}

/*
 * @tc.name: GetCastScreenEnableSkipWindow002
 * @tc.desc: Test GetCastScreenEnableSkipWindow, input id in keys of screens_， and screens[id] ==null
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, GetCastScreenEnableSkipWindow002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    ASSERT_EQ(screenManagerImpl.GetCastScreenEnableSkipWindow(id), false);
}

/*
 * @tc.name: GetCastScreenEnableSkipWindow003
 * @tc.desc: Test GetCastScreenEnableSkipWindow, input id in keys of screens_， and screens[id] !=null
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, GetCastScreenEnableSkipWindow003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);
    bool enable = false;
    ASSERT_EQ(screenManagerImpl.SetCastScreenEnableSkipWindow(id, enable), StatusCode::SUCCESS);
    ASSERT_EQ(screenManagerImpl.GetCastScreenEnableSkipWindow(id), enable);
}

/*
 * @tc.name: GetVirtualScreenBlackList001
 * @tc.desc: Test GetVirtualScreenBlackList, input id not in keys of screens_
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenBlackList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    ASSERT_TRUE(screenManagerImpl.GetVirtualScreenBlackList(id).empty());
}

/*
 * @tc.name: GetVirtualScreenBlackList002
 * @tc.desc: Test GetVirtualScreenBlackList, input id  in keys of screens_,screens_[id]=null
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenBlackList002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = nullptr;
    ASSERT_TRUE(screenManagerImpl.GetVirtualScreenBlackList(id).empty());
}

/*
 * @tc.name: GetVirtualScreenBlackList003
 * @tc.desc: Test GetVirtualScreenBlackList, input id in keys of screens_,screens_[id]!=null
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenBlackList003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = std::make_unique<impl::RSScreen>(id, false, nullptr, nullptr);

    const std::vector<uint64_t>& blackList = { 1, 2, 3, 4 };
    std::unordered_set<NodeId> screenBlackList(blackList.begin(), blackList.end());

    ScreenId mainId = id;
    screenManagerImpl.SetDefaultScreenId(mainId);
    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);

    const std::unordered_set<uint64_t>& blackList_ = screenManagerImpl.GetVirtualScreenBlackList(id);
    ASSERT_TRUE(std::set<NodeId>(blackList_.begin(), blackList_.end()) ==
                std::set<NodeId>(screenBlackList.begin(), screenBlackList.end()));
}

/*
 * @tc.name: TrySimpleProcessHotPlugEvents_001
 * @tc.desc: Test TrySimpleProcessHotPlugEvents when isHwcDead_ equals true, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, TrySimpleProcessHotPlugEvents_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.isHwcDead_ = true;
    ASSERT_FALSE(screenManager->TrySimpleProcessHotPlugEvents());
}

/*
 * @tc.name: TrySimpleProcessHotPlugEvents_002
 * @tc.desc: Test TrySimpleProcessHotPlugEvents when pendingHotPlugEvents_ is not empty, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, TrySimpleProcessHotPlugEvents_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.isHwcDead_ = false;
    impl::ScreenHotPlugEvent screenHotPlugEvent;
    ScreenId id = 1;
    screenManagerImpl.pendingHotPlugEvents_[id] = screenHotPlugEvent;
    ASSERT_FALSE(screenManager->TrySimpleProcessHotPlugEvents());
}

/*
 * @tc.name: TrySimpleProcessHotPlugEvents_003
 * @tc.desc: Test TrySimpleProcessHotPlugEvents when connectedIds_ is not empty, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, TrySimpleProcessHotPlugEvents_003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.isHwcDead_ = false;
    screenManagerImpl.pendingHotPlugEvents_.clear();
    ScreenId screenId = SCREEN_ID;
    screenManagerImpl.connectedIds_.push_back(screenId);
    ASSERT_FALSE(screenManager->TrySimpleProcessHotPlugEvents());
}

/*
 * @tc.name: TrySimpleProcessHotPlugEvents_004
 * @tc.desc: Test TrySimpleProcessHotPlugEvents, expect true.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, TrySimpleProcessHotPlugEvents_004, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.isHwcDead_ = false;
    screenManagerImpl.pendingHotPlugEvents_.clear();
    screenManagerImpl.connectedIds_.clear();
    ASSERT_TRUE(screenManager->TrySimpleProcessHotPlugEvents());
}

/*
 * @tc.name: GenerateVirtualScreenIdLocked_001
 * @tc.desc: Test GenerateVirtualScreenIdLocked when freeVirtualScreenIds_ is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, GenerateVirtualScreenIdLocked_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.freeVirtualScreenIds_ = std::queue<ScreenId>();
    ASSERT_EQ(screenManagerImpl.freeVirtualScreenIds_.size(), 0);

    screenManagerImpl.virtualScreenCount_ = 0;
    auto screenId = (static_cast<ScreenId>(screenManagerImpl.virtualScreenCount_) << 32) | 0xffffffffu;
    ASSERT_EQ(screenManagerImpl.GenerateVirtualScreenIdLocked(), screenId);
}

/*
 * @tc.name: GenerateVirtualScreenIdLocked_002
 * @tc.desc: Test GenerateVirtualScreenIdLocked when freeVirtualScreenIds_ is not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, GenerateVirtualScreenIdLocked_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.freeVirtualScreenIds_.push(SCREEN_ID);
    auto screenId = screenManagerImpl.freeVirtualScreenIds_.front();
    ASSERT_EQ(screenManagerImpl.GenerateVirtualScreenIdLocked(), screenId);
}

/*
 * @tc.name: ReuseVirtualScreenIdLocked_001
 * @tc.desc: Test ReuseVirtualScreenIdLocked.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, ReuseVirtualScreenIdLocked_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId screenId = SCREEN_ID;
    screenManagerImpl.ReuseVirtualScreenIdLocked(screenId);
    ASSERT_EQ(screenManagerImpl.freeVirtualScreenIds_.back(), screenId);
}

/*
 * @tc.name: ReleaseScreenDmaBufferTest_001
 * @tc.desc: Test ReleaseScreenDmaBuffer.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, ReleaseScreenDmaBufferTest_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId screenId = SCREEN_ID;
    impl::RSScreenManager::ReleaseScreenDmaBuffer(screenId);
    ASSERT_EQ(screenManagerImpl.GetOutput(screenId), nullptr);

    screenManagerImpl.screens_[SCREEN_ID] = std::make_unique<impl::RSScreen>(SCREEN_ID, false, nullptr, nullptr);
    impl::RSScreenManager::ReleaseScreenDmaBuffer(screenId);
    ASSERT_NE(screenManagerImpl.GetOutput(screenId), nullptr);
}
} // namespace OHOS::Rosen