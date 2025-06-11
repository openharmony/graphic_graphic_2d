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

namespace {
constexpr int32_t KFLOAT_PRECISION = 1000000;
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
}

class NdkTextLineTest : public testing::Test {
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
    bool DrawingRectEquals(OH_Drawing_Rect* rect1, OH_Drawing_Rect* rect2);

protected:
    OH_Drawing_TypographyStyle* typoStyle_ = nullptr;
    OH_Drawing_TextStyle* txtStyle_ = nullptr;
    OH_Drawing_FontCollection* fontCollection_ = nullptr;
    OH_Drawing_TypographyCreate* handler_ = nullptr;
    OH_Drawing_Typography* typography_ = nullptr;
    OH_Drawing_Bitmap* cBitmap_ = nullptr;
    OH_Drawing_Canvas* canvas_ = nullptr;
};

void NdkTextLineTest::PrepareCreateTextLine(const std::string& text)
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

bool NdkTextLineTest::DrawingRectEquals(OH_Drawing_Rect* rect1, OH_Drawing_Rect* rect2)
{
    return
    std::round(OH_Drawing_RectGetLeft(rect1) * KFLOAT_PRECISION) / KFLOAT_PRECISION == OH_Drawing_RectGetLeft(rect2) &&
    std::round(OH_Drawing_RectGetTop(rect1) * KFLOAT_PRECISION) / KFLOAT_PRECISION == OH_Drawing_RectGetTop(rect2) &&
    std::round(OH_Drawing_RectGetBottom(rect1) * KFLOAT_PRECISION) / KFLOAT_PRECISION ==
        OH_Drawing_RectGetBottom(rect2) &&
    std::round(OH_Drawing_RectGetRight(rect1) * KFLOAT_PRECISION) / KFLOAT_PRECISION == OH_Drawing_RectGetRight(rect2);
}

/*
 * @tc.name: NdkTextLineTest001
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest001, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest002
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest002, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest003
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest003, TestSize.Level1)
{
    PrepareCreateTextLine("");
    OH_Drawing_Array* textLines = textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 0);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest004
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest004, TestSize.Level1)
{
    PrepareCreateTextLine("\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 2);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest005
 * @tc.desc: test for the textLine GetTextLines.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest005, TestSize.Level1)
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
 * @tc.name: NdkTextLineTest006
 * @tc.desc: test for the textLine GetGlyphCount.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest006, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    OH_Drawing_TextLine* textLine0 = OH_Drawing_GetTextLineByIndex(textLines, 0);
    double count0 = OH_Drawing_TextLineGetGlyphCount(textLine0);
    EXPECT_EQ(count0, 13);

    OH_Drawing_TextLine* textLine1 = OH_Drawing_GetTextLineByIndex(textLines, 1);
    double count1 = OH_Drawing_TextLineGetGlyphCount(textLine1);
    EXPECT_EQ(count1, 34);

    OH_Drawing_TextLine* textLine2 = OH_Drawing_GetTextLineByIndex(textLines, 2);
    double count2 = OH_Drawing_TextLineGetGlyphCount(textLine2);
    EXPECT_EQ(count2, 29);
    
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest007
 * @tc.desc: test for the textLine GetGlyphCount.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest007, TestSize.Level1)
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
            EXPECT_EQ(count, 0);
        } else if (index == 6) {
            EXPECT_EQ(count, 3);
        } else {
            EXPECT_GE(count, 10);
        }
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest008
 * @tc.desc: test for the textLine GetGlyphCount.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest008, TestSize.Level1)
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
 * @tc.name: NdkTextLineTest009
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest009, TestSize.Level1)
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
        if (index == 0) {
            EXPECT_EQ(start, 0);
            EXPECT_EQ(end, 16);
        }else if (index == 1) {
            EXPECT_EQ(start, 17);
            EXPECT_EQ(end, 51);
        }else {
            EXPECT_EQ(start, 52);
            EXPECT_EQ(end, 87);
        }
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest010
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest010, TestSize.Level1)
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
    std::vector<int32_t> startArr = {0, 26, 62, 98, 99, 176, 219};
    std::vector<int32_t> endArr = {25, 61, 97, 98, 176, 218, 222};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_TextLineGetTextRange(textLine, &start, &end);
        EXPECT_EQ(start, startArr[index]);
        EXPECT_EQ(end, endArr[index]);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest011
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest011, TestSize.Level1)
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
 * @tc.name: NdkTextLineTest012
 * @tc.desc: test for the textLine GetTextRange.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest012, TestSize.Level1)
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
 * @tc.name: NdkTextLineTest013
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest013, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);
    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    std::vector<float> widthArr = {206.639786, 490.139404, 459.509460};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double width = OH_Drawing_TextLineGetTypographicBounds(textLine, &ascent, &descent, &leading);
        EXPECT_NEAR(ascent, -27.84, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(descent, 7.32, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(leading, 0.0, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(width, widthArr[index], FLOAT_DATA_EPSILON);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest014
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest014, TestSize.Level1)
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
    std::vector<float> widthArr = {290.939697, 498.239380, 458.309509, 0.0, 497.952301, 409.497314, 51.300049};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double width = OH_Drawing_TextLineGetTypographicBounds(textLine, &ascent, &descent, &leading);
        EXPECT_EQ(leading, 0);
        if (index == 4) {
            EXPECT_NEAR(ascent, -27.84, FLOAT_DATA_EPSILON);
            EXPECT_NEAR(descent, 7.431193, FLOAT_DATA_EPSILON);
        } else if (index == 5) {
            EXPECT_NEAR(ascent, -35.369999, FLOAT_DATA_EPSILON);
            EXPECT_NEAR(descent, 9.690001, FLOAT_DATA_EPSILON);
        } else {
            EXPECT_NEAR(ascent, -27.84, FLOAT_DATA_EPSILON);
            EXPECT_NEAR(descent, 7.32, FLOAT_DATA_EPSILON);
        }
        EXPECT_NEAR(width, widthArr[index], FLOAT_DATA_EPSILON);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest015
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest015, TestSize.Level1)
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
        EXPECT_NEAR(ascent, -27.84, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(descent, 7.32, FLOAT_DATA_EPSILON);
        EXPECT_EQ(leading, 0);
        EXPECT_EQ(width, 0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest016
 * @tc.desc: test for the textLine GetTypographicBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest016, TestSize.Level1)
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
    EXPECT_EQ(leading, 0);
    EXPECT_EQ(width, 0);

    width = OH_Drawing_TextLineGetTypographicBounds(textLine, nullptr, nullptr, nullptr);
    EXPECT_EQ(width, 0);

    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest017
 * @tc.desc: test for the textLine GetImageBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest017, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Rect *rect = OH_Drawing_TextLineGetImageBounds(textLine);
        if (index == 0) {
            auto lineRect = OH_Drawing_RectCreate(2.0, 2.0, 196.329819, 29.0);
            EXPECT_TRUE(DrawingRectEquals(rect, lineRect));
            OH_Drawing_RectDestroy(lineRect);
        }else if (index == 1) {
            auto lineRect = OH_Drawing_RectCreate(1.0, 5.0, 481.109436, 37.0);
            EXPECT_TRUE(DrawingRectEquals(rect, lineRect));
            OH_Drawing_RectDestroy(lineRect);
        }else if (index == 2) {
            auto lineRect = OH_Drawing_RectCreate(1.0, 8.0, 441.099548, 42.0);
            EXPECT_TRUE(DrawingRectEquals(rect, lineRect));
            OH_Drawing_RectDestroy(lineRect);
        }
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest018
 * @tc.desc: test for the textLine GetImageBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest018, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);

    std::vector<OH_Drawing_Rect *> lineRectArr = {OH_Drawing_RectCreate(2.0, 3.0, 280.629761, 32.0),
        OH_Drawing_RectCreate(9.099991, 5.0, 489.209412, 37.0), OH_Drawing_RectCreate(1.0, 8.0, 447.999573, 42.0),
        OH_Drawing_RectCreate(0.0, 0.0, 0.0, 0.0), OH_Drawing_RectCreate(24.299973, 8.0, 498.514801, 44.0),
        OH_Drawing_RectCreate(0.0, 8.0, 409.497314, 44.0), OH_Drawing_RectCreate(2.0, 1.0, 50.199951, 25.0)};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Rect *rect = OH_Drawing_TextLineGetImageBounds(textLine);
        auto lineRect = lineRectArr[index];
        EXPECT_TRUE(DrawingRectEquals(rect, lineRect));
        OH_Drawing_RectDestroy(lineRect);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest019
 * @tc.desc: test for the textLine GetImageBounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest019, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 5);

    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Rect *rect = OH_Drawing_TextLineGetImageBounds(textLine);
        EXPECT_EQ(OH_Drawing_RectGetLeft(rect), 0);
        EXPECT_EQ(OH_Drawing_RectGetRight(rect), 0);
        EXPECT_EQ(OH_Drawing_RectGetTop(rect), 0);
        EXPECT_EQ(OH_Drawing_RectGetBottom(rect), 0);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_DestroyTextLines(textLines);

    OH_Drawing_Rect *rect = OH_Drawing_TextLineGetImageBounds(nullptr);
    EXPECT_TRUE(rect == nullptr);
}

/*
 * @tc.name: NdkTextLineTest020
 * @tc.desc: test for the textLine GetTrailingSpaceWidth.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest020, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    std::vector<float> widthArr = {8.099991, 8.099976, 16.199951};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double width = OH_Drawing_TextLineGetTrailingSpaceWidth(textLine);
        EXPECT_NEAR(width, widthArr[index], FLOAT_DATA_EPSILON);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest021
 * @tc.desc: test for the textLine GetTrailingSpaceWidth.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest021, TestSize.Level1)
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

        double width = OH_Drawing_TextLineGetTrailingSpaceWidth(textLine);
        if (index < 3) {
            EXPECT_NEAR(width, 8.099976, FLOAT_DATA_EPSILON);
        } else {
            EXPECT_EQ(width, 0.0);
        }
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest022
 * @tc.desc: test for the textLine GetTrailingSpaceWidth.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest022, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 5);

    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double width = OH_Drawing_TextLineGetTrailingSpaceWidth(textLine);
        EXPECT_EQ(width, 0.0);
    }
    OH_Drawing_DestroyTextLines(textLines);

    double width = OH_Drawing_TextLineGetTrailingSpaceWidth(nullptr);
    EXPECT_EQ(width, 0.0);
}

/*
 * @tc.name: NdkTextLineTest023
 * @tc.desc: test for the textLine GetStringIndexForPosition.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest023, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    const int maxCharacterNum = 88;
    std::vector<int32_t> leftPointIndexArr = {0, 15, 50};
    std::vector<int32_t> rightPointIndexArr = {6, 20, 55};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Point *point = OH_Drawing_PointCreate(1.0, 2.0);
        int32_t characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(textLine, point);
        EXPECT_EQ(characterIndex, leftPointIndexArr[index]);

        EXPECT_LT(characterIndex, maxCharacterNum);
        OH_Drawing_PointSet(point, 90.0, 4.0);
        characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(textLine, point);
        EXPECT_EQ(characterIndex, rightPointIndexArr[index]);

        OH_Drawing_PointDestroy(point);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest024
 * @tc.desc: test for the textLine GetStringIndexForPosition.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest024, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);
    std::vector<int32_t> leftPointIndexArr = {0, 20, 56, 89, 89, 113, 134};
    std::vector<int32_t> rightPointIndexArr = {20, 47, 83, 89, 110, 133, 137};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Point *point = OH_Drawing_PointCreate(1.0, 2.0);
        int32_t characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(textLine, point);
        EXPECT_EQ(characterIndex, leftPointIndexArr[index]);

        OH_Drawing_PointSet(point, 400.0, 4.0);
        characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(textLine, point);
        EXPECT_EQ(characterIndex, rightPointIndexArr[index]);
        OH_Drawing_PointDestroy(point);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest025
 * @tc.desc: test for the textLine GetStringIndexForPosition.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest025, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 4);

    for (size_t index = 0; index < size - 1; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Point *point = OH_Drawing_PointCreate(0.0, 2.0);
        int32_t characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(textLine, point);
        EXPECT_EQ(characterIndex, index + 1);
        OH_Drawing_PointSet(point, 400.0, 4.0);
        characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(textLine, point);
        EXPECT_EQ(characterIndex, index + 1);
        OH_Drawing_PointDestroy(point);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest026
 * @tc.desc: test for the textLine GetStringIndexForPosition.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest026, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    OH_Drawing_TextLine* textLine = textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    EXPECT_TRUE(textLine != nullptr);

    int32_t characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(textLine, nullptr);
    EXPECT_EQ(characterIndex, 0);

    OH_Drawing_Point *point = OH_Drawing_PointCreate(0.0, 2.0);
    characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(nullptr, point);
    EXPECT_EQ(characterIndex, 0);
    OH_Drawing_PointDestroy(point);

    characterIndex = OH_Drawing_TextLineGetStringIndexForPosition(nullptr, nullptr);
    EXPECT_EQ(characterIndex, 0);

    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest027
 * @tc.desc: test for the textLine GetOffsetForStringIndex.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest027, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    const int maxCharacterNum = 88;
    std::vector<float> offSetArr = {206.639786, 490.139404, 459.509460};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 0);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 10);
        if (index == 0) {
            EXPECT_NEAR(offset, 161.939835, FLOAT_DATA_EPSILON);
        } else {
            EXPECT_EQ(offset, 0.0);
        }
        EXPECT_LE(offset, 500.0);
        offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, maxCharacterNum);
        EXPECT_NEAR(offset, offSetArr[index], FLOAT_DATA_EPSILON);
        offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, -2);
        EXPECT_EQ(offset, 0.0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest028
 * @tc.desc: test for the textLine GetOffsetForStringIndex.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest028, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);

    const int maxCharacterNum = 88;
    std::vector<float> offSetArr = {290.939697, 498.239380, 458.309509};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 0);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 10);
        if (index == 0) {
            EXPECT_NEAR(offset, 155.129852, FLOAT_DATA_EPSILON);
        } else {
            EXPECT_EQ(offset, 0.0);
        }
        EXPECT_LE(offset, 500.0);
        offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, maxCharacterNum);
        if (index <= 2) {
            EXPECT_NEAR(offset, offSetArr[index], FLOAT_DATA_EPSILON);
        }  else {
            EXPECT_EQ(offset, 0.0);
        }
        EXPECT_LE(offset, 500.0);
        offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, -2);
        EXPECT_EQ(offset, 0.0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest029
 * @tc.desc: test for the textLine GetOffsetForStringIndex.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest029, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 0);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 100);
        EXPECT_EQ(offset, 0.0);
    }
    OH_Drawing_DestroyTextLines(textLines);

    double offset = OH_Drawing_TextLineGetOffsetForStringIndex(nullptr, 0);
    EXPECT_EQ(offset, 0.0);
}

/*
 * @tc.name: NdkTextLineTest030
 * @tc.desc: test for the textLine GetAlignmentOffset.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest030, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    std::vector<float> offSetArr = {250.730103, 108.980286, 128.345245};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 0.0, 600);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 0.5, 700);
        EXPECT_NEAR(offset, offSetArr[index], FLOAT_DATA_EPSILON);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, -1.0, 700);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 2.0, 20);
        EXPECT_EQ(offset, 0.0);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest031
 * @tc.desc: test for the textLine GetAlignmentOffset.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest031, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);

    std::vector<float> offSetArr = {208.580139, 104.930298, 124.895233, 350.000000, 101.023849, 145.251343, 324.349976};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 0.0, 600);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 0.5, 700);
        EXPECT_NEAR(offset, offSetArr[index], FLOAT_DATA_EPSILON);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, -1.0, 700);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 2.0, 20);
        if (index == 3) {
            EXPECT_EQ(offset, 20.0);
        } else {
            EXPECT_EQ(offset, 0.0);
        }
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest032
 * @tc.desc: test for the textLine GetAlignmentOffset.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest032, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 5);

    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        double offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 0.0, 600);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 0.5, 700);
        EXPECT_EQ(offset, 350.0);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, -1.0, -700);
        EXPECT_EQ(offset, 0.0);
        offset = OH_Drawing_TextLineGetAlignmentOffset(textLine, 2.0, 20);
        EXPECT_EQ(offset, 20.0);
    }
    OH_Drawing_DestroyTextLines(textLines);

    double offset = OH_Drawing_TextLineGetAlignmentOffset(nullptr, 0.0, 0.0);
    EXPECT_EQ(offset, 0.0);
}

/*
 * @tc.name: NdkTextLineTest033
 * @tc.desc: test for the textLine EnumerateCaretOffsets.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest033, TestSize.Level1)
{
    PrepareCreateTextLine("H测😀مرحب\n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);
    OH_Drawing_TextLine* textLine1 = OH_Drawing_GetTextLineByIndex(textLines, 0);
    OH_Drawing_TextLineEnumerateCaretOffsets(textLine1, [](double offset, int32_t index, bool leadingEdge) {
        static int offsetNum = 0;
        if (index == 0 && leadingEdge) {
            EXPECT_NEAR(offset, 0.0, FLOAT_DATA_EPSILON);
        } else if (index == 1 && leadingEdge) {
            EXPECT_NEAR(offset, 22.349976, FLOAT_DATA_EPSILON);
        } else if (index == 2 && leadingEdge) {
            EXPECT_NEAR(offset, 52.349945, FLOAT_DATA_EPSILON);
        } else {
            EXPECT_LE(offset, 500.0);
        }
        if (offsetNum++ % 2 == 0) {
            EXPECT_TRUE(leadingEdge);
        } else {
            EXPECT_FALSE(leadingEdge);
        }
        EXPECT_LE(index, 51);
        return index > 50;
    });
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest034
 * @tc.desc: test for the textLine EnumerateCaretOffsets.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest034, TestSize.Level1)
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

        OH_Drawing_TextLineEnumerateCaretOffsets(textLine, [](double offset, int32_t index, bool leadingEdge) {
            EXPECT_GE(index, 0);
            EXPECT_EQ(offset, 0.0);
            EXPECT_TRUE(leadingEdge);
            return true;
        });
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest035
 * @tc.desc: test for the textLine EnumerateCaretOffsets.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest035, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 4);

    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_TextLineEnumerateCaretOffsets(textLine, [](double offset, int32_t index, bool leadingEdge) {
            EXPECT_GE(index, 0);
            EXPECT_EQ(offset, 0.0);
            EXPECT_TRUE(leadingEdge);
            return false;
        });
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest036
 * @tc.desc: test for the textLine EnumerateCaretOffsets.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest036, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    OH_Drawing_TextLine* textLine = textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    EXPECT_TRUE(textLine != nullptr);
    OH_Drawing_TextLineEnumerateCaretOffsets(textLine, nullptr);
    OH_Drawing_TextLineEnumerateCaretOffsets(nullptr, [](double offset, int32_t index, bool leadingEdge) {
            return false;
        });
    OH_Drawing_TextLineEnumerateCaretOffsets(nullptr, nullptr);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest037
 * @tc.desc: test for the textLine CreateTruncatedLine.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest037, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    std::vector<int32_t> countArr1 = {7, 10, 7};
    std::vector<int32_t> countArr2 = {4, 4, 5};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_TextLine *truncatedLine =
        OH_Drawing_TextLineCreateTruncatedLine(textLine, 100, ELLIPSIS_MODAL_HEAD, "...");
        EXPECT_TRUE(truncatedLine != nullptr);
        OH_Drawing_TextLinePaint(truncatedLine, canvas_, 30, 250);
        double count = OH_Drawing_TextLineGetGlyphCount(truncatedLine);
        EXPECT_EQ(count, countArr1[index]);
        
        OH_Drawing_DestroyTextLine(truncatedLine);
        truncatedLine = OH_Drawing_TextLineCreateTruncatedLine(textLine, 80, ELLIPSIS_MODAL_MIDDLE, "...");
        EXPECT_TRUE(truncatedLine == nullptr);
        OH_Drawing_DestroyTextLine(truncatedLine);
        truncatedLine = OH_Drawing_TextLineCreateTruncatedLine(textLine, 50, ELLIPSIS_MODAL_TAIL, "...");
        EXPECT_TRUE(truncatedLine != nullptr);
        OH_Drawing_TextLinePaint(truncatedLine, canvas_, 30, 550);
        count = OH_Drawing_TextLineGetGlyphCount(truncatedLine);
        EXPECT_EQ(count, countArr2[index]);
        OH_Drawing_DestroyTextLine(truncatedLine);
    }
    OH_Drawing_DestroyTextLines(textLines);
}
/*
 * @tc.name: NdkTextLineTest038
 * @tc.desc: test for the textLine CreateTruncatedLine.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest038, TestSize.Level1)
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

        OH_Drawing_TextLine *truncatedLine =
        OH_Drawing_TextLineCreateTruncatedLine(textLine, 30, ELLIPSIS_MODAL_TAIL, "123");
        EXPECT_TRUE(truncatedLine != nullptr);
        OH_Drawing_TextLinePaint(truncatedLine, canvas_, 30, 150);
        double count = OH_Drawing_TextLineGetGlyphCount(truncatedLine);
        if (index == 3) {
            EXPECT_EQ(count, 0);
        } else {
            EXPECT_EQ(count, 3);
        }
        OH_Drawing_DestroyTextLine(truncatedLine);
        truncatedLine = OH_Drawing_TextLineCreateTruncatedLine(textLine, 30, ELLIPSIS_MODAL_HEAD, "测试");
        EXPECT_TRUE(truncatedLine != nullptr);
        OH_Drawing_TextLinePaint(truncatedLine, canvas_, 30, 300);
        count = OH_Drawing_TextLineGetGlyphCount(truncatedLine);
        if (index == 3) {
            EXPECT_EQ(count, 0);
        } else {
            EXPECT_EQ(count, 3);
        }
        OH_Drawing_DestroyTextLine(truncatedLine);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest039
 * @tc.desc: test for the textLine CreateTruncatedLine.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest039, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    OH_Drawing_TextLine* textLine = textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    EXPECT_TRUE(textLine != nullptr);

    OH_Drawing_TextLine *truncatedLine = OH_Drawing_TextLineCreateTruncatedLine(nullptr, 20, ELLIPSIS_MODAL_TAIL, "1");
    EXPECT_TRUE(truncatedLine == nullptr);

    truncatedLine = OH_Drawing_TextLineCreateTruncatedLine(textLine, 200, 5, "1");
    EXPECT_TRUE(truncatedLine == nullptr);

    truncatedLine = OH_Drawing_TextLineCreateTruncatedLine(textLine, 100, ELLIPSIS_MODAL_TAIL, nullptr);
    EXPECT_TRUE(truncatedLine == nullptr);

    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: NdkTextLineTest040
 * @tc.desc: test for the textLine GetGlyphRuns.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest040, TestSize.Level1)
{
    PrepareCreateTextLine("Hello 测 World \n!@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp 试 Drawing  ");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    std::vector<int32_t> sizeArr = {4, 1, 6};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Array *runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        EXPECT_TRUE(runs != nullptr);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        EXPECT_EQ(runsSize, sizeArr[index]);
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}
/*
 * @tc.name: NdkTextLineTest041
 * @tc.desc: test for the textLine GetGlyphRuns.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest041, TestSize.Level1)
{
    PrepareCreateTextLine(
        "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/ Drawing testlp 试 "
        "Drawing \n\n   \u231A \u513B"
        " \u00A9\uFE0F aaa clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 World测试文本\n123");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 7);

    std::vector<int32_t> sizeArr = {6, 1, 6, 0, 7, 8, 1};
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Array *runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        if (index == 3) {
            EXPECT_TRUE(runs == nullptr);
            EXPECT_EQ(runsSize, 0);
        } else if (index == 1 || index == 6) {
            EXPECT_TRUE(runs != nullptr);
            EXPECT_EQ(runsSize, 1);
        } else {
            EXPECT_TRUE(runs != nullptr);
            EXPECT_GE(runsSize, 6);
        }
        EXPECT_EQ(runsSize, sizeArr[index]);
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}
/*
 * @tc.name: NdkTextLineTest042
 * @tc.desc: test for the textLine GetGlyphRuns.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest042, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 4);

    for (size_t index = 0; index < size - 1; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);

        OH_Drawing_Array *runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        EXPECT_TRUE(runs == nullptr);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        EXPECT_EQ(runsSize, 0);
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}
/*
 * @tc.name: NdkTextLineTest043
 * @tc.desc: test for the textLine GetGlyphRuns.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextLineTest, NdkTextLineTest043, TestSize.Level1)
{
    PrepareCreateTextLine("\n\n");
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_EQ(size, 3);

    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    EXPECT_TRUE(textLine != nullptr);

    OH_Drawing_Array *runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    EXPECT_TRUE(runs == nullptr);

    runs = OH_Drawing_TextLineGetGlyphRuns(nullptr);
    EXPECT_TRUE(runs == nullptr);

    OH_Drawing_DestroyTextLines(textLines);
}
}