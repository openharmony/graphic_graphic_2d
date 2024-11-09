
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

#include <cstddef>
#include <memory>

#include "drawing_fuzzer.h"
#include "drawing_text_run.h"
#include "get_object.h"

namespace OHOS::Rosen::Drawing {

void OHDrawingRunTest(OH_Drawing_Array* runs, OH_Drawing_Canvas* canvas)
{
    size_t runsSize = OH_Drawing_GetDrawingArraySize(runs);
    for (size_t runIndex = 0; runIndex < runsSize; runIndex++) {
        OH_Drawing_Run* run = OH_Drawing_GetRunByIndex(runs, runIndex);
        uint32_t count = OH_Drawing_GetRunGlyphCount(run);
        uint64_t location = GetObject<uint64_t>();
        uint64_t length = GetObject<uint64_t>();
        OH_Drawing_GetRunStringRange(run, nullptr, nullptr);
        OH_Drawing_GetRunStringRange(run, &location, &length);
        OH_Drawing_GetRunStringIndices(run, 0, 0);
        OH_Drawing_Array* stringIndicesArr = OH_Drawing_GetRunStringIndices(run, 0, count);
        size_t drawingArraySize = OH_Drawing_GetDrawingArraySize(stringIndicesArr);
        for (size_t stringIndex = 0; stringIndex < drawingArraySize; stringIndex++) {
            uint64_t indices = OH_Drawing_GetRunStringIndicesByIndex(stringIndicesArr, stringIndex);
        }
        OH_Drawing_DestroyRunStringIndices(stringIndicesArr);
        float myAscent = GetObject<float>();
        float myDescent = GetObject<float>();
        float myLeading = GetObject<float>();
        OH_Drawing_GetRunTypographicBounds(run, nullptr, nullptr, nullptr);
        OH_Drawing_GetRunTypographicBounds(run, &myAscent, &myDescent, &myLeading);
        OH_Drawing_Rect* bounds = OH_Drawing_GetRunImageBounds(run);
        OH_Drawing_DestroyRunImageBounds(bounds);
        OH_Drawing_DestroyRunImageBounds(nullptr);
        OH_Drawing_Array* glyphs = OH_Drawing_GetRunGlyphs(run, 0, count);
        OH_Drawing_GetRunGlyphs(run, 0, 0);

        size_t glyphSize = OH_Drawing_GetDrawingArraySize(glyphs);
        for (size_t glyphsIndex = 0; glyphsIndex < glyphSize; glyphsIndex++) {
            OH_Drawing_GetRunGlyphsByIndex(glyphs, glyphsIndex);
        }
        OH_Drawing_DestroyRunGlyphs(glyphs);
        OH_Drawing_Array* positions = OH_Drawing_GetRunPositions(run, 0, count);
        OH_Drawing_GetRunPositions(run, 0, 0);

        size_t positionSize = OH_Drawing_GetDrawingArraySize(positions);
        for (size_t posIndex = 0; posIndex < positionSize; posIndex++) {
            OH_Drawing_Point* pos = OH_Drawing_GetRunPositionsByIndex(positions, posIndex);
            float x = 0.0;
            OH_Drawing_PointGetX(pos, &x);
        }
        OH_Drawing_DestroyRunPositions(positions);
        OH_Drawing_RunPaint(canvas, run, GetObject<double>(), GetObject<double>());
    }
}
void OHDrawingTextRunTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    CreateTypographyStyle(typoStyle);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    CreateTextStyle(txtStyle);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    CreateTypographyHandler(handler, typoStyle, txtStyle);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);

    OH_Drawing_Bitmap* bitmap = CreateBitmap();
    OH_Drawing_Canvas* canvas = CreateCanvas(bitmap);
    OH_Drawing_TypographyLayout(typography, GetObject<double>());
    OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography);
    // 针对每一行
    size_t drawingArraySize = OH_Drawing_GetDrawingArraySize(textLines);
    for (size_t lineIndex = 0; lineIndex < size; lineIndex++) {
        OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, lineIndex);
        OH_Drawing_Array* runs = OH_Drawing_TextLineGetGlyphRuns(textLine);

        // 针对每个run
        OHDrawingRunTest(runs, canvas);
        OHDrawingRunTest(nullptr, canvas);

        OH_Drawing_DestroyRuns(runs);
    }
    OH_Drawing_DestroyTextLines(textLines);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

} // namespace OHOS::Rosen::Drawing