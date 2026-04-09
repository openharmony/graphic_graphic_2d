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
 * @tc.name: TypographyTest001
 * @tc.desc: test for creating TypographyStyle
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typoStyle, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyStyle(nullptr);
}

/*
 * @tc.name: TypographyTest002
 * @tc.desc: test for text direction
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest002, TestSize.Level0)
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
 * @tc.name: TypographyTest003
 * @tc.desc: test for text alignment
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest003, TestSize.Level0)
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
 * @tc.name: TypographyTest004
 * @tc.desc: test for max lines
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest004, TestSize.Level0)
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
 * @tc.name: TypographyBreakStrategyTest001
 * @tc.desc: test for break strategy GREEDY
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyBreakStrategyTest001, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(nullptr, 0);
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->breakStrategy, BreakStrategy::GREEDY);

    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_GREEDY);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->breakStrategy, BreakStrategy::GREEDY);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 200);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 1180);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 200);
}

/*
 * @tc.name: TypographyBreakStrategyTest002
 * @tc.desc: test for break strategy HIGH_QUALITY
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyBreakStrategyTest002, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_HIGH_QUALITY);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->breakStrategy, BreakStrategy::HIGH_QUALITY);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 200);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 1180);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 200);
}

/*
 * @tc.name: TypographyBreakStrategyTest003
 * @tc.desc: test for break strategy BALANCED
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyBreakStrategyTest003, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->breakStrategy, BreakStrategy::BALANCED);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 200);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 1180);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 200);
}

/*
 * @tc.name: TypographyWordBreakTest001
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyWordBreakTest001, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextWordBreakType(nullptr, 0);
    OH_Drawing_SetTypographyTextWordBreakType(fTypoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->wordBreakType, WordBreakType::BREAK_WORD);

    OH_Drawing_SetTypographyTextWordBreakType(fTypoStyle, WORD_BREAK_TYPE_NORMAL);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->wordBreakType, WordBreakType::NORMAL);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 200);
    EXPECT_EQ(OH_Drawing_TypographyGetLineCount(fTypography), 20);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 1180);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 200);
}

/*
 * @tc.name: TypographyWordBreakTest002
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyWordBreakTest002, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextWordBreakType(fTypoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->wordBreakType, WordBreakType::BREAK_ALL);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 200);
    EXPECT_EQ(OH_Drawing_TypographyGetLineCount(fTypography), 18);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 1062);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 213);
}

/*
 * @tc.name: TypographyWordBreakTest003
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyWordBreakTest003, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextWordBreakType(fTypoStyle, WORD_BREAK_TYPE_BREAK_WORD);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->wordBreakType, WordBreakType::BREAK_WORD);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 200);
    EXPECT_EQ(OH_Drawing_TypographyGetLineCount(fTypography), 20);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 1180);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 200);
}

/*
 * @tc.name: TypographyWordBreakTest004
 * @tc.desc: test for word break type
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyWordBreakTest004, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextWordBreakType(fTypoStyle, WORD_BREAK_TYPE_BREAK_HYPHEN);
    EXPECT_EQ(ConvertToOriginalText(fTypoStyle)->wordBreakType, WordBreakType::BREAK_HYPHEN);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 200);
    EXPECT_EQ(OH_Drawing_TypographyGetLineCount(fTypography), 20);
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 1180);
    EXPECT_EQ(::round(OH_Drawing_TypographyGetLongestLineWithIndent(fTypography)), 200);
}

/*
 * @tc.name: TypographyTest019
 * @tc.desc: test for ellipsis modal
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest019, TestSize.Level0)
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
 * @tc.name: TypographyTest029
 * @tc.desc: test for font weight of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest029, TestSize.Level0)
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
 * @tc.name: TypographyTest030
 * @tc.desc: test for font style of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest030, TestSize.Level0)
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
 * @tc.name: TypographyTest031
 * @tc.desc: test for font family of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest031, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontFamily(typoStyle, "monospace");
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontFamily, "monospace");
    OH_Drawing_SetTypographyTextFontFamily(nullptr, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyStyle(nullptr);
}

/*
 * @tc.name: TypographyTest032
 * @tc.desc: test for font size of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest032, TestSize.Level0)
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
 * @tc.name: TypographyTest033
 * @tc.desc: test for font height of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest033, TestSize.Level0)
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
 * @tc.name: TypographyTest048
 * @tc.desc: test for font weight for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest048, TestSize.Level0)
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
 * @tc.name: TypographyTest052
 * @tc.desc: test for setting the mode of leading over and under text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest052, TestSize.Level0)
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
 * @tc.name: TypographyHeightBehaviorTest001
 * @tc.desc: test for getting the mode of leading over and under text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyHeightBehaviorTest001, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextFontHeight(fTypoStyle, 10.01);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(nullptr), TEXT_HEIGHT_ALL);

    OH_Drawing_SetTypographyTextFontHeight(fTypoStyle, 10);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle, TEXT_HEIGHT_ALL);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(fTypoStyle), TEXT_HEIGHT_ALL);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 38000);
}

/*
 * @tc.name: TypographyHeightBehaviorTest002
 * @tc.desc: test for getting the mode of leading over and under text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyHeightBehaviorTest002, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextFontHeight(fTypoStyle, 10);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle, TEXT_HEIGHT_DISABLE_FIRST_ASCENT);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(fTypoStyle), TEXT_HEIGHT_DISABLE_FIRST_ASCENT);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 37650);
}

/*
 * @tc.name: TypographyHeightBehaviorTest003
 * @tc.desc: test for getting the mode of leading over and under text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyHeightBehaviorTest003, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextFontHeight(fTypoStyle, 10);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle, TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(fTypoStyle), TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 37908);
}

/*
 * @tc.name: TypographyHeightBehaviorTest004
 * @tc.desc: test for getting the mode of leading over and under text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyHeightBehaviorTest004, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextFontHeight(fTypoStyle, 10);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle, TEXT_HEIGHT_DISABLE_ALL);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(fTypoStyle), TEXT_HEIGHT_DISABLE_ALL);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_LONG_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetHeight(fTypography), 37558);
}

/*
 * @tc.name: TypographyTest066
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest066, TestSize.Level0)
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
 * @tc.name: TypographyTest067
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest067, TestSize.Level0)
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
 * @tc.name: TypographyTest068
 * @tc.desc: test typography ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest068, TestSize.Level0)
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
 * @tc.name: TypographyTest069
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest069, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* from = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyStyle* to = OH_Drawing_CreateTypographyStyle();
    bool result = OH_Drawing_TypographyStyleEquals(from, to);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyStyleEquals(nullptr, to);
    EXPECT_FALSE(result);
    result = OH_Drawing_TypographyStyleEquals(from, nullptr);
    EXPECT_FALSE(result);

    OH_Drawing_SetTypographyTextFontWeight(from, FONT_WEIGHT_100);
    EXPECT_FALSE(OH_Drawing_TypographyStyleEquals(from, to));

    OH_Drawing_SetTypographyTextFontWeight(to, FONT_WEIGHT_100);
    EXPECT_TRUE(OH_Drawing_TypographyStyleEquals(from, to));

    OH_Drawing_TypographyStyleSetHintsEnabled(from, true);
    EXPECT_TRUE(OH_Drawing_TypographyStyleEquals(from, to));

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextStyle(from, textStyle);
    EXPECT_TRUE(OH_Drawing_TypographyStyleEquals(from, to));

    OH_Drawing_TypographyTextSetHeightBehavior(from, TEXT_HEIGHT_DISABLE_ALL);
    EXPECT_TRUE(OH_Drawing_TypographyStyleEquals(from, to));

    OH_Drawing_DestroyTypographyStyle(from);
    OH_Drawing_DestroyTypographyStyle(to);
}

/*
 * @tc.name: TypographyTest070
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest070, TestSize.Level0)
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
 * @tc.name: TypographyTest071
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest071, TestSize.Level0)
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
 * @tc.name: TypographyTest072
 * @tc.desc: test for create and releases the memory occupied by system font configuration information
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest072, TestSize.Level0)
{
    OH_Drawing_FontConfigInfoErrorCode code = ERROR_FONT_CONFIG_INFO_UNKNOWN;
    OH_Drawing_FontConfigInfo* configJsonInfo = OH_Drawing_GetSystemFontConfigInfo(&code);
    if (configJsonInfo != nullptr) {
        EXPECT_EQ(code, SUCCESS_FONT_CONFIG_INFO);
        uint32_t fontGenericInfoSize = configJsonInfo->fontGenericInfoSize;
        uint32_t fallbackInfoSize = configJsonInfo->fallbackGroupSet[0].fallbackInfoSize;
        EXPECT_EQ(fontGenericInfoSize, 6);
        EXPECT_EQ(fallbackInfoSize, 136);
    } else {
        EXPECT_NE(code, SUCCESS_FONT_CONFIG_INFO);
    }
    OH_Drawing_DestroySystemFontConfigInfo(configJsonInfo);
    OH_Drawing_DestroySystemFontConfigInfo(nullptr);
}

/*
 * @tc.name: TypographyTest082
 * @tc.desc: test for getting and setting typography style
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest082, TestSize.Level0)
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
 * @tc.name: TypographyTest094
 * @tc.desc: test for setting font weight for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest094, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextFontWeight(nullptr, FONT_WEIGHT_100);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, -1);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->fontWeight, FontWeight::W400);
    OH_Drawing_SetTypographyTextFontStyle(nullptr, FONT_STYLE_NORMAL);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyTest095
 * @tc.desc: test for setting font height for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest095, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    // -1.2 for unit test
    OH_Drawing_SetTypographyTextFontHeight(typoStyle, -1.2);
    OH_Drawing_SetTypographyTextFontHeight(nullptr, 0);
    EXPECT_TRUE(ConvertToOriginalText(typoStyle)->heightOnly);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->heightScale, 0);
}

/*
 * @tc.name: TypographyTest096
 * @tc.desc: test for setting half leading for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest096, TestSize.Level0)
{
    bool halfLeading = false;
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextHalfLeading(typoStyle, halfLeading);
    OH_Drawing_SetTypographyTextHalfLeading(nullptr, halfLeading);
    EXPECT_FALSE(ConvertToOriginalText(typoStyle)->halfLeading);
}

/*
 * @tc.name: TypographyTest100
 * @tc.desc: test for getting style struct for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest100, TestSize.Level0)
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
 * @tc.name: TypographyTest101
 * @tc.desc: test for getting font style struct for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest101, TestSize.Level0)
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
 * @tc.name: SetTypographyTextEllipsisTest001
 * @tc.desc: test for the OH_Drawing_SetTypographyTextLocale.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, SetTypographyTextEllipsisTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* style = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(style, nullptr);
    const char* locale = "zh-cn";
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

    const char* locale2 = "en-gb";
    OH_Drawing_SetTypographyTextLocale(nullptr, locale2);
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
 * @tc.name: TypographyStyleAttributeFontWidthTest001
 * @tc.desc: test invalid data for font width.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeFontWidthTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    auto res = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, 0);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    constexpr int maxThanRange = 10;
    res = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, maxThanRange);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    int outResult = 0;
    res = OH_Drawing_GetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, &outResult);
    EXPECT_EQ(outResult, FONT_WIDTH_NORMAL);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleAttributeFontWidthTest002
 * @tc.desc: test valid data for font width.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyStyleAttributeFontWidthTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    auto res = OH_Drawing_SetTypographyStyleAttributeInt(
        typoStyle, TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, FONT_WIDTH_SEMI_EXPANDED);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    int outResult = 0;
    res = OH_Drawing_GetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, &outResult);
    EXPECT_EQ(outResult, FONT_WIDTH_SEMI_EXPANDED);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
} // namespace OHOS
