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
 * @tc.name: OH_Drawing_TypographyInnerBadgeTypeTest001
 * @tc.desc: Test for badge text
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyInnerBadgeTypeTest001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle superscriptTextStyle;
    superscriptTextStyle.fontSize = 50;
    superscriptTextStyle.badgeType = TextBadgeType::SUPERSCRIPT;
    OHOS::Rosen::TextStyle subscriptTextStyle;
    subscriptTextStyle.fontSize = 50;
    subscriptTextStyle.badgeType = TextBadgeType::SUBSCRIPT;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = 50;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::TypographyCreate> superTypographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::TypographyCreate> subTypographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);

    std::u16string text = u"你好测试 textstyle hello";
    superTypographyCreate->PushStyle(superscriptTextStyle);
    superTypographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> superscriptTypography = superTypographyCreate->CreateTypography();
    double maxWidth = 10000.0;
    superscriptTypography->Layout(maxWidth);

    subTypographyCreate->PushStyle(subscriptTextStyle);
    subTypographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> subscriptTypography = subTypographyCreate->CreateTypography();
    subscriptTypography->Layout(maxWidth);

    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography  = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    EXPECT_NE(superscriptTypography->GetHeight(), typography->GetHeight());
    EXPECT_NE(superscriptTypography->GetLongestLineWithIndent(), typography->GetLongestLineWithIndent());
    EXPECT_TRUE(skia::textlayout::nearlyEqual(superscriptTypography->GetHeight(), subscriptTypography->GetHeight()));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(superscriptTypography->GetLongestLineWithIndent(),
        subscriptTypography->GetLongestLineWithIndent()));
}

/*
 * @tc.name: OH_Drawing_TypographyInnerBadgeTypeTest002
 * @tc.desc: Test the conflict between the text badge and fontFeature
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyInnerBadgeTypeTest002, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle superscriptTextStyle;
    superscriptTextStyle.fontSize = 50;
    superscriptTextStyle.badgeType = TextBadgeType::SUPERSCRIPT;
    superscriptTextStyle.fontFeatures.SetFeature("sups", 1);
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = 50;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::TypographyCreate> superTypographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);

    std::u16string text = u"你好测试 textstyle hello";
    superTypographyCreate->PushStyle(superscriptTextStyle);
    superTypographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> superscriptTypography = superTypographyCreate->CreateTypography();
    double maxWidth = 10000.0;
    superscriptTypography->Layout(maxWidth);

    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography  = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(superscriptTypography->GetHeight(), typography->GetHeight()));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(superscriptTypography->GetLongestLineWithIndent(),
        typography->GetLongestLineWithIndent()));
}

/*
 * @tc.name: OH_Drawing_TypographyTest001
 * @tc.desc: test for get max width for Typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest001, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest002, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest003, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest004, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest005, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest006, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest007, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest008, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest009, TestSize.Level0)
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest010, TestSize.Level0)
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
#ifdef USE_M133_SKIA
    ASSERT_EQ(paintRegion, Drawing::RectI(0, 7, 54, 401));
#else
    ASSERT_EQ(paintRegion, Drawing::RectI(0, 7, 60, 385));
#endif
}

/*
 * @tc.name: OH_Drawing_TypographyTest011
 * @tc.desc: test for truncated hight surrogate emoji text building and layouting
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest011, TestSize.Level0)
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
        OHOS::Rosen::SPText::SINCE_API18_VERSION;
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest012, TestSize.Level0)
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
        OHOS::Rosen::SPText::SINCE_API18_VERSION;
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest013, TestSize.Level0)
{
    // Init: Create a paragraph without spacing between paragraphs. Use its height as baseline.
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.isEndAddParagraphSpacing = false;
    typographyStyle0.paragraphSpacing = 0;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"testParagraphSpacing";
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    double maxWidth = 1000;
    typography0->Layout(maxWidth);

    // Test the scenario of setting paragraph spacing for text containing one runs.
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest014, TestSize.Level0)
{
    // Init: Create a paragraph without spacing between paragraphs. Use its height as baseline.
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.isEndAddParagraphSpacing = false;
    typographyStyle0.paragraphSpacing = 0;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"test ParagraphSpacing. 不是一个run.";
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    double maxWidth = 100;
    typography0->Layout(maxWidth);

    // Test the scenario of setting paragraph spacing for text containing multiple runs.
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
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest015, TestSize.Level0)
{
    // Init: Create a paragraph without spacing between paragraphs. Use its height as baseline.
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.isEndAddParagraphSpacing = false;
    typographyStyle0.paragraphSpacing = 0;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"Test paragraphSpacing. Without hard line breaks and with isEndAddParagraphSpacing set to "
                        u"false, paragraph spacing should not take effect at this time.";
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    double maxWidth = 500;
    typography0->Layout(maxWidth);
    
    // Test paragraphSpacing is positive, there is no hard break within the paragraph, and isEndAddParagraphSpacing
    // is false: In this scenario, the paragraph spacing does not take effect.
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

    // Test paragraphSpacing is abnormal. In this scenario, the paragraph spacing does not take effect.
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

/*
 * @tc.name: OH_Drawing_TypographyTest016
 * @tc.desc: test for the actual effective value of textstyle: fallback textstyle.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest016, TestSize.Level0)
{
    // Use interfaces such as OH_Drawing_SetTypographyTextFontSize to test the fallback textstyle.
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.fontSize = 100; // set the fallback text style in typographstyle
    typographyStyle0.heightOnly = true;
    typographyStyle0.heightScale = 1;
    OHOS::Rosen::TextStyle textStyle = typographyStyle0.GetTextStyle();
    EXPECT_EQ(textStyle.fontSize, 100);
    EXPECT_EQ(textStyle.heightOnly, true);
    EXPECT_EQ(textStyle.heightScale, 1);
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"text textstyle";
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    double maxWidth = 10000.0;
    typography0->Layout(maxWidth);
    EXPECT_EQ(typography0->GetHeight(), 100);
    std::vector<LineMetrics> myLinesMetric = typography0->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;
    EXPECT_EQ(runMetrics.size(), 1);
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->fontSize, 100);
        EXPECT_EQ(item.second.textStyle->heightScale, 1);
    }
}

/*
 * @tc.name: OH_Drawing_TypographyTest017
 * @tc.desc: test for the actual effective value of textstyle: default textstyle.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest017, TestSize.Level0)
{
    // After setting the default text style in typographstyle, the fallback text style becomes ineffective.
    OHOS::Rosen::TypographyStyle typographyStyle1;
    typographyStyle1.fontSize = 100;
    typographyStyle1.heightOnly = true;
    typographyStyle1.heightScale = 1;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection1 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    OHOS::Rosen::TextStyle textStyle1;
    textStyle1.fontSize = 30;
    textStyle1.heightOnly = true;
    textStyle1.heightScale = 2;
    typographyStyle1.SetTextStyle(textStyle1); // set the default text style in typographstyle
    std::u16string text1 = u"你好测试文本样式";
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate1 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle1, fontCollection1);
    typographyCreate1->AppendText(text1);
    std::unique_ptr<OHOS::Rosen::Typography> typography1 = typographyCreate1->CreateTypography();
    double maxWidth = 10000.0;
    typography1->Layout(maxWidth);
    EXPECT_EQ(typography1->GetHeight(), 60);
    std::vector<LineMetrics> myLinesMetric1 = typography1->GetLineMetrics();
    auto runMetrics1 = myLinesMetric1[0].runMetrics;
    EXPECT_EQ(runMetrics1.size(), 1);
    for (const auto& item : runMetrics1) {
        EXPECT_EQ(item.second.textStyle->fontSize, 30);
        EXPECT_EQ(item.second.textStyle->heightScale, 2);
    }
}

/*
 * @tc.name: OH_Drawing_TypographyTest018
 * @tc.desc: test for the actual effective value of textstyle: pushtext style.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest018, TestSize.Level0)
{
    // After pushing a new text style, the default text style becomes ineffective.
    OHOS::Rosen::TypographyStyle typographyStyle2;
    typographyStyle2.fontSize = 100;
    typographyStyle2.heightOnly = true;
    typographyStyle2.heightScale = 1;
    OHOS::Rosen::TextStyle textStyle2;
    textStyle2.fontSize = 30;
    textStyle2.heightOnly = true;
    textStyle2.heightScale = 2;
    typographyStyle2.SetTextStyle(textStyle2);
    textStyle2.fontSize = 50;
    textStyle2.heightOnly = true;
    textStyle2.heightScale = 3;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection2 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate2 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle2, fontCollection2);
    typographyCreate2->PushStyle(textStyle2); // pushtext style
    std::u16string text2 = u"你好测试 textstyle hello ";
    typographyCreate2->AppendText(text2);
    std::unique_ptr<OHOS::Rosen::Typography> typography2 = typographyCreate2->CreateTypography();
    double maxWidth = 10000.0;
    typography2->Layout(maxWidth);
    EXPECT_EQ(typography2->GetHeight(), 150);
    std::vector<LineMetrics> myLinesMetric2 = typography2->GetLineMetrics();
    auto runMetrics2 = myLinesMetric2[0].runMetrics;
    for (const auto& item : runMetrics2) {
        EXPECT_EQ(item.second.textStyle->fontSize, 50);
        EXPECT_EQ(item.second.textStyle->heightScale, 3);
    }
}

/*
 * @tc.name: OH_Drawing_TypographyTest019
 * @tc.desc: test obtaining some textStyle property from runMetrics.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest019, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = 50;
    textStyle.heightOnly = true;
    textStyle.heightScale = 3;
    textStyle.color = Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0);
    TextShadow myShadow;
    myShadow.color = Drawing::Color::ColorQuadSetARGB(255, 0, 255, 255);
    myShadow.blurRadius = 10;
    Drawing::Point myPoint(10, -10);
    myShadow.offset = myPoint;
    TextShadow myShadowNull;
    textStyle.shadows = { myShadow, myShadow, myShadowNull };
    textStyle.decorationColor = Drawing::Color::ColorQuadSetARGB(255, 0, 255, 0);
    textStyle.decorationStyle = TextDecorationStyle::DASHED;
    textStyle.decorationThicknessScale = 10;
    textStyle.decoration = TextDecoration::OVERLINE;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"测试textstyle的runMetrics: fontSize, heightOnly, heightScale, color, shadows, decoration.";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = 100.0;
    typography->Layout(maxWidth);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;

    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->fontSize, 50);
        EXPECT_EQ(item.second.textStyle->heightOnly, true);
        EXPECT_EQ(item.second.textStyle->heightScale, 3);
        EXPECT_EQ(item.second.textStyle->color.CastToColorQuad(), Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0));
        EXPECT_EQ(item.second.textStyle->shadows.size(), 3);
        EXPECT_EQ(item.second.textStyle->shadows.at(0).color.CastToColorQuad(),
            Drawing::Color::ColorQuadSetARGB(255, 0, 255, 255));
        EXPECT_EQ(item.second.textStyle->shadows.at(0).HasShadow(), true);
        EXPECT_EQ(item.second.textStyle->shadows.at(0).offset.GetX(), 10);
        EXPECT_EQ(item.second.textStyle->shadows.at(0).offset.GetY(), -10);
        EXPECT_EQ(item.second.textStyle->shadows.at(1).blurRadius, 10);
        EXPECT_EQ(item.second.textStyle->shadows.at(2).HasShadow(), false);
        EXPECT_EQ(
            item.second.textStyle->decorationColor.CastToColorQuad(), Drawing::Color::ColorQuadSetARGB(255, 0, 255, 0));
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::DASHED);
        EXPECT_EQ(item.second.textStyle->decorationThicknessScale, 10);
        EXPECT_EQ(item.second.textStyle->decoration, TextDecoration::OVERLINE);
    }
}

/*
 * @tc.name: OH_Drawing_TypographyTest020
 * @tc.desc: test obtaining the some textStyle property from runMetrics.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest020, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    FontFeatures myFontFeatures;
    myFontFeatures.SetFeature("frac", 1);
    myFontFeatures.SetFeature("sups", 1);
    textStyle.fontFeatures = myFontFeatures;
    std::optional<Drawing::Brush> myBrush;
    myBrush->SetARGB(255, 255, 0, 0);
    textStyle.foregroundBrush = myBrush;
    textStyle.backgroundBrush = myBrush;
    std::optional<Drawing::Pen> myPen;
    myPen->SetARGB(255, 0, 255, 255);
    textStyle.foregroundPen = myPen;
    textStyle.backgroundPen = myPen;
    RectStyle myBackgroundRect = { Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0), 10.0, 11.0, 1.0, 2.0 };
    textStyle.backgroundRect = myBackgroundRect;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, textstyle中的runMetrics: fontFeatures, forebackgroundBrushPen, backgroundRect.";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = 100.0;
    typography->Layout(maxWidth);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;

    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->fontFeatures.GetFontFeatures().at(0).first, "frac");
        EXPECT_EQ(item.second.textStyle->fontFeatures.GetFontFeatures().at(0).second, 1);
        EXPECT_EQ(item.second.textStyle->fontFeatures.GetFontFeatures().at(1).first, "sups");
        EXPECT_EQ(item.second.textStyle->fontFeatures.GetFontFeatures().size(), 2);
        EXPECT_EQ(item.second.textStyle->fontFeatures.GetFeatureSettings(), "frac=1,sups=1");
        EXPECT_EQ(item.second.textStyle->fontFeatures.featureSet_.size(), 2);
        EXPECT_EQ(item.second.textStyle->fontFeatures.featureSet_.at(1).second, 1);
        EXPECT_EQ(item.second.textStyle->foregroundBrush, myBrush);
        EXPECT_EQ(item.second.textStyle->backgroundBrush, myBrush);
        EXPECT_EQ(item.second.textStyle->foregroundPen, myPen);
        EXPECT_EQ(item.second.textStyle->backgroundPen, myPen);
        EXPECT_EQ(item.second.textStyle->backgroundRect, myBackgroundRect);
    }
}

/*
 * @tc.name: OH_Drawing_TypographyTest021
 * @tc.desc: test obtaining the remaining textStyle property from runMetrics.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest021, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontWeight = FontWeight::W100;
    textStyle.fontStyle = FontStyle::ITALIC;
    textStyle.baseline = TextBaseline::ALPHABETIC;
    textStyle.fontFamilies = { "Text", "Text2" };
    textStyle.letterSpacing = -10;
    textStyle.wordSpacing = 5;
    textStyle.halfLeading = true;
    textStyle.locale = "zh-Hans";
    textStyle.baseLineShift = 10;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, textstyle中的runMetrics: fontWeight, fontStyle, baseline, fontFamilies, "
                          u"letterSpacing, wordSpacing, halfLeading, locale, baseLineShift.";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = 100.0;
    typography->Layout(maxWidth);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;

    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->fontWeight, FontWeight::W100);
        EXPECT_EQ(item.second.textStyle->fontStyle, FontStyle::ITALIC);
        EXPECT_EQ(item.second.textStyle->baseline, TextBaseline::ALPHABETIC);
        EXPECT_EQ(item.second.textStyle->fontFamilies.size(), 3);

        // The first two are the theme font and the system font, respectively.
        EXPECT_EQ(item.second.textStyle->fontFamilies.at(1), "Text");
        EXPECT_EQ(item.second.textStyle->fontFamilies.at(2), "Text2");
        EXPECT_EQ(item.second.textStyle->letterSpacing, -10);
        EXPECT_EQ(item.second.textStyle->wordSpacing, 5);
        EXPECT_EQ(item.second.textStyle->halfLeading, true);
        EXPECT_EQ(item.second.textStyle->locale, "zh-Hans");
        EXPECT_EQ(item.second.textStyle->baseLineShift, 10);
    }
}

/*
 * @tc.name: OH_Drawing_TypographyTest022
 * @tc.desc: test for multi-run and multi-line paragraph width with autospace
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest022, TestSize.Level0)
{
    OHOS::Rosen::TextStyle style;
    style.fontSize = 40;

    // Init: Create a multi runs and lines paragraph width without autospace. Use its width as baseline.
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    typographyCreate0->PushStyle(style);
    std::u16string text = u"版权所有©2002-2001叉叉公司保留一切权利。";
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    double maxWidth = 1500;
    typography0->Layout(maxWidth);

    // Test the scenario of setting autospace for text containing multiple runs and lines paragraph.
    OHOS::Rosen::TypographyStyle typographyStyle1;
    typographyStyle1.enableAutoSpace = true;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection1 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate1 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle1, fontCollection1);
    typographyCreate1->PushStyle(style);
    typographyCreate1->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography1 = typographyCreate1->CreateTypography();
    typography1->Layout(maxWidth);

    EXPECT_NEAR(typography0->GetLineWidth(0) + style.fontSize / 8 * 3, typography1->GetLineWidth(0), 1e-6f);
    EXPECT_NEAR(typography0->GetLongestLineWithIndent() + style.fontSize / 8 * 3,
        typography1->GetLongestLineWithIndent(), 1e-6f);
}

/*
 * @tc.name: OH_Drawing_TypographyTest023
 * @tc.desc: test for single-run and one-line paragraph width with autospace
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest023, TestSize.Level0)
{
    OHOS::Rosen::TextStyle style;
    style.fontSize = 40;

    // Init: Create a single-run and one-line paragraph width without autospace. Use its width as baseline.
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate0 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    typographyCreate0->PushStyle(style);
    std::u16string text = u"SingRun©2002-2001";
    typographyCreate0->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate0->CreateTypography();
    double maxWidth = 1500;
    typography0->Layout(maxWidth);

    // Test the scenario of setting autospace for text containing multiple runs and lines paragraph.
    OHOS::Rosen::TypographyStyle typographyStyle1;
    typographyStyle1.enableAutoSpace = true;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection1 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate1 =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle1, fontCollection1);
    typographyCreate1->PushStyle(style);
    typographyCreate1->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography1 = typographyCreate1->CreateTypography();
    typography1->Layout(maxWidth);

    EXPECT_NEAR(typography0->GetLineWidth(0) + style.fontSize / 8 * 2, typography1->GetLineWidth(0), 1e-6f);
    EXPECT_NEAR(typography0->GetLongestLineWithIndent() + style.fontSize / 8 * 2,
        typography1->GetLongestLineWithIndent(), 1e-6f);
}

/*
 * @tc.name: OH_Drawing_TypographyTest024
 * @tc.desc: test for text effect
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest024, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    std::u16string text = u"test effect.";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    double maxWidth = 100;
    typography->Layout(maxWidth);
    OHOS::Rosen::Drawing::Canvas canvas;
    typography->Paint(&canvas, 0, 0);

    std::unique_ptr<SPText::Paragraph> paragraphTemp = nullptr;
    AdapterTxt::Typography* typographyImpl = static_cast<AdapterTxt::Typography*>(typography.get());
    typographyImpl->paragraph_.swap(paragraphTemp);
    EXPECT_EQ(typography->GetTextBlobRecordInfo().size(), 0);
    auto animationFunc = typographyImpl->GetAnimation();
    EXPECT_EQ(animationFunc, nullptr);
    typography->SetSkipTextBlobDrawing(true);
    EXPECT_FALSE(typography->HasSkipTextBlobDrawing());

    typographyImpl->paragraph_.swap(paragraphTemp);
    EXPECT_NE(typography->GetTextBlobRecordInfo().size(), 0);
    typography->SetSkipTextBlobDrawing(true);
    EXPECT_TRUE(typography->HasSkipTextBlobDrawing());
    typography->SetTextEffectAssociation(true);
    EXPECT_TRUE(typography->GetTextEffectAssociation());
}
} // namespace Rosen
} // namespace OHOS