/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_divided_ui_capture.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSDividedUICaptureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDividedUICaptureTest::SetUpTestCase() {}
void RSDividedUICaptureTest::TearDownTestCase() {}
void RSDividedUICaptureTest::SetUp() {}
void RSDividedUICaptureTest::TearDown() {}

/**
 * @tc.name: TakeLocalCapture
 * @tc.desc: test results of RegisterNode
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSDividedUICaptureTest, TakeLocalCapture, TestSize.Level1)
{
    RSDividedUICapture rsDividedUICapture(1, 1.0, 1.0);
    auto pixelmap = rsDividedUICapture.TakeLocalCapture();

    RSDividedUICapture rsDividedUICapture2(1, 0, 1.0);
    pixelmap = rsDividedUICapture2.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);

    RSDividedUICapture rsDividedUICapture3(1, 1.0, 0);
    pixelmap = rsDividedUICapture3.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);

    RSDividedUICapture rsDividedUICapture4(1, -1, 1);
    pixelmap = rsDividedUICapture4.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);

    RSDividedUICapture rsDividedUICapture5(1, 1.0, -1);
    pixelmap = rsDividedUICapture5.TakeLocalCapture();
    ASSERT_EQ(nullptr, pixelmap);
}

} // namespace Rosen
} // namespace OHOS