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

#include <limits>
#include <string>
#include <vector>

#include "array_mgr.h"
#include "drawing_text_typography.h"
#include "drawing_text_declaration.h"
#include "drawing_font_collection.h"
#include "drawing_text_line.h"
#include "drawing_text_run.h"
#include "drawing_text_font_descriptor.h"
#include "drawing_rect.h"
#include "drawing_error_code.h"
#include "gtest/gtest.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "rosen_text/typography_types.h"
#include "file_ex.h"
#include "font_parser.h"
#include "string_ex.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
// Test constants
const double DEFAULT_FONT_SIZE = 50.0;
const double LAYOUT_WIDTH = 300.0;

// Test text samples
const char* SHORT_TEXT = "Hello World";
const char* LONG_TEXT =
    "This is a long text that should span multiple lines when layout with constraints is applied. "
    "The text should wrap properly and return multiple ranges for each line.";

// Font file paths
constexpr const char* SYMBOL_FILE = "/system/fonts/HMSymbolVF.ttf";
constexpr const char* CJK_FILE = "/system/fonts/NotoSansCJK-Regular.ttc";
constexpr const char* NOTO_SANS_FILE = "/system/fonts/NotoSans[wdth,wght].ttf";
} // namespace

/**
 * @brief Test fixture for DestroyArray functionality
 *
 * This test class provides comprehensive testing for the OH_Drawing_ReleaseArrayBuffer API,
 * covering all 6 ObjectType types: STRING, TEXT_LINE, TEXT_RUN, DRAWING_RECT,
 * FONT_FULL_DESCRIPTOR, and TEXT_RANGE.
 */
class NdkDestroyArrayTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void CreateTypographyHandler();
    void CreateTypography();
    void AddText(const char* text);

private:
    OH_Drawing_TypographyCreate* fHandler{nullptr};
    OH_Drawing_Typography* fTypography{nullptr};
    OH_Drawing_TypographyStyle* fTypoStyle{nullptr};
    OH_Drawing_FontCollection* fFontCollection{nullptr};
};

void NdkDestroyArrayTest::SetUp()
{
    fTypoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(fTypoStyle, nullptr);
    OH_Drawing_SetTypographyTextFontSize(fTypoStyle, DEFAULT_FONT_SIZE);

    fFontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fFontCollection, nullptr);
}

void NdkDestroyArrayTest::TearDown()
{
    if (fHandler != nullptr) {
        OH_Drawing_DestroyTypographyHandler(fHandler);
        fHandler = nullptr;
    }
    if (fTypography != nullptr) {
        OH_Drawing_DestroyTypography(fTypography);
        fTypography = nullptr;
    }
    if (fTypoStyle != nullptr) {
        OH_Drawing_DestroyTypographyStyle(fTypoStyle);
        fTypoStyle = nullptr;
    }
    if (fFontCollection != nullptr) {
        OH_Drawing_DestroyFontCollection(fFontCollection);
        fFontCollection = nullptr;
    }
}

void NdkDestroyArrayTest::CreateTypographyHandler()
{
    OH_Drawing_SetTypographyTextWordBreakType(fTypoStyle, WORD_BREAK_TYPE_BREAK_WORD);
    fHandler = OH_Drawing_CreateTypographyHandler(fTypoStyle, fFontCollection);
    ASSERT_NE(fHandler, nullptr);
}

void NdkDestroyArrayTest::CreateTypography()
{
    fTypography = OH_Drawing_CreateTypography(fHandler);
    ASSERT_NE(fTypography, nullptr);
}

void NdkDestroyArrayTest::AddText(const char* text)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(fHandler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(fHandler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/**
 * @tc.name: DestroyArrayTest001
 * @tc.desc: Test destroying STRING array created by OH_Drawing_GetSystemFontFullNamesByType
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest001, TestSize.Level0)
{
    OH_Drawing_Array* fontList = OH_Drawing_GetSystemFontFullNamesByType(
        OH_Drawing_SystemFontType::ALL);
    ASSERT_NE(fontList, nullptr);

    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_GT(size, 0);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(fontList);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest002
 * @tc.desc: Test destroying empty STRING array
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest002, TestSize.Level0)
{
    OH_Drawing_Array* fontList = OH_Drawing_GetSystemFontFullNamesByType(
        static_cast<OH_Drawing_SystemFontType>(999)); // Invalid type to get empty list
    // May return nullptr or empty array depending on implementation
    if (fontList != nullptr) {
        OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(fontList);
        EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    }
}

/**
 * @tc.name: DestroyArrayTest003
 * @tc.desc: Test destroying TEXT_LINE array with single line
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest003, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, LAYOUT_WIDTH);

    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(fTypography);
    ASSERT_NE(textLines, nullptr);

    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_GT(size, 0);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(textLines);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest004
 * @tc.desc: Test destroying TEXT_LINE array with multiple lines
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest004, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(LONG_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, 100.0); // Small width to force multiple lines

    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(fTypography);
    ASSERT_NE(textLines, nullptr);

    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    EXPECT_GT(size, 1); // Should have multiple lines

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(textLines);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest005
 * @tc.desc: Test destroying TEXT_RUN (string indices) array
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest005, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, LAYOUT_WIDTH);

    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(fTypography);
    ASSERT_NE(textLines, nullptr);

    size_t lineCount = OH_Drawing_GetDrawingArraySize(textLines);
    ASSERT_GT(lineCount, 0);

    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    ASSERT_NE(textLine, nullptr);

    // Get glyph runs
    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    ASSERT_NE(runs, nullptr);

    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
    ASSERT_GT(runsSize, 0);

    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, 0);
    ASSERT_NE(run, nullptr);

    uint32_t glyphCount = OH_Drawing_GetRunGlyphCount(run);
    ASSERT_GT(glyphCount, 0);

    // Get string indices array
    OH_Drawing_Array* stringIndices = OH_Drawing_GetRunStringIndices(run, 0, glyphCount);
    ASSERT_NE(stringIndices, nullptr);

    size_t indicesSize = OH_Drawing_GetDrawingArraySize(stringIndices);
    EXPECT_GT(indicesSize, 0);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(stringIndices);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);

    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/**
 * @tc.name: DestroyArrayTest006
 * @tc.desc: Test destroying empty TEXT_RUN array
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest006, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, LAYOUT_WIDTH);

    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(fTypography);
    ASSERT_NE(textLines, nullptr);

    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    ASSERT_NE(textLine, nullptr);

    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    ASSERT_NE(runs, nullptr);

    OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, 0);
    ASSERT_NE(run, nullptr);

    // Get string indices with 0 count
    OH_Drawing_Array* stringIndices = OH_Drawing_GetRunStringIndices(run, 0, 0);
    ASSERT_NE(stringIndices, nullptr);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(stringIndices);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);

    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
}

/**
 * @tc.name: DestroyArrayTest007
 * @tc.desc: Test destroying DRAWING_RECT array with single element
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest007, TestSize.Level0)
{
    const size_t arraySize = 1;
    OH_Drawing_Array* rectArray = OH_Drawing_RectCreateArray(arraySize);
    ASSERT_NE(rectArray, nullptr);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(rectArray);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest008
 * @tc.desc: Test destroying DRAWING_RECT array with multiple elements
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest008, TestSize.Level0)
{
    const size_t arraySize = 5;
    OH_Drawing_Array* rectArray = OH_Drawing_RectCreateArray(arraySize);
    ASSERT_NE(rectArray, nullptr);

    size_t size = 0;
    OH_Drawing_ErrorCode sizeErrorCode = OH_Drawing_RectGetArraySize(rectArray, &size);
    EXPECT_EQ(sizeErrorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(size, arraySize);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(rectArray);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest009
 * @tc.desc: Test destroying large DRAWING_RECT array
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest009, TestSize.Level0)
{
    const size_t arraySize = 100;
    OH_Drawing_Array* rectArray = OH_Drawing_RectCreateArray(arraySize);
    ASSERT_NE(rectArray, nullptr);

    size_t size = 0;
    OH_Drawing_ErrorCode sizeErrorCode = OH_Drawing_RectGetArraySize(rectArray, &size);
    EXPECT_EQ(sizeErrorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(size, arraySize);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(rectArray);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest010
 * @tc.desc: Test destroying FONT_FULL_DESCRIPTOR array from file path
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest010, TestSize.Level0)
{
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(NOTO_SANS_FILE);
    ASSERT_NE(fontFullDescArr, nullptr);

    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_GT(size, 0);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(fontFullDescArr);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest011
 * @tc.desc: Test destroying FONT_FULL_DESCRIPTOR array from stream
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest011, TestSize.Level0)
{
    std::vector<char> content;
    EXPECT_TRUE(OHOS::LoadBufferFromFile(SYMBOL_FILE, content));
    ASSERT_GT(content.size(), 0);

    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(
        content.data(), content.size());
    ASSERT_NE(fontFullDescArr, nullptr);

    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_GT(size, 0);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(fontFullDescArr);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest012
 * @tc.desc: Test destroying FONT_FULL_DESCRIPTOR array with multiple descriptors (TTC file)
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest012, TestSize.Level0)
{
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(CJK_FILE);
    ASSERT_NE(fontFullDescArr, nullptr);

    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    // CJK ttc has 10 ttf fonts
    EXPECT_GT(size, 1);

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(fontFullDescArr);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest013
 * @tc.desc: Test destroying TEXT_RANGE array from LayoutWithConstraints
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest013, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;
    OH_Drawing_RectSize size;
    size.width = LAYOUT_WIDTH;
    size.height = 200.0;

    OH_Drawing_TypographyLayoutWithConstraintsWithBuffer(fTypography, size, &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_GT(rangeCount, 0);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(rangeArray);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest014
 * @tc.desc: Test destroying TEXT_RANGE array with multiple ranges
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest014, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(LONG_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;
    OH_Drawing_RectSize size;
    size.width = 100.0;  // Small width to force multiple lines
    size.height = 50.0;   // Small height to force multiple ranges

    OH_Drawing_TypographyLayoutWithConstraintsWithBuffer(fTypography, size, &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(rangeArray);
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: DestroyArrayTest016
 * @tc.desc: Test destroying array with invalid type (INVALID ObjectType)
 * @tc.type: FUNC
 */
HWTEST_F(NdkDestroyArrayTest, DestroyArrayTest016, TestSize.Level0)
{
    // Create a manually constructed ObjectArray with INVALID type
    // This simulates what would happen if an array was corrupted
    ObjectArray* invalidArray = new ObjectArray();
    invalidArray->addr = new char[100]; // Allocate some memory
    invalidArray->num = 1;
    invalidArray->type = ObjectType::INVALID;

    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(
        reinterpret_cast<OH_Drawing_Array*>(invalidArray));

    // Should return error for invalid type
    EXPECT_EQ(errorCode, OH_Drawing_ErrorCode::OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    // Clean up manually since DestroyArray won't do it for INVALID type
    if (invalidArray->addr != nullptr) {
        delete[] static_cast<char*>(invalidArray->addr);
    }
    delete invalidArray;
}
} // namespace OHOS
