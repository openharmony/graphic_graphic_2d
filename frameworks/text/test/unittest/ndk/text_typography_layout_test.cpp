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
 * @tc.name: TypographyTest016
 * @tc.desc: test for typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest016, TestSize.Level0)
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
 * @tc.name: TypographyTest026
 * @tc.desc: test for typography and txtStyle
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest026, TestSize.Level0)
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
 * @tc.name: TypographyTest027
 * @tc.desc: test for getting line info for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest027, TestSize.Level0)
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
 * @tc.name: TypographyTest044
 * @tc.desc: test for effectiveAlignment, isLineUnlimited, isEllipsized for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest044, TestSize.Level0)
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
 * @tc.name: TypographyTest054
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest054, TestSize.Level0)
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
 * @tc.name: TypographyTest055
 * @tc.desc: test for unresolved glyphs count of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest055, TestSize.Level0)
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
 * @tc.name: TypographyTest056
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest056, TestSize.Level0)
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
 * @tc.name: TypographyTest073
 * @tc.desc: test for getting all font metrics array from current line
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest073, TestSize.Level0)
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
 * @tc.name: TypographyTest089
 * @tc.desc: test for getting line  metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest089, TestSize.Level0)
{
    OH_Drawing_Typography* typography = nullptr;
    OH_Drawing_LineMetrics* vectorMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    EXPECT_EQ(vectorMetrics, nullptr);
}
/*
 * @tc.name: TypographyTest092
 * @tc.desc: test for getting indets of index for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest092, TestSize.Level0)
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
 * @tc.name: TypographyTest093
 * @tc.desc: test for getting line font metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest093, TestSize.Level0)
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
 * @tc.name: TypographyTest105
 * @tc.desc: test for the text box
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest105, TestSize.Level0)
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
 * @tc.name: TypographyTest106
 * @tc.desc: test for the textbox.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest106, TestSize.Level0)
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
 * @tc.name: TypographyTest112
 * @tc.desc: test for truncated emoji text drawing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest112, TestSize.Level0)
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
 * @tc.name: TypographyTest113
 * @tc.desc: test for the input nullptr.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TypographyTest113, TestSize.Level0)
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
 * @tc.name: TypographyGetLineMetricsTest001
 * @tc.desc: test for the OH_Drawing_TypographyGetLineMetrics.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TypographyGetLineMetricsTest001, TestSize.Level0)
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
 * @tc.name: TypographyGetLineFontMetricsTest001
 * @tc.desc: test for the OH_Drawing_TypographyGetLineFontMetrics.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TypographyGetLineFontMetricsTest001, TestSize.Level0)
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
 * @tc.name: TypographyLineInfoTest001
 * @tc.desc: test for getting line info and line font metrics for one line with white space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyLineInfoTest001, TestSize.Level0)
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
 * @tc.name: TypographyLineInfoTest002
 * @tc.desc: test for getting line info and line font metrics for one line with white space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyLineInfoTest002, TestSize.Level0)
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
 * @tc.name: TypographyRectsForRangeTest001
 * @tc.desc: test for typography rectsForRange
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyRectsForRangeTest001, TestSize.Level0)
{
    OH_Drawing_TypographyCreate* handler = nullptr;
    OH_Drawing_Typography* typography = CreateTypographyForRangeTest(&handler);
    for (const auto& testCase : RANGE_TEST_RESULT) {
        VerifyTextBoxResult(typography, testCase);
    }
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}
} // namespace OHOS
