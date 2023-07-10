/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "txt/convert.h"
#include "rosen_text/text_style.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
class OH_Drawing_RosenConverterTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_RosenConverterTest001
 * @tc.desc: test for font weight
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest001, TestSize.Level1)
{
    TextStyle style;
    style.fontWeight = FontWeight::W100;
    txt::TextStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w100);

    style.fontWeight = FontWeight::W200;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w200);

    style.fontWeight = FontWeight::W300;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w300);

    style.fontWeight = FontWeight::W400;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w400);

    style.fontWeight = FontWeight::W500;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w500);

    style.fontWeight = FontWeight::W600;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w600);

    style.fontWeight = FontWeight::W700;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w700);

    style.fontWeight = FontWeight::W800;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w800);

    style.fontWeight = FontWeight::W900;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w900);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest002
 * @tc.desc: test for font style
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest002, TestSize.Level1)
{
    TextStyle style;
    style.fontStyle = FontStyle::NORMAL;
    txt::TextStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_style, txt::FontStyle::normal);

    style.fontStyle = FontStyle::ITALIC;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.font_style, txt::FontStyle::italic);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest003
 * @tc.desc: test for baseline
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest003, TestSize.Level1)
{
    TextStyle style;
    style.baseline = TextBaseline::ALPHABETIC;
    txt::TextStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_baseline, txt::TextBaseline::kAlphabetic);

    style.baseline = TextBaseline::IDEOGRAPHIC;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_baseline, txt::TextBaseline::kIdeographic);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest004
 * @tc.desc: test for decoration
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest004, TestSize.Level1)
{
    TextStyle style;
    style.decoration = TextDecoration::NONE;
    txt::TextStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kNone);

    style.decoration = TextDecoration::UNDERLINE;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kUnderline);

    style.decoration = TextDecoration::OVERLINE;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kOverline);

    style.decoration = TextDecoration::LINE_THROUGH;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kLineThrough);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest005
 * @tc.desc: test for decoration style
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest005, TestSize.Level1)
{
    TextStyle style;
    style.decorationStyle = TextDecorationStyle::SOLID;
    txt::TextStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kSolid);

    style.decorationStyle = TextDecorationStyle::DOUBLE;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDouble);

    style.decorationStyle = TextDecorationStyle::DOTTED;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDotted);

    style.decorationStyle = TextDecorationStyle::DASHED;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDashed);

    style.decorationStyle = TextDecorationStyle::WAVY;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kWavy);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest006
 * @tc.desc: test for break strategy
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest006, TestSize.Level1)
{
    TypographyStyle style;
    style.breakStrategy = BreakStrategy::GREEDY;
    txt::ParagraphStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_Greedy);

    style.breakStrategy = BreakStrategy::HIGH_QUALITY;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_HighQuality);

    style.breakStrategy = BreakStrategy::BALANCED;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_Balanced);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest007
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */
#ifndef NEW_SKIA
HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest007, TestSize.Level1)
{
    TypographyStyle style;
    style.wordBreakType = WordBreakType::BREAK_WORD;
    txt::ParagraphStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_Normal);

    style.wordBreakType = WordBreakType::BREAK_ALL;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_BreakAll);

    style.wordBreakType = WordBreakType::BREAK_WORD;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_BreakWord);
}
#endif
/*
 * @tc.name: OH_Drawing_RosenConverterTest008
 * @tc.desc: test for text direction
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest008, TestSize.Level1)
{
    TypographyStyle style;
    style.textDirection = TextDirection::RTL;
    txt::ParagraphStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_direction, txt::TextDirection::rtl);

    style.textDirection = TextDirection::LTR;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_direction, txt::TextDirection::ltr);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest009
 * @tc.desc: test for text align
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest009, TestSize.Level1)
{
    TypographyStyle style;
    style.textAlign = TextAlign::LEFT;
    txt::ParagraphStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::left);

    style.textAlign = TextAlign::RIGHT;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::right);

    style.textAlign = TextAlign::CENTER;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::center);

    style.textAlign = TextAlign::JUSTIFY;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::justify);

    style.textAlign = TextAlign::START;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::start);

    style.textAlign = TextAlign::END;
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::end);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest010
 * @tc.desc: test for text style conversion
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest010, TestSize.Level1)
{
    OHOS::Rosen::Drawing::Point offset;
    TextShadow textShadow(Rosen::Drawing::Color::COLOR_BLACK, offset, 0.0);
    TextStyle style;
    style.shadows.push_back(textShadow);
    txt::TextStyle txtStyle;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#ifdef USE_CANVASKIT0310_SKIA
    EXPECT_EQ(txtStyle.text_shadows[0].blur_sigma, 0.0);
#else
    EXPECT_EQ(txtStyle.text_shadows[0].blur_radius, 0.0);
#endif
}
}