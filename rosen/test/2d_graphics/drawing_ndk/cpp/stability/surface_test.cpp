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
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>
#include <native_drawing/drawing_types.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SURFACE_0100
@Description:SurfaceCreateFromGpuContext-SurfaceDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、GpuContext和Surface相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crahs
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilitySurfaceCreateFromGpuContext::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("SurfaceCreateFromGpuContext OnTestStability");
    TestRend rand;
    OH_Drawing_GpuContextOptions options;
    // 2 for test,rand true or false
    options.allowPathMaskCaching = (rand.nextU() % 2 == 0);
    OH_Drawing_GpuContext* gpuContext = OH_Drawing_GpuContextCreateFromGL(options);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Image_Info imageInfo;
        imageInfo.width = rand.nextULessThan(bitmapWidth_);
        imageInfo.height = rand.nextULessThan(bitmapHeight_);
        imageInfo.colorType = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        imageInfo.alphaType = DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i);
        // 2 for test,rand true or false
        OH_Drawing_Surface* surface = OH_Drawing_SurfaceCreateFromGpuContext(gpuContext, (i % 2 == 0), imageInfo);
        OH_Drawing_SurfaceDestroy(surface);
    }
    OH_Drawing_GpuContextDestroy(gpuContext);
}

void StabilitySurfaceFuncAll::SurfaceGetCanvasTest(OH_Drawing_Surface* surface)
{
    OH_Drawing_SurfaceGetCanvas(surface);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SURFACE_0200
@Description:SurfaceCreateFromGpuContext-相关配置接口全调用-SurfaceDestroy循环,无配置接口，改为操作接口全调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、GpuContext和Surface相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crahs
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilitySurfaceFuncAll::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("SurfaceFuncAll OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_GpuContextOptions options;
        // 2 for test,rand true or false
        options.allowPathMaskCaching = (i % 2 == 0);
        OH_Drawing_GpuContext* gpuContext = OH_Drawing_GpuContextCreateFromGL(options);
        OH_Drawing_Image_Info imageInfo;
        imageInfo.width = rand_.nextULessThan(bitmapWidth_);
        imageInfo.height = rand_.nextULessThan(bitmapHeight_);
        imageInfo.colorType = DrawGetEnum(COLOR_FORMAT_ALPHA_8, COLOR_FORMAT_BGRA_8888, i);
        imageInfo.alphaType = DrawGetEnum(ALPHA_FORMAT_OPAQUE, ALPHA_FORMAT_UNPREMUL, i);
        // 2 for test,rand true or false
        OH_Drawing_Surface* surface = OH_Drawing_SurfaceCreateFromGpuContext(gpuContext, (i % 2 == 0), imageInfo);
        handlers_[rand_.nextRangeU(0, OPERATION_FUNCTION_SURFACE_MAX - 1)](surface);
        OH_Drawing_GpuContextDestroy(gpuContext);
        OH_Drawing_SurfaceDestroy(surface);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_GPU_CONTEXT_0100
@Description:GpuContextCreateFromGL-GpuContextDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、GpuContext相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crahs
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityGpuContextCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_GpuContextOptions options;
        // 2 for test,rand true or false
        options.allowPathMaskCaching = (i % 2 == 0);
        OH_Drawing_GpuContext* gpuContext = OH_Drawing_GpuContextCreateFromGL(options);
        OH_Drawing_GpuContextDestroy(gpuContext);
    }
}