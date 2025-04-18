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

#include "gtest/gtest.h"
#define private public
#define protected public
#include "gfx/performance/rs_perfmonitor_reporter.h"
#undef private
#undef protected
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
int32_t DRAWING_CACHE_DURATION_TIMEOUT_THRESHOLD = 1000;
int32_t REPORT_INTERVAL = 120000000;
int32_t INTERVAL_THRESHOLD = 1000000;

class RSPerfmonitorReporterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPerfmonitorReporterTest::SetUpTestCase() {}
void RSPerfmonitorReporterTest::TearDownTestCase() {}
void RSPerfmonitorReporterTest::SetUp() {}
void RSPerfmonitorReporterTest::TearDown() {}

/**
 * @tc.name: SetFocusAppInfoTest
 * @tc.desc: Test SetFocusAppInfo
 * @tc.type: FUNC
 * @tc.require: #IBZ6AZ
 */
HWTEST_F(RSPerfmonitorReporterTest, SetFocusAppInfoTest, TestSize.Level1)
{
    RSPerfMonitorReporter perfMonitor;
    std::string bundleName = "SCBDesktop5";
    perfMonitor.SetFocusAppInfo(bundleName.c_str());
    perfMonitor.ReportAtRsFrameEnd();
    EXPECT_EQ(perfMonitor.currentBundleName_, bundleName);
}

/**
 * @tc.name: EndRendergroupMonitorTest
 * @tc.desc: Test EndRendergroupMonitor
 * @tc.type: FUNC
 * @tc.require: #IBZ6AZ
 */
HWTEST_F(RSPerfmonitorReporterTest, EndRendergroupMonitorTest, TestSize.Level1)
{
    RSPerfMonitorReporter perfMonitor;
    std::shared_ptr<RSContext> ctx = std::make_shared<RSContext>();
    int updateTimes = 1;
    NodeId nodeId1 = 1;
    auto startTime = high_resolution_clock::now();
    perfMonitor.EndRendergroupMonitor(startTime, nodeId1, ctx, updateTimes);
    EXPECT_EQ(perfMonitor.drawingCacheLastTwoTimestampMap_[nodeId1].size(), 1);

    updateTimes = 8;
    NodeId nodeId2 = 2;
    startTime = high_resolution_clock::now() - std::chrono::microseconds(5001);
    perfMonitor.EndRendergroupMonitor(startTime, nodeId2, ctx, updateTimes);
    EXPECT_EQ(perfMonitor.drawingCacheLastTwoTimestampMap_[nodeId2].size(), 1);
    
    NodeId nodeId3 = 3;
    ctx = nullptr;
    startTime = high_resolution_clock::now();
    perfMonitor.EndRendergroupMonitor(startTime, nodeId3, ctx, updateTimes);
    EXPECT_EQ(perfMonitor.drawingCacheLastTwoTimestampMap_[nodeId3].size(), 1);
}

/**
 * @tc.name: ProcessRendergroupSubhealthTest
 * @tc.desc: Test ProcessRendergroupSubhealth
 * @tc.type: FUNC
 * @tc.require: #IBZ6AZ
 */
HWTEST_F(RSPerfmonitorReporterTest, ProcessRendergroupSubhealthTest, TestSize.Level1)
{
    RSPerfMonitorReporter perfMonitor;
    NodeId nodeId = 1;
    int updateTimes = 1;
    auto startTime = high_resolution_clock::now();
    std::shared_ptr<RSContext> ctx = std::make_shared<RSContext>();
    int interval = 1000;
    perfMonitor.ProcessRendergroupSubhealth(nodeId, ctx, updateTimes, interval, startTime);
    auto startTime2 = startTime + std::chrono::microseconds(1000);
    perfMonitor.ProcessRendergroupSubhealth(nodeId, ctx, updateTimes + 1, interval, startTime2);
    auto startTime3 = startTime + std::chrono::microseconds(2000);
    perfMonitor.ProcessRendergroupSubhealth(nodeId, ctx, updateTimes + 2, interval, startTime3);
    auto startTime4 = startTime + std::chrono::microseconds(3000);
    perfMonitor.ProcessRendergroupSubhealth(nodeId, ctx, updateTimes + 3, interval, startTime4);
    EXPECT_EQ(perfMonitor.drawingCacheTimeTakenMap_[nodeId].size(), 3);
}

/**
 * @tc.name: NeedReportSubHealthTest
 * @tc.desc: Test NeedReportSubHealth
 * @tc.type: FUNC
 * @tc.require: #IBZ6AZ
 */
HWTEST_F(RSPerfmonitorReporterTest, NeedReportSubHealthTest, TestSize.Level1)
{
    RSPerfMonitorReporter perfMonitor;
    // test not meet report frequency control
    NodeId nodeId1 = 1;
    int updateTimes = 1;
    auto startTime = high_resolution_clock::now();
    perfMonitor.drawingCacheLastReportTimeMap_[nodeId1] = startTime - std::chrono::microseconds(1);
    EXPECT_FALSE(perfMonitor.NeedReportSubHealth(nodeId1, updateTimes, startTime));

    // test timestampCounts lower than STORED_TIMESTAMP_COUNT
    NodeId nodeId2 = 2;
    perfMonitor.drawingCacheLastReportTimeMap_[nodeId2] = startTime - std::chrono::microseconds(REPORT_INTERVAL + 1);
    perfMonitor.drawingCacheLastTwoTimestampMap_[nodeId2].push(
        startTime - std::chrono::microseconds(INTERVAL_THRESHOLD + 1));
    EXPECT_FALSE(perfMonitor.NeedReportSubHealth(nodeId2, updateTimes, startTime));

    // test interval greater than INTERVAL_THRESHOLD
    perfMonitor.drawingCacheLastTwoTimestampMap_[nodeId2].push(startTime - std::chrono::microseconds(1));
    EXPECT_FALSE(perfMonitor.NeedReportSubHealth(nodeId2, updateTimes, startTime));

    // test true
    NodeId nodeId3 = 3;
    perfMonitor.drawingCacheLastReportTimeMap_[nodeId3] = startTime - std::chrono::microseconds(REPORT_INTERVAL + 1);
    perfMonitor.drawingCacheLastTwoTimestampMap_[nodeId3].push(startTime - std::chrono::microseconds(2));
    perfMonitor.drawingCacheLastTwoTimestampMap_[nodeId3].push(startTime - std::chrono::microseconds(1));

    int64_t duration = DRAWING_CACHE_DURATION_TIMEOUT_THRESHOLD;
    std::vector<int64_t> value3 = {duration, duration, duration};
    perfMonitor.drawingCacheTimeTakenMap_[nodeId3] = value3;
    EXPECT_TRUE(perfMonitor.NeedReportSubHealth(nodeId3, updateTimes, startTime));

    updateTimes = 8;
    std::vector<int64_t> value4 = {duration - 1, duration, duration};
    perfMonitor.drawingCacheTimeTakenMap_[nodeId3] = value4;
    EXPECT_TRUE(perfMonitor.NeedReportSubHealth(nodeId3, updateTimes, startTime));

    updateTimes = 3;
    EXPECT_FALSE(perfMonitor.NeedReportSubHealth(nodeId3, updateTimes, startTime));
}

/**
 * @tc.name: CheckAllDrawingCacheDurationTimeoutTest
 * @tc.desc: Test CheckAllDrawingCacheDurationTimeout
 * @tc.type: FUNC
 * @tc.require: #IBZ6AZ
 */
HWTEST_F(RSPerfmonitorReporterTest, CheckAllDrawingCacheDurationTimeoutTest, TestSize.Level1)
{
    RSPerfMonitorReporter perfMonitor;
    NodeId nodeId1 = 1;
    int64_t duration = DRAWING_CACHE_DURATION_TIMEOUT_THRESHOLD;
    std::vector<int64_t> value = {duration, duration, duration};
    perfMonitor.drawingCacheTimeTakenMap_[nodeId1] = value;
    EXPECT_TRUE(perfMonitor.CheckAllDrawingCacheDurationTimeout(nodeId1));

    NodeId nodeId2 = 2;
    std::vector<int64_t> value2 = {duration - 1, duration, duration};
    perfMonitor.drawingCacheTimeTakenMap_[nodeId2] = value2;
    EXPECT_FALSE(perfMonitor.CheckAllDrawingCacheDurationTimeout(nodeId2));
}

/**
 * @tc.name: MeetReportFrequencyControlTest
 * @tc.desc: Test MeetReportFrequencyControl
 * @tc.type: FUNC
 * @tc.require: #IBZ6AZ
 */
HWTEST_F(RSPerfmonitorReporterTest, MeetReportFrequencyControlTest, TestSize.Level1)
{
    RSPerfMonitorReporter perfMonitor;
    // test not reported previously
    NodeId nodeId1 = 1;
    auto startTime1 = high_resolution_clock::now();
    EXPECT_TRUE(perfMonitor.MeetReportFrequencyControl(nodeId1, startTime1));

    // test meet report frequency control
    NodeId nodeId2 = 2;
    auto startTime2 = high_resolution_clock::now();
    perfMonitor.drawingCacheLastReportTimeMap_[nodeId2] = startTime2 - std::chrono::microseconds(REPORT_INTERVAL + 1);
    EXPECT_TRUE(perfMonitor.MeetReportFrequencyControl(nodeId2, startTime2));

    // test not meet report frequency control
    NodeId nodeId3 = 3;
    auto startTime3 = high_resolution_clock::now();
    perfMonitor.drawingCacheLastReportTimeMap_[nodeId3] = startTime3 - std::chrono::microseconds(1);
    EXPECT_FALSE(perfMonitor.MeetReportFrequencyControl(nodeId3, startTime3));
}

/**
 * @tc.name: GetUpdateCacheTimeTakenTest
 * @tc.desc: Test GetUpdateCacheTimeTaken
 * @tc.type: FUNC
 * @tc.require: #IBZ6AZ
 */
HWTEST_F(RSPerfmonitorReporterTest, GetUpdateCacheTimeTakenTest, TestSize.Level1)
{
    RSPerfMonitorReporter perfMonitor;
    // test no data
    NodeId nodeId1 = 1;
    EXPECT_EQ(perfMonitor.GetUpdateCacheTimeTaken(nodeId1), "");

    // test normal
    NodeId nodeId2 = 2;
    std::vector<int64_t> value = {1000, 1001, 1002};
    perfMonitor.drawingCacheTimeTakenMap_[nodeId2] = value;
    EXPECT_EQ(perfMonitor.GetUpdateCacheTimeTaken(nodeId2), "1000,1001,1002");
}

}