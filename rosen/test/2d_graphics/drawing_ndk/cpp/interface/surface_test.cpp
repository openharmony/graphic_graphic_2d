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

#include "surface_test.h"

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

void SurfaceCreateFromGpuContext::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 只能用gpu来画，用cpu会闪退
    TestRend rand;
    const int32_t width = 500;  // 500 绘图表面的宽度
    const int32_t height = 500; // 500 绘图表面的高度
    OH_Drawing_GpuContextOptions options;
    options.allowPathMaskCaching = false;
    OH_Drawing_GpuContext* gpuContext = OH_Drawing_GpuContextCreateFromGL(options);
    OH_Drawing_Image_Info imageInfo = { width, height, COLOR_FORMAT_RGBA_8888,
        ALPHA_FORMAT_OPAQUE }; // COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE 绘图表面的颜色和透明度
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_Surface* surface = nullptr;
    for (int i = 0; i < testCount_; i++) {
        surface = OH_Drawing_SurfaceCreateFromGpuContext(gpuContext, budgeted, imageInfo);
        OH_Drawing_SurfaceDestroy(surface);
    }

    OH_Drawing_CanvasDrawRect(canvas, dst);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_GpuContextDestroy(gpuContext);
}