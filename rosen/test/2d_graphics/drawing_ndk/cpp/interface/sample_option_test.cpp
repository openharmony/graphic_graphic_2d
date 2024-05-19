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

#include "sample_option_test.h"

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

void SamplingOptionsCreate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat { COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE };
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat); // 200, 200 创建位图大小
    OH_Drawing_Canvas* bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(l, t, r, b);
    OH_Drawing_SamplingOptions* sampling = nullptr;
    for (int i = 0; i < testCount_; i++) {
        sampling = OH_Drawing_SamplingOptionsCreate(filterMode, mipmapMode);
        OH_Drawing_SamplingOptionsDestroy(sampling);
    }
    sampling = OH_Drawing_SamplingOptionsCreate(filterMode, mipmapMode);
    OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, nullptr, dst, sampling);
    OH_Drawing_SamplingOptionsDestroy(sampling);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
}