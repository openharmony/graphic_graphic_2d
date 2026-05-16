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
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/DartTypes.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "rosen_text/typography.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
class NdkTypographyStyleTest : public testing::Test {
};

static skia::textlayout::ParagraphImpl* GetSkiaParagraph(OH_Drawing_Typography* typography)
{
    OHOS::Rosen::Typography* rosenTypography = reinterpret_cast<OHOS::Rosen::Typography*>(typography);
    OHOS::Rosen::SPText::ParagraphImpl* paragraph =
        reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(rosenTypography->GetParagraph());
    return reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraph->paragraph_.get());
}

static OH_Drawing_Typography* CreateParagraphWithCustomStyle(
    double layoutWidth, const char* text, OH_Drawing_TypographyStyle* typoStyle, OH_Drawing_TextStyle* textStyle)
{
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
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
 * @tc.name: FallbackLineSpacingEnglishLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for English when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingEnglishLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingLineLimitGreaterThanRun
 * @tc.desc: Test for maxLineHeight and minLineHeight greater than run height when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingLineLimitGreaterThanRun, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingHeightScaleShort
 * @tc.desc: Test for line height scale and line height shorter than run height when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingHeightScaleShort, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingHeightScaleLarge
 * @tc.desc: Test for line height scale and line height greater than run height when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingHeightScaleLarge, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingGetter
 * @tc.desc: Test for fallbackLineSpacing getter
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingGetter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingChineseLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for Chinese when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingChineseLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingEmojiLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for Emoji when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingEmojiLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingTibetanLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for Tibetan when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingTibetanLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingUyghurLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for Uyghur when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingUyghurLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingBurmeseLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for Burmese when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingBurmeseLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingJapaneseLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for Japanese when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingJapaneseLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingRussianLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight for Russian when open fallbackLineSpacing
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingRussianLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingKoreanLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight when open fallbackLineSpacing (Korean)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingKoreanLineLimitShorter, TestSize.Level0)
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
 * @tc.name: FallbackLineSpacingThaiLineLimitShorter
 * @tc.desc: Test for maxLineHeight and minLineHeight when open fallbackLineSpacing (Thai)
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, FallbackLineSpacingThaiLineLimitShorter, TestSize.Level0)
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
    // Expect line height is 80
    double expectLineHeight = 80;
    EXPECT_DOUBLE_EQ(std::ceil(runHeight), expectLineHeight);
    for (size_t lineIndex = 0; lineIndex < totalLineCnt; ++lineIndex) {
        EXPECT_DOUBLE_EQ(skiaParagraph->lines()[lineIndex].height(), expectLineHeight);
    }
    OH_Drawing_DestroyTypography(typography);
}
} // namespace OHOS
