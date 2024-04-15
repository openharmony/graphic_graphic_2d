/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
    rsJankStats.SetOnVsyncStartTime(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL);
    pid_t appPid = 1;
    rsJankStats.SetAppFirstFrame(appPid);
    rsJankStats.SetEndTime(true);
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
    rsJankStats.SetOnVsyncStartTime(TIMESTAMP_INITIAL, TIMESTAMP_INITIAL);
    rsJankStats.SetEndTime(false);
    rsJankStats.SetStartTime();
    JankDurationParams rsParams;
    rsJankStats.HandleDirectComposition(rsParams, true);
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
    usleep(50*1000);
    rsJankStats.SetEndTime(false);
    usleep(100*1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_6_FREQ
    rsJankStats.SetReportEventResponse(info1);
    usleep(100*1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_15_FREQ
    usleep(100*1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_20_FREQ
    usleep(300*1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_36_FREQ
    usleep(250*1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_48_FREQ
    usleep(1000*1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_60_FREQ
    usleep(400*1000);
    rsJankStats.SetEndTime(false); // JANK_FRAME_120_FREQ
    usleep(800*1000);
    rsJankStats.SetEndTime(false); // jank frames skip more than 180
}
} // namespace Rosen
} // namespace OHOS