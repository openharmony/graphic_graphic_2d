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
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "rosen_text/typography.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
const double DEFAULT_DOUBLE_PLACEHOLDER = 100.0;
const double DEFAULT_DOUBLE_MAXWIDTH = 100.0;
const int DEFAULT_INT_TWO = 2;
const int DEFAULT_INT_THREE = 3;
} // namespace
class NdkTypographyStyleEllipsisTest : public testing::Test {
public:
    void TearDown() override;
    void PrepareCreateParagraphWithMulTextStyle(vector<std::string>& textVec, int maxLine,
        OH_Drawing_EllipsisModal ellipsisModal, double layoutWidth, bool isBalanced = false);
    void CreateAndPushTextStyle(std::string& text, double fontSize, OH_Drawing_FontWeight fontWeight);
protected:
    OH_Drawing_FontCollection* fontCollection_{nullptr};
    OH_Drawing_TextStyle* txtStyle_{nullptr};
    OH_Drawing_TypographyCreate* handler_{nullptr};
    OH_Drawing_TypographyStyle* typoStyle_{nullptr};
    OH_Drawing_Typography* typography_{nullptr};
};

void NdkTypographyStyleEllipsisTest::CreateAndPushTextStyle(std::string& text, double fontSize,
    OH_Drawing_FontWeight fontWeight)
{
    if (txtStyle_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle_);
        txtStyle_ = nullptr;
    }
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);

    OH_Drawing_SetTextStyleFontSize(txtStyle_, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle_, fontWeight);

    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
}
void NdkTypographyStyleEllipsisTest::PrepareCreateParagraphWithMulTextStyle(vector<std::string>& textVec, int maxLine,
    OH_Drawing_EllipsisModal ellipsisModal, double layoutWidth, bool isBalanced)
{
    constexpr size_t spanSize = 4;
    ASSERT_EQ(textVec.size(), spanSize);
    constexpr double fontSize1 = 40;
    constexpr double fontSize2 = 80;
    constexpr double fontSize3 = 50;
    constexpr double fontSize4 = 60;

    fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    ASSERT_NE(fontCollection_, nullptr);
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle_, maxLine);
    OH_Drawing_SetTypographyTextEllipsis(typoStyle_, "...");
    auto res = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle_, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ellipsisModal);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    if (isBalanced) {
        OH_Drawing_SetTypographyTextBreakStrategy(typoStyle_, OH_Drawing_BreakStrategy::BREAK_STRATEGY_BALANCED);
        OH_Drawing_SetTypographyTextWordBreakType(typoStyle_, OH_Drawing_WordBreakType::WORD_BREAK_TYPE_BREAK_WORD);
    }

    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    ASSERT_NE(handler_, nullptr);
    // set textstyle span1
    CreateAndPushTextStyle(textVec[0], fontSize1, FONT_WEIGHT_400);
    // set textstyle span2
    CreateAndPushTextStyle(textVec[1], fontSize2, FONT_WEIGHT_400);
    // set textstyle span3
    CreateAndPushTextStyle(textVec[DEFAULT_INT_TWO], fontSize3, FONT_WEIGHT_400);
    // set placeholder span
    OH_Drawing_PlaceholderSpan PlaceholderSpan;
    PlaceholderSpan.width = DEFAULT_DOUBLE_PLACEHOLDER;
    PlaceholderSpan.height = DEFAULT_DOUBLE_PLACEHOLDER;
    OH_Drawing_TypographyHandlerAddPlaceholder(handler_, &PlaceholderSpan);
    // set textstyle span4
    CreateAndPushTextStyle(textVec[DEFAULT_INT_THREE], fontSize4, FONT_WEIGHT_800);

    typography_ = OH_Drawing_CreateTypography(handler_);
    ASSERT_NE(typography_, nullptr);
    OH_Drawing_TypographyLayout(typography_, layoutWidth);
}

void NdkTypographyStyleEllipsisTest::TearDown()
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
    if (txtStyle_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle_);
        txtStyle_ = nullptr;
    }
    if (typoStyle_ != nullptr) {
        OH_Drawing_DestroyTypographyStyle(typoStyle_);
        typoStyle_ = nullptr;
    }
}

/*
 * @tc.name: TypographyStyleAttributeEllipsisModalTest001
 * @tc.desc: test invalid data for ellipsis modal via attribute API.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyStyleAttributeEllipsisModalTest001, TestSize.Level0)
{
    // Test for invalid SetTypographyStyleAttributeInt
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    auto res = OH_Drawing_SetTypographyStyleAttributeInt(nullptr, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ELLIPSIS_MODAL_MULTILINE_HEAD);
    EXPECT_EQ(res, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // Exceeds the maximum enumeration value.
    res = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL, 5);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    // Less than the minimum enumeration value.
    res = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL, -1);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);

    // Test for invalid GetTypographyStyleAttributeInt
    int output = 0;
    res = OH_Drawing_GetTypographyStyleAttributeInt(nullptr, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL, &output);
    EXPECT_EQ(res, OH_DRAWING_ERROR_INVALID_PARAMETER);
    res = OH_Drawing_GetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL, nullptr);
    EXPECT_EQ(res, OH_DRAWING_ERROR_INVALID_PARAMETER);
    
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyStyleAttributeEllipsisModalTest002
 * @tc.desc: test valid data for ellipsis modal via attribute API.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyStyleAttributeEllipsisModalTest002, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    auto res = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ELLIPSIS_MODAL_MULTILINE_HEAD);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    int outResult = 0;
    res = OH_Drawing_GetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL, &outResult);
    EXPECT_EQ(outResult, ELLIPSIS_MODAL_MULTILINE_HEAD);

    // also test multiline enum
    res = OH_Drawing_SetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        ELLIPSIS_MODAL_MULTILINE_MIDDLE);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    res = OH_Drawing_GetTypographyStyleAttributeInt(typoStyle, TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL, &outResult);
    EXPECT_EQ(outResult, ELLIPSIS_MODAL_MULTILINE_MIDDLE);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange001
 * @tc.desc: test for miltiline head ellipsis style: span change, layoutWidth is very small and
 * the maxWidth can’t even fit.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange001, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_GT(longesline, layoutWidth);
    constexpr double realLongesline = 87.7199401;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange002
 * @tc.desc: test for miltiline head ellipsis style: span change, layoutWidth is small and the paragraph contains
 * only the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange002, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    // The longes line should be greater than layout width.
    EXPECT_GT(longesline, layoutWidth);
    constexpr double realLongesline = 87.7199401;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange003
 * @tc.desc: test for miltiline head ellipsis style: span change. It marks the boundary between span2 and span3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange003, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 580;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 547.7196044;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange004
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange004, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 87.7199401;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 59.99996948;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange005
 * @tc.desc: test for miltiline head ellipsis style: maxline is 3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange005, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 87.7199401;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 59.99996948;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange006
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, text has hardbreak and layout width
 * is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange006, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 5;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 107.51989746;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_GT(lastLineWidth, layoutWidth);
    constexpr double realLastLineWidth = 52.07995605;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange007
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1 and text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange007, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 100;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 52.07995605;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, layoutWidth);
    constexpr double realLastLineWidth = 52.07995605;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange008
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange008, TestSize.Level0)
{
    vector<std::string> textVec = { "\n", "\n", "\n", "\n" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 0;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 0;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange009
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange009, TestSize.Level0)
{
    vector<std::string> textVec = { "بەلگىسى ئۇسلۇبى", "سىناق چەكمە", "ئۇسلۇبى", "سىناق چەكمە" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 96.8601074;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 64.14013671;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange010
 * @tc.desc: test for miltiline head ellipsis style: span change and breakStrategy is balance.
 * It marks the boundary between span3 and placeholderspan.
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange010, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 820;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    constexpr double realLongesline = 816.439331054;
    EXPECT_LT(lastLineWidth, layoutWidth);
    EXPECT_LT(realLongesline, layoutWidth);
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange011
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1 and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange011, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 87.7199401;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 59.99996948;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange012
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, text has hardbreak, layout width
 * is very small and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange012, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 5;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 107.51989746;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_GT(lastLineWidth, layoutWidth);
    constexpr double realLastLineWidth = 52.07995605;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange013
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1 and text has hardbreak
 * and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange013, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    constexpr double layoutWidth = 100;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 52.07995605;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, layoutWidth);
    constexpr double realLastLineWidth = 52.07995605;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange014
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has only hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange014, TestSize.Level0)
{
    vector<std::string> textVec = { "\n", "\n", "\n", "\n" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 0;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 0;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisStyleChange015
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance, R2L-ur,
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisStyleChange015, TestSize.Level0)
{
    vector<std::string> textVec = { "بەلگىسى ئۇسلۇبى", "سىناق چەكمە", "ئۇسلۇبى", "سىناق چەكمە" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 91.8601074;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 64.1401367;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange001
 * @tc.desc: test for miltiline middle ellipsis style: span change, layoutWidth is very small and
 * the maxWidth can’t even fit.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange001, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_GT(longesline, layoutWidth);
    constexpr double realLongesline = 27.719971;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange002
 * @tc.desc: test for miltiline middle ellipsis style: span change, layoutWidth is small and the paragraph contains
 * only the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange002, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    constexpr double realLongesline = 27.719971;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange003
 * @tc.desc: test for miltiline middle ellipsis style: span change. It marks the boundary between span2 and span3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange003, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 580;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 526.559570;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange004
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange004, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 80.51991271;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 21.8799743;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange005
 * @tc.desc: test for miltiline middle ellipsis style: maxline is 3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange005, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 84.55992126;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 18.1999816;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange006
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, text has hardbreak and layout width
 * is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange006, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 5;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 55.4399414;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, layoutWidth);
    EXPECT_NEAR(lastLineWidth, 0, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange007
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1 and text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange007, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 100;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 52.0799560;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, layoutWidth);
    constexpr double realLastLineWidth = 52.0799560;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange008
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange008, TestSize.Level0)
{
    vector<std::string> textVec = { "\n", "\n", "\n", "\n" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 0;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 0;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange009
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange009, TestSize.Level0)
{
    vector<std::string> textVec = { "بەلگىسى ئۇسلۇبى", "سىناق چەكمە", "ئۇسلۇبى", "سىناق چەكمە" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 86.9599151;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 59.23994445;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange010
 * @tc.desc: test for miltiline middle ellipsis style: span change and breakStrategy is balance.
 * It marks the boundary between span3 and placeholderspan.
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange010, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 820;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 786.5593261;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange011
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1 and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange011, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 97.3598937;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 69.6399230;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange012
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, text has hardbreak, layout width
 * is very small and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange012, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 5;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 55.4399414;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, layoutWidth);
    EXPECT_NEAR(lastLineWidth, 0, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange013
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1 and text has hardbreak
 * and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange013, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    constexpr double layoutWidth = 100;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 52.0799560;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, layoutWidth);
    constexpr double realLastLineWidth = 52.0799560;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange014
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has only hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange014, TestSize.Level0)
{
    vector<std::string> textVec = { "\n", "\n", "\n", "\n" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 0;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 0;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisStyleChange015
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance, R2L-ur,
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisStyleChange015, TestSize.Level0)
{
    vector<std::string> textVec = { "بەلگىسى ئۇسلۇبى", "سىناق چەكمە", "ئۇسلۇبى", "سىناق چەكمە" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_MIDDLE;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH, true);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 76.9999237;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 48.67994689;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange001
 * @tc.desc: test for tail ellipsis style: span change, layoutWidth is very small and the maxWidth can’t even fit
 * the ellipsis, yet an ellipsis is still drawn.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange001, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    constexpr double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_GT(longesline, layoutWidth);
    constexpr double realLongesline = 27.719971;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange002
 * @tc.desc: test for tail ellipsis style: span change, layoutWidth is small and the paragraph contains only
 * the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange002, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    constexpr double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    constexpr double realLongesline = 27.719971;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange003
 * @tc.desc: test for tail ellipsis style: span change. The code will take the shapestring fallback branch
 * (between span1 and span2)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange003, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    constexpr double layoutWidth = 280;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 266.559723;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange004
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between span2 and span3 in textstyles
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange004, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    constexpr double layoutWidth = 580;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 565.769470;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange005
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between span3 and placeholderspan
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange005, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    constexpr double layoutWidth = 820;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 765.769226;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange006
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between placeholderspan and span4
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange006, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    constexpr double layoutWidth = 890;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 880.259216;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange007
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between span2 and span3 in textstyles
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange007, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    constexpr double layoutWidth = 220;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 211.119781;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange008
 * @tc.desc: test for middle ellipsis style: span change: layoutWidth is very small and the maxWidth can’t even fit
 * the ellipsis, yet an ellipsis is still drawn.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange008, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    constexpr double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_GT(longesline, layoutWidth);
    constexpr double realLongesline = 27.719971;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange009
 * @tc.desc: test for middle ellipsis style: span change: layoutWidth is very small and the paragraph contains only
 * the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange009, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    constexpr double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    constexpr double realLongesline = 27.719971;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange010
 * @tc.desc: test for middle ellipsis style: span change: contains only one cluster and ellipsis.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange010, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    constexpr double layoutWidth = 100;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 81.999924;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange011
 * @tc.desc: test for middle ellipsis style: span change: the boundary between span1 and span2.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange011, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    constexpr double layoutWidth = 120;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 118.719894;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange012
 * @tc.desc: test for middle ellipsis style: span change: the boundary between span2 and span3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange012, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    constexpr double layoutWidth = 700;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 662.909424;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange013
 * @tc.desc: test for middle ellipsis style: span change: the boundary between span3 and span4.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange013, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    constexpr double layoutWidth = 1300;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 1271.758911;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange014
 * @tc.desc: test for head ellipsis style: span change: layoutWidth is even smaller than the width of the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange014, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    constexpr double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 55.440125;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange015
 * @tc.desc: test for head ellipsis style: span change: layoutWidth is bigger than the width of the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange015, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    // The current spec for head ellipsis requires at least the ellipsis itself plus one cluster to be shown.
    constexpr double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 55.440125;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange016
 * @tc.desc: test for head ellipsis style: span change: ellipsis always inherits the style of the first cluster
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange016, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    constexpr double layoutWidth = 500;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 516.959717;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange017
 * @tc.desc: test for head ellipsis style: span change.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange017, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    constexpr double layoutWidth = 1300;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 1275.778931;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange018
 * @tc.desc: test for tail ellipsis style: only Hard line-break
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange018, TestSize.Level0)
{
    vector<std::string> textVec = { "\n", "\n", "\n", "\n" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    constexpr double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 27.719971;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange019
 * @tc.desc: test for tail ellipsis style: Hard line-break and other clusters
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange019, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    constexpr double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 107.519897;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange020
 * @tc.desc: test for tail ellipsis style: Hard line-break and other clusters
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange020, TestSize.Level0)
{
    vector<std::string> textVec = { "A\nB", "\n", "C\n", "D\n" };
    constexpr int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    constexpr double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // The current ellipsis style follows that of the second hard line break
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 81.479919;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange021
 * @tc.desc: test for tail ellipsis style: R2L-ur and placeholder
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange021, TestSize.Level0)
{
    vector<std::string> textVec = { "بەلگىسى ئۇسلۇبى", "سىناق چەكمە", "ئۇسلۇبى", "سىناق چەكمە" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    constexpr double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // The current ellipsis style follows that of the placeholder style
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 969.139099;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange022
 * @tc.desc: test for tail ellipsis style: RTL and LTR mixed layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange022, TestSize.Level0)
{
    vector<std::string> textVec = { "测试", "test", "测试", "سىناق چەكمە" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    constexpr double layoutWidth = 500;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 468.579651;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange023
 * @tc.desc: test for tail ellipsis style: RTL and LTR mixed layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyEllipsisStyleChange023, TestSize.Level0)
{
    vector<std::string> textVec = { "测试", "test", "测试", "سىناق چەكمە" };
    constexpr int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    constexpr double layoutWidth = 700;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // The current ellipsis style follows that of span4(uyghur)
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 636.279480;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
}
} // namespace OHOS