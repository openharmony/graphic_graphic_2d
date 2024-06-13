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

#include "rs_frame_report_ext.h"

namespace OHOS {
namespace Rosen {
using namespace testing;
using namespace testing::ext;

class RsFrameReportExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFrameReportExtTest::SetUpTestCase() {}
void RsFrameReportExtTest::TearDownTestCase() {}
void RsFrameReportExtTest::SetUp() {}
void RsFrameReportExtTest::TearDown() {}

/**
 * @tc.name: Init
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportExtTest, Init001, Function | MediumTest | Level2)
{
    OHOS::Rosen::RsFrameReportExt rsFrameReportExt_;
    rsFrameReportExt_.Init();
}

/**
 * @tc.name: GetEnable
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportExtTest, GetEnable001, Function | MediumTest | Level2)
{
    OHOS::Rosen::RsFrameReportExt rsFrameReportExt_;
    rsFrameReportExt_.GetEnable();
}

/**
 * @tc.name: HandleSwapBuffer
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportExtTest, HandleSwapBuffer001, Function | MediumTest | Level2)
{
    OHOS::Rosen::RsFrameReportExt rsFrameReportExt_;
    rsFrameReportExt_.HandleSwapBuffer();
}

/**
 * @tc.name: RequestNextVSync
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportExtTest, RequestNextVSync001, Function | MediumTest | Level2)
{
    OHOS::Rosen::RsFrameReportExt rsFrameReportExt_;
    rsFrameReportExt_.RequestNextVSync();
}

/**
 * @tc.name: ReceiveVSync
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RsFrameReportExtTest, ReceiveVSync001, Function | MediumTest | Level2)
{
    OHOS::Rosen::RsFrameReportExt rsFrameReportExt_;
    rsFrameReportExt_.ReceiveVSync();
}
} // namespace Rosen
} // namespace OHOS
