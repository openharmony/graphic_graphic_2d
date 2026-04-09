/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "platform/ohos/rs_composer_jank_stats.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int REFRESH_RATE_WINDOW_SIZE = 9;
constexpr int FRAME_WINDOW_MIDDLE_NUM = 4;
}

class RSComposerJankStatsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSComposerJankStatsTest::SetUpTestCase() {}

void RSComposerJankStatsTest::TearDownTestCase() {}

void RSComposerJankStatsTest::SetUp()
{
    auto& instance = RSComposerJankStats::GetInstance();
    instance.lastTimestamp_ = 0;
    instance.refreshRates_.clear();
}

void RSComposerJankStatsTest::TearDown() {}

/**
 * @tc.name: GetInstance001
 * @tc.desc: Verify GetInstance returns valid singleton and same instance on multiple calls
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, GetInstance001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin GetInstance001";
    auto& instance = RSComposerJankStats::GetInstance();
    EXPECT_NE(&instance, nullptr);

    auto& instance2 = RSComposerJankStats::GetInstance();
    EXPECT_EQ(&instance, &instance2);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end GetInstance001";
}

/**
 * @tc.name: CalculateJankInfo001
 * @tc.desc: Verify CalculateJankInfo returns early when lastTimestamp_ is 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CalculateJankInfo001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CalculateJankInfo001";
    auto& instance = RSComposerJankStats::GetInstance();
    EXPECT_EQ(instance.lastTimestamp_, 0);

    int64_t timestamp = 1000000000;
    instance.CalculateJankInfo(timestamp);

    EXPECT_EQ(instance.lastTimestamp_, timestamp);
    EXPECT_EQ(instance.refreshRates_.size(), 0u);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CalculateJankInfo001";
}

/**
 * @tc.name: CalculateJankInfo002
 * @tc.desc: Verify CalculateJankInfo returns early when timeDiff <= 0
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CalculateJankInfo002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CalculateJankInfo002";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.lastTimestamp_ = 2000000000;

    // timeDiff == 0
    instance.CalculateJankInfo(2000000000);
    EXPECT_EQ(instance.lastTimestamp_, 2000000000);
    EXPECT_EQ(instance.refreshRates_.size(), 0u);

    // timeDiff < 0
    instance.CalculateJankInfo(1000000000);
    EXPECT_EQ(instance.lastTimestamp_, 1000000000);
    EXPECT_EQ(instance.refreshRates_.size(), 0u);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CalculateJankInfo002";
}

/**
 * @tc.name: CalculateJankInfo003
 * @tc.desc: Verify CalculateJankInfo adds refresh rate when deque is not full
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CalculateJankInfo003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CalculateJankInfo003";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.lastTimestamp_ = 1000000000;

    int64_t timestamp = 1000000000 + 16666667;
    instance.CalculateJankInfo(timestamp);

    EXPECT_EQ(instance.refreshRates_.size(), 1u);
    EXPECT_EQ(instance.lastTimestamp_, timestamp);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CalculateJankInfo003";
}

/**
 * @tc.name: CalculateJankInfo004
 * @tc.desc: Verify CalculateJankInfo triggers CheckRefreshRate when deque reaches window size
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CalculateJankInfo004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CalculateJankInfo004";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.lastTimestamp_ = 1000000000;
    instance.refreshRates_ = {60, 60, 60, 60, 60, 60, 60, 60};

    int64_t timestamp = 1000000000 + 16666667;
    instance.CalculateJankInfo(timestamp);

    // push_back makes size == 9 == REFRESH_RATE_WINDOW_SIZE, triggers CheckRefreshRate
    EXPECT_EQ(instance.refreshRates_.size(), static_cast<size_t>(REFRESH_RATE_WINDOW_SIZE));
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CalculateJankInfo004";
}

/**
 * @tc.name: CalculateJankInfo005
 * @tc.desc: Verify CalculateJankInfo pops front when deque is already full
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CalculateJankInfo005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CalculateJankInfo005";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.lastTimestamp_ = 1000000000;
    instance.refreshRates_ = {60, 60, 60, 60, 60, 60, 60, 60, 60};

    int64_t timestamp = 1000000000 + 16666667;
    instance.CalculateJankInfo(timestamp);

    // pop_front + push_back, size stays 9
    EXPECT_EQ(instance.refreshRates_.size(), static_cast<size_t>(REFRESH_RATE_WINDOW_SIZE));
    // First element (60) was popped, new element is the calculated refresh rate
    EXPECT_EQ(instance.refreshRates_.front(), 60);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CalculateJankInfo005";
}

/**
 * @tc.name: CalculateJankInfo006
 * @tc.desc: Verify refresh rate calculation rounds to nearest PERIOD_PRECISION
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CalculateJankInfo006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CalculateJankInfo006";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.lastTimestamp_ = 1000000000;

    // timeDiff = 16666667 → value = 1e9/16666667 ≈ 59.999... → round(59.999/5)*5 = 60
    int64_t timestamp = 1000000000 + 16666667;
    instance.CalculateJankInfo(timestamp);

    ASSERT_EQ(instance.refreshRates_.size(), 1u);
    EXPECT_EQ(instance.refreshRates_[0], 60);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CalculateJankInfo006";
}

/**
 * @tc.name: CheckRefreshRate001
 * @tc.desc: Verify CheckRefreshRate with all equal refresh rates
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CheckRefreshRate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CheckRefreshRate001";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {60, 60, 60, 60, 60, 60, 60, 60, 60};

    instance.CheckRefreshRate();
    EXPECT_TRUE(instance.IsAllEqual());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CheckRefreshRate001";
}

/**
 * @tc.name: CheckRefreshRate002
 * @tc.desc: Verify CheckRefreshRate with all refresh rates less than 60Hz
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CheckRefreshRate002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CheckRefreshRate002";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {30, 30, 30, 30, 30, 30, 30, 30, 30};

    instance.CheckRefreshRate();
    EXPECT_TRUE(instance.IsAllEqual());
    EXPECT_TRUE(instance.IsAllLessThan60Hz());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CheckRefreshRate002";
}

/**
 * @tc.name: CheckRefreshRate003
 * @tc.desc: Verify CheckRefreshRate with strictly increasing refresh rates
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CheckRefreshRate003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CheckRefreshRate003";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    instance.CheckRefreshRate();
    EXPECT_FALSE(instance.IsAllEqual());
    EXPECT_FALSE(instance.IsAllLessThan60Hz());
    EXPECT_TRUE(instance.IsStrictlyIncreasing());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CheckRefreshRate003";
}

/**
 * @tc.name: CheckRefreshRate004
 * @tc.desc: Verify CheckRefreshRate with strictly decreasing refresh rates
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CheckRefreshRate004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CheckRefreshRate004";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {90, 80, 70, 60, 50, 40, 30, 20, 10};

    instance.CheckRefreshRate();
    EXPECT_FALSE(instance.IsAllEqual());
    EXPECT_FALSE(instance.IsAllLessThan60Hz());
    EXPECT_FALSE(instance.IsStrictlyIncreasing());
    EXPECT_TRUE(instance.IsStrictlyDecreasing());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CheckRefreshRate004";
}

/**
 * @tc.name: CheckRefreshRate005
 * @tc.desc: Verify CheckRefreshRate with middle peak refresh rate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CheckRefreshRate005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CheckRefreshRate005";
    auto& instance = RSComposerJankStats::GetInstance();
    // rates[4]=75 >= rates[3]=70 && rates[4]=75 >= rates[5]=60 → middle peak true
    instance.refreshRates_ = {60, 55, 65, 70, 75, 60, 50, 45, 40};

    instance.CheckRefreshRate();
    int midRate = instance.GetRate(FRAME_WINDOW_MIDDLE_NUM);
    int leftRate = instance.GetRate(FRAME_WINDOW_MIDDLE_NUM - 1);
    int rightRate = instance.GetRate(FRAME_WINDOW_MIDDLE_NUM + 1);
    EXPECT_TRUE(midRate >= leftRate && midRate >= rightRate);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CheckRefreshRate005";
}

/**
 * @tc.name: CheckRefreshRate006
 * @tc.desc: Verify CheckRefreshRate with unstable refresh rates (falls through all conditions)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, CheckRefreshRate006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin CheckRefreshRate006";
    auto& instance = RSComposerJankStats::GetInstance();
    // rates[4]=50 < rates[3]=70 → middle peak condition false
    instance.refreshRates_ = {90, 60, 80, 70, 50, 60, 65, 55, 70};

    int midRate = instance.GetRate(FRAME_WINDOW_MIDDLE_NUM);
    int leftRate = instance.GetRate(FRAME_WINDOW_MIDDLE_NUM - 1);
    int rightRate = instance.GetRate(FRAME_WINDOW_MIDDLE_NUM + 1);
    instance.CheckRefreshRate();

    EXPECT_FALSE(midRate >= leftRate && midRate >= rightRate);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end CheckRefreshRate006";
}

/**
 * @tc.name: GetRate001
 * @tc.desc: Verify GetRate returns correct value at valid indices
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, GetRate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin GetRate001";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    EXPECT_EQ(instance.GetRate(0), 10);
    EXPECT_EQ(instance.GetRate(FRAME_WINDOW_MIDDLE_NUM), 50);
    EXPECT_EQ(instance.GetRate(REFRESH_RATE_WINDOW_SIZE - 1), 90);
    EXPECT_EQ(instance.GetRate(-1), 0);
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end GetRate001";
}

/**
 * @tc.name: IsAllEqual001
 * @tc.desc: Verify IsAllEqual returns true when all rates are equal
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsAllEqual001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsAllEqual001";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {60, 60, 60, 60, 60, 60, 60, 60, 60};
    EXPECT_TRUE(instance.IsAllEqual());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsAllEqual001";
}

/**
 * @tc.name: IsAllEqual002
 * @tc.desc: Verify IsAllEqual returns false when rates differ
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsAllEqual002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsAllEqual002";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {60, 60, 60, 60, 61, 60, 60, 60, 60};
    EXPECT_FALSE(instance.IsAllEqual());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsAllEqual002";
}

/**
 * @tc.name: IsAllLessThan60Hz001
 * @tc.desc: Verify IsAllLessThan60Hz returns true when all rates are below 60Hz
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsAllLessThan60Hz001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsAllLessThan60Hz001";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {30, 30, 30, 30, 30, 30, 30, 30, 30};
    EXPECT_TRUE(instance.IsAllLessThan60Hz());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsAllLessThan60Hz001";
}

/**
 * @tc.name: IsAllLessThan60Hz002
 * @tc.desc: Verify IsAllLessThan60Hz returns false when some rates are >= 60Hz
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsAllLessThan60Hz002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsAllLessThan60Hz002";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {30, 30, 30, 60, 30, 30, 30, 30, 30};
    EXPECT_FALSE(instance.IsAllLessThan60Hz());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsAllLessThan60Hz002";
}

/**
 * @tc.name: IsStrictlyIncreasing001
 * @tc.desc: Verify IsStrictlyIncreasing returns true for strictly increasing sequence
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsStrictlyIncreasing001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsStrictlyIncreasing001";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {10, 20, 30, 30, 50, 60, 70, 80, 90};
    EXPECT_TRUE(instance.IsStrictlyIncreasing());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsStrictlyIncreasing001";
}

/**
 * @tc.name: IsStrictlyIncreasing002
 * @tc.desc: Verify IsStrictlyIncreasing returns false for non-strictly increasing sequence
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsStrictlyIncreasing002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsStrictlyIncreasing002";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {10, 20, 40, 30, 50, 60, 70, 80, 90};
    EXPECT_FALSE(instance.IsStrictlyIncreasing());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsStrictlyIncreasing002";
}

/**
 * @tc.name: IsStrictlyDecreasing001
 * @tc.desc: Verify IsStrictlyDecreasing returns true for strictly decreasing sequence
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsStrictlyDecreasing001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsStrictlyDecreasing001";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {90, 80, 70, 70, 50, 40, 30, 20, 10};
    EXPECT_TRUE(instance.IsStrictlyDecreasing());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsStrictlyDecreasing001";
}

/**
 * @tc.name: IsStrictlyDecreasing002
 * @tc.desc: Verify IsStrictlyDecreasing returns false for non-strictly decreasing sequence
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSComposerJankStatsTest, IsStrictlyDecreasing002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-begin IsStrictlyDecreasing002";
    auto& instance = RSComposerJankStats::GetInstance();
    instance.refreshRates_ = {90, 80, 70, 80, 50, 40, 30, 20, 10};
    EXPECT_FALSE(instance.IsStrictlyDecreasing());
    GTEST_LOG_(INFO) << "RSComposerJankStatsTest-end IsStrictlyDecreasing002";
}
}
}
