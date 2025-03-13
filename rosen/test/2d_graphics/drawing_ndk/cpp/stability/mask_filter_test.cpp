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

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_rect.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MASK_FILTER_0100
@Description:MaskFilterCreateBlur-MaskFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_MaskFilterCreateBlur-OH_Drawing_MaskFilterDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityMaskFilterCreateBlur::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityMaskFilterCreateBlur OnTestStability");
    TestRend rand = TestRend();
    for (int i = 0; i < testCount_; i++) {
        float min = 1;
        // 50 for test
        float max = 50;
        float sigma = rand.nextRangeF(min, max);
        OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(DrawGetEnum(NORMAL, INNER, i), sigma, true);
        OH_Drawing_MaskFilterDestroy(maskFilter);
    }
}