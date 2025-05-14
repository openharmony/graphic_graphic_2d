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

#include "drawing_font_collection.h"
#include "drawing_text_typography.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class NdkAddTextTest : public testing::Test {};

static int32_t GetUnresolvedGlyphCount(const void* text, size_t length, OH_Drawing_TextEncoding encode)
{
    auto typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typoStyle, nullptr);
    auto handler = OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_GetFontCollectionGlobalInstance());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerAddEncodedText(handler, text, length, encode);
    auto typography = OH_Drawing_CreateTypography(handler);
    EXPECT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, 100.0f);
    int32_t unresolved = OH_Drawing_TypographyGetUnresolvedGlyphsCount(typography);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    return unresolved;
}
/**
 * @tc.name: AddValidUTF8TextTest
 * @tc.desc: Test valid UTF-8 text input scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddValidUTF8TextTest, TestSize.Level1)
{
    // Basic ASCII characters
    const char ascii[] = u8"Hello World";
    EXPECT_EQ(GetUnresolvedGlyphCount(ascii, sizeof(ascii), TEXT_ENCODING_UTF8), 0);

    // Non-ASCII characters (Chinese)
    const char chinese[] = u8"你好世界";
    EXPECT_EQ(GetUnresolvedGlyphCount(chinese, sizeof(chinese), TEXT_ENCODING_UTF8), 0);

    // Special characters
    const char tabsNewline[] = u8"Tabs\tNewline\n";
    EXPECT_EQ(GetUnresolvedGlyphCount(tabsNewline, sizeof(tabsNewline), TEXT_ENCODING_UTF8), 0);

    // 4-byte emoji characters
    const char emoji[] = u8"😊🌍🎉";
    EXPECT_EQ(GetUnresolvedGlyphCount(emoji, sizeof(emoji), TEXT_ENCODING_UTF8), 0);

    // Mixed case and symbols
    const char mixedCaseSymbols[] = u8"Aa1!Ωπ";
    EXPECT_EQ(GetUnresolvedGlyphCount(mixedCaseSymbols, sizeof(mixedCaseSymbols), TEXT_ENCODING_UTF8), 0);
}

/**
 * @tc.name: AddInvalidUTF8TextTest
 * @tc.desc: Test invalid UTF-8 text input scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddInvalidUTF8TextTest, TestSize.Level1)
{
    // Invalid continuation bytes
    const char invalid1[] = "abc\x80\xff";
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid1, sizeof(invalid1), TEXT_ENCODING_UTF8), 0);

    // Truncated multi-byte sequence
    const char invalid2[] = u8"正常\xE6\x97继续";
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid2, sizeof(invalid2), TEXT_ENCODING_UTF8), 0);

    // Overlong encoding
    const char invalid3[] = "\xF0\x82\x82\xAC"; // Overlong € symbol
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid3, sizeof(invalid3), TEXT_ENCODING_UTF8), 0);
}

/**
 * @tc.name: AddValidUTF16TextTest
 * @tc.desc: Test valid UTF-16 text input scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddValidUTF16TextTest, TestSize.Level1)
{
    // Basic BMP characters
    const char16_t ascii16[] = u"Hello World";
    EXPECT_EQ(GetUnresolvedGlyphCount(ascii16, sizeof(ascii16), TEXT_ENCODING_UTF16), 0);

    // Non-BMP characters (emoji)
    const char16_t emoji16[] = u"😀🌈";
    EXPECT_EQ(GetUnresolvedGlyphCount(emoji16, sizeof(emoji16), TEXT_ENCODING_UTF16), 0); // 😀🌈

    // RTL text with control characters
    const char16_t rtlText16[] = u"\x202E右到左文本";
    EXPECT_EQ(GetUnresolvedGlyphCount(rtlText16, sizeof(rtlText16), TEXT_ENCODING_UTF16), 0);

    // Combining characters
    const char16_t combiningChars16[] = u"A\u0300\u0301";
    EXPECT_EQ(GetUnresolvedGlyphCount(combiningChars16, sizeof(combiningChars16), TEXT_ENCODING_UTF16), 0); // À́
}

/**
 * @tc.name: AddInvalidUTF16TextTest
 * @tc.desc: Test invalid UTF-16 text input scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddInvalidUTF16TextTest, TestSize.Level1)
{
    // Unpaired high surrogate
    const char16_t invalid1[] = u"\xD800\x0041";
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid1, sizeof(invalid1), TEXT_ENCODING_UTF16), 0);

    // Unpaired low surrogate
    const char16_t invalid2[] = u"\xDC00";
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid2, sizeof(invalid2), TEXT_ENCODING_UTF16), 0);

    // Swapped surrogate pair
    const char16_t invalid3[] = u"\xDC00\xD800";
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid3, sizeof(invalid3), TEXT_ENCODING_UTF16), 0);
}

/**
 * @tc.name: AddValidUTF32TextTest
 * @tc.desc: Test valid UTF-32 text input scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddValidUTF32TextTest, TestSize.Level1)
{
    // Basic ASCII range
    const char32_t ascii32[] = U"ASCII Text!";
    EXPECT_EQ(GetUnresolvedGlyphCount(ascii32, sizeof(ascii32), TEXT_ENCODING_UTF32), 0);

    // Non-BMP characters
    const char32_t emoji32[] = { 0x1F600, 0x1F308, 0 }; // 😀🌈
    EXPECT_EQ(GetUnresolvedGlyphCount(emoji32, sizeof(emoji32), TEXT_ENCODING_UTF32), 0);

    // Maximum valid code point
    const char32_t maxUnicode32[] = { 0x10FFFF };
    EXPECT_EQ(GetUnresolvedGlyphCount(maxUnicode32, sizeof(maxUnicode32), TEXT_ENCODING_UTF32), 1);
}
/**
 * @tc.name: AddInvalidUTF32TextTest
 * @tc.desc: Test invalid UTF-32 text input scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddInvalidUTF32TextTest, TestSize.Level1)
{
    // Code point beyond U+10FFFF
    const char32_t invalid1[] = { 0x110000 };
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid1, sizeof(invalid1), TEXT_ENCODING_UTF32), 0);

    // Surrogate code points
    const char32_t invalid2[] = { 0xD800, 0xDFFF };
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid2, sizeof(invalid2), TEXT_ENCODING_UTF32), 0);

    // Negative value
    const char32_t invalid3[] = { -100 };
    EXPECT_EQ(GetUnresolvedGlyphCount(invalid3, sizeof(invalid3), TEXT_ENCODING_UTF32), 0);
}

/**
 * @tc.name: AddTextBoundaryTest
 * @tc.desc: Test boundary cases and edge conditions
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddTextBoundaryTest, TestSize.Level1)
{
    // Empty input with different encodings
    EXPECT_EQ(GetUnresolvedGlyphCount("", 0, TEXT_ENCODING_UTF8), -1);
    EXPECT_EQ(GetUnresolvedGlyphCount(u"", 0, TEXT_ENCODING_UTF16), -1);
    EXPECT_EQ(GetUnresolvedGlyphCount(U"", 0, TEXT_ENCODING_UTF32), -1);

    // Null pointer with zero length
    EXPECT_EQ(GetUnresolvedGlyphCount(nullptr, 0, TEXT_ENCODING_UTF8), -1);

    // Maximum code point boundary
    const char32_t boundary1[] = { 0x10FFFF, 0x0000 };
    EXPECT_EQ(GetUnresolvedGlyphCount(boundary1, sizeof(boundary1), TEXT_ENCODING_UTF32), 1);

    // Minimum code point boundary
    const char32_t boundary2[] = { 0x0000 };
    EXPECT_EQ(GetUnresolvedGlyphCount(boundary2, sizeof(boundary2), TEXT_ENCODING_UTF32), 0);
}

/**
 * @tc.name: AddTextStressTest
 * @tc.desc: Test stress and performance scenarios
 * @tc.type: PERF
 */
HWTEST_F(NdkAddTextTest, AddTextStressTest, TestSize.Level1)
{
    // Large UTF-8 text
    std::vector<char> bigUtf8(10000, 'A');
    EXPECT_EQ(GetUnresolvedGlyphCount(bigUtf8.data(), bigUtf8.size(), TEXT_ENCODING_UTF8), 0);

    // Large UTF-16 text with surrogate pairs
    std::vector<char16_t> bigUtf16;
    for (int i = 0; i < 10000; ++i) {
        bigUtf16.push_back(0xD83D);
        bigUtf16.push_back(0xDE00);
    }
    EXPECT_EQ(GetUnresolvedGlyphCount(bigUtf16.data(), bigUtf16.size(), TEXT_ENCODING_UTF16), 0);

    // Mixed valid/invalid UTF-32 data
    std::vector<char32_t> mixed_utf32(10000, 0x1F600);
    mixed_utf32[5000] = 0x110000; // Insert invalid code point
    EXPECT_EQ(GetUnresolvedGlyphCount(mixed_utf32.data(), mixed_utf32.size(), TEXT_ENCODING_UTF32), 0);
}

/**
 * @tc.name: AddTextInvalidEncodingTest
 * @tc.desc: Test invalid encoding type scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkAddTextTest, AddTextInvalidEncodingTest, TestSize.Level1)
{
    // Invalid encoding type
    const char testStr[] = "test";
    EXPECT_EQ(GetUnresolvedGlyphCount(testStr, sizeof(testStr), TEXT_ENCODING_GLYPH_ID), -1);

    // Mismatched encoding and data
    const char16_t utf16Data[] = u"test";
    EXPECT_EQ(GetUnresolvedGlyphCount(utf16Data, sizeof(utf16Data), TEXT_ENCODING_UTF8), 0);

    // Null pointer with non-zero length
    EXPECT_EQ(GetUnresolvedGlyphCount(nullptr, 5, TEXT_ENCODING_UTF16), -1);
}
} // namespace Rosen
} // namespace OHOS