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
#include "placeholder_run.h"
#include "rosen_converter_txt.h"
#include "text_style.h"

using namespace rosen;
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
    style.fontWeight_ = FontWeight::W100;
    txt::TextStyle txtStyle;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w100);

    style.fontWeight_ = FontWeight::W200;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w200);

    style.fontWeight_ = FontWeight::W300;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w300);

    style.fontWeight_ = FontWeight::W400;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w400);

    style.fontWeight_ = FontWeight::W500;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w500);

    style.fontWeight_ = FontWeight::W600;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w600);

    style.fontWeight_ = FontWeight::W700;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w700);

    style.fontWeight_ = FontWeight::W800;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_weight, txt::FontWeight::w800);

    style.fontWeight_ = FontWeight::W900;
    RosenConvertTxtStyle(style, txtStyle);
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
    style.fontStyle_ = FontStyle::NORMAL;
    txt::TextStyle txtStyle;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.font_style, txt::FontStyle::normal);

    style.fontStyle_ = FontStyle::ITALIC;
    RosenConvertTxtStyle(style, txtStyle);
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
    style.textBaseline_ = TextBaseline::ALPHABETIC;
    txt::TextStyle txtStyle;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_baseline, txt::TextBaseline::kAlphabetic);

    style.textBaseline_ = TextBaseline::IDEOGRAPHIC;
    RosenConvertTxtStyle(style, txtStyle);
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
    style.decoration_ = TextDecoration::NONE;
    txt::TextStyle txtStyle;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kNone);

    style.decoration_ = TextDecoration::UNDERLINE;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kUnderline);

    style.decoration_ = TextDecoration::OVERLINE;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.decoration, txt::TextDecoration::kOverline);

    style.decoration_ = TextDecoration::LINETHROUGH;
    RosenConvertTxtStyle(style, txtStyle);
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
    style.decorationStyle_ = TextDecorationStyle::SOLID;
    txt::TextStyle txtStyle;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kSolid);

    style.decorationStyle_ = TextDecorationStyle::DOUBLE;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDouble);

    style.decorationStyle_ = TextDecorationStyle::DOTTED;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDotted);

    style.decorationStyle_ = TextDecorationStyle::DASHED;
    RosenConvertTxtStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.decoration_style, txt::TextDecorationStyle::kDashed);

    style.decorationStyle_ = TextDecorationStyle::WAVY;
    RosenConvertTxtStyle(style, txtStyle);
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
    style.breakStrategy_ = BreakStrategy::BreakStrategyGreedy;
    txt::ParagraphStyle txtStyle;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_Greedy);

    style.breakStrategy_ = BreakStrategy::BreakStrategyHighQuality;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_HighQuality);

    style.breakStrategy_ = BreakStrategy::BreakStrategyBalanced;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.break_strategy, minikin::BreakStrategy::kBreakStrategy_Balanced);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest007
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest007, TestSize.Level1)
{
    TypographyStyle style;
    style.wordBreakType_ = WordBreakType::WordBreakTypeNormal;
    txt::ParagraphStyle txtStyle;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_Normal);

    style.wordBreakType_ = WordBreakType::WordBreakTypeBreakAll;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_BreakAll);

    style.wordBreakType_ = WordBreakType::WordBreakTypeBreakWord;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.word_break_type, minikin::WordBreakType::kWordBreakType_BreakWord);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest008
 * @tc.desc: test for text direction
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest008, TestSize.Level1)
{
    TypographyStyle style;
    style.textDirection_ = TextDirection::RTL;
    txt::ParagraphStyle txtStyle;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_direction, txt::TextDirection::rtl);
    EXPECT_EQ(TxtConvertRosenTextDirection(txtStyle.text_direction), style.textDirection_);

    style.textDirection_ = TextDirection::LTR;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_direction, txt::TextDirection::ltr);
    EXPECT_EQ(TxtConvertRosenTextDirection(txtStyle.text_direction), style.textDirection_);
}

/*
 * @tc.name: OH_Drawing_RosenConverterTest009
 * @tc.desc: test for text align
 * @tc.type: FUNC
 */

HWTEST_F(OH_Drawing_RosenConverterTest, OH_Drawing_RosenConverterTest009, TestSize.Level1)
{
    TypographyStyle style;
    style.textAlign_ = TextAlign::LEFT;
    txt::ParagraphStyle txtStyle;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::left);

    style.textAlign_ = TextAlign::RIGHT;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::right);

    style.textAlign_ = TextAlign::CENTER;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::center);

    style.textAlign_ = TextAlign::JUSTIFY;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::justify);

    style.textAlign_ = TextAlign::START;
    RosenConvertTypographyStyle(style, txtStyle);
    EXPECT_EQ(txtStyle.text_align, txt::TextAlign::start);

    style.textAlign_ = TextAlign::END;
    RosenConvertTypographyStyle(style, txtStyle);
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
    style.textShadows_.push_back(textShadow);
    txt::TextStyle txtStyle;
    RosenConvertTxtStyle(style, txtStyle);
#ifdef USE_CANVASKIT0310_SKIA
    EXPECT_EQ(txtStyle.text_shadows[0].blur_sigma, 0.0);
#else
    EXPECT_EQ(txtStyle.text_shadows[0].blur_radius, 0.0);
#endif
}
}