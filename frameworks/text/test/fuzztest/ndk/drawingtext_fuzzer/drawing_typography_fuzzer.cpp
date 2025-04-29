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

#include <cstddef>

#include "drawing_text_fuzzer.h"
namespace OHOS::Rosen::Drawing {
std::unique_ptr<char[]> GetRandomString()
{
    // 使用随机的 char 数组可能会 crash ，所以暂时使用常量
    const char s[] = "你好";
    std::unique_ptr dest = std::make_unique<char[]>(sizeof(s));
    dest.get()[sizeof(s) - 1] = 0;
    strcpy_s(dest.get(), sizeof(s), s);
    return dest;
}

void SetTypographyStyle(OH_Drawing_TypographyStyle* typoStyle)
{
    std::unique_ptr str = GetRandomString();
    // 这是用于测试的字体家族字符串的二进制表示
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e,
        0x73 };
    const char* fontFamilies[] = { str.get() };
    const char* fontFamilies1[] = { fontFamiliesTest };

    OH_Drawing_SetTypographyTextDirection(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE1);
    OH_Drawing_SetTypographyTextAlign(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE4);
    OH_Drawing_SetTypographyTextMaxLines(typoStyle, DATA_MAX_ENUM_SIZE3);
    OH_Drawing_SetTypographyTextBreakStrategy(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE2);
    OH_Drawing_SetTypographyTextWordBreakType(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE2);
    OH_Drawing_SetTypographyTextHalfLeading(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE2);
    OH_Drawing_SetTypographyTextFontWeight(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE5);
    OH_Drawing_SetTypographyTextFontStyle(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE2);
    OH_Drawing_SetTypographyTextFontFamily(typoStyle, str.get());
    OH_Drawing_SetTypographyTextFontFamily(typoStyle, fontFamiliesTest);
    OH_Drawing_SetTypographyTextFontSize(typoStyle, DATA_MAX_ENUM_FONTSIZE);
    OH_Drawing_SetTypographyTextFontHeight(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTypographyTextHalfLeading(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTypographyTextUseLineStyle(typoStyle, GetObject<bool>());
    OH_Drawing_SetTypographyTextLineStyleFontWeight(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE5);
    OH_Drawing_SetTypographyTextLineStyleFontStyle(typoStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE2);
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 1, fontFamilies);
    OH_Drawing_SetTypographyTextLineStyleFontFamilies(typoStyle, 1, fontFamilies1);
    OH_Drawing_SetTypographyTextLineStyleFontSize(typoStyle, DATA_MAX_ENUM_FONTSIZE);
    OH_Drawing_SetTypographyTextLineStyleFontHeight(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTypographyTextLineStyleHalfLeading(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTypographyTextLineStyleSpacingScale(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTypographyTextLineStyleOnly(typoStyle, GetObject<bool>());
    OH_Drawing_SetTypographyTextLocale(typoStyle, str.get());
    OH_Drawing_SetTypographyTextSplitRatio(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
}

OH_Drawing_TypographyStyle* UpdateTypographyStyle(OH_Drawing_TypographyStyle* typoStyle)
{
    OH_Drawing_TextStyle* textstyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_SetTypographyTextStyle(typoStyle, textstyle);
    OH_Drawing_DestroyTextStyle(textstyle);
    SetTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(OH_Drawing_TypographyGetTextStyle(typoStyle));
    OH_Drawing_TypographyGetEffectiveAlignment(typoStyle);
    OH_Drawing_TypographyIsLineUnlimited(typoStyle);
    OH_Drawing_TypographyIsEllipsized(typoStyle);
    OH_Drawing_SetTypographyTextEllipsisModal(typoStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_TypographyTextSetHeightBehavior(typoStyle, GetObject<OH_Drawing_TextHeightBehavior>());
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
    OH_Drawing_TypographyStyleSetHintsEnabled(typoStyle, GetObject<bool>());
    OH_Drawing_FontStyleStruct structStyle = GetObject<OH_Drawing_FontStyleStruct>();
    OH_Drawing_SetTypographyStyleFontStyleStruct(typoStyle, structStyle);
    OH_Drawing_TypographyStyleGetFontStyleStruct(typoStyle);
    OH_Drawing_TextTab* textTab = OH_Drawing_CreateTextTab(GetObject<OH_Drawing_TextAlign>(), 0);
    OH_Drawing_GetTextTabAlignment(textTab);
    OH_Drawing_GetTextTabLocation(textTab);
    OH_Drawing_SetTypographyTextTab(typoStyle, textTab);
    OH_Drawing_DestroyTextTab(textTab);
    return typoStyle;
}

void SetTextStyle(OH_Drawing_TextStyle* txtStyle)
{
    uint32_t red = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    uint32_t gree = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    uint32_t blue = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    uint32_t alpha = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_SetTextStyleFontSize(txtStyle, DATA_MAX_ENUM_FONTSIZE);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE5);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE1);
    OH_Drawing_SetTextStyleDecoration(txtStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE3);
    OH_Drawing_SetTextStyleDecorationColor(txtStyle, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_SetTextStyleFontHeight(txtStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTextStyleFontStyle(txtStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE3);
    std::unique_ptr dest = GetRandomString();
    OH_Drawing_SetTextStyleLocale(txtStyle, dest.get());
    const char* fontFamilies[] = { dest.get() };
    // 这是用于测试的字体家族字符串的二进制表示
    const char fontFamiliesTest[] = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x5f, 0x53, 0x61, 0x6e,
        0x73 };
    const char* fontFamilies1[] = { fontFamiliesTest };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_SetTextStyleDecorationStyle(txtStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE3);
    OH_Drawing_SetTextStyleDecorationThicknessScale(txtStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTextStyleLetterSpacing(txtStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTextStyleWordSpacing(txtStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTextStyleHalfLeading(txtStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_SetTextStyleEllipsisModal(txtStyle, GetObject<uint32_t>() % DATA_MAX_ENUM_SIZE2);
    OH_Drawing_SetTextStyleEllipsis(txtStyle, dest.get());
    OH_Drawing_TextStyleAddFontVariation(txtStyle, dest.get(), GetObject<uint32_t>() % DATA_MAX_RANDOM);
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

OH_Drawing_TextStyle* CreateTextStyle(OH_Drawing_TextStyle* txtStyle)
{
    SetTextStyle(txtStyle);
    OH_Drawing_TextShadow* shadow = OH_Drawing_CreateTextShadow();
    OH_Drawing_Point* offset = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_SetTextShadow(
        shadow, GetObject<uint32_t>() % DATA_MAX_RANDOM, offset, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_TextStyleAddShadow(txtStyle, shadow);
    OH_Drawing_DestroyTextShadows(OH_Drawing_TextStyleGetShadows(txtStyle));
    OH_Drawing_TextStyleGetShadowCount(txtStyle);
    OH_Drawing_TextStyleGetShadowWithIndex(txtStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_TextStyleClearShadows(txtStyle);
    OH_Drawing_DestroyTextShadow(shadow);

    OH_Drawing_RectStyle_Info rectStyleInfo;
    OH_Drawing_TextStyleSetBackgroundRect(txtStyle, &rectStyleInfo, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_TextStyleAddFontFeature(txtStyle, GetRandomString().get(), GetObject<uint32_t>() % DATA_MAX_RANDOM);
    size_t fontFeaturesSize = OH_Drawing_TextStyleGetFontFeatureSize(txtStyle);
    OH_Drawing_FontFeature* fontFeatures = OH_Drawing_TextStyleGetFontFeatures(txtStyle);
    OH_Drawing_TextStyleDestroyFontFeatures(fontFeatures, fontFeaturesSize);
    OH_Drawing_TextStyleClearFontFeature(txtStyle);
    OH_Drawing_TextStyleSetBaselineShift(txtStyle, GetObject<uint32_t>() % DATA_MAX_RANDOM);
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
    OH_Drawing_TextStyleIsAttributeMatched(txtStyle, comparedStyle, GetObject<OH_Drawing_TextStyleType>());
    OH_Drawing_TextStyleSetPlaceholder(txtStyle);
    OH_Drawing_TextStyleIsPlaceholder(txtStyle);
    OH_Drawing_FontStyleStruct structStyle = GetObject<OH_Drawing_FontStyleStruct>();
    OH_Drawing_SetTextStyleFontStyleStruct(txtStyle, structStyle);
    OH_Drawing_TextStyleGetFontStyleStruct(txtStyle);
    OH_Drawing_DestroyTextStyle(comparedStyle);
    OH_Drawing_PointDestroy(offset);
    return txtStyle;
}

OH_Drawing_TypographyCreate* CreateTypographyHandler(
    OH_Drawing_TypographyCreate* handler, OH_Drawing_TypographyStyle* typoStyle, OH_Drawing_TextStyle* txtStyle)
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
    std::unique_ptr dest = GetRandomString();
    OH_Drawing_TypographyHandlerAddText(handler, dest.get());
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    return handler;
}

void OHDrawingRange(OH_Drawing_Typography* typography)
{
    OH_Drawing_Range* range = OH_Drawing_TypographyGetWordBoundary(typography, GetObject<size_t>() % DATA_MAX_RANDOM);
    OH_Drawing_GetStartFromRange(range);
    OH_Drawing_GetEndFromRange(range);
    free(range);
}

void OHDrawingPositionAndAffinity(OH_Drawing_Typography* typography)
{
    OH_Drawing_PositionAndAffinity* positionAffinity = OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(
        typography, GetObject<uint32_t>() % DATA_MAX_RANDOM, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_GetPositionFromPositionAndAffinity(positionAffinity);
    OH_Drawing_GetAffinityFromPositionAndAffinity(positionAffinity);
    free(positionAffinity);
}

OH_Drawing_Typography* CreateTypography(OH_Drawing_Typography* typography, OH_Drawing_TypographyCreate* handler)
{
    OH_Drawing_TypographyLayout(typography, DATA_MAX_RANDOM);
    OH_Drawing_TypographyGetMaxWidth(typography);
    OH_Drawing_TypographyGetHeight(typography);
    OH_Drawing_TypographyGetLongestLine(typography);
    OH_Drawing_TypographyGetMinIntrinsicWidth(typography);
    OH_Drawing_TypographyGetMaxIntrinsicWidth(typography);
    OH_Drawing_TypographyGetAlphabeticBaseline(typography);
    OH_Drawing_TypographyGetIdeographicBaseline(typography);
    OH_Drawing_TypographyGetLongestLineWithIndent(typography);
    OH_Drawing_TextBox* textBox = OH_Drawing_TypographyGetRectsForRange(typography,
        GetObject<size_t>() % DATA_MAX_RANDOM, GetObject<size_t>() % DATA_MAX_RANDOM,
        GetObject<OH_Drawing_RectHeightStyle>(), GetObject<OH_Drawing_RectWidthStyle>());
    OH_Drawing_TypographyDestroyTextBox(OH_Drawing_TypographyGetRectsForPlaceholders(typography));
    OH_Drawing_GetLeftFromTextBox(textBox, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_GetRightFromTextBox(textBox, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_GetTopFromTextBox(textBox, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_GetBottomFromTextBox(textBox, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_GetTextDirectionFromTextBox(textBox, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_GetSizeOfTextBox(textBox);
    OH_Drawing_TypographyDestroyTextBox(textBox);
    free(OH_Drawing_TypographyGetGlyphPositionAtCoordinate(
        typography, GetObject<uint32_t>() % DATA_MAX_RANDOM, GetObject<uint32_t>() % DATA_MAX_RANDOM));
    OHDrawingPositionAndAffinity(typography);
    OHDrawingRange(typography);
    OH_Drawing_TypographyGetLineCount(typography);
    OH_Drawing_TypographyGetLineHeight(typography, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_TypographyGetLineWidth(typography, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    free(OH_Drawing_TypographyGetLineTextRange(typography, GetObject<uint32_t>() % DATA_MAX_RANDOM, GetObject<bool>()));
    OH_Drawing_LineMetrics* lineMetrics = OH_Drawing_TypographyGetLineMetrics(typography);
    OH_Drawing_LineMetricsGetSize(lineMetrics);
    OH_Drawing_TypographyGetLineInfo(
        typography, GetObject<uint32_t>() % DATA_MAX_RANDOM, GetObject<bool>(), GetObject<bool>(), lineMetrics);
    OH_Drawing_LineMetrics metrics = GetObject<OH_Drawing_LineMetrics>();
    OH_Drawing_TypographyGetLineMetricsAt(typography, GetObject<uint32_t>() % DATA_MAX_RANDOM, &metrics);
    float indents[] = { GetObject<uint32_t>() % DATA_MAX_RANDOM, GetObject<uint32_t>() % DATA_MAX_RANDOM };
    OH_Drawing_TypographySetIndents(typography, sizeof(indents) / sizeof(float), indents);
    OH_Drawing_TypographyGetIndentsWithIndex(typography, GetObject<uint32_t>() % DATA_MAX_RANDOM);
    OH_Drawing_TypographyMarkDirty(typography);
    OH_Drawing_TypographyGetUnresolvedGlyphsCount(typography);
    OH_Drawing_TypographyUpdateFontSize(typography, GetObject<size_t>() % DATA_MAX_RANDOM,
        GetObject<size_t>() % DATA_MAX_RANDOM, DATA_MAX_ENUM_FONTSIZE + 1);
    size_t size;
    OH_Drawing_Font_Metrics* fontMetrics =
        OH_Drawing_TypographyGetLineFontMetrics(typography, GetObject<size_t>() % DATA_MAX_RANDOM, &size);
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

OH_Drawing_Canvas* CreateCanvas(OH_Drawing_Bitmap* bitmap)
{
    uint32_t red = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    uint32_t gree = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    uint32_t blue = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    uint32_t alpha = GetObject<uint32_t>() % DATA_MAX_RANDOM;
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    return canvas;
}

void OHDrawingParserTest()
{
    OH_Drawing_FontParser* parser = OH_Drawing_CreateFontParser();
    size_t listSize;
    char** fontList = OH_Drawing_FontParserGetSystemFontList(parser, &listSize);
    OH_Drawing_DestroySystemFontList(fontList, listSize);
    std::unique_ptr dest = GetRandomString();
    OH_Drawing_FontParserGetFontByName(parser, dest.get());
    OH_Drawing_DestroyFontParser(parser);
}

void OHDrawingTypographyTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    UpdateTypographyStyle(typoStyle);
    UpdateTypographyStyle(nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    CreateTextStyle(txtStyle);
    CreateTextStyle(nullptr);
    OH_Drawing_FontCollection* myFontCollection = OH_Drawing_CreateFontCollection();
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, myFontCollection);
    OH_Drawing_DestroyFontCollection(myFontCollection);
    CreateTypographyHandler(handler, typoStyle, txtStyle);
    CreateTypographyHandler(nullptr, nullptr, nullptr);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    CreateTypography(typography, handler);
    CreateTypography(nullptr, nullptr);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_TextStyleGetFontMetrics(typography, txtStyle, &fontMetrics);
    double position[] = { GetObject<uint32_t>() % DATA_MAX_RANDOM, GetObject<uint32_t>() % DATA_MAX_RANDOM };
    OH_Drawing_Bitmap* bitmap = CreateBitmap();
    OH_Drawing_Canvas* canvas = CreateCanvas(bitmap);
    OH_Drawing_CanvasDestroy(CreateCanvas(nullptr));
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(path, DATA_PATH_SIZE, DATA_PATH_SIZE, DATA_PATH_SIZE, DATA_PATH_SIZE, 0, 0);
    OH_Drawing_TypographyPaintOnPath(
        typography, canvas, nullptr, GetObject<uint32_t>() % DATA_MAX_RANDOM, GetObject<uint32_t>() % DATA_MAX_RANDOM);
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