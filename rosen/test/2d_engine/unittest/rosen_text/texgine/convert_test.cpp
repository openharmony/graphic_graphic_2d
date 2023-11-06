/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "rosen_text/text_style.h"
#include "convert.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHTexgineRosenConverterTest : public testing::Test {};
namespace {
constexpr int32_t INVALID_VALUE = -1;
}

/*
 * @tc.name: OHTexgineRosenConverterTest001
 * @tc.desc: test for font weight
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest001, TestSize.Level1)
{
    TextStyle style;
    TextEngine::TextStyle textStyle;
    style.fontWeight = FontWeight::W100;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W100);
    style.fontWeight = FontWeight::W200;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W200);
    style.fontWeight = FontWeight::W300;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W300);
    style.fontWeight = FontWeight::W400;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W400);
    style.fontWeight = FontWeight::W500;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W500);
    style.fontWeight = FontWeight::W600;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W600);
    style.fontWeight = FontWeight::W700;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W700);
    style.fontWeight = FontWeight::W900;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W900);
    style.fontWeight = static_cast<FontWeight>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontWeight, TextEngine::FontWeight::W400);
}

/*
 * @tc.name: OHTexgineRosenConverterTest002
 * @tc.desc: test for font style
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest002, TestSize.Level1)
{
    TextStyle style;
    TextEngine::TextStyle textStyle;
    style.fontStyle = FontStyle::NORMAL;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontStyle, TextEngine::FontStyle::NORMAL);
    style.fontStyle = FontStyle::ITALIC;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontStyle, TextEngine::FontStyle::ITALIC);
    style.fontStyle = static_cast<FontStyle>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.fontStyle, TextEngine::FontStyle::NORMAL);
}

/*
 * @tc.name: OHTexgineRosenConverterTest003
 * @tc.desc: test for baseline
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest003, TestSize.Level1)
{
    TextStyle style;
    style.baseline = TextBaseline::ALPHABETIC;
    TextEngine::TextStyle textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.baseline, TextEngine::TextBaseline::ALPHABETIC);
    style.baseline = TextBaseline::IDEOGRAPHIC;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.baseline, TextEngine::TextBaseline::IDEOGRAPHIC);

    style.baseline = static_cast<TextBaseline>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.baseline, TextEngine::TextBaseline::ALPHABETIC);
}

/*
 * @tc.name: OHTexgineRosenConverterTest004
 * @tc.desc: test for decoration
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest004, TestSize.Level1)
{
    TextStyle style;
    style.decoration = TextDecoration::NONE;
    TextEngine::TextStyle textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decoration, TextEngine::TextDecoration::NONE);

    style.decoration = TextDecoration::UNDERLINE;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decoration, TextEngine::TextDecoration::UNDERLINE);

    style.decoration = TextDecoration::OVERLINE;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decoration, TextEngine::TextDecoration::OVERLINE);

    style.decoration = TextDecoration::LINE_THROUGH;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decoration, TextEngine::TextDecoration::LINE_THROUGH);
}

/*
 * @tc.name: OHTexgineRosenConverterTest005
 * @tc.desc: test for decoration style
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest005, TestSize.Level1)
{
    TextStyle style;
    style.decorationStyle = TextDecorationStyle::SOLID;
    TextEngine::TextStyle textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decorationStyle, TextEngine::TextDecorationStyle::SOLID);

    style.decorationStyle = TextDecorationStyle::DOUBLE;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decorationStyle, TextEngine::TextDecorationStyle::DOUBLE);

    style.decorationStyle = TextDecorationStyle::DOTTED;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decorationStyle, TextEngine::TextDecorationStyle::DOTTED);

    style.decorationStyle = TextDecorationStyle::DASHED;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decorationStyle, TextEngine::TextDecorationStyle::DASHED);

    style.decorationStyle = TextDecorationStyle::WAVY;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decorationStyle, TextEngine::TextDecorationStyle::WAVY);

    style.decorationStyle = static_cast<TextDecorationStyle>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.decorationStyle, TextEngine::TextDecorationStyle::SOLID);
}

/*
 * @tc.name: OHTexgineRosenConverterTest006
 * @tc.desc: test for break strategy
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest006, TestSize.Level1)
{
    TypographyStyle style;
    style.breakStrategy = BreakStrategy::GREEDY;
    TextEngine::TypographyStyle textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.breakStrategy, TextEngine::BreakStrategy::GREEDY);

    style.breakStrategy = BreakStrategy::HIGH_QUALITY;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.breakStrategy, TextEngine::BreakStrategy::HIGH_QUALITY);

    style.breakStrategy = BreakStrategy::BALANCED;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.breakStrategy, TextEngine::BreakStrategy::BALANCED);

    style.breakStrategy = static_cast<BreakStrategy>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.breakStrategy, TextEngine::BreakStrategy::GREEDY);
}

/*
 * @tc.name: OHTexgineRosenConverterTest007
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest007, TestSize.Level1)
{
    WordBreakType style;
    style = WordBreakType::NORMAL;
    TextEngine::WordBreakType textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::WordBreakType::NORMAL);

    style = WordBreakType::BREAK_ALL;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::WordBreakType::BREAK_ALL);

    style = WordBreakType::BREAK_WORD;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::WordBreakType::BREAK_WORD);

    style = static_cast<WordBreakType>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::WordBreakType::NORMAL);
}

/*
 * @tc.name: OHTexgineRosenConverterTest008
 * @tc.desc: test for text direction
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest008, TestSize.Level1)
{
    TypographyStyle style;
    style.textDirection = TextDirection::RTL;
    TextEngine::TypographyStyle textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.direction, TextEngine::TextDirection::RTL);

    style.textDirection = TextDirection::LTR;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.direction, TextEngine::TextDirection::LTR);

    style.textDirection = static_cast<TextDirection>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.direction, TextEngine::TextDirection::LTR);
}

/*
 * @tc.name: OHTexgineRosenConverterTest009
 * @tc.desc: test for text align
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest009, TestSize.Level1)
{
    TypographyStyle style;
    style.textAlign = TextAlign::LEFT;
    TextEngine::TypographyStyle textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.align, TextEngine::TextAlign::LEFT);

    style.textAlign = TextAlign::RIGHT;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.align, TextEngine::TextAlign::RIGHT);

    style.textAlign = TextAlign::CENTER;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.align, TextEngine::TextAlign::CENTER);

    style.textAlign = TextAlign::JUSTIFY;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.align, TextEngine::TextAlign::JUSTIFY);

    style.textAlign = TextAlign::START;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.align, TextEngine::TextAlign::START);

    style.textAlign = TextAlign::END;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.align, TextEngine::TextAlign::END);

    style.textAlign = static_cast<TextAlign>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.align, TextEngine::TextAlign::START);
}

/*
 * @tc.name: OHTexgineRosenConverterTest010
 * @tc.desc: test for text rect width style
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest010, TestSize.Level1)
{
    TextRectWidthStyle style = TextRectWidthStyle::TIGHT;
    TextEngine::TextRectWidthStyle textStyle;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectWidthStyle::TIGHT);

    style = TextRectWidthStyle::MAX;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectWidthStyle::MAX_WIDTH);

    style = static_cast<TextRectWidthStyle>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectWidthStyle::TIGHT);
}

/*
 * @tc.name: OHTexgineRosenConverterTest011
 * @tc.desc: test for text rect height style
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest011, TestSize.Level1)
{
    TextRectHeightStyle style;
    TextEngine::TextRectHeightStyle textStyle;

    style = TextRectHeightStyle::TIGHT;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectHeightStyle::TIGHT);

    style = TextRectHeightStyle::COVER_TOP_AND_BOTTOM;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectHeightStyle::COVER_TOP_AND_BOTTOM);

    style = TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM);

    style = TextRectHeightStyle::COVER_TOP;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectHeightStyle::COVER_TOP);

    style = TextRectHeightStyle::COVER_BOTTOM;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectHeightStyle::COVER_BOTTOM);

    style = TextRectHeightStyle::FOLLOW_BY_STRUT;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectHeightStyle::FOLLOW_BY_LINE_STYLE);

    style = static_cast<TextRectHeightStyle>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::TextRectHeightStyle::TIGHT);
}

/*
 * @tc.name: OHTexgineRosenConverterTest012
 * @tc.desc: test for text boundary
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest012, TestSize.Level1)
{
    int32_t leftIndex = 1;
    int32_t rightIndex = 2;
    TextEngine::Boundary style = { leftIndex, rightIndex };
    Boundary textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.leftIndex, style.leftIndex);
    EXPECT_EQ(textStyle.rightIndex, style.rightIndex);
}

/*
 * @tc.name: OHTexgineRosenConverterTest013
 * @tc.desc: test for text rect
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest013, TestSize.Level1)
{
    TextEngine::TextRect style;
    *style.rect.fLeft_ = 1.0;
    *style.rect.fTop_ = 2.0;
    *style.rect.fRight_ = 3.0;
    *style.rect.fBottom_ = 4.0;
    TextRect textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.rect.GetLeft(), *style.rect.fLeft_);
    EXPECT_EQ(textStyle.rect.GetTop(), *style.rect.fTop_);
    EXPECT_EQ(textStyle.rect.GetRight(), *style.rect.fRight_);
    EXPECT_EQ(textStyle.rect.GetBottom(), *style.rect.fBottom_);

    style.direction = TextEngine::TextDirection::LTR;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.direction, TextDirection::LTR);

    style.direction = TextEngine::TextDirection::RTL;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.direction, TextDirection::RTL);
}

/*
 * @tc.name: OHTexgineRosenConverterTest014
 * @tc.desc: test for text alignment
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest014, TestSize.Level1)
{
    PlaceholderVerticalAlignment style;
    TextEngine::AnySpanAlignment textStyle;

    style = PlaceholderVerticalAlignment::OFFSET_AT_BASELINE;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::AnySpanAlignment::OFFSET_AT_BASELINE);

    style = PlaceholderVerticalAlignment::ABOVE_BASELINE;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::AnySpanAlignment::ABOVE_BASELINE);

    style = PlaceholderVerticalAlignment::BELOW_BASELINE;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::AnySpanAlignment::BELOW_BASELINE);

    style = PlaceholderVerticalAlignment::TOP_OF_ROW_BOX;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::AnySpanAlignment::TOP_OF_ROW_BOX);

    style = PlaceholderVerticalAlignment::BOTTOM_OF_ROW_BOX;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::AnySpanAlignment::BOTTOM_OF_ROW_BOX);

    style = PlaceholderVerticalAlignment::CENTER_OF_ROW_BOX;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::AnySpanAlignment::CENTER_OF_ROW_BOX);

    style = static_cast<PlaceholderVerticalAlignment>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, TextEngine::AnySpanAlignment::OFFSET_AT_BASELINE);
}

/*
 * @tc.name: OHTexgineRosenConverterTest015
 * @tc.desc: test for text affinity
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest015, TestSize.Level1)
{
    TextEngine::Affinity style;
    Affinity textStyle;

    style = TextEngine::Affinity::PREV;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, Affinity::PREV);

    style = TextEngine::Affinity::NEXT;
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, Affinity::NEXT);

    style = static_cast<TextEngine::Affinity>(INVALID_VALUE);
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle, Affinity::PREV);
}

/*
 * @tc.name: OHTexgineRosenConverterTest016
 * @tc.desc: test for text index and affinity
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenConverterTest, OHTexgineRosenConverterTest016, TestSize.Level1)
{
    size_t index = 1;
    TextEngine::Affinity affinity = TextEngine::Affinity::PREV;
    TextEngine::IndexAndAffinity style = { index, affinity };
    IndexAndAffinity textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.index, index);
    EXPECT_EQ(textStyle.affinity, Affinity::PREV);

    affinity = TextEngine::Affinity::NEXT;
    style = { index, affinity };
    textStyle = AdapterTextEngine::Convert(style);
    EXPECT_EQ(textStyle.index, index);
    EXPECT_EQ(textStyle.affinity, Affinity::NEXT);
}
} // namespace Rosen
} // namespace OHOS
