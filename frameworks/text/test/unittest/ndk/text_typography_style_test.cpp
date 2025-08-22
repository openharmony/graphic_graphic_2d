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

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
const double DEFAULT_FONT_SIZE = 40;
const double DEFAULT_LAYOUT_WIDTH = 1000;
const double DEFAULT_DOUBLE_PALCEHOLDER = 100.0;
const int DEFAULT_INT_TWO = 2;
const int DEFAULT_INT_THREE = 3;
} // namespace
class NdkTypographyStyleTest : public testing::Test {
public:
    void TearDown() override;
    void PrepareWorkForTypographyStyleTest();
    void PrepareWorkForAutoSpaceTest(std::string& text, double layoutWidth);
    void PrepareCreateParagraphWithMulTextStyle(vector<std::string>& textVec, int maxLine,
        OH_Drawing_EllipsisModal ellipsisModal, double layoutWidth);

protected:
    OH_Drawing_FontCollection* fontCollection_{nullptr};
    OH_Drawing_TextStyle* txtStyle_{nullptr};
    OH_Drawing_TextStyle* txtStyle2_{nullptr};
    OH_Drawing_TextStyle* txtStyle3_{nullptr};
    OH_Drawing_TextStyle* txtStyle4_{nullptr};
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
    if (txtStyle2_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle2_);
        txtStyle2_ = nullptr;
    }
    if (txtStyle3_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle3_);
        txtStyle3_ = nullptr;
    }
    if (txtStyle4_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle4_);
        txtStyle4_ = nullptr;
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

void NdkTypographyStyleTest::PrepareCreateParagraphWithMulTextStyle(vector<std::string>& textVec, int maxLine,
    OH_Drawing_EllipsisModal ellipsisModal, double layoutWidth)
{
    // There are 4 text-style spans.
    ASSERT_EQ(textVec.size(), 4);

    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    ASSERT_NE(fontCollection_, nullptr);
    // set paragraphStyle
    OH_Drawing_SetTypographyTextMaxLines(typoStyle_, maxLine);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle_, ellipsisModal);
    OH_Drawing_SetTypographyTextEllipsis(typoStyle_, "...");

    // textstyle span1
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    // Test for fontsize 40
    OH_Drawing_SetTextStyleFontSize(txtStyle_, 40);
    OH_Drawing_SetTextStyleDecoration(txtStyle_, TEXT_DECORATION_UNDERLINE);

    //  textstyle span2
    txtStyle2_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle2_, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle2_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0x00));
    // Test for fontsize 80
    OH_Drawing_SetTextStyleFontSize(txtStyle2_, 80);
    OH_Drawing_SetTextStyleDecoration(txtStyle2_, TEXT_DECORATION_LINE_THROUGH);

    // textstyle span3
    txtStyle3_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle3_, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle3_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0xFF));
    // Test for fontsize 50
    OH_Drawing_SetTextStyleFontSize(txtStyle3_, 50);
    OH_Drawing_SetTextStyleDecoration(txtStyle3_, TEXT_DECORATION_OVERLINE);

    // placeholder span
    OH_Drawing_PlaceholderSpan* PlaceholderSpan = new OH_Drawing_PlaceholderSpan();
    ASSERT_NE(PlaceholderSpan, nullptr);
    PlaceholderSpan->width = DEFAULT_DOUBLE_PALCEHOLDER;
    PlaceholderSpan->height = DEFAULT_DOUBLE_PALCEHOLDER;

    // textstyle span4
    txtStyle4_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle4_, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle4_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));
    // Test for fontsize 60
    OH_Drawing_SetTextStyleFontSize(txtStyle4_, 60);
    OH_Drawing_SetTextStyleFontWeight(txtStyle4_, FONT_WEIGHT_800);
    OH_Drawing_SetTextStyleDecoration(txtStyle4_, TEXT_DECORATION_UNDERLINE);

    handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    ASSERT_NE(handler_, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, textVec[0].c_str());
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle2_);
    OH_Drawing_TypographyHandlerAddText(handler_, textVec[1].c_str());
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle3_);
    OH_Drawing_TypographyHandlerAddText(handler_, textVec[DEFAULT_INT_TWO].c_str());
    OH_Drawing_TypographyHandlerAddPlaceholder(handler_, PlaceholderSpan);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle4_);
    OH_Drawing_TypographyHandlerAddText(handler_, textVec[DEFAULT_INT_THREE].c_str());
    typography_ = OH_Drawing_CreateTypography(handler_);
    ASSERT_NE(typography_, nullptr);
    OH_Drawing_TypographyLayout(typography_, layoutWidth);

    delete PlaceholderSpan;
}

/*
 * @tc.name: TypographyEllipsisStyleChange001
 * @tc.desc: test for tail ellipsis style when span change, maxwidth = 10.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange001, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    // Test for layout width 10: When layoutWidth is very small and the paragraph contains only the ellipsis.
    double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_GT(longesline, layoutWidth);
    // In this scenario, longestLine is 27.719971
    EXPECT_NEAR(longesline, 27.719971, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange002
 * @tc.desc: test for tail ellipsis style when span change, maxwidth = 28.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange002, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    // Test for layout width 28: When layoutWidth is small and the paragraph contains only the ellipsis.
    double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 27.719971
    EXPECT_NEAR(longesline, 27.719971, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange003
 * @tc.desc: test for tail ellipsis style when span change, maxwidth = 280.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange003, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    // When layoutWidth is 280, the code will take the shapestring fallback branch.
    double layoutWidth = 280;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 266.559723
    EXPECT_NEAR(longesline, 266.559723, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange004
 * @tc.desc: test for tail ellipsis style when span change, maxwidth = 580.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange004, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    // When layoutWidth is 580, it marks the boundary between span2 and span3 in the text styles.
    double layoutWidth = 580;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 565.769470
    EXPECT_NEAR(longesline, 565.769470, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange005
 * @tc.desc: test for tail ellipsis style when span change, maxwidth = 820.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange005, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    // When layoutWidth is 820, it marks the boundary between span3 and placeholderspan in the text styles.
    double layoutWidth = 820;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 765.769226
    EXPECT_NEAR(longesline, 765.769226, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange006
 * @tc.desc: test for tail ellipsis style when span change, maxwidth = 890.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange006, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    // When layoutWidth is 890, it marks the boundary between placeholderspan and span4 in the text styles.
    double layoutWidth = 890;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 880.259216
    EXPECT_NEAR(longesline, 880.259216, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange007
 * @tc.desc: test for tail ellipsis style when span change, maxwidth is 220, maxline is 3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange007, TestSize.Level0)
{
    vector<std::string> textVec = { "test ellipisis", "测试标题", "这是正文", "好" };
    // Test for paragraph maxline 3
    int maxline = 3;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;

    // When layoutWidth is 220, it marks the boundary between span2 and span3 in the text styles.
    double layoutWidth = 220;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 211.119781
    EXPECT_NEAR(longesline, 211.119781, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange008
 * @tc.desc: test for middle ellipsis style when span change: layoutWidth is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange008, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    // Test for layout width 10: When layoutWidth is very small and the paragraph contains only the ellipsis:
    double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_GT(longesline, layoutWidth);
    // In this scenario, longestLine is 27.719971, and layoutWidth < longesline.
    EXPECT_NEAR(longesline, 27.719971, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange009
 * @tc.desc: test for middle ellipsis style when span change: layoutWidth is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange009, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    // Test for layout width 28: layoutWidth is very small and the paragraph contains only the ellipsis
    double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 27.719971, and layoutWidth > longesline
    EXPECT_NEAR(longesline, 27.719971, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange010
 * @tc.desc: test for middle ellipsis style when span change: contains only one cluster and ellipsis.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange010, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    // Test for layout width 100: layoutWidth is very small and the paragraph contains only one cluster and ellipsis.
    double layoutWidth = 100;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 81.999924
    EXPECT_NEAR(longesline, 81.999924, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange011
 * @tc.desc: test for middle ellipsis style when span change: between span1 and span2.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange011, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    // When layoutWidth is 120, it marks the boundary between span1 and span2 in the text styles.
    double layoutWidth = 120;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 118.719894
    EXPECT_NEAR(longesline, 118.719894, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange012
 * @tc.desc: test for middle ellipsis style when span change: between span2 and span3.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange012, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    // When layoutWidth is 700, it marks the boundary between span2 and span3 in the text styles.
    double layoutWidth = 700;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 662.909424
    EXPECT_NEAR(longesline, 662.909424, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange013
 * @tc.desc: test for middle ellipsis style when span change: between span3 and span4.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange013, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL MIDDLE" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_MIDDLE;

    // When layoutWidth is 1300, it marks the boundary between span2 and span3 in the text styles.
    double layoutWidth = 1300;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 1271.758911
    EXPECT_NEAR(longesline, 1271.758911, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange014
 * @tc.desc: test for head ellipsis style when span change: layoutWidth is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange014, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    // When layoutWidth is 10, it’s even smaller than the width of the ellipsis.
    double layoutWidth = 10;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    // In this scenario, longestLine is 55.440125
    EXPECT_NEAR(longesline, 55.440125, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange015
 * @tc.desc: test for head ellipsis style when span change: layoutWidth is very small.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange015, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    // When layoutWidth is 28, it’s bigger than the width of the ellipsis. But the current spec for head ellipsis
    // requires at least the ellipsis itself plus one cluster to be shown.
    double layoutWidth = 28;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    // In this scenario, longestLine is 55.440125
    EXPECT_NEAR(longesline, 55.440125, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange016
 * @tc.desc: test for head ellipsis style when span change.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange016, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    // Test for layout width 500: the ellipsis always inherits the style of the first cluster.
    double layoutWidth = 500;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    // In this scenario, longestLine is 516.959717
    EXPECT_NEAR(longesline, 516.959717, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange017
 * @tc.desc: test for head ellipsis style when span change.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange017, TestSize.Level0)
{
    vector<std::string> textVec = { "A", "测试标题", "这是正文", "ELLIPSIS MODAL HEAD" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_HEAD;

    // Test for layout width 1300: the ellipsis always inherits the style of the first cluster.
    double layoutWidth = 1300;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);
    // Note: The head-ellipsis calculation currently over-counts by one ellipsis width in longestLine.
    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    // In this scenario, longestLine is 1275.778931
    EXPECT_NEAR(longesline, 1275.778931, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange018
 * @tc.desc: test for tail ellipsis style: Hard line-break
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange018, TestSize.Level0)
{
    vector<std::string> textVec = { "\n", "\n", "\n", "\n" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    // Test for layout width 1000
    double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // The current ellipsis style follows that of the first hard line break, longestLine is 27.719971
    EXPECT_NEAR(longesline, 27.719971, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange019
 * @tc.desc: test for tail ellipsis style: Hard line-break
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange019, TestSize.Level0)
{
    vector<std::string> textVec = { "A\n", "B\n", "C\n", "D\n" };
    // Test for paragraph maxline 2
    int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    // Test for layout width 1000
    double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // The current ellipsis style follows that of the second hard line break, longestLine is 107.519897
    EXPECT_NEAR(longesline, 107.519897, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange020
 * @tc.desc: test for tail ellipsis style: Hard line-break
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange020, TestSize.Level0)
{
    vector<std::string> textVec = { "A\nB", "\n", "C\n", "D\n" };
    // Test for paragraph maxline 2
    int maxline = 2;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    // Test for layout width 1000
    double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // The current ellipsis style follows that of the second hard line break, longestLine is 81.479919
    EXPECT_NEAR(longesline, 81.479919, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange021
 * @tc.desc: test for tail ellipsis style: R2L-ur and placeholder
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange021, TestSize.Level0)
{
    vector<std::string> textVec = { "بەلگىسى ئۇسلۇبى", "سىناق چەكمە", "ئۇسلۇبى", "سىناق چەكمە" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    // Test for layout width 1000
    double layoutWidth = 1000;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // The current ellipsis style follows that of the placeholder style, longestLine is 969.139099
    EXPECT_NEAR(longesline, 969.139099, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange022
 * @tc.desc: test for tail ellipsis style: RTL and LTR mixed layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange022, TestSize.Level0)
{
    vector<std::string> textVec = { "测试", "test", "测试", "سىناق چەكمە" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    // Test for layout width 500
    double layoutWidth = 500;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // In this scenario, longestLine is 468.579651
    EXPECT_NEAR(longesline, 468.579651, FLOAT_DATA_EPSILON);
}

/*
 * @tc.name: TypographyEllipsisStyleChange023
 * @tc.desc: test for tail ellipsis style: RTL and LTR mixed layout
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, TypographyEllipsisStyleChange023, TestSize.Level0)
{
    vector<std::string> textVec = { "测试", "test", "测试", "سىناق چەكمە" };
    // Test for paragraph maxline 1
    int maxline = 1;
    OH_Drawing_EllipsisModal ellipsisModal = ELLIPSIS_MODAL_TAIL;
    // Test for layout width 700
    double layoutWidth = 700;
    PrepareCreateParagraphWithMulTextStyle(textVec, maxline, ellipsisModal, layoutWidth);

    double longesline = OH_Drawing_TypographyGetLongestLine(typography_);
    EXPECT_LT(longesline, layoutWidth);
    // The current ellipsis style follows that of span4(uyghur), longestLine is 636.279480
    EXPECT_NEAR(longesline, 636.279480, FLOAT_DATA_EPSILON);
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
} // namespace OHOS