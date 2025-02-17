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
* Function: OnReceiveEvent001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OnReceiveEvent
 */
HWTEST_F(ResschedEventListenerTest, OnReceiveEvent001, Function | MediumTest| Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), true);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), true);
    ResschedEventListener::GetInstance()->OnReceiveEvent(ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), false);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
    ResschedEventListener::GetInstance()->OnReceiveEvent(-1,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), false);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
    ResschedEventListener::GetInstance()->OnReceiveEvent(ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        -1, extInfo);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), false);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
    ResschedEventListener::GetInstance()->OnReceiveEvent(-1,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_STOP, extInfo);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsNeedReport(), false);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
}

/*
* Function: ReportFrameToRSS001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ReportFrameToRSS
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameToRSS001, Function | MediumTest| Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    ResschedEventListener::GetInstance()->OnReceiveEvent(ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
    ResschedEventListener::GetInstance()->OnReceiveEvent(ResourceSchedule::ResType::EventType::EVENT_DRAW_FRAME_REPORT,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_DRAW_FRAME_REPORT_START, extInfo);
    ResschedEventListener::GetInstance()->SetIsFirstReport(false);
    ResschedEventListener::GetInstance()->ReportFrameToRSS();
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetIsFirstReport(), false);
}

/*
* Function: ReportFrameToRSS001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OnReceiveEvent
 */
HWTEST_F(ResschedEventListenerTest, OnReceiveEvent002, Function | MediumTest| Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "100";
    extInfo["type"] = "1";
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_BREAK, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_END, extInfo);
    sleep(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
}

/*
* Function: ReportFrameRateToRSS001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ReportFrameRateToRSS001
 */
HWTEST_F(ResschedEventListenerTest, ReportFrameRateToRSS001, Function | MediumTest| Level3)
{
    std::unordered_map<std::string, std::string> extInfo;
    extInfo["pid"] = "100";
    extInfo["type"] = "1";
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_START, extInfo);
    sleep(1);
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(100);
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 100);
    ResschedEventListener::GetInstance()->OnReceiveEvent(
        ResourceSchedule::ResType::EventType::EVENT_FRAME_RATE_STATISTICS,
        ResourceSchedule::ResType::EventValue::EVENT_VALUE_FRAME_RATE_STATISTICS_BREAK, extInfo);
    sleep(1);
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(100);
    ResschedEventListener::GetInstance()->ReportFrameCountAsync(1);
    ASSERT_EQ(ResschedEventListener::GetInstance()->GetCurrentPid(), 0);
}


} // namespace
} // namespace Rosen
} // namespace OHOS
