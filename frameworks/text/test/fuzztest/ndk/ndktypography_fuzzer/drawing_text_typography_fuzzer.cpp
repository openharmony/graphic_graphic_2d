/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "drawing_text_typography_fuzzer.h"

#include <cstddef>
#include <fuzzer/FuzzedDataProvider.h>

#include "unicode/unistr.h"
#include "utils/string_util.h"

namespace OHOS::Rosen::Drawing {
constexpr inline size_t DATA_MAX_LAYOUT_WIDTH = 100;

void SetTypographyStyle(OH_Drawing_TypographyStyle* typoStyle, FuzzedDataProvider& fdp)
{
    std::string str = fdp.ConsumeRandomLengthString();
    // 这是用于测试的字体家族字符串的二进制表示
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e,
        0x73 };
    const char* fontFamilies[] = { str.c_str() };
    const char* fontFamilies1[] = { fontFamiliesTest };

    OH_Drawing_SetTypographyTextDirection(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextAlign(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, 40);
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextHalfLeading(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextFontStyle(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextFontFamily(typoStyle, str.c_str());
    OH_Drawing_SetTypographyTextFontFamily(typoStyle, fontFamiliesTest);
    OH_Drawing_SetTypographyTextFontSize(typoStyle, 40.0f);
    OH_Drawing_SetTypographyTextFontHeight(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextHalfLeading(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, fdp.ConsumeBool());
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextLineStyleFontStyle(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 1, fontFamilies);
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 1, fontFamilies1);
    OH_Drawing_SetTypographyTextLineStyleFontSize(typoStyle, 40.0f);
    OH_Drawing_SetTypographyTextLineStyleFontHeight(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTypographyTextLineStyleOnly(typoStyle, fdp.ConsumeBool());
    OH_Drawing_SetTypographyTextLocale(typoStyle, str.c_str());
    OH_Drawing_SetTypographyTextSplitRatio(typoStyle, fdp.ConsumeIntegral<uint32_t>());
}

OH_Drawing_TypographyStyle* UpdateTypographyStyle(OH_Drawing_TypographyStyle* typoStyle, FuzzedDataProvider& fdp)
{
    OH_Drawing_TextStyle* textstyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextStyle(typoStyle, textstyle);
    OH_Drawing_DestroyTextStyle(textstyle);
    SetTypographyStyle(typoStyle, fdp);
    OH_Drawing_DestroyTextStyle(OH_Drawing_TypographyGetTextStyle(typoStyle));
    OH_Drawing_TypographyGetEffectiveAlignment(typoStyle);
    OH_Drawing_TypographyIsLineUnlimited(typoStyle);
    OH_Drawing_TypographyIsEllipsized(typoStyle);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TypographyTextSetHeightBehavior(
        typoStyle, static_cast<OH_Drawing_TextHeightBehavior>(fdp.ConsumeIntegral<uint8_t>()));
    OH_Drawing_TypographyTextGetHeightBehavior(typoStyle);
    OH_Drawing_TypographyTextGetLineStyle(typoStyle);
    OH_Drawing_TypographyTextlineStyleGetFontWeight(typoStyle);
    OH_Drawing_TypographyTextlineStyleGetFontStyle(typoStyle);
    size_t size;
    char** fontFamily = OH_Drawing_TypographyTextlineStyleGetFontFamilies(typoStyle, &size);
    OH_Drawing_TypographyTextlineStyleDestroyFontFamilies(fontFamily, size);
    OH_Drawing_TypographyTextlineStyleGetFontSize(typoStyle);
    OH_Drawing_TypographyTextlineStyleGetHeightScale(typoStyle);
    OH_Drawing_TypographyTextlineStyleGetHeightOnly(typoStyle);
    OH_Drawing_TypographyTextlineStyleGetHalfLeading(typoStyle);
    OH_Drawing_TypographyTextlineStyleGetSpacingScale(typoStyle);
    OH_Drawing_TypographyTextlineGetStyleOnly(typoStyle);
    OH_Drawing_TypographyGetTextAlign(typoStyle);
    OH_Drawing_TypographyGetTextDirection(typoStyle);
    OH_Drawing_TypographyGetTextMaxLines(typoStyle);
    char* ellipsis = OH_Drawing_TypographyGetTextEllipsis(typoStyle);
    OH_Drawing_TypographyDestroyEllipsis(ellipsis);
    OH_Drawing_TypographyStyleEquals(typoStyle, typoStyle);
    OH_Drawing_TypographyStyleGetEffectiveAlignment(typoStyle);
    OH_Drawing_TypographyStyleIsHintEnabled(typoStyle);
    OH_Drawing_StrutStyle* strutStyle = OH_Drawing_TypographyStyleGetStrutStyle(typoStyle);
    OH_Drawing_SetTypographyStyleTextStrutStyle(typoStyle, strutStyle);
    OH_Drawing_TypographyStyleDestroyStrutStyle(strutStyle);
    OH_Drawing_StrutStyle* getStrutStyle = OH_Drawing_TypographyStyleGetStrutStyle(typoStyle);
    OH_Drawing_TypographyStyleStrutStyleEquals(getStrutStyle, getStrutStyle);
    OH_Drawing_TypographyStyleDestroyStrutStyle(getStrutStyle);
    OH_Drawing_TypographyStyleSetHintsEnabled(typoStyle, fdp.ConsumeBool());
    std::shared_ptr<OH_Drawing_FontStyleStruct> structStyle = std::make_shared<OH_Drawing_FontStyleStruct>();
    if (fdp.remaining_bytes() >= sizeof(OH_Drawing_FontStyleStruct)) {
        fdp.ConsumeData(structStyle.get(), sizeof(OH_Drawing_FontStyleStruct));
    }
    OH_Drawing_SetTypographyStyleFontStyleStruct(typoStyle, *structStyle);
    OH_Drawing_TypographyStyleGetFontStyleStruct(typoStyle);
    OH_Drawing_TextTab* textTab =
        OH_Drawing_CreateTextTab(static_cast<OH_Drawing_TextAlign>(fdp.ConsumeIntegral<uint8_t>()), 0);
    OH_Drawing_GetTextTabAlignment(textTab);
    OH_Drawing_GetTextTabLocation(textTab);
    OH_Drawing_SetTypographyTextTab(typoStyle, textTab);
    OH_Drawing_DestroyTextTab(textTab);
    return typoStyle;
}

void SetTextStyle(OH_Drawing_TextStyle* txtStyle, FuzzedDataProvider& fdp)
{
    uint32_t red = fdp.ConsumeIntegral<uint32_t>();
    uint32_t gree = fdp.ConsumeIntegral<uint32_t>();
    uint32_t blue = fdp.ConsumeIntegral<uint32_t>();
    uint32_t alpha = fdp.ConsumeIntegral<uint32_t>();
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_SetTextStyleFontSize(txtStyle, 40.0f);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleBaseLine(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleDecoration(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleDecorationColor(txtStyle, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_SetTextStyleFontHeight(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleFontStyle(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    auto dest = fdp.ConsumeRandomLengthString();
    OH_Drawing_SetTextStyleLocale(txtStyle, dest.c_str());
    const char* fontFamilies[] = { dest.c_str() };
    // 这是用于测试的字体家族字符串的二进制表示
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e,
        0x73 };
    const char* fontFamilies1[] = { fontFamiliesTest };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleDecorationThicknessScale(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleEllipsisModal(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_SetTextStyleEllipsis(txtStyle, "...");
    OH_Drawing_TextStyleAddFontVariation(txtStyle, dest.c_str(), fdp.ConsumeIntegral<uint32_t>());
    auto brus = OH_Drawing_BrushCreate();
    auto pen = OH_Drawing_PenCreate();
    OH_Drawing_SetTextStyleForegroundBrush(txtStyle, brus);
    OH_Drawing_SetTextStyleBackgroundBrush(txtStyle, brus);
    OH_Drawing_SetTextStyleBackgroundPen(txtStyle, pen);
    OH_Drawing_SetTextStyleForegroundPen(txtStyle, pen);
    OH_Drawing_TextStyleGetBackgroundBrush(txtStyle, brus);
    OH_Drawing_TextStyleGetForegroundBrush(txtStyle, brus);
    OH_Drawing_TextStyleGetBackgroundPen(txtStyle, pen);
    OH_Drawing_TextStyleGetForegroundPen(txtStyle, pen);
    OH_Drawing_BrushDestroy(brus);
    OH_Drawing_PenDestroy(pen);
}

OH_Drawing_TextStyle* CreateTextStyle(OH_Drawing_TextStyle* txtStyle, FuzzedDataProvider& fdp)
{
    SetTextStyle(txtStyle, fdp);
    OH_Drawing_TextShadow* shadow = OH_Drawing_CreateTextShadow();
    OH_Drawing_Point* offset = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_SetTextShadow(shadow, fdp.ConsumeIntegral<uint32_t>(), offset, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TextStyleAddShadow(txtStyle, shadow);
    OH_Drawing_DestroyTextShadows(OH_Drawing_TextStyleGetShadows(txtStyle));
    OH_Drawing_TextStyleGetShadowCount(txtStyle);
    OH_Drawing_TextStyleGetShadowWithIndex(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TextStyleClearShadows(txtStyle);
    OH_Drawing_DestroyTextShadow(shadow);

    OH_Drawing_RectStyle_Info rectStyleInfo;
    std::string str = fdp.ConsumeRandomLengthString();
    OH_Drawing_TextStyleSetBackgroundRect(txtStyle, &rectStyleInfo, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TextStyleAddFontFeature(txtStyle, str.c_str(), fdp.ConsumeIntegral<uint32_t>());
    size_t fontFeaturesSize = OH_Drawing_TextStyleGetFontFeatureSize(txtStyle);
    OH_Drawing_FontFeature* fontFeatures = OH_Drawing_TextStyleGetFontFeatures(txtStyle);
    OH_Drawing_TextStyleDestroyFontFeatures(fontFeatures, fontFeaturesSize);
    OH_Drawing_TextStyleClearFontFeature(txtStyle);
    OH_Drawing_TextStyleSetBaselineShift(txtStyle, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TextStyleGetBaselineShift(txtStyle);
    OH_Drawing_TextStyleGetColor(txtStyle);
    OH_Drawing_TextStyleGetDecorationStyle(txtStyle);
    OH_Drawing_TextStyleGetFontWeight(txtStyle);
    OH_Drawing_TextStyleGetFontStyle(txtStyle);
    OH_Drawing_TextStyleGetBaseline(txtStyle);
    size_t familySize;
    char** fontFamily = OH_Drawing_TextStyleGetFontFamilies(txtStyle, &familySize);
    OH_Drawing_TextStyleDestroyFontFamilies(fontFamily, familySize);
    OH_Drawing_TextStyleGetFontSize(txtStyle);
    OH_Drawing_TextStyleGetLetterSpacing(txtStyle);
    OH_Drawing_TextStyleGetWordSpacing(txtStyle);
    OH_Drawing_TextStyleGetFontHeight(txtStyle);
    OH_Drawing_TextStyleGetHalfLeading(txtStyle);
    OH_Drawing_TextStyleGetLocale(txtStyle);
    OH_Drawing_TextStyle* comparedStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TextStyleIsEqual(txtStyle, comparedStyle);
    OH_Drawing_TextStyleIsEqualByFont(txtStyle, comparedStyle);
    OH_Drawing_TextStyleIsAttributeMatched(
        txtStyle, comparedStyle, static_cast<OH_Drawing_TextStyleType>(fdp.ConsumeIntegral<uint8_t>()));
    OH_Drawing_TextStyleSetPlaceholder(txtStyle);
    OH_Drawing_TextStyleIsPlaceholder(txtStyle);
    std::shared_ptr<OH_Drawing_FontStyleStruct> structStyle = std::make_shared<OH_Drawing_FontStyleStruct>();
    if (fdp.remaining_bytes() >= sizeof(OH_Drawing_FontStyleStruct)) {
        fdp.ConsumeData(structStyle.get(), sizeof(OH_Drawing_FontStyleStruct));
    }
    OH_Drawing_SetTextStyleFontStyleStruct(txtStyle, *structStyle);
    OH_Drawing_TextStyleGetFontStyleStruct(txtStyle);
    OH_Drawing_DestroyTextStyle(comparedStyle);
    OH_Drawing_PointDestroy(offset);
    return txtStyle;
}

OH_Drawing_TypographyCreate* CreateTypographyHandler(OH_Drawing_TypographyCreate* handler,
    OH_Drawing_TypographyStyle* typoStyle, OH_Drawing_TextStyle* txtStyle, FuzzedDataProvider& fdp)
{
    int width = 10.0;
    int height = 5.0;
    OH_Drawing_PlaceholderSpan placeholder = { .width = width,
        .height = height,
        .alignment = ALIGNMENT_OFFSET_AT_BASELINE,
        .baseline = TEXT_BASELINE_ALPHABETIC,
        .baselineOffset = 0 };
    OH_Drawing_TypographyHandlerAddPlaceholder(handler, &placeholder);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    std::string dest = fdp.ConsumeBytesAsString(DATA_MAX_LAYOUT_WIDTH);
    std::u16string u16 = Str8ToStr16ByIcu(dest);
    icu::UnicodeString unicodeString(u16.data(), u16.size());
    unicodeString.toUTF8String(dest);
    OH_Drawing_TypographyHandlerAddText(handler, dest.c_str());
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    return handler;
}

void OHDrawingRange(OH_Drawing_Typography* typography, FuzzedDataProvider& fdp)
{
    OH_Drawing_Range* range = OH_Drawing_TypographyGetWordBoundary(typography, fdp.ConsumeIntegral<size_t>());
    OH_Drawing_GetStartFromRange(range);
    OH_Drawing_GetEndFromRange(range);
    free(range);
}

void OHDrawingPositionAndAffinity(OH_Drawing_Typography* typography, FuzzedDataProvider& fdp)
{
    OH_Drawing_PositionAndAffinity* positionAffinity = OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(
        typography, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_GetPositionFromPositionAndAffinity(positionAffinity);
    OH_Drawing_GetAffinityFromPositionAndAffinity(positionAffinity);
    free(positionAffinity);
}

OH_Drawing_Typography* CreateTypography(
    OH_Drawing_Typography* typography, OH_Drawing_TypographyCreate* handler, FuzzedDataProvider& fdp)
{
    OH_Drawing_TypographyLayout(typography, DATA_MAX_LAYOUT_WIDTH);
    OH_Drawing_TypographyGetMaxWidth(typography);
    OH_Drawing_TypographyGetHeight(typography);
    OH_Drawing_TypographyGetLongestLine(typography);
    OH_Drawing_TypographyGetMinIntrinsicWidth(typography);
    OH_Drawing_TypographyGetMaxIntrinsicWidth(typography);
    OH_Drawing_TypographyGetAlphabeticBaseline(typography);
    OH_Drawing_TypographyGetIdeographicBaseline(typography);
    OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    OH_Drawing_TextBox* textBox = OH_Drawing_TypographyGetRectsForRange(typography, fdp.ConsumeIntegral<size_t>(),
        fdp.ConsumeIntegral<size_t>(), static_cast<OH_Drawing_RectHeightStyle>(fdp.ConsumeIntegral<uint8_t>()),
        static_cast<OH_Drawing_RectWidthStyle>(fdp.ConsumeIntegral<uint8_t>()));
    OH_Drawing_TypographyDestroyTextBox(OH_Drawing_TypographyGetRectsForPlaceholders(typography));
    OH_Drawing_GetLeftFromTextBox(textBox, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_GetRightFromTextBox(textBox, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_GetTopFromTextBox(textBox, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_GetBottomFromTextBox(textBox, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_GetTextDirectionFromTextBox(textBox, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_GetSizeOfTextBox(textBox);
    OH_Drawing_TypographyDestroyTextBox(textBox);
    free(OH_Drawing_TypographyGetGlyphPositionAtCoordinate(
        typography, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>()));
    OHDrawingPositionAndAffinity(typography, fdp);
    OHDrawingRange(typography, fdp);
    OH_Drawing_TypographyGetLineCount(typography);
    OH_Drawing_TypographyGetLineHeight(typography, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TypographyGetLineWidth(typography, fdp.ConsumeIntegral<uint32_t>());
    free(OH_Drawing_TypographyGetLineTextRange(typography, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeBool()));
    OH_Drawing_LineMetrics* lineMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    OH_Drawing_LineMetricsGetSize(lineMetrics);
    OH_Drawing_TypographyGetLineInfo(
        typography, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeBool(), fdp.ConsumeBool(), lineMetrics);
    std::shared_ptr<OH_Drawing_LineMetrics> metrics = std::make_shared<OH_Drawing_LineMetrics>();
    if (fdp.remaining_bytes() >= sizeof(OH_Drawing_LineMetrics)) {
        fdp.ConsumeData(metrics.get(), sizeof(OH_Drawing_LineMetrics));
    }
    OH_Drawing_TypographyGetLineMetricsAt(typography, fdp.ConsumeIntegral<uint32_t>(), metrics.get());
    float indents[] = { fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>() };
    OH_Drawing_TypographySetIndents(typography, sizeof(indents) / sizeof(float), indents);
    OH_Drawing_TypographyGetIndentsWithIndex(typography, fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TypographyMarkDirty(typography);
    OH_Drawing_TypographyGetUnresolvedGlyphsCount(typography);
    OH_Drawing_TypographyUpdateFontSize(
        typography, fdp.ConsumeIntegral<size_t>(), fdp.ConsumeIntegral<size_t>(), 40.0f + 1);
    size_t size;
    OH_Drawing_Font_Metrics* fontMetrics =
        OH_Drawing_TypographyGetLineFontMetrics(typography, fdp.ConsumeIntegral<size_t>(), &size);
    OH_Drawing_TypographyDestroyLineFontMetrics(fontMetrics);
    OH_Drawing_DestroyLineMetrics(lineMetrics);
    return typography;
}

OH_Drawing_Bitmap* CreateBitmap()
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    // 这里使用随机的 width 和 width 可能会 crash ，所以暂时使用常量 20
    OH_Drawing_BitmapBuild(bitmap, 20, 20, &cFormat);
    OH_Drawing_BitmapGetWidth(bitmap);
    OH_Drawing_BitmapGetHeight(bitmap);
    return bitmap;
}

OH_Drawing_Canvas* CreateCanvas(OH_Drawing_Bitmap* bitmap, FuzzedDataProvider& fdp)
{
    uint32_t red = fdp.ConsumeIntegral<uint32_t>();
    uint32_t gree = fdp.ConsumeIntegral<uint32_t>();
    uint32_t blue = fdp.ConsumeIntegral<uint32_t>();
    uint32_t alpha = fdp.ConsumeIntegral<uint32_t>();
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    return canvas;
}

void OHDrawingParserTest(FuzzedDataProvider& fdp)
{
    OH_Drawing_FontParser* parser = OH_Drawing_CreateFontParser();
    size_t listSize;
    char** fontList = OH_Drawing_FontParserGetSystemFontList(parser, &listSize);
    OH_Drawing_DestroySystemFontList(fontList, listSize);
    std::string dest = fdp.ConsumeRandomLengthString();
    OH_Drawing_FontParserGetFontByName(parser, dest.c_str());
    OH_Drawing_DestroyFontParser(parser);
}

void OHDrawingTypographyTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_SetTextUndefinedGlyphDisplay(
        static_cast<OH_Drawing_TextUndefinedGlyphDisplay>(fdp.ConsumeIntegral<uint8_t>()));
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    UpdateTypographyStyle(typoStyle, fdp);
    UpdateTypographyStyle(nullptr, fdp);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    CreateTextStyle(txtStyle, fdp);
    CreateTextStyle(nullptr, fdp);
    OH_Drawing_FontCollection* myFontCollection = OH_Drawing_CreateFontCollection();
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, myFontCollection);
    OH_Drawing_DestroyFontCollection(myFontCollection);
    CreateTypographyHandler(handler, typoStyle, txtStyle, fdp);
    CreateTypographyHandler(nullptr, nullptr, nullptr, fdp);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    CreateTypography(typography, handler, fdp);
    CreateTypography(nullptr, nullptr, fdp);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_TextStyleGetFontMetrics(typography, txtStyle, &fontMetrics);
    double position[] = { fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>() };
    OH_Drawing_Bitmap* bitmap = CreateBitmap();
    OH_Drawing_Canvas* canvas = CreateCanvas(bitmap, fdp);
    OH_Drawing_CanvasDestroy(CreateCanvas(nullptr, fdp));
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(path, 0, 0, 1, 1, 0, 0);
    OH_Drawing_TypographyPaintOnPath(
        typography, canvas, nullptr, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_TypographyPaint(typography, canvas, position[0], position[1]);
    OH_Drawing_FontDescriptor* fontDescriptor = OH_Drawing_CreateFontDescriptor();
    OH_Drawing_DestroyFontDescriptor(fontDescriptor);
    OH_Drawing_FontConfigInfoErrorCode errorCode;
    OH_Drawing_FontConfigInfo* fontConfig = OH_Drawing_GetSystemFontConfigInfo(&errorCode);
    OH_Drawing_DestroySystemFontConfigInfo(fontConfig);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_PathDestroy(path);
}
} // namespace OHOS::Rosen::Drawing

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawingTypographyTest(data, size);
    return 0;
}