/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "typography.h"
#include "typography_create.h"
#include "font_collection.h"
#include "txt/text_bundle_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const double ARC_FONT_SIZE = 28;
class OH_Drawing_TypographyTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TypographyTest001
 * @tc.desc: test for get max width for Typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest001, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetMaxWidth(), 0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest002
 * @tc.desc: test for get height for Typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest002, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetHeight(), 0);
    EXPECT_EQ(typography->GetActualWidth(), 0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest003
 * @tc.desc: test for get actual width for Typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest003, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetActualWidth(), 0);
    EXPECT_EQ(typography->GetMaxIntrinsicWidth(), 0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest004
 * @tc.desc: test for get min intrinsic width for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest004, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetMinIntrinsicWidth(), 0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest005
 * @tc.desc: test for get members for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest005, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetAlphabeticBaseline(), 0);
    EXPECT_EQ(typography->GetIdeographicBaseline(), 0);
    typography->GetGlyphsBoundsTop();
    typography->GetGlyphsBoundsBottom();
    typography->GetGlyphsBoundsLeft();
    typography->GetGlyphsBoundsRight();
    EXPECT_FALSE(typography->DidExceedMaxLines());
    EXPECT_EQ(typography->GetLineCount(), 0);
    typography->MarkDirty();
    EXPECT_EQ(typography->GetUnresolvedGlyphsCount(), -1);
}

/*
 * @tc.name: OH_Drawing_TypographyTest006
 * @tc.desc: test for updatting font size for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest006, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    // 1, 10, 20.0 for unit test
    typography->UpdateFontSize(1, 10, 20.0);
    // {1.2, 3.4} for unit test
    std::vector<float> indents = {1.2, 3.4};
    typography->SetIndents(indents);
    //3 > indents.size(), return indents.back()
    EXPECT_EQ(typography->DetectIndents(3), indents[1]);
    // 100 for unit test
    typography->Layout(100);
    SkCanvas* canvas = nullptr;
    // 10.0, 10.0 for unit test
    typography->Paint(canvas, 10.0, 10.0);
    OHOS::Rosen::Drawing::Canvas* drawingCanvas = nullptr;
    // 20.0, 20.0 for unit test
    typography->Paint(drawingCanvas, 20.0, 20.0);
    TextRectHeightStyle textRectHeightStyle = TextRectHeightStyle::TIGHT;
    TextRectWidthStyle textRectWidthtStyle = TextRectWidthStyle::MAX;
    std::vector<TextRect> vectorTextRect = typography->GetTextRectsByBoundary(
        1, 2, textRectHeightStyle, textRectWidthtStyle);
    vectorTextRect = typography->GetTextRectsOfPlaceholders();
    // 1.0, 2.0 for unit test
    typography->GetGlyphIndexByCoordinate(1.0, 2.0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest007
 * @tc.desc: test for getting word boundary by index for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest007, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(
        typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    // 0 for unit test
    typography->GetWordBoundaryByIndex(0);
    typography->GetActualTextRange(0, false);
    EXPECT_EQ(typography->GetLineHeight(0), 0.0);
    EXPECT_EQ(typography->GetLineWidth(0), 0.0);
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc;
    typography->SetAnimation(animationFunc);
    typography->SetParagraghId(0);
    typography->MeasureText();
    LineMetrics* lineMetrics = nullptr;
    EXPECT_FALSE(typography->GetLineInfo(0, true, true, lineMetrics));
    typography->GetLineMetrics();
    EXPECT_FALSE(typography->GetLineMetricsAt(0, lineMetrics));
    OHOS::Rosen::TextStyle txtStyle;
    typography->GetFontMetrics(txtStyle);
    size_t charNum = 0;
    std::vector<Drawing::FontMetrics> vectorFontMetrics;
    EXPECT_FALSE(typography->GetLineFontMetrics(0, charNum, vectorFontMetrics));
    typography->GetTextLines();
    typography->CloneSelf();
}

/*
 * @tc.name: OH_Drawing_TypographyTest008
 * @tc.desc: test for GetLongestLineWithIndent & GetLineInfo & GetLineMetrics
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest008, TestSize.Level1)
{
    double maxWidth = 50;
    std::vector<float> indents = {1.2, 3.4};
    OHOS::Rosen::TypographyStyle typographyStyle;
    auto textStyle = typographyStyle.GetTextStyle();
    EXPECT_EQ(textStyle.fontSize, 14);
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"text";
    typographyCreate->AppendText(text);
    OHOS::Rosen::TextStyle typographyTextStyle;
    typographyCreate->PushStyle(typographyTextStyle);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->SetIndents(indents);
    typography->Layout(maxWidth);

    // 0 for unit test
    EXPECT_EQ(std::round(typography->GetLongestLineWithIndent()), 26);
    EXPECT_LE(typography->GetLongestLineWithIndent(), maxWidth);

    LineMetrics lineMetrics;
    EXPECT_TRUE(typography->GetLineInfo(0, true, true, &lineMetrics));
    EXPECT_EQ(std::round(lineMetrics.ascender), 13);
    EXPECT_EQ(std::round(lineMetrics.descender), 3);
    EXPECT_EQ(lineMetrics.height, 16);
    EXPECT_EQ(lineMetrics.x, indents[0]);
    EXPECT_EQ(lineMetrics.y, 0);
    EXPECT_EQ(lineMetrics.startIndex, 0);
    EXPECT_EQ(lineMetrics.endIndex, text.size());

    std::vector<LineMetrics> lines = typography->GetLineMetrics();
    ASSERT_EQ(lines.size(), 1);
    LineMetrics firstLineMetrics = lines[0];
    EXPECT_EQ(std::round(firstLineMetrics.ascender), 13);
    EXPECT_EQ(std::round(firstLineMetrics.descender), 3);
    EXPECT_EQ(firstLineMetrics.height, 16);
    EXPECT_EQ(firstLineMetrics.x, indents[0]);
    EXPECT_EQ(firstLineMetrics.y, 0);
    EXPECT_EQ(firstLineMetrics.startIndex, 0);
    EXPECT_EQ(firstLineMetrics.endIndex, text.size());
}

/*
 * @tc.name: OH_Drawing_TypographyTest009
 * @tc.desc: test for GetEllipsisTextRange
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest009, TestSize.Level1)
{
    double maxWidth = 50;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"text";
    typographyCreate->AppendText(text);
    OHOS::Rosen::TextStyle typographyTextStyle;
    typographyCreate->PushStyle(typographyTextStyle);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();

    Boundary range = typography->GetEllipsisTextRange();
    ASSERT_EQ(range, Boundary(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max()));
    typography->Layout(maxWidth);

    Boundary range1 = typography->GetEllipsisTextRange();
    ASSERT_EQ(range1, Boundary(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max()));

    OHOS::Rosen::TypographyStyle typographyStyle1;
    typographyStyle1.maxLines = 1;
    std::u16string ellipsisStr = TypographyStyle::ELLIPSIS;
    typographyStyle1.ellipsis = ellipsisStr;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate1 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle1, fontCollection);
    std::u16string text1 = u"text is too long";
    typographyCreate1->AppendText(text1);
    OHOS::Rosen::TextStyle typographyTextStyle1;
    typographyCreate1->PushStyle(typographyTextStyle1);
    std::unique_ptr<OHOS::Rosen::Typography> typography1 = typographyCreate1->CreateTypography();
    typography1->Layout(maxWidth);
    Boundary range2 = typography1->GetEllipsisTextRange();
    ASSERT_EQ(range2, Boundary(5, 16));

    // For branch coverage
    OHOS::Rosen::TypographyStyle typographyStyle2;
    typographyStyle2.maxLines = 1;
    typographyStyle2.ellipsis = ellipsisStr;
    typographyStyle2.ellipsisModal = EllipsisModal::MIDDLE;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate2 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle2, fontCollection);
    typographyCreate2->AppendText(text1);
    OHOS::Rosen::TextStyle typographyTextStyle2;
    typographyCreate2->PushStyle(typographyTextStyle2);
    std::unique_ptr<OHOS::Rosen::Typography> typography2 = typographyCreate2->CreateTypography();
    typography2->Layout(maxWidth);
    Boundary range3 = typography2->GetEllipsisTextRange();
    ASSERT_EQ(range3, Boundary(2, 14));
    typography2->GetEllipsisTextRange();
}

/*
 * @tc.name: OH_Drawing_TypographyTest010
 * @tc.desc: test for GeneratePaintRegion
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest010, TestSize.Level1)
{
    double maxWidth = 50;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"text is too long                                 ";
    PlaceholderSpan placeholderSpan;
    placeholderSpan.width = 10;
    placeholderSpan.height = 300;
    placeholderSpan.alignment = PlaceholderVerticalAlignment::TOP_OF_ROW_BOX;
    placeholderSpan.baseline = TextBaseline::ALPHABETIC;
    placeholderSpan.baselineOffset = 0;
    OHOS::Rosen::TextStyle typographyTextStyle;
    typographyCreate->PushStyle(typographyTextStyle);
    typographyCreate->AppendPlaceholder(placeholderSpan);
    typographyCreate->AppendText(text);
    TextShadow shadow;
    shadow.blurRadius = 5.0f;
    shadow.offset = Drawing::Point(0, 10);
    typographyTextStyle.shadows.emplace_back(shadow);
    typographyCreate->PushStyle(typographyTextStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();

    Drawing::RectI paintRegion = typography->GeneratePaintRegion(5.5, 5.1);
    ASSERT_EQ(paintRegion, Drawing::RectI(5, 5, 5, 5));
    typography->Layout(maxWidth);
    paintRegion = typography->GeneratePaintRegion(5.5, 5.1);
    ASSERT_EQ(paintRegion, Drawing::RectI(0, 7, 60, 385));
}

/*
 * @tc.name: OH_Drawing_TypographyTest011
 * @tc.desc: test for truncated hight surrogate emoji text building and layouting
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest011, TestSize.Level1)
{
    double maxWidth = 50;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text;
    // hight surrogate emoji
    text.push_back(0xD83D);
    OHOS::Rosen::TextStyle typographyTextStyle;
    typographyCreate->PushStyle(typographyTextStyle);

    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().initStatus_ = true;
    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().bundleApiVersion_ =
        OHOS::Rosen::SPText::SINCE_API16_VERSION;
    typographyCreate->AppendText(text);
    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().initStatus_ = false;

    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    // The value of longestlineWithIndent will Close to 16 if the truncation of emoji fails.
    ASSERT_TRUE(skia::textlayout::nearlyEqual(typography->GetLongestLineWithIndent(), ARC_FONT_SIZE / 2));
}

/*
 * @tc.name: OH_Drawing_TypographyTest012
 * @tc.desc: test for truncated surrogate pair reverse emoji text building and layouting
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest012, TestSize.Level1)
{
    double maxWidth = 50;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text;
    // emoji low surrogate
    text.push_back(0xDC7B);
    // emoji hight surrogate
    text.push_back(0xD83D);
    OHOS::Rosen::TextStyle typographyTextStyle;
    typographyCreate->PushStyle(typographyTextStyle);

    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().initStatus_ = true;
    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().bundleApiVersion_ =
        OHOS::Rosen::SPText::SINCE_API16_VERSION;
    typographyCreate->AppendText(text);
    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().initStatus_ = false;

    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();

    typography->Layout(maxWidth);
    // The value of longestlineWithIndent will Close to 32 if the truncation of emoji fails.
    ASSERT_TRUE(skia::textlayout::nearlyEqual(typography->GetLongestLineWithIndent(), ARC_FONT_SIZE));
}

/*
 * @tc.name: OH_Drawing_TypographyTest013
 * @tc.desc: test for one-run and one-line paragraph height with paragraphspacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest013, TestSize.Level1)
{
    // 初始化环境：创建一个不包含段落间距的段落，其段落高度作为基线与后文进行对比。
    double maxWidth = 1000;
    std::u16string text = u"testParagraphSpacing";
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.isEndAddParagraphSpacing = false;
    typographyStyle0.paragraphSpacing = 0;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    typography0->Layout(maxWidth);
    // 测试为只包含1个run的文本设置段落间距的场景
    OHOS::Rosen::TypographyStyle typographyStyle1;
    typographyStyle1.isEndAddParagraphSpacing = true;
    typographyStyle1.paragraphSpacing = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection1 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate1 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle1, fontCollection1);
    typographyCreate1->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography1 = typographyCreate1->CreateTypography();
    typography1->Layout(maxWidth);
    EXPECT_EQ(typography0->GetHeight() + 40, typography1->GetHeight());
}

/*
 * @tc.name: OH_Drawing_TypographyTest014
 * @tc.desc: test for multi-run paragraph height with paragraphspacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest014, TestSize.Level1)
{
    // 初始化环境：创建一个不包含段落间距的段落，其段落高度作为基线与后文进行对比。
    double maxWidth = 100;
    std::u16string text = u"test ParagraphSpacing. 不是一个run.";
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.isEndAddParagraphSpacing = false;
    typographyStyle0.paragraphSpacing = 0;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    typography0->Layout(maxWidth);
    // 测试为包含多个run的文本设置段落间距的场景。
    OHOS::Rosen::TypographyStyle typographyStyle1;
    typographyStyle1.isEndAddParagraphSpacing = true;
    typographyStyle1.paragraphSpacing = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection1 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate1 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle1, fontCollection1);
    typographyCreate1->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography1 = typographyCreate1->CreateTypography();
    typography1->Layout(maxWidth);
    EXPECT_EQ(typography0->GetHeight() + 40, typography1->GetHeight());
}

/*
 * @tc.name: OH_Drawing_TypographyTest015
 * @tc.desc: test for height with paragraphspacing(ineffective)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest015, TestSize.Level1)
{
    // 初始化环境：创建一个不包含段落间距的段落，其段落高度作为基线与后文进行对比。
    double maxWidth = 500;
    std::u16string text = u"Test paragraphSpacing. Without hard line breaks and with isEndAddParagraphSpacing set to "
                          u"false, paragraph spacing should not take effect at this time.";
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.isEndAddParagraphSpacing = false;
    typographyStyle0.paragraphSpacing = 0;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    typography0->Layout(maxWidth);
    // 测试paragraphSpacing设为正值，但是段落中没有硬换行且isEndAddParagraphSpacing为false的场景：该场景下段落间距不生效。
    OHOS::Rosen::TypographyStyle typographyStyle1;
    typographyStyle1.isEndAddParagraphSpacing = false;
    typographyStyle1.paragraphSpacing = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection1 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate1 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle1, fontCollection1);
    typographyCreate1->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography1 = typographyCreate1->CreateTypography();
    typography1->Layout(maxWidth);
    EXPECT_EQ(typography0->GetHeight(), typography1->GetHeight());

    // 测试设置异常的paragraphSpacing，该场景下段落间距不生效。
    OHOS::Rosen::TypographyStyle typographyStyle2;
    typographyStyle2.isEndAddParagraphSpacing = true;
    typographyStyle2.paragraphSpacing = -40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection2 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate2 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle2, fontCollection2);
    typographyCreate2->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography2 = typographyCreate2->CreateTypography();
    typography2->Layout(maxWidth);
    EXPECT_EQ(typography0->GetHeight(), typography2->GetHeight());
}
} // namespace Rosen
} // namespace OHOS