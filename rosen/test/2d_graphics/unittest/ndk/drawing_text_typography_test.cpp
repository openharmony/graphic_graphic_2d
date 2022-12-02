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
#include "c/drawing_bitmap.h"
#include "c/drawing_canvas.h"
#include "c/drawing_color.h"
#include "c/drawing_font_collection.h"
#include "c/drawing_text_declaration.h"
#include "c/drawing_text_typography.h"
#include "rosen_text/ui/typography.h"
#include "rosen_text/ui/typography_create.h"

using namespace rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
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

/*
 * @tc.name: OH_Drawing_TypographyTest001
 * @tc.desc: test for creating TypographyStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_EQ(typoStyle == nullptr, false);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
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
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textDirection_, TextDirection::LTR);
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_RTL);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textDirection_, TextDirection::RTL);
    OH_Drawing_SetTypographyTextDirection(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textDirection_, TextDirection::LTR);
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
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign_, TextAlign::LEFT);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_RIGHT);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign_, TextAlign::RIGHT);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_CENTER);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign_, TextAlign::CENTER);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_JUSTIFY);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign_, TextAlign::JUSTIFY);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_START);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign_, TextAlign::START);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_END);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign_, TextAlign::END);
    OH_Drawing_SetTypographyTextAlign(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->textAlign_, TextAlign::LEFT);
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
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->maxLines_, 100);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 200);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->maxLines_, 200);
}

/*
 * @tc.name: OH_Drawing_TypographyTest005
 * @tc.desc: test for creating text style
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest005, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_EQ(txtStyle == nullptr, false);
    OH_Drawing_DestroyTextStyle(txtStyle);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->color_, 0xFF000000);
    // red
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->color_, 0xFFFF0000);
    // blue
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0xFF));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->color_, 0xFF0000FF);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontSize_, 80);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontSize_, 40);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W100);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_200);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W200);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_300);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W300);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W400);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_500);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W500);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_600);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W600);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_700);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W700);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_800);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W800);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_900);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W900);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontWeight_, FontWeight::W400);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->textBaseline_, TextBaseline::ALPHABETIC);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_IDEOGRAPHIC);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->textBaseline_, TextBaseline::IDEOGRAPHIC);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->textBaseline_, TextBaseline::ALPHABETIC);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration_, TextDecoration::NONE);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration_, TextDecoration::UNDERLINE);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_OVERLINE);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration_, TextDecoration::OVERLINE);
    OH_Drawing_SetTextStyleDecoration(txtStyle, TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration_, TextDecoration::LINETHROUGH);
    OH_Drawing_SetTextStyleDecoration(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration_, TextDecoration::NONE);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationColor_, 0xFF000000);
    OH_Drawing_SetTextStyleDecorationColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decorationColor_, 0xFFFF0000);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->height_, 0.0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest013
 * @tc.desc: test for font families
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest013, TestSize.Level1)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    const char* fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    std::vector<std::string> fontFamiliesResult = {"Roboto"};
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontFamilies_, fontFamiliesResult);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontStyle_, FontStyle::NORMAL);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_ITALIC);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontStyle_, FontStyle::ITALIC);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontStyle_, FontStyle::NORMAL);
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
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->locale_, "en");
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
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle,
        OH_Drawing_CreateFontCollection());
    EXPECT_TRUE(handler != nullptr);
    
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);

    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
    double position[2] = {10.0, 15.0};
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    uint32_t width = 20;
    uint32_t height = 40;
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    EXPECT_EQ(width, OH_Drawing_BitmapGetWidth(cBitmap));
    EXPECT_EQ(height, OH_Drawing_BitmapGetHeight(cBitmap));

    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));

    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography) != 0.0, true);
    EXPECT_EQ(OH_Drawing_TypographyGetLongestLine(typography) != 0.0, true);
    EXPECT_EQ(OH_Drawing_TypographyGetMinIntrinsicWidth(typography) <=
        OH_Drawing_TypographyGetMaxIntrinsicWidth(typography), true);
    EXPECT_EQ(OH_Drawing_TypographyGetAlphabeticBaseline(typography) != 0.0, true);
    EXPECT_EQ(OH_Drawing_TypographyGetIdeographicBaseline(typography) != 0.0, true);
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}
}