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
 * @tc.name: FontFamiliesTest001
 * @tc.desc: test for the OH_Drawing_TypographyTextlineStyleDestroyFontFamilies.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, FontFamiliesTest001, TestSize.Level0)
{
    char** fontFamilies = new char*[3]; // 3 means the number of font
    std::string tempStr1 = "Test1";
    std::string tempStr3 = "Test3";
    fontFamilies[0] = new char[tempStr1.size() + 1];
    std::copy(tempStr1.begin(), tempStr1.end(), fontFamilies[0]);
    fontFamilies[0][tempStr1.size()] = '\0';
    fontFamilies[1] = nullptr;
    fontFamilies[2] = new char[tempStr3.size() + 1];              // 2 means the index of font
    std::copy(tempStr3.begin(), tempStr3.end(), fontFamilies[2]); // 2 means the index of font
    fontFamilies[2][tempStr3.size()] = '\0';                      // 2 means the index of font
    size_t num = 3;                                               // 3 means the number of font
    ASSERT_NE(fontFamilies, nullptr);
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(fontFamilies, num);

    fontFamilies = nullptr;
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(fontFamilies, num);
}

/*
 * @tc.name: TextStyleAddFontVariationTest001
 * @tc.desc: test for the OH_Drawing_TextStyleAddFontVariation.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TextStyleAddFontVariationTest001, TestSize.Level0)
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
 * @tc.name: TextStyleAddFontVariationTest002
 * @tc.desc: test for the fontvariation wdth axis.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleAddFontVariationTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    const char* fontFamilies[] = { "Noto Sans" };
    OH_Drawing_SetTextStyleFontFamilies(textStyle, 1, fontFamilies);
    // 50.0 is half of the maximum font width supported by the font
    OH_Drawing_TextStyleAddFontVariation(textStyle, "wdth", 50.0);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection, nullptr);
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    const char* text = "HelloWorld";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    constexpr double maxWidth = 1000.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);

    constexpr double expectedResult = 55.1037902832;
    EXPECT_NEAR(OH_Drawing_TypographyGetLongestLine(typography), expectedResult, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: TextStyleAddFontVariationNormalizedTest001
 * @tc.desc: test for the fontvariation axis with normalized.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleAddFontVariationNormalizedTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    const char* fontFamilies[] = { "Noto Sans" };
    OH_Drawing_SetTextStyleFontFamilies(textStyle, 1, fontFamilies);
    OH_Drawing_TextStyleAddFontVariationWithNormalization(textStyle, nullptr, 0);
    OH_Drawing_TextStyleAddFontVariationWithNormalization(nullptr, "wdth", 0);
    OH_Drawing_TextStyleAddFontVariationWithNormalization(textStyle, "wdth", -1);
    OH_Drawing_TextStyleAddFontVariationWithNormalization(textStyle, "wght", 1);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection, nullptr);
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    const char* text = "HelloWorld";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    constexpr double maxWidth = 1000.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);

    constexpr double expectedResult = 64.41375732421875;
    EXPECT_NEAR(OH_Drawing_TypographyGetLongestLine(typography), expectedResult, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

} // namespace OHOS
