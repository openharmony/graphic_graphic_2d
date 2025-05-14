/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "feature/capture/rs_ui_capture_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUiCaptureHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUiCaptureHelperTest::SetUpTestCase() {}
void RSUiCaptureHelperTest::TearDownTestCase() {}
void RSUiCaptureHelperTest::SetUp() {}
void RSUiCaptureHelperTest::TearDown() {}

/**
 * @tc.name: UiCaptureCmdsExecutedFlagTest
 * @tc.desc: verify UiCaptureCmdsExecutedFlag
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSUiCaptureHelperTest, UiCaptureCmdsExecutedFlagTest, TestSize.Level1)
{
    NodeId id = 0;
    auto uiCaptureHelper = std::make_unique<RSUiCaptureHelper>();
    uiCaptureHelper->InsertUiCaptureCmdsExecutedFlag(id, false);
    auto cmdFlag = uiCaptureHelper->GetUiCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, false);
    uiCaptureHelper->EraseUiCaptureCmdsExecutedFlag(id);
    cmdFlag = uiCaptureHelper->GetUiCaptureCmdsExecutedFlag(id);
    EXPECT_EQ(cmdFlag.first, true);
}
} // namespace OHOS::Rosen