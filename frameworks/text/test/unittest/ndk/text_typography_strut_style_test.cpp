/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <securec.h>

#include "drawing_font_collection.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

constexpr float FLOAT_DATA_EPSILON = 1e-6f;
class NdkTypographyStrutStyleTest : public testing::Test {
};

/*
 * @tc.name: TypographyStrutStyleTest001
 * @tc.desc: test for getting and setting strut style
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStrutStyleTest, TypographyStrutStyleTest001, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_StrutStyle* strutstyle = new OH_Drawing_StrutStyle();
    strutstyle->weight = FONT_WEIGHT_400;
    strutstyle->style = FONT_STYLE_ITALIC;
    // 17.0 For size
    strutstyle->size = 17.0;
    // 2.0 For heightScale
    strutstyle->heightScale = 2;
    strutstyle->heightOverride = true;
    strutstyle->halfLeading = true;
    // 3.0 For leading
    strutstyle->leading = 3.0;
    strutstyle->forceStrutHeight = true;
    // 4 For families size
    strutstyle->familiesSize = 4;
    strutstyle->families = (char**)malloc(strutstyle->familiesSize * sizeof(char*));
    const char* temp[] = { "1", "2", "3", "4" };
    for (int i = 0; i < strutstyle->familiesSize; i++) {
        // 2 For families member size
        strutstyle->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(strutstyle->families[i], 2, temp[i]);
    }
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutstyle);
    EXPECT_NE(OH_Drawing_TypographyStyleGetStrutStyle(typoStyle), nullptr);
    OH_Drawing_TypographyStyleDestroyStrutStyle(strutstyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStrutStyleTest002
 * @tc.desc: test for strutstyle equals
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStrutStyleTest, TypographyStrutStyleTest002, TestSize.Level1)
{
    OH_Drawing_StrutStyle* from = new OH_Drawing_StrutStyle();
    OH_Drawing_StrutStyle* to = new OH_Drawing_StrutStyle();
    bool result = OH_Drawing_TypographyStyleStrutStyleEquals(from, to);
    EXPECT_TRUE(result);
    result = OH_Drawing_TypographyStyleStrutStyleEquals(nullptr, to);
    EXPECT_FALSE(result);
    result = OH_Drawing_TypographyStyleStrutStyleEquals(from, nullptr);
    EXPECT_FALSE(result);
    OH_Drawing_TypographyStyleDestroyStrutStyle(from);
    OH_Drawing_TypographyStyleDestroyStrutStyle(to);
}

/*
 * @tc.name: TypographyStrutStyleTest003
 * @tc.desc: test for setting strut style and getting strut style
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStrutStyleTest, TypographyStrutStyleTest003, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 10);

    OH_Drawing_StrutStyle* strutstyle = new OH_Drawing_StrutStyle();
    strutstyle->weight = FONT_WEIGHT_400;
    strutstyle->style = FONT_STYLE_ITALIC;
    // 17.0 For size
    strutstyle->size = 17.0;
    // 2.0 For heightScale
    strutstyle->heightScale = 2;
    strutstyle->heightOverride = true;
    strutstyle->halfLeading = true;
    // 3.0 For leading
    strutstyle->leading = 3.0;
    strutstyle->forceStrutHeight = true;
    // 4 For families size
    strutstyle->familiesSize = 4;
    strutstyle->families = (char**)malloc(strutstyle->familiesSize * sizeof(char*));
    const char* temp[] = { "1", "2", "3", "4" };
    for (int i = 0; i < strutstyle->familiesSize; i++) {
        // 2 For families member size
        strutstyle->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(strutstyle->families[i], 2, temp[i]);
    }
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutstyle);
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, true);
    ASSERT_NE(OH_Drawing_TypographyStyleGetStrutStyle(typoStyle), nullptr);

    OH_Drawing_TypographyCreate *handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    OH_Drawing_TextStyle *textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 52);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);

    const char* text = "HelloWorldHelloWorldHelloWorldHelloWorld";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200);

    OH_Drawing_LineMetrics lineMerics;
    OH_Drawing_TypographyGetLineMetricsAt(typography, 0, &lineMerics);
    EXPECT_EQ(lineMerics.height, 85);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_TypographyStyleDestroyStrutStyle(strutstyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

 /*
 * @tc.name: TypographyStrutStyleTest004
 * @tc.desc: test for strutstyle not equal
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStrutStyleTest, TypographyStrutStyleTest004, TestSize.Level1)
{
    OH_Drawing_StrutStyle* from = new OH_Drawing_StrutStyle();
    // 4 For families size
    from->familiesSize = 4;
    from->families = (char**)malloc(from->familiesSize * sizeof(char*));
    const char* temp[] = { "1", "2", "3", "4" };
    for (int i = 0; i < from->familiesSize; i++) {
        // 2 For families member size
        from->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(from->families[i], 2, temp[i]);
    }
    OH_Drawing_StrutStyle* to = new OH_Drawing_StrutStyle();
    // 3 For families size
    to->familiesSize = 3;
    to->families = (char**)malloc(to->familiesSize * sizeof(char*));
    const char* temp1[] = { "3", "2", "1"};
    for (int i = 0; i < to->familiesSize; i++) {
        // 2 For families member size
        to->families[i] = (char*)malloc(2 * sizeof(char));
        strcpy_s(to->families[i], 2, temp1[i]);
    }
    bool result = OH_Drawing_TypographyStyleStrutStyleEquals(from, to);
    EXPECT_FALSE(result);

    OH_Drawing_StrutStyle* from1 = new OH_Drawing_StrutStyle();
    from1->size = 98;
    OH_Drawing_StrutStyle* to1 = new OH_Drawing_StrutStyle();
    to1->size = 99;
    bool result1 = OH_Drawing_TypographyStyleStrutStyleEquals(from1, to1);
    EXPECT_FALSE(result1);
    OH_Drawing_TypographyStyleDestroyStrutStyle(from);
    OH_Drawing_TypographyStyleDestroyStrutStyle(to);
    OH_Drawing_TypographyStyleDestroyStrutStyle(from1);
    OH_Drawing_TypographyStyleDestroyStrutStyle(to1);
}

/*
 * @tc.name: TypographyStrutStyleTest005
 * @tc.desc: test for getting strut style where familySize equals 0
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStrutStyleTest, TypographyStrutStyleTest005, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_StrutStyle* strutstyle1 = new OH_Drawing_StrutStyle();
    strutstyle1->familiesSize = 0;
    strutstyle1->families = (char**)malloc(1 * sizeof(char*));
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutstyle1);
    OH_Drawing_StrutStyle* structStyle = OH_Drawing_TypographyStyleGetStrutStyle(typoStyle);
    EXPECT_EQ(structStyle->familiesSize, 0);
    EXPECT_EQ(structStyle->families, nullptr);

    free(strutstyle1->families);
    OH_Drawing_TypographyStyleDestroyStrutStyle(strutstyle1);

    // branch coverage
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, nullptr);
    OH_Drawing_TypographyStyleDestroyStrutStyle(nullptr);

    OH_Drawing_TypographyStyleDestroyStrutStyle(structStyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStrutStyleTest006
 * @tc.desc: test for strut style should not take effect when not enabled
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStrutStyleTest, TypographyStrutStyleTest006, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateSharedFontCollection();
    OH_Drawing_StrutStyle* strutStyle = new OH_Drawing_StrutStyle();
    strutStyle->size = 15;
    strutStyle->heightScale = 2;
    strutStyle->heightOverride = true;
    strutStyle->forceStrutHeight  = true;
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutStyle);

    const char* text = "你好世界";
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 15);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1000);

    OH_Drawing_LineMetrics lineMetrics;
    OH_Drawing_TypographyGetLineMetricsAt(typography, 0, &lineMetrics);
    EXPECT_NEAR(lineMetrics.height, 18.000000, FLOAT_DATA_EPSILON);

    delete strutStyle;
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
}