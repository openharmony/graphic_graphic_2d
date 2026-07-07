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

#include "font_collection.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "paragraph.h"
#include "src/ParagraphImpl.h"
#include "typography.h"
#include "typography_create.h"
#include "font_collection.h"
#include "txt/text_bundle_config_parser.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const double ARC_FONT_SIZE = 28;
const double DEFAULT_FONT_SIZE = 40;
const size_t DEFAULT_MAXLINES = 2;
const double DEFAULT_MAX_WIDTHS = 100;

// Helper function to convert Typography to Skia ParagraphImpl
static skia::textlayout::ParagraphImpl* GetSkiaParagraph(Typography* typography)
{
    if (typography == nullptr) {
        return nullptr;
    }
    SPText::ParagraphImpl* paragraphImpl =
        static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    if (paragraphImpl == nullptr) {
        return nullptr;
    }
    return static_cast<skia::textlayout::ParagraphImpl*>(paragraphImpl->paragraph_.get());
}

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
    EXPECT_FALSE(typography1->CanPaintAllText());
    typography1->Layout(500);
    EXPECT_TRUE(typography1->CanPaintAllText());

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
    // Expect rect left 0, top 7, right 54, bottom 401
    ASSERT_EQ(paintRegion, Drawing::RectI(0, 7, 54, 401));
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
    EXPECT_FALSE(typography->CanPaintAllText());

    typographyImpl->paragraph_.swap(paragraphTemp);
    EXPECT_NE(typography->GetTextBlobRecordInfo().size(), 0);
    typography->SetSkipTextBlobDrawing(true);
    EXPECT_TRUE(typography->HasSkipTextBlobDrawing());
    typography->SetTextEffectAssociation(true);
    EXPECT_TRUE(typography->GetTextEffectAssociation());
}

/*
 * @tc.name: TypographyFontTypefacesSetSingleTypefaceTest
 * @tc.desc: Test setting a single fontTypeface
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesSetSingleTypefaceTest, TestSize.Level0)
{
    TextStyle textStyle;
    auto typeface = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    ASSERT_NE(typeface, nullptr);
    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface);
    textStyle.SetFontTypefaces(typefaces);
    auto result = textStyle.GetFontTypefaces();
    EXPECT_EQ(result.size(), 1);
    EXPECT_NE(result[0], nullptr);
}

/*
 * @tc.name: TypographyFontTypefacesSetMultipleTypefacesTest
 * @tc.desc: Test setting multiple fontTypefaces
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesSetMultipleTypefacesTest, TestSize.Level0)
{
    TextStyle textStyle;
    auto typeface1 = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    ASSERT_NE(typeface1, nullptr);
    auto typeface2 = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSansCJK-Regular.ttc");
    ASSERT_NE(typeface2, nullptr);

    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface1);
    typefaces.push_back(typeface2);

    textStyle.SetFontTypefaces(typefaces);

    auto result = textStyle.GetFontTypefaces();
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], typeface1);
    EXPECT_EQ(result[1], typeface2);
}

/*
 * @tc.name: TypographyFontTypefacesCppSetEmptyArrayTest
 * @tc.desc: Test setting empty array to clear priority setting
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesCppSetEmptyArrayTest, TestSize.Level0)
{
    TextStyle textStyle;
    auto typeface = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    ASSERT_NE(typeface, nullptr);

    // First set a typeface
    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface);
    textStyle.SetFontTypefaces(typefaces);

    auto result1 = textStyle.GetFontTypefaces();
    EXPECT_EQ(result1.size(), 1);

    // Then clear with empty array
    std::vector<std::shared_ptr<Drawing::Typeface>> emptyTypefaces;
    textStyle.SetFontTypefaces(emptyTypefaces);

    auto result2 = textStyle.GetFontTypefaces();
    EXPECT_EQ(result2.size(), 0);
}

/*
 * @tc.name: TypographyFontTypefacesLayoutAllSupportedTest
 * @tc.desc: Test layout with all characters supported by priority font
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesLayoutAllSupportedTest, TestSize.Level0)
{
    auto typeface = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    ASSERT_NE(typeface, nullptr);

    TypographyStyle typographyStyle;
    std::shared_ptr<FontCollection> fontCollection =
        FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection, nullptr);

    auto typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 40;

    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface);
    textStyle.SetFontTypefaces(typefaces);

    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(u"Hello World");
    auto typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);

    typography->Layout(500);

    auto* skParagraph = GetSkiaParagraph(typography.get());
    ASSERT_NE(skParagraph, nullptr);

    EXPECT_EQ(skParagraph->fRuns.size(), 1);
    for (const auto& run : skParagraph->fRuns) {
        EXPECT_EQ(run.font().GetTypeface()->GetFamilyName(), "Noto Sans");
    }
}

/*
 * @tc.name: TypographyFontTypefacesLayoutFirstFallbackTest
 * @tc.desc: Test layout with first typeface failing, using second
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesLayoutFirstFallbackTest, TestSize.Level0)
{
    auto typeface1 = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    ASSERT_NE(typeface1, nullptr);
    auto typeface2 = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSansCJK-Regular.ttc");
    ASSERT_NE(typeface2, nullptr);

    TypographyStyle typographyStyle;
    std::shared_ptr<FontCollection> fontCollection =
        FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection, nullptr);

    auto typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 40;

    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface1);
    typefaces.push_back(typeface2);
    textStyle.SetFontTypefaces(typefaces);

    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(u"Hello世界");
    auto typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);

    typography->Layout(500);

    auto* skParagraph = GetSkiaParagraph(typography.get());
    ASSERT_NE(skParagraph, nullptr);

    EXPECT_EQ(skParagraph->fRuns.size(), 2);
    EXPECT_EQ(skParagraph->fRuns[0].font().GetTypeface()->GetFamilyName(), "Noto Sans");
    EXPECT_EQ(skParagraph->fRuns[1].font().GetTypeface()->GetFamilyName(), "Noto Sans CJK JP");
}

/*
 * @tc.name: TypographyFontTypefacesLayoutFirstFallbackTest
 * @tc.desc: Test layout with first typeface failing, using fallback
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesLayoutFirstAndFallbackTest, TestSize.Level0)
{
    auto typeface1 = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    ASSERT_NE(typeface1, nullptr);

    TypographyStyle typographyStyle;
    std::shared_ptr<FontCollection> fontCollection =
        FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection, nullptr);

    auto typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 40;

    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface1);
    textStyle.SetFontTypefaces(typefaces);

    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(u"Hello世界");
    auto typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);

    typography->Layout(500);

    auto* skParagraph = GetSkiaParagraph(typography.get());
    ASSERT_NE(skParagraph, nullptr);

    EXPECT_EQ(skParagraph->fRuns.size(), 2);
    EXPECT_EQ(skParagraph->fRuns[0].font().GetTypeface()->GetFamilyName(), "Noto Sans");
    EXPECT_EQ(skParagraph->fRuns[1].font().GetTypeface()->GetFamilyName(), "HarmonyOS Sans SC");
}

/*
 * @tc.name: TypographyFontTypefacesLayoutPriorityTest
 * @tc.desc: Test layout with fontFamilies and typefaces, using typefaces
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesLayoutPriorityTest, TestSize.Level0)
{
    auto typeface = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSansCJK-Regular.ttc");
    ASSERT_NE(typeface, nullptr);

    TypographyStyle typographyStyle;
    std::shared_ptr<FontCollection> fontCollection =
        FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection, nullptr);

    auto typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    TextStyle textStyle;
    textStyle.fontSize = 40;
    textStyle.fontFamilies.push_back("HarmonyOS Sans SC");

    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface);
    textStyle.SetFontTypefaces(typefaces);

    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(u"你好世界");
    auto typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);

    typography->Layout(500);

    auto* skParagraph = GetSkiaParagraph(typography.get());
    ASSERT_NE(skParagraph, nullptr);

    EXPECT_EQ(skParagraph->fRuns.size(), 1);
    EXPECT_EQ(skParagraph->fRuns[0].font().GetTypeface()->GetFamilyName(), "Noto Sans CJK JP");
}

/*
 * @tc.name: TypographyFontTypefacesLayoutNoTypefacesTest
 * @tc.desc: Test layout without fontTypefaces set, using default font matching
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesLayoutNoTypefacesTest, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    std::shared_ptr<FontCollection> fontCollection =
        FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection, nullptr);

    auto typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    // Don't set fontTypefaces, use default behavior
    TextStyle textStyle;
    textStyle.fontSize = 40;

    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(u"Hello World");
    auto typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);

    typography->Layout(500);

    auto* skParagraph = GetSkiaParagraph(typography.get());
    ASSERT_NE(skParagraph, nullptr);

    EXPECT_EQ(skParagraph->fRuns.size(), 1);
    EXPECT_EQ(skParagraph->fRuns[0].font().GetTypeface()->GetFamilyName(), "HarmonyOS-Sans");
}

/*
 * @tc.name: TypographyFontTypefacesLayoutLineMetricsTest
 * @tc.desc: Test for fontTypefaces from runMetrics's textStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyFontTypefacesLayoutLineMetricsTest, TestSize.Level0)
{
    TypographyStyle typographyStyle;
    std::shared_ptr<FontCollection> fontCollection =
        FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection, nullptr);

    auto typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);

    TextStyle style;
    style.fontSize = 40;
    auto typeface = Drawing::Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    ASSERT_NE(typeface, nullptr);

    std::vector<std::shared_ptr<Drawing::Typeface>> typefaces;
    typefaces.push_back(typeface);
    style.SetFontTypefaces(typefaces);
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(u"你好世界");
    auto paragraph = typographyCreate->CreateTypography();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);
    LineMetrics lineMetrics;
    paragraph->GetLineMetricsAt(0, &lineMetrics);
    for (const auto& metrics : lineMetrics.runMetrics) {
        const auto& runMetricsStyle = metrics.second.textStyle;
        auto typefaces = runMetricsStyle->GetFontTypefaces();
        EXPECT_EQ(typefaces.size(), 1);
        for (const auto& typeface : typefaces) {
            EXPECT_EQ(typeface->GetFamilyName(), "Noto Sans");
        }
    }
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText001
 * @tc.desc: test for split run in rtl language situation
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TypographyStyle typographyDefaultStyle;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyDefaultCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyDefaultStyle, fontCollection);
    ASSERT_NE(typographyDefaultCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 11 for rtl situation
    style.fontSize = 11;
    std::u16string text = u"ولكنمعانهيارالعلاقاتبينالصينوروسيا،سقطتفي";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    // Special layout width 20 for rtl situation
    double maxWidth = 20;
    typography->Layout(maxWidth);
    typographyDefaultCreate->PushStyle(style);
    typographyDefaultCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> defaultTypography = typographyDefaultCreate->CreateTypography();
    ASSERT_NE(defaultTypography, nullptr);
    defaultTypography->Layout(maxWidth);

    SPText::ParagraphImpl* defaultParagraph = static_cast<SPText::ParagraphImpl*>(defaultTypography->GetParagraph());
    ASSERT_NE(defaultParagraph, nullptr);
    auto defaultRuns = static_cast<skia::textlayout::ParagraphImpl*>(defaultParagraph->paragraph_.get())->runs();
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    EXPECT_EQ(defaultRuns.size(), 1);
    EXPECT_EQ(runs.size(), paragraph->GetLineCount());
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText002
 * @tc.desc: test for split run in Myanmese language situation
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText002, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TypographyStyle typographyDefaultStyle;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyDefaultCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyDefaultStyle, fontCollection);
    ASSERT_NE(typographyDefaultCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 11 for Myanmese language situation
    style.fontSize = 11;
    std::u16string text =
        u"ဘယ်တော့မှပြန်မသွားရန်မြင့်မားသောအခ\
        န်းများရှိတောက်ပသောမှန်များတွင်မည်မျှချစ်စ\
        ရာကောင်းသောသော့ခလောက်များကိုကြည့်ပါ။.";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    // Special layout width 20 for Myanmese language situation
    double maxWidth = 20;
    typography->Layout(maxWidth);
    typographyDefaultCreate->PushStyle(style);
    typographyDefaultCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> defaultTypography = typographyDefaultCreate->CreateTypography();
    ASSERT_NE(defaultTypography, nullptr);
    defaultTypography->Layout(maxWidth);

    SPText::ParagraphImpl* defaultParagraph = static_cast<SPText::ParagraphImpl*>(defaultTypography->GetParagraph());
    ASSERT_NE(defaultParagraph, nullptr);
    auto defaultRuns = static_cast<skia::textlayout::ParagraphImpl*>(defaultParagraph->paragraph_.get())->runs();
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    // Expect run count is 6 before split runs
    EXPECT_EQ(defaultRuns.size(), 6);
    // Expect run count is 44 after split runs
    EXPECT_EQ(runs.size(), 44);
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText003
 * @tc.desc: test for split run in New Thai language situation
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText003, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TypographyStyle typographyDefaultStyle;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyDefaultCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyDefaultStyle, fontCollection);
    ASSERT_NE(typographyDefaultCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 11 for New Thai language situation
    style.fontSize = 11;
    // Run's clusterIndexes: [0, 3, 3, 6, 9...]
    std::u16string text = u"மொழிகளில்";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    // Special layout width 20 for New Thai language situation
    double maxWidth = 20;
    typography->Layout(maxWidth);
    typographyDefaultCreate->PushStyle(style);
    typographyDefaultCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> defaultTypography = typographyDefaultCreate->CreateTypography();
    ASSERT_NE(defaultTypography, nullptr);
    defaultTypography->Layout(maxWidth);

    SPText::ParagraphImpl* defaultParagraph = static_cast<SPText::ParagraphImpl*>(defaultTypography->GetParagraph());
    ASSERT_NE(defaultParagraph, nullptr);
    auto defaultRuns = static_cast<skia::textlayout::ParagraphImpl*>(defaultParagraph->paragraph_.get())->runs();
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    EXPECT_EQ(defaultRuns.size(), 1);
    EXPECT_EQ(runs.size(), paragraph->GetLineCount());
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText004
 * @tc.desc: test for split run in hard break symbol situation
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText004, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TypographyStyle typographyDefaultStyle;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyDefaultCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyDefaultStyle, fontCollection);
    ASSERT_NE(typographyDefaultCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 11 for hard break situation
    style.fontSize = 11;
    // Symbol < and hardbreak will shape to one run
    std::u16string text = u"<\n";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    // Special layout width 20 for hard break situation
    double maxWidth = 20;
    typography->Layout(maxWidth);
    typographyDefaultCreate->PushStyle(style);
    typographyDefaultCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> defaultTypography = typographyDefaultCreate->CreateTypography();
    ASSERT_NE(defaultTypography, nullptr);
    defaultTypography->Layout(maxWidth);

    SPText::ParagraphImpl* defaultParagraph = static_cast<SPText::ParagraphImpl*>(defaultTypography->GetParagraph());
    ASSERT_NE(defaultParagraph, nullptr);
    auto defaultRuns = static_cast<skia::textlayout::ParagraphImpl*>(defaultParagraph->paragraph_.get())->runs();
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    EXPECT_EQ(defaultRuns.size(), 1);
    EXPECT_EQ(runs.size(), paragraph->GetLineCount());
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText005
 * @tc.desc: test for split run in narmal English characters situation
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText005, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TypographyStyle typographyDefaultStyle;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyDefaultCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyDefaultStyle, fontCollection);
    ASSERT_NE(typographyDefaultCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 11 for normal English characters situation
    style.fontSize = 11;
    std::u16string text = u"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    // Special layout width 20 for normal English characters situation
    double maxWidth = 20;
    typography->Layout(maxWidth);
    typographyDefaultCreate->PushStyle(style);
    typographyDefaultCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> defaultTypography = typographyDefaultCreate->CreateTypography();
    ASSERT_NE(defaultTypography, nullptr);
    defaultTypography->Layout(maxWidth);

    SPText::ParagraphImpl* defaultParagraph = static_cast<SPText::ParagraphImpl*>(defaultTypography->GetParagraph());
    ASSERT_NE(defaultParagraph, nullptr);
    auto defaultRuns = static_cast<skia::textlayout::ParagraphImpl*>(defaultParagraph->paragraph_.get())->runs();
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    EXPECT_EQ(defaultRuns.size(), 1);
    EXPECT_EQ(runs.size(), paragraph->GetLineCount());
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText006
 * @tc.desc: test for split run in special characters situation, no crash
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText006, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    typographyStyle.ellipsis = u"...";
    typographyStyle.ellipsisModal = EllipsisModal::TAIL;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 100 for normal English characters situation
    style.fontSize = 100;
    std::u16string text =
        u"\u{000a}\u{0604}\u{514d}\u{ff09}\u{04b0}\u{0035}\u{00cd}\u{00e0}\u{ff1b}\u{04c3}\u{5621}\u{6233}\u{0f50}"
        u"\u{d610}\u{00f3}\u{000d}\u{00d9}\u{00e9}\u{04eb}\u{cfb5}\u{00c1}\u{007b}\u{5756}\u{4fa0}\u{559d}\u{5311}"
        u"\u{00bf}\u{0032}\u{51aa}\u{ff1f}\u{0009}\u{60eb}\u{06a8}\u{00ca}\u{3040}\u{0038}\u{30df}\u{0fb1}\u{523a}"
        u"\u{0653}\u{2026}\u{acb0}\u{0f06}\u{61d6}\u{4e63}\u{3047}\u{5c8c}\u{50c4}\u{00c8}\u{3002}\u{5ac6}\u{00d2}"
        u"\u{00a1}\u{5e46}\u{0410}\u{ff0c}\u{0038}\u{00e8}\u{00c1}\u{0693}\u{cd15}\u{00e9}\u{000a}\u{304a}\u{04d3}"
        u"\u{5695}\u{ff08}\u{0037}\u{047b}\u{518e}\u{3083}\u{005c}\u{0fbd}\u{06af}\u{59fd}\u{3001}\u{0f86}\u{5c8b}"
        u"\u{5911}\u{5354}\u{0033}\u{0657}\u{ff1f}\u{0036}\u{5458}\u{00c2}\u{309f}\u{00ca}\u{00e0}\u{0032}\u{0041}"
        u"\u{00cd}\u{00ea}\u{ae94}\u{5625}\u{0032}\u{00f4}\u{00eb}\u{0038}\u{000a}";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->LayoutWithConstraints({340, 198});
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    size_t expectRunSize = 70;
    EXPECT_EQ(runs.size(), expectRunSize);
    size_t expectLineSize = 1;
    EXPECT_EQ(paragraph->GetLineCount(), expectLineSize);
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText007
 * @tc.desc: test for split run in balanced strategy with special characters, no crash
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText007, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    typographyStyle.breakStrategy = BreakStrategy::BALANCED;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 76 for normal English characters situation
    style.fontSize = 76;
    style.letterSpacing = 25;
    style.fontStyle = FontStyle::ITALIC;
    std::u16string text = u"\u{0033}\u{0031}\u{007d}\u{007d}\u{007d}\u{5fa3}\u{000d}\u{007d}";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(141.328);
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    size_t expectRunSize = 6;
    EXPECT_EQ(runs.size(), expectRunSize);
    size_t expectLineSize = 5;
    EXPECT_EQ(paragraph->GetLineCount(), expectLineSize);
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsText008
 * @tc.desc: test for split run in balanced strategy with special characters, no crash
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographySplitRunsText008, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    typographyStyle.breakStrategy = BreakStrategy::BALANCED;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 76 for normal English characters situation
    style.fontSize = 76;
    style.letterSpacing = 25;
    style.fontStyle = FontStyle::ITALIC;
    std::u16string text = u"\u{0033}\u{0031}\u{007d}\u{007d}\u{007d}\u{5fa3}\u{000d}\u{007d}";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(39.5);
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto runs = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->runs();
    size_t expectRunSize = 8;
    EXPECT_EQ(runs.size(), expectRunSize);
    size_t expectLineSize = 7;
    EXPECT_EQ(paragraph->GetLineCount(), expectLineSize);
}

/*
 * @tc.name: OH_Drawing_TypographyRtlClusterIndexOffset001
 * @tc.desc: test for rtl's text adjusting textRange
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyRtlClusterIndexOffset001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.verticalAlignment = TextVerticalAlign::CENTER;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    // Special font size 11 for normal English characters situation
    style.fontSize = 11;
    std::u16string text = u"لآؗۘئ";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    // Special layout width 10 for rtl situation
    double maxWidth = 10;
    typography->Layout(maxWidth);
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph =
        static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get());
    ASSERT_NE(skiaParagraph, nullptr);
    // The byte value of لآؗۘئ is 11
    EXPECT_EQ(skiaParagraph->fClustersIndexFromCodeUnit.size(), 11);
    EXPECT_EQ(skiaParagraph->fClustersIndexFromCodeUnit[0], 0);
    EXPECT_EQ(skiaParagraph->fClustersIndexFromCodeUnit[1], 0);
}

/*
 * @tc.name: TypographyStyleEllipsisTest01
 * @tc.desc: test for MULTILINE_HEAD ellipsisModal in normal situation.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyStyleEllipsisTest01, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    typographyStyle.ellipsis = u"...";
    typographyStyle.ellipsisModal = OHOS::Rosen::EllipsisModal::MULTILINE_HEAD;
    typographyStyle.maxLines = DEFAULT_MAXLINES;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, paragraphyStyle中的ellipsis and ellipsisModal属性。";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = DEFAULT_MAX_WIDTHS;
    typography->Layout(maxWidth);
    std::vector<std::shared_ptr<TextLineBase>> textLine = typography->GetTextLines();
    EXPECT_EQ(typography->GetLineCount(), typographyStyle.maxLines);
    EXPECT_LT(typography->GetLineWidth(typographyStyle.maxLines - 1), maxWidth);
}

/*
 * @tc.name: TypographyStyleEllipsisTest02
 * @tc.desc: test for MULTILINE_MIDDLE ellipsisModal in normal situation.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyStyleEllipsisTest02, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    typographyStyle.ellipsis = u"...";
    typographyStyle.ellipsisModal = OHOS::Rosen::EllipsisModal::MULTILINE_MIDDLE;
    typographyStyle.maxLines = DEFAULT_MAXLINES;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, paragraphyStyle中的ellipsis and ellipsisModal属性。";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = DEFAULT_MAX_WIDTHS;
    typography->Layout(maxWidth);
    std::vector<std::shared_ptr<TextLineBase>> textLine = typography->GetTextLines();
    EXPECT_EQ(typography->GetLineCount(), typographyStyle.maxLines);
    EXPECT_LT(typography->GetLineWidth(typographyStyle.maxLines - 1), maxWidth);
}

/*
 * @tc.name: TypographyStyleEllipsisTest03
 * @tc.desc: test for MULTILINE_MIDDLE ellipsisModal BALANCED breakStrategy situation.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyStyleEllipsisTest03, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    typographyStyle.ellipsis = u"...";
    typographyStyle.ellipsisModal = OHOS::Rosen::EllipsisModal::MULTILINE_HEAD;
    typographyStyle.maxLines = DEFAULT_MAXLINES;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, paragraphyStyle中的ellipsis and ellipsisModal属性。";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = DEFAULT_MAX_WIDTHS;
    typography->Layout(maxWidth);
    std::vector<std::shared_ptr<TextLineBase>> textLine = typography->GetTextLines();
    EXPECT_EQ(typography->GetLineCount(), typographyStyle.maxLines);
    EXPECT_LT(typography->GetLineWidth(typographyStyle.maxLines - 1), maxWidth);
}

/*
 * @tc.name: TypographyStyleEllipsisTest04
 * @tc.desc: test for MULTILINE_MIDDLE ellipsisModal in BALANCED breakStrategy situation.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyStyleEllipsisTest04, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    typographyStyle.ellipsis = u"...";
    typographyStyle.ellipsisModal = OHOS::Rosen::EllipsisModal::MULTILINE_MIDDLE;
    typographyStyle.breakStrategy = OHOS::Rosen::BreakStrategy::BALANCED;
    typographyStyle.maxLines = DEFAULT_MAXLINES;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, paragraphyStyle中的ellipsis and ellipsisModal属性。";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = DEFAULT_MAX_WIDTHS;
    typography->Layout(maxWidth);
    std::vector<std::shared_ptr<TextLineBase>> textLine = typography->GetTextLines();
    EXPECT_EQ(typography->GetLineCount(), typographyStyle.maxLines);
    EXPECT_LT(typography->GetLineWidth(typographyStyle.maxLines - 1), maxWidth);
}

/*
 * @tc.name: TypographyStyleEllipsisTest05
 * @tc.desc: test for MULTILINE_MIDDLE ellipsisModal: BALANCED breakStrategy, ellipsis is ###.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyStyleEllipsisTest05, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    typographyStyle.ellipsis = u"###";
    typographyStyle.ellipsisModal = OHOS::Rosen::EllipsisModal::MULTILINE_HEAD;
    typographyStyle.maxLines = DEFAULT_MAXLINES;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, paragraphyStyle中的ellipsis and ellipsisModal属性。";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = DEFAULT_MAX_WIDTHS;
    typography->Layout(maxWidth);
    std::vector<std::shared_ptr<TextLineBase>> textLine = typography->GetTextLines();
    EXPECT_EQ(typography->GetLineCount(), typographyStyle.maxLines);
    EXPECT_LT(typography->GetLineWidth(typographyStyle.maxLines - 1), maxWidth);
}

/*
 * @tc.name: TypographyStyleEllipsisTest06
 * @tc.desc: test for MULTILINE_MIDDLE ellipsisModal: BALANCED breakStrategy, ellipsis is ###.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyStyleEllipsisTest06, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    typographyStyle.ellipsis = u"###";
    typographyStyle.ellipsisModal = OHOS::Rosen::EllipsisModal::MULTILINE_MIDDLE;
    typographyStyle.breakStrategy = OHOS::Rosen::BreakStrategy::BALANCED;
    typographyStyle.maxLines = DEFAULT_MAXLINES;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好, 测试, paragraphyStyle中的ellipsis and ellipsisModal属性。";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double maxWidth = DEFAULT_MAX_WIDTHS;
    typography->Layout(maxWidth);
    std::vector<std::shared_ptr<TextLineBase>> textLine = typography->GetTextLines();
    EXPECT_EQ(typography->GetLineCount(), typographyStyle.maxLines);
    EXPECT_LT(typography->GetLineWidth(typographyStyle.maxLines - 1), maxWidth);
}

namespace {
class BundleApiVersionGuard {
public:
    explicit BundleApiVersionGuard(uint32_t version)
    {
        prevInitStatus_ = SPText::TextBundleConfigParser::GetInstance().initStatus_;
        prevApiVersion_ = SPText::TextBundleConfigParser::GetInstance().bundleApiVersion_;
        SPText::TextBundleConfigParser::GetInstance().initStatus_ = true;
        SPText::TextBundleConfigParser::GetInstance().bundleApiVersion_ = version;
    }
    ~BundleApiVersionGuard()
    {
        SPText::TextBundleConfigParser::GetInstance().initStatus_ = prevInitStatus_;
        SPText::TextBundleConfigParser::GetInstance().bundleApiVersion_ = prevApiVersion_;
    }
private:
    bool prevInitStatus_{false};
    uint32_t prevApiVersion_{0};
};
} // namespace

/*
 * @tc.name: TypographyControlCharLetterSpacingTest001
 * @tc.desc: Test that ZWJ (U+200B) does not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest001, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.letterSpacing = 10;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // Layout "你\u200B好" (with ZWJ) and "你好" (without ZWJ), widths should be the same
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    textStyle.fontSize = 40;
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"你\u200B好");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"你好");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest002
 * @tc.desc: Test that LRM (U+200E) does not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest002, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.letterSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // Layout "Hello\u200E世界" (with LRM) and "Hello世界" (without LRM)
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"Hello\u200E世界");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"Hello世界");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest003
 * @tc.desc: Test that text without control characters has unchanged behavior
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest003, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.letterSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builder =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"Hello World");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(10000);
    // "Hello World" has 11 chars, letterSpacing applies to 10 gaps = 10 * 10 = 100
    EXPECT_DOUBLE_EQ(typography->GetLongestLineWithIndent(), 323.51983642578125);
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest004
 * @tc.desc: Test that multiple consecutive control characters do not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest004, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.letterSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // Layout "A‍‍‌B" (multiple control chars) and "AB"
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"A\u200D\u200BB");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"AB");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest005
 * @tc.desc: Test that multi-style paragraphs with control characters skip letterSpacing correctly
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest005, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());

    OHOS::Rosen::TextStyle style1;
    style1.fontSize = 40;
    style1.letterSpacing = 10;
    style1.fontWeight = FontWeight::W700;

    OHOS::Rosen::TextStyle style2;
    style2.fontSize = 40;
    style2.letterSpacing = 15;
    style2.fontStyle = FontStyle::ITALIC;

    // "A‍B" with two different styles (multi-run)
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(style1);
    builderWithCtrl->AppendText(u"A\u200D");
    builderWithCtrl->PushStyle(style2);
    builderWithCtrl->AppendText(u"B");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    // "AB" with two different styles (multi-run, no control chars)
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(style1);
    builderNoCtrl->AppendText(u"A");
    builderNoCtrl->PushStyle(style2);
    builderNoCtrl->AppendText(u"B");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest006
 * @tc.desc: Test text with only control characters has zero letterSpacing contribution
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest006, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.letterSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());

    // Text with only control chars: "‍‌"
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builder =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"\u200D\u200B");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(10000);
    // Only control characters, no letterSpacing should be triggered
    auto skiaParagraph = GetSkiaParagraph(typography.get());
    ASSERT_NE(skiaParagraph, nullptr);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typography->GetLongestLineWithIndent(), 0));
}

void generateGeneralCategoryZeroWidthControlCharacters(std::vector<char16_t>& controlChars)
{
    // C0 & Zero Width: U+0001–U+0008;
    for (char16_t c = 0x0001; c <= 0x0008; c++) {
        controlChars.push_back(c);
    }
    // C0 & Zero Width: U+000E–U+001C
    for (char16_t c = 0x000D; c <= 0x001C; c++) {
        controlChars.push_back(c);
    }
    // C0 & Zero Width: U+001D–U+001F
    for (char16_t c = 0x001D; c <= 0x001F; c++) {
        controlChars.push_back(c);
    }
}

void generateGeneralCategoryNonZeroWidthControlCharacters(std::vector<char16_t>& controlChars)
{
    // C0 & Non zero With: U+0009;
    controlChars.push_back(0x0009);
}

void generateBreakGeneralCategoryNonZeroWidthControlCharacters(std::vector<char16_t>& controlChars)
{
    // C0 & Non zero Width & Break: U+000A–U+000C;
    for (char16_t c = 0x000A; c <= 0x000C; c++) {
        controlChars.push_back(c);
    }
}

void generateDelGeneralCategoryZeroWidthControlCharacters(std::vector<char16_t>& controlChars)
{
    // DEL + C1: U+007F–U+009F
    for (char16_t c = 0x007F; c <= 0x009F; c++) {
        controlChars.push_back(c);
    }
}


void generateFormatGeneralCategoryZeroWidthControlCharacters(std::vector<char16_t>& controlChars)
{
    // Format: U+200B–U+200F
    for (char16_t c = 0x200B; c <= 0x200F; c++) {
        controlChars.push_back(c);
    }
}

void generateBidiGeneralCategoryZeroWidthControlCharacters(std::vector<char16_t>& controlChars)
{
    // Bidi: U+202A–U+202E
    for (char16_t c = 0x202A; c <= 0x202E; c++) {
        controlChars.push_back(c);
    }
}

float getLayoutWidth(const std::u16string& text, float letterSpacing = 0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    OHOS::Rosen::TextStyle style;
    // Font size 40
    style.fontSize = 40;
    style.letterSpacing = letterSpacing;
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(style);
    builder->AppendText(text);
    auto typo = builder->CreateTypography();
    // Layout width 10000
    typo->Layout(10000);
    return typo->GetLongestLineWithIndent();
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest007
 * @tc.desc: Test c0 zero width kControl characters do not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest007, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A";
        text += ctrl;
        text += u"B";
        // "A<ctrl>B" has 2 visible chars → if control char is skipped, gapCount = 2
        double widthDelta = getLayoutWidth(text, 10) - getLayoutWidth(text);
        EXPECT_DOUBLE_EQ(widthDelta, 20)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 20;
    }
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest008
 * @tc.desc: Test c0 non zero width kControl characters do not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest008, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateGeneralCategoryNonZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A";
        text += ctrl;
        text += u"B";
        // "A<ctrl>B" has 3 visible chars → if control char is skipped, gapCount = 2
        double widthDelta = getLayoutWidth(text, 10) - getLayoutWidth(text);
        EXPECT_DOUBLE_EQ(widthDelta, 30)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 30;
    }
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest009
 * @tc.desc: Test break c0 zero width kControl characters do not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest009, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateBreakGeneralCategoryNonZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A";
        text += ctrl;
        text += u"B";
        // "A<ctrl>B" has 2 visible chars → if control char is skipped, gapCount = 2
        double widthDelta = getLayoutWidth(text, 10) - getLayoutWidth(text);
        EXPECT_DOUBLE_EQ(widthDelta, 10)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 10;
    }
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest010
 * @tc.desc: Test del cc zero width kControl characters do not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest010, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateDelGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A";
        text += ctrl;
        text += u"B";
        // "A<ctrl>B" has 2 visible chars → if control char is skipped, gapCount = 2
        double widthDelta = getLayoutWidth(text, 10) - getLayoutWidth(text);
        EXPECT_DOUBLE_EQ(widthDelta, 20)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 20;
    }
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest011
 * @tc.desc: Test format cf zero width kControl characters do not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest011, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateFormatGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A";
        text += ctrl;
        text += u"B";
        // "A<ctrl>B" has 2 visible chars → if control char is skipped, gapCount = 2
        double widthDelta = getLayoutWidth(text, 10) - getLayoutWidth(text);
        EXPECT_DOUBLE_EQ(widthDelta, 20)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 20;
    }
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest012
 * @tc.desc: Test bidi cf zero width kControl characters do not trigger letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest012, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateBidiGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A";
        text += ctrl;
        text += u"B";
        // "A<ctrl>B" has 2 visible chars → if control char is skipped, gapCount = 2
        double widthDelta = getLayoutWidth(text, 10) - getLayoutWidth(text);
        EXPECT_DOUBLE_EQ(widthDelta, 20)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 20;
    }
}

float getLayoutWidthWithWordSpacing(const std::u16string& text, float wordSpacing = 0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    OHOS::Rosen::TextStyle style;
    // Font size 40
    style.fontSize = 40;
    style.wordSpacing = wordSpacing;
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(style);
    builder->AppendText(text);
    auto typo = builder->CreateTypography();
    // Layout width 10000
    typo->Layout(10000);
    return typo->GetLongestLineWithIndent();
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest001
 * @tc.desc: Test that ZWSP (U+200B) between spaces does not trigger extra wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest001, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.wordSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // Layout "Hello \u200B World" (ZWSP between spaces) and "Hello  World" (two spaces),
    // widths should be the same when control char does not break consecutive whitespace
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"Hello \u200B World");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"Hello  World");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest002
 * @tc.desc: Test that LRM (U+200E) between spaces does not trigger extra wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest002, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.wordSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // Layout "Hello \u200E World" (LRM between spaces) and "Hello  World" (two spaces)
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"Hello \u200E World");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"Hello  World");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest003
 * @tc.desc: Test that text without control characters has unchanged wordSpacing behavior
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest003, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    // "Hello World" has 1 word boundary, wordSpacing applies to 1 gap = 1 * 10 = 10
    std::u16string text = u"Hello World";
    double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
    EXPECT_DOUBLE_EQ(widthDelta, 10);
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest004
 * @tc.desc: Test that multiple consecutive control characters do not trigger wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest004, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.wordSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // Layout "A \u200D\u200B B" (multiple control chars between spaces) and "A  B"
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"A \u200D\u200B B");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"A  B");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest005
 * @tc.desc: Test that multi-style paragraphs with control characters skip wordSpacing correctly
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest005, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());

    OHOS::Rosen::TextStyle style1;
    style1.fontSize = 40;
    style1.wordSpacing = 10;
    style1.fontWeight = FontWeight::W700;

    OHOS::Rosen::TextStyle style2;
    style2.fontSize = 40;
    style2.wordSpacing = 15;
    style2.fontStyle = FontStyle::ITALIC;

    // "A \u200D B" with two different styles (multi-run, control char between spaces)
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(style1);
    builderWithCtrl->AppendText(u"A \u200D");
    builderWithCtrl->PushStyle(style2);
    builderWithCtrl->AppendText(u" B");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    // "A  B" with two different styles (multi-run, no control chars)
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(style1);
    builderNoCtrl->AppendText(u"A ");
    builderNoCtrl->PushStyle(style2);
    builderNoCtrl->AppendText(u" B");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(),
        typographyNoCtrl->GetLongestLineWithIndent()));
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest006
 * @tc.desc: Test text with only spaces and control characters has zero wordSpacing contribution
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest006, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    // Text with only spaces and control chars: " \u200D " (no word, no wordSpacing triggered)
    std::u16string text = u" \u200D ";
    double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
    EXPECT_DOUBLE_EQ(widthDelta, 0);
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest007
 * @tc.desc: Test c0 zero width kControl characters do not trigger wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest007, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A ";
        text += ctrl;
        text += u" B";
        // "A <ctrl> B" has 1 word boundary → control char should not break grouping → gapCount = 1
        double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
        EXPECT_DOUBLE_EQ(widthDelta, 10)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 10;
    }
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest008
 * @tc.desc: Test c0 non zero width kControl characters do not trigger wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest008, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateGeneralCategoryNonZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A ";
        text += ctrl;
        text += u" B";
        // "A <ctrl> B" has 1 word boundary → control char should not break grouping → gapCount = 1
        double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
        EXPECT_DOUBLE_EQ(widthDelta, 20)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 20;
    }
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest009
 * @tc.desc: Test break c0 zero width kControl characters do not trigger wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest009, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateBreakGeneralCategoryNonZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A ";
        text += ctrl;
        text += u" B";
        // "A <ctrl> B" has 1 word boundary → control char should not break grouping → gapCount = 1
        double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
        EXPECT_DOUBLE_EQ(widthDelta, 10)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 10;
    }
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest010
 * @tc.desc: Test del cc zero width kControl characters do not trigger wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest010, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateDelGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A ";
        text += ctrl;
        text += u" B";
        // "A <ctrl> B" has 1 word boundary → control char should not break grouping → gapCount = 1
        double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
        EXPECT_DOUBLE_EQ(widthDelta, 10)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 10;
    }
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest011
 * @tc.desc: Test format cf zero width kControl characters do not trigger wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest011, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateFormatGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A ";
        text += ctrl;
        text += u" B";
        // "A <ctrl> B" has 1 word boundary → control char should not break grouping → gapCount = 1
        double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
        EXPECT_DOUBLE_EQ(widthDelta, 10)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 10;
    }
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest012
 * @tc.desc: Test bidi cf zero width kControl characters do not trigger wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest012, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API26_VERSION);
    std::vector<char16_t> controlChars;
    generateBidiGeneralCategoryZeroWidthControlCharacters(controlChars);

    for (char16_t ctrl : controlChars) {
        std::u16string text = u"A ";
        text += ctrl;
        text += u" B";
        // "A <ctrl> B" has 1 word boundary → control char should not break grouping → gapCount = 1
        double widthDelta = getLayoutWidthWithWordSpacing(text, 10) - getLayoutWidthWithWordSpacing(text);
        EXPECT_DOUBLE_EQ(widthDelta, 10)
            << "Control char U+" << std::hex << static_cast<int>(ctrl)
            << " widthDelta=" << std::dec << widthDelta
            << " expected=" << 10;
    }
}

/*
 * @tc.name: TypographyControlCharLetterSpacingTest013
 * @tc.desc: Test that with API18 (feature disabled), ZWJ (U+200B) still triggers letterSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharLetterSpacingTest013, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API18_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.letterSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // With API18 the feature is disabled, so ZWJ still triggers letterSpacing;
    // "你\u200B好" gets one more spacing gap than "你好" → widths should differ.
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"你\u200B好");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"你好");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_FALSE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(), typographyNoCtrl->GetLongestLineWithIndent()));
    EXPECT_GT(typographyWithCtrl->GetLongestLineWithIndent(), typographyNoCtrl->GetLongestLineWithIndent());
}

/*
 * @tc.name: TypographyControlCharWordSpacingTest013
 * @tc.desc: Test that with API18 (feature disabled), ZWSP (U+200B) between spaces triggers extra wordSpacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyControlCharWordSpacingTest013, TestSize.Level0)
{
    BundleApiVersionGuard apiGuard(SPText::SINCE_API18_VERSION);
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.wordSpacing = 10;
    textStyle.fontSize = 40;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    // With API18 the feature is disabled, so ZWSP between spaces is treated as a word boundary;
    // "Hello \u200B World" gets one more wordSpacing than "Hello  World" → widths should differ.
    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderWithCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderWithCtrl->PushStyle(textStyle);
    builderWithCtrl->AppendText(u"Hello \u200D World");
    auto typographyWithCtrl = builderWithCtrl->CreateTypography();
    typographyWithCtrl->Layout(10000);

    std::unique_ptr<OHOS::Rosen::TypographyCreate> builderNoCtrl =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builderNoCtrl->PushStyle(textStyle);
    builderNoCtrl->AppendText(u"Hello  World");
    auto typographyNoCtrl = builderNoCtrl->CreateTypography();
    typographyNoCtrl->Layout(10000);

    EXPECT_FALSE(skia::textlayout::nearlyEqual(
        typographyWithCtrl->GetLongestLineWithIndent(), typographyNoCtrl->GetLongestLineWithIndent()));
    EXPECT_GT(typographyWithCtrl->GetLongestLineWithIndent(), typographyNoCtrl->GetLongestLineWithIndent());
}

/*
 * @tc.name: TypographyGetDumpInfoTest
 * @tc.desc: test for get dump info (paragraph-level state, runs, blocks, lines)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyGetDumpInfoTest, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style1;
    style1.fontSize = 50;
    style1.color = Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0);
    style1.fontWeight = FontWeight::W500;
    style1.fontWidth = FontWidth::SEMI_EXPANDED;
    style1.fontStyle = FontStyle::OBLIQUE;
    std::u16string text = u"你好, 测试GetDumpInfo中返回的数据, 是否符合预期";
    typographyCreate->PushStyle(style1);
    typographyCreate->AppendText(text);
    OHOS::Rosen::TextStyle style2;
    style2.fontSize = 60;
    style2.color = Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0);
    style2.fontWeight = FontWeight::W300;
    style2.fontWidth = FontWidth::EXPANDED;
    style2.fontStyle = FontStyle::ITALIC;
    typographyCreate->PushStyle(style2);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    double maxWidth = 500;
    typography->Layout(maxWidth);

    OHOS::Rosen::Drawing::Canvas canvas;
    typography->Paint(&canvas, 0, 0);
    std::unique_ptr<SPText::Paragraph> paragraphTemp = nullptr;
    AdapterTxt::Typography* typographyImpl = static_cast<AdapterTxt::Typography*>(typography.get());
    typographyImpl->paragraph_.swap(paragraphTemp);
    EXPECT_EQ(typography->GetDumpInfo(), "");
    typographyImpl->paragraph_.swap(paragraphTemp);

    // Verify dump info contains basic structure fields
    std::string dumpInfo = typography->GetDumpInfo();
    EXPECT_NE(dumpInfo.find("Paragraph dump:"), std::string::npos);
    EXPECT_NE(dumpInfo.find("Text sz:126"), std::string::npos);
    EXPECT_NE(dumpInfo.find("State:Drawn"), std::string::npos);
    EXPECT_NE(dumpInfo.find("Run0 glyph sz:"), std::string::npos);
    EXPECT_NE(dumpInfo.find("Blk0 rng[0-63)"), std::string::npos);
    EXPECT_NE(dumpInfo.find("Blk1 rng[63-126)"), std::string::npos);
    EXPECT_NE(dumpInfo.find("Paragraph glyph sz:62"), std::string::npos);
}

/*
 * @tc.name: TypographyGetDumpInfoFamilyNameTest
 * @tc.desc: test for get dump info contains typeface family name and unique ID per run
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyGetDumpInfoFamilyNameTest, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style1;
    style1.fontSize = 50;
    style1.color = Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0);
    std::u16string text = u"你好, 测试GetDumpInfo中返回的数据, 是否符合预期";
    typographyCreate->PushStyle(style1);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    double maxWidth = 500;
    typography->Layout(maxWidth);

    OHOS::Rosen::Drawing::Canvas canvas;
    typography->Paint(&canvas, 0, 0);
    std::string dumpInfo = typography->GetDumpInfo();

    // Each Run entry must contain "family:" and "id:" fields from typeface
    EXPECT_NE(dumpInfo.find("family:"), std::string::npos);
    EXPECT_NE(dumpInfo.find(",id:"), std::string::npos);

    // Verify "family:" appears immediately after the rng field in Run0
    EXPECT_NE(dumpInfo.find("rng[0-6),family:"), std::string::npos);

    // Verify "id:" appears after "family:" - at least one run has id > 0
    // Format is "family:<name>,id:<number>,"
    auto familyPos = dumpInfo.find("family:");
    auto idPos = dumpInfo.find(",id:", familyPos);
    EXPECT_NE(idPos, std::string::npos);
}

/*
 * @tc.name: TypographyGetDumpInfoSymbolTest
 * @tc.desc: test for get dump info appends symbol run info when hmSymbols_ is not empty
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyGetDumpInfoSymbolTest, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style1;
    style1.fontSize = 50;
    style1.color = Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0);
    std::u16string text = u"你好";
    typographyCreate->PushStyle(style1);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    double maxWidth = 500;
    typography->Layout(maxWidth);

    OHOS::Rosen::Drawing::Canvas canvas;
    typography->Paint(&canvas, 0, 0);
    // Cast to ParagraphImpl to access hmSymbols_
    SPText::ParagraphImpl* paragraphImpl =
        static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraphImpl, nullptr);

    // hmSymbols_ is empty by default - no symbol info in dump
    std::string dumpNoSymbol = typography->GetDumpInfo();
    EXPECT_EQ(dumpNoSymbol.find("{SymbolRun"), std::string::npos);

    // Create a HMSymbolRun and set known symbol data
    auto symbolRun = std::make_shared<SPText::HMSymbolRun>();
    SPText::HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolUid(100); // 100 is test uid
    symbolTxt.SetRenderMode(RSSymbolRenderingStrategy::SINGLE);
    RSSColor rsColor = {1.0f, 255, 0, 0};
    symbolTxt.SetRenderColor(rsColor);
    symbolRun->SetSymbolTxt(symbolTxt);
    paragraphImpl->hmSymbols_.push_back(symbolRun);

    // Verify symbol info is appended to dump
    std::string dumpWithSymbol = typography->GetDumpInfo();
    EXPECT_NE(dumpWithSymbol.find("{SymbolRun0}uid:100"), std::string::npos);
    EXPECT_NE(dumpWithSymbol.find(",rmode:0,"), std::string::npos); // 0 is SINGLE
    EXPECT_NE(dumpWithSymbol.find("clrCnt:1"), std::string::npos);
    EXPECT_NE(dumpWithSymbol.find("clr0:[255,0,0,1]"), std::string::npos);
    EXPECT_NE(dumpWithSymbol.find(".uiClrCnt:0"), std::string::npos);

    // Verify paragraph dump prefix is still present
    EXPECT_NE(dumpWithSymbol.find("Paragraph dump:"), std::string::npos);
}

/*
 * @tc.name: TypographyGetDumpInfoSymbolUIColorTest
 * @tc.desc: test for get dump info includes UIColor and ColorSpace for symbol runs
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyGetDumpInfoSymbolUIColorTest, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style1;
    style1.fontSize = 50;
    style1.color = Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0);
    std::u16string text = u"你好";
    typographyCreate->PushStyle(style1);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    double maxWidth = 500;
    typography->Layout(maxWidth);

    OHOS::Rosen::Drawing::Canvas canvas;
    typography->Paint(&canvas, 0, 0);
    SPText::ParagraphImpl* paragraphImpl =
        static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraphImpl, nullptr);

    // Create a symbol run with UIColor data
    auto symbolRun = std::make_shared<SPText::HMSymbolRun>();
    SPText::HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolUid(200); // 200 is test uid
    symbolTxt.SetRenderMode(RSSymbolRenderingStrategy::MULTIPLE_COLOR); // rmode=1
    RSSColor rsColor1 = {1.0f, 255, 0, 0};
    RSSColor rsColor2 = {0.5f, 0, 255, 0};
    symbolTxt.SetRenderColor({rsColor1, rsColor2});

    // Set UIColor with specific values for dump verification
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    symbolTxt.SetRenderUIColor({ uiColor }, { SymbolColorSpace::DISPLAY_P3 });

    symbolRun->SetSymbolTxt(symbolTxt);
    paragraphImpl->hmSymbols_.push_back(symbolRun);

    std::string dumpInfo = typography->GetDumpInfo();

    // Verify uid and render mode
    EXPECT_NE(dumpInfo.find("{SymbolRun0}uid:200"), std::string::npos);
    EXPECT_NE(dumpInfo.find(",rmode:1,"), std::string::npos); // 1 is MULTIPLE_COLOR

    // Verify RSSColor count and first color
    EXPECT_NE(dumpInfo.find("clrCnt:0"), std::string::npos);
    EXPECT_EQ(dumpInfo.find("clrCnt:2"), std::string::npos);
    EXPECT_EQ(dumpInfo.find("clr0:[255,0,0,1]"), std::string::npos);
    EXPECT_EQ(dumpInfo.find("clr1:[0,255,0,0.5]"), std::string::npos);

    // Verify UIColor count and values
    EXPECT_NE(dumpInfo.find(".uiClrCnt:1"), std::string::npos);
    EXPECT_NE(dumpInfo.find("uiClr0:[1,0.5,0.3,1,1]"), std::string::npos);

    // Verify ColorSpace: DISPLAY_P3 = 1
    EXPECT_NE(dumpInfo.find("cspace0:1"), std::string::npos);
}

/*
 * @tc.name: TypographyGetDumpInfoSymbolNullptrTest
 * @tc.desc: test for get dump info skips nullptr symbol runs
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyGetDumpInfoSymbolNullptrTest, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style1;
    style1.fontSize = 50;
    style1.color = Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0);
    std::u16string text = u"你好";
    typographyCreate->PushStyle(style1);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    double maxWidth = 500;
    typography->Layout(maxWidth);

    OHOS::Rosen::Drawing::Canvas canvas;
    typography->Paint(&canvas, 0, 0);
    SPText::ParagraphImpl* paragraphImpl =
        static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraphImpl, nullptr);

    // Push nullptr as a symbol run - should be skipped
    paragraphImpl->hmSymbols_.push_back(nullptr);

    // Push a valid symbol run - should appear with index 1 (not 0)
    auto symbolRun = std::make_shared<SPText::HMSymbolRun>();
    SPText::HMSymbolTxt symbolTxt;
    symbolTxt.SetSymbolUid(300); // 300 is test uid
    symbolTxt.SetRenderMode(RSSymbolRenderingStrategy::SINGLE);
    symbolRun->SetSymbolTxt(symbolTxt);
    paragraphImpl->hmSymbols_.push_back(symbolRun);

    std::string dumpInfo = typography->GetDumpInfo();
    // The nullptr at index 0 is skipped, so the valid one at index 1 appears as {SymbolRun1}
    EXPECT_NE(dumpInfo.find("{SymbolRun1}uid:300"), std::string::npos);
    // {SymbolRun0} should NOT appear (nullptr was skipped)
    EXPECT_EQ(dumpInfo.find("{SymbolRun0}"), std::string::npos);
}

/*
 * @tc.name: TypographyForceReuseRasterResultTest001
 * @tc.desc: Verify default value is false, set/get works correctly
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyForceReuseRasterResultTest001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = ARC_FONT_SIZE;
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"test");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    EXPECT_FALSE(typography->GetForceReuseRasterResult());
    typography->SetForceReuseRasterResult(true);
    EXPECT_TRUE(typography->GetForceReuseRasterResult());
    typography->Layout(DEFAULT_MAX_WIDTHS);
    EXPECT_TRUE(typography->GetForceReuseRasterResult());
    OHOS::Rosen::Drawing::Canvas canvas;
    typography->Paint(&canvas, 0, 0);
    EXPECT_TRUE(typography->GetForceReuseRasterResult());
    SPText::ParagraphImpl* paragraphImpl =
        static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraphImpl, nullptr);
    EXPECT_TRUE(paragraphImpl->GetForceReuseRasterResult());
    
    typography->SetForceReuseRasterResult(false);
    EXPECT_FALSE(typography->GetForceReuseRasterResult());
    typography->Layout(DEFAULT_MAX_WIDTHS);
    EXPECT_FALSE(typography->GetForceReuseRasterResult());
    typography->Paint(&canvas, 0, 0);
    EXPECT_FALSE(typography->GetForceReuseRasterResult());
    SPText::ParagraphImpl* paragraphImpl2 =
        static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraphImpl2, nullptr);
    EXPECT_FALSE(paragraphImpl2->GetForceReuseRasterResult());
}

/*
 * @tc.name: TypographyForceReuseRasterResultTest002
 * @tc.desc: Verify Paint with flag set does not crash
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyForceReuseRasterResultTest002, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = ARC_FONT_SIZE;
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"test");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->SetForceReuseRasterResult(true);
    typography->Layout(DEFAULT_MAX_WIDTHS);
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format {
        OHOS::Rosen::Drawing::ColorType::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::AlphaType::ALPHATYPE_OPAQUE
    };
    bitmap.Build(200, 100, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_WHITE);
    EXPECT_NO_FATAL_FAILURE(typography->Paint(&canvas, 0, 0));
}

/*
 * @tc.name: TypographyForceReuseRasterResultTest003
 * @tc.desc: Verify Layout does not affect the flag state
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyForceReuseRasterResultTest004, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = ARC_FONT_SIZE;
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"test");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->SetForceReuseRasterResult(true);
    typography->Layout(500.0);
    EXPECT_TRUE(typography->GetForceReuseRasterResult());
    typography->Layout(300.0);
    EXPECT_TRUE(typography->GetForceReuseRasterResult());
}

/*
 * @tc.name: TypographyForceReuseRasterResultTest004
 * @tc.desc: Verify PaintOnPath with flag set does not crash
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyForceReuseRasterResultTest005, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = ARC_FONT_SIZE;
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"test");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->SetForceReuseRasterResult(true);
    typography->Layout(DEFAULT_MAX_WIDTHS);
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format {
        OHOS::Rosen::Drawing::ColorType::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::AlphaType::ALPHATYPE_OPAQUE
    };
    bitmap.Build(200, 100, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_WHITE);
    OHOS::Rosen::Drawing::Path path;
    path.MoveTo(10, 50);
    path.LineTo(190, 50);
    EXPECT_NO_FATAL_FAILURE(typography->Paint(&canvas, &path, 0, 0));
}

/*
 * @tc.name: TypographyForceReuseRasterResultTest005
 * @tc.desc: Verify repeated Set/Get cycles return consistent values
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyForceReuseRasterResultTest007, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = ARC_FONT_SIZE;
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"test");
    auto typography = builder->CreateTypography();
    EXPECT_FALSE(typography->GetForceReuseRasterResult());
    ASSERT_NE(typography, nullptr);
    for (int i = 0; i < 100; i++) {
        typography->SetForceReuseRasterResult(true);
        EXPECT_TRUE(typography->GetForceReuseRasterResult());
        typography->SetForceReuseRasterResult(false);
        EXPECT_FALSE(typography->GetForceReuseRasterResult());
    }
}

/*
 * @tc.name: TypographyForceReuseRasterResultTest006
 * @tc.desc: Verify flag only takes effect on next Paint after Set
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyForceReuseRasterResultTest008, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = ARC_FONT_SIZE;
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"test");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(DEFAULT_MAX_WIDTHS);
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format {
        OHOS::Rosen::Drawing::ColorType::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::AlphaType::ALPHATYPE_OPAQUE
    };
    bitmap.Build(200, 100, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_WHITE);
    EXPECT_NO_FATAL_FAILURE(typography->Paint(&canvas, 0, 0));
    EXPECT_FALSE(typography->GetForceReuseRasterResult());

    typography->SetForceReuseRasterResult(true);
    EXPECT_NO_FATAL_FAILURE(typography->Paint(&canvas, 0, 0));
    EXPECT_TRUE(typography->GetForceReuseRasterResult());
}

/*
 * @tc.name: TypographyForceReuseRasterResultTest007
 * @tc.desc: Verify toggle flag between Paints works correctly
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, TypographyForceReuseRasterResultTest009, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = ARC_FONT_SIZE;
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    auto builder = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    builder->PushStyle(textStyle);
    builder->AppendText(u"test");
    auto typography = builder->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(DEFAULT_MAX_WIDTHS);
    OHOS::Rosen::Drawing::Bitmap bitmap;
    OHOS::Rosen::Drawing::BitmapFormat format {
        OHOS::Rosen::Drawing::ColorType::COLORTYPE_RGBA_8888,
        OHOS::Rosen::Drawing::AlphaType::ALPHATYPE_OPAQUE
    };
    bitmap.Build(200, 100, format);
    OHOS::Rosen::Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    canvas.Clear(OHOS::Rosen::Drawing::Color::COLOR_WHITE);
    typography->SetForceReuseRasterResult(true);
    EXPECT_NO_FATAL_FAILURE(typography->Paint(&canvas, 0, 0));

    typography->SetForceReuseRasterResult(false);
    EXPECT_NO_FATAL_FAILURE(typography->Paint(&canvas, 0, 0));
    EXPECT_FALSE(typography->GetForceReuseRasterResult());
}

/*
 * @tc.name: OH_Drawing_TypographyBalanceTest001
 * @tc.desc: test for score algorithm in old balanced strategy with special characters, no freeze
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyBalanceTest001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.breakStrategy = BreakStrategy::BALANCED;
    TextTab myTab;
    myTab.alignment = TextAlign::CENTER;
    myTab.location = 1;
    typographyStyle.tab = myTab;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    style.fontSize = 76;
    std::u16string text =
        u"\u{0046}\u{007d}\u{00b7}\u{0038}\u{5572}\u{5a36}\u{0030}\u{5784}\u{0075}\u{0621}\u{0033}\u{00bf}\u{007b}"
        u"\u{5f7f}\u{0036}\u{0038}\u{0075}\u{007b}\u{0037}\u{007b}\u{0033}\u{0034}\u{0075}\u{0075}\u{0075}\u{0609}"
        u"\u{007d}\u{005c}\u{0075}\u{30cb}\u{530b}\u{007d}\u{0045}\u{0044}\u{4f31}\u{005c}\u{0075}\u{005c}\u{007d}"
        u"\u{0041}\u{618a}\u{0032}\u{000a}\u{0034}\u{0033}\u{0038}\u{59d0}\u{0031}\u{000a}\u{5285}\u{0044}\u{0032}"
        u"\u{007d}\u{0075}\u{0075}\u{5781}\u{0041}\u{005c}\u{0035}\u{007b}\u{5cde}\u{0046}\u{007d}\u{0037}\u{52e6}"
        u"\u{005c}\u{6120}\u{0034}\u{007b}\u{007d}\u{0075}\u{005c}\u{007d}\u{007b}\u{5a27}\u{4f74}\u{56ae}\u{0037}"
        u"\u{60eb}\u{007d}\u{005c}\u{007b}\u{0033}\u{5311}\u{ff1b}\u{5644}\u{0044}\u{007d}\u{007d}\u{007b}\u{005c}"
        u"\u{005c}\u{0034}\u{007b}\u{0038}\u{04cb}\u{007b}\u{000a}\u{007d}\u{005c}";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(0.1);
}

/*
 * @tc.name: OH_Drawing_TypographyGetImageBounds001
 * @tc.desc: test for run get image bounds with special characters, no crash
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyGetImageBounds001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    ASSERT_NE(typographyCreate, nullptr);
    OHOS::Rosen::TextStyle style;
    std::u16string text =
        u"\u{0043}\u{0046}\u{0046}\u{0041}\u{5230}\u{007b}\u{007d}\u{0031}\u{0030}\u{2026}\u{0042}\u{500d}\u{0035}"
        u"\u{005c}\u{0043}\u{0033}\u{007d}\u{005c}\u{0075}\u{0036}\u{0075}\u{007d}\u{005c}\u{0044}\u{0035}\u{0030}"
        u"\u{0075}\u{0030}\u{005c}\u{0031}\u{0032}\u{007b}\u{0075}\u{0037}\u{007b}\u{0046}\u{007b}\u{4e28}\u{005c}"
        u"\u{007d}\u{0035}\u{005c}\u{0030}\u{595d}\u{0037}\u{0037}\u{007b}\u{0075}\u{005c}\u{007d}\u{5b94}\u{5a87}"
        u"\u{005c}\u{005c}\u{0039}\u{0032}\u{0038}\u{0075}\u{0037}\u{005c}\u{0075}\u{0038}\u{0033}\u{0038}\u{0039}"
        u"\u{0039}\u{0034}\u{0f79}\u{0030}\u{0036}\u{007d}\u{52e4}\u{0041}\u{005c}\u{ff09}\u{007d}\u{0075}\u{005c}"
        u"\u{007b}\u{0039}\u{0037}\u{0039}\u{005c}\u{0030}\u{007b}\u{0075}\u{0031}\u{0044}\u{0035}\u{0038}\u{300b}"
        u"\u{0044}\u{0037}\u{0039}\u{0037}\u{005c}\u{007b}\u{007b}\u{007b}\u{007d}";
    typographyCreate->PushStyle(style);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    ASSERT_NE(typography, nullptr);
    typography->Layout(980);
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    auto lines = static_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get())->GetTextLines();
    size_t expectLineSize = 1;
    EXPECT_EQ(lines.size(), expectLineSize);
    for (size_t i = 0; i < lines.size(); i++) {
        auto runs = lines[i]->getGlyphRuns();
        for (size_t j = 0; j < runs.size(); j++) {
            runs[j]->getImageBounds();
        }
    }
}
} // namespace Rosen
} // namespace OHOS
