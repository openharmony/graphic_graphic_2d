/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <fstream>
#include <string>

#include "drawing_color.h"
#include "drawing_font.h"
#include "drawing_font_collection.h"
#include "drawing_text_declaration.h"
#include "drawing_text_line.h"
#include "drawing_text_lineTypography.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {

class LineTypographyTest : public testing::Test {
};

OH_Drawing_TypographyCreate *CreateTypographyCreateHandler(const char* text)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* typographyCreateHandler = OH_Drawing_CreateTypographyHandler(typoStyle,
        OH_Drawing_CreateFontCollection());
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(typographyCreateHandler, txtStyle);
    if (text != nullptr) {
        OH_Drawing_TypographyHandlerAddText(typographyCreateHandler, text);
    }
    return typographyCreateHandler;
}

/*
 * @tc.name: OH_Drawing_CreateLineTypographyTest001
 * @tc.desc: testing for the OH_Drawing_CreateTypographyCreateHandler and OH_Drawing_DestroyLineTypography
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_CreateLineTypographyTest001, TestSize.Level1)
{
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyCreate *handler1 = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography1 = OH_Drawing_CreateLineTypography(handler1);
    EXPECT_NE(lineTypography1, nullptr);

    OH_Drawing_LineTypography *lineTypography2 = OH_Drawing_CreateLineTypography(handler1);
    EXPECT_EQ(lineTypography2, nullptr);
    OH_Drawing_DestroyLineTypography(lineTypography1);

    OH_Drawing_LineTypography *nullLineTypograph = OH_Drawing_CreateLineTypography(nullptr);
    EXPECT_EQ(nullLineTypograph, nullptr);
    OH_Drawing_DestroyTypographyHandler(handler1);

    OH_Drawing_TypographyCreate *handler2 = CreateTypographyCreateHandler(nullptr);
    OH_Drawing_LineTypography *lineTypography3 = OH_Drawing_CreateLineTypography(handler2);
    EXPECT_EQ(lineTypography3, nullptr);
    OH_Drawing_DestroyTypographyHandler(handler2);
}

/*
 * @tc.name: OH_Drawing_GetLineBreakTest002
 * @tc.desc: normal testing for the OH_Drawing_LineTypographyGetLineBreak
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_GetLineBreakTest002, TestSize.Level1)
{
    const char* text = "OpenHarmony\n";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    double maxWidth = 800.0;
    size_t startIndex = 0;
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, strlen(text));
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_GetLineBreakTest003
 * @tc.desc: testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_GetLineBreakTest003, TestSize.Level1)
{
    const char* text1 = "hello\n world";
    const char* text2 = "hello\n";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text1);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    double maxWidth = 800.0;
    size_t startIndex = 0;
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, strlen(text2));
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_GetLineBreakTest004
 * @tc.desc: boundary testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_GetLineBreakTest004, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    double maxWidth = 800.0;
    size_t startIndex = strlen(text) - 1;
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 1);
    maxWidth = 0;
    startIndex = 0;
    count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 0);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_GetLineBreakTest005
 * @tc.desc: abnormal testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_GetLineBreakTest005, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    double maxWidth = 800.0;
    size_t startIndex = strlen(text);
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 0);
    maxWidth = 0.01;
    startIndex = 0;
    count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 1);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_CreateLineTest006
 * @tc.desc: testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_CreateLineTest006, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    size_t startIndex = 0;
    size_t count = strlen(text);
    auto line = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line, nullptr);
    OH_Drawing_DestroyTextLine(line);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_CreateLineTest007
 * @tc.desc: testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_CreateLineTest007, TestSize.Level1)
{
    const char* text = "Hello\n world!";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    size_t startIndex = 0;
    size_t count = strlen(text);
    auto line = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line, nullptr);
    OH_Drawing_DestroyTextLine(line);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_CreateLineTest008
 * @tc.desc: boundary testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_CreateLineTest008, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    size_t startIndex = strlen(text) - 1;
    size_t count = 1;
    auto line1 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line1, nullptr);
    OH_Drawing_DestroyTextLine(line1);

    startIndex = 0;
    count = 0;
    auto line2 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line2, nullptr);
    OH_Drawing_DestroyTextLine(line2);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_CreateLineTest009
 * @tc.desc: abnormal testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_CreateLineTest009, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    OH_Drawing_TypographyCreate *handler = CreateTypographyCreateHandler(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler);
    EXPECT_NE(lineTypography, nullptr);
    size_t startIndex = strlen(text);
    size_t count = 1;
    auto line1 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_EQ(line1, nullptr);
    OH_Drawing_DestroyTextLine(line1);

    startIndex = 0;
    count = strlen(text) + 1;
    auto line2 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_EQ(line2, nullptr);
    OH_Drawing_DestroyTextLine(line2);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: OH_Drawing_LineTypographyTest010
 * @tc.desc: complex scenes test for the LineTypography
 * @tc.type: FUNC
 */
HWTEST_F(LineTypographyTest, OH_Drawing_LineTypographyTest010, TestSize.Level1)
{
    std::string text = "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 ";
    text += "Drawing \n\n   \u231A \u513B \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本";
    OH_Drawing_TypographyCreate *createHandler = CreateTypographyCreateHandler(text.c_str());
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(createHandler);
    EXPECT_NE(lineTypography, nullptr);
    double maxWidth = 800.0;

    size_t startIndex = 0;
    do {
        auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
        if (count == 0) {
            break;
        }
        OH_Drawing_TextLine *line = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
        EXPECT_NE(line, nullptr);
        OH_Drawing_DestroyTextLine(line);
        startIndex += count;
    } while (true);
    OH_Drawing_DestroyLineTypography(lineTypography);
    OH_Drawing_DestroyTypographyHandler(createHandler);
}
} // namespace OHOS
