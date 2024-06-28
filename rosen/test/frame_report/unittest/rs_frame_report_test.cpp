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
    RsFrameReport::GetInstance().LoadLibrary();
    RsFrameReport::GetInstance().GetEnable();
}

/**
 * @tc.name: ProcessCommandsStart001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, ProcessCommandsStart001, TestSize.Level1)
{
    RsFrameReport::GetInstance().ProcessCommandsStart();
}

/**
 * @tc.name: AnimateStart001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, AnimateStart001, TestSize.Level1)
{
    RsFrameReport::GetInstance().AnimateStart();
}

/**
 * @tc.name: RenderStart001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, RenderStart001, TestSize.Level1)
{
    uint64_t timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    RsFrameReport::GetInstance().RenderStart(timestamp);
}

/**
 * @tc.name: RSRenderStart001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, RSRenderStart001, TestSize.Level1)
{
    RsFrameReport::GetInstance().RSRenderStart();
}

/**
 * @tc.name: RenderEnd001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, RenderEnd001, TestSize.Level1)
{
    RsFrameReport::GetInstance().RenderEnd();
}

/**
 * @tc.name: RSRenderEnd001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, RSRenderEnd001, TestSize.Level1)
{
    RsFrameReport::GetInstance().RSRenderEnd();
}

/**
 * @tc.name: SendCommandsStart001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, SendCommandsStart001, TestSize.Level1)
{
    RsFrameReport::GetInstance().SendCommandsStart();
}

/**
 * @tc.name: SetFrameParam001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportTest, SetFrameParam001, TestSize.Level1)
{
    RsFrameReport::GetInstance().SetFrameParam(0, 0, 0, 0);
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
    fr.LoadLibrary();
    fr.CloseLibrary();
}
} // namespace Rosen
} // namespace OHOS
