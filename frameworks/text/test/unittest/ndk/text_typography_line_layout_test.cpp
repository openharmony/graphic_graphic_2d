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

#include <string>

#include "drawing_bitmap.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_error_code.h"
#include "drawing_font_collection.h"
#include "drawing_text_declaration.h"
#include "drawing_text_line.h"
#include "drawing_text_run.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "rosen_text/typography.h"
#include "text_style.h"
#include "typography_types.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
const double MAX_WIDTH = 800.0;
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
} // namespace

class NdkTypographyLineLayoutTest : public testing::Test {};

/*
 * @tc.name: TypographyDidExceedMaxLinesTest001
 * @tc.desc: test for typography mutiple lines.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyDidExceedMaxLinesTest001, TestSize.Level0)
{
    std::vector<pair<int, double>> lineResult = { { 75, 796.899231 }, { 59, 780.399231 }, { 59, 750.999268 },
        { 59, 773.041809 }, { 59, 774.646362 }, { 68, 595.349548 }, { 59, 349.772766 } };
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    const char* fontFamilies[] = { "HMOS Color Emoji", "Roboto" };
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 2, fontFamilies);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World 这\n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World 这是一个 ..... "
           "\u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    size_t fontFamilyCount = 0;
    char** fontFamiliesResult = OH_Drawing_TypographyTextlineStyleGetFontFamilies(typoStyle, &fontFamilyCount);
    EXPECT_EQ(fontFamilyCount, 2);
    for (size_t i = 0; i < fontFamilyCount; i++) {
        EXPECT_STREQ(fontFamilies[i], fontFamiliesResult[i]);
    }
    int lineCount = OH_Drawing_TypographyGetLineCount(typography);
    EXPECT_NEAR(OH_Drawing_TypographyGetLongestLine(typography), 796.899231, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TypographyGetHeight(typography), 438.000000, FLOAT_DATA_EPSILON);
    EXPECT_EQ(lineCount, 7);
    EXPECT_EQ(OH_Drawing_TypographyDidExceedMaxLines(typography), false);
    double maxLineWidth = 0.0, expectedLineHeight = 0.0;
    for (int i = 0; i < lineCount; i++) {
        double lineHeight = OH_Drawing_TypographyGetLineHeight(typography, i);
        double lineWidth = OH_Drawing_TypographyGetLineWidth(typography, i);
        EXPECT_NEAR(lineHeight, lineResult[i].first, FLOAT_DATA_EPSILON);
        EXPECT_NEAR(lineWidth, lineResult[i].second, FLOAT_DATA_EPSILON);
        maxLineWidth = std::max(maxLineWidth, lineWidth);
        expectedLineHeight += lineHeight;
    }
    EXPECT_EQ(OH_Drawing_TypographyDidExceedMaxLines(typography), false);
    EXPECT_NEAR(OH_Drawing_TypographyGetLongestLine(typography), maxLineWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TypographyGetHeight(typography), expectedLineHeight, FLOAT_DATA_EPSILON);
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(fontFamiliesResult, fontFamilyCount);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TypographyGetWordBoundaryTest001
 * @tc.desc: test for typography mutiple lines，but set the maximum number of lines
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyGetWordBoundaryTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, FONT_STYLE_NORMAL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 2);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World 这\n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World 这是一个 ..... \u1234排版信息";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    bool isExceedMaxLines = OH_Drawing_TypographyDidExceedMaxLines(typography);
    int currentLines = OH_Drawing_TypographyGetLineCount(typography);
    int maxLines = OH_Drawing_TypographyGetTextMaxLines(typoStyle);
    double longestLine = OH_Drawing_TypographyGetLongestLine(typography);
    double longestLineIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);

    OH_Drawing_Range* rangeFirst = OH_Drawing_TypographyGetWordBoundary(typography, 0);
    int startFirst = OH_Drawing_GetStartFromRange(rangeFirst);
    int endFirst = OH_Drawing_GetEndFromRange(rangeFirst);
    EXPECT_EQ(startFirst, 0);
    EXPECT_EQ(endFirst, 1);
    OH_Drawing_Range* rangeLast = OH_Drawing_TypographyGetWordBoundary(typography, 10);
    int startLast = OH_Drawing_GetStartFromRange(rangeLast);
    int endLast = OH_Drawing_GetEndFromRange(rangeLast);
    EXPECT_EQ(startLast, 8);
    EXPECT_EQ(endLast, 22);

    OH_Drawing_Range* rangeWord = OH_Drawing_TypographyGetWordBoundary(typography, 34);
    int startWord = OH_Drawing_GetStartFromRange(rangeWord);
    int endWord = OH_Drawing_GetEndFromRange(rangeWord);
    EXPECT_EQ(startWord, 32);
    EXPECT_EQ(endWord, 37);

    EXPECT_EQ(currentLines, 2);
    EXPECT_EQ(maxLines, 2);
    EXPECT_EQ(isExceedMaxLines, true);
    EXPECT_EQ(longestLine, longestLineIndent);
    EXPECT_NEAR(longestLine, 796.899231, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TypographyGetIndentsWithIndexTest001
 * @tc.desc: test for typography mutiple lines，but set mutiple indents
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyGetIndentsWithIndexTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World 这\n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World这是一个 ..... "
           "\u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    const float indents[2] = { 500.0, 55.1 };
    int indentSize = 2;
    OH_Drawing_TypographySetIndents(typography, indentSize, indents);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLine = OH_Drawing_TypographyGetLongestLine(typography);
    double longestLineIndent = OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    double lineCount = OH_Drawing_TypographyGetLineCount(typography);
    double maxLineWidth = 0.0;
    double maxLineWidthIndent = 0.0;
    for (int i = 0; i < lineCount; i++) {
        double lineWidth = OH_Drawing_TypographyGetLineWidth(typography, i);
        maxLineWidth = std::max(maxLineWidth, lineWidth);
        double indent = OH_Drawing_TypographyGetIndentsWithIndex(typography, i);
        maxLineWidthIndent = std::max(maxLineWidthIndent, lineWidth + indent);
    }
    EXPECT_NEAR(longestLine, maxLineWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longestLineIndent, maxLineWidthIndent, 0.0001);
    EXPECT_NEAR(longestLine, 739.896423, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longestLineIndent, 799.999756, FLOAT_DATA_EPSILON);

    double minIntrinsicWidth = OH_Drawing_TypographyGetMinIntrinsicWidth(typography);
    double maxIntrinsicWidth = OH_Drawing_TypographyGetMaxIntrinsicWidth(typography);
    EXPECT_NEAR(minIntrinsicWidth, 349.772766, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(maxIntrinsicWidth, 3338.310059, FLOAT_DATA_EPSILON);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TypographyGetLineTextRangeTest001
 * @tc.desc: test for typography mutiple lines，but set Set end line spaces and ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyGetLineTextRangeTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 4);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    const char* elipss = "...";
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, elipss);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, 2);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World     \n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World这是一个 ..... "
           "\u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    double longestLine = OH_Drawing_TypographyGetLongestLine(typography);
    double lineCount = OH_Drawing_TypographyGetLineCount(typography);
    double maxLineWidth = 0.0;
    for (int i = 0; i < lineCount; i++) {
        double lineWidth = OH_Drawing_TypographyGetLineWidth(typography, i);
        maxLineWidth = std::max(maxLineWidth, lineWidth);
    }
    OH_Drawing_Range* range1 = OH_Drawing_TypographyGetLineTextRange(typography, 1, false);
    EXPECT_EQ(55, OH_Drawing_GetStartFromRange(range1));
    EXPECT_EQ(93, OH_Drawing_GetEndFromRange(range1));
    OH_Drawing_Range* range2 = OH_Drawing_TypographyGetLineTextRange(typography, 1, true);
    EXPECT_EQ(55, OH_Drawing_GetStartFromRange(range2));
    EXPECT_EQ(98, OH_Drawing_GetEndFromRange(range2));
    EXPECT_NEAR(longestLine, maxLineWidth, FLOAT_DATA_EPSILON);
    OH_Drawing_PositionAndAffinity* posAAClusrterDown =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 80, 0);
    int affinityClusterDown = OH_Drawing_GetAffinityFromPositionAndAffinity(posAAClusrterDown);
    int aPositionClusterDown = OH_Drawing_GetPositionFromPositionAndAffinity(posAAClusrterDown);
    EXPECT_EQ(0, affinityClusterDown);
    EXPECT_EQ(2, aPositionClusterDown);
    OH_Drawing_PositionAndAffinity* posAAClusrterUp =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 100, 100);
    int affinityClusterUp = OH_Drawing_GetAffinityFromPositionAndAffinity(posAAClusrterUp);
    int aPositionClusterUp = OH_Drawing_GetPositionFromPositionAndAffinity(posAAClusrterUp);
    EXPECT_EQ(1, affinityClusterUp);
    EXPECT_EQ(25, aPositionClusterUp);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TypographyGetLineTextRangeTest002
 * @tc.desc: test for typography mutiple lines，but set Set end line spaces and ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyGetLineTextRangeTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x11, 0x11, 0xFF));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 50);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 4);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    const char* elipss = "...";
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, elipss);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, 2);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "这是一个排版信息دددھساسااساساس获取接口的测试文本：Hello World     \n是版信息获取接测试文本Drawing.";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    text = "这是一个排版信息བསདབད获取接口的སངབངསབ测试文lo World这是一个 ..... "
           "\u1234排版信息的测试文སསསས本Drawing.དདདདདད.       ";
    OH_Drawing_TypographyHandlerAddText(handler, text);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_Range* range1 = OH_Drawing_TypographyGetLineTextRange(typography, 0, false);
    EXPECT_EQ(0, OH_Drawing_GetStartFromRange(range1));
    EXPECT_EQ(0, OH_Drawing_GetEndFromRange(range1));
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);

    double lineCount = OH_Drawing_TypographyGetLineCount(typography);
    OH_Drawing_Range* range2 = OH_Drawing_TypographyGetLineTextRange(typography, lineCount, false);
    EXPECT_EQ(0, OH_Drawing_GetStartFromRange(range2));
    EXPECT_EQ(0, OH_Drawing_GetEndFromRange(range2));
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TypographyTest084
 * @tc.desc: test for BREAK_STRATEGY_GREEDY
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyBreakStrategyWithIndentsTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_GREEDY);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "breakStrategyTest breakStrategy breakStrategyGreedyTest";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // {1.2, 3.4} for unit test
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    // 300.0 for unit test
    double maxWidth = 300.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    OH_Drawing_TypographyLayout(nullptr, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
}

/*
 * @tc.name: TypographyTest085
 * @tc.desc: test for BREAK_STRATEGY_BALANCED
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyBreakStrategyWithIndentsTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_BALANCED);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "breakStrategyTest breakStrategy breakStrategyBALANCEDTest";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // {1.2, 3.4} for unit test
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    // 300.0 for unit test
    double maxWidth = 300.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
}

/*
 * @tc.name: TypographyTest086
 * @tc.desc: test for BREAK_STRATEGY_HIGH_QUALITY
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyBreakStrategyWithIndentsTest003, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_HIGH_QUALITY);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    EXPECT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "breakStrategyTest breakStrategy breakStrategyHighQualityTest";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    // {1.2, 3.4} for unit test
    const float indents[] = { 1.2, 3.4 };
    OH_Drawing_TypographySetIndents(typography, 2, indents);
    OH_Drawing_TypographySetIndents(nullptr, 0, indents);
    // 300.0 for unit test
    double maxWidth = 300.0;
    OH_Drawing_TypographyLayout(typography, maxWidth);
    EXPECT_EQ(maxWidth, OH_Drawing_TypographyGetMaxWidth(typography));
}

/*
 * @tc.name: OH_Drawing_TypographyInnerBalanceExceedMaxLinesTest
 * @tc.desc: Test for balance strategy exceed maxLines interface when layout line count equal to max line count
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyBalanceStrategy001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    // Test for balance strategy 2
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 2);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 1);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 16
    OH_Drawing_SetTextStyleFontSize(txtStyle, 16);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "测试: Balance exceed maxLines test";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, MAX_WIDTH);
    EXPECT_FALSE(OH_Drawing_TypographyDidExceedMaxLines(typography));
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyInnerBalanceMaxLinesTest
 * @tc.desc: Test for balance strategy set maxLines limit
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyBalanceStrategy002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    // Test for balance strategy 2
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 2);
    // Test for maxLines limit
    size_t maxLines { 3 };
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, maxLines);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 16
    OH_Drawing_SetTextStyleFontSize(txtStyle, 16);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "测试: Balance exceed maxLines test";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 50
    OH_Drawing_TypographyLayout(typography, 50);
    EXPECT_EQ(OH_Drawing_TypographyGetLineCount(typography), maxLines);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyInnerBalanceMaxLinesTest
 * @tc.desc: Test for balance strategy set maxLines limit and maxLines equals to layout line count
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyBalanceStrategy003, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, BREAK_STRATEGY_BALANCED);
    // Test for maxLines limit 2
    size_t maxLines { 2 };
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, maxLines);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 48
    OH_Drawing_SetTextStyleFontSize(txtStyle, 48);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "不易过敏，您可以尽情享受大自然的拥抱";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 450
    OH_Drawing_TypographyLayout(typography, 450);
    EXPECT_EQ(OH_Drawing_TypographyGetLineCount(typography), maxLines);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyLineSpacingTest
 * @tc.desc: Test for lineSpacing basic functions
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyLineSpacingTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    // Test for line spacing 30
    OH_Drawing_SetTypographyStyleAttributeDouble(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, 30.0);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt; ++i) {
        // Test for every single line width 77
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 77));
    }
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyLineSpacingTest
 * @tc.desc: Test for lineSpacing function with text behavior function in greedy layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyLineSpacingTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    // Test for line spacing 30
    OH_Drawing_SetTypographyStyleAttributeDouble(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, 30.0);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, OH_Drawing_TextHeightBehavior::TEXT_HEIGHT_DISABLE_ALL);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt - 1; ++i) {
        // Test for every single line width 77
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 77.0));
    }
    EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, lineCnt - 1), 47.0));
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyLineSpacingTest
 * @tc.desc: Test for lineSpacing function with text behavior function in balance layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, TypographyLineSpacingTest003, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 2);
    ASSERT_NE(typoStyle, nullptr);
    // Test for line spacing 30
    OH_Drawing_SetTypographyStyleAttributeDouble(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, 30.0);
    double lineSpacing = 0.0f;
    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetTypographyStyleAttributeDouble(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_D_LINE_SPACING, &lineSpacing);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, OH_Drawing_TextHeightBehavior::TEXT_HEIGHT_DISABLE_ALL);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt - 1; ++i) {
        // Test for every single line width 77
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 77.0));
    }
    EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, lineCnt - 1), 47.0));
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyMaxAndMinLineHeight
 * @tc.desc: Test for maxLineHeight and minLineHeight basic functions(trigger max limit)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, OH_Drawing_TypographyMaxAndMinLineHeightTest001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    // Test for min line height 30
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 30.0);
    // Test for min line height 30
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, 30.0);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt; ++i) {
        // Test for every single line width 30
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 30.0));
    }
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyMaxAndMinLineHeight
 * @tc.desc: Test for maxLineHeight and minLineHeight basic functions(trigger min limit)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, OH_Drawing_TypographyMaxAndMinLineHeightTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    // Test for min line height 100
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 100.0);
    // Test for min line height 100
    double maxLineHeight = 0.0f;
    OH_Drawing_GetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, &maxLineHeight);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(maxLineHeight, 100.0));
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, 100.0);
    double minLineHeight = 0.0f;
    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, &minLineHeight);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt; ++i) {
        // Test for every single line width 100
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 100.0));
    }
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyMaxAndMinLineHeight
 * @tc.desc: Test for maxLineHeight less than minLineHeight
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, OH_Drawing_TypographyMaxAndMinLineHeightTest003, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    // Test for min line height 10
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 10.0);
    // Test for min line height 30
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, 30.0);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt; ++i) {
        // Test for every single line width 10
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 10.0));
    }
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: OH_Drawing_TypographyMaxAndMinLineHeight
 * @tc.desc: Test for maxLineHeight less than zero
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyLineLayoutTest, OH_Drawing_TypographyMaxAndMinLineHeightTest004, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, 0);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test for font size 40
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40);
    // Test for min line height -10
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, -10.0);
    // Test for min line height 20
    OH_Drawing_SetTextStyleAttributeDouble(txtStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, 20.0);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    const char* text = "行高测试";
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 30
    OH_Drawing_TypographyLayout(typography, 30);
    size_t lineCnt = OH_Drawing_TypographyGetLineCount(typography);
    // Test for layout count 4
    EXPECT_EQ(lineCnt, 4);
    for (size_t i = 0; i < lineCnt; ++i) {
        // Test for every single line width 47
        EXPECT_TRUE(skia::textlayout::nearlyEqual(OH_Drawing_TypographyGetLineHeight(typography, i), 47.0));
    }
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}
} // namespace OHOS
