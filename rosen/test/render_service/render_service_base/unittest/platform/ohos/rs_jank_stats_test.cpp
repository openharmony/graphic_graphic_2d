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
    rsJankStats.SetStartTime();
    rsJankStats.SetFirstFrame();
    rsJankStats.SetEndTime();
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
    rsJankStats.SetEndTime();
    rsJankStats.ReportJankStats();
}
} // namespace Rosen
} // namespace OHOS