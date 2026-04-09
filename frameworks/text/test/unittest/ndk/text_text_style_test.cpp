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
 * @tc.name: TypographyTest005
 * @tc.desc: test for creating text style
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest005, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle, nullptr);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextStyle(nullptr);
}

/*
 * @tc.name: TypographyTest006
 * @tc.desc: test for text color
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest006, TestSize.Level0)
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
 * @tc.name: TypographyTest007
 * @tc.desc: test for font size
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest007, TestSize.Level0)
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
 * @tc.name: TypographyTest008
 * @tc.desc: test for font weight
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest008, TestSize.Level0)
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
 * @tc.name: TypographyTest009
 * @tc.desc: test for baseline location
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest009, TestSize.Level0)
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
 * @tc.name: TypographyTest010
 * @tc.desc: test for text decoration
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest010, TestSize.Level0)
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
 * @tc.name: TypographyTest011
 * @tc.desc: test for text decoration color
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest011, TestSize.Level0)
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
 * @tc.name: TypographyTest012
 * @tc.desc: test for font height
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest012, TestSize.Level0)
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
 * @tc.name: TypographyTest013
 * @tc.desc: test for font families
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest013, TestSize.Level0)
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
 * @tc.name: TypographyTest014
 * @tc.desc: test for font italic
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest014, TestSize.Level0)
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
 * @tc.name: TypographyTest015
 * @tc.desc: test for font locale
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest015, TestSize.Level0)
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
 * @tc.name: TypographyTest020
 * @tc.desc: test for decoration style
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest020, TestSize.Level0)
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
 * @tc.name: TypographyTest021
 * @tc.desc: test for decoration thickness scale
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest021, TestSize.Level0)
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
 * @tc.name: TypographyTest022
 * @tc.desc: test for letter spacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest022, TestSize.Level0)
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
 * @tc.name: TypographyTest023
 * @tc.desc: test for word spacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest023, TestSize.Level0)
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
 * @tc.name: TypographyTest024
 * @tc.desc: test for ellipsis modal
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest024, TestSize.Level0)
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
 * @tc.name: TypographyTest025
 * @tc.desc: test for set ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest025, TestSize.Level0)
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
 * @tc.name: TypographyTest050
 * @tc.desc: test for getting numbers for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest050, TestSize.Level0)
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
 * @tc.name: TypographyTest051
 * @tc.desc: test for getting line info for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest051, TestSize.Level0)
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
 * @tc.name: TypographyTest075
 * @tc.desc: test for sets and gets isPlaceholder for TextStyle objects
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest075, TestSize.Level0)
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
 * @tc.name: TypographyTest076
 * @tc.desc: test for the two TextStyle objects have matching properties
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest076, TestSize.Level0)
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
 * @tc.name: TypographyTest080
 * @tc.desc: test for whether two TextStyle objects are equal
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest080, TestSize.Level0)
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
 * @tc.name: TypographyTest081
 * @tc.desc: test for getting and setting text style
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest081, TestSize.Level0)
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
 * @tc.name: TypographyTest083
 * @tc.desc: test for the font properties of two TextStyle objects are equal
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest083, TestSize.Level0)
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
 * @tc.name: AddTextStyleDecorationTest001
 * @tc.desc: test for add multiple text decoration
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, AddTextStyleDecorationTest001, TestSize.Level0)
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
 * @tc.name: RemoveTextStyleDecorationTest001
 * @tc.desc: test for remove specific text decoration
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, RemoveTextStyleDecorationTest001, TestSize.Level0)
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

    OH_Drawing_AddTextStyleDecoration(
        txtStyle, TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_OVERLINE | TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration,
        TextDecoration::UNDERLINE | TextDecoration::OVERLINE | TextDecoration::LINE_THROUGH);
    OH_Drawing_RemoveTextStyleDecoration(txtStyle, TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_LINE_THROUGH);
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->decoration, TextDecoration::OVERLINE);
    OH_Drawing_RemoveTextStyleDecoration(nullptr, TEXT_DECORATION_UNDERLINE | TEXT_DECORATION_LINE_THROUGH);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontWidthTest001
 * @tc.desc: test invalid data for font width.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleAttributeFontWidthTest001, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    auto res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_WIDTH, 0);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    constexpr int maxThanRange = 10;
    res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_WIDTH, maxThanRange);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    int outResult = 0;
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_WIDTH, &outResult);
    EXPECT_EQ(outResult, FONT_WIDTH_NORMAL);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontWidthTest002
 * @tc.desc: test valid data for font width.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleAttributeFontWidthTest002, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    auto res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_WIDTH, FONT_WIDTH_SEMI_EXPANDED);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    int outResult = 0;
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_WIDTH, &outResult);
    EXPECT_EQ(outResult, FONT_WIDTH_SEMI_EXPANDED);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

} // namespace OHOS
