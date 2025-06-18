/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "canvas_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_font.h"
#include "drawing_image.h"
#include "drawing_matrix.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_pixel_map.h"
#include "drawing_point.h"
#include "drawing_record_cmd.h"
#include "drawing_rect.h"
#include "drawing_region.h"
#include "drawing_round_rect.h"
#include "drawing_sampling_options.h"
#include "drawing_types.h"
#include "drawing_text_blob.h"
#include "image/pixelmap_native.h"

constexpr uint32_t MAX_ARRAY_MAX = 5000;
constexpr uint32_t ENUM_RANGE_ONE = 1;
constexpr uint32_t ENUM_RANGE_TWO = 2;
constexpr uint32_t ENUM_RANGE_THREE = 3;
constexpr uint32_t ENUM_RANGE_FIVE = 5;
constexpr uint32_t ENUM_RANGE_TWENTY_EIGHT = 28;
constexpr uint32_t WIDTH_FACTOR = 4;
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t PATH_DIRECTION_ENUM_SIZE = 2;

namespace OHOS {
namespace Rosen {
namespace Drawing {
void CanvasFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    uint16_t textSize = GetObject<uint16_t>() % MAX_ARRAY_MAX + 1;
    char* text = new char[textSize];
    for (int i = 0; i < textSize; i++) {
        text[i] = GetObject<char>();
    }
    text[textSize - 1] = '\0';
    OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobCreateFromString(text, font,
        GetObject<OH_Drawing_TextEncoding>());

    OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawTextBlob(nullptr, textBlob, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawTextBlob(canvas, nullptr, GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobDestroy(textBlob);
    if (text != nullptr) {
        delete[] text;
        text = nullptr;
    }
}

void CanvasFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Point* point = OH_Drawing_PointCreate(GetObject<float>(), GetObject<float>());
    uint32_t width = GetObject<uint32_t>() % MAX_ARRAY_MAX;
    uint32_t height = GetObject<uint32_t>() % MAX_ARRAY_MAX;
    uint32_t size_pix = width * height;
    uint32_t* pixels = new uint32_t[size_pix];
    for (uint32_t i = 0; i < size_pix; ++i) {
        pixels[i] = GetObject<uint32_t>();
    }
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * WIDTH_FACTOR);

    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_CanvasBind(nullptr, bitmap);
    OH_Drawing_CanvasBind(canvas, nullptr);

    OH_Drawing_CanvasReadPixelsToBitmap(canvas, bitmap, GetObject<int32_t>(), GetObject<int32_t>());
    OH_Drawing_CanvasReadPixelsToBitmap(nullptr, bitmap, GetObject<int32_t>(), GetObject<int32_t>());

    OH_Drawing_CanvasDrawBitmap(canvas, bitmap, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawBitmap(nullptr, bitmap, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawBitmap(canvas, nullptr, GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasDrawCircle(canvas, point, GetObject<float>());
    OH_Drawing_CanvasDrawCircle(nullptr, point, GetObject<float>());
    OH_Drawing_CanvasDrawCircle(canvas, nullptr, GetObject<float>());
    OH_Drawing_CanvasDrawCircle(canvas, point, -abs(GetObject<float>()));

    OH_Drawing_CanvasDrawLine(canvas, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawLine(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_PointDestroy(point);
    if (pixels != nullptr) {
        delete[] pixels;
        pixels = nullptr;
    }
}

void CanvasFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();

    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, GetObject<float>(), GetObject<float>());
    uint32_t width = GetObject<uint32_t>() % MAX_ARRAY_MAX;
    uint32_t height = GetObject<uint32_t>() % MAX_ARRAY_MAX;
    uint32_t size_pix = width * height;
    uint32_t* pixels = new uint32_t[size_pix];
    for (uint32_t i = 0; i < size_pix; ++i) {
        pixels[i] = GetObject<uint32_t>();
    }
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * WIDTH_FACTOR);

    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
    OH_Drawing_CanvasDrawRoundRect(nullptr, roundRect);
    OH_Drawing_CanvasDrawRoundRect(canvas, nullptr);

    OH_Drawing_CanvasClipRoundRect(canvas, roundRect, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());
    OH_Drawing_CanvasClipRoundRect(nullptr, roundRect, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());
    OH_Drawing_CanvasClipRoundRect(canvas, nullptr, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RectDestroy(rect);
    if (pixels != nullptr) {
        delete[] pixels;
        pixels = nullptr;
    }
}

void CanvasFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        static_cast<OH_Drawing_FilterMode>(GetObject<uint32_t>() % ENUM_RANGE_ONE),
        static_cast<OH_Drawing_MipmapMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO));
    uint32_t width = GetObject<uint32_t>() % MAX_ARRAY_MAX;
    uint32_t height = GetObject<uint32_t>() % MAX_ARRAY_MAX;
    uint32_t size_pix = width * height;
    uint32_t* pixels = new uint32_t[size_pix];
    for (uint32_t i = 0; i < size_pix; ++i) {
        pixels[i] = GetObject<uint32_t>();
    }
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * WIDTH_FACTOR);

    OH_Drawing_BitmapFormat cFormat{static_cast<OH_Drawing_ColorFormat>(GetObject<uint32_t>() % ENUM_RANGE_FIVE),
        static_cast<OH_Drawing_AlphaFormat>(GetObject<uint32_t>() % ENUM_RANGE_THREE)};
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    void* dstPixels = OH_Drawing_BitmapGetPixels(bitmap);
    if (width != 0 && height != 0) {
        OH_Drawing_CanvasReadPixels(canvas, &imageInfo, dstPixels, width * WIDTH_FACTOR, GetObject<int32_t>() % width,
            GetObject<int32_t>() % height);
        OH_Drawing_CanvasReadPixels(nullptr, &imageInfo, dstPixels, width * WIDTH_FACTOR, GetObject<int32_t>() % width,
            GetObject<int32_t>() % height);
    }

    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, rect, rect2, samplingOptions);
    OH_Drawing_CanvasDrawBitmapRect(nullptr, bitmap, rect, rect2, samplingOptions);
    OH_Drawing_CanvasDrawBitmapRect(canvas, nullptr, rect, rect2, samplingOptions);
    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, nullptr, rect2, samplingOptions);
    
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_BitmapDestroy(bitmap);
    if (pixels != nullptr) {
        delete[] pixels;
        pixels = nullptr;
    }
}

void CanvasFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        static_cast<OH_Drawing_FilterMode>(GetObject<uint32_t>() % ENUM_RANGE_ONE),
        static_cast<OH_Drawing_MipmapMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO));

    OH_Drawing_CanvasDrawOval(canvas, rect);
    OH_Drawing_CanvasDrawOval(nullptr, rect);
    OH_Drawing_CanvasDrawOval(canvas, nullptr);

    OH_Drawing_CanvasDrawArc(canvas, rect, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawArc(nullptr, rect, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawArc(canvas, nullptr, GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasDrawImageRectWithSrc(canvas, image, rect, rect2, samplingOptions,
        static_cast<OH_Drawing_SrcRectConstraint>(GetObject<uint32_t>() % ENUM_RANGE_ONE));
    OH_Drawing_CanvasDrawImageRectWithSrc(nullptr, image, rect, rect2, samplingOptions,
        static_cast<OH_Drawing_SrcRectConstraint>(GetObject<uint32_t>() % ENUM_RANGE_ONE));

    OH_Drawing_CanvasDrawImageRect(canvas, image, rect, samplingOptions);
    OH_Drawing_CanvasDrawImageRect(nullptr, image, rect, samplingOptions);

    OH_Drawing_CanvasClipRect(canvas, rect, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());
    OH_Drawing_CanvasClipRect(nullptr, rect, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());
    OH_Drawing_CanvasClipRect(canvas, nullptr, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
}

void CanvasFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());

    OH_Drawing_CanvasRotate(canvas, GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasRotate(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasTranslate(canvas, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasTranslate(nullptr, GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasScale(canvas, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasScale(nullptr, GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasSkew(canvas, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasSkew(nullptr, GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasRestoreToCount(canvas, GetObject<uint32_t>());
    OH_Drawing_CanvasRestoreToCount(nullptr, GetObject<uint32_t>());

    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasAttachBrush(nullptr, brush);
    OH_Drawing_CanvasAttachBrush(canvas, nullptr);

    OH_Drawing_CanvasSaveLayer(canvas, rect, brush);
    OH_Drawing_CanvasSaveLayer(nullptr, rect, brush);

    OH_Drawing_CanvasDrawBackground(canvas, brush);
    OH_Drawing_CanvasDrawBackground(nullptr, brush);
    OH_Drawing_CanvasDrawBackground(canvas, nullptr);

    OH_Drawing_CanvasGetLocalClipBounds(canvas, rect);
    OH_Drawing_CanvasGetLocalClipBounds(nullptr, rect);
    OH_Drawing_CanvasGetLocalClipBounds(canvas, nullptr);

    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDrawRect(nullptr, rect);
    OH_Drawing_CanvasDrawRect(canvas, nullptr);

    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rect);
}

void CanvasFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();

    OH_Drawing_CanvasDrawRegion(canvas, region);
    OH_Drawing_CanvasDrawRegion(nullptr, region);
    OH_Drawing_CanvasDrawRegion(canvas, nullptr);

    uint32_t vertexCount = GetObject<uint32_t>() % MAX_ARRAY_MAX + 1;
    OH_Drawing_Point2D* pts = new OH_Drawing_Point2D[vertexCount];
    for (size_t i = 0; i < vertexCount; i++) {
        pts[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_Point2D* texs = new OH_Drawing_Point2D[vertexCount];
    for (size_t i = 0; i < vertexCount; i++) {
        texs[i] = {GetObject<float>(), GetObject<float>()};
    }
    uint32_t* colors = new uint32_t[vertexCount];
    for (size_t i = 0; i < vertexCount; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    uint32_t indexCount = GetObject<uint32_t>() % MAX_ARRAY_MAX + 1;
    uint16_t* indices = new uint16_t[indexCount];
    for (size_t i = 0; i < indexCount; i++) {
        indices[i] = GetObject<uint16_t>();
    }
    OH_Drawing_CanvasDrawVertices(canvas, static_cast<OH_Drawing_VertexMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO),
        vertexCount, pts, texs, colors, indexCount, indices,
        static_cast<OH_Drawing_BlendMode>(GetObject<uint32_t>() % ENUM_RANGE_TWENTY_EIGHT));
    OH_Drawing_CanvasDrawVertices(canvas, static_cast<OH_Drawing_VertexMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO),
        vertexCount, pts, texs, nullptr, indexCount, indices,
        static_cast<OH_Drawing_BlendMode>(GetObject<uint32_t>() % ENUM_RANGE_TWENTY_EIGHT));
    OH_Drawing_CanvasDrawVertices(nullptr, static_cast<OH_Drawing_VertexMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO),
        vertexCount, pts, texs, colors, indexCount, indices,
        static_cast<OH_Drawing_BlendMode>(GetObject<uint32_t>() % ENUM_RANGE_TWENTY_EIGHT));
    if (pts != nullptr || texs != nullptr || colors != nullptr || indices != nullptr) {
        delete[] pts, delete[] texs, delete[] colors, delete[] indices;
        pts = nullptr, texs = nullptr, colors = nullptr, indices = nullptr;
    }

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_CanvasDestroy(canvas);
}

void CanvasFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();

    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasAttachPen(nullptr, pen);
    OH_Drawing_CanvasAttachPen(canvas, nullptr);

    OH_Drawing_CanvasGetTotalMatrix(canvas, matrix);
    OH_Drawing_CanvasGetTotalMatrix(nullptr, matrix);

    OH_Drawing_CanvasConcatMatrix(canvas, matrix);
    OH_Drawing_CanvasConcatMatrix(nullptr, matrix);

    OH_Drawing_CanvasSetMatrix(canvas, matrix);
    OH_Drawing_CanvasSetMatrix(nullptr, matrix);

    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDrawPath(nullptr, path);
    OH_Drawing_CanvasDrawPath(canvas, nullptr);

    OH_Drawing_CanvasClipPath(canvas, path, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());
    OH_Drawing_CanvasClipPath(nullptr, path, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());
    OH_Drawing_CanvasClipPath(canvas, nullptr, static_cast<OH_Drawing_CanvasClipOp>(GetObject<uint32_t>() %
        ENUM_RANGE_ONE), GetObject<bool>());

    OH_Drawing_Point3D planeParams = {GetObject<float>(), GetObject<float>(), GetObject<float>()};
    OH_Drawing_Point3D devLightPos = {GetObject<float>(), GetObject<float>(), GetObject<float>()};
    OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, devLightPos, GetObject<float>(), GetObject<uint32_t>(),
        GetObject<uint32_t>(), static_cast<OH_Drawing_CanvasShadowFlags>(GetObject<uint32_t>() % ENUM_RANGE_THREE));
    OH_Drawing_CanvasDrawShadow(nullptr, path, planeParams, devLightPos, GetObject<float>(), GetObject<uint32_t>(),
        GetObject<uint32_t>(), static_cast<OH_Drawing_CanvasShadowFlags>(GetObject<uint32_t>() % ENUM_RANGE_THREE));
    OH_Drawing_CanvasDrawShadow(canvas, path, planeParams, devLightPos, GetObject<float>(), GetObject<uint32_t>(),
        GetObject<uint32_t>(), static_cast<OH_Drawing_CanvasShadowFlags>(GetObject<uint32_t>()));

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_PathDestroy(path);
}

void CanvasFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachPen(nullptr);

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_CanvasDetachBrush(nullptr);

    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasSave(nullptr);

    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasRestore(nullptr);

    OH_Drawing_CanvasGetSaveCount(canvas);
    OH_Drawing_CanvasGetSaveCount(nullptr);

    OH_Drawing_CanvasGetWidth(canvas);
    OH_Drawing_CanvasGetWidth(nullptr);

    OH_Drawing_CanvasGetHeight(canvas);
    OH_Drawing_CanvasGetHeight(nullptr);

    OH_Drawing_CanvasResetMatrix(canvas);
    OH_Drawing_CanvasResetMatrix(nullptr);

    OH_Drawing_CanvasClear(canvas, GetObject<uint32_t>());
    OH_Drawing_CanvasClear(nullptr, GetObject<uint32_t>());

    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_MAX + 1;
    OH_Drawing_Point2D* pts = new OH_Drawing_Point2D[count];
    for (size_t i = 0; i < count; i++) {
        pts[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_CanvasDrawPoints(canvas, static_cast<OH_Drawing_PointMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO),
        count, pts);
    OH_Drawing_CanvasDrawPoints(nullptr, static_cast<OH_Drawing_PointMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO),
        count, pts);
    OH_Drawing_CanvasDrawPoints(canvas, static_cast<OH_Drawing_PointMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO),
        count, nullptr);
    if (pts != nullptr) {
        delete[] pts;
        pts = nullptr;
    }

    OH_Drawing_CanvasDestroy(canvas);
}

void CanvasFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();

    OH_Pixelmap_InitializationOptions *options = nullptr;
    OH_PixelmapNative *pixelMap = nullptr;
    size_t width = GetObject<size_t>() % MAX_ARRAY_MAX;
    size_t height = GetObject<size_t>() % MAX_ARRAY_MAX;
    OH_PixelmapInitializationOptions_Create(&options);
    OH_PixelmapInitializationOptions_SetWidth(options, width);
    OH_PixelmapInitializationOptions_SetHeight(options, height);
    OH_PixelmapInitializationOptions_SetPixelFormat(options, WIDTH_FACTOR);
    OH_PixelmapInitializationOptions_SetAlphaType(options, ENUM_RANGE_TWO);
    size_t dataLength = width * height * WIDTH_FACTOR;
    uint8_t* colorData = new uint8_t[dataLength];
    for (size_t i = 0; i < width * height; i++) {
        colorData[i] = GetObject<uint8_t>();
    }
    
    OH_PixelmapNative_CreatePixelmap(colorData, dataLength, options, &pixelMap);
    OH_Drawing_PixelMap *drPixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMap);
    OH_Drawing_Rect* srcRect =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_Rect* dstRect =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        static_cast<OH_Drawing_FilterMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO),
        static_cast<OH_Drawing_MipmapMode>(GetObject<uint32_t>() % ENUM_RANGE_THREE));
    OH_Drawing_CanvasDrawPixelMapRect(canvas, drPixelMap, srcRect, dstRect, samplingOptions);
    OH_Drawing_CanvasDrawPixelMapRect(canvas, drPixelMap, srcRect, dstRect, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(canvas, drPixelMap, srcRect, nullptr, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(canvas, drPixelMap, nullptr, nullptr, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(canvas, nullptr, nullptr, nullptr, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(nullptr, nullptr, nullptr, nullptr, nullptr);
    OH_Drawing_PixelMapDissolve(drPixelMap);
    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(options);
    OH_Drawing_RectDestroy(srcRect);
    OH_Drawing_RectDestroy(dstRect);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);

    if (colorData != nullptr) {
        delete[] colorData;
        colorData = nullptr;
    }

    OH_Drawing_CanvasDestroy(canvas);
}

void CanvasFuzzTest011(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    OH_Drawing_Canvas* canvas = GetObject<OH_Drawing_Canvas*>();
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    if (code == OH_DRAWING_SUCCESS) {
        OH_Drawing_RecordCmd* recordCmd = GetObject<OH_Drawing_RecordCmd*>();
        code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils, &recordCmd);
        if (code == OH_DRAWING_SUCCESS) {
            OH_Drawing_Canvas* recordCanvas = OH_Drawing_CanvasCreate();
            OH_Drawing_CanvasDrawRecordCmd(recordCanvas, recordCmd);
            OH_Drawing_CanvasDestroy(recordCanvas);
            OH_Drawing_RecordCmdDestroy(recordCmd);
        }
    }
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
}

void CanvasFuzzTest012(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool quickReject = GetObject<bool>();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    uint32_t pathDirection = GetObject<uint32_t>();
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path, GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>(), static_cast<OH_Drawing_PathDirection>(pathDirection % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_CanvasQuickRejectPath(nullptr, path, &quickReject);
    OH_Drawing_CanvasQuickRejectPath(canvas, nullptr, &quickReject);
    OH_Drawing_CanvasQuickRejectPath(canvas, path, nullptr);
    OH_Drawing_CanvasQuickRejectPath(canvas, path, &quickReject);
    OH_Drawing_Rect* rect =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasQuickRejectRect(nullptr, rect, &quickReject);
    OH_Drawing_CanvasQuickRejectRect(canvas, nullptr, &quickReject);
    OH_Drawing_CanvasQuickRejectRect(canvas, rect, nullptr);
    OH_Drawing_CanvasQuickRejectRect(canvas, rect, &quickReject);

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_RectDestroy(rect);
}

void CanvasFuzzTest013(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool useCenter = GetObject<bool>();
    float startAngle = GetObject<float>();
    float sweepAngle = GetObject<float>();
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Rect* rect =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawArcWithCenter(nullptr, rect, startAngle, sweepAngle, useCenter);
    OH_Drawing_CanvasDrawArcWithCenter(canvas, nullptr, startAngle, sweepAngle, useCenter);
    OH_Drawing_CanvasDrawArcWithCenter(canvas, rect, startAngle, sweepAngle, useCenter);
    OH_Drawing_Rect* rectOuter =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_Rect* rectInner =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_RoundRect* outer = OH_Drawing_RoundRectCreate(rectOuter, GetObject<float>(), GetObject<float>());
    OH_Drawing_RoundRect* inner = OH_Drawing_RoundRectCreate(rectInner, GetObject<float>(), GetObject<float>());
    OH_Drawing_CanvasDrawNestedRoundRect(nullptr, outer, inner);
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, nullptr, inner);
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer, nullptr);
    OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer, inner);

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rectOuter);
    OH_Drawing_RectDestroy(rectInner);
    OH_Drawing_RoundRectDestroy(outer);
    OH_Drawing_RoundRectDestroy(inner);
}

void CanvasFuzzTest014(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Pixelmap_InitializationOptions *options = nullptr;
    OH_PixelmapNative *pixelMap = nullptr;
    size_t width = GetObject<size_t>() % MAX_ARRAY_MAX;
    size_t height = GetObject<size_t>() % MAX_ARRAY_MAX;
    OH_PixelmapInitializationOptions_Create(&options);
    OH_PixelmapInitializationOptions_SetWidth(options, width);
    OH_PixelmapInitializationOptions_SetHeight(options, height);
    OH_PixelmapInitializationOptions_SetPixelFormat(options, WIDTH_FACTOR);
    OH_PixelmapInitializationOptions_SetAlphaType(options, ENUM_RANGE_TWO);
    size_t dataLength = width * height * WIDTH_FACTOR;
    uint8_t* colorData = new uint8_t[dataLength];
    for (size_t i = 0; i < width * height; i++) {
        colorData[i] = GetObject<uint8_t>();
    }
    OH_PixelmapNative_CreatePixelmap(colorData, dataLength, options, &pixelMap);
    OH_Drawing_PixelMap *drPixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMap);
    OH_Drawing_Rect* center =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_Rect* dst =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());

    OH_Drawing_CanvasDrawPixelMapNine(nullptr, drPixelMap, center, dst,
        static_cast<OH_Drawing_FilterMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO));
    OH_Drawing_CanvasDrawPixelMapNine(canvas, nullptr, center, dst,
        static_cast<OH_Drawing_FilterMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO));
    OH_Drawing_CanvasDrawPixelMapNine(canvas, drPixelMap, center, nullptr,
        static_cast<OH_Drawing_FilterMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO));
    OH_Drawing_CanvasDrawPixelMapNine(canvas, drPixelMap, center, dst,
        static_cast<OH_Drawing_FilterMode>(GetObject<uint32_t>() % ENUM_RANGE_TWO));

    OH_Drawing_PixelMapDissolve(drPixelMap);
    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(options);
    OH_Drawing_RectDestroy(center);
    OH_Drawing_RectDestroy(dst);

    if (colorData != nullptr) {
        delete[] colorData;
        colorData = nullptr;
    }
    OH_Drawing_CanvasDestroy(canvas);
}

void CanvasFuzzTest015(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    OH_Drawing_Canvas* canvas = GetObject<OH_Drawing_Canvas*>();
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    if (code == OH_DRAWING_SUCCESS) {
        OH_Drawing_RecordCmd* recordCmd = GetObject<OH_Drawing_RecordCmd*>();
        code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils, &recordCmd);
        if (code == OH_DRAWING_SUCCESS) {
            OH_Drawing_Canvas* recordCanvas = OH_Drawing_CanvasCreate();
            OH_Drawing_CanvasDrawRecordCmdNesting(recordCanvas, recordCmd);
            OH_Drawing_CanvasDestroy(recordCanvas);
            OH_Drawing_RecordCmdDestroy(recordCmd);
        }
    }
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
}

void CanvasFuzzTest016(const uint8_t* data, size_t size)
{
        if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_MAX + 1;
    char* str = new char[count];
    for (size_t i = 0; i < count; i++) {
        str[i] = GetObject<char>();
    }
    str[count - 1] = '\0';
    uint32_t count2 = GetObject<uint32_t>() % MAX_ARRAY_MAX + 1;
    char* str2 = new char[count2];
    for (size_t i = 0; i < count2; i++) {
        str2[i] = GetObject<char>();
    }
    str2[count2 - 1] = '\0';
    float val = GetObject<float>();
    
    float x = GetObject<float>();
    float y = GetObject<float>();

    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontFeatures* features = OH_Drawing_FontFeaturesCreate();
    OH_Drawing_FontFeaturesAddFeature(features, str2, val);

    OH_Drawing_CanvasDrawSingleCharacterWithFeatures(canvas, str, font, x, y, features);
    OH_Drawing_CanvasDrawSingleCharacterWithFeatures(nullptr, str, font, x, y, features);
    OH_Drawing_CanvasDrawSingleCharacterWithFeatures(canvas, nullptr, font, x, y, features);
    OH_Drawing_CanvasDrawSingleCharacterWithFeatures(canvas, str, nullptr, x, y, features);
    OH_Drawing_CanvasDrawSingleCharacterWithFeatures(canvas, str, font, x, y, nullptr);

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_FontFeaturesDestroy(features);
    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    if (str2 != nullptr) {
        delete [] str2;
        str2 = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::CanvasFuzzTest001(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest002(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest003(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest004(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest005(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest006(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest007(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest008(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest009(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest010(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest011(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest012(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest013(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest014(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest015(data, size);
    OHOS::Rosen::Drawing::CanvasFuzzTest016(data, size);
    return 0;
}
