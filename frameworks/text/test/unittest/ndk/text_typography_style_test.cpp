/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <securec.h>

#include "drawing_font_collection.h"
#include "drawing_point.h"
#include "drawing_rect.h"
#include "drawing_text_line.h"
#include "drawing_text_run.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "rosen_text/typography.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
const double DEFAULT_FONT_SIZE = 40;
const double DEFAULT_FONT_SIZE1 = 50;
const double DEFAULT_LAYOUT_WIDTH = 1000;
const double DEFAULT_LAYOUT_WIDTH1 = 200;
} // namespace
class NdkTypographyStyleTest : public testing::Test {
public:
    void TearDown() override;
    void PrepareWorkForTypographyStyleTest();
    void PrepareWorkForAutoSpaceTest(std::string& text, double layoutWidth);
    void PrepareWorkForCompressPunctuationTest(std::string& text, double layoutWidth,
        int fontFamiliesNumber = 0, const char* fontFamilies[] = nullptr, bool isSetFontFeatureSs08 = false);

protected:
    OH_Drawing_FontCollection* fontCollection_{nullptr};
    OH_Drawing_TextStyle* txtStyle_{nullptr};
    OH_Drawing_TypographyCreate* handler_{nullptr};
    OH_Drawing_TypographyCreate* handler2_{nullptr};
    OH_Drawing_TypographyStyle* typoStyle_{nullptr};
    OH_Drawing_TypographyStyle* typoStyle2_{nullptr};
    OH_Drawing_Typography* typography_{nullptr};
    OH_Drawing_Typography* typography2_{nullptr};
};

void NdkTypographyStyleTest::PrepareWorkForTypographyStyleTest()
{
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);
    fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    ASSERT_NE(fontCollection_, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle_, DEFAULT_FONT_SIZE);
}
void NdkTypographyStyleTest::PrepareWorkForAutoSpaceTest(std::string& text, double layoutWidth)
{
    PrepareWorkForTypographyStyleTest();
    // paragraph1 with autospace
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle_, true);
    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    ASSERT_NE(handler_, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
    typography_ = OH_Drawing_CreateTypography(handler_);
    ASSERT_NE(typography_, nullptr);
    OH_Drawing_TypographyLayout(typography_, layoutWidth);

    // paragraph2 without autospace
    typoStyle2_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle2_, nullptr);
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle2_, false);
    handler2_ = OH_Drawing_CreateTypographyHandler(typoStyle2_, fontCollection_);
    ASSERT_NE(handler2_, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler2_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler2_, text.c_str());
    typography2_ = OH_Drawing_CreateTypography(handler2_);
    ASSERT_NE(typography2_, nullptr);
    OH_Drawing_TypographyLayout(typography2_, DEFAULT_LAYOUT_WIDTH);
}

void NdkTypographyStyleTest::PrepareWorkForCompressPunctuationTest(std::string& text, double layoutWidth,
    int fontFamiliesNumber, const char* fontFamilies[], bool isSetFontFeatureSs08)
{
    PrepareWorkForTypographyStyleTest();
    // paragraph1 with compressPunctuation
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_, TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    ASSERT_NE(handler_, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle_, DEFAULT_FONT_SIZE1);
    if (fontFamilies != nullptr) {
        OH_Drawing_SetTextStyleFontFamilies(txtStyle_, fontFamiliesNumber, fontFamilies);
    }
    if (isSetFontFeatureSs08) {
        OH_Drawing_TextStyleAddFontFeature(txtStyle_, "ss08", 1);
    }
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
    typography_ = OH_Drawing_CreateTypography(handler_);
    ASSERT_NE(typography_, nullptr);
    OH_Drawing_TypographyLayout(typography_, layoutWidth);

    // paragraph2 without compressPunctuation
    typoStyle2_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle2_, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle2_, TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, false);
    handler2_ = OH_Drawing_CreateTypographyHandler(typoStyle2_, fontCollection_);
    ASSERT_NE(handler2_, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler2_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler2_, text.c_str());
    typography2_ = OH_Drawing_CreateTypography(handler2_);
    ASSERT_NE(typography2_, nullptr);
    OH_Drawing_TypographyLayout(typography2_, layoutWidth);
}

void NdkTypographyStyleTest::TearDown()
{
    if (fontCollection_ != nullptr) {
        OH_Drawing_DestroyFontCollection(fontCollection_);
        fontCollection_ = nullptr;
    }
    if (handler_ != nullptr) {
        OH_Drawing_DestroyTypographyHandler(handler_);
        handler_ = nullptr;
    }
    if (typography_ != nullptr) {
        OH_Drawing_DestroyTypography(typography_);
        typography_ = nullptr;
    }
    if (typoStyle_ != nullptr) {
        OH_Drawing_DestroyTypographyStyle(typoStyle_);
        typoStyle_ = nullptr;
    }
    if (handler2_ != nullptr) {
        OH_Drawing_DestroyTypographyHandler(handler2_);
        handler2_ = nullptr;
    }
    if (typography2_ != nullptr) {
        OH_Drawing_DestroyTypography(typography2_);
        typography2_ = nullptr;
    }
    if (typoStyle2_ != nullptr) {
        OH_Drawing_DestroyTypographyStyle(typoStyle2_);
        typoStyle2_ = nullptr;
    }
    if (txtStyle_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle_);
        txtStyle_ = nullptr;
    }
}

/*
 * @tc.name: ParagraphTestGlyphPositionAtCoordinateWithCluster001
 * @tc.desc: test for GlyphPositionAtCoordinate with dash
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, ParagraphTestGlyphPositionAtCoordinateWithCluster001, TestSize.Level0)
{
    std::string text3 = "————————";

    double maxWidth3 = 1000.0;
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_GetFontCollectionGlobalInstance();

    OH_Drawing_SetTextStyleFontSize(txtStyle, 30);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);

    OH_Drawing_TypographyHandlerAddText(handler, text3.c_str());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, maxWidth3);

    float xCoords[] = { 0, 20, 30, 45, 60, 75, 100 };
    int expectedPositions[] = { 0, 1, 1, 2, 2, 3, 3 };
    int expectedAffinities[] = { 1, 0, 1, 0, 1, 0, 1 };
    OH_Drawing_PositionAndAffinity* results[7];
    for (int i = 0; i < 7; i++) {
        results[i] = OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, xCoords[i], 0);
        EXPECT_EQ(OH_Drawing_GetPositionFromPositionAndAffinity(results[i]), expectedPositions[i]);
        EXPECT_EQ(OH_Drawing_GetAffinityFromPositionAndAffinity(results[i]), expectedAffinities[i]);
    }

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
    for (int i = 0; i < 7; i++) {
        free(results[i]);
    }
}

/*
 * @tc.name: OH_Drawing_SetTypographyStyleAttributeBool001
 * @tc.desc: test for set typography style descriptor attribute bool and error code.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, OH_Drawing_SetTypographyStyleAttributeBool001, TestSize.Level0)
{
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_ErrorCode errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    EXPECT_EQ(errCodeSet, 0);
    errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(nullptr,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    EXPECT_EQ(errCodeSet, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, true);
    EXPECT_EQ(errCodeSet, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
}

/*
 * @tc.name: OH_Drawing_GetTypographyStyleAttributeBool001
 * @tc.desc: test for get typography style descriptor attribute bool and error code.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, OH_Drawing_GetTypographyStyleAttributeBool001, TestSize.Level0)
{
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_ErrorCode errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    EXPECT_EQ(errCodeSet, 0);
    
    bool value = false;
    OH_Drawing_ErrorCode errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, &value);
    EXPECT_EQ(errCodeGet, 0);
    EXPECT_TRUE(value);
    errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(nullptr,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, &value);
    EXPECT_EQ(errCodeGet, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, nullptr);
    EXPECT_EQ(errCodeGet, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, &value);
    EXPECT_EQ(errCodeGet, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
}

/*
 * @tc.name: TypographyCompressPunctuation001
 * @tc.desc: test for compress punctuation:《【『「『（〈【〖〔［｛
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation001, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "《围城【测试『你好「哈哈（可以〈很好【丰盛〖盛打〔给你［极速｛自己";
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1);
    constexpr double lineWidthComp1 = 124.999878;
    constexpr double lineWidthComp2 = 124.999847;
    constexpr double lineWidthComp3 = 124.999908;
    constexpr double lineWidthComp4 = 124.999969;
    constexpr size_t lineNum = 11;

    double lineWidthsTrue[lineNum] = { lineWidthComp1, lineWidthComp1, lineWidthComp2, lineWidthComp2, lineWidthComp2,
        lineWidthComp2, lineWidthComp3, lineWidthComp4, lineWidthComp4, lineWidthComp4, lineWidthComp4 };
    size_t lineNumberTrue = OH_Drawing_TypographyGetLineCount(typography_);
    size_t lineNumberFalse = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_EQ(lineNumberTrue, lineNumberFalse);
    EXPECT_EQ(lineNumberTrue, lineNum);
    for (int i = 0; i < lineNum; i++) {
        double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, i);
        EXPECT_NEAR(lineTrue, lineWidthsTrue[i], FLOAT_DATA_EPSILON);
        double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, i);
        EXPECT_LT(lineTrue, lineFalse);
    }
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_NEAR(longeslineTrue, lineWidthComp4, FLOAT_DATA_EPSILON);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_LT(longeslineTrue, longeslineFalse);
}

/*
 * @tc.name: TypographyCompressPunctuation002
 * @tc.desc: test for compress punctuation:“'
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation002, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "‘好的 “标签";
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1);
    constexpr double lineWidthComp1 = 112.049896;
    constexpr double lineWidthComp2 = 124.999878;
    constexpr size_t lineNumber = 2;

    double lineWidthsFalse[lineNumber] = { lineWidthComp1, lineWidthComp2 };
    size_t lineNumberTrue = OH_Drawing_TypographyGetLineCount(typography_);
    size_t lineNumberFalse = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_EQ(lineNumberTrue, lineNumberFalse);
    EXPECT_EQ(lineNumberTrue, lineNumber);
    for (int i = 0; i < lineNumber; i++) {
        double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, i);
        EXPECT_NEAR(lineTrue, lineWidthsFalse[i], FLOAT_DATA_EPSILON);
        double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, i);
        EXPECT_NEAR(lineFalse, lineWidthsFalse[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(lineTrue, lineFalse, FLOAT_DATA_EPSILON);
    }
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_NEAR(longeslineTrue, longeslineFalse, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyCompressPunctuation003
 * @tc.desc: test for compress punctuation with hard break
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation003, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "《围城\n【测试\n『你好\n「哈哈\n（可以\n〈很好\n【丰盛\n〖盛打\n〔给你\n［极速\n｛自己";
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1);
    constexpr size_t lineNumber = 11;

    for (int i = 0; i < lineNumber; i++) {
        double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, i);
        double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, i);
        EXPECT_LT(lineTrue, lineFalse);
    }
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_LT(longeslineTrue, longeslineFalse);
}

/*
 * @tc.name: TypographyCompressPunctuation004
 * @tc.desc: test for compress punctuation with font Family name:“'
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation004, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "‘好的 “标签";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1, 1, fontFamilies);
    constexpr double lineWidthComp1 = 112.049896;
    constexpr double lineWidthComp2 = 124.999878;
    constexpr size_t lineNumber = 2;

    double lineWidthsFalse[lineNumber] = { lineWidthComp1, lineWidthComp2 };
    for (int i = 0; i < lineNumber; i++) {
        double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, i);
        EXPECT_NEAR(lineTrue, lineWidthsFalse[i], FLOAT_DATA_EPSILON);
        double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, i);
        EXPECT_NEAR(lineFalse, lineWidthsFalse[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(lineTrue, lineFalse, FLOAT_DATA_EPSILON);
    }
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_NEAR(longeslineTrue, longeslineFalse, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyCompressPunctuation005
 * @tc.desc: test for compress punctuation when setting font feature ss08.
 * ss08 can compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation005, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "《围城\n【测试\n『你好\n「哈哈\n（可以\n〈很好\n【丰盛\n〖盛打\n〔给你\n［极速\n｛自己";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1, 1, fontFamilies, true);
    constexpr size_t lineNumber = 11;

    for (int i = 0; i < lineNumber; i++) {
        double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, i);
        double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, i);
        EXPECT_NEAR(lineTrue, lineFalse, FLOAT_DATA_EPSILON);
    }
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_NEAR(longeslineTrue, longeslineFalse, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyCompressPunctuation006
 * @tc.desc: test for compress punctuation when setting font feature ss08.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation006, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "《测《试《测《试";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    constexpr double lineWidthComp1 = 174.999832;
    constexpr double lineWidthComp2 = 174.999817;
    constexpr double lineWidthComp3 = 199.999802;
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1, 1, fontFamilies, true);

    double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    EXPECT_NEAR(lineTrue, lineFalse, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(lineTrue, lineWidthComp1, FLOAT_DATA_EPSILON);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 1);
    EXPECT_LT(line1True, line1False);
    EXPECT_NEAR(line1True, lineWidthComp2, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1False, lineWidthComp3, FLOAT_DATA_EPSILON);
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_LT(longeslineTrue, longeslineFalse);
}

/*
 * @tc.name: TypographyCompressPunctuation007
 * @tc.desc: test for compress punctuation when text consists entirely of punctuation and ss08 is on.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation007, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "《《《《《《《《》》》》》》》》";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    constexpr double lineWidthComp1 = 199.999756;
    constexpr double lineWidthComp2 = 174.999786;
    constexpr size_t lineNumber = 3;

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1, 1, fontFamilies, true);
    size_t lineNumberTrue = OH_Drawing_TypographyGetLineCount(typography_);
    size_t lineNumberFalse = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_EQ(lineNumberTrue, lineNumberFalse);
    EXPECT_EQ(lineNumberTrue, lineNumber);

    double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    EXPECT_NEAR(lineTrue, lineFalse, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(lineTrue, lineWidthComp1, FLOAT_DATA_EPSILON);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 1);
    EXPECT_NEAR(line1True, line1False, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1False, lineWidthComp2, FLOAT_DATA_EPSILON);
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_NEAR(longeslineTrue, longeslineFalse, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyCompressPunctuation008
 * @tc.desc: test for compress punctuation when text consists entirely of punctuation and ss08 is off.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation008, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "《《《《》》》》";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    constexpr size_t lineNumber = 2;

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1, 1, fontFamilies, true);
    size_t lineNumberTrue = OH_Drawing_TypographyGetLineCount(typography_);
    size_t lineNumberFalse = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_EQ(lineNumberTrue, lineNumberFalse);
    EXPECT_EQ(lineNumberTrue, lineNumber);

    double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    EXPECT_NEAR(lineTrue, lineFalse, FLOAT_DATA_EPSILON);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 1);
    EXPECT_NEAR(line1True, line1False, FLOAT_DATA_EPSILON);
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_NEAR(longeslineTrue, longeslineFalse, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyCompressPunctuation009
 * @tc.desc: test for compress punctuation with symbol.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation009, TestSize.Level0)
{
    std::string text = "【😊😂】😊";

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1);
    size_t lineNumberTrue = OH_Drawing_TypographyGetLineCount(typography_);
    size_t lineNumberFalse = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_EQ(lineNumberTrue, lineNumberFalse);

    double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    EXPECT_GT(lineTrue, lineFalse);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 1);
    EXPECT_LT(line1True, line1False);
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_GT(longeslineTrue, longeslineFalse);
}

/*
 * @tc.name: TypographyCompressPunctuation010
 * @tc.desc: test for compress punctuation with single punctuation.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation010, TestSize.Level0)
{
    std::string text = "【";

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH1);
    size_t lineNumberTrue = OH_Drawing_TypographyGetLineCount(typography_);
    size_t lineNumberFalse = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_EQ(lineNumberTrue, lineNumberFalse);

    double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double lineFalse = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    EXPECT_LT(lineTrue, lineFalse);
    double longeslineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double longeslineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    EXPECT_LT(longeslineTrue, longeslineFalse);
}

/*
 * @tc.name: TypographyCompressPunctuation011
 * @tc.desc: test for paragraph cache with single punctuation when open compress punctuation.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation011, TestSize.Level0)
{
    std::string text = "【";
    PrepareWorkForTypographyStyleTest();
    // paragraph1 with compressPunctuation
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_, TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    ASSERT_NE(handler_, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle_, DEFAULT_FONT_SIZE1);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
    typography_ = OH_Drawing_CreateTypography(handler_);
    ASSERT_NE(typography_, nullptr);
    // first layout
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH1);
    size_t lineNumberFirst = OH_Drawing_TypographyGetLineCount(typography_);
    double lineFirst = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double longeslineFirst = OH_Drawing_TypographyGetLongestLine(typography_);
    // re layout
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH1);
    size_t lineNumberSecond = OH_Drawing_TypographyGetLineCount(typography_);
    double lineSecond = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double longeslineSecond = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_EQ(lineNumberFirst, lineNumberSecond);
    EXPECT_NEAR(lineFirst, lineSecond, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longeslineFirst, longeslineSecond, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyCompressPunctuation012
 * @tc.desc: test for paragraph cache with multi line when open compress punctuation.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyCompressPunctuation012, TestSize.Level0)
{
    std::string text = "《围城【测试『你好「哈哈（可以〈很好【丰盛〖盛打〔给你［极速｛自己";
    constexpr double lineWidthComp1 = 124.999878;
    constexpr double lineWidthComp2 = 124.999847;
    constexpr double lineWidthComp3 = 124.999908;
    constexpr double lineWidthComp4 = 124.999969;
    constexpr size_t lineNum = 11;
    double lineWidthsTrue[lineNum] = { lineWidthComp1, lineWidthComp1, lineWidthComp2, lineWidthComp2, lineWidthComp2,
        lineWidthComp2, lineWidthComp3, lineWidthComp4, lineWidthComp4, lineWidthComp4, lineWidthComp4 };

    PrepareWorkForTypographyStyleTest();
    // paragraph1 with compressPunctuation
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_, TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    ASSERT_NE(handler_, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle_, DEFAULT_FONT_SIZE1);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
    typography_ = OH_Drawing_CreateTypography(handler_);
    ASSERT_NE(typography_, nullptr);
    // first layout
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH1);
    size_t lineNumberFirst = OH_Drawing_TypographyGetLineCount(typography_);
    double longeslineFirst = OH_Drawing_TypographyGetLongestLine(typography_);
    // re layout
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH1);
    size_t lineNumberSecond = OH_Drawing_TypographyGetLineCount(typography_);
    double longeslineSecond = OH_Drawing_TypographyGetLongestLine(typography_);

    EXPECT_EQ(lineNumberFirst, lineNumberSecond);
    EXPECT_EQ(lineNumberFirst, lineNum);
    EXPECT_NEAR(longeslineFirst, longeslineSecond, FLOAT_DATA_EPSILON);
    for (int i = 0; i < lineNum; i++) {
        double lineTrue = OH_Drawing_TypographyGetLineWidth(typography_, i);
        EXPECT_NEAR(lineTrue, lineWidthsTrue[i], FLOAT_DATA_EPSILON);
    }
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest001
 * @tc.desc: test for set auto space when paragraph with single run
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest001, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "SingRun©2002-2001";
    PrepareWorkForAutoSpaceTest(text, DEFAULT_LAYOUT_WIDTH);

    // paragraph1
    double longestLineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    EXPECT_NEAR(longestLineTrue, line1True, FLOAT_DATA_EPSILON);

    // paragraph2
    double longestLineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    EXPECT_NEAR(longestLineFalse, line1False, FLOAT_DATA_EPSILON);

    // compare paragraph1 and paragraph2
    EXPECT_NEAR(longestLineTrue, longestLineFalse + DEFAULT_FONT_SIZE / 8 * 2, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1True, line1False + DEFAULT_FONT_SIZE / 8 * 2, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest002
 * @tc.desc: test for set auto space when paragraph with single run and the layout width is at the boundary value.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest002, TestSize.Level0)
{
    // test boundary value：Use longestline without autospace as layout width when autospace enabled, line count + 1
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "SingRun©2002-2001";
    double layoutWidth = 388.319641; // boundary value
    PrepareWorkForAutoSpaceTest(text, layoutWidth);

    // paragraph1
    double longestLineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double line2True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    size_t lineCount = OH_Drawing_TypographyGetLineCount(typography_);
    EXPECT_NEAR(longestLineTrue, std::max(line1True, line2True), FLOAT_DATA_EPSILON);
    EXPECT_GT(layoutWidth, longestLineTrue);
    EXPECT_GT(layoutWidth, line1True);
    EXPECT_GT(line2True, 0);

    // paragraph2
    double longestLineFalse2 = OH_Drawing_TypographyGetLongestLine(typography2_);
    double line1False2 = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    size_t lineCount2 = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_NEAR(longestLineFalse2, line1False2, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longestLineFalse2, layoutWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1False2, layoutWidth, FLOAT_DATA_EPSILON);

    // compare paragraph1 and paragraph2
    EXPECT_GT(longestLineFalse2, longestLineTrue);
    EXPECT_GT(line1False2, line1True);
    EXPECT_EQ(lineCount, lineCount2 + 1);
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest003
 * @tc.desc: test for set auto space when paragraph with many lines
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest003, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "嫌疑者X的牺牲\n版权所有©2002-2001华为技术有限公司保留一切权利\n卸载USB设备";
    PrepareWorkForAutoSpaceTest(text, DEFAULT_LAYOUT_WIDTH);

    // paragraph1
    double longestLineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double line2True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    double line3True = OH_Drawing_TypographyGetLineWidth(typography_, 2);
    EXPECT_NEAR(longestLineTrue, std::max(line1True, std::max(line2True, line3True)), FLOAT_DATA_EPSILON);

    // paragraph2
    double longestLineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    double line2False = OH_Drawing_TypographyGetLineWidth(typography2_, 1);
    double line3False = OH_Drawing_TypographyGetLineWidth(typography2_, 2);
    EXPECT_NEAR(longestLineFalse, std::max(line1False, std::max(line2False, line3False)), FLOAT_DATA_EPSILON);

    // compare paragraph1 and paragraph2
    EXPECT_NEAR(longestLineTrue, longestLineFalse + DEFAULT_FONT_SIZE / 8 * 3, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1True, line1False + DEFAULT_FONT_SIZE / 8 * 2, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line2True, line2False + DEFAULT_FONT_SIZE / 8 * 3, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line3True, line3False + DEFAULT_FONT_SIZE / 8 * 2, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest004
 * @tc.desc: test for set auto space when paragraph is many lines and the layout width is at the boundary value.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest004, TestSize.Level0)
{
    // test boundary value：Use longestline without autospace as layout width when autospace enabled, line count + 1
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "嫌疑者X的牺牲\n版权所有©2002-2001华为技术有限公司保留一切权利\n卸载USB设备";
    double layoutWidth = 956.159546; // boundary value
    PrepareWorkForAutoSpaceTest(text, layoutWidth);

    // paragraph1
    double longestLineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double line2True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    double line3True = OH_Drawing_TypographyGetLineWidth(typography_, 2);
    double line4True = OH_Drawing_TypographyGetLineWidth(typography_, 3);
    size_t lineCount = OH_Drawing_TypographyGetLineCount(typography_);
    EXPECT_GT(layoutWidth, longestLineTrue);
    EXPECT_GT(layoutWidth, line1True);
    EXPECT_GT(line3True, 0);

    // paragraph2
    double longestLineFalse2 = OH_Drawing_TypographyGetLongestLine(typography2_);
    double line1False2 = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    double line2False2 = OH_Drawing_TypographyGetLineWidth(typography2_, 1);
    double line3False2 = OH_Drawing_TypographyGetLineWidth(typography2_, 2);
    size_t lineCount2 = OH_Drawing_TypographyGetLineCount(typography2_);
    EXPECT_NEAR(longestLineFalse2, layoutWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line2False2, layoutWidth, FLOAT_DATA_EPSILON);

    // compare paragraph1 and paragraph2
    EXPECT_GT(longestLineFalse2, longestLineTrue);
    EXPECT_GT(line1True, line1False2);
    // The critical width value squeezed the second line down, so the characters are fewer, and the width is shorter.
    EXPECT_GT(line2False2, line2True);
    EXPECT_GT(line4True, line3False2);
    EXPECT_EQ(lineCount, lineCount2 + 1);
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest005
 * @tc.desc: test for width from OH_Drawing_TextLineGetImageBounds when set auto space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest005, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "嫌疑者X的牺牲";
    PrepareWorkForAutoSpaceTest(text, DEFAULT_LAYOUT_WIDTH);
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    ASSERT_NE(textLines, nullptr);
    OH_Drawing_Array* textLines2 = OH_Drawing_TypographyGetTextLines(typography2_);
    ASSERT_NE(textLines2, nullptr);
    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    ASSERT_NE(textLine, nullptr);
    OH_Drawing_TextLine* textLine2 = OH_Drawing_GetTextLineByIndex(textLines2, 0);
    ASSERT_NE(textLine2, nullptr);

    // compare paragraph1 and paragraph2
    OH_Drawing_Rect* rect = nullptr;
    OH_Drawing_Rect* rect2 = nullptr;

    rect = OH_Drawing_TextLineGetImageBounds(textLine);
    rect2 = OH_Drawing_TextLineGetImageBounds(textLine2);
    EXPECT_NEAR(OH_Drawing_RectGetLeft(rect), OH_Drawing_RectGetLeft(rect2), FLOAT_DATA_EPSILON);
    EXPECT_NEAR(
        OH_Drawing_RectGetRight(rect), OH_Drawing_RectGetRight(rect2) + DEFAULT_FONT_SIZE / 8 * 2, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyTextLines(textLines);
    OH_Drawing_DestroyTextLines(textLines2);
    OH_Drawing_DestroyRunImageBounds(rect);
    OH_Drawing_DestroyRunImageBounds(rect2);
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest006
 * @tc.desc: test for width from OH_Drawing_TextLineGetOffsetForStringIndex when set auto space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest006, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "嫌疑者X的牺牲";
    PrepareWorkForAutoSpaceTest(text, DEFAULT_LAYOUT_WIDTH);
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    ASSERT_NE(textLines, nullptr);
    OH_Drawing_Array* textLines2 = OH_Drawing_TypographyGetTextLines(typography2_);
    ASSERT_NE(textLines2, nullptr);
    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    ASSERT_NE(textLine, nullptr);
    OH_Drawing_TextLine* textLine2 = OH_Drawing_GetTextLineByIndex(textLines2, 0);
    ASSERT_NE(textLine2, nullptr);

    // compare paragraph1 and paragraph2
    double textLineWidth3 = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 4) -
        OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 3);
    double textLineWidth4 = OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 5) -
        OH_Drawing_TextLineGetOffsetForStringIndex(textLine, 4);
    double textLine2Width3 = OH_Drawing_TextLineGetOffsetForStringIndex(textLine2, 4) -
        OH_Drawing_TextLineGetOffsetForStringIndex(textLine2, 3);
    double textLine2Width4 = OH_Drawing_TextLineGetOffsetForStringIndex(textLine2, 5) -
        OH_Drawing_TextLineGetOffsetForStringIndex(textLine2, 4);
    EXPECT_NEAR(textLineWidth3, textLine2Width3 + DEFAULT_FONT_SIZE / 8, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(textLineWidth4, textLine2Width4 + DEFAULT_FONT_SIZE / 8, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyTextLines(textLines);
    OH_Drawing_DestroyTextLines(textLines2);
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest007
 * @tc.desc: test for width from OH_Drawing_GetRunTypographicBounds when set auto space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest007, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "嫌疑者X的牺牲";
    PrepareWorkForAutoSpaceTest(text, DEFAULT_LAYOUT_WIDTH);

    // paragraph1
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    ASSERT_NE(textLines, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    ASSERT_GT(size, 0);
    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    ASSERT_NE(textLine, nullptr);
    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    ASSERT_NE(runs, nullptr);
    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
    ASSERT_GT(runsSize, 0);

    // paragraph2
    OH_Drawing_Array* textLines2 = OH_Drawing_TypographyGetTextLines(typography2_);
    ASSERT_NE(textLines2, nullptr);
    size_t size2 = OH_Drawing_GetDrawingArraySize(textLines2);
    ASSERT_GT(size2, 0);
    OH_Drawing_TextLine* textLine2 = OH_Drawing_GetTextLineByIndex(textLines2, 0);
    ASSERT_NE(textLine2, nullptr);
    OH_Drawing_Array* runs2 = OH_Drawing_TextLineGetGlyphRuns(textLine2);
    ASSERT_NE(runs2, nullptr);
    size_t runsSize2 = OH_Drawing_GetDrawingArraySize(runs2);
    ASSERT_GT(runsSize2, 0);

    // compare paragraph1 and paragraph2
    std::vector<float> widthAddArr = { 0, DEFAULT_FONT_SIZE / 8, DEFAULT_FONT_SIZE / 8 };
    for (int i = 0; i < runsSize; i++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, i);
        OH_Drawing_Run* run2 = OH_Drawing_GetRunByIndex(runs2, i);
        ASSERT_NE(run, nullptr);
        ASSERT_NE(run2, nullptr);
        float ascent = 0.0;
        float descent = 0.0;
        float leading = 0.0;
        float width = OH_Drawing_GetRunTypographicBounds(run, &ascent, &descent, &leading);
        float width2 = OH_Drawing_GetRunTypographicBounds(run2, &ascent, &descent, &leading);
        EXPECT_NEAR(width, width2 + widthAddArr[i], FLOAT_DATA_EPSILON);
    }
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
    OH_Drawing_DestroyRuns(runs2);
    OH_Drawing_DestroyTextLines(textLines2);
}

/*
 * @tc.name: SetTypographyTextAutoSpaceTest008
 * @tc.desc: test for width from OH_Drawing_GetRunImageBounds when set auto space
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyTextAutoSpaceTest008, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on autospace and turn off autospace respectively.
    std::string text = "嫌疑者X的牺牲";
    PrepareWorkForAutoSpaceTest(text, DEFAULT_LAYOUT_WIDTH);

    // paragraph1
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography_);
    ASSERT_NE(textLines, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(textLines);
    ASSERT_GT(size, 0);
    OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, 0);
    ASSERT_NE(textLine, nullptr);
    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);
    ASSERT_NE(runs, nullptr);
    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
    ASSERT_GT(runsSize, 0);

    // paragraph2
    OH_Drawing_Array* textLines2 = OH_Drawing_TypographyGetTextLines(typography2_);
    ASSERT_NE(textLines2, nullptr);
    size_t size2 = OH_Drawing_GetDrawingArraySize(textLines2);
    ASSERT_GT(size2, 0);
    OH_Drawing_TextLine* textLine2 = OH_Drawing_GetTextLineByIndex(textLines2, 0);
    ASSERT_NE(textLine2, nullptr);
    OH_Drawing_Array* runs2 = OH_Drawing_TextLineGetGlyphRuns(textLine2);
    ASSERT_NE(runs2, nullptr);
    size_t runsSize2 = OH_Drawing_GetDrawingArraySize(runs2);
    ASSERT_GT(runsSize2, 0);

    // compare paragraph1 and paragraph2
    std::vector<float> leftAddArr = { 0, 0, DEFAULT_FONT_SIZE / 8 };
    std::vector<float> rightAddArr = { 0, DEFAULT_FONT_SIZE / 8, DEFAULT_FONT_SIZE / 8 * 2 };
    for (int i = 0; i < runsSize; i++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, i);
        OH_Drawing_Run* run2 = OH_Drawing_GetRunByIndex(runs2, i);
        ASSERT_NE(run, nullptr);
        ASSERT_NE(run2, nullptr);
        OH_Drawing_Rect* rect = OH_Drawing_GetRunImageBounds(run);
        OH_Drawing_Rect* rect2 = OH_Drawing_GetRunImageBounds(run2);
        // It will accumulate as the number of autospaces needs to be increased.
        EXPECT_NEAR(OH_Drawing_RectGetLeft(rect), OH_Drawing_RectGetLeft(rect2) + leftAddArr[i], FLOAT_DATA_EPSILON);
        EXPECT_NEAR(OH_Drawing_RectGetRight(rect), OH_Drawing_RectGetRight(rect2) + rightAddArr[i], FLOAT_DATA_EPSILON);
        OH_Drawing_DestroyRunImageBounds(rect);
        OH_Drawing_DestroyRunImageBounds(rect2);
    }
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_DestroyTextLines(textLines);
    OH_Drawing_DestroyRuns(runs2);
    OH_Drawing_DestroyTextLines(textLines2);
}

skia::textlayout::ParagraphImpl* GetSkiaParagraph(OH_Drawing_Typography* typography)
{
    OHOS::Rosen::Typography* rosenTypography = reinterpret_cast<OHOS::Rosen::Typography*>(typography);
    OHOS::Rosen::SPText::ParagraphImpl* paragraph =
        reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(rosenTypography->GetParagraph());
    return reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get());
}

OH_Drawing_Typography* CreateParagraphWithCustomStyle(
    double layoutWidth, const char* text, OH_Drawing_TypographyStyle* typoStyle, OH_Drawing_TextStyle* textStyle)
{
    if (typoStyle == nullptr) {
        typoStyle = OH_Drawing_CreateTypographyStyle();
    }
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    if (textStyle == nullptr) {
        textStyle = OH_Drawing_CreateTextStyle();
    }
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, layoutWidth);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    return typography;
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest001
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest001, TestSize.Level0)
{
    const char* text = "Hello";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect run height is 22
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    // Expect run height is 16.407999038696289
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest002
 * @tc.desc: Test for simple text breakShapedTextIntoLines branch when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest002, TestSize.Level0)
{
    const char* text = "Hello World";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    // Test for layout width 20
    double layoutWidth = 20;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 20
    EXPECT_DOUBLE_EQ(firstLineHeight, 20);
    // Second line expect height is 16
    EXPECT_DOUBLE_EQ(middleLineHeight, 16);
    // Last line expect height is 18
    EXPECT_DOUBLE_EQ(lastLineHeight, 18);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest003
 * @tc.desc: Test for Burmese text breakShapedTextIntoLines branch when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest003, TestSize.Level0)
{
    const char* text =
        "ရှည်လျားသောသမိုင်းရှိရုံသာမကအလွန်ကြွယ်ဝ။.";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for layout width 50
    double layoutWidth = 50;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 122
    EXPECT_DOUBLE_EQ(firstLineHeight, 122);
    // Second line expect height is 109
    EXPECT_DOUBLE_EQ(middleLineHeight, 109);
    // Last line expect height is 115
    EXPECT_DOUBLE_EQ(lastLineHeight, 115);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest004
 * @tc.desc: Test for simple text breakShapedTextIntoLines branch when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest004, TestSize.Level0)
{
    const char* text =
        "سوتسىيالىستىك فېدېراتسىيە جۇمھۇرىيىتىنىڭ ئورتاق تىلى";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 88
    EXPECT_DOUBLE_EQ(firstLineHeight, 88);
    // Second line expect height is 75
    EXPECT_DOUBLE_EQ(middleLineHeight, 75);
    // Last line expect height is 81
    EXPECT_DOUBLE_EQ(lastLineHeight, 81);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest005
 * @tc.desc: Test for maxLineHeight and minLineHeight shorter than run height when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest005, TestSize.Level0)
{
    const char* text = "Hello World";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 20
    double layoutWidth = 20;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 43
    EXPECT_DOUBLE_EQ(firstLineHeight, 43);
    // Second line expect height is 30
    EXPECT_DOUBLE_EQ(middleLineHeight, 30);
    // Last line expect height is 35
    EXPECT_DOUBLE_EQ(lastLineHeight, 35);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest006
 * @tc.desc: Test for maxLineHeight and minLineHeight greater than run height when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest006, TestSize.Level0)
{
    const char* text = "Hello World";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 80
    double lineLimit = 80;
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 93
    EXPECT_DOUBLE_EQ(firstLineHeight, 93);
    // Second line expect height is 80
    EXPECT_DOUBLE_EQ(middleLineHeight, 80);
    // Last line expect height is 85
    EXPECT_DOUBLE_EQ(lastLineHeight, 85);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest007
 * @tc.desc: Test for line height scale and line height shorter than run height when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest007, TestSize.Level0)
{
    const char* text = "fontPadding&lineHeightScale测试";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line height scale 0.8
    double heightScale = 0.8;
    OH_Drawing_SetTextStyleFontHeight(textStyle, heightScale);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 53
    EXPECT_DOUBLE_EQ(firstLineHeight, 53);
    // Second line expect height is 40
    EXPECT_DOUBLE_EQ(middleLineHeight, 40);
    // Last line expect height is 45
    EXPECT_DOUBLE_EQ(lastLineHeight, 45);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest008
 * @tc.desc: Test for line height scale and line height greater than run height when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest008, TestSize.Level0)
{
    const char* text = "fontPadding&lineHeightScale测试";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line height scale 2
    double heightScale = 2;
    OH_Drawing_SetTextStyleFontHeight(textStyle, heightScale);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 113
    EXPECT_DOUBLE_EQ(firstLineHeight, 113);
    // Second line expect height is 100
    EXPECT_DOUBLE_EQ(middleLineHeight, 100);
    // Last line expect height is 105
    EXPECT_DOUBLE_EQ(lastLineHeight, 105);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest009
 * @tc.desc: Test for heightBehavior when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest009, TestSize.Level0)
{
    const char* text = "fontPadding&lineHeightScale测试";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, OH_Drawing_TextHeightBehavior::TEXT_HEIGHT_DISABLE_ALL);
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line height scale 2
    double heightScale = 2;
    OH_Drawing_SetTextStyleFontHeight(textStyle, heightScale);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 80
    EXPECT_DOUBLE_EQ(firstLineHeight, 80);
    // Second line expect height is 100
    EXPECT_DOUBLE_EQ(middleLineHeight, 100);
    // Last line expect height is 97
    EXPECT_DOUBLE_EQ(lastLineHeight, 97);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing010
 * @tc.desc: Test for includeFontPadding getter
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest010, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    bool includeFontPadding = false;
    OH_Drawing_GetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, &includeFontPadding);
    EXPECT_EQ(includeFontPadding, true);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest011
 * @tc.desc: Test for tibetan language text breakShapedTextIntoLines branch when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest011, TestSize.Level0)
{
    const char* text = "ག་གནས་ཀྱི་འདུགཡུལ";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 100
    OH_Drawing_SetTextStyleFontSize(textStyle, 100);
    // Test for layout width 300
    double layoutWidth = 300;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    // Expect line count 3
    EXPECT_EQ(totalLineCnt, 3);
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    // First line expect height is 143
    EXPECT_DOUBLE_EQ(firstLineHeight, 143);
    // Second line expect height is 117
    EXPECT_DOUBLE_EQ(middleLineHeight, 117);
    // Last line expect height is 128
    EXPECT_DOUBLE_EQ(lastLineHeight, 128);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest012
 * @tc.desc: Test Devanagari text breaking when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest012, TestSize.Level0)
{
    const char* text = "यह एक परीक्षण वाक्य है जो कई पंक्तियों में टूट जाएगा";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 80
    OH_Drawing_SetTextStyleFontSize(textStyle, 80);
    // Test for layout width 300
    double layoutWidth = 300;
    OH_Drawing_Typography* typography =
        CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t lineCount = skiaParagraph->lineNumber();
    // Expect line count 6
    EXPECT_EQ(lineCount, 6);
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[lineCount - 1].height();
    // First line expect height is 127
    EXPECT_DOUBLE_EQ(firstLineHeight, 127);
    // Second line expect height is 107
    EXPECT_DOUBLE_EQ(middleLineHeight, 107);
    // Last line expect height is 116
    EXPECT_DOUBLE_EQ(lastLineHeight, 116);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest013
 * @tc.desc: Test Thai script line break when open includeFontPadding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest013, TestSize.Level0)
{
    const char* text =
        "ภาษาไทยไม่มีการเว้นวรรคทำให้ต้องตัดคำตามพจนานุกรม";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 80
    OH_Drawing_SetTextStyleFontSize(textStyle, 80);
    // Test for layout width 280
    double layoutWidth = 280;
    OH_Drawing_Typography* typography =
        CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t lineCount = skiaParagraph->lineNumber();
    // Expect line count 9
    EXPECT_EQ(lineCount, 9);
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[lineCount - 1].height();
    // First line expect height is 141
    EXPECT_DOUBLE_EQ(firstLineHeight, 141);
    // Second line expect height is 121
    EXPECT_DOUBLE_EQ(middleLineHeight, 121);
    // Last line expect height is 130
    EXPECT_DOUBLE_EQ(lastLineHeight, 130);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest014
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Japanese)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest014, TestSize.Level0)
{
    const char* text = "こんにちは";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 22
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    // Expect line run height is 16.407999038696289
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest015
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Korean)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest015, TestSize.Level0)
{
    const char* text = "안녕하세요";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 25
    EXPECT_DOUBLE_EQ(lineHeight, 25);
    // Expect run height is 20.271999359130859
    EXPECT_DOUBLE_EQ(runHeight, 20.271999359130859);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest016
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Thai)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest016, TestSize.Level0)
{
    const char* text = "สวัสดี";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 26
    EXPECT_DOUBLE_EQ(lineHeight, 26);
    // Expect run height is 21.153999328613281
    EXPECT_DOUBLE_EQ(runHeight, 21.153999328613281);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest017
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Chinese)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest017, TestSize.Level0)
{
    const char* text = "你好世界";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 22
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    // Expect line run height is 16.407999038696289
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest018
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Emoji)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest018, TestSize.Level0)
{
    const char* text = "😀😁😂🤣😃";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 22
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    // Expect line run height is 16.440366744995117
    EXPECT_DOUBLE_EQ(runHeight, 16.440366744995117);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest019
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Symbols)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest019, TestSize.Level0)
{
    const char* text = "∑∏√∞≈≠≤≥";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 22
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    // Expect line run height is 16.407999038696289
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest020
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Tibetan)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest020, TestSize.Level0)
{
    const char* text = "བོད་ཡིག";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 22
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    // Expect line run height is 16.407999038696289
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest021
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Uyghur)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest021, TestSize.Level0)
{
    const char* text = "ئۇيغۇرچە";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 26
    EXPECT_DOUBLE_EQ(lineHeight, 26);
    // Expect line run height is 21.027999877929688
    EXPECT_DOUBLE_EQ(runHeight, 21.027999877929688);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest022
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Burmese)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest022, TestSize.Level0)
{
    const char* text = "မင်္ဂလာပါ";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 36
    EXPECT_DOUBLE_EQ(lineHeight, 36);
    // Expect line run height is 30.576000213623047
    EXPECT_DOUBLE_EQ(runHeight, 30.576000213623047);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleIncludeFontPaddingTest023
 * @tc.desc: Test for positionShapedTextIntoLine branch when open includeFontPadding (Russian)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleIncludeFontPaddingTest023, TestSize.Level0)
{
    const char* text = "межстрочного";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::Typographic,
        skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    // Expect line height is 22
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    // Expect line run height is 16.407999038696289
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing001
 * @tc.desc: Test for maxLineHeight and minLineHeight for English when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing001, TestSize.Level0)
{
    const char* text = "max/minLineHeight&fallbackLineSpacing test";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 59 if open fallbackLineSpacing
    double expectLineHeight = 59;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing002
 * @tc.desc: Test for maxLineHeight and minLineHeight greater than run height when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing002, TestSize.Level0)
{
    const char* text = "max/minLineHeight&fallbackLineSpacing测试";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 80
    double lineLimit = 80;
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(
        textStyle, OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::CSS, skia::textlayout::LineMetricStyle::CSS);
    EXPECT_DOUBLE_EQ(runHeight, lineLimit);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), lineLimit);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing003
 * @tc.desc: Test for line height scale and line height shorter than run height when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing003, TestSize.Level0)
{
    const char* text = "fallbackLineSpacing&lineHeightScale测试";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line height scale 0.8
    double heightScale = 0.8;
    OH_Drawing_SetTextStyleFontHeight(textStyle, heightScale);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 59 if open fallbackLineSpacing
    double expectLineHeight = 59;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing004
 * @tc.desc: Test for line height scale and line height greater than run height when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing004, TestSize.Level0)
{
    const char* text = "fallbackLineSpacing&lineHeightScale测试";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line height scale 2
    double heightScale = 2;
    OH_Drawing_SetTextStyleFontHeight(textStyle, heightScale);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 100 if open fallbackLineSpacing
    double expectLineHeight = 100;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing005
 * @tc.desc: Test for fallbackLineSpacing getter
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing005, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(
        typoStyle, OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    bool fallbackLineSpacing = false;
    OH_Drawing_GetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, &fallbackLineSpacing);
    EXPECT_EQ(fallbackLineSpacing, true);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing006
 * @tc.desc: Test for maxLineHeight and minLineHeight for Chinese when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing006, TestSize.Level0)
{
    const char* text = "测试行高回退中文场景";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 59
    double expectLineHeight = 59;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing007
 * @tc.desc: Test for maxLineHeight and minLineHeight for Emoji when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing007, TestSize.Level0)
{
    const char* text = "😀😁😂🤣😃😄😅😆";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 59
    double expectLineHeight = 59;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing008
 * @tc.desc: Test for maxLineHeight and minLineHeight for Tibetan when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing008, TestSize.Level0)
{
    const char* text = "བོད་ཡིག་སྡེ་ཚན་ཚོགས་སྦྱོང་།";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 89
    double expectLineHeight = 89;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing009
 * @tc.desc: Test for maxLineHeight and minLineHeight for Uyghur when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing009, TestSize.Level0)
{
    const char* text = "ئۇيغۇرچە يېزىقنى سىناش";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect run height is 76
    double expectRunHeight = 76;
    // Expect line height is 75
    double expectLineHeight = 75;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectRunHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing010
 * @tc.desc: Test for maxLineHeight and minLineHeight for Burmese when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing010, TestSize.Level0)
{
    const char* text = "မြန်မာစာ စမ်းသပ်မှု";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect run height is 110
    double expectRunHeight = 110;
    // Expect line height is 109
    double expectLineHeight = 109;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectRunHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing011
 * @tc.desc: Test for maxLineHeight and minLineHeight for Japanese when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing011, TestSize.Level0)
{
    const char* text = "日本語フォールバック行間のテスト";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 59
    double expectLineHeight = 59;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing012
 * @tc.desc: Test for maxLineHeight and minLineHeight for Russian when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing012, TestSize.Level0)
{
    const char* text = "Тест межстрочного интервала";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    // Test for font size 50
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    // Test for line limit 30
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    // Test for layout width 80
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 59
    double expectLineHeight = 59;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing013
 * @tc.desc: Test for maxLineHeight and minLineHeight when open fallbackLineSpacing (Korean)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing013, TestSize.Level0)
{
    const char* text = "한글 테스트";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect run height is 73
    double expectRunHeight = 73;
    // Expect line height is 72
    double expectLineHeight = 72;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectRunHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleFallbackLineSpacing014
 * @tc.desc: Test for maxLineHeight and minLineHeight when open fallbackLineSpacing (Thai)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleFallbackLineSpacing014, TestSize.Level0)
{
    const char* text = "การทดสอบ ภาษาไทย";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_B_FALLBACK_LINE_SPACING, true);
    ASSERT_TRUE(errorCode == OH_Drawing_ErrorCode::OH_DRAWING_SUCCESS);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    double lineLimit = 30;
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, lineLimit);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle,
        OH_Drawing_TextStyleAttributeId::TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, lineLimit);
    double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);
    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double runHeight = skiaParagraph->runs()[0].calculateHeight(skia::textlayout::LineMetricStyle::CSS,
        skia::textlayout::LineMetricStyle::CSS);
    // Expect line height is 76
    double expectLineHeight = 76;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}

/*
 * @tc.name: TypographStyleDestroyPositionAndAffinity001
 * @tc.desc: Test for destroy nullptr for OH_Drawing_DestroyPositionAndAffinity
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleDestroyPositionAndAffinity001, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_DestroyPositionAndAffinity(nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographStyleDestroyPositionAndAffinity002
 * @tc.desc: Test for destroy positionAndAffinity object
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographStyleDestroyPositionAndAffinity002, TestSize.Level0)
{
    const char* text = "Destroy positionAndAffinity";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 500
    double layoutWidth = 500;
    OH_Drawing_TypographyLayout(typography, layoutWidth);
    OH_Drawing_PositionAndAffinity* positionAndAffinity =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 0, 1);
    ASSERT_NE(positionAndAffinity, nullptr);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
} // namespace OHOS