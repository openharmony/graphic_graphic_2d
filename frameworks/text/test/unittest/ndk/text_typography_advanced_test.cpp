/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "text_typography_test_common.h"

namespace OHOS {

namespace {
OH_Drawing_Typography* PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment align, bool addPlaceholder,
    OH_Drawing_PlaceholderVerticalAlignment placeholderAlign =
        OH_Drawing_PlaceholderVerticalAlignment::ALIGNMENT_ABOVE_BASELINE)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyVerticalAlignment(typoStyle, align);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());

    if (addPlaceholder) {
        OH_Drawing_PlaceholderSpan placeholderSpan { 20, 20, placeholderAlign, TEXT_BASELINE_IDEOGRAPHIC, 0 };
        OH_Drawing_TypographyHandlerAddPlaceholder(handler, &placeholderSpan);
    }
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(txtStyle, DEFAULT_FONT_SIZE);
    const char* text = "ohos";
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_SetTextStyleFontSize(txtStyle, DEFAULT_FONT_SIZE + DEFAULT_FONT_SIZE);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
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
    return typography;
}

bool CompareRunBoundsBetweenTwoParagraphs(
    OH_Drawing_Typography* defaultParagraph, OH_Drawing_Typography* comparedParagraph)
{
    OH_Drawing_Array* defaultLines = OH_Drawing_TypographyGetTextLines(defaultParagraph);
    OH_Drawing_Array* comparedLines = OH_Drawing_TypographyGetTextLines(comparedParagraph);
    OH_Drawing_TextLine* defaultFirstLine = OH_Drawing_GetTextLineByIndex(defaultLines, 0);
    OH_Drawing_TextLine* comparedFirstLine = OH_Drawing_GetTextLineByIndex(comparedLines, 0);
    OH_Drawing_Array* defaultRuns = OH_Drawing_TextLineGetGlyphRuns(defaultFirstLine);
    OH_Drawing_Array* comparedRuns = OH_Drawing_TextLineGetGlyphRuns(comparedFirstLine);
    OH_Drawing_Run* defaultFirstRun = OH_Drawing_GetRunByIndex(defaultRuns, 0);
    OH_Drawing_Run* comparedFirstRun = OH_Drawing_GetRunByIndex(comparedRuns, 0);
    float ascent { 0.0f };
    float descent { 0.0f };
    float leading { 0.0f };
    OH_Drawing_GetRunTypographicBounds(defaultFirstRun, &ascent, &descent, &leading);
    float comparedAscent { 0.0f };
    float comparedDescent { 0.0f };
    float comparedLeading { 0.0f };
    OH_Drawing_GetRunTypographicBounds(comparedFirstRun, &comparedAscent, &comparedDescent, &comparedLeading);
    return skia::textlayout::nearlyEqual(ascent, comparedAscent) &&
           skia::textlayout::nearlyEqual(descent, comparedDescent);
}

bool ComparePlaceholderRectsBetweenTwoParagraphs(
    OH_Drawing_Typography* defaultParagraph, OH_Drawing_Typography* comparedParagraph)
{
    OH_Drawing_TextBox* defaultPlaceholderRect = OH_Drawing_TypographyGetRectsForPlaceholders(defaultParagraph);
    OH_Drawing_TextBox* comparedPlaceholderRect = OH_Drawing_TypographyGetRectsForPlaceholders(comparedParagraph);
    return skia::textlayout::nearlyEqual(OH_Drawing_GetTopFromTextBox(defaultPlaceholderRect, 0),
        OH_Drawing_GetTopFromTextBox(comparedPlaceholderRect, 0));
}

OH_Drawing_Typography* LayoutForSplitRunsEllipsisSample(
    const char* text, float layoutWidth, OH_Drawing_TextVerticalAlignment align)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, OH_Drawing_EllipsisModal::ELLIPSIS_MODAL_TAIL);
    OH_Drawing_SetTypographyVerticalAlignment(typoStyle, align);
    int maxLines = 12;
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, maxLines);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    double fontSize = 50;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, layoutWidth);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    return typography;
}

skia::textlayout::ParagraphImpl* GetSkiaParagraphByDrawingTypography(OH_Drawing_Typography* typography)
{
    OHOS::Rosen::Typography* rosenTypography = reinterpret_cast<OHOS::Rosen::Typography*>(typography);
    OHOS::Rosen::SPText::ParagraphImpl* paragraph =
        reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(rosenTypography->GetParagraph());
    return reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get());
}

bool CheckEllipsisRunIndex(skia::textlayout::ParagraphImpl* paragraph, size_t runIndex)
{
    auto lines = paragraph->lines();
    auto ellipsisLine = std::find_if(lines.begin(), lines.end(), [](const auto& line) { return line.ellipsis(); });
    if (ellipsisLine == lines.end()) {
        return false;
    }
    return ellipsisLine->ellipsis()->index() == runIndex;
}
} // namespace

/*
 * @tc.name: TypographyTest103
 * @tc.desc: test arc text offset
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest104, TestSize.Level0)
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
    OH_Drawing_TypographyPaintOnPath(nullptr, nullptr, nullptr, -1, -2);
    OH_Drawing_TypographyPaintOnPath(typography, cCanvas, cPath, ARC_FONT_SIZE, ARC_FONT_SIZE);
    OH_Drawing_TypographyPaintOnPath(typography, cCanvas, cPath, 1.5, 100);
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
 * @tc.name: TypographyTest104
 * @tc.desc: test arc text drawing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest103, TestSize.Level0)
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
 * @tc.name: TypographyHandlerPushTextStyleTest001
 * @tc.desc: test for the actual effective value of textstyle in each of the three scenarios.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TypographyHandlerPushTextStyleTest001, TestSize.Level0)
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
 * @tc.name: TypographyHandlerPushTextStyleTest002
 * @tc.desc: test the height of Tibetan and Uighur in push textstyle scenarios.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TypographyHandlerPushTextStyleTest002, TestSize.Level0)
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
    EXPECT_NE(OH_Drawing_TypographyGetHeight(typography5), 0);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(typography5), 240);
}

/*
 * @tc.name: TypographyBadgeTypeTest001
 * @tc.desc: Test for text's super script
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyBadgeTypeTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyCreate* superTxtHandler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(superTxtHandler, nullptr);

    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBadgeType(txtStyle, OH_Drawing_TextBadgeType::TEXT_BADGE_NONE);

    OH_Drawing_TextStyle* superTxtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(superTxtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(superTxtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_SetTextStyleFontWeight(superTxtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBadgeType(superTxtStyle, OH_Drawing_TextBadgeType::TEXT_SUPERSCRIPT);

    const char* text = "OpenHarmony";
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);

    OH_Drawing_TypographyHandlerPushTextStyle(superTxtHandler, superTxtStyle);
    OH_Drawing_TypographyHandlerAddText(superTxtHandler, text);
    OH_Drawing_Typography* superTxtTypography = OH_Drawing_CreateTypography(superTxtHandler);
    ASSERT_NE(superTxtTypography, nullptr);
    OH_Drawing_TypographyLayout(superTxtTypography, MAX_WIDTH);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLongestLine(typography), 334.8996887));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLongestLine(superTxtTypography), 217.6851959));
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);

    OH_Drawing_DestroyTypography(superTxtTypography);
    OH_Drawing_DestroyTypographyHandler(superTxtHandler);
    OH_Drawing_DestroyTextStyle(superTxtStyle);
}

/*
 * @tc.name: TypographyBadgeTypeTest002
 * @tc.desc: Test for text's sub script
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyBadgeTypeTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyCreate* subTxtHandler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(subTxtHandler, nullptr);

    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBadgeType(txtStyle, OH_Drawing_TextBadgeType::TEXT_BADGE_NONE);

    OH_Drawing_TextStyle* subTxtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(subTxtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(subTxtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_SetTextStyleFontWeight(subTxtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBadgeType(subTxtStyle, OH_Drawing_TextBadgeType::TEXT_SUBSCRIPT);

    const char* text = "你好世界";
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);

    OH_Drawing_TypographyHandlerPushTextStyle(subTxtHandler, subTxtStyle);
    OH_Drawing_TypographyHandlerAddText(subTxtHandler, text);
    OH_Drawing_Typography* subTxtTypography = OH_Drawing_CreateTypography(subTxtHandler);
    ASSERT_NE(subTxtTypography, nullptr);
    OH_Drawing_TypographyLayout(subTxtTypography, MAX_WIDTH);

    EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLongestLine(typography), 200));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLongestLine(subTxtTypography), 130)),
        OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);

    OH_Drawing_DestroyTypography(subTxtTypography);
    OH_Drawing_DestroyTypographyHandler(subTxtHandler);
    OH_Drawing_DestroyTextStyle(subTxtStyle);
}

/*
 * @tc.name: TypographyBadgeTypeTest003
 * @tc.desc: Test for text's badge valid params
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyBadgeTypeTest003, TestSize.Level0)
{
    EXPECT_NO_FATAL_FAILURE(OH_Drawing_SetTextStyleBadgeType(nullptr, OH_Drawing_TextBadgeType::TEXT_BADGE_NONE));
}

/*
 * @tc.name: TypographyVerticalTest001
 * @tc.desc: Test for vertical align valid params
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyVerticalTest001, TestSize.Level0)
{
    EXPECT_NO_FATAL_FAILURE(OH_Drawing_SetTypographyVerticalAlignment(
        nullptr, OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_BOTTOM));
}

/*
 * @tc.name: TypographyVerticalTest002
 * @tc.desc: Test for vertical align
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyVerticalTest002, TestSize.Level0)
{
    OH_Drawing_Typography* typographyOne =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_BASELINE, false);
    ASSERT_NE(typographyOne, nullptr);
    OH_Drawing_Typography* topAlignTypography =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_TOP, false);
    ASSERT_NE(topAlignTypography, nullptr);
    OH_Drawing_Typography* typographyTwo =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_BASELINE, false);
    ASSERT_NE(typographyTwo, nullptr);
    OH_Drawing_Typography* centerAlignTypography =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_CENTER, false);
    ASSERT_NE(centerAlignTypography, nullptr);
    OH_Drawing_Typography* typographyThree =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_BASELINE, false);
    ASSERT_NE(typographyThree, nullptr);
    OH_Drawing_Typography* bottomAlignTypography =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_BOTTOM, false);
    ASSERT_NE(bottomAlignTypography, nullptr);
    OH_Drawing_Typography* typographyWithPlaceholder =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_TOP, true,
            OH_Drawing_PlaceholderVerticalAlignment::ALIGNMENT_ABOVE_BASELINE);
    ASSERT_NE(typographyWithPlaceholder, nullptr);
    OH_Drawing_Typography* followTypographyWithPlaceholder =
        PrepareParagraphForVerticalAlign(OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_TOP, true,
            OH_Drawing_PlaceholderVerticalAlignment::ALIGNMENT_FOLLOW_PARAGRAPH);
    ASSERT_NE(followTypographyWithPlaceholder, nullptr);
    EXPECT_FALSE(CompareRunBoundsBetweenTwoParagraphs(typographyOne, topAlignTypography));
    EXPECT_FALSE(CompareRunBoundsBetweenTwoParagraphs(typographyTwo, centerAlignTypography));
    EXPECT_FALSE(CompareRunBoundsBetweenTwoParagraphs(typographyThree, bottomAlignTypography));
    EXPECT_FALSE(
        ComparePlaceholderRectsBetweenTwoParagraphs(typographyWithPlaceholder, followTypographyWithPlaceholder));
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsEllipsisTest001
 * @tc.desc: test for thai language triggering ellipsis and split runs scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, OH_Drawing_TypographySplitRunsEllipsisTest001, TestSize.Level0)
{
    const char* text = "অসমীয়া ভাষা ভাৰতৰ উত্তৰ-পূৱাঞ্চলৰ অসম ৰাজ্যৰ মুখ্য ভাষা। ইয়াৰ \
        সমৃদ্ধ সাহিত্যিক পৰম্পৰা আৰু সাংস্কৃতিক ঐতিহ্য আছে।";
    // Test for layout width 119
    OH_Drawing_Typography* paragraph =
        LayoutForSplitRunsEllipsisSample(text, 119, OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_BOTTOM);
    ASSERT_NE(paragraph, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraphByDrawingTypography(paragraph);
    // Test for ellipsis run index 17
    size_t expectEllipsisRunIndex = 17;
    EXPECT_TRUE(CheckEllipsisRunIndex(skiaParagraph, expectEllipsisRunIndex));
    OH_Drawing_DestroyTypography(paragraph);
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsEllipsisTest002
 * @tc.desc: test for thai language triggering ellipsis and split runs scenarios but layout width is 64
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, OH_Drawing_TypographySplitRunsEllipsisTest002, TestSize.Level0)
{
    const char* text = "অসমীয়া ভাষা ভাৰতৰ উত্তৰ-পূৱাঞ্চলৰ অসম ৰাজ্যৰ মুখ্য ভাষা। ইয়াৰ \
        সমৃদ্ধ সাহিত্যিক পৰম্পৰা আৰু সাংস্কৃতিক ঐতিহ্য আছে।";
    // Test for layout width 64
    OH_Drawing_Typography* paragraph =
        LayoutForSplitRunsEllipsisSample(text, 64, OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_CENTER);
    ASSERT_NE(paragraph, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraphByDrawingTypography(paragraph);
    ASSERT_NE(skiaParagraph, nullptr);
    // Test for ellipsis run index 14
    size_t expectEllipsisRunIndex = 14;
    EXPECT_TRUE(CheckEllipsisRunIndex(skiaParagraph, expectEllipsisRunIndex));
    OH_Drawing_DestroyTypography(paragraph);
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsEllipsisTest003
 * @tc.desc: test for uyghur language triggering ellipsis and split runs scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, OH_Drawing_TypographySplitRunsEllipsisTest003, TestSize.Level0)
{
    const char* text =
        " ئۇيغۇرچە، ياكى ئۇيغۇر تىلى، ئاساسلىقى شىنجاڭ ئۇيغۇر ئاپتونوم رايونى ۋە ئەتراپىدىكى رايونلاردا ئى\
        شلىتىلىدۇ. ئۇيغۇرچە ئۇيغۇرلارنىڭ ئانىلىك تىلى بولۇپ، ئۇزاق تارىخقا ۋە مول مەدەنىيەت ميراسقا ئىگە";
    // Test for layout width 64
    OH_Drawing_Typography* paragraph =
        LayoutForSplitRunsEllipsisSample(text, 64, OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_CENTER);
    ASSERT_NE(paragraph, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraphByDrawingTypography(paragraph);
    ASSERT_NE(skiaParagraph, nullptr);
    // Test for ellipsis run index 14
    size_t expectEllipsisRunIndex = 14;
    EXPECT_TRUE(CheckEllipsisRunIndex(skiaParagraph, expectEllipsisRunIndex));
    OH_Drawing_DestroyTypography(paragraph);
}

/*
 * @tc.name: OH_Drawing_TypographySplitRunsEllipsisTest004
 * @tc.desc: test for chinese language triggering ellipsis and split runs scenarios
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, OH_Drawing_TypographySplitRunsEllipsisTest004, TestSize.Level0)
{
    const char* text = "比如单个字母出现的时候，比如「嫌疑者X的牺牲」中，这个X前后便有1/8EM的空格，因为单个字母太窄了";
    // Test for layout width 175
    OH_Drawing_Typography* paragraph =
        LayoutForSplitRunsEllipsisSample(text, 175, OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_TOP);
    ASSERT_NE(paragraph, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraphByDrawingTypography(paragraph);
    ASSERT_NE(skiaParagraph, nullptr);
    // Test for ellipsis run index 16
    size_t expectEllipsisRunIndex = 16;
    EXPECT_TRUE(CheckEllipsisRunIndex(skiaParagraph, expectEllipsisRunIndex));
    OH_Drawing_DestroyTypography(paragraph);
}

} // namespace OHOS
