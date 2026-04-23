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

#include "adapter/skia/txt/paragraph_style.h"
#include "gtest/gtest.h"
#include "rosen_text/text_config.h"
#include "rosen_text/typography_style.h"
#include "service/skia_txt/convert.h"
#include "service/skia_txt/font_collection.h"
#include "service/skia_txt/typography_create.h"
#include "service/skia_txt/typography.h"
#include "typography_types.h"

#include <codecvt>
#include <locale>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

// Glyph ID constants for test verification
constexpr uint16_t DOT_GLYPH_ID = 766;                         // Period/dot character '.'
constexpr uint16_t CHARACTER_SHUO_GLYPH_ZH = 23332;           // Character '说' (zh-Hans, en-Latn, en-US)
constexpr uint16_t CHARACTER_SHUO_GLYPH_JA_KO = 38513;         // Character '说' (ja-JP, ko-KR)
constexpr uint16_t LEFT_CORNER_BRACKET_GLYPH = 28998;         // Left corner bracket ('「')
constexpr uint16_t LEFT_DOUBLE_ANGLE_BRACKET_GLYPH = 29002;   // Left double angle bracket ('《')
constexpr uint16_t LEFT_WHITE_CORNER_BRACKET_GLYPH = 29014;   // Left white corner bracket ('『')
constexpr uint16_t FULLWIDTH_LEFT_PARENTHESIS_GLYPH = 29006;  // Fullwidth left parenthesis ('（')
constexpr uint16_t LEFT_SINGLE_QUOTATION_MARK_GLYPH = 13663;  // Left single quotation mark (''')
constexpr uint16_t LEFT_DOUBLE_QUOTATION_MARK_ALT_GLYPH = 14435; // Left double quotation mark alternative ('"')
constexpr uint16_t DIGIT_ZERO_GLYPH = 633;                    // Digit '0'

class OHDrawingUseLocaleForTextBreakTest : public testing::Test {
public:
    std::shared_ptr<FontCollection> fontCollection_;
    std::unique_ptr<TypographyCreate> typographyCreate_;
    std::unique_ptr<Typography> typography_;

    void SetUp() override
    {
        fontCollection_ = FontCollection::Create();
        ASSERT_NE(fontCollection_, nullptr);
    }

    void TearDown() override
    {
        typographyCreate_.reset();
        fontCollection_.reset();
    }

    std::u16string StrToU16Str(const std::string& str)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
    }

    // Helper function to get the first glyph ID of a specific line
    uint16_t GetFirstGlyphIdOfLine(size_t lineIndex, const std::unique_ptr<Typography>& typography)
    {
        if (!typography) {
            return 0;
        }

        auto textLines = typography->GetTextLines();
        if (lineIndex >= textLines.size()) {
            return 0;
        }

        auto& textLine = textLines[lineIndex];
        auto glyphRuns = textLine->GetGlyphRuns();
        if (glyphRuns.empty()) {
            return 0;
        }

        auto& firstRun = glyphRuns[0];
        auto glyphs = firstRun->GetGlyphs();
        if (glyphs.empty()) {
            return 0;
        }

        return glyphs[0];
    }

    void LayoutForLocaleTest(bool useLocaleForTextBreak, std::string str, double layoutWidth, std::string locale)
    {
        TypographyStyle typographyStyle;
        typographyStyle.locale = locale;
        typographyStyle.useLocaleForTextBreak = useLocaleForTextBreak;
        TextStyle textStyle;
        textStyle.locale = locale;
        // Test for fontSize 50
        textStyle.fontSize = 50;
        typographyStyle.SetTextStyle(textStyle);

        typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
        ASSERT_NE(typographyCreate_, nullptr);

        typographyCreate_->PushStyle(textStyle);
        std::u16string text = StrToU16Str(str);
        typographyCreate_->AppendText(text);
        typography_ = typographyCreate_->CreateTypography();
        ASSERT_NE(typography_, nullptr);

        typography_->Layout(layoutWidth);
    }

    void LayoutForLineBreakStrategyTest(std::string str, double layoutWidth, BreakStrategy strategy,
        WordBreakType breakType, std::string locale = "zh-Hans")
    {
        TypographyStyle typographyStyle;
        typographyStyle.locale = locale;
        typographyStyle.useLocaleForTextBreak = true;
        typographyStyle.breakStrategy = strategy;
        typographyStyle.wordBreakType = breakType;
        TextStyle textStyle;
        textStyle.locale = locale;
        // Test for fontSize 50
        textStyle.fontSize = 50;
        typographyStyle.SetTextStyle(textStyle);

        typographyCreate_ = TypographyCreate::Create(typographyStyle, fontCollection_);
        ASSERT_NE(typographyCreate_, nullptr);

        typographyCreate_->PushStyle(textStyle);
        std::u16string text = StrToU16Str(str);
        typographyCreate_->AppendText(text);
        typography_ = typographyCreate_->CreateTypography();
        ASSERT_NE(typography_, nullptr);

        typography_->Layout(layoutWidth);
    }
};

// ============ Basic Branch Tests ============

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest001
 * @tc.desc: Test TypographyStyle copy assignment with useLocaleForTextBreak = true and default value
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest001, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    EXPECT_EQ(typographyStyle.useLocaleForTextBreak, false);
    typographyStyle.useLocaleForTextBreak = true;

    TypographyStyle typographyStyleCopy = typographyStyle;
    EXPECT_EQ(typographyStyleCopy.useLocaleForTextBreak, true);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest002
 * @tc.desc: Test TypographyStyle copy constructor with useLocaleForTextBreak
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest002, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.useLocaleForTextBreak = true;

    TypographyStyle typographyStyleCopy(typographyStyle);
    EXPECT_EQ(typographyStyleCopy.useLocaleForTextBreak, true);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest003
 * @tc.desc: Test TypographyStyle operator== with same useLocaleForTextBreak values
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest003, TestSize.Level0)
{
    TypographyStyle style1;
    TypographyStyle style2;
    style1.useLocaleForTextBreak = true;
    style2.useLocaleForTextBreak = true;
    EXPECT_EQ(style1 == style2, true);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest004
 * @tc.desc: Test TypographyStyle operator== with different useLocaleForTextBreak values
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest004, TestSize.Level0)
{
    TypographyStyle style1;
    TypographyStyle style2;
    style1.useLocaleForTextBreak = true;
    style2.useLocaleForTextBreak = false;
    EXPECT_EQ(style1 == style2, false);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest005
 * @tc.desc: Test Convert TypographyStyle to ParagraphStyle with useLocaleForTextBreak = true
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest005, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    typographyStyle.useLocaleForTextBreak = true;

    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);
    EXPECT_EQ(paragraphStyle.useLocaleForTextBreak, true);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest006
 * @tc.desc: Test Convert with TypographyStyle=false, TextConfig=false
 * @tc.type: FUNC
 *
 * Scenario: Convert TypographyStyle with useLocaleForTextBreak=false and TextConfig disabled
 *           Target: ParagraphStyle.useLocaleForTextBreak should be false (false || false = false)
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest006, TestSize.Level0)
{
    // Given: Both TypographyStyle and TextConfig are disabled
    TextConfig::SetLocaleTextBreakEnabled(false);
    EXPECT_EQ(TextConfig::IsLocaleTextBreakEnabled(), false);
    TypographyStyle typographyStyle;
    typographyStyle.useLocaleForTextBreak = false;

    // When: Convert to ParagraphStyle
    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);

    // Then: Result should be false (false || false = false)
    EXPECT_EQ(paragraphStyle.useLocaleForTextBreak, false);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest007
 * @tc.desc: Test Convert with TypographyStyle=true, TextConfig=false
 * @tc.type: FUNC
 *
 * Scenario: Convert TypographyStyle with useLocaleForTextBreak=true and TextConfig disabled
 *           Target: ParagraphStyle.useLocaleForTextBreak should be true (true || false = true)
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest007, TestSize.Level0)
{
    // Given: TypographyStyle enabled, TextConfig disabled
    TextConfig::SetLocaleTextBreakEnabled(false);
    EXPECT_EQ(TextConfig::IsLocaleTextBreakEnabled(), false);
    TypographyStyle typographyStyle;
    typographyStyle.useLocaleForTextBreak = true;

    // When: Convert to ParagraphStyle
    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);

    // Then: Result should be true (true || false = true)
    EXPECT_EQ(paragraphStyle.useLocaleForTextBreak, true);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest008
 * @tc.desc: Test Convert with TypographyStyle=false, TextConfig=true
 * @tc.type: FUNC
 *
 * Scenario: Convert TypographyStyle with useLocaleForTextBreak=false and TextConfig enabled
 *           Target: ParagraphStyle.useLocaleForTextBreak should be true (false || true = true)
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest008, TestSize.Level0)
{
    // Given: TypographyStyle disabled, TextConfig enabled
    TextConfig::SetLocaleTextBreakEnabled(true);
    EXPECT_EQ(TextConfig::IsLocaleTextBreakEnabled(), true);
    TypographyStyle typographyStyle;
    typographyStyle.useLocaleForTextBreak = false;

    // When: Convert to ParagraphStyle
    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);

    // Then: Result should be true (false || true = true)
    EXPECT_EQ(paragraphStyle.useLocaleForTextBreak, true);

    // Cleanup: Reset TextConfig to default
    TextConfig::SetLocaleTextBreakEnabled(false);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakTest009
 * @tc.desc: Test Convert with TypographyStyle=true, TextConfig=true
 * @tc.type: FUNC
 *
 * Scenario: Convert TypographyStyle with useLocaleForTextBreak=true and TextConfig enabled
 *           Target: ParagraphStyle.useLocaleForTextBreak should be true (true || true = true)
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakTest009, TestSize.Level0)
{
    // Given: Both TypographyStyle and TextConfig are enabled
    TextConfig::SetLocaleTextBreakEnabled(true);
    EXPECT_EQ(TextConfig::IsLocaleTextBreakEnabled(), true);
    TypographyStyle typographyStyle;
    typographyStyle.useLocaleForTextBreak = true;

    // When: Convert to ParagraphStyle
    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);

    // Then: Result should be true (true || true = true)
    EXPECT_EQ(paragraphStyle.useLocaleForTextBreak, true);

    // Cleanup: Reset TextConfig to default
    TextConfig::SetLocaleTextBreakEnabled(false);
}

// ============ Email Breaking Function Tests ============

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakEmailTest001
 * @tc.desc: Test email address soft break with locale-aware text breaking disabled
 * @tc.type: FUNC
 *
 * Scenario: Test "sample@sample.com" with useLocaleForTextBreak = false
 *           Font size: 50, Max width: 420, locale: en-US
 *           Target: Verify email address doesn't break at '.' without locale-aware text breaking
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakEmailTest001, TestSize.Level0)
{
    LayoutForLocaleTest(false, "sample@sample.com", 420, "en-US");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line does NOT start with '.' glyph (no email-specific breaking)
    EXPECT_NE(firstGlyphId, DOT_GLYPH_ID);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakEmailTest002
 * @tc.desc: Test email with Chinese text and @ symbol but disable useLocaleForTextBreak
 * @tc.type: FUNC
 *
 * Scenario: Test "你好www@jfkls.40000000024.166acom" with useLocaleForTextBreak = false
 *           Font size: 50, Max width: 480, locale: zh-Hans
 *           Target: Verify Chinese text with email breaks correctly, second line starts with 'w'
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakEmailTest002, TestSize.Level0)
{
    LayoutForLocaleTest(false, "你好www@jfkls.40000000024.166acom", 480, "en-US");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t lastLineGlyphId = GetFirstGlyphIdOfLine(2, typography_);
    // Verify the third line starts with '0' glyph 633
    EXPECT_EQ(lastLineGlyphId, DIGIT_ZERO_GLYPH);
}

// ============ Chinese Punctuation Breaking Function Tests ============

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest001
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking enabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark "她说：“你好世界”"
 *           Font size: 50, Max width: 222, locale: zh-Hans
 *           Target: Verify left double quotation mark (") triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest001, TestSize.Level0)
{
    LayoutForLocaleTest(true, "她说：“你好世界”", 222, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "(", glyph code is 29085
    uint16_t leftQuotationMark = 29085;
    EXPECT_EQ(firstGlyphId, leftQuotationMark);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest002
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking disable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark “她说：”你好世界””
 *           Font size: 50, Max width: 222, locale: zh-Hans
 *           Target: Verify line breaks with character '说' when locale-aware text breaking disabled
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest002, TestSize.Level0)
{
    LayoutForLocaleTest(false, "她说：“你好世界”", 222, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "说"
    EXPECT_EQ(firstGlyphId, CHARACTER_SHUO_GLYPH_ZH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest003
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking disabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark "她说："你好世界""
 *           Font size: 50, Max width: 217, locale: zh-Hans
 *           Target: Verify left double quotation mark does not trigger soft break without locale-aware text breaking
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest003, TestSize.Level0)
{
    LayoutForLocaleTest(false, "她说：\"你好世界\"", 217, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "说"
    EXPECT_EQ(firstGlyphId, CHARACTER_SHUO_GLYPH_ZH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest004
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking enable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark "她说："你好世界""
 *           Font size: 50, Max width: 217, locale: zh-Hans
 *           Target: Verify left double quotation mark does not trigger soft break without locale-aware text breaking
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest004, TestSize.Level0)
{
    LayoutForLocaleTest(true, "她说：\"你好世界\"", 217, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "说"
    EXPECT_EQ(firstGlyphId, CHARACTER_SHUO_GLYPH_ZH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest005
 * @tc.desc: Test Chinese left corner bracket break with locale-aware text breaking enabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with left corner brackets "这是「测试」文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify left corner bracket (「) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest005, TestSize.Level0)
{
    LayoutForLocaleTest(true, "这是「测试」文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "「"
    EXPECT_EQ(firstGlyphId, LEFT_CORNER_BRACKET_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest006
 * @tc.desc: Test Chinese left corner bracket break with locale-aware text breaking disable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left corner brackets "这是「测试」文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify left corner bracket (「) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest006, TestSize.Level0)
{
    LayoutForLocaleTest(false, "这是「测试」文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "「"
    EXPECT_EQ(firstGlyphId, LEFT_CORNER_BRACKET_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest007
 * @tc.desc: Test Chinese left double angle bracket break with locale-aware text breaking enabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double angle brackets "这是《测试》文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify left double angle bracket (《) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest007, TestSize.Level0)
{
    LayoutForLocaleTest(true, "这是《测试》文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "《"
    EXPECT_EQ(firstGlyphId, LEFT_DOUBLE_ANGLE_BRACKET_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest008
 * @tc.desc: Test Chinese left double angle bracket break with locale-aware text breaking disable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double angle brackets "这是《测试》文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify left double angle bracket (《) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest008, TestSize.Level0)
{
    LayoutForLocaleTest(false, "这是《测试》文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "《"
    EXPECT_EQ(firstGlyphId, LEFT_DOUBLE_ANGLE_BRACKET_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest009
 * @tc.desc: Test Chinese left white corner bracket break with locale-aware text breaking enabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with left white corner brackets "这是『测试』文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify left white corner bracket (『) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest009, TestSize.Level0)
{
    LayoutForLocaleTest(true, "这是『测试』文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "『"
    EXPECT_EQ(firstGlyphId, LEFT_WHITE_CORNER_BRACKET_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest010
 * @tc.desc: Test Chinese left white corner bracket break with locale-aware text breaking enabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with left white corner brackets "这是『测试』文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify left white corner bracket (『) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest010, TestSize.Level0)
{
    LayoutForLocaleTest(false, "这是『测试』文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "『"
    EXPECT_EQ(firstGlyphId, LEFT_WHITE_CORNER_BRACKET_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest011
 * @tc.desc: Test fullwidth left parenthesis break with locale-aware text breaking enabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with fullwidth left parenthesis "这是（测试）文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify fullwidth left parenthesis (（) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest011, TestSize.Level0)
{
    LayoutForLocaleTest(true, "这是（测试）文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "（"
    EXPECT_EQ(firstGlyphId, FULLWIDTH_LEFT_PARENTHESIS_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest012
 * @tc.desc: Test fullwidth left parenthesis break with locale-aware text breaking disable
 * @tc.type: FUNC
 *
 * Scenario: Test text with fullwidth left parenthesis "这是（测试）文本"
 *           Font size: 50, Max width: 192, locale: zh-Hans
 *           Target: Verify fullwidth left parenthesis (（) triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest012, TestSize.Level0)
{
    LayoutForLocaleTest(false, "这是（测试）文本", 192, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "（"
    EXPECT_EQ(firstGlyphId, FULLWIDTH_LEFT_PARENTHESIS_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest013
 * @tc.desc: Test Chinese left single quotation mark break with locale-aware text breaking enabled
 * @tc.type: FUNC
 *
 * Scenario: Test text with left single quotation mark "这是'测试'文本"
 *           Font size: 50, Max width: 158, locale: zh-Hans
 *           Target: Verify left single quotation mark (') triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest013, TestSize.Level0)
{
    LayoutForLocaleTest(true, "这是'测试'文本", 158, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "'""
    EXPECT_EQ(firstGlyphId, LEFT_SINGLE_QUOTATION_MARK_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest014
 * @tc.desc: Test Chinese left single quotation mark break with locale-aware text breaking disable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left single quotation mark "这是'测试'文本"
 *           Font size: 50, Max width: 158, locale: zh-Hans
 *           Target: Verify left single quotation mark (') triggers soft break before it
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest014, TestSize.Level0)
{
    LayoutForLocaleTest(false, "这是'测试'文本", 158, "zh-Hans");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 4);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "'""
    EXPECT_EQ(firstGlyphId, LEFT_SINGLE_QUOTATION_MARK_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest015
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking enable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark “她说：”你好世界””
 *           Font size: 50, Max width: 222, locale: zh-Hant
 *           Target: Verify line breaks with left double quotation mark at start of second line with zh-Hant locale
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest015, TestSize.Level0)
{
    LayoutForLocaleTest(true, "她说：“你好世界”", 222, "zh-Hant");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "“"
    EXPECT_EQ(firstGlyphId, LEFT_DOUBLE_QUOTATION_MARK_ALT_GLYPH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest016
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking enable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark "她说：“你好世界”"
 *           Font size: 50, Max width: 222, locale: en-Latn
 *           Target: Verify line breaks with character "说" at start of second line with en-Latn locale
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest016, TestSize.Level0)
{
    LayoutForLocaleTest(true, "她说：“你好世界”", 222, "en-Latn");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "说"
    EXPECT_EQ(firstGlyphId, CHARACTER_SHUO_GLYPH_ZH);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest017
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking enable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark "她说：“你好世界”"
 *           Font size: 50, Max width: 222, locale: ja-JP
 *           Target: Verify line breaks with character "说" at start of second line with ja-JP locale
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest017, TestSize.Level0)
{
    LayoutForLocaleTest(true, "她说：“你好世界”", 222, "ja-JP");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "说"
    EXPECT_EQ(firstGlyphId, CHARACTER_SHUO_GLYPH_JA_KO);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest018
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking enable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark "她说：“你好世界”"
 *           Font size: 50, Max width: 222, locale: ko-KR
 *           Target: Verify line breaks with character "说" at start of second line with ko-KR locale
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest018, TestSize.Level0)
{
    LayoutForLocaleTest(true, "她说：“你好世界”", 222, "ko-KR");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "说"
    EXPECT_EQ(firstGlyphId, CHARACTER_SHUO_GLYPH_JA_KO);
}

/*
 * @tc.name: OHDrawingUseLocaleForTextBreakPunctuationTest019
 * @tc.desc: Test Chinese left double quotation mark break with locale-aware text breaking enable
 * @tc.type: FUNC
 *
 * Scenario: Test text with left double quotation mark "她说：“你好世界”"
 *           Font size: 50, Max width: 222, locale: en-US
 *           Target: Verify line breaks with character "说" at start of second line with en-US locale
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingUseLocaleForTextBreakPunctuationTest019, TestSize.Level0)
{
    LayoutForLocaleTest(true, "她说：“你好世界”", 222, "en-US");
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Expect glyph's code is "说"
    EXPECT_EQ(firstGlyphId, CHARACTER_SHUO_GLYPH_ZH);
}

// ============ BreakWord Email Breaking Function Tests ============

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest001
 * @tc.desc: Test basic email address breaking with BREAK_WORD and HIGH_QUALITY
 * @tc.type: FUNC
 *
 * Scenario: Test "Hello World 12345678@example.com" with BREAK_WORD + HIGH_QUALITY
 *           Font size: 50, Max width: 485
 *           Target: Verify email address breaks within characters, second line starts with digit
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest001, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("Hello World 12345678@example.com", 485, BreakStrategy::HIGH_QUALITY,
        WordBreakType::BREAK_WORD);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with '8'
    EXPECT_EQ(firstGlyphId, 641);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest002
 * @tc.desc: Test email address with multiple dots
 * @tc.type: FUNC
 *
 * Scenario: Test "Email: user.name@domain.co.uk" with BREAK_WORD + HIGH_QUALITY
 *           Font size: 50, Max width: 395
 *           Target: Verify email with multiple dots breaks correctly
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest002, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("Email: user.name@domain.co.uk", 395, BreakStrategy::HIGH_QUALITY,
        WordBreakType::BREAK_WORD);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with @ symbol
    EXPECT_EQ(firstGlyphId, 844);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest003
 * @tc.desc: Test text with email pattern (@ symbol)
 * @tc.type: FUNC
 *
 * Scenario: Test "Hello World Wonderful" with BREAK_WORD + HIGH_QUALITY
 *           Font size: 50, Max width: 500
 *           Target: Verify non-email text doesn't break within words
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest003, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("Hello World Wonderful user.name@domain.co.uk", 500, BreakStrategy::HIGH_QUALITY,
        WordBreakType::BREAK_WORD);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with 'W'
    EXPECT_EQ(firstGlyphId, 170);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest004
 * @tc.desc: Test text with @ but no dot (not an email)
 * @tc.type: FUNC
 *
 * Scenario: Test "user@localhost server" with BREAK_WORD + HIGH_QUALITY
 *           Font size: 50, Max width: 450
 *           Target: Verify text with @ but no dot doesn't break as email
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest004, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("user@localhost server", 450, BreakStrategy::HIGH_QUALITY,
        WordBreakType::BREAK_WORD);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with 's'
    EXPECT_EQ(firstGlyphId, 333);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest005
 * @tc.desc: Test email with dot before @ (invalid email pattern)
 * @tc.type: FUNC
 *
 * Scenario: Test "email user.name@domain" with BREAK_WORD + HIGH_QUALITY
 *           Font size: 50, Max width: 500
 *           Target: Verify email with dot before @ but no dot after @ is not treated as valid email
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest005, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("email user.name@domain", 500, BreakStrategy::HIGH_QUALITY,
        WordBreakType::BREAK_WORD);
    size_t lineCount = typography_->GetLineCount();
    // Should break at word boundary only (not as email)
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with 'u'
    EXPECT_EQ(firstGlyphId, 346);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest006
 * @tc.desc: Test BREAK_WORD with GREEDY strategy (no email breaking)
 * @tc.type: FUNC
 *
 * Scenario: Test "Contact: test@example.com" with BREAK_WORD + GREEDY
 *           Font size: 50, Max width: 460
 *           Target: Verify GREEDY strategy doesn't apply email breaking enhancement
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest006, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("Contact: test@example.com", 460, BreakStrategy::GREEDY,
        WordBreakType::BREAK_WORD);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with 't'
    EXPECT_EQ(firstGlyphId, 341);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest007
 * @tc.desc: Test BREAK_ALL instead of BREAK_WORD (no email breaking)
 * @tc.type: FUNC
 *
 * Scenario: Test "info@company.com" with BREAK_ALL + HIGH_QUALITY
 *           Font size: 50, Max width: 349
 *           Target: Verify BREAK_ALL doesn't apply email-specific breaking (uses general BREAK_ALL logic)
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest007, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("info@company.com", 349, BreakStrategy::HIGH_QUALITY,
        WordBreakType::BREAK_ALL);
    size_t lineCount = typography_->GetLineCount();
    // BREAK_ALL allows breaks after any character (except punctuation)
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with '.'
    EXPECT_EQ(firstGlyphId, DOT_GLYPH_ID);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest008
 * @tc.desc: Test multiple emails in one line
 * @tc.type: FUNC
 *
 * Scenario: Test "Email1: a@b.com Email2: x@y.z" with BREAK_WORD + HIGH_QUALITY
 *           Font size: 50, Max width: 560
 *           Target: Verify multiple email addresses are correctly identified and broken
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest008, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("Email1: a@b.com Email2: x@y.z", 560, BreakStrategy::HIGH_QUALITY,
        WordBreakType::BREAK_WORD);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with 'E'
    EXPECT_EQ(firstGlyphId, 41);
}

/*
 * @tc.name: OHDrawingBreakWordEmailBreakTest009
 * @tc.desc: Test for relayout word break type with email pattern (@ symbol)
 * @tc.type: FUNC
 *
 * Scenario: Test "Hello World 12345678@example.com" with BREAK_WORD + HIGH_QUALITY
 *           Font size: 50, Max width: 500
 *           Target: Verify non-email text doesn't break within words
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingBreakWordEmailBreakTest009, TestSize.Level0)
{
    fontCollection_->ClearCaches();
    LayoutForLineBreakStrategyTest("Hello World 12345678@example.com", 485, BreakStrategy::GREEDY,
        WordBreakType::BREAK_WORD);
    TypographyStyle typographyStyle;
    typographyStyle.breakStrategy = BreakStrategy::HIGH_QUALITY;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::BREAKSTRAGY));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    TextStyle textStyle;
    std::vector<OHOS::Rosen::TextStyle> relayouttextstyles;
    relayouttextstyles.push_back(textStyle);
    typography_->Relayout(485, typographyStyle, relayouttextstyles);

    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 2);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with '8'
    EXPECT_EQ(firstGlyphId, 641);
}

// ============ HighQuality + BreakAll Punctuation Tests ============

/*
 * @tc.name: OHDrawingHighQualityBreakAllPunctuationTest001
 * @tc.desc: Test Chinese comma (，) doesn't appear at line start with HIGH_QUALITY + BREAK_ALL
 * @tc.type: FUNC
 *
 * Scenario: Test "这是一段很长的文本内容，用于测试标点符号的断行行为" with HIGH_QUALITY + BREAK_ALL
 *           Font size: 50, Max width: 580
 *           Target: Verify Chinese comma (，) doesn't appear at the start of any line
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingHighQualityBreakAllPunctuationTest001, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("这是一段很长的文本内容，用于测试标点符号的断行行为", 580,
        BreakStrategy::HIGH_QUALITY, WordBreakType::BREAK_ALL);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with '容'
    EXPECT_EQ(firstGlyphId, 10985);
}

/*
 * @tc.name: OHDrawingHighQualityBreakAllPunctuationTest002
 * @tc.desc: Test Chinese period (。) doesn't appear at line start with HIGH_QUALITY + BREAK_ALL
 * @tc.type: FUNC
 *
 * Scenario: Test "这是一个测试句子。这是另一个测试句子。" with HIGH_QUALITY + BREAK_ALL
 *           Font size: 50, Max width: 440
 *           Target: Verify Chinese period (。) doesn't appear at the start of any line
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingHighQualityBreakAllPunctuationTest002, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("这是一个测试句子。这是另一个测试句子。", 440,
        BreakStrategy::HIGH_QUALITY, WordBreakType::BREAK_ALL);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with '子'
    EXPECT_NE(firstGlyphId, 124);
}

/*
 * @tc.name: OHDrawingHighQualityBreakAllPunctuationTest003
 * @tc.desc: Test English comma (,) doesn't appear at line start with HIGH_QUALITY + BREAK_ALL
 * @tc.type: FUNC
 *
 * Scenario: Test "This is a long English text, with punctuation marks for testing" with HIGH_QUALITY + BREAK_ALL
 *           Font size: 50, Max width: 584
 *           Target: Verify English comma (,) doesn't appear at the start of any line
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingHighQualityBreakAllPunctuationTest003, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("This is a long English text, with punctuation marks for testing", 584,
        BreakStrategy::HIGH_QUALITY, WordBreakType::BREAK_ALL);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with 't'
    EXPECT_NE(firstGlyphId, 124);
}

/*
 * @tc.name: OHDrawingHighQualityBreakAllPunctuationTest004
 * @tc.desc: Test mixed Chinese and English punctuation with HIGH_QUALITY + BREAK_ALL
 * @tc.type: FUNC
 *
 * Scenario: Test "这是中文，This is English text.混合文本测试" with HIGH_QUALITY + BREAK_ALL
 *           Font size: 50, Max width: 214
 *           Target: Verify punctuation marks don't appear at line starts in mixed text
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingHighQualityBreakAllPunctuationTest004, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("这是中文，This is English text.混合文本测试", 214,
        BreakStrategy::HIGH_QUALITY, WordBreakType::BREAK_ALL);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 5);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    EXPECT_EQ(firstGlyphId, 13495);
}

/*
 * @tc.name: OHDrawingHighQualityBreakAllPunctuationTest005
 * @tc.desc: Test BREAK_ALL with GREEDY strategy (punctuation may appear at line start)
 * @tc.type: FUNC
 *
 * Scenario: Test "这是一段很长文本，测试不同断行策略的效果" with BREAK_ALL + GREEDY
 *           Font size: 50, Max width: 430
 *           Target: Verify GREEDY strategy may allow punctuation at line starts (different from HIGH_QUALITY)
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingHighQualityBreakAllPunctuationTest005, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("这是一段很长文本，测试不同断行策略的效果", 430,
        BreakStrategy::GREEDY, WordBreakType::BREAK_ALL);
    size_t lineCount = typography_->GetLineCount();
    // GREEDY strategy may have different line breaking behavior
    EXPECT_EQ(lineCount, 3);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    // Verify the second line starts with '，'
    EXPECT_NE(firstGlyphId, 124);
}

/*
 * @tc.name: OHDrawingHighQualityBreakAllPunctuationTest006
 * @tc.desc: Test Chinese text with multiple punctuation marks
 * @tc.type: FUNC
 *
 * Scenario: Test "你好，世界！这是测试；今天天气很好。" with HIGH_QUALITY + BREAK_ALL
 *           Font size: 50, Max width: 113
 *           Target: Verify various Chinese punctuation marks (，！；。) don't appear at line starts
 */
HWTEST_F(OHDrawingUseLocaleForTextBreakTest, OHDrawingHighQualityBreakAllPunctuationTest006, TestSize.Level0)
{
    LayoutForLineBreakStrategyTest("你好，世界！这是测试；今天天气很好。", 113,
        BreakStrategy::HIGH_QUALITY, WordBreakType::BREAK_ALL);
    size_t lineCount = typography_->GetLineCount();
    EXPECT_EQ(lineCount, 11);
    uint16_t firstGlyphId = GetFirstGlyphIdOfLine(1, typography_);
    EXPECT_EQ(firstGlyphId, 10413);
}
} // namespace Rosen
} // namespace OHOS