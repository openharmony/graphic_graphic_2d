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
}
class HgmFrameRateMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
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
        // param 1/2：pkg size
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, 1, {pkg0}); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, 1, {pkg1}); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, 1, {pkg2}); }));
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandlePackageEvent(i, 2, {pkg0, pkg1}); }));

        // HandleRefreshRateEvent
        testThreads.push_back(std::thread([&] () { frameRateMgr.HandleRefreshRateEvent(i, {}); }));

        // HandleTouchEvent
        // param 1: touchCnt
        testThreads.push_back(std::thread([&] () {
            frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_DOWN, "", i, 1);
        }));
        testThreads.push_back(std::thread([&] () {
            frameRateMgr.HandleTouchEvent(i, TouchStatus::TOUCH_UP, "", i, 1);
        }));

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