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

#include "drawing_font_collection.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "typography.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    const double DEFAULT_FONT_SIZE = 50;
    const char* DEFAULT_TEXT = "text";
} // namespace

class OhDrawingTypographyPaintAttributeTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
protected:
    OH_Drawing_TypographyCreate* fHandler{nullptr};
    OH_Drawing_Typography* fTypography{nullptr};
    OH_Drawing_TypographyStyle* fTypoStyle{nullptr};
    OH_Drawing_TextStyle* fTxtStyle{nullptr};
    int fLayoutWidth{50};
};

static Typography* ConvertToOriginalText(OH_Drawing_Typography* style)
{
    return reinterpret_cast<Typography*>(style);
}

void OhDrawingTypographyPaintAttributeTest::SetUp()
{
    fTypoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(fTypoStyle, nullptr);
    OH_Drawing_SetTypographyTextFontSize(fTypoStyle, DEFAULT_FONT_SIZE);
    fHandler = OH_Drawing_CreateTypographyHandler(fTypoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(fHandler, nullptr);
    fTxtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(fTxtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(fTxtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, fTxtStyle);
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    fTypography = OH_Drawing_CreateTypography(fHandler);
    ASSERT_NE(fTypography, nullptr);
    OH_Drawing_TypographyLayout(fTypography, fLayoutWidth);
}

void OhDrawingTypographyPaintAttributeTest::TearDown()
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
    if (fTxtStyle != nullptr) {
        OH_Drawing_DestroyTextStyle(fTxtStyle);
        fTxtStyle = nullptr;
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateFontColorTest001
 * @tc.desc: test for updating the font color
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateFontColorTest001, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateFontColor(fTypography, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));
    OH_Drawing_TypographyUpdateFontColor(nullptr, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->color.CastToColorQuad(), Drawing::Color::ColorQuadSetARGB(255, 0, 255, 255));
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationTest001
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_LINE_THROUGH
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationTest001, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecoration(fTypography, TEXT_DECORATION_LINE_THROUGH);
    OH_Drawing_TypographyUpdateDecoration(nullptr, TEXT_DECORATION_LINE_THROUGH);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decoration, TextDecoration::LINE_THROUGH);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationTest002
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_UNDERLINE
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationTest002, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecoration(fTypography, TEXT_DECORATION_UNDERLINE);
    OH_Drawing_TypographyUpdateDecoration(nullptr, TEXT_DECORATION_UNDERLINE);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decoration, TextDecoration::UNDERLINE);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationTest003
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_OVERLINE
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationTest003, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecoration(fTypography, TEXT_DECORATION_OVERLINE);
    OH_Drawing_TypographyUpdateDecoration(nullptr, TEXT_DECORATION_OVERLINE);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decoration, TextDecoration::OVERLINE);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationTest004
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_NONE
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationTest004, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecoration(fTypography, TEXT_DECORATION_NONE);
    OH_Drawing_TypographyUpdateDecoration(nullptr, TEXT_DECORATION_NONE);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decoration, TextDecoration::NONE);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationThicknessScaleTest001
 * @tc.desc: test for updating the decoration thickness scale
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationThicknessScaleTest001,
    TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecorationThicknessScale(fTypography, 2.0);
    OH_Drawing_TypographyUpdateDecorationThicknessScale(nullptr, 2.0);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decorationThicknessScale, 2.0);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationStyleTest001
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_STYLE_DOTTED
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationStyleTest001, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecorationStyle(fTypography, TEXT_DECORATION_STYLE_DOTTED);
    OH_Drawing_TypographyUpdateDecorationStyle(nullptr, TEXT_DECORATION_STYLE_DOTTED);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::DOTTED);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationStyleTest002
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_STYLE_DOTTED
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationStyleTest002, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecorationStyle(fTypography, TEXT_DECORATION_STYLE_SOLID);
    OH_Drawing_TypographyUpdateDecorationStyle(nullptr, TEXT_DECORATION_STYLE_SOLID);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::SOLID);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationStyleTest003
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_STYLE_WAVY
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationStyleTest003, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecorationStyle(fTypography, TEXT_DECORATION_STYLE_WAVY);
    OH_Drawing_TypographyUpdateDecorationStyle(nullptr, TEXT_DECORATION_STYLE_WAVY);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::WAVY);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationStyleTest004
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_STYLE_DASHED
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationStyleTest004, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecorationStyle(fTypography, TEXT_DECORATION_STYLE_DASHED);
    OH_Drawing_TypographyUpdateDecorationStyle(nullptr, TEXT_DECORATION_STYLE_DASHED);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::DASHED);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateDecorationStyleTest005
 * @tc.desc: test for updating the decoration to the value of TEXT_DECORATION_STYLE_DOUBLE
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateDecorationStyleTest005, TestSize.Level1)
{
    OH_Drawing_TypographyUpdateDecorationStyle(fTypography, TEXT_DECORATION_STYLE_DOUBLE);
    OH_Drawing_TypographyUpdateDecorationStyle(nullptr, TEXT_DECORATION_STYLE_DOUBLE);

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::DOUBLE);
    }
}

/*
 * @tc.name: OhDrawingTypographyUpdateColorAndDecoration001
 * @tc.desc: test for updating the font color and decoration in mutiple text.
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyPaintAttributeTest, OhDrawingTypographyUpdateColorAndDecoration001, TestSize.Level1)
{
    OH_Drawing_SetTextStyleColor(fTxtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0x00));
    OH_Drawing_SetTextStyleDecoration(fTxtStyle, TEXT_DECORATION_NONE);
    OH_Drawing_SetTextStyleDecorationStyle(fTxtStyle, TEXT_DECORATION_STYLE_SOLID);
    OH_Drawing_SetTextStyleDecorationThicknessScale(fTxtStyle, 1.5);
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, fTxtStyle);
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);

    OH_Drawing_TypographyUpdateDecoration(fTypography, TEXT_DECORATION_LINE_THROUGH);
    OH_Drawing_TypographyUpdateDecorationStyle(fTypography, TEXT_DECORATION_STYLE_DOTTED);
    OH_Drawing_TypographyUpdateDecorationThicknessScale(fTypography, 2.0);
    OH_Drawing_TypographyUpdateFontColor(fTypography, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));

    Typography* typography = ConvertToOriginalText(fTypography);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->decoration, TextDecoration::LINE_THROUGH);
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::DOTTED);
        EXPECT_EQ(item.second.textStyle->decorationThicknessScale, 2.0);
        EXPECT_EQ(item.second.textStyle->color.CastToColorQuad(), Drawing::Color::ColorQuadSetARGB(255, 0, 255, 255));
    }
}
} // namespace OHOS