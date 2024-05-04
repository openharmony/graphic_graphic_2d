/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * @tc.name: CreateRSTimeOutDetector
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDetectorTest, CreateRSTimeOutDetector, TestSize.Level1)
{
    ASSERT_NE(rsDetector, nullptr);
}

/**
 * @tc.name: settings
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEventDetectorTest, settings, TestSize.Level1)
{
    rsDetector->SetParam("0", "1");
    rsDetector->SetParam("timeOutThresholdMs", "1");
    rsDetector->SetParam("timeOutThresholdMs", "-1");
    rsDetector->SetLoopStartTag();
    std::string bundle = "bundle";
    std::string ability = "ability";
    rsDetector->SetLoopFinishTag(1, 1, bundle, ability);
}

/**
 * @tc.name: GetSysTimeMsTest
 * @tc.desc: Verify function GetSysTimeMs
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventDetectorTest, GetSysTimeMsTest, TestSize.Level1)
{
    EXPECT_NE(RSEventTimer::GetSysTimeMs(), 0);
}

/**
 * @tc.name: RSTimeOutDetectorTest
 * @tc.desc: Verify function RSTimeOutDetector
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventDetectorTest, RSTimeOutDetectorTest, TestSize.Level1)
{
    auto rsTimeOutDetector = std::make_shared<RSTimeOutDetector>(1, "0");
    EXPECT_TRUE(rsTimeOutDetector);
}

/**
 * @tc.name: EventReportTest
 * @tc.desc: Verify function EventReport
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSEventDetectorTest, EventReportTest, TestSize.Level1)
{
    auto rsTimeOutDetector = std::make_shared<RSTimeOutDetector>(1, "0");
    rsTimeOutDetector->eventCallback_ = [](const RSSysEventMsg& eventMsg) {};
    rsTimeOutDetector->EventReport(1);
    EXPECT_TRUE(rsTimeOutDetector->eventCallback_ != nullptr);
}
} // namespace Rosen
} // namespace OHOS