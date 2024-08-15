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
    EXPECT_EQ(adapterFontCollection != nullptr, true);
}

/*
 * @tc.name: OHDrawingConvertTest002
 * @tc.desc: test for  Convert Affinity
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest002, TestSize.Level1)
{
    // 0 for unit test
    const SPText::PositionWithAffinity positionWithAffinity = { 0, SPText::Affinity::UPSTREAM };
    IndexAndAffinity indexAndAffinity = AdapterTxt::Convert(positionWithAffinity);
    EXPECT_EQ(indexAndAffinity.index == 0, true);
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
    EXPECT_EQ(boundary.leftIndex == 0, true);
}

/*
 * @tc.name: OHDrawingConvertTest004
 * @tc.desc: test for  Convert TextBox
 * @tc.type: FUNC
 */
HWTEST_F(OHDrawingConvertTest, OHDrawingConvertTest004, TestSize.Level1)
{
    SkRect skRect;
    const SPText::TextBox box(skRect, SPText::TextDirection::RTL);
    TextRect textRect = AdapterTxt::Convert(box);
    EXPECT_EQ(textRect.direction == TextDirection::RTL, true);
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
    EXPECT_EQ(rectHeightStyle == SPText::RectHeightStyle::TIGHT, true);
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
    EXPECT_EQ(rectWidthStyle == SPText::RectWidthStyle::MAX, true);
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
    EXPECT_EQ(paragraphStyle.fontWeight == SPText::FontWeight::W400, true);
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
    EXPECT_EQ(placeholderRun.alignment == SPText::PlaceholderAlignment::BASELINE, true);
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
    // The default fontsize for TextStyle is 14.0
    EXPECT_EQ(sptextStyle.fontSize == 14.0, true);
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
    textStyle.backgroundBrush = Drawing::Brush();
    textStyle.foregroundBrush = Drawing::Brush();
    textStyle.backgroundPen = Drawing::Pen();
    textStyle.foregroundPen = Drawing::Pen();
    std::vector<TextShadow> shadows = { {}, {} };

    SPText::TextStyle sptextStyle = AdapterTxt::Convert(textStyle);

    EXPECT_EQ(sptextStyle.fontSize == fontParam, true);
    EXPECT_EQ(sptextStyle.background.has_value(), true);
    EXPECT_EQ(sptextStyle.foreground.has_value(), true);
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
    sptextStyle.background = SPText::PaintRecord(Drawing::Brush(), Drawing::Pen());
    sptextStyle.foreground = SPText::PaintRecord(Drawing::Brush(), Drawing::Pen());
    std::vector<TextShadow> TextShadow = { {}, {} };

    TextStyle textStyle = AdapterTxt::Convert(sptextStyle);

    EXPECT_EQ(textStyle.fontSize == spFontParam, true);
    EXPECT_EQ(textStyle.fontFeatures.GetFeatureSettings().empty(), false);
    EXPECT_EQ(textStyle.fontFeatures.GetFontFeatures().size() > 0, true);
    EXPECT_EQ(textStyle.fontVariations.GetAxisValues().empty(), false);
    EXPECT_EQ(textStyle.isSymbolGlyph, true);
    EXPECT_EQ(textStyle.backgroundBrush.has_value(), true);
    EXPECT_EQ(textStyle.foregroundBrush.has_value(), true);
}
} // namespace Rosen
} // namespace OHOS