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
#include "ressched_event_listener.h"

#include "res_sched_client.h"
#include "res_type.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ResschedEventListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void ResschedEventListenerTest::SetUpTestCase() {}

void ResschedEventListenerTest::TearDownTestCase() {}

namespace {
/*
* Function: OnReceiveEvent_DrawFrameReportStart
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnReceiveEvent with EVENT_DRAW_FRAME_REPORT start event
 */
HWTEST_F(ResschedEventListenerTest, OnReceiveEvent_DrawFrameReportStart, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), true);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), true);
}

/*
* Function: OnReceiveEvent_DrawFrameReportStop
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnReceiveEvent with EVENT_DRAW_FRAME_REPORT stop event
 */
HWTEST_F(ResschedEventListenerTest, OnReceiveEvent_DrawFrameReportStop, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
    
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), false);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
}

/*
* Function: OnReceiveEvent_InvalidEventType
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnReceiveEvent with invalid event type
 */
HWTEST_F(ResschedEventListenerTest, OnReceiveEvent_InvalidEventType, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        static_cast<uint32_t>(-1),
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), false);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
}

/*
* Function: OnReceiveEvent_InvalidEventValue
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test OnReceiveEvent with invalid event value
 */
HWTEST_F(ResschedEventListenerTest, OnReceiveEvent_InvalidEventValue, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        static_cast<uint32_t>(-1), extInfo);
    
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), false);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
}

/*
* Function: HandleFrameRateStatisticsReport_ValidInput
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test HandleFrameRateStatisticsReport with valid input
 */
HWTEST_F(ResschedEventListenerTest, HandleFrameRateStatisticsReport_ValidInput, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "100";
    extInfo["type"] = "1";
    
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
}

/*
* Function: HandleFrameRateStatisticsReport_InvalidPid
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test HandleFrameRateStatisticsReport with invalid PID string
 */
HWTEST_F(ResschedEventListenerTest, HandleFrameRateStatisticsReport_InvalidPid, Function | MediumTest | Level3)
{
    ResschedEventListener::GetInstance()->currentPid_ = 0;

    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "invalid_pid";
    extInfo["type"] = "1";
    
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    
    // Should not change anything due to invalid PID parsing
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
}

/*
* Function: HandleFrameRateStatisticsReport_InvalidType
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test HandleFrameRateStatisticsReport with invalid type string
 */
HWTEST_F(ResschedEventListenerTest, HandleFrameRateStatisticsReport_InvalidType, Function | MediumTest | Level3)
{
    ResschedEventListener::GetInstance()->currentPid_ = 0;

    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "100";
    extInfo["type"] = "invalid_type";
    
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    
    // Should not change anything due to invalid type parsing
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
}

/*
* Function: ReportFrameToRSS_FirstReport
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ReportFrameToRSS with first report
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameToRSS_FirstReport, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
}

/*
* Function: ReportFrameToRSS_SubsequentReport
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ReportFrameToRSS with subsequent report
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameToRSS_SubsequentReport, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    
    ResschedEventListener::GetInstance()->SetIsFirstReport(false);
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
}

/*
* Function: ReportFrameCountAsync_WithValidQueue
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ReportFrameCountAsync when queue is available
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameCountAsync_WithValidQueue, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "100";
    extInfo["type"] = "1";
    
    // Initialize the frame rate statistics
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    
    // Call ReportFrameCountAsync
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(100);
    sleep(1);
    
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
}

/*
* Function: ReportFrameCountAsync_WithInvalidQueue
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ReportFrameCountAsync when queue creation fails
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameCountAsync_WithInvalidQueue, Function | MediumTest | Level3)
{
    ResschedEventListener::GetInstance()->currentPid_ = 0;

    // Simulate queue failure by setting it to null before the first call
    ResschedEventListener::GetInstance()->ffrtHighPriorityQueue_ = nullptr;
    
    // This call should return early due to queue unavailability
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(100);
    
    // No state change expected since function returns early
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
}

/*
* Function: ReportFrameCountAsync_WithWrongPid
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ReportFrameCountAsync with PID that doesn't match current PID
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameCountAsync_WithWrongPid, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "100";
    extInfo["type"] = "1";
    
    // Initialize with PID 100
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    
    // Call with different PID - internal logic should return early
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(200);
    sleep(1);
    
    // Current PID should remain unchanged
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
}

/*
* Function: ReportFrameCountAsync_WithDefaultType
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ReportFrameCountAsync when current type is DEFAULT_TYPE
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameCountAsync_WithDefaultType, Function | MediumTest | Level3)
{
    ResschedEventListener::GetInstance()->currentPid_ = 0;

    // Set currentType_ to DEFAULT_TYPE (0)
    ResschedEventListener::GetInstance()->currentType_ = 0;
    
    // Call ReportFrameCountAsync - internal logic should return early
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(100);
    sleep(1);
    
    // Current PID should remain unchanged
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
}

/*
* Function: FrameRateStatistics_Lifecycle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test complete frame rate statistics lifecycle
 */
HWTEST_F(ResschedEventListenerTest, FrameRateStatistics_Lifecycle, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "100";
    extInfo["type"] = "1";
    
    // Start frame rate statistics
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
    
    // Add some frame counts
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(100);
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(100);
    sleep(1);
    
    // Break frame rate statistics
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_BREAK, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
    
    // Restart statistics
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
    
    // End frame rate statistics
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_END, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
}

/*
* Function: GetFfrtHighPriorityQueue_Initialization
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test GetFfrtHighPriorityQueue initialization behavior
 */
HWTEST_F(ResschedEventListenerTest, GetFfrtHighPriorityQueue_Initialization, Function | MediumTest | Level3)
{
    // Reset the queue to test initialization
    ResschedEventListener::GetInstance()->ffrtHighPriorityQueue_ = nullptr;
    
    bool result = ResschedEventListener::GetInstance()->GetFfrtHighPriorityQueue();
    ASSERT_TRUE(result);
    
    // Call again to test that it returns true immediately without re-initialization
    bool result2 = ResschedEventListener::GetInstance()->GetFfrtHighPriorityQueue();
    ASSERT_TRUE(result2);
}

/*
* Function: ReportFrameRateToRSS_WithValidQueue
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: Test ReportFrameRateToRSS with valid queue
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameRateToRSS_WithValidQueue, Function | MediumTest | Level3)
{
    std::unordered_map<std::string, std::string> mapPayload;
    mapPayload["pid"] = "100";
    mapPayload["type"] = "1";
    mapPayload["frameRate"] = "60";
    
    bool result = ResschedEventListener::GetInstance()->GetFfrtHighPriorityQueue();
    ASSERT_TRUE(result);
    
    ResschedEventListener::GetInstance()->ReportFrameRateToRSS(mapPayload);
    sleep(1);

    ResschedEventListener::GetInstance()->ffrtHighPriorityQueue_ = nullptr;
    ResschedEventListener::GetInstance()->ReportFrameRateToRSS(mapPayload);
    sleep(1);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
