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
#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "rosen_text/typography.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace {
const double DEFAULT_LAYOUT_WIDTH = 1000;
} // namespace
class NdkTypographyStyleTest : public testing::Test {
};

static skia::textlayout::ParagraphImpl* GetSkiaParagraph(OH_Drawing_Typography* typography)
{
    OHOS::Rosen::Typography* rosenTypography = reinterpret_cast<OHOS::Rosen::Typography*>(typography);
    OHOS::Rosen::SPText::ParagraphImpl* paragraph =
        reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(rosenTypography->GetParagraph());
    return reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get());
}

static OH_Drawing_Typography* CreateParagraphWithCustomStyle(double layoutWidth, const char* text,
    OH_Drawing_TypographyStyle* typoStyle, OH_Drawing_TextStyle* textStyle)
{
    if (typoStyle == nullptr) {
        typoStyle = OH_Drawing_CreateTypographyStyle();
    }
    if (textStyle == nullptr) {
        textStyle = OH_Drawing_CreateTextStyle();
    }
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_GetFontCollectionGlobalInstance();
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, layoutWidth);
    OH_Drawing_DestroyTypographyHandler(handler);
    return typography;
}

/*
 * @tc.name: IncludeFontPaddingEnglishHello
 * @tc.desc: test for includeFontPadding with English single-line text "Hello".
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingEnglishHello, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "Hello";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingEnglishMultiLine
 * @tc.desc: test for includeFontPadding with English multi-line text "Hello World".
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingEnglishMultiLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "Hello World";
    constexpr double layoutWidth = 20;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 20);
    EXPECT_DOUBLE_EQ(middleLineHeight, 16);
    EXPECT_DOUBLE_EQ(lastLineHeight, 18);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingBurmeseMultiLine
 * @tc.desc: test for includeFontPadding with Burmese multi-line text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingBurmeseMultiLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);

    const char* text = "ရှည်လျားသောသမိုင်းရှိရုံသာမကအလွန်ကြွယ်ဝ။.";
    constexpr double layoutWidth = 50;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 122);
    EXPECT_DOUBLE_EQ(middleLineHeight, 109);
    EXPECT_DOUBLE_EQ(lastLineHeight, 115);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingUyghurMultiLine
 * @tc.desc: test for includeFontPadding with Uyghur multi-line text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingUyghurMultiLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);

    const char* text = "سوتسىيالىستىك فېدېراتسىيە جۇمھۇرىيىتىنىڭ ئورتاق تىلى";
    constexpr double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 88);
    EXPECT_DOUBLE_EQ(middleLineHeight, 75);
    EXPECT_DOUBLE_EQ(lastLineHeight, 81);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingLineHeightShorterThanRun
 * @tc.desc: test for includeFontPadding when line height is shorter than run height.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingLineHeightShorterThanRun, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle, TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 30);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle, TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, 30);

    const char* text = "Hello World";
    constexpr double layoutWidth = 20;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 43);
    EXPECT_DOUBLE_EQ(middleLineHeight, 30);
    EXPECT_DOUBLE_EQ(lastLineHeight, 35);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingLineHeightGreaterThanRun
 * @tc.desc: test for includeFontPadding when line height is greater than run height.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingLineHeightGreaterThanRun, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle, TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, 80);
    OH_Drawing_SetTextStyleAttributeDouble(textStyle, TEXT_STYLE_ATTR_D_LINE_HEIGHT_MINIMUM, 80);

    const char* text = "Hello World";
    constexpr double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 93);
    EXPECT_DOUBLE_EQ(middleLineHeight, 80);
    EXPECT_DOUBLE_EQ(lastLineHeight, 85);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingLineHeightScaleShort
 * @tc.desc: test for includeFontPadding with short line height scale.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingLineHeightScaleShort, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    OH_Drawing_SetTextStyleFontHeight(textStyle, 0.8);

    const char* text = "fontPadding&lineHeightScale测试";
    constexpr double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 53);
    EXPECT_DOUBLE_EQ(middleLineHeight, 40);
    EXPECT_DOUBLE_EQ(lastLineHeight, 45);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingLineHeightScaleLarge
 * @tc.desc: test for includeFontPadding with large line height scale.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingLineHeightScaleLarge, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    OH_Drawing_SetTextStyleFontHeight(textStyle, 2);

    const char* text = "fontPadding&lineHeightScale测试";
    constexpr double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 113);
    EXPECT_DOUBLE_EQ(middleLineHeight, 100);
    EXPECT_DOUBLE_EQ(lastLineHeight, 105);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingDisableAllHeightBehavior
 * @tc.desc: test for includeFontPadding with disable all height behavior.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingDisableAllHeightBehavior, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, TEXT_HEIGHT_DISABLE_ALL);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 50);
    OH_Drawing_SetTextStyleFontHeight(textStyle, 2);

    const char* text = "fontPadding&lineHeightScale测试";
    constexpr double layoutWidth = 80;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 80);
    EXPECT_DOUBLE_EQ(middleLineHeight, 100);
    EXPECT_DOUBLE_EQ(lastLineHeight, 97);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingGetter
 * @tc.desc: test for getting includeFontPadding attribute value.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingGetter, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);
    bool value = false;
    OH_Drawing_ErrorCode errCode = OH_Drawing_GetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, &value);
    EXPECT_EQ(errCode, OH_DRAWING_SUCCESS);
    EXPECT_TRUE(value);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingTibetanMultiLine
 * @tc.desc: test for includeFontPadding with Tibetan multi-line text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingTibetanMultiLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 100);

    const char* text = "ག་གནས་ཀྱི་འདུགཡུལ";
    constexpr double layoutWidth = 300;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    EXPECT_EQ(totalLineCnt, 3);
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 143);
    EXPECT_DOUBLE_EQ(middleLineHeight, 117);
    EXPECT_DOUBLE_EQ(lastLineHeight, 128);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingDevanagariMultiLine
 * @tc.desc: test for includeFontPadding with Devanagari multi-line text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingDevanagariMultiLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 80);

    const char* text = "यह एक परीक्षण वाक्य है जो कई पंक्तियों में टूट जाएगा";
    constexpr double layoutWidth = 300;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    EXPECT_EQ(totalLineCnt, 6);
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 127);
    EXPECT_DOUBLE_EQ(middleLineHeight, 107);
    EXPECT_DOUBLE_EQ(lastLineHeight, 116);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingThaiMultiLine
 * @tc.desc: test for includeFontPadding with Thai multi-line text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingThaiMultiLine, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(textStyle, 80);

    const char* text = "ภาษาไทยไม่มีการเว้นวรรคทำให้ต้องตัดคำตามพจนานุกรม";
    constexpr double layoutWidth = 280;
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(layoutWidth, text, typoStyle, textStyle);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    size_t totalLineCnt = skiaParagraph->lineNumber();
    EXPECT_EQ(totalLineCnt, 9);
    double firstLineHeight = skiaParagraph->lines()[0].height();
    double middleLineHeight = skiaParagraph->lines()[1].height();
    double lastLineHeight = skiaParagraph->lines()[totalLineCnt - 1].height();
    EXPECT_DOUBLE_EQ(firstLineHeight, 148);
    EXPECT_DOUBLE_EQ(middleLineHeight, 128);
    EXPECT_DOUBLE_EQ(lastLineHeight, 136);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

/*
 * @tc.name: IncludeFontPaddingJapanese
 * @tc.desc: test for includeFontPadding with Japanese text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingJapanese, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "こんにちは";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingKorean
 * @tc.desc: test for includeFontPadding with Korean text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingKorean, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "안녕하세요";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 25);
    EXPECT_DOUBLE_EQ(runHeight, 20.271999359130859);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingThai
 * @tc.desc: test for includeFontPadding with Thai text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingThai, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "สวัสดี";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 27);
    EXPECT_DOUBLE_EQ(runHeight, 22.371999740600586);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingChinese
 * @tc.desc: test for includeFontPadding with Chinese text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingChinese, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "你好世界";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingEmoji
 * @tc.desc: test for includeFontPadding with Emoji text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingEmoji, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "😀😁😂🤣😃";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    EXPECT_DOUBLE_EQ(runHeight, 16.440366744995117);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingSymbols
 * @tc.desc: test for includeFontPadding with mathematical symbols text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingSymbols, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "∑∏√∞≈≠≤≥";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingTibetan
 * @tc.desc: test for includeFontPadding with Tibetan text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingTibetan, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "བོད་ཡིག";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingUyghur
 * @tc.desc: test for includeFontPadding with Uyghur text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingUyghur, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "ئۇيغۇرچە";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 26);
    EXPECT_DOUBLE_EQ(runHeight, 21.027999877929688);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingBurmese
 * @tc.desc: test for includeFontPadding with Burmese text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingBurmese, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "မင်္ဂလာပါ";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 36);
    EXPECT_DOUBLE_EQ(runHeight, 30.576000213623047);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: IncludeFontPaddingRussian
 * @tc.desc: test for includeFontPadding with Russian text.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, IncludeFontPaddingRussian, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle,
        TYPOGRAPHY_STYLE_ATTR_B_INCLUDE_FONT_PADDING, true);

    const char* text = "межстрочного";
    OH_Drawing_Typography* typography = CreateParagraphWithCustomStyle(DEFAULT_LAYOUT_WIDTH, text, typoStyle, nullptr);
    ASSERT_NE(typography, nullptr);

    skia::textlayout::ParagraphImpl* skiaParagraph = GetSkiaParagraph(typography);
    ASSERT_NE(skiaParagraph, nullptr);
    double runHeight = skiaParagraph->runs()[0].calculateHeight(
        skia::textlayout::LineMetricStyle::Typographic, skia::textlayout::LineMetricStyle::Typographic);
    double lineHeight = skiaParagraph->lines()[0].height();
    EXPECT_DOUBLE_EQ(lineHeight, 22);
    EXPECT_DOUBLE_EQ(runHeight, 16.407999038696289);

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
} // namespace OHOS
