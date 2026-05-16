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

#include <securec.h>

#include "drawing_font_collection.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
const double DEFAULT_FONT_SIZE = 40;
const double DEFAULT_FONT_SIZE1 = 50;
const double DEFAULT_LAYOUT_WIDTH = 200;
} // namespace
class NdkTypographyStyleCompressPuncTest : public testing::Test {
public:
    void TearDown() override;
    void PrepareWorkForTypographyStyleTest();
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

void NdkTypographyStyleCompressPuncTest::PrepareWorkForTypographyStyleTest()
{
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);
    fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    ASSERT_NE(fontCollection_, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle_, DEFAULT_FONT_SIZE);
}

void NdkTypographyStyleCompressPuncTest::PrepareWorkForCompressPunctuationTest(std::string& text, double layoutWidth,
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

void NdkTypographyStyleCompressPuncTest::TearDown()
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
 * @tc.name: CompressPunctuationMultiLineHeadMarks
 * @tc.desc: test for compress punctuation:《【『「『（〈【〖〔［｛
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationMultiLineHeadMarks, TestSize.Level0)
{
    // Prepare Paragraph 1 and Paragraph 2, and turn on and turn off compressPunctuation respectively.
    std::string text = "《围城【测试『你好「哈哈（可以〈很好【丰盛〖盛打〔给你［极速｛自己";
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH);
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
 * @tc.name: CompressPunctuationQuotationMarks
 * @tc.desc: test for compress punctuation:"'
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationQuotationMarks, TestSize.Level0)
{
    std::string text = "‘好的 “标签";
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH);
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
 * @tc.name: CompressPunctuationHardBreak
 * @tc.desc: test for compress punctuation with hard break
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationHardBreak, TestSize.Level0)
{
    std::string text = "《围城\n【测试\n『你好\n「哈哈\n（可以\n〈很好\n【丰盛\n〖盛打\n〔给你\n［极速\n｛自己";
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH);
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
 * @tc.name: CompressPunctuationWithFontFamily
 * @tc.desc: test for compress punctuation with font Family name:"'
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationWithFontFamily, TestSize.Level0)
{
    std::string text = "‘好的 “标签";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH, 1, fontFamilies);
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
 * @tc.name: CompressPunctuationWithFontFeatureSs08FullCompress
 * @tc.desc: test for compress punctuation when setting font feature ss08.
 * ss08 can compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationWithFontFeatureSs08FullCompress, TestSize.Level0)
{
    std::string text = "《围城\n【测试\n『你好\n「哈哈\n（可以\n〈很好\n【丰盛\n〖盛打\n〔给你\n［极速\n｛自己";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH, 1, fontFamilies, true);
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
 * @tc.name: CompressPunctuationWithFontFeatureSs08PartialCompress
 * @tc.desc: test for compress punctuation when setting font feature ss08.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationWithFontFeatureSs08PartialCompress, TestSize.Level0)
{
    std::string text = "《测《试《测《试";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    constexpr double lineWidthComp1 = 174.999832;
    constexpr double lineWidthComp2 = 174.999817;
    constexpr double lineWidthComp3 = 199.999802;
    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH, 1, fontFamilies, true);

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
 * @tc.name: CompressPunctuationAllPunctuationWithSs08
 * @tc.desc: test for compress punctuation when text consists entirely of punctuation and ss08 is on.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationAllPunctuationWithSs08, TestSize.Level0)
{
    std::string text = "《《《《《《《《》》》》》》》》";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    constexpr double lineWidthComp1 = 199.999756;
    constexpr double lineWidthComp2 = 174.999786;
    constexpr size_t lineNumber = 3;

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH, 1, fontFamilies, true);
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
 * @tc.name: CompressPunctuationAllPunctuationWithoutSs08
 * @tc.desc: test for compress punctuation when text consists entirely of punctuation and ss08 is off.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationAllPunctuationWithoutSs08, TestSize.Level0)
{
    std::string text = "《《《《》》》》";
    // Binary representation of the Chinese font library in the system font for testing
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x20, 0x53, 0x61, 0x6e,
        0x73, 0x20, 0x53, 0x43};
    const char* fontFamilies[] = { fontFamiliesTest };
    constexpr size_t lineNumber = 2;

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH, 1, fontFamilies, true);
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
 * @tc.name: CompressPunctuationWithEmojiSymbol
 * @tc.desc: test for compress punctuation with symbol.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationWithEmojiSymbol, TestSize.Level0)
{
    std::string text = "【😊😂】😊";

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH);
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
 * @tc.name: CompressPunctuationSingleMark
 * @tc.desc: test for compress punctuation with single punctuation.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationSingleMark, TestSize.Level0)
{
    std::string text = "【";

    PrepareWorkForCompressPunctuationTest(text, DEFAULT_LAYOUT_WIDTH);
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
 * @tc.name: CompressPunctuationCacheSinglePunctuation
 * @tc.desc: test for paragraph cache with single punctuation when open compress punctuation.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationCacheSinglePunctuation, TestSize.Level0)
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
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH);
    size_t lineNumberFirst = OH_Drawing_TypographyGetLineCount(typography_);
    double lineFirst = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double longeslineFirst = OH_Drawing_TypographyGetLongestLine(typography_);
    // re layout
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH);
    size_t lineNumberSecond = OH_Drawing_TypographyGetLineCount(typography_);
    double lineSecond = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double longeslineSecond = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_EQ(lineNumberFirst, lineNumberSecond);
    EXPECT_NEAR(lineFirst, lineSecond, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longeslineFirst, longeslineSecond, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: CompressPunctuationCacheMultiLine
 * @tc.desc: test for paragraph cache with multi line when open compress punctuation.
 * ss08 cannot compress all head punctuation marks.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleCompressPuncTest, CompressPunctuationCacheMultiLine, TestSize.Level0)
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
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH);
    size_t lineNumberFirst = OH_Drawing_TypographyGetLineCount(typography_);
    double longeslineFirst = OH_Drawing_TypographyGetLongestLine(typography_);
    // re layout
    OH_Drawing_TypographyLayout(typography_, DEFAULT_LAYOUT_WIDTH);
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
} // namespace OHOS
