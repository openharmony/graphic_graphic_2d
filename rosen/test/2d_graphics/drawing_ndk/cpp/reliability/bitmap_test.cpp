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
#include <thread>

#include "interface/canvas_test.h"
#include "test_common.h"

#include "common/log_common.h"

// 两个线程
static void threadFunction(OH_Drawing_Canvas* canvas)
{
    // 100 draw count,for test
    float drawCount = 100;
    for (int i = 0; i < drawCount; i++) {
        DrawingFunc(canvas);
    }
}

/*
@Description:销毁Bitmap后调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind绑定到bitmap画布
    3、OH_Drawing_BitmapDestroy
    4、OH_Drawing_CanvasDraw相关接口调用
    5、OH_Drawing_CanvasDestroy
@Result:
    图片与基线一致
 */
void ReliabilityDeleteBitmap::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDeleteBitmap OnTestReliability");
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = { COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_UNPREMUL };
    OH_Drawing_BitmapBuild(bitmap, bitmapWidth_, bitmapHeight_, &format);
    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_BitmapDestroy(bitmap);
    DrawingFunc(canvas);
}

/*
@Description:销毁pixels后调用draw接口
@Step:
    1、OH_Drawing_CanvasCreate
    2、OH_Drawing_CanvasBind绑定到bitmap画布
    3、销毁pixels
    4、OH_Drawing_CanvasDraw相关接口调用
    5、OH_Drawing_CanvasDestroy
@Result:
    图片与基线一致
 */
void ReliabilityDeletePixel::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDeletePixel OnTestReliability");
    // 300 width,for test
    int32_t width = 300;
    // 300 height,for test
    int32_t height = 300;
    uint32_t* pixels = new uint32_t[width * height];
    for (int i = 0; i < width * height; i++) {
        pixels[i] = 0xFFFF0000;
    }
    OH_Drawing_Image_Info imageInfo { (int32_t)width, (int32_t)height, COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_UNPREMUL };
    // 4 bgra
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, width * 4);
    OH_Drawing_CanvasBind(canvas, bitmap);
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap, 0, 0);
    delete[] pixels;
    OH_Drawing_CanvasDrawBitmap(canvas, bitmap, 0, 0);
    DrawingFunc(canvas);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
@Description:draw过程中，销毁Bitmap
@Step:
    1、启线程1创建Bitmap
    2、启线程2 OH_Drawing_BitmapCreateFromPixels，传入线程1 Bitmap
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建Bitmap
    5、canvas置空
@Result:
    程序运行正常，无crash
 */
void ReliabilityDeleteBitmapJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDeleteBitmapJoinThread OnTestReliability");
    // 100 draw count,for test
    float drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    for (int i = 0; i < drawCount; i++) {
        OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
        OH_Drawing_BitmapFormat format = { COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_UNPREMUL };
        OH_Drawing_BitmapBuild(bitmap, bitmapWidth_, bitmapHeight_, &format);
        OH_Drawing_CanvasBind(canvas, bitmap);
        OH_Drawing_BitmapDestroy(bitmap);
    }
    subThread.join();
}

/*
@Description:draw过程中，销毁pixels
@Step:
    1、启线程1创建pixels
    2、启线程2 OH_Drawing_BitmapCreateFromPixels，传入线程1 pixels
    3、循环调用draw接口
    4、在第3步操作过程中，线程1重复销毁-创建pixels
    5、OH_Drawing_CanvasDestroy
@Result:
    程序运行正常，无crash
 */
void ReliabilityDeletePixelJoinThread::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ReliabilityDeletePixelJoinThread OnTestReliability");
    // 100 draw count,for test
    float drawCount = 100;
    std::thread subThread(threadFunction, canvas);
    for (int i = 0; i < drawCount; i++) {
        uint32_t* pixels = new uint32_t[bitmapWidth_ * bitmapHeight_];
        for (int i = 0; i < bitmapWidth_ * bitmapHeight_; i++) {
            pixels[i] = 0xFFFF0000;
        }
        OH_Drawing_Image_Info imageInfo { (int32_t)bitmapWidth_, (int32_t)bitmapHeight_, COLOR_FORMAT_BGRA_8888,
            ALPHA_FORMAT_UNPREMUL };
        // 4 bgra
        OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, bitmapWidth_ * 4);
        OH_Drawing_CanvasBind(canvas, bitmap);
        delete[] pixels;
        OH_Drawing_BitmapDestroy(bitmap);
    }
    subThread.join();
}
