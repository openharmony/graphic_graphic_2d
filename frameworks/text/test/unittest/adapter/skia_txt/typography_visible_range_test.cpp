/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "font_collection.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "paragraph.h"
#include "src/ParagraphImpl.h"
#include "typography.h"
#include "typography_create.h"
#include "txt/text_bundle_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr double DEFAULT_FONT_SIZE = 50.0;

class OhDrawingTypographyTest : public testing::Test {
};

// Test helper function to create and layout Typography with common configuration
static std::unique_ptr<OHOS::Rosen::Typography> CreateAndLayoutTypography(
    const std::u16string& text,
    double maxWidth,
    const std::function<void(TypographyStyle&)>& styleSetup = nullptr,
    const std::function<void(TextStyle&)>& textStyleSetup = nullptr)
{
    TypographyStyle typographyStyle;
    if (styleSetup != nullptr) {
        styleSetup(typographyStyle);
    }

    std::shared_ptr<FontCollection> fontCollection =
        FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<TypographyCreate> typographyCreate =
        TypographyCreate::Create(typographyStyle, fontCollection);

    TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    if (textStyleSetup != nullptr) {
        textStyleSetup(textStyle);
    }
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);

    std::unique_ptr<Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    return typography;
}

/*
 * @tc.name: TypographyGetVisibleTextRangesNoLayoutTest
 * @tc.desc: test GetVisibleTextRanges without layout
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesNoLayoutTest, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    std::u16string text = u"Hello World Test Text";
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 0);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with no ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography =
        CreateAndLayoutTypography(u"Hello World", 10000.0);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 11);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesTailEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with tail ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesTailEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"This is a very long text that will be truncated", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 10);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMiddleEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with middle ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMiddleEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Start middle end", 200.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MIDDLE;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 2);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 3);
    EXPECT_EQ(ranges[1].start, 14);
    EXPECT_EQ(ranges[1].end, 16);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesEmptyTextTest
 * @tc.desc: test GetVisibleTextRanges with empty text
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesEmptyTextTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography =
        CreateAndLayoutTypography(u"", 300.0);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 0);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesHeadEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with head ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesHeadEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"This is a very long text that will be truncated at the head", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::HEAD;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 49);
    EXPECT_EQ(ranges[0].end, 59);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMultiLineTailEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with multi-line tail ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMultiLineTailEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"This is a very long text that will be truncated with multiple lines of content", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 2;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 25);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMultiLineHeadEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with multi-line head ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMultiLineHeadEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"This is a very long text that will be truncated with multiple lines", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 2;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MULTILINE_HEAD;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 2);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 15);
    EXPECT_EQ(ranges[1].start, 55);
    EXPECT_EQ(ranges[1].end, 67);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesSingleCharTest
 * @tc.desc: test GetVisibleTextRanges with single character
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesSingleCharTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography =
        CreateAndLayoutTypography(u"A", 100.0);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 1);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesSmallWidthTest
 * @tc.desc: test GetVisibleTextRanges with very small width (extreme ellipsis)
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesSmallWidthTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Very long text for testing", 10.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 0);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesChineseTextTest
 * @tc.desc: test GetVisibleTextRanges with Chinese text and truncation (no ellipsis mode set)
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesChineseTextTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"这是一段很长的中文文本用于测试省略号功能", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 6);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMixedTextTest
 * @tc.desc: test GetVisibleTextRanges with mixed Chinese and English text and truncation (no ellipsis mode set)
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMixedTextTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"中文MixedText混合文本", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 2);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMaxLinesZeroTest
 * @tc.desc: test GetVisibleTextRanges with maxLines set to 0
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMaxLinesZeroTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Test text", 300.0,
        [](TypographyStyle& style) { style.maxLines = 0; });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 0);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesRTLNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with RTL text direction and no ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesRTLNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"مرحبا بالعالم", 10000.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 13);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesRTLTailEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with RTL text direction and tail ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesRTLTailEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"هذا نص طويل سيتم قطعه", 300.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 7);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesRTLHeadEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with RTL text direction and head ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesRTLHeadEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"هذا نص طويل سيتم قطع مقدمة", 300.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::HEAD;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 16);
    EXPECT_EQ(ranges[0].end, 26);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesRTLMiddleEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with RTL text direction and middle ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesRTLMiddleEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"البداية والنهاية", 200.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MIDDLE;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 2);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 4);
    EXPECT_EQ(ranges[1].start, 12);
    EXPECT_EQ(ranges[1].end, 16);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesRTLMultiLineTailEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with RTL text direction and multi-line tail ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesRTLMultiLineTailEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"هذا نص طويل جدا سيتم قطعه مع عدة أسطر من المحتوى", 300.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
            style.maxLines = 2;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 21);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesRTLMultiLineHeadEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with RTL text direction and multi-line head ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesRTLMultiLineHeadEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"هذا نص طويل سيتم قطع مقدمة مع عدة أسطر", 300.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
            style.maxLines = 2;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MULTILINE_HEAD;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 2);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 12);
    EXPECT_EQ(ranges[1].start, 28);
    EXPECT_EQ(ranges[1].end, 38);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesHebrewRTLTest
 * @tc.desc: test GetVisibleTextRanges with Hebrew RTL text and truncation (no ellipsis mode set)
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesHebrewRTLTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"שלום עולם זהו טקסט ארוך", 300.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 13);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMixedLTRRTLTest
 * @tc.desc: test GetVisibleTextRanges with mixed LTR and RTL text
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMixedLTRRTLTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Hello מרון Text", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 10);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesEmojiNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with emoji and no ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesEmojiNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Hello 😊 World 🌍", 10000.0);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 17);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesEmojiTailEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with emoji and tail ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesEmojiTailEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Hello 😊 World 🌍 This is a long text with emoji", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 9);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesEmojiMultiCharTest
 * @tc.desc: test GetVisibleTextRanges with multi-char emoji sequences
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesEmojiMultiCharTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"👨‍👩‍👧‍👦 Family emoji test", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 18);
}


/*
 * @tc.name: TypographyGetVisibleTextRangesEmojiMultiLineTest
 * @tc.desc: test GetVisibleTextRanges with emoji in multi-line text
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesEmojiMultiLineTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"First line with 😊 emoji\nSecond line with 🌍 emoji and more text", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 2;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 18);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesEmojiRTLTest
 * @tc.desc: test GetVisibleTextRanges with emoji and RTL text
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesEmojiRTLTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"مرحبا 😊 عالم", 300.0,
        [](TypographyStyle& style) {
            style.textDirection = TextDirection::RTL;
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 13);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMultiLineNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with multiple lines, no ellipsis, no maxLines
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMultiLineNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"First line\nSecond line\nThird line\nFourth line\nFifth line", 500.0);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 56);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMultiLineWithMaxLinesTest
 * @tc.desc: test GetVisibleTextRanges with multiple lines, no ellipsis, with maxLines=3
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMultiLineWithMaxLinesTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"First line\nSecond line\nThird line\nFourth line\nFifth line", 500.0,
        [](TypographyStyle& style) {
            style.maxLines = 3;
        });

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 3);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 33);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesChineseMultiLineNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with Chinese multi-line text, no ellipsis, no maxLines
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesChineseMultiLineNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"第一行文本\n第二行文本\n第三行文本\n第四行文本", 500.0);

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 4);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 23);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMaxLinesDefaultTest
 * @tc.desc: test GetVisibleTextRanges with maxLines set to default value (1e9)
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMaxLinesDefaultTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"First line\nSecond line\nThird line\nFourth line\nFifth line", 500.0,
        [](TypographyStyle& style) {
            // Set maxLines to the default value (1e9 = unlimited)
            style.maxLines = static_cast<size_t>(1e9);
        });

    // Should return all 5 lines since maxLines = 1e9 (unlimited)
    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 56);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMultilineMiddleEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with multi-line middle ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMultilineMiddleEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Line1\nLine2\nLine3\nLine4\nLine5\nLine6\nLine7\nLine8", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 3;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MULTILINE_MIDDLE;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 17);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesNumberTextWithTailEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with number text and tail ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesNumberTextWithTailEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"123456789012345678901234567890", 200.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 5);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesPunctuationTextEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with punctuation/symbol text and ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesPunctuationTextEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"!@#$%^&*()_+-=[]{}|;':\",./<>?", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 4);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesSingleCharWithEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with single character and ellipsis config
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesSingleCharWithEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"A", 10.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MIDDLE;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 1);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMixedLangWithTailEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with mixed language text and tail ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMixedLangWithTailEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Hello世界World测试Mixed混合Language语言", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 7);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMixedLangWithMiddleEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with mixed language text and middle ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMixedLangWithMiddleEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Hello世界World测试Mixed混合Language语言", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MIDDLE;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 2);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 5);
    EXPECT_EQ(ranges[1].start, 28);
    EXPECT_EQ(ranges[1].end, 31);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesEmptyTextWithEllipsisConfigTest
 * @tc.desc: test GetVisibleTextRanges with empty text and ellipsis configuration
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesEmptyTextWithEllipsisConfigTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::TAIL;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 0);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesExtremeWidthMiddleEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with extreme small width and middle ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesExtremeWidthMiddleEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"LongMiddleEllipsisText", 5.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MIDDLE;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 0);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesSpacesTextEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with spaces text and ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesSpacesTextEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Word1     Word2     Word3     Word4", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 9);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesJapaneseTextEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with Japanese text and ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesJapaneseTextEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"これは省略記号をテストするための非常に長い日本語のテキストです", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 6);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMixedLanguageRTLWithEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with mixed LTR/RTL languages and ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMixedLanguageRTLWithEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Hello مرحبا World نص Text", 300.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 11);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMaxLinesOneNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with maxLines=1 and no ellipsis triggered
 *           This tests the condition path: maxLines > 0 && maxLines < UNLIMITED_MAX_LINES &&
 *           ellipsisRange.start == INFINITE_RANGE_INDEX && visibleLineCount > 0
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMaxLinesOneNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Short text", 10000.0,
        [](TypographyStyle& style) { style.maxLines = 1; });

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 1);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 10);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMaxLinesTwoNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with maxLines=2 and no ellipsis triggered
 *           This tests the condition path with maxLines > 1
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMaxLinesTwoNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Line1\nLine2\nLine3", 10000.0,
        [](TypographyStyle& style) { style.maxLines = 2; });

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 2);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 11);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMaxLinesThreeNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with maxLines=3 and no ellipsis triggered
 *           This tests the condition path with more lines
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMaxLinesThreeNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"First line\nSecond line\nThird line\nFourth line", 10000.0,
        [](TypographyStyle& style) { style.maxLines = 3; });

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 3);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 33);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMultiLineAllVisibleNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with multi-line text, default maxLines (unlimited), and no ellipsis
 *           This tests the condition path when all lines are visible
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMultiLineAllVisibleNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Line1\nLine2\nLine3\nLine4\nLine5", 10000.0);

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 5);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 29);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesChineseMaxLinesTwoNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with Chinese text, maxLines=2, and no ellipsis
 *           This tests the condition path with non-ASCII text
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesChineseMaxLinesTwoNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"第一行文本\n第二行文本\n第三行文本", 10000.0,
        [](TypographyStyle& style) { style.maxLines = 2; });

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 2);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 11);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMaxLinesFiveNoEllipsisTest
 * @tc.desc: test GetVisibleTextRanges with maxLines=5 and no ellipsis triggered
 *           This tests the condition path with larger maxLines value
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMaxLinesFiveNoEllipsisTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"Line1\nLine2\nLine3\nLine4\nLine5\nLine6\nLine7", 10000.0,
        [](TypographyStyle& style) { style.maxLines = 5; });

    int lineCount = typography->GetLineCount();
    EXPECT_EQ(lineCount, 5);

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 29);
}

/*
 * @tc.name: TypographyGetVisibleTextRangesMiddleEllipsisExtremeWidthTest
 * @tc.desc: test GetVisibleTextRanges with middle ellipsis and extreme small width
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyTest, TypographyGetVisibleTextRangesMiddleEllipsisExtremeWidthTest, TestSize.Level0)
{
    std::unique_ptr<Typography> typography = CreateAndLayoutTypography(
        u"StartMiddleEndText", 1.0,
        [](TypographyStyle& style) {
            style.maxLines = 1;
            style.ellipsis = TypographyStyle::ELLIPSIS;
            style.ellipsisModal = EllipsisModal::MIDDLE;
        });

    std::vector<TextRange> ranges = typography->GetVisibleTextRanges();
    // With extreme small width in middle ellipsis mode
    EXPECT_EQ(ranges.size(), 1);
    EXPECT_EQ(ranges[0].start, 0);
    EXPECT_EQ(ranges[0].end, 0);
}
} // namespace Rosen
} // namespace OHOS
