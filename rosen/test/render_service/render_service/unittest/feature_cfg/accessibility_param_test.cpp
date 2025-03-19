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

#include "accessibility_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class AccessibilityParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AccessibilityParamTest::SetUpTestCase() {}
void AccessibilityParamTest::TearDownTestCase() {}
void AccessibilityParamTest::SetUp() {}
void AccessibilityParamTest::TearDown() {}

/**
 * @tc.name: HighContrastEnabledTest
 * @tc.desc: Verify the SetHighContrastEnabled IsHighContrastEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, HighContrastEnabledTest, Function | SmallTest | Level1)
{
    AccessibilityParam::SetHighContrastEnabled(true);
    ASSERT_EQ(AccessibilityParam::IsHighContrastEnabled(), true);
    AccessibilityParam::SetHighContrastEnabled(false);
    ASSERT_EQ(AccessibilityParam::IsHighContrastEnabled(), false);
}

/**
 * @tc.name: CurtainScreenEnabledTest
 * @tc.desc: Verify the SetCurtainScreenEnabled IsCurtainScreenEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, CurtainScreenEnabledTest, Function | SmallTest | Level1)
{
    AccessibilityParam::SetCurtainScreenEnabled(true);
    ASSERT_EQ(AccessibilityParam::IsCurtainScreenEnabled(), true);
    AccessibilityParam::SetCurtainScreenEnabled(false);
    ASSERT_EQ(AccessibilityParam::IsCurtainScreenEnabled(), false);
}

/**
 * @tc.name: ColorReverseEnabledTest
 * @tc.desc: Verify the SetColorReverseEnabled IsColorReverseEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, ColorReverseEnabledTest, Function | SmallTest | Level1)
{
    AccessibilityParam::SetColorReverseEnabled(true);
    ASSERT_EQ(AccessibilityParam::IsColorReverseEnabled(), true);
    AccessibilityParam::SetColorReverseEnabled(false);
    ASSERT_EQ(AccessibilityParam::IsColorReverseEnabled(), false);
}

/**
 * @tc.name: ColorCorrectionEnabledTest
 * @tc.desc: Verify the SetColorCorrectionEnabled IsColorCorrectionEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, ColorCorrectionEnabledTest, Function | SmallTest | Level1)
{
    AccessibilityParam::SetColorCorrectionEnabled(true);
    ASSERT_EQ(AccessibilityParam::IsColorCorrectionEnabled(), true);
    AccessibilityParam::SetColorCorrectionEnabled(false);
    ASSERT_EQ(AccessibilityParam::IsColorCorrectionEnabled(), false);
}
} // namespace Rosen
} // namespace OHOS