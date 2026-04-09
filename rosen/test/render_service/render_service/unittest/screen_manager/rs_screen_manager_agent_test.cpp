/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "callback/rs_screen_callback_manager.h"
#include "gtest/gtest.h"
#include "public/rs_screen_manager_agent.h"
#include "rs_screen_manager.h"
#include "rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr size_t MAX_SPECIAL_LAYER_NUM = 1024;

class RSScreenManagerAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    ScreenId GenerateScreenId()
    {
        testScreenId++;
        return testScreenId;
    }

    void GetGlobalPreprocessor()
    {
        screenManagerAgent_->screenManager_->preprocessor_ = std::make_unique<RSScreenPreprocessor>(
            *screenManager_, *callbackMgr_, handler_, false);
    }

private:
    ScreenId testScreenId = 10;
    std::shared_ptr<RSScreenCallbackManager> callbackMgr_ = std::make_shared<RSScreenCallbackManager>();
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    sptr<RSScreenManager> screenManager_ = sptr<RSScreenManager>::MakeSptr();
    sptr<RSScreenManagerAgent> screenManagerAgent_ = sptr<RSScreenManagerAgent>::MakeSptr(screenManager_);
};

void RSScreenManagerAgentTest::SetUpTestCase() {}
void RSScreenManagerAgentTest::TearDownTestCase() {}
void RSScreenManagerAgentTest::SetUp() {}
void RSScreenManagerAgentTest::TearDown() {}

/*
 * @tc.name: RSScreenManagerAgent001
 * @tc.desc: Test RSScreenManagerAgent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, RSScreenManagerAgent001, TestSize.Level1)
{
    sptr<RSScreenManagerAgent> screenManagerAgent1 = sptr<RSScreenManagerAgent>::MakeSptr(nullptr);
    sptr<RSScreenManagerAgent> screenManagerAgent2 = sptr<RSScreenManagerAgent>::MakeSptr(nullptr);
    screenManagerAgent2->agentListener_ = nullptr;
    sptr<RSScreenManagerAgent> screenManagerAgent3 = sptr<RSScreenManagerAgent>::MakeSptr(screenManager_);
    EXPECT_NE(screenManagerAgent3->screenManager_, nullptr);
    screenManagerAgent3->agentListener_ = nullptr;
    sptr<RSScreenManagerAgent> screenManagerAgent4 = sptr<RSScreenManagerAgent>::MakeSptr(screenManager_);
    EXPECT_NE(screenManagerAgent4->screenManager_, nullptr);

    ScreenChangeReason reason = ScreenChangeReason::DEFAULT;
    screenManagerAgent4->agentListener_->SetScreenChangeCallback(nullptr);
    screenManagerAgent4->agentListener_->OnScreenConnected(INVALID_SCREEN_ID, reason, nullptr);
    screenManagerAgent4->agentListener_->OnScreenDisconnected(INVALID_SCREEN_ID, reason);
}

/*
 * @tc.name: SetScreenChangeCallback001
 * @tc.desc: Test SetScreenChangeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenChangeCallback001, TestSize.Level1)
{
    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    sptr<RSIScreenChangeCallback> changCallbackMgr_;
    ErrCode status = screenManagerAgent_->SetScreenChangeCallback(changCallbackMgr_);
    EXPECT_EQ(status, StatusCode::SUCCESS);
    screenManagerAgent_->screenManager_ = screenManager;
    status = screenManagerAgent_->SetScreenChangeCallback(changCallbackMgr_);
    EXPECT_EQ(status, StatusCode::SUCCESS);
}

/*
 * @tc.name: CleanVirtualScreens001
 * @tc.desc: Test CleanVirtualScreens
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, CleanVirtualScreens001, TestSize.Level1)
{
    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->CleanVirtualScreens();
    screenManagerAgent_->screenManager_ = screenManager;

    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, 0, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    screenManagerAgent_->CleanVirtualScreens();
    std::vector<ScreenId> ids = screenManagerAgent_->GetAllScreenIds();
    EXPECT_TRUE(find(ids.begin(), ids.end(), virtualScreenId) == ids.end());
}

/*
 * @tc.name: GetDefaultScreenId001
 * @tc.desc: Test GetDefaultScreenId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetDefaultScreenId001, TestSize.Level1)
{
    screenManager_->defaultScreenId_ = GenerateScreenId();
    ScreenId screenId = INVALID_SCREEN_ID;
    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetDefaultScreenId(screenId);
    screenManagerAgent_->screenManager_ = screenManager;

    screenManagerAgent_->GetDefaultScreenId(screenId);
    ASSERT_EQ(screenId, testScreenId);
}

/*
 * @tc.name: GetActiveScreenId001
 * @tc.desc: Test GetActiveScreenId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetActiveScreenId001, TestSize.Level1)
{
    screenManager_->isFoldScreenFlag_ = true;
    ScreenId activeScreenId = INVALID_SCREEN_ID;
    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetActiveScreenId(activeScreenId);
    screenManagerAgent_->screenManager_ = screenManager;

    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetActiveScreenId(activeScreenId);
    ASSERT_EQ(activeScreenId, INVALID_SCREEN_ID);
}

/*
 * @tc.name: CreateVirtualScreen001
 * @tc.desc: Test GetAllScreenIds And CreateVirtualScreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, CreateVirtualScreen001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<ScreenId> ids = screenManagerAgent_->GetAllScreenIds();
    EXPECT_TRUE(find(ids.begin(), ids.end(), virtualScreenId) != ids.end());

    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);
    ids = screenManagerAgent_->GetAllScreenIds();
    EXPECT_TRUE(find(ids.begin(), ids.end(), virtualScreenId) == ids.end());

    std::vector<NodeId> whiteList(MAX_SPECIAL_LAYER_NUM + 1, 0);
    screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1, whiteList);
    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetAllScreenIds();
    screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenSurface001
 * @tc.desc: Test SetVirtualScreenSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenSurface001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, 0, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    auto result = screenManagerAgent_->SetVirtualScreenSurface(virtualScreenId, psurface);
    ASSERT_EQ(result, 0);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetVirtualScreenSurface(virtualScreenId, psurface);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenActiveMode001
 * @tc.desc: Test SetScreenActiveMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenActiveMode001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    RSScreenModeInfo screenModeInfo;
    uint32_t modeId = 0;
    auto result = screenManagerAgent_->SetScreenActiveMode(screenId, modeId);
    EXPECT_EQ(result, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->GetScreenActiveMode(screenId, screenModeInfo);
    EXPECT_EQ(screenModeInfo.GetScreenModeId(), -1);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenActiveMode(screenId, modeId);
    screenManagerAgent_->GetScreenActiveMode(screenId, screenModeInfo);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetAsMainScreenTest001
 * @tc.desc: Test SetAsMainScreen
 * @tc.type: FUNC
 * @tc.require: #23043
 */
HWTEST_F(RSScreenManagerAgentTest, SetAsMainScreenTest001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    ScreenId screenId = INVALID_SCREEN_ID;
    bool isMainScreen = true;
    screenManagerAgent_->SetAsMainScreen(screenId, isMainScreen);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    auto ret = screenManagerAgent_->SetAsMainScreen(screenId, isMainScreen);
    ASSERT_EQ(ret, SCREEN_NOT_FOUND);

    // restore
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetMainScreenIdTest001
 * @tc.desc: Test GetMainScreenId
 * @tc.type: FUNC
 * @tc.require: #23043
 */
HWTEST_F(RSScreenManagerAgentTest, GetMainScreenIdTest001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    ScreenId screenId = screenManagerAgent_->GetMainScreenId();

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenId = screenManagerAgent_->GetMainScreenId();
    ASSERT_EQ(screenId, INVALID_SCREEN_ID);

    // restore
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenSupportedModes001
 * @tc.desc: Test GetScreenSupportedModes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenSupportedModes001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    auto supportedScreenModes = screenManagerAgent_->GetScreenSupportedModes(screenId);
    ASSERT_EQ(supportedScreenModes.size(), 0);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenSupportedModes(screenId);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenCapability001
 * @tc.desc: Test GetScreenCapability
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenCapability001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, 0, {});
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    RSScreenCapability screenCapability = screenManagerAgent_->GetScreenCapability(virtualScreenId);
    ASSERT_EQ(screenCapability.GetType(), DISP_INVALID);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenCapability(virtualScreenId);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenPowerStatus001
 * @tc.desc: Test GetScreenPowerStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenPowerStatus001, TestSize.Level1)
{
    ScreenPowerStatus powerStatus = screenManagerAgent_->GetScreenPowerStatus(INVALID_SCREEN_ID);
    EXPECT_NE(powerStatus, ScreenPowerStatus::POWER_STATUS_ON);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenPowerStatus(INVALID_SCREEN_ID);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenData001
 * @tc.desc: Test GetScreenData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenData001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    VirtualScreenConfigs cfgVirtual;
    cfgVirtual.id = screenId;
    auto rsScreen = std::make_shared<RSScreen>(cfgVirtual);
    screenManager_->screens_[screenId] = rsScreen;
    RSScreenData screenData = screenManagerAgent_->GetScreenData(screenId);
    ASSERT_EQ(screenData.GetSupportModeInfo().size(), 0);
    ASSERT_EQ(screenData.GetCapability().GetType(), DISP_INVALID);
    ASSERT_EQ(screenData.GetActivityModeInfo().GetScreenModeId(), -1);
    ASSERT_EQ(screenData.GetSupportModeInfo().size(), 0);
    ASSERT_EQ(screenData.GetPowerStatus(), INVALID_POWER_STATUS);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenData(screenId);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenBacklight001
 * @tc.desc: Test GetScreenBacklight
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenBacklight001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    auto backLight = screenManagerAgent_->GetScreenBacklight(screenId);
    ASSERT_EQ(backLight, INVALID_BACKLIGHT_VALUE);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenBacklight(screenId);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenBacklight001
 * @tc.desc: Test SetScreenBacklight
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenBacklight001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create();
    EXPECT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, pSurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    screenManager_->screenBacklight_[virtualScreenId] = 50;
    screenManagerAgent_->SetScreenBacklight(virtualScreenId, 100);
    EXPECT_EQ(screenManager_->screenBacklight_[virtualScreenId], 100);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenBacklight(virtualScreenId, 100);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: DisablePowerOffRenderControl001
 * @tc.desc: Test DisablePowerOffRenderControl
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, DisablePowerOffRenderControl001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    ScreenId screenId = GenerateScreenId();
    screenManagerAgent_->DisablePowerOffRenderControl(screenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->DisablePowerOffRenderControl(screenId);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenBlackList001
 * @tc.desc: Test SetVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenBlackList001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, pSurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeId> blackList = {1, 2, 3};
    int32_t result = screenManagerAgent_->SetVirtualScreenBlackList(virtualScreenId, blackList);
    ASSERT_EQ(result, 0);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    std::vector<NodeId> blackListMax(MAX_SPECIAL_LAYER_NUM + 1, 0);
    std::vector<NodeId> blackListEmpty = {};
    screenManagerAgent_->SetVirtualScreenBlackList(virtualScreenId, blackListMax);
    screenManagerAgent_->SetVirtualScreenBlackList(virtualScreenId, blackListEmpty);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenTypeBlackList001
 * @tc.desc: Test SetVirtualScreenTypeBlackList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenTypeBlackList001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, pSurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeType> typeBlackList = {1, 2, 3};
    int32_t result = screenManagerAgent_->SetVirtualScreenTypeBlackList(virtualScreenId, typeBlackList);
    ASSERT_EQ(result, 0);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    std::vector<NodeType> typeBlackListMax(MAX_SPECIAL_LAYER_NUM + 1, 0);
    std::vector<NodeType> typeBlackListEmpty = {};
    screenManagerAgent_->SetVirtualScreenTypeBlackList(virtualScreenId, typeBlackListMax);
    screenManagerAgent_->SetVirtualScreenTypeBlackList(virtualScreenId, typeBlackListEmpty);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: AddVirtualScreenBlackList001
 * @tc.desc: Test AddVirtualScreenBlackList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, AddVirtualScreenBlackList001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, pSurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeId> blackList = {1, 2, 3};
    int32_t result1 = screenManagerAgent_->AddVirtualScreenBlackList(virtualScreenId, blackList);
    ASSERT_EQ(result1, 0);
    int32_t result2 = screenManagerAgent_->RemoveVirtualScreenBlackList(virtualScreenId, blackList);
    ASSERT_EQ(result2, 0);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    std::vector<NodeId> blackListMax(MAX_SPECIAL_LAYER_NUM + 1, 0);
    std::vector<NodeId> blackListEmpty = {};
    screenManagerAgent_->AddVirtualScreenBlackList(virtualScreenId, blackListMax);
    screenManagerAgent_->AddVirtualScreenBlackList(virtualScreenId, blackListEmpty);
    screenManagerAgent_->RemoveVirtualScreenBlackList(virtualScreenId, blackListEmpty);
    screenManagerAgent_->AddVirtualScreenBlackList(virtualScreenId, blackList);
    screenManagerAgent_->RemoveVirtualScreenBlackList(virtualScreenId, blackList);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: AddVirtualScreenWhiteList001
 * @tc.desc: Test AddVirtualScreenWhiteList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, AddVirtualScreenWhiteList001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, pSurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    std::vector<NodeId> whiteList = {1, 2, 3};
    int32_t result1 = screenManagerAgent_->AddVirtualScreenWhiteList(virtualScreenId, whiteList);
    ASSERT_EQ(result1, 0);
    int32_t result2 = screenManagerAgent_->RemoveVirtualScreenWhiteList(virtualScreenId, whiteList);
    ASSERT_EQ(result2, 0);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    std::vector<NodeId> whiteListMax(MAX_SPECIAL_LAYER_NUM + 1, 0);
    std::vector<NodeId> whiteListEmpty = {};
    screenManagerAgent_->AddVirtualScreenWhiteList(virtualScreenId, whiteListMax);
    screenManagerAgent_->AddVirtualScreenWhiteList(virtualScreenId, whiteListEmpty);
    screenManagerAgent_->RemoveVirtualScreenWhiteList(virtualScreenId, whiteListEmpty);
    screenManagerAgent_->AddVirtualScreenWhiteList(virtualScreenId, whiteList);
    screenManagerAgent_->RemoveVirtualScreenWhiteList(virtualScreenId, whiteList);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenSwitchingNotifyCallback001
 * @tc.desc: Test SetScreenSwitchingNotifyCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenSwitchingNotifyCallback001, TestSize.Level1)
{
    sptr<RSIScreenSwitchingNotifyCallback> callback;
    auto result = screenManagerAgent_->SetScreenSwitchingNotifyCallback(callback);
    ASSERT_EQ(result, StatusCode::SUCCESS);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenSwitchingNotifyCallback(callback);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenSecurityExemptionList001
 * @tc.desc: Test SetVirtualScreenSecurityExemptionList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenSecurityExemptionList001, TestSize.Level1)
{
    std::vector<NodeId> securityExemptionList = {};
    auto result = screenManagerAgent_->SetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID, securityExemptionList);
    EXPECT_EQ(result, INVALID_ARGUMENTS);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    std::vector<NodeId> securityExemptionListMax(MATAKEY_BLUE_PRIMARY_X + 1, 0);
    screenManagerAgent_->SetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID, securityExemptionListMax);
    screenManagerAgent_->SetVirtualScreenSecurityExemptionList(INVALID_SCREEN_ID, securityExemptionList);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenSecurityMask001
 * @tc.desc: Test SetScreenSecurityMask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenSecurityMask001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, pSurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t ret = screenManagerAgent_->SetScreenSecurityMask(virtualScreenId, nullptr);
    EXPECT_EQ(ret, SUCCESS);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenSecurityMask(virtualScreenId, nullptr);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetMirrorScreenVisibleRect001
 * @tc.desc: Test SetMirrorScreenVisibleRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetMirrorScreenVisibleRect001, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    Rect mainScreenRect;
    int32_t ret = screenManagerAgent_->SetMirrorScreenVisibleRect(screenId, mainScreenRect, true);
    EXPECT_EQ(ret, INVALID_ARGUMENTS);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetMirrorScreenVisibleRect(screenId, mainScreenRect, true);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetCastScreenEnableSkipWindow001
 * @tc.desc: Test SetCastScreenEnableSkipWindow
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetCastScreenEnableSkipWindow001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create();
    ASSERT_NE(cSurface, nullptr);

    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(pSurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, pSurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    bool enable = true;
    int32_t ret = screenManagerAgent_->SetCastScreenEnableSkipWindow(virtualScreenId, enable);
    EXPECT_EQ(ret, SUCCESS);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetCastScreenEnableSkipWindow(virtualScreenId, enable);
    screenManagerAgent_->screenManager_ = screenManager;
}


/*
 * @tc.name: SetPhysicalScreenResolution001
 * @tc.desc: Test SetPhysicalScreenResolution
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetPhysicalScreenResolution001, TestSize.Level1)
{
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    auto ret = screenManagerAgent_->SetPhysicalScreenResolution(INVALID_SCREEN_ID, newWidth, newHeight);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetPhysicalScreenResolution(INVALID_SCREEN_ID, newWidth, newHeight);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenResolution001
 * @tc.desc: Test SetVirtualScreenResolution
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenResolution001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    auto curVirtualScreenResolution = screenManagerAgent_->GetVirtualScreenResolution(virtualScreenId);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenWidth(), defaultWidth);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenHeight(), defaultHeight);

    screenManagerAgent_->SetVirtualScreenResolution(virtualScreenId, newWidth, newHeight);

    curVirtualScreenResolution = screenManagerAgent_->GetVirtualScreenResolution(virtualScreenId);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenWidth(), newWidth);
    EXPECT_EQ(curVirtualScreenResolution.GetVirtualScreenHeight(), newHeight);

    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetVirtualScreenResolution(virtualScreenId, newWidth, newHeight);
    screenManagerAgent_->GetVirtualScreenResolution(virtualScreenId);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetRogScreenResolution001
 * @tc.desc: Test SetRogScreenResolution
 * @tc.type: FUNC
 */
HWTEST_F(RSScreenManagerAgentTest, SetRogScreenResolution001, Function | SmallTest | Level2)
{
    ScreenId screenId = GenerateScreenId();
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    auto ret = screenManagerAgent_->SetRogScreenResolution(screenId, newWidth, newHeight);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
 * @tc.name: SetScreenPowerStatus001
 * @tc.desc: Test SetScreenPowerStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenPowerStatus001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    screenManagerAgent_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    ScreenPowerStatus powerStatus = screenManagerAgent_->GetScreenPowerStatus(screenId);
    EXPECT_NE(powerStatus, ScreenPowerStatus::POWER_STATUS_OFF);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenPowerStatus(screenId, ScreenPowerStatus::POWER_STATUS_OFF);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: MarkPowerOffNeedProcessOneFrame001
 * @tc.desc: Test MarkPowerOffNeedProcessOneFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, MarkPowerOffNeedProcessOneFrame001, TestSize.Level1)
{
    screenManagerAgent_->MarkPowerOffNeedProcessOneFrame();
    EXPECT_EQ(screenManager_->powerOffNeedProcessOneFrame_, true);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->MarkPowerOffNeedProcessOneFrame();
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: ResizeVirtualScreen001
 * @tc.desc: Test ResizeVirtualScreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, ResizeVirtualScreen001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    uint32_t newWidth = 1920;
    uint32_t newHeight = 1080;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t ret = screenManagerAgent_->ResizeVirtualScreen(virtualScreenId, newWidth, newHeight);
    ASSERT_EQ(ret, 0);

    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->ResizeVirtualScreen(virtualScreenId, newWidth, newHeight);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenColorGamut001
 * @tc.desc: Test SetScreenColorGamut
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenColorGamut001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    auto ret = screenManagerAgent_->SetScreenColorGamut(screenId, 0);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenColorGamut(screenId, 0);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenColorGamut001
 * @tc.desc: Test GetScreenColorGamut
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenColorGamut001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    ScreenColorGamut mode;
    auto ret = screenManagerAgent_->GetScreenColorGamut(screenId, mode);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenColorGamut(screenId, mode);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenSupportedColorGamuts001
 * @tc.desc: Test GetScreenSupportedColorGamuts
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenSupportedColorGamuts001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    std::vector<ScreenColorGamut> modeList = {};
    auto ret = screenManagerAgent_->GetScreenSupportedColorGamuts(screenId, modeList);


    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ScreenColorGamut mode;
    ret = screenManagerAgent_->GetScreenColorGamut(screenId, mode);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenGamutMap001
 * @tc.desc: Test SetScreenGamutMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenGamutMap001, TestSize.Level1)
{
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    int ret = screenManagerAgent_->SetScreenGamutMap(INVALID_SCREEN_ID, gamutMap);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->SetScreenGamutMap(INVALID_SCREEN_ID, gamutMap);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenGamutMap001
 * @tc.desc: Test GetScreenGamutMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenGamutMap001, TestSize.Level1)
{
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    int ret = screenManagerAgent_->GetScreenGamutMap(INVALID_SCREEN_ID, gamutMap);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->GetScreenGamutMap(INVALID_SCREEN_ID, gamutMap);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenHDRFormat001
 * @tc.desc: Test SetScreenHDRFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenHDRFormat001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    auto ret = screenManagerAgent_->SetScreenHDRFormat(screenId, 0);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->SetScreenHDRFormat(screenId, 0);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenHDRFormat001
 * @tc.desc: Test GetScreenHDRFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenHDRFormat001, TestSize.Level1)
{
    ScreenHDRFormat hdrFormat = ScreenHDRFormat::NOT_SUPPORT_HDR;
    auto ret = screenManagerAgent_->GetScreenHDRFormat(INVALID_SCREEN_ID, hdrFormat);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->GetScreenHDRFormat(INVALID_SCREEN_ID, hdrFormat);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenSupportedHDRFormats001
 * @tc.desc: Test GetScreenSupportedHDRFormats
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenSupportedHDRFormats001, TestSize.Level1)
{
    std::vector<ScreenHDRFormat> hdrFormats;
    int ret = screenManagerAgent_->GetScreenSupportedHDRFormats(INVALID_SCREEN_ID, hdrFormats);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->GetScreenSupportedHDRFormats(INVALID_SCREEN_ID, hdrFormats);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenHDRCapability001
 * @tc.desc: Test GetScreenHDRCapability
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenHDRCapability001, TestSize.Level1)
{
    RSScreenHDRCapability hdrCapability;
    int ret = screenManagerAgent_->GetScreenHDRCapability(INVALID_SCREEN_ID, hdrCapability);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    EXPECT_EQ(hdrCapability.GetMaxLum(), 0);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->GetScreenHDRCapability(INVALID_SCREEN_ID, hdrCapability);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenSupportedMetaDataKeys001
 * @tc.desc: Test GetScreenSupportedMetaDataKeys
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenSupportedMetaDataKeys001, TestSize.Level1)
{
    std::vector<ScreenHDRMetadataKey> keys;
    int ret = screenManagerAgent_->GetScreenSupportedMetaDataKeys(INVALID_SCREEN_ID, keys);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->GetScreenSupportedMetaDataKeys(INVALID_SCREEN_ID, keys);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenCorrection001
 * @tc.desc: Test SetScreenCorrection
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenCorrection001, TestSize.Level1)
{
    auto ret = screenManagerAgent_->SetScreenCorrection(INVALID_SCREEN_ID, ScreenRotation::INVALID_SCREEN_ROTATION);
    ASSERT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->SetScreenCorrection(INVALID_SCREEN_ID, ScreenRotation::INVALID_SCREEN_ROTATION);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation001
 * @tc.desc: Test SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualMirrorScreenCanvasRotation001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_EQ(screenManagerAgent_->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, true), true);
    EXPECT_EQ(screenManagerAgent_->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, false), true);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetVirtualMirrorScreenCanvasRotation(virtualScreenId, true);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenAutoRotation001
 * @tc.desc: Test SetVirtualScreenAutoRotation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenAutoRotation001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t ret = screenManagerAgent_->SetVirtualScreenAutoRotation(virtualScreenId, true);
    ASSERT_EQ(ret, 0);

    ret = screenManagerAgent_->SetVirtualScreenAutoRotation(INVALID_SCREEN_ID, true);
    ASSERT_NE(ret, 0);

    ret = screenManagerAgent_->SetVirtualScreenAutoRotation(0, true);
    ASSERT_NE(ret, 0);

    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetVirtualScreenAutoRotation(INVALID_SCREEN_ID, true);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode001
 * @tc.desc: Test SetVirtualMirrorScreenScaleMode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualMirrorScreenScaleMode001, TestSize.Level1)
{
    ASSERT_FALSE(screenManagerAgent_->
        SetVirtualMirrorScreenScaleMode(INVALID_SCREEN_ID, ScreenScaleMode::INVALID_MODE));

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->
        SetVirtualMirrorScreenScaleMode(INVALID_SCREEN_ID, ScreenScaleMode::INVALID_MODE);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetPixelFormat001
 * @tc.desc: Test GetPixelFormat and SetPixelFormat
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetPixelFormat001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    GraphicPixelFormat curPixelFormat;
    int32_t ret1 = screenManagerAgent_->GetPixelFormat(virtualScreenId, curPixelFormat);
    EXPECT_EQ(ret1, 0);
    EXPECT_EQ(curPixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);

    int32_t ret2 = screenManagerAgent_->SetPixelFormat(virtualScreenId, pixelFormat);
    EXPECT_EQ(ret2, 0);

    int32_t ret3 = screenManagerAgent_->GetPixelFormat(virtualScreenId, curPixelFormat);
    EXPECT_EQ(ret3, 0);
    EXPECT_EQ(curPixelFormat, GRAPHIC_PIXEL_FMT_BGRA_8888);

    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetPixelFormat(virtualScreenId, pixelFormat);
    screenManagerAgent_->GetPixelFormat(virtualScreenId, curPixelFormat);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenColorSpace001
 * @tc.desc: Test SetScreenColorSpace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenColorSpace001, TestSize.Level1)
{
    auto ret = screenManagerAgent_->SetScreenColorSpace(
        INVALID_SCREEN_ID, GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenColorSpace(
        INVALID_SCREEN_ID, GraphicCM_ColorSpaceType::GRAPHIC_CM_COLORSPACE_NONE);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenColorSpace001
 * @tc.desc: Test GetScreenColorSpace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenColorSpace001, TestSize.Level1)
{
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    int ret = screenManagerAgent_->GetScreenColorSpace(INVALID_SCREEN_ID, colorSpace);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenColorSpace(INVALID_SCREEN_ID, colorSpace);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenSupportedColorSpaces001
 * @tc.desc: Test GetScreenSupportedColorSpaces
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenSupportedColorSpaces001, TestSize.Level1)
{
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    int ret = screenManagerAgent_->GetScreenSupportedColorSpaces(INVALID_SCREEN_ID, colorSpaces);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenSupportedColorSpaces(INVALID_SCREEN_ID, colorSpaces);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetScreenType001
 * @tc.desc: Test GetScreenType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetScreenType001, TestSize.Level1)
{
    RSScreenType type;
    int ret = screenManagerAgent_->GetScreenType(INVALID_SCREEN_ID, type);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->GetScreenType(INVALID_SCREEN_ID, type);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenSkipFrameInterval001
 * @tc.desc: Test SetScreenSkipFrameInterval
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenSkipFrameInterval001, TestSize.Level1)
{
    ScreenId id = INVALID_SCREEN_ID;
    uint32_t skipFrameInterval = 0;
    int ret = screenManagerAgent_->SetScreenSkipFrameInterval(id, skipFrameInterval);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    ret = screenManagerAgent_->SetScreenSkipFrameInterval(id, skipFrameInterval);
    EXPECT_EQ(ret, SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenRefreshRate001
 * @tc.desc: Test SetVirtualScreenRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenRefreshRate001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    uint32_t maxRefreshRate = 0;
    uint32_t actualRefreshRate = 0;
    int ret = screenManagerAgent_->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
    EXPECT_EQ(ret, SCREEN_NOT_FOUND);
}

/*
 * @tc.name: GetDisplayIdentificationData001
 * @tc.desc: Test GetDisplayIdentificationData
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetDisplayIdentificationData001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    uint8_t outPort = 1;
    std::vector<uint8_t> edidData(5);
    int ret = screenManager_->GetDisplayIdentificationData(screenId, outPort, edidData);
    EXPECT_EQ(ret, SCREEN_NOT_FOUND);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManager_->GetDisplayIdentificationData(screenId, outPort, edidData);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenActiveRect001
 * @tc.desc: Test SetScreenActiveRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenActiveRect001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    Rect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(screenManagerAgent_->SetScreenActiveRect(virtualScreenId, activeRect), StatusCode::HDI_ERROR);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenActiveRect(virtualScreenId, activeRect);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenOffset001
 * @tc.desc: Test SetScreenOffset
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenOffset001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    ScreenId screenId = GenerateScreenId();
    int32_t offsetX = 100;
    int32_t offsetY = 100;
    screenManagerAgent_->SetScreenOffset(screenId, offsetX, offsetY);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenOffset(screenId, offsetX, offsetY);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetVirtualScreenStatus001
 * @tc.desc: Test SetVirtualScreenStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetVirtualScreenStatus001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    ScreenId screenId = GenerateScreenId();
    GetGlobalPreprocessor();
    ASSERT_NE(screenManagerAgent_->screenManager_->preprocessor_, nullptr);
    ScreenId virtualScreenId = screenManagerAgent_->CreateVirtualScreen(
        "virtual", defaultWidth, defaultHeight, psurface, screenId, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    EXPECT_TRUE(screenManagerAgent_->SetVirtualScreenStatus(virtualScreenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY));
    EXPECT_TRUE(screenManagerAgent_->SetVirtualScreenStatus(
        virtualScreenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE));
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetVirtualScreenStatus(virtualScreenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenSwitchStatus001
 * @tc.desc: Test SetScreenSwitchStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenSwitchStatus001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    screenManagerAgent_->SetScreenSwitchStatus(false);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenSwitchStatus(false);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetScreenFrameGravity001
 * @tc.desc: Test SetScreenFrameGravity
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenFrameGravity001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    ScreenId screenId = INVALID_SCREEN_ID;
    int32_t gravity = 0;
    screenManagerAgent_->SetScreenFrameGravity(screenId, gravity);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    screenManagerAgent_->SetScreenFrameGravity(screenId, gravity);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: SetDualScreenState001
 * @tc.desc: Test SetDualScreenState
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetDualScreenState001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    ScreenId screenId = INVALID_SCREEN_ID;
    DualScreenStatus status = DualScreenStatus::DUAL_SCREEN_EXIT;
    screenManagerAgent_->SetDualScreenState(screenId, status);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    auto ret = screenManagerAgent_->SetDualScreenState(screenId, status);
    ASSERT_EQ(ret, SCREEN_NOT_FOUND);
    screenManagerAgent_->screenManager_ = screenManager;
}

/*
 * @tc.name: GetPanelPowerStatus001
 * @tc.desc: Test GetPanelPowerStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, GetPanelPowerStatus001, TestSize.Level1)
{
    ASSERT_NE(screenManagerAgent_, nullptr);
    ScreenId screenId = INVALID_SCREEN_ID;
    screenManagerAgent_->GetPanelPowerStatus(screenId);

    auto screenManager = screenManagerAgent_->screenManager_;
    screenManagerAgent_->screenManager_ = nullptr;
    auto ret = screenManagerAgent_->GetPanelPowerStatus(screenId);
    ASSERT_EQ(ret, PanelPowerStatus::INVALID_PANEL_POWER_STATUS);
    screenManagerAgent_->screenManager_ = screenManager;
}
}