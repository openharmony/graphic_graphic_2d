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

#include <string>
#include <fstream>

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_font.h"
#include "drawing_font_collection.h"
#include "drawing_point.h"
#include "drawing_rect.h"
#include "drawing_text_declaration.h"
#include "drawing_text_line.h"
#include "drawing_text_typography.h"

#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {

class NativeDrawingLineTest : public testing::Test {
public:
    void SetUp() override
    {
        typoStyle_ = nullptr;
        txtStyle_ = nullptr;
        fontCollection_ = nullptr;
        handler_ = nullptr;
        typography_ = nullptr;
        cBitmap_ = nullptr;
        canvas_ = nullptr;
    }

    void TearDown() override
    {
        if (canvas_ != nullptr) {
            OH_Drawing_CanvasDestroy(canvas_);
            canvas_ = nullptr;
        }
        if (typography_ != nullptr) {
            OH_Drawing_DestroyTypography(typography_);
            typography_ = nullptr;
        }
        if (handler_ != nullptr) {
            OH_Drawing_DestroyTypographyHandler(handler_);
            handler_ = nullptr;
        }
        if (txtStyle_ != nullptr) {
            OH_Drawing_DestroyTextStyle(txtStyle_);
            txtStyle_ = nullptr;
        }
        if (typoStyle_ != nullptr) {
            OH_Drawing_DestroyTypographyStyle(typoStyle_);
            typoStyle_ = nullptr;
        }
        if (cBitmap_ != nullptr) {
            OH_Drawing_BitmapDestroy(cBitmap_);
            cBitmap_ = nullptr;
        }
        if (fontCollection_ != nullptr) {
            OH_Drawing_DestroyFontCollection(fontCollection_);
            fontCollection_ = nullptr;
        }
    }

    void PrepareCreateTextLine(const std::string& text);

protected:
    OH_Drawing_TypographyStyle* typoStyle_ = nullptr;
    OH_Drawing_TextStyle* txtStyle_ = nullptr;
    OH_Drawing_FontCollection* fontCollection_ = nullptr;
    OH_Drawing_TypographyCreate* handler_ = nullptr;
    OH_Drawing_Typography* typography_ = nullptr;
    OH_Drawing_Bitmap* cBitmap_ = nullptr;
    OH_Drawing_Canvas* canvas_ = nullptr;
};

void NativeDrawingLineTest::PrepareCreateTextLine(const std::string& text)
{
    double maxWidth = 500.0;
    uint32_t height = 40;
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    EXPECT_TRUE(typoStyle_ != nullptr);
    txtStyle_ = OH_Drawing_CreateTextStyle();
    EXPECT_TRUE(txtStyle_ != nullptr);
    fontCollection_ = OH_Drawing_CreateFontCollection();
    EXPECT_TRUE(fontCollection_ != nullptr);
    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    EXPECT_TRUE(handler_ != nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle_, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle_, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle_, halfLeading);
    const char* fontFamilies[] = {"Roboto"};
    OH_Drawing_SetTextStyleFontFamilies(txtStyle_, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
    OH_Drawing_TypographyHandlerPopTextStyle(handler_);
    typography_ = OH_Drawing_CreateTypography(handler_);
    EXPECT_TRUE(typography_ != nullptr);
    OH_Drawing_TypographyLayout(typography_, maxWidth);
    double position[2] = {10.0, 15.0};
    cBitmap_ = OH_Drawing_BitmapCreate();
    EXPECT_TRUE(cBitmap_ != nullptr);
    OH_Drawing_BitmapFormat cFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    uint32_t width = 20;
    OH_Drawing_BitmapBuild(cBitmap_, width, height, &cFormat);
    canvas_ = OH_Drawing_CanvasCreate();
    EXPECT_TRUE(canvas_ != nullptr);
    OH_Drawing_CanvasBind(canvas_, cBitmap_);
    OH_Drawing_CanvasClear(canvas_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography_, canvas_, position[0], position[1]);
}

/*
 * @tc.name: NativeDrawingLineTest001
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest001, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest002
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest002, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest003
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest003, TestSize.Level1)
{
    PrepareCreateTextLine("");
    OH_Drawing_Array* textLines = textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 0);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest004
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest004, TestSize.Level1)
{
    PrepareCreateTextLine("\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 2);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest005
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest005, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(nullptr);
    EXPECT_TRUE(textLines == nullptr);

    textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 4);

    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, -1);
    EXPECT_TRUE(textLine == nullptr);

    textLine = OH_Drawing_GetTextLineByIndex(textLines, 10);
    EXPECT_TRUE(textLine == nullptr);

    textLine = OH_Drawing_GetTextLineByIndex(nullptr, 0);
    EXPECT_TRUE(textLine == nullptr);

    textLine = OH_Drawing_GetTextLineByIndex(nullptr, -10);
    EXPECT_TRUE(textLine == nullptr);

    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest006
 * @tc.desc: test for the textLine GetGlyphCount.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest006, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double count = OH_Drawing_TextLineGetGlyphCount(textLine);
        EXPECT_GE(count, 10);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest007
 * @tc.desc: test for the textLine GetGlyphCount.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest007, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double count = OH_Drawing_TextLineGetGlyphCount(textLine);
        if (index == 3) {
            EXPECT_GE(count, 0);
        } else if (index == 6) {
            EXPECT_GE(count, 3);
        } else {
            EXPECT_GE(count, 10);
        }
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest008
 * @tc.desc: test for the textLine GetGlyphCount.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest008, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 5);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double count = OH_Drawing_TextLineGetGlyphCount(textLine);
        EXPECT_EQ(count, 0);
    }
    OH_Drawing_DestroyTextLines(textLines);

    double count = OH_Drawing_TextLineGetGlyphCount(nullptr);
    EXPECT_EQ(count, 0);
}

/*
 * @tc.name: NativeDrawingLineTest009
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest009, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);
    size_t start = 0;
    size_t end = 0;
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_TextLineGetTextRange(textLine, &start, &end);
        EXPECT_GE(start, 0);
        EXPECT_GT(end, 0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest010
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest010, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);

    size_t start = 0;
    size_t end = 0;
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_TextLineGetTextRange(textLine, &start, &end);
        EXPECT_GE(start, 0);
        EXPECT_GT(end, 0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest011
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest011, TestSize.Level1)
{
    PrepareCreateTextLine("\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 2);

    size_t start = 0;
    size_t end = 0;
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_TextLineGetTextRange(textLine, &start, &end);
        EXPECT_EQ(start, 0);
        EXPECT_EQ(end, 1);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest012
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest012, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 4);

    OH_Drawing_TextLine* textLine = textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    EXPECT_TRUE(textLine != nullptr);

    size_t start = 0;
    OH_Drawing_TextLineGetTextRange(textLine, &start, nullptr);
    EXPECT_EQ(start, 0);

    size_t end = 0;
    OH_Drawing_TextLineGetTextRange(textLine, nullptr, &end);
    EXPECT_EQ(end, 0);

    OH_Drawing_TextLineGetTextRange(textLine, nullptr, nullptr);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest013
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest013, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);
    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double width = OH_Drawing_TextLineGetTypographicBounds(textLine, &ascent, &descent, &leading);
        EXPECT_GE(ascent, 0);
        EXPECT_GE(descent, 0);
        EXPECT_GE(leading, 0);
        EXPECT_GT(width, 0);
        EXPECT_LE(width, 500.0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest014
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest014, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);

    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double width = OH_Drawing_TextLineGetTypographicBounds(textLine, &ascent, &descent, &leading);
        EXPECT_GE(ascent, 0);
        EXPECT_GE(descent, 0);
        EXPECT_GE(leading, 0);
        if (index == 3) {
            EXPECT_EQ(width, 0);
        } else {
            EXPECT_GT(width, 0);
        }
        EXPECT_LE(width, 500.0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest015
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest015, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 5);

    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double width = OH_Drawing_TextLineGetTypographicBounds(textLine, &ascent, &descent, &leading);
        EXPECT_GE(ascent, 0);
        EXPECT_GT(descent, 0);
        EXPECT_EQ(leading, 0);
        EXPECT_EQ(width, 0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NativeDrawingLineTest016
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingLineTest, NativeDrawingLineTest016, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    OH_Drawing_TextLine* textLine = textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    EXPECT_TRUE(textLine != nullptr);

    double ascent = 0.0;
    double width = OH_Drawing_TextLineGetTypographicBounds(textLine, &ascent, nullptr, nullptr);
    EXPECT_EQ(ascent, 0);
    EXPECT_EQ(width, 0);

    double descent = 0.0;
    width = OH_Drawing_TextLineGetTypographicBounds(textLine, nullptr, &descent, nullptr);
    EXPECT_EQ(descent, 0);
    EXPECT_EQ(width, 0);

    double leading = 0.0;
    width = OH_Drawing_TextLineGetTypographicBounds(textLine, nullptr, nullptr, &leading);
    EXPECT_EQ(descent, 0);
    EXPECT_EQ(width, 0);

    width = OH_Drawing_TextLineGetTypographicBounds(textLine, nullptr, nullptr, nullptr);
    EXPECT_EQ(width, 0);

    OH_Drawing_DestroyTextLines(textLines);
}
}