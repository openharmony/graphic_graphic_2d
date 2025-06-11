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
#include "drawing_rect.h"
#include "drawing_text_line.h"
#include "drawing_text_run.h"
#include "drawing_types.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "string"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
}

class NdkRunTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
#ifdef RS_ENABLE_VK
        Rosen::RsVulkanContext::SetRecyclable(false);
#endif
    }
    static void TearDownTestCase() {}
    void SetUp() override {};
    void TearDown() override;
    void PrepareCreateTextLine();
    void PrepareCreateTextLineForGlyphDrawing();

    OH_Drawing_TypographyStyle* typeStyle_ = nullptr;
    OH_Drawing_TextStyle* txtStyle_ = nullptr;
    OH_Drawing_FontCollection* fontCollection_ = nullptr;
    OH_Drawing_TypographyCreate* handler_ = nullptr;
    OH_Drawing_Typography* typography_ = nullptr;
    OH_Drawing_Bitmap* cBitmap_ = nullptr;
    OH_Drawing_Canvas* canvas_ = nullptr;
    OH_Drawing_Array* runs_ = nullptr;
    OH_Drawing_Array* textLines_ = nullptr;
    std::string text_;
};

void NdkRunTest::PrepareCreateTextLine()
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

void NdkRunTest::PrepareCreateTextLineForGlyphDrawing()
{
    double maxWidth = 1200.0;
    typeStyle_ = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typeStyle_, nullptr);
    txtStyle_ = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle_, nullptr);
    fontCollection_ = OH_Drawing_CreateFontCollection();
    EXPECT_NE(fontCollection_, nullptr);
    handler_ = OH_Drawing_CreateTypographyHandler(typeStyle_, fontCollection_);
    EXPECT_NE(handler_, nullptr);
    double fontSize = 100;
    OH_Drawing_SetTextStyleFontSize(txtStyle_, fontSize);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text_.c_str());
    typography_ = OH_Drawing_CreateTypography(handler_);
    EXPECT_NE(typography_, nullptr);
    OH_Drawing_TypographyLayout(typography_, maxWidth);
    textLines_ = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_NE(textLines_, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines_);
    EXPECT_TRUE(size != 0);
    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines_, 0);
    EXPECT_NE(textLine, nullptr);
    runs_ = OH_Drawing_TextLineGetGlyphRuns(textLine);
    EXPECT_NE(runs_, nullptr);
    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs_);
    EXPECT_TRUE(runsSize != 0);
}

void NdkRunTest::TearDown()
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
    if (textLines_ != nullptr) {
        OH_Drawing_DestroyTextLines(textLines_);
        textLines_ = nullptr;
    }
    if (runs_ != nullptr) {
        OH_Drawing_DestroyRuns(runs_);
        runs_ = nullptr;
    }
}

/*
 * @tc.name: RunTest001
 * @tc.desc: Test for get run by index.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest001, TestSize.Level1)
{
    text_ = "Hello 你好 World";
    PrepareCreateTextLine();
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    EXPECT_TRUE(textLines != nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    ASSERT_GT(size, 0);
    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);

    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
    ASSERT_GT(runsSize, 0);

    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(nullptr, 0);
    EXPECT_EQ(run, nullptr);
    run = OH_Drawing_GetRunByIndex(runs, -1);
    EXPECT_EQ(run, nullptr);
    run = OH_Drawing_GetRunByIndex(runs, runsSize);
    EXPECT_EQ(run, nullptr);
    run = OH_Drawing_GetRunByIndex(runs, 0);
    uint32_t count = OH_Drawing_GetRunGlyphCount(run);
    EXPECT_EQ(count, 6);

    // branchCoverage
    auto nullCount = OH_Drawing_GetDrawingArraySize(nullptr);
    EXPECT_EQ(nullCount, 0);
    OH_Drawing_DestroyRuns(nullptr);

    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: RunTest002
 * @tc.desc: Test for get run glyph count.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest002, TestSize.Level1)
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
    std::vector<int32_t> countArr = {6, 2, 1, 5, 5, 5};
    for (int i = 0; i < runsSize; i++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, i);
        uint32_t count = OH_Drawing_GetRunGlyphCount(run);
        EXPECT_EQ(count, countArr[i]);
    }

    // branchCoverage
    OH_Drawing_GetRunGlyphCount(nullptr);

    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: RunTest003
 * @tc.desc: Test for get glyph index in paragraph.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest003, TestSize.Level1)
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
    OH_Drawing_Run* run1 = OH_Drawing_GetRunByIndex(runs, 1);
    uint32_t count = OH_Drawing_GetRunGlyphCount(run1);
    auto glyphIndexArr = OH_Drawing_GetRunStringIndices(run1, 0, count);
    auto glyphIndexArrSize = OH_Drawing_GetDrawingArraySize(glyphIndexArr);
    std::vector<int32_t> indexndexArr = {6, 7};
    for (int j = 0; j < glyphIndexArrSize; j++) {
        auto glyphIndex = OH_Drawing_GetRunStringIndicesByIndex(glyphIndexArr, j);
        EXPECT_EQ(glyphIndex, indexndexArr[j]);
    }
    // branchCoverage
    OH_Drawing_GetRunStringIndices(nullptr, -1, -1);
    OH_Drawing_GetRunStringIndicesByIndex(glyphIndexArr, glyphIndexArrSize);
    OH_Drawing_DestroyRunStringIndices(glyphIndexArr);
    OH_Drawing_GetRunGlyphCount(nullptr);
    
     
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: RunTest004
 * @tc.desc: Test for get run string range.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest004, TestSize.Level1)
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
    
    std::vector<int32_t> locationArr = {0, 6, 8, 9, 14, 19};
    std::vector<int32_t> lengthArr = {6, 2, 1, 5, 5, 5};
    for (int i = 0; i < runsSize; i++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, i);
        uint64_t location = 0;
        uint64_t length = 0;
        OH_Drawing_GetRunStringRange(run, &location, &length);
        EXPECT_EQ(location, locationArr[i]);
        EXPECT_EQ(length, lengthArr[i]);
        // branchCoverage
        OH_Drawing_GetRunStringRange(run, nullptr, nullptr);
    }
    
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: RunTest005
 * @tc.desc: Test for get run typographic bounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest005, TestSize.Level1)
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
 * @tc.name: RunTest006
 * @tc.desc: Test for get run image bounds.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest006, TestSize.Level1)
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
    
    std::vector<float> leftArr = {2.0, 78.929932, 147.029861, 147.029861, 228.539764, 417.727264};
    std::vector<float> topArr = {1.0, 3.0, 0.0, 1.0, 8.0, 3.0};
    std::vector<float> bottomArr = {25.0, 32.0, 0.0, 25.0, 44.0, 33.0};
    std::vector<float> rightArr = {70.099960, 137.929901, 147.029861, 226.329788, 416.289764, 484.857208};
    for (int i = 0; i < runsSize; i++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, i);
        OH_Drawing_Rect* bounds = OH_Drawing_GetRunImageBounds(run);

        EXPECT_NEAR(OH_Drawing_RectGetLeft(bounds), leftArr[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(OH_Drawing_RectGetTop(bounds), topArr[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(OH_Drawing_RectGetBottom(bounds), bottomArr[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(OH_Drawing_RectGetRight(bounds), rightArr[i], FLOAT_DATA_EPSILON);
        OH_Drawing_DestroyRunImageBounds(bounds);
    }
    // branchCoverage
    OH_Drawing_GetRunImageBounds(nullptr);
      
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: RunTest007
 * @tc.desc: Test for get run glyphs.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest007, TestSize.Level1)
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
    
    std::vector<int32_t> glyphSizeArr = {6, 2, 1, 5, 5, 5};
    std::vector<int32_t> glyphArr = {7824, 10413};
    for (int i = 0; i < runsSize; i++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, i);
        uint32_t count = OH_Drawing_GetRunGlyphCount(run);
        OH_Drawing_Array* glyphs = OH_Drawing_GetRunGlyphs(run, 0, count);
        size_t glyphSize = OH_Drawing_GetDrawingArraySize(glyphs);
        EXPECT_EQ(glyphSize, glyphSizeArr[i]);
        if (i == 1) {
            for (int j = 0; j < glyphSize; j++) {
                EXPECT_EQ(OH_Drawing_GetRunGlyphsByIndex(glyphs, j), glyphArr[j]);
            }
        }
        // branchCoverage
        OH_Drawing_GetRunGlyphsByIndex(glyphs, glyphSize);

        OH_Drawing_DestroyRunGlyphs(glyphs);
    }
    // branchCoverage
    OH_Drawing_GetRunGlyphs(nullptr, -1, -1);
       
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: RunTest008
 * @tc.desc: Test for get run positions.
 * @tc.type: FUNC
 */
 HWTEST_F(NdkRunTest, RunTest008, TestSize.Level1)
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
    
    std::vector<int32_t> positionSizeArr = {6, 2, 1, 5, 5, 5};
    std::vector<float> posXArr = {0, 29.999969};
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, 1);
    uint32_t count = OH_Drawing_GetRunGlyphCount(run);
    OH_Drawing_Array* positions = OH_Drawing_GetRunPositions(run, 0, count);
    EXPECT_TRUE(positions != nullptr);
    size_t positionSize = OH_Drawing_GetDrawingArraySize(positions);
    for (size_t posIndex = 0; posIndex < positionSize; posIndex++) {
        OH_Drawing_Point* pos = OH_Drawing_GetRunPositionsByIndex(positions, posIndex);
        EXPECT_TRUE(pos != nullptr);
        float x = 0.0;
        OH_Drawing_PointGetX(pos, &x);
        EXPECT_NEAR(x, posXArr[posIndex], FLOAT_DATA_EPSILON);
    }
    EXPECT_EQ(positionSize, 2);
    
    // branchCoverage
    OH_Drawing_GetRunPositionsByIndex(positions, positionSize);
    OH_Drawing_DestroyRunPositions(positions);
    OH_Drawing_GetRunPositions(nullptr, -1, -1);
        
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/*
 * @tc.name: RunTest009
 * @tc.desc: Test for special value input parameters testing.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest009, TestSize.Level1)
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
 * @tc.name: RunTest010
 * @tc.desc: Test for the run of nullptr pointer testing.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, RunTest010, TestSize.Level1)
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


/*
 * @tc.name: GlyDrawingTest001
 * @tc.desc: Test for the glyph drawing of nullptr testing.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_GetRunFont(nullptr);
    EXPECT_TRUE(font == nullptr);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(nullptr);
    EXPECT_EQ(direction, TEXT_DIRECTION_LTR);

    OH_Drawing_Array* advances = OH_Drawing_GetRunGlyphAdvances(nullptr, 0, 0);
    EXPECT_TRUE(advances == nullptr);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 0);
    EXPECT_TRUE(advance == nullptr);

    OH_Drawing_DestroyRunGlyphAdvances(advances);
}

/*
 * @tc.name: GlyDrawingTest002
 * @tc.desc: Test for the glyph drawing of the English text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest002, TestSize.Level1)
{
    text_ = "Hello你好";
    PrepareCreateTextLineForGlyphDrawing();
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs_, 0);
    EXPECT_NE(run, nullptr);

    OH_Drawing_Font* font = OH_Drawing_GetRunFont(run);
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -105.599998, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -92.799995, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 24.399999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom,  27.100000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 50.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 248.600006, FLOAT_DATA_EPSILON);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(run);
    EXPECT_EQ(direction, TEXT_DIRECTION_LTR);

    OH_Drawing_Array* advances = OH_Drawing_GetRunGlyphAdvances(run, 0, 0);
    EXPECT_NE(advances, nullptr);
    size_t advancesSize = OH_Drawing_GetDrawingArraySize(advances);
    EXPECT_TRUE(advancesSize != 0);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 0);
    EXPECT_NE(advance, nullptr);
    float x = 0.0;
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 74.499923, FLOAT_DATA_EPSILON);
    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 1);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 54.699951, FLOAT_DATA_EPSILON);
    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 10);
    EXPECT_TRUE(advance == nullptr);

    OH_Drawing_DestroyRunGlyphAdvances(advances);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GlyDrawingTest003
 * @tc.desc: Test for the glyph drawing of the emoji text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest003, TestSize.Level1)
{
    text_ = "⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭";
    PrepareCreateTextLineForGlyphDrawing();
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs_, 0);
    EXPECT_NE(run, nullptr);

    OH_Drawing_Font* font = OH_Drawing_GetRunFont(run);
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -92.660552, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -92.660552, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 24.770643, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom,  24.770643, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 124.511718, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 124.7706451, FLOAT_DATA_EPSILON);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(run);
    EXPECT_EQ(direction, TEXT_DIRECTION_LTR);

    OH_Drawing_Array* advancesNullTest1 = OH_Drawing_GetRunGlyphAdvances(run, -10, -20);
    EXPECT_TRUE(advancesNullTest1 == nullptr);
    OH_Drawing_DestroyRunGlyphAdvances(advancesNullTest1);

    OH_Drawing_Array* advancesNullTest2 = OH_Drawing_GetRunGlyphAdvances(run, 100, 20);
    EXPECT_TRUE(advancesNullTest2 == nullptr);
    OH_Drawing_DestroyRunGlyphAdvances(advancesNullTest2);

    OH_Drawing_Array* advances = OH_Drawing_GetRunGlyphAdvances(run, 1, 2);
    EXPECT_NE(advances, nullptr);
    size_t advancesSize = OH_Drawing_GetDrawingArraySize(advances);
    EXPECT_EQ(advancesSize, 2);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 0);
    EXPECT_NE(advance, nullptr);
    float x = 0.0;
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 124.765625, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyRunGlyphAdvances(advances);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GlyDrawingTest004
 * @tc.desc: Test for the glyph drawing of the RTL text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest004, TestSize.Level1)
{
    text_ = "مرحبا";
    PrepareCreateTextLineForGlyphDrawing();
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs_, 0);
    EXPECT_NE(run, nullptr);

    OH_Drawing_Font* font = OH_Drawing_GetRunFont(run);
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -139.800003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -117.900001, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 32.300003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 39.299999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 70.300003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 160.900009, FLOAT_DATA_EPSILON);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(run);
    EXPECT_EQ(direction, TEXT_DIRECTION_RTL);

    OH_Drawing_Array* advances = OH_Drawing_GetRunGlyphAdvances(run, 1, 2);
    EXPECT_NE(advances, nullptr);
    size_t advancesSize = OH_Drawing_GetDrawingArraySize(advances);
    EXPECT_EQ(advancesSize, 2);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 0);
    EXPECT_NE(advance, nullptr);
    float x = 0.0;
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 32.999969, FLOAT_DATA_EPSILON);
    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 1);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 64.599945, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyRunGlyphAdvances(advances);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GlyDrawingTest005
 * @tc.desc: Test for the glyph drawing of the Arabic and Latin ligature text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest005, TestSize.Level1)
{
    text_ = "\u0644\u0627Hello你好连字测试\u0066\u0069";
    PrepareCreateTextLineForGlyphDrawing();
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs_, 0);
    EXPECT_NE(run, nullptr);

    OH_Drawing_Font* font = OH_Drawing_GetRunFont(run);
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -139.800003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -117.900001, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 32.300003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 39.299999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 70.300003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 160.900009, FLOAT_DATA_EPSILON);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(run);
    EXPECT_EQ(direction, TEXT_DIRECTION_RTL);

    OH_Drawing_Array* advancesTest1 = OH_Drawing_GetRunGlyphAdvances(run, 0, 1);
    EXPECT_NE(advancesTest1, nullptr);
    size_t advancesSize = OH_Drawing_GetDrawingArraySize(advancesTest1);
    EXPECT_EQ(advancesSize, 1);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest1, 0);
    EXPECT_NE(advance, nullptr);
    float x = 0.0;
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 48.999954, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyRunGlyphAdvances(advancesTest1);

    run = OH_Drawing_GetRunByIndex(runs_, 3);
    OH_Drawing_Array* advancesTest2 = OH_Drawing_GetRunGlyphAdvances(run, 0, 2);
    EXPECT_NE(advancesTest2, nullptr);
    advancesSize = OH_Drawing_GetDrawingArraySize(advancesTest2);
    EXPECT_EQ(advancesSize, 1);

    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest2, 0);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 57.399948, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyRunGlyphAdvances(advancesTest2);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GlyDrawingTest006
 * @tc.desc: Test for the glyph drawing of the Sanskrit combination text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest006, TestSize.Level1)
{
    text_ = "क्Hello你好天成文测试का";
    PrepareCreateTextLineForGlyphDrawing();
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs_, 0);
    EXPECT_NE(run, nullptr);

    OH_Drawing_Font* font = OH_Drawing_GetRunFont(run);
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -134.699997, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -89.600006, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 40.799999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 39.399998, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 55.500000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 201.300003, FLOAT_DATA_EPSILON);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(run);
    EXPECT_EQ(direction, TEXT_DIRECTION_LTR);

    OH_Drawing_Array* advancesTest1 = OH_Drawing_GetRunGlyphAdvances(run, 0, 2);
    EXPECT_NE(advancesTest1, nullptr);
    size_t advancesSize = OH_Drawing_GetDrawingArraySize(advancesTest1);
    EXPECT_EQ(advancesSize, 2);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest1, 0);
    EXPECT_NE(advance, nullptr);
    float x = 0.0;
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 76.199921, FLOAT_DATA_EPSILON);

    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest1, 1);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 0.000000, FLOAT_DATA_EPSILON);

    run = OH_Drawing_GetRunByIndex(runs_, 3);
    OH_Drawing_Array* advancesTest2 = OH_Drawing_GetRunGlyphAdvances(run, 0, 2);
    EXPECT_NE(advancesTest2, nullptr);
    advancesSize = OH_Drawing_GetDrawingArraySize(advancesTest2);
    EXPECT_EQ(advancesSize, 2);

    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest2, 0);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 76.199921, FLOAT_DATA_EPSILON);

    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest2, 1);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 25.899979, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyRunGlyphAdvances(advancesTest2);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GlyDrawingTest007
 * @tc.desc: Test for the glyph drawing of the Sanskrit combination text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest007, TestSize.Level1)
{
    text_ = "གྷHello你好藏文测试སྒྲ";
    PrepareCreateTextLineForGlyphDrawing();
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs_, 0);
    EXPECT_NE(run, nullptr);
    OH_Drawing_Font* font = OH_Drawing_GetRunFont(run);
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -110.679001, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -115.813995, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 106.570999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 78.605003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 49.848999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 206.347992, FLOAT_DATA_EPSILON);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(run);
    EXPECT_EQ(direction, TEXT_DIRECTION_LTR);

    OH_Drawing_Array* advancesTest1 = OH_Drawing_GetRunGlyphAdvances(run, 0, 1);
    EXPECT_NE(advancesTest1, nullptr);
    size_t advancesSize = OH_Drawing_GetDrawingArraySize(advancesTest1);
    EXPECT_EQ(advancesSize, 1);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest1, 0);
    EXPECT_NE(advance, nullptr);
    float x = 0.0;
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 56.405991, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyRunGlyphAdvances(advancesTest1);

    run = OH_Drawing_GetRunByIndex(runs_, 3);
    OH_Drawing_Array* advancesTest2 = OH_Drawing_GetRunGlyphAdvances(run, 0, 1);
    EXPECT_NE(advancesTest2, nullptr);
    advancesSize = OH_Drawing_GetDrawingArraySize(advancesTest2);
    EXPECT_EQ(advancesSize, 1);
    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advancesTest2, 0);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 53.324997, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyRunGlyphAdvances(advancesTest2);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GlyDrawingTest008
 * @tc.desc: Test for the glyph drawing of the Thai text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkRunTest, GlyDrawingTest008, TestSize.Level1)
{
    text_ = "\u0E01\u0E33\u0E41\u0E01Hello你好泰文测试";
    PrepareCreateTextLineForGlyphDrawing();
    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs_, 0);
    EXPECT_NE(run, nullptr);

    OH_Drawing_Font* font = OH_Drawing_GetRunFont(run);
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -100.899994, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -106.099998, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 45.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 43.299999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0.0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 55.800003, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 165.100006, FLOAT_DATA_EPSILON);

    OH_Drawing_TextDirection direction = OH_Drawing_GetRunTextDirection(run);
    EXPECT_EQ(direction, TEXT_DIRECTION_LTR);

    OH_Drawing_Array* advances = OH_Drawing_GetRunGlyphAdvances(run, 0, 5);
    EXPECT_NE(advances, nullptr);
    size_t advancesSize = OH_Drawing_GetDrawingArraySize(advances);
    EXPECT_EQ(advancesSize, 5);

    OH_Drawing_Point* advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 0);
    EXPECT_NE(advance, nullptr);
    float x = 0.0;
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 59.999939, FLOAT_DATA_EPSILON);

    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 1);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 0.000000, FLOAT_DATA_EPSILON);

    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 2);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 40.599960, FLOAT_DATA_EPSILON);
        advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 3);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 56.799942, FLOAT_DATA_EPSILON);

    advance = OH_Drawing_GetRunGlyphAdvanceByIndex(advances, 4);
    EXPECT_NE(advance, nullptr);
    OH_Drawing_PointGetX(advance, &x);
    EXPECT_NEAR(x, 59.999939, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyRunGlyphAdvances(advances);
    OH_Drawing_FontDestroy(font);
}
}  // namespace OHOS