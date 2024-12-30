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

#include "color_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_color_space.h>
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

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0100
@Description:ColorFilterCreateBlendMode-ColorFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ColorFilter相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityColorFilterCreateBlendMode::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(
            rand.nextU(), DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, i));
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0200
@Description:ColorFilterCreateCompose-ColorFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ColorFilter相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityColorFilterCreateCompose::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFilter* colorFilter1 = OH_Drawing_ColorFilterCreateBlendMode(
            rand.nextU(), DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, i));
        OH_Drawing_ColorFilter* colorFilter2 = OH_Drawing_ColorFilterCreateBlendMode(
            rand.nextU(), DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, i));
        OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateCompose(colorFilter1, colorFilter2);
        OH_Drawing_ColorFilterDestroy(colorFilter);
        OH_Drawing_ColorFilterDestroy(colorFilter1);
        OH_Drawing_ColorFilterDestroy(colorFilter2);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0300
@Description:ColorFilterCreateMatrix-ColorFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ColorFilter相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityColorFilterCreateMatrix::OnTestStability(OH_Drawing_Canvas* canvas)
{
    float colorMatrix[20] = { 1, 0, 0, 0, 0, // 1, 0, 0, 0, 0, 颜色矩阵参数
        0, 1, 0, 0, 0,                       // 0, 1, 0, 0, 0, 颜色矩阵参数
        0, 0, 0.5, 0.5, 0,                   // 0, 0, 0.5, 0.5, 0, 颜色矩阵参数
        0, 0, 0.5, 0.5, 0 };                 // 0, 0, 0.5, 0.5, 0 颜色矩阵参数
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFilter* colorFilter = colorFilter = OH_Drawing_ColorFilterCreateMatrix(colorMatrix);
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0400
@Description:ColorFilterCreateLinearToSrgbGamma-ColorFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ColorFilter相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityColorFilterCreateLinearToSrgbGamma::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORFILTER_0500
@Description:ColorFilterCreateSrgbGammaToLinear-ColorFilterDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ColorFilter相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityColorFilterCreateSrgbGammaToLinear::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateSrgbGammaToLinear();
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
}

// todo:用例中暂时不包含，后续使用
void StabilityColorFilterCreateLuma::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateLuma();
        OH_Drawing_ColorFilterDestroy(colorFilter);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORSPACE_0100
@Description:ColorSpaceCreateSrgb-ColorSpaceDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ColorSpace相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityColorSpaceCreateSrgb::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
        OH_Drawing_ColorSpaceDestroy(colorSpace);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_NDK_DRAWING_COLORSPACE_0200
@Description:ColorSpaceCreateSrgbLinear-ColorSpaceDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、ColorSpace相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityColorSpaceCreateSrgbLinear::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgbLinear();
        OH_Drawing_ColorSpaceDestroy(colorSpace);
    }
}
