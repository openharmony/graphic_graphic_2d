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
 * @tc.name: TypographyTest028
 * @tc.desc: test for getting line info for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest028, TestSize.Level0)
{
    OH_Drawing_TextShadow* textShadow = OH_Drawing_CreateTextShadow();
    EXPECT_NE(textShadow, nullptr);
    OH_Drawing_DestroyTextShadow(textShadow);
    OH_Drawing_DestroyTextShadow(nullptr);
}

/*
 * @tc.name: TypographyTest042
 * @tc.desc: test for text shadow for textstyle
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest042, TestSize.Level0)
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
 * @tc.name: TypographyTest107
 * @tc.desc: test for default textshadow.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest107, TestSize.Level0)
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
 * @tc.name: TextStyleAddShadowTest001
 * @tc.desc: test for multiple shadow parameters and abnormal shadow parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TextStyleAddShadowTest001, TestSize.Level0)
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
 * @tc.name: TextStyleGetShadowCountTest001
 * @tc.desc: test for the OH_Drawing_TextStyleGetShadowCount.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TextStyleGetShadowCountTest001, TestSize.Level0)
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
 * @tc.name: SetTextShadowTest001
 * @tc.desc: test for the OH_Drawing_SetTextShadow.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, SetTextShadowTest001, TestSize.Level0)
{
    OH_Drawing_TextStyle* style = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextShadow* originShadow = OH_Drawing_CreateTextShadow();
    OH_Drawing_TextStyleAddShadow(style, originShadow);
    OH_Drawing_TextShadow* shadow = OH_Drawing_TextStyleGetShadowWithIndex(style, 0);
    ASSERT_NE(shadow, nullptr);
    uint32_t color = 0;
    OH_Drawing_Point* offset = OH_Drawing_PointCreate(0, 0);
    ASSERT_NE(offset, nullptr);
    double blurRadius = 0.0;
    OH_Drawing_SetTextShadow(shadow, color, offset, blurRadius);
    OH_Drawing_SetTextShadow(shadow, color, nullptr, blurRadius);
    OH_Drawing_SetTextShadow(nullptr, color, offset, blurRadius);

    OH_Drawing_DestroyTextStyle(style);
    OH_Drawing_PointDestroy(offset);
    OH_Drawing_DestroyTextShadow(originShadow);
}

/*
 * @tc.name: TextStyleAddShadowTest002
 * @tc.desc: test for the OH_Drawing_TextStyleAddShadow.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TextStyleAddShadowTest002, TestSize.Level0)
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
 * @tc.name: TextStyleGetShadowWithIndexTest001
 * @tc.desc: test for the OH_Drawing_TextStyleGetShadowWithIndex.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TextStyleGetShadowWithIndexTest001, TestSize.Level0)
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
 * @tc.name: TextStyleGetShadowsTest001
 * @tc.desc: test for the OH_Drawing_DestroyTextShadows.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, TextStyleGetShadowsTest001, TestSize.Level0)
{
    OH_Drawing_TextStyle* style = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextShadow* shadow = OH_Drawing_TextStyleGetShadows(style);
    ASSERT_NE(shadow, nullptr);
    OH_Drawing_DestroyTextShadows(shadow);
    OH_Drawing_DestroyTextShadows(nullptr);
    OH_Drawing_DestroyTextStyle(style);
}

} // namespace OHOS
