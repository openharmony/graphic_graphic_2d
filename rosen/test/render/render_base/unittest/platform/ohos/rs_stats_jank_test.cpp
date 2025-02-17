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
#include "common/rs_common_def.h"
#include "hisysevent.h"
#include "platform/ohos/rs_jank_stats.h"

#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>
#include <chrono>

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
 * @tc.name: UpdateEndTimeTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, UpdateEndTimeTest001, TestSize.Level1)
{
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    rsJankStatsInstance.isFirstSetEnd_ = false;
    rsJankStatsInstance.UpdateEndTime();
    rsJankStatsInstance.isFirstSetEnd_ = true;
    rsJankStatsInstance.UpdateEndTime();
    ASSERT_NE(rsJankStatsInstance.isFirstSetEnd_, false);
}

/**
 * @tc.name: SetEndTimeTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetEndTimeTest001, TestSize.Level1)
{
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    rsJankStatsInstance.SetEndTime(false);
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetOnVsyncStartTime(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    pid_t appPid = 1;
    rsJankStatsInstance.SetAppFirstFrame(appPid);
    rsJankStatsInstance.SetEndTime(true);
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: HandleDirectCompositionTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, HandleDirectCompositionTest001, TestSize.Level1)
{
    JankDurationParams rsParams;
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetEndTime(true);
    rsJankStatsInstance.SetOnVsyncStartTime(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL, TIMESTAMP_INITIAL_FLOAT);
    rsJankStatsInstance.SetEndTime(false);
    rsJankStatsInstance.SetStartTime();

    rsParams.timeStart_ = rsJankStatsInstance.GetCurrentSystimeMs();
    rsParams.timeStartSteady_ = rsJankStatsInstance.GetCurrentSteadyTimeMs();
    rsParams.timeEnd_ = rsJankStatsInstance.GetCurrentSystimeMs();
    rsParams.timeEndSteady_ = rsJankStatsInstance.GetCurrentSteadyTimeMs();
    rsJankStatsInstance.HandleDirectComposition(rsParams, true);
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: SetReportEventResponseTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportEventResponseTest001, TestSize.Level1)
{
    DataBaseRs info1;
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    info1.uniqueId = 0;
    rsJankStatsInstance.SetReportEventResponse(info1);
    rsJankStatsInstance.SetReportEventResponse(info1);
    info1.uniqueId = 1;
    rsJankStatsInstance.SetReportEventResponse(info1);
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetEndTime(false);
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: UpdateJankFrameTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, ReportJankStatsTest001, TestSize.Level1)
{
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetEndTime(false);
    rsJankStatsInstance.ReportJankStats();
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: SetReportEventCompleteTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportEventCompleteTest001, TestSize.Level1)
{
    DataBaseRs info1;
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    info1.uniqueId = 2;
    rsJankStatsInstance.SetReportEventComplete(info1);
    rsJankStatsInstance.SetReportEventComplete(info1);
    info1.uniqueId = 3;
    rsJankStatsInstance.SetReportEventComplete(info1);
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetEndTime(false);
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: ConvertTimeToSystimeTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, ConvertTimeToSystimeTest001, TestSize.Level1)
{
    DataBaseRs info1;
    info1.uniqueId = 0;
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetReportEventResponse(info1);

    usleep(50 * 1000);
    rsJankStatsInstance.SetEndTime(false);
    usleep(100 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_6_FREQ
    rsJankStatsInstance.SetReportEventResponse(info1);
    usleep(240 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_15_FREQ
    usleep(300 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_20_FREQ
    usleep(500 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_36_FREQ
    usleep(700 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_48_FREQ
    usleep(900 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_60_FREQ
    usleep(1900 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_120_FREQ
    usleep(2900 * 1000);
    rsJankStatsInstance.SetEndTime(false); // JANK_FRAME_180_FREQ
    usleep(3000 * 1000);
    rsJankStatsInstance.SetEndTime(false); // jank frames skip more than 180
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: SetReportEventJankFrameTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankStatsTest, SetReportEventJankFrameTest001, TestSize.Level1)
{
    DataBaseRs info1;
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    info1.uniqueId = 1;
    rsJankStatsInstance.SetReportEventJankFrame(info1, false);
    info1.uniqueId = 4;
    rsJankStatsInstance.SetReportEventJankFrame(info1, true);
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetEndTime(false);
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: RecordJankFrameSingleTest001
 * @tc.desc: test RecordJankFrameSingle
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSJankStatsTest, RecordJankFrameSingleTest001, TestSize.Level1)
{
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    JankFrameRecordStats recordStats = {"test", 1};
    recordStats.isRecorded_ = true;
    rsJankStatsInstance.RecordJankFrameSingle(1, recordStats);
    recordStats.isRecorded_ = false;
    rsJankStatsInstance.RecordJankFrameSingle(1, recordStats);
    rsJankStatsInstance.RecordJankFrameSingle(0, recordStats);
    ASSERT_EQ(recordStats.isRecorded_, true);
}

/**
 * @tc.name: SetRSJankStats001
 * @tc.desc: SetRSJankStats and GetEffectiveFrameTime test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetRSJankStats001, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats1 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats1, nullptr);

    rsJankStats1->rsStartTimeSteady_ = 0;
    rsJankStats1->rtLastEndTimeSteady_ = 0;
    // isCurrentFrameSwitchToNotDoDirectComposition_ is false
    rsJankStats1->rtEndTimeSteady_ = 1;
    // isCurrentFrameSwitchToNotDoDirectComposition_ is true
    rsJankStats1->isCurrentFrameSwitchToNotDoDirectComposition_ = true;

    // dynamicRefreshRate is retained for future algorithm adjustment, keep it unused currently
    uint32_t dynamicRefreshRate = 0;
    // isConsiderRsStartTime is true
    rsJankStats1->SetRSJankStats(true, dynamicRefreshRate);
    // SetRSJankStatsTest is missedVsync = 0
    rsJankStats1->rsJankStats_[0] = USHRT_MAX;
    SetRSJankStatsTest(rsJankStats1, 1);
    // SetRSJankStatsTest is missedVsync = 1
    SetRSJankStatsTest(rsJankStats1, 17);
    // SetRSJankStatsTest is missedVsync = 10
    SetRSJankStatsTest(rsJankStats1, 166);
    // SetRSJankStatsTest is missedVsync = 15
    SetRSJankStatsTest(rsJankStats1, 249);
    // SetRSJankStatsTest is missedVsync = 20
    SetRSJankStatsTest(rsJankStats1, 332);
    // SetRSJankStatsTest is missedVsync = 36
    SetRSJankStatsTest(rsJankStats1, 598);
    // SetRSJankStatsTest is missedVsync = 48
    SetRSJankStatsTest(rsJankStats1, 797);
    // SetRSJankStatsTest is missedVsync = 60
    SetRSJankStatsTest(rsJankStats1, 996);
    // SetRSJankStatsTest is missedVsync = 120
    SetRSJankStatsTest(rsJankStats1, 1992);
    // SetRSJankStatsTest is missedVsync = 180
    SetRSJankStatsTest(rsJankStats1, 2988);
}

/**
 * @tc.name: ReportEventComplete001
 * @tc.desc: test ReportEventComplete
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSJankStatsTest, ReportEventComplete001, TestSize.Level1)
{
    auto& rsJankStatsInstance = RSJankStats::GetInstance();
    DataBaseRs info1;
    info1.uniqueId = 1;
    rsJankStatsInstance.SetReportEventResponse(info1);
    rsJankStatsInstance.SetReportEventComplete(info1);
    rsJankStatsInstance.SetStartTime();
    rsJankStatsInstance.SetEndTime(false, true);
    EXPECT_FALSE(rsJankStatsInstance.isFirstSetStart_);
}

/**
 * @tc.name: SetImplicitAnimationEnd002
 * @tc.desc: SetImplicitAnimationEnd ReportEventJankFrame ReportEventHitchTimeRatio ReportEventFirstFrame test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetImplicitAnimationEnd002, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats2 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats2, nullptr);

    // SetImplicitAnimationEnd test
    rsJankStats2->SetImplicitAnimationEnd(false);
    JankFrames jankFramesTest1;
    jankFramesTest1.isDisplayAnimator_ = true;
    JankFrames jankFramesTest2;
    rsJankStats2->animateJankFrames_.emplace(std::make_pair(1, ""), jankFramesTest1);
    rsJankStats2->animateJankFrames_.emplace(std::make_pair(2, ""), jankFramesTest2);
    rsJankStats2->SetImplicitAnimationEnd(true);

    // ReportEventJankFrame and ReportEventHitchTimeRatio test
    JankFrames jankFramesTest3;
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats2->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats2->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.lastTotalFrameTimeSteadyForHTR_ = 0;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, true);
    jankFramesTest3.lastTotalFrameTimeSteadyForHTR_ = 1;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, true);

    jankFramesTest3.totalFrames_ = 0;
    rsJankStats2->ReportEventJankFrame(jankFramesTest3, false);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.totalFrames_ = 1;
    rsJankStats2->ReportEventJankFrame(jankFramesTest3, false);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, false);
    jankFramesTest3.totalFrames_ = 0;
    rsJankStats2->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 0;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, true);
    jankFramesTest3.totalFrames_ = 1;
    rsJankStats2->ReportEventJankFrame(jankFramesTest3, true);
    jankFramesTest3.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats2->ReportEventHitchTimeRatio(jankFramesTest3, true);

    // ReportEventFirstFrame test
    rsJankStats2->firstFrameAppPids_.emplace(1);
    rsJankStats2->ReportEventFirstFrame();
}

/**
 * @tc.name: ReportJankStats003
 * @tc.desc: ReportJankStats and HandleImplicitAnimationEndInAdvance test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, ReportJankStats003, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats3 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats3, nullptr);

    // ReportJankStats test
    rsJankStats3->ReportJankStats();

    rsJankStats3->lastReportTime_ = 1;
    rsJankStats3->lastReportTimeSteady_ = 1;
    rsJankStats3->lastJankFrame6FreqTimeSteady_ = 1;
    rsJankStats3->ReportJankStats();

    rsJankStats3->isNeedReportJankStats_ = true;
    rsJankStats3->ReportJankStats();
    EXPECT_FALSE(rsJankStats3->isNeedReportJankStats_);

    // HandleImplicitAnimationEndInAdvance test
    JankFrames jankFrames;
    jankFrames.isDisplayAnimator_ = true;
    rsJankStats3->HandleImplicitAnimationEndInAdvance(jankFrames, false);

    jankFrames.isDisplayAnimator_ = false;
    jankFrames.isSetReportEventComplete_ = true;
    jankFrames.isSetReportEventJankFrame_ = true;
    rsJankStats3->HandleImplicitAnimationEndInAdvance(jankFrames, false);
    EXPECT_FALSE(jankFrames.isSetReportEventComplete_);
    EXPECT_FALSE(jankFrames.isSetReportEventJankFrame_);

    jankFrames.isSetReportEventJankFrame_ = true;
    rsJankStats3->HandleImplicitAnimationEndInAdvance(jankFrames, false);
    EXPECT_FALSE(jankFrames.isSetReportEventJankFrame_);
}

/**
 * @tc.name: RecordJankFrame004
 * @tc.desc: RecordJankFrame and RecordJankFrameSingle test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, RecordJankFrame004, TestSize.Level1)
{
    // RecordJankFrame test
    std::shared_ptr<RSJankStats> rsJankStats4 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats4, nullptr);

    rsJankStats4->accumulatedBufferCount_ = 6;
    rsJankStats4->rtEndTimeSteady_ = 266;
    rsJankStats4->rtLastEndTimeSteady_ = 0;
    rsJankStats4->rsStartTimeSteady_ = 0;
    rsJankStats4->explicitAnimationTotal_ = 1;
    rsJankStats4->isFirstSetEnd_ = true;
    rsJankStats4->implicitAnimationTotal_ = 1;
    rsJankStats4->RecordJankFrame(0);

    // RecordJankFrameSingle test
    JankFrameRecordStats recordStats = { "test", 1 };
    recordStats.isRecorded_ = true;
    rsJankStats4->RecordJankFrameSingle(1, recordStats);
    ASSERT_EQ(recordStats.isRecorded_, true);
}

/**
 * @tc.name: RecordAnimationDynamicFrameRate005
 * @tc.desc: RecordAnimationDynamicFrameRate test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSJankStatsTest, RecordAnimationDynamicFrameRate005, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats5 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats5, nullptr);

    JankFrames jankFrames;
    jankFrames.isFrameRateRecorded_ = true;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);

    jankFrames.isFrameRateRecorded_ = false;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, true);
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);

    jankFrames.traceId_ = -1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.traceId_ = 0;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    rsJankStats5->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    rsJankStats5->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, true);

    jankFrames.totalFrameTimeSteadyForHTR_ = 1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.totalFrames_ = 1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.lastTotalFrameTimeSteadyForHTR_ = 1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, true);
    jankFrames.lastTotalFrames_ = 1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, true);

    jankFrames.startTimeSteady_ = 1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, true);

    jankFrames.endTimeSteady_ = 1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    jankFrames.lastEndTimeSteady_ = 1;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, true);

    jankFrames.endTimeSteady_ = 2;
    jankFrames.isFrameRateRecorded_ = false;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, false);
    EXPECT_TRUE(jankFrames.isFrameRateRecorded_);
    jankFrames.lastEndTimeSteady_ = 2;
    jankFrames.isFrameRateRecorded_ = false;
    rsJankStats5->RecordAnimationDynamicFrameRate(jankFrames, true);
    EXPECT_TRUE(jankFrames.isFrameRateRecorded_);
}

/**
 * @tc.name: SetAnimationTraceBegin006
 * @tc.desc: SetAnimationTraceBegin test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetAnimationTraceBegin006, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats6 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats6, nullptr);

    JankFrames jankFrames;
    std::pair<int64_t, std::string> animationId = { 0, "test" };
    rsJankStats6->SetAnimationTraceBegin(animationId, jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats6->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats6->SetAnimationTraceBegin(animationId, jankFrames);

    rsJankStats6->animationAsyncTraces_.clear();
    rsJankStats6->SetAnimationTraceBegin(animationId, jankFrames);

    jankFrames.info_.isDisplayAnimator = true;
    rsJankStats6->animationAsyncTraces_.clear();
    rsJankStats6->SetAnimationTraceBegin(animationId, jankFrames);
}

/**
 * @tc.name: SetAnimationTraceEnd007
 * @tc.desc: SetAnimationTraceEnd test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, SetAnimationTraceEnd007, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats7 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats7, nullptr);

    JankFrames jankFrames;
    rsJankStats7->SetAnimationTraceEnd(jankFrames);

    jankFrames.traceId_ = 0;
    rsJankStats7->SetAnimationTraceEnd(jankFrames);

    rsJankStats7->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats7->animationAsyncTraces_.at(0).isDisplayAnimator_ = false;
    rsJankStats7->implicitAnimationTotal_ = 1;
    rsJankStats7->SetAnimationTraceEnd(jankFrames);

    rsJankStats7->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats7->animationAsyncTraces_.at(0).isDisplayAnimator_ = true;
    rsJankStats7->explicitAnimationTotal_ = 1;
    rsJankStats7->SetAnimationTraceEnd(jankFrames);
}

/**
 * @tc.name: CheckAnimationTraceTimeout008
 * @tc.desc: CheckAnimationTraceTimeout test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, CheckAnimationTraceTimeout008, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats8 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats8, nullptr);

    rsJankStats8->CheckAnimationTraceTimeout();
    rsJankStats8->animationTraceCheckCnt_ = 20;
    rsJankStats8->CheckAnimationTraceTimeout();

    rsJankStats8->rtEndTimeSteady_ = 5000;
    rsJankStats8->animationTraceCheckCnt_ = 20;
    rsJankStats8->explicitAnimationTotal_ = 1;
    rsJankStats8->implicitAnimationTotal_ = 1;
    rsJankStats8->animationAsyncTraces_.emplace(0, AnimationTraceStats());
    rsJankStats8->animationAsyncTraces_.at(0).isDisplayAnimator_ = false;
    rsJankStats8->animationAsyncTraces_.emplace(1, AnimationTraceStats());
    rsJankStats8->animationAsyncTraces_.at(1).isDisplayAnimator_ = true;
    rsJankStats8->CheckAnimationTraceTimeout();
}

/**
 * @tc.name: GetTraceIdInit009
 * @tc.desc: GetTraceIdInit GetEffectiveFrameTimeFloat ConvertTimeToSystime test
 * @tc.type: FUNC
 * @tc.require: issuesI9TD1K
 */
HWTEST_F(RSJankStatsTest, GetTraceIdInit009, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats9 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats9, nullptr);

    // GetTraceIdInit test
    DataBaseRs info;
    info.uniqueId = 0;
    EXPECT_EQ(rsJankStats9->GetTraceIdInit(info, 0), 0);

    rsJankStats9->traceIdRemainder_.at(0).remainder_ = 10;
    EXPECT_EQ(rsJankStats9->GetTraceIdInit(info, 0), 0);

    // GetEffectiveFrameTimeFloat test
    rsJankStats9->rtLastEndTimeSteadyFloat_ = 0;
    rsJankStats9->rsStartTimeSteadyFloat_ = 0;
    rsJankStats9->rtEndTimeSteadyFloat_ = 1;
    rsJankStats9->isCurrentFrameSwitchToNotDoDirectComposition_ = true;

    // ConvertTimeToSystime test
    EXPECT_NE(rsJankStats9->ConvertTimeToSystime(1), 0);
}

/**
 * @tc.name: GetMaxJankInfo010
 * @tc.desc: GetMaxJankInfo test
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSJankStatsTest, GetMaxJankInfo010, TestSize.Level1)
{
    std::shared_ptr<RSJankStats> rsJankStats10 = std::make_shared<RSJankStats>();
    EXPECT_NE(rsJankStats10, nullptr);

    JankFrames jankFrames;
    int64_t maxFrameTimeFromStart = 0;
    int64_t maxHitchTimeFromStart = 0;
    int64_t duration = 0;
    rsJankStats10->GetMaxJankInfo(jankFrames, false, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    rsJankStats10->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.traceCreateTimeSteady_ = -1;
    jankFrames.startTimeSteady_ = -1;
    jankFrames.endTimeSteady_ = -1;
    rsJankStats10->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = -1;
    jankFrames.startTimeSteady_ = -1;
    jankFrames.endTimeSteady_ = 0;
    rsJankStats10->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = -1;
    jankFrames.startTimeSteady_ = 0;
    jankFrames.endTimeSteady_ = -1;
    rsJankStats10->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = -1;
    jankFrames.startTimeSteady_ = 0;
    jankFrames.endTimeSteady_ = 0;
    rsJankStats10->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);
    jankFrames.traceCreateTimeSteady_ = 0;
    rsJankStats10->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.maxFrameOccurenceTimeSteady_ = 0;
    jankFrames.maxHitchOccurenceTimeSteady_ = 0;
    rsJankStats10->GetMaxJankInfo(jankFrames, false, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.lastMaxFrameOccurenceTimeSteady_ = 0;
    jankFrames.lastMaxHitchOccurenceTimeSteady_ = 0;
    rsJankStats10->GetMaxJankInfo(jankFrames, true, maxFrameTimeFromStart, maxHitchTimeFromStart, duration);

    jankFrames.isFrameRateRecorded_ = false;
    EXPECT_FALSE(jankFrames.isFrameRateRecorded_);
}
} // namespace Rosen
} // namespace OHOS