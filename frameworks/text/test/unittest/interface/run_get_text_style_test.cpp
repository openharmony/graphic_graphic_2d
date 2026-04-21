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
#include "rosen_text/font_collection.h"
#include "rosen_text/run.h"
#include "rosen_text/text_style.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "rosen_text/typography_style.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;

namespace {
class RunGetTextStyleTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

protected:
    std::shared_ptr<FontCollection> fontCollection_;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate_;
    std::shared_ptr<OHOS::Rosen::Typography> typography_;
    std::vector<std::unique_ptr<OHOS::Rosen::Run>> spRuns_;
};

void RunGetTextStyleTest::SetUp()
{
    fontCollection_ = FontCollection::Create();
    ASSERT_NE(fontCollection_, nullptr);
    fontCollection_->EnableGlobalFontMgr();
}

void RunGetTextStyleTest::TearDown()
{
    typography_.reset();
    typographyCreate_.reset();
    spRuns_.clear();
    fontCollection_.reset();
}

/*
 * @tc.name: RunGetTextStyleTest001
 * @tc.desc: Test GetTextStyle with default TextStyle values
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    std::u16string text = u"Hello";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(100.0);

    auto textLines = typography_->GetTextLines();
    ASSERT_GT(textLines.size(), 0);

    spRuns_ = textLines[0]->GetGlyphRuns();
    ASSERT_GT(spRuns_.size(), 0);

    TextStyle textStyle = spRuns_[0]->GetTextStyle();
    EXPECT_EQ(textStyle.color, Drawing::Color::COLOR_WHITE);
    EXPECT_DOUBLE_EQ(textStyle.fontSize, 14.0);
}

/*
 * @tc.name: RunGetTextStyleTest002
 * @tc.desc: Test GetTextStyle with font properties (fontSize, fontWeight, fontWidth, fontStyle, fontFamilies)
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest002, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 24.0;
    textStyle.fontWeight = FontWeight::W900;
    textStyle.fontWidth = FontWidth::EXPANDED;
    textStyle.fontStyle = OHOS::Rosen::FontStyle::ITALIC;
    textStyle.fontFamilies = { "sans-serif", "Arial" };

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"FontTest";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(200.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_DOUBLE_EQ(resultStyle.fontSize, 24.0);
    EXPECT_EQ(resultStyle.fontWeight, FontWeight::W900);
    EXPECT_EQ(resultStyle.fontWidth, FontWidth::EXPANDED);
    EXPECT_EQ(resultStyle.fontStyle, OHOS::Rosen::FontStyle::ITALIC);
    EXPECT_GT(resultStyle.fontFamilies.size(), 0);
}

/*
 * @tc.name: RunGetTextStyleTest003
 * @tc.desc: Test GetTextStyle with different font sizes comparison
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest003, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle1;
    auto paragraphBuilder1 = OHOS::Rosen::TypographyCreate::Create(paragraphStyle1, fontCollection_);
    ASSERT_NE(paragraphBuilder1, nullptr);

    std::u16string text = u"Hello";
    paragraphBuilder1->AppendText(text);

    auto paragraph1 = paragraphBuilder1->CreateTypography();
    ASSERT_NE(paragraph1, nullptr);

    paragraph1->Layout(100.0);

    auto textLines1 = paragraph1->GetTextLines();
    auto spRuns1 = textLines1[0]->GetGlyphRuns();
    TextStyle style1 = spRuns1[0]->GetTextStyle();

    OHOS::Rosen::TypographyStyle paragraphStyle2;
    auto paragraphBuilder2 = OHOS::Rosen::TypographyCreate::Create(paragraphStyle2, fontCollection_);
    ASSERT_NE(paragraphBuilder2, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 30.0;
    paragraphBuilder2->PushStyle(textStyle);
    paragraphBuilder2->AppendText(text);

    auto paragraph2 = paragraphBuilder2->CreateTypography();
    ASSERT_NE(paragraph2, nullptr);

    paragraph2->Layout(100.0);

    auto textLines2 = paragraph2->GetTextLines();
    auto spRuns2 = textLines2[0]->GetGlyphRuns();
    TextStyle style2 = spRuns2[0]->GetTextStyle();

    EXPECT_GT(style2.fontSize, style1.fontSize);
    EXPECT_DOUBLE_EQ(style1.fontSize, 14.0);
    EXPECT_DOUBLE_EQ(style2.fontSize, 30.0);
}

/*
 * @tc.name: RunGetTextStyleTest004
 * @tc.desc: Test GetTextStyle with decoration properties (decoration, decorationColor, decorationStyle,
 * decorationThicknessScale)
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest004, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.decoration = TextDecoration::UNDERLINE;
    textStyle.decorationColor = Drawing::Color::COLOR_RED;
    textStyle.decorationStyle = TextDecorationStyle::DOUBLE;
    textStyle.decorationThicknessScale = 2.0;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"DecorationTest";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(150.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.decoration, TextDecoration::UNDERLINE);
    EXPECT_EQ(resultStyle.decorationColor, Drawing::Color::COLOR_RED);
    EXPECT_EQ(resultStyle.decorationStyle, TextDecorationStyle::DOUBLE);
    EXPECT_DOUBLE_EQ(resultStyle.decorationThicknessScale, 2.0);
}

/*
 * @tc.name: RunGetTextStyleTest005
 * @tc.desc: Test GetTextStyle with spacing properties (letterSpacing, wordSpacing, heightScale)
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest005, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.letterSpacing = 2.5;
    textStyle.wordSpacing = 5.0;
    textStyle.heightOnly = true;
    textStyle.heightScale = 1.5;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"Spacing Test";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(200.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_DOUBLE_EQ(resultStyle.letterSpacing, 2.5);
    EXPECT_DOUBLE_EQ(resultStyle.wordSpacing, 5.0);
    EXPECT_TRUE(resultStyle.heightOnly);
    EXPECT_DOUBLE_EQ(resultStyle.heightScale, 1.5);
}

/*
 * @tc.name: RunGetTextStyleTest006
 * @tc.desc: Test GetTextStyle with line height properties (halfLeading, heightOnly, maxLineHeight, minLineHeight,
 * lineHeightStyle)
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest006, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.halfLeading = true;
    textStyle.heightOnly = true;
    textStyle.maxLineHeight = 50.0;
    textStyle.minLineHeight = 10.0;
    textStyle.lineHeightStyle = LineHeightStyle::kFontSize;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"LineHeight";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(150.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.halfLeading, true);
    EXPECT_EQ(resultStyle.heightOnly, true);
    EXPECT_DOUBLE_EQ(resultStyle.maxLineHeight, 50.0);
    EXPECT_DOUBLE_EQ(resultStyle.minLineHeight, 10.0);
    EXPECT_EQ(resultStyle.lineHeightStyle, LineHeightStyle::kFontSize);
}

/*
 * @tc.name: RunGetTextStyleTest007
 * @tc.desc: Test GetTextStyle with baseline properties (baseline, baseLineShift)
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest007, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.baseline = TextBaseline::IDEOGRAPHIC;
    textStyle.baseLineShift = 3.5;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"BaselineTest";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(150.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.baseline, TextBaseline::IDEOGRAPHIC);
    EXPECT_DOUBLE_EQ(resultStyle.baseLineShift, 3.5);
}

/*
 * @tc.name: RunGetTextStyleTest008
 * @tc.desc: Test GetTextStyle with color property
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest008, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.color = Drawing::Color(0xFF00FF00);

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"ColorTest";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(100.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.color, Drawing::Color(0xFF00FF00));
}

/*
 * @tc.name: RunGetTextStyleTest009
 * @tc.desc: Test GetTextStyle with fontEdging property
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest009, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.fontEdging = Drawing::FontEdging::ALIAS;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"Edging";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(100.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.fontEdging, Drawing::FontEdging::ALIAS);
}

/*
 * @tc.name: RunGetTextStyleTest010
 * @tc.desc: Test GetTextStyle with backgroundRect property
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest010, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.backgroundRect = { 0xFF0000FF, 5.0, 5.0, 5.0, 5.0 };

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"Background";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(100.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.backgroundRect.color, 0xFF0000FFu);
    EXPECT_DOUBLE_EQ(resultStyle.backgroundRect.leftTopRadius, 5.0);
    EXPECT_DOUBLE_EQ(resultStyle.backgroundRect.rightTopRadius, 5.0);
    EXPECT_DOUBLE_EQ(resultStyle.backgroundRect.rightBottomRadius, 5.0);
    EXPECT_DOUBLE_EQ(resultStyle.backgroundRect.leftBottomRadius, 5.0);
}

/*
 * @tc.name: RunGetTextStyleTest011
 * @tc.desc: Test GetTextStyle with locale property
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest011, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.locale = "zh-CN";

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"Locale";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(100.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.locale, "zh-CN");
}

/*
 * @tc.name: RunGetTextStyleTest013
 * @tc.desc: Test GetTextStyle with styleId property
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest013, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.styleId = 123;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"StyleId";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(100.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.styleId, 123);
}

/*
 * @tc.name: RunGetTextStyleTest015
 * @tc.desc: Test GetTextStyle with badgeType property
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest015, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.badgeType = TextBadgeType::SUPERSCRIPT;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"Badge";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(100.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.badgeType, TextBadgeType::SUPERSCRIPT);
}

/*
 * @tc.name: RunGetTextStyleTest016
 * @tc.desc: Test GetTextStyle with combined properties for comprehensive validation
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest016, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle textStyle;
    textStyle.color = Drawing::Color(0xFF00FF00);
    textStyle.decoration = TextDecoration::LINE_THROUGH;
    textStyle.decorationColor = Drawing::Color::COLOR_BLUE;
    textStyle.decorationStyle = TextDecorationStyle::DOTTED;
    textStyle.decorationThicknessScale = 1.5;
    textStyle.fontSize = 18.0;
    textStyle.fontWeight = FontWeight::W600;
    textStyle.fontWidth = FontWidth::CONDENSED;
    textStyle.fontStyle = OHOS::Rosen::FontStyle::ITALIC;
    textStyle.fontFamilies = { "serif" };
    textStyle.letterSpacing = 2.0;
    textStyle.wordSpacing = 3.0;
    textStyle.heightScale = 1.2;
    textStyle.halfLeading = false;
    textStyle.heightOnly = false;
    textStyle.baseline = TextBaseline::ALPHABETIC;
    textStyle.baseLineShift = 1.0;
    textStyle.locale = "en-US";
    textStyle.styleId = 456;
    textStyle.isPlaceholder = false;
    textStyle.badgeType = TextBadgeType::SUBSCRIPT;

    typographyCreate_->PushStyle(textStyle);

    std::u16string text = u"Combined";
    typographyCreate_->AppendText(text);

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(150.0);

    auto textLines = typography_->GetTextLines();
    spRuns_ = textLines[0]->GetGlyphRuns();

    TextStyle resultStyle = spRuns_[0]->GetTextStyle();

    EXPECT_EQ(resultStyle.color, Drawing::Color(0xFF00FF00));
    EXPECT_EQ(resultStyle.decoration, TextDecoration::LINE_THROUGH);
    EXPECT_EQ(resultStyle.decorationColor, Drawing::Color::COLOR_BLUE);
    EXPECT_EQ(resultStyle.decorationStyle, TextDecorationStyle::DOTTED);
    EXPECT_DOUBLE_EQ(resultStyle.decorationThicknessScale, 1.5);
    EXPECT_DOUBLE_EQ(resultStyle.fontSize, 18.0);
    EXPECT_EQ(resultStyle.fontWeight, FontWeight::W600);
    EXPECT_EQ(resultStyle.fontWidth, FontWidth::CONDENSED);
    EXPECT_EQ(resultStyle.fontStyle, OHOS::Rosen::FontStyle::ITALIC);
    EXPECT_DOUBLE_EQ(resultStyle.letterSpacing, 2.0);
    EXPECT_DOUBLE_EQ(resultStyle.wordSpacing, 3.0);
    EXPECT_DOUBLE_EQ(resultStyle.heightScale, 0);
    EXPECT_EQ(resultStyle.halfLeading, false);
    EXPECT_EQ(resultStyle.heightOnly, false);
    EXPECT_EQ(resultStyle.baseline, TextBaseline::ALPHABETIC);
    EXPECT_DOUBLE_EQ(resultStyle.baseLineShift, 1.0);
    EXPECT_EQ(resultStyle.locale, "en-US");
    EXPECT_EQ(resultStyle.styleId, 456);
    EXPECT_EQ(resultStyle.isPlaceholder, false);
    EXPECT_EQ(resultStyle.badgeType, TextBadgeType::SUBSCRIPT);
}

/*
 * @tc.name: RunGetTextStyleTest017
 * @tc.desc: Test GetTextStyle with multiple text segments having different styles in one paragraph
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest017, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle style1;
    style1.fontSize = 16.0;
    style1.color = Drawing::Color::COLOR_RED;
    style1.fontWeight = FontWeight::W400;
    style1.styleId = 1;

    typographyCreate_->PushStyle(style1);
    typographyCreate_->AppendText(u"First");

    TextStyle style2;
    style2.fontSize = 24.0;
    style2.color = Drawing::Color::COLOR_BLUE;
    style2.fontWeight = FontWeight::W700;
    style2.styleId = 2;

    typographyCreate_->PushStyle(style2);
    typographyCreate_->AppendText(u"Second");

    TextStyle style3;
    style3.fontSize = 32.0;
    style3.color = Drawing::Color::COLOR_GREEN;
    style3.fontWeight = FontWeight::W900;
    style3.styleId = 3;

    typographyCreate_->PushStyle(style3);
    typographyCreate_->AppendText(u"Third");

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(500.0);

    auto textLines = typography_->GetTextLines();
    ASSERT_GT(textLines.size(), 0);

    auto runs = textLines[0]->GetGlyphRuns();
    ASSERT_GE(runs.size(), 3);

    TextStyle resultStyle1 = runs[0]->GetTextStyle();
    EXPECT_DOUBLE_EQ(resultStyle1.fontSize, 16.0);
    EXPECT_EQ(resultStyle1.color, Drawing::Color::COLOR_RED);
    EXPECT_EQ(resultStyle1.fontWeight, FontWeight::W400);
    EXPECT_EQ(resultStyle1.styleId, 1);

    TextStyle resultStyle2 = runs[1]->GetTextStyle();
    EXPECT_DOUBLE_EQ(resultStyle2.fontSize, 24.0);
    EXPECT_EQ(resultStyle2.color, Drawing::Color::COLOR_BLUE);
    EXPECT_EQ(resultStyle2.fontWeight, FontWeight::W700);
    EXPECT_EQ(resultStyle2.styleId, 2);

    TextStyle resultStyle3 = runs[2]->GetTextStyle();
    EXPECT_DOUBLE_EQ(resultStyle3.fontSize, 32.0);
    EXPECT_EQ(resultStyle3.color, Drawing::Color::COLOR_GREEN);
    EXPECT_EQ(resultStyle3.fontWeight, FontWeight::W900);
    EXPECT_EQ(resultStyle3.styleId, 3);
}

/*
 * @tc.name: RunGetTextStyleTest018
 * @tc.desc: Test GetTextStyle with multiple languages having different styles
 * @tc.type: FUNC
 */
HWTEST_F(RunGetTextStyleTest, RunGetTextStyleTest018, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle paragraphStyle;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(paragraphStyle, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);

    TextStyle englishStyle;
    englishStyle.locale = "en-US";
    englishStyle.fontSize = 18.0;
    englishStyle.fontWeight = FontWeight::W400;
    englishStyle.color = Drawing::Color::COLOR_BLACK;
    englishStyle.styleId = 100;

    typographyCreate_->PushStyle(englishStyle);
    typographyCreate_->AppendText(u"English");

    TextStyle chineseStyle;
    chineseStyle.locale = "zh-CN";
    chineseStyle.fontSize = 20.0;
    chineseStyle.fontWeight = FontWeight::W500;
    chineseStyle.color = Drawing::Color::COLOR_RED;
    chineseStyle.styleId = 200;

    typographyCreate_->PushStyle(chineseStyle);
    typographyCreate_->AppendText(u"中文测试");

    TextStyle arabicStyle;
    arabicStyle.locale = "ar-SA";
    arabicStyle.fontSize = 16.0;
    arabicStyle.fontWeight = FontWeight::W600;
    arabicStyle.color = Drawing::Color::COLOR_BLUE;
    arabicStyle.styleId = 300;

    typographyCreate_->PushStyle(arabicStyle);
    typographyCreate_->AppendText(u"اختبار");

    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    typography_->Layout(600.0);

    auto textLines = typography_->GetTextLines();
    ASSERT_GT(textLines.size(), 0);

    auto runs = textLines[0]->GetGlyphRuns();
    ASSERT_GE(runs.size(), 3);

    TextStyle resultEnglish = runs[0]->GetTextStyle();
    EXPECT_EQ(resultEnglish.locale, "en-US");
    EXPECT_DOUBLE_EQ(resultEnglish.fontSize, 18.0);
    EXPECT_EQ(resultEnglish.fontWeight, FontWeight::W400);
    EXPECT_EQ(resultEnglish.styleId, 100);

    TextStyle resultChinese = runs[1]->GetTextStyle();
    EXPECT_EQ(resultChinese.locale, "zh-CN");
    EXPECT_DOUBLE_EQ(resultChinese.fontSize, 20.0);
    EXPECT_EQ(resultChinese.fontWeight, FontWeight::W500);
    EXPECT_EQ(resultChinese.styleId, 200);

    TextStyle resultArabic = runs[2]->GetTextStyle();
    EXPECT_EQ(resultArabic.locale, "ar-SA");
    EXPECT_DOUBLE_EQ(resultArabic.fontSize, 16.0);
    EXPECT_EQ(resultArabic.fontWeight, FontWeight::W600);
    EXPECT_EQ(resultArabic.styleId, 300);
}

} // namespace