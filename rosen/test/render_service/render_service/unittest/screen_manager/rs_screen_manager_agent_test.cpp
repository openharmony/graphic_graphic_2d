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
 * @tc.name: SetScreenChangeCallback001
 * @tc.desc: Test SetScreenChangeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenChangeCallback001, TestSize.Level1)
{
    sptr<RSIScreenChangeCallback> changCallbackMgr_;
    ErrCode status = screenManagerAgent_->SetScreenChangeCallback(changCallbackMgr_);
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
    // screenManager_->activeScreenId_ = GenerateScreenId();
    screenManager_->isFoldScreenFlag_ = true;
    ScreenId activeScreenId = INVALID_SCREEN_ID;
    screenManagerAgent_->GetActiveScreenId(activeScreenId);
    ASSERT_EQ(activeScreenId, testScreenId);
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
}

/*
 * @tc.name: SetScreenSkipFrameInterval001
 * @tc.desc: Test SetScreenSkipFrameInterval
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenManagerAgentTest, SetScreenSkipFrameInterval001, TestSize.Level1)
{
    ScreenId screenId = GenerateScreenId();
    uint32_t skipFrameInterval = 0;
    int ret = screenManager_->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, SCREEN_NOT_FOUND);
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
    uint32_t actualRefreshRate = 8;
    int ret = screenManager_->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
    EXPECT_EQ(ret, SCREEN_NOT_FOUND);
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
    GraphicIRect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(screenManagerAgent_->SetScreenActiveRect(virtualScreenId, activeRect), StatusCode::HDI_ERROR);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);
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
    EXPECT_EQ(screenManagerAgent_->SetVirtualScreenStatus(virtualScreenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY), true);
    EXPECT_EQ(screenManagerAgent_->SetVirtualScreenStatus(virtualScreenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE), true);
    screenManagerAgent_->RemoveVirtualScreen(virtualScreenId);
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
    ScreenId screenId = GenerateScreenId();
    screenManagerAgent_->SetScreenSwitchStatus(screenId, false);
}
}