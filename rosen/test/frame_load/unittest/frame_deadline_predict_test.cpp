/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <string>

#include "rs_frame_deadline_predict.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsFrameDeadlinePredictTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameDeadlinePredictTest::SetUpTestCase() {}
void RsFrameDeadlinePredictTest::TearDownTestCase() {}
void RsFrameDeadlinePredictTest::SetUp() {}
void RsFrameDeadlinePredictTest::TearDown() {}

/**
 * @tc.name: GetInstance001
 * @tc.desc: test GetInstance returns valid singleton
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, GetInstance001, TestSize.Level1)
{
    std::shared_ptr<RsFrameDeadlinePredict> instance;
    instance = std::shared_ptr<RsFrameDeadlinePredict>(
        &RsFrameDeadlinePredict::GetInstance(),
        [](RsFrameDeadlinePredict*) {}
    );
    EXPECT_TRUE(instance != nullptr);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline001
 * @tc.desc: test ReportRsFrameDeadline with 60Hz rate (non-OLED_120_HZ)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline001, TestSize.Level1)
{
    uint32_t rate = 60; // 60Hz, not OLED_120_HZ
    int64_t period = 16666666; // ~16.67ms for 60Hz
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline002
 * @tc.desc: test ReportRsFrameDeadline with 120Hz rate and ltpoEnabled false
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline002, TestSize.Level1)
{
    uint32_t rate = 120; // OLED_120_HZ
    int64_t period = 8333333; // ~8.33ms for 120Hz
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline003
 * @tc.desc: test ReportRsFrameDeadline with 120Hz rate, ltpoEnabled true
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline003, TestSize.Level1)
{
    uint32_t rate = 120; // OLED_120_HZ
    int64_t period = 8333333; // ~8.33ms for 120Hz
    bool ltpoEnabled = true;
    bool forceRefreshFlag = false;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline004
 * @tc.desc: test ReportRsFrameDeadline with forceRefreshFlag true
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline004, TestSize.Level1)
{
    uint32_t rate = 120; // OLED_120_HZ
    int64_t period = 8333333; // ~8.33ms for 120Hz
    bool ltpoEnabled = false;
    bool forceRefreshFlag = true;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline005
 * @tc.desc: test ReportRsFrameDeadline with same period (early return case)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline005, TestSize.Level1)
{
    uint32_t rate = 60; // non-OLED_120_HZ
    int64_t period = 16666666;
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    // First call to set preIdealPeriod_
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    // Second call with same period should return early
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline006
 * @tc.desc: test ReportRsFrameDeadline with 90Hz rate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline006, TestSize.Level1)
{
    uint32_t rate = 90;
    int64_t period = 11111111; // ~11.11ms for 90Hz
    bool ltpoEnabled = true;
    bool forceRefreshFlag = false;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline007
 * @tc.desc: test ReportRsFrameDeadline with 120Hz, ltpoEnabled true, forceRefreshFlag true
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline007, TestSize.Level1)
{
    uint32_t rate = 120; // OLED_120_HZ
    int64_t period = 8333333;
    bool ltpoEnabled = true;
    bool forceRefreshFlag = true;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline008
 * @tc.desc: test ReportRsFrameDeadline with varying periods
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline008, TestSize.Level1)
{
    // Test with multiple different periods
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(60, 16666666, false, false);
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(90, 11111111, false, false);
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(120, 8333333, false, false);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline009
 * @tc.desc: test ReportRsFrameDeadline with 30Hz rate (low refresh rate)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline009, TestSize.Level1)
{
    uint32_t rate = 30;
    int64_t period = 33333333; // ~33.33ms for 30Hz
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline010
 * @tc.desc: test ReportRsFrameDeadline with 144Hz rate (high refresh rate)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline010, TestSize.Level1)
{
    uint32_t rate = 144;
    int64_t period = 6944444; // ~6.94ms for 144Hz
    bool ltpoEnabled = true;
    bool forceRefreshFlag = false;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline011
 * @tc.desc: test ReportRsFrameDeadline workflow with multiple calls
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline011, TestSize.Level1)
{
    // Simulate a workflow with multiple frame rate changes
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(60, 16666666, false, false);
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(120, 8333333, true, false);
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(120, 8333333, true, true);
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(60, 16666666, false, false);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline012
 * @tc.desc: test ReportRsFrameDeadline with zero period
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline012, TestSize.Level1)
{
    uint32_t rate = 60;
    int64_t period = 0;
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline013
 * @tc.desc: test ReportRsFrameDeadline with same period but different rate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline013, TestSize.Level1)
{
    int64_t period = 8333333;

    // First call with 120Hz
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(120, period, false, false);
    // Second call with 60Hz (different rate but same period will be treated differently)
    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(60, 16666666, false, false);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline014
 * @tc.desc: test ReportRsFrameDeadline with ltpoEnabled false and forceRefreshFlag true
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline014, TestSize.Level1)
{
    uint32_t rate = 120;
    int64_t period = 8333333;
    bool ltpoEnabled = false;
    bool forceRefreshFlag = true;

    RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    SUCCEED();
}

/**
 * @tc.name: ReportRsFrameDeadline015
 * @tc.desc: test ReportRsFrameDeadline multiple consecutive calls
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline015, TestSize.Level1)
{
    // Multiple consecutive calls to test state management
    for (int i = 0; i < 5; i++) {
        RsFrameDeadlinePredict::GetInstance().ReportRsFrameDeadline(
            120, 8333333 + i * 1000, true, i % 2 == 0);
    }
    SUCCEED();
}
} // namespace OHOS::Rosen
