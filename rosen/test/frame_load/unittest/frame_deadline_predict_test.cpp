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
 * @tc.desc: test GetInstance returns same singleton instance
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, GetInstance001, TestSize.Level1)
{
    auto& instance1 = RsFrameDeadlinePredict::GetInstance();
    auto& instance2 = RsFrameDeadlinePredict::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: ReportRsFrameDeadline001
 * @tc.desc: test ReportRsFrameDeadline with 60Hz rate updates preIdealPeriod_
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline001, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 60;
    int64_t period = 16666666;
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    EXPECT_EQ(instance.preExtraReserve_, 0);
}

/**
 * @tc.name: ReportRsFrameDeadline002
 * @tc.desc: test ReportRsFrameDeadline with 120Hz and ltpoEnabled=false uses FIXED_EXTRA_DRAWING_TIME
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline002, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 120;
    int64_t period = 8333333;
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    // FIXED_EXTRA_DRAWING_TIME = 3000000 (3ms)
    EXPECT_EQ(instance.preExtraReserve_, 3000000);
}

/**
 * @tc.name: ReportRsFrameDeadline003
 * @tc.desc: test ReportRsFrameDeadline with same period returns early without update
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline003, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 60;
    int64_t period = 19999999; // unique period to avoid interference
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);

    // Second call with same period should return early (preIdealPeriod_ unchanged)
    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
}

/**
 * @tc.name: ReportRsFrameDeadline004
 * @tc.desc: test ReportRsFrameDeadline with different period updates preIdealPeriod_
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline004, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    int64_t period1 = 18888888;
    int64_t period2 = 17777777;

    instance.ReportRsFrameDeadline(60, period1, false, false);
    EXPECT_EQ(instance.preIdealPeriod_, period1);

    instance.ReportRsFrameDeadline(60, period2, false, false);
    EXPECT_EQ(instance.preIdealPeriod_, period2);
}

/**
 * @tc.name: ReportRsFrameDeadline005
 * @tc.desc: test ReportRsFrameDeadline with 120Hz ltpoEnabled=true uses vsyncOffset
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline005, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 120;
    int64_t period = 8444444; // unique period
    bool ltpoEnabled = true;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    // When ltpoEnabled=true, extraReserve depends on vsyncOffset from CreateVSyncGenerator
    // vsyncOffset <= SINGLE_SHIFT (2700000) means extraReserve = 0
}

/**
 * @tc.name: ReportRsFrameDeadline006
 * @tc.desc: test ReportRsFrameDeadline with 90Hz rate (non-OLED_120_HZ) extraReserve=0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline006, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 90;
    int64_t period = 11111111;
    bool ltpoEnabled = true;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    EXPECT_EQ(instance.preExtraReserve_, 0);
}

/**
 * @tc.name: ReportRsFrameDeadline007
 * @tc.desc: test ReportRsFrameDeadline with 30Hz low refresh rate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline007, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 30;
    int64_t period = 33333333;
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    EXPECT_EQ(instance.preExtraReserve_, 0);
}

/**
 * @tc.name: ReportRsFrameDeadline008
 * @tc.desc: test ReportRsFrameDeadline with 144Hz high refresh rate
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline008, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 144;
    int64_t period = 6944444;
    bool ltpoEnabled = true;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    EXPECT_EQ(instance.preExtraReserve_, 0);
}

/**
 * @tc.name: ReportRsFrameDeadline009
 * @tc.desc: test ReportRsFrameDeadline workflow with multiple frame rate changes
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline009, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();

    // Start with 60Hz
    instance.ReportRsFrameDeadline(60, 16666660, false, false);
    EXPECT_EQ(instance.preIdealPeriod_, 16666660);
    EXPECT_EQ(instance.preExtraReserve_, 0);

    // Switch to 120Hz with ltpoEnabled=false
    instance.ReportRsFrameDeadline(120, 8333330, false, false);
    EXPECT_EQ(instance.preIdealPeriod_, 8333330);
    EXPECT_EQ(instance.preExtraReserve_, 3000000);

    // Switch back to 60Hz
    instance.ReportRsFrameDeadline(60, 16666661, false, false);
    EXPECT_EQ(instance.preIdealPeriod_, 16666661);
    EXPECT_EQ(instance.preExtraReserve_, 0);
}

/**
 * @tc.name: ReportRsFrameDeadline010
 * @tc.desc: test ReportRsFrameDeadline with zero period
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline010, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 60;
    int64_t period = 0;
    bool ltpoEnabled = false;
    bool forceRefreshFlag = false;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, 0);
    EXPECT_EQ(instance.preExtraReserve_, 0);
}

/**
 * @tc.name: ReportRsFrameDeadline011
 * @tc.desc: test ReportRsFrameDeadline with 120Hz same period but different ltpoEnabled
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline011, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    int64_t period = 8555555;

    // First call with ltpoEnabled=false, extraReserve=FIXED_EXTRA_DRAWING_TIME
    instance.ReportRsFrameDeadline(120, period, false, false);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    EXPECT_EQ(instance.preExtraReserve_, 3000000);

    // Same period with ltpoEnabled=true (extraReserve depends on vsyncOffset)
    instance.ReportRsFrameDeadline(120, period, true, false);
    EXPECT_EQ(instance.preIdealPeriod_, period);
}

/**
 * @tc.name: ReportRsFrameDeadline012
 * @tc.desc: test ReportRsFrameDeadline with non-120Hz same period early return
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline012, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    int64_t period = 12222222;

    // First call sets preIdealPeriod_
    instance.ReportRsFrameDeadline(60, period, false, false);
    int64_t savedPeriod = instance.preIdealPeriod_;

    // Second call with same period for non-120Hz should return early
    instance.ReportRsFrameDeadline(90, period, true, false);
    // For non-120Hz, if period same and rate != 120, should return early
    EXPECT_EQ(instance.preIdealPeriod_, savedPeriod);
}

/**
 * @tc.name: ReportRsFrameDeadline013
 * @tc.desc: test ReportRsFrameDeadline forceRefreshFlag parameter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline013, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    int64_t period = 8666666;

    // forceRefreshFlag affects drawingTime calculation but not state storage
    instance.ReportRsFrameDeadline(120, period, false, true);
    EXPECT_EQ(instance.preIdealPeriod_, period);
    EXPECT_EQ(instance.preExtraReserve_, 3000000);
}

/**
 * @tc.name: ReportRsFrameDeadline014
 * @tc.desc: test ReportRsFrameDeadline with 120Hz ltpoEnabled=true forceRefreshFlag=true
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline014, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();
    uint32_t rate = 120;
    int64_t period = 8777777;
    bool ltpoEnabled = true;
    bool forceRefreshFlag = true;

    instance.ReportRsFrameDeadline(rate, period, ltpoEnabled, forceRefreshFlag);
    EXPECT_EQ(instance.preIdealPeriod_, period);
}

/**
 * @tc.name: ReportRsFrameDeadline015
 * @tc.desc: test ReportRsFrameDeadline multiple consecutive calls with varying periods
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline015, TestSize.Level1)
{
    auto& instance = RsFrameDeadlinePredict::GetInstance();

    for (int i = 0; i < 5; i++) {
        int64_t period = 8000000 + i * 100000;
        instance.ReportRsFrameDeadline(120, period, false, false);
        EXPECT_EQ(instance.preIdealPeriod_, period);
        EXPECT_EQ(instance.preExtraReserve_, 3000000);
    }
}

/**
 * @tc.name: ReportRsFrameDeadline016
 * @tc.desc: test ReportRsFrameDeadline singleton shared_ptr access
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameDeadlinePredictTest, ReportRsFrameDeadline016, TestSize.Level1)
{
    std::shared_ptr<RsFrameDeadlinePredict> instance;
    instance = std::shared_ptr<RsFrameDeadlinePredict>(
        &RsFrameDeadlinePredict::GetInstance(),
        [](RsFrameDeadlinePredict*) {}
    );
    EXPECT_TRUE(instance != nullptr);

    instance->ReportRsFrameDeadline(60, 13333333, false, false);
    EXPECT_EQ(instance->preIdealPeriod_, 13333333);
}
} // namespace OHOS::Rosen
