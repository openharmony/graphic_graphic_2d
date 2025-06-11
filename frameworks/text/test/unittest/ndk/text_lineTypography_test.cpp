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

class NdkLineTypographyTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void PrepareTypographyCreate(const char* text);

protected:
    OH_Drawing_TypographyCreate* handler_ = nullptr;
    OH_Drawing_FontCollection* fontCollection_ = nullptr;
    OH_Drawing_TextStyle* txtStyle_ = nullptr;
    OH_Drawing_TypographyStyle* typoStyle_ = nullptr;
};

void NdkLineTypographyTest::SetUp()
{
    handler_ = nullptr;
    fontCollection_ = nullptr;
    txtStyle_ = nullptr;
    typoStyle_ = nullptr;
}

void NdkLineTypographyTest::TearDown()
{
    if (handler_ != nullptr) {
        OH_Drawing_DestroyTypographyHandler(handler_);
        handler_ = nullptr;
    }
    if (txtStyle_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle_);
        txtStyle_ = nullptr;
    }
    if (fontCollection_ != nullptr) {
        OH_Drawing_DestroyFontCollection(fontCollection_);
        fontCollection_ = nullptr;
    }
    if (typoStyle_ != nullptr) {
        OH_Drawing_DestroyTypographyStyle(typoStyle_);
        typoStyle_ = nullptr;
    }
}

void NdkLineTypographyTest::PrepareTypographyCreate(const char* text)
{
    fontCollection_ = OH_Drawing_CreateFontCollection();
    EXPECT_TRUE(fontCollection_ != nullptr);
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    EXPECT_TRUE(typoStyle_ != nullptr);
    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    EXPECT_TRUE(handler_ != nullptr);
    txtStyle_ = OH_Drawing_CreateTextStyle();
    EXPECT_TRUE(txtStyle_ != nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle_, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle_, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle_, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle_, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    if (text != nullptr) {
        OH_Drawing_TypographyHandlerAddText(handler_, text);
    }
}

/*
 * @tc.name: CreateLineTypographyTest001
 * @tc.desc: testing for the OH_Drawing_CreateLineTypography and OH_Drawing_DestroyLineTypography
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, CreateLineTypographyTest001, TestSize.Level1)
{
    PrepareTypographyCreate("OpenHarmony\n");
    OH_Drawing_LineTypography *lineTypography1 = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography1, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography1);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 12);

    OH_Drawing_LineTypography *lineTypography2 = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_EQ(lineTypography2, nullptr);
    OH_Drawing_DestroyLineTypography(lineTypography1);

    OH_Drawing_LineTypography *nullLineTypograph = OH_Drawing_CreateLineTypography(nullptr);
    EXPECT_EQ(nullLineTypograph, nullptr);
}

/*
 * @tc.name: CreateLineTypographyTest002
 * @tc.desc: testing for the OH_Drawing_TypographyCreate does not contain text
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, CreateLineTypographyTest002, TestSize.Level1)
{
    PrepareTypographyCreate(nullptr);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_EQ(lineTypography, nullptr);
}

/*
 * @tc.name: GetLineBreakTest003
 * @tc.desc: normal testing for the OH_Drawing_LineTypographyGetLineBreak
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, GetLineBreakTest003, TestSize.Level1)
{
    const char* text = "OpenHarmony\n";
    PrepareTypographyCreate(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 12);

    double maxWidth = 800.0;
    size_t startIndex = 0;
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, strlen(text));
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: GetLineBreakTest004
 * @tc.desc: testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, GetLineBreakTest004, TestSize.Level1)
{
    const char* text1 = "hello\n world";
    const char* text2 = "hello\n";
    PrepareTypographyCreate(text1);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 12);

    double maxWidth = 800.0;
    size_t startIndex = 0;
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, strlen(text2));
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: GetLineBreakTest005
 * @tc.desc: boundary testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, GetLineBreakTest005, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    PrepareTypographyCreate(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 13);

    double maxWidth = 800.0;
    size_t startIndex = strlen(text) - 1;
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 1);
    maxWidth = 0;
    startIndex = 0;
    count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 0);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: GetLineBreakTest006
 * @tc.desc: abnormal testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, GetLineBreakTest006, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    PrepareTypographyCreate(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 13);

    double maxWidth = 800.0;
    size_t startIndex = strlen(text);
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 0);
    maxWidth = 0.01;
    startIndex = 0;
    count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
    EXPECT_EQ(count, 1);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: CreateLineTest007
 * @tc.desc: testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, CreateLineTest007, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    PrepareTypographyCreate(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 13);

    size_t startIndex = 0;
    size_t count = strlen(text);
    auto line = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line, nullptr);
    EXPECT_EQ(OH_Drawing_TextLineGetGlyphCount(line), 12L);
    OH_Drawing_DestroyTextLine(line);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: CreateLineTest008
 * @tc.desc: testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, CreateLineTest008, TestSize.Level1)
{
    const char* text = "Hello\n world!";
    PrepareTypographyCreate(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 13);

    size_t startIndex = 0;
    size_t count = strlen(text);
    auto line = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line, nullptr);
    EXPECT_EQ(OH_Drawing_TextLineGetGlyphCount(line), 5L);
    OH_Drawing_DestroyTextLine(line);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: CreateLineTest009
 * @tc.desc: boundary testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, CreateLineTest009, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    PrepareTypographyCreate(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 13);

    size_t startIndex = strlen(text) - 1;
    size_t count = 1;
    auto line1 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line1, nullptr);
    EXPECT_EQ(OH_Drawing_TextLineGetGlyphCount(line1), 0L);
    OH_Drawing_DestroyTextLine(line1);

    startIndex = 0;
    count = 0;
    auto line2 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_NE(line2, nullptr);
    EXPECT_EQ(OH_Drawing_TextLineGetGlyphCount(line2), 12L);
    OH_Drawing_DestroyTextLine(line2);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: CreateLineTest010
 * @tc.desc: abnormal testing for the OH_Drawing_LineTypographyGetLineBreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, CreateLineTest010, TestSize.Level1)
{
    const char* text = "OpenHarmoney\n";
    PrepareTypographyCreate(text);
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypography);
    EXPECT_EQ(innerlineTypography->GetUnicodeSize(), 13);

    size_t startIndex = strlen(text);
    size_t count = 1;
    auto line1 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_EQ(OH_Drawing_TextLineGetGlyphCount(line1), 0L);
    EXPECT_EQ(line1, nullptr);
    OH_Drawing_DestroyTextLine(line1);

    startIndex = 0;
    count = strlen(text) + 1;
    auto line2 = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    EXPECT_EQ(line2, nullptr);
    EXPECT_EQ(OH_Drawing_TextLineGetGlyphCount(line2), 0L);
    OH_Drawing_DestroyTextLine(line2);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: LineTypographyTest011
 * @tc.desc: complex scenes test for the LineTypography
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, LineTypographyTest011, TestSize.Level1)
{
    std::string text = "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 ";
    text += "Drawing \n\n   \u231A \u513B \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本";
    PrepareTypographyCreate(text.c_str());
    OH_Drawing_LineTypography *lineTypography = OH_Drawing_CreateLineTypography(handler_);
    EXPECT_NE(lineTypography, nullptr);
    double maxWidth = 800.0;
    size_t startIndex = 0;
    int yPosition = 0;
    do {
        auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, maxWidth);
        if (count == 0) {
            break;
        }
        OH_Drawing_TextLine *line = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
        EXPECT_NE(line, nullptr);
        yPosition += 30;
        OH_Drawing_DestroyTextLine(line);
        startIndex += count;
    } while (true);
    OH_Drawing_DestroyLineTypography(lineTypography);
}

/*
 * @tc.name: LineTypographyTest012
 * @tc.desc: complex scenes test for the LineTypography runsize
 * @tc.type: FUNC
 */
HWTEST_F(NdkLineTypographyTest, LineTypographyTest012, TestSize.Level1)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextDirection(typoStyle, TEXT_DIRECTION_LTR);
    OH_Drawing_SetTypographyTextAlign(typoStyle, TEXT_ALIGN_LEFT);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "头好痒 test 我 test";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_LineTypography* lineTypography = OH_Drawing_CreateLineTypography(handler);
    OH_Drawing_TextLine* textLine = OH_Drawing_LineTypographyCreateLine(lineTypography, 0, 15);
    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
    EXPECT_EQ(runsSize, 6);
    OH_Drawing_DestroyTextLine(textLine);
    OH_Drawing_DestroyLineTypography(lineTypography);
}
} // namespace OHOS
