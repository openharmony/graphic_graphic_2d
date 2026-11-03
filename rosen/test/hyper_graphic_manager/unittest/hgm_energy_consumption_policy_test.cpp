/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "hgm_energy_consumption_policy.h"
#include "hgm_test_base.h"
#include "common/rs_common_hook.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int DEFAULT_MAX_FPS = 120;
constexpr int IDLE_FPS = 60;
}

class HgmEnergyConsumptionPolicyTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    void SetConfigEnable(std::string isEnable);
    void SetIdleStateEnable(bool isIdle);
};

void HgmEnergyConsumptionPolicyTest::SetConfigEnable(std::string isEnable)
{
    std::unordered_map<std::string, std::string> animationPowerConfig = {
        { "animation_energy_assurance_enable", isEnable },
        { "animation_idle_fps", "60" },
        { "animation_idle_duration", "2000" } };
    std::unordered_map<std::string, std::string> uiPowerConfig = { { "ui_animation", "30" }, { "display_sync", "31" },
    { "ace_component", "32" }, { "display_soloist", "33" } };

    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionConfig(animationPowerConfig);
    if (isEnable != "true") {
        uiPowerConfig.clear();
    }
    HgmEnergyConsumptionPolicy::Instance().SetUiEnergyConsumptionConfig(uiPowerConfig);
}

void HgmEnergyConsumptionPolicyTest::SetIdleStateEnable(bool isIdle)
{
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(isIdle);
    HgmEnergyConsumptionPolicy::Instance().isTouchIdle_ = isIdle;
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isTouchIdle_, isIdle);
}

/**
 * @tc.name: SetEnergyConsumptionConfigTest1
 * @tc.desc: test results of SetEnergyConsumptionConfigTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionConfigTest1, TestSize.Level0)
{
    SetConfigEnable("true");
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, true);
}

/**
 * @tc.name: SetEnergyConsumptionConfigTest2
 * @tc.desc: test results of SetEnergyConsumptionConfigTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionConfigTest2, TestSize.Level0)
{
    SetConfigEnable("false");
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, false);
}

/**
 * @tc.name: SetEnergyConsumptionAssuranceModeTest1
 * @tc.desc: test results of SetEnergyConsumptionAssuranceModeTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionAssuranceModeTest1, TestSize.Level0)
{
    SetConfigEnable("true");
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, true);
    SetIdleStateEnable(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isTouchIdle_, true);
    SetIdleStateEnable(false);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isTouchIdle_, false);
}

/**
 * @tc.name: SetEnergyConsumptionAssuranceModeTest2
 * @tc.desc: test results of SetEnergyConsumptionAssuranceModeTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionAssuranceModeTest2, TestSize.Level0)
{
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    SetIdleStateEnable(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isTouchIdle_, true);
}

/**
 * @tc.name: SetEnergyConsumptionAssuranceModeTest3
 * @tc.desc: test results of SetEnergyConsumptionAssuranceModeTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionAssuranceModeTest3, TestSize.Level0)
{
    SetConfigEnable("false");
    SetIdleStateEnable(false);
    SetIdleStateEnable(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, false);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isTouchIdle_, true);
}

/**
 * @tc.name: GetAnimationIdleFpsTest1
 * @tc.desc: test results of GetAnimationIdleFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetAnimationIdleFpsTest1, TestSize.Level0)
{
    SetConfigEnable("false");
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);
}

/**
 * @tc.name: GetAnimationIdleFpsTest2
 * @tc.desc: test results of GetAnimationIdleFpsTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetAnimationIdleFpsTest2, TestSize.Level0)
{
    SetConfigEnable("true");
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = 1719544263071;
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);
}

/**
 * @tc.name: GetAnimationIdleFpsTest3
 * @tc.desc: test results of GetAnimationIdleFpsTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetAnimationIdleFpsTest3, TestSize.Level0)
{
    SetConfigEnable("true");
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = 1719544261071;
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, IDLE_FPS);
    ASSERT_EQ(rsRange.min_, IDLE_FPS);
    ASSERT_EQ(rsRange.preferred_, IDLE_FPS);
}

/**
 * @tc.name: GetUiAnimationIdleFpsTest1
 * @tc.desc: test results of GetUiAnimationIdleFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetUiAnimationIdleFpsTest1, TestSize.Level0)
{
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, UI_ANIMATION_FRAME_RATE_TYPE };
    SetConfigEnable("false");
    ASSERT_FALSE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);

    SetConfigEnable("true");
    ASSERT_TRUE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, 30);
    ASSERT_EQ(rsRange.min_, 30);
    ASSERT_EQ(rsRange.preferred_, 30);
    HgmEnergyConsumptionPolicy::Instance().isTouchIdle_ = true;
    ASSERT_TRUE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
}

/**
 * @tc.name: GetDisplaySyncIdleFpsTest1
 * @tc.desc: test results of GetDisplaySyncIdleFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetDisplaySyncIdleFpsTest1, TestSize.Level0)
{
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DISPLAY_SYNC_FRAME_RATE_TYPE };
    SetConfigEnable("false");
    ASSERT_FALSE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);

    SetConfigEnable("true");
    ASSERT_TRUE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, 31);
    ASSERT_EQ(rsRange.min_, 31);
    ASSERT_EQ(rsRange.preferred_, 31);
}

/**
 * @tc.name: GetAceComponentIdleFpsTest1
 * @tc.desc: test results of GetAceComponentIdleFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetAceComponentIdleFpsTest1, TestSize.Level0)
{
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, ACE_COMPONENT_FRAME_RATE_TYPE };
    SetConfigEnable("false");
    ASSERT_FALSE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);

    SetConfigEnable("true");
    ASSERT_TRUE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, 32);
    ASSERT_EQ(rsRange.min_, 32);
    ASSERT_EQ(rsRange.preferred_, 32);
}

/**
 * @tc.name: GetDisplaySoloistIdleFpsTest1
 * @tc.desc: test results of GetDisplaySoloistIdleFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetDisplaySoloistIdleFpsTest1, TestSize.Level0)
{
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DISPLAY_SOLOIST_FRAME_RATE_TYPE };
    SetConfigEnable("false");
    ASSERT_FALSE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);

    SetConfigEnable("true");
    ASSERT_TRUE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange));
    ASSERT_EQ(rsRange.max_, 33);
    ASSERT_EQ(rsRange.min_, 33);
    ASSERT_EQ(rsRange.preferred_, 33);
}

/**
 * @tc.name: EnergyConsumptionAssureanceTest
 * @tc.desc: test drag scene
 * @tc.type: FUNC
 * @tc.require: issuesICH496
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, EnergyConsumptionAssureanceTest, TestSize.Level0)
{
    SetConfigEnable("true");
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS,
        SCROLLABLE_MULTI_TASK_FRAME_RATE_TYPE };
    EventInfo eventInfo = { .eventName = "ENERGY_CONSUMPTION_ASSURANCE", .eventStatus = false,
        .description = "DRAG_SCENE:1000" };
    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionAssuranceSceneInfo(eventInfo);
    ASSERT_FALSE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange, 1000));
    eventInfo = { .eventName = "ENERGY_CONSUMPTION_ASSURANCE", .eventStatus = true,
        .description = "TEST" };
    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionAssuranceSceneInfo(eventInfo);
    ASSERT_FALSE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange, 1000));
    eventInfo = { .eventName = "ENERGY_CONSUMPTION_ASSURANCE", .eventStatus = true,
        .description = "DRAG_SCENE:1000" };
    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionAssuranceSceneInfo(eventInfo);
    ASSERT_TRUE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange, 1000));
    eventInfo = { .eventName = "ENERGY_CONSUMPTION_ASSURANCE", .eventStatus = false,
        .description = "DRAG_SCENE:2000" };
    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionAssuranceSceneInfo(eventInfo);
    ASSERT_TRUE(HgmEnergyConsumptionPolicy::Instance().GetUiIdleFps(rsRange, 1000));
}

/**
 * @tc.name: SetRefreshRateTest
 * @tc.desc: test results of SetRefreshRateTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetRefreshRateTest, TestSize.Level0)
{
    int32_t curRefreshRateMode = -1;
    std::string curScreenStragyId = "LTPO-DEFAULT";
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.SetRefreshRateMode(curRefreshRateMode, curScreenStragyId);
    ASSERT_EQ(hgmEnergyConsumptionPolicy.curScreenStrategyId_, curScreenStragyId);
}

/**
 * @tc.name: PrintEnergyConsumptionLogTest
 * @tc.desc: test results of PrintEnergyConsumptionLogTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, PrintEnergyConsumptionLogTest, TestSize.Level0)
{
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DISPLAY_SOLOIST_FRAME_RATE_TYPE };
    rsRange.isEnergyAssurance_ = true;
    rsRange.componentScene_ = ComponentScene::SWIPER_FLING;
    HgmEnergyConsumptionPolicy::Instance().PrintEnergyConsumptionLog(rsRange);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAssuranceLog_, "NO_CONSUMPTION_ASSURANCE");
    rsRange.isEnergyAssurance_ = false;
    HgmEnergyConsumptionPolicy::Instance().PrintEnergyConsumptionLog(rsRange);
}

/**
 * @tc.name: SetTouchStateTest
 * @tc.desc: test results of SetTouchStateTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetTouchStateTest, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().SetTouchState(TouchState::DOWN_STATE);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isTouchIdle_, false);
}

/**
 * @tc.name: SetVideoCallSceneInfoTest
 * @tc.desc: test results of SetVideoCallSceneInfoTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetVideoCallSceneInfoTest, TestSize.Level0)
{
    EventInfo eventInfo = {
        .eventName = "VOTER_VIDEO_CALL",
        .maxRefreshRate = 15,
        .description = "flutterVsyncName:1234",
        .eventStatus = true,

    };
    HgmEnergyConsumptionPolicy::Instance().SetVideoCallSceneInfo(eventInfo);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoCallVsyncName_, "flutterVsyncName");
    eventInfo.description = "flutterVsyncName:0";
    HgmEnergyConsumptionPolicy::Instance().SetVideoCallSceneInfo(eventInfo);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoCallVsyncName_, "flutterVsyncName");
    EventInfo eventInfo2 = {
        .eventName = "VOTER_VIDEO_CALL",
        .maxRefreshRate = 15,
        .description = "flutterVsyncName:1234",
        .eventStatus = false,
    };
    HgmEnergyConsumptionPolicy::Instance().SetVideoCallSceneInfo(eventInfo2);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoCallVsyncName_, "");
}

/**
 * @tc.name: StatisticsVideoCallBufferCountTest
 * @tc.desc: test results of StatisticsVideoCallBufferCountTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticsVideoCallBufferCountTest, TestSize.Level0)
{
    EventInfo eventInfo = {
        .eventName = "VOTER_VIDEO_CALL",
        .maxRefreshRate = 15,
        .description = "flutterVsyncName:1234",
        .eventStatus = true,

    };
    HgmEnergyConsumptionPolicy::Instance().SetVideoCallSceneInfo(eventInfo);
    std::string bufferNamePrefix = "buffer";
    HgmEnergyConsumptionPolicy::Instance().videoCallLayerName_ = bufferNamePrefix;
    pid_t pid = 1234;
    for (int i = 0; i < 100; i++) {
        HgmEnergyConsumptionPolicy::Instance().StatisticsVideoCallBufferCount(
            pid, std::string(bufferNamePrefix) + std::to_string(i));
    }
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoBufferCount_.load(), 100);
}

/**
 * @tc.name: CheckOnlyVideoCallExistTest
 * @tc.desc: test results of CheckOnlyVideoCallExistTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, CheckOnlyVideoCallExistTest, TestSize.Level0)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.isEnableVideoCall_.store(true);
    hgmEnergyConsumptionPolicy.videoBufferCount_.store(1);
    hgmEnergyConsumptionPolicy.CheckOnlyVideoCallExist();
    ASSERT_EQ(hgmEnergyConsumptionPolicy.isSubmitDecisionTask_.load(), true);
}

/**
 * @tc.name: GetVideoCallVsyncChangeTest
 * @tc.desc: test results of GetVideoCallVsyncChangeTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallVsyncChangeTest, TestSize.Level0)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.isVideoCallVsyncChange_.store(true);
    auto result = hgmEnergyConsumptionPolicy.GetVideoCallVsyncChange();
    ASSERT_EQ(result, true);
    ASSERT_EQ(hgmEnergyConsumptionPolicy.isVideoCallVsyncChange_.load(), false);
}

/**
 * @tc.name: GetVideoCallFrameRate_PidMismatch
 * @tc.desc: Test pid != videoCallPid_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_PidMismatch, TestSize.Level0)
{
    // pid != videoCallPid_.load()
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);

    pid_t pid = 9999;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_FALSE(result);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
}

/**
 * @tc.name: GetVideoCallFrameRate_VsyncMismatch
 * @tc.desc: Test pid == videoCallPid_ && vsyncName != videoCallVsyncName_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_VsyncMismatch, TestSize.Level0)
{
    // pid == videoCallPid_.load() && vsyncName != videoCallVsyncName_
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);

    pid_t pid = 1000;
    std::string vsyncName = "wrongVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_FALSE(result);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
}

/**
 * @tc.name: GetVideoCallFrameRate_VsyncMatch
 * @tc.desc: Test pid == videoCallPid_ && vsyncName == videoCallVsyncName_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_VsyncMatch, TestSize.Level0)
{
    // pid == videoCallPid_.load() && vsyncName == videoCallVsyncName_
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
}

/**
 * @tc.name: GetVideoCallFrameRate_DisableVideoCall
 * @tc.desc: Test isVideoCall_.load() && isVideoCall && isSyncVideoCallToRp_ when !isEnableVideoCall_.load()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_DisableVideoCall, TestSize.Level0)
{
    // !isEnableVideoCall_.load()
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(false);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_FALSE(result);
    ASSERT_FALSE(policy.isVideoCall_.load());
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_OnlyVideoCallFalse
 * @tc.desc: Test isEnableVideoCall_.load() && !isOnlyVideoCallExist_.load()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_OnlyVideoCallFalse, TestSize.Level0)
{
    // isEnableVideoCall_.load() && !isOnlyVideoCallExist_.load()
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(false);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_FALSE(result);
    ASSERT_FALSE(policy.isVideoCall_.load());
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_MaxFrameRateZero
 * @tc.desc: Test videoCallMaxFrameRate_.load() == 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_MaxFrameRateZero, TestSize.Level0)
{
    // videoCallMaxFrameRate_.load() == 0
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(0);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_FALSE(result);
    ASSERT_FALSE(policy.isVideoCall_.load());
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_AvcodeDisable
 * @tc.desc: Test avcodeVideoCallEnable_.load() is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_AvcodeDisable, TestSize.Level0)
{
    // !avcodeVideoCallEnable_.load()
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(false);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_FALSE(result);
    ASSERT_FALSE(policy.isVideoCall_.load());
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_AvcodeEnable
 * @tc.desc: Test avcodeVideoCallEnable_.load() is true, all conditions met
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_AvcodeEnable, TestSize.Level0)
{
    // avcodeVideoCallEnable_.load()
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    ASSERT_TRUE(policy.isVideoCall_.load());
    ASSERT_EQ(finalRange.preferred_, 60);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_VideoCallStateChange
 * @tc.desc: Test isVideoCall_.load() != isVideoCall
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_VideoCallStateChange, TestSize.Level0)
{
    // isVideoCall_.load() != isVideoCall
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(false);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    ASSERT_FALSE(policy.isSyncVideoCallToRp_);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_VideoCallStateEqual
 * @tc.desc: Test isVideoCall_.load() == isVideoCall
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_VideoCallStateEqual, TestSize.Level0)
{
    // isVideoCall_.load() == isVideoCall
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    ASSERT_TRUE(policy.isSyncVideoCallToRp_);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_IsVideoCallFalse
 * @tc.desc: Test !isVideoCall_.load()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_IsVideoCallFalse, TestSize.Level0)
{
    // !isVideoCall_.load()
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(false);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    ASSERT_TRUE(policy.isVideoCall_.load());
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_IsVideoCallTrueNotSync
 * @tc.desc: Test isVideoCall_.load() && !isVideoCall && !isSyncVideoCallToRp_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_IsVideoCallTrueNotSync, TestSize.Level0)
{
    // isVideoCall_.load() && !isVideoCall && !isSyncVideoCallToRp_
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = false;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    ASSERT_TRUE(policy.isVideoCall_.load());
    ASSERT_FALSE(policy.isSyncVideoCallToRp_);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_FirstSyncToRp
 * @tc.desc: Test isVideoCall_.load() && isVideoCall && isSyncVideoCallToRp_ is false, first sync
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_FirstSyncToRp, TestSize.Level0)
{
    // isVideoCall_.load() && isVideoCall && !isSyncVideoCallToRp_ (first sync to RP)
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = false;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    ASSERT_TRUE(policy.isVideoCall_.load());
    ASSERT_EQ(finalRange.preferred_, 0);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallFrameRate_AlreadySynced
 * @tc.desc: Test isVideoCall_.load() && isVideoCall && isSyncVideoCallToRp_ is true, already synced
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRate_AlreadySynced, TestSize.Level0)
{
    // isVideoCall_.load() && isVideoCall && isSyncVideoCallToRp_ (already synced)
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedVideoCallVsyncName = policy.videoCallVsyncName_;
    auto savedIsEnableVideoCall = policy.isEnableVideoCall_.load();
    auto savedIsOnlyVideoCallExist = policy.isOnlyVideoCallExist_.load();
    auto savedVideoCallMaxFrameRate = policy.videoCallMaxFrameRate_.load();
    auto savedAvcodeVideoCallEnable = policy.avcodeVideoCallEnable_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(1000);
    policy.videoCallVsyncName_ = "testVsync";
    policy.isEnableVideoCall_.store(true);
    policy.isOnlyVideoCallExist_.store(true);
    policy.videoCallMaxFrameRate_.store(60);
    policy.avcodeVideoCallEnable_.store(true);
    policy.isVideoCall_.store(true);
    policy.isSyncVideoCallToRp_ = true;

    pid_t pid = 1000;
    std::string vsyncName = "testVsync";
    FrameRateRange finalRange;
    bool result = policy.GetVideoCallFrameRate(pid, vsyncName, finalRange);

    ASSERT_TRUE(result);
    ASSERT_TRUE(policy.isVideoCall_.load());
    ASSERT_EQ(finalRange.preferred_, 60);
    policy.videoCallPid_.store(savedVideoCallPid);
    policy.videoCallVsyncName_ = savedVideoCallVsyncName;
    policy.isEnableVideoCall_.store(savedIsEnableVideoCall);
    policy.isOnlyVideoCallExist_.store(savedIsOnlyVideoCallExist);
    policy.videoCallMaxFrameRate_.store(savedVideoCallMaxFrameRate);
    policy.avcodeVideoCallEnable_.store(savedAvcodeVideoCallEnable);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: GetVideoCallPid
 * @tc.desc: Test GetVideoCallPid function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallPid, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    auto savedVideoCallPid = policy.videoCallPid_.load();
    auto savedIsVideoCall = policy.isVideoCall_.load();
    auto savedIsSyncVideoCallToRp = policy.isSyncVideoCallToRp_;

    policy.videoCallPid_.store(12345);
    policy.isVideoCall_.store(true);
    pid_t result = policy.GetVideoCallPid();

    ASSERT_TRUE(policy.isSyncVideoCallToRp_);
    ASSERT_EQ(result, 12345);

    policy.videoCallPid_.store(savedVideoCallPid);
    policy.isVideoCall_.store(savedIsVideoCall);
    policy.isSyncVideoCallToRp_ = savedIsSyncVideoCallToRp;
}

/**
 * @tc.name: SetCurrentPkgNameTest
 * @tc.desc: test results of SetCurrentPkgNameTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetCurrentPkgNameTest, TestSize.Level0)
{
    auto& hgmCore = HgmCore::Instance();
    std::vector<std::string> pkgNames;
    HgmEnergyConsumptionPolicy::Instance().SetCurrentPkgName(pkgNames);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoCallLayerName_, "");
    hgmCore.InitXmlConfig();
    auto configData = hgmCore.GetPolicyConfigData();
    pkgNames.push_back("package1:");
    HgmEnergyConsumptionPolicy::Instance().SetCurrentPkgName(pkgNames);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoCallLayerName_, "");
    configData->videoCallLayerConfig_["package1"] = "value1";
    HgmEnergyConsumptionPolicy::Instance().SetCurrentPkgName(pkgNames);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoCallLayerName_, "value1");
}

/**
 * @tc.name: SetCurrentPkgNameTest2
 * @tc.desc: test videoCallLayerNameStr is not empty in for loop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetCurrentPkgNameTest2, TestSize.Level0)
{
    auto& hgmCore = HgmCore::Instance();
    hgmCore.InitXmlConfig();
    auto configData = hgmCore.GetPolicyConfigData();
    configData->videoCallLayerConfig_["package1"] = "value1";
    configData->videoCallLayerConfig_["package2"] = "value2";

    std::string originLayerName = HgmEnergyConsumptionPolicy::Instance().videoCallLayerName_;
    std::vector<std::string> pkgNames = { "package1:", "package2:" };
    HgmEnergyConsumptionPolicy::Instance().SetCurrentPkgName(pkgNames);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().videoCallLayerName_, "value1");

    HgmEnergyConsumptionPolicy::Instance().videoCallLayerName_ = originLayerName;
}

/**
 * @tc.name: HgmFrameRateManager
 * @tc.desc: test results of HgmFrameRateManager
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, HgmFrameRateManager, TestSize.Level1)
{
    HgmFrameRateManager mgr;
    mgr.rsFrameRateLinker_ = std::make_shared<RSRenderFrameRateLinker>();
    std::shared_ptr<RSRenderFrameRateLinker> linker = std::make_shared<RSRenderFrameRateLinker>();
    FrameRateLinkerMap appFrameRateLinkers_;
    mgr.currRefreshRate_ = DEFAULT_MAX_FPS;
    appFrameRateLinkers_[((NodeId)1000) << 32] = linker;
    mgr.appFrameRateLinkers_ = appFrameRateLinkers_;
    linker->SetExpectedRange(FrameRateRange(0, 0, 0,
        ACE_COMPONENT_FRAME_RATE_TYPE));
    HgmEnergyConsumptionPolicy::Instance().SetTouchState(TouchState::IDLE_STATE);
    mgr.UpdateSoftVSync(true);
    EventInfo eventInfo = { .eventName = "ENERGY_CONSUMPTION_ASSURANCE", .eventStatus = false,
        .description = "DRAG_SCENE:1000" };
    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionAssuranceSceneInfo(eventInfo);
    linker->SetExpectedRange(FrameRateRange(0, 0, 0,
        SCROLLABLE_MULTI_TASK_FRAME_RATE_TYPE));
    mgr.UpdateSoftVSync(true);
    HgmEnergyConsumptionPolicy::Instance().SetTouchState(TouchState::DOWN_STATE);
    linker->SetExpectedRange(FrameRateRange(0, 0, 0,
        ANIMATION_STATE_FIRST_FRAME));
    mgr.UpdateSoftVSync(true);
    linker->SetExpectedRange(FrameRateRange(DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS,
        ANIMATION_STATE_FIRST_FRAME));
    mgr.UpdateSoftVSync(true);
    EXPECT_EQ(mgr.currRefreshRate_, DEFAULT_MAX_FPS);
}

/**
 * @tc.name: VoterVideoCallFrameRate
 * @tc.desc: test results of VoterVideoCallFrameRate
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoCallFrameRate001, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    auto& hgmCore = HgmCore::Instance();
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);

    // frameRateMgr != nullptr
    hgmEnergyConsumptionPolicy.isVideoCallVsyncChange_.store(false);
    hgmEnergyConsumptionPolicy.VoterVideoCallFrameRate();
    ASSERT_EQ(hgmEnergyConsumptionPolicy.isVideoCallVsyncChange_.load(), true);

    // frameRateMgr == nullptr
    HgmCore::Instance().hgmFrameRateMgr_ = nullptr;
    hgmEnergyConsumptionPolicy.isVideoCallVsyncChange_.store(false);
    hgmEnergyConsumptionPolicy.VoterVideoCallFrameRate();
    ASSERT_EQ(hgmEnergyConsumptionPolicy.isVideoCallVsyncChange_.load(), false);

    HgmCore::Instance().hgmFrameRateMgr_ = frameRateMgr;
    ASSERT_NE(HgmCore::Instance().GetFrameRateMgr(), nullptr);
}

/**
 * @tc.name: StatisticAnimationTimeTest1
 * @tc.desc: Test isAnimationEnergyAssuranceEnable_ = false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest1, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(false);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, false);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData;

    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(commonData);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StatisticAnimationTimeTest2
 * @tc.desc: Test isAnimationEnergyConsumptionAssuranceMode_ = false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest2, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(false);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyConsumptionAssuranceMode_, false);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData;

    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(commonData);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StatisticAnimationTimeTest3
 * @tc.desc: Test commonData is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest3, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyConsumptionAssuranceMode_, true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData;

    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(commonData);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StatisticAnimationTimeTest4
 * @tc.desc: Test commonData not empty but no STATIC_ANIMATION_TIME key
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest4, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyConsumptionAssuranceMode_, true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "OTHER_KEY", "OTHER_VALUE" } };

    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(commonData);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StatisticAnimationTimeTest5
 * @tc.desc: Test can find STATIC_ANIMATION_TIME key
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest5, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_, true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyConsumptionAssuranceMode_, true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "STATIC_ANIMATION_TIME", "1719544265071" } };

    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(commonData);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StartNewAnimationTest1
 * @tc.desc: Test commonData is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest1, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = testTimestamp;
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData;

    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(commonData);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_.load(), testTimestamp);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StartNewAnimationTest2
 * @tc.desc: Test commonData not empty but no COMPONENT_NAME key, componentName is empty, idleFps == UNKNOWN_IDLE_FPS
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest2, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = testTimestamp;
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "OTHER_KEY", "OTHER_VALUE" } };

    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(commonData);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_.load(), testTimestamp);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StartNewAnimationTest3
 * @tc.desc: Test idleFps != UNKNOWN_IDLE_FPS, enable == true, mode == true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest3, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = testTimestamp;
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "COMPONENT_NAME", "SWIPER_FLING" } };

    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(commonData);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_.load(), testTimestamp);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StartNewAnimationTest4
 * @tc.desc: Test idleFps == UNKNOWN_IDLE_FPS, enable == true, mode == true
 * @tc.type: FUNC
 * @tc.require:
HW */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest4, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = testTimestamp;
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "COMPONENT_NAME", "OTHER_COMPONENT" } };

    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(commonData);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_.load(), testTimestamp);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StartNewAnimationTest5
 * @tc.desc: Test idleFps == UNKNOWN_IDLE_FPS, enable == false, mode == true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest5, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(false);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(true);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = testTimestamp;
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "COMPONENT_NAME", "OTHER_COMPONENT" } };

    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(commonData);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_.load(), testTimestamp);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StartNewAnimationTest6
 * @tc.desc: Test idleFps == UNKNOWN_IDLE_FPS, enable == true, mode == false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest6, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(true);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(false);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = testTimestamp;
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "COMPONENT_NAME", "OTHER_COMPONENT" } };

    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(commonData);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_.load(), testTimestamp);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: StartNewAnimationTest7
 * @tc.desc: Test idleFps == UNKNOWN_IDLE_FPS, enable == false, mode == false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest7, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().isAnimationEnergyAssuranceEnable_.store(false);
    HgmEnergyConsumptionPolicy::Instance().SetAnimationEnergyConsumptionAssuranceMode(false);
    uint64_t testTimestamp = 1719544264071;
    HgmCore::Instance().SetTimestamp(testTimestamp);
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = testTimestamp;
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = testTimestamp;
    std::unordered_map<std::string, std::string> commonData = { { "COMPONENT_NAME", "OTHER_COMPONENT" } };

    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(commonData);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_.load(), testTimestamp);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_.load(), testTimestamp);
}

/**
 * @tc.name: HandleEnergyCommonDataTest1
 * @tc.desc: Test commonData is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, HandleEnergyCommonDataTest1, TestSize.Level0)
{
    EnergyCommonDataMap commonData;
    HgmEnergyConsumptionPolicy::Instance().HandleEnergyCommonData(commonData);
    ASSERT_TRUE(commonData.empty());
}

/**
 * @tc.name: HandleEnergyCommonDataTest2
 * @tc.desc: Test commonData not empty and energyDataFunc != commonDataMapFunc_.end()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, HandleEnergyCommonDataTest2, TestSize.Level0)
{
    EnergyCommonDataMap commonData;
    std::unordered_map<std::string, std::string> testData = { { "TEST_KEY", "TEST_VALUE" } };
    commonData[EnergyEvent::START_NEW_ANIMATION] = testData;

    HgmEnergyConsumptionPolicy::Instance().HandleEnergyCommonData(commonData);
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: HandleEnergyCommonDataTest3
 * @tc.desc: Test commonData not empty and energyDataFunc == commonDataMapFunc_.end()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, HandleEnergyCommonDataTest3, TestSize.Level0)
{
    EnergyCommonDataMap commonData;
    std::unordered_map<std::string, std::string> testData = { { "INVALID_KEY", "INVALID_VALUE" } };
    commonData[static_cast<EnergyEvent>(999)] = testData; // Use an invalid event type

    HgmEnergyConsumptionPolicy::Instance().HandleEnergyCommonData(commonData);
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: VoterVideoFrameRateTest1
 * @tc.desc: Test pidIter == commonData.end()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoFrameRateTest1, TestSize.Level0)
{
    std::unordered_map<std::string, std::string> commonData;
    HgmEnergyConsumptionPolicy::Instance().VoterVideoFrameRate(commonData);
    ASSERT_TRUE(commonData.empty());
}

/**
 * @tc.name: VoterVideoFrameRateTest2
 * @tc.desc: Test !XMLParser::IsNumber(pidIter->second)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoFrameRateTest2, TestSize.Level0)
{
    std::unordered_map<std::string, std::string> commonData = { { "PID", "NOT_A_NUMBER" } };
    HgmEnergyConsumptionPolicy::Instance().VoterVideoFrameRate(commonData);
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: VoterVideoFrameRateTest3
 * @tc.desc: Test eventNameIter == commonData.end()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoFrameRateTest3, TestSize.Level0)
{
    std::unordered_map<std::string, std::string> commonData = { { "PID", "12345" } };
    HgmEnergyConsumptionPolicy::Instance().VoterVideoFrameRate(commonData);
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: VoterVideoFrameRateTest4
 * @tc.desc: Test eventStatusIter == commonData.end()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoFrameRateTest4, TestSize.Level0)
{
    std::unordered_map<std::string, std::string> commonData = { { "PID", "12345" }, { "EVENT_NAME", "TEST_EVENT" } };
    HgmEnergyConsumptionPolicy::Instance().VoterVideoFrameRate(commonData);
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: VoterVideoFrameRateTest5
 * @tc.desc: Test refreshRateIter == commonData.end()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoFrameRateTest5, TestSize.Level0)
{
    std::unordered_map<std::string, std::string> commonData = { { "PID", "12345" }, { "EVENT_NAME", "TEST_EVENT" },
        { "EVENT_STATUS", "true" } };
    HgmEnergyConsumptionPolicy::Instance().VoterVideoFrameRate(commonData);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: VoterVideoFrameRateTest6
 * @tc.desc: Test !XMLParser::IsNumber(refreshRateIter->second)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoFrameRateTest6, TestSize.Level0)
{
    std::unordered_map<std::string, std::string> commonData = { { "PID", "12345" }, { "EVENT_NAME", "TEST_EVENT" },
        { "EVENT_STATUS", "true" }, { "REFRESH_RATE", "NOT_A_NUMBER" } };
    HgmEnergyConsumptionPolicy::Instance().VoterVideoFrameRate(commonData);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: VoterVideoFrameRateTest7
 * @tc.desc: Test  XMLParser::IsNumber(refreshRateIter->second)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, VoterVideoFrameRateTest7, TestSize.Level0)
{
    std::unordered_map<std::string, std::string> commonData = { { "PID", "12345" }, { "EVENT_NAME", "TEST_EVENT" },
        { "EVENT_STATUS", "true" }, { "REFRESH_RATE", "60" } };
    HgmEnergyConsumptionPolicy::Instance().VoterVideoFrameRate(commonData);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    ASSERT_FALSE(commonData.empty());
}

/**
 * @tc.name: NotifyVideoParamsTest1
 * @tc.desc: Test missing "pid" key triggers early return
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest1, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    std::unordered_map<std::string, std::string> videoRateInfo = { { "decRate", "30" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), true);
}

/**
 * @tc.name: NotifyVideoParamsTest2
 * @tc.desc: Test "pid" value not a number triggers early return
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest2, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "not_a_number" }, { "decRate", "30" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), true);
}

/**
 * @tc.name: NotifyVideoParamsTest3
 * @tc.desc: Test missing "decRate" key triggers early return
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest3, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "1234" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), true);
}

/**
 * @tc.name: NotifyVideoParamsTest4
 * @tc.desc: Test "decRate" value not a number triggers early return
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest4, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "1234" }, { "decRate", "not_a_number" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), true);
}

/**
 * @tc.name: NotifyVideoParamsTest5
 * @tc.desc: Test valid input with isEnableVideoCall_=false triggers early return
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest5, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    policy.isEnableVideoCall_.store(false);
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "1234" }, { "decRate", "30" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.videoCallPid_.store(1234);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), true);
    policy.isEnableVideoCall_.store(true);
}

/**
 * @tc.name: NotifyVideoParamsTest6
 * @tc.desc: Test valid input with pid != videoCallPid_ triggers early return
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest6, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    policy.isEnableVideoCall_.store(true);
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "1234" }, { "decRate", "30" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.videoCallPid_.store(5678);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), true);
}

/**
 * @tc.name: NotifyVideoParamsTest7
 * @tc.desc: Test decRate < maxFps + FPS_MARGIN sets avcodeVideoCallEnable to true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest7, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    policy.isEnableVideoCall_.store(true);
    pid_t testPid = 1234;
    policy.videoCallPid_.store(testPid);
    policy.videoCallMaxFrameRate_.store(30);
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "1234" }, { "decRate", "25" } };
    policy.avcodeVideoCallEnable_.store(false);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), true);
}

/**
 * @tc.name: NotifyVideoParamsTest8
 * @tc.desc: Test decRate >= maxFps + FPS_MARGIN sets avcodeVideoCallEnable to false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest8, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    policy.isEnableVideoCall_.store(true);
    pid_t testPid = 1234;
    policy.videoCallPid_.store(testPid);
    policy.videoCallMaxFrameRate_.store(30);
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "1234" }, { "decRate", "40" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), false);
}

/**
 * @tc.name: NotifyVideoParamsTest9
 * @tc.desc: Test decRate == maxFps + FPS_MARGIN (boundary) sets avcodeVideoCallEnable to false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, NotifyVideoParamsTest9, TestSize.Level0)
{
    auto& policy = HgmEnergyConsumptionPolicy::Instance();
    policy.isEnableVideoCall_.store(true);
    pid_t testPid = 1234;
    policy.videoCallPid_.store(testPid);
    policy.videoCallMaxFrameRate_.store(30);
    std::unordered_map<std::string, std::string> videoRateInfo = { { "pid", "1234" }, { "decRate", "33" } };
    policy.avcodeVideoCallEnable_.store(true);
    policy.NotifyVideoParams(videoRateInfo);
    ASSERT_EQ(policy.avcodeVideoCallEnable_.load(), false);
}
} // namespace Rosen
} // namespace OHOS