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
#include "screen_manager/rs_screen.h"
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
    static constexpr uint32_t SLEEP_TIME_FOR_DELAY = 1000000; // 1000ms
    static constexpr uint32_t LIGHT_LEVEL = 1;
    static constexpr uint64_t SCREEN_ID = 10;
    static constexpr uint32_t MAX_BLACK_LIST_NUM = 1024;
    static inline ScreenId mockScreenId_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;

    class RSIScreenChangeCallbackConfig : public RSIScreenChangeCallback {
    public:
        RSIScreenChangeCallbackConfig() = default;
        explicit RSIScreenChangeCallbackConfig(std::function<void()> callbackFunc_)
            : callbackFunc_(std::move(callbackFunc_))
        {}
        ~RSIScreenChangeCallbackConfig() = default;
        void OnScreenChanged(ScreenId id, ScreenEvent event,
            ScreenChangeReason reason = ScreenChangeReason::DEFAULT) override;
        sptr<IRemoteObject> AsObject() override;
    private:
        std::function<void()> callbackFunc_;
    };
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
void RSScreenManagerTest::RSIScreenChangeCallbackConfig::OnScreenChanged(ScreenId id,
    ScreenEvent event, ScreenChangeReason reason)
{
    return;
}

sptr<IRemoteObject> RSScreenManagerTest::RSIScreenChangeCallbackConfig::AsObject()
{
    return nullptr;
}

/*
 * @tc.name: HandlePostureData
 * @tc.desc: Test CreateOrGetScreenManager
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenManagerTest, HandlePostureData, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->HandlePostureData(nullptr);
    SensorEvent event[10];
    screenManager->HandlePostureData(event);
    uint8_t sensorData1[7] = {0, 0, 0, 0, 0, 0, -1};
    event[0].data = sensorData1;
    event[0].dataLen = sizeof(PostureData);
    screenManager->HandlePostureData(event);
    uint8_t sensorData2[7] = {0, 0, 0, 0, 0, 0, 150};
    event[0].data = sensorData2;
    screenManager->HandlePostureData(event);
    uint8_t sensorData3[7] = {0, 0, 0, 0, 0, 0, 180};
    event[0].data = sensorData3;
    screenManager->HandlePostureData(event);
}

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

/**
 * @tc.name: SetVirtualScreenAutoRotationTest
 * @tc.desc: Test SetVirtualScreenAutoRotation
 * @tc.type: FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenAutoRotationTest, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    ScreenId screenId = 500;
    screenManager->RemoveVirtualScreen(screenId);

    ASSERT_EQ(screenManager->SetVirtualScreenAutoRotation(screenId, true), StatusCode::SCREEN_NOT_FOUND);

    auto id = screenManager->CreateVirtualScreen(name, width, height, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(screenManager->SetVirtualScreenAutoRotation(id, true), StatusCode::SUCCESS);
    screenManager->RemoveVirtualScreen(id);
    sleep(1);
}

/**
 * @tc.name: GetVirtualScreenAutoRotationTest
 * @tc.desc: Test GetVirtualScreenAutoRotation
 * @tc.type: FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenAutoRotationTest, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    ScreenId screenId = 500;
    screenManager->RemoveVirtualScreen(screenId);

    ASSERT_FALSE(screenManager->GetVirtualScreenAutoRotation(screenId));

    auto id = screenManager->CreateVirtualScreen(name, width, height, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_FALSE(screenManager->GetVirtualScreenAutoRotation(id));
    ASSERT_EQ(screenManager->SetVirtualScreenAutoRotation(id, true), StatusCode::SUCCESS);
    ASSERT_TRUE(screenManager->GetVirtualScreenAutoRotation(id));

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
 * @tc.name: SetPhysicalScreenResolution_001
 * @tc.desc: Test SetPhysicalScreenResolution
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetPhysicalScreenResolution_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId id = mockScreenId_;
    auto rsScreen = std::make_shared<impl::RSScreen>(id, false, HdiOutput::CreateHdiOutput(id), nullptr);
    ASSERT_NE(rsScreen, nullptr);

    uint32_t width = 1920;
    uint32_t height = 1080;
    auto result = screenManager->SetPhysicalScreenResolution(id, width, height);
    ASSERT_EQ(result, SCREEN_NOT_FOUND);
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
 * @tc.name: ProcessScreenHotPlugEvents_002
 * @tc.desc: Test ProcessScreenHotPlugEvents
 * @tc.type: FUNC
 * @tc.require: issueIC9IVH
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenHotPlugEvents_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    impl::RSScreenManager& screenManagerImpl = static_cast<impl::RSScreenManager&>(*screenManager);
    constexpr ScreenId mockScreenId = 100;
    screenManagerImpl.pendingHotPlugEvents_[mockScreenId] = impl::ScreenHotPlugEvent{nullptr, true};
    screenManagerImpl.ProcessScreenHotPlugEvents();
    EXPECT_TRUE(screenManagerImpl.pendingHotPlugEvents_.empty());
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
 * @tc.name: AddScreenChangeCallback_002
 * @tc.desc: Test AddScreenChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueIC9IVH
 */
HWTEST_F(RSScreenManagerTest, AddScreenChangeCallback_002, TestSize.Level1)
{
    auto screenManager = sptr<impl::RSScreenManager>::MakeSptr();
    ASSERT_NE(nullptr, screenManager);
    sptr<RSIScreenChangeCallback> callback = sptr<RSScreenManagerTest::RSIScreenChangeCallbackConfig>::MakeSptr();
    ASSERT_NE(nullptr, callback);
    // case1: The screen is nullptr.
    constexpr ScreenId mockNullptrScreen = 0;
    screenManager->screens_[mockNullptrScreen] = nullptr;
    // case2: The screen is virtual.
    constexpr ScreenId mockVirtualScreen = 1;
    screenManager->screens_[mockVirtualScreen] =
        std::make_shared<impl::RSScreen>(mockVirtualScreen, true, nullptr, nullptr);
    // case3: The screen is physical.
    constexpr ScreenId mockPhysicalScreen = 2;
    screenManager->screens_[mockPhysicalScreen] =
        std::make_shared<impl::RSScreen>(mockPhysicalScreen, false, nullptr, nullptr);
    ASSERT_EQ(SUCCESS, screenManager->AddScreenChangeCallback(callback));
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);

    GraphicIRect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(screenManager->SetScreenActiveRect(screenId, activeRect), StatusCode::SUCCESS);
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
 * @tc.name: GetScreenHDRCapability_002
 * @tc.desc: Test GetScreenHDRCapability
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, GetScreenHDRCapability_002, TestSize.Level1)
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
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
    screenManager->DumpCurrentFrameLayers();
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
 * @tc.name: GetScreenCapability_002
 * @tc.desc: Test GetScreenCapability
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenCapability_002, TestSize.Level1)
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
    auto rsScreen = std::make_shared<impl::RSScreen>(newId, false, HdiOutput::CreateHdiOutput(newId), nullptr);
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
 * @tc.name: CheckPSurfaceChanged
 * @tc.desc: Test CheckPSurfaceChanged while suface change
 * @tc.type: FUNC
 * @tc.require: issueICMB9P
 */
HWTEST_F(RSScreenManagerTest, CheckPSurfaceChanged, TestSize.Level2)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    ScreenId id = mockScreenId_;
    ASSERT_EQ(screenManagerImpl.CheckPSurfaceChanged(id), false);
    auto screen = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    screen->isVirtual_ = true;
    screen->SetPSurfaceChange(true);
    screenManagerImpl.screens_[id] = std::move(screen);
    ASSERT_EQ(true, screenManagerImpl.CheckPSurfaceChanged(id));
    ASSERT_EQ(false, screenManagerImpl.CheckPSurfaceChanged(id));
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager->MockHdiScreenConnected(rsScreen);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    usleep(SLEEP_TIME_FOR_DELAY);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_ON);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    usleep(SLEEP_TIME_FOR_DELAY);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_ON_ADVANCED);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    usleep(SLEEP_TIME_FOR_DELAY);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_OFF);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED);
    usleep(SLEEP_TIME_FOR_DELAY);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_OFF_ADVANCED);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE);
    usleep(SLEEP_TIME_FOR_DELAY);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_DOZE);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE_SUSPEND);
    usleep(SLEEP_TIME_FOR_DELAY);
    ASSERT_EQ(screenManagerImpl.screenPowerStatus_[screenId], POWER_STATUS_DOZE_SUSPEND);
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, false, hdiOutput, nullptr);
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
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), POWER_STATUS_DOZE);
    screenManager->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE_SUSPEND);
    ASSERT_EQ(screenManager->GetScreenPowerStatus(screenId), POWER_STATUS_DOZE_SUSPEND);
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
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
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    screenManager->SetScreenCorrection(screenId, ScreenRotation::ROTATION_270);
    ASSERT_EQ(screenManager->GetScreenCorrection(screenId), ScreenRotation::ROTATION_270);
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
 * @tc.name: PostAndRemoveForceRefreshTaskTest
 * @tc.desc: Test PostForceRefreshTask and RemoveForceRefreshTask.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, PostAndRemoveForceRefreshTaskTest, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->PostForceRefreshTask();
    screenManager->RemoveForceRefreshTask();
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
 * @tc.name: DisablePowerOffRenderControl001
 * @tc.desc: Test disable power off render, input invalid screen id.
 * @tc.type: FUNC
 * @tc.require: issueICON9P
 */
HWTEST_F(RSScreenManagerTest, DisablePowerOffRenderControl001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = INVALID_SCREEN_ID;
    screenManagerImpl.DisablePowerOffRenderControl(id);
    ASSERT_EQ(screenManagerImpl.GetDisableRenderControlScreensCount(), 0);
}

/*
 * @tc.name: DisablePowerOffRenderControl002
 * @tc.desc: Test disable power off render, input valid screen id.
 * @tc.type: FUNC
 * @tc.require: issueICON9P
 */
HWTEST_F(RSScreenManagerTest, DisablePowerOffRenderControl002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    screenManagerImpl.DisablePowerOffRenderControl(id);
    ASSERT_NE(screenManagerImpl.GetDisableRenderControlScreensCount(), 0);
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
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_.erase(id);
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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
    screenManagerImpl.screens_.erase(mainId);

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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

    for (auto nodeId = 0; nodeId <= MAX_BLACK_LIST_NUM + 1; nodeId++) {
        blackList.push_back(nodeId);
    }
    ASSERT_EQ(screenManager->AddVirtualScreenBlackList(id, blackList), StatusCode::INVALID_ARGUMENTS);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    screenManagerImpl.screens_[mainId] = std::make_shared<impl::RSScreen>(mainId, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[mainId], nullptr);
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManagerImpl.AddVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);

    for (auto nodeId = 0; nodeId <= MAX_BLACK_LIST_NUM + 1; nodeId++) {
        blackList.push_back(nodeId);
    }
    ASSERT_EQ(screenManager->AddVirtualScreenBlackList(id, blackList), StatusCode::INVALID_ARGUMENTS);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_NE(screenManagerImpl.screens_[id], nullptr);
    screenManagerImpl.screens_[mainId] = std::make_shared<impl::RSScreen>(mainId, false, nullptr, nullptr);
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    screenManagerImpl.screens_[mainId] = std::make_shared<impl::RSScreen>(mainId, false, nullptr, nullptr);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenBlackList008
 * @tc.desc: Test SetVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require: issueICHZO3
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList008, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 10;
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    std::vector<uint64_t> blackList1 = {1, 2, 3, 4};
    ASSERT_EQ(screenManager->SetVirtualScreenBlackList(id, blackList1), StatusCode::SUCCESS);
    ASSERT_EQ(screenManager->GetVirtualScreenBlackList(id).size(), blackList1.size());

    std::vector<uint64_t> blackList2 = {1, 2, 3};
    ASSERT_EQ(screenManager->SetVirtualScreenBlackList(id, blackList2), StatusCode::SUCCESS);
    ASSERT_EQ(screenManager->GetVirtualScreenBlackList(id).size(), blackList2.size());
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

    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, true, nullptr, nullptr);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, true, nullptr, nullptr);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, true, nullptr, nullptr);
    int32_t ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SUCCESS);

    rect = {-10, -10, -100, -100};  // test rect value
    ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: SetMirrorScreenVisibleRect003
 * @tc.desc: Test SetMirrorScreenVisibleRect with zero rect.
 * @tc.type: FUNC
 * @tc.require: issueIB9E4C
 */
HWTEST_F(RSScreenManagerTest, SetMirrorScreenVisibleRect003, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, true, nullptr, nullptr);
    screenManagerImpl.screens_[id]->SetEnableVisibleRect(true);
    Rect rect = {0, 0, 0, 0};
    int32_t ret = screenManagerImpl.SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
    ASSERT_EQ(screenManagerImpl.screens_[id]->GetEnableVisibleRect(), false);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, true, nullptr, nullptr);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    bool enable = false;
    ASSERT_EQ(screenManagerImpl.SetCastScreenEnableSkipWindow(id, enable), StatusCode::SUCCESS);
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
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);

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
 * @tc.name: GetBlackListVirtualScreenByNode001
 * @tc.desc: Test GetBlackListVirtualScreenByNode
 * @tc.type: FUNC
 * @tc.require: issueIC9I11
 */
HWTEST_F(RSScreenManagerTest, GetBlackListVirtualScreenByNode001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    ScreenId id = 1;
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    const std::vector<uint64_t>& blackList = { 1, 2, 3, 4 };
    std::unordered_set<NodeId> screenBlackList(blackList.begin(), blackList.end());
    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);

    ASSERT_EQ(screenManagerImpl.GetBlackListVirtualScreenByNode(1).empty(), false);
}

/*
 * @tc.name: GetBlackListVirtualScreenByNode002
 * @tc.desc: Test GetBlackListVirtualScreenByNode
 * @tc.type: FUNC
 * @tc.require: issueICHZO3
 */
HWTEST_F(RSScreenManagerTest, GetBlackListVirtualScreenByNode002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    screenManagerImpl.blackListInVirtualScreen_.clear();

    const std::vector<uint64_t>& blackList = { 1, 2, 3, 4 };
    ASSERT_EQ(screenManagerImpl.SetVirtualScreenBlackList(INVALID_SCREEN_ID, blackList), StatusCode::SUCCESS);
    ASSERT_EQ(screenManagerImpl.GetBlackListVirtualScreenByNode(2).empty(), true);

    ScreenId id = 10;
    screenManagerImpl.screens_[id] = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    ASSERT_EQ(screenManagerImpl.GetBlackListVirtualScreenByNode(2).empty(), true);

    ScreenId virtualScreenId = 100;
    screenManagerImpl.screens_[virtualScreenId] = std::make_shared<impl::RSScreen>(id, true, nullptr, nullptr);
    ASSERT_EQ(screenManagerImpl.GetBlackListVirtualScreenByNode(2).empty(), true);

    screenManagerImpl.screens_[virtualScreenId]->SetCastScreenEnableSkipWindow(true);
    ASSERT_EQ(screenManagerImpl.GetBlackListVirtualScreenByNode(2).empty(), false);
}

/*
 * @tc.name: GetAllBlackList001
 * @tc.desc: Test GetAllBlackList
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSScreenManagerTest, GetAllBlackList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    ScreenId id = 1;
    auto screen = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    screen->SetCastScreenEnableSkipWindow(false);
    screenManagerImpl.screens_[id] = std::move(screen);

    const std::vector<uint64_t>& screenBlackList = { 1, 2, 3, 4 };
    ScreenId mainId = id;
    screenManagerImpl.SetDefaultScreenId(mainId);
    EXPECT_EQ(screenManagerImpl.SetVirtualScreenBlackList(id, screenBlackList), StatusCode::SUCCESS);

    auto allBlackList = screenManagerImpl.GetAllBlackList();
    for (const auto nodeId : screenBlackList) {
        EXPECT_EQ(allBlackList.count(nodeId), 1);
    }
}

/*
 * @tc.name: GetWhiteList001
 * @tc.desc: Test GetWhiteList
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSScreenManagerTest, GetWhiteList001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    ScreenId id = 1;
    auto screen = std::make_shared<impl::RSScreen>(id, false, nullptr, nullptr);
    const std::unordered_set<NodeId>& whiteList = { 1, 2, 3, 4 };
    screen->whiteList_ = whiteList;
    screenManagerImpl.screens_[id] = std::move(screen);

    auto allWhiteList = screenManagerImpl.GetAllWhiteList();
    for (const auto nodeId : whiteList) {
        EXPECT_EQ(allWhiteList.count(nodeId), 1);
    }

    auto screenWhiteList = screenManagerImpl.GetScreenWhiteList();
    ASSERT_NE(screenWhiteList.empty(), true);
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
 * @tc.desc: Test TrySimpleProcessHotPlugEvents when pendingConnectedIds_ is not empty, expect false.
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
    screenManagerImpl.pendingConnectedIds_.push_back(screenId);
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
    screenManagerImpl.pendingConnectedIds_.clear();
    ASSERT_TRUE(screenManager->TrySimpleProcessHotPlugEvents());
}

/*
 * @tc.name: GenerateVirtualScreenId_001
 * @tc.desc: Test GenerateVirtualScreenId when freeVirtualScreenIds_ is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, GenerateVirtualScreenId_001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.freeVirtualScreenIds_ = std::queue<ScreenId>();
    ASSERT_EQ(screenManagerImpl.freeVirtualScreenIds_.size(), 0);

    screenManagerImpl.virtualScreenCount_ = 0;
    auto screenId = (static_cast<ScreenId>(screenManagerImpl.virtualScreenCount_) << 32) | 0xffffffffu;
    ASSERT_EQ(screenManagerImpl.GenerateVirtualScreenId(), screenId);
}

/*
 * @tc.name: GenerateVirtualScreenId_002
 * @tc.desc: Test GenerateVirtualScreenId when freeVirtualScreenIds_ is not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, GenerateVirtualScreenId_002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

    screenManagerImpl.freeVirtualScreenIds_.push(SCREEN_ID);
    auto screenId = screenManagerImpl.freeVirtualScreenIds_.front();
    ASSERT_EQ(screenManagerImpl.GenerateVirtualScreenId(), screenId);
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
    screenManagerImpl.ReleaseScreenDmaBuffer(screenId);
    ASSERT_EQ(screenManagerImpl.GetOutput(screenId), nullptr);

    screenManagerImpl.screens_[SCREEN_ID] = std::make_shared<impl::RSScreen>(SCREEN_ID, false, nullptr, nullptr);
    screenManagerImpl.ReleaseScreenDmaBuffer(screenId);
    ASSERT_EQ(screenManagerImpl.GetOutput(screenId), nullptr);
}

/*
 * Function: SetScreenHasProtectedLayer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: create screenManager
 *                  2. operation: SetScreenHasProtectedLayer
 *                  3. result: screenManager is nullptr
 */
HWTEST_F(RSScreenManagerTest, SetScreenHasProtectedLayer001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    screenManager->SetScreenHasProtectedLayer(SCREEN_ID, true);
}

/*
 * @tc.name: GetVirtualScreenStatus
 * @tc.desc: Test GetVirtualScreenStatus
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenStatus, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto res = screenManager->GetVirtualScreenStatus(SCREEN_ID);
    ASSERT_EQ(res, VIRTUAL_SCREEN_PLAY);
}

/*
 * @tc.name: GetDisplayPropertyForHardCursor
 * @tc.desc: Test GetDisplayPropertyForHardCursor
 * @tc.type: FUNC
 * @tc.require: issueIBBM19
 */
HWTEST_F(RSScreenManagerTest, GetDisplayPropertyForHardCursor, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto res = screenManager->GetDisplayPropertyForHardCursor(SCREEN_ID);
    ASSERT_EQ(res, false);
}

/*
 * @tc.name: SetScreenSecurityMask001
 * @tc.desc: Test SetScreenSecurityMask001 with nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenSecurityMask001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtualScreen01", sizeWidth, sizeHeight, nullptr);
    auto ret = screenManager->SetScreenSecurityMask(virtualScreenId, nullptr);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: SetScreenSecurityMask002
 * @tc.desc: Test SetScreenSecurityMask002 with normal param
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenSecurityMask002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto virtualScreenId = screenManager->CreateVirtualScreen(
        "VirtualScreen01", sizeWidth, sizeHeight, psurface);
    const uint32_t color[1] = { 0x6f0000ff };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(color, colorLength, opts);
    auto ret = screenManager->SetScreenSecurityMask(virtualScreenId, std::move(pixelMap));
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenSecurityMask001
 * @tc.desc: Test GetScreenSecurityMask001 with normal param
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenSecurityMask001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtualScreen01", sizeWidth, sizeHeight, nullptr);
    screenManager->SetScreenSecurityMask(virtualScreenId, nullptr);
    auto ScreenSecurityMaskGet = screenManager->GetScreenSecurityMask(virtualScreenId);
    ASSERT_EQ(ScreenSecurityMaskGet, nullptr);
}

/*
 * @tc.name: GetVirtualScreenSecLayerOption001
 * @tc.desc: Test GetVirtualScreenSecLayerOption
 * @tc.type: FUNC
 * @tc.require: issueICCRA8
 */
HWTEST_F(RSScreenManagerTest, GetMirrorScreenSecLayerOption001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(nullptr, screenManager);
    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtualScreen01", sizeWidth, sizeHeight, nullptr, 0, 1);
    ASSERT_EQ(screenManager->GetVirtualScreenSecLayerOption(virtualScreenId), 1);
}

/*
 * @tc.name: SetScreenLinearMatrix
 * @tc.desc: Test SetScreenLinearMatrix
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenLinearMatrix, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    std::vector<float> vecMatix(5, 3.14f);

    screenManagerImpl->screens_.clear();

    auto res = screenManagerImpl->RSScreenManager::SetScreenLinearMatrix(100, vecMatix);
    EXPECT_EQ(StatusCode::SUCCESS, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::SetScreenLinearMatrix(100, vecMatix);
    EXPECT_EQ(StatusCode::SUCCESS, res);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    screenManagerImpl->screens_[100] = rsScreen0;
    res = screenManagerImpl->RSScreenManager::SetScreenLinearMatrix(100, vecMatix);
    EXPECT_EQ(StatusCode::SUCCESS, res);
    usleep(SLEEP_TIME_US);
}

/*
 * @tc.name: SetScreenHasProtectedLayer
 * @tc.desc: Test SetScreenHasProtectedLayer
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenHasProtectedLayer, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();
    screenManagerImpl->RSScreenManager::SetScreenHasProtectedLayer(100, true);
    EXPECT_EQ(false, rsScreen0->GetHasProtectedLayer());

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    screenManagerImpl->RSScreenManager::SetScreenHasProtectedLayer(100, true);
    EXPECT_EQ(false, rsScreen0->GetHasProtectedLayer());

    screenManagerImpl->screens_[100] = rsScreen0;
    screenManagerImpl->RSScreenManager::SetScreenHasProtectedLayer(100, true);
    EXPECT_EQ(true, rsScreen0->GetHasProtectedLayer());
}

/*
 * @tc.name: GetDisplayPropertyForHardCursor001
 * @tc.desc: Test GetDisplayPropertyForHardCursor001
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetDisplayPropertyForHardCursor001, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    rsScreen0->isHardCursorSupport_ = true;

    screenManagerImpl->screens_.clear();
    auto res = screenManagerImpl->RSScreenManager::GetDisplayPropertyForHardCursor(100);
    EXPECT_EQ(false, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::GetDisplayPropertyForHardCursor(100);
    EXPECT_EQ(false, res);

    screenManagerImpl->screens_[100] = rsScreen0;
    res = screenManagerImpl->RSScreenManager::GetDisplayPropertyForHardCursor(100);
    EXPECT_EQ(true, res);
}

/*
 * @tc.name: GetVirtualScreenStatus001
 * @tc.desc: Test GetVirtualScreenStatus001
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenStatus001, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    VirtualScreenStatus status = VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE;
    rsScreen0->screenStatus_ = status;

    screenManagerImpl->screens_.clear();
    auto res = screenManagerImpl->RSScreenManager::GetVirtualScreenStatus(100);
    EXPECT_EQ(VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::GetVirtualScreenStatus(100);
    EXPECT_EQ(VirtualScreenStatus::VIRTUAL_SCREEN_INVALID_STATUS, res);

    screenManagerImpl->screens_[100] = rsScreen0;
    res = screenManagerImpl->RSScreenManager::GetVirtualScreenStatus(100);
    EXPECT_EQ(VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE, res);
}

/*
 * @tc.name: SetVirtualScreenStatus
 * @tc.desc: Test SetVirtualScreenStatus
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenStatus, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    VirtualScreenStatus status = VirtualScreenStatus::VIRTUAL_SCREEN_PLAY;

    screenManagerImpl->screens_.clear();
    auto res = screenManagerImpl->RSScreenManager::SetVirtualScreenStatus(100, status);
    EXPECT_EQ(false, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::SetVirtualScreenStatus(100, status);
    EXPECT_EQ(false, res);

    screenManagerImpl->screens_[100] = rsScreen0;
    res = screenManagerImpl->RSScreenManager::SetVirtualScreenStatus(100, status);
    EXPECT_EQ(true, res);
}

/*
 * @tc.name: GetActualScreenMaxResolution
 * @tc.desc: Test GetActualScreenMaxResolution
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetActualScreenMaxResolution, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(200, true, HdiOutput::CreateHdiOutput(200), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    screenManagerImpl->screens_.insert(std::make_pair(200, rsScreen0));

    auto rsScreen1 =
        std::make_shared<impl::RSScreen>(300, false, HdiOutput::CreateHdiOutput(300), nullptr);
    EXPECT_NE(nullptr, rsScreen1);
    screenManagerImpl->screens_.insert(std::make_pair(300, rsScreen1));
    rsScreen1->phyWidth_ = 0;
    rsScreen1->phyHeight_ = 0;

    auto rsScreen2 =
        std::make_shared<impl::RSScreen>(400, false, HdiOutput::CreateHdiOutput(400), nullptr);
    EXPECT_NE(nullptr, rsScreen2);
    screenManagerImpl->screens_.insert(std::make_pair(400, rsScreen2));
    rsScreen2->phyWidth_ = 1;
    rsScreen2->phyHeight_ = 1;
    rsScreen2->width_ = 20;
    rsScreen2->height_ = 25;
    auto screenInfo = screenManagerImpl->RSScreenManager::GetActualScreenMaxResolution();
    EXPECT_EQ(20, screenInfo.width);
    EXPECT_EQ(25, screenInfo.height);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys
 * @tc.desc: Test GetScreenSupportedMetaDataKeys
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedMetaDataKeys, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    ScreenHDRMetadataKey dataKey = ScreenHDRMetadataKey::MATAKEY_HDR10_PLUS;
    std::vector<ScreenHDRMetadataKey> metaDataKey(1, dataKey);
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(200, false, HdiOutput::CreateHdiOutput(200), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    screenManagerImpl->screens_.insert(std::make_pair(200, rsScreen0));

    auto rsScreen1 =
        std::make_shared<impl::RSScreen>(300, true, HdiOutput::CreateHdiOutput(300), nullptr);
    EXPECT_NE(nullptr, rsScreen1);
    screenManagerImpl->screens_.insert(std::make_pair(300, rsScreen1));

    auto res = screenManagerImpl->RSScreenManager::GetScreenSupportedMetaDataKeys(100, metaDataKey);
    EXPECT_EQ(1, res);
}

/*
 * @tc.name: GetScreenColorSpace
 * @tc.desc: Test GetScreenColorSpace
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenColorSpace, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    screenManagerImpl->screens_.clear();
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;
    auto res = screenManagerImpl->RSScreenManager::GetScreenColorSpace(100, colorSpace);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    res = screenManagerImpl->RSScreenManager::GetScreenColorSpace(100, colorSpace);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: GetScreenSupportedColorSpaces
 * @tc.desc: Test GetScreenSupportedColorSpaces
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedColorSpaces, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    screenManagerImpl->screens_.clear();
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;
    std::vector<GraphicCM_ColorSpaceType> spaceType(1, colorSpace);
    auto res = screenManagerImpl->RSScreenManager::GetScreenSupportedColorSpaces(100, spaceType);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    res = screenManagerImpl->RSScreenManager::GetScreenSupportedColorSpaces(100, spaceType);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: Test SetScreenHDRFormat
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenHDRFormat, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    screenManagerImpl->screens_.clear();
    auto res = screenManagerImpl->RSScreenManager::SetScreenHDRFormat(100, 10);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    res = screenManagerImpl->RSScreenManager::SetScreenHDRFormat(100, 10);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: Test GetScreenHDRFormat
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenHDRFormat, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    ScreenHDRFormat format = {ScreenHDRFormat::IMAGE_HDR_VIVID_DUAL};
    screenManagerImpl->screens_.clear();
    auto res = screenManagerImpl->RSScreenManager::GetScreenHDRFormat(100, format);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    res = screenManagerImpl->RSScreenManager::GetScreenHDRFormat(100, format);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: GetScreenSupportedHDRFormats
 * @tc.desc: Test GetScreenSupportedHDRFormats
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedHDRFormats, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    screenManagerImpl->screens_.clear();

    ScreenHDRFormat format = { ScreenHDRFormat::IMAGE_HDR_VIVID_DUAL };
    std::vector<ScreenHDRFormat> screenFormat(5, format);
    auto res = screenManagerImpl->RSScreenManager::GetScreenSupportedHDRFormats(100, screenFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    res = screenManagerImpl->RSScreenManager::GetScreenSupportedHDRFormats(100, screenFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: SetPixelFormat
 * @tc.desc: Test SetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetPixelFormat, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    screenManagerImpl->screens_.clear();

    GraphicPixelFormat pixelFormat = { GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BGRA_8888 };
    auto res = screenManagerImpl->RSScreenManager::SetPixelFormat(100, pixelFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    res = screenManagerImpl->RSScreenManager::SetPixelFormat(100, pixelFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: GetPixelFormat
 * @tc.desc: Test GetPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetPixelFormat, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    screenManagerImpl->screens_.clear();

    GraphicPixelFormat pixelFormat = { GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BGRA_8888 };
    auto res = screenManagerImpl->RSScreenManager::GetPixelFormat(100, pixelFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    res = screenManagerImpl->RSScreenManager::GetPixelFormat(100, pixelFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: GetDisplayIdentificationData
 * @tc.desc: Test GetDisplayIdentificationData
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetDisplayIdentificationData, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();

    uint8_t outPort = 1;
    std::vector<uint8_t> edidData(5);

    int res = screenManagerImpl->RSScreenManager::
        GetDisplayIdentificationData(100, outPort, edidData);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::
        GetDisplayIdentificationData(100, outPort, edidData);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_[100] = rsScreen0;
    res = screenManagerImpl->RSScreenManager::
        GetDisplayIdentificationData(100, outPort, edidData);
    EXPECT_EQ(HDI_ERROR, res);
}

/*
 * @tc.name: SetEqualVsyncPeriod
 * @tc.desc: Test SetEqualVsyncPeriod
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetEqualVsyncPeriod, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();

    screenManagerImpl->RSScreenManager::SetEqualVsyncPeriod(100, false);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    screenManagerImpl->RSScreenManager::SetEqualVsyncPeriod(100, false);

    screenManagerImpl->screens_[100] = rsScreen0;
    screenManagerImpl->RSScreenManager::SetEqualVsyncPeriod(100, true);
    EXPECT_EQ(true, rsScreen0->RSScreen::GetEqualVsyncPeriod());
}

/*
 * @tc.name: SetVirtualScreenRefreshRate
 * @tc.desc: Test SetVirtualScreenRefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenRefreshRate, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();

    uint32_t maxRefreshRate1 = 0;
    uint32_t actualRefreshRate = 8;

    int res = screenManagerImpl->RSScreenManager::
        SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::
        SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_[100] = rsScreen0;
    rsScreen0->isVirtual_ = false;
    res = screenManagerImpl->RSScreenManager::
        SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    rsScreen0->isVirtual_ = true;
    res = screenManagerImpl->RSScreenManager::
        SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);

    uint32_t maxRefreshRate2 = 6;
    res = screenManagerImpl->RSScreenManager::
        SetVirtualScreenRefreshRate(100, maxRefreshRate2, actualRefreshRate);
    EXPECT_EQ(StatusCode::SUCCESS, res);

    uint32_t maxRefreshRate3 = 200;
    res = screenManagerImpl->RSScreenManager::
        SetVirtualScreenRefreshRate(100, maxRefreshRate3, actualRefreshRate);
    EXPECT_EQ(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: SetScreenSkipFrameInterval002
 * @tc.desc: Test SetScreenSkipFrameInterval002
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenSkipFrameInterval002, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, rsScreen0));

    auto res = screenManagerImpl->RSScreenManager::SetScreenSkipFrameInterval(100, 0);
    EXPECT_EQ(INVALID_ARGUMENTS, res);

    res = screenManagerImpl->RSScreenManager::SetScreenSkipFrameInterval(100, 1800);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys_002
 * @tc.desc: Test GetScreenSupportedMetaDataKeys
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedMetaDataKeys_002, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();

    ScreenHDRMetadataKey key = ScreenHDRMetadataKey::MATAKEY_BLUE_PRIMARY_X;
    std::vector<ScreenHDRMetadataKey> dataKey(5, key);
    auto res = screenManagerImpl->RSScreenManager::
        GetScreenSupportedMetaDataKeys(100, dataKey);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::
        GetScreenSupportedMetaDataKeys(100, dataKey);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    rsScreen0->isVirtual_  = true;
    screenManagerImpl->screens_[100] = rsScreen0;
    res = screenManagerImpl->RSScreenManager::
        GetScreenSupportedMetaDataKeys(100, dataKey);
    EXPECT_EQ(INVALID_BACKLIGHT_VALUE, res);
}

/*
 * @tc.name: SetScreenConstraint001
 * @tc.desc: Test SetScreenConstraint001
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenConstraint001, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, rsScreen0));

    rsScreen0->isVirtual_ = true;
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_ABSOLUTE;
    auto res = screenManagerImpl->RSScreenManager::
        SetScreenConstraint(100, 1000, type);
    EXPECT_EQ(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: RemoveScreenChangeCallback
 * @tc.desc: Test RemoveScreenChangeCallback
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, RemoveScreenChangeCallback, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);

    sptr<RSIScreenChangeCallback> callback1 =
        sptr<RSScreenManagerTest::RSIScreenChangeCallbackConfig>::MakeSptr();
    EXPECT_NE(nullptr, callback1);

    sptr<RSIScreenChangeCallback> callback2 =
        sptr<RSScreenManagerTest::RSIScreenChangeCallbackConfig>::MakeSptr();
    EXPECT_NE(nullptr, callback2);

    screenManagerImpl->screenChangeCallbacks_.push_back(callback1);
    screenManagerImpl->RSScreenManager::RemoveScreenChangeCallback(callback2);
    EXPECT_EQ(1, screenManagerImpl->screenChangeCallbacks_.size());

    screenManagerImpl->screenChangeCallbacks_.push_back(callback2);
    EXPECT_EQ(2, screenManagerImpl->screenChangeCallbacks_.size());

    screenManagerImpl->RSScreenManager::RemoveScreenChangeCallback(callback2);
    EXPECT_EQ(1, screenManagerImpl->screenChangeCallbacks_.size());
}

/*
 * @tc.name: RegisterScreenNodeListener001
 * @tc.desc: Test RegisterScreenNodeListener001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, RegisterScreenNodeListener001, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);
    screenManagerImpl->RegisterScreenNodeListener(nullptr);
    EXPECT_EQ(screenManagerImpl->screenNodeListener_, nullptr);
}

/*
 * @tc.name: SetScreenOffset001
 * @tc.desc: Test SetScreenOffset001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, SetScreenOffset001, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);
    screenManagerImpl->SetScreenOffset(0, 0, 0);
    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, rsScreen0));
    screenManagerImpl->SetScreenOffset(0, 0, 0);
}

/*
 * @tc.name: NotifyScreenNodeChange001
 * @tc.desc: Test NotifyScreenNodeChange001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, NotifyScreenNodeChange001, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);
    screenManagerImpl->screenNodeListener_ = nullptr;
    screenManagerImpl->NotifyScreenNodeChange(0, false);
}

/*
 * @tc.name: AddScreenChangeCallback001
 * @tc.desc: Test AddScreenChangeCallback001
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, AddScreenChangeCallback001, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));

    auto rsScreen1 =
        std::make_shared<impl::RSScreen>(200, true, HdiOutput::CreateHdiOutput(200), nullptr);
    EXPECT_NE(nullptr, rsScreen1);
    rsScreen0->isVirtual_ = false;
    screenManagerImpl->screens_.insert(std::make_pair(200, rsScreen1));

    auto rsScreen2 =
        std::make_shared<impl::RSScreen>(300, true, HdiOutput::CreateHdiOutput(300), nullptr);
    EXPECT_NE(nullptr, rsScreen2);
    rsScreen0->isVirtual_ = true;
    screenManagerImpl->screens_.insert(std::make_pair(200, rsScreen1));

    sptr<RSScreenManagerTest::RSIScreenChangeCallbackConfig> callback =
        sptr<RSScreenManagerTest::RSIScreenChangeCallbackConfig>::MakeSptr();
    EXPECT_NE(nullptr, callback);

    auto res = screenManagerImpl->RSScreenManager::AddScreenChangeCallback(callback);
    EXPECT_EQ(SUCCESS, res);
}

/*
 * @tc.name: SetVirtualScreenResolution003
 * @tc.desc: Test SetVirtualScreenResolution
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenResolution003, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(nullptr, screenManagerImpl);
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t width1 = 68800;
    uint32_t height1 = 100;
    auto result = screenManagerImpl->SetVirtualScreenResolution(screenId, width1, height1);
    EXPECT_EQ(result, INVALID_ARGUMENTS);

    uint32_t width2 = 100;
    uint32_t height2 = 68800;
    result = screenManagerImpl->SetVirtualScreenResolution(screenId, width2, height2);
    EXPECT_EQ(result, INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetPhysicalScreenResolution002
 * @tc.desc: Test SetPhysicalScreenResolution
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetPhysicalScreenResolution002, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    rsScreen0->isVirtual_ = true;
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, rsScreen0));

    ScreenId screenId = 100;
    uint32_t width = 100;
    uint32_t height = 100;
    auto res = screenManagerImpl->RSScreenManager::SetPhysicalScreenResolution(screenId, width, height);
    EXPECT_EQ(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: SetScreenActiveRect002
 * @tc.desc: Test SetScreenActiveRect
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetScreenActiveRect002, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    ScreenId screenId = 100;
    GraphicIRect rect = {1, 2, 3, 4};
    auto res = screenManagerImpl->RSScreenManager::SetScreenActiveRect(screenId, rect);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    rsScreen0->isVirtual_ = true;
    screenManagerImpl->screens_[100] = rsScreen0;
    res = screenManagerImpl->RSScreenManager::SetScreenActiveRect(screenId, rect);
    EXPECT_EQ(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: GetAndResetVirtualSurfaceUpdateFlag002
 * @tc.desc: Test get virtualSurface update flag correctly
 * @tc.type: FUNC
 * @tc.require: issueIA9QG0
 */
HWTEST_F(RSScreenManagerTest, GetAndResetVirtualSurfaceUpdateFlag002, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    screenManagerImpl->screens_.clear();
    ScreenId id = 10086;
    auto res = screenManagerImpl->RSScreenManager::GetAndResetVirtualSurfaceUpdateFlag(id);
    EXPECT_EQ(false, res);
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::GetAndResetVirtualSurfaceUpdateFlag(id);
    EXPECT_EQ(false, res);
}

/*
 * @tc.name: GetScreenSecurityMask002
 * @tc.desc: Test GetScreenSecurityMask001 with normal param
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenSecurityMask002, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);

    ScreenId id = 100;

    screenManagerImpl->screens_.clear();
    rsScreen0->securityMask_ = nullptr;
    screenManagerImpl->screens_.insert(std::make_pair(100, rsScreen0));
    auto res = screenManagerImpl->RSScreenManager::GetScreenSecurityMask(id);
    EXPECT_EQ(nullptr, res);
}

/*
 * @tc.name: SetScreenSecurityMask003
 * @tc.desc: Test SetScreenSecurityMask003
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenSecurityMask003, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    auto res = screenManagerImpl->RSScreenManager::SetScreenSecurityMask(INVALID_SCREEN_ID, nullptr);
    EXPECT_EQ(INVALID_ARGUMENTS, res);

    screenManagerImpl->screens_.clear();
    ScreenId id = 100;
    res = screenManagerImpl->RSScreenManager::SetScreenSecurityMask(id, nullptr);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::SetScreenSecurityMask(id, nullptr);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(200, true, HdiOutput::CreateHdiOutput(100), nullptr);
    EXPECT_NE(nullptr, rsScreen0);
    rsScreen0->isVirtual_ = true;
    screenManagerImpl->screens_[100] =  rsScreen0;
    res = screenManagerImpl->RSScreenManager::SetScreenSecurityMask(id, nullptr);
    EXPECT_EQ(SUCCESS, res);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList005
 * @tc.desc: Test RemoveVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList005, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> black;
    black.emplace_back(1024);
    black.emplace_back(2048);

    screenManagerImpl->castScreenBlackList_.clear();
    screenManagerImpl->castScreenBlackList_.insert(10086);
    screenManagerImpl->castScreenBlackList_.insert(1024);
    auto res = screenManagerImpl->RemoveVirtualScreenBlackList(id, black);
    EXPECT_EQ(SUCCESS, res);
}

/*
 * @tc.name: CreateVirtualScreen004
 * @tc.desc: Test CreateVirtualScreen004
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, CreateVirtualScreen004, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    std::string name = "WeChat";
    uint32_t width1 = 68000;
    uint32_t height1 = 100;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    ScreenId mirrorId = 10086;
    int32_t flag = 32;

    NodeId node = 100;
    std::vector<NodeId> whiteList(1, node);

    screenManagerImpl->currentVirtualScreenNum_ = 64;
    auto res = screenManagerImpl->CreateVirtualScreen(
        name, width1, height1, surface, mirrorId, flag, whiteList);
    EXPECT_EQ(INVALID_SCREEN_ID, res);

    screenManagerImpl->currentVirtualScreenNum_ = 2;
    res = screenManagerImpl->CreateVirtualScreen(
        name, width1, height1, surface, mirrorId, flag, whiteList);
    EXPECT_EQ(INVALID_SCREEN_ID, res);

    uint32_t width2 = 100;
    uint32_t height2 = 68000;
    res = screenManagerImpl->CreateVirtualScreen(
        name, width2, height2, surface, mirrorId, flag, whiteList);
    EXPECT_EQ(INVALID_SCREEN_ID, res);
}

/*
 * @tc.name: GetScreenCapability002
 * @tc.desc: Test GetScreenCapability002
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenCapability002, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    screenManagerImpl->screens_.clear();
    ScreenId id = 100;
    auto res = screenManagerImpl->RSScreenManager::GetScreenCapability(id);
    EXPECT_EQ(0, res.GetPhyWidth());

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    res = screenManagerImpl->RSScreenManager::GetScreenCapability(id);
    EXPECT_EQ(0, res.GetPhyHeight());
}

/*
 * @tc.name: HandleDefaultScreenDisConnected
 * @tc.desc: Test HandleDefaultScreenDisConnected
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, HandleDefaultScreenDisConnected, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    screenManagerImpl->screens_.clear();
    screenManagerImpl->RSScreenManager::HandleDefaultScreenDisConnected();

    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    screenManagerImpl->RSScreenManager::HandleDefaultScreenDisConnected();

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(200), nullptr);
    rsScreen0->isVirtual_ = false;
    screenManagerImpl->screens_[100] = rsScreen0;
    screenManagerImpl->RSScreenManager::HandleDefaultScreenDisConnected();
    EXPECT_EQ(screenManagerImpl->defaultScreenId_, 100);

    rsScreen0->isVirtual_ = true;
    screenManagerImpl->RSScreenManager::HandleDefaultScreenDisConnected();
    EXPECT_EQ(screenManagerImpl->defaultScreenId_, 100);
}

/*
 * @tc.name: RegSetScreenVsyncEnabledCallbackForMainThread
 * @tc.desc: Test RegSetScreenVsyncEnabledCallbackForMainThread
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, RegSetScreenVsyncEnabledCallbackForMainThread, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    ScreenId id = 100;
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    screenManagerImpl->RSScreenManager::RegSetScreenVsyncEnabledCallbackForMainThread(id);


    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(200), nullptr);
    screenManagerImpl->screens_[100] = rsScreen0;
    screenManagerImpl->RSScreenManager::RegSetScreenVsyncEnabledCallbackForMainThread(id);
    auto sampler = CreateVSyncSampler();
    OHOS::Rosen::impl::VSyncSampler& implSampler =
        static_cast<OHOS::Rosen::impl::VSyncSampler&>(*sampler);
    EXPECT_EQ(implSampler.vsyncEnabledScreenId_, 100);
}

/*
 * @tc.name: ProcessScreenDisConnected
 * @tc.desc: Test ProcessScreenDisConnected
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenDisConnected, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    uint32_t id = 100;
    std::shared_ptr<HdiOutput> output = std::make_shared<HdiOutput>(id);
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    screenManagerImpl->defaultScreenId_.store(static_cast<ScreenId>(id));
    screenManagerImpl->RSScreenManager::ProcessScreenDisConnected(output);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    screenManagerImpl->screens_.insert(std::make_pair(100, rsScreen0));
    screenManagerImpl->defaultScreenId_.store(static_cast<ScreenId>(1000));
    screenManagerImpl->RSScreenManager::ProcessScreenDisConnected(output);
    EXPECT_EQ(1, screenManagerImpl->screens_.size());
}

/*
 * @tc.name: ProcessScreenConnected
 * @tc.desc: Test ProcessScreenConnected
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenConnected, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    uint32_t id = 100;
    std::shared_ptr<HdiOutput> output = std::make_shared<HdiOutput>(id);
    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    screenManagerImpl->isFoldScreenFlag_ = false;
    screenManagerImpl->RSScreenManager::ProcessScreenConnected(output);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    screenManagerImpl->screens_[100] = rsScreen0;
    screenManagerImpl->isFoldScreenFlag_ = true;
    screenManagerImpl->RSScreenManager::ProcessScreenConnected(output);

    screenManagerImpl->screens_.insert(std::make_pair(200, rsScreen0));
    screenManagerImpl->RSScreenManager::ProcessScreenConnected(output);
    EXPECT_EQ(2, screenManagerImpl->screens_.size());
}

/*
 * @tc.name: OnScreenVBlankIdleEvent
 * @tc.desc: Test OnScreenVBlankIdleEvent
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, OnScreenVBlankIdleEvent, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    uint32_t id = 100;
    uint64_t ns = 1024;

    screenManagerImpl->screens_.clear();
    screenManagerImpl->screens_.insert(std::make_pair(100, nullptr));
    screenManagerImpl->RSScreenManager::OnScreenVBlankIdleEvent(id, ns);

    auto rsScreen0 =
        std::make_shared<impl::RSScreen>(100, true, HdiOutput::CreateHdiOutput(100), nullptr);
    screenManagerImpl->screens_[100] = rsScreen0;
    screenManagerImpl->RSScreenManager::OnScreenVBlankIdleEvent(id, ns);
    EXPECT_EQ(false, screenManagerImpl->screens_.empty());
}

/*
 * @tc.name: OnRefresh
 * @tc.desc: Test OnRefresh
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, OnRefresh, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    ScreenId sId = 100;
    auto screenManager = sptr<impl::RSScreenManager>::MakeSptr();
    screenManagerImpl->RSScreenManager::OnRefresh(sId, nullptr);
    screenManagerImpl->RSScreenManager::OnRefresh(sId, screenManager);
    EXPECT_NE(screenManager, nullptr);
}

/*
 * @tc.name: OnHwcDeadEvent
 * @tc.desc: Test OnHwcDeadEvent
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
 HWTEST_F(RSScreenManagerTest, OnHwcDeadEvent, TestSize.Level1)
 {
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    EXPECT_NE(screenManagerImpl, nullptr);

    ScreenId sId0 = 0;
    screenManagerImpl->screens_[sId0] = std::make_shared<impl::RSScreen>(sId0, false, nullptr, nullptr);
    ScreenId sId1 = 1;
    screenManagerImpl->screens_[sId1] = std::make_shared<impl::RSScreen>(sId1, true, nullptr, nullptr);
    screenManagerImpl->RSScreenManager::OnHwcDeadEvent();
    EXPECT_EQ(screenManagerImpl->screens_.size(), 1);
 }

/*
 * @tc.name: OnHotPlug
 * @tc.desc: Test OnHotPlug
 * @tc.type: FUNC
 * @tc.require: issueIC9IVH
 */
HWTEST_F(RSScreenManagerTest, OnHotPlug, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    ASSERT_NE(screenManagerImpl, nullptr);
    std::shared_ptr<HdiOutput> output = nullptr;
    // case1: output is nullptr
    screenManagerImpl->RSScreenManager::OnHotPlug(output, true, nullptr);
    // case2: data is not nullptr
    constexpr ScreenId mockScreenId = 100;
    output = HdiOutput::CreateHdiOutput(mockScreenId);
    screenManagerImpl->RSScreenManager::OnHotPlug(output, true, screenManagerImpl);
    // case3: data is nullptr && screenManager is nullptr
    // To confirm GetInstance() has been invoked at least one time;
    OHOS::Rosen::impl::RSScreenManager::GetInstance();
    sptr<OHOS::Rosen::RSScreenManager> instance = OHOS::Rosen::impl::RSScreenManager::instance_;
    OHOS::Rosen::impl::RSScreenManager::instance_ = nullptr;
    ASSERT_NE(nullptr, instance);
    ASSERT_EQ(nullptr, OHOS::Rosen::impl::RSScreenManager::instance_);
    screenManagerImpl->RSScreenManager::OnHotPlug(output, true, nullptr);
    std::swap(OHOS::Rosen::impl::RSScreenManager::instance_, instance);
    ASSERT_NE(nullptr, OHOS::Rosen::impl::RSScreenManager::instance_);
}

/*
 * @tc.name: OnHotPlugEvent
 * @tc.desc: Test OnHotPlugEvent
 * @tc.type: FUNC
 * @tc.require: issueIC9IVH
 */
HWTEST_F(RSScreenManagerTest, OnHotPlugEvent, TestSize.Level1)
{
    auto screenManagerImpl = sptr<impl::RSScreenManager>::MakeSptr();
    ASSERT_NE(screenManagerImpl, nullptr);
    constexpr ScreenId mockScreenId = 100;
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(mockScreenId);
    // case1: add FirstEvent
    screenManagerImpl->RSScreenManager::OnHotPlugEvent(output, true);
    // case2: Same screenId event cover
    screenManagerImpl->RSScreenManager::OnHotPlugEvent(output, true);
}

/*
 * @tc.name: ProcessVSyncScreenIdWhilePowerStatusChangedTest
 * @tc.desc: Test ProcessVSyncScreenIdWhilePowerStatusChanged
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, ProcessVSyncScreenIdWhilePowerStatusChangedTest, TestSize.Level1)
{
    auto sampler = CreateVSyncSampler();
    VSyncSampler::SetScreenVsyncEnabledCallback cb = [](uint64_t screenId, bool enabled) {};
    sampler->RegSetScreenVsyncEnabledCallback(cb);
    auto screenManager = CreateOrGetScreenManager();
    static_cast<impl::RSScreenManager*>(screenManager.GetRefPtr())->ProcessVSyncScreenIdWhilePowerStatusChanged(
        0, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(static_cast<impl::VSyncSampler*>(sampler.GetRefPtr())->hardwareVSyncStatus_, false);
}
} // namespace OHOS::Rosen