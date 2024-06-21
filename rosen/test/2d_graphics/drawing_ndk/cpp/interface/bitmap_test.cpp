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

#include "bitmap_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

#include "common/log_common.h"

void ImageBuildFromBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_BitmapFormat format = { COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bm, 256, 256, &format); // 256, 256 宽高
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR, MIPMAP_MODE_NEAREST);

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ImageBuildFromBitmap(image, bm);
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 256, 256); // 0, 0, 256, 256 创建矩形
    OH_Drawing_CanvasDrawImageRect(canvas, image, rect, samplingOptions);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_BitmapDestroy(bm);
}

void BitmapReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 性能测试:100000次66ms
    const unsigned int width = 100;  // 100 宽度
    const unsigned int height = 100; // 100 高度
    OH_Drawing_Bitmap* cBitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat bitmapFormat { fCT, fAT };
    OH_Drawing_BitmapBuild(cBitmap, width, height, &bitmapFormat);
    OH_Drawing_Image_Info imageInfo { width, height, fCT, fAT };
    void* pixels = new uint32_t[width * height];
    bool resTrue = false;
    // 将位图中的矩形区域像素数据读取到指定的内存缓冲区中,循环攻击此接口
    for (int i = 0; i < testCount_; i++) {
        resTrue =
            OH_Drawing_BitmapReadPixels(cBitmap, &imageInfo, pixels, width * 4, 0, 0); // 4目标像素数据每行的字节数
    }
    if (resTrue == true) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, width, height);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
    // 释放
    if (pixels != nullptr) {
        delete[] reinterpret_cast<uint32_t*>(pixels);
        pixels = nullptr;
    }
    OH_Drawing_BitmapDestroy(cBitmap);
}

void BitmapBuild::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 128;
    int32_t h = 128;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    void* pixels = OH_Drawing_BitmapGetPixels(bm);
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_ARGB_4444, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BitmapBuild(bm, w, h, &cFormat);
    }
    OH_Drawing_Bitmap* cPixels = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, w);
    OH_Drawing_CanvasBind(canvas, cPixels);  // 将画布与bitmap绑定
    OH_Drawing_CanvasDrawRect(canvas, rect); // 画一个矩形
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cPixels);
    OH_Drawing_BitmapDestroy(bm);
}

void BitmapCreateFromPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 256;
    int32_t h = 256;
    OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
    void* pixels = OH_Drawing_BitmapGetPixels(bm);
    OH_Drawing_Image_Info imageInfo { w, h, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL };
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, w, h);
    OH_Drawing_BitmapBuild(bm, w, h, &cFormat);
    OH_Drawing_Bitmap* cPixels = 0;
    for (int i = 0; i < testCount_; i++) {
        cPixels = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, w);
    }
    OH_Drawing_CanvasBind(canvas, cPixels);  // 将画布与bitmap绑定
    OH_Drawing_CanvasDrawOval(canvas, rect); // 画一个椭圆
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(cPixels);
    OH_Drawing_BitmapDestroy(bm);
}
