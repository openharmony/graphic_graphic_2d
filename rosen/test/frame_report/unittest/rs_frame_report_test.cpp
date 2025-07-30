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
 * @tc.name: GetEnable001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, GetEnable001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    fr.LoadLibrary();
    fr.GetEnable();
    EXPECT_EQ(fr.GetEnable(), 1);
}

/**
 * @tc.name: SendCommandsStart001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, SendCommandsStart001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    fr.LoadLibrary();
    EXPECT_EQ(fr.sendCommandsStartFunc_, nullptr);
    fr.SendCommandsStart();
}

/**
 * @tc.name: SetFrameParam001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, SetFrameParam001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    EXPECT_EQ(fr.setFrameParamFunc_, nullptr);
    fr.SetFrameParam(0, 0, 0, 0);
}

/**
 * @tc.name: LoadLibrary001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, LoadLibrary001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    fr.LoadLibrary();
    EXPECT_TRUE(fr.frameSchedSoLoaded_);
}

/**
 * @tc.name: LoadSymbol001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, LoadSymbol001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    fr.LoadSymbol("function");
    fr.LoadLibrary();
    fr.LoadSymbol("function");
    EXPECT_TRUE(fr.frameSchedSoLoaded_);
}

/**
 * @tc.name: LoadLibrary002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, LoadLibrary002, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    fr.frameSchedSoLoaded_ = false;
    fr.LoadLibrary();
    fr.frameSchedSoLoaded_ = true;
    EXPECT_TRUE(fr.LoadLibrary());
}

/**
 * @tc.name: ReportSchedEvent001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportSchedEvent001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    EXPECT_EQ(fr.reportSchedEventFunc_, nullptr);
    fr.ReportSchedEvent(FrameSchedEvent::INIT, {});
    EXPECT_NE(fr.reportSchedEventFunc_, nullptr);
    fr.ReportSchedEvent(FrameSchedEvent::INIT, {});
}

/**
 * @tc.name: ReportScbSceneInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportScbSceneInfo001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    std::string description = "test";
    bool eventStatus = true;
    fr.ReportScbSceneInfo(description, eventStatus);
    EXPECT_NE(fr.reportSchedEventFunc_, nullptr);
    eventStatus = false;
    fr.ReportScbSceneInfo(description, eventStatus);
    EXPECT_NE(fr.reportSchedEventFunc_, nullptr);
}

/**
 * @tc.name: ReportFrameDeadline001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportFrameDeadline001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    int deadline = 8333;
    uint32_t currentRate = 120;
    fr.ReportFrameDeadline(deadline, currentRate);
    EXPECT_NE(fr.reportSchedEventFunc_, nullptr);
    deadline = -8333;
    fr.ReportFrameDeadline(deadline, currentRate);
    EXPECT_NE(fr.reportSchedEventFunc_, nullptr);
    deadline = 0;
    fr.ReportFrameDeadline(deadline, currentRate);
    EXPECT_NE(fr.reportSchedEventFunc_, nullptr);
}

/**
 * @tc.name: ReportBufferCount001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportBufferCount001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    EXPECT_EQ(fr.bufferCount_, 0);
    fr.ReportBufferCount(1);
    EXPECT_EQ(fr.bufferCount_, 1);
}

/**
 * @tc.name: ReportHardwareInfo001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ReportHardwareInfo001, TestSize.Level1)
{
    RsFrameReport& fr = RsFrameReport::GetInstance();
    EXPECT_EQ(fr.hardwareTid_, 0);
    fr.ReportHardwareInfo(1);
    EXPECT_EQ(fr.hardwareTid_, 1);
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
    RsFrameReport& fr = RsFrameReport::GetInstance();
    fr.ReportUnmarshalData(unmarshalTid, dataSize);
    unmarshalTid = 0;
    fr.ReportUnmarshalData(unmarshalTid, dataSize);
    EXPECT_EQ(fr.hardwareTid_, 1);
}
} // namespace Rosen
} // namespace OHOS
