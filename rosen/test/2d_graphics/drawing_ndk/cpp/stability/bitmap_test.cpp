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

#include <cstdint>
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

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SURFACE_0100
@Description:BitmapCreate-BitmapBuild-相关配置接口全调用-BitmapDestroy循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、执行自动化用例：
    OH_Drawing_BitmapCreate-OH_Drawing_BitmapBuild-OH_Drawing_BitmapDestroy 循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityBitmapCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    uint32_t width = bitmapWidth_;
    uint32_t height = bitmapHeight_;
    void* pixels1 = new uint32_t[width * height];
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
        OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        OH_Drawing_BitmapFormat format = { colorFormat, DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        if (!bm) {
            continue;
        }
        OH_Drawing_BitmapBuild(bm, width, height, &format);
        OH_Drawing_CanvasDrawBitmap(canvas, bm, 0, 0);
        OH_Drawing_BitmapDestroy(bm);
    }
    delete[] pixels1;
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BITMAP_0200
@Description:BitmapCreateFromPixels-相关配置接口全调用-BitmapDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、执行自动化用例：
    OH_Drawing_BitmapCreateFromPixels初始化不同参数bitmap-OH_Drawing_BitmapDestroy 循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityBitmapCreateFromPixels::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityBitmapCreateFromPixels OnTestStability");
    int32_t width = bitmapWidth_;
    int32_t height = bitmapHeight_;
    uint32_t* pixels = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        pixels[i] = 0xFFFF0000;
    }
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        OH_Drawing_Image_Info imageInfo { width, height, colorFormat,
            DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        // 4 rgba
        OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * 4);
        if (!bm) {
            // 会存在报错，format，colorFormat会存在不匹配的情况，正常现象
            continue;
        }
        OH_Drawing_CanvasDrawBitmap(canvas, bm, 0, 0);
        OH_Drawing_BitmapDestroy(bm);
    }

    delete[] pixels;
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BITMAP_0300
@Description:BitmapBuild多次循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、执行自动化用例：
    OH_Drawing_BitmapCreateFromPixels，OH_Drawing_BitmapBuild循环调用初始化不同bitmap1000次，OH_Drawing_BitmapDestroy
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityBitmapInit::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityBitmapInit OnTestStability");
    int32_t width = bitmapWidth_;
    int32_t height = bitmapHeight_;
    uint32_t* pixels = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        pixels[i] = 0xFFFF0000;
    }
    OH_Drawing_Bitmap* bm = nullptr;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        OH_Drawing_BitmapFormat format = { colorFormat, DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        OH_Drawing_Image_Info imageInfo { width, height, colorFormat,
            DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        // 2 for test,rand true or false
        if (i % 2 == 0) {
            bm = OH_Drawing_BitmapCreate();
            OH_Drawing_BitmapBuild(bm, width, height, &format);
        } else {
            ////4 rgba, 会存在报错，format，colorFormat会存在不匹配的情况，正常现象
            bm = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * 4);
        }
        if (!bm) {
            // 会存在报错，format，colorFormat会存在不匹配的情况，正常现象
            continue;
        }
        OH_Drawing_CanvasDrawBitmap(canvas, bm, 0, 0);
        OH_Drawing_BitmapDestroy(bm);
        bm = nullptr;
    }
    delete[] pixels;
}

void StabilityBitmapRandInvoke::BitmapGetWidthTest(OH_Drawing_Bitmap* bitmap)
{
    OH_Drawing_BitmapGetWidth(bitmap);
}

void StabilityBitmapRandInvoke::BitmapGetHeightTest(OH_Drawing_Bitmap* bitmap)
{
    OH_Drawing_BitmapGetHeight(bitmap);
}

void StabilityBitmapRandInvoke::BitmapGetColorFormatTest(OH_Drawing_Bitmap* bitmap)
{
    OH_Drawing_BitmapGetColorFormat(bitmap);
}

void StabilityBitmapRandInvoke::BitmapGetAlphaFormatTest(OH_Drawing_Bitmap* bitmap)
{
    OH_Drawing_BitmapGetAlphaFormat(bitmap);
}

void StabilityBitmapRandInvoke::BitmapGetPixelsTest(OH_Drawing_Bitmap* bitmap)
{
    OH_Drawing_BitmapGetPixels(bitmap);
}

void StabilityBitmapRandInvoke::BitmapGetImageInfoTest(OH_Drawing_Bitmap* bitmap)
{
    OH_Drawing_Image_Info info;
    OH_Drawing_BitmapGetImageInfo(bitmap, &info);
}

void StabilityBitmapRandInvoke::BitmapReadPixelsTest(OH_Drawing_Bitmap* bitmap)
{
    uint32_t* pixels1 = new uint32_t[bitmapWidth_ * bitmapHeight_];
    OH_Drawing_Image_Info info;
    OH_Drawing_BitmapGetImageInfo(bitmap, &info);
    OH_Drawing_BitmapReadPixels(bitmap, &info, pixels1, bitmapWidth_ * 4, 0, 0); //4 rgba
    delete[] pixels1;
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_BITMAP_0400
@Description:Bitmap相关操作随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、执行自动化用例：
    OH_Drawing_BitmapCreate，OH_Drawing_BitmapBuild
    bitmap相关设置获取接口调用，循环调用1000次
    OH_Drawing_BitmapDestroy
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityBitmapRandInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGE("StabilityBitmapRandInvoke OnTestStability");
    int32_t width = bitmapWidth_;
    int32_t height = bitmapHeight_;
    uint32_t* pixels = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        pixels[i] = 0xFFFF0000;
    }
    OH_Drawing_Bitmap* bm = nullptr;
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        OH_Drawing_BitmapFormat format = { colorFormat, DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        OH_Drawing_Image_Info imageInfo { width, height, colorFormat,
            DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        // 2 for test,rand true or false
        if (i % 2 == 0) {
            bm = OH_Drawing_BitmapCreate();
            OH_Drawing_BitmapBuild(bm, width, height, &format);
        } else {
            // 4 rgba,会存在报错，format，colorFormat会存在不匹配的情况，正常现象
            bm = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * 4);
        }
        uint32_t index = rand.nextRangeU(0, OPERATION_FUNCTION_BITMAP_MAX - 1);
        handlers_[index](bm);
        if (!bm) {
            // 会存在报错，format，colorFormat会存在不匹配的情况，正常现象
            continue;
        }
        OH_Drawing_CanvasDrawBitmap(canvas, bm, 0, 0);
        OH_Drawing_BitmapDestroy(bm);
        bm = nullptr;
    }
    delete[] pixels;
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_0100
@Description:ImageCreate-ImageBuildFromBitmap-ImageDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Image和Bitmap相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityImageCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    uint32_t width = bitmapWidth_;
    uint32_t height = bitmapHeight_;
    void* pixels1 = new uint32_t[width * height];
    for (int i = 0; i < testCount_; i++) {
        DRAWING_LOGI("StabilityImageCreate %{public}d", i);
        OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
        OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        OH_Drawing_BitmapFormat format = { colorFormat, DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        if (!bm) {
            DRAWING_LOGE("StabilityImageCreate OnTestStability error, bitmap empty", i);
            continue;
        }
        OH_Drawing_BitmapBuild(bm, width, height, &format);
        OH_Drawing_Image* image = OH_Drawing_ImageCreate();
        OH_Drawing_ImageBuildFromBitmap(image, bm);
        OH_Drawing_ImageDestroy(image);
        OH_Drawing_BitmapDestroy(bm);
    }
    delete[] pixels1;
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_0200
@Description:ImageCreate-相关配置接口全调用-ImageDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Image和Bitmap相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityImageInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityImageInvoke OnTestStability");
    uint32_t width = bitmapWidth_;
    uint32_t height = bitmapHeight_;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Bitmap* bm = OH_Drawing_BitmapCreate();
        OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        OH_Drawing_BitmapFormat format = { colorFormat, DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        if (!bm) {
            DRAWING_LOGE("StabilityImageInvoke OnTestStability error, bitmap empty", i);
            continue;
        }
        OH_Drawing_BitmapBuild(bm, width, height, &format);
        OH_Drawing_Image* image = OH_Drawing_ImageCreate();
        OH_Drawing_ImageBuildFromBitmap(image, bm);
        OH_Drawing_ImageDestroy(image);
        OH_Drawing_BitmapDestroy(bm);
    }
}

void StabilityImageRandInvoke::ImageGetWidthTest(OH_Drawing_Image* image)
{
    OH_Drawing_ImageGetWidth(image);
}

void StabilityImageRandInvoke::ImageGetHeightTest(OH_Drawing_Image* image)
{
    OH_Drawing_ImageGetHeight(image);
}

void StabilityImageRandInvoke::ImageGetImageInfoTest(OH_Drawing_Image* image)
{
    OH_Drawing_Image_Info info;
    OH_Drawing_ImageGetImageInfo(image, &info);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_0300
@Description:Image相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Image和Bitmap相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityImageRandInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityBitmapRandInvoke OnTestStability");
    int32_t width = bitmapWidth_;
    int32_t height = bitmapHeight_;
    uint32_t* pixels = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        pixels[i] = 0xFFFF0000;
    }
    OH_Drawing_Bitmap* bm = nullptr;
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFormat colorFormat = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        OH_Drawing_BitmapFormat format = { colorFormat, DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        OH_Drawing_Image_Info imageInfo { width, height, colorFormat,
            DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i) };
        // 2 for test,rand true or false
        if (i % 2 == 0) {
            bm = OH_Drawing_BitmapCreate();
            OH_Drawing_BitmapBuild(bm, width, height, &format);
        } else {
            // 4 rgba,会存在报错，format，colorFormat会存在不匹配的情况，正常现象
            bm = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * 4);
        }
        if (!bm) {
            // 会存在报错，format，colorFormat会存在不匹配的情况，正常现象
            continue;
        }
        OH_Drawing_Image* image = OH_Drawing_ImageCreate();
        OH_Drawing_ImageBuildFromBitmap(image, bm);
        uint32_t index = rand.nextRangeU(0, OPERATION_FUNCTION_IMAGE_MAX - 1);
        handlers_[index](image);
        OH_Drawing_ImageDestroy(image);
        OH_Drawing_BitmapDestroy(bm);
        bm = nullptr;
    }
    delete[] pixels;
}
