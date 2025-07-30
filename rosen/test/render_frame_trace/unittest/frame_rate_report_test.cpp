/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "frame_rate_report.h"

using namespace testing;
using namespace testing::ext;
using namespace FRAME_TRACE;

namespace OHOS {
namespace Rosen {
class FrameRateReportTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FrameRateReportTest::SetUpTestCase() {}
void FrameRateReportTest::TearDownTestCase() {}
void FrameRateReportTest::SetUp() {}
void FrameRateReportTest::TearDown() {}

/**
 * @tc.name: SendFrameRates001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(FrameRateReportTest, SendFrameRates001, TestSize.Level1)
{
    std::unordered_map<int, uint32_t> rates;
    FrameRateReport::GetInstance().SendFrameRates(rates);
    rates[1008] = 60;
    rates[-1] = 120;
    bool ret = FrameRateReport::GetInstance().SendFrameRates(rates);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SendFrameRatesToRss001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(FrameRateReportTest, SendFrameRatesToRss001, TestSize.Level1)
{
    std::unordered_map<int, uint32_t> rates;
    FrameRateReport::GetInstance().SendFrameRatesToRss(rates);
    rates[1008] = 60;
    rates[-1] = 120;
    FrameRateReport::GetInstance().SendFrameRatesToRss(rates);
    EXPECT_TRUE(nullptr != FrameRateReport::GetInstance().instance_);
}
} // namespace Rosen
} // namespace OHOS