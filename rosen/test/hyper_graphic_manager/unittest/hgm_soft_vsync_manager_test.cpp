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

#include <gtest/gtest.h>
#include <limits>
#include <test_header.h>

#include "common/rs_common_def.h"
#include "hgm_soft_vsync_manager.h"
#include "vsync_controller.h"
#include "vsync_generator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int32_t frameRateLinkerId1 = 1;
    constexpr int32_t frameRateLinkerId2 = 2;
    constexpr int32_t frameRateLinkerId3 = 3;
    constexpr int32_t frameRateLinkerId4 = 4;
    constexpr int32_t frameRateLinkerId5 = 5;
    constexpr uint64_t windowId1 = 1;
    constexpr uint64_t windowId2 = 2;
    constexpr uint64_t windowId3 = 3;
    constexpr uint64_t windowId4 = 4;
    constexpr uint64_t windowId5 = 5;
    const std::string vsyncName1 = "vsync1";
    const std::string vsyncName2 = "vsync2";
    const std::string vsyncName3 = "vsync3";
    const std::string vsyncName4 = "vsync4";
    const std::string vsyncName5 = "vsync5";
    constexpr int32_t OLED_72_HZ = 72;
}

class HgmSoftVSyncManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    void InitHgmSoftVSyncManager(HgmSoftVSyncManager &softVSyncMgr)
    {
        auto vsyncGenerator = CreateVSyncGenerator();
        sptr<Rosen::VSyncController> rsController = new VSyncController(vsyncGenerator, 0);
        sptr<Rosen::VSyncController> appController = new VSyncController(vsyncGenerator, 0);
        sptr<VSyncDistributor> appDistributor = new VSyncDistributor(appController, "connection");
        auto controller_ = std::make_shared<HgmVSyncGeneratorController>(rsController, appController, vsyncGenerator);

        softVSyncMgr.InitController(controller_, appDistributor);
    }
    FrameRateLinkerMap InitAppFrameRateLinkers()
    {
        FrameRateRange appExpectedRange = {OLED_60_HZ, OLED_120_HZ, OLED_72_HZ};
        FrameRateLinkerMap appFrameRateLinkers;

        std::shared_ptr<RSRenderFrameRateLinker> appFrameRateLinker1 = std::make_shared<RSRenderFrameRateLinker>();
        appFrameRateLinker1->SetExpectedRange(appExpectedRange);
        appFrameRateLinker1->SetWindowNodeId(windowId1);
        appFrameRateLinkers[frameRateLinkerId1] = appFrameRateLinker1;

        std::shared_ptr<RSRenderFrameRateLinker> appFrameRateLinker2 = std::make_shared<RSRenderFrameRateLinker>();
        appFrameRateLinker2->SetExpectedRange(appExpectedRange);
        appFrameRateLinker2->SetWindowNodeId(windowId2);
        appFrameRateLinkers[frameRateLinkerId2] = appFrameRateLinker2;

        std::shared_ptr<RSRenderFrameRateLinker> appFrameRateLinker3 = std::make_shared<RSRenderFrameRateLinker>();
        appFrameRateLinker1->SetExpectedRange(appExpectedRange);
        appFrameRateLinker1->SetWindowNodeId(windowId3);
        appFrameRateLinkers[frameRateLinkerId3] = appFrameRateLinker3;

        std::shared_ptr<RSRenderFrameRateLinker> appFrameRateLinker4 = std::make_shared<RSRenderFrameRateLinker>();
        appFrameRateLinker1->SetExpectedRange(appExpectedRange);
        appFrameRateLinker1->SetWindowNodeId(windowId4);
        appFrameRateLinkers[frameRateLinkerId1] = appFrameRateLinker4;

        std::shared_ptr<RSRenderFrameRateLinker> appFrameRateLinker5 = std::make_shared<RSRenderFrameRateLinker>();
        appFrameRateLinker1->SetExpectedRange(appExpectedRange);
        appFrameRateLinker1->SetWindowNodeId(windowId5);
        appFrameRateLinkers[frameRateLinkerId5] = appFrameRateLinker5;

        return appFrameRateLinkers;
    }
};

/**
 * @tc.name: SetWindowExpectedRefreshWindowIdRateTest_window001
 * @tc.desc: Verify the result of SetWindowExpectedRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, SetWindowExpectedRefreshRate_window001, Function | SmallTest | Level1)
{
    auto softVSyncManager = HgmSoftVSyncManager();
    auto appVoteData = softVSyncManager.appVoteData_;

    FrameRateLinkerMap appFrameRateLinkers = InitAppFrameRateLinkers();

    EventInfo eventInfo1 =
        { .eventName = "VOTER_LOW", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo2 =
        { .eventName = "VOTER_MID", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo3 =
        { .eventName = "VOTER_HIGH", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo4 =
        { .eventName = "VOTER_GAMEFRAMEINSERTION", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo5 =
        { .eventName = "VOTER_VIDEOCALL", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    std::unordered_map<WindowId, EventInfo> voters = {
        {windowId1, eventInfo1}, {windowId2, eventInfo2}, {windowId3, eventInfo3}, {windowId4, eventInfo4},
        {windowId5, eventInfo5},
    };
    softVSyncManager.appFrameRateLinkers_ = appFrameRateLinkers;
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 5);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId2], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId3], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId4], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId5], 60);

    eventInfo1 =
        { .eventName = "VOTER_MID", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo2 =
        { .eventName = "VOTER_HIGH", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo3 =
        { .eventName = "VOTER_GAMEFRAMEINSERTION", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo4 =
        { .eventName = "VOTER_VIDEOCALL", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo5 =
        { .eventName = "VOTER_RATE", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    voters = {
        {windowId1, eventInfo1}, {windowId2, eventInfo2}, {windowId3, eventInfo3}, {windowId4, eventInfo4},
        {windowId5, eventInfo5},
    };
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 5);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId2], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId3], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId4], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId5], 90);
}

/**
 * @tc.name: SetWindowExpectedRefreshWindowIdRateTest_window002
 * @tc.desc: Verify the result of SetWindowExpectedRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, SetWindowExpectedRefreshRate_window002, Function | SmallTest | Level1)
{
    auto softVSyncManager = HgmSoftVSyncManager();
    auto appVoteData = softVSyncManager.appVoteData_;

    FrameRateLinkerMap appFrameRateLinkers = InitAppFrameRateLinkers();
    EventInfo eventInfo =
        { .eventName = "VOTER_LOW", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    std::unordered_map<WindowId, EventInfo> voters = { {windowId1, eventInfo} };
    softVSyncManager.appFrameRateLinkers_ = appFrameRateLinkers;
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 1);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 60);

    eventInfo =
        { .eventName = "VOTER_MID", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    voters = { {windowId1, eventInfo} };
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 1);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 90);

    voters[windowId1].eventStatus = false;
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 1);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 60);
}

/**
 * @tc.name: SetWindowExpectedRefreshWindowIdRateTest_vsync001
 * @tc.desc: Verify the result of SetWindowExpectedRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, SetWindowExpectedRefreshWindowIdRateTest_vsync001, Function | SmallTest | Level1)
{
    auto softVSyncManager = HgmSoftVSyncManager();
    auto appVoteData = softVSyncManager.appVoteData_;

    FrameRateLinkerMap appFrameRateLinkers = InitAppFrameRateLinkers();

    EventInfo eventInfo1 =
        { .eventName = "VOTER_LOW", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo2 =
        { .eventName = "VOTER_MID", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo3 =
        { .eventName = "VOTER_HIGH", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo4 =
        { .eventName = "VOTER_GAMEFRAMEINSERTION", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    EventInfo eventInfo5 =
        { .eventName = "VOTER_VIDEOCALL", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    std::unordered_map<VsyncName, EventInfo> voters = {
        {vsyncName1, eventInfo1}, {vsyncName2, eventInfo2}, {vsyncName3, eventInfo3}, {vsyncName4, eventInfo4},
        {vsyncName5, eventInfo5},
    };
    softVSyncManager.appFrameRateLinkers_ = appFrameRateLinkers;
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 5);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId2], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId3], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId4], 60);
    EXPECT_EQ(appVoteData[frameRateLinkerId5], 60);

    eventInfo1 =
        { .eventName = "VOTER_MID", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo2 =
        { .eventName = "VOTER_HIGH", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo3 =
        { .eventName = "VOTER_GAMEFRAMEINSERTION", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo4 =
        { .eventName = "VOTER_VIDEOCALL", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    eventInfo5 =
        { .eventName = "VOTER_RATE", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    voters = {
        {vsyncName1, eventInfo1}, {vsyncName2, eventInfo2}, {vsyncName3, eventInfo3}, {vsyncName4, eventInfo4},
        {vsyncName5, eventInfo5},
    };
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 5);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId2], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId3], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId4], 90);
    EXPECT_EQ(appVoteData[frameRateLinkerId5], 90);
}

/**
 * @tc.name: SetWindowExpectedRefreshWindowIdRateTest_vsync002
 * @tc.desc: Verify the result of SetWindowExpectedRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, SetWindowExpectedRefreshRate_vsync002, Function | SmallTest | Level1)
{
    auto softVSyncManager = HgmSoftVSyncManager();
    auto appVoteData = softVSyncManager.appVoteData_;

    FrameRateLinkerMap appFrameRateLinkers = InitAppFrameRateLinkers();
    EventInfo eventInfo =
        { .eventName = "VOTER_LOW", .minRefreshRate = 60, .maxRefreshRate = 60, .eventStatus = true };
    std::unordered_map<VsyncName, EventInfo> voters = { {vsyncName1, eventInfo} };
    softVSyncManager.appFrameRateLinkers_ = appFrameRateLinkers;
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 1);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 60);

    eventInfo =
        { .eventName = "VOTER_MID", .minRefreshRate = 90, .maxRefreshRate = 90, .eventStatus = true };
    voters = { {vsyncName1, eventInfo} };
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 1);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 90);

    voters[vsyncName1].eventStatus = false;
    softVSyncManager.SetWindowExpectedRefreshRate(0, voters);
    EXPECT_EQ(appVoteData.size(), 1);
    EXPECT_EQ(appVoteData[frameRateLinkerId1], 60);
}

/**
 * @tc.name: CollectVRateChange
 * @tc.desc: Verify the result of CollectVRateChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, CollectVRateChange, Function | SmallTest | Level2)
{
    HgmSoftVSyncManager mgr;
    FrameRateRange finalRange = {OLED_60_HZ, OLED_120_HZ, OLED_60_HZ};
    mgr.vRatesMap_ = {
        {0, 0},
        {1, 1},
        {2, 2}
    };
    uint64_t linkerId = 2;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);

    linkerId = 0;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);

    linkerId = 1;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);
    
    linkerId = 2;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_60_HZ);
    EXPECT_EQ(finalRange.max_, OLED_120_HZ);
    EXPECT_EQ(finalRange.preferred_, OLED_60_HZ);

    finalRange.preferred_ = 0;
    mgr.controllerRate_ = 0;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_NULL_HZ);
    EXPECT_EQ(finalRange.max_, OLED_144_HZ);
    EXPECT_EQ(finalRange.preferred_, 1);

    finalRange.preferred_ = 0;
    mgr.controllerRate_ = 100;
    mgr.CollectVRateChange(linkerId, finalRange);
    EXPECT_EQ(finalRange.min_, OLED_NULL_HZ);
    EXPECT_EQ(finalRange.max_, OLED_144_HZ);
    EXPECT_EQ(finalRange.preferred_, 50);
}

/**
 * @tc.name: GetDrawingFrameRate
 * @tc.desc: Verify the result of GetDrawingFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, GetDrawingFrameRate01, Function | SmallTest | Level2)
{
    HgmSoftVSyncManager mgr;
    uint32_t refreshRate_60 = 60;
    uint32_t refreshRate_120 = 120;
    FrameRateRange dynamic_120(0, 120, 120);
    EXPECT_EQ(dynamic_120.IsDynamic(), true);
    FrameRateRange static_120(120, 120, 120);
    EXPECT_EQ(static_120.IsDynamic(), false);

    EXPECT_EQ(mgr.GetDrawingFrameRate(refreshRate_60, dynamic_120), 60);
    EXPECT_EQ(mgr.GetDrawingFrameRate(refreshRate_60, static_120), 60);
    EXPECT_EQ(mgr.GetDrawingFrameRate(refreshRate_120, dynamic_120), 120);
    EXPECT_EQ(mgr.GetDrawingFrameRate(refreshRate_120, static_120), 120);
}

/**
 * @tc.name: GetDrawingFrameRate
 * @tc.desc: Verify the result of GetDrawingFrameRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, GetDrawingFrameRate02, Function | SmallTest | Level2)
{
    std::vector<std::pair<std::pair<uint32_t, FrameRateRange>, uint32_t>> inputAndOutput = {
        {{0, {0, 120, 60}}, 0},
        {{60, {0, 120, 0}}, 0},
        {{60, {0, 90, 120}}, 60},
        {{60, {0, 120, 120}}, 60},
        {{90, {0, 120, 30}}, 30},
        {{80, {0, 120, 30}}, 40},
        {{70, {0, 120, 30}}, 35},
        {{60, {0, 120, 30}}, 30},
        {{50, {0, 120, 30}}, 50}
    };

    for (const auto& [input, output] : inputAndOutput) {
        EXPECT_EQ(HgmSoftVSyncManager::GetDrawingFrameRate(input.first, input.second), output);
    }
}

/**
 * @tc.name: CollectFrameRateChange
 * @tc.desc: Verify the result of CollectFrameRateChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmSoftVSyncManagerTest, CollectFrameRateChangeTest, Function | SmallTest | Level2)
{
    HgmSoftVSyncManager mgr;
    InitHgmSoftVSyncManager(mgr);
    FrameRateRange finalRange = {OLED_60_HZ, OLED_120_HZ, OLED_60_HZ};
    FrameRateRange appExpectedRange = {OLED_60_HZ, OLED_120_HZ, OLED_72_HZ};
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker = std::make_shared<RSRenderFrameRateLinker>();
    std::shared_ptr<RSRenderFrameRateLinker> appFrameRateLinker = std::make_shared<RSRenderFrameRateLinker>();
    appFrameRateLinker->SetExpectedRange(appExpectedRange);

    FrameRateLinkerMap appFrameRateLinkers = {
        {frameRateLinkerId1, nullptr},
        {frameRateLinkerId2, appFrameRateLinker}
    };
    EXPECT_EQ(mgr.CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers, 0), false);
    EXPECT_GT(HgmSoftVSyncManager::GetDrawingFrameRate(OLED_60_HZ, finalRange), 0);
    EXPECT_EQ(mgr.CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers, OLED_60_HZ), true);
    mgr.controller_ = nullptr;
    EXPECT_EQ(mgr.CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers, OLED_60_HZ), false);
}
} // namespace Rosen
} // namespace OHOS