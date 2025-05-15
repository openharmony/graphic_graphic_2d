/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "platform/ohos/rs_jank_stats_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSJankStatsHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSJankStatsHelperTest::SetUpTestCase() {}
void RSJankStatsHelperTest::TearDownTestCase() {}
void RSJankStatsHelperTest::SetUp() {}
void RSJankStatsHelperTest::TearDown() {}

/**
 * @tc.name: JankStatsStartTest
 * @tc.desc: test RSJankStatsRenderFrameHelper functions
 * @tc.type: FUNC
 * @tc.require: issue#IC58TN
 */
HWTEST_F(RSJankStatsHelperTest, JankStatsStartTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStatsRenderFrameHelper::GetInstance();
    rsJankStats.JankStatsStart();
    EXPECT_FALSE(rsJankStats.rtSkipJankAnimatorFrame_.load());
}

/**
 * @tc.name: JankStatsAfterSyncTest
 * @tc.desc: test RSJankStatsRenderFrameHelper functions
 * @tc.type: FUNC
 * @tc.require: issue#IC58TN
 */
HWTEST_F(RSJankStatsHelperTest, JankStatsAfterSyncTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStatsRenderFrameHelper::GetInstance();
    int accumulatedBufferCount = 0;
    rsJankStats.JankStatsAfterSync(nullptr, accumulatedBufferCount);
    EXPECT_FALSE(rsJankStats.doJankStats_);
    auto params = std::make_unique<RSRenderThreadParams>();
    rsJankStats.JankStatsAfterSync(params, accumulatedBufferCount);
    EXPECT_FALSE(rsJankStats.doJankStats_);
    params->SetIsUniRenderAndOnVsync(true);
    rsJankStats.JankStatsAfterSync(params, accumulatedBufferCount);
    EXPECT_TRUE(rsJankStats.doJankStats_);
}

/**
 * @tc.name: JankStatsEndTest
 * @tc.desc: test RSJankStatsRenderFrameHelper functions
 * @tc.type: FUNC
 * @tc.require: issue#IC58TN
 */
HWTEST_F(RSJankStatsHelperTest, JankStatsEndTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStatsRenderFrameHelper::GetInstance();
    auto params = std::make_unique<RSRenderThreadParams>();
    int accumulatedBufferCount = 0;
    rsJankStats.JankStatsAfterSync(params, accumulatedBufferCount);
    rsJankStats.JankStatsEnd(STANDARD_REFRESH_RATE);
    EXPECT_FALSE(rsJankStats.doJankStats_);
    EXPECT_FALSE(rsJankStats.rtDiscardJankFrames_.load());
    params->SetIsUniRenderAndOnVsync(true);
    rsJankStats.JankStatsAfterSync(params, accumulatedBufferCount);
    rsJankStats.JankStatsEnd(STANDARD_REFRESH_RATE);
    EXPECT_TRUE(rsJankStats.doJankStats_);
    EXPECT_FALSE(rsJankStats.rtDiscardJankFrames_.load());
}

/**
 * @tc.name: SetSkipJankAnimatorFrameTest
 * @tc.desc: test RSJankStatsRenderFrameHelper functions
 * @tc.type: FUNC
 * @tc.require: issue#IC58TN
 */
HWTEST_F(RSJankStatsHelperTest, SetSkipJankAnimatorFrameTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStatsRenderFrameHelper::GetInstance();
    rsJankStats.SetSkipJankAnimatorFrame(false);
    EXPECT_FALSE(rsJankStats.rtSkipJankAnimatorFrame_.load());
    rsJankStats.SetSkipJankAnimatorFrame(true);
    EXPECT_TRUE(rsJankStats.rtSkipJankAnimatorFrame_.load());
}

/**
 * @tc.name: SetDiscardJankFramesTest
 * @tc.desc: test RSJankStatsRenderFrameHelper functions
 * @tc.type: FUNC
 * @tc.require: issue#IC58TN
 */
HWTEST_F(RSJankStatsHelperTest, SetDiscardJankFramesTest, TestSize.Level1)
{
    auto& rsJankStats = RSJankStatsRenderFrameHelper::GetInstance();
    rsJankStats.SetDiscardJankFrames(false);
    EXPECT_FALSE(rsJankStats.rtDiscardJankFrames_.load());
    rsJankStats.SetDiscardJankFrames(true);
    EXPECT_TRUE(rsJankStats.rtDiscardJankFrames_.load());
}
} // namespace Rosen
} // namespace OHOS
