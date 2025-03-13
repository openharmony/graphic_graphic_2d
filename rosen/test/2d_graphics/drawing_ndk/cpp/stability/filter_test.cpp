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

#include "filter_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_color_space.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_image_filter.h>
#include <native_drawing/drawing_mask_filter.h>
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
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_FILTEER_0100
@Description:FilterCreate-FilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_FilterCreate-OH_Drawing_FilterDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityFilterCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        OH_Drawing_FilterDestroy(filter);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_FILTEER_0200
@Description:Filter相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Filter相关操作接口随机循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityFilterRandInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        handlers_[rand.nextRangeU(0, OPERATION_FUNCTION_FILTER_MAX - 1)](filter);
    }
    OH_Drawing_FilterDestroy(filter);
}

void StabilityFilterRandInvoke::FilterGetColorFilterTest(OH_Drawing_Filter* filter)
{
    OH_Drawing_FilterGetColorFilter(filter, colorFilter_);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_FILTEER_0300
@Description:FilterCreate-相关配置接口全调用-FilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、FilterCreate-相关配置接口全调用-FilterDestroy循环
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityFilterInvoke::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    for (int i = 0; i < testCount_; i++) {
        handlers_[rand.nextRangeU(0, SETTING_FUNCTION_FILTER_MAX - 1)](filter);
    }
    OH_Drawing_FilterDestroy(filter);
}

void StabilityFilterInvoke::FilterSetImageFilterTest(OH_Drawing_Filter* filter)
{
    OH_Drawing_FilterSetImageFilter(filter, imageFilter_);
}

void StabilityFilterInvoke::FilterSetMaskFilterTest(OH_Drawing_Filter* filter)
{
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter_);
}

void StabilityFilterInvoke::FilterSetColorFilterTest(OH_Drawing_Filter* filter)
{
    OH_Drawing_FilterSetColorFilter(filter, colorFilter_);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_FILTER_0100
@Description:ImageFilterCreateBlur-ImageFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ImageFilterCreateBlur-ImageFilterDestroy循环
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityImageFilterCreateBlur::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        // 10 for test, 20 for test
        OH_Drawing_ImageFilter* imageFilter =
            OH_Drawing_ImageFilterCreateBlur(10, 20, DrawGetEnum(CLAMP, DECAL, i), nullptr);
        // 10 for test, 20 for test
        OH_Drawing_ImageFilter* imageFilter1 =
            OH_Drawing_ImageFilterCreateBlur(10, 20, DrawGetEnum(CLAMP, DECAL, i), imageFilter);
        OH_Drawing_ImageFilterDestroy(imageFilter1);
        OH_Drawing_ImageFilterDestroy(imageFilter);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_IMAGE_FILTER_0200
@Description:ImageFilterCreateFromColorFilter-ImageFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ImageFilterCreateFromColorFilter-ImageFilterDestroy循环
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityImageFilterCreateFromColorFilter::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(
        rand.nextU(), DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, rand.nextU()));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ImageFilter* imageFilter = OH_Drawing_ImageFilterCreateFromColorFilter(colorFilter, nullptr);
        OH_Drawing_ImageFilter* imageFilter1 = OH_Drawing_ImageFilterCreateFromColorFilter(colorFilter, imageFilter);
        OH_Drawing_ImageFilterDestroy(imageFilter1);
        OH_Drawing_ImageFilterDestroy(imageFilter);
    }
    OH_Drawing_ColorFilterDestroy(colorFilter);
}