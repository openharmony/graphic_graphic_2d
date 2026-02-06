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
 * @tc.name: StatisticAnimationTimeTest1
 * @tc.desc: test results of StatisticAnimationTimeTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest1, TestSize.Level1)
{
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 0;
    SetConfigEnable("false");
    SetIdleStateEnable(false);
    uint64_t currentTime = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(currentTime);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_, currentTime);
}

/**
 * @tc.name: StatisticAnimationTimeTest2
 * @tc.desc: test results of StatisticAnimationTimeTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest2, TestSize.Level1)
{
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 0;
    SetConfigEnable("true");
    SetIdleStateEnable(false);
    uint64_t currentTime = 1719544264072;
    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(currentTime);
    ASSERT_NE(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_, currentTime);
}

/**
 * @tc.name: StatisticAnimationTimeTest3
 * @tc.desc: test results of StatisticAnimationTimeTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest3, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 0;
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    uint64_t currentTime = 1719544264073;
    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(currentTime);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_, currentTime);
}

/**
 * @tc.name: StartNewAnimationTest1
 * @tc.desc: test results of StartNewAnimationTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest1, TestSize.Level0)
{
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 0;
    SetConfigEnable("true");
    std::string componentName = "SWIPER_FLING";
    HgmCore::Instance().SetTimestamp(0);
    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation(componentName);
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_, 0);
}

/**
 * @tc.name: StartNewAnimationTest2
 * @tc.desc: test results of StartNewAnimationTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest2, TestSize.Level1)
{
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 0;
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    std::string componentName = "";
    HgmCore::Instance().SetActualTimestamp(0);
    for (int i = 0; i < 2000; i++) {
        RsCommonHook::Instance().OnStartNewAnimation(componentName);
    }
    ASSERT_EQ(HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_,
        HgmCore::Instance().GetActualTimestamp() / NS_PER_MS);
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
 * @tc.name: GetCommponentFpsTest1
 * @tc.desc: test results of GetCommponentFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetCommponentFpsTest1, TestSize.Level0)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.isTouchIdle_ = true;
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DISPLAY_SOLOIST_FRAME_RATE_TYPE };
    hgmEnergyConsumptionPolicy.GetComponentFps(rsRange);
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);
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
 * @tc.name: GetVideoCallFrameRateTest
 * @tc.desc: test results of GetVideoCallFrameRateTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetVideoCallFrameRateTest, TestSize.Level0)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    EventInfo eventInfo = {
        .eventName = "VOTER_VIDEO_CALL",
        .maxRefreshRate = 15,
        .description = "flutterVsyncName:1234",
        .eventStatus = true,

    };
    hgmEnergyConsumptionPolicy.SetVideoCallSceneInfo(eventInfo);
    std::string vsyncName = "flutterVsyncName";
    pid_t pid = 1234;
    FrameRateRange frameRateRange;
    hgmEnergyConsumptionPolicy.GetVideoCallFrameRate(pid, vsyncName, frameRateRange);
    ASSERT_EQ(frameRateRange.preferred_, 0);
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
 * @tc.name: SetComponentDefaultFpsInfo001
 * @tc.desc: Test when energyInfo_.componentName == "" && !eventInfo.eventStatus
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo001, TestSize.Level1)
{
    EventInfo eventInfo;
    eventInfo.eventStatus = false;
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_EQ(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo002
 * @tc.desc: Test when energyInfo_.componentName == "" && currentRefreshMode_ != HGM_REFRESHRATE_MODE_AUTO
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo002, TestSize.Level1)
{
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = 1;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_EQ(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "");
}


/**
 * @tc.name: SetComponentDefaultFpsInfo003
 * @tc.desc: Test when energyInfo_.componentName != "" && !eventInfo.eventStatus
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo003, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "TestComponent";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    EventInfo eventInfo;
    eventInfo.eventStatus = false;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_EQ(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo004
 * @tc.desc: Test when energyInfo_.componentName != "" && currentRefreshMode_ != HGM_REFRESHRATE_MODE_AUTO
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo004, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "TestComponent";
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = 1;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_EQ(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo005
 * @tc.desc: eventInfo.maxRefreshRate != 0
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo005, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "TestComponent:1234";
    eventInfo.maxRefreshRate = 60;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_NE(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo006
 * @tc.desc: Test when eventInfo.eventStatus && energyInfo_.componentName != "", pid == 0, eventInfo.maxRefreshRate != 0
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo006, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "TestComponent";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "TestComponent:0";
    eventInfo.maxRefreshRate = 60;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_EQ(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "TestComponent");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo007
 * @tc.desc: Test when eventInfo.maxRefreshRate == 0
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo007, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "TestComponent";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "TestComponent:1234";
    eventInfo.maxRefreshRate = 0;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_EQ(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "TestComponent");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo008
 * @tc.desc: Test when eventInfo.maxRefreshRate == 0
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo008, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "TestComponent";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "TestComponent:0";
    eventInfo.maxRefreshRate = 0;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_EQ(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "TestComponent");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo009
 * @tc.desc: Test when syncEnergyInfoFunc_ == nullptr
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo009, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    hgmEnergyConsumptionPolicy.syncEnergyInfoFunc_ = nullptr;
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "TestComponent:1234";
    eventInfo.maxRefreshRate = 60;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_NE(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "");
}

/**
 * @tc.name: SetComponentDefaultFpsInfo010
 * @tc.desc: Test when syncEnergyInfoFunc_ != nullptr
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetComponentDefaultFpsInfo010, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    auto syncFunc = [](const EnergyInfo& info) {};
    hgmEnergyConsumptionPolicy.syncEnergyInfoFunc_ = syncFunc;
    EventInfo eventInfo;
    eventInfo.eventStatus = true;
    eventInfo.description = "TestComponent:1234";
    eventInfo.maxRefreshRate = 60;
    hgmEnergyConsumptionPolicy.SetComponentDefaultFpsInfo(eventInfo);
    EXPECT_NE(hgmEnergyConsumptionPolicy.energyInfo_.componentName, "");
}

/**
 * @tc.name: DisableTouchHighFrameRate001
 * @tc.desc: Test when currentRefreshMode_ != HGM_REFRESHRATE_MODE_AUTO
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, DisableTouchHighFrameRate001, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = 1;
    FrameRateRange finalRange;
    finalRange.componentScene_ = ComponentScene::SWIPER_FLING;
    finalRange.type_ = SWIPER_DRAG_FRAME_RATE_TYPE;

    bool result = hgmEnergyConsumptionPolicy.DisableTouchHighFrameRate(finalRange);
    EXPECT_FALSE(result);
}


/**
 * @tc.name: DisableTouchHighFrameRate002
 * @tc.desc: Test when energyInfo_.componentName != "swiper_drag_scene"
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, DisableTouchHighFrameRate002, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "TestComponent";
    FrameRateRange finalRange;
    finalRange.componentScene_ = ComponentScene::SWIPER_FLING;
    finalRange.type_ = SWIPER_DRAG_FRAME_RATE_TYPE;

    bool result = hgmEnergyConsumptionPolicy.DisableTouchHighFrameRate(finalRange);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DisableTouchHighFrameRate003
 * @tc.desc: Test when finalRange.type_ & SWIPER_DRAG_FRAME_RATE_TYPE == 0
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, DisableTouchHighFrameRate003, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "swiper_drag_scene";
    FrameRateRange finalRange;
    finalRange.componentScene_ = ComponentScene::SWIPER_FLING;
    finalRange.type_ = 0;

    bool result = hgmEnergyConsumptionPolicy.DisableTouchHighFrameRate(finalRange);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DisableTouchHighFrameRate004
 * @tc.desc: Test when finalRange.type_ & SWIPER_DRAG_FRAME_RATE_TYPE == 1
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, DisableTouchHighFrameRate004, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "swiper_drag_scene";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    FrameRateRange finalRange;
    finalRange.componentScene_ = ComponentScene::UNKNOWN_SCENE;
    finalRange.type_ = SWIPER_DRAG_FRAME_RATE_TYPE;

    bool result = hgmEnergyConsumptionPolicy.DisableTouchHighFrameRate(finalRange);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DisableTouchHighFrameRate005
 * @tc.desc: Test when finalRange.type_ & SWIPER_DRAG_FRAME_RATE_TYPE == 0
 * @tc.type: FUNC
 * @tc.require:issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, DisableTouchHighFrameRate005, TestSize.Level1)
{
    auto& hgmEnergyConsumptionPolicy = HgmEnergyConsumptionPolicy::Instance();
    hgmEnergyConsumptionPolicy.energyInfo_.componentName = "swiper_drag_scene";
    hgmEnergyConsumptionPolicy.currentRefreshMode_ = HGM_REFRESHRATE_MODE_AUTO;
    FrameRateRange finalRange;
    finalRange.componentScene_ = ComponentScene::UNKNOWN_SCENE;
    finalRange.type_ = 0;

    bool result = hgmEnergyConsumptionPolicy.DisableTouchHighFrameRate(finalRange);
    EXPECT_FALSE(result);
}
} // namespace Rosen
} // namespace OHOS