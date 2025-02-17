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
    fr.CloseLibrary();
    fr.GetEnable();
    EXPECT_EQ(fr.GetEnable(), 0);
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
    EXPECT_EQ(fr.sendCommandsStartFunc_, nullptr);
    fr.SendCommandsStart();
    EXPECT_NE(fr.sendCommandsStartFunc_, nullptr);
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
    EXPECT_NE(fr.setFrameParamFunc_, nullptr);
    fr.SetFrameParam(1, 1, 1, 1);
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
    fr.CloseLibrary();
    EXPECT_FALSE(fr.frameSchedSoLoaded_);
    fr.LoadLibrary();
    EXPECT_TRUE(fr.frameSchedSoLoaded_);
    fr.CloseLibrary();
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
    fr.CloseLibrary();
    EXPECT_FALSE(fr.frameSchedSoLoaded_);
    fr.LoadSymbol("function");
    fr.LoadLibrary();
    EXPECT_TRUE(fr.frameSchedSoLoaded_);
    fr.LoadSymbol("function");
    fr.CloseLibrary();
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
} // namespace Rosen
} // namespace OHOS
