/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "platform/ohos/rs_jank_stats.h"
#include "hisysevent.h"
#include "common/rs_common_def.h"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const int64_t ASSERTION_MIN = 99;
const int64_t ASSERTION_MAX = 2988;
class RSJankStatsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSJankStatsTest::SetUpTestCase() {}
void RSJankStatsTest::TearDownTestCase() {}
void RSJankStatsTest::SetUp() {}
void RSJankStatsTest::TearDown() {}

void SetRSJankStatsTest(std::shared_ptr<RSJankStats> rsJankStats, int64_t rtEndTimeSteady,
    uint32_t dynamicRefreshRate = 0, bool skipJankStats = false)
{
    // dynamicRefreshRate is retained for future algorithm adjustment, keep it unused currently
    rsJankStats->isNeedReportJankStats_ = false;
    rsJankStats->rtEndTimeSteady_ = rtEndTimeSteady;
    rsJankStats->SetRSJankStats(skipJankStats, dynamicRefreshRate);
    if (ASSERTION_MIN < rtEndTimeSteady && rtEndTimeSteady < ASSERTION_MAX) {
        EXPECT_TRUE(rsJankStats->isNeedReportJankStats_);
    }
}

/**
 * @tc.name: SetEndTimeTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetEndTimeTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    rsJankStats.SetEndTime(false);
    rsJankStats.SetStartTime();
    rsJankStats.SetOnVsyncStartTime(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    pid_t appPid = 1;
    rsJankStats.SetAppFirstFrame(appPid);
    rsJankStats.SetEndTime(true);
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: UpdateEndTimeTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, UpdateEndTimeTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    rsJankStats.isFirstSetEnd_ = false;
    rsJankStats.UpdateEndTime();
    rsJankStats.isFirstSetEnd_ = true;
    rsJankStats.UpdateEndTime();
    ASSERT_NE(rsJankStats.isFirstSetEnd_, false);
}

/**
 * @tc.name: HandleDirectCompositionTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, HandleDirectCompositionTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    rsJankStats.SetStartTime();
    rsJankStats.SetEndTime(true);
    rsJankStats.SetOnVsyncStartTime(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    rsJankStats.SetEndTime(false);
    rsJankStats.SetStartTime();
    JankDurationParams rsParams;
    rsParams.timeStart_ = rsJankStats.GetCurrentSystimeMs();
    rsParams.timeStartSteady_ = rsJankStats.GetCurrentSteadyTimeMs();
    rsParams.timeEnd_ = rsJankStats.GetCurrentSystimeMs();
    rsParams.timeEndSteady_ = rsJankStats.GetCurrentSteadyTimeMs();
    rsJankStats.HandleDirectComposition(rsParams, true);
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: UpdateJankFrameTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, ReportJankStatsTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    rsJankStats.SetStartTime();
    rsJankStats.SetEndTime(false);
    rsJankStats.ReportJankStats();
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: SetReportEventResponseTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportEventResponseTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    DataBaseRs info1;
    info1.uniqueId = 0;
    rsJankStats.SetReportEventResponse(info1);
    rsJankStats.SetReportEventResponse(info1);
    info1.uniqueId = 1;
    rsJankStats.SetReportEventResponse(info1);
    rsJankStats.SetStartTime();
    rsJankStats.SetEndTime(false);
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: SetReportEventCompleteTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportEventCompleteTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    DataBaseRs info1;
    info1.uniqueId = 2;
    rsJankStats.SetReportEventComplete(info1);
    rsJankStats.SetReportEventComplete(info1);
    info1.uniqueId = 3;
    rsJankStats.SetReportEventComplete(info1);
    rsJankStats.SetStartTime();
    rsJankStats.SetEndTime(false);
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: SetReportSceneJankStatsTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportSceneJankStatsTest001, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    rsJankStats->lastSceneReportTime_ = 1;
    rsJankStats->lastSceneReportTimeSteady_ = 1;
    rsJankStats->lastSceneJankFrame6FreqTimeSteady_ = 1;
    rsJankStats->isNeedReportSceneJankStats_ = true;

    AppInfo appInfo;
    appInfo.bundleName = "com.rs.scene.test";
    appInfo.pid = 1;
    appInfo.startTime = 1;
    rsJankStats->appInfo_ = appInfo;
    rsJankStats->SetReportRsSceneJankStart(appInfo);
    EXPECT_FALSE(rsJankStats->isLastReportSceneDone_);
    appInfo.endTime = 1;
    rsJankStats->SetReportRsSceneJankEnd(appInfo);
    EXPECT_EQ((rsJankStats->appInfo_).startTime, 0);
    EXPECT_EQ((rsJankStats->appInfo_).endTime, 0);
    EXPECT_EQ((rsJankStats->appInfo_).pid, 0);
    EXPECT_TRUE(rsJankStats->isLastReportSceneDone_);
}

/**
 * @tc.name: SetReportSceneJankStatsTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportSceneJankStatsTest002, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    rsJankStats->lastSceneReportTime_ = 1;
    rsJankStats->lastSceneReportTimeSteady_ = 1;
    rsJankStats->lastSceneJankFrame6FreqTimeSteady_ = 1;
    rsJankStats->isNeedReportSceneJankStats_ = true;

    AppInfo appInfo;
    appInfo.bundleName = "com.rs.scene.test";
    appInfo.pid = 1;
    appInfo.startTime = 1;
    appInfo.endTime = 1;
    rsJankStats->appInfo_ = appInfo;
    rsJankStats->ReportSceneJankStats(rsJankStats->appInfo_);
    EXPECT_EQ((rsJankStats->appInfo_).startTime, 0);
    EXPECT_EQ((rsJankStats->appInfo_).endTime, 0);
    EXPECT_EQ((rsJankStats->appInfo_).pid, 0);
    EXPECT_TRUE(rsJankStats->isLastReportSceneDone_);
}

/**
 * @tc.name: ReportSceneJankFrameTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, ReportSceneJankFrameTest, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    rsJankStats->rtEndTimeSteady_ = 100;
    rsJankStats->rtLastEndTimeSteady_ = 10;
    rsJankStats->rsStartTimeSteady_ = 20;
    EXPECT_EQ(rsJankStats->GetEffectiveFrameTime(true), 80);

    AppInfo appInfo;
    appInfo.bundleName = "com.rs.scene.test";
    appInfo.pid = 1;
    appInfo.startTime = 1;
    appInfo.endTime = 1;
    rsJankStats->appInfo_ = appInfo;
    rsJankStats->accumulatedBufferCount_ = 12;
    rsJankStats->ReportSceneJankFrame(0);
    EXPECT_NE((rsJankStats->appInfo_).startTime, 0);
    EXPECT_NE((rsJankStats->appInfo_).endTime, 0);
    EXPECT_NE((rsJankStats->appInfo_).pid, 0);
}

/**
 * @tc.name: SetReportEventJankFrameTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportEventJankFrameTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    DataBaseRs info1;
    info1.uniqueId = 1;
    rsJankStats.SetReportEventJankFrame(info1, false);
    info1.uniqueId = 4;
    rsJankStats.SetReportEventJankFrame(info1, true);
    rsJankStats.SetStartTime();
    rsJankStats.SetEndTime(false);
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: ConvertTimeToSystimeTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, ConvertTimeToSystimeTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    rsJankStats.SetStartTime();
    DataBaseRs info1;
    info1.uniqueId = 0;
    rsJankStats.SetReportEventResponse(info1);
    usleep(50 * 1000);
    rsJankStats.SetEndTime(false);
    usleep(100 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_6_FREQ
    rsJankStats.SetReportEventResponse(info1);
    usleep(240 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_15_FREQ
    usleep(300 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_20_FREQ
    usleep(500 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_36_FREQ
    usleep(700 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_48_FREQ
    usleep(900 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_60_FREQ
    usleep(1900 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_120_FREQ
    usleep(2900 * 1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_180_FREQ
    usleep(3000 * 1000);
    rsJankStats.SetEndTime(false); // jank frames skip more than 180
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: RecordJankFrameSingleTest
 * @tc.desc: test RecordJankFrameSingle
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSJankStatsTest, RecordJankFrameSingleTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    JankFrameRecordStats recordStats = {"test", 1};
    recordStats.isRecorded_ = true;
    rsJankStats.RecordJankFrameSingle(1, recordStats);
    recordStats.isRecorded_ = false;
    rsJankStats.RecordJankFrameSingle(1, recordStats);
    rsJankStats.RecordJankFrameSingle(0, recordStats);
    ASSERT_EQ(recordStats.isRecorded_, true);
}

/**
 * @tc.name: ReportEventCompleteTest
 * @tc.desc: test ReportEventComplete
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSJankStatsTest, ReportEventCompleteTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStats::GetInstance();
    DataBaseRs info1;
    info1.uniqueId = 1;
    rsJankStats.SetReportEventResponse(info1);
    rsJankStats.SetReportEventComplete(info1);
    rsJankStats.SetStartTime();
    rsJankStats.SetEndTime(false, true);
    EXPECT_FALSE(rsJankStats.isFirstSetStart_);
}

/**
 * @tc.name: SetRSJankStatsTest001
 * @tc.desc: SetRSJankStats and GetEffectiveFrameTime test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetRSJankStatsTest001, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    rsJankStats->rtLastEndTimeSteady_ = 0;
    rsJankStats->rsStartTimeSteady_ = 0;
    // isCurrentFrameSwitchToNotDoDirectComposition_ is false
    rsJankStats->rtEndTimeSteady_ = 1;
    EXPECT_EQ(rsJankStats->GetEffectiveFrameTime(false), 1);
    // isCurrentFrameSwitchToNotDoDirectComposition_ is true
    rsJankStats->isCurrentFrameSwitchToNotDoDirectComposition_ = true;
    EXPECT_EQ(rsJankStats->GetEffectiveFrameTime(false), 1);
    // isConsiderRsStartTime is true
    EXPECT_EQ(rsJankStats->GetEffectiveFrameTime(true), 1);

    // dynamicRefreshRate is retained for future algorithm adjustment, keep it unused currently
    uint32_t dynamicRefreshRate = 0;
    // isConsiderRsStartTime is true
    rsJankStats->SetRSJankStats(true, dynamicRefreshRate);
    // SetRSJankStatsTest is missedVsync = 0
    rsJankStats->rsJankStats_[0] = USHRT_MAX;
    SetRSJankStatsTest(rsJankStats, 1);
    // SetRSJankStatsTest is missedVsync = 1
    SetRSJankStatsTest(rsJankStats, 17);
    // SetRSJankStatsTest is missedVsync = 10
    SetRSJankStatsTest(rsJankStats, 166);
    // SetRSJankStatsTest is missedVsync = 15
    SetRSJankStatsTest(rsJankStats, 249);
    // SetRSJankStatsTest is missedVsync = 20
    SetRSJankStatsTest(rsJankStats, 332);
    // SetRSJankStatsTest is missedVsync = 36
    SetRSJankStatsTest(rsJankStats, 598);
    // SetRSJankStatsTest is missedVsync = 48
    SetRSJankStatsTest(rsJankStats, 797);
    // SetRSJankStatsTest is missedVsync = 60
    SetRSJankStatsTest(rsJankStats, 996);
    // SetRSJankStatsTest is missedVsync = 120
    SetRSJankStatsTest(rsJankStats, 1992);
    // SetRSJankStatsTest is missedVsync = 180
    SetRSJankStatsTest(rsJankStats, 2988);
}

/**
 * @tc.name: ReportJankStatsTest002
 * @tc.desc: ReportJankStats and HandleImplicitAnimationEndInAdvance test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, ReportJankStatsTest002, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    // ReportJankStats test
    rsJankStats->ReportJankStats();

    rsJankStats->lastReportTime_ = 1;
    rsJankStats->lastReportTimeSteady_ = 1;
    rsJankStats->lastJankFrame6FreqTimeSteady_ = 1;
    rsJankStats->ReportJankStats();

    EXPECT_EQ(rsJankStats->lastJankFrame6FreqTimeSteady_, -1);
    rsJankStats->isNeedReportJankStats_ = true;
    rsJankStats->ReportJankStats();
    EXPECT_FALSE(rsJankStats->isNeedReportJankStats_);

    // HandleImplicitAnimationEndInAdvance test
    JankFrames jankFrames;
    jankFrames.isDisplayAnimator_ = true;
    rsJankStats->HandleImplicitAnimationEndInAdvance(jankFrames, false);

    jankFrames.isDisplayAnimator_ = false;
    jankFrames.isSetReportEventComplete_ = true;
    jankFrames.isSetReportEventJankFrame_ = true;
    rsJankStats->HandleImplicitAnimationEndInAdvance(jankFrames, false);
    EXPECT_FALSE(jankFrames.isSetReportEventComplete_);
    EXPECT_FALSE(jankFrames.isSetReportEventJankFrame_);

    jankFrames.isSetReportEventJankFrame_ = true;
    rsJankStats->HandleImplicitAnimationEndInAdvance(jankFrames, false);
    EXPECT_FALSE(jankFrames.isSetReportEventJankFrame_);
}

/**
 * @tc.name: SetImplicitAnimationEndTest003
 * @tc.desc: SetImplicitAnimationEnd ReportEventJankFrame ReportEventHitchTimeRatio ReportEventFirstFrame test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetImplicitAnimationEnd003, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    // SetImplicitAnimationEnd test
    rsJankStats->SetImplicitAnimationEnd(false);
    JankFrames jankFramesTest1;
    jankFramesTest1.isDisplayAnimator_ = true;
    JankFrames jankFramesTest2;
    rsJankStats->animateJankFrames_.emplace(std::make_pair(1, ""), jankFramesTest1);
    rsJankStats->animateJankFrames_.emplace(std::make_pair(2, ""), jankFramesTest2);
    rsJankStats->SetImplicitAnimationEnd(true);

    // ReportEventJankFrame and ReportEventHitchTimeRatio test
    JankFrames jankFramesTest3;
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.lastTotalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, true);
    jankFramesTest3.lastTotalFrameTimeSteadyForHTR_ = 1;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, true);

    jankFramesTest3.totalFrames_ = 0;
    rsJankStats->ReportEventJankFrame(jankFramesTest3, false);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.totalFrames_ = 1;
    rsJankStats->ReportEventJankFrame(jankFramesTest3, false);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.totalFrames_ = 0;
    rsJankStats->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, true);
    jankFramesTest3.totalFrames_ = 1;
    rsJankStats->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats->ReportEventHitchTimeRatio(jankFramesTest3, true);

    // ReportEventFirstFrame test
    rsJankStats->firstFrameAppPids_.emplace(1);
    EXPECT_EQ(rsJankStats->firstFrameAppPids_.size(), 1);
    rsJankStats->ReportEventFirstFrame();
    EXPECT_EQ(rsJankStats->firstFrameAppPids_.size(), 0);
}

/**
 * @tc.name: RecordJankFrameTest004
 * @tc.desc: RecordJankFrame and RecordJankFrameSingle test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, RecordJankFrameTest004, TestSize.Level1)
{
    // RecordJankFrame test
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    rsJankStats->accumulatedBufferCount_ = 6;
    rsJankStats->rtEndTimeSteady_ = 266;
    rsJankStats->rtLastEndTimeSteady_ = 0;
    rsJankStats->rsStartTimeSteady_ = 0;
    rsJankStats->explicitAnimationTotal_ = 1;
    rsJankStats->isFirstSetEnd_ = true;
    rsJankStats->implicitAnimationTotal_ = 1;
    rsJankStats->RecordJankFrame(0);

    // RecordJankFrameSingle test
    JankFrameRecordStats recordStats = { "test", 1 };
    recordStats.isRecorded_ = true;
    rsJankStats->RecordJankFrameSingle(1, recordStats);
}

/**
 * @tc.name: RecordAnimationDynamicFrameRateTest005
 * @tc.desc: RecordAnimationDynamicFrameRate test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSJankStatsTest, RecordAnimationDynamicFrameRateTest005, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    jankFrames.isFrameRateRecorded_ = true;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);

    jankFrames.isFrameRateRecorded_ = false;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, true);
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);

    jankFrames.traceId_ = -1;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.traceId_ = 0;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, true);

    jankFrames.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.totalFrames_ = 1;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.lastTotalFrameTimeSteadyForHTR_ = 1;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, true);
    jankFrames.lastTotalFrames_ = 1;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, true);

    jankFrames.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.lastTotalFrameTimeSteadyForHTR_ = 0;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, true);

    jankFrames.totalFrameTimeSteadyForHTR_ = 1;
    jankFrames.isFrameRateRecorded_ = false;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, false);
    EXPECT_TRUE(jankFrames.isFrameRateRecorded_);
    jankFrames.lastTotalFrameTimeSteadyForHTR_ = 1;
    jankFrames.isFrameRateRecorded_ = false;
    rsJankStats->RecordAnimationDynamicFrameRate(jankFrames, true);
    EXPECT_TRUE(jankFrames.isFrameRateRecorded_);
}

/**
 * @tc.name: SetAnimationTraceBeginTest006
 * @tc.desc: SetAnimationTraceBegin test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetAnimationTraceBeginTest006, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    std::pair<int64_t, std::string> animationId = { 0, "test" };
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->implicitAnimationTotal_, 1);

    jankFrames.info_.isDisplayAnimator = true;
    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->explicitAnimationTotal_, 1);
}

/**
 * @tc.name: SetAnimationTraceEndTest007
 * @tc.desc: SetAnimationTraceEnd test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetAnimationTraceEndTest007, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    rsJankStats->SetAnimationTraceEnd(jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats->SetAnimationTraceEnd(jankFrames);

    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->animationAsyncTraces_.at(0).isDisplayAnimator_ = false;
    rsJankStats->implicitAnimationTotal_ = 1;
    rsJankStats->SetAnimationTraceEnd(jankFrames);
    EXPECT_EQ(rsJankStats->implicitAnimationTotal_, 0);

    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->animationAsyncTraces_.at(0).isDisplayAnimator_ = true;
    rsJankStats->explicitAnimationTotal_ = 1;
    rsJankStats->SetAnimationTraceEnd(jankFrames);
    EXPECT_EQ(rsJankStats->explicitAnimationTotal_, 0);
}

/**
 * @tc.name: CheckAnimationTraceTimeoutTest008
 * @tc.desc: CheckAnimationTraceTimeout test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, CheckAnimationTraceTimeoutTest008, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    rsJankStats->CheckAnimationTraceTimeout();
    rsJankStats->animationTraceCheckCnt_ = 20;
    rsJankStats->CheckAnimationTraceTimeout();
    EXPECT_EQ(rsJankStats->animationTraceCheckCnt_, 0);

    rsJankStats->rtEndTimeSteady_ = 10000;
    rsJankStats->animationTraceCheckCnt_ = 20;
    rsJankStats->explicitAnimationTotal_ = 1;
    rsJankStats->implicitAnimationTotal_ = 1;
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->animationAsyncTraces_.at(0).isDisplayAnimator_ = false;
    rsJankStats->animationAsyncTraces_.emplace(1, AnimationTraceStats());
    rsJankStats->animationAsyncTraces_.at(1).isDisplayAnimator_ = true;
    rsJankStats->CheckAnimationTraceTimeout();
    EXPECT_EQ(rsJankStats->explicitAnimationTotal_, 0);
    EXPECT_EQ(rsJankStats->implicitAnimationTotal_, 0);
}

/**
 * @tc.name: GetTraceIdInitTest009
 * @tc.desc: GetTraceIdInit GetEffectiveFrameTimeFloat ConvertTimeToSystime test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, GetTraceIdInitTest009, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    // GetTraceIdInit test
    DataBaseRs info;
    info.uniqueId = 0;
    EXPECT_EQ(rsJankStats->GetTraceIdInit(info, 0), 0);
    EXPECT_EQ(rsJankStats->traceIdRemainder_.size(), 1);

    rsJankStats->traceIdRemainder_.at(0).remainder_ = 10;
    EXPECT_EQ(rsJankStats->GetTraceIdInit(info, 0), 0);
    EXPECT_EQ(rsJankStats->traceIdRemainder_.at(0).remainder_, 1);

    // GetEffectiveFrameTimeFloat test
    rsJankStats->rtLastEndTimeSteadyFloat_ = 0;
    rsJankStats->rsStartTimeSteadyFloat_ = 0;
    rsJankStats->rtEndTimeSteadyFloat_ = 1;
    EXPECT_EQ(rsJankStats->GetEffectiveFrameTimeFloat(false), 1);
    rsJankStats->isCurrentFrameSwitchToNotDoDirectComposition_ = true;
    EXPECT_EQ(rsJankStats->GetEffectiveFrameTimeFloat(false), 1);
    EXPECT_EQ(rsJankStats->GetEffectiveFrameTimeFloat(true), 1);

    // ConvertTimeToSystime test
    EXPECT_EQ(rsJankStats->ConvertTimeToSystime(0), 0);
    EXPECT_NE(rsJankStats->ConvertTimeToSystime(1), 0);
}

/**
 * @tc.name: GetMaxJankInfoTest010
 * @tc.desc: GetMaxJankInfo test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSJankStatsTest, GetMaxJankInfoTest010, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    int64_t maxFrameTimeFromStart = 0;
    int64_t maxHitchTimeFromStart = 0;
    int64_t duration = 0;
    rsJankStats->GetMaxJankInfo(jankFrames, false, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    rsJankStats->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.traceCreateTimeSteady_ = -1;
    jankFrames.traceTerminateTimeSteady_ = -1;
    rsJankStats->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = -1;
    jankFrames.traceTerminateTimeSteady_ = 0;
    rsJankStats->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = 0;
    jankFrames.traceTerminateTimeSteady_ = -1;
    rsJankStats->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = 0;
    jankFrames.traceTerminateTimeSteady_ = 0;
    rsJankStats->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = 0;
    rsJankStats->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.maxFrameOccurenceTimeSteady_ = 0;
    jankFrames.maxHitchOccurenceTimeSteady_ = 0;
    rsJankStats->GetMaxJankInfo(jankFrames, false, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.lastMaxFrameOccurenceTimeSteady_ = 0;
    jankFrames.lastMaxHitchOccurenceTimeSteady_ = 0;
    rsJankStats->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.isFrameRateRecorded_ = false;
    EXPECT_FALSE(jankFrames.isFrameRateRecorded_);
}

/**
 * @tc.name: IsAnimationEmptyTest001
 * @tc.desc: test IsAnimationEmpty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, IsAnimationEmptyTest001, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);
    rsJankStats->SetStartTime();
    rsJankStats->SetEndTime(false, true);
    EXPECT_TRUE(rsJankStats->IsAnimationEmpty());
}

/**
 * @tc.name: IsAnimationEmptyTest002
 * @tc.desc: test IsAnimationEmpty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, IsAnimationEmptyTest002, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);
    rsJankStats->implicitAnimationTotal_ = 0;
    rsJankStats->explicitAnimationTotal_ = 0;
    EXPECT_EQ(rsJankStats->IsAnimationEmpty(), true);
}

HWTEST_F(RSJankStatsTest, SetEarlyZFlagTest011, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    std::pair<int64_t, std::string> animationId = { 0, "test" };
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->implicitAnimationTotal_, 1);

    jankFrames.info_.isDisplayAnimator = true;
    jankFrames.info_.sceneId = SWITCH_SCENE_NAME;
    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->explicitAnimationTotal_, 1);
    EXPECT_TRUE(rsJankStats->ddgrEarlyZEnableFlag_);
    rsJankStats->SetAnimationTraceEnd(jankFrames);
    EXPECT_FALSE(rsJankStats->ddgrEarlyZEnableFlag_);
}

HWTEST_F(RSJankStatsTest, SetEarlyZFlagTest012, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    std::pair<int64_t, std::string> animationId = { 0, "test" };
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->implicitAnimationTotal_, 1);

    jankFrames.info_.isDisplayAnimator = true;
    jankFrames.info_.sceneId = SWITCH_SCENE_NAME;
    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->explicitAnimationTotal_, 1);
    EXPECT_TRUE(rsJankStats->ddgrEarlyZEnableFlag_);
    jankFrames.traceId_ = 5;
    rsJankStats->animationAsyncTraces_.emplace(jankFrames.traceId_, AnimationTraceStats());
    rsJankStats->SetAnimationTraceEnd(jankFrames);
    EXPECT_TRUE(rsJankStats->ddgrEarlyZEnableFlag_);
}

HWTEST_F(RSJankStatsTest, SetEarlyZFlagTest013, TestSize.Level1)
{
    RSSystemProperties::isEnableEarlyZ_ = false;
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    std::pair<int64_t, std::string> animationId = { 0, "test" };
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->implicitAnimationTotal_, 1);

    jankFrames.info_.isDisplayAnimator = true;
    jankFrames.info_.sceneId = SWITCH_SCENE_NAME;
    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    jankFrames.traceId_ = 5;
    rsJankStats->animationAsyncTraces_.emplace(jankFrames.traceId_, AnimationTraceStats());
    rsJankStats->SetAnimationTraceEnd(jankFrames);
    EXPECT_NE(rsJankStats->lastReportEarlyZTraceId_, jankFrames.traceId_);
    RSSystemProperties::isEnableEarlyZ_ = true;
}

HWTEST_F(RSJankStatsTest, SetEarlyZFlagTest014, TestSize.Level1)
{
    RSSystemProperties::isEnableEarlyZ_ = false;
    std::shared_ptr<RSJankStats> rsJankStats = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats, nullptr);

    JankFrames jankFrames;
    std::pair<int64_t, std::string> animationId = { 0, "test" };
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);

    rsJankStats->animationAsyncTraces_.clear();
    rsJankStats->SetAnimationTraceBegin(animationId, jankFrames);
    EXPECT_EQ(rsJankStats->implicitAnimationTotal_, 1);

    jankFrames.traceId_ = 5;
    rsJankStats->lastReportEarlyZTraceId_ = jankFrames.traceId_;
    rsJankStats->animationAsyncTraces_.emplace(jankFrames.traceId_, AnimationTraceStats());
    rsJankStats->SetAnimationTraceEnd(jankFrames);
    EXPECT_FALSE(rsJankStats->ddgrEarlyZEnableFlag_);
    RSSystemProperties::isEnableEarlyZ_ = true;
}

} // namespace Rosen
} // namespace OHOS