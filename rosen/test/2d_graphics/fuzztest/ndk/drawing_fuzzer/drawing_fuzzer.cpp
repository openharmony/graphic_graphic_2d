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

#include "drawing_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_font_collection.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_text_declaration.h"
#include "drawing_text_typography.h"
#include "drawing_types.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
} // namespace

namespace Drawing {
void NativeDrawingBitmapTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    uint32_t width = static_cast<uint32_t>(data[0]);
    uint32_t height = static_cast<uint32_t>(data[1]);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
    OH_Drawing_BitmapDestroy(bitmap);
}

void NativeDrawingBrushTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    uint32_t red = static_cast<uint32_t>(data[0]);
    uint32_t gree = static_cast<uint32_t>(data[1]);
    uint32_t blue = static_cast<uint32_t>(data[0]);
    uint32_t alpha = static_cast<uint32_t>(data[1]);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, false);
    OH_Drawing_BrushIsAntiAlias(brush);
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_BrushGetColor(brush);
    OH_Drawing_BrushDestroy(brush);
}

void NativeDrawingCanvasBitmapTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_BitmapDestroy(bitmap);
}

void NativeDrawingCanvasPenTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_PenDestroy(pen);
}

void NativeDrawingCanvasBrushTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_BrushDestroy(brush);
}

void NativeDrawingCanvasDrawLineTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float x1 = static_cast<float>(data[0]);
    float y1 = static_cast<float>(data[1]);
    float x2 = static_cast<float>(data[1]);
    float y2 = static_cast<float>(data[0]);
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasDrawLine(canvas, x1, y1, x2, y2);
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingCanvasDrawPathTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingCanvasClearTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(uint32_t))) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetObject<uint32_t>();
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasClear(canvas, color);
    OH_Drawing_CanvasDestroy(canvas);
}

void NativeDrawingColorTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(uint32_t))) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetObject<uint32_t>();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, false);
    OH_Drawing_BrushIsAntiAlias(brush);
    OH_Drawing_BrushSetColor(brush, color);
    OH_Drawing_BrushGetColor(brush);
    OH_Drawing_BrushDestroy(brush);
}

void OHDrawingFontCollectionTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    OH_Drawing_DestroyFontCollection(fontCollection);
}

void NativeDrawingPathMoveToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float x = static_cast<float>(data[0]);
    float y = static_cast<float>(data[1]);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, x, y);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathLineToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float x = static_cast<float>(data[0]);
    float y = static_cast<float>(data[1]);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path, x, y);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathResetTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float x = static_cast<float>(data[0]);
    float y = static_cast<float>(data[1]);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, x, y);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathArcToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float x1 = static_cast<float>(data[0]);
    float y1 = static_cast<float>(data[1]);
    float x2 = static_cast<float>(data[1]);
    float y2 = static_cast<float>(data[0]);
    float startDeg = static_cast<float>(data[0]);
    float sweepDeg = static_cast<float>(data[1]);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(path, x1, y1, x2, y2, startDeg, sweepDeg);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathQuadToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float ctrlX = static_cast<float>(data[0]);
    float ctrlY = static_cast<float>(data[1]);
    float endX = static_cast<float>(data[1]);
    float endY = static_cast<float>(data[0]);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathQuadTo(path, ctrlX, ctrlY, endX, endY);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathCubicToTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float ctrlX1 = static_cast<float>(data[0]);
    float ctrlY1 = static_cast<float>(data[1]);
    float ctrlX2 = static_cast<float>(data[1]);
    float ctrlY2 = static_cast<float>(data[0]);
    float endX = static_cast<float>(data[0]);
    float endY = static_cast<float>(data[1]);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path, ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathCloseTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    float x = static_cast<float>(data[0]);
    float y = static_cast<float>(data[1]);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path, x, y);
    OH_Drawing_PathClose(path);
    OH_Drawing_PathDestroy(path);
}

void OHDrawingTypographyTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    uint32_t width = static_cast<float>(data[1]);
    uint32_t height = static_cast<float>(data[1]);
    uint32_t red = static_cast<float>(data[1]);
    uint32_t gree = static_cast<float>(data[1]);
    uint32_t blue = static_cast<float>(data[0]);
    uint32_t alpha = static_cast<float>(data[1]);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_SetTextStyleFontSize(txtStyle, static_cast<float>(data[0]));
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_SetTextStyleBaseLine(txtStyle, TEXT_BASELINE_ALPHABETIC);
    const char* fontFamilies[] = { "Roboto" };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, "OpenHarmony\n");
    OH_Drawing_TypographyHandlerPopTextStyle(handler);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, static_cast<float>(data[0]));
    OH_Drawing_TypographyGetMaxWidth(typography);
    double position[2] = { 10.0, 15.0 }; // 2 mean array number, 10.0 mean first number and 15.0 mean second number
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(cBitmap, width, height, &cFormat);
    OH_Drawing_BitmapGetWidth(cBitmap);
    OH_Drawing_BitmapGetHeight(cBitmap);
    OH_Drawing_Canvas* cCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(cCanvas, cBitmap);
    OH_Drawing_CanvasClear(cCanvas, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_TypographyGetHeight(typography);
    OH_Drawing_TypographyGetLongestLine(typography);
    OH_Drawing_TypographyGetMinIntrinsicWidth(typography);
    OH_Drawing_TypographyGetMaxIntrinsicWidth(typography);
    OH_Drawing_TypographyGetAlphabeticBaseline(typography);
    OH_Drawing_TypographyGetIdeographicBaseline(typography);
    OH_Drawing_TypographyPaint(typography, cCanvas, position[0], position[1]);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_BitmapDestroy(cBitmap);
    OH_Drawing_CanvasDestroy(cCanvas);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

void NativeDrawingPenTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    uint32_t red = static_cast<float>(data[0]);
    uint32_t gree = static_cast<float>(data[0]);
    uint32_t blue = static_cast<float>(data[0]);
    uint32_t alpha = static_cast<float>(data[0]);
    float width = static_cast<float>(data[1]);
    float miter = static_cast<float>(data[1]);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenIsAntiAlias(pen);
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(alpha, red, gree, blue));
    OH_Drawing_PenGetColor(pen);
    OH_Drawing_PenSetWidth(pen, width);
    OH_Drawing_PenGetWidth(pen);
    OH_Drawing_PenSetMiterLimit(pen, miter);
    OH_Drawing_PenGetMiterLimit(pen);
    OH_Drawing_PenSetCap(pen, OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    OH_Drawing_PenGetCap(pen);
    OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_PenGetJoin(pen);
    OH_Drawing_PenDestroy(pen);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingBitmapTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingBrushTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasBitmapTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasPenTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasBrushTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasDrawLineTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasDrawPathTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingCanvasClearTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingColorTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingFontCollectionTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathMoveToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathLineToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathResetTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathArcToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathQuadToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathCubicToTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathCloseTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingTypographyTest(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPenTest(data, size);

    return 0;
}
