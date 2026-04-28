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
 * @tc.name: LineHeightStyleByFontSize
 * @tc.desc: Test for lineHeightStyle fontSize
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, LineHeightStyleByFontSize, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    OH_Drawing_SetTextStyleAttributeInt(txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE,
        OH_Drawing_LineHeightStyle::TEXT_LINE_HEIGHT_BY_FONT_SIZE);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt; ++i) {
        // Test for every single line width 40
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 40.0));
    }
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: LineHeightStyleByFontHeight
 * @tc.desc: Test for lineHeightStyle fontHeight
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, LineHeightStyleByFontHeight, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    OH_Drawing_SetTextStyleAttributeInt(txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE,
        OH_Drawing_LineHeightStyle::TEXT_LINE_HEIGHT_BY_FONT_HEIGHT);
    int lineHeightStyle = 0;
    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetTextStyleAttributeInt(
        txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, &lineHeightStyle);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 1);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt; ++i) {
        // Test for every single line width 47
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 47.0));
    }
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}
/*
 * @tc.name: LineHeightStyleInvalidParamOutOfRange
 * @tc.desc: Test for lineHeightStyle invalid param
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, LineHeightStyleInvalidParamOutOfRange, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    // Invalid param 3
    int heightStyle = 3;
    OH_Drawing_ErrorCode styleErrorCode = OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, heightStyle);
    EXPECT_EQ(styleErrorCode, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_ErrorCode paragraphStyleErrorCode = OH_Drawing_SetTypographyStyleAttributeInt(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, heightStyle);
    EXPECT_EQ(paragraphStyleErrorCode, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    // Invalid param 3
    int invalidHeightStyle = -1;
    OH_Drawing_ErrorCode paragraphStyleErrorCodeTwo = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, invalidHeightStyle);
    EXPECT_EQ(paragraphStyleErrorCodeTwo, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
/*
 * @tc.name: AttributeSetterInvalidParam
 * @tc.desc: Test for attribute setter invalid param
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, AttributeSetterInvalidParam, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    OH_Drawing_ErrorCode errorCodeOne = OH_Drawing_SetTextStyleAttributeInt(
        nullptr, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, 0);
    EXPECT_EQ(errorCodeOne, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCodeTwo = OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 0);
    EXPECT_EQ(errorCodeTwo, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_ErrorCode errorCodeThree = OH_Drawing_SetTextStyleAttributeDouble(
        nullptr, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, 0.0f);
    EXPECT_EQ(errorCodeThree, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCodeFour = OH_Drawing_SetTextStyleAttributeDouble(
        txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, 0.0f);
    EXPECT_EQ(errorCodeFour, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_ErrorCode errorCodeFive = OH_Drawing_SetTypographyStyleAttributeInt(
        nullptr, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, 0);
    EXPECT_EQ(errorCodeFive, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCodeSix = OH_Drawing_SetTypographyStyleAttributeInt(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 0);
    EXPECT_EQ(errorCodeSix, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_ErrorCode errorCodeSeven = OH_Drawing_SetTypographyStyleAttributeDouble(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, 0);
    EXPECT_EQ(errorCodeSeven, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_ErrorCode errorCodeEight = OH_Drawing_SetTextStyleAttributeDouble(
        nullptr, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 0.0f);
    EXPECT_EQ(errorCodeEight, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCodeNine = OH_Drawing_SetTypographyStyleAttributeDouble(nullptr,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM,
        std::numeric_limits<double>::max());
    EXPECT_EQ(errorCodeNine, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
/*
 * @tc.name: AttributeGetterInvalidParam
 * @tc.desc: Test for attribute getter invalid param
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, AttributeGetterInvalidParam, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    int lineHeightStyleInt = 0;
    OH_Drawing_ErrorCode errorCodeOne = OH_Drawing_GetTextStyleAttributeInt(
        nullptr, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, &lineHeightStyleInt);
    EXPECT_EQ(errorCodeOne, OH_DRAWING_ERROR_INVALID_PARAMETER);
    int lineHeightMaxInt = 0;
    OH_Drawing_ErrorCode errorCodeTwo = OH_Drawing_GetTextStyleAttributeInt(
        txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, &lineHeightMaxInt);
    EXPECT_EQ(errorCodeTwo, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    double lineHeightStyleDouble = 0.0f;
    OH_Drawing_ErrorCode errorCodeThree = OH_Drawing_GetTextStyleAttributeDouble(
        txtStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, &lineHeightStyleDouble);
    EXPECT_EQ(errorCodeThree, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_ErrorCode errorCodeFour = OH_Drawing_GetTypographyStyleAttributeInt(
        nullptr, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, &lineHeightStyleInt);
    EXPECT_EQ(errorCodeFour, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCodeFive = OH_Drawing_GetTypographyStyleAttributeInt(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, &lineHeightMaxInt);
    EXPECT_EQ(errorCodeFive, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_ErrorCode errorCodeSix = OH_Drawing_GetTypographyStyleAttributeDouble(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, &lineHeightStyleDouble);
    EXPECT_EQ(errorCodeSix, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_ErrorCode errorCodeSeven = OH_Drawing_GetTypographyStyleAttributeDouble(nullptr,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, &lineHeightStyleDouble);
    EXPECT_EQ(errorCodeSeven, OH_DRAWING_ERROR_INVALID_PARAMETER);
    double limitLineHeight = 0.0f;
    OH_Drawing_ErrorCode maxHeightCode = OH_Drawing_GetTypographyStyleAttributeDouble(
        nullptr, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, &limitLineHeight);
    EXPECT_EQ(maxHeightCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCodeEight = OH_Drawing_GetTextStyleAttributeDouble(
        nullptr, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_I_LINE_HEIGHT_STYLE, &lineHeightStyleDouble);
    EXPECT_EQ(errorCodeEight, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
/*
 * @tc.name: LineMetricsGetAllVsAtIndexMatch
 * @tc.desc: test for getting size of line metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, LineMetricsGetAllVsAtIndexMatch, TestSize.Level0)
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
    const char* text = "这是一个排版信息获取接口的测试文本";
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
 * @tc.name: LineMetricsAtReturnsExpectedValues
 * @tc.desc: test returning line metrics info for the line text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, LineMetricsAtReturnsExpectedValues, TestSize.Level0)
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
    const char* text = "这是一个排版信息获取接口的测试文本";
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
 * @tc.name: GetBasicTextStyleAttributes
 * @tc.desc: test for getting basic attributes from textstyle
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, GetBasicTextStyleAttributes, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_WAVY);
    EXPECT_EQ(OH_Drawing_TextStyleGetDecorationStyle(txtStyle), 4);
    EXPECT_EQ(OH_Drawing_TextStyleGetDecorationStyle(nullptr), TEXT_DECORATION_STYLE_SOLID);

    OH_Drawing_SetTextStyleHalfLeading(txtStyle, false);
    EXPECT_FALSE(OH_Drawing_TextStyleGetHalfLeading(txtStyle));
    EXPECT_EQ(OH_Drawing_TextStyleGetHalfLeading(nullptr), TEXT_DECORATION_STYLE_SOLID);
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, -2);
    EXPECT_TRUE(OH_Drawing_TextStyleGetHalfLeading(txtStyle));
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, 20);
    EXPECT_TRUE(OH_Drawing_TextStyleGetHalfLeading(txtStyle));

    OH_Drawing_SetTextStyleFontSize(txtStyle, -20); // This value does not actually take effect.
    EXPECT_EQ(OH_Drawing_TextStyleGetFontSize(txtStyle), -20);
    OH_Drawing_SetTextStyleFontSize(txtStyle, 10.5632);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontSize(txtStyle), 10.5632);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontSize(nullptr), 0.0);

    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_200);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyle), 1);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(nullptr), FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, -100);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyle), FONT_WEIGHT_400);

    // The current value does not take effect and the returned enumeration value is 1
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_OBLIQUE);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(txtStyle), 1);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_ITALIC);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(txtStyle), 1);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(nullptr), FONT_STYLE_NORMAL);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, -1);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(txtStyle), FONT_STYLE_NORMAL);

    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_IDEOGRAPHIC);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaseline(txtStyle), 1);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaseline(nullptr), TEXT_BASELINE_ALPHABETIC);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, -5);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaseline(txtStyle), TEXT_BASELINE_ALPHABETIC);

    const char* fontFamilies[] = { "Text", "Text2" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 2, fontFamilies);
    size_t fontFamiliesNumber;
    char** fontFamiliesList = OH_Drawing_TextStyleGetFontFamilies(txtStyle, &fontFamiliesNumber);
    EXPECT_EQ(fontFamiliesNumber, 2);
    EXPECT_NE(fontFamiliesList, nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFamilies(nullptr, &fontFamiliesNumber), nullptr);
    OH_Drawing_TextStyleDestroyFontFamilies(fontFamiliesList, fontFamiliesNumber);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
/*
 * @tc.name: GetOtherTextStyleAttributes
 * @tc.desc: test for getting other attributes value from textstyle
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, GetOtherTextStyleAttributes, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, -20);
    EXPECT_EQ(OH_Drawing_TextStyleGetLetterSpacing(txtStyle), -20);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, 0.56);
    EXPECT_EQ(OH_Drawing_TextStyleGetLetterSpacing(txtStyle), 0.56);
    EXPECT_EQ(OH_Drawing_TextStyleGetLetterSpacing(nullptr), 0.0);

    OH_Drawing_SetTextStyleFontHeight(txtStyle, -0.50);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontHeight(txtStyle), -0.50);
    OH_Drawing_SetTextStyleFontHeight(txtStyle, 0.5632);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontHeight(txtStyle), 0.5632);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontHeight(nullptr), 0.0);

    OH_Drawing_SetTextStyleWordSpacing(txtStyle, -30);
    EXPECT_EQ(OH_Drawing_TextStyleGetWordSpacing(txtStyle), -30);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, -2.50);
    EXPECT_EQ(OH_Drawing_TextStyleGetWordSpacing(txtStyle), -2.50);
    EXPECT_EQ(OH_Drawing_TextStyleGetWordSpacing(nullptr), 0.0);

    OH_Drawing_SetTextStyleLocale(txtStyle, "zh-Hans");
    EXPECT_EQ(std::strcmp(OH_Drawing_TextStyleGetLocale(txtStyle), "zh-Hans"), 0);
    EXPECT_EQ(OH_Drawing_TextStyleGetLocale(nullptr), nullptr);

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(255, 50, 0, 255));
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyle, brush);
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_TextStyleGetBackgroundBrush(txtStyle, brush1);
    EXPECT_EQ(OH_Drawing_BrushGetAlpha(brush), OH_Drawing_BrushGetAlpha(brush1));
    EXPECT_NE(brush, brush1);

    EXPECT_FALSE(OH_Drawing_TextStyleIsPlaceholder(txtStyle));
    EXPECT_FALSE(OH_Drawing_TextStyleIsPlaceholder(nullptr));
    OH_Drawing_TextStyleSetPlaceholder(txtStyle);
    EXPECT_TRUE(OH_Drawing_TextStyleIsPlaceholder(txtStyle));
    OH_Drawing_TextStyleSetPlaceholder(txtStyle);
    EXPECT_TRUE(OH_Drawing_TextStyleIsPlaceholder(txtStyle));

    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_BrushDestroy(brush1);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
/*
 * @tc.name: SetAndGetTextStyleFontStyleStruct
 * @tc.desc: test for OH_Drawing_SetTextStyleFontStyleStruct
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetAndGetTextStyleFontStyleStruct, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();

    // Test the default value.
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyle), 3);
    OH_Drawing_FontStyleStruct fontStyle;
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyle), 3);
    EXPECT_EQ(fontStyle.slant, 0);
    EXPECT_EQ(fontStyle.width, 0);
    OH_Drawing_SetTextStyleFontStyleStruct(txtStyle, fontStyle);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyle), 0);
    EXPECT_EQ(fontStyle.slant, 0);
    EXPECT_EQ(fontStyle.width, 0);
    OH_Drawing_FontStyleStruct fontStyle1 = OH_Drawing_TextStyleGetFontStyleStruct(txtStyle);
    EXPECT_EQ(fontStyle1.slant, FONT_STYLE_NORMAL);
    EXPECT_EQ(fontStyle1.slant, 0);
    EXPECT_EQ(fontStyle1.width, FONT_WIDTH_NORMAL);
    EXPECT_EQ(fontStyle1.width, 5);
    EXPECT_EQ(fontStyle1.weight, FONT_WEIGHT_100);

    // Test normal assignment
    fontStyle.slant = FONT_STYLE_OBLIQUE;
    fontStyle.weight = FONT_WEIGHT_600;
    fontStyle.width = FONT_WIDTH_EXTRA_EXPANDED;
    OH_Drawing_SetTextStyleFontStyleStruct(txtStyle, fontStyle);
    fontStyle1 = OH_Drawing_TextStyleGetFontStyleStruct(txtStyle);

    // Specification: The enumerated value of FONT_STYLE_OBLIQUE passed out is uniformly 1.
    EXPECT_EQ(fontStyle1.slant, 1);
    EXPECT_NE(fontStyle1.slant, fontStyle.slant);
    EXPECT_EQ(fontStyle1.width, fontStyle.width);
    EXPECT_EQ(fontStyle1.width, FONT_WIDTH_EXTRA_EXPANDED);
    EXPECT_EQ(fontStyle1.weight, fontStyle.weight);
    EXPECT_EQ(fontStyle1.weight, FONT_WEIGHT_600);

    // Test the abnormal values.
    fontStyle.slant = static_cast<OH_Drawing_FontStyle>(-1);
    fontStyle.weight = static_cast<OH_Drawing_FontWeight>(-1);
    fontStyle.width = static_cast<OH_Drawing_FontWidth>(-1);
    OH_Drawing_SetTextStyleFontStyleStruct(txtStyle, fontStyle);
    fontStyle1 = OH_Drawing_TextStyleGetFontStyleStruct(txtStyle);
    EXPECT_EQ(fontStyle1.slant, FONT_STYLE_NORMAL);
    EXPECT_EQ(fontStyle1.width, FONT_WIDTH_NORMAL);
    EXPECT_EQ(fontStyle1.weight, FONT_WEIGHT_400);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyleStruct(nullptr).slant, FONT_STYLE_NORMAL);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyleStruct(nullptr).width, FONT_WIDTH_NORMAL);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyleStruct(nullptr).weight, FONT_WEIGHT_400);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
/*
 * @tc.name: TextStyleFontFeature
 * @tc.desc: test for TextStyle fontFeature
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleFontFeature, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "frac", 1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "numr", 1);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFeatureSize(txtStyle), 2);
    OH_Drawing_TextStyleClearFontFeature(txtStyle);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFeatureSize(txtStyle), 0);

    OH_Drawing_TextStyleAddFontFeature(txtStyle, "frac", 1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "frac", 0);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "numr", 1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "liga", 1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "liga", 1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "", 1);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, "testtesttesttesttest", 10);
    size_t mySize = OH_Drawing_TextStyleGetFontFeatureSize(txtStyle);
    EXPECT_EQ(mySize, 7);

    OH_Drawing_FontFeature* myFontFeatures = OH_Drawing_TextStyleGetFontFeatures(txtStyle);
    EXPECT_STREQ(myFontFeatures[0].tag, "frac");
    EXPECT_EQ(myFontFeatures[0].value, 1);
    EXPECT_STREQ(myFontFeatures[5].tag, "");
    EXPECT_EQ(myFontFeatures[5].value, 1);
    EXPECT_STREQ(myFontFeatures[3].tag, myFontFeatures[4].tag);
    EXPECT_EQ(myFontFeatures[3].value, myFontFeatures[4].value);
    EXPECT_STREQ(myFontFeatures[mySize - 1].tag, "testtesttesttesttest");
    EXPECT_EQ(myFontFeatures[mySize - 1].value, 10);
    OH_Drawing_TextStyleDestroyFontFeatures(myFontFeatures, mySize);
    OH_Drawing_DestroyTextStyle(txtStyle);
}
/*
 * @tc.name: TextStyleAttributeNotMatched
 * @tc.desc: test for OH_Drawing_TextStyleIsAttributeMatched false
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleAttributeNotMatched, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_WAVY);
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, false);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_200);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_OBLIQUE);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_IDEOGRAPHIC);
    const char* fontFamilies[] = { "Text", "Text2" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 2, fontFamilies);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, -20);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, -30);
    OH_Drawing_SetTextStyleLocale(txtStyle, "zh-Hans");

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(255, 50, 0, 255));
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyle, brush);

    OH_Drawing_TextShadow* shadow = OH_Drawing_CreateTextShadow();
    OH_Drawing_Point* point = OH_Drawing_PointCreate(10, -5);
    OH_Drawing_SetTextShadow(shadow, OH_Drawing_ColorSetArgb(255, 255, 0, 255), point, 10);
    OH_Drawing_TextStyleAddShadow(txtStyle, shadow);

    OH_Drawing_TextStyle* textStyleGet = OH_Drawing_CreateTextStyle();
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_NONE));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_BACKGROUND));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_ALL_ATTRIBUTES));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_FONT));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_SHADOW));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_DECORATIONS));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_WORD_SPACING));
    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, textStyleGet, TEXT_STYLE_LETTER_SPACING));

    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(textStyleGet);
    OH_Drawing_PointDestroy(point);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_DestroyTextShadow(shadow);
}
/*
 * @tc.name: TextStyleAttributeMatched
 * @tc.desc: test for OH_Drawing_TextStyleIsAttributeMatched true
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleAttributeMatched, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyle* txtStyleCompare = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, TEXT_DECORATION_STYLE_WAVY);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyleCompare, TEXT_DECORATION_STYLE_WAVY);
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, false);
    OH_Drawing_SetTextStyleHalfLeading(txtStyleCompare, false);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_200);
    OH_Drawing_SetTextStyleFontWeight(txtStyleCompare, FONT_WEIGHT_200);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_OBLIQUE);
    OH_Drawing_SetTextStyleFontStyle(txtStyleCompare, FONT_STYLE_OBLIQUE);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_IDEOGRAPHIC);
    OH_Drawing_SetTextStyleBaseLine(txtStyleCompare, TEXT_BASELINE_IDEOGRAPHIC);
    const char* fontFamilies[] = { "Text", "Text2" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 2, fontFamilies);
    OH_Drawing_SetTextStyleFontFamilies(txtStyleCompare, 2, fontFamilies);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, -20);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyleCompare, -20);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, -30);
    OH_Drawing_SetTextStyleWordSpacing(txtStyleCompare, -30);
    OH_Drawing_SetTextStyleLocale(txtStyle, "zh-Hans");
    OH_Drawing_SetTextStyleLocale(txtStyleCompare, "zh-Hans");

    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(255, 50, 0, 255));
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyle, brush);
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyleCompare, brush);

    OH_Drawing_TextShadow* shadow = OH_Drawing_CreateTextShadow();
    OH_Drawing_Point* point = OH_Drawing_PointCreate(10, -5);
    OH_Drawing_SetTextShadow(shadow, OH_Drawing_ColorSetArgb(255, 255, 0, 255), point, 10);
    OH_Drawing_TextStyleAddShadow(txtStyle, shadow);
    OH_Drawing_TextStyleAddShadow(txtStyleCompare, shadow);

    EXPECT_FALSE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_NONE));
    EXPECT_TRUE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_BACKGROUND));
    EXPECT_TRUE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_ALL_ATTRIBUTES));
    EXPECT_TRUE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_FONT));
    EXPECT_TRUE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_SHADOW));
    EXPECT_TRUE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_DECORATIONS));
    EXPECT_TRUE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_WORD_SPACING));
    EXPECT_TRUE(OH_Drawing_TextStyleIsAttributeMatched(txtStyle, txtStyleCompare, TEXT_STYLE_LETTER_SPACING));

    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(txtStyleCompare);
    OH_Drawing_PointDestroy(point);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_DestroyTextShadow(shadow);
}

/*
 * @tc.name: TypographyStyleAttributeDoubleArrayTest001
 * @tc.desc: test for OH_Drawing_SetTypographyStyleAttributeDoubleArray invalid param
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeDoubleArrayTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    double values[] = { 10.0 };
    // nullptr style
    OH_Drawing_ErrorCode errorCodeOne = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        nullptr, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, values, 1);
    EXPECT_EQ(errorCodeOne, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // nullptr arrayValue
    OH_Drawing_ErrorCode errorCodeTwo = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, nullptr, 1);
    EXPECT_EQ(errorCodeTwo, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // zero arrayLength
    OH_Drawing_ErrorCode errorCodeThree = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, values, 0);
    EXPECT_EQ(errorCodeThree, OH_DRAWING_ERROR_INVALID_PARAMETER);
    double nagetiveValues[] = { -10.0 };
    // negative arrayValue
    errorCodeThree = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, nagetiveValues, 1);
    EXPECT_EQ(errorCodeThree, OH_DRAWING_ERROR_INVALID_PARAMETER);
    errorCodeThree = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, nagetiveValues, 1);
    EXPECT_EQ(errorCodeThree, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // mismatched attribute id (using double id for array setter)
    OH_Drawing_ErrorCode errorCodeFour = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, values, 1);
    EXPECT_EQ(errorCodeFour, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleAttributeDoubleArrayTest002
 * @tc.desc: test for OH_Drawing_GetTypographyStyleAttributeDoubleArray invalid param
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeDoubleArrayTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    double* outArray = nullptr;
    size_t outLen = 0;
    // nullptr style
    OH_Drawing_ErrorCode errorCodeOne = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        nullptr, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, &outArray, &outLen);
    EXPECT_EQ(errorCodeOne, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // nullptr arrayValue output
    OH_Drawing_ErrorCode errorCodeTwo = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, nullptr, &outLen);
    EXPECT_EQ(errorCodeTwo, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // nullptr arrayLength output
    OH_Drawing_ErrorCode errorCodeThree = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, &outArray, nullptr);
    EXPECT_EQ(errorCodeThree, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // mismatched attribute id (using double id for array getter)
    OH_Drawing_ErrorCode errorCodeFour = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, &outArray, &outLen);
    EXPECT_EQ(errorCodeFour, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleAttributeDoubleArrayTest003
 * @tc.desc: test for setting and getting line head indent array
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeDoubleArrayTest003, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    double headIndents[] = { 10.0, 20.0, 30.0 };
    OH_Drawing_ErrorCode setCode = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, headIndents, 3);
    EXPECT_EQ(setCode, OH_DRAWING_SUCCESS);
    double* outArray = nullptr;
    size_t outLen = 0;
    OH_Drawing_ErrorCode getCode = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, &outArray, &outLen);
    EXPECT_EQ(getCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outLen, 3);
    EXPECT_NEAR(outArray[0], 10.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(outArray[1], 20.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(outArray[2], 30.0, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleAttributeDoubleArrayTest004
 * @tc.desc: test for setting and getting line tail indent array
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeDoubleArrayTest004, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    double tailIndents[] = { 5.0, 15.0, 25.0, 35.0 };
    OH_Drawing_ErrorCode setCode = OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, tailIndents, 4);
    EXPECT_EQ(setCode, OH_DRAWING_SUCCESS);
    double* outArray = nullptr;
    size_t outLen = 0;
    OH_Drawing_ErrorCode getCode = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, &outArray, &outLen);
    EXPECT_EQ(getCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outLen, 4);
    EXPECT_NEAR(outArray[0], 5.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(outArray[1], 15.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(outArray[2], 25.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(outArray[3], 35.0, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleAttributeDoubleArrayTest005
 * @tc.desc: test for setting and getting empty tail indent array
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeDoubleArrayTest005, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    // Default state: tail indents should be empty
    double* outArray = nullptr;
    size_t outLen = 0;
    OH_Drawing_ErrorCode getCode = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_TAIL_INDENT, &outArray, &outLen);
    EXPECT_EQ(getCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outLen, 0);
    EXPECT_EQ(outArray, nullptr);
    getCode = OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, &outArray, &outLen);
    EXPECT_EQ(outLen, 0);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleAttributeDoubleArrayTest006
 * @tc.desc: test for replacing indent array with new values
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeDoubleArrayTest006, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    // First set
    double indents1[] = { 10.0, 20.0 };
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, indents1, 2);
    // Replace with different values
    double indents2[] = { 100.0 };
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, indents2, 1);
    double* outArray = nullptr;
    size_t outLen = 0;
    OH_Drawing_GetTypographyStyleAttributeDoubleArray(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_DA_LINE_HEAD_INDENT, &outArray, &outLen);
    EXPECT_EQ(outLen, 1);
    EXPECT_NEAR(outArray[0], 100.0, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleFirstLineHeadIndentTest001
 * @tc.desc: test for setting and getting first line head indent via double attribute
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleFirstLineHeadIndentTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode setCode = OH_Drawing_SetTypographyStyleAttributeDouble(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, 40.0);
    EXPECT_EQ(setCode, OH_DRAWING_SUCCESS);
    double value = 0.0;
    OH_Drawing_ErrorCode getCode = OH_Drawing_GetTypographyStyleAttributeDouble(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, &value);
    EXPECT_EQ(getCode, OH_DRAWING_SUCCESS);
    EXPECT_NEAR(value, 40.0, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleFirstLineHeadIndentTest002
 * @tc.desc: test for first line head indent with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleFirstLineHeadIndentTest002, TestSize.Level0)
{
    OH_Drawing_ErrorCode setCode = OH_Drawing_SetTypographyStyleAttributeDouble(
        nullptr, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, 40.0);
    EXPECT_EQ(setCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    double value = 0.0;
    OH_Drawing_ErrorCode getCode = OH_Drawing_GetTypographyStyleAttributeDouble(
        nullptr, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, &value);
    EXPECT_EQ(getCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: TypographyStyleFirstLineHeadIndentTest003
 * @tc.desc: test for first line head indent with negative value
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleFirstLineHeadIndentTest003, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode setCode = OH_Drawing_SetTypographyStyleAttributeDouble(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, -20.0);
    EXPECT_EQ(setCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    double value = 0.0;
    OH_Drawing_ErrorCode getCode = OH_Drawing_GetTypographyStyleAttributeDouble(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_D_FIRST_LINE_HEAD_INDENT, &value);
    EXPECT_EQ(getCode, OH_DRAWING_SUCCESS);
    EXPECT_NEAR(value, -1.0, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
} // namespace OHOS
