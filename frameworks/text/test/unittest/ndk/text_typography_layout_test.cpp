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
/*
 * @tc.name: TypographyLayoutWithIndentsAndBaselinesTest
 * @tc.desc: test for typography layout with indents, baselines, max width, height and paint
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyLayoutWithIndentsAndBaselinesTest, TestSize.Level0)
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
 * @tc.name: TypographyPlaceholderAndTextQueryTest
 * @tc.desc: test for typography with placeholder span, rectsForRange, glyphPosition, wordBoundary and lineCount
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyPlaceholderAndTextQueryTest, TestSize.Level0)
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
 * @tc.name: TypographyGetLineInfoTest
 * @tc.desc: test for getting line info for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyGetLineInfoTest, TestSize.Level0)
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
 * @tc.name: TypographyEffectiveAlignmentAndEllipsisTest
 * @tc.desc: test for effectiveAlignment, isLineUnlimited, isEllipsized, fontMetrics and typography properties
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyEffectiveAlignmentAndEllipsisTest, TestSize.Level0)
{
    CreateTypographyHandler();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, txtStyle);
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyHandlerAddText(fHandler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(fHandler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(fHandler);
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
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, text);
    OH_Drawing_SetTypographyTextLocale(fTypoStyle, text);
    OH_Drawing_SetTypographyTextSplitRatio(fTypoStyle, fontSize);
    OH_Drawing_TypographyGetTextStyle(fTypoStyle);
    EXPECT_EQ(OH_Drawing_TypographyGetEffectiveAlignment(fTypoStyle), 0);
    EXPECT_TRUE(OH_Drawing_TypographyIsLineUnlimited(fTypoStyle));
    EXPECT_TRUE(OH_Drawing_TypographyIsEllipsized(fTypoStyle));
    OH_Drawing_SetTypographyTextStyle(fTypoStyle, txtStyle);
    OH_Drawing_TypographyLayout(typography, fLayoutWidth);
    OH_Drawing_PositionAndAffinity* positionAndAffinity =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 100, 10);
    EXPECT_EQ(OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity), 2);
    OH_Drawing_DestroyTypography(typography);
}
/*
 * @tc.name: TypographyHalfLeadingAndMarkDirtyTest
 * @tc.desc: test for halfLeading, line style, markDirty and longestLineWithIndent of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyHalfLeadingAndMarkDirtyTest, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText();
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 118);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 45);
    OH_Drawing_TypographyMarkDirty(fTypography);
    OH_Drawing_TypographyMarkDirty(nullptr);
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 118);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 45);
    Paint();
}
/*
 * @tc.name: TypographyGetUnresolvedGlyphsCountTest
 * @tc.desc: test for unresolved glyphs count of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyGetUnresolvedGlyphsCountTest, TestSize.Level0)
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
 * @tc.name: TypographyUpdateFontSizeTest
 * @tc.desc: test for updating font size dynamically in text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyUpdateFontSizeTest, TestSize.Level0)
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
 * @tc.name: TypographyGetLineFontMetricsBasicTest
 * @tc.desc: test for getting all font metrics array from current line
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyGetLineFontMetricsBasicTest, TestSize.Level0)
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
 * @tc.name: TypographyGetLineMetricsNullptrTest
 * @tc.desc: test for getting line metrics with nullptr typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyGetLineMetricsNullptrTest, TestSize.Level0)
{
    OH_Drawing_Typography* typography = nullptr;
    OH_Drawing_LineMetrics* vectorMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    EXPECT_EQ(vectorMetrics, nullptr);
}
/*
 * @tc.name: TypographyGetIndentsWithIndexTest
 * @tc.desc: test for getting indents by index for text typography including nullptr and out-of-bounds
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyGetIndentsWithIndexTest, TestSize.Level0)
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
 * @tc.name: TypographyGetLineFontMetricsEdgeCaseTest
 * @tc.desc: test for getting line font metrics with nullptr and edge cases
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyGetLineFontMetricsEdgeCaseTest, TestSize.Level0)
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
 * @tc.name: TypographyPlaceholderAlignmentAndTextBoxTest
 * @tc.desc: test for placeholder spans with various alignments, textBox getters, word boundary, line height and width
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyPlaceholderAlignmentAndTextBoxTest, TestSize.Level0)
{
    CreateTypographyHandler();

    OH_Drawing_PlaceholderSpan placeholderSpan1 = { 150, 150, ALIGNMENT_OFFSET_AT_BASELINE, TEXT_BASELINE_IDEOGRAPHIC,
        0 };
    OH_Drawing_TypographyHandlerAddPlaceholder(fHandler, &placeholderSpan1);

    OH_Drawing_PlaceholderSpan placeholderSpan2 = { 100, 100, ALIGNMENT_ABOVE_BASELINE, TEXT_BASELINE_ALPHABETIC, 0 };
    OH_Drawing_TypographyHandlerAddPlaceholder(fHandler, &placeholderSpan2);

    OH_Drawing_PlaceholderSpan placeholderSpan3 = { 150, 150, ALIGNMENT_BELOW_BASELINE, TEXT_BASELINE_IDEOGRAPHIC, 0 };
    OH_Drawing_TypographyHandlerAddPlaceholder(fHandler, &placeholderSpan3);

    OH_Drawing_PlaceholderSpan placeholderSpan4 = { 70.5, 70.5, ALIGNMENT_TOP_OF_ROW_BOX, TEXT_BASELINE_ALPHABETIC, 0 };
    OH_Drawing_TypographyHandlerAddPlaceholder(fHandler, &placeholderSpan4);

    OH_Drawing_PlaceholderSpan placeholderSpan5 = { 150, 150, ALIGNMENT_BOTTOM_OF_ROW_BOX, TEXT_BASELINE_IDEOGRAPHIC,
        0 };
    OH_Drawing_TypographyHandlerAddPlaceholder(fHandler, &placeholderSpan5);

    OH_Drawing_PlaceholderSpan placeholderSpan6 = { 0, 0, ALIGNMENT_CENTER_OF_ROW_BOX, TEXT_BASELINE_ALPHABETIC, 0 };
    OH_Drawing_TypographyHandlerAddPlaceholder(fHandler, &placeholderSpan6);

    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();

    OH_Drawing_TextBox* textBox = OH_Drawing_TypographyGetRectsForPlaceholders(fTypography);
    EXPECT_EQ(OH_Drawing_GetLeftFromTextBox(textBox, 0), 0);
    EXPECT_EQ(OH_Drawing_GetRightFromTextBox(textBox, 0), 150);
    EXPECT_EQ(::round(OH_Drawing_GetTopFromTextBox(textBox, 0)), 0);
    EXPECT_EQ(::round(OH_Drawing_GetBottomFromTextBox(textBox, 0)), 150);
    EXPECT_EQ(OH_Drawing_GetTextDirectionFromTextBox(textBox, 0), 1);
    EXPECT_EQ(OH_Drawing_GetSizeOfTextBox(textBox), 6);

    OH_Drawing_PositionAndAffinity* positionAndAffinity =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinate(fTypography, 50.5, 10.5);
    EXPECT_EQ(OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity), 0);
    EXPECT_EQ(OH_Drawing_GetPositionFromPositionAndAffinity(nullptr), 0);
    EXPECT_EQ(OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity), 0);
    EXPECT_EQ(OH_Drawing_GetAffinityFromPositionAndAffinity(nullptr), 0);

    OH_Drawing_Range* range = OH_Drawing_TypographyGetWordBoundary(fTypography, 1);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(range), 1);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(nullptr), 0);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(range), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(nullptr), 0);
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 1), 112);
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(nullptr, 1), 0);
    EXPECT_EQ(OH_Drawing_TypographyGetLineWidth(fTypography, 1), 100);
    EXPECT_EQ(OH_Drawing_TypographyGetLineWidth(nullptr, 1), 0);
}
/*
 * @tc.name: TypographyPlaceholderRectsTest
 * @tc.desc: test for getting placeholder textBox rects after layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyPlaceholderRectsTest, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());

    OH_Drawing_PlaceholderSpan* placeholderSpan = new OH_Drawing_PlaceholderSpan();
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
 * @tc.name: TypographyTruncatedEmojiDrawingTest
 * @tc.desc: test for truncated emoji text drawing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTruncatedEmojiDrawingTest, TestSize.Level0)
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
 * @tc.name: TypographyHandlerNullptrInputTest
 * @tc.desc: test for the input nullptr.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TypographyHandlerNullptrInputTest, TestSize.Level0)
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
 * @tc.name: GetLineMetricsWithNoText
 * @tc.desc: test for the OH_Drawing_TypographyGetLineMetrics.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, GetLineMetricsWithNoText, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_LineMetrics* lineMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    ASSERT_EQ(lineMetrics, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}
/*
 * @tc.name: GetLineFontMetricsWithNullptrAndEdgeCases
 * @tc.desc: test for the OH_Drawing_TypographyGetLineFontMetrics.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, GetLineFontMetricsWithNullptrAndEdgeCases, TestSize.Level0)
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
 * @tc.name: LineInfoAndFontMetricsSingleLine
 * @tc.desc: test for getting line info and line font metrics for one line with white space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, LineInfoAndFontMetricsSingleLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e,
        0x73 };
    const char* fontFamilies[] = { fontFamiliesTest };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一个 test123排版信息获取接口   ";
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
 * @tc.name: LineInfoAndFontMetricsMultiLine
 * @tc.desc: test for getting line info and line font metrics for one line with white space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, LineInfoAndFontMetricsMultiLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e,
        0x73 };
    const char* fontFamilies[] = { fontFamiliesTest };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一个 test123排版信息获取接口   123444  Test  ";
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
    OH_Drawing_Font_Metrics* metrics = OH_Drawing_TypographyGetLineFontMetrics(typography, 2, &fontMetricsSize);
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

std::vector<TextBoxTestCase> RANGE_TEST_RESULT = { { 0, 2, RECT_HEIGHT_STYLE_MAX, RECT_WIDTH_STYLE_TIGHT, 0.0f, 0.0f,
                                                       239.999786f, 360.000000f },
    { 5, 7, RECT_HEIGHT_STYLE_STRUCT, RECT_WIDTH_STYLE_TIGHT, 599.999451f, 60.0f, 839.999207f, 300.0f },
    { 34, 36, RECT_HEIGHT_STYLE_INCLUDELINESPACETOP, RECT_WIDTH_STYLE_TIGHT, 806.039307f, 720.000000f, 1046.039062f,
        1080.000000f },
    { 37, 39, RECT_HEIGHT_STYLE_INCLUDELINESPACEMIDDLE, RECT_WIDTH_STYLE_TIGHT, 0.000000f, 1149.337036f, 239.999817f,
        1509.337036f },
    { 41, 43, RECT_HEIGHT_STYLE_INCLUDELINESPACEBOTTOM, RECT_WIDTH_STYLE_TIGHT, 479.999573f, 1218.674072f, 719.999329f,
        1578.674072f },
    { 45, 47, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT, 959.999084f, 1218.674072f, 1199.998779f, 1359.314209f },
    { 49, 51, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_MAX, 240.000000f, 1578.674072f, 480.000000f, 1719.314209f } };

static OH_Drawing_Typography* CreateTypographyForRangeTest(OH_Drawing_TypographyCreate** handler)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_StrutStyle strutStyle = { .weight = FONT_WEIGHT_400,
        .style = FONT_STYLE_NORMAL,
        .size = 120.0,
        .heightScale = 2,
        .heightOverride = true,
        .halfLeading = true,
        .leading = 1.0 };
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, &strutStyle);
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, true);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    double fontSize = 120.0;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    *handler = OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(*handler, txtStyle);
    const char* text = "这是一个排版信息获取接口的测试文本: Hello World";
    OH_Drawing_TypographyHandlerAddText(*handler, text);
    text = "这是一个排版信息获取接口的排版信息排版信息排版信息排版信息.";
    OH_Drawing_TypographyHandlerAddText(*handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(*handler);
    double layoutWidth = 1200.0;
    OH_Drawing_TypographyLayout(typography, layoutWidth);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    return typography;
}

static void VerifyTextBoxResult(OH_Drawing_Typography* typography, const TextBoxTestCase& testCase)
{
    auto rect = OH_Drawing_TypographyGetRectsForRange(
        typography, testCase.start, testCase.end, testCase.heightStyle, testCase.widthStyle);
    EXPECT_NEAR(OH_Drawing_GetLeftFromTextBox(rect, 0), testCase.expectedLeft, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_GetRightFromTextBox(rect, 0), testCase.expectedRight, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_GetTopFromTextBox(rect, 0), testCase.expectedTop, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_GetBottomFromTextBox(rect, 0), testCase.expectedBottom, FLOAT_DATA_EPSILON);
    OH_Drawing_TypographyDestroyTextBox(rect);
}

/*
 * @tc.name: RectsForRangeWithVariousHeightStyles
 * @tc.desc: test for typography rectsForRange
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, RectsForRangeWithVariousHeightStyles, TestSize.Level0)
{
    OH_Drawing_TypographyCreate* handler = nullptr;
    OH_Drawing_Typography* typography = CreateTypographyForRangeTest(&handler);
    for (const auto& testCase : RANGE_TEST_RESULT) {
        VerifyTextBoxResult(typography, testCase);
    }
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: SetTailIndents_EmptyArray
 * @tc.desc: test for setting empty tail indent array
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetTailIndents_EmptyArray, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一段测试文本用于验证行尾缩进";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    double tailIndents[] = {};
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 0);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 799.9991455078125);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetTailIndents_PositiveValue
 * @tc.desc: test for setting positive tail indent value
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetTailIndents_PositiveValue, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    double tailIndents[] = { 50.0 };
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一段测试文本用于验证行尾缩进正值效果";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 849.99920654296875);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetTailIndents_NegativeValue
 * @tc.desc: test for setting negative tail indent value
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetTailIndents_NegativeValue, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    double tailIndents[] = { -20.0 };
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一段测试文本用于验证行尾缩进负值效果";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 799.9991455078125);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetTailIndents_ZeroValue
 * @tc.desc: test for setting zero tail indent value
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetTailIndents_ZeroValue, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    double tailIndents[] = { 0.0 };
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一段测试文本用于验证行尾缩进零值效果";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 799.9991455078125);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetBothIndents_Combined
 * @tc.desc: test for setting both head and tail indents
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetBothIndents_Combined, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    double headIndents[] = { 40.0 };
    double tailIndents[] = { 30.0 };
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, headIndents, 1);
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一段测试文本用于验证左右缩进组合效果";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 799.999267578125);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetIndents_FirstLine
 * @tc.desc: test for setting first line head indent
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetIndents_FirstLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeDouble(typoStyle, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, 40.0);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一段测试文本用于验证首行缩进效果";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 789.99920654296875);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetIndents_MultipleParagraphs
 * @tc.desc: test for setting first line indent with multiple paragraphs
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetIndents_MultipleParagraphs, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeDouble(typoStyle, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, 40.0);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "第一段文本\n第二段文本\n第三段文本";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 289.999755859375);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetTailIndents_LargerThanWidth
 * @tc.desc: test for setting tail indent larger than layout width
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetTailIndents_LargerThanWidth, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    double tailIndents[] = { 900.0 }; // larger than MAX_WIDTH (800)
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "测试文本";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Should not crash when indent exceeds width
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 1850);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: SetBothIndents_ExceedWidth
 * @tc.desc: test for setting both indents exceeding layout width
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetBothIndents_ExceedWidth, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    double headIndents[] = { 500.0 };
    double tailIndents[] = { 500.0 }; // 500 + 500 = 1000 > 800
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, headIndents, 1);
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "测试文本";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Should not crash when combined indents exceed width
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLineWithIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    EXPECT_DOUBLE_EQ(longestLineWithIndent, 1550);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

skia::textlayout::ParagraphImpl* GetSkiaParagraphByTypography(OH_Drawing_Typography* typography)
{
    OHOS::Rosen::Typography* rosenTypography = reinterpret_cast<OHOS::Rosen::Typography*>(typography);
    OHOS::Rosen::SPText::ParagraphImpl* paragraph =
        reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(rosenTypography->GetParagraph());
    return reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get());
}

// Helper function to test zero-width character fallback
// Tests that default ignorable unicode characters are properly handled when
// the font doesn't have glyphs, using zero-width placeholder mechanism
// expectedValues: array of 3 ints containing expected values for:
//   [0] = expected number of runs
//   [1] = expected number of glyphs in the zero-width character run
//   [2] = expected glyph ID for the zero-width character
static void TestZeroWidthCharFallback(const char* ignoreChar, const char* charDescription, const int expectedValues[3])
{
    const char* text0 = "test1\n";
    const char* text1 = "\ntest1";

    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);

    // Set vertical alignment to CENTER
    OH_Drawing_SetTypographyVerticalAlignment(
        typoStyle, OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_CENTER);

    const char* fontFamilies[] = { "notdef" };
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyle* normalTextStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    ASSERT_NE(normalTextStyle, nullptr);

    // FontSize 14
    OH_Drawing_SetTextStyleFontSize(normalTextStyle, 14);
    OH_Drawing_SetTextStyleFontFamilies(normalTextStyle, 1, fontFamilies);

    // FontSize 14
    OH_Drawing_SetTextStyleFontSize(txtStyle, 14);
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    // LineHeight 50
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 50);

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);

    // 1. Normal style for "test1\n"
    OH_Drawing_TypographyHandlerPushTextStyle(handler, normalTextStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text0);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    // 2. "notdef" font style for zero-width character
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, ignoreChar);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    // 3. Normal style for "\ntest1"
    OH_Drawing_TypographyHandlerPushTextStyle(handler, normalTextStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text1);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);

    // Layout width is 500, should not crash
    OH_Drawing_TypographyLayout(typography, 500);

    // Verify runs and glyphs (matching original test)
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraphByTypography(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    auto runs = skiaParagraph->runs();
    ASSERT_EQ(runs.size(), expectedValues[0]);  // Expected number of runs
    int zeroWidthRunIndex = 2;
    int expectedGlyphIndex = 2;
    // Expected number of glyphs in zero-width character run
    ASSERT_EQ(runs[zeroWidthRunIndex].glyphs().size(), expectedValues[1]);
    // Expected glyph ID for zero-width character
    EXPECT_EQ(runs[zeroWidthRunIndex].glyphs()[0], expectedValues[expectedGlyphIndex]);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(normalTextStyle);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest001
 * @tc.desc: Test zero-width space (U+200B) - ZERO WIDTH SPACE (ZWSP)
 *           This is the most commonly used default ignorable character
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest001, TestSize.Level1)
{
    const char* ignoreChar = "\u200B";  // ZERO WIDTH SPACE
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 3 glyph, glyph ID 2
    TestZeroWidthCharFallback(ignoreChar, "U+200B ZERO WIDTH SPACE (ZWSP)", expectedValues);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest002
 * @tc.desc: Test zero-width joiner characters (U+200C, U+200D)
 *           ZWNJ and ZWJ are commonly used in Indic scripts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest002, TestSize.Level1)
{
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 1 glyph, glyph ID 2

    // Test U+200C ZERO WIDTH NON-JOINER
    const char* ignoreChar1 = "\u200C";
    TestZeroWidthCharFallback(ignoreChar1, "U+200C ZERO WIDTH NON-JOINER (ZWNJ)", expectedValues);

    // Test U+200D ZERO WIDTH JOINER
    const char* ignoreChar2 = "\u200D";
    TestZeroWidthCharFallback(ignoreChar2, "U+200D ZERO WIDTH JOINER (ZWJ)", expectedValues);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest003
 * @tc.desc: Test word joiner and BOM (U+2060, U+FEFF)
 *           WORD JOINER and ZERO WIDTH NO-BREAK SPACE (BOM)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest003, TestSize.Level1)
{
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 3 glyph, glyph ID 2

    // Test U+2060 WORD JOINER
    const char* ignoreChar1 = "\u2060";
    TestZeroWidthCharFallback(ignoreChar1, "U+2060 WORD JOINER", expectedValues);

    const int expectedValuesTwo[] = {5, 3, 2};  // 5 runs, 2 glyph, glyph ID 2
    // Test U+FEFF ZERO WIDTH NO-BREAK SPACE (BOM)
    const char* ignoreChar2 = "\uFEFF";
    TestZeroWidthCharFallback(ignoreChar2, "U+FEFF ZERO WIDTH NO-BREAK SPACE (BOM)", expectedValuesTwo);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest004
 * @tc.desc: Test soft hyphen and combining grapheme joiner (U+00AD, U+034F)
 *           Format control characters that may not have glyphs in some fonts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest004, TestSize.Level1)
{
    const int expectedValues[] = {5, 2, 2};  // 5 runs, 2 glyph, glyph ID 2

    // Test U+00AD SOFT HYPHEN
    const char* ignoreChar1 = "\u00AD";
    TestZeroWidthCharFallback(ignoreChar1, "U+00AD SOFT HYPHEN (SHY)", expectedValues);

    const int expectedValuesTwo[] = {5, 2, 2};  // 5 runs, 2 glyph, glyph ID 2
    // Test U+034F COMBINING GRAPHEME JOINER
    const char* ignoreChar2 = "\u034F";
    TestZeroWidthCharFallback(ignoreChar2, "U+034F COMBINING GRAPHEME JOINER (CGJ)", expectedValuesTwo);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest005
 * @tc.desc: Test bidirectional control characters (U+200E, U+200F, U+202A-202E)
 *           LTR/RTL marks and BiDi control characters for text direction
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest005, TestSize.Level1)
{
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 3 glyph, glyph ID 2

    // Test bidirectional marks
    const char* bidiMark1 = "\u200E";  // LEFT-TO-RIGHT MARK
    const char* bidiMark2 = "\u200F";  // RIGHT-TO-LEFT MARK
    const char* bidiControl1 = "\u202A";  // LEFT-TO-RIGHT EMBEDDING
    const char* bidiControl2 = "\u202B";  // RIGHT-TO-LEFT EMBEDDING
    const char* bidiControl3 = "\u202C";  // POP DIRECTIONAL FORMATTING
    const char* bidiControl4 = "\u202D";  // LEFT-TO-RIGHT OVERRIDE
    const char* bidiControl5 = "\u202E";  // RIGHT-TO-LEFT OVERRIDE

    TestZeroWidthCharFallback(bidiMark1, "U+200E LEFT-TO-RIGHT MARK", expectedValues);
    TestZeroWidthCharFallback(bidiMark2, "U+200F RIGHT-TO-LEFT MARK", expectedValues);
    TestZeroWidthCharFallback(bidiControl1, "U+202A LEFT-TO-RIGHT EMBEDDING", expectedValues);
    TestZeroWidthCharFallback(bidiControl2, "U+202B RIGHT-TO-LEFT EMBEDDING", expectedValues);
    TestZeroWidthCharFallback(bidiControl3, "U+202C POP DIRECTIONAL FORMATTING", expectedValues);
    TestZeroWidthCharFallback(bidiControl4, "U+202D LEFT-TO-RIGHT OVERRIDE", expectedValues);
    TestZeroWidthCharFallback(bidiControl5, "U+202E RIGHT-TO-LEFT OVERRIDE", expectedValues);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest006
 * @tc.desc: Test Mongolian vowel separator range (U+180B-U+180E)
 *           Format control characters for Mongolian script
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest006, TestSize.Level1)
{
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 3 glyph, glyph ID 2

    // Test U+180B-0x180E range (4 characters)
    const char* mongolian1 = "\u180B";  // MONGOLIAN BIRGA
    const char* mongolian2 = "\u180C";  // MONGOLIAN VIRGULE
    const char* mongolian3 = "\u180D";  // MONGOLIAN DOT BEFORE
    const char* mongolian4 = "\u180E";  // MONGOLIAN NUN BV

    TestZeroWidthCharFallback(mongolian1, "U+180B MONGOLIAN BIRGA", expectedValues);
    TestZeroWidthCharFallback(mongolian2, "U+180C MONGOLIAN VIRGULE", expectedValues);
    TestZeroWidthCharFallback(mongolian3, "U+180D MONGOLIAN DOT BEFORE", expectedValues);
    TestZeroWidthCharFallback(mongolian4, "U+180E MONGOLIAN NUN BV", expectedValues);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest007
 * @tc.desc: Test variation selectors (U+FE00-U+FE0F) + BOM
 *           Variation Selectors used with emoji and special characters
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest007, TestSize.Level1)
{
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 3 glyph, glyph ID 2

    // Test variation selector range (selected representatives)
    const char* varSel1 = "\uFE00";  // VARIATION SELECTOR-1
    const char* varSel2 = "\uFE01";  // VARIATION SELECTOR-2
    const char* varSel3 = "\uFE0F";  // VARIATION SELECTOR-16

    TestZeroWidthCharFallback(varSel1, "U+FE00 VARIATION SELECTOR-1", expectedValues);
    TestZeroWidthCharFallback(varSel2, "U+FE01 VARIATION SELECTOR-2", expectedValues);
    TestZeroWidthCharFallback(varSel3, "U+FE0F VARIATION SELECTOR-16", expectedValues);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest008
 * @tc.desc: Test Tagalog range (U+17B4-U+17B5)
 *           Special control characters for Tagalog script
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest008, TestSize.Level1)
{
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 3 glyph, glyph ID 2

    // Test U+17B4-0x17B5 range (2 characters)
    const char* tagalog1 = "\u17B4";  // TAGALOG LETTER A
    const char* tagalog2 = "\u17B5";  // TAGALOG LETTER A

    TestZeroWidthCharFallback(tagalog1, "U+17B4 TAGALOG LETTER A", expectedValues);
    TestZeroWidthCharFallback(tagalog2, "U+17B5 TAGALOG LETTER A", expectedValues);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest009
 * @tc.desc: Test music range in plane 1 (U+1D173-U+D17A)
 *           Musical symbols and control characters
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest009, TestSize.Level1)
{
    const int expectedValues[] = {5, 4, 2};  // 5 runs, 4 glyph, glyph ID 2

    // Test U+1D173 range (selected representatives)
    const char* music1 = "\U0001D173";  // MUSICAL SYMBOL BEGIN BEAM
    const char* music2 = "\U0001D174";  // MUSICAL SYMBOL END BEAM
    const char* music3 = "\U0001D175";  // MUSICAL SYMBOL BEGIN TIE
    const char* music4 = "\U0001D176";  // MUSICAL SYMBOL END TIE

    TestZeroWidthCharFallback(music1, "U+1D173 MUSICAL SYMBOL BEGIN BEAM", expectedValues);
    TestZeroWidthCharFallback(music2, "U+1D174 MUSICAL SYMBOL END BEAM", expectedValues);
    TestZeroWidthCharFallback(music3, "U+1D175 MUSICAL SYMBOL BEGIN TIE", expectedValues);
    TestZeroWidthCharFallback(music4, "U+1D176 MUSICAL SYMBOL END TIE", expectedValues);
}

/*
 * @tc.name: TypographyDefaultIgnorableCharsTest010
 * @tc.desc: Test CJK compatibility ideographs range (U+FFF0-U+FFF8)
 *           CJK compatibility ideographs that are default ignorable
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyDefaultIgnorableCharsTest010, TestSize.Level1)
{
    const int expectedValues[] = {5, 3, 2};  // 5 runs, 3 glyph, glyph ID 2

    // Test CJK compatibility ideographs (selected representatives)
    const char* cjk1 = "\uFFF0";  // CJK COMPATIBILITY IDEOGRAPH-0F00
    const char* cjk2 = "\uFFF1";  // CJK COMPATIBILITY IDEOGRAPH-0F01
    const char* cjk3 = "\uFFF8";  // CJK COMPATIBILITY IDEOGRAPH-0F08

    TestZeroWidthCharFallback(cjk1, "U+FFF0 CJK COMPATIBILITY IDEOGRAPH-0F00", expectedValues);
    TestZeroWidthCharFallback(cjk2, "U+FFF1 CJK COMPATIBILITY IDEOGRAPH-0F01", expectedValues);
    TestZeroWidthCharFallback(cjk3, "U+FFF8 CJK COMPATIBILITY IDEOGRAPH-0F08", expectedValues);
}
} // namespace OHOS
