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

#include "canvas_draw_performance_c_operation_switch.h"

#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>

#include "hilog/log.h"
#include "native_drawing/drawing_error_code.h"
#include "native_drawing/drawing_pixel_map.h"
#include "native_drawing/drawing_shadow_layer.h"
#include "performance.h"
#include "test_common.h"

#include "common/drawing_type.h"
#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0100
@Description:曲线-直线绘制切换
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、OH_Drawing_CanvasDrawLine绘制直线、OH_Drawing_CanvasDrawPath绘制二阶曲线，
       OH_Drawing_CanvasDrawPath绘制直线，OH_Drawing_CanvasDrawPath绘制三阶曲线
    5、drawpath 偏移但与前一次操作区域相交，重复调用1000次
    6、仅attach pen，重复操作5
    7、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawLineAndPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceDrawOpsWitch OnTestPerformance");
    // 100, testCount, >100 may crash
    testCount_ = 100;
    if (!canvas) {
        DRAWING_LOGE("PerformanceDrawOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    // 100 init x,move position x
    int32_t x = 100;
    // 100 init y,move position y
    int32_t y = 100;
    performance_->AttachWithType(canvas, attachType_);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        // 10 for test,OH_Drawing_CanvasDrawLine绘制直线
        OH_Drawing_CanvasDrawLine(canvas, x, y, x + 10, y + 10);
        OH_Drawing_PathReset(path);
        OH_Drawing_PathMoveTo(path, x, y);
        // 60 for test,OH_Drawing_CanvasDrawPath绘制直线
        OH_Drawing_PathLineTo(path, x + 60, y);
        OH_Drawing_CanvasDrawPath(canvas, path);
        // 30,10,60 for test,OH_Drawing_CanvasDrawPath绘制二阶曲线
        OH_Drawing_PathQuadTo(path, x + 30, y + 10, x + 60, y);
        OH_Drawing_CanvasDrawPath(canvas, path);
        // 30,40,50,120 for test,OH_Drawing_CanvasDrawPath绘制三阶曲线
        OH_Drawing_PathCubicTo(path, x + 30, y + 3, x + 40, y + 50, x + 120, y);
        OH_Drawing_CanvasDrawPath(canvas, path);
        // 15 for test,move position x
        x += 15;
        // 600 for test,end x
        if (x > 600) {
            // 100 init x,move position x
            x = 100;
            // 15 for test,move position y
            y += 15;
        }
    }
    OH_Drawing_PathDestroy(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH_0200
@Description:绘制操作切换
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、顺序调用draw相关查操作：OH_Drawing_CanvasDrawLine、OH_Drawing_CanvasDrawPath、
        OH_Drawing_CanvasDrawPixelMapRect、OH_Drawing_CanvasDrawRegion、OH_Drawing_CanvasDrawPoints、
        OH_Drawing_CanvasDrawBitmap、OH_Drawing_CanvasDrawBitmapRect、OH_Drawing_CanvasDrawRect、
        OH_Drawing_CanvasDrawCircle、OH_Drawing_CanvasDrawOval、OH_Drawing_CanvasDrawArc、OH_Drawing_CanvasDrawRoundRect、
        OH_Drawing_CanvasDrawTextBlob、OH_Drawing_CanvasDrawShadow、OH_Drawing_CanvasDrawImageRectWithSrc、
        OH_Drawing_CanvasDrawImageRect，重复调用1000次，获取调用时间
    6、仅attach pen，重复操作5
    7、仅attach brush，重复操作5
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线
 */
void PerformanceDrawOpsWitch::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceDrawOpsWitch OnTestPerformance");
    // 10 testCount,if textCount >10 ,may crash
    testCount_ = 10;
    if (!canvas) {
        DRAWING_LOGE("PerformanceDrawOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);

    performance_->AttachWithType(canvas, attachType_);
    // 10 testCount
    for (int i = 0; i < testCount_; i++) {
        xOffset_ = i % itemWidth;
        for (int j = 0; j < CANVAS_DRAW_FUNCTION_MAX; j++) {
            handlers_[j](canvas);
        }
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

void PerformanceDrawOpsWitch::CreatePixelMap()
{
    samplingOptions_ = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NEAREST);
    image_ = OH_Drawing_ImageCreate();
    // draw bitmap

    OH_Drawing_BitmapFormat bitmapFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };

    OH_Drawing_Image_Info imageInfo { imageWidth_, imageHeight_, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    // 4 rgba
    srcPixels_ = new uint32_t[imageWidth_ * imageHeight_];
    if (srcPixels_ == nullptr) {
        return;
    }
    for (int i = 0; i < imageWidth_ * imageHeight_; i++) {
        // 3 for test
        if (i < imageWidth_ * imageHeight_ / 3) {
            srcPixels_[i] = 0xFF5500FF;
            // 3, 2 for test
        } else if (i < imageWidth_ * imageHeight_ * 2 / 3) {
            srcPixels_[i] = 0xFF0055FF;
        } else {
            srcPixels_[i] = 0xFFFF5500;
        }
    }

    bitmap1_ = OH_Drawing_BitmapCreateFromPixels(&imageInfo, srcPixels_, imageWidth_ * 4); // 4 for test
    if (!bitmap1_) {
        DRAWING_LOGE("SubBasicGraphicsSpecialPerformance create bitmap error");
    }

    OH_Drawing_ImageBuildFromBitmap(image_, bitmap1_);
    imageRectSrc_ = OH_Drawing_RectCreate(0, 0, 100, 100); // 100 for test
    // 3, 100 for test
    imageRect_ = OH_Drawing_RectCreate(itemWidth * 3, itemHeight * 3, itemWidth * 3 + 100,
        itemHeight * 3 + 100); // 3, 100 for test

    auto ret = OH_PixelmapInitializationOptions_Create(&createOps_);
    if (ret != IMAGE_SUCCESS || !createOps_) {
        DRAWING_LOGE(
            "PerformanceDrawOpsWitch:: OH_PixelmapInitializationOptions_Create failed %{public}d",
            ret);
        return;
    }
    int32_t imageWidth = 1024 * 4;  // 1024 * 4 for test,4k
    int32_t imageHeight = 1080 * 2; // 1024 * 4 for test,4k
    OH_PixelmapInitializationOptions_SetWidth(createOps_, imageWidth);
    OH_PixelmapInitializationOptions_SetHeight(createOps_, imageHeight);
    OH_PixelmapInitializationOptions_SetPixelFormat(createOps_, 3);    // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetSrcPixelFormat(createOps_, 3); // 3 is RGBA fromat
    OH_PixelmapInitializationOptions_SetAlphaType(createOps_, 2);      // 2 is ALPHA_FORMAT_PREMUL
    OH_PixelmapNative_CreatePixelmap(
        (uint8_t*)srcPixels_, imageWidth_ * imageHeight_ * 4, createOps_, &pixelMapNative_); // 4 rgba
    pixelMap_ = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMapNative_);
}

void PerformanceDrawOpsWitch::DrawLineTest(OH_Drawing_Canvas* canvas)
{
    // 30,70 for test
    OH_Drawing_CanvasDrawLine(canvas, itemWidth + xOffset_, itemHeight, itemWidth + 30, itemHeight + 70);
}

void PerformanceDrawOpsWitch::DrawPathTest(OH_Drawing_Canvas* canvas)
{
    // draw path
    OH_Drawing_PathReset(path_);
    // 2, for test
    OH_Drawing_PathMoveTo(path_, itemWidth * 2 + xOffset_, itemHeight);
    // 2,50,50 for test
    OH_Drawing_PathLineTo(path_, itemWidth * 2 + 50, itemHeight + 50);
    // 2 for test
    int multiple = 2;
    // 50 for test
    float left = itemWidth * multiple + 50;
    // 50 for test
    float top = itemHeight + 50;
    // 80 for test
    float right = itemWidth * multiple + 80;
    // 90 for test
    float bottom = itemHeight + 90;
    OH_Drawing_PathAddRect(path_, left, top, right, bottom, PATH_DIRECTION_CCW);
    OH_Drawing_CanvasDrawPath(canvas, path_);
}

void PerformanceDrawOpsWitch::DrawPixelMapRect(OH_Drawing_Canvas* canvas)
{
    // 3, 4 for test
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(
        itemWidth * 3 + xOffset_, itemHeight, itemWidth * 4 + xOffset_, itemHeight + imageHeight_);
    OH_Drawing_CanvasDrawPixelMapRect(canvas, pixelMap_, nullptr, dst, samplingOptions_);
    OH_Drawing_RectDestroy(dst);
}

void PerformanceDrawOpsWitch::DrawRegion(OH_Drawing_Canvas* canvas)
{
    // 4,4,70,70 for test
    OH_Drawing_Rect* regionRect =
        OH_Drawing_RectCreate(itemWidth * 4 + xOffset_, itemHeight, itemWidth * 4 + 70 + xOffset_, itemHeight + 70);
    OH_Drawing_RegionSetRect(region_, regionRect);
    OH_Drawing_CanvasDrawRegion(canvas, region_);
    OH_Drawing_RectDestroy(regionRect);
}

void PerformanceDrawOpsWitch::DrawPoints(OH_Drawing_Canvas* canvas)
{
    // draw point
    const int32_t pointCount = 100; // 100 for test
    OH_Drawing_Point2D point2D[pointCount];
    for (int i = 0; i < pointCount; i++) {
        point2D[i] = { itemWidth + i + xOffset_ * 1.f, itemHeight + i * 1.f };
    }
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS, pointCount, point2D);
}

void PerformanceDrawOpsWitch::DrawBitmap(OH_Drawing_Canvas* canvas)
{
    // 6 for test position x
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap1_, itemWidth * 6 + xOffset_, itemHeight);
}

void PerformanceDrawOpsWitch::DrawBitmapRect(OH_Drawing_Canvas* canvas)
{
    // 2,100 for test
    OH_Drawing_Rect* bitMapDst =
        OH_Drawing_RectCreate(itemWidth + xOffset_, itemHeight * 2, itemWidth * 2 + xOffset_, itemHeight * 2 + 100);
    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap1_, imageRectSrc_, bitMapDst, samplingOptions_);
    OH_Drawing_RectDestroy(bitMapDst);
}

void PerformanceDrawOpsWitch::DrawRect(OH_Drawing_Canvas* canvas)
{
    // draw rect, 2,20,30,50 for test
    OH_Drawing_Rect* canvasRect = OH_Drawing_RectCreate(
        itemWidth * 2 + xOffset_, itemHeight * 2 + 20, itemWidth * 2 + xOffset_ + 30, itemHeight * 2 + 50);
    OH_Drawing_CanvasDrawRect(canvas, canvasRect);
    OH_Drawing_RectDestroy(canvasRect);
}

void PerformanceDrawOpsWitch::DrawCircle(OH_Drawing_Canvas* canvas)
{
    // 10 for test
    OH_Drawing_CanvasDrawCircle(canvas, point_, 10 + xOffset_);
}

void PerformanceDrawOpsWitch::DrawOval(OH_Drawing_Canvas* canvas)
{
    // 4,2,10,50,80 for test
    OH_Drawing_Rect* ovalRect = OH_Drawing_RectCreate(
        itemWidth * 4 + xOffset_, itemHeight * 2 + 10, itemWidth * 4 + 50 + xOffset_, itemHeight * 2 + 80);
    OH_Drawing_CanvasDrawOval(canvas, ovalRect);
    OH_Drawing_RectDestroy(ovalRect);
}

void PerformanceDrawOpsWitch::DrawArc(OH_Drawing_Canvas* canvas)
{
    // 1.8 for test
    OH_Drawing_CanvasDrawArc(canvas, arcRect_, 0, 1.8 + xOffset_);
}

void PerformanceDrawOpsWitch::DrawRoundRect(OH_Drawing_Canvas* canvas)
{
    // 5, 10 for test
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(roundRectRect_, 5 + xOffset_, 10);
    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

void PerformanceDrawOpsWitch::DrawTextBlob(OH_Drawing_Canvas* canvas)
{
    // 3, 50 for test,mean right
    OH_Drawing_CanvasDrawTextBlob(canvas, textBlob_, itemWidth + xOffset_, itemHeight * 3 + 50);
}

void PerformanceDrawOpsWitch::DrawShadow(OH_Drawing_Canvas* canvas)
{
    // 5, 1.f, 6, 7, 5 for test
    OH_Drawing_Point3D planeParams = { 5 + xOffset_ * 1.f, 6, 7 };
    // 1, 2, 3, for test
    OH_Drawing_Point3D devLightPos = { 1, 2, 3 };
    float lightRadius = 5;
    OH_Drawing_CanvasDrawShadow(
        canvas, shadowPath_, planeParams, devLightPos, lightRadius, 0xFF0000FF, 0xFFFF00FF, SHADOW_FLAGS_NONE);
}

void PerformanceDrawOpsWitch::DrawImageRectWithSrc(OH_Drawing_Canvas* canvas)
{
    // 2,3,100 for test
    OH_Drawing_Rect* imageRectDst = OH_Drawing_RectCreate(
        itemWidth * 2 + xOffset_, itemHeight * 3, itemWidth * 2 + xOffset_ + 100, itemHeight * 3 + 100);
    OH_Drawing_CanvasDrawImageRectWithSrc(
        canvas, image_, imageRectSrc_, imageRectDst, samplingOptions_, STRICT_SRC_RECT_CONSTRAINT);
    OH_Drawing_RectDestroy(imageRectDst);
}

void PerformanceDrawOpsWitch::DrawImageRect(OH_Drawing_Canvas* canvas)
{
    // 3,100 for test
    OH_Drawing_Rect* imageRect = OH_Drawing_RectCreate(
        itemWidth * 3 + xOffset_, itemHeight * 3, itemWidth * 3 + xOffset_ + 100, itemHeight * 3 + 100);
    OH_Drawing_CanvasDrawImageRect(canvas, image_, imageRect, samplingOptions_);
    OH_Drawing_RectDestroy(imageRect);
}