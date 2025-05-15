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

#include "convert.h"
#include "gtest/gtest.h"
#include "rosen_text/text_style.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHDrawingConvertTest : public testing::Test {};

/*
 * @tc.name: OHDrawingConvertTest001
 * @tc.desc: test for  Convert FontCollection
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest001, TestSize.Level1)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::shared_ptr<OHOS::Rosen::AdapterTxt::FontCollection> adapterFontCollection =
        AdapterTxt::Convert(fontCollection);
    EXPECT_NE(adapterFontCollection, nullptr);
    EXPECT_NE(adapterFontCollection->fontCollection_, nullptr);
    EXPECT_NE(adapterFontCollection->dfmanager_, nullptr);
}

/*
 * @tc.name: OHDrawingConvertTest002
 * @tc.desc: test for  Convert Affinity
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest002, TestSize.Level1)
{
    // 0 for unit test
    const SPText::PositionWithAffinity zeroAndUpstream = { 0, SPText::Affinity::UPSTREAM };
    IndexAndAffinity zeroUpstream = AdapterTxt::Convert(zeroAndUpstream);
    EXPECT_EQ(zeroUpstream.index == 0, true);
    EXPECT_EQ(zeroUpstream.affinity, OHOS::Rosen::Affinity::PREV);

    // 1 for unit test
    const SPText::PositionWithAffinity oneAndDownstream = { 1, SPText::Affinity::DOWNSTREAM };
    IndexAndAffinity oneDownstream = AdapterTxt::Convert(oneAndDownstream);
    EXPECT_EQ(oneDownstream.index, 1);
    EXPECT_EQ(oneDownstream.affinity, OHOS::Rosen::Affinity::NEXT);
}

/*
 * @tc.name: OHDrawingConvertTest003
 * @tc.desc: test for  Convert Range
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest003, TestSize.Level1)
{
    // 0 for unit test
    SPText::Range<size_t> range(0, 0);
    Boundary boundary = AdapterTxt::Convert(range);
    EXPECT_EQ(boundary.leftIndex, 0);
    EXPECT_EQ(boundary.rightIndex, 0);

    // 1, 10 for unit test
    SPText::Range<size_t> range1(1, 10);
    Boundary boundary1 = AdapterTxt::Convert(range1);
    EXPECT_EQ(boundary1.leftIndex, 1);
    EXPECT_EQ(boundary1.rightIndex, 10);
}

/*
 * @tc.name: OHDrawingConvertTest004
 * @tc.desc: test for  Convert TextBox
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest004, TestSize.Level1)
{
    SkRect skRect = SkRect::MakeEmpty();
    const SPText::TextBox box(skRect, SPText::TextDirection::RTL);
    TextRect textRect = AdapterTxt::Convert(box);
    EXPECT_EQ(textRect.direction, TextDirection::RTL);
    EXPECT_EQ(textRect.rect, Drawing::RectF(0, 0, 0, 0));

    const SPText::TextBox box1(skRect, SPText::TextDirection::LTR);
    TextRect textRect1 = AdapterTxt::Convert(box1);
    EXPECT_EQ(textRect1.direction, TextDirection::LTR);
}

/*
 * @tc.name: OHDrawingConvertTest005
 * @tc.desc: test for  Convert TextRectHeightStyle
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest005, TestSize.Level1)
{
    TextRectHeightStyle textRectHeightStyle = TextRectHeightStyle::TIGHT;
    SPText::RectHeightStyle rectHeightStyle = AdapterTxt::Convert(textRectHeightStyle);
    EXPECT_EQ(rectHeightStyle, SPText::RectHeightStyle::TIGHT);

    TextRectHeightStyle trhs1 = TextRectHeightStyle::COVER_TOP_AND_BOTTOM;
    SPText::RectHeightStyle rhs1 = AdapterTxt::Convert(trhs1);
    EXPECT_EQ(rhs1, SPText::RectHeightStyle::MAX);

    TextRectHeightStyle trhs2 = TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM;
    SPText::RectHeightStyle rhs2 = AdapterTxt::Convert(trhs2);
    EXPECT_EQ(rhs2, SPText::RectHeightStyle::INCLUDE_LINESPACING_MIDDLE);

    TextRectHeightStyle trhs3 = TextRectHeightStyle::COVER_TOP;
    SPText::RectHeightStyle rhs3 = AdapterTxt::Convert(trhs3);
    EXPECT_EQ(rhs3, SPText::RectHeightStyle::INCLUDE_LINESPACING_TOP);

    TextRectHeightStyle trhs4 = TextRectHeightStyle::COVER_BOTTOM;
    SPText::RectHeightStyle rhs4 = AdapterTxt::Convert(trhs4);
    EXPECT_EQ(rhs4, SPText::RectHeightStyle::INCLUDE_LINESPACING_BOTTOM);

    TextRectHeightStyle trhs5 = TextRectHeightStyle::FOLLOW_BY_STRUT;
    SPText::RectHeightStyle rhs5 = AdapterTxt::Convert(trhs5);
    EXPECT_EQ(rhs5, SPText::RectHeightStyle::STRUT);
}

/*
 * @tc.name: OHDrawingConvertTest006
 * @tc.desc: test for  Convert TextRectWidthStyle
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest006, TestSize.Level1)
{
    TextRectWidthStyle textRectWidthStyle = TextRectWidthStyle::MAX;
    SPText::RectWidthStyle rectWidthStyle = AdapterTxt::Convert(textRectWidthStyle);
    EXPECT_EQ(rectWidthStyle, SPText::RectWidthStyle::MAX);

    TextRectWidthStyle textRectWidthStyle1 = TextRectWidthStyle::TIGHT;
    SPText::RectWidthStyle rectWidthStyle1 = AdapterTxt::Convert(textRectWidthStyle1);
    EXPECT_EQ(rectWidthStyle1, SPText::RectWidthStyle::TIGHT);
}

/*
 * @tc.name: OHDrawingConvertTest007
 * @tc.desc: test for  Convert TypographyStyle
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest007, TestSize.Level1)
{
    TypographyStyle typographyStyle;
    SPText::ParagraphStyle paragraphStyle = AdapterTxt::Convert(typographyStyle);
    EXPECT_EQ(paragraphStyle.fontSize, typographyStyle.fontSize);
    EXPECT_EQ(paragraphStyle.paragraphSpacing, typographyStyle.paragraphSpacing);
    EXPECT_EQ(paragraphStyle.isEndAddParagraphSpacing, typographyStyle.isEndAddParagraphSpacing);
    EXPECT_EQ(paragraphStyle.isTrailingSpaceOptimized, typographyStyle.isTrailingSpaceOptimized);
}

/*
 * @tc.name: OHDrawingConvertTest008
 * @tc.desc: test for  Convert PlaceholderSpan
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest008, TestSize.Level1)
{
    PlaceholderSpan placeholderSpan;
    placeholderSpan.alignment = PlaceholderVerticalAlignment::OFFSET_AT_BASELINE;
    SPText::PlaceholderRun placeholderRun = AdapterTxt::Convert(placeholderSpan);
    EXPECT_EQ(placeholderRun.alignment, SPText::PlaceholderAlignment::BASELINE);
}

/*
 * @tc.name: OHDrawingConvertTest009
 * @tc.desc: test for  Convert PlaceholderSpan
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest009, TestSize.Level1)
{
    TextStyle textStyle;
    SPText::TextStyle sptextStyle = AdapterTxt::Convert(textStyle);
    EXPECT_EQ(sptextStyle.color, textStyle.color);
    // 此处 decoration 不是同一个类型，所以转换后判断，下同
    EXPECT_EQ(static_cast<int>(sptextStyle.decoration), static_cast<int>(textStyle.decoration));
    EXPECT_EQ(static_cast<int>(sptextStyle.decorationStyle), static_cast<int>(textStyle.decorationStyle));
    EXPECT_EQ(static_cast<int>(sptextStyle.fontWeight), static_cast<int>(textStyle.fontWeight));
    EXPECT_EQ(static_cast<int>(sptextStyle.fontStyle), static_cast<int>(textStyle.fontStyle));
    EXPECT_EQ(sptextStyle.decorationColor, textStyle.decorationColor);
    EXPECT_EQ(sptextStyle.decorationThicknessMultiplier, textStyle.decorationThicknessScale);
    EXPECT_EQ(sptextStyle.letterSpacing, textStyle.letterSpacing);
    EXPECT_EQ(sptextStyle.wordSpacing, textStyle.wordSpacing);
    EXPECT_EQ(sptextStyle.height, textStyle.heightScale);
    EXPECT_EQ(sptextStyle.heightOverride, textStyle.heightOnly);
    EXPECT_EQ(sptextStyle.halfLeading, textStyle.halfLeading);
    EXPECT_EQ(sptextStyle.locale, textStyle.locale);
    EXPECT_EQ(sptextStyle.styleId, textStyle.styleId);
    // 此处 rect 不是同一个类型，所以只判断 color 是否相同
    EXPECT_EQ(sptextStyle.backgroundRect.color, textStyle.backgroundRect.color);
    EXPECT_EQ(sptextStyle.foreground->brush, textStyle.foregroundBrush);
    EXPECT_EQ(sptextStyle.foreground->pen, textStyle.foregroundPen);
    EXPECT_EQ(sptextStyle.fontFamilies, textStyle.fontFamilies);
    // 此处 textShadow 不是同一个类型，所以转换后判断，所以只判断大小是否相同。
    EXPECT_EQ(sptextStyle.textShadows.size(), textStyle.shadows.size());
}

/*
 * @tc.name: OHDrawingConvertTest010
 * @tc.desc: test for  Convert PlaceholderSpan
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest010, TestSize.Level2)
{
    TextStyle textStyle;
    int tagFeature = 10;
    float tagAxis = 1.2;
    double fontParam = 14.0;
    textStyle.symbol.SetVisualMode(VisualMode::VISUAL_SMALL);
    textStyle.fontFeatures.SetFeature("tag", tagFeature);
    textStyle.fontVariations.SetAxisValue("tag", tagAxis);
    textStyle.backgroundBrush = Drawing::Brush(Drawing::Color::COLOR_CYAN);
    textStyle.foregroundBrush = Drawing::Brush(Drawing::Color::COLOR_DKGRAY);
    textStyle.backgroundPen = Drawing::Pen(Drawing::Color::COLOR_CYAN);
    textStyle.foregroundPen = Drawing::Pen(Drawing::Color::COLOR_DKGRAY);
    std::vector<TextShadow> shadows = { {}, {} };

    SPText::TextStyle sptextStyle = AdapterTxt::Convert(textStyle);

    EXPECT_EQ(sptextStyle.fontSize, fontParam);
    EXPECT_EQ(sptextStyle.fontFeatures.GetFontFeatures().size(), 1);
    EXPECT_EQ(sptextStyle.fontFeatures.GetFontFeatures().begin()->first, "tag");
    EXPECT_EQ(sptextStyle.fontFeatures.GetFontFeatures().begin()->second, tagFeature);
    EXPECT_EQ(sptextStyle.fontVariations.GetAxisValues().size(), 1);
    EXPECT_EQ(sptextStyle.fontVariations.GetAxisValues().at("tag"), tagAxis);
    EXPECT_EQ(sptextStyle.background->brush.value().GetColor(), Drawing::Color::COLOR_CYAN);
    EXPECT_EQ(sptextStyle.background->pen.value().GetColor(), Drawing::Color::COLOR_CYAN);
    EXPECT_EQ(sptextStyle.foreground->brush.value().GetColor(), Drawing::Color::COLOR_DKGRAY);
    EXPECT_EQ(sptextStyle.foreground->pen.value().GetColor(), Drawing::Color::COLOR_DKGRAY);
}

/*
 * @tc.name: OHDrawingConvertTest011
 * @tc.desc: test for  Convert PlaceholderSpan
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest011, TestSize.Level1)
{
    SPText::TextStyle sptextStyle;
    int spTagFeature = 0;
    float spTagAxis = 1.2;
    double spFontParam = 14.0;
    sptextStyle.fontFeatures.SetFeature("tag", spTagFeature);
    sptextStyle.fontVariations.SetAxisValue("tag", spTagAxis);
    sptextStyle.isSymbolGlyph = true;
    sptextStyle.background = SPText::PaintRecord(Drawing::Brush(Drawing::Color::COLOR_LTGRAY), Drawing::Pen());
    sptextStyle.foreground = SPText::PaintRecord(Drawing::Brush(Drawing::Color::COLOR_MAGENTA), Drawing::Pen());
    std::vector<TextShadow> TextShadow = { {}, {} };

    TextStyle textStyle = AdapterTxt::Convert(sptextStyle);

    EXPECT_EQ(textStyle.fontSize, spFontParam);
    EXPECT_EQ(textStyle.fontFeatures.GetFontFeatures().size(), 1);
    EXPECT_EQ(textStyle.fontFeatures.GetFontFeatures().begin()->first, "tag");
    EXPECT_EQ(textStyle.fontFeatures.GetFontFeatures().begin()->second, spTagFeature);
    EXPECT_EQ(textStyle.fontVariations.GetAxisValues().size(), 1);
    EXPECT_EQ(textStyle.fontVariations.GetAxisValues().at("tag"), spTagAxis);
    EXPECT_EQ(textStyle.isSymbolGlyph, true);
    EXPECT_EQ(textStyle.backgroundBrush.value().GetColor(), Drawing::Color::COLOR_LTGRAY);
    EXPECT_EQ(textStyle.foregroundBrush.value().GetColor(), Drawing::Color::COLOR_MAGENTA);
}
} // namespace Rosen
} // namespace OHOS