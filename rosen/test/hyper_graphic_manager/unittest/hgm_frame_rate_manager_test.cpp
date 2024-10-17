/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include <test_header.h>

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_config_callback_manager.h"
#include "hgm_idle_detector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string otherSurface = "Other_SF";
    const std::string settingStrategyName = "99";
    constexpr uint64_t  currTime = 200000000;
    constexpr uint64_t  lastTime = 100000000;
    constexpr pid_t appPid = 0;
    constexpr uint32_t touchCount = 1;
    constexpr uint32_t delay_60Ms = 60;
    constexpr uint32_t delay_110Ms = 110;
    ScreenSize screenSize = {720, 1080, 685, 1218}; // width, height, phyWidth, phyHeight
    constexpr int32_t internalScreenId = 5;
    constexpr int32_t externalScreenId = 0;
}
class HgmFrameRateMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void InitHgmFrameRateManager(HgmFrameRateManager &frameRateMgr);
};

void HgmFrameRateMgrTest::SetUpTestCase() {}
void HgmFrameRateMgrTest::TearDownTestCase() {}
void HgmFrameRateMgrTest::SetUp() {}
void HgmFrameRateMgrTest::TearDown() {}

class CustomHgmCallback : public IRemoteStub<RSIHgmConfigChangeCallback> {
public:
    explicit CustomHgmCallback() {}
    ~CustomHgmCallback() override {};

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override {}
    void OnHgmRefreshRateModeChanged(int32_t refreshRateModeName) override {}
    void OnHgmRefreshRateUpdate(int32_t refreshRateUpdate) override {}
};

/**
 * @tc.name: MergeRangeByPriority
 * @tc.desc: Verify the result of MergeRangeByPriority function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, MergeRangeByPriority, Function | SmallTest | Level1)
{
    VoteRange voteRange0 = { OLED_40_HZ, OLED_120_HZ };
    VoteRange voteRange1 = { OLED_30_HZ, OLED_40_HZ };
    VoteRange voteRange2 = { OLED_60_HZ, OLED_90_HZ };
    VoteRange voteRange3 = { OLED_120_HZ, OLED_144_HZ };
    VoteRange voteRange4 = { OLED_30_HZ, OLED_144_HZ };
    VoteRange voteRangeRes;

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange1);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_40_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_40_HZ);

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange2);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_60_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_90_HZ);

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange3);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_120_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_120_HZ);

    voteRangeRes = voteRange0;
    HgmFrameRateManager::MergeRangeByPriority(voteRangeRes, voteRange4);
    ASSERT_EQ(voteRangeRes.first, OledRefreshRate::OLED_40_HZ);
    ASSERT_EQ(voteRangeRes.second, OledRefreshRate::OLED_120_HZ);
}

void HgmFrameRateMgrTest::InitHgmFrameRateManager(HgmFrameRateManager &frameRateMgr)
{
    int64_t offset = 0;
    auto vsyncGenerator = CreateVSyncGenerator();
    sptr<Rosen::VSyncController> rsController = new VSyncController(vsyncGenerator, offset);
    sptr<Rosen::VSyncController> appController = new VSyncController(vsyncGenerator, offset);
    frameRateMgr.Init(rsController, appController, vsyncGenerator);

    auto strategyConfigs = frameRateMgr.multiAppStrategy_.GetStrategyConfigs();
    auto screenSetting = frameRateMgr.multiAppStrategy_.GetScreenSetting();
    strategyConfigs[settingStrategyName] = { .min = OLED_NULL_HZ, .max = OLED_120_HZ, .down = OLED_144_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED, .isFactor = true };
    screenSetting.strategy = settingStrategyName;
    frameRateMgr.multiAppStrategy_.SetStrategyConfigs(strategyConfigs);
    frameRateMgr.multiAppStrategy_.SetScreenSetting(screenSetting);
    frameRateMgr.ReportHiSysEvent({ .extInfo = "ON" });
}

/**
 * @tc.name: HgmUiFrameworkDirtyNodeTest
 * @tc.desc: Verify the result of HgmUiFrameworkDirtyNodeTest function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmUiFrameworkDirtyNodeTest, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    std::vector<std::weak_ptr<RSRenderNode>> uiFwkDirtyNodes;
    PART("HgmUiFrameworkDirtyNodeTest") {
        STEP("1. Test empty uiFwkDirtyNodes") {
            ASSERT_EQ(uiFwkDirtyNodes.size(), 0);
            frameRateMgr.GetUiFrameworkDirtyNodes(uiFwkDirtyNodes);
            {
                std::shared_ptr<RSRenderNode> renderNode1 = std::make_shared<RSRenderNode>(0);
                uiFwkDirtyNodes.emplace_back(renderNode1);
                ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
            }
            ASSERT_EQ(frameRateMgr.GetUiFrameworkDirtyNodes(uiFwkDirtyNodes).empty(), true);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 0);
        }
        STEP("2. Test uiFwkDirtyNodes with a clean renderNode") {
            std::shared_ptr<RSRenderNode> renderNode2 = std::make_shared<RSRenderNode>(0);
            uiFwkDirtyNodes.emplace_back(renderNode2);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
            ASSERT_EQ(renderNode2->IsDirty(), false);
            ASSERT_EQ(frameRateMgr.GetUiFrameworkDirtyNodes(uiFwkDirtyNodes).empty(), true);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
        }
        STEP("3. Test uiFwkDirtyNodes with a dirty renderNode") {
            std::shared_ptr<RSRenderNode> renderNode3 = std::make_shared<RSRenderNode>(0);
            uiFwkDirtyNodes.emplace_back(renderNode3);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 2);

            frameRateMgr.GetUiFrameworkDirtyNodes(uiFwkDirtyNodes);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);

            renderNode3->SetDirty();
            ASSERT_EQ(renderNode3->IsDirty(), true);
            ASSERT_EQ(frameRateMgr.GetUiFrameworkDirtyNodes(uiFwkDirtyNodes).empty(), false);
            ASSERT_EQ(uiFwkDirtyNodes.size(), 1);
        }
    }
}

/**
 * @tc.name: HgmConfigCallbackManagerTest
 * @tc.desc: Verify the result of HgmConfigCallbackManagerTest function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmConfigCallbackManagerTest, Function | SmallTest | Level1)
{
    sptr<HgmConfigCallbackManager> hccMgr = HgmConfigCallbackManager::GetInstance();
    PART("HgmConfigCallbackManagerTest") {
        STEP("1. Callback is nullptr") {
            hccMgr->RegisterHgmRefreshRateModeChangeCallback(0, nullptr);
        }
        STEP("2. Test SyncHgmConfigChangeCallback without callback") {
            std::unordered_map<pid_t, sptr<RSIHgmConfigChangeCallback>> emptyCallback;
            std::swap(hccMgr->animDynamicCfgCallbacks_, emptyCallback);
            ASSERT_EQ(hccMgr->animDynamicCfgCallbacks_.empty(), true);
            hccMgr->SyncHgmConfigChangeCallback();
        }
        STEP("3. Test SyncCallback function with callback") {
            sptr<CustomHgmCallback> cb = new CustomHgmCallback();
            hccMgr->animDynamicCfgCallbacks_[0] = cb;
            hccMgr->refreshRateModeCallbacks_[0] = cb;
            ASSERT_EQ(hccMgr->animDynamicCfgCallbacks_.empty(), false);
            hccMgr->SyncHgmConfigChangeCallback();
            hccMgr->SyncRefreshRateModeChangeCallback(0);
        }
    }
}

/**
 * @tc.name: HgmSetTouchUpFPS001
 * @tc.desc: Verify the result of HgmSetTouchUpFPS001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmSetTouchUpFPS001, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    InitHgmFrameRateManager(frameRateMgr);
    PolicyConfigData::StrategyConfig strategyConfig;
    PART("CaseDescription") {
        STEP("1. init") {
            frameRateMgr.idleDetector_.SetAppSupportedState(true);
            std::vector<std::string> supportedAppBufferList = { otherSurface };
            frameRateMgr.idleDetector_.UpdateSupportAppBufferList(supportedAppBufferList);
            frameRateMgr.UpdateSurfaceTime(otherSurface, lastTime, appPid, UIFWKType::FROM_SURFACE);
        }
        STEP("2. handle touch up event") {
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
            frameRateMgr.UpdateGuaranteedPlanVote(currTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            if (frameRateMgr.multiAppStrategy_.GetVoteRes(strategyConfig) != EXEC_SUCCESS) {
                return; // xml is empty, return
            }

            std::vector<std::pair<std::string, int32_t>> appBufferList;
            appBufferList.push_back(std::make_pair(otherSurface, OLED_90_HZ));
            frameRateMgr.idleDetector_.UpdateAppBufferList(appBufferList);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
            frameRateMgr.UpdateGuaranteedPlanVote(currTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            ASSERT_EQ(frameRateMgr.multiAppStrategy_.GetVoteRes(strategyConfig), EXEC_SUCCESS);

            appBufferList.clear();
            appBufferList.push_back(std::make_pair(otherSurface, OLED_120_HZ));
            frameRateMgr.idleDetector_.ClearAppBufferList();
            frameRateMgr.idleDetector_.UpdateAppBufferList(appBufferList);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
            frameRateMgr.UpdateGuaranteedPlanVote(currTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            ASSERT_EQ(frameRateMgr.multiAppStrategy_.GetVoteRes(strategyConfig), EXEC_SUCCESS);
        }
    }
    frameRateMgr.touchManager_.ChangeState(TouchState::IDLE_STATE);
    sleep(1); // wait for handler task finished
}

/**
 * @tc.name: HgmSetTouchUpFPS002
 * @tc.desc: Verify the result of HgmSetTouchUpFPS002 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmSetTouchUpFPS002, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    InitHgmFrameRateManager(frameRateMgr);
    PolicyConfigData::StrategyConfig strategyConfig;
    PART("CaseDescription") {
        STEP("1. init") {
            frameRateMgr.idleDetector_.SetAppSupportedState(true);
            std::vector<std::string> supportedAppBufferList = { otherSurface };
            frameRateMgr.idleDetector_.UpdateSupportAppBufferList(supportedAppBufferList);
            frameRateMgr.UpdateSurfaceTime(otherSurface, lastTime, appPid, UIFWKType::FROM_SURFACE);
        }
        STEP("2. handle touch up event") {
            std::vector<std::string> appBufferBlackList = { otherSurface };
            frameRateMgr.idleDetector_.UpdateAppBufferBlackList(appBufferBlackList);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
            frameRateMgr.UpdateGuaranteedPlanVote(currTime);
            ASSERT_EQ(frameRateMgr.idleDetector_.ThirdFrameNeedHighRefresh(), false);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            ASSERT_EQ(frameRateMgr.touchManager_.GetState(), TouchState::IDLE_STATE);
        }
    }
    frameRateMgr.touchManager_.ChangeState(TouchState::IDLE_STATE);
    sleep(1); // wait for handler task finished
}

/**
 * @tc.name: MultiThread001
 * @tc.desc: Verify the result of MultiThread001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, MultiThread001, Function | SmallTest | Level1)
{
    int64_t offset = 0;
    int32_t testThreadNum = 100;
    std::string pkg0 = "com.pkg.other:0:-1";
    std::string pkg1 = "com.ss.hm.ugc.aweme:1001:10067";
    std::string pkg2 = "com.wedobest.fivechess.harm:1002:10110";

    HgmFrameRateManager frameRateMgr;
    auto vsyncGenerator = CreateVSyncGenerator();
    sptr<Rosen::VSyncController> rsController = new VSyncController(vsyncGenerator, offset);
    sptr<Rosen::VSyncController> appController = new VSyncController(vsyncGenerator, offset);
    frameRateMgr.Init(rsController, appController, vsyncGenerator);
    
    for (int i = 0; i < testThreadNum; i++) {
        // HandleLightFactorStatus
        frameRateMgr.HandleLightFactorStatus(i, true);
        frameRateMgr.HandleLightFactorStatus(i, false);

        // HandlePackageEvent
        frameRateMgr.HandlePackageEvent(i, {pkg0});
        frameRateMgr.HandlePackageEvent(i, {pkg1});
        frameRateMgr.HandlePackageEvent(i, {pkg2});
        frameRateMgr.HandlePackageEvent(i, {pkg0, pkg1});

        // HandleRefreshRateEvent
        frameRateMgr.HandleRefreshRateEvent(i, {});

        // HandleTouchEvent
        // param 1: touchCnt
        frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_DOWN, 1);
        frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_UP, 1);

        // HandleRefreshRateMode
        // param -1、0、1、2、3：refresh rate mode
        frameRateMgr.HandleRefreshRateMode(-1);
        frameRateMgr.HandleRefreshRateMode(0);
        frameRateMgr.HandleRefreshRateMode(1);
        frameRateMgr.HandleRefreshRateMode(2);
        frameRateMgr.HandleRefreshRateMode(3);

        // HandleScreenPowerStatus
        frameRateMgr.HandleScreenPowerStatus(i, ScreenPowerStatus::POWER_STATUS_ON);
        frameRateMgr.HandleScreenPowerStatus(i, ScreenPowerStatus::POWER_STATUS_OFF);
    }
    sleep(1); // wait for handler task finished
}

/**
 * @tc.name: CleanPidCallbackTest
 * @tc.desc: Verify the result of CleanPidCallbackTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, CleanPidCallbackTest, Function | SmallTest | Level2)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    int32_t defaultPid = 0;
    int32_t gamePid = 10024;
    uint32_t undefinedCallbackType = 0xff;
    std::string defaultScreenStrategyId = "LTPO-DEFAULT";
    std::string invalidScreenStrategyId = "DEFAULT-INVALID";
    auto &hgm = HgmCore::Instance();

    mgr->CleanVote(defaultPid);
    mgr->cleanPidCallback_[gamePid].insert(CleanPidCallbackType::TOUCH_EVENT);
    mgr->cleanPidCallback_[gamePid].insert(CleanPidCallbackType::GAMES);
    mgr->cleanPidCallback_[gamePid].insert(static_cast<CleanPidCallbackType>(undefinedCallbackType));
    mgr->CleanVote(gamePid);

    ASSERT_EQ(mgr->sceneStack_.empty(), true);
    mgr->sceneStack_.push_back(std::make_pair("sceneName", 0));
    ASSERT_EQ(mgr->sceneStack_.empty(), false);

    std::string savedScreenStrategyId = mgr->curScreenStrategyId_;
    ASSERT_EQ(savedScreenStrategyId, defaultScreenStrategyId);
    mgr->curScreenStrategyId_ = invalidScreenStrategyId;
    mgr->UpdateVoteRule();
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
    std::shared_ptr<PolicyConfigData> cachedPolicyConfigData = nullptr;
    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_EQ(hgm.mPolicyConfigData_, nullptr);
    mgr->UpdateVoteRule();
    mgr->curScreenStrategyId_ = savedScreenStrategyId;
    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
}

/**
 * @tc.name: HandleEventTest
 * @tc.desc: Verify the result of HandleEventTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleEventTest, Function | SmallTest | Level2)
{
    std::string pkg0 = "com.pkg.other:0:-1";
    std::string pkg1 = "com.pkg.other:1:-1";

    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    auto &hgm = HgmCore::Instance();
    mgr->DeliverRefreshRateVote({"VOTER_GAMES", 120, 90, 0}, true);

    mgr->GetExpectedFrameRate(static_cast<RSPropertyUnit>(0xff), 100.f);
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
    std::shared_ptr<PolicyConfigData> cachedPolicyConfigData = nullptr;
    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_EQ(hgm.mPolicyConfigData_, nullptr);
    ASSERT_EQ(nullptr, hgm.GetPolicyConfigData());
    mgr->GetPreferredFps("translate", 100.f);

    EventInfo eventInfo = { .eventName = "VOTER_GAMES", .eventStatus = false,
        .description = pkg0,
    };
    mgr->HandleRefreshRateEvent(0, eventInfo);
    mgr->HandleGamesEvent(0, eventInfo);
    eventInfo.eventStatus = true;
    mgr->HandleGamesEvent(0, eventInfo);
    eventInfo.description = pkg1;
    mgr->HandleGamesEvent(0, eventInfo);
    mgr->HandleGamesEvent(1, eventInfo);
    mgr->HandleIdleEvent(true);
    mgr->HandleIdleEvent(false);
    std::swap(hgm.mPolicyConfigData_, cachedPolicyConfigData);
    EXPECT_NE(hgm.mPolicyConfigData_, nullptr);
}

/**
 * @tc.name: GetDrawingFrameRateTest
 * @tc.desc: Verify the result of GetDrawingFrameRateTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, GetDrawingFrameRateTest, Function | SmallTest | Level2)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    uint32_t refreshRate_60 = 60;
    uint32_t refreshRate_120 = 120;
    FrameRateRange dynamic_120(0, 120, 120);
    EXPECT_EQ(dynamic_120.IsDynamic(), true);
    FrameRateRange static_120(120, 120, 120);
    EXPECT_EQ(static_120.IsDynamic(), false);
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_60, dynamic_120), 60);
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_60, static_120), 60);
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_120, dynamic_120), 120);
    EXPECT_EQ(mgr->GetDrawingFrameRate(refreshRate_120, static_120), 120);
}

/**
 * @tc.name: HgmOneShotTimerTest
 * @tc.desc: Verify the result of HgmOneShotTimerTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmOneShotTimerTest, Function | SmallTest | Level2)
{
    auto timer = HgmOneShotTimer("HgmOneShotTimer", std::chrono::milliseconds(20), nullptr, nullptr);
    timer.Start();
    timer.Reset();
    timer.Stop();
    sleep(1); // wait for timer stop
}

/**
 * @tc.name: HgmSimpleTimerTest
 * @tc.desc: Verify the result of HgmSimpleTimerTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmSimpleTimerTest, Function | SmallTest | Level2)
{
    auto timer = HgmSimpleTimer("HgmSimpleTimer", std::chrono::milliseconds(20), nullptr, nullptr);
    timer.Start();
    timer.Reset();
    timer.Stop();
    sleep(1); // wait for timer stop
}

/**
 * @tc.name: HgmRsIdleTimerTest
 * @tc.desc: Verify the result of HgmRsIdleTimerTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HgmRsIdleTimerTest, Function | SmallTest | Level2)
{
    int32_t interval = 700; // 700ms waiting time

    HgmFrameRateManager mgr;
    mgr.minIdleFps_ = OLED_30_HZ;
    mgr.InitRsIdleTimer();
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    mgr.HandleRsFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    sleep(1); // wait for timer stop
}

/**
 * @tc.name: HandleScreenPowerStatus
 * @tc.desc: Verify the result of HandleScreenPowerStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmFrameRateMgrTest, HandleScreenPowerStatus, Function | SmallTest | Level1)
{
    ScreenId extraScreenId = 1;
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData == nullptr || frameRateMgr == nullptr) {
        return;
    }
    // init
    EXPECT_EQ(hgmCore.AddScreen(externalScreenId, 0, screenSize), EXEC_SUCCESS);
    EXPECT_EQ(hgmCore.AddScreen(internalScreenId, 0, screenSize), EXEC_SUCCESS);
    configData->screenStrategyConfigs_["screen0_LTPS"] = "LTPS-DEFAULT";
    configData->screenStrategyConfigs_["screen0_LTPO"] = "LTPO-DEFAULT";
    configData->screenStrategyConfigs_["screen5_LTPS"] = "LTPS-DEFAULT";
    configData->screenStrategyConfigs_["screen5_LTPO"] = "LTPO-DEFAULT";

    // fold -> expand -> fold
    frameRateMgr->HandleScreenPowerStatus(internalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);
    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);

    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    frameRateMgr->HandleScreenPowerStatus(internalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);

    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, internalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);

    EXPECT_EQ(hgmCore.AddScreen(extraScreenId, 0, screenSize), EXEC_SUCCESS);
    frameRateMgr->HandleScreenPowerStatus(internalScreenId, ScreenPowerStatus::POWER_STATUS_SUSPEND);
    frameRateMgr->HandleScreenPowerStatus(externalScreenId, ScreenPowerStatus::POWER_STATUS_ON);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);
    EXPECT_EQ(hgmCore.RemoveScreen(extraScreenId), EXEC_SUCCESS);
    EXPECT_EQ(frameRateMgr->curScreenId_, externalScreenId);
}
} // namespace Rosen
} // namespace OHOS
