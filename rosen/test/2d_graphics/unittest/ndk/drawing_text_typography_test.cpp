/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <fstream>
#include <string>

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_font.h"
#include "drawing_font_collection.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_rect.h"
#include "drawing_point.h"
#include "drawing_text_declaration.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "txt/text_bundle_config_parser.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
const double LEFT_POS = 50.0;
const double RIGHT_POS = 150.0;
const double ARC_FONT_SIZE = 30;
const double MAX_WIDTH = 800.0;
const double SWEEP_DEGREE = 180.0;
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
} // namespace

class OH_Drawing_TypographyTest : public testing::Test {
};

static TypographyStyle* ConvertToOriginalText(OH_Drawing_TypographyStyle* style)
{
    return reinterpret_cast<TypographyStyle*>(style);
}

static TextStyle* ConvertToOriginalText(OH_Drawing_TextStyle* style)
{
    return reinterpret_cast<TextStyle*>(style);
}

static void CheckTypographyRectsForRange(OH_Drawing_Typography* typography)
{
    struct TextBoxTestCase {
        size_t start;
        size_t end;
        OH_Drawing_RectHeightStyle heightStyle;
        OH_Drawing_RectWidthStyle widthStyle;
        float expectedLeft;
        float expectedTop;
        float expectedRight;
        float expectedBottom;
    };
    std::vector<TextBoxTestCase> RANGE_TEST_RESULT = {
        { 0, 2, RECT_HEIGHT_STYLE_MAX, RECT_WIDTH_STYLE_TIGHT, 0.0f, 0.0f, 239.999786f, 360.000000f },
        { 5, 7, RECT_HEIGHT_STYLE_STRUCT, RECT_WIDTH_STYLE_TIGHT, 599.999451f, 60.0f, 839.999207f, 300.0f },
        { 34, 36, RECT_HEIGHT_STYLE_INCLUDELINESPACETOP, RECT_WIDTH_STYLE_TIGHT, 806.039307f, 720.000000f,
            1046.039062f, 1080.000000f },
        { 37, 39, RECT_HEIGHT_STYLE_INCLUDELINESPACEMIDDLE, RECT_WIDTH_STYLE_TIGHT, 0.000000f, 1149.337036f,
            239.999817f, 1509.337036f },
        { 41, 43, RECT_HEIGHT_STYLE_INCLUDELINESPACEBOTTOM, RECT_WIDTH_STYLE_TIGHT, 479.999573f, 1218.674072f,
            719.999329f, 1578.674072f },
        { 45, 47, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT,
            959.999084f, 1218.674072f, 1199.998779f, 1359.314209f },
        { 49, 51, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_MAX, 240.000000f, 1578.674072f, 480.000000f, 1719.314209f }
    };
    for (size_t i = 0; i < RANGE_TEST_RESULT.size(); ++i) {
        auto rect = OH_Drawing_TypographyGetRectsForRange(typography, RANGE_TEST_RESULT[i].start,
            RANGE_TEST_RESULT[i].end, RANGE_TEST_RESULT[i].heightStyle, RANGE_TEST_RESULT[i].widthStyle);
        const auto& testCase = RANGE_TEST_RESULT[i];
        const float left = OH_Drawing_GetLeftFromTextBox(rect, 0);
        const float right = OH_Drawing_GetRightFromTextBox(rect, 0);
        const float top = OH_Drawing_GetTopFromTextBox(rect, 0);
        const float bottom = OH_Drawing_GetBottomFromTextBox(rect, 0);
        EXPECT_NEAR(left, testCase.expectedLeft, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(right, testCase.expectedRight, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(top, testCase.expectedTop, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(bottom, testCase.expectedBottom, FLOAT_DATA_EPSILON);
        OH_Drawing_TypographyDestroyTextBox(rect);
    }
}
/*
 * @tc.name: OH_Drawing_TypographyTest001
 * @tc.desc: test for creating TypographyStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typoStyle, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyStyle(nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest002
 * @tc.desc: test for text direction
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest002, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textDirection, TextDirection::LTR);
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_RTL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textDirection, TextDirection::RTL);
    OH_Drawing_SetTypographyTextDirection(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textDirection, TextDirection::LTR);
    OH_Drawing_SetTypographyTextDirection(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest003
 * @tc.desc: test for text alignment
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest003, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_LEFT);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign, TextAlign::LEFT);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_RIGHT);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign, TextAlign::RIGHT);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_CENTER);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign, TextAlign::CENTER);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_JUSTIFY);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign, TextAlign::JUSTIFY);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_START);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign, TextAlign::START);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_END);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign, TextAlign::END);
    OH_Drawing_SetTypographyTextAlign(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign, TextAlign::LEFT);
    OH_Drawing_SetTypographyTextAlign(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest004
 * @tc.desc: test for max lines
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest004, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 100);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->maxLines, 100);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 200);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->maxLines, 200);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, -100);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->maxLines, 0);
    OH_Drawing_SetTypographyTextMaxLines(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest005
 * @tc.desc: test for creating text style
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest005, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle, nullptr);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest006
 * @tc.desc: test for text color
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest006, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    // black
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->color, 0xFF000000);
    // red
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->color, 0xFFFF0000);
    // blue
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0xFF));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->color, 0xFF0000FF);
    OH_Drawing_SetTextStyleColor(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest007
 * @tc.desc: test for font size
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest007, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(txtStyle, 80);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontSize, 80);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontSize, 40);
    OH_Drawing_SetTextStyleFontSize(txtStyle, -40);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontSize, -40);
    OH_Drawing_SetTextStyleFontSize(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest008
 * @tc.desc: test for font weight
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest008, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_100);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W100);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_200);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W200);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_300);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W300);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W400);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_500);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W500);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_600);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W600);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_700);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W700);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_800);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W800);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_900);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W900);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight, FontWeight::W400);
    OH_Drawing_SetTextStyleFontWeight(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest009
 * @tc.desc: test for baseline location
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest009, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->baseline, TextBaseline::ALPHABETIC);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_IDEOGRAPHIC);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->baseline, TextBaseline::IDEOGRAPHIC);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->baseline, TextBaseline::ALPHABETIC);
    OH_Drawing_SetTextStyleBaseLine(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest010
 * @tc.desc: test for text decoration
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest010, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_NONE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::NONE);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::UNDERLINE);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_OVERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::OVERLINE);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::LINE_THROUGH);
    OH_Drawing_SetTextStyleDecoration(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::NONE);
    OH_Drawing_SetTextStyleDecoration(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest011
 * @tc.desc: test for text decoration color
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest011, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleDecorationColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationColor, 0xFF000000);
    OH_Drawing_SetTextStyleDecorationColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationColor, 0xFFFF0000);
    OH_Drawing_SetTextStyleDecorationColor(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest012
 * @tc.desc: test for font height
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest012, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 0.0);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->heightScale, 0.0);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, -1.0);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->heightScale, -1.0);
    OH_Drawing_SetTextStyleFontHeight(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest013
 * @tc.desc: test for font families
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest013, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    const char* fontFamilies[] = { "Roboto", "Arial" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 2, fontFamilies);
    std::vector<std::string> fontFamiliesResult = { "Roboto", "Arial" };
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontFamilies, fontFamiliesResult);
    OH_Drawing_SetTextStyleFontFamilies(nullptr, 0, nullptr);
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 0, nullptr);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest014
 * @tc.desc: test for font italic
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest014, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontStyle, FontStyle::NORMAL);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_ITALIC);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontStyle, FontStyle::ITALIC);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontStyle, FontStyle::NORMAL);
    OH_Drawing_SetTextStyleFontStyle(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest015
 * @tc.desc: test for font locale
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest015, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleLocale(txtStyle, "en");
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->locale, "en");
    OH_Drawing_SetTextStyleLocale(txtStyle, "zh");
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->locale, "zh");
    OH_Drawing_SetTextStyleLocale(nullptr, "");
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest016
 * @tc.desc: test for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest016, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);

    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    EXPECT_EQ(indents[1], OH_Drawing_TypographyGetIndentsWithIndex(typography, 1));
    EXPECT_EQ(indents[0], OH_Drawing_TypographyGetIndentsWithIndex(typography, 0));
    double maxWidth = 800.0;
    EXPECT_EQ(static_cast<int>(OH_Drawing_TypographyGetAlphabeticBaseline(typography)), 0);
    EXPECT_EQ(static_cast<int>(OH_Drawing_TypographyGetIdeographicBaseline(typography)), 0);
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(cBitmap));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(cBitmap));
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography), 70);
    EXPECT_EQ(static_cast<int>(OH_Drawing_TypographyGetLongestLine(typography)), 200);
    EXPECT_TRUE(OH_Drawing_TypographyGetLongestLine(typography) <= maxWidth);
    EXPECT_EQ(
        OH_Drawing_TypographyGetMinIntrinsicWidth(typography), OH_Drawing_TypographyGetMaxIntrinsicWidth(typography));
    EXPECT_EQ(static_cast<int>(OH_Drawing_TypographyGetAlphabeticBaseline(typography)), 27);
    EXPECT_EQ(static_cast<int>(OH_Drawing_TypographyGetIdeographicBaseline(typography)), 35);
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest017
 * @tc.desc: test for break strategy
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest017, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_GREEDY);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->breakStrategy, BreakStrategy::GREEDY);
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_HIGH_QUALITY);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->breakStrategy, BreakStrategy::HIGH_QUALITY);
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_BALANCED);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->breakStrategy, BreakStrategy::BALANCED);
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->breakStrategy, BreakStrategy::GREEDY);
    OH_Drawing_SetTypographyTextBreakStrategy(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest018
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest018, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_NORMAL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->wordBreakType, WordBreakType::NORMAL);
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->wordBreakType, WordBreakType::BREAK_ALL);
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_WORD);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->wordBreakType, WordBreakType::BREAK_WORD);
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->wordBreakType, WordBreakType::BREAK_WORD);
    OH_Drawing_SetTypographyTextWordBreakType(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest019
 * @tc.desc: test for ellipsis modal
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest019, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, ELLIPSIS_MODAL_HEAD);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->ellipsisModal, EllipsisModal::HEAD);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, ELLIPSIS_MODAL_MIDDLE);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->ellipsisModal, EllipsisModal::MIDDLE);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, ELLIPSIS_MODAL_TAIL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->ellipsisModal, EllipsisModal::TAIL);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->ellipsisModal, EllipsisModal::TAIL);
    OH_Drawing_SetTypographyTextEllipsisModal(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest020
 * @tc.desc: test for decoration style
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest020, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_SOLID);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationStyle, TextDecorationStyle::SOLID);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_DOUBLE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationStyle, TextDecorationStyle::DOUBLE);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_DOTTED);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationStyle, TextDecorationStyle::DOTTED);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_DASHED);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationStyle, TextDecorationStyle::DASHED);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_WAVY);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationStyle, TextDecorationStyle::WAVY);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationStyle, TextDecorationStyle::SOLID);
    OH_Drawing_SetTextStyleDecorationStyle(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest021
 * @tc.desc: test for decoration thickness scale
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest021, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleDecorationThicknessScale(txtStyle, 10);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationThicknessScale, 10);
    OH_Drawing_SetTextStyleDecorationThicknessScale(txtStyle, 20);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationThicknessScale, 20);
    OH_Drawing_SetTextStyleDecorationThicknessScale(txtStyle, -20);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationThicknessScale, -20);
    OH_Drawing_SetTextStyleDecorationThicknessScale(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest022
 * @tc.desc: test for letter spacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest022, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, 10);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->letterSpacing, 10);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, 20);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->letterSpacing, 20);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, -20);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->letterSpacing, -20);
    OH_Drawing_SetTextStyleLetterSpacing(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest023
 * @tc.desc: test for word spacing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest023, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, 10);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->wordSpacing, 10);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, 20);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->wordSpacing, 20);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, -20);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->wordSpacing, -20);
    OH_Drawing_SetTextStyleWordSpacing(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest024
 * @tc.desc: test for ellipsis modal
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest024, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleEllipsisModal(txtStyle, ELLIPSIS_MODAL_HEAD);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->ellipsisModal, EllipsisModal::HEAD);
    OH_Drawing_SetTextStyleEllipsisModal(txtStyle, ELLIPSIS_MODAL_MIDDLE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->ellipsisModal, EllipsisModal::MIDDLE);
    OH_Drawing_SetTextStyleEllipsisModal(txtStyle, ELLIPSIS_MODAL_TAIL);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->ellipsisModal, EllipsisModal::TAIL);
    OH_Drawing_SetTextStyleEllipsisModal(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->ellipsisModal, EllipsisModal::TAIL);
    OH_Drawing_SetTextStyleEllipsisModal(nullptr, 0);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest025
 * @tc.desc: test for set ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest025, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleEllipsis(txtStyle, "...");
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->ellipsis, u"...");
    OH_Drawing_SetTextStyleEllipsis(txtStyle, "hello");
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->ellipsis, u"hello");
    OH_Drawing_SetTextStyleEllipsis(nullptr, nullptr);
    OH_Drawing_SetTextStyleEllipsis(txtStyle, nullptr);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest026
 * @tc.desc: test for typography and txtStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest026, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_PlaceholderSpan placeholderSpan = { 20, 40, ALIGNMENT_OFFSET_AT_BASELINE, TEXT_BASELINE_ALPHABETIC, 10 };
    OH_Drawing_TypographyHandlerAddPlaceholder(handler, &placeholderSpan);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    EXPECT_NE(OH_Drawing_TypographyDidExceedMaxLines(typography), true);
    OH_Drawing_RectHeightStyle heightStyle = RECT_HEIGHT_STYLE_TIGHT;
    OH_Drawing_RectWidthStyle widthStyle = RECT_WIDTH_STYLE_TIGHT;
    auto box = OH_Drawing_TypographyGetRectsForRange(typography, 1, 2, heightStyle, widthStyle);
    EXPECT_NE(box, nullptr);
    OH_Drawing_GetRightFromTextBox(box, 0);
    EXPECT_NE(OH_Drawing_TypographyGetRectsForPlaceholders(typography), nullptr);
    EXPECT_NE(OH_Drawing_TypographyGetGlyphPositionAtCoordinate(typography, 1, 0), nullptr);
    EXPECT_NE(OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 1, 0), nullptr);
    EXPECT_NE(OH_Drawing_TypographyGetWordBoundary(typography, 1), nullptr);
    EXPECT_NE(OH_Drawing_TypographyGetLineTextRange(typography, 1, true), nullptr);
    EXPECT_NE(OH_Drawing_TypographyGetLineCount(typography), 0);
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest027
 * @tc.desc: test for getting line info for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest027, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    int lineNum = 0;
    bool oneLine = true;
    bool includeWhitespace = true;
    OH_Drawing_LineMetrics lineMetrics;
    EXPECT_FALSE(OH_Drawing_TypographyGetLineInfo(typography, lineNum, oneLine, includeWhitespace, nullptr));
    EXPECT_FALSE(OH_Drawing_TypographyGetLineInfo(typography, -1, oneLine, includeWhitespace, &lineMetrics));
    EXPECT_TRUE(OH_Drawing_TypographyGetLineInfo(typography, lineNum, oneLine, includeWhitespace, &lineMetrics));
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest028
 * @tc.desc: test for getting line info for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest028, TestSize.Level1)
{
    OH_Drawing_TextShadow* textShadow = OH_Drawing_CreateTextShadow();
    EXPECT_NE(textShadow, nullptr);
    OH_Drawing_DestroyTextShadow(textShadow);
    OH_Drawing_DestroyTextShadow(nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest029
 * @tc.desc: test for font weight of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest029, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_100);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W100);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_200);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W200);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_300);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W300);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_400);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W400);
    OH_Drawing_SetTypographyTextFontWeight(nullptr, FONT_WEIGHT_100);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest030
 * @tc.desc: test for font style of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest030, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontStyle(typoStyle, FONT_STYLE_NORMAL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontStyle, FontStyle::NORMAL);
    OH_Drawing_SetTypographyTextFontStyle(typoStyle, FONT_STYLE_ITALIC);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontStyle, FontStyle::ITALIC);
    OH_Drawing_SetTypographyTextFontStyle(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontStyle, FontStyle::NORMAL);
    OH_Drawing_SetTypographyTextFontStyle(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest031
 * @tc.desc: test for font family of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest031, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontFamily(typoStyle, "monospace");
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontFamily, "monospace");
    OH_Drawing_SetTypographyTextFontFamily(nullptr, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyStyle(nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest032
 * @tc.desc: test for font size of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest032, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontSize(typoStyle, 80);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontSize, 80);
    OH_Drawing_SetTypographyTextFontSize(typoStyle, 40);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontSize, 40);
    // -1 is invalid value
    OH_Drawing_SetTypographyTextFontSize(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontSize, -1);
    OH_Drawing_SetTypographyTextFontSize(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest033
 * @tc.desc: test for font height of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest033, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontHeight(typoStyle, 0.0);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->heightScale, 0.0);
    // -1 is invalid value
    OH_Drawing_SetTypographyTextFontHeight(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->heightScale, 0);
    OH_Drawing_SetTypographyTextFontHeight(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest034
 * @tc.desc: test for font style of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest034, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextLineStyleFontStyle(typoStyle, FONT_STYLE_NORMAL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontStyle, FontStyle::NORMAL);
    OH_Drawing_SetTypographyTextLineStyleFontStyle(typoStyle, FONT_STYLE_ITALIC);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontStyle, FontStyle::ITALIC);
    OH_Drawing_SetTypographyTextLineStyleFontStyle(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontStyle, FontStyle::NORMAL);
    OH_Drawing_SetTypographyTextLineStyleFontStyle(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest035
 * @tc.desc: test for font weight of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest035, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_100);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W100);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_200);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W200);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_300);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W300);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_400);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W400);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_500);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W500);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest036
 * @tc.desc: test for font size of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest036, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextLineStyleFontSize(typoStyle, 80);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontSize, 80);
    OH_Drawing_SetTypographyTextLineStyleFontSize(typoStyle, 40);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontSize, 40);
    OH_Drawing_SetTypographyTextLineStyleFontSize(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest037
 * @tc.desc: test for font families of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest037, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 1, fontFamilies);
    std::vector<std::string> fontFamiliesResult = { "Roboto" };
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontFamilies, fontFamiliesResult);
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(nullptr, 0, nullptr);
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 0, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest038
 * @tc.desc: test for spacing scale of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest038, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(typoStyle, 1.0);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleSpacingScale, 1.0);
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(typoStyle, 2.0);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleSpacingScale, 2.0);
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest039
 * @tc.desc: test for font height of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest039, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextLineStyleFontHeight(typoStyle, 0.0);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleHeightScale, 0.0);
    OH_Drawing_SetTypographyTextLineStyleFontHeight(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest040
 * @tc.desc: test for line metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest040, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_FontDescriptor* descriptor = OH_Drawing_CreateFontDescriptor();
    OH_Drawing_FontParser* parser = OH_Drawing_CreateFontParser();

    static const std::string FILE_NAME = "/system/fonts/visibility_list.json";
    std::ifstream fileStream(FILE_NAME.c_str());
    if (fileStream.is_open()) {
        size_t fontNum;
        char** list = OH_Drawing_FontParserGetSystemFontList(parser, &fontNum);
        EXPECT_NE(list, nullptr);
        const char* name = list[0];
        EXPECT_NE(OH_Drawing_FontParserGetFontByName(parser, name), nullptr);
        OH_Drawing_DestroySystemFontList(list, fontNum);
    }
    OH_Drawing_DestroyFontParser(parser);
    OH_Drawing_DestroyFontDescriptor(descriptor);
    OH_Drawing_LineMetrics* vectorMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    EXPECT_NE(vectorMetrics, nullptr);
    EXPECT_NE(OH_Drawing_LineMetricsGetSize(vectorMetrics), 0);
    OH_Drawing_DestroyLineMetrics(vectorMetrics);
    OH_Drawing_LineMetrics* metrics = new OH_Drawing_LineMetrics();
    EXPECT_TRUE(OH_Drawing_TypographyGetLineMetricsAt(typography, 0, metrics));
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: OH_Drawing_TypographyTest041
 * @tc.desc: test for font weight of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest041, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_600);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W600);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_700);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W700);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_800);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W800);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, FONT_WEIGHT_900);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W900);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest042
 * @tc.desc: test for text shadow for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest042, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    EXPECT_NE(OH_Drawing_TextStyleGetShadows(txtStyle), nullptr);
    OH_Drawing_TextStyleClearShadows(txtStyle);
    OH_Drawing_TextShadow* textshadows = OH_Drawing_TextStyleGetShadows(txtStyle);
    OH_Drawing_DestroyTextShadows(textshadows);
    OH_Drawing_DestroyTextShadows(nullptr);
    OH_Drawing_TextStyleAddShadow(txtStyle, nullptr);
    OH_Drawing_TextStyleAddShadow(txtStyle, OH_Drawing_CreateTextShadow());
    EXPECT_NE(OH_Drawing_TextStyleGetShadowWithIndex(txtStyle, 0), nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowWithIndex(txtStyle, 10000000), nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowWithIndex(nullptr, 0), nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowCount(txtStyle), 1);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowCount(nullptr), 0);
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: OH_Drawing_TypographyTest043
 * @tc.desc: test for foreground brush for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest043, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_Brush* foregroundBrush = OH_Drawing_BrushCreate();
    uint8_t alpha = 128;
    OH_Drawing_BrushSetAlpha(foregroundBrush, alpha);
    OH_Drawing_SetTextStyleForegroundBrush(txtStyle, nullptr);
    OH_Drawing_SetTextStyleForegroundBrush(txtStyle, foregroundBrush);
    OH_Drawing_Brush* resForegroundBrush = OH_Drawing_BrushCreate();
    OH_Drawing_TextStyleGetForegroundBrush(txtStyle, nullptr);
    OH_Drawing_TextStyleGetForegroundBrush(txtStyle, resForegroundBrush);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(resForegroundBrush), alpha);
    OH_Drawing_BrushDestroy(resForegroundBrush);
    OH_Drawing_BrushDestroy(foregroundBrush);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: OH_Drawing_TypographyTest044
 * @tc.desc: test for effectiveAlignment, isLineUnlimited, isEllipsized for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest044, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);

    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_Font_Metrics fontmetrics;
    EXPECT_TRUE(OH_Drawing_TextStyleGetFontMetrics(typography, txtStyle, &fontmetrics));
    EXPECT_FALSE(OH_Drawing_TextStyleGetFontMetrics(nullptr, txtStyle, &fontmetrics));
    OH_Drawing_DisableFontCollectionFallback(OH_Drawing_CreateFontCollection());
    OH_Drawing_DisableFontCollectionFallback(nullptr);
    OH_Drawing_DisableFontCollectionSystemFont(OH_Drawing_CreateFontCollection());
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, text);
    OH_Drawing_SetTypographyTextLocale(typoStyle, text);
    OH_Drawing_SetTypographyTextSplitRatio(typoStyle, fontSize);
    OH_Drawing_TypographyGetTextStyle(typoStyle);
    EXPECT_EQ(OH_Drawing_TypographyGetEffectiveAlignment(typoStyle), 0);
    EXPECT_TRUE(OH_Drawing_TypographyIsLineUnlimited(typoStyle));
    EXPECT_TRUE(OH_Drawing_TypographyIsEllipsized(typoStyle));
    OH_Drawing_SetTypographyTextStyle(typoStyle, txtStyle);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: OH_Drawing_TypographyTest045
 * @tc.desc: test for background brush for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest045, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);

    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_Brush* backgroundBrush = OH_Drawing_BrushCreate();
    uint8_t backgroundAlpha = 64;
    OH_Drawing_BrushSetAlpha(backgroundBrush, backgroundAlpha);
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyle, nullptr);
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyle, backgroundBrush);
    OH_Drawing_Brush* resBackgroundBrush = OH_Drawing_BrushCreate();
    OH_Drawing_TextStyleGetBackgroundBrush(txtStyle, nullptr);
    OH_Drawing_TextStyleGetBackgroundBrush(txtStyle, resBackgroundBrush);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(resBackgroundBrush), backgroundAlpha);
    OH_Drawing_BrushDestroy(resBackgroundBrush);
    OH_Drawing_BrushDestroy(backgroundBrush);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: OH_Drawing_TypographyTest046
 * @tc.desc: test for background pen for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest046, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);

    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_Pen* backgroundPen = OH_Drawing_PenCreate();
    float backgroundPenWidth = 10;
    OH_Drawing_PenSetWidth(backgroundPen, backgroundPenWidth);
    OH_Drawing_SetTextStyleBackgroundPen(txtStyle, nullptr);
    OH_Drawing_SetTextStyleBackgroundPen(txtStyle, backgroundPen);
    OH_Drawing_Pen* resBackgroundPen = OH_Drawing_PenCreate();
    OH_Drawing_TextStyleGetBackgroundPen(txtStyle, nullptr);
    OH_Drawing_TextStyleGetBackgroundPen(txtStyle, resBackgroundPen);
    EXPECT_EQ(OH_Drawing_PenGetWidth(resBackgroundPen), backgroundPenWidth);
    OH_Drawing_PenDestroy(resBackgroundPen);
    OH_Drawing_PenDestroy(backgroundPen);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: OH_Drawing_TypographyTest047
 * @tc.desc: test for foreground pen for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest047, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_Pen* foregroundPen = OH_Drawing_PenCreate();
    float foregroundPenWidth = 20;
    OH_Drawing_PenSetWidth(foregroundPen, foregroundPenWidth);
    OH_Drawing_SetTextStyleForegroundPen(txtStyle, nullptr);
    OH_Drawing_SetTextStyleForegroundPen(txtStyle, foregroundPen);
    OH_Drawing_Pen* resForegroundPen = OH_Drawing_PenCreate();
    OH_Drawing_TextStyleGetForegroundPen(txtStyle, nullptr);
    OH_Drawing_TextStyleGetForegroundPen(txtStyle, resForegroundPen);
    EXPECT_EQ(OH_Drawing_PenGetWidth(resForegroundPen), foregroundPenWidth);
    OH_Drawing_PenDestroy(resForegroundPen);
    OH_Drawing_PenDestroy(foregroundPen);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: OH_Drawing_TypographyTest048
 * @tc.desc: test for font weight for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest048, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_500);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W500);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_600);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W600);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_700);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W700);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_800);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W800);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, FONT_WEIGHT_900);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W900);
    OH_Drawing_SetTypographyTextFontWeight(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest049
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest049, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    bool halfLeading = true;
    OH_Drawing_SetTypographyTextHalfLeading(typoStyle, halfLeading);
    EXPECT_TRUE(ConvertToOriginalText(typoStyle)->halfLeading);
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(typoStyle, halfLeading);
    EXPECT_TRUE(ConvertToOriginalText(typoStyle)->lineStyleHalfLeading);
    bool uselineStyle = true;
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, uselineStyle);
    EXPECT_TRUE(ConvertToOriginalText(typoStyle)->useLineStyle);
    bool linestyleOnly = false;
    OH_Drawing_SetTypographyTextLineStyleOnly(typoStyle, linestyleOnly);
    EXPECT_FALSE(ConvertToOriginalText(typoStyle)->lineStyleOnly);
    OH_Drawing_SetTypographyTextLineStyleOnly(nullptr, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest050
 * @tc.desc: test for getting numbers for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest050, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, 1);
    EXPECT_EQ(OH_Drawing_TextStyleGetColor(txtStyle), 1);
    EXPECT_EQ(OH_Drawing_TextStyleGetColor(nullptr), 0xFFFFFFFF);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_SOLID);
    EXPECT_EQ(OH_Drawing_TextStyleGetDecorationStyle(txtStyle), 0);
    EXPECT_EQ(OH_Drawing_TextStyleGetDecorationStyle(nullptr), TEXT_DECORATION_STYLE_SOLID);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_100);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyle), 0);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(nullptr), FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(txtStyle), 0);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(nullptr), FONT_STYLE_NORMAL);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaseline(txtStyle), 0);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaseline(nullptr), TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    size_t fontFamiliesNumber;
    char** fontFamiliesList = OH_Drawing_TextStyleGetFontFamilies(txtStyle, &fontFamiliesNumber);
    EXPECT_NE(fontFamiliesList, nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFamilies(nullptr, &fontFamiliesNumber), nullptr);
    OH_Drawing_TextStyleDestroyFontFamilies(fontFamiliesList, fontFamiliesNumber);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 60); // 60 means font size for test
    EXPECT_EQ(OH_Drawing_TextStyleGetFontSize(txtStyle), 60);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontSize(nullptr), 0.0);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, 20); // 20 means letter spacing for test
    EXPECT_EQ(OH_Drawing_TextStyleGetLetterSpacing(txtStyle), 20);
    EXPECT_EQ(OH_Drawing_TextStyleGetLetterSpacing(nullptr), 0.0);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, 80); // 80 means word spacing for test
    EXPECT_EQ(OH_Drawing_TextStyleGetWordSpacing(txtStyle), 80);
    EXPECT_EQ(OH_Drawing_TextStyleGetWordSpacing(nullptr), 0.0);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 0.0); // 0.0 means font height for test
    EXPECT_EQ(OH_Drawing_TextStyleGetFontHeight(txtStyle), 0.0);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontHeight(nullptr), 0.0);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    EXPECT_TRUE(OH_Drawing_TextStyleGetHalfLeading(txtStyle));
    EXPECT_FALSE(OH_Drawing_TextStyleGetHalfLeading(nullptr));
    OH_Drawing_SetTextStyleLocale(txtStyle, "en");
    EXPECT_EQ(std::strcmp(OH_Drawing_TextStyleGetLocale(txtStyle), "en"), 0);
    EXPECT_EQ(OH_Drawing_TextStyleGetLocale(nullptr), nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest051
 * @tc.desc: test for getting line info for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest051, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_RectStyle_Info rectStyleInfo = { 1, 1.5, 1.5, 1.5, 1.5 }; // 1.5 means corner radius for test
    int styleId = 1;                                                     // 1 means styleId for test
    OH_Drawing_TextStyleSetBackgroundRect(txtStyle, nullptr, styleId);
    OH_Drawing_TextStyleSetBackgroundRect(nullptr, &rectStyleInfo, styleId);
    OH_Drawing_TextStyleSetBackgroundRect(txtStyle, &rectStyleInfo, styleId);
    uint32_t symbol = 2; // 2 means symbol for test
    OH_Drawing_TypographyHandlerAddSymbol(handler, symbol);
    OH_Drawing_TypographyHandlerAddSymbol(nullptr, symbol);
    const char* key1 = "宋体";
    int value1 = 1; // 1 for test
    OH_Drawing_TextStyleAddFontFeature(nullptr, key1, value1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, nullptr, value1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, key1, value1);
    const char* key2 = "斜体";
    int value2 = 2; // 2 for test
    OH_Drawing_TextStyleAddFontFeature(txtStyle, key2, value2);
    const char* key3 = "方体";
    int value3 = 3; // 3 for test
    OH_Drawing_TextStyleAddFontFeature(txtStyle, key3, value3);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFeatureSize(txtStyle), 3); // 3 means font feature size for test
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFeatureSize(nullptr), 0);
    OH_Drawing_FontFeature* fontFeaturesArray = OH_Drawing_TextStyleGetFontFeatures(txtStyle);
    EXPECT_NE(fontFeaturesArray, nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFeatures(nullptr), nullptr);
    OH_Drawing_TextStyleDestroyFontFeatures(fontFeaturesArray, OH_Drawing_TextStyleGetFontFeatureSize(txtStyle));
    OH_Drawing_TextStyleDestroyFontFeatures(nullptr, OH_Drawing_TextStyleGetFontFeatureSize(txtStyle));
    OH_Drawing_TextStyleClearFontFeature(txtStyle);
    OH_Drawing_TextStyleClearFontFeature(nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFeatureSize(txtStyle), 0);
    double lineShift = 1.5; // 1.5 means baseline shift for test
    OH_Drawing_TextStyleSetBaselineShift(nullptr, lineShift);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaselineShift(nullptr), 0.0);
    OH_Drawing_TextStyleSetBaselineShift(txtStyle, lineShift);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaselineShift(txtStyle), 1.5);
}

/*
 * @tc.name: OH_Drawing_TypographyTest052
 * @tc.desc: test for setting the mode of leading over and under text
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest052, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_ALL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textHeightBehavior, TextHeightBehavior::ALL);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_DISABLE_FIRST_ASCENT);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textHeightBehavior, TextHeightBehavior::DISABLE_FIRST_ASCENT);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textHeightBehavior, TextHeightBehavior::DISABLE_LAST_ASCENT);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_DISABLE_ALL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textHeightBehavior, TextHeightBehavior::DISABLE_ALL);
    OH_Drawing_TypographyTextSetHeightBehavior(nullptr, TEXT_HEIGHT_ALL);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest053
 * @tc.desc: test for getting the mode of leading over and under text
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest053, TestSize.Level1)
{
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(nullptr), TEXT_HEIGHT_ALL);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_ALL);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(typoStyle), TEXT_HEIGHT_ALL);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_DISABLE_FIRST_ASCENT);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(typoStyle), TEXT_HEIGHT_DISABLE_FIRST_ASCENT);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(typoStyle), TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_DISABLE_ALL);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(typoStyle), TEXT_HEIGHT_DISABLE_ALL);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest054
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest054, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection, nullptr);
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    ASSERT_NE(handler, nullptr);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyMarkDirty(typography);
    OH_Drawing_TypographyMarkDirty(nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyTest055
 * @tc.desc: test for unresolved glyphs count of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest055, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection, nullptr);
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    ASSERT_NE(handler, nullptr);
    const char* text = "OpenHarmony";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    int32_t result = OH_Drawing_TypographyGetUnresolvedGlyphsCount(typography);
    EXPECT_NE(result, 0);
    const char* unresolvedGlyphText = "\uFFFF";
    OH_Drawing_TypographyHandlerAddText(handler, unresolvedGlyphText);
    typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    result = OH_Drawing_TypographyGetUnresolvedGlyphsCount(typography);
    EXPECT_NE(result, 1);
    result = OH_Drawing_TypographyGetUnresolvedGlyphsCount(nullptr);
    EXPECT_EQ(result, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyTest056
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest056, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection, nullptr);
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    ASSERT_NE(handler, nullptr);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    size_t from = 10;     // 10 means font size for test
    size_t to = 11;       // 11 means font size for test
    float fontSize = 1.0; // 1.0 means font size for test
    OH_Drawing_TypographyUpdateFontSize(typography, from, to, fontSize);
    OH_Drawing_TypographyUpdateFontSize(nullptr, from, to, fontSize);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyTest057
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest057, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    bool useLineStyle = true;
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, useLineStyle);
    bool result = OH_Drawing_TypographyTextGetLineStyle(typoStyle);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyTextGetLineStyle(nullptr);
    EXPECT_FALSE(result);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest058
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest058, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    int weight = FONT_WEIGHT_100;
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, weight);
    OH_Drawing_FontWeight result = OH_Drawing_TypographyTextlineStyleGetFontWeight(typoStyle);
    EXPECT_EQ(result, FONT_WEIGHT_100);
    result = OH_Drawing_TypographyTextlineStyleGetFontWeight(nullptr);
    EXPECT_EQ(result, FONT_WEIGHT_400);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest059
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest059, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    int fontStyle = FONT_STYLE_ITALIC;
    OH_Drawing_SetTypographyTextLineStyleFontStyle(typoStyle, fontStyle);
    OH_Drawing_FontStyle result = OH_Drawing_TypographyTextlineStyleGetFontStyle(typoStyle);
    EXPECT_EQ(result, FONT_STYLE_ITALIC);
    result = OH_Drawing_TypographyTextlineStyleGetFontStyle(nullptr);
    EXPECT_EQ(result, FONT_STYLE_NORMAL);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest060
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest060, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    size_t fontNum = 1; // 1 means font number for test
    const char* fontFamilies[] = { "Roboto" };
    int fontFamiliesNumber = 1; // 1 means font families number for test
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, fontFamiliesNumber, fontFamilies);
    char** result = OH_Drawing_TypographyTextlineStyleGetFontFamilies(typoStyle, &fontNum);
    EXPECT_NE(result, nullptr);
    result = OH_Drawing_TypographyTextlineStyleGetFontFamilies(nullptr, &fontNum);
    EXPECT_EQ(result, nullptr);
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(result, fontNum);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest061
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest061, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    double result = OH_Drawing_TypographyTextlineStyleGetFontSize(typoStyle);
    // 14.0 Fontsize default value
    EXPECT_EQ(result, 14.0);
    result = OH_Drawing_TypographyTextlineStyleGetFontSize(nullptr);
    EXPECT_EQ(result, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest062
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest062, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    double result = OH_Drawing_TypographyTextlineStyleGetHeightScale(typoStyle);
    EXPECT_EQ(result, 1.0); // 1.0 means enable the font height for line styles in text layout only
    result = OH_Drawing_TypographyTextlineStyleGetHeightScale(nullptr);
    EXPECT_EQ(result, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest063
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest063, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    // 2.0 measn font height for test
    double lineStyleFontHeight = 2.0;
    OH_Drawing_SetTypographyTextLineStyleFontHeight(typoStyle, lineStyleFontHeight);
    bool result = OH_Drawing_TypographyTextlineStyleGetHeightOnly(typoStyle);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyTextlineStyleGetHeightOnly(nullptr);
    EXPECT_FALSE(result);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest064
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest064, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    bool lineStyleHalfLeading = true;
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(typoStyle, lineStyleHalfLeading);
    bool result = OH_Drawing_TypographyTextlineStyleGetHalfLeading(typoStyle);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyTextlineStyleGetHalfLeading(nullptr);
    EXPECT_FALSE(result);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest065
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest065, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    double result = OH_Drawing_TypographyTextlineStyleGetSpacingScale(typoStyle);
    // -1.0 for test
    EXPECT_EQ(result, -1.0);
    result = OH_Drawing_TypographyTextlineStyleGetSpacingScale(nullptr);
    EXPECT_EQ(result, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest066
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest066, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    int direction = TEXT_DIRECTION_RTL;
    OH_Drawing_SetTypographyTextDirection(typoStyle, direction);
    OH_Drawing_TextDirection result = OH_Drawing_TypographyGetTextDirection(typoStyle);
    EXPECT_EQ(result, TEXT_DIRECTION_RTL);
    result = OH_Drawing_TypographyGetTextDirection(nullptr);
    EXPECT_EQ(result, TEXT_DIRECTION_LTR);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest067
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest067, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    size_t result = OH_Drawing_TypographyGetTextMaxLines(typoStyle);
    EXPECT_NE(result, 0);
    result = OH_Drawing_TypographyGetTextMaxLines(nullptr);
    EXPECT_EQ(result, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest068
 * @tc.desc: test typography ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest068, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    char* result = OH_Drawing_TypographyGetTextEllipsis(typoStyle);
    EXPECT_NE(result, nullptr);
    result = OH_Drawing_TypographyGetTextEllipsis(nullptr);
    EXPECT_EQ(result, nullptr);
    OH_Drawing_TypographyDestroyEllipsis(result);
    EXPECT_FALSE(OH_Drawing_TypographyIsEllipsized(typoStyle));
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, "");
    EXPECT_FALSE(OH_Drawing_TypographyIsEllipsized(typoStyle));
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, "...");
    EXPECT_TRUE(OH_Drawing_TypographyIsEllipsized(typoStyle));
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest069
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest069, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* from = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyStyle* to = OH_Drawing_CreateTypographyStyle();
    bool result = OH_Drawing_TypographyStyleEquals(from, to);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyStyleEquals(nullptr, to);
    EXPECT_FALSE(result);
    result = OH_Drawing_TypographyStyleEquals(from, nullptr);
    EXPECT_FALSE(result);
    OH_Drawing_DestroyTypographyStyle(from);
    OH_Drawing_DestroyTypographyStyle(to);
}

/*
 * @tc.name: OH_Drawing_TypographyTest070
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest070, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyTextLineStyleOnly(typoStyle, true);
    bool result = OH_Drawing_TypographyTextlineGetStyleOnly(typoStyle);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyTextlineGetStyleOnly(nullptr);
    EXPECT_FALSE(result);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest071
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest071, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    int align = TEXT_ALIGN_RIGHT;
    OH_Drawing_SetTypographyTextAlign(typoStyle, align);
    OH_Drawing_TextAlign result = OH_Drawing_TypographyGetTextAlign(typoStyle);
    EXPECT_EQ(result, TEXT_ALIGN_RIGHT);
    result = OH_Drawing_TypographyGetTextAlign(nullptr);
    EXPECT_EQ(result, TEXT_ALIGN_LEFT);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest072
 * @tc.desc: test for create and releases the memory occupied by system font configuration information
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest072, TestSize.Level1)
{
    OH_Drawing_FontConfigInfoErrorCode code = ERROR_FONT_CONFIG_INFO_UNKNOWN;
    OH_Drawing_FontConfigInfo* configJsonInfo = OH_Drawing_GetSystemFontConfigInfo(&code);
    if (configJsonInfo != nullptr) {
        EXPECT_EQ(code, SUCCESS_FONT_CONFIG_INFO);
    } else {
        EXPECT_NE(code, SUCCESS_FONT_CONFIG_INFO);
    }
    OH_Drawing_DestroySystemFontConfigInfo(configJsonInfo);
    OH_Drawing_DestroySystemFontConfigInfo(nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest073
 * @tc.desc: test for getting all font metrics array from current line
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest073, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle, nullptr);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    size_t charNumber = 0;
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    EXPECT_NE(typography, nullptr);
    OH_Drawing_Font_Metrics* StartLineFont = OH_Drawing_TypographyGetLineFontMetrics(typography, 1, &charNumber);
    EXPECT_EQ(StartLineFont, nullptr);
    OH_Drawing_TypographyDestroyLineFontMetrics(StartLineFont);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest074
 * @tc.desc: test for getting and setting strut style
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest074, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_StrutStyle* strutstyle = new OH_Drawing_StrutStyle();
    strutstyle->weight = FONT_WEIGHT_400;
    strutstyle->style = FONT_STYLE_ITALIC;
    // 17.0 For size
    strutstyle->size = 17.0;
    // 2.0 For heightScale
    strutstyle->heightScale = 2;
    strutstyle->heightOverride = true;
    strutstyle->halfLeading = true;
    // 3.0 For leading
    strutstyle->leading = 3.0;
    strutstyle->forceStrutHeight = true;
    // 4 For families size
    strutstyle->familiesSize = 4;
    strutstyle->families = (char**)malloc(strutstyle->familiesSize * sizeof(char*));
    const char* temp[] = { "1", "2", "3", "4" };
    for (int i = 0; i < strutstyle->familiesSize; i++) {
        // 2 For families member size
        strutstyle->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(strutstyle->families[i], 2, temp[i]);
    }
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutstyle);
    EXPECT_NE(OH_Drawing_TypographyStyleGetStrutStyle(typoStyle), nullptr);
    OH_Drawing_TypographyStyleDestroyStrutStyle(strutstyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest075
 * @tc.desc: test for sets and gets isPlaceholder for TextStyle objects
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest075, TestSize.Level1)
{
    EXPECT_FALSE(OH_Drawing_TextStyleIsPlaceholder(nullptr));
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_FALSE(OH_Drawing_TextStyleIsPlaceholder(txtStyle));
    OH_Drawing_TextStyleSetPlaceholder(nullptr);
    OH_Drawing_TextStyleSetPlaceholder(txtStyle);
    EXPECT_TRUE(OH_Drawing_TextStyleIsPlaceholder(txtStyle));
    EXPECT_TRUE(ConvertToOriginalText(txtStyle)->isPlaceholder);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest076
 * @tc.desc: test for the two TextStyle objects have matching properties
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest076, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyle* txtStyleCompare = OH_Drawing_CreateTextStyle();
    bool result = OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_ALL_ATTRIBUTES);
    EXPECT_TRUE(result);
    OH_Drawing_SetTextStyleLocale(txtStyle, "en");
    result = OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_ALL_ATTRIBUTES);
    EXPECT_FALSE(result);
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(nullptr, txtStyleCompare, TEXT_STYLE_ALL_ATTRIBUTES));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, nullptr, TEXT_STYLE_ALL_ATTRIBUTES));
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(txtStyleCompare);
}

/*
 * @tc.name: OH_Drawing_TypographyTest077
 * @tc.desc: test for strutstyle equals
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest077, TestSize.Level1)
{
    OH_Drawing_StrutStyle* from = new OH_Drawing_StrutStyle();
    OH_Drawing_StrutStyle* to = new OH_Drawing_StrutStyle();
    bool result = OH_Drawing_TypographyStyleStrutStyleEquals(from, to);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyStyleStrutStyleEquals(nullptr, to);
    EXPECT_FALSE(result);
    result = OH_Drawing_TypographyStyleStrutStyleEquals(from, nullptr);
    EXPECT_FALSE(result);
    OH_Drawing_TypographyStyleDestroyStrutStyle(from);
    OH_Drawing_TypographyStyleDestroyStrutStyle(to);
}

/*
 * @tc.name: OH_Drawing_TypographyTest078
 * @tc.desc: test for gets the typoStyle alignment mode and whether to enable text prompts
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest078, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(typoStyle), TEXT_ALIGN_LEFT);
    EXPECT_FALSE(OH_Drawing_TypographyStyleIsHintEnabled(typoStyle));
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest079
 * @tc.desc: test for setting the hinting of text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest079, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyStyleSetHintsEnabled(typoStyle, true);
    EXPECT_TRUE(ConvertToOriginalText(typoStyle)->hintingIsOn);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest080
 * @tc.desc: test for whether two TextStyle objects are equal
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest080, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyle* txtStyleCompare = OH_Drawing_CreateTextStyle();
    bool result = OH_Drawing_TextStyleIsEqual(txtStyle, txtStyleCompare);
    EXPECT_TRUE(result);

    OH_Drawing_SetTextStyleColor(txtStyle, 1);
    result = OH_Drawing_TextStyleIsEqual(txtStyle, txtStyleCompare);
    EXPECT_FALSE(result);
    OH_Drawing_SetTextStyleColor(txtStyleCompare, 1);
    result = OH_Drawing_TextStyleIsEqual(txtStyle, txtStyleCompare);
    EXPECT_TRUE(result);
    EXPECT_FALSE(OH_Drawing_TextStyleIsEqual(nullptr, txtStyleCompare));
    EXPECT_FALSE(OH_Drawing_TextStyleIsEqual(txtStyle, nullptr));
    EXPECT_TRUE(OH_Drawing_TextStyleIsEqual(nullptr, nullptr));
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(txtStyleCompare);
}

/*
 * @tc.name: OH_Drawing_TypographyTest081
 * @tc.desc: test for getting and setting text style
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest081, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle, nullptr);
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;
    OH_Drawing_SetTextStyleFontStyleStruct(txtStyle, normalStyle);
    OH_Drawing_SetTextStyleFontStyleStruct(nullptr, normalStyle);

    OH_Drawing_FontStyleStruct style = OH_Drawing_TextStyleGetFontStyleStruct(txtStyle);
    EXPECT_EQ(style.weight, normalStyle.weight);
    EXPECT_EQ(style.width, normalStyle.width);
    EXPECT_EQ(style.slant, normalStyle.slant);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest082
 * @tc.desc: test for getting and setting typography style
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest082, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typoStyle, nullptr);
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;
    OH_Drawing_SetTypographyStyleFontStyleStruct(typoStyle, normalStyle);
    OH_Drawing_SetTypographyStyleFontStyleStruct(nullptr, normalStyle);

    OH_Drawing_FontStyleStruct style = OH_Drawing_TypographyStyleGetFontStyleStruct(typoStyle);
    EXPECT_EQ(style.weight, normalStyle.weight);
    EXPECT_EQ(style.width, normalStyle.width);
    EXPECT_EQ(style.slant, normalStyle.slant);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest083
 * @tc.desc: test for the font properties of two TextStyle objects are equal
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest083, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyle* txtStyleCompare = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleLocale(txtStyle, "en");
    OH_Drawing_SetTextStyleLocale(txtStyleCompare, "en");
    bool result = OH_Drawing_TextStyleIsEqualByFont(txtStyle, txtStyleCompare);
    EXPECT_TRUE(result);

    OH_Drawing_SetTextStyleLocale(txtStyle, "ch");
    result = OH_Drawing_TextStyleIsEqualByFont(txtStyle, txtStyleCompare);
    EXPECT_FALSE(result);
    EXPECT_FALSE(OH_Drawing_TextStyleIsEqualByFont(nullptr, txtStyleCompare));
    EXPECT_FALSE(OH_Drawing_TextStyleIsEqualByFont(txtStyle, nullptr));
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(txtStyleCompare);
}

/*
 * @tc.name: OH_Drawing_TypographyTest084
 * @tc.desc: test for BREAK_STRATEGY_GREEDY
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest084, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_GREEDY);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "breakStrategyTest breakStrategy breakStrategyGreedyTest";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // {1.2, 3.4} for unit test
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    // 300.0 for unit test
    double maxWidth = 300.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    OH_Drawing_TypographyLayout(nullptr, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
}

/*
 * @tc.name: OH_Drawing_TypographyTest085
 * @tc.desc: test for BREAK_STRATEGY_BALANCED
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest085, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_BALANCED);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "breakStrategyTest breakStrategy breakStrategyBALANCEDTest";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // {1.2, 3.4} for unit test
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    // 300.0 for unit test
    double maxWidth = 300.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
}

/*
 * @tc.name: OH_Drawing_TypographyTest086
 * @tc.desc: test for BREAK_STRATEGY_HIGH_QUALITY
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest086, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_HIGH_QUALITY);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "breakStrategyTest breakStrategy breakStrategyHighQualityTest";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // {1.2, 3.4} for unit test
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    OH_Drawing_TypographySetIndents(nullptr, 0, indents);
    // 300.0 for unit test
    double maxWidth = 300.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
}

/*
 * @tc.name: OH_Drawing_TypographyTest089
 * @tc.desc: test for getting line  metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest089, TestSize.Level1)
{
    OH_Drawing_Typography* typography = nullptr;
    OH_Drawing_LineMetrics* vectorMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    EXPECT_EQ(vectorMetrics, nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest090
 * @tc.desc: test for getting size of line metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest090, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79,
        0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e, 0x73 };
    const char *fontFamilies[] = {fontFamiliesTest};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息获取接口的测试文本";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
 
    OH_Drawing_LineMetrics* lineMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    EXPECT_NE(lineMetrics, nullptr);
    int lineMetricsSize = OH_Drawing_LineMetricsGetSize(lineMetrics);
    EXPECT_EQ(lineMetricsSize, 2);
    OH_Drawing_Font_Metrics metrics = lineMetrics[0].firstCharMetrics;
 
    OH_Drawing_LineMetrics lineMetric;
    OH_Drawing_TypographyGetLineMetricsAt(typography, 0, &lineMetric);
    OH_Drawing_Font_Metrics metric = lineMetric.firstCharMetrics;
 
    EXPECT_EQ(lineMetric.ascender, lineMetrics[0].ascender);
    EXPECT_EQ(lineMetric.descender, lineMetrics[0].descender);
    EXPECT_EQ(lineMetric.width, lineMetrics[0].width);
    EXPECT_EQ(lineMetric.height, lineMetrics[0].height);
    EXPECT_EQ(lineMetric.x, lineMetrics[0].x);
    EXPECT_EQ(lineMetric.y, lineMetrics[0].y);
    EXPECT_EQ(lineMetric.startIndex, lineMetrics[0].startIndex);
    EXPECT_EQ(lineMetric.endIndex, lineMetrics[0].endIndex);
 
    EXPECT_EQ(metric.flags, metrics.flags);
    EXPECT_EQ(metric.top, metrics.top);
    EXPECT_EQ(metric.ascent, metrics.ascent);
    EXPECT_EQ(metric.descent, metrics.descent);
    EXPECT_EQ(metric.avgCharWidth, metrics.avgCharWidth);
    EXPECT_EQ(metric.maxCharWidth, metrics.maxCharWidth);
    EXPECT_EQ(metric.xMin, metrics.xMin);
    EXPECT_EQ(metric.xMax, metrics.xMax);
    EXPECT_EQ(metric.xHeight, metrics.xHeight);
    EXPECT_EQ(metric.underlineThickness, metrics.underlineThickness);
    EXPECT_EQ(metric.underlinePosition, metrics.underlinePosition);
    EXPECT_EQ(metric.strikeoutThickness, metrics.strikeoutThickness);
    EXPECT_EQ(metric.strikeoutPosition, metrics.strikeoutPosition);
 
    OH_Drawing_DestroyLineMetrics(lineMetrics);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest091
 * @tc.desc: test returning line metrics info for the line text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest091, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79,
        0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e, 0x73 };
    const char *fontFamilies[] = {fontFamiliesTest};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息获取接口的测试文本";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
 
    OH_Drawing_LineMetrics lineMetric;
    OH_Drawing_TypographyGetLineMetricsAt(typography, 0, &lineMetric);
    EXPECT_NEAR(lineMetric.ascender, 46.399998, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(lineMetric.descender, 12.200000, FLOAT_DATA_EPSILON);
    EXPECT_EQ(lineMetric.capHeight, 35.0);
    EXPECT_EQ(lineMetric.xHeight, 25.0);
    EXPECT_NEAR(lineMetric.width, 799.999146, FLOAT_DATA_EPSILON);
    EXPECT_EQ(lineMetric.height, 59.0);
    EXPECT_EQ(lineMetric.x, 0.0);
    EXPECT_EQ(lineMetric.y, 0.0);
    EXPECT_EQ(lineMetric.startIndex, 0);
    EXPECT_EQ(lineMetric.endIndex, 16);
 
    OH_Drawing_Font_Metrics metric = lineMetric.firstCharMetrics;
    EXPECT_NEAR(metric.top, -52.799999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metric.underlineThickness, 2.500000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metric.underlinePosition, 10.350000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metric.strikeoutThickness, 2.500000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metric.strikeoutPosition, -15.000001, FLOAT_DATA_EPSILON);
 
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest092
 * @tc.desc: test for getting indets of index for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest092, TestSize.Level1)
{
    OH_Drawing_Typography* typography = nullptr;
    EXPECT_EQ(0.0, OH_Drawing_TypographyGetIndentsWithIndex(typography, 1));
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    typography = OH_Drawing_CreateTypography(handler);
    EXPECT_EQ(0.0, OH_Drawing_TypographyGetIndentsWithIndex(typography, -1));
    // {1.2, 3.4} for unit test
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    int indexOutOfBounds = 3;
    EXPECT_EQ(0, OH_Drawing_TypographyGetIndentsWithIndex(nullptr, 0));
    EXPECT_EQ(indents[1], OH_Drawing_TypographyGetIndentsWithIndex(typography, indexOutOfBounds));
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest093
 * @tc.desc: test for getting line font metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest093, TestSize.Level1)
{
    size_t charNumber = 0;
    EXPECT_EQ(OH_Drawing_TypographyGetLineFontMetrics(nullptr, 1, &charNumber), nullptr);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    EXPECT_EQ(OH_Drawing_TypographyGetLineFontMetrics(typography, 1, nullptr), nullptr);
    EXPECT_EQ(OH_Drawing_TypographyGetLineFontMetrics(typography, 0, &charNumber), nullptr);
    EXPECT_EQ(OH_Drawing_TypographyGetLineFontMetrics(nullptr, 0, nullptr), nullptr);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest094
 * @tc.desc: test for setting font weight for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest094, TestSize.Level1)
{
    OH_Drawing_SetTypographyTextFontWeight(nullptr, FONT_WEIGHT_100);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W400);
    OH_Drawing_SetTypographyTextFontStyle(nullptr, FONT_STYLE_NORMAL);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest095
 * @tc.desc: test for setting font height for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest095, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    // -1.2 for unit test
    OH_Drawing_SetTypographyTextFontHeight(typoStyle, -1.2);
    OH_Drawing_SetTypographyTextFontHeight(nullptr, 0);
    EXPECT_TRUE(ConvertToOriginalText(typoStyle)->heightOnly);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->heightScale, 0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest096
 * @tc.desc: test for setting half leading for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest096, TestSize.Level1)
{
    bool halfLeading = false;
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextHalfLeading(typoStyle, halfLeading);
    OH_Drawing_SetTypographyTextHalfLeading(nullptr, halfLeading);
    EXPECT_FALSE(ConvertToOriginalText(typoStyle)->halfLeading);
}

/*
 * @tc.name: OH_Drawing_TypographyTest097
 * @tc.desc: test for setting text line style font weight for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest097, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    // -1 for unit test
    int weight = -1;
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, weight);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(nullptr, weight);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W400);
}

/*
 * @tc.name: OH_Drawing_TypographyTest098
 * @tc.desc: test for text line style getting font families for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest098, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    char** result = OH_Drawing_TypographyTextlineStyleGetFontFamilies(typoStyle, nullptr);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest099
 * @tc.desc: test for text line style setting half leading for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest099, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    bool lineStyleHalfLeading = false;
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(typoStyle, lineStyleHalfLeading);
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(nullptr, lineStyleHalfLeading);
    bool result = OH_Drawing_TypographyTextlineStyleGetHalfLeading(typoStyle);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: OH_Drawing_TypographyTest100
 * @tc.desc: test for getting style struct for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest100, TestSize.Level1)
{
    // 0.0 for unit test
    double lineShift = 0.0;
    EXPECT_EQ(OH_Drawing_TypographyStyleGetStrutStyle(nullptr), nullptr);
    OH_Drawing_TextStyleSetBaselineShift(nullptr, lineShift);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaselineShift(nullptr), 0.0);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(nullptr), TEXT_ALIGN_START);
    EXPECT_FALSE(OH_Drawing_TypographyStyleIsHintEnabled(nullptr));
}

/*
 * @tc.name: OH_Drawing_TypographyTest101
 * @tc.desc: test for getting font style struct for text typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest101, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typoStyle, nullptr);
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_900;
    normalStyle.width = FONT_WIDTH_ULTRA_EXPANDED;
    normalStyle.slant = FONT_STYLE_ITALIC;
    OH_Drawing_SetTypographyStyleFontStyleStruct(typoStyle, normalStyle);

    OH_Drawing_FontStyleStruct style = OH_Drawing_TypographyStyleGetFontStyleStruct(typoStyle);
    EXPECT_EQ(style.weight, FONT_WEIGHT_900);
    EXPECT_EQ(style.width, FONT_WIDTH_ULTRA_EXPANDED);
    EXPECT_EQ(style.slant, FONT_STYLE_ITALIC);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest102
 * @tc.desc: test for the font parser
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest102, TestSize.Level1)
{
    OH_Drawing_FontParser* parser = OH_Drawing_CreateFontParser();
    static const std::string FILE_NAME = "/system/fonts/visibility_list.json";
    std::ifstream fileStream(FILE_NAME.c_str());
    if (fileStream.is_open()) {
        size_t fontNum;
        char** list = OH_Drawing_FontParserGetSystemFontList(parser, &fontNum);
        EXPECT_NE(list, nullptr);
        EXPECT_EQ(OH_Drawing_FontParserGetSystemFontList(nullptr, &fontNum), nullptr);
        const char* name = list[0] ;
        EXPECT_NE(OH_Drawing_FontParserGetFontByName(parser, name), nullptr);
        EXPECT_EQ(OH_Drawing_FontParserGetFontByName(nullptr, name), nullptr);
        OH_Drawing_DestroySystemFontList(list, fontNum);
        OH_Drawing_DestroySystemFontList(nullptr, fontNum);
    }
    OH_Drawing_DestroyFontParser(parser);
}

/*
 * @tc.name: OH_Drawing_TypographyTest103
 * @tc.desc: test arc text offset
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest104, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_SetTextStyleFontSize(txtStyle, ARC_FONT_SIZE);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    OH_Drawing_Path* cPath = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(cPath, LEFT_POS, LEFT_POS, RIGHT_POS, RIGHT_POS, 0, SWEEP_DEGREE);

    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_CanvasDrawPath(cCanvas, cPath);
    OH_Drawing_TypographyPaintOnPath(typography, cCanvas, cPath, ARC_FONT_SIZE, ARC_FONT_SIZE);
    OH_Drawing_Font_Metrics fontmetrics;
    EXPECT_TRUE(OH_Drawing_TextStyleGetFontMetrics(typography, txtStyle, &fontmetrics));
    OH_Drawing_SetTypographyTextStyle(typoStyle, txtStyle);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_PathDestroy(cPath);
    OH_Drawing_CanvasDestroy(cCanvas);
}

/*
 * @tc.name: OH_Drawing_TypographyTest104
 * @tc.desc: test arc text drawing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest103, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_SetTextStyleFontSize(txtStyle, ARC_FONT_SIZE);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    OH_Drawing_Path* cPath = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(cPath, LEFT_POS, LEFT_POS, RIGHT_POS, RIGHT_POS, 0, SWEEP_DEGREE);

    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_CanvasDrawPath(cCanvas, cPath);
    OH_Drawing_TypographyPaintOnPath(typography, cCanvas, cPath, ARC_FONT_SIZE, ARC_FONT_SIZE);
    OH_Drawing_Font_Metrics fontmetrics;
    EXPECT_TRUE(OH_Drawing_TextStyleGetFontMetrics(typography, txtStyle, &fontmetrics));
    OH_Drawing_SetTypographyTextStyle(typoStyle, txtStyle);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_PathDestroy(cPath);
    OH_Drawing_CanvasDestroy(cCanvas);
}

/*
 * @tc.name: OH_Drawing_TypographyTest105
 * @tc.desc: test for the text box
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest105, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TextBox* textBox = OH_Drawing_TypographyGetRectsForPlaceholders(typography);
    OH_Drawing_GetLeftFromTextBox(textBox, 0);
    OH_Drawing_GetRightFromTextBox(textBox, 0);
    OH_Drawing_GetTopFromTextBox(textBox, 0);
    OH_Drawing_GetBottomFromTextBox(textBox, 0);
    EXPECT_EQ(OH_Drawing_GetTextDirectionFromTextBox(textBox, 0), 0);
    EXPECT_EQ(OH_Drawing_GetSizeOfTextBox(textBox), 0);

    OH_Drawing_PositionAndAffinity* positionAndAffinity =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinate(typography, 1, 0);
    OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    OH_Drawing_GetPositionFromPositionAndAffinity(nullptr);
    OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    OH_Drawing_GetAffinityFromPositionAndAffinity(nullptr);

    OH_Drawing_Range* range = OH_Drawing_TypographyGetWordBoundary(typography, 1);
    OH_Drawing_GetStartFromRange(range);
    OH_Drawing_GetStartFromRange(nullptr);
    OH_Drawing_GetEndFromRange(range);
    OH_Drawing_GetEndFromRange(nullptr);
    OH_Drawing_TypographyGetLineHeight(typography, 1);
    OH_Drawing_TypographyGetLineHeight(nullptr, 1);
    OH_Drawing_TypographyGetLineWidth(typography, 1);
    OH_Drawing_TypographyGetLineWidth(nullptr, 1);
}

/*
 * @tc.name: OH_Drawing_TypographyTest106
 * @tc.desc: test for the textbox.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest106, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());

    OH_Drawing_PlaceholderSpan *placeholderSpan = new OH_Drawing_PlaceholderSpan();
    placeholderSpan->width = 150.0;
    placeholderSpan->height = 160.0;
    OH_Drawing_TypographyHandlerAddPlaceholder(handler, placeholderSpan);
    OH_Drawing_TypographyHandlerAddPlaceholder(handler, placeholderSpan);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    OH_Drawing_TextBox* textBox = OH_Drawing_TypographyGetRectsForPlaceholders(typography);
    int size = OH_Drawing_GetSizeOfTextBox(textBox);
    EXPECT_EQ(size, 2);
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(OH_Drawing_GetLeftFromTextBox(textBox, i), 0.000000 + i * 150.000000);
        EXPECT_EQ(OH_Drawing_GetRightFromTextBox(textBox, i), 150.000000 + i * 150.000000);
        EXPECT_EQ(static_cast<int>(OH_Drawing_GetTopFromTextBox(textBox, i)), 0);
        EXPECT_EQ(static_cast<int>(OH_Drawing_GetBottomFromTextBox(textBox, i)), 159);
    }
    OH_Drawing_TypographyGetRectsForPlaceholders(nullptr);
    EXPECT_NE(textBox, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_TypographyDestroyTextBox(textBox);
}

/*
 * @tc.name: OH_Drawing_TypographyTest107
 * @tc.desc: test for default textshadow.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest107, TestSize.Level1)
{
    // Test default scenario
    OH_Drawing_TextShadow* shadow = OH_Drawing_CreateTextShadow();
    EXPECT_NE(shadow, nullptr);
    uint32_t color = 0;
    OH_Drawing_Point* offset = OH_Drawing_PointCreate(0, 0);
    double blurRadius = 0.0;
    OH_Drawing_SetTextShadow(shadow, color, offset, blurRadius);
    OH_Drawing_DestroyTextShadow(shadow);
    OH_Drawing_PointDestroy(offset);
    OH_Drawing_SetTextShadow(nullptr, color, nullptr, blurRadius);
    OH_Drawing_SetTextShadow(shadow, color, nullptr, blurRadius);
    OH_Drawing_DestroyTextShadow(nullptr);
    OH_Drawing_PointDestroy(nullptr);
    EXPECT_NE(shadow, nullptr);
}

/*
 * @tc.name: OH_Drawing_TextStyleAddShadowTest
 * @tc.desc: test for multiple shadow parameters and abnormal shadow parameters.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TextStyleAddShadowTest, TestSize.Level1)
{
    // Test the full shadow parameters of the scene
    OH_Drawing_TextShadow* shadow2 = OH_Drawing_CreateTextShadow();
    uint32_t color2 = OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00);
    OH_Drawing_Point* offset2 = OH_Drawing_PointCreate(10, 10);
    double blurRadius2 = 10.0;
    OH_Drawing_SetTextShadow(shadow2, color2, offset2, blurRadius2);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyleAddShadow(textStyle, shadow2);
    OH_Drawing_TextStyleAddShadow(textStyle, shadow2);
    int getCount2 = OH_Drawing_TextStyleGetShadowCount(textStyle);
    EXPECT_EQ(getCount2, 2);
    OH_Drawing_TextStyleClearShadows(textStyle);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowCount(textStyle), 0);
    OH_Drawing_TextShadow* shadow21 = OH_Drawing_CreateTextShadow();
    uint32_t color21 = OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0x00);
    OH_Drawing_Point* offset21 = OH_Drawing_PointCreate(-10, -10);
    double blurRadius21 = 20.0;
    OH_Drawing_SetTextShadow(shadow21, color21, offset21, blurRadius21);
    OH_Drawing_TextStyleAddShadow(textStyle, shadow21);
    OH_Drawing_TextStyleAddShadow(textStyle, shadow2);
    OH_Drawing_TextShadow* getShadow2 = OH_Drawing_TextStyleGetShadowWithIndex(textStyle, getCount2 - 1);
    EXPECT_NE(getShadow2, nullptr);
    OH_Drawing_TextShadow* getShadow21 = OH_Drawing_TextStyleGetShadowWithIndex(textStyle, 0);
    EXPECT_NE(getShadow21, nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowWithIndex(textStyle, -1), nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowWithIndex(textStyle, getCount2), nullptr);
    OH_Drawing_PointDestroy(offset2);
    OH_Drawing_PointDestroy(offset21);
    OH_Drawing_DestroyTextShadow(shadow2);
    OH_Drawing_DestroyTextShadow(shadow21);
    EXPECT_NE(shadow2, nullptr);
    EXPECT_NE(shadow21, nullptr);

    // Test the scene for abnormal shadow parameters
    OH_Drawing_TextShadow* shadow3 = OH_Drawing_CreateTextShadow();
    uint32_t color3 = -1;
    OH_Drawing_Point* offset3 = OH_Drawing_PointCreate(10, 10);
    double blurRadius3 = -10.0;
    OH_Drawing_SetTextShadow(shadow3, color3, offset3, blurRadius3);
    EXPECT_NE(shadow3, nullptr);
    OH_Drawing_TextStyleAddShadow(textStyle, shadow3);
    OH_Drawing_TextStyleAddShadow(textStyle, shadow3);
    OH_Drawing_PointDestroy(offset3);
    OH_Drawing_DestroyTextShadow(shadow3);
    OH_Drawing_TextShadow* shadow3AllGet = OH_Drawing_TextStyleGetShadows(textStyle);
    EXPECT_NE(shadow3AllGet, nullptr);
    OH_Drawing_DestroyTextShadows(shadow3AllGet);
    EXPECT_NE(shadow3AllGet, nullptr);
}

/*
 * @tc.name: OH_Drawing_AddTextStyleDecorationTest
 * @tc.desc: test for add multiple text decoration
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_AddTextStyleDecorationTest, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_NONE);

    OH_Drawing_AddTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::UNDERLINE);

    OH_Drawing_AddTextStyleDecoration(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::UNDERLINE);

    OH_Drawing_AddTextStyleDecoration(txtStyle, TEXT_DECORATION_OVERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::UNDERLINE | TextDecoration::OVERLINE);

    OH_Drawing_AddTextStyleDecoration(nullptr, TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::UNDERLINE | TextDecoration::OVERLINE);

    OH_Drawing_AddTextStyleDecoration(txtStyle, TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration,
        TextDecoration::UNDERLINE | TextDecoration::OVERLINE | TextDecoration::LINE_THROUGH);

    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_NONE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::NONE);
    OH_Drawing_AddTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::UNDERLINE | TextDecoration::LINE_THROUGH);
    OH_Drawing_AddTextStyleDecoration(nullptr, TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_LINE_THROUGH);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_RemoveTextStyleDecorationTest
 * @tc.desc: test for remove specific text decoration
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_RemoveTextStyleDecorationTest, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_NONE);

    OH_Drawing_AddTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE);
    OH_Drawing_AddTextStyleDecoration(txtStyle, TEXT_DECORATION_OVERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::UNDERLINE | TextDecoration::OVERLINE);
    OH_Drawing_RemoveTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::OVERLINE);

    OH_Drawing_RemoveTextStyleDecoration(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::OVERLINE);

    OH_Drawing_RemoveTextStyleDecoration(nullptr, TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::OVERLINE);

    OH_Drawing_AddTextStyleDecoration(txtStyle,
        TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_OVERLINE | TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration,
        TextDecoration::UNDERLINE | TextDecoration::OVERLINE | TextDecoration::LINE_THROUGH);
    OH_Drawing_RemoveTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::OVERLINE);
    OH_Drawing_RemoveTextStyleDecoration(nullptr, TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_LINE_THROUGH);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest108
 * @tc.desc: test for the text tab create and destroy
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest108, TestSize.Level1)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, 0.0);
    EXPECT_NE(textTab, nullptr);
    OH_Drawing_TextTab* textTab2 = OH_Drawing_CreateTextTab(TEXT_ALIGN_END, -1.0);
    EXPECT_NE(textTab2, nullptr);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_DestroyTextTab(textTab2);
}

/*
 * @tc.name: OH_Drawing_TypographyTest109
 * @tc.desc: test for get alignment of the text tab
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest109, TestSize.Level1)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, 0.0);
    EXPECT_EQ(OH_Drawing_GetTextTabAlignment(textTab), TEXT_ALIGN_LEFT);
    OH_Drawing_TextTab* textTab2 = OH_Drawing_CreateTextTab(TEXT_ALIGN_JUSTIFY, 0.0);
    EXPECT_EQ(OH_Drawing_GetTextTabAlignment(textTab), TEXT_ALIGN_LEFT);
    EXPECT_EQ(OH_Drawing_GetTextTabAlignment(nullptr), TEXT_ALIGN_LEFT);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_DestroyTextTab(textTab2);
}

/*
 * @tc.name: OH_Drawing_TypographyTest110
 * @tc.desc: test for get location of the text tab
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest110, TestSize.Level1)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, 0.0);
    EXPECT_EQ(OH_Drawing_GetTextTabLocation(textTab), 0.0);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_TextTab* textTab2 = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, -100.0);
    EXPECT_EQ(OH_Drawing_GetTextTabLocation(textTab2), -100.0);
    EXPECT_EQ(OH_Drawing_GetTextTabLocation(nullptr), 0.0);
    OH_Drawing_DestroyTextTab(textTab2);
}

/*
 * @tc.name: OH_Drawing_TypographyTest111
 * @tc.desc: test for typography style set text tab
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest111, TestSize.Level1)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, -1.0);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextTab(typoStyle, textTab);
    OH_Drawing_SetTypographyTextTab(nullptr, textTab);
    OH_Drawing_SetTypographyTextTab(typoStyle, nullptr);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->tab.alignment, TextAlign::LEFT);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->tab.location, -1.0);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest112
 * @tc.desc: test for truncated emoji text drawing
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest112, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_SetTextStyleFontSize(txtStyle, ARC_FONT_SIZE);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "\xF0\x9F\x98";
    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().initStatus_ = true;
    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().bundleApiVersion_ =
        OHOS::Rosen::SPText::SINCE_API18_VERSION;
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OHOS::Rosen::SPText::TextBundleConfigParser::GetInstance().initStatus_ = false;
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    EXPECT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);

    float longestLineWidth = OH_Drawing_TypographyGetLongestLine(typography);
    ASSERT_TRUE(skia::textlayout::nearlyEqual(longestLineWidth, ARC_FONT_SIZE));

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyTest113
 * @tc.desc: test for the input nullptr.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest113, TestSize.Level1)
{
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(nullptr, nullptr);
    EXPECT_EQ(handler, nullptr);
    OH_Drawing_DestroyTypographyHandler(handler);
    handler = OH_Drawing_CreateTypographyHandler(OH_Drawing_CreateTypographyStyle(), OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, nullptr);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_TypographyHandlerAddText(handler, nullptr);
}

/*
 * @tc.name: OH_Drawing_TypographyTest114
 * @tc.desc: test for setting strut style and getting strut style
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest114, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 10);

    OH_Drawing_StrutStyle* strutstyle = new OH_Drawing_StrutStyle();
    strutstyle->weight = FONT_WEIGHT_400;
    strutstyle->style = FONT_STYLE_ITALIC;
    // 17.0 For size
    strutstyle->size = 17.0;
    // 2.0 For heightScale
    strutstyle->heightScale = 2;
    strutstyle->heightOverride = true;
    strutstyle->halfLeading = true;
    // 3.0 For leading
    strutstyle->leading = 3.0;
    strutstyle->forceStrutHeight = true;
    // 4 For families size
    strutstyle->familiesSize = 4;
    strutstyle->families = (char**)malloc(strutstyle->familiesSize * sizeof(char*));
    const char* temp[] = { "1", "2", "3", "4" };
    for (int i = 0; i < strutstyle->familiesSize; i++) {
        // 2 For families member size
        strutstyle->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(strutstyle->families[i], 2, temp[i]);
    }
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutstyle);
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, true);
    ASSERT_NE(OH_Drawing_TypographyStyleGetStrutStyle(typoStyle), nullptr);
    
    OH_Drawing_TypographyCreate *handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    OH_Drawing_TextStyle *textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 52);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);

    const char *text = "HelloWorldHelloWorldHelloWorldHelloWorld";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography *typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200);

    OH_Drawing_LineMetrics lineMerics;
    OH_Drawing_TypographyGetLineMetricsAt(typography, 0, &lineMerics);
    EXPECT_EQ(lineMerics.height, 85);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_TypographyStyleDestroyStrutStyle(strutstyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

 /*
 * @tc.name: OH_Drawing_TypographyTest115
 * @tc.desc: test for strutstyle not equal
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest115, TestSize.Level1)
{
    OH_Drawing_StrutStyle* from = new OH_Drawing_StrutStyle();
    // 4 For families size
    from->familiesSize = 4;
    from->families = (char**)malloc(from->familiesSize * sizeof(char*));
    const char* temp[] = { "1", "2", "3", "4" };
    for (int i = 0; i < from->familiesSize; i++) {
        // 2 For families member size
        from->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(from->families[i], 2, temp[i]);
    }
    OH_Drawing_StrutStyle* to = new OH_Drawing_StrutStyle();
    // 3 For families size
    to->familiesSize = 4;
    to->families = (char**)malloc(to->familiesSize * sizeof(char*));
    const char* temp1[] = { "3", "2", "1"};
    for (int i = 0; i < to->familiesSize; i++) {
        // 2 For families member size
        to->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(to->families[i], 2, temp1[i]);
    }
    bool result = OH_Drawing_TypographyStyleStrutStyleEquals(from, to);
    EXPECT_FALSE(result);
    
    OH_Drawing_StrutStyle* from1 = new OH_Drawing_StrutStyle();
    from1->size = 98;
    OH_Drawing_StrutStyle* to1 = new OH_Drawing_StrutStyle();
    to1->size = 99;
    bool result1 = OH_Drawing_TypographyStyleStrutStyleEquals(from1, to1);
    EXPECT_FALSE(result1);
    OH_Drawing_TypographyStyleDestroyStrutStyle(from);
    OH_Drawing_TypographyStyleDestroyStrutStyle(to);
    OH_Drawing_TypographyStyleDestroyStrutStyle(from1);
    OH_Drawing_TypographyStyleDestroyStrutStyle(to1);
}

/*
 * @tc.name: OH_Drawing_TypographyTest116
 * @tc.desc: test for getting strut style where familySize equals 0
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest116, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_StrutStyle* strutstyle1 = new OH_Drawing_StrutStyle();
    strutstyle1->familiesSize = 0;
    strutstyle1->families = (char**)malloc(1 * sizeof(char*));
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutstyle1);
    OH_Drawing_StrutStyle* structStyle = OH_Drawing_TypographyStyleGetStrutStyle(typoStyle);
    EXPECT_EQ(structStyle->familiesSize, 0);
    EXPECT_EQ(structStyle->families, nullptr);

    free(strutstyle1->families);
    OH_Drawing_TypographyStyleDestroyStrutStyle(strutstyle1);

    // branch coverage
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, nullptr);
    OH_Drawing_TypographyStyleDestroyStrutStyle(nullptr);
    
    OH_Drawing_TypographyStyleDestroyStrutStyle(structStyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: OH_Drawing_FontParserGetSystemFontListTest001
 * @tc.desc: test for the OH_Drawing_FontParserGetSystemFontList.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_FontParserGetSystemFontListTest001, TestSize.Level1)
{
    OH_Drawing_FontParser* fontParser = OH_Drawing_CreateFontParser();
    ASSERT_NE(fontParser, nullptr);
    size_t value = 100; // 100 for test
    size_t* num = &value;
    ASSERT_NE(OH_Drawing_FontParserGetSystemFontList(fontParser, num), nullptr);
    num = nullptr;
    ASSERT_EQ(OH_Drawing_FontParserGetSystemFontList(fontParser, num), nullptr);
    fontParser = nullptr;
    ASSERT_EQ(OH_Drawing_FontParserGetSystemFontList(fontParser, num), nullptr);
}

/*
 * @tc.name: OH_Drawing_DestroySystemFontListTest001
 * @tc.desc: test for the OH_Drawing_DestroySystemFontList.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_DestroySystemFontListTest001, TestSize.Level1)
{
    char** fontList = new char*[3]; // 3 means the number of font
    std::string tempStr1 = "Test1";
    std::string tempStr3 = "Test3";
    fontList[0] = new char[tempStr1.size() + 1];
    std::copy(tempStr1.begin(), tempStr1.end(), fontList[0]);
    fontList[0][tempStr1.size()] = '\0';
    fontList[1] = nullptr;
    fontList[2] = new char[tempStr3.size() + 1]; // 2 means the index of font
    std::copy(tempStr3.begin(), tempStr3.end(), fontList[2]); // 2 means the index of font
    fontList[2][tempStr3.size()] = '\0'; // 2 means the index of font
    size_t num = 3; // 3 means the number of font
    ASSERT_NE(fontList, nullptr);
    OH_Drawing_DestroySystemFontList(fontList, num);

    fontList = nullptr;
    OH_Drawing_DestroySystemFontList(fontList, num);
}

/*
 * @tc.name: OH_Drawing_FontParserGetFontByNameTest001
 * @tc.desc: test for the OH_Drawing_FontParserGetFontByName.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_FontParserGetFontByNameTest001, TestSize.Level1)
{
    OH_Drawing_FontParser* fontParser = OH_Drawing_CreateFontParser();
    ASSERT_NE(fontParser, nullptr);
    const char* name = "test";
    OH_Drawing_FontDescriptor* fontDescriptor = OH_Drawing_FontParserGetFontByName(fontParser, name);
    ASSERT_EQ(fontDescriptor, nullptr);

    OH_Drawing_FontDescriptor* fontDescriptor1 = OH_Drawing_FontParserGetFontByName(fontParser, nullptr);
    ASSERT_EQ(fontDescriptor1, nullptr);

    OH_Drawing_FontDescriptor* fontDescriptor2 = OH_Drawing_FontParserGetFontByName(nullptr, name);
    ASSERT_EQ(fontDescriptor2, nullptr);
    OH_Drawing_DestroyFontParser(fontParser);
}

/*
 * @tc.name: OH_Drawing_TypographyGetLineMetricsTest001
 * @tc.desc: test for the OH_Drawing_TypographyGetLineMetrics.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyGetLineMetricsTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle,
        OH_Drawing_CreateFontCollection());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_LineMetrics* lineMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    ASSERT_EQ(lineMetrics, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TextStyleGetShadowCountTest001
 * @tc.desc: test for the OH_Drawing_TextStyleGetShadowCount.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TextStyleGetShadowCountTest001, TestSize.Level1)
{
    OH_Drawing_TextStyle* style = OH_Drawing_CreateTextStyle();
    ASSERT_NE(style, nullptr);
    ASSERT_EQ(OH_Drawing_TextStyleGetShadowCount(style), 0);
    OH_Drawing_TextShadow* textShadow = OH_Drawing_TextStyleGetShadows(style);
    ASSERT_NE(textShadow, nullptr);

    ASSERT_EQ(OH_Drawing_TextStyleGetShadowCount(nullptr), 0);
    OH_Drawing_TextShadow* textShadow1 = OH_Drawing_TextStyleGetShadows(nullptr);
    ASSERT_EQ(textShadow1, nullptr);
    OH_Drawing_DestroyTextStyle(style);
    OH_Drawing_DestroyTextShadows(textShadow);
}

/*
 * @tc.name: OH_Drawing_SetTextShadowTest001
 * @tc.desc: test for the OH_Drawing_SetTextShadow.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_SetTextShadowTest001, TestSize.Level1)
{
    OH_Drawing_TextStyle* style = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextShadow* shadow = OH_Drawing_TextStyleGetShadows(style);
    ASSERT_NE(shadow, nullptr);
    uint32_t color = 0;
    OH_Drawing_Point* offset = OH_Drawing_PointCreate(0, 0);
    ASSERT_NE(offset, nullptr);
    double blurRadius = true;
    OH_Drawing_SetTextShadow(shadow, color, offset, blurRadius);
    OH_Drawing_SetTextShadow(shadow, color, nullptr, blurRadius);
    OH_Drawing_SetTextShadow(nullptr, color, offset, blurRadius);

    OH_Drawing_DestroyTextStyle(style);
    OH_Drawing_PointDestroy(offset);
    OH_Drawing_DestroyTextShadows(shadow);
}

/*
 * @tc.name: OH_Drawing_TextStyleAddShadowTest001
 * @tc.desc: test for the OH_Drawing_TextStyleAddShadow.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TextStyleAddShadowTest001, TestSize.Level1)
{
    OH_Drawing_TextStyle* style = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextShadow* shadow = OH_Drawing_TextStyleGetShadows(style);
    ASSERT_NE(shadow, nullptr);
    OH_Drawing_TextStyleAddShadow(style, shadow);
    EXPECT_NE(OH_Drawing_TextStyleGetShadows(style), nullptr);
    OH_Drawing_TextStyleClearShadows(style);
    OH_Drawing_TextStyleAddShadow(nullptr, shadow);
    OH_Drawing_TextStyleClearShadows(nullptr);
    OH_Drawing_TextStyleAddShadow(style, nullptr);
    OH_Drawing_DestroyTextStyle(style);
    OH_Drawing_DestroyTextShadows(shadow);
}

/*
 * @tc.name: OH_Drawing_TextStyleGetShadowWithIndexTest001
 * @tc.desc: test for the OH_Drawing_TextStyleGetShadowWithIndex.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TextStyleGetShadowWithIndexTest001, TestSize.Level1)
{
    OH_Drawing_TextStyle* style = OH_Drawing_CreateTextStyle();
    ASSERT_NE(style, nullptr);
    OH_Drawing_TextShadow* textShadow = OH_Drawing_TextStyleGetShadowWithIndex(style, 0);
    ASSERT_EQ(textShadow, nullptr);
    // 2 for test
    OH_Drawing_TextShadow* textShadow1 = OH_Drawing_TextStyleGetShadowWithIndex(style, 2);
    ASSERT_EQ(textShadow1, nullptr);
    // -1 for test
    OH_Drawing_TextShadow* textShadow2 = OH_Drawing_TextStyleGetShadowWithIndex(style, -1);
    ASSERT_EQ(textShadow2, nullptr);
    // -1 for test
    OH_Drawing_TextShadow* textShadow3 = OH_Drawing_TextStyleGetShadowWithIndex(nullptr, -1);
    ASSERT_EQ(textShadow3, nullptr);
    OH_Drawing_DestroyTextStyle(style);
}

/*
 * @tc.name: OH_Drawing_TextStyleGetShadowsTest001
 * @tc.desc: test for the OH_Drawing_DestroyTextShadows.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TextStyleGetShadowsTest001, TestSize.Level1)
{
    OH_Drawing_TextStyle* style = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextShadow* shadow = OH_Drawing_TextStyleGetShadows(style);
    ASSERT_NE(shadow, nullptr);
    OH_Drawing_DestroyTextShadows(shadow);
    OH_Drawing_DestroyTextShadows(nullptr);
    OH_Drawing_DestroyTextStyle(style);
}

/*
 * @tc.name: OH_Drawing_SetTypographyTextEllipsisTest001
 * @tc.desc: test for the OH_Drawing_SetTypographyTextLocale.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_SetTypographyTextEllipsisTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* style = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(style, nullptr);
    const char* locale = "test";
    OH_Drawing_SetTypographyTextLocale(style, locale);
    OH_Drawing_SetTypographyTextSplitRatio(style, 0.f);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_TypographyGetTextStyle(style);
    ASSERT_NE(textStyle, nullptr);
    ASSERT_EQ(OH_Drawing_TypographyGetEffectiveAlignment(style), 0);
    ASSERT_EQ(OH_Drawing_TypographyIsLineUnlimited(style), true);
    ASSERT_EQ(OH_Drawing_TypographyIsEllipsized(style), false);
    const char* ellipsis = "ellipsis";
    OH_Drawing_SetTypographyTextEllipsis(style, ellipsis);

    const char* ellipsisVal = nullptr;
    OH_Drawing_SetTypographyTextEllipsis(style, ellipsisVal);

    OH_Drawing_SetTypographyTextLocale(nullptr, locale);
    OH_Drawing_SetTypographyTextSplitRatio(nullptr, 0.f);
    OH_Drawing_TextStyle* textStyle1 = OH_Drawing_TypographyGetTextStyle(nullptr);
    ASSERT_EQ(textStyle1, nullptr);
    ASSERT_EQ(OH_Drawing_TypographyGetEffectiveAlignment(nullptr), 0);
    ASSERT_EQ(OH_Drawing_TypographyIsLineUnlimited(nullptr), false);
    ASSERT_EQ(OH_Drawing_TypographyIsEllipsized(nullptr), false);
    OH_Drawing_SetTypographyTextEllipsis(nullptr, ellipsisVal);
    OH_Drawing_DestroyTypographyStyle(style);
}

/*
 * @tc.name: OH_Drawing_TypographyHandlerPushTextStyle001
 * @tc.desc: test for the actual effective value of textstyle in each of the three scenarios.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyHandlerPushTextStyle001, TestSize.Level1)
{
    // Use interfaces such as OH_Drawing_SetTypographyTextFontSize to test the fallback textstyle.
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontSize(typoStyle, 100);
    OH_Drawing_SetTypographyTextFontHeight(typoStyle, 1);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_TypographyGetTextStyle(typoStyle);
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    ASSERT_NE(handler, nullptr);
    const char* text = "test OH_Drawing_SetTypographyTextLineStylexxx";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 10000.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography), 100);

    // After setting the default text style in typographstyle, the fallback text style becomes ineffective.
    OH_Drawing_TypographyStyle* typoStyle2 = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontSize(typoStyle2, 500);
    OH_Drawing_SetTypographyTextFontHeight(typoStyle2, 1);
    OH_Drawing_TextStyle* textStyle2 = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle2, 30);
    OH_Drawing_SetTextStyleFontHeight(textStyle2, 2);
    OH_Drawing_SetTypographyTextStyle(typoStyle2, textStyle2);
    OH_Drawing_TypographyCreate* handler2 =
        OH_Drawing_CreateTypographyHandler(typoStyle2, OH_Drawing_CreateSharedFontCollection());
    ASSERT_NE(handler2, nullptr);
    const char* text2 = "test OH_Drawing_OH_Drawing_SetTypographyTextStyle, 该方法优先";
    OH_Drawing_TypographyHandlerAddText(handler2, text2);
    OH_Drawing_Typography* typography2 = OH_Drawing_CreateTypography(handler2);
    OH_Drawing_TypographyLayout(typography2, maxWidth);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography2), 60);

    // After pushing a new text style, the default text style becomes ineffective.
    OH_Drawing_TypographyStyle* typoStyle3 = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontSize(typoStyle3, 500);
    OH_Drawing_SetTypographyTextFontHeight(typoStyle3, 1);
    OH_Drawing_TextStyle* textStyle3 = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle3, 30);
    OH_Drawing_SetTextStyleFontHeight(textStyle3, 2);
    OH_Drawing_SetTypographyTextStyle(typoStyle3, textStyle3);
    OH_Drawing_SetTextStyleFontSize(textStyle3, 80);
    OH_Drawing_SetTextStyleFontHeight(textStyle3, 3);
    OH_Drawing_TypographyCreate* handler3 =
        OH_Drawing_CreateTypographyHandler(typoStyle3, OH_Drawing_CreateSharedFontCollection());
    ASSERT_NE(handler3, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler3, textStyle3);
    const char* text3 = "test OH_Drawing_TypographyHandlerPushTextStyle, 该方法优先";
    OH_Drawing_TypographyHandlerAddText(handler3, text3);
    OH_Drawing_Typography* typography3 = OH_Drawing_CreateTypography(handler3);
    OH_Drawing_TypographyLayout(typography3, maxWidth);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography3), 240);
}

/*
 * @tc.name: OH_Drawing_TypographyHandlerPushTextStyle002
 * @tc.desc: test the height of Tibetan and Uighur in push textstyle scenarios.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyHandlerPushTextStyle002, TestSize.Level1)
{
    // After pushing a new text style, the default text style becomes ineffective.
    OH_Drawing_TypographyStyle* typoStyle3 = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontSize(typoStyle3, 500);
    OH_Drawing_SetTypographyTextFontHeight(typoStyle3, 1);
    OH_Drawing_TextStyle* textStyle3 = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle3, 30);
    OH_Drawing_SetTextStyleFontHeight(textStyle3, 2);
    OH_Drawing_SetTypographyTextStyle(typoStyle3, textStyle3);
    OH_Drawing_SetTextStyleFontSize(textStyle3, 80);
    OH_Drawing_SetTextStyleFontHeight(textStyle3, 3);
    
    // Testing the line height of Uyghur text in 'heightonly' mode
    const char* text4 = "بۇ ئۇسۇل ئالدىنقى ئورۇندا";
    OH_Drawing_TypographyCreate* handler4 =
        OH_Drawing_CreateTypographyHandler(typoStyle3, OH_Drawing_CreateSharedFontCollection());
    ASSERT_NE(handler4, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler4, textStyle3);
    OH_Drawing_TypographyHandlerAddText(handler4, text4);
    OH_Drawing_Typography* typography4 = OH_Drawing_CreateTypography(handler4);
    double maxWidth = 10000.0;
    OH_Drawing_TypographyLayout(typography4, maxWidth);
    EXPECT_NE(OH_Drawing_TypographyGetHeight(typography4), 240);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography4), 242);

    // Testing the line height of Tibetan text in 'heightonly' mode.
    const char* text5 = "ཐབས་ལམ་འདི་ལྡནཐབས་ལམ་འདི་ལྡན";
    OH_Drawing_TypographyCreate* handler5 =
        OH_Drawing_CreateTypographyHandler(typoStyle3, OH_Drawing_CreateSharedFontCollection());
    ASSERT_NE(handler4, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler5, textStyle3);
    OH_Drawing_TypographyHandlerAddText(handler5, text5);
    OH_Drawing_Typography* typography5 = OH_Drawing_CreateTypography(handler5);
    OH_Drawing_TypographyLayout(typography5, maxWidth);
    EXPECT_NE(OH_Drawing_TypographyGetHeight(typography5), 240);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography5), 190);
}

/*
 * @tc.name: OH_Drawing_FontFamiliesTest001
 * @tc.desc: test for the OH_Drawing_TypographyTextlineStyleDestroyFontFamilies.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_FontFamiliesTest001, TestSize.Level1)
{
    char** fontFamilies = new char*[3]; // 3 means the number of font
    std::string tempStr1 = "Test1";
    std::string tempStr3 = "Test3";
    fontFamilies[0] = new char[tempStr1.size() + 1];
    std::copy(tempStr1.begin(), tempStr1.end(), fontFamilies[0]);
    fontFamilies[0][tempStr1.size()] = '\0';
    fontFamilies[1] = nullptr;
    fontFamilies[2] = new char[tempStr3.size() + 1]; // 2 means the index of font
    std::copy(tempStr3.begin(), tempStr3.end(), fontFamilies[2]); // 2 means the index of font
    fontFamilies[2][tempStr3.size()] = '\0'; // 2 means the index of font
    size_t num = 3; // 3 means the number of font
    ASSERT_NE(fontFamilies, nullptr);
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(fontFamilies, num);

    fontFamilies = nullptr;
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(fontFamilies, num);
}

/*
 * @tc.name: OH_Drawing_TypographyGetLineFontMetricsTest001
 * @tc.desc: test for the OH_Drawing_TypographyGetLineFontMetrics.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyGetLineFontMetricsTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    size_t lineNumber = 1;
    size_t fontMetrics = 1;
    size_t* fontMetricsSize = &fontMetrics;
    ASSERT_NE(fontMetricsSize, nullptr);
    OH_Drawing_Font_Metrics* metrics = OH_Drawing_TypographyGetLineFontMetrics(typography, lineNumber, fontMetricsSize);
    ASSERT_EQ(metrics, nullptr);

    OH_Drawing_Font_Metrics* metrics1 = OH_Drawing_TypographyGetLineFontMetrics(typography, 0, fontMetricsSize);
    ASSERT_EQ(metrics1, nullptr);
    OH_Drawing_Font_Metrics* metrics2 = OH_Drawing_TypographyGetLineFontMetrics(typography, lineNumber, nullptr);
    ASSERT_EQ(metrics2, nullptr);
    OH_Drawing_Font_Metrics* metrics3 = OH_Drawing_TypographyGetLineFontMetrics(nullptr, lineNumber, fontMetricsSize);
    ASSERT_EQ(metrics3, nullptr);
    OH_Drawing_TypographyDestroyLineFontMetrics(metrics);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TextStyleAddFontVariationTest001
 * @tc.desc: test for the OH_Drawing_TextStyleAddFontVariation.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TextStyleAddFontVariationTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* style = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(style, nullptr);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_TypographyGetTextStyle(style);
    ASSERT_NE(textStyle, nullptr);
    const char* axis = "T";
    const float value = 1.f;
    OH_Drawing_TextStyleAddFontVariation(textStyle, axis, value);
    OH_Drawing_TextStyleAddFontVariation(textStyle, nullptr, value);
    OH_Drawing_TextStyleAddFontVariation(nullptr, axis, value);
    OH_Drawing_DestroyTypographyStyle(style);
}

/*
* @tc.name: OH_Drawing_TypographyLineInfoTest001
* @tc.desc: test for getting line info and line font metrics for one line with white space
* @tc.type: FUNC
*/
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyLineInfoTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79,
        0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e, 0x73 };
    const char *fontFamilies[] = {fontFamiliesTest};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个 test123排版信息获取接口   ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
 
    OH_Drawing_LineMetrics lineInfoNoSpace;
    OH_Drawing_TypographyGetLineInfo(typography, 0, true, false, &lineInfoNoSpace);
    OH_Drawing_LineMetrics lineInfoHasSpace;
    OH_Drawing_TypographyGetLineInfo(typography, 0, true, true, &lineInfoHasSpace);
 
    EXPECT_EQ(lineInfoNoSpace.startIndex, 0);
    EXPECT_EQ(lineInfoNoSpace.endIndex, 23);
    EXPECT_NEAR(lineInfoNoSpace.width, 786.149231, FLOAT_DATA_EPSILON);
    EXPECT_EQ(lineInfoHasSpace.startIndex, 0);
    EXPECT_EQ(lineInfoHasSpace.endIndex, 23);
    EXPECT_NEAR(lineInfoHasSpace.width, 826.649230, FLOAT_DATA_EPSILON);
 
    OH_Drawing_Font_Metrics textStyleMetrics;
    OH_Drawing_TextStyleGetFontMetrics(typography, txtStyle, &textStyleMetrics);
    EXPECT_NEAR(textStyleMetrics.top, -52.799999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.avgCharWidth, 25.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.maxCharWidth, 124.300003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.xHeight, 25.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.capHeight, 35.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.underlineThickness, 2.500000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.underlinePosition, 10.350000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.strikeoutThickness, 2.500000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textStyleMetrics.strikeoutPosition, -15.000001, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
* @tc.name: OH_Drawing_TypographyLineInfoTest002
* @tc.desc: test for getting line info and line font metrics for one line with white space
* @tc.type: FUNC
*/
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyLineInfoTest002, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79,
        0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e, 0x73 };
    const char *fontFamilies[] = {fontFamiliesTest};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个 test123排版信息获取接口   123444  Test  ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
 
    OH_Drawing_LineMetrics lineInfoNoSpace;
    OH_Drawing_TypographyGetLineInfo(typography, 1, false, false, &lineInfoNoSpace);
    OH_Drawing_LineMetrics lineInfoHasSpace;
    OH_Drawing_TypographyGetLineInfo(typography, 1, false, true, &lineInfoHasSpace);
 
    EXPECT_EQ(lineInfoNoSpace.startIndex, 23);
    EXPECT_EQ(lineInfoNoSpace.endIndex, 37);
    EXPECT_NEAR(lineInfoNoSpace.width, 289.999756, FLOAT_DATA_EPSILON);
    EXPECT_EQ(lineInfoHasSpace.startIndex, 23);
    EXPECT_EQ(lineInfoHasSpace.endIndex, 37);
    EXPECT_NEAR(lineInfoHasSpace.width, 316.999756, FLOAT_DATA_EPSILON);
 
    size_t fontMetricsSize = 0;
    OH_Drawing_Font_Metrics* metrics = OH_Drawing_TypographyGetLineFontMetrics(typography, 2,
         &fontMetricsSize);
    EXPECT_NE(metrics, nullptr);
    EXPECT_EQ(fontMetricsSize, 12);
    EXPECT_NEAR(metrics[1].top, -52.799999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].avgCharWidth, 25.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].maxCharWidth, 124.300003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].xMin, -27.400000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].xMax, 96.900002, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].xHeight, 25.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].capHeight, 35.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].underlineThickness, 2.500000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].underlinePosition, 10.350000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].strikeoutThickness, 2.500000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(metrics[1].strikeoutPosition, -15.000001, FLOAT_DATA_EPSILON);
    OH_Drawing_TypographyDestroyLineFontMetrics(metrics);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyRectsForRangeTest001
 * @tc.desc: test for typography rectsForRange
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyRectsForRangeTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_StrutStyle strutStyle = {
        .weight = FONT_WEIGHT_400, .style = FONT_STYLE_NORMAL, .size = 120.0,
        .heightScale = 2, .heightOverride = true, .halfLeading = true, .leading = 1.0
    };
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, &strutStyle);
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, true);
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(txtStyle, 120);
    OH_Drawing_TypographyCreate *handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息获取接口的测试文本: Hello World";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息获取接口的排版信息排版信息排版信息排版信息.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200.0);
    CheckTypographyRectsForRange(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
* @tc.name: OH_Drawing_TypographyDidExceedMaxLinesTest001
* @tc.desc: test for typography mutiple lines.
* @tc.type: FUNC
*/
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyDidExceedMaxLinesTest001, TestSize.Level1)
{
    std::vector<pair<int, double>> lineResult = {
        {75, 796.899231}, {59, 780.399231}, {59, 750.999268},
        {59, 773.041809}, {59, 774.646362}, {68, 595.349548}, {59, 349.772766}};
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    const char *fontFamilies[] = {"HMOS Color Emoji", "Roboto"};
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 2,  fontFamilies);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World 这\n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World 这是一个 ..... \u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    size_t fontFamilyCount = 0;
    char** fontFamiliesResult = OH_Drawing_TypographyTextlineStyleGetFontFamilies(typoStyle, &fontFamilyCount);
    EXPECT_EQ(fontFamilyCount, 2);
    for (size_t i = 0; i < fontFamilyCount; i++) {
        EXPECT_STREQ(fontFamilies[i], fontFamiliesResult[i]);
    }
    int lineCount = OH_Drawing_TypographyGetLineCount(typography);
    EXPECT_NEAR(OH_Drawing_TypographyGetLongestLine(typography), 796.899231, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TypographyGetHeight(typography), 438.000000, FLOAT_DATA_EPSILON);
    EXPECT_EQ(lineCount, 7);
    EXPECT_EQ(OH_Drawing_TypographyDidExceedMaxLines(typography), false);
    double maxLineWidth = 0.0, expectedLineHeight = 0.0;
    for (int i = 0; i < lineCount; i++) {
        double lineHeight = OH_Drawing_TypographyGetLineHeight(typography, i);
        double lineWidth = OH_Drawing_TypographyGetLineWidth(typography, i);
        EXPECT_NEAR(lineHeight, lineResult[i].first, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(lineWidth, lineResult[i].second, FLOAT_DATA_EPSILON);
        maxLineWidth = std::max(maxLineWidth, lineWidth);
        expectedLineHeight += lineHeight;
    }
    EXPECT_EQ(OH_Drawing_TypographyDidExceedMaxLines(typography), false);
    EXPECT_NEAR(OH_Drawing_TypographyGetLongestLine(typography), maxLineWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TypographyGetHeight(typography), expectedLineHeight, FLOAT_DATA_EPSILON);
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(fontFamiliesResult, fontFamilyCount);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
* @tc.name: OH_Drawing_TypographyGetWordBoundaryTest001
* @tc.desc: test for typography mutiple lines，but set the maximum number of lines
* @tc.type: FUNC
*/
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyGetWordBoundaryTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 2);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World 这\n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World 这是一个 ..... \u1234排版信息";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    bool isExceedMaxLines = OH_Drawing_TypographyDidExceedMaxLines(typography);
    int currentLines = OH_Drawing_TypographyGetLineCount(typography);
    int maxLines = OH_Drawing_TypographyGetTextMaxLines(typoStyle);
    double longestLine = OH_Drawing_TypographyGetLongestLine(typography);
    double longestLineIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
 
    OH_Drawing_Range* rangeFirst = OH_Drawing_TypographyGetWordBoundary(typography, 0);
    int startFirst = OH_Drawing_GetStartFromRange(rangeFirst);
    int endFirst = OH_Drawing_GetEndFromRange(rangeFirst);
    EXPECT_EQ(startFirst, 0);
    EXPECT_EQ(endFirst, 1);
    OH_Drawing_Range* rangeLast = OH_Drawing_TypographyGetWordBoundary(typography, 10);
    int startLast = OH_Drawing_GetStartFromRange(rangeLast);
    int endLast = OH_Drawing_GetEndFromRange(rangeLast);
    EXPECT_EQ(startLast, 8);
    EXPECT_EQ(endLast, 22);
 
    OH_Drawing_Range* rangeWord = OH_Drawing_TypographyGetWordBoundary(typography, 34);
    int startWord = OH_Drawing_GetStartFromRange(rangeWord);
    int endWord = OH_Drawing_GetEndFromRange(rangeWord);
    EXPECT_EQ(startWord, 32);
    EXPECT_EQ(endWord, 37);
 
    EXPECT_EQ(currentLines, 2);
    EXPECT_EQ(maxLines, 2);
    EXPECT_EQ(isExceedMaxLines, true);
    EXPECT_EQ(longestLine, longestLineIndent);
    EXPECT_NEAR(longestLine, 796.899231, FLOAT_DATA_EPSILON);
 
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
* @tc.name: OH_Drawing_TypographyGetIndentsWithIndexTest001
* @tc.desc: test for typography mutiple lines，but set mutiple indents
* @tc.type: FUNC
*/
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyGetIndentsWithIndexTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World 这\n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World这是一个 ..... \u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    const float indents[2] = {500.0, 55.1};
    int indentSize = 2;
    OH_Drawing_TypographySetIndents(typography, indentSize, indents);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLine = OH_Drawing_TypographyGetLongestLine(typography);
    double longestLineIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    double lineCount = OH_Drawing_TypographyGetLineCount(typography);
    double maxLineWidth = 0.0;
    double maxLineWidthIndent = 0.0;
    for (int i = 0; i < lineCount; i++) {
        double lineWidth = OH_Drawing_TypographyGetLineWidth(typography, i);
        maxLineWidth = std::max(maxLineWidth, lineWidth);
        double indent = OH_Drawing_TypographyGetIndentsWithIndex(typography, i);
        maxLineWidthIndent = std::max(maxLineWidthIndent, lineWidth + indent);
    }
    EXPECT_NEAR(longestLine, maxLineWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longestLineIndent, maxLineWidthIndent, 0.0001);
    EXPECT_NEAR(longestLine, 739.896423, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longestLineIndent, 799.999756, FLOAT_DATA_EPSILON);
 
    double minIntrinsicWidth = OH_Drawing_TypographyGetMinIntrinsicWidth(typography);
    double maxIntrinsicWidth = OH_Drawing_TypographyGetMaxIntrinsicWidth(typography);
    EXPECT_NEAR(minIntrinsicWidth, 349.772766, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(maxIntrinsicWidth, 3338.310059, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
 
/*
* @tc.name: OH_Drawing_TypographyGetLineTextRangeTest001
* @tc.desc: test for typography mutiple lines，but set Set end line spaces and ellipsis
* @tc.type: FUNC
*/
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyGetLineTextRangeTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_SetTypographyTextMaxLines (typoStyle, 4);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    const char *elipss = "...";
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, elipss);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, 2);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World     \n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World这是一个 ..... \u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLine = OH_Drawing_TypographyGetLongestLine(typography);
    double lineCount = OH_Drawing_TypographyGetLineCount(typography);
    double maxLineWidth = 0.0;
    for (int i = 0; i < lineCount; i++) {
        double lineWidth = OH_Drawing_TypographyGetLineWidth(typography, i);
        maxLineWidth = std::max(maxLineWidth, lineWidth);
    }
    OH_Drawing_Range *range1 = OH_Drawing_TypographyGetLineTextRange(typography, 1, false);
    EXPECT_EQ(55, OH_Drawing_GetStartFromRange(range1));
    EXPECT_EQ(93, OH_Drawing_GetEndFromRange(range1));
    OH_Drawing_Range *range2 = OH_Drawing_TypographyGetLineTextRange(typography, 1, true);
    EXPECT_EQ(55, OH_Drawing_GetStartFromRange(range2));
    EXPECT_EQ(98, OH_Drawing_GetEndFromRange(range2));
    EXPECT_NEAR(longestLine, maxLineWidth, FLOAT_DATA_EPSILON);
    OH_Drawing_PositionAndAffinity* posAAClusrterDown =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 80, 0);
    int affinityClusterDown = OH_Drawing_GetAffinityFromPositionAndAffinity(posAAClusrterDown);
    int aPositionClusterDown = OH_Drawing_GetPositionFromPositionAndAffinity(posAAClusrterDown);
    EXPECT_EQ(0, affinityClusterDown);
    EXPECT_EQ(2, aPositionClusterDown);
    OH_Drawing_PositionAndAffinity* posAAClusrterUp =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 100, 100);
    int affinityClusterUp = OH_Drawing_GetAffinityFromPositionAndAffinity(posAAClusrterUp);
    int aPositionClusterUp = OH_Drawing_GetPositionFromPositionAndAffinity(posAAClusrterUp);
    EXPECT_EQ(1, affinityClusterUp);
    EXPECT_EQ(25, aPositionClusterUp);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
* @tc.name: OH_Drawing_TypographyGetLineTextRangeTest002
* @tc.desc: test for typography mutiple lines，but set Set end line spaces and ellipsis
* @tc.type: FUNC
*/
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyGetLineTextRangeTest002, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle *txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_SetTypographyTextMaxLines (typoStyle, 4);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    const char *elipss = "...";
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, elipss);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, 2);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char *text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World     \n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World这是一个 ..... \u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_Range *range1 = OH_Drawing_TypographyGetLineTextRange(typography, 0, false);
    EXPECT_EQ(0, OH_Drawing_GetStartFromRange(range1));
    EXPECT_EQ(0, OH_Drawing_GetEndFromRange(range1));
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double lineCount = OH_Drawing_TypographyGetLineCount(typography);
 
    OH_Drawing_Range *range2 = OH_Drawing_TypographyGetLineTextRange(typography, lineCount, false);
    EXPECT_EQ(0, OH_Drawing_GetStartFromRange(range2));
    EXPECT_EQ(0, OH_Drawing_GetEndFromRange(range2));
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
} // namespace OHOS