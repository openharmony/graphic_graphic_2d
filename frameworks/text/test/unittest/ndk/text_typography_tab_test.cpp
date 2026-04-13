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
 * @tc.name: CreateAndDestroyTextTab
 * @tc.desc: test for the text tab create and destroy
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, CreateAndDestroyTextTab, TestSize.Level0)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, 0.0);
    EXPECT_NE(textTab, nullptr);
    OH_Drawing_TextTab* textTab2 = OH_Drawing_CreateTextTab(TEXT_ALIGN_END, -1.0);
    EXPECT_NE(textTab2, nullptr);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_DestroyTextTab(textTab2);
}

/*
 * @tc.name: GetTextTabAlignment
 * @tc.desc: test for get alignment of the text tab
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, GetTextTabAlignment, TestSize.Level0)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, 0.0);
    EXPECT_EQ(OH_Drawing_GetTextTabAlignment(textTab), TEXT_ALIGN_LEFT);
    OH_Drawing_TextTab* textTab2 = OH_Drawing_CreateTextTab(TEXT_ALIGN_JUSTIFY, 0.0);
    EXPECT_EQ(OH_Drawing_GetTextTabAlignment(textTab), TEXT_ALIGN_LEFT);
    EXPECT_EQ(OH_Drawing_GetTextTabAlignment(nullptr), TEXT_ALIGN_LEFT);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_DestroyTextTab(textTab2);
}

/*
 * @tc.name: GetTextTabLocation
 * @tc.desc: test for get location of the text tab
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, GetTextTabLocation, TestSize.Level0)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, 0.0);
    EXPECT_EQ(OH_Drawing_GetTextTabLocation(textTab), 0.0);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_TextTab* textTab2 = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, -100.0);
    EXPECT_EQ(OH_Drawing_GetTextTabLocation(textTab2), -100.0);
    EXPECT_EQ(OH_Drawing_GetTextTabLocation(nullptr), 0.0);
    OH_Drawing_DestroyTextTab(textTab2);
}

/*
 * @tc.name: SetTypographyTextTab
 * @tc.desc: test for typography style set text tab
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, SetTypographyTextTab, TestSize.Level0)
{
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, -1.0);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextTab(typoStyle, textTab);
    OH_Drawing_SetTypographyTextTab(nullptr, textTab);
    OH_Drawing_SetTypographyTextTab(typoStyle, nullptr);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->tab.alignment, TextAlign::LEFT);
    EXPECT_EQ(ConvertToOriginalText(typoStyle)->tab.location, -1.0);
    OH_Drawing_DestroyTextTab(textTab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
/*
 * @tc.name: TabWithLeftAlignment
 * @tc.desc: test for text tab with left alignment
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TabWithLeftAlignment, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 10);

    OH_Drawing_TextTab* tab = OH_Drawing_CreateTextTab(TEXT_ALIGN_LEFT, 100);
    OH_Drawing_SetTypographyTextTab(typoStyle, tab);

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 52);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);

    const char* text = "Hello\tWorld\tHello";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200);

    double width = OH_Drawing_TypographyGetLongestLine(typography);
    EXPECT_NEAR(width, 522.772095, FLOAT_DATA_EPSILON);

    // branch coverage
    OH_Drawing_SetTypographyTextTab(typoStyle, nullptr);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextTab(tab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TabWithRightAlignment
 * @tc.desc: test for text tab with right alignment
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TabWithRightAlignment, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 10);

    OH_Drawing_TextTab* tab = OH_Drawing_CreateTextTab(TEXT_ALIGN_RIGHT, 100);
    OH_Drawing_SetTypographyTextTab(typoStyle, tab);

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 52);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);

    const char* text = "Hello\tWorld\tHello";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200);

    double width = OH_Drawing_TypographyGetLongestLine(typography);
    EXPECT_NEAR(width, 386.828308, FLOAT_DATA_EPSILON);

    // branch coverage
    OH_Drawing_SetTypographyTextTab(typoStyle, nullptr);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextTab(tab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TabWithCenterAlignment
 * @tc.desc: test for text tab with center alignment
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TabWithCenterAlignment, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 10);

    OH_Drawing_TextTab* tab = OH_Drawing_CreateTextTab(TEXT_ALIGN_CENTER, 100);
    OH_Drawing_SetTypographyTextTab(typoStyle, tab);

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 52);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);

    const char* text = "Hello\tWorld\tHello";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200);

    double width = OH_Drawing_TypographyGetLongestLine(typography);
    EXPECT_NEAR(width, 393.414185, FLOAT_DATA_EPSILON);

    // branch coverage
    OH_Drawing_SetTypographyTextTab(typoStyle, nullptr);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextTab(tab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TabWithCenterAlignmentAndTextAlign
 * @tc.desc: test for setting both the texttab and text layout direction
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TabWithCenterAlignmentAndTextAlign, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 10);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_CENTER);

    OH_Drawing_TextTab* tab = OH_Drawing_CreateTextTab(TEXT_ALIGN_CENTER, 100);
    OH_Drawing_SetTypographyTextTab(typoStyle, tab);

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 52);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);

    const char* text = "Hello\tWorld\tHello";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200);

    double width = OH_Drawing_TypographyGetLongestLine(typography);
    EXPECT_NEAR(width, 414.388336, FLOAT_DATA_EPSILON);

    // branch coverage
    OH_Drawing_SetTypographyTextTab(typoStyle, nullptr);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextTab(tab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TabWithEllipsis
 * @tc.desc: test for setting both the texttab and ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyTest, TabWithEllipsis, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, WORD_BREAK_TYPE_BREAK_ALL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 10);
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, ELLIPSIS_MODAL_TAIL);

    OH_Drawing_TextTab* tab = OH_Drawing_CreateTextTab(TEXT_ALIGN_CENTER, 100);
    OH_Drawing_SetTypographyTextTab(typoStyle, tab);

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateSharedFontCollection());
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 52);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);

    const char* text = "Hello\tWorld\tHello";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1200);

    double width = OH_Drawing_TypographyGetLongestLine(typography);
    EXPECT_NEAR(width, 414.388336, FLOAT_DATA_EPSILON);

    // branch coverage
    OH_Drawing_SetTypographyTextTab(typoStyle, nullptr);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextTab(tab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
} // namespace OHOS
