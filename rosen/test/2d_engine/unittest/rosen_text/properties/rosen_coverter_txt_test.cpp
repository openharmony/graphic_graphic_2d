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
#ifndef USE_GRAPHIC_TEXT_GINE
#include "placeholder_run.h"
#include "rosen_converter_txt.h"
#include "text_style.h"
#else
#include "txt/convert.h"
#include "rosen_text/text_style.h"
#endif

#ifndef USE_GRAPHIC_TEXT_GINE
using namespace rosen;
#else
using namespace OHOS::Rosen;
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W100;
#else
    style.fontWeight = FontWeight::W100;
#endif
    txt::TextStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTxtStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w100);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W200;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W200;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w200);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W300;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W300;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w300);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W400;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W400;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w400);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W500;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W500;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w500);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W600;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W600;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w600);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W700;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W700;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w700);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W800;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W800;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w800);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontWeight_ = FontWeight::W900;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontWeight = FontWeight::W900;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontStyle_ = FontStyle::NORMAL;
#else
    style.fontStyle = FontStyle::NORMAL;
#endif
    txt::TextStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTxtStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.font_style, txt::FontStyle::normal);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.fontStyle_ = FontStyle::ITALIC;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.fontStyle = FontStyle::ITALIC;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.textBaseline_ = TextBaseline::ALPHABETIC;
#else
    style.baseline = TextBaseline::ALPHABETIC;
#endif
    txt::TextStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTxtStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_baseline, txt::TextBaseline::kAlphabetic);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.textBaseline_ = TextBaseline::IDEOGRAPHIC;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.baseline = TextBaseline::IDEOGRAPHIC;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.decoration_ = TextDecoration::NONE;
#else
    style.decoration = TextDecoration::NONE;
#endif
    txt::TextStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTxtStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kNone);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.decoration_ = TextDecoration::UNDERLINE;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.decoration = TextDecoration::UNDERLINE;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kUnderline);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.decoration_ = TextDecoration::OVERLINE;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.decoration = TextDecoration::OVERLINE;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kOverline);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.decoration_ = TextDecoration::LINETHROUGH;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.decoration = TextDecoration::LINE_THROUGH;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.decorationStyle_ = TextDecorationStyle::SOLID;
#else
    style.decorationStyle = TextDecorationStyle::SOLID;
#endif
    txt::TextStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTxtStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kSolid);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.decorationStyle_ = TextDecorationStyle::DOUBLE;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.decorationStyle = TextDecorationStyle::DOUBLE;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDouble);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.decorationStyle_ = TextDecorationStyle::DOTTED;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.decorationStyle = TextDecorationStyle::DOTTED;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDotted);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.decorationStyle_ = TextDecorationStyle::DASHED;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.decorationStyle = TextDecorationStyle::DASHED;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDashed);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.decorationStyle_ = TextDecorationStyle::WAVY;
    RosenConvertTxtStyle(style, txtStyle);
#else
    style.decorationStyle = TextDecorationStyle::WAVY;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.breakStrategy_ = BreakStrategy::BreakStrategyGreedy;
#else
    style.breakStrategy = BreakStrategy::GREEDY;
#endif
    txt::ParagraphStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTypographyStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_Greedy);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.breakStrategy_ = BreakStrategy::BreakStrategyHighQuality;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.breakStrategy = BreakStrategy::HIGH_QUALITY;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_HighQuality);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.breakStrategy_ = BreakStrategy::BreakStrategyBalanced;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.breakStrategy = BreakStrategy::BALANCED;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.wordBreakType_ = WordBreakType::WordBreakTypeNormal;
#else
    style.wordBreakType = WordBreakType::BREAK_WORD;
#endif
    txt::ParagraphStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTypographyStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_Normal);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.wordBreakType_ = WordBreakType::WordBreakTypeBreakAll;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.wordBreakType = WordBreakType::BREAK_ALL;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_BreakAll);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.wordBreakType_ = WordBreakType::WordBreakTypeBreakWord;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.wordBreakType = WordBreakType::BREAK_WORD;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.textDirection_ = TextDirection::RTL;
#else
    style.textDirection = TextDirection::RTL;
#endif
    txt::ParagraphStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTypographyStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_direction, txt::TextDirection::rtl);
#ifndef USE_GRAPHIC_TEXT_GINE
    EXPECT_EQ(TxtConvertRosenTextDirection(txtStyle.text_direction), style.textDirection_);
#endif

#ifndef USE_GRAPHIC_TEXT_GINE
    style.textDirection_ = TextDirection::LTR;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.textDirection = TextDirection::LTR;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_direction, txt::TextDirection::ltr);
#ifndef USE_GRAPHIC_TEXT_GINE
    EXPECT_EQ(TxtConvertRosenTextDirection(txtStyle.text_direction), style.textDirection_);
#endif
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest009
 * @tc.desc: test for text align
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest009, TestSize.Level1)
{
    TypographyStyle style;
#ifndef USE_GRAPHIC_TEXT_GINE
    style.textAlign_ = TextAlign::LEFT;
#else
    style.textAlign = TextAlign::LEFT;
#endif
    txt::ParagraphStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTypographyStyle(style, txtStyle);
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::left);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.textAlign_ = TextAlign::RIGHT;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.textAlign = TextAlign::RIGHT;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::right);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.textAlign_ = TextAlign::CENTER;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.textAlign = TextAlign::CENTER;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::center);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.textAlign_ = TextAlign::JUSTIFY;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.textAlign = TextAlign::JUSTIFY;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::justify);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.textAlign_ = TextAlign::START;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.textAlign = TextAlign::START;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::start);

#ifndef USE_GRAPHIC_TEXT_GINE
    style.textAlign_ = TextAlign::END;
    RosenConvertTypographyStyle(style, txtStyle);
#else
    style.textAlign = TextAlign::END;
    txtStyle = Rosen::AdapterTxt::Convert(style);
#endif
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
#ifndef USE_GRAPHIC_TEXT_GINE
    style.textShadows_.push_back(textShadow);
#else
    style.shadows.push_back(textShadow);
#endif
    txt::TextStyle txtStyle;
#ifndef USE_GRAPHIC_TEXT_GINE
    RosenConvertTxtStyle(style, txtStyle);
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    EXPECT_EQ(txtStyle.text_shadows[0].blur_sigma, 0.0);
#else
    EXPECT_EQ(txtStyle.text_shadows[0].blur_radius, 0.0);
#endif
#else
    txtStyle = Rosen::AdapterTxt::Convert(style);
    EXPECT_EQ(txtStyle.text_shadows[0].blur_sigma, 0.0);
#endif
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest011
 * @tc.desc: test for text style conversion
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest011, TestSize.Level1)
{
    TextStyle style;
    TextStyle placeholderRun;
#ifndef USE_GRAPHIC_TEXT_GINE
    txt::PlaceholderRun RosenConvertPlaceholderRun();
#endif
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest012
 * @tc.desc: test for text style conversion
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest012, TestSize.Level1)
{
    TextStyle style;
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::TextBox TxtConvertRosenTextBox();
#endif
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest012
 * @tc.desc: test for text style conversion
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest013, TestSize.Level1)
{
    TextStyle style;
    TextStyle placeholderRun;
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::Affinity TxtConvertRosenAffinity();
#endif
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest012
 * @tc.desc: test for text style conversion
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest014, TestSize.Level1)
{
    TextStyle style;
    TextStyle placeholderRun;
#ifndef USE_GRAPHIC_TEXT_GINE
    TypographyProperties::PositionAndAffinity TxtConvertPosAndAffinity();
#endif
}
}