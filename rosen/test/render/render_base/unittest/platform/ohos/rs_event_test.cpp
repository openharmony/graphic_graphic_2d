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

#include "platform/common/rs_event_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class RSEventDetectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSBaseEventDetector> rsDetector = nullptr;
};

void RSEventDetectorTest::SetUpTestCase()
{
    rsDetector = RSBaseEventDetector::CreateRSTimeOutDetector(1, "0");
}
void RSEventDetectorTest::TearDownTestCase() {}
void RSEventDetectorTest::SetUp() {}
void RSEventDetectorTest::TearDown() {}

/**
 * @tc.name: settings001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDetectorTest, settings001, TestSize.Level1)
{
    rsDetector->SetParam("0", "1");
    rsDetector->SetParam("timeOutThresholdMs", "1");
    rsDetector->SetParam("timeOutThresholdMs", "-1");
    rsDetector->SetLoopStartTag();
    std::string abilityStr = "ability";
    std::string bundleStr = "bundle";
    rsDetector->SetLoopFinishTag(1, 1, bundleStr, abilityStr);
    RSTimeOutDetector rsTimeOutDetector(0, "1");
    rsTimeOutDetector.SetLoopStartTag();
    rsTimeOutDetector.timeOutThresholdMs_ = rsTimeOutDetector.startTimeStampMs_;
    rsTimeOutDetector.SetLoopFinishTag(1, 1, bundleStr, abilityStr);
    rsTimeOutDetector.startTimeStampMs_ += rsTimeOutDetector.startTimeStampMs_;
    rsTimeOutDetector.SetLoopFinishTag(1, 1, bundleStr, abilityStr);
    ASSERT_EQ(rsTimeOutDetector.focusAppPid_, 1);
    ASSERT_EQ(rsTimeOutDetector.focusAppUid_, 1);
    ASSERT_EQ(rsTimeOutDetector.focusAppBundleName_, bundleStr);
    ASSERT_EQ(rsTimeOutDetector.focusAppAbilityName_, abilityStr);
}

/**
 * @tc.name: CreateRSTimeOutDetector001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDetectorTest, CreateRSTimeOutDetector001, TestSize.Level1)
{
    ASSERT_NE(rsDetector, nullptr);
}

/**
 * @tc.name: RSTimeOutDetectorTest001
 * @tc.desc: Verify function RSTimeOutDetector
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventDetectorTest, RSTimeOutDetectorTest001, TestSize.Level1)
{
    auto rsTimeOutDetector1 = std::make_shared<RSTimeOutDetector>(1, "0");
    EXPECT_TRUE(rsTimeOutDetector1);
}

/**
 * @tc.name: GetSysTimeMsTest001
 * @tc.desc: Verify function GetSysTimeMs
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventDetectorTest, GetSysTimeMsTest001, TestSize.Level1)
{
    EXPECT_NE(RSEventTimer::GetSysTimeMs(), 0);
}

/**
 * @tc.name: EventReportTest001
 * @tc.desc: Verify function EventReport
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventDetectorTest, EventReportTest001, TestSize.Level1)
{
    auto rsTimeOutDetector1 = std::make_shared<RSTimeOutDetector>(1, "0");
    rsTimeOutDetector1->EventReport(1);
    EXPECT_TRUE(rsTimeOutDetector1->eventCallback_ == nullptr);
    rsTimeOutDetector1->eventCallback_ = [](const RSSysEventMsg& eventMsg) {};
    rsTimeOutDetector1->EventReport(1);
    EXPECT_TRUE(rsTimeOutDetector1->eventCallback_ != nullptr);
}
} // namespace Rosen
} // namespace OHOS