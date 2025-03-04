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
 * @tc.name: SetHighContrastEnabled
 * @tc.desc: Verify the SetHighContrastEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, SetHighContrastEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.SetHighContrastEnabled(true);
    ASSERT_EQ(accessibilityParam_.isHighContrastEnabled_, true);
    accessibilityParam_.SetHighContrastEnabled(false);
    ASSERT_EQ(accessibilityParam_.isHighContrastEnabled_, false);
}

/**
 * @tc.name: SetCurtainScreenEnabled
 * @tc.desc: Verify the SetCurtainScreenEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, SetCurtainScreenEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.SetCurtainScreenEnabled(true);
    ASSERT_EQ(accessibilityParam_.isCurtainScreenEnabled_, true);
    accessibilityParam_.SetCurtainScreenEnabled(false);
    ASSERT_EQ(accessibilityParam_.isCurtainScreenEnabled_, false);
}

/**
 * @tc.name: SetColorReverseEnabled
 * @tc.desc: Verify the SetColorReverseEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, SetColorReverseEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.SetColorReverseEnabled(true);
    ASSERT_EQ(accessibilityParam_.isColorReverseEnabled_, true);
    accessibilityParam_.SetColorReverseEnabled(false);
    ASSERT_EQ(accessibilityParam_.isColorReverseEnabled_, false);
}

/**
 * @tc.name: SetColorCorrectionEnabled
 * @tc.desc: Verify the SetColorCorrectionEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, SetColorCorrectionEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.SetColorCorrectionEnabled(true);
    ASSERT_EQ(accessibilityParam_.isColorCorrectionEnabled_, true);
    accessibilityParam_.SetColorCorrectionEnabled(false);
    ASSERT_EQ(accessibilityParam_.isColorCorrectionEnabled_, false);
}

/**
 * @tc.name: IsHighContrastEnabled
 * @tc.desc: Verify the IsHighContrastEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, IsHighContrastEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.isColorCorrectionEnabled_ = true;
    ASSERT_EQ(accessibilityParam_.IsHighContrastEnabled(), true);
    accessibilityParam_.isColorCorrectionEnabled_ = false;
    ASSERT_EQ(accessibilityParam_.IsHighContrastEnabled(), false);
}

/**
 * @tc.name: IsCurtainScreenEnabled
 * @tc.desc: Verify the IsCurtainScreenEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, IsCurtainScreenEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.isCurtainScreenEnabled_ = true;
    ASSERT_EQ(accessibilityParam_.IsCurtainScreenEnabled(), true);
    accessibilityParam_.isCurtainScreenEnabled_ = false;
    ASSERT_EQ(accessibilityParam_.IsCurtainScreenEnabled(), false);
}

/**
 * @tc.name: IsColorReverseEnabled
 * @tc.desc: Verify the IsColorReverseEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, IsColorReverseEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.isColorReverseEnabled_ = true;
    ASSERT_EQ(accessibilityParam_.IsColorReverseEnabled(), true);
    accessibilityParam_.isColorReverseEnabled_ = false;
    ASSERT_EQ(accessibilityParam_.IsColorReverseEnabled(), false);
}

/**
 * @tc.name: IsColorCorrectionEnabled
 * @tc.desc: Verify the IsColorCorrectionEnabled function
 * @tc.type: FUNC
 * @tc.require: #IBIE4T
 */
HWTEST_F(AccessibilityParamTest, IsColorCorrectionEnabled, Function | SmallTest | Level1)
{
    AccessibilityParam accessibilityParam_;
    accessibilityParam_.isColorCorrectionEnabled_ = true;
    ASSERT_EQ(accessibilityParam_.IsColorCorrectionEnabled(), true);
    accessibilityParam_.isColorCorrectionEnabled_ = false;
    ASSERT_EQ(accessibilityParam_.IsColorCorrectionEnabled(), false);
}
} // namespace Rosen
} // namespace OHOS