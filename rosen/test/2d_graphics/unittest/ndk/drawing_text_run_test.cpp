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

#include "drawing_bitmap.h"
#include "drawing_font_collection.h"
#include "drawing_point.h"
#include "drawing_text_line.h"
#include "drawing_text_run.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "string"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
    constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
}

class NativeDrawingRunTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {};
    void TearDown() override;
    void PrepareCreateTextLine();

    OH_Drawing_TypographyStyle* typeStyle_ = nullptr;
    OH_Drawing_TextStyle* txtStyle_ = nullptr;
    OH_Drawing_FontCollection* fontCollection_ = nullptr;
    OH_Drawing_TypographyCreate* handler_ = nullptr;
    OH_Drawing_Typography* typography_ = nullptr;
    OH_Drawing_Bitmap* cBitmap_ = nullptr;
    OH_Drawing_Canvas* canvas_ = nullptr;
    std::string text_;
};

void NativeDrawingRunTest::PrepareCreateTextLine()
{
    double maxWidth = 500.0;
    uint32_t height = 40;
    typeStyle_ = OH_Drawing_CreateTypographyStyle();
    EXPECT_TRUE(typeStyle_ != nullptr);
    txtStyle_ = OH_Drawing_CreateTextStyle();
    EXPECT_TRUE(txtStyle_ != nullptr);
    fontCollection_ = OH_Drawing_CreateFontCollection();
    EXPECT_TRUE(fontCollection_ != nullptr);
    handler_ = OH_Drawing_CreateTypographyHandler(typeStyle_, fontCollection_);
    EXPECT_TRUE(handler_ != nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle_, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle_, FONT_WEIGHT_400);
    bool halfLeading = true;
    OH_Drawing_SetTextStyleHalfLeading(txtStyle_, halfLeading);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle_, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text_.c_str());
    OH_Drawing_TypographyHandlerPopTextStyle(handler_);
    typography_ = OH_Drawing_CreateTypography(handler_);
    EXPECT_TRUE(typography_ != nullptr);
    OH_Drawing_TypographyLayout(typography_, maxWidth);
    double position[2] = { 10.0, 15.0 };
    OH_Drawing_Bitmap* cBitmap_ = OH_Drawing_BitmapCreate();
    EXPECT_TRUE(cBitmap_ != nullptr);
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    uint32_t width = 20;
    OH_Drawing_BitmapBuild(cBitmap_, width, height, &cFormat);
    canvas_ = OH_Drawing_CanvasCreate();
    EXPECT_TRUE(canvas_ != nullptr);
    OH_Drawing_CanvasBind(canvas_, cBitmap_);
    OH_Drawing_CanvasClear(canvas_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(typography_, canvas_, position[0], position[1]);
}

void NativeDrawingRunTest::TearDown()
{
    OH_Drawing_CanvasDestroy(canvas_);
    canvas_ = nullptr;
    OH_Drawing_BitmapDestroy(cBitmap_);
    cBitmap_ = nullptr;
    OH_Drawing_DestroyTypography(typography_);
    typography_ = nullptr;
    OH_Drawing_DestroyTypographyHandler(handler_);
    handler_ = nullptr;
    OH_Drawing_DestroyFontCollection(fontCollection_);
    fontCollection_ = nullptr;
    OH_Drawing_DestroyTextStyle(txtStyle_);
    txtStyle_ = nullptr;
    OH_Drawing_DestroyTypographyStyle(typeStyle_);
    typeStyle_ = nullptr;
    text_ = "";
}

/*
 * @tc.name: OH_Drawing_RunTest001
 * @tc.desc: Test for the run one line testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest001, TestSize.Level1)
{
    text_ = "Hello 你好 World";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        EXPECT_TRUE(runs != nullptr);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
            OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
            EXPECT_TRUE(run != nullptr);
            uint32_t count = OH_Drawing_GetRunGlyphCount(run);
            EXPECT_TRUE(count > 0);
            uint64_t location = 0;
            uint64_t length = 0;
            OH_Drawing_GetRunStringRange(run, &location, &length);
            EXPECT_TRUE(location >= 0);
            EXPECT_TRUE(length > 0);
            OH_Drawing_Array* stringIndicesArr = OH_Drawing_GetRunStringIndices(run, 0, count);
            size_t size = OH_Drawing_GetDrawingArraySize(stringIndicesArr);
            for (size_t stringIndex = 0; stringIndex < size; stringIndex++) {
                uint64_t indices = OH_Drawing_GetRunStringIndicesByIndex(stringIndicesArr, stringIndex);
                EXPECT_TRUE(indices >= 0);
            }
            OH_Drawing_DestroyRunStringIndices(stringIndicesArr);
            OH_Drawing_Rect* bounds = OH_Drawing_GetRunImageBounds(run);
            EXPECT_TRUE(bounds != nullptr);
            OH_Drawing_DestroyRunImageBounds(bounds);
            OH_Drawing_RunPaint(canvas_, run, 0.0, 0.0);
        }
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest002
 * @tc.desc: Test for the run one line testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest002, TestSize.Level1)
{
    text_ = "Hello 你好 World";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
            OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
            uint32_t count = OH_Drawing_GetRunGlyphCount(run);
            float ascent = 0.0;
            float descent = 0.0;
            float leading = 0.0;
            float width = OH_Drawing_GetRunTypographicBounds(run, &ascent, &descent, &leading);
            EXPECT_TRUE(ascent < 0);
            EXPECT_TRUE(descent > 0);
            EXPECT_TRUE(leading >= 0); // The leading minimum is 0
            EXPECT_TRUE(width > 0);
            OH_Drawing_Array* glyphs = OH_Drawing_GetRunGlyphs(run, 0, count);
            EXPECT_TRUE(glyphs != nullptr);
            size_t glyphSize = OH_Drawing_GetDrawingArraySize(glyphs);
            for (size_t glyphsIndex = 0; glyphsIndex < glyphSize; glyphsIndex++) {
                EXPECT_TRUE(OH_Drawing_GetRunGlyphsByIndex(glyphs, glyphsIndex) > 0);
            }
            OH_Drawing_DestroyRunGlyphs(glyphs);
            OH_Drawing_Array* positions = OH_Drawing_GetRunPositions(run, 0, count);
            EXPECT_TRUE(positions != nullptr);
            size_t positionSize = OH_Drawing_GetDrawingArraySize(positions);
            for (size_t posIndex = 0; posIndex < positionSize; posIndex++) {
                OH_Drawing_Point* pos = OH_Drawing_GetRunPositionsByIndex(positions, posIndex);
                EXPECT_TRUE(pos != nullptr);
                float x = 0.0;
                OH_Drawing_PointGetX(pos, &x);
                EXPECT_TRUE(x >= 0.0);
            }
            OH_Drawing_DestroyRunPositions(positions);
        }
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest003
 * @tc.desc: Test for the run of multilingual, multi-line string testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest003, TestSize.Level1)
{
    text_ = "Hello\t中国 World \n !@#%^&*){}[] 123456789 -="
        " ,. < >、/ Draclp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测文本\n 123";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
            OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
            EXPECT_TRUE(run != nullptr);
            uint32_t count = OH_Drawing_GetRunGlyphCount(run);
            EXPECT_TRUE(count > 0);
            uint64_t location = 0;
            uint64_t length = 0;
            OH_Drawing_GetRunStringRange(run, &location, &length);
            EXPECT_TRUE(location >= 0);
            EXPECT_TRUE(length > 0);
            OH_Drawing_Array* stringIndicesArr = OH_Drawing_GetRunStringIndices(run, 0, count);
            size_t indiceSize = OH_Drawing_GetDrawingArraySize(stringIndicesArr);
            for (size_t stringIndex = 0; stringIndex < indiceSize; stringIndex++) {
                uint64_t indices = OH_Drawing_GetRunStringIndicesByIndex(stringIndicesArr, stringIndex);
                EXPECT_TRUE(indices >= 0);
            }
            OH_Drawing_DestroyRunStringIndices(stringIndicesArr);
            stringIndicesArr = nullptr;
            OH_Drawing_Rect* bounds = OH_Drawing_GetRunImageBounds(run);
            EXPECT_TRUE(bounds != nullptr);
            OH_Drawing_DestroyRunImageBounds(bounds);
            OH_Drawing_RunPaint(canvas_, run, 0, 0);
        }
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest004
 * @tc.desc: Test for the run of multilingual, multi-line string testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest004, TestSize.Level1)
{
    text_ = "Hello\t中国 World \n !@#%^&*){}[] 123456789 -="
        " ,. < >、/ Draclp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测文本\n 123";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
            OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
            uint32_t count = OH_Drawing_GetRunGlyphCount(run);
            float ascent = 0.0;
            float descent = 0.0;
            float leading = 0.0;
            float width = OH_Drawing_GetRunTypographicBounds(run, &ascent, &descent, &leading);
            EXPECT_TRUE(ascent < 0);
            EXPECT_TRUE(descent > 0);
            EXPECT_TRUE(leading >= 0); // The leading minimum is 0
            EXPECT_TRUE(width > 0);
            OH_Drawing_Array* glyphs = OH_Drawing_GetRunGlyphs(run, 0, count);
            size = OH_Drawing_GetDrawingArraySize(glyphs);
            EXPECT_TRUE(glyphs != nullptr);
            for (size_t glyphsIndex = 0; glyphsIndex < size; glyphsIndex++) {
                EXPECT_TRUE(OH_Drawing_GetRunGlyphsByIndex(glyphs, glyphsIndex) > 0);
            }
            OH_Drawing_DestroyRunGlyphs(glyphs);
            OH_Drawing_Array* positions = OH_Drawing_GetRunPositions(run, 0, count);
            size = OH_Drawing_GetDrawingArraySize(positions);
            EXPECT_TRUE(positions != nullptr);
            for (size_t posIndex = 0; posIndex < size; posIndex++) {
                OH_Drawing_Point* pos = OH_Drawing_GetRunPositionsByIndex(positions, posIndex);
                EXPECT_TRUE(pos != nullptr);
                float x = 0.0;
                OH_Drawing_PointGetX(pos, &x);
                EXPECT_TRUE(x >= 0.0);
            }
            OH_Drawing_DestroyRunPositions(positions);
        }
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest005
 * @tc.desc: Test for get run typographic bounds.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest005, TestSize.Level1)
{
    text_ = "Hello 你好 World⌚😀👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测文本\n 123";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    ASSERT_GT(size, 0);
    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);

    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
    ASSERT_GT(runsSize, 0);

    std::vector<float> widthArr = {78.929932, 59.999939, 8.099991, 81.509903, 187.187500, 64.349945};
    std::vector<float> ascentArr = {-27.840000, -27.840000, -27.840000, -27.840000, -27.798166, -35.369999};
    std::vector<float> descentArr = {7.320000, 7.320000, 7.320000, 7.320000, 7.431193, 9.690001};
    for (int i = 0; i < runsSize; i++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, i);
        float ascent = 0.0;
        float descent = 0.0;
        float leading = 0.0;
        float width = OH_Drawing_GetRunTypographicBounds(run, &ascent, &descent, &leading);
        EXPECT_EQ(leading, 0);
        EXPECT_NEAR(ascent, ascentArr[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(descent, descentArr[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(width, widthArr[i], FLOAT_DATA_EPSILON);
        // branchCoverage
        OH_Drawing_GetRunTypographicBounds(run, nullptr, nullptr, nullptr);
        OH_Drawing_GetRunTypographicBounds(nullptr, &ascent, &descent, &leading);
    }

    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest006
 * @tc.desc: Test for invalid input parameters testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest006, TestSize.Level1)
{
    text_ = "Hello\t中国 World \n !@#%^&*){}[] 123456789 -= ,."
        " < >、/ Draclp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测文本\n 123";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        EXPECT_TRUE(runs != nullptr);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
            OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
            EXPECT_TRUE(run != nullptr);
            // Get the actual size of the run
            uint32_t count = OH_Drawing_GetRunGlyphCount(run);
            EXPECT_TRUE(count > 0);

            // -1 and -100 is invalid parameters
            OH_Drawing_Array* glyphs = OH_Drawing_GetRunGlyphs(run, -1, -100);
            EXPECT_TRUE(glyphs == nullptr);

            glyphs = OH_Drawing_GetRunGlyphs(run, 0, count);
            EXPECT_TRUE(glyphs != nullptr);

            // -1 is invalid parameter
            uint64_t glyphId = OH_Drawing_GetRunGlyphsByIndex(glyphs, -1);
            EXPECT_TRUE(glyphId == 0);

            // 1000 is greater than the actual size of run is also an invalid parameter
            glyphId = OH_Drawing_GetRunGlyphsByIndex(glyphs, 1000);
            EXPECT_TRUE(glyphId == 0);
            OH_Drawing_DestroyRunGlyphs(glyphs);
            glyphs = nullptr;
        }
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest007
 * @tc.desc: Test for invalid input parameters testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest007, TestSize.Level1)
{
    text_ = "Hello\t中国 World \n !@#%^&*){}[] 123456789 -= ,."
        "< >、/ Draclp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测文本\n 123";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        EXPECT_TRUE(runs != nullptr);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
            OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
            EXPECT_TRUE(run != nullptr);
            // Get the actual size of the run
            uint32_t count = OH_Drawing_GetRunGlyphCount(run);
            EXPECT_TRUE(count > 0);

            // -1 and -100 is invalid parameters
            OH_Drawing_Array* positions = OH_Drawing_GetRunPositions(run, -1, -100);
            EXPECT_TRUE(positions == nullptr);

            positions = OH_Drawing_GetRunPositions(run, 0, count);
            EXPECT_TRUE(positions != nullptr);
            OH_Drawing_DestroyRunPositions(positions);
            // -1 is invalid parameter
            OH_Drawing_Point* point = OH_Drawing_GetRunPositionsByIndex(positions, -1);
            EXPECT_TRUE(point == nullptr);

            // 1000 is greater than the actual size of run is also an invalid parameter
            point = OH_Drawing_GetRunPositionsByIndex(positions, 1000);
            EXPECT_TRUE(point == nullptr);
        }
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest008
 * @tc.desc: Test for special value input parameters testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest008, TestSize.Level1)
{
    text_ = "Hello\t中国 World \n !@#%^&*){}[] 123456789 -= ,."
        "< >、/ Draclp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测文本\n 123";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t index = 0; index < size; index++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index);
        EXPECT_TRUE(textLine != nullptr);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
        EXPECT_TRUE(runs != nullptr);
        size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
        for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
            OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
            EXPECT_TRUE(run != nullptr);
            // Get the actual size of the run
            uint32_t count = OH_Drawing_GetRunGlyphCount(run);
            EXPECT_TRUE(count > 0);
            // If both start and end are 0, all data of the current run
            OH_Drawing_Array* positions = OH_Drawing_GetRunPositions(run, 0, 0);
            EXPECT_TRUE(positions != nullptr);
            size_t positionSize = OH_Drawing_GetDrawingArraySize(positions);
            EXPECT_EQ(positionSize, count);
            OH_Drawing_DestroyRunPositions(positions);

            // If both start and end are 0, all data of the current run
            OH_Drawing_Array* glyphs = OH_Drawing_GetRunGlyphs(run, 0, 0);
            EXPECT_TRUE(glyphs != nullptr);
            size_t glyphSize = OH_Drawing_GetDrawingArraySize(glyphs);
            EXPECT_EQ(glyphSize, count);
            OH_Drawing_DestroyRunGlyphs(glyphs);

            // If both start and end are 0, all data of the current run
            OH_Drawing_Array* indices = OH_Drawing_GetRunStringIndices(run, 0, 0);
            EXPECT_TRUE(indices != nullptr);
            size_t indiceSize = OH_Drawing_GetDrawingArraySize(indices);
            EXPECT_EQ(indiceSize, count);
            OH_Drawing_DestroyRunStringIndices(indices);
        }
        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: OH_Drawing_RunTest009
 * @tc.desc: Test for the run of nullptr pointer testing.
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRunTest, OH_Drawing_RunTest009, TestSize.Level1)
{
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(nullptr, 0);
    EXPECT_TRUE(run == nullptr);
    uint32_t count = OH_Drawing_GetRunGlyphCount(nullptr);
    EXPECT_EQ(count, 0);
    uint64_t location = 0;
    uint64_t length = 0;
    OH_Drawing_GetRunStringRange(nullptr, &location, &length);
    EXPECT_EQ(location, 0);
    EXPECT_EQ(length, 0);
    EXPECT_TRUE(OH_Drawing_GetRunStringIndices(nullptr, 0, 0) == nullptr);
    EXPECT_TRUE(OH_Drawing_GetRunGlyphs(nullptr, 0, 0) == nullptr);
    EXPECT_TRUE(OH_Drawing_GetRunPositions(nullptr, 0, 0) == nullptr);
    EXPECT_TRUE(OH_Drawing_GetRunImageBounds(nullptr) == nullptr);
    OH_Drawing_RunPaint(nullptr, nullptr, 0, 0);
    float ascent = 0.0;
    float descent = 0.0;
    float leading = 0.0;
    float width = OH_Drawing_GetRunTypographicBounds(nullptr, &ascent, &descent, &leading);
    EXPECT_EQ(ascent, 0.0);
    EXPECT_EQ(descent, 0.0);
    EXPECT_EQ(leading, 0.0);
    EXPECT_EQ(width, 0.0);
}
}  // namespace OHOS