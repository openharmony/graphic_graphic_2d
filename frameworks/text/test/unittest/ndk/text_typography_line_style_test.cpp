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
 * @tc.name: TypographyTest034
 * @tc.desc: test for font style of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest034, TestSize.Level0)
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
 * @tc.name: TypographyTest035
 * @tc.desc: test for font weight of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest035, TestSize.Level0)
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
 * @tc.name: TypographyTest036
 * @tc.desc: test for font size of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest036, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);

    OH_Drawing_SetTypographyTextLineStyleFontSize(fTypoStyle, 80);
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetFontSize(fTypoStyle), 80);
    OH_Drawing_SetTypographyTextLineStyleFontSize(fTypoStyle, 100);
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetFontSize(fTypoStyle), 100);
    OH_Drawing_SetTypographyTextLineStyleFontSize(nullptr, 0);

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 117);
}

/*
 * @tc.name: TypographyTest037
 * @tc.desc: test for font families of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest037, TestSize.Level0)
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
 * @tc.name: TypographyTest038
 * @tc.desc: test for spacing scale of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest038, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);

    OH_Drawing_SetTypographyTextLineStyleSpacingScale(fTypoStyle, 1.0);
    EXPECT_EQ((ConvertToOriginalText(fTypoStyle)->lineStyleSpacingScale), 1.0);
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(fTypoStyle, 30.0);
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetSpacingScale(fTypoStyle), 30.0);
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(nullptr, 0);

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 436);
}

/*
 * @tc.name: TypographyTest039
 * @tc.desc: test for font height of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest039, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);

    OH_Drawing_SetTypographyTextLineStyleFontHeight(nullptr, 0);
    OH_Drawing_SetTypographyTextLineStyleFontHeight(fTypoStyle, 10.5);
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetHeightScale(fTypoStyle), 10.5);
    EXPECT_TRUE(OH_Drawing_TypographyTextlineStyleGetHeightOnly(fTypoStyle));

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 147);
}

/*
 * @tc.name: TypographyTest041
 * @tc.desc: test for font weight of line style for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest041, TestSize.Level0)
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
 * @tc.name: TypographyTest049
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest049, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextFontHeight(fTypoStyle, 2);
    bool halfLeading = true;
    OH_Drawing_SetTypographyTextHalfLeading(fTypoStyle, halfLeading);
    EXPECT_TRUE(ConvertToOriginalText(fTypoStyle)->halfLeading);

    OH_Drawing_SetTypographyTextLineStyleHalfLeading(fTypoStyle, halfLeading);
    EXPECT_TRUE(ConvertToOriginalText(fTypoStyle)->lineStyleHalfLeading);

    OH_Drawing_SetTypographyTextLineStyleFontSize(fTypoStyle, 80);
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetFontSize(fTypoStyle), 80);

    bool uselineStyle = true;
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, uselineStyle);
    EXPECT_TRUE(ConvertToOriginalText(fTypoStyle)->useLineStyle);

    bool linestyleOnly = true;
    OH_Drawing_SetTypographyTextLineStyleOnly(fTypoStyle, linestyleOnly);
    EXPECT_TRUE(ConvertToOriginalText(fTypoStyle)->lineStyleOnly);
    OH_Drawing_SetTypographyTextLineStyleOnly(nullptr, 0);

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();

    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 94);
}

/*
 * @tc.name: TypographyLineStyleTest001
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyLineStyleTest001, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);
    EXPECT_TRUE(OH_Drawing_TypographyTextGetLineStyle(fTypoStyle));
    EXPECT_FALSE(OH_Drawing_TypographyTextGetLineStyle(nullptr));

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 59);
}

/*
 * @tc.name: TypographyLineStyleTest002
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyLineStyleTest002, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);
    EXPECT_TRUE(OH_Drawing_TypographyTextGetLineStyle(fTypoStyle));
    EXPECT_FALSE(OH_Drawing_TypographyTextGetLineStyle(nullptr));
    OH_Drawing_SetTypographyTextLineStyleFontSize(fTypoStyle, DEFAULT_FONT_SIZE + 1);

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 60);
}

/*
 * @tc.name: TypographyLineStyleTest003
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyLineStyleTest003, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);
    EXPECT_TRUE(OH_Drawing_TypographyTextGetLineStyle(fTypoStyle));
    EXPECT_FALSE(OH_Drawing_TypographyTextGetLineStyle(nullptr));
    OH_Drawing_SetTypographyTextLineStyleFontSize(fTypoStyle, DEFAULT_FONT_SIZE - 1);
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 59);
}

/*
 * @tc.name: TypographyTest058
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest058, TestSize.Level0)
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
 * @tc.name: TypographyTest059
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest059, TestSize.Level0)
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
 * @tc.name: TypographyTest060
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest060, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);
    size_t fontNum = 1; // 1 means font number for test
    const char* fontFamilies[] = { "Roboto" };
    int fontFamiliesNumber = 1; // 1 means font families number for test
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(fTypoStyle, fontFamiliesNumber, fontFamilies);
    char** result = OH_Drawing_TypographyTextlineStyleGetFontFamilies(fTypoStyle, &fontNum);
    EXPECT_NE(result, nullptr);
    result = OH_Drawing_TypographyTextlineStyleGetFontFamilies(nullptr, &fontNum);
    EXPECT_EQ(result, nullptr);
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(result, fontNum);

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 59);
}

/*
 * @tc.name: TypographyTest061
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest061, TestSize.Level0)
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
 * @tc.name: TypographyTest062
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest062, TestSize.Level0)
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
 * @tc.name: TypographyTest063
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest063, TestSize.Level0)
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
 * @tc.name: TypographyTest064
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest064, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextUseLineStyle(fTypoStyle, true);

    OH_Drawing_SetTypographyTextLineStyleHalfLeading(fTypoStyle, true);
    EXPECT_TRUE(OH_Drawing_TypographyTextlineStyleGetHalfLeading(fTypoStyle));
    EXPECT_FALSE(OH_Drawing_TypographyTextlineStyleGetHalfLeading(nullptr));

    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    CreateTypography();
    Layout();
    EXPECT_EQ(OH_Drawing_TypographyGetLineHeight(fTypography, 0), 59);
}

/*
 * @tc.name: TypographyTest065
 * @tc.desc: test for halfleading, uselinestyle linestyleonly of text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest065, TestSize.Level0)
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
 * @tc.name: TypographyTest097
 * @tc.desc: test for setting text line style font weight for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest097, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    // -1 for unit test
    int weight = -1;
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, weight);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(nullptr, weight);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->lineStyleFontWeight, FontWeight::W400);
}

/*
 * @tc.name: TypographyTest098
 * @tc.desc: test for text line style getting font families for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest098, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    char** result = OH_Drawing_TypographyTextlineStyleGetFontFamilies(typoStyle, nullptr);
    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: TypographyTest099
 * @tc.desc: test for text line style setting half leading for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest099, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    bool lineStyleHalfLeading = false;
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(typoStyle, lineStyleHalfLeading);
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(nullptr, lineStyleHalfLeading);
    bool result = OH_Drawing_TypographyTextlineStyleGetHalfLeading(typoStyle);
    EXPECT_FALSE(result);
}

} // namespace OHOS
