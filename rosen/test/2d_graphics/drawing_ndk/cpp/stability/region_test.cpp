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

#include "region_test.h"

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

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_REGION_0100
@Description:RegionCreate-RegionDestroy
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_Region-OH_Drawing_RegionDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRegionCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("RegionCreate OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Region* region = OH_Drawing_RegionCreate();
        OH_Drawing_RegionDestroy(region);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_REGION_0200
@Description:RegionCreate-相关配置接口全调用-RegionDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、RegionCreate-相关配置接口全调用-RegionDestroy循环
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRegionAll::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityRegionAll OnTestStability");
    other_ = OH_Drawing_RegionCreate();
    rect_ = OH_Drawing_RectCreate(0, 0, rand_.nextULessThan(bitmapWidth_), rand_.nextULessThan(bitmapHeight_));
    path_ = OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path_, rand_.nextULessThan(bitmapWidth_), rand_.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        index_ = i;
        uint32_t funcIndexMax = SETTING_FUNCTION_REGION_DESTROY;
        for (int j = 0; j <= funcIndexMax; j++) {
            handlers_[j](region_);
        }
    }
    OH_Drawing_RectDestroy(rect_);
    OH_Drawing_PathDestroy(path_);
    OH_Drawing_RegionDestroy(other_);
}

void StabilityRegionAll::RegionCreateTest(OH_Drawing_Region* region)
{
    region = OH_Drawing_RegionCreate();
}

void StabilityRegionAll::RegionSetRectTest(OH_Drawing_Region* region)
{
    OH_Drawing_RegionSetRect(region, rect_);
}

void StabilityRegionAll::RegionSetPathTest(OH_Drawing_Region* region)
{
    OH_Drawing_RegionSetPath(region, path_, other_);
}

void StabilityRegionAll::RegionDestroyTest(OH_Drawing_Region* region)
{
    OH_Drawing_RegionDestroy(region);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_REGION_0300
@Description:Region相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Region相关操作接口随机循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRegionRandomFunc::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityRegionRandomFunc OnTestStability");
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        uint32_t funcIndexMax = OPERATION_FUNCTION_REGION_OP;
        uint32_t index = rand.nextRangeU(0, funcIndexMax);
        handlers_[index](region_);
    }
}

void StabilityRegionRandomFunc::RegionContainsTest(OH_Drawing_Region* region)
{
    OH_Drawing_RegionContains(region, rand_.nextULessThan(bitmapWidth_), rand_.nextULessThan(bitmapHeight_));
}

void StabilityRegionRandomFunc::RegionOpTest(OH_Drawing_Region* region)
{
    OH_Drawing_RegionOp(region, other_, DrawGetEnum(REGION_OP_MODE_DIFFERENCE, REGION_OP_MODE_REPLACE, index_));
}