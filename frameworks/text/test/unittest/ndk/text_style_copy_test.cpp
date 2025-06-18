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

#include "drawing_brush.h"
#include "drawing_font_collection.h"
#include "drawing_pen.h"
#include "drawing_point.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
const double DOUBLE_TEST_SIZE_NOT_DEFAULT = 20.0;
const float FLOAT_TEST_SIZE_NOT_DEFAULT = 10.0f;
const int INT_TEST_SIZE_NOT_DEFAULT = 2;

} // namespace
class NDKTextStyleCopyTest : public testing::Test {
public:
    void TearDown() override;

protected:
    OH_Drawing_FontCollection* fontCollection_ {nullptr};
    OH_Drawing_TextStyle* txtStyle_ {nullptr};
    OH_Drawing_TypographyCreate* handler_ {nullptr};
    OH_Drawing_Typography* typography_ {nullptr};
    OH_Drawing_TypographyStyle* typoStyle_ {nullptr};

    OH_Drawing_TextStyle* txtStyleCopy_ {nullptr};
    OH_Drawing_TypographyStyle* typoStyleCopy_ {nullptr};
};

void NDKTextStyleCopyTest::TearDown()
{
    if (fontCollection_ != nullptr) {
        OH_Drawing_DestroyFontCollection(fontCollection_);
        fontCollection_ = nullptr;
    }
    if (txtStyle_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyle_);
        txtStyle_ = nullptr;
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
    if (txtStyleCopy_ != nullptr) {
        OH_Drawing_DestroyTextStyle(txtStyleCopy_);
        txtStyleCopy_ = nullptr;
    }
    if (typoStyleCopy_ != nullptr) {
        OH_Drawing_DestroyTypographyStyle(typoStyleCopy_);
        typoStyleCopy_ = nullptr;
    }
}

/*
 * @tc.name: CopyTextStyle001
 * @tc.desc: test for copy text style：base
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTextStyle001, TestSize.Level0)
{
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);

    OH_Drawing_SetTextStyleColor(txtStyle_, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle_, TEXT_DECORATION_STYLE_DOTTED);
    OH_Drawing_SetTextStyleFontWeight(txtStyle_, FONT_WEIGHT_900);
    OH_Drawing_SetTextStyleFontStyle(txtStyle_, FONT_STYLE_ITALIC);
    OH_Drawing_SetTextStyleBaseLine(txtStyle_, TEXT_BASELINE_IDEOGRAPHIC);
    const char* fontFamilies[] = { "Text", "Text2" };
    int fontFamiliesNumBefore = 2;
    OH_Drawing_SetTextStyleFontFamilies(txtStyle_, fontFamiliesNumBefore, fontFamilies);
    OH_Drawing_SetTextStyleFontSize(txtStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTextStyleFontHeight(txtStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTextStyleLocale(txtStyle_, "TestLocal");
    OH_Drawing_TextStyleSetBaselineShift(txtStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_TextStyleSetPlaceholder(txtStyle_);

    txtStyleCopy_ = OH_Drawing_CopyTextStyle(txtStyle_);
    ASSERT_NE(txtStyleCopy_, nullptr);

    EXPECT_EQ(OH_Drawing_TextStyleGetColor(txtStyleCopy_), 0xFF00FFFF);
    EXPECT_EQ(OH_Drawing_TextStyleGetDecorationStyle(txtStyleCopy_), TEXT_DECORATION_STYLE_DOTTED);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyleCopy_), FONT_WEIGHT_900);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(txtStyleCopy_), FONT_STYLE_ITALIC);
    EXPECT_EQ(OH_Drawing_TextStyleGetBaseline(txtStyleCopy_), TEXT_BASELINE_IDEOGRAPHIC);
    size_t fontFamiliesNumAfter;
    char** fontFamiliesList = OH_Drawing_TextStyleGetFontFamilies(txtStyleCopy_, &fontFamiliesNumAfter);
    EXPECT_EQ(fontFamiliesNumAfter, fontFamiliesNumBefore);
    EXPECT_NE(fontFamiliesList, nullptr);
    EXPECT_NEAR(OH_Drawing_TextStyleGetFontSize(txtStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TextStyleGetLetterSpacing(txtStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TextStyleGetWordSpacing(txtStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TextStyleGetFontHeight(txtStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_EQ(std::strcmp(OH_Drawing_TextStyleGetLocale(txtStyleCopy_), "TestLocal"), 0);
    EXPECT_NEAR(OH_Drawing_TextStyleGetBaselineShift(txtStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_TRUE(OH_Drawing_TextStyleIsPlaceholder(txtStyleCopy_));
    OH_Drawing_TextStyleDestroyFontFamilies(fontFamiliesList, fontFamiliesNumAfter);
}

/*
 * @tc.name: CopyTextStyle002
 * @tc.desc: test for copy text style: FontStyleStruct、AddFontFeature
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTextStyle002, TestSize.Level0)
{
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);

    OH_Drawing_FontStyleStruct fontStyle;
    fontStyle.slant = FONT_STYLE_ITALIC;
    fontStyle.weight = FONT_WEIGHT_600;
    fontStyle.width = FONT_WIDTH_EXTRA_EXPANDED;
    OH_Drawing_SetTextStyleFontStyleStruct(txtStyle_, fontStyle);
    const char* tag = "frac";
    const char* tag2 = "test";
    OH_Drawing_TextStyleAddFontFeature(txtStyle_, tag, INT_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_TextStyleAddFontFeature(txtStyle_, tag2, INT_TEST_SIZE_NOT_DEFAULT);

    txtStyleCopy_ = OH_Drawing_CopyTextStyle(txtStyle_);
    ASSERT_NE(txtStyleCopy_, nullptr);

    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyleStruct(txtStyleCopy_).weight, FONT_WEIGHT_600);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyleStruct(txtStyleCopy_).slant, FONT_STYLE_ITALIC);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyleStruct(txtStyleCopy_).width, FONT_WIDTH_EXTRA_EXPANDED);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontFeatureSize(txtStyleCopy_), 2); // Set 2 font features
}

/*
 * @tc.name: CopyTextStyle003
 * @tc.desc: test for copy text style: brush
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTextStyle003, TestSize.Level0)
{
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);

    OH_Drawing_Brush* backgroundBrush = OH_Drawing_BrushCreate();
    OH_Drawing_Brush* foregroundBrush = OH_Drawing_BrushCreate();
    ASSERT_NE(backgroundBrush, nullptr);
    ASSERT_NE(foregroundBrush, nullptr);
    OH_Drawing_BrushSetColor(backgroundBrush, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));
    OH_Drawing_BrushSetColor(foregroundBrush, OH_Drawing_ColorSetArgb(0xFF, 0x01, 0xFF, 0xFF));
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyle_, backgroundBrush);
    OH_Drawing_SetTextStyleForegroundBrush(txtStyle_, foregroundBrush);

    txtStyleCopy_ = OH_Drawing_CopyTextStyle(txtStyle_);
    ASSERT_NE(txtStyleCopy_, nullptr);

    OH_Drawing_Brush* backgroundBrushCopy = OH_Drawing_BrushCreate();
    OH_Drawing_Brush* foregroundBrushCopy = OH_Drawing_BrushCreate();
    ASSERT_NE(backgroundBrushCopy, nullptr);
    ASSERT_NE(foregroundBrushCopy, nullptr);
    OH_Drawing_TextStyleGetBackgroundBrush(txtStyleCopy_, backgroundBrushCopy);
    OH_Drawing_TextStyleGetForegroundBrush(txtStyleCopy_, foregroundBrushCopy);
    EXPECT_EQ(OH_Drawing_BrushGetColor(backgroundBrushCopy), 0xFF00FFFF);
    EXPECT_EQ(OH_Drawing_BrushGetColor(foregroundBrushCopy), 0xFF01FFFF);

    // 修改复制之后的brush的color不会影响copy前brush的颜色
    OH_Drawing_BrushSetColor(backgroundBrushCopy, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_BrushGetColor(backgroundBrushCopy), 0xFF000000);
    EXPECT_EQ(OH_Drawing_BrushGetColor(backgroundBrush), 0xFF00FFFF);
    OH_Drawing_BrushDestroy(backgroundBrush);
    OH_Drawing_BrushDestroy(foregroundBrush);
    OH_Drawing_BrushDestroy(backgroundBrushCopy);
    OH_Drawing_BrushDestroy(foregroundBrushCopy);
}

/*
 * @tc.name: CopyTextStyle004
 * @tc.desc: test for copy text style: pen
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTextStyle004, TestSize.Level0)
{
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);

    OH_Drawing_Pen* backgroundPen = OH_Drawing_PenCreate();
    OH_Drawing_Pen* foregroundPen = OH_Drawing_PenCreate();
    ASSERT_NE(backgroundPen, nullptr);
    ASSERT_NE(foregroundPen, nullptr);
    OH_Drawing_PenSetColor(backgroundPen, OH_Drawing_ColorSetArgb(0xFF, 0x02, 0xFF, 0xFF));
    OH_Drawing_PenSetColor(foregroundPen, OH_Drawing_ColorSetArgb(0xFF, 0x03, 0xFF, 0xFF));
    OH_Drawing_SetTextStyleBackgroundPen(txtStyle_, backgroundPen);
    OH_Drawing_SetTextStyleForegroundPen(txtStyle_, foregroundPen);

    OH_Drawing_CopyTextStyle(nullptr);
    txtStyleCopy_ = OH_Drawing_CopyTextStyle(txtStyle_);
    ASSERT_NE(txtStyleCopy_, nullptr);

    OH_Drawing_Pen* backgroundPenCopy = OH_Drawing_PenCreate();
    OH_Drawing_Pen* foregroundPenCopy = OH_Drawing_PenCreate();
    ASSERT_NE(backgroundPenCopy, nullptr);
    ASSERT_NE(foregroundPenCopy, nullptr);
    OH_Drawing_TextStyleGetBackgroundPen(txtStyleCopy_, backgroundPenCopy);
    OH_Drawing_TextStyleGetForegroundPen(txtStyleCopy_, foregroundPenCopy);
    EXPECT_EQ(OH_Drawing_PenGetColor(backgroundPenCopy), 0xFF02FFFF);
    EXPECT_EQ(OH_Drawing_PenGetColor(foregroundPenCopy), 0xFF03FFFF);

    // 修改复制之后的pen的color不会影响copy前pen的颜色
    OH_Drawing_PenSetColor(backgroundPenCopy, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_PenGetColor(backgroundPenCopy), 0xFF000000);
    EXPECT_EQ(OH_Drawing_PenGetColor(backgroundPen), 0xFF02FFFF);
    OH_Drawing_PenDestroy(backgroundPen);
    OH_Drawing_PenDestroy(foregroundPen);
    OH_Drawing_PenDestroy(backgroundPenCopy);
    OH_Drawing_PenDestroy(foregroundPenCopy);
}

/*
 * @tc.name: CopyTextShadow001
 * @tc.desc: test for copy text shadow and copy textstyle with shadow
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTextShadow001, TestSize.Level0)
{
    OH_Drawing_TextShadow* shadow = OH_Drawing_CreateTextShadow();
    ASSERT_NE(shadow, nullptr);
    OH_Drawing_Point* offset = OH_Drawing_PointCreate(FLOAT_TEST_SIZE_NOT_DEFAULT, FLOAT_TEST_SIZE_NOT_DEFAULT);
    ASSERT_NE(offset, nullptr);
    double blurRadius = DOUBLE_TEST_SIZE_NOT_DEFAULT;
    OH_Drawing_SetTextShadow(shadow, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00), offset, blurRadius);
    OH_Drawing_TextShadow* shadowCopy = OH_Drawing_CopyTextShadow(shadow);
    OH_Drawing_CopyTextShadow(nullptr);
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);
    OH_Drawing_TextStyleAddShadow(txtStyle_, shadow);
    OH_Drawing_TextStyleAddShadow(txtStyle_, shadowCopy);

    txtStyleCopy_ = OH_Drawing_CopyTextStyle(txtStyle_);
    ASSERT_NE(txtStyleCopy_, nullptr);
    OH_Drawing_TextShadow* shadowsCopy = OH_Drawing_TextStyleGetShadows(txtStyleCopy_);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowCount(txtStyleCopy_), OH_Drawing_TextStyleGetShadowCount(txtStyle_));
    OH_Drawing_PointDestroy(offset);
    OH_Drawing_DestroyTextShadow(shadow);
    OH_Drawing_DestroyTextShadow(shadowCopy);
    OH_Drawing_DestroyTextShadows(shadowsCopy);
}

/*
 * @tc.name: CopyTypographStyle001
 * @tc.desc: test for copy typography style
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTypographStyle001, TestSize.Level0)
{
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);

    OH_Drawing_SetTypographyTextFontHeight(typoStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle_, true);
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle_, FONT_WEIGHT_600);
    OH_Drawing_SetTypographyTextLineStyleFontStyle(typoStyle_, FONT_STYLE_ITALIC);
    const char* fontFamilies[] = { "Text1", "Text2" };
    int fontFamiliesNumBefore = 2;
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle_, fontFamiliesNumBefore, fontFamilies);
    OH_Drawing_SetTypographyTextLineStyleFontSize(typoStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTypographyTextLineStyleFontHeight(typoStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(typoStyle_, true);
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(typoStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTypographyTextLineStyleOnly(typoStyle_, true);
    OH_Drawing_SetTypographyTextAlign(typoStyle_, TEXT_ALIGN_CENTER);
    OH_Drawing_SetTypographyTextDirection(typoStyle_, TEXT_DIRECTION_RTL);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle_, INT_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTypographyTextEllipsis(typoStyle_, "...");
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle_, TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    OH_Drawing_TypographyStyleSetHintsEnabled(typoStyle_, true);

    typoStyleCopy_ = OH_Drawing_CopyTypographyStyle(typoStyle_);
    ASSERT_NE(typoStyleCopy_, nullptr);

    EXPECT_TRUE(OH_Drawing_TypographyTextGetLineStyle(typoStyleCopy_));
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetFontWeight(typoStyleCopy_), FONT_WEIGHT_600);
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetFontStyle(typoStyleCopy_), FONT_STYLE_ITALIC);
    size_t num;
    char** fontFamiliesList = OH_Drawing_TypographyTextlineStyleGetFontFamilies(typoStyleCopy_, &num);
    EXPECT_EQ(num, fontFamiliesNumBefore);
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetFontSize(typoStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT);
    EXPECT_TRUE(OH_Drawing_TypographyTextlineStyleGetHeightOnly(typoStyleCopy_));
    EXPECT_NEAR(OH_Drawing_TypographyTextlineStyleGetHeightScale(typoStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT,
        FLOAT_DATA_EPSILON);
    EXPECT_TRUE(OH_Drawing_TypographyTextlineStyleGetHalfLeading(typoStyleCopy_));
    EXPECT_EQ(OH_Drawing_TypographyTextlineStyleGetSpacingScale(typoStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT);
    EXPECT_TRUE(OH_Drawing_TypographyTextlineGetStyleOnly(typoStyleCopy_));
    EXPECT_EQ(OH_Drawing_TypographyGetTextAlign(typoStyleCopy_), TEXT_ALIGN_CENTER);
    EXPECT_EQ(OH_Drawing_TypographyGetTextDirection(typoStyleCopy_), TEXT_DIRECTION_RTL);
    EXPECT_EQ(OH_Drawing_TypographyGetTextMaxLines(typoStyleCopy_), INT_TEST_SIZE_NOT_DEFAULT);
    EXPECT_EQ(std::strcmp(OH_Drawing_TypographyGetTextEllipsis(typoStyleCopy_), "..."), 0);
    EXPECT_EQ(OH_Drawing_TypographyTextGetHeightBehavior(typoStyleCopy_), TEXT_HEIGHT_DISABLE_LAST_ASCENT);
    EXPECT_TRUE(OH_Drawing_TypographyStyleIsHintEnabled(typoStyleCopy_));
    OH_Drawing_TextStyleDestroyFontFamilies(fontFamiliesList, num);
}

/*
 * @tc.name: CopyTypographStyle002
 * @tc.desc: test for copy typography style：FontWeight、FontStyle、FontFamily、FontSize、FontHeight
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTypographStyle002, TestSize.Level0)
{
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);

    OH_Drawing_SetTypographyTextFontWeight(typoStyle_, FONT_WEIGHT_900);
    OH_Drawing_SetTypographyTextFontStyle(typoStyle_, FONT_STYLE_ITALIC);
    OH_Drawing_SetTypographyTextFontFamily(typoStyle_, "Test");
    OH_Drawing_SetTypographyTextFontSize(typoStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);
    OH_Drawing_SetTypographyTextFontHeight(typoStyle_, DOUBLE_TEST_SIZE_NOT_DEFAULT);

    OH_Drawing_CopyTypographyStyle(nullptr);
    typoStyleCopy_ = OH_Drawing_CopyTypographyStyle(typoStyle_);
    ASSERT_NE(typoStyleCopy_, nullptr);

    txtStyleCopy_ = OH_Drawing_TypographyGetTextStyle(typoStyleCopy_);
    ASSERT_NE(txtStyleCopy_, nullptr);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontWeight(txtStyleCopy_), FONT_WEIGHT_900);
    EXPECT_EQ(OH_Drawing_TextStyleGetFontStyle(txtStyleCopy_), FONT_STYLE_ITALIC);
    size_t fontFamiliesNumAfter;
    char** fontFamiliesList = OH_Drawing_TextStyleGetFontFamilies(txtStyleCopy_, &fontFamiliesNumAfter);
    EXPECT_EQ(fontFamiliesNumAfter, 1);
    EXPECT_NEAR(OH_Drawing_TextStyleGetFontSize(txtStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(OH_Drawing_TextStyleGetFontHeight(txtStyleCopy_), DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    OH_Drawing_TextStyleDestroyFontFamilies(fontFamiliesList, fontFamiliesNumAfter);
}

/*
 * @tc.name: CopyTypographStyle003
 * @tc.desc: test for copy typography style: FontStyleStruct
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTypographStyle003, TestSize.Level0)
{
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_600;
    normalStyle.width = FONT_WIDTH_EXTRA_EXPANDED;
    normalStyle.slant = FONT_STYLE_ITALIC;
    OH_Drawing_SetTypographyStyleFontStyleStruct(typoStyle_, normalStyle);

    typoStyleCopy_ = OH_Drawing_CopyTypographyStyle(typoStyle_);
    ASSERT_NE(typoStyleCopy_, nullptr);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetFontStyleStruct(typoStyleCopy_).weight, FONT_WEIGHT_600);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetFontStyleStruct(typoStyleCopy_).slant, FONT_STYLE_ITALIC);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetFontStyleStruct(typoStyleCopy_).width, FONT_WIDTH_EXTRA_EXPANDED);
}

/*
 * @tc.name: CopyTypographStyle004
 * @tc.desc: test for copy text shadow: typographystyle->textstyle->shadow
 * @tc.type: FUNC
 */
HWTEST_F(NDKTextStyleCopyTest, CopyTypographStyle004, TestSize.Level0)
{
    OH_Drawing_TextShadow* shadow = OH_Drawing_CreateTextShadow();
    ASSERT_NE(shadow, nullptr);
    OH_Drawing_Point* offset = OH_Drawing_PointCreate(FLOAT_TEST_SIZE_NOT_DEFAULT, FLOAT_TEST_SIZE_NOT_DEFAULT);
    ASSERT_NE(offset, nullptr);
    double blurRadius = DOUBLE_TEST_SIZE_NOT_DEFAULT;
    OH_Drawing_SetTextShadow(shadow, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00), offset, blurRadius);
    OH_Drawing_TextShadow* shadowCopy = OH_Drawing_CopyTextShadow(shadow);
    txtStyle_ = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle_, nullptr);
    OH_Drawing_TextStyleAddShadow(txtStyle_, shadow);
    OH_Drawing_TextStyleAddShadow(txtStyle_, shadow);
    OH_Drawing_TextStyleAddShadow(txtStyle_, shadowCopy);

    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_SetTypographyTextStyle(typoStyle_, txtStyle_);
    typoStyleCopy_ = OH_Drawing_CopyTypographyStyle(typoStyle_);
    ASSERT_NE(typoStyleCopy_, nullptr);

    // 接口OH_Drawing_TypographyGetTextStyle与OH_Drawing_SetTypographyTextStyle并不是配套接口。
    // set的是insideTextstyle，但是get是兜底的textstyle属性。
    txtStyleCopy_ = OH_Drawing_TypographyGetTextStyle(typoStyleCopy_);
    ASSERT_NE(txtStyleCopy_, nullptr);
    OH_Drawing_TextShadow* shadowsCopy = OH_Drawing_TextStyleGetShadows(txtStyleCopy_);
    ASSERT_NE(shadowsCopy, nullptr);
    EXPECT_NE(OH_Drawing_TextStyleGetShadowCount(txtStyleCopy_), OH_Drawing_TextStyleGetShadowCount(txtStyle_));
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowCount(txtStyleCopy_), 0);
    EXPECT_EQ(OH_Drawing_TextStyleGetShadowCount(txtStyle_), 3); // 3是因为OH_Drawing_TextStyleAddShadow加了三个shadow。
    OH_Drawing_PointDestroy(offset);
    OH_Drawing_DestroyTextShadow(shadow);
    OH_Drawing_DestroyTextShadow(shadowCopy);
    OH_Drawing_DestroyTextShadows(shadowsCopy);
}
} // namespace OHOS