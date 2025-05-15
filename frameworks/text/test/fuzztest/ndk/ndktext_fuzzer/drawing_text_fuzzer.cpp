/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "drawing_text_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "drawing_font_collection.h"
#include "drawing_point.h"
#include "drawing_rect.h"
#include "drawing_text_line.h"
#include "drawing_text_lineTypography.h"
#include "drawing_text_typography.h"

constexpr size_t DATA_MAX_LAYOUT_WIDTH = 100;
constexpr size_t DATA_MAX_FONTSIZE = 40;
namespace OHOS::Rosen::Drawing {
void OHDrawingTextLineArray(OH_Drawing_Array* linesArray, const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    size_t linesSize = OH_Drawing_GetDrawingArraySize(linesArray);
    OH_Drawing_GetTextLineByIndex(nullptr, linesSize - 1);
    OH_Drawing_TextLine* line = OH_Drawing_GetTextLineByIndex(linesArray, fdp.ConsumeIntegral<size_t>());
    OH_Drawing_TextLineGetGlyphCount(nullptr);
    OH_Drawing_TextLineGetGlyphCount(line);
    size_t start = 0;
    size_t end = 1;
    OH_Drawing_TextLineGetTextRange(nullptr, &start, &end);
    OH_Drawing_TextLineGetTextRange(line, &start, &end);
    OH_Drawing_TextLineGetGlyphRuns(nullptr);
    OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(line);
    OH_Drawing_GetRunByIndex(runs, fdp.ConsumeIntegral<size_t>());
    OH_Drawing_GetRunByIndex(nullptr, 0);
    OH_Drawing_GetDrawingArraySize(nullptr);
    OH_Drawing_GetDrawingArraySize(runs);
    OH_Drawing_DestroyRuns(runs);
    OH_Drawing_TextLinePaint(nullptr, nullptr, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>());
    auto canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_TextLinePaint(line, canvas, fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>());
    OH_Drawing_CanvasDestroy(canvas);
    double ascent = 0.0;
    double descent = 0.0;
    double leading = 0.0;
    OH_Drawing_TextLineGetTypographicBounds(nullptr, &ascent, &descent, &leading);
    OH_Drawing_TextLineGetTypographicBounds(line, &ascent, &descent, &leading);
    OH_Drawing_TextLineGetImageBounds(nullptr);
    OH_Drawing_RectDestroy(OH_Drawing_TextLineGetImageBounds(line));
    OH_Drawing_TextLineGetTrailingSpaceWidth(nullptr);
    OH_Drawing_TextLineGetTrailingSpaceWidth(line);
    const char* ellipsis = "...";
    OH_Drawing_TextLineCreateTruncatedLine(nullptr, DATA_MAX_LAYOUT_WIDTH, 0, ellipsis);
    OH_Drawing_TextLine* truncatedLine =
        OH_Drawing_TextLineCreateTruncatedLine(line, DATA_MAX_LAYOUT_WIDTH, 0, ellipsis);
    OH_Drawing_DestroyTextLine(truncatedLine);
    OH_Drawing_Point* point = OH_Drawing_PointCreate(fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>());
    int32_t index = OH_Drawing_TextLineGetStringIndexForPosition(line, point);
    int32_t index1 = OH_Drawing_TextLineGetStringIndexForPosition(nullptr, point);
    OH_Drawing_PointDestroy(point);
    OH_Drawing_TextLineGetOffsetForStringIndex(line, index);
    OH_Drawing_TextLineGetOffsetForStringIndex(nullptr, index1);
    OH_Drawing_TextLineEnumerateCaretOffsets(line, [](double, int, bool) { return false; });
    OH_Drawing_TextLineEnumerateCaretOffsets(line, nullptr);
    OH_Drawing_TextLineGetAlignmentOffset(line, fdp.ConsumeIntegral<uint32_t>(), DATA_MAX_LAYOUT_WIDTH);
    OH_Drawing_TextLineGetAlignmentOffset(nullptr, fdp.ConsumeIntegral<uint32_t>(), DATA_MAX_LAYOUT_WIDTH);
    OH_Drawing_DestroyTextLine(line);
}

void OHDrawTextLineTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t red = fdp.ConsumeIntegral<uint32_t>();
    uint32_t gree = fdp.ConsumeIntegral<uint32_t>();
    uint32_t blue = fdp.ConsumeIntegral<uint32_t>();
    uint32_t alpha = fdp.ConsumeIntegral<uint32_t>();
    uint32_t fontSize = fdp.ConsumeIntegral<uint32_t>();
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, width);
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    std::string text = "Hello World 测试文本";
    OH_Drawing_TypographyHandlerAddText(handler, text.c_str());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, DATA_MAX_LAYOUT_WIDTH);
    OH_Drawing_TypographyGetTextLines(nullptr);
    OH_Drawing_Array* linesArray = OH_Drawing_TypographyGetTextLines(typography);
    OHDrawingTextLineArray(linesArray, data, size);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTextLines(linesArray);
}

void OHDrawingLineTypographyTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    uint32_t red = fdp.ConsumeIntegral<uint32_t>();
    uint32_t gree = fdp.ConsumeIntegral<uint32_t>();
    uint32_t blue = fdp.ConsumeIntegral<uint32_t>();
    uint32_t alpha = fdp.ConsumeIntegral<uint32_t>();
    OH_Drawing_TypographyStyle* typographStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    OH_Drawing_TypographyCreate* createHandler = OH_Drawing_CreateTypographyHandler(typographStyle, fontCollection);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_SetTextStyleColor(textStyle, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_SetTextStyleFontSize(textStyle, DATA_MAX_FONTSIZE);
    OH_Drawing_SetTextStyleFontWeight(textStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(textStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(textStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(createHandler, textStyle);
    std::string text = "Hello \t 中国 测 World \n !@#$%^&*~(){}[] 123 4567890 - = ,. < >、/Drawing testlp ";
    text += "试 Drawing \n\n   \u231A \u513B \u00A9\uFE0F aaa "
            "clp11⌚😀😁🤣👨‍🔬👩‍👩‍👧‍👦👭مرحبا中国 测 "
            "World测试文本";
    OH_Drawing_TypographyHandlerAddText(createHandler, text.c_str());
    OH_Drawing_TypographyGetTextLines(nullptr);
    OH_Drawing_LineTypography* lineTypography = OH_Drawing_CreateLineTypography(createHandler);
    OH_Drawing_CreateLineTypography(nullptr);
    size_t startIndex = fdp.ConsumeIntegral<size_t>();
    auto count = OH_Drawing_LineTypographyGetLineBreak(lineTypography, startIndex, DATA_MAX_LAYOUT_WIDTH);
    OH_Drawing_TextLine* line = OH_Drawing_LineTypographyCreateLine(lineTypography, startIndex, count);
    OH_Drawing_DestroyTextLine(line);

    OH_Drawing_DestroyLineTypography(lineTypography);
    OH_Drawing_DestroyTypographyHandler(createHandler);
    OH_Drawing_DestroyTypographyStyle(typographStyle);
    OH_Drawing_DestroyTextStyle(textStyle);
}

void NativeDrawingTextStyleDecorationTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    uint32_t decorationA = fdp.ConsumeIntegral<uint32_t>();
    uint32_t decorationB = fdp.ConsumeIntegral<uint32_t>();
    OH_Drawing_AddTextStyleDecoration(txtStyle, decorationA);
    OH_Drawing_AddTextStyleDecoration(txtStyle, decorationB);
    OH_Drawing_RemoveTextStyleDecoration(txtStyle, decorationA);
    OH_Drawing_RemoveTextStyleDecoration(txtStyle, decorationB);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

void OHDrawingTextTabTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    uint32_t location = fdp.ConsumeIntegral<uint32_t>();
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextTab* tab = OH_Drawing_CreateTextTab(OH_Drawing_TextAlign::TEXT_ALIGN_LEFT, location);
    OH_Drawing_GetTextTabAlignment(tab);
    OH_Drawing_GetTextTabLocation(tab);
    OH_Drawing_SetTypographyTextTab(typoStyle, tab);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextTab(tab);
}
void OHDrawingCreateSharedFontCollectionTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_DisableFontCollectionFallback(nullptr);
    OH_Drawing_DestroyFontCollection(nullptr);
    OH_Drawing_DisableFontCollectionSystemFont(nullptr);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateSharedFontCollection();
    OH_Drawing_DisableFontCollectionFallback(fontCollection);
    OH_Drawing_DisableFontCollectionSystemFont(fontCollection);
    OH_Drawing_ClearFontCaches(fontCollection);
    OH_Drawing_ClearFontCaches(nullptr);
    OH_Drawing_DestroyFontCollection(fontCollection);
}

void OHDrawingCreateFontCollectionGlobalInstanceTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_GetFontCollectionGlobalInstance();
    OH_Drawing_DisableFontCollectionFallback(fontCollection);
    OH_Drawing_DisableFontCollectionSystemFont(fontCollection);
    OH_Drawing_ClearFontCaches(fontCollection);
    OH_Drawing_ClearFontCaches(nullptr);
}
} // namespace OHOS::Rosen::Drawing

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawTextLineTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingLineTypographyTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingTextStyleDecorationTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingTextTabTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingCreateSharedFontCollectionTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingCreateFontCollectionGlobalInstanceTest(data, size);
    return 0;
}
