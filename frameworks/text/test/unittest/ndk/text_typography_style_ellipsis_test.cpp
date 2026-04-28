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
#include "impl/paragraph_impl.h"
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

skia::textlayout::ParagraphImpl* GetSkParagraph(OH_Drawing_Typography* typography)
{
    OHOS::Rosen::Typography* rosenTypography = reinterpret_cast<OHOS::Rosen::Typography*>(typography);
    OHOS::Rosen::SPText::ParagraphImpl* paragraph =
        reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(rosenTypography->GetParagraph());
    return reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get());
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
 * @tc.name: TypographyStyleAttributeEllipsisModalInvalidParameter
 * @tc.desc: test invalid data for ellipsis modal via attribute API.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyStyleAttributeEllipsisModalInvalidParameter, TestSize.Level0)
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
 * @tc.name: TypographyStyleAttributeEllipsisModalValidParameter
 * @tc.desc: test valid data for ellipsis modal via attribute API.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyStyleAttributeEllipsisModalValidParameter, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisVerySmallWidthCannotFit
 * @tc.desc: test for miltiline head ellipsis style: span change, layoutWidth is very small and
 * the maxWidth can’t even fit.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisVerySmallWidthCannotFit, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisSmallWidthOnlyEllipsis
 * @tc.desc: test for miltiline head ellipsis style: span change, layoutWidth is small and the paragraph contains
 * only the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisSmallWidthOnlyEllipsis, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisSpanBoundarySpan2Span3
 * @tc.desc: test for miltiline head ellipsis style: span change. It marks the boundary between span2 and span3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisSpanBoundarySpan2Span3, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisMaxLine2
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisMaxLine2, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisMaxLine3
 * @tc.desc: test for miltiline head ellipsis style: maxline is 3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisMaxLine3, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    constexpr double realLongesline = 87.71981811;
    EXPECT_NEAR(longesline, realLongesline, FLOAT_DATA_EPSILON);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 59.99984741;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyMultiHeadEllipsisMaxLine2HardBreakSmallWidth
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, text has hardbreak and layout width
 * is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisMaxLine2HardBreakSmallWidth, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisMaxLine2HardBreak
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1 and text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisMaxLine2HardBreak, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisMaxLine3OnlyHardBreaks
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisMaxLine3OnlyHardBreaks, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisMaxLine3RightToLeftUyghur
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisMaxLine3RightToLeftUyghur, TestSize.Level0)
{
    vector<std::string> textVec = { "بەلگىسى ئۇسلۇبى", "سىناق چەكمە", "ئۇسلۇبى", "سىناق چەكمە" };
    constexpr int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MULTILINE_HEAD;

    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, DEFAULT_DOUBLE_MAXWIDTH);
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, DEFAULT_DOUBLE_MAXWIDTH);
    double lastLineWidth = OH_Drawing_TypographyGetLineWidth(typography_, maxline - 1);
    EXPECT_LT(lastLineWidth, DEFAULT_DOUBLE_MAXWIDTH);
    constexpr double realLastLineWidth = 64.14013671;
    EXPECT_NEAR(lastLineWidth, realLastLineWidth, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: MultiHeadEllipsisBalancedSpanBoundarySpan3Placeholder
 * @tc.desc: test for miltiline head ellipsis style: span change and breakStrategy is balance.
 * It marks the boundary between span3 and placeholderspan.
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, MultiHeadEllipsisBalancedSpanBoundarySpan3Placeholder, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisBalancedMaxLine3
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1 and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisBalancedMaxLine3, TestSize.Level0)
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
 * @tc.name: MultiHeadEllipsisBalancedMaxLine2HardBreakSmallWidth
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, text has hardbreak, layout width
 * is very small and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, MultiHeadEllipsisBalancedMaxLine2HardBreakSmallWidth, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisBalancedMaxLine2HardBreak
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1 and text has hardbreak
 * and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisBalancedMaxLine2HardBreak, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisBalancedMaxLine3OnlyHardBreaks
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has only hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisBalancedMaxLine3OnlyHardBreaks, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisBalancedMaxLine3RightToLeftUyghur
 * @tc.desc: test for miltiline head ellipsis style: maxline is over 1, breakStrategy is balance, R2L-ur,
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisBalancedMaxLine3RightToLeftUyghur, TestSize.Level0)
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
 * @tc.name: TypographyMultiHeadEllipsisVerticalAlignCenter
 * @tc.desc: test for miltiline head ellipsis style: open vertical align.
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiHeadEllipsisVerticalAlignCenter, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 2);
    const char* ellipsis = "...";
    OH_Drawing_SetTypographyTextEllipsis(typoStyle, ellipsis);
    OH_Drawing_SetTypographyVerticalAlignment(
        typoStyle, OH_Drawing_TextVerticalAlignment::TEXT_VERTICAL_ALIGNMENT_CENTER);
    OH_Drawing_SetTypographyStyleAttributeInt(
        typoStyle,
        OH_Drawing_TypographyStyleAttributeId::TYPOGRAPHY_STYLE_ATTR_I_ELLIPSIS_MODAL,
        OH_Drawing_EllipsisModal::ELLIPSIS_MODAL_MULTILINE_HEAD);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_GetFontCollectionGlobalInstance();
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    OH_Drawing_TypographyHandlerAddText(handler, "还好还还好号好hello哈哈哈哈哈");
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 300);
    EXPECT_DOUBLE_EQ(OH_Drawing_TypographyGetLineWidth(typography, 1), 249.99971008300781);
    skia::textlayout::ParagraphImpl* skParagraph = GetSkParagraph(typography);
    auto lines = skParagraph->lines();
    size_t ellipisisRunIndex = 0;
    for (const auto& line : lines) {
        if (!line.ellipsis()) {
            continue;
        }
        ellipisisRunIndex = line.ellipsis()->index();
    }
    EXPECT_EQ(ellipisisRunIndex, 3);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TypographyMultiMiddleEllipsisVerySmallWidthCannotFit
 * @tc.desc: test for miltiline middle ellipsis style: span change, layoutWidth is very small and
 * the maxWidth can’t even fit.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisVerySmallWidthCannotFit, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisSmallWidthOnlyEllipsis
 * @tc.desc: test for miltiline middle ellipsis style: span change, layoutWidth is small and the paragraph contains
 * only the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisSmallWidthOnlyEllipsis, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisSpanBoundarySpan2Span3
 * @tc.desc: test for miltiline middle ellipsis style: span change. It marks the boundary between span2 and span3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisSpanBoundarySpan2Span3, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisMaxLine2
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisMaxLine2, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisMaxLine3
 * @tc.desc: test for miltiline middle ellipsis style: maxline is 3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisMaxLine3, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisMaxLine2HardBreakSmallWidth
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, text has hardbreak and layout width
 * is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisMaxLine2HardBreakSmallWidth, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisMaxLine2HardBreak
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1 and text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisMaxLine2HardBreak, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisMaxLine3OnlyHardBreaks
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisMaxLine3OnlyHardBreaks, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisMaxLine3RightToLeftUyghur
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisMaxLine3RightToLeftUyghur, TestSize.Level0)
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
 * @tc.name: MultiMiddleEllipsisBalancedSpanBoundarySpan3Placeholder
 * @tc.desc: test for miltiline middle ellipsis style: span change and breakStrategy is balance.
 * It marks the boundary between span3 and placeholderspan.
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, MultiMiddleEllipsisBalancedSpanBoundarySpan3Placeholder, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisBalancedMaxLine3
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1 and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisBalancedMaxLine3, TestSize.Level0)
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
 * @tc.name: MultiMiddleEllipsisBalancedMaxLine2HardBreakSmallWidth
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, text has hardbreak, layout width
 * is very small and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, MultiMiddleEllipsisBalancedMaxLine2HardBreakSmallWidth, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisBalancedMaxLine2HardBreak
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1 and text has hardbreak
 * and breakStrategy is balance.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisBalancedMaxLine2HardBreak, TestSize.Level0)
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
 * @tc.name: TypographyMultiMiddleEllipsisBalancedMaxLine3OnlyHardBreaks
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance and
 * text has only hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMultiMiddleEllipsisBalancedMaxLine3OnlyHardBreaks, TestSize.Level0)
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
 * @tc.name: MultiMiddleEllipsisBalancedMaxLine3RightToLeftUyghur
 * @tc.desc: test for miltiline middle ellipsis style: maxline is over 1, breakStrategy is balance, R2L-ur,
 * text has hardbreak.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, MultiMiddleEllipsisBalancedMaxLine3RightToLeftUyghur, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisVerySmallWidthCannotFit
 * @tc.desc: test for tail ellipsis style: span change, layoutWidth is very small and the maxWidth can’t even fit
 * the ellipsis, yet an ellipsis is still drawn.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisVerySmallWidthCannotFit, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisSmallWidthOnlyEllipsis
 * @tc.desc: test for tail ellipsis style: span change, layoutWidth is small and the paragraph contains only
 * the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisSmallWidthOnlyEllipsis, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisFallbackSpan1Span2
 * @tc.desc: test for tail ellipsis style: span change. The code will take the shapestring fallback branch
 * (between span1 and span2)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisFallbackSpan1Span2, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisSpanBoundarySpan2Span3
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between span2 and span3 in textstyles
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisSpanBoundarySpan2Span3, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisSpanBoundarySpan3Placeholder
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between span3 and placeholderspan
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisSpanBoundarySpan3Placeholder, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisSpanBoundaryPlaceholderSpan4
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between placeholderspan and span4
 * in the text styles.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisSpanBoundaryPlaceholderSpan4, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisMaxLine3SpanBoundarySpan2Span3
 * @tc.desc: test for tail ellipsis style: span change. It marks the boundary between span2 and span3 in textstyles
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisMaxLine3SpanBoundarySpan2Span3, TestSize.Level0)
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
 * @tc.name: TypographyMiddleEllipsisVerySmallWidthCannotFit
 * @tc.desc: test for middle ellipsis style: span change: layoutWidth is very small and the maxWidth can’t even fit
 * the ellipsis, yet an ellipsis is still drawn.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMiddleEllipsisVerySmallWidthCannotFit, TestSize.Level0)
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
 * @tc.name: TypographyMiddleEllipsisSmallWidthOnlyEllipsis
 * @tc.desc: test for middle ellipsis style: span change: layoutWidth is very small and the paragraph contains only
 * the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMiddleEllipsisSmallWidthOnlyEllipsis, TestSize.Level0)
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
 * @tc.name: TypographyMiddleEllipsisSingleClusterAndEllipsis
 * @tc.desc: test for middle ellipsis style: span change: contains only one cluster and ellipsis.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMiddleEllipsisSingleClusterAndEllipsis, TestSize.Level0)
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
 * @tc.name: TypographyMiddleEllipsisSpanBoundarySpan1Span2
 * @tc.desc: test for middle ellipsis style: span change: the boundary between span1 and span2.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMiddleEllipsisSpanBoundarySpan1Span2, TestSize.Level0)
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
 * @tc.name: TypographyMiddleEllipsisSpanBoundarySpan2Span3
 * @tc.desc: test for middle ellipsis style: span change: the boundary between span2 and span3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMiddleEllipsisSpanBoundarySpan2Span3, TestSize.Level0)
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
 * @tc.name: TypographyMiddleEllipsisSpanBoundarySpan3Span4
 * @tc.desc: test for middle ellipsis style: span change: the boundary between span3 and span4.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyMiddleEllipsisSpanBoundarySpan3Span4, TestSize.Level0)
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
 * @tc.name: TypographyHeadEllipsisVerySmallWidthCannotFit
 * @tc.desc: test for head ellipsis style: span change: layoutWidth is even smaller than the width of the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyHeadEllipsisVerySmallWidthCannotFit, TestSize.Level0)
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
 * @tc.name: TypographyHeadEllipsisSmallWidthBiggerThanEllipsis
 * @tc.desc: test for head ellipsis style: span change: layoutWidth is bigger than the width of the ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyHeadEllipsisSmallWidthBiggerThanEllipsis, TestSize.Level0)
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
 * @tc.name: TypographyHeadEllipsisInheritsFirstClusterStyle
 * @tc.desc: test for head ellipsis style: span change: ellipsis always inherits the style of the first cluster
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyHeadEllipsisInheritsFirstClusterStyle, TestSize.Level0)
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
 * @tc.name: TypographyHeadEllipsisLargeWidthSpanChange
 * @tc.desc: test for head ellipsis style: span change.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyHeadEllipsisLargeWidthSpanChange, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisOnlyHardLineBreaks
 * @tc.desc: test for tail ellipsis style: only Hard line-break
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisOnlyHardLineBreaks, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisHardBreakAndClustersMaxLine2
 * @tc.desc: test for tail ellipsis style: Hard line-break and other clusters
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisHardBreakAndClustersMaxLine2, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisHardBreakWithinSpanMaxLine2
 * @tc.desc: test for tail ellipsis style: Hard line-break and other clusters
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisHardBreakWithinSpanMaxLine2, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisRightToLeftUyghurWithPlaceholder
 * @tc.desc: test for tail ellipsis style: R2L-ur and placeholder
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisRightToLeftUyghurWithPlaceholder, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisMixedRtlLtrWidth500
 * @tc.desc: test for tail ellipsis style: RTL and LTR mixed layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisMixedRtlLtrWidth500, TestSize.Level0)
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
 * @tc.name: TypographyTailEllipsisMixedRtlLtrWidth700
 * @tc.desc: test for tail ellipsis style: RTL and LTR mixed layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleEllipsisTest, TypographyTailEllipsisMixedRtlLtrWidth700, TestSize.Level0)
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