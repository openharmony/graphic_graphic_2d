/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "rs_frame_report.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsFrameReportTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameReportTest::SetUpTestCase() {}
void RsFrameReportTest::TearDownTestCase() {}
void RsFrameReportTest::SetUp() {}
void RsFrameReportTest::TearDown() {}

/**
 * @tc.name: NoBranchTestCase001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, NoBranchTestCase, TestSize.Level1)
{
    ASSERT_FALSE(RsFrameReport::IsInitSchedCompleted());
    RsFrameReport::InitDeadline();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
    
    RsFrameReport::SetFrameParam(0, 0, 0, 0);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::SendCommandsStart();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::RenderStart(1, 1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::RenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::DirectRenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::UniRenderStart();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::UniRenderEnd();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckUnblockMainThreadPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckPostAndWaitPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::CheckBeginFlushPoint();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::ReportComposerInfo(1, 1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());

    RsFrameReport::ReportDDGRTaskInfo();
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportScbSceneInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportScbSceneInfo001, TestSize.Level1)
{
    std::string description = "test";
    bool eventStatus = true;
    RsFrameReport::ReportScbSceneInfo(description, eventStatus);
 
    eventStatus = false;
    RsFrameReport::ReportScbSceneInfo(description, eventStatus);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportFrameDeadline001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportFrameDeadline001, TestSize.Level1)
{
    int deadline = 8333;
    uint32_t currentRate = 120;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
 
    deadline = -8333;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
 
    deadline = 0;
    RsFrameReport::ReportFrameDeadline(deadline, currentRate);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportBufferCount001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportBufferCount001, TestSize.Level1)
{
    RsFrameReport::ReportBufferCount(1);
    RsFrameReport::ReportBufferCount(1);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}

/**
 * @tc.name: ReportUnmarshalData001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportUnmarshalData001, TestSize.Level1)
{
    int unmarshalTid = 123456;
    size_t dataSize = 20 * 1024;
    RsFrameReport::ReportUnmarshalData(unmarshalTid, dataSize);
 
    unmarshalTid = 0;
    RsFrameReport::ReportUnmarshalData(unmarshalTid, dataSize);
    ASSERT_TRUE(RsFrameReport::IsInitSchedCompleted());
}
} // namespace Rosen
} // namespace OHOS
