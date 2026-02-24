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

#include "drawing_text_typography.h"
#include "drawing_font_collection.h"
#include "drawing_error_code.h"
#include "drawing_text_font_descriptor.h"
#include "gtest/gtest.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "rosen_text/typography_types.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
// Test constants
const double DEFAULT_FONT_SIZE = 50.0;
const double LAYOUT_WIDTH_SMALL = 100.0;
const double LAYOUT_WIDTH_MEDIUM = 300.0;
const double LAYOUT_WIDTH_LARGE = 800.0;
const double LAYOUT_HEIGHT_SMALL = 100.0;
const double LAYOUT_HEIGHT_MEDIUM = 200.0;
const double LAYOUT_HEIGHT_LARGE = 1000.0;
const double NEGATIVE_LAYOUT_LENGTH = -10.0;
const double DEFAULT_SINGLE_LINE_HEIGHT = 59.0;
const double FONT_HEIGHT_SCALE = 1.5;

// Test text samples
const char* SHORT_TEXT = "Hello";
const char* MEDIUM_TEXT = "Hello World, this is a test.";
const char* LONG_TEXT =
    "This is a long text that should span multiple lines when layout with constraints is applied. "
    "The text should wrap properly and return multiple ranges for each line.";
const char* CHINESE_TEXT = "你好世界测试文本";
const char* MIXED_TEXT = "你好世界测试文本The text should wrap properly and return multiple ranges for each line. 世界";
const char* EMPTY_TEXT = "";
const char* SINGLE_CHAR_TEXT = "H";

// Multi-language text samples
const char* ARABIC_TEXT = "مرحبا بالعالم";  // Arabic text
const char* JAPANESE_TEXT = "こんにちは世界";  // Japanese text
const char* KOREAN_TEXT = "안녕하세요 세계";  // Korean text
const char* THAI_TEXT = "สวัสดีชาวโลก";  // Thai text
const char* EMOJI_TEXT = "Hello 😀🌍🎉 World"; // Emoji text

// BreakStrategy test text samples
const char* SHORT_ENGLISH_TEXT = "Hello World";
const char* MEDIUM_ENGLISH_TEXT = "The quick brown fox jumps over the lazy dog.";
const char* LONG_ENGLISH_TEXT =
    "This is a comprehensive test of the break strategy functionality. "
    "The text should wrap properly according to the selected break strategy. "
    "Different strategies will produce different line breaking patterns.";
const char* MIXED_CH_ENGLISH_TEXT = "你好世界Hello世界测试Test文本";
} // namespace

/**
 * @brief Test fixture for Typography LayoutWithConstraints functionality
 *
 * This test class provides comprehensive testing for the LayoutWithConstraints API,
 * including parameter validation, basic functionality, edge cases, and multi-language support.
 */
class NdkTypographyLayoutConstraintsTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void CreateTypographyHandler();
    void CreateTypography();
    void AddText(const char* text);
    void AddTextWithStyle(const char* text, double fontSize);
    void CleanupTypography();
    void ResetTypoStyleWithFontSize(double fontSize);
    OH_Drawing_Typography* GetTypography() { return fTypography; }

    // Helper methods for LayoutWithConstraints testing
    OH_Drawing_RectSize CallLayoutWithConstraints(double width, double height,
                                                   OH_Drawing_Array** rangeArray,
                                                   size_t* rangeCount);

private:
    OH_Drawing_TypographyCreate* fHandler{nullptr};
    OH_Drawing_Typography* fTypography{nullptr};
    OH_Drawing_TypographyStyle* fTypoStyle{nullptr};
    OH_Drawing_TextStyle* fTextStyle{nullptr};
    OH_Drawing_FontCollection* fFontCollection{nullptr};
};

void NdkTypographyLayoutConstraintsTest::SetUp()
{
    fTypoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(fTypoStyle, nullptr);
    OH_Drawing_SetTypographyTextFontSize(fTypoStyle, DEFAULT_FONT_SIZE);
    fTextStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(fTextStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(fTextStyle, DEFAULT_FONT_SIZE);

    fFontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fFontCollection, nullptr);
}

void NdkTypographyLayoutConstraintsTest::TearDown()
{
    CleanupTypography();

    if (fTypoStyle != nullptr) {
        OH_Drawing_DestroyTypographyStyle(fTypoStyle);
        fTypoStyle = nullptr;
    }

    if (fFontCollection != nullptr) {
        OH_Drawing_DestroyFontCollection(fFontCollection);
        fFontCollection = nullptr;
    }
}

void NdkTypographyLayoutConstraintsTest::CreateTypographyHandler()
{
    OH_Drawing_SetTypographyTextWordBreakType(fTypoStyle, WORD_BREAK_TYPE_BREAK_WORD);
    fHandler = OH_Drawing_CreateTypographyHandler(fTypoStyle, fFontCollection);
    ASSERT_NE(fHandler, nullptr);
}

void NdkTypographyLayoutConstraintsTest::CreateTypography()
{
    fTypography = OH_Drawing_CreateTypography(fHandler);
    ASSERT_NE(fTypography, nullptr);
}

void NdkTypographyLayoutConstraintsTest::AddText(const char* text)
{
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, fTextStyle);
    OH_Drawing_TypographyHandlerAddText(fHandler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(fHandler);
    OH_Drawing_DestroyTextStyle(fTextStyle);
}

void NdkTypographyLayoutConstraintsTest::AddTextWithStyle(const char* text, double fontSize)
{
    OH_Drawing_SetTextStyleFontSize(fTextStyle, fontSize);
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, fTextStyle);
    OH_Drawing_TypographyHandlerAddText(fHandler, text);
    OH_Drawing_TypographyHandlerPopTextStyle(fHandler);
    OH_Drawing_DestroyTextStyle(fTextStyle);
}

void NdkTypographyLayoutConstraintsTest::CleanupTypography()
{
    if (fHandler != nullptr) {
        OH_Drawing_DestroyTypographyHandler(fHandler);
        fHandler = nullptr;
    }
    if (fTypography != nullptr) {
        OH_Drawing_DestroyTypography(fTypography);
        fTypography = nullptr;
    }
}

void NdkTypographyLayoutConstraintsTest::ResetTypoStyleWithFontSize(double fontSize)
{
    if (fTypoStyle != nullptr) {
        OH_Drawing_DestroyTypographyStyle(fTypoStyle);
        fTypoStyle = nullptr;
    }
    fTypoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextFontSize(fTypoStyle, fontSize);
}

OH_Drawing_RectSize NdkTypographyLayoutConstraintsTest::CallLayoutWithConstraints(
    double width, double height, OH_Drawing_Array** rangeArray, size_t* rangeCount)
{
    OH_Drawing_RectSize size;
    size.width = width;
    size.height = height;
    return OH_Drawing_TypographyLayoutWithConstraintsWithBuffer(fTypography, size, rangeArray, rangeCount);
}

void VerifyRange(size_t start, size_t end, OH_Drawing_Range* range)
{
    ASSERT_NE(range, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(range), start);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(range), end);
}

void VerifyAllRanges(const std::vector<std::pair<size_t, size_t>>& expectedRanges,
    OH_Drawing_Array* rangeArray, size_t rangeCount)
{
    ASSERT_NE(rangeArray, nullptr);
    ASSERT_EQ(expectedRanges.size(), rangeCount);

    for (size_t i = 0; i < rangeCount; ++i) {
        OH_Drawing_Range* range = OH_Drawing_GetRangeByArrayIndex(rangeArray, i);
        VerifyRange(expectedRanges[i].first, expectedRanges[i].second, range);
    }
}

void VerifyRectSize(double expectedWidth, double expectedHeight, OH_Drawing_RectSize rectSize)
{
    EXPECT_DOUBLE_EQ(rectSize.width, expectedWidth);
    EXPECT_DOUBLE_EQ(rectSize.height, expectedHeight);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest001
 * @tc.desc: Test LayoutWithConstraints with null typography pointer
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest001, TestSize.Level0)
{
    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;
    OH_Drawing_RectSize size;

    size.width = LAYOUT_WIDTH_MEDIUM;
    size.height = LAYOUT_HEIGHT_MEDIUM;

    OH_Drawing_RectSize result =
        OH_Drawing_TypographyLayoutWithConstraintsWithBuffer(nullptr, size, &rangeArray, &rangeCount);
    VerifyRectSize(0, 0, result);
    EXPECT_EQ(rangeCount, 0);
    EXPECT_EQ(rangeArray, nullptr);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest002
 * @tc.desc: Test LayoutWithConstraints with null rangeArray pointer
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest002, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    size_t rangeCount = 0;
    OH_Drawing_RectSize size;

    size.width = LAYOUT_WIDTH_MEDIUM;
    size.height = LAYOUT_HEIGHT_MEDIUM;

    OH_Drawing_RectSize result =
        OH_Drawing_TypographyLayoutWithConstraintsWithBuffer(fTypography, size, nullptr, &rangeCount);
    EXPECT_EQ(rangeCount, 0);
    VerifyRectSize(0, 0, result);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest003
 * @tc.desc: Test LayoutWithConstraints with null rangeCount pointer
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest003, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    OH_Drawing_RectSize size;

    size.width = LAYOUT_WIDTH_MEDIUM;
    size.height = LAYOUT_HEIGHT_MEDIUM;

    OH_Drawing_RectSize result =
        OH_Drawing_TypographyLayoutWithConstraintsWithBuffer(fTypography, size, &rangeArray, nullptr);
    EXPECT_DOUBLE_EQ(result.width, 0);
    EXPECT_DOUBLE_EQ(result.height, 0);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest004
 * @tc.desc: Test LayoutWithConstraints with all null parameters
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest004, TestSize.Level0)
{
    OH_Drawing_RectSize size;
    size.width = LAYOUT_WIDTH_MEDIUM;
    size.height = LAYOUT_HEIGHT_MEDIUM;

    OH_Drawing_RectSize result =
        OH_Drawing_TypographyLayoutWithConstraintsWithBuffer(nullptr, size, nullptr, nullptr);
    EXPECT_DOUBLE_EQ(result.width, 0);
    EXPECT_DOUBLE_EQ(result.height, 0);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest005
 * @tc.desc: Test LayoutWithConstraints with zero width constraint
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest005, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(0.0, LAYOUT_HEIGHT_MEDIUM, &rangeArray, &rangeCount);

    EXPECT_NE(rangeArray, nullptr);
    // With zero width, each character should be on its own line or truncated
    EXPECT_EQ(rangeCount, 1);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest006
 * @tc.desc: Test LayoutWithConstraints with zero height constraint
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest006, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, 0.0, &rangeArray, &rangeCount);

    EXPECT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest007
 * @tc.desc: Test LayoutWithConstraints with both zero width and height
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest007, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(0.0, 0.0, &rangeArray, &rangeCount);

    EXPECT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest008
 * @tc.desc: Test LayoutWithConstraints with negative width constraint
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest008, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result =
        CallLayoutWithConstraints(NEGATIVE_LAYOUT_LENGTH, LAYOUT_HEIGHT_MEDIUM, &rangeArray, &rangeCount);

    EXPECT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest009
 * @tc.desc: Test LayoutWithConstraints with negative height constraint
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest009, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result =
        CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, NEGATIVE_LAYOUT_LENGTH, &rangeArray, &rangeCount);

    EXPECT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest010
 * @tc.desc: Test LayoutWithConstraints and Layout.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest010, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_LARGE, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);
    OH_Drawing_TypographyLayout(GetTypography(), LAYOUT_WIDTH_LARGE);
    EXPECT_DOUBLE_EQ(result.width, OH_Drawing_TypographyGetLongestLineWithIndent(GetTypography()));

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest011
 * @tc.desc: Test LayoutWithConstraints with medium text requiring multiple lines
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest011, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(MEDIUM_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 28;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 95.69989013671875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 8;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest012
 * @tc.desc: Test LayoutWithConstraints with very long text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest012, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(LONG_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 164;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 311.8497314453125;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 15;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest013
 * @tc.desc: Test LayoutWithConstraints with empty text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest013, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(EMPTY_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_MEDIUM,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 0;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 0;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest014
 * @tc.desc: Test LayoutWithConstraints with single character
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest014, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SINGLE_CHAR_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_SMALL,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 1;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 37.249969482421875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest015
 * @tc.desc: Test LayoutWithConstraints with Chinese text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest015, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(CHINESE_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 8;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 99.999908447265625;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 4;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest016
 * @tc.desc: Test LayoutWithConstraints with mixed English and Chinese text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest016, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 82;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 304.99969482421875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 9;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest017
 * @tc.desc: Test LayoutWithConstraints with Arabic text (RTL)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest017, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(ARABIC_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 13;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 236.79977416992188;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 75;
    VerifyRectSize(expectRectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest018
 * @tc.desc: Test LayoutWithConstraints with Japanese text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest018, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(JAPANESE_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 7;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 299.99972534179688;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 2;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest019
 * @tc.desc: Test LayoutWithConstraints with Korean text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest019, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(KOREAN_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 8;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 289.49969482421875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 2;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 144;
    VerifyRectSize(expectRectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest020
 * @tc.desc: Test LayoutWithConstraints with Thai text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest020, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(THAI_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 12;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 258.44976806640625;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 76;
    VerifyRectSize(expectRectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest021
 * @tc.desc: Test GetRangeByArrayIndex with null array
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest021, TestSize.Level0)
{
    OH_Drawing_Range* range = OH_Drawing_GetRangeByArrayIndex(nullptr, 0);
    EXPECT_EQ(range, nullptr);
    OH_Drawing_Array* fontList = OH_Drawing_GetSystemFontFullNamesByType(OH_Drawing_SystemFontType::ALL);
    OH_Drawing_Range* range1 = OH_Drawing_GetRangeByArrayIndex(fontList, 0);
    EXPECT_EQ(range1, nullptr);
    OH_Drawing_ReleaseArrayBuffer(fontList);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest022
 * @tc.desc: Test GetRangeByArrayIndex with valid index
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest022, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(MEDIUM_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result =
        CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE, &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 28;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 95.69989013671875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 8;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest023
 * @tc.desc: Test GetRangeByArrayIndex with out of bounds index
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest023, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result =
        CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_MEDIUM, &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest024
 * @tc.desc: Test GetRangeByArrayIndex with all valid indices
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest024, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(LONG_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result =
        CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE, &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 55;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 106.4998779296875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 16;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest026
 * @tc.desc: Test DestroyRectSize with valid pointer
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest026, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(SHORT_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result =
        CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_MEDIUM, &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 5;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 118.04988098144531;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest027
 * @tc.desc: Test RectSize dimensions with different constraints
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest027, TestSize.Level0)
{
    CreateTypographyHandler();
    AddText(MEDIUM_TEXT);
    CreateTypography();

    // Test with small constraint
    OH_Drawing_Array* rangeArray1 = nullptr;
    size_t rangeCount1 = 0;
    OH_Drawing_RectSize result1 =
        CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE, &rangeArray1, &rangeCount1);

    ASSERT_NE(rangeArray1, nullptr);

    EXPECT_EQ(rangeCount1, 1);
    const size_t rangeEnd = 28;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray1, rangeCount1);
    const double expectRectWidth = 95.69989013671875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 8;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result1);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray1), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);

    // Test with large constraint
    OH_Drawing_Array* rangeArray2 = nullptr;
    size_t rangeCount2 = 0;
    OH_Drawing_RectSize result2 =
        CallLayoutWithConstraints(LAYOUT_WIDTH_LARGE, LAYOUT_HEIGHT_LARGE, &rangeArray2, &rangeCount2);

    ASSERT_NE(rangeArray2, nullptr);

    EXPECT_EQ(rangeCount2, 1);
    const double expectRectWidth2 = 577.29949951171875;
    const size_t expectLineCnt2 = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyAllRanges(expectedRanges, rangeArray2, rangeCount2);
    VerifyRectSize(expectRectWidth2, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt2, result2);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray2), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest029
 * @tc.desc: Test LayoutWithConstraints with emoji characters (4-byte UTF-8)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest029, TestSize.Level0)
{
    CreateTypographyHandler();
    OH_Drawing_TypographyHandlerAddText(fHandler, EMOJI_TEXT);
    CreateTypography();

    OH_Drawing_Array* fitStrRangeArr = nullptr;
    size_t arrayLen = 0;
    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
        &fitStrRangeArr, &arrayLen);

    ASSERT_NE(fitStrRangeArr, nullptr);

    EXPECT_EQ(arrayLen, 1);
    const size_t rangeEnd = 18;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, fitStrRangeArr, arrayLen);
    const double expectRectWidth = 93.549896240234375;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 7;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(fitStrRangeArr), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_ReleaseArrayBuffer(nullptr);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest030
 * @tc.desc: Test LayoutWithConstraints with single line HEAD ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest030, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(fTypoStyle, ELLIPSIS_MODAL_HEAD);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_SMALL,
        &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);

    EXPECT_EQ(rangeCount, 1);
    const size_t rangeStart = 72;
    const size_t rangeEnd = 82;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {rangeStart, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 316.8992919921875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest031
 * @tc.desc: Test LayoutWithConstraints with single line MIDDLE ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest031, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(fTypoStyle, ELLIPSIS_MODAL_MIDDLE);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_SMALL,
        &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);

    // Middle ellipsis text's range count will be 2
    EXPECT_EQ(rangeCount, 2);
    // Expect midlle ellipsis text's ranges is {0, 3} and {79, 82}
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, 3}, {79, 82} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 299.99972534179688;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 1;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest032
 * @tc.desc: Test LayoutWithConstraints with multiline HEAD ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest032, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyStyleAttributeInt(fTypoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ELLIPSIS_MODAL_MULTILINE_HEAD);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_MEDIUM,
        &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);

    // Multiline head ellipsis text's range count will be 2
    EXPECT_EQ(rangeCount, 2);
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, 17}, {72, 82} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 300.79971313476562;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 3;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest033
 * @tc.desc: Test LayoutWithConstraints with multiline MIDDLE ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest033, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyStyleAttributeInt(fTypoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ELLIPSIS_MODAL_MULTILINE_MIDDLE);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_MEDIUM,
        &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);

    // Multiline middle ellipsis text's range count will be 2
    EXPECT_EQ(rangeCount, 2);
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, 22}, {78, 82} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 300.79971313476562;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 3;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest034
 * @tc.desc: Test LayoutWithConstraints with multiline TAIL ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest034, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(fTypoStyle, ELLIPSIS_MODAL_TAIL);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_LARGE, LAYOUT_HEIGHT_MEDIUM,
        &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);

    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 82;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectRectWidth = 782.04937744140625;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 3;
    EXPECT_EQ(lineCnt, expectLineCnt);
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest035
 * @tc.desc: Test LayoutWithConstraints with HIGH_QUALITY strategy on short English text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest035, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_HIGH_QUALITY);

    CreateTypographyHandler();
    AddText(SHORT_ENGLISH_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 11;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 4;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 93.549911499023438;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest036
 * @tc.desc: Test LayoutWithConstraints with HIGH_QUALITY strategy on medium English text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest036, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_HIGH_QUALITY);

    CreateTypographyHandler();
    AddText(MEDIUM_ENGLISH_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 44;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 13;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 113.14984130859375;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest037
 * @tc.desc: Test LayoutWithConstraints with HIGH_QUALITY strategy on long English text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest037, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_HIGH_QUALITY);

    CreateTypographyHandler();
    AddText(LONG_ENGLISH_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 171;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 16;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 306.2496337890625;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest038
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy on short English text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest038, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);

    CreateTypographyHandler();
    AddText(SHORT_ENGLISH_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 11;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 3;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 90.549896240234375;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest039
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy on medium English text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest039, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);

    CreateTypographyHandler();
    AddText(MEDIUM_ENGLISH_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 44;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 12;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 110.14987182617188;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest040
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy on long English text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest040, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);

    CreateTypographyHandler();
    AddText(LONG_ENGLISH_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 171;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 16;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 299.10009765625;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest041
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy on mixed Chinese-English text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest041, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);

    CreateTypographyHandler();
    AddText(MIXED_CH_ENGLISH_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 19;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 8;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 99.999908447265625;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest042
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy on emoji text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest042, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);

    CreateTypographyHandler();
    AddText(EMOJI_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_SMALL, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 18;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 7;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 75.390625;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest043
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy and HEAD ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest043, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(fTypoStyle, ELLIPSIS_MODAL_HEAD);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_SMALL,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_GE(rangeCount, 1);
    const size_t rangeStart = 72;
    const size_t rangeEnd = 82;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {rangeStart, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const size_t expectLineCnt = 1;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 316.8992919921875;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest044
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy and MIDDLE ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest044, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(fTypoStyle, ELLIPSIS_MODAL_MIDDLE);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_SMALL,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 2);
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, 3}, {79, 82} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const size_t expectLineCnt = 1;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 298.14971923828125;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest045
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy and TAIL ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest045, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyTextEllipsisModal(fTypoStyle, ELLIPSIS_MODAL_TAIL);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_LARGE, LAYOUT_HEIGHT_SMALL,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_GE(rangeCount, 1);
    const size_t rangeEnd = 17;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const size_t expectLineCnt = 1;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 635.44940185546875;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest046
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy and MULTILINE_HEAD ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest046, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyStyleAttributeInt(fTypoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ELLIPSIS_MODAL_MULTILINE_HEAD);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_MEDIUM,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 2);
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, 8}, {72, 82} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const size_t expectLineCnt = 3;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 282.24957275390625;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest047
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy and MULTILINE_MIDDLE ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest047, TestSize.Level0)
{
    ResetTypoStyleWithFontSize(DEFAULT_FONT_SIZE);
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);
    OH_Drawing_SetTypographyTextEllipsis(fTypoStyle, "...");
    OH_Drawing_SetTypographyStyleAttributeInt(fTypoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ELLIPSIS_MODAL_MULTILINE_MIDDLE);

    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_MEDIUM,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 2);
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, 13}, {78, 82} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const size_t expectLineCnt = 3;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 275.99969482421875;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest048
 * @tc.desc: Test LayoutWithConstraints with BALANCED strategy on emoji text
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest048, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextBreakStrategy(fTypoStyle, BREAK_STRATEGY_BALANCED);

    CreateTypographyHandler();
    AddText(EMOJI_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 18;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 2;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectRectWidth = 256.33111572265625;
    VerifyRectSize(expectRectWidth, DEFAULT_SINGLE_LINE_HEIGHT * expectLineCnt, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest049
 * @tc.desc: Test LayoutWithConstraints with disable all height behavior
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest049, TestSize.Level0)
{
    OH_Drawing_SetTextStyleFontHeight(fTextStyle, FONT_HEIGHT_SCALE);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle,
        OH_Drawing_TextHeightBehavior::TEXT_HEIGHT_DISABLE_ALL);
    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 82;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectWidth = 304.99969482421875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 9;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 659;
    VerifyRectSize(expectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest050
 * @tc.desc: Test LayoutWithConstraints with disable first height behavior
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest050, TestSize.Level0)
{
    OH_Drawing_SetTextStyleFontHeight(fTextStyle, FONT_HEIGHT_SCALE);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle,
        OH_Drawing_TextHeightBehavior::TEXT_HEIGHT_DISABLE_FIRST_ASCENT);
    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 82;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectWidth = 304.99969482421875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 9;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 662;
    VerifyRectSize(expectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest051
 * @tc.desc: Test LayoutWithConstraints with disable last height behavior
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest051, TestSize.Level0)
{
    OH_Drawing_SetTextStyleFontHeight(fTextStyle, FONT_HEIGHT_SCALE);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle,
        OH_Drawing_TextHeightBehavior::TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, LAYOUT_HEIGHT_LARGE,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 82;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectWidth = 304.99969482421875;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 9;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 672;
    VerifyRectSize(expectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest052
 * @tc.desc: Test LayoutWithConstraints with lineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest052, TestSize.Level0)
{
    const double lineSpacing = 100;
    OH_Drawing_SetTypographyStyleAttributeDouble(fTypoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, lineSpacing);
    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    const double rectHeight = 400;
    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, rectHeight,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 16;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectWidth = 300.79971313476562;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    const size_t expectLineCnt = 2;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 318;
    VerifyRectSize(expectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}

/**
 * @tc.name: TypographyLayoutWithConstraintsTest053
 * @tc.desc: Test LayoutWithConstraints with lineSpacing and disable last height behavior
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLayoutConstraintsTest, TypographyLayoutWithConstraintsTest053, TestSize.Level0)
{
    const double lineSpacing = 100;
    OH_Drawing_SetTypographyStyleAttributeDouble(fTypoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, lineSpacing);
    OH_Drawing_TypographyTextSetHeightBehavior(fTypoStyle,
        OH_Drawing_TextHeightBehavior::TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    CreateTypographyHandler();
    AddText(MIXED_TEXT);
    CreateTypography();

    OH_Drawing_Array* rangeArray = nullptr;
    size_t rangeCount = 0;

    const double rectHeight = 400;
    OH_Drawing_RectSize result = CallLayoutWithConstraints(LAYOUT_WIDTH_MEDIUM, rectHeight,
                                                           &rangeArray, &rangeCount);

    ASSERT_NE(rangeArray, nullptr);
    EXPECT_EQ(rangeCount, 1);
    const size_t rangeEnd = 28;
    std::vector<std::pair<size_t, size_t>> expectedRanges = { {0, rangeEnd} };
    VerifyAllRanges(expectedRanges, rangeArray, rangeCount);
    const double expectWidth = 300.79971313476562;
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(GetTypography());
    // lineSpacing is always added optimistically per line,
    // and MUST be subtracted when the current line is treated as the last line.
    // This is required for correct constraints layout.
    const size_t expectLineCnt = 3;
    EXPECT_EQ(lineCnt, expectLineCnt);
    const double expectHeight = 377;
    VerifyRectSize(expectWidth, expectHeight, result);

    EXPECT_EQ(OH_Drawing_ReleaseArrayBuffer(rangeArray), OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
}
} // namespace OHOS