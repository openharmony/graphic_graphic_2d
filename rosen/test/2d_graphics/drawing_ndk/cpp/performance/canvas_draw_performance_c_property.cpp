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

#include "canvas_draw_performance_c_property.h"

#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
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
#include <rawfile/raw_file.h>
#include <unistd.h>

#include "hilog/log.h"
#include "interface/sample_option_test.h"
#include "native_drawing/drawing_error_code.h"
#include "native_drawing/drawing_pixel_map.h"
#include "native_drawing/drawing_shadow_layer.h"
#include "performance.h"
#include "test_common.h"

#include "common/drawing_type.h"
#include "common/log_common.h"

std::map<uint32_t, std::string> PerformanceCCanvasLargeImageLoad::functionNameMap_ = {
    { LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC, "DrawImageRectWithSrc" },
    { LARGE_IMAGE_LOAD_FUNCTION_CANVAS_DRAW_IMAGE_RECT, "DrawImageRect" },
    { LARGE_IMAGE_LOAD_FUNCTION_CANVAS_READ_PIXELS, "ReadPixels" }
};

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1700
@Description:超多顶点数组绘制-OH_Drawing_CanvasDrawVertices
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、OH_Drawing_CanvasDrawVertices，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线；
 */
void PerformanceCCanvasDrawVerticesBrush::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("SubBasicGraphicsSpecialPerformanceCProperty OnTestPerformance OH_Drawing_CanvasDrawVertices");
    if (!canvas) {
        DRAWING_LOGE("PerformanceDrawOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    // 10 testCount, if >10, may crash
    testCount_ = 10;
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    OH_Drawing_VertexMode mode = VERTEX_MODE_TRIANGLES;
    const int32_t vertexCount = 1000; // 1000 for test
    OH_Drawing_Point2D positions[vertexCount];

    OH_Drawing_Point2D textures[vertexCount];

    uint32_t colors[vertexCount];
    uint16_t indexes[] = { 0, 1, 2, 3, 4 }; // 1, 2, 3, 4 for test
    uint32_t min = 0;
    uint32_t max = 500;
    uint32_t colorMax = 255;
    for (int i = 0; i < testCount_; i++) {
        for (int i = 0; i < vertexCount; i++) {
            positions[i] = { itemWidth + static_cast<float>(CreateRandomNum(min, max)),
                itemHeight + static_cast<float>(CreateRandomNum(min, max)) };
        }
        for (int i = 0; i < vertexCount; i++) {
            textures[i] = { itemWidth + static_cast<float>(CreateRandomNum(min, max)),
                itemHeight + static_cast<float>(CreateRandomNum(min, max)) };
        }
        // 4 for test
        uint32_t color1 = ((CreateRandomNum(min, colorMax)) << 4);
        // 2 for test
        uint32_t color2 = ((CreateRandomNum(min, colorMax)) << 2);
        uint32_t color3 = CreateRandomNum(min, colorMax);
        for (int i = 0; i < vertexCount; i++) {
            colors[i] = 0xFF000000 + color1 + color2 + color3;
        }
        OH_Drawing_CanvasDrawVertices(
            canvas, mode, vertexCount, positions, textures, colors, 0, indexes, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

void PerformanceCCanvasReadPixels::CreatePixelMap()
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
        if (i % 3 == 0) {
            srcPixels_[i] = 0xFF5500FF;
            // 3, 1 for test
        } else if (i % 3 == 1) {
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
    // 3, 100 for test
        itemHeight * 3 + 100);
    dstPixels_ = static_cast<char*>(malloc(imageWidth_ * imageHeight_ * 4)); // 4 rgba
    if (dstPixels_ == nullptr) {
        return;
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1800
@Description:超大图片绘制性能-OH_Drawing_CanvasReadPixels attach pen and brush
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、OH_Drawing_CanvasReadPixels，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线；
 */
void PerformanceCCanvasReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceCCanvasReadPixels OnTestPerformance");
    if (!canvas) {
        DRAWING_LOGE("PerformanceCCanvasReadPixels canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    bool res;
    CreatePixelMap();
    uint32_t bitmapX = 100; // 100 for test
    uint32_t bitmapY = 100; // 100 for test
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap1_, bitmapX, bitmapY);
    for (int i = 0; i < testCount_; i++) {
        res = OH_Drawing_CanvasReadPixels(canvas, &imageInfo_, dstPixels_, imageWidth_ * 4, bitmapX, bitmapY); // 4 rgba
    }
    if (!res) {
        DRAWING_LOGE("PerformanceDrawOpsWitch OH_Drawing_CanvasReadPixels failed");
    }
    OH_Drawing_Bitmap* bitmap1 = OH_Drawing_BitmapCreateFromPixels(&imageInfo_, dstPixels_, imageWidth_ * 4);
    // 500,500 bitmap position (x,y)
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap1, 500, 500);
    OH_Drawing_BitmapDestroy(bitmap1);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

void PerformanceCCanvasReadPixelsToBitmap::CreatePixelMap()
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
            // 2/3 for test
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
    // 3, 100 for test
    imageRect_ = OH_Drawing_RectCreate(itemWidth * 3, itemHeight * 3, itemWidth * 3 + 100,
    // 3 for test
        itemHeight * 3 + imageHeight_);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_1900
@Description:超大图片绘制性能-OH_Drawing_CanvasReadPixelsToBitmap attach pen and brush,attach pen,attach brush
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、OH_Drawing_CanvasReadPixelsToBitmap，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线；
 */
void PerformanceCCanvasReadPixelsToBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("SubBasicGraphicsSpecialPerformanceCProperty OnTestPerformance OH_Drawing_CanvasReadPixelsToBitmap");
    if (!canvas) {
        DRAWING_LOGE("PerformanceDrawOpsWitch OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);

    OH_Drawing_CanvasDrawImageRect(canvas, image_, imageRect_, samplingOptions_);

    bool res;
    for (int i = 0; i < testCount_; i++) {
        res = OH_Drawing_CanvasReadPixelsToBitmap(
            canvas, bitmap1_, OH_Drawing_RectGetLeft(imageRect_), OH_Drawing_RectGetTop(imageRect_));
    }
    if (!res) {
        DRAWING_LOGE("PerformanceDrawOpsWitch OH_Drawing_CanvasReadPixelsToBitmap faild");
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}

void PerformanceCCanvasLargeImageLoad::CreatePixelMap()
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
            // 2/3 for test
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
    imageRectSrc_ = OH_Drawing_RectCreate(0, 0, 100, 100); // 100 for test
    OH_Drawing_ImageBuildFromBitmap(image_, bitmap1_);
}

// 超大字体包加载性能OH_Drawing_CanvasDrawImageRectWithSrc
void PerformanceCCanvasLargeImageLoad::DrawImageRectWithSrcTest(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDrawImageRect(canvas, image_, imageRectSrc_, samplingOptions_);
    for (int i = 0; i < testCount_; i++) {
        // 0.5, 3, 0.5, 3, for test
        OH_Drawing_Rect* imageRectDst =
            OH_Drawing_RectCreate(itemWidth + i * 0.5, itemHeight, itemWidth + imageWidth_ + i * 0.5, itemHeight * 6);
        OH_Drawing_CanvasDrawImageRectWithSrc(
            canvas, image_, imageRectSrc_, imageRectDst, samplingOptions_, STRICT_SRC_RECT_CONSTRAINT);
        OH_Drawing_RectDestroy(imageRectDst);
    }
}

// 超大字体包加载性能OH_Drawing_CanvasDrawImageRect
void PerformanceCCanvasLargeImageLoad::DrawImageRectTest(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        // 0.5, 3, 0.5, 3, for test
        OH_Drawing_Rect* imageRect =
            OH_Drawing_RectCreate(itemWidth + i * 0.5, itemHeight, itemWidth + imageWidth_ + i * 0.5, itemHeight * 6);
        OH_Drawing_CanvasDrawImageRect(canvas, image_, imageRect, samplingOptions_);
        OH_Drawing_RectDestroy(imageRect);
    }
}

// 超大字体包加载性能OH_Drawing_CanvasReadPixels
void PerformanceCCanvasLargeImageLoad::ReadPixelsTest(OH_Drawing_Canvas* canvas)
{
    uint32_t bitmapX = 100; // 100 for test
    uint32_t bitmapY = 100; // 100 for test
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap1_, bitmapX, bitmapY);
    OH_Drawing_Image_Info imageInfo { imageWidth_, imageHeight_, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    bool res;
    for (int i = 0; i < testCount_; i++) {
        // imageWidth * 4 argb buffer width
        res = OH_Drawing_CanvasReadPixels(canvas, &imageInfo, dstPixels_, imageWidth_ * 4, bitmapX, bitmapY);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_PROPERTY_2000
@Description:超大字体包加载性能OH_Drawing_CanvasDrawImageRectWithSrc、OH_Drawing_CanvasDrawImageRect、OH_Drawing_CanvasReadPixels
@Step:
    1、attach pen，attach brush
    2、brush 设置所有效果
    3、pen 设置所有效果
    4、图片处理相关接口调用：OH_Drawing_CanvasDrawImageRectWithSrc，重复调用1000次
    OH_Drawing_CanvasDrawImageRect，重复调用1000次
    OH_Drawing_CanvasReadPixels，重复调用1000次
    5、仅attach pen，重复操作4
    6、仅attach brush，重复操作4
@Result:
    1、无内存泄露
    2、程序运行正常
    3、指令数符合基线
    4、获取的运行时间小于基线；
 */
void PerformanceCCanvasLargeImageLoad::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PerformanceCCanvasLargeImageLoad OnTestPerformance");
    if (!canvas) {
        DRAWING_LOGE("PerformanceCLargeImageLoad OnTestPerformance canvas ==nullptr");
        return;
    }
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    performance_->AttachWithType(canvas, attachType_);
    handlers_[function_](canvas);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_CanvasDetachBrush(canvas);
}