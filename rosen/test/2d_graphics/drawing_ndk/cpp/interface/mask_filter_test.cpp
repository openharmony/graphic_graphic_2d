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

#include "mask_filter_test.h"

#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <cstdio.h>
#include <cstdlib.h>
#include <ctime.h>

#include "test_common.h"

#include "common/log_common.h"

void MaskFilterCreateBlur::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 性能测试:10000次15ms
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    // 创建画刷brush对象
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    // 创建一个滤波器对象
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    // 创建一个模板滤波器
    OH_Drawing_MaskFilter* maskFilter;
    for (int i = 0; i < testCount_; i++) {
        float sigma = 10.f; // 10.f 高斯模糊的标准偏差
        maskFilter = OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, sigma, true);
    }
    // 为滤波器设置模板
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    // 设置画笔滤波器
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    // 上画布
    OH_Drawing_CanvasDrawOval(canvas, rect);
    // 释放
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_BrushDestroy(brush);
}