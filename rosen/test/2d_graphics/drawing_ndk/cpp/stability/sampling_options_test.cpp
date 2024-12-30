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

#include "sampling_options_test.h"

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
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SAMPLING_OPTIONS_0100
@Description:SamplingOptionsCreate-SamplingOptionsDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_SamplingOptionsCreate-OH_Drawing_SamplingOptionsDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilitySamplingOptionsCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("SamplingOptionsCreate OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_SamplingOptions* samplingOptions =
            OH_Drawing_SamplingOptionsCreate(DrawGetEnum(FILTER_MODE_NEAREST, FILTER_MODE_LINEAR, i),
                DrawGetEnum(MIPMAP_MODE_NONE, MIPMAP_MODE_LINEAR, i));
        OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    }
}