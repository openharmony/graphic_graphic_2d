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

#include "round_rect_test.h"

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

#include "test_base.h"
#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_ROUND_RECT_0100
@Description:RoundRectCreate-RoundRectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_RoundRectCreate-OH_Drawing_RoundRectDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRoundRectCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityRoundRectCreate OnTestStability");
    TestRend rand;
    OH_Drawing_Rect* rect =
        OH_Drawing_RectCreate(0, 0, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_RoundRect* roundRect =
            OH_Drawing_RoundRectCreate(rect, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_RoundRectDestroy(roundRect);
    }
    OH_Drawing_RectDestroy(rect);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_ROUND_RECT_0200
@Description:RoundRectCreate-相关配置接口全调用-RoundRectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、RoundRectCreate-相关配置接口全调用-RoundRectDestroy循环
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRoundRectAll::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("RoundRectAll OnTestStability");
    OH_Drawing_Rect* rect =
    OH_Drawing_RectCreate(0, 0, rand_.nextULessThan(bitmapWidth_), rand_.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect,
            rand_.nextULessThan(bitmapWidth_), rand_.nextULessThan(bitmapHeight_));
        for (int j = 0; j < OPERATION_FUNCTION_ROUND_RECT_MAX; j++) {
            handlers_[j](roundRect);
        }
        OH_Drawing_RoundRectDestroy(roundRect);
    }
    OH_Drawing_RectDestroy(rect);
}

void StabilityRoundRectAll::RoundRectSetCornerTest(OH_Drawing_RoundRect* roundRect)
{
    OH_Drawing_Point2D positions = { rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_) };
    OH_Drawing_RoundRectSetCorner(
        roundRect, DrawGetEnum(CORNER_POS_TOP_LEFT, CORNER_POS_BOTTOM_LEFT, rand_.nextU()), positions);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_ROUND_RECT_0300
@Description:RoundRect相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、RoundRect相关操作接口随机循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityRoundRectRandomFunc::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("RoundRectRandomFunc OnTestStability");
    OH_Drawing_Rect* rect =
        OH_Drawing_RectCreate(0, 0, rand_.nextULessThan(bitmapWidth_), rand_.nextULessThan(bitmapHeight_));
    OH_Drawing_RoundRect* roundRect =
        OH_Drawing_RoundRectCreate(rect, rand_.nextULessThan(bitmapWidth_), rand_.nextULessThan(bitmapHeight_));
    OH_Drawing_Point2D positions = { rand_.nextRangeF(0, bitmapWidth_), rand_.nextRangeF(0, bitmapHeight_) };
    OH_Drawing_RoundRectSetCorner(
        roundRect, DrawGetEnum(CORNER_POS_TOP_LEFT, CORNER_POS_BOTTOM_LEFT, rand_.nextU()), positions);
    for (int i = 0; i < testCount_; i++) {
        uint32_t index = rand_.nextRangeU(0, OPERATION_FUNCTION_ROUND_RECT_MAX - 1);
        handlers_[index](roundRect);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

void StabilityRoundRectRandomFunc::RoundRectGetCornerTest(OH_Drawing_RoundRect* roundRect)
{
    OH_Drawing_RoundRectGetCorner(roundRect, DrawGetEnum(CORNER_POS_TOP_LEFT, CORNER_POS_BOTTOM_LEFT, rand_.nextU()));
}