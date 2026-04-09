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
 * @tc.name: TypographyTest040
 * @tc.desc: test for line metrics for text typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest040, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    // Test for fontSize 30
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, "OpenHarmony\n");
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // Test for layout width 800.0
    double maxWidth = 800.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
    // Test for postion 10.0 and 15.0
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    // Test for postion canvas width 20, height 40
    OH_Drawing_BitmapBuild(cBitmap, 20, 40, &cFormat);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_FontDescriptor* descriptor = OH_Drawing_CreateFontDescriptor();
    OH_Drawing_FontParser* parser = OH_Drawing_CreateFontParser();

    if (std::filesystem::exists(VIS_LIST_FILE_NAME)) {
        size_t fontNum;
        char** list = OH_Drawing_FontParserGetSystemFontList(parser, &fontNum);
        EXPECT_NE(list, nullptr);
        EXPECT_NE(OH_Drawing_FontParserGetFontByName(parser, list[0]), nullptr);
        OH_Drawing_DestroySystemFontList(list, fontNum);
    }
    OH_Drawing_DestroyFontParser(parser);
    OH_Drawing_DestroyFontDescriptor(descriptor);
    OH_Drawing_LineMetrics* vectorMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    EXPECT_NE(vectorMetrics, nullptr);
    EXPECT_NE(OH_Drawing_LineMetricsGetSize(vectorMetrics), 0);
    OH_Drawing_DestroyLineMetrics(vectorMetrics);
    OH_Drawing_LineMetrics* metrics = new OH_Drawing_LineMetrics();
    EXPECT_TRUE(OH_Drawing_TypographyGetLineMetricsAt(typography, 0, metrics));
    OH_Drawing_DestroyLineMetrics(metrics);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
}

/*
 * @tc.name: TypographyTest102
 * @tc.desc: test for the font parser
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TypographyTest102, TestSize.Level0)
{
    OH_Drawing_FontParser* parser = OH_Drawing_CreateFontParser();
    if (std::filesystem::exists(VIS_LIST_FILE_NAME)) {
        size_t fontNum;
        char** list = OH_Drawing_FontParserGetSystemFontList(parser, &fontNum);
        EXPECT_NE(list, nullptr);
        EXPECT_EQ(OH_Drawing_FontParserGetSystemFontList(nullptr, &fontNum), nullptr);
        const char* name = list[0];
        EXPECT_NE(OH_Drawing_FontParserGetFontByName(parser, name), nullptr);
        EXPECT_EQ(OH_Drawing_FontParserGetFontByName(nullptr, name), nullptr);
        OH_Drawing_DestroySystemFontList(list, fontNum);
        OH_Drawing_DestroySystemFontList(nullptr, fontNum);
    }
    char** listNull = OH_Drawing_FontParserGetSystemFontList(parser, nullptr);
    EXPECT_EQ(listNull, nullptr);
    OH_Drawing_DestroyFontParser(parser);
}

/*
 * @tc.name: FontParserGetSystemFontListTest001
 * @tc.desc: test for the OH_Drawing_FontParserGetSystemFontList.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, FontParserGetSystemFontListTest001, TestSize.Level0)
{
    OH_Drawing_FontParser* fontParser = OH_Drawing_CreateFontParser();
    ASSERT_NE(fontParser, nullptr);
    size_t value = 100; // 100 for test
    size_t* num = &value;
    if (std::filesystem::exists(VIS_LIST_FILE_NAME)) {
        ASSERT_NE(OH_Drawing_FontParserGetSystemFontList(fontParser, num), nullptr);
        EXPECT_EQ(value, 1);
    } else {
        ASSERT_EQ(OH_Drawing_FontParserGetSystemFontList(fontParser, num), nullptr);
        EXPECT_EQ(value, 0);
    }
    num = nullptr;
    ASSERT_EQ(OH_Drawing_FontParserGetSystemFontList(fontParser, num), nullptr);
    fontParser = nullptr;
    ASSERT_EQ(OH_Drawing_FontParserGetSystemFontList(fontParser, num), nullptr);
}

/*
 * @tc.name: DestroySystemFontListTest001
 * @tc.desc: test for the OH_Drawing_DestroySystemFontList.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, DestroySystemFontListTest001, TestSize.Level0)
{
    char** fontList = new char*[3]; // 3 means the number of font
    std::string tempStr1 = "Test1";
    std::string tempStr3 = "Test3";
    fontList[0] = new char[tempStr1.size() + 1];
    std::copy(tempStr1.begin(), tempStr1.end(), fontList[0]);
    fontList[0][tempStr1.size()] = '\0';
    fontList[1] = nullptr;
    fontList[2] = new char[tempStr3.size() + 1];              // 2 means the index of font
    std::copy(tempStr3.begin(), tempStr3.end(), fontList[2]); // 2 means the index of font
    fontList[2][tempStr3.size()] = '\0';                      // 2 means the index of font
    size_t num = 3;                                           // 3 means the number of font
    ASSERT_NE(fontList, nullptr);
    OH_Drawing_DestroySystemFontList(fontList, num);

    fontList = nullptr;
    OH_Drawing_DestroySystemFontList(fontList, num);
}

/*
 * @tc.name: FontParserGetFontByNameTest001
 * @tc.desc: test for the OH_Drawing_FontParserGetFontByName.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(NdkTypographyTest, FontParserGetFontByNameTest001, TestSize.Level0)
{
    OH_Drawing_FontParser* fontParser = OH_Drawing_CreateFontParser();
    ASSERT_NE(fontParser, nullptr);
    const char* name = "test";
    OH_Drawing_FontDescriptor* fontDescriptor = OH_Drawing_FontParserGetFontByName(fontParser, name);
    ASSERT_EQ(fontDescriptor, nullptr);

    OH_Drawing_FontDescriptor* fontDescriptor1 = OH_Drawing_FontParserGetFontByName(fontParser, nullptr);
    ASSERT_EQ(fontDescriptor1, nullptr);

    OH_Drawing_FontDescriptor* fontDescriptor2 = OH_Drawing_FontParserGetFontByName(nullptr, name);
    ASSERT_EQ(fontDescriptor2, nullptr);
    OH_Drawing_DestroyFontParser(fontParser);
}

} // namespace OHOS
