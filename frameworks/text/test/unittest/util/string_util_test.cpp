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

#include "common_utils/string_util.h"

#include "utils/string_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class StringConvertTest : public testing::Test {};

/**
 * @tc.name: Str8ToStr16ByIcuTest
 * @tc.desc: test Str8ToStr16ByIcu
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, Str8ToStr16ByIcuTest, TestSize.Level1)
{
    // Test valid UTF-8 string
    EXPECT_EQ(Str8ToStr16ByIcu("Hello"), u"Hello");

    // Test empty string
    EXPECT_EQ(Str8ToStr16ByIcu(""), u"");

    // Test invalid UTF-8 string
    EXPECT_EQ(Str8ToStr16ByIcu("\x80\x81"), u"\xfffd\xfffd");

    // Test UTF-8 string containing non-ASCII characters
    EXPECT_EQ(Str8ToStr16ByIcu("你好"), u"你好");

    // Test UTF-8 string containing special characters (such as newline, tab, etc.)
    EXPECT_EQ(Str8ToStr16ByIcu("Hello\nWorld\t!"), u"Hello\nWorld\t!");

    // Test 3-byte characters (Japanese)
    EXPECT_EQ(Str8ToStr16ByIcu(u8"こんにちは"), u"\u3053\u3093\u306B\u3061\u306F");

    // Test 4-byte characters (emoji)
    EXPECT_EQ(Str8ToStr16ByIcu(u8"😊"), u"\xD83D\xDE0A");

    // Test invalid UTF-8 sequence (incomplete sequence)
    EXPECT_EQ(Str8ToStr16ByIcu("\xE6\x97"), u"\xFFFD");

    // Test illegal byte values
    EXPECT_EQ(Str8ToStr16ByIcu("abc\xFF\xFE"), u"abc\xFFFD\xFFFD");

    // Test mixed valid and invalid sequences
    EXPECT_EQ(Str8ToStr16ByIcu(u8"正常\xE6\x97继续"), u"正常\xFFFD继续");
}

/**
 * @tc.name: Str32ToStr16ByIcuTest
 * @tc.desc: test Str32ToStr16ByIcuTest
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, Str32ToStr16ByIcuTest, TestSize.Level1)
{
    // Test valid UTF-32 string
    std::vector<int32_t> valid = { 0x0048, 0x0045, 0x004C, 0x004C, 0x004F }; // UTF-32 representation of "HELLO"
    std::u16string expected = u"HELLO";
    EXPECT_EQ(Str32ToStr16ByIcu(valid.data(), valid.size()), expected);

    // Test empty array
    std::vector<int32_t> empty {};
    EXPECT_EQ(Str32ToStr16ByIcu(empty.data(), empty.size()), u"");

    // Test invalid UTF-32 value (out of Unicode range)
    std::vector<int32_t> invalid = { 0x110000 }; // Out of Unicode range
    EXPECT_EQ(Str32ToStr16ByIcu(invalid.data(), invalid.size()), u"\xfffd");

    // Test NULL pointer input
    EXPECT_EQ(Str32ToStr16ByIcu(nullptr, 5), u"");

    // Test zero-length input for UTF-32 array
    int* nul = nullptr; // No actual input needed, length is 0
    EXPECT_EQ(Str32ToStr16ByIcu(nul, 0), u"");

    // Test UTF-32 string containing multiple characters
    std::vector<int32_t> multiple = { 0x0048, 0x0065, 0x006C, 0x006C, 0x006F, 0x0020, 0x0057, 0x006F, 0x0072, 0x006C,
        0x0064 }; // "Hello World"
    EXPECT_EQ(Str32ToStr16ByIcu(multiple.data(), multiple.size()), u"Hello World");

    // Test UTF-32 string containing non-BMP characters
    std::vector<int32_t> emoji = { 0x1F600 }; // Emoji "😀"
    EXPECT_EQ(Str32ToStr16ByIcu(emoji.data(), emoji.size()), u"\xd83d\xde00");

    // Test long string input
    std::vector<int32_t> longStr = { 0x0048, 0x0065, 0x006C, 0x006C, 0x006F, 0x0020, 0x0057, 0x006F, 0x0072, 0x006C,
        0x0064, 0x0021, 0x0020, 0x1F600 }; // "Hello World! 😀"
    EXPECT_EQ(Str32ToStr16ByIcu(longStr.data(), longStr.size()), u"Hello World! 😀");

    // Test characters with different cases
    std::vector<int32_t> ascii = { 0x0041, 0x005A, 0x0061, 0x007A }; // "AZaz"
    EXPECT_EQ(Str32ToStr16ByIcu(ascii.data(), ascii.size()), u"AZaz");

    // Test UTF-32 array containing mixed characters (such as letters + symbols)
    std::vector<int32_t> mixed = { 0x0048, 0x0045, 0x0021 }; // "HE!"
    EXPECT_EQ(Str32ToStr16ByIcu(mixed.data(), mixed.size()), u"HE!");

    // Test invalid code point (out of Unicode range)
    std::vector<int32_t> invalid1 { 0x110000 };
    EXPECT_EQ(Str32ToStr16ByIcu(invalid1.data(), invalid1.size()), u"\xFFFD");

    // Test illegal surrogate pair code point
    std::vector<int32_t> invalid2 { 0xD800 };
    EXPECT_EQ(Str32ToStr16ByIcu(invalid2.data(), invalid2.size()), u"\xFFFD");

    // Test mixed valid and invalid code points
    std::vector<int32_t> mixed2 { 0x41, 0x10FFFF, 0x42 };
    EXPECT_EQ(Str32ToStr16ByIcu(mixed2.data(), mixed2.size()), u"\u0041\xDBFF\xDFFF\u0042");

    // Test negative value
    std::vector<int32_t> negative { -100 };
    EXPECT_EQ(Str32ToStr16ByIcu(negative.data(), negative.size()), u"\xFFFD");
}

/**
 * @tc.name: BoundaryCases
 * @tc.desc: test BoundaryCases
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, BoundaryCases, TestSize.Level1)
{
    // Test maximum valid Unicode code point
    std::vector<int32_t> maxValid { 0x10FFFF };
    EXPECT_EQ(Str32ToStr16ByIcu(maxValid.data(), maxValid.size()), u"\xDBFF\xDFFF");

    // Test minimum surrogate code point
    std::vector<int32_t> minSurrogate { 0xD800 };
    EXPECT_EQ(Str32ToStr16ByIcu(minSurrogate.data(), minSurrogate.size()), u"\xFFFD");

    // Test maximum surrogate code point
    std::vector<int32_t> maxSurrogate { 0xDFFF };
    EXPECT_EQ(Str32ToStr16ByIcu(maxSurrogate.data(), maxSurrogate.size()), u"\xFFFD");

    // Test minimum code point
    std::vector<int32_t> zero { 0x0000 };
    std::u16string result = u"\x00";
    result.resize(1);
    EXPECT_EQ(Str32ToStr16ByIcu(zero.data(), zero.size()), result);
}

/**
 * @tc.name: StressTest
 * @tc.desc: use huge input to test stress
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, StressTest, TestSize.Level1)
{
    // Test long string
    std::string longStr(4096, 'A');
    auto result = Str8ToStr16ByIcu(longStr);
    EXPECT_EQ(result.size(), 4096);
    EXPECT_EQ(result[4095], u'A');

    // Test large character set
    std::vector<int32_t> bigChars(65536, 0x1F601); // 😁
    std::u16string expected;
    for (int i = 0; i < 65536; i++) {
        // 0xD83D + 0xDE01 = 😁
        expected.push_back(0xD83D);
        expected.push_back(0xDE01);
    }
    EXPECT_EQ(Str32ToStr16ByIcu(bigChars.data(), bigChars.size()), expected);
}
/**
 * @tc.name: SingleHighSurrogate
 * @tc.desc: Test handling of a single high surrogate
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, SingleHighSurrogate, TestSize.Level1)
{
    std::u16string input = u"\xD800"; // Single high surrogate
    std::u16string expected = u"\xFFFD";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}

/**
 * @tc.name: SingleLowSurrogate
 * @tc.desc: Test handling of a single low surrogate
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, SingleLowSurrogate, TestSize.Level1)
{
    std::u16string input = u"\xDC00"; // Single low surrogate
    std::u16string expected = u"\xFFFD";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}

/**
 * @tc.name: ValidSurrogatePair
 * @tc.desc: Test handling of a valid surrogate pair
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, ValidSurrogatePair, TestSize.Level1)
{
    std::u16string input = u"\xD800\xDC00"; // Valid surrogate pair
    std::u16string expected = u"\xD800\xDC00";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}

/**
 * @tc.name: HighFollowedByNonSurrogate
 * @tc.desc: Test handling of a high surrogate followed by a non-surrogate character
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, HighFollowedByNonSurrogate, TestSize.Level1)
{
    std::u16string input = u"\xD800\u0041"; // High surrogate followed by non-surrogate character
    std::u16string expected = u"\xFFFD\u0041";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}

/**
 * @tc.name: InvertedSurrogates
 * @tc.desc: Test handling of inverted surrogates (low surrogate before high surrogate)
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, InvertedSurrogates, TestSize.Level1)
{
    std::u16string input = u"\xDC00\xD800"; // Low surrogate before high surrogate
    std::u16string expected = u"\xFFFD\xFFFD";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}

/**
 * @tc.name: MultipleInvalidSurrogates
 * @tc.desc: Test handling of multiple invalid surrogates
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, MultipleInvalidSurrogates, TestSize.Level1)
{
    std::u16string input = u"\xD800\xD801\xDC01"; // Two high surrogates, one low surrogate
    std::u16string expected = u"\xFFFD\xD801\xDC01";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}

/**
 * @tc.name: MixedValidAndInvalid
 * @tc.desc: Test handling of mixed valid and invalid surrogate pairs
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, MixedValidAndInvalid, TestSize.Level1)
{
    // Valid surrogate pair, invalid high surrogate, invalid low surrogate, normal character
    std::u16string input = u"\xD800\xDC00\xD801\x0041\xDC01";
    std::u16string expected = u"\xD800\xDC00\xFFFD\x0041\xFFFD";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}

/**
 * @tc.name: EmptyString
 * @tc.desc: Test handling of an empty string
 * @tc.type: FUNC
 */
HWTEST_F(StringConvertTest, EmptyString, TestSize.Level1)
{
    std::u16string input = u""; // Empty string
    std::u16string expected = u"";
    SPText::Utf16Utils::HandleIncompleteSurrogatePairs(input);
    EXPECT_EQ(input, expected);
}
} // namespace Rosen
} // namespace OHOS