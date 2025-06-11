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

#include "performance/canvas_draw_performance.h"

#include <multimedia/image_framework/image/pixelmap_native.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_text_blob.h>

#include "performance/performance.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0100
@Description:超多点绘制性能-OH_Drawing_CanvasDrawPoints
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、DrawPoints绘制100个点，重复调用1000次
    5、仅attach pen，重复操作5
    6、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawPoints::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    brush_ = performance_->GetBrush();
    // 使用brush绘制points时，brush的filter效果只能设置为colorFilter，否则无绘制效果
    OH_Drawing_BrushSetFilter(brush_, nullptr);
    OH_Drawing_CanvasAttachBrush(canvas, brush_);
    int pointCount = 100;
    int interval = 4;
    OH_Drawing_Point2D pts[pointCount];
    for (int i = 0; i < testCount_; i++) {
        for (int j = 0; j < pointCount; j++) {
            pts[j].x = interval * j;
            pts[j].y = interval * i;
        }
        OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS, pointCount, pts);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0200
@Description:超大折线段绘制性能-OH_Drawing_Path
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造200段折线组成的path，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawBrokenLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    int numLines = 200;
    int lineHeight = 10;
    int lineSpacing = 10;
    int currentX = 0;
    int currentY = 0;
    // Line direction sign
    int isUp = true;
    int interval = 2;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, 0, interval * i);
        currentX = 0;
        currentY = interval * i;
        for (int j = 0; j < numLines; j++) {
            if (isUp) {
                currentY += lineHeight;
            } else {
                currentY -= lineHeight;
            }
            // Switch the direction of the polyline
            isUp = !isUp;
            currentX += lineSpacing;
            OH_Drawing_PathLineTo(path, currentX, currentY);
        }
    }
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0300
@Description:超大曲线段绘制性能-OH_Drawing_Path
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造10段曲线（椭圆曲线+贝塞尔曲线）组成的path
    5、drawpath 偏移到下一行（部分重叠），重复调用10次
    6、仅attach pen，重复操作5
    7、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawCurveLine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    int drawCount = 10;
    float xInterval = 20;
    float yInterval = 30;
    for (int i = 0; i < drawCount; i++) {
        for (int j = 0; j < drawCount; j++) {
            float left = xInterval * j;
            float top = yInterval * i;
            float right = xInterval + xInterval * j;
            float bottom = yInterval + yInterval * i;
            OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
            OH_Drawing_CanvasDrawOval(canvas, rect);
            OH_Drawing_PathMoveTo(path, xInterval * j, yInterval * i);
            OH_Drawing_PathRQuadTo(path, xInterval * j, yInterval * i, xInterval * j + xInterval, yInterval * i);
            OH_Drawing_CanvasDrawPath(canvas, path);
        }
        OH_Drawing_PathMoveTo(path, 0, yInterval * i);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0400
@Description:超大矩形组绘制性能-OH_Drawing_CanvasDrawRegion
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造100个矩形（矩形+圆角矩形）组成的矩形组，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawRegion::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    int drawCount = 100;
    float xInterval = 10;
    float yInterval = 15;
    float width = 20;
    float height = 30;
    for (int i = 0; i < testCount_; i++) {
        for (int j = 0; j < drawCount; j++) {
            float left = xInterval * j;
            float top = yInterval * i;
            float right = width + xInterval * j;
            float bottom = height + yInterval * i;
            float xRad = 5;
            float yRad = 5;
            OH_Drawing_CanvasDrawRegion(canvas, region);
            OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
            OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);
            int even = 2;
            if (j % even) {
                OH_Drawing_CanvasDrawRect(canvas, rect);
            } else {
                OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
            }
            OH_Drawing_RectDestroy(rect);
            OH_Drawing_RoundRectDestroy(roundRect);
        }
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0500
@Description:超大位图绘制性能-OH_Drawing_CanvasDrawBitmap
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造1个位图，重复调用20次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawBitMap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    int32_t imageWidth = 1920 * 4;  // 1920, 4 for test
    int32_t imageHeight = 1080 * 2; // 1080, 2 for test
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    char* pixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 4 for test
    if (pixels == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t*)pixels)[i] = 0xFF0000FF;
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, imageWidth * 4); // 4 for test
    int drawCount = 20;
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, i, i);
    }
    OH_Drawing_BitmapDestroy(bitmap);
    free(pixels);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0600
@Description:超大位图绘制性能-OH_Drawing_CanvasDrawBitmapRect
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、构造1个矩形位图，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawBitMapRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    const int32_t itemWidth = 100;
    const int32_t itemHeight = 100;
    int32_t imageWidth = 1920 * 4;  // 1920, 4 for test
    int32_t imageHeight = 1080 * 2; // 1080, 2 for test
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    char* pixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4));
    if (pixels == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t*)pixels)[i] = 0xFF0000FF;
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, imageWidth * 4); // 4 for test
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    OH_Drawing_Rect* bitMapSrc = OH_Drawing_RectCreate(0, 0, itemWidth, itemHeight);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* bitMapDst = OH_Drawing_RectCreate(itemWidth, 0, itemWidth * 2, itemHeight + i); // 2 for test
        OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, bitMapSrc, bitMapDst, samplingOptions);
        OH_Drawing_RectDestroy(bitMapDst);
    }
    OH_Drawing_RectDestroy(bitMapSrc);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0700
@Description:超大图片绘制性能
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、图片处理相关接口调用：
        OH_Drawing_CanvasDrawImageRectWithSrc，重复调用1000次
        OH_Drawing_CanvasDrawImageRect，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawImage::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    const int32_t itemWidth = 100;
    const int32_t itemHeight = 100;
    int32_t imageWidth = 1920 * 4;  // 1920, 4 for test
    int32_t imageHeight = 1080 * 2; // 1080, 2 for test
    OH_Drawing_Image_Info imageInfo { imageWidth, imageHeight, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE };
    char* pixels = static_cast<char*>(malloc(imageWidth * imageHeight * 4)); // 4 for test
    if (pixels == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t*)pixels)[i] = 0xFF0000FF;
    }
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, imageWidth * 4);
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_Rect* imageRectSrc = OH_Drawing_RectCreate(0, 0, itemWidth, itemHeight);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* imageRectDst =
            OH_Drawing_RectCreate(itemWidth, itemHeight, itemWidth + 100, itemHeight + i); // 100 for test
        OH_Drawing_Rect* imageRect =
            OH_Drawing_RectCreate(itemWidth * 3, itemHeight, itemWidth * 3 + 100, itemHeight + i); // 3, 100 for test
        OH_Drawing_CanvasDrawImageRectWithSrc(
            canvas, image, imageRectSrc, imageRectDst, samplingOptions, STRICT_SRC_RECT_CONSTRAINT);
        OH_Drawing_CanvasDrawImageRect(canvas, image, imageRect, samplingOptions);
        OH_Drawing_RectDestroy(imageRect);
        OH_Drawing_RectDestroy(imageRectDst);
    }
    OH_Drawing_RectDestroy(imageRectSrc);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_0800
@Description:超大源绘制性能-OH_Drawing_CanvasDrawPixelMapRect
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、图片处理相关接口调用：
        OH_Drawing_RectCreate，重复调用1000次
        OH_Drawing_CanvasDrawPixelMapRect，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawPixelMap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    const int32_t itemWidth = 100;
    const int32_t itemHeight = 100;
    OH_Pixelmap_InitializationOptions* createOps = nullptr;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps);
    int32_t imageWidth = 100;
    int32_t imageHeight = 100;
    OH_PixelmapInitializationOptions_SetWidth(createOps, imageWidth);
    OH_PixelmapInitializationOptions_SetHeight(createOps, imageHeight);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps, 3);    // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps, 2);      // 2 is ALPHA_FORMAT_PREMUL
    size_t bufferSize = imageWidth * imageHeight * 4;                 // 4 for test
    void* bitmapAddr = malloc(bufferSize);
    if (bitmapAddr == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t*)bitmapAddr)[i] = DRAW_COLORBLUE;
    }
    OH_PixelmapNative* pixelMapNative = nullptr;
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    ret = OH_PixelmapNative_CreatePixelmap((uint8_t*)bitmapAddr, bufferSize, createOps, &pixelMapNative);
    OH_Drawing_PixelMap* pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMapNative);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* dst = OH_Drawing_RectCreate(itemWidth * 3, 0, itemWidth * 8, itemHeight + i); // 3, 8 for test
        OH_Drawing_CanvasDrawPixelMapRect(canvas, pixelMap, nullptr, dst, samplingOptions);
        OH_Drawing_RectDestroy(dst);
    }
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_PixelmapNative_Release(pixelMapNative);
    free(bitmapAddr);
    OH_PixelmapInitializationOptions_Release(createOps);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

void PerformanceCanvasQuickRejectPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    bool quickReject = false;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 100 + i * 0.01f, 100 + i * 0.01f);
        OH_Drawing_PathLineTo(path, 150 + i * 0.01f, 50 + i * 0.01f);
        OH_Drawing_CanvasQuickRejectPath(canvas, path, &quickReject);
        OH_Drawing_PathDestroy(path);
    }
}

void PerformanceCanvasQuickRejectRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    bool quickReject = false;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(
            100 + i * 0.01f, 100 + i * 0.01f, 300 + i * 0.01f, 300 + i * 0.01f);
        OH_Drawing_CanvasQuickRejectRect(canvas, rect, &quickReject);
        OH_Drawing_RectDestroy(rect);
    }
}

void PerformanceCanvasDrawArcWithCenter::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(
            100 + i * 0.01f, 100 + i * 0.01f, 300 + i * 0.01f, 300 + i * 0.01f);
        OH_Drawing_CanvasDrawArcWithCenter(canvas, rect, 0, 180, true);
        OH_Drawing_RectDestroy(rect);
    }
}

void PerformanceCanvasDrawNestedRoundRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(
            100 + i * 0.01f, 100 + i * 0.01f, 300 + i * 0.01f, 300 + i * 0.01f);
        OH_Drawing_RoundRect* outer = OH_Drawing_RoundRectCreate(rect, 5, 3);
        OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(
            120 + i * 0.01f, 120 + i * 0.01f, 280 + i * 0.01f, 280 + i * 0.01f);
        OH_Drawing_RoundRect* inner = OH_Drawing_RoundRectCreate(rect1, 3, 5);
        OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer, inner);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_RectDestroy(rect1);
        OH_Drawing_RoundRectDestroy(outer);
        OH_Drawing_RoundRectDestroy(inner);
    }
}

void PerformanceCanvasDrawImageNine::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Pixelmap_InitializationOptions* createOps = nullptr;
    auto ret = OH_PixelmapInitializationOptions_Create(&createOps);
    int32_t imageWidth = 100;
    int32_t imageHeight = 100;
    OH_PixelmapInitializationOptions_SetWidth(createOps, imageWidth);
    OH_PixelmapInitializationOptions_SetHeight(createOps, imageHeight);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps, 3);    // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps, 2);      // 2 is ALPHA_FORMAT_PREMUL
    size_t bufferSize = imageWidth * imageHeight * 4;                 // 4 for test
    void* bitmapAddr = malloc(bufferSize);
    if (bitmapAddr == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth * imageHeight; i++) {
        ((uint32_t*)bitmapAddr)[i] = DRAW_COLORBLUE;
    }
    OH_PixelmapNative* pixelMapNative = nullptr;
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    ret = OH_PixelmapNative_CreatePixelmap((uint8_t*)bitmapAddr, bufferSize, createOps, &pixelMapNative);
    OH_Drawing_PixelMap* pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMapNative);
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(0, 0, 500, 500); // 3, 8 for test

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawPixelMapNine(
            canvas, pixelMap, nullptr, dst, OH_Drawing_FilterMode::FILTER_MODE_NEAREST);
    }
    
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_PixelmapNative_Release(pixelMapNative);
    free(bitmapAddr);
    OH_PixelmapInitializationOptions_Release(createOps);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}