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
#include "hgm_core.h"
#include "limit_number.h"
#include <parameter.h>
#include <parameters.h>
#include "param/sys_param.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "rs_screen_manager.h"
#include "rs_screen.h"
#include "hdi_output.h"
#include "transaction/rs_interfaces.h"
#include "mock_hdi_device.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "rs_render_composer_manager.h"

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
    static constexpr uint32_t SLEEP_TIME_FOR_BACKGROUND = 1000000; // 1000ms
    static constexpr uint32_t LIGHT_LEVEL = 1;
    static constexpr uint64_t SCREEN_ID = 10;
    static constexpr uint64_t HGM_SCREEN_ID = 1031;
    static inline ScreenId mockScreenId_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
    static inline sptr<RSScreenManager> screenManager_;

    class RSIScreenChangeCallbackConfig : public RSIScreenChangeCallback {
    public:
        RSIScreenChangeCallbackConfig() = default;
        ~RSIScreenChangeCallbackConfig() = default;
        void OnScreenChanged(ScreenId id, ScreenEvent event, ScreenChangeReason reason = ScreenChangeReason::DEFAULT,
                             sptr<IRemoteObject> obj = nullptr) override {}
        sptr<IRemoteObject> AsObject() override
        {
            return nullptr;
        }
    };

    class RSIScreenSwitchingNotifyCallbackConfig : public RSIScreenSwitchingNotifyCallback {
    public:
        RSIScreenSwitchingNotifyCallbackConfig() = default;
        ~RSIScreenSwitchingNotifyCallbackConfig() = default;
        void OnScreenSwitchingNotify(bool status) override {}
        sptr<IRemoteObject> AsObject() override
        {
            return nullptr;
        }
    };
};

void RSScreenManagerTest::SetUpTestCase()
{
    mockScreenId_ = 0xFFFF;
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    screenManager_ = sptr<RSScreenManager>::MakeSptr();
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    screenManager_->Init(handler);
    EXPECT_CALL(*hdiDeviceMock_, SetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SaveArg<1>(&Mock::HdiDeviceMock::powerStatusMock_), testing::Return(0)));
    EXPECT_CALL(*hdiDeviceMock_, GetScreenPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::powerStatusMock_)), testing::Return(0)));
    EXPECT_CALL(*hdiDeviceMock_, GetPanelPowerStatus(mockScreenId_, _)).WillRepeatedly(
        DoAll(SetArgReferee<1>(ByRef(Mock::HdiDeviceMock::panelPowerStatusMock_)), testing::Return(0)));
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}

void RSScreenManagerTest::TearDownTestCase()
{
    usleep(200);
}
void RSScreenManagerTest::SetUp() {}
void RSScreenManagerTest::TearDown() {}

/*
 * @tc.name: GetDefaultScreenId_001
 * @tc.desc: Test GetDefaultScreenId
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetDefaultScreenId_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    screenManager_->defaultScreenId_ = 0;
    ASSERT_NE(INVALID_SCREEN_ID, screenManager_->GetDefaultScreenId());
}

/*
 * @tc.name: CreateVirtualScreen_001
 * @tc.desc: Test CreateVirtualScreen
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, CreateVirtualScreen_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<NodeId> whiteList = {};

    auto id = screenManager_->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, whiteList);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<NodeId> whiteList = {1};

    auto id = screenManager_->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, whiteList);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);
    ASSERT_LT(0, static_cast<int32_t>(screenManager_->GetAllScreenIds().size()));
    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = 1000;
    EXPECT_EQ(screenManager_->SetVirtualScreenAutoRotation(screenId, true), StatusCode::SCREEN_NOT_FOUND);

    screenManager_->screens_[screenId] = std::make_shared<RSScreen>(0);
    EXPECT_EQ(screenManager_->SetVirtualScreenAutoRotation(screenId, true), StatusCode::INVALID_ARGUMENTS);
}

/**
 * @tc.name: GetVirtualScreenAutoRotationTest
 * @tc.desc: Test GetVirtualScreenAutoRotation
 * @tc.type: FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSScreenManagerTest, GetVirtualScreenAutoRotationTest, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = 1000;
    EXPECT_FALSE(screenManager_->GetVirtualScreenAutoRotation(screenId));

    screenManager_->screens_[screenId] = std::make_shared<RSScreen>(0);
    screenManager_->GetVirtualScreenAutoRotation(screenId);
}

/*
 * @tc.name: SetVirtualScreenSurface_001
 * @tc.desc: Test SetVirtualScreenSurface
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSurface_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto id = screenManager_->CreateVirtualScreen(name, width, height, nullptr, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager_->SetVirtualScreenSurface(id, psurface)));
    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(screenManager_, nullptr);

    ScreenId id = mockScreenId_;
    auto rsScreen = std::make_shared<RSScreen>(id);
    ASSERT_NE(rsScreen, nullptr);

    uint32_t width = 1920;
    uint32_t height = 1080;
    auto result = screenManager_->SetPhysicalScreenResolution(id, width, height);
    ASSERT_EQ(result, SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetDualScreenState_001
 * @tc.desc: Test SetDualScreenState with invalid screen id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, SetDualScreenState_001, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);

    ScreenId id = INVALID_SCREEN_ID;
    int32_t ret = screenManager_->SetDualScreenState(id, DualScreenStatus::DUAL_SCREEN_ENTER);
    EXPECT_EQ(ret, static_cast<int32_t>(StatusCode::SCREEN_NOT_FOUND));
}

/*
 * @tc.name: SetDualScreenState_002
 * @tc.desc: Test SetDualScreenState with valid screen id
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, SetDualScreenState_002, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);

    // mock HDI device
    ScreenId id = mockScreenId_;
    auto rsScreen = std::make_shared<RSScreen>(id);
    ASSERT_NE(rsScreen, nullptr);
    ASSERT_NE(rsScreen->hdiScreen_, nullptr);
    ASSERT_NE(hdiDeviceMock_, nullptr);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;

    // mount rsScreen to screen manager
    screenManager_->screens_[mockScreenId_] = rsScreen;
    EXPECT_CALL(*hdiDeviceMock_, SetDisplayProperty(id, _, _)).WillOnce(testing::Return(0));
    int32_t ret = screenManager_->SetDualScreenState(id, DualScreenStatus::DUAL_SCREEN_ENTER);
    EXPECT_EQ(ret, static_cast<int32_t>(StatusCode::SUCCESS));

    // unmount rsScreen
    screenManager_->screens_.erase(mockScreenId_);
}

/*
 * @tc.name: SetVirtualScreenResolution_001
 * @tc.desc: Test SetVirtualScreenResolution
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenResolution_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    width = 960;
    height = 640;

    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager_->SetVirtualScreenResolution(id, width, height)));
    screenManager_->RemoveVirtualScreen(id);
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
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t width = static_cast<uint32_t>(100);
    uint32_t height = static_cast<uint32_t>(100);
    auto result = screenManager_->SetVirtualScreenResolution(screenId, width, height);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 1344;
    uint32_t height = 2772;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    width = 1088;
    height = 2224;

    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager_->SetVirtualScreenResolution(id, width, height)));
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager_->GetVirtualScreenResolution(id, virtualScreenResolution);
    ASSERT_EQ(virtualScreenResolution.GetVirtualScreenWidth(), width);
    ASSERT_EQ(virtualScreenResolution.GetVirtualScreenHeight(), height);
    screenManager_->RemoveVirtualScreen(id);
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
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t width = static_cast<uint32_t>(100);
    uint32_t height = static_cast<uint32_t>(100);
    auto result = screenManager_->SetVirtualScreenResolution(screenId, width, height);
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager_->GetVirtualScreenResolution(screenId, virtualScreenResolution);
    ASSERT_NE(virtualScreenResolution.GetVirtualScreenWidth(), width);
    ASSERT_NE(virtualScreenResolution.GetVirtualScreenHeight(), height);
    ASSERT_EQ(result, SCREEN_NOT_FOUND);
}

/*
 * @tc.name: GetScreenActiveRefreshRate_001
 * @tc.desc: Test GetScreenActiveRefreshRate
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenActiveRefreshRate_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    ScreenId id = 0;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(0);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    screenManager_->defaultScreenId_ = id;
    auto screen = screenManager_->GetScreen(id);
    ASSERT_NE(screen, nullptr);
    screenManager_->GetScreenActiveRefreshRate(id);

    id = INVALID_SCREEN_ID;
    screenManager_->GetScreenActiveRefreshRate(id);
}

/*
 * @tc.name: GetScreenBacklight_001
 * @tc.desc: Test GetScreenBacklight
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenBacklight_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(INVALID_BACKLIGHT_VALUE, screenManager_->GetScreenBacklight(id));

    screenManager_->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenSupportedColorGamuts_001
 * @tc.desc: Test GetScreenSupportedColorGamuts false.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedColorGamuts_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    std::vector<ScreenColorGamut> mode;
    ASSERT_EQ(SCREEN_NOT_FOUND, screenManager_->GetScreenSupportedColorGamuts(SCREEN_ID, mode));
}

/*
 * @tc.name: GetScreenSupportedColorGamuts_002
 * @tc.desc: Test GetScreenSupportedColorGamuts succeed.
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedColorGamuts_002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    std::vector<ScreenColorGamut> mode;
    ASSERT_EQ(SUCCESS, screenManager_->GetScreenSupportedColorGamuts(id, mode));
    ASSERT_LT(0, mode.size());

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    ScreenColorGamut mode;
    ASSERT_EQ(SCREEN_NOT_FOUND, screenManager_->GetScreenColorGamut(SCREEN_ID, mode));
}

/*
 * @tc.name: GetScreenColorGamut_002
 * @tc.desc: Test GetScreenColorGamut succeed
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenColorGamut_002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SCREEN_NOT_FOUND, screenManager_->SetScreenColorGamut(INVALID_SCREEN_ID, 1));
    ASSERT_EQ(SUCCESS, screenManager_->SetScreenColorGamut(id, 1));
    ScreenColorGamut mode;
    ASSERT_EQ(SUCCESS, screenManager_->GetScreenColorGamut(id, mode));
    ASSERT_EQ(COLOR_GAMUT_DCI_P3, mode);

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    RSScreenType type;
    ASSERT_EQ(StatusCode::SUCCESS, screenManager_->GetScreenType(id, type));

    screenManager_->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: GetScreenConnectionType_001
 * @tc.desc: Test GetScreenConnectionType false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, GetScreenConnectionType_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    ASSERT_EQ(ScreenConnectionType::INVALID_DISPLAY_CONNECTION_TYPE,
        screenManager_->GetScreenConnectionType(SCREEN_ID));
}

/*
 * @tc.name: GetScreenConnectionType_002
 * @tc.desc: Test GetScreenConnectionType succeed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, GetScreenConnectionType_002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    screenManager_->GetScreenConnectionType(id);

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);

    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto mirrorId = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, mirrorId);
    // timestamp is 10000000 ns
    screenManager_->SetScreenConstraint(mirrorId, 10000000, ScreenConstraintType::CONSTRAINT_NONE);
}

/*
 * @tc.name: SetScreenActiveMode_001
 * @tc.desc: Test SetScreenActiveMode
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, SetScreenActiveMode_001, testing::ext::TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);
    screenManager_->defaultScreenId_ = 0;
    ScreenId screenId = screenManager_->GetDefaultScreenId();
    ASSERT_NE(INVALID_SCREEN_ID, screenId);
    // [PLANNING]: ASSERT_NE
    RSScreenModeInfo screenModeInfo0;
    std::vector<RSScreenModeInfo> screenSupportedModes;
    screenSupportedModes = screenManager_->GetScreenSupportedModes(screenId);
    screenManager_->SetScreenActiveMode(screenId, screenSupportedModes.size() + 3);
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
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(screenId);
    screenManager_->MockHdiScreenConnected(rsScreen);

    Rect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(screenManager_->SetScreenActiveRect(screenId, activeRect), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: GetScreenActiveMode_001
 * @tc.desc: Test GetScreenActiveMode
 * @tc.type: FUNC
 * @tc.require: issueI5ZK2I
 */
HWTEST_F(RSScreenManagerTest, GetScreenActiveMode_001, testing::ext::TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    RSScreenModeInfo screenModeInfo1;
    screenManager_->GetScreenActiveMode(OHOS::Rosen::INVALID_SCREEN_ID, screenModeInfo1);
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
    ASSERT_NE(nullptr, screenManager_);
    std::vector<RSScreenModeInfo> screenSupportedModes1;
    screenSupportedModes1 = screenManager_->GetScreenSupportedModes(OHOS::Rosen::INVALID_SCREEN_ID);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    RSScreenCapability screenCapability = screenManager_->GetScreenCapability(id);
    ASSERT_EQ("virtualScreen01", screenCapability.GetName());

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    RSScreenHDRCapability screenHdrCapability;
    auto ret = screenManager_->GetScreenHDRCapability(OHOS::Rosen::INVALID_SCREEN_ID, screenHdrCapability);
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
    auto virtualScreenId =
        screenManager_->CreateVirtualScreen("virtualScreen01", 480, 320, nullptr, INVALID_SCREEN_ID, 0, {});
    RSScreenHDRCapability screenHdrCapability;
    auto ret = screenManager_->GetScreenHDRCapability(virtualScreenId, screenHdrCapability);
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
    ASSERT_NE(nullptr, screenManager_);
    RSScreenData screenData1;
    screenData1 = screenManager_->GetScreenData(OHOS::Rosen::INVALID_SCREEN_ID);
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
    ScreenId screenId = 1;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = screenId;
    auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
    rsScreen->property_.SetIsVirtual(true);
    screenManager_->screens_[screenId] = rsScreen;
    RSScreenData screenData = screenManager_->GetScreenData(screenId);
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
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = mockScreenId_;
    auto rsScreen = std::make_shared<RSScreen>(screenId);
    rsScreen->PhysicalScreenInit();
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager_->MockHdiScreenConnected(rsScreen);
    screenManager_->SetScreenPowerStatus(screenId, POWER_STATUS_ON_ADVANCED);
    RSScreenData screenData = screenManager_->GetScreenData(screenId);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string dumpString = "";
    std::string arg = "";
    std::string empty = "";
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(csurface, nullptr);
    auto id = screenManager_->CreateVirtualScreen("VirtualScreen", 200, 200, psurface, 0, 0);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager_->DisplayDump(dumpString);
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
    ASSERT_NE(nullptr, screenManager_);
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    int32_t setStatusCode = screenManager_->SetScreenGamutMap(SCREEN_ID, mode);
    ASSERT_EQ(setStatusCode, StatusCode::SCREEN_NOT_FOUND);
    int32_t getStatusCode = screenManager_->GetScreenGamutMap(SCREEN_ID, mode);
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
    ASSERT_NE(nullptr, screenManager_);
    auto virtualScreenId = screenManager_->CreateVirtualScreen(
        "virtualScreen01", 480, 320, nullptr, INVALID_SCREEN_ID, 0, {});
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);
    int32_t setStatusCode = screenManager_->SetScreenGamutMap(virtualScreenId, mode);
    ASSERT_EQ(setStatusCode, StatusCode::SUCCESS);
    int32_t getStatusCode = screenManager_->GetScreenGamutMap(virtualScreenId, mode);
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
    auto virtualScreenId = screenManager_->CreateVirtualScreen(
        "virtualScreen01", 480, 320, nullptr, INVALID_SCREEN_ID, 0, {});
    RSScreenCapability screenCapability;
    screenCapability.SetName("virtualScreen01");
    screenCapability.SetType(ScreenInterfaceType::DISP_INTF_LCD);
    screenCapability.SetPhyWidth(0);
    screenCapability.SetPhyHeight(0);
    screenCapability.SetSupportLayers(0);
    ASSERT_EQ(screenCapability.GetSupportLayers(), 0);
    screenCapability.SetVirtualDispCount(0);
    ASSERT_EQ(screenCapability.GetVirtualDispCount(), 0);
    screenCapability.SetSupportWriteBack(false);
    ASSERT_EQ(screenCapability.GetSupportWriteBack(), 0);
    std::vector<RSScreenProps> props = {};
    screenCapability.SetProps(props);
    screenCapability.GetProps();
    screenCapability = screenManager_->GetScreenCapability(virtualScreenId);
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
    ScreenId screenId = INVALID_SCREEN_ID;
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_EQ(screenManager_->SetVirtualScreenSurface(screenId, psurface), SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenSurface_003
 * @tc.desc: Test SetVirtualScreenSurface
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSurface_003, TestSize.Level1)
{
    ScreenId newId = 0;
    ASSERT_NE(screenManager_->SetVirtualScreenSurface(newId, nullptr), SUCCESS);
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = newId;
    auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->MockHdiScreenConnected(rsScreen);
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(screenManager_->SetVirtualScreenSurface(newId, psurface), SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenSurface_004
 * @tc.desc: Test SetVirtualScreenSurface while suface not unique
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSurface_004, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id1 = screenManager_->
        CreateVirtualScreen("virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, nullptr);
    ASSERT_NE(id1, INVALID_SCREEN_ID);
    auto id2 = screenManager_->
        CreateVirtualScreen("virtualScreen02", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface);
    ASSERT_NE(id2, INVALID_SCREEN_ID);

    ASSERT_EQ(static_cast<StatusCode>(screenManager_->SetVirtualScreenSurface(id1, psurface)), SURFACE_NOT_UNIQUE);
    screenManager_->RemoveVirtualScreen(id1);
    screenManager_->RemoveVirtualScreen(id2);
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
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager_->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: RemoveVirtualScreen_002
 * @tc.desc: Test RemoveVirtualScreen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreen_002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen("virtualScreen001", 480, 320, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager_->RemoveVirtualScreen(id);
}

/*
 * @tc.name: SetScreenPowerStatus_001
 * @tc.desc: Test SetScreenPowerStatus, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenPowerStatus_001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager_->screenPowerStatus_.count(screenId), 0);
}

/*
 * @tc.name: SetScreenPowerStatus_002
 * @tc.desc: Test SetScreenPowerStatus, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenPowerStatus_002, TestSize.Level1)
{
    ScreenId screenId = 1;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = screenId;
    auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->MockHdiScreenConnected(rsScreen);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager_->screenPowerStatus_[screenId], ScreenPowerStatus::POWER_STATUS_ON);
}

/*
 * @tc.name: SetScreenPowerStatus_003
 * @tc.desc: Test SetScreenPowerStatus, test POWER_STATUS_ON_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenPowerStatus_003, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = mockScreenId_;
    auto rsScreen = std::make_shared<RSScreen>(screenId);
    rsScreen->PhysicalScreenInit();
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager_->MockHdiScreenConnected(rsScreen);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    usleep(SLEEP_TIME_FOR_BACKGROUND);
    ASSERT_EQ(screenManager_->screenPowerStatus_[screenId], POWER_STATUS_ON);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    usleep(SLEEP_TIME_FOR_BACKGROUND);
    ASSERT_EQ(screenManager_->screenPowerStatus_[screenId], POWER_STATUS_ON_ADVANCED);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    usleep(SLEEP_TIME_FOR_BACKGROUND);
    ASSERT_EQ(screenManager_->screenPowerStatus_[screenId], POWER_STATUS_OFF);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED);
    usleep(SLEEP_TIME_FOR_BACKGROUND);
    ASSERT_EQ(screenManager_->screenPowerStatus_[screenId], POWER_STATUS_OFF_ADVANCED);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE);
    usleep(SLEEP_TIME_FOR_BACKGROUND);
    ASSERT_EQ(screenManager_->screenPowerStatus_[screenId], POWER_STATUS_DOZE);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE_SUSPEND);
    usleep(SLEEP_TIME_FOR_BACKGROUND);
    ASSERT_EQ(screenManager_->screenPowerStatus_[screenId], POWER_STATUS_DOZE_SUSPEND);
}

/*
 * @tc.name: GetScreenPowerStatus_001
 * @tc.desc: Test GetScreenPowerStatus, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatus_001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), INVALID_POWER_STATUS);
}

/*
 * @tc.name: GetScreenPowerStatus_002
 * @tc.desc: Test GetScreenPowerStatus, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatus_002, TestSize.Level1)
{
    ScreenId screenId = 1;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = screenId;
    auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->MockHdiScreenConnected(rsScreen);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), ScreenPowerStatus::INVALID_POWER_STATUS);
}

/*
 * @tc.name: GetScreenPowerStatus_003
 * @tc.desc: Test GetScreenPowerStatus, test POWER_STATUS_OFF_ADVANCED with mock HDI device
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenPowerStatus_003, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = mockScreenId_;
    auto rsScreen = std::make_shared<RSScreen>(screenId);
    rsScreen->PhysicalScreenInit();
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager_->MockHdiScreenConnected(rsScreen);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), POWER_STATUS_ON);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), POWER_STATUS_ON_ADVANCED);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), POWER_STATUS_OFF);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), POWER_STATUS_OFF_ADVANCED);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), POWER_STATUS_DOZE);
    screenManager_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_DOZE_SUSPEND);
    ASSERT_EQ(screenManager_->GetScreenPowerStatus(screenId), POWER_STATUS_DOZE_SUSPEND);
}

/*
 * @tc.name: GetPanelPowerStatus_001
 * @tc.desc: Test GetPanelPowerStatus, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, GetPanelPowerStatus_001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_EQ(screenManager_->GetPanelPowerStatus(screenId), PanelPowerStatus::INVALID_PANEL_POWER_STATUS);
}

/*
 * @tc.name: GetPanelPowerStatus_002
 * @tc.desc: Test GetPanelPowerStatus, with virtual screen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, GetPanelPowerStatus_002, TestSize.Level1)
{
    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(screenId);
    screenManager_->MockHdiScreenConnected(rsScreen);
    ASSERT_EQ(screenManager_->GetPanelPowerStatus(screenId), PanelPowerStatus::INVALID_PANEL_POWER_STATUS);
}

/*
 * @tc.name: GetPanelPowerStatus_003
 * @tc.desc: Test GetPanelPowerStatus, with mock HDI device
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, GetPanelPowerStatus_003, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    ScreenId screenId = mockScreenId_;
    auto rsScreen = std::make_shared<RSScreen>(screenId);
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager_->MockHdiScreenConnected(rsScreen);
    ASSERT_EQ(screenManager_->GetPanelPowerStatus(screenId), PanelPowerStatus::PANEL_POWER_STATUS_ON);
}

/*
 * @tc.name: SetScreenCorrection_001
 * @tc.desc: Test SetScreenCorrection, with INVALID_SCREEN_ID
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, SetScreenCorrection_001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_EQ(screenManager_->SetScreenCorrection(screenId, ScreenRotation::ROTATION_0), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetScreenCorrection_002
 * @tc.desc: Test SetScreenCorrection, with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI9AT9P
 */
HWTEST_F(RSScreenManagerTest, SetScreenCorrection_002, TestSize.Level1)
{
    ScreenId screenId = 1;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = screenId;
    auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->MockHdiScreenConnected(rsScreen);
    ASSERT_EQ(screenManager_->SetScreenCorrection(screenId, ScreenRotation::ROTATION_270), StatusCode::SUCCESS);
}

/*
 * @tc.name: GetScreenBacklight_002
 * @tc.desc: Test GetScreenBacklight
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, GetScreenBacklight_002, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    auto result = screenManager_->GetScreenBacklight(screenId);
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
    ASSERT_NE(screenManager_, nullptr);
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManager_->SetScreenBacklight(screenId, LIGHT_LEVEL);
}

/*
 * @tc.name: SetScreenBacklight_002
 * @tc.desc: Test SetScreenBacklight succeed.
 * @tc.type: FUNC
 * @tc.require: issueI9S56D
 */
HWTEST_F(RSScreenManagerTest, SetScreenBacklight_002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId screenId = mockScreenId_;
    auto rsScreen = std::make_shared<RSScreen>(screenId);
    rsScreen->PhysicalScreenInit();
    rsScreen->hdiScreen_->device_ = hdiDeviceMock_;
    screenManager_->MockHdiScreenConnected(rsScreen);

    screenManager_->SetScreenBacklight(screenId, LIGHT_LEVEL);
    auto ret = screenManager_->GetScreenBacklight(screenId);
    ASSERT_EQ(ret, LIGHT_LEVEL);

    screenManager_->RemoveVirtualScreen(screenId);
    sleep(1);
}

/*
 * @tc.name: SetScreenSkipFrameInterval_001
 * @tc.desc: Test SetScreenSkipFrameInterval
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetScreenSkipFrameInterval_001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t interval = static_cast<uint32_t>(1);
    auto result = screenManager_->SetScreenSkipFrameInterval(screenId, interval);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager_->SetPixelFormat(id, static_cast<GraphicPixelFormat>(20)));

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager_->SetPixelFormat(id, static_cast<GraphicPixelFormat>(20)));
    GraphicPixelFormat pixelFormat;
    ASSERT_EQ(SUCCESS, screenManager_->GetPixelFormat(id, pixelFormat));
    ASSERT_EQ(GRAPHIC_PIXEL_FMT_BGRA_8888, pixelFormat);

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    std::vector<ScreenHDRFormat> hdrFormats;
    ASSERT_EQ(SUCCESS, screenManager_->GetScreenSupportedHDRFormats(id, hdrFormats));
    ASSERT_LT(0, hdrFormats.size());

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager_->SetScreenHDRFormat(id, 0));
    ScreenHDRFormat hdrFormat;
    ASSERT_EQ(SUCCESS, screenManager_->GetScreenHDRFormat(id, hdrFormat));
    ASSERT_EQ(NOT_SUPPORT_HDR, hdrFormat);

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    ASSERT_EQ(SUCCESS, screenManager_->GetScreenSupportedColorSpaces(id, colorSpaces));
    ASSERT_LT(0, colorSpaces.size());

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(SUCCESS, screenManager_->SetScreenColorSpace(
        id, static_cast<GraphicCM_ColorSpaceType>(1 | (2 << 8) | (3 << 16) | (1 << 21))));
    GraphicCM_ColorSpaceType colorSpace;
    ASSERT_EQ(SUCCESS, screenManager_->GetScreenColorSpace(id, colorSpace));
    ASSERT_EQ(GRAPHIC_CM_SRGB_FULL, colorSpace);

    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);
    auto ret = screenManager_->ResizeVirtualScreen(OHOS::Rosen::INVALID_SCREEN_ID, VIRTUAL_SCREEN_WIDTH,
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen0";

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    uint32_t width = 500;
    uint32_t height = 300;
    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, 0, false, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    width = 0;
    height = 0;
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager_->ResizeVirtualScreen(id, width, height)));
    width = 100;
    height = 200;
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager_->ResizeVirtualScreen(id, width, height)));

    width = 10000;
    height = 9000;
    ASSERT_EQ(SUCCESS, static_cast<StatusCode>(screenManager_->ResizeVirtualScreen(id, width, height)));

    screenManager_->RemoveVirtualScreen(id);
    sleep(1);
}

/**
 * @tc.name: SetRogScreenResolution_001
 * @tc.desc: Test SetRogScreenResolution while screen's id doesn't match
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetRogScreenResolution_001, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->
        CreateVirtualScreen("virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(static_cast<StatusCode>(screenManager_->
        SetRogScreenResolution(INVALID_SCREEN_ID, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT)), SCREEN_NOT_FOUND);
    screenManager_->RemoveVirtualScreen(id);
    usleep(SLEEP_TIME_US);
}

/**
 * @tc.name: SetRogScreenResolution_002
 * @tc.desc: Test SetRogScreenResolution while the screen's id match
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetRogScreenResolution_002, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->
        CreateVirtualScreen("virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(static_cast<StatusCode>(screenManager_->
        SetRogScreenResolution(id, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT)), SUCCESS);
    screenManager_->RemoveVirtualScreen(id);
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
    ASSERT_NE(nullptr, screenManager_);

    ASSERT_EQ(static_cast<StatusCode>(
        screenManager_->SetVirtualMirrorScreenCanvasRotation(INVALID_SCREEN_ID, true)), false);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation_002
 * @tc.desc: Test SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSScreenManagerTest, SetVirtualMirrorScreenCanvasRotationn_002, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(
        "virtualScreen01", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    ASSERT_EQ(static_cast<StatusCode>(
        screenManager_->SetVirtualMirrorScreenCanvasRotation(id, true)), true);
}

/*
 * @tc.name: GetCanvasRotation_001
 * @tc.desc: Test GetCanvasRotation, input invalid id, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, GetCanvasRotation_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_FALSE(screenManager_->GetCanvasRotation(screenId));
}

/*
 * @tc.name: GetCanvasRotation_002
 * @tc.desc: Test GetCanvasRotation, input screens_ with nullptr, expect false.
 * @tc.type: FUNC
 * @tc.require: issueIAINL7
 */
HWTEST_F(RSScreenManagerTest, GetCanvasRotation_002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId screenId = SCREEN_ID;
    screenManager_->screens_[screenId] = nullptr;
    ASSERT_FALSE(screenManager_->GetCanvasRotation(screenId));
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode_001
 * @tc.desc: Test SetVirtualMirrorScreenScaleMode false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, SetVirtualMirrorScreenScaleMode_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    auto scaleMode = screenManager_->SetVirtualMirrorScreenScaleMode(SCREEN_ID, ScreenScaleMode::INVALID_MODE);
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
    ASSERT_NE(nullptr, screenManager_);
    std::string name = "virtualScreen01";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto id = screenManager_->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, 0, {});
    ASSERT_NE(INVALID_SCREEN_ID, id);

    auto scaleMode = screenManager_->SetVirtualMirrorScreenScaleMode(id, ScreenScaleMode::FILL_MODE);
    ASSERT_EQ(true, scaleMode);

    screenManager_->RemoveVirtualScreen(id);
    sleep(1);
}

/*
 * @tc.name: DisablePowerOffRenderControl001
 * @tc.desc: Test disable power off render, input invalid screen id.
 * @tc.type: FUNC
 * @tc.require: issueICON9P
 */
HWTEST_F(RSScreenManagerTest, DisablePowerOffRenderControl001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = INVALID_SCREEN_ID;
    screenManager_->DisablePowerOffRenderControl(id);
}

/*
 * @tc.name: DisablePowerOffRenderControl002
 * @tc.desc: Test disable power off render, input valid screen id.
 * @tc.type: FUNC
 * @tc.require: issueICON9P
 */
HWTEST_F(RSScreenManagerTest, DisablePowerOffRenderControl002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->DisablePowerOffRenderControl(id);
}

/*
 * @tc.name: SetVirtualScreenBlackList001
 * @tc.desc: Test SetVirtualScreenBlackList, input invalid id
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenBlackList002
 * @tc.desc: Test SetVirtualScreenBlackList, input id  which not found in screens_
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    screenManager_->screens_.erase(id);
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetVirtualScreenBlackList003
 * @tc.desc: Test SetVirtualScreenBlackList, input id in screens_, and screens_[id] = null,
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList003, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    screenManager_->screens_[id] = nullptr;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
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
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    ScreenId mainId = id;
    screenManager_->defaultScreenId_ = mainId;

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManager_->SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
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
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManager_->defaultScreenId_ = mainId;
    screenManager_->screens_.erase(mainId);
    screenManager_->screens_.clear();

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    std::vector<uint64_t> blackList = {};

    ASSERT_NE(screenManager_->SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
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
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManager_->defaultScreenId_ = mainId;
    screenManager_->screens_.clear();

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->screens_[mainId] = nullptr;
    std::vector<uint64_t> blackList = {};

    ASSERT_NE(screenManager_->SetVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: AddVirtualScreenBlackList001
 * @tc.desc: Test AddVirtualScreenBlackList, with multi-id
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);

    for (auto nodeId = 0; nodeId <= MAX_SPECIAL_LAYER_NUM + 1; nodeId++) {
        blackList.push_back(nodeId);
    }
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(id, blackList), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: AddVirtualScreenBlackList002
 * @tc.desc: Test AddVirtualScreenBlackList, input id in screens_, and screens_[id] is multiple
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    ScreenId id = 1;
    screenManager_->screens_[id] = nullptr;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);

    ScreenId mainId = id;
    screenManager_->defaultScreenId_ = mainId;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: AddVirtualScreenBlackList003
 * @tc.desc: Test AddVirtualScreenBlackList, id!=mainId, screens_[id]!=nullptr, screens_[mainId]==nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList003, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManager_->defaultScreenId_ = mainId;
    std::vector<uint64_t> blackList = {};

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    ASSERT_NE(screenManager_->AddVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);

    screenManager_->screens_[mainId] = nullptr;
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(mainId, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: AddVirtualScreenBlackList004
 * @tc.desc: Test AddVirtualScreenBlackList, id!=mainId, screens_[id]!=nullptr, screens_[mainId]!=nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList004, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManager_->defaultScreenId_ = mainId;

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    cfgVirtual.id = mainId;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);

    for (auto nodeId = 0; nodeId <= MAX_SPECIAL_LAYER_NUM + 1; nodeId++) {
        blackList.push_back(nodeId);
    }
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(id, blackList), StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: AddVirtualScreenBlackList005
 * @tc.desc: Test AddVirtualScreenBlackList and RemoveVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require: issueICHZO3
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList005, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 10;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(0);
    std::vector<uint64_t> blackList1 = {1, 2, 3, 4};
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(id, blackList1), StatusCode::SUCCESS);
    ASSERT_EQ(screenManager_->RemoveVirtualScreenBlackList(id, blackList1), StatusCode::SUCCESS);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList001
 * @tc.desc: Test RemoveVirtualScreenBlackList, with multi-id
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList002
 * @tc.desc: Test RemoveVirtualScreenBlackList, input id in screens_, and screens_[id] is multiple
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    screenManager_->screens_[id] = nullptr;
    std::vector<uint64_t> blackList = {};
    ASSERT_EQ(screenManager_->RemoveVirtualScreenBlackList(id, blackList), StatusCode::SCREEN_NOT_FOUND);

    ScreenId mainId = id;
    screenManager_->defaultScreenId_ = mainId;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    ASSERT_EQ(screenManager_->RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList003
 * @tc.desc: Test RemoveVirtualScreenBlackList, id!=mainId, screens_[id]!=nullptr, screens_[mainId]==nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList003, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManager_->defaultScreenId_ = mainId;
    std::vector<uint64_t> blackList = {};

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    ASSERT_EQ(screenManager_->RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);

    screenManager_->screens_[mainId] = nullptr;
    ASSERT_EQ(screenManager_->RemoveVirtualScreenBlackList(mainId, blackList), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: RemoveVirtualScreenBlackList004
 * @tc.desc: Test RemoveVirtualScreenBlackList, id!=mainId, screens_[id]!=nullptr, screens_[mainId]!=nullptr
 * @tc.type: FUNC
 * @tc.require: issueIB2WOT
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenBlackList004, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManager_->defaultScreenId_ = mainId;

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[id], nullptr);
    cfgVirtual.id = mainId;
    screenManager_->screens_[mainId] = std::make_shared<RSScreen>(cfgVirtual);
    ASSERT_NE(screenManager_->screens_[mainId], nullptr);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManager_->RemoveVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
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
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    ScreenId mainId = 0;
    screenManager_->defaultScreenId_ = mainId;

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    cfgVirtual.id = mainId;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    std::vector<uint64_t> blackList = {};

    ASSERT_EQ(screenManager_->SetVirtualScreenBlackList(id, blackList), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenBlackList008
 * @tc.desc: Test SetVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenBlackList008, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 10;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(0);
    std::vector<uint64_t> blackList1 = {1, 2, 3, 4};
    ASSERT_EQ(screenManager_->SetVirtualScreenBlackList(id, blackList1), StatusCode::SUCCESS);

    std::vector<uint64_t> blackList2 = {1, 2, 3};
    ASSERT_EQ(screenManager_->SetVirtualScreenBlackList(id, blackList2), StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList001
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with abnormal param
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSecurityExemptionList001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    auto ret = screenManager_->SetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);

    ScreenId id = 11;  // screenId for test
    ret = screenManager_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    screenManager_->screens_[id] = nullptr;
    ret = screenManager_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    screenManager_->screens_[id] = std::make_shared<RSScreen>(0);
    ret = screenManager_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
    screenManager_->screens_.erase(id);
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList002
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList with normal param
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenSecurityExemptionList002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    std::vector<uint64_t> securityExemptionList = {1, 2};  // id for test
    auto screen = std::make_shared<RSScreen>(0);
    screenManager_->screens_[id] = screen;
    screen->property_.SetIsVirtual(true);
    auto ret = screenManager_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: SetMirrorScreenVisibleRect001
 * @tc.desc: Test SetMirrorScreenVisibleRect with abnormal params.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenManagerTest, SetMirrorScreenVisibleRect001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    uint32_t width = 720;  // width value for test
    uint32_t height = 1280;  // height value for test
    Rect rect = {0, 0, width, height};
    int32_t ret = screenManager_->SetMirrorScreenVisibleRect(INVALID_SCREEN_ID, rect);
    ASSERT_EQ(ret, StatusCode::INVALID_ARGUMENTS);

    screenManager_->screens_.erase(id);
    ret = screenManager_->SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    screenManager_->screens_[id] = nullptr;
    ret = screenManager_->SetMirrorScreenVisibleRect(id, rect);
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
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    uint32_t width = 720;  // width value for test
    uint32_t height = 1280;  // height value for test
    Rect rect = {0, 0, width, height};
    auto screen = std::make_shared<RSScreen>(0);
    screenManager_->screens_[id] = screen;
    screen->property_.SetIsVirtual(true);
    int32_t ret = screenManager_->SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SUCCESS);

    rect = {-10, -10, -100, -100};  // test rect value
    ret = screenManager_->SetMirrorScreenVisibleRect(id, rect);
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
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->SetMirrorScreenVisibleRect(id, Rect());
}

/*
 * @tc.name: SetMirrorScreenVisibleRect004
 * @tc.desc: Test SetMirrorScreenVisibleRect with normal params.
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSScreenManagerTest, SetMirrorScreenVisibleRect004, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    uint32_t width = 720;  // width value for test
    uint32_t height = 1280;  // height value for test
    Rect rect = {0, 0, width, height};
    auto screen = std::make_shared<RSScreen>(0);
    screen->property_.SetIsVirtual(true);
    screenManager_->screens_[id] = screen;
    int32_t ret = screenManager_->SetMirrorScreenVisibleRect(id, rect);
    ASSERT_EQ(ret, StatusCode::SUCCESS);

    rect = {-10, -10, -100, -100};  // test rect value
    ret = screenManager_->SetMirrorScreenVisibleRect(id, rect, true);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow001
 * @tc.desc: Test SetCastScreenEnableSkipWindow, input id not in keys of screens_
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetCastScreenEnableSkipWindow001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = INVALID_SCREEN_ID;
    bool enable = false;
    ASSERT_EQ(screenManager_->SetCastScreenEnableSkipWindow(id, enable), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow002
 * @tc.desc: Test SetCastScreenEnableSkipWindow, input id in keys of screens_, and screens_[id] = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetCastScreenEnableSkipWindow002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    screenManager_->screens_[id] = nullptr;
    bool enable = false;
    ASSERT_EQ(screenManager_->SetCastScreenEnableSkipWindow(id, enable), StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow003
 * @tc.desc: Test SetCastScreenEnableSkipWindow, input id in keys of screens_, and screens_[id] != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIMIW
 */
HWTEST_F(RSScreenManagerTest, SetCastScreenEnableSkipWindow003, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId id = 1;
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = id;
    screenManager_->screens_[id] = std::make_shared<RSScreen>(cfgVirtual);
    bool enable = false;
    ASSERT_EQ(screenManager_->SetCastScreenEnableSkipWindow(id, enable), StatusCode::SUCCESS);
}

/*
 * @tc.name: GenerateVirtualScreenId_001
 * @tc.desc: Test GenerateVirtualScreenId when freeVirtualScreenIds_ is empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, GenerateVirtualScreenId_001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    screenManager_->freeVirtualScreenIds_ = std::queue<ScreenId>();
    ASSERT_EQ(screenManager_->freeVirtualScreenIds_.size(), 0);

    screenManager_->virtualScreenCount_ = 0;
    auto screenId = (static_cast<ScreenId>(screenManager_->virtualScreenCount_) << 32) | 0xffffffffu;
    ASSERT_EQ(screenManager_->GenerateVirtualScreenId(), screenId);
}

/*
 * @tc.name: GenerateVirtualScreenId_002
 * @tc.desc: Test GenerateVirtualScreenId when freeVirtualScreenIds_ is not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAJ6B9
 */
HWTEST_F(RSScreenManagerTest, GenerateVirtualScreenId_002, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);

    screenManager_->freeVirtualScreenIds_.push(SCREEN_ID);
    auto screenId = screenManager_->freeVirtualScreenIds_.front();
    ASSERT_EQ(screenManager_->GenerateVirtualScreenId(), screenId);
}

/*
 * @tc.name: SetScreenSecurityMask001
 * @tc.desc: Test SetScreenSecurityMask001 with nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenSecurityMask001, TestSize.Level1)
{
    ASSERT_NE(nullptr, screenManager_);
    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto virtualScreenId = screenManager_->CreateVirtualScreen(
        "virtualScreen01", sizeWidth, sizeHeight, nullptr, INVALID_SCREEN_ID, 0, {});
    auto ret = screenManager_->SetScreenSecurityMask(virtualScreenId, nullptr);
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
    ASSERT_NE(nullptr, screenManager_);
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    constexpr uint32_t sizeWidth = 720;
    constexpr uint32_t sizeHeight = 1280;
    auto virtualScreenId = screenManager_->CreateVirtualScreen(
        "VirtualScreen01", sizeWidth, sizeHeight, psurface, INVALID_SCREEN_ID, 0, {});
    const uint32_t color[1] = { 0x6f0000ff };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(color, colorLength, opts);
    auto ret = screenManager_->SetScreenSecurityMask(virtualScreenId, std::move(pixelMap));
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/*
 * @tc.name: SetVirtualScreenStatus
 * @tc.desc: Test SetVirtualScreenStatus
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenStatus, TestSize.Level1)
{
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);
    rsScreen0->property_.SetIsVirtual(true);
    VirtualScreenStatus status = VirtualScreenStatus::VIRTUAL_SCREEN_PLAY;

    screenManager_->screens_.clear();
    auto res = screenManager_->SetVirtualScreenStatus(100, status);
    EXPECT_EQ(false, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    res = screenManager_->SetVirtualScreenStatus(100, status);
    EXPECT_EQ(false, res);

    screenManager_->screens_[100] = rsScreen0;
    res = screenManager_->SetVirtualScreenStatus(100, status);
    EXPECT_EQ(true, res);
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys
 * @tc.desc: Test GetScreenSupportedMetaDataKeys
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, GetScreenSupportedMetaDataKeys, TestSize.Level1)
{
    EXPECT_NE(nullptr, screenManager_);
    ScreenHDRMetadataKey dataKey = ScreenHDRMetadataKey::MATAKEY_HDR10_PLUS;
    std::vector<ScreenHDRMetadataKey> metaDataKey(1, dataKey);
    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 200;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);
    screenManager_->screens_.insert(std::make_pair(200, rsScreen0));

    cfgVirtual.id = 300;
    auto rsScreen1 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen1);
    screenManager_->screens_.insert(std::make_pair(300, rsScreen1));

    auto res = screenManager_->GetScreenSupportedMetaDataKeys(100, metaDataKey);
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
    EXPECT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;
    auto res = screenManager_->GetScreenColorSpace(100, colorSpace);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->GetScreenColorSpace(100, colorSpace);
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
    EXPECT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;
    std::vector<GraphicCM_ColorSpaceType> spaceType(1, colorSpace);
    auto res = screenManager_->GetScreenSupportedColorSpaces(100, spaceType);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->GetScreenSupportedColorSpaces(100, spaceType);
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
    EXPECT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();
    auto res = screenManager_->SetScreenHDRFormat(100, 10);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->SetScreenHDRFormat(100, 10);
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
    EXPECT_NE(nullptr, screenManager_);
    ScreenHDRFormat format = {ScreenHDRFormat::IMAGE_HDR_VIVID_DUAL};
    screenManager_->screens_.clear();
    auto res = screenManager_->GetScreenHDRFormat(100, format);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->GetScreenHDRFormat(100, format);
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
    EXPECT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    ScreenHDRFormat format = { ScreenHDRFormat::IMAGE_HDR_VIVID_DUAL };
    std::vector<ScreenHDRFormat> screenFormat(5, format);
    auto res = screenManager_->GetScreenSupportedHDRFormats(100, screenFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->GetScreenSupportedHDRFormats(100, screenFormat);
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
    EXPECT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    GraphicPixelFormat pixelFormat = { GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BGRA_8888 };
    auto res = screenManager_->SetPixelFormat(100, pixelFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->SetPixelFormat(100, pixelFormat);
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
    EXPECT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    GraphicPixelFormat pixelFormat = { GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BGRA_8888 };
    auto res = screenManager_->GetPixelFormat(100, pixelFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->GetPixelFormat(100, pixelFormat);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);
}

/*
 * @tc.name: SetScreenColorSpace
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenColorSpace, TestSize.Level1)
{
    EXPECT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;
    auto res = screenManager_->SetScreenColorSpace(100, colorSpace);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));

    res = screenManager_->SetScreenColorSpace(100, colorSpace);
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
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);

    screenManager_->screens_.clear();

    uint8_t outPort = 1;
    std::vector<uint8_t> edidData(5);

    int res = screenManager_->GetDisplayIdentificationData(100, outPort, edidData);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    res = screenManager_->GetDisplayIdentificationData(100, outPort, edidData);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    screenManager_->screens_[100] = rsScreen0;
    res = screenManager_->GetDisplayIdentificationData(100, outPort, edidData);
    EXPECT_EQ(HDI_ERROR, res);
}

/*
 * @tc.name: SetVirtualScreenRefreshRate
 * @tc.desc: Test SetVirtualScreenRefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenRefreshRate, TestSize.Level1)
{
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);

    screenManager_->screens_.clear();

    uint32_t maxRefreshRate1 = 0;
    uint32_t actualRefreshRate = 8;

    int res = screenManager_->SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    res = screenManager_->SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_[100] = rsScreen0;
    rsScreen0->property_.SetIsVirtual(false);
    res = screenManager_->SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    rsScreen0->property_.SetIsVirtual(true);
    res = screenManager_->SetVirtualScreenRefreshRate(100, maxRefreshRate1, actualRefreshRate);
    EXPECT_EQ(StatusCode::INVALID_ARGUMENTS, res);

    uint32_t maxRefreshRate2 = 6;
    res = screenManager_->SetVirtualScreenRefreshRate(100, maxRefreshRate2, actualRefreshRate);
    EXPECT_EQ(StatusCode::SUCCESS, res);

    uint32_t maxRefreshRate3 = 200;
    res = screenManager_->SetVirtualScreenRefreshRate(100, maxRefreshRate3, actualRefreshRate);
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
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);

    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, rsScreen0));

    auto res = screenManager_->SetScreenSkipFrameInterval(100, 0);
    EXPECT_EQ(INVALID_ARGUMENTS, res);

    res = screenManager_->SetScreenSkipFrameInterval(100, 1800);
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
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);

    screenManager_->screens_.clear();

    ScreenHDRMetadataKey key = ScreenHDRMetadataKey::MATAKEY_BLUE_PRIMARY_X;
    std::vector<ScreenHDRMetadataKey> dataKey(5, key);
    auto res = screenManager_->GetScreenSupportedMetaDataKeys(100, dataKey);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    res = screenManager_->GetScreenSupportedMetaDataKeys(100, dataKey);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    rsScreen0->property_.SetIsVirtual(true);
    screenManager_->screens_[100] = rsScreen0;
    res = screenManager_->GetScreenSupportedMetaDataKeys(100, dataKey);
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
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);

    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, rsScreen0));

    rsScreen0->property_.SetIsVirtual(true);
    ScreenConstraintType type = ScreenConstraintType::CONSTRAINT_ABSOLUTE;
    auto res = screenManager_->SetScreenConstraint(100, 1000, type);
    EXPECT_EQ(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: SetScreenOffset001
 * @tc.desc: Test SetScreenOffset001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, SetScreenOffset001, TestSize.Level1)
{
    EXPECT_NE(screenManager_, nullptr);
    screenManager_->SetScreenOffset(0, 0, 0);
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, rsScreen0));
    screenManager_->SetScreenOffset(100, 0, 0);
}

/*
 * @tc.name: SetVirtualScreenResolution003
 * @tc.desc: Test SetVirtualScreenResolution
 * @tc.type: FUNC
 * @tc.require: issueI7AABN
 */
HWTEST_F(RSScreenManagerTest, SetVirtualScreenResolution003, TestSize.Level1)
{
    EXPECT_NE(nullptr, screenManager_);
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t width1 = 68800;
    uint32_t height1 = 100;
    auto result = screenManager_->SetVirtualScreenResolution(screenId, width1, height1);
    EXPECT_EQ(result, INVALID_ARGUMENTS);

    uint32_t width2 = 100;
    uint32_t height2 = 68800;
    result = screenManager_->SetVirtualScreenResolution(screenId, width2, height2);
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
    EXPECT_NE(screenManager_, nullptr);

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);
    rsScreen0->property_.SetIsVirtual(true);
    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, rsScreen0));

    ScreenId screenId = 100;
    uint32_t width = 100;
    uint32_t height = 100;
    auto res = screenManager_->SetPhysicalScreenResolution(screenId, width, height);
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
    EXPECT_NE(screenManager_, nullptr);

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 100;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    EXPECT_NE(nullptr, rsScreen0);
    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    ScreenId screenId = 100;
    Rect rect = {1, 2, 3, 4};
    auto res = screenManager_->SetScreenActiveRect(screenId, rect);
    EXPECT_EQ(StatusCode::SCREEN_NOT_FOUND, res);

    rsScreen0->property_.SetIsVirtual(true);
    screenManager_->screens_[100] = rsScreen0;
    res = screenManager_->SetScreenActiveRect(screenId, rect);
    EXPECT_NE(StatusCode::SUCCESS, res);
}

/*
 * @tc.name: SetScreenSecurityMask003
 * @tc.desc: Test SetScreenSecurityMask003
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, SetScreenSecurityMask003, TestSize.Level1)
{
    EXPECT_NE(screenManager_, nullptr);

    auto res = screenManager_->SetScreenSecurityMask(INVALID_SCREEN_ID, nullptr);
    EXPECT_EQ(INVALID_ARGUMENTS, res);

    screenManager_->screens_.clear();
    ScreenId id = 100;
    res = screenManager_->SetScreenSecurityMask(id, nullptr);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    res = screenManager_->SetScreenSecurityMask(id, nullptr);
    EXPECT_EQ(SCREEN_NOT_FOUND, res);

    auto rsScreen0 = std::make_shared<RSScreen>(100);
    EXPECT_NE(nullptr, rsScreen0);
    rsScreen0->property_.SetIsVirtual(true);
    screenManager_->screens_[100] =  rsScreen0;
    res = screenManager_->SetScreenSecurityMask(id, nullptr);
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
    EXPECT_NE(screenManager_, nullptr);

    ScreenId id = INVALID_SCREEN_ID;
    std::vector<uint64_t> black;
    black.emplace_back(1024);
    black.emplace_back(2048);

    auto res = screenManager_->RemoveVirtualScreenBlackList(id, black);
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
    EXPECT_NE(screenManager_, nullptr);

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

    screenManager_->currentVirtualScreenNum_ = 64;
    auto res = screenManager_->CreateVirtualScreen(
        name, width1, height1, surface, mirrorId, flag, whiteList);
    EXPECT_EQ(INVALID_SCREEN_ID, res);

    screenManager_->currentVirtualScreenNum_ = 2;
    res = screenManager_->CreateVirtualScreen(
        name, width1, height1, surface, mirrorId, flag, whiteList);
    EXPECT_EQ(INVALID_SCREEN_ID, res);

    uint32_t width2 = 100;
    uint32_t height2 = 68000;
    res = screenManager_->CreateVirtualScreen(
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
    EXPECT_NE(screenManager_, nullptr);

    screenManager_->screens_.clear();
    ScreenId id = 100;
    auto res = screenManager_->GetScreenCapability(id);
    EXPECT_EQ(0, res.GetPhyWidth());

    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    res = screenManager_->GetScreenCapability(id);
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
    EXPECT_NE(screenManager_, nullptr);

    screenManager_->screens_.clear();
    screenManager_->HandleDefaultScreenDisConnected();

    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    screenManager_->HandleDefaultScreenDisConnected();

    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = 200;
    auto rsScreen0 = std::make_shared<RSScreen>(cfgVirtual);
    rsScreen0->property_.SetIsVirtual(false);
    screenManager_->screens_[100] = rsScreen0;
    screenManager_->HandleDefaultScreenDisConnected();
    EXPECT_EQ(screenManager_->defaultScreenId_, 100);

    rsScreen0->property_.SetIsVirtual(true);
    screenManager_->HandleDefaultScreenDisConnected();
    EXPECT_EQ(screenManager_->defaultScreenId_, 100);
}

/*
 * @tc.name: ProcessScreenDisConnected
 * @tc.desc: Test ProcessScreenDisConnected
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenDisConnected, TestSize.Level1)
{
    EXPECT_NE(screenManager_, nullptr);

    uint32_t id = 100;
    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    screenManager_->defaultScreenId_.store(static_cast<ScreenId>(id));
    screenManager_->ProcessScreenDisConnected(id);

    auto rsScreen0 = std::make_shared<RSScreen>(100);
    screenManager_->screens_.insert(std::make_pair(100, rsScreen0));
    screenManager_->defaultScreenId_.store(static_cast<ScreenId>(1000));
    screenManager_->ProcessScreenDisConnected(id);
    EXPECT_EQ(1, screenManager_->screens_.size());
}

/*
 * @tc.name: ProcessScreenConnected
 * @tc.desc: Test ProcessScreenConnected
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenConnected, TestSize.Level1)
{
    EXPECT_NE(screenManager_, nullptr);

    uint32_t id = 100;
    screenManager_->screens_.clear();
    screenManager_->screens_.insert(std::make_pair(100, nullptr));
    screenManager_->isFoldScreenFlag_ = false;
    screenManager_->ProcessScreenConnected(id);

    auto rsScreen0 = std::make_shared<RSScreen>(100);
    screenManager_->screens_[100] = rsScreen0;
    screenManager_->isFoldScreenFlag_ = true;
    screenManager_->ProcessScreenConnected(id);

    screenManager_->screens_.insert(std::make_pair(200, rsScreen0));
    screenManager_->ProcessScreenConnected(id);
    EXPECT_EQ(2, screenManager_->screens_.size());
}

/*
 * @tc.name: ProcessScreenConnected01
 * @tc.desc: Test ProcessScreenConnected
 * @tc.type: FUNC
 * @tc.require: issue20799
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenConnected01, TestSize.Level1)
{
    EXPECT_NE(screenManager_, nullptr);

    uint32_t id = 0;
    screenManager_->screens_.clear();
    screenManager_->foldScreenIds_.clear();
    screenManager_->isFoldScreenFlag_ = true;
    screenManager_->ProcessScreenConnected(id);
    bool found = screenManager_->foldScreenIds_.find(id) != screenManager_->foldScreenIds_.end();
    ASSERT_FALSE(found);

    id = 1;
    screenManager_->ProcessScreenConnected(id);
    found = screenManager_->foldScreenIds_.find(id) != screenManager_->foldScreenIds_.end();
    ASSERT_FALSE(found);

    id = 3;
    screenManager_->ProcessScreenConnected(id);
    found = screenManager_->foldScreenIds_.find(id) != screenManager_->foldScreenIds_.end();
    ASSERT_FALSE(found);

    screenManager_->screens_.clear();
    screenManager_->foldScreenIds_.clear();
    screenManager_->isFoldScreenFlag_ = false;
}

/*
 * @tc.name: OnHwcDeadEvent
 * @tc.desc: Test OnHwcDeadEvent
 * @tc.type: FUNC
 * @tc.require: issueIBIQ0Q
 */
HWTEST_F(RSScreenManagerTest, OnHwcDeadEvent, TestSize.Level1)
{
    EXPECT_NE(screenManager_, nullptr);

    ScreenId sId0 = 0;
    screenManager_->screens_[sId0] = std::make_shared<RSScreen>(sId0);
    ScreenId sId1 = 1;
    VirtualScreenConfigs config {.id = sId1};
    auto virtualScreen = std::make_shared<RSScreen>(config);
    screenManager_->screens_[sId1] = virtualScreen;
    std::map<ScreenId, std::shared_ptr<RSScreen>> retScreens = {};
    screenManager_->OnHwcDeadEvent(retScreens);
    EXPECT_NE(retScreens.size(), 0);
}

/*
 * @tc.name: QueryScreenPropertyTest
 * @tc.desc: Test results of QueryScreenProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, QueryScreenPropertyTest, TestSize.Level1)
{
    ScreenId screenId = 10;
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->screens_[screenId] = nullptr;
    auto property = screenManager_->QueryScreenProperty(screenId);
    EXPECT_EQ(property, nullptr);
    screenManager_->screens_[screenId] = std::make_shared<RSScreen>(screenId);
    property = screenManager_->QueryScreenProperty(screenId);
    EXPECT_NE(property, nullptr);
}

/*
 * @tc.name: ProcessPendingConnectionsTest
 * @tc.desc: Test ProcessPendingConnections
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerTest, ProcessPendingConnectionsTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->ProcessPendingConnections();
    ASSERT_FALSE(screenManager_->screens_.empty());
    ScreenId screenId = screenManager_->screens_.begin()->first;
    screenManager_->pendingConnectedIds_.emplace_back(screenId);
    screenManager_->ProcessPendingConnections();
    screenManager_->screenCorrection_[screenId] = ScreenRotation::ROTATION_0;
    screenManager_->screenBacklight_[screenId] = 0;
    screenManager_->pendingConnectedIds_.emplace_back(INVALID_SCREEN_ID);
    screenManager_->ProcessPendingConnections();
}

/*
 * @tc.name: UpdateVsyncEnabledScreenIdTest
 * @tc.desc: Test UpdateVsyncEnabledScreenId
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, UpdateVsyncEnabledScreenIdTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    bool isFoldScreenFlag = screenManager_->isFoldScreenFlag_;
    screenManager_->isFoldScreenFlag_ = false;
    EXPECT_TRUE(screenManager_->UpdateVsyncEnabledScreenId(INVALID_SCREEN_ID));
    screenManager_->isFoldScreenFlag_ = true;
    screenManager_->foldScreenIds_.clear();
    EXPECT_TRUE(screenManager_->UpdateVsyncEnabledScreenId(INVALID_SCREEN_ID));
    screenManager_->foldScreenIds_[0] = { .isConnected = true, .isPowerOn = true };
    screenManager_->foldScreenIds_[5] = { .isConnected = false, .isPowerOn = true };
    EXPECT_FALSE(screenManager_->UpdateVsyncEnabledScreenId(INVALID_SCREEN_ID));
    screenManager_->foldScreenIds_[0] = { .isConnected = false, .isPowerOn = true };
    EXPECT_TRUE(screenManager_->UpdateVsyncEnabledScreenId(INVALID_SCREEN_ID));
    screenManager_->isFoldScreenFlag_ = isFoldScreenFlag;
}

/*
 * @tc.name: UpdateFoldScreenConnectStatusLockedTest
 * @tc.desc: Test UpdateFoldScreenConnectStatusLocked
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, UpdateFoldScreenConnectStatusLockedTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    bool isFoldScreenFlag = screenManager_->isFoldScreenFlag_;
    screenManager_->isFoldScreenFlag_ = false;
    screenManager_->UpdateFoldScreenConnectStatusLocked(0, false);
    screenManager_->isFoldScreenFlag_ = true;
    screenManager_->UpdateFoldScreenConnectStatusLocked(INVALID_SCREEN_ID, false);
    screenManager_->UpdateFoldScreenConnectStatusLocked(0, true);
    EXPECT_TRUE(screenManager_->foldScreenIds_[0].isConnected);
    screenManager_->UpdateFoldScreenConnectStatusLocked(0, false);
    EXPECT_FALSE(screenManager_->foldScreenIds_[0].isConnected);
    screenManager_->isFoldScreenFlag_ = isFoldScreenFlag;
}

/*
 * @tc.name: SetScreenVsyncEnableByIdTest
 * @tc.desc: Test SetScreenVsyncEnableById
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, SetScreenVsyncEnableByIdTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->screens_[0] = std::make_shared<RSScreen>(0);
    ASSERT_TRUE(screenManager_->screens_.find(0) != screenManager_->screens_.end());
    screenManager_->SetScreenVsyncEnableById(INVALID_SCREEN_ID, INVALID_SCREEN_ID, false);
    screenManager_->SetScreenVsyncEnableById(INVALID_SCREEN_ID, 0, false);
    screenManager_->SetScreenVsyncEnableById(5, 0, false);
    screenManager_->SetScreenVsyncEnableById(1, 1, false);
    screenManager_->SetScreenVsyncEnableById(0, 0, false);
}

/*
 * @tc.name: GetScreenVsyncEnableByIdTest
 * @tc.desc: Test GetScreenVsyncEnableById
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, GetScreenVsyncEnableByIdTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->screens_.clear();
    {
        ScreenId screenId = 1;
        VirtualScreenConfigs cfgVirtual;
        cfgVirtual.id = screenId;
        auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
        rsScreen->property_.SetIsVirtual(true);
        screenManager_->screens_[screenId] = rsScreen;
    }
    screenManager_->screens_[2] = nullptr;
    EXPECT_EQ(screenManager_->GetScreenVsyncEnableById(INVALID_SCREEN_ID), INVALID_SCREEN_ID);
    screenManager_->screens_[0] = std::make_shared<RSScreen>(0);
    EXPECT_EQ(screenManager_->GetScreenVsyncEnableById(INVALID_SCREEN_ID), 0);
    screenManager_->screens_.clear();
}

/*
 * @tc.name: SetScreenFrameGravityTest
 * @tc.desc: Test SetScreenFrameGravity
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, SetScreenFrameGravityTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->SetScreenFrameGravity(0, 0);
    screenManager_->screens_[0] = std::make_shared<RSScreen>(0);
    screenManager_->SetScreenFrameGravity(0, 0);
}

/*
 * @tc.name: ExecuteCallbackTest
 * @tc.desc: Test ExecuteCallbackTest
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, ExecuteCallbackTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    const auto callback = sptr<RSIScreenChangeCallbackConfig>::MakeSptr();
    ASSERT_NE(callback, nullptr);
    screenManager_->screens_.clear();
    screenManager_->ExecuteCallback(callback);
    screenManager_->screens_[0] = nullptr;
    screenManager_->ExecuteCallback(callback);
    {
        ScreenId screenId = 1;
        VirtualScreenConfigs cfgVirtual;
        cfgVirtual.id = screenId;
        auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
        rsScreen->property_.SetIsVirtual(true);
        screenManager_->screens_[screenId] = rsScreen;
    }
    screenManager_->ExecuteCallback(callback);
    screenManager_->screens_[2] = std::make_shared<RSScreen>(0);
    screenManager_->ExecuteCallback(callback);
    screenManager_->screens_.clear();
}

/*
 * @tc.name: ProcessScreenDisConnectedTest
 * @tc.desc: Test ProcessScreenDisConnected
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerTest, ProcessScreenDisConnectedTest, TestSize.Level1)
{
    ASSERT_NE(screenManager_, nullptr);
    screenManager_->ProcessScreenDisConnected(0);
}

/*
 * @tc.name: ModifyVirtualScreenWhiteList001
 * @tc.desc: modify virtual screen white list while screenId invalid
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, ModifyVirtualScreenWhiteList001, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);
    screenManager_->screens_.clear();

    ScreenId screenId = INVALID_SCREEN_ID;
    ASSERT_NE(screenManager_->AddVirtualScreenWhiteList(screenId, {}), ERR_OK);
    ASSERT_NE(screenManager_->RemoveVirtualScreenWhiteList(screenId, {}), ERR_OK);
}

/*
 * @tc.name: ModifyVirtualScreenWhiteList002
 * @tc.desc: modify virtual screen when the screen type is not a virtual screen
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, ModifyVirtualScreenWhiteList002, TestSize.Level2)
{
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    ASSERT_NE(nullptr, screenManager);

    ScreenId screenId = 1;
    screenManager->screens_[screenId] = std::make_shared<RSScreen>(screenId);

    ASSERT_NE(screenManager->AddVirtualScreenWhiteList(screenId, {}), ERR_OK);
    ASSERT_NE(screenManager->RemoveVirtualScreenWhiteList(screenId, {}), ERR_OK);
}

/*
 * @tc.name: ModifyVirtualScreenWhiteList003
 * @tc.desc: modify virtual screen success
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, ModifyVirtualScreenWhiteList003, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto screenId =
        screenManager_->CreateVirtualScreen("virtual0", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, nullptr);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    NodeId nodeId = 0;
    ASSERT_EQ(screenManager_->AddVirtualScreenWhiteList(screenId, {nodeId}), ERR_OK);
    ASSERT_EQ(screenManager_->RemoveVirtualScreenWhiteList(screenId, {nodeId}), ERR_OK);

    // restore
    screenManager_->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: ModifyVirtualScreenWhiteList004
 * @tc.desc: modify screen's whitelist by RSScreen's interfaces
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, ModifyVirtualScreenWhiteList004, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto screenId =
        screenManager_->CreateVirtualScreen("virtual0", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, nullptr);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    auto screen = screenManager_->GetScreen(screenId);
    ASSERT_NE(screen, nullptr);

    NodeId nodeId = 0;
    // modify with OnPropertyChangedCallback
    screen->SetWhiteList({nodeId});
    ASSERT_FALSE(screen->GetWhiteList().empty());
    screen->AddWhiteList({nodeId});;
    ASSERT_FALSE(screen->GetWhiteList().empty());
    screen->RemoveWhiteList({nodeId});
    ASSERT_TRUE(screen->GetWhiteList().empty());

    // modify without OnPropertyChangedCallback
    screen->onPropertyChange_ = nullptr;
    screen->SetWhiteList({nodeId});
    ASSERT_FALSE(screen->GetWhiteList().empty());
    screen->AddWhiteList({nodeId});;
    ASSERT_FALSE(screen->GetWhiteList().empty());
    screen->RemoveWhiteList({nodeId});
    ASSERT_TRUE(screen->GetWhiteList().empty());

    // restore
    screenManager_->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: AddVirtualScreenWhiteList001
 * @tc.desc: modify virtual screen when whitelist exceeds the limit
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenWhiteList001, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto screenId =
        screenManager_->CreateVirtualScreen("virtual0", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, nullptr);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    std::vector<NodeId> whiteList(MAX_SPECIAL_LAYER_NUM + 1);
    ASSERT_NE(screenManager_->AddVirtualScreenWhiteList(screenId, whiteList), ERR_OK);

    // restore
    screenManager_->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: AddVirtualScreenBlackList006
 * @tc.desc: Test AddVirtualScreenBlackList while main screen's blacklist oversize
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenBlackList006, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    ScreenId mainId = 0;
    ScreenId mirrorId1 = 1;
    ScreenId mirrorId2 = 2;
    {
        std::lock_guard<std::mutex> lock(screenManager_->screenMapMutex_);
        screenManager_->screens_[mainId] = std::make_shared<RSScreen>(mainId);
        screenManager_->screens_[mirrorId1] = std::make_shared<RSScreen>(mirrorId1);
        screenManager_->screens_[mirrorId2] = std::make_shared<RSScreen>(mirrorId2);
    }
    screenManager_->defaultScreenId_ = mainId;

    std::vector<NodeId> blackList1(1);
    ASSERT_EQ(screenManager_->AddVirtualScreenBlackList(mirrorId1, blackList1), StatusCode::SUCCESS);

    // restore
    {
        std::lock_guard<std::mutex> lock(screenManager_->screenMapMutex_);
        screenManager_->screens_.erase(mainId);
        screenManager_->screens_.erase(mirrorId1);
        screenManager_->screens_.erase(mirrorId2);
    }
}

/*
 * @tc.name: AddVirtualScreenWhiteList002
 * @tc.desc: check if whitelist change after AddVirtualScreenWhiteList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, AddVirtualScreenWhiteList002, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto screenId =
        screenManager_->CreateVirtualScreen("virtual0", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, nullptr);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    NodeId nodeId = 0;
    ASSERT_EQ(screenManager_->AddVirtualScreenWhiteList(screenId, {++nodeId}), ERR_OK);
    ASSERT_EQ(screenManager_->AddVirtualScreenWhiteList(screenId, {nodeId}), ERR_OK);

    // restore
    screenManager_->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: RemoveVirtualScreenWhiteList001
 * @tc.desc: check if whitelist change after RemoveVirtualScreenWhiteList
 * @tc.type: FUNC
 * @tc.require: issue21114
 */
HWTEST_F(RSScreenManagerTest, RemoveVirtualScreenWhiteList001, TestSize.Level2)
{
    ASSERT_NE(nullptr, screenManager_);

    auto screenId =
        screenManager_->CreateVirtualScreen("virtual0", VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, nullptr);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    NodeId nodeId = 0;
    ASSERT_EQ(screenManager_->AddVirtualScreenWhiteList(screenId, {nodeId}), ERR_OK);
    ASSERT_EQ(screenManager_->RemoveVirtualScreenWhiteList(screenId, {nodeId++}), ERR_OK);
    ASSERT_EQ(screenManager_->RemoveVirtualScreenWhiteList(screenId, {nodeId}), ERR_OK);

    // restore
    screenManager_->RemoveVirtualScreen(screenId);
}
} // namespace OHOS::Rosen