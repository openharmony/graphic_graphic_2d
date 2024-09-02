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
    int32_t width = 720;
    int32_t height = 1080;
    int32_t phyWidth = 685;
    int32_t phyHeight = 1218;
    ScreenSize screenSize = {width, height, phyWidth, phyHeight};
    const std::string otherSurface = "Other_SF";
    const std::string settingStrategyName = "99";
    constexpr uint64_t  currTime = 200000000;
    constexpr uint64_t  lastTime = 100000000;
    constexpr pid_t appPid = 0;
    constexpr uint32_t touchCount = 1;
    constexpr uint32_t delay_60Ms = 60;
    constexpr uint32_t delay_110Ms = 110;
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
            frameRateMgr.UpdateSurfaceTime(otherSurface, lastTime, appPid);
        }
        STEP("2. handle touch up event") {
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
            frameRateMgr.UpdateGuaranteedPlanVote(currTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            HgmErrCode res = frameRateMgr.multiAppStrategy_.GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, OLED_120_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_120_HZ);

            std::vector<std::pair<std::string, int32_t>> appBufferList;
            appBufferList.push_back(std::make_pair(otherSurface, OLED_90_HZ));
            frameRateMgr.idleDetector_.UpdateAppBufferList(appBufferList);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
            frameRateMgr.UpdateGuaranteedPlanVote(currTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            res = frameRateMgr.multiAppStrategy_.GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, OLED_90_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_90_HZ);

            appBufferList.clear();
            appBufferList.push_back(std::make_pair(otherSurface, OLED_120_HZ));
            frameRateMgr.idleDetector_.ClearAppBufferList();
            frameRateMgr.idleDetector_.UpdateAppBufferList(appBufferList);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_DOWN, touchCount);
            frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_UP, touchCount);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
            frameRateMgr.UpdateGuaranteedPlanVote(currTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_60Ms));
            res = frameRateMgr.multiAppStrategy_.GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, OLED_120_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_120_HZ);
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
            frameRateMgr.UpdateSurfaceTime(otherSurface, lastTime, appPid);
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
    int64_t touchCnt = 1;
    int32_t testThreadNum = 100;
    std::string pkg0 = "com.pkg.other:0:-1";
    std::string pkg1 = "com.ss.hm.ugc.aweme:1001:10067";
    std::string pkg2 = "com.wedobest.fivechess.harm:1002:10110";

    HgmFrameRateManager frameRateMgr;
    auto vsyncGenerator = CreateVSyncGenerator();
    sptr<Rosen::VSyncController> rsController = new VSyncController(vsyncGenerator, offset);
    sptr<Rosen::VSyncController> appController = new VSyncController(vsyncGenerator, offset);
    frameRateMgr.Init(rsController, appController, vsyncGenerator);

    std::vector<std::thread> testThreads;
    
    for (int i = 0; i < testThreadNum; i++) {
        // HandleLightFactorStatus
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleLightFactorStatus(i, true); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleLightFactorStatus(i, false); }));

        // HandlePackageEvent
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, {pkg0}); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, {pkg1}); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, {pkg2}); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, {pkg0, pkg1}); }));

        // HandleRefreshRateEvent
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleRefreshRateEvent(i, {}); }));

        // HandleTouchEvent
        // param 1: touchCnt
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_DOWN, 1); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_UP, 1); }));

        // HandleRefreshRateMode
        // param -1、0、1、2、3：refresh rate mode
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleRefreshRateMode(-1); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleRefreshRateMode(0); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleRefreshRateMode(1); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleRefreshRateMode(2); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleRefreshRateMode(3); }));

        // HandleScreenPowerStatus
        testThreads.push_back(std::thread([&] () {
            frameRateMgr.HandleScreenPowerStatus(i, ScreenPowerStatus::POWER_STATUS_ON);
        }));
        testThreads.push_back(std::thread([&] () {
            frameRateMgr.HandleScreenPowerStatus(i, ScreenPowerStatus::POWER_STATUS_OFF);
        }));
    }
    for (auto &testThread : testThreads) {
        if (testThread.joinable()) {
            testThread.join();
        }
    }
    sleep(1); // wait for handler task finished
}

/**
 * @tc.name: HgmOneShotTimerTest
 * @tc.desc: Verify the result of HgmOneShotTimerTest
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, HgmOneShotTimerTest, Function | SmallTest | Level2)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    ScreenId id = 1;
    int32_t interval = 200; // 200ms means waiting time

    PART("CaseDescription") {
        STEP("1. set force update callback") {
            mgr->SetForceUpdateCallback([](bool idleTimerExpired, bool forceUpdate) {});
        }
        STEP("2. insert and start screenTimer") {
            mgr->StartScreenTimer(id, interval, nullptr, nullptr);
            auto timer = mgr->GetScreenTimer(id);
            STEP_ASSERT_NE(timer, nullptr);
        }
        STEP("3. reset screenTimer") {
            mgr->ResetScreenTimer(id);
            auto timer = mgr->GetScreenTimer(id);
            STEP_ASSERT_NE(timer, nullptr);
        }
    }
}

/**
 * @tc.name: HgmOneShotTimerTest001
 * @tc.desc: Verify the result of HgmOneShotTimerTest001
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, HgmOneShotTimerTest001, Function | SmallTest | Level2)
{
    int32_t interval = 200; // 200ms means waiting time
    int32_t testThreadNum = 100;

    auto timer = HgmOneShotTimer("timer", std::chrono::milliseconds(interval), nullptr, nullptr);
    std::vector<std::thread> testThreads;
    for (int i = 0; i < testThreadNum; i++) {
        testThreads.push_back(std::thread([&timer] () { return timer.Start(); }));
        testThreads.push_back(std::thread([&timer] () { return timer.Reset(); }));
        testThreads.push_back(std::thread([&timer] () { return timer.Stop(); }));
    }
    for (auto &testThread : testThreads) {
        if (testThread.joinable()) {
            testThread.join();
        }
    }
    sleep(1); // wait for handler task finished
}

} // namespace Rosen
} // namespace OHOS