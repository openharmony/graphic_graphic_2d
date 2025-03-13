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

#include "gtest/gtest.h"
#include "render_thread/jank_detector/rs_jank_detector.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsJankDetectorsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsJankDetectorsTest::SetUpTestCase() {}
void RsJankDetectorsTest::TearDownTestCase() {}
void RsJankDetectorsTest::SetUp() {}
void RsJankDetectorsTest::TearDown() {}

/**
 * @tc.name: UpdateUiDrawFrameMsgTest1
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsJankDetectorsTest, UpdateUiDrawFrameMsgTest1, TestSize.Level1)
{
    uint64_t endTimeStamp = 10;
    uint64_t startTimeStamp = 1;
    std::string abilityName = "ABILITY_NAME";
    RSJankDetector rsJankDetector;
    rsJankDetector.UpdateUiDrawFrameMsg(startTimeStamp, endTimeStamp, abilityName);
    EXPECT_FALSE(rsJankDetector.uiDrawFrames_.empty());
}

/**
 * @tc.name: GetRefreshPeriodTest1
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsJankDetectorsTest, GetRefreshPeriodTest1, TestSize.Level1)
{
    RSJankDetector rsJankDetector;
    auto refreshPeriod = rsJankDetector.GetRefreshPeriod();
    ASSERT_EQ(refreshPeriod, 16666667);
}

/**
 * @tc.name: CalculateSkippedFrameTest1
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsJankDetectorsTest, CalculateSkippedFrameTest1, TestSize.Level1)
{
    uint64_t endTimeStamp = 10;
    uint64_t startTimeStamp = 1;
    uint64_t refreshPeriod = 16666667;
    int jankSkippedThreshold = 5;
    uint64_t noDrawThreshold = 5000000000;
    std::string abilityName = "ABILITY_NAME";
    RSJankDetector rsJankDetector;
    rsJankDetector.UpdateUiDrawFrameMsg(startTimeStamp, endTimeStamp, abilityName);
    uint64_t renderStartTimeStamp = 1;
    uint64_t renderEndTimeStamp = 83333336;
    uint64_t renderDrawTime = renderEndTimeStamp - renderStartTimeStamp;
    int skippedFrame = static_cast<int>(renderDrawTime / refreshPeriod);
    rsJankDetector.CalculateSkippedFrame(renderStartTimeStamp, renderEndTimeStamp);
    ASSERT_TRUE(skippedFrame >= jankSkippedThreshold);
    uint64_t renderStartTimeStamp_ = 1;
    uint64_t renderEndTimeStamp_ = 5000000001;
    uint64_t renderDrawTime_ = renderEndTimeStamp_ - renderStartTimeStamp_;
    rsJankDetector.CalculateSkippedFrame(renderStartTimeStamp_, renderEndTimeStamp_);
    ASSERT_TRUE(renderDrawTime_ >= noDrawThreshold);
}

/**
 * @tc.name: SetRefreshPeriodTest1
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsJankDetectorsTest, SetRefreshPeriodTest1, TestSize.Level1)
{
    auto refreshPeriod = 1;
    RSJankDetector rsJankDetector;
    rsJankDetector.SetRefreshPeriod(refreshPeriod);
    ASSERT_EQ(rsJankDetector.GetRefreshPeriod(), refreshPeriod);
}
} // namespace OHOS::Rosen
