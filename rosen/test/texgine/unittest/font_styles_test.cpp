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

#include <gtest/gtest.h>

#include "font_styles.h"
#include "param_test_macros.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class FontStylesTest : public testing::Test {
};

FontStyles GetFontStyles(int weight, int style)
{
    FontStyles fs(static_cast<FontWeight>(weight),
                  static_cast<FontStyle>(style));
    return fs;
}

FontStyles GetFontStyles(int weight, int width, int slant)
{
    FontStyles fs(static_cast<FontStyles::Weight>(weight),
                  static_cast<FontStyles::Width>(width),
                  static_cast<FontStyles::Slant>(slant));
    return fs;
}

/**
 * @tc.name: FontStyles1
 * @tc.desc: Verify the FontStyles
 * @tc.type:FUNC
 * @tc.require: issueI6V6I4
 */
HWTEST_F(FontStylesTest, FontStyles1, TestSize.Level1)
{
    constexpr auto excption = ExceptionType::INVALID_ARGUMENT;
    constexpr auto weight = 0;
    constexpr auto style = 0;

    // FontWeight: -100 < -1 < 0 <= 3 <= 8 < 9 < 100
    ASSERT_EXCEPTION(excption, GetFontStyles(-100, style));
    ASSERT_EXCEPTION(excption, GetFontStyles(-1, style));
    EXPECT_NO_THROW(GetFontStyles(0, style));
    EXPECT_NO_THROW(GetFontStyles(3, style));
    EXPECT_NO_THROW(GetFontStyles(8, style));
    ASSERT_EXCEPTION(excption, GetFontStyles(9, style));
    ASSERT_EXCEPTION(excption, GetFontStyles(100, style));

    // FontStyle: -100 < -1 < 0 <= 1 < 2 < 100
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, -100));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, -1));
    EXPECT_NO_THROW(GetFontStyles(weight, 0));
    EXPECT_NO_THROW(GetFontStyles(weight, 1));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, 2));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, 100));
}

/**
 * @tc.name: FontStyles2
 * @tc.desc: Verify the FontStyles
 * @tc.type:FUNC
 * @tc.require: issueI6V6I4
 */
HWTEST_F(FontStylesTest, FontStyles2, TestSize.Level1)
{
    constexpr auto excption = ExceptionType::INVALID_ARGUMENT;
    constexpr auto weight = 0;
    constexpr auto width = 0;
    constexpr auto slant = 0;

    // Weight: -100 < -1 < 0 <= 4 <= 10 < 11 < 100
    ASSERT_EXCEPTION(excption, GetFontStyles(-100, width, slant));
    ASSERT_EXCEPTION(excption, GetFontStyles(-1, width, slant));
    EXPECT_NO_THROW(GetFontStyles(0, width, slant));
    EXPECT_NO_THROW(GetFontStyles(4, width, slant));
    EXPECT_NO_THROW(GetFontStyles(10, width, slant));
    ASSERT_EXCEPTION(excption, GetFontStyles(11, width, slant));
    ASSERT_EXCEPTION(excption, GetFontStyles(100, width, slant));

    // Width: -100 < -1 < 0 <= 4 <= 8 < 9 < 100
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, -100, slant));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, -1, slant));
    EXPECT_NO_THROW(GetFontStyles(weight, 0, slant));
    EXPECT_NO_THROW(GetFontStyles(weight, 4, slant));
    EXPECT_NO_THROW(GetFontStyles(weight, 8, slant));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, 9, slant));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, 100, slant));

    // Slant: -100 < -1 < 0 <= 1 <= 2 < 3 < 100
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, width, -100));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, width, -1));
    EXPECT_NO_THROW(GetFontStyles(weight, width, 0));
    EXPECT_NO_THROW(GetFontStyles(weight, width, 1));
    EXPECT_NO_THROW(GetFontStyles(weight, width, 2));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, width, 3));
    ASSERT_EXCEPTION(excption, GetFontStyles(weight, width, 100));
}

/**
 * @tc.name: ToTexgineFontStyle
 * @tc.desc: Verify the ToTexgineFontStyle
 * @tc.type:FUNC
 * @tc.require: issueI6V6I4
 */
HWTEST_F(FontStylesTest, ToTexgineFontStyle, TestSize.Level1)
{
    // (0, 0) is (FontWeight, FontStyle)
    EXPECT_EQ(GetFontStyles(0, 0).ToTexgineFontStyle().GetFontStyle()->GetWeight(), 100);
    EXPECT_EQ(GetFontStyles(0, 0).ToTexgineFontStyle().GetFontStyle()->GetSlant(), 0);

    // (1, 0, 0) is (FontStyles::Weight, FontStyles::Width, FontStyles::Slant)
    EXPECT_EQ(GetFontStyles(1, 0, 0).ToTexgineFontStyle().GetFontStyle()->GetWeight(), 100);
    EXPECT_EQ(GetFontStyles(1, 0, 0).ToTexgineFontStyle().GetFontStyle()->GetWidth(), 1);
    EXPECT_EQ(GetFontStyles(1, 0, 0).ToTexgineFontStyle().GetFontStyle()->GetSlant(), 0);
}

/**
 * @tc.name: OperatorEqual
 * @tc.desc: Verify the OperatorEqual
 * @tc.type:FUNC
 * @tc.require: issueI6V6I4
 */
HWTEST_F(FontStylesTest, OperatorEqual, TestSize.Level1)
{
    auto fontStyles = GetFontStyles(1, 1, 1);
    EXPECT_NE(GetFontStyles(0, 1, 1), fontStyles);
    EXPECT_NE(GetFontStyles(1, 0, 1), fontStyles);
    EXPECT_NE(GetFontStyles(1, 1, 0), fontStyles);
    EXPECT_EQ(GetFontStyles(1, 1, 1), fontStyles);
}

/**
 * @tc.name: OperatorLessThan
 * @tc.desc: Verify the OperatorLessThan
 * @tc.type:FUNC
 * @tc.require: issueI6V6I4
 */
HWTEST_F(FontStylesTest, OperatorLessThan, TestSize.Level1)
{
    auto fontStyles = GetFontStyles(1, 1, 1);
    EXPECT_TRUE(GetFontStyles(0, 1, 1) < fontStyles);
    EXPECT_TRUE(GetFontStyles(1, 0, 1) < fontStyles);
    EXPECT_TRUE(GetFontStyles(1, 1, 0) < fontStyles);
    EXPECT_FALSE(GetFontStyles(1, 1, 1) < fontStyles);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
