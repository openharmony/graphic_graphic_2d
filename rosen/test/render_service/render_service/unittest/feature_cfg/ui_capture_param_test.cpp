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

#include <gtest/gtest.h>
#include <test_header.h>

#include "ui_capture_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class UICaptureParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UICaptureParamTest::SetUpTestCase() {}
void UICaptureParamTest::TearDownTestCase() {}
void UICaptureParamTest::SetUp() {}
void UICaptureParamTest::TearDown() {}

/*
 * @tc.name: IsUseOptimizedFlushAndSubmitEnabled
 * @tc.desc: Test IsUseOptimizedFlushAndSubmitEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UICaptureParamTest, IsUseOptimizedFlushAndSubmitEnabled, Function | SmallTest | Level1)
{
    std::shared_ptr<UICaptureParam> uiCaptureParam = std::make_shared<UICaptureParam>();
    uiCaptureParam->SetUseOptimizedFlushAndSubmitEnabled(true);
    EXPECT_EQ(uiCaptureParam->IsUseOptimizedFlushAndSubmitEnabled(), true);
    uiCaptureParam->SetUseOptimizedFlushAndSubmitEnabled(false);
    EXPECT_EQ(uiCaptureParam->IsUseOptimizedFlushAndSubmitEnabled(), false);
}

/*
 * @tc.name: IsUseDMAProcessEnabled
 * @tc.desc: Test IsUseDMAProcessEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(UICaptureParamTest, IsUseDMAProcessEnabled, Function | SmallTest | Level1)
{
    std::shared_ptr<UICaptureParam> uiCaptureParam = std::make_shared<UICaptureParam>();
    uiCaptureParam->SetUseDMAProcessEnabled(true);
    EXPECT_EQ(uiCaptureParam->IsUseDMAProcessEnabled(), true);
    uiCaptureParam->SetUseDMAProcessEnabled(false);
    EXPECT_EQ(uiCaptureParam->IsUseDMAProcessEnabled(), false);
}
} // namespace OHOS::Rosen