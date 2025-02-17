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

#include "pen_test.h"

#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_memory_stream.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_typeface.h>
#include <native_drawing/drawing_types.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0100
@Description:PenCreate-PenDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_PenCreate-OH_Drawing_PenDestroy循环执行1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPenCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityPenCreate OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
        OH_Drawing_PenDestroy(pen);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0200
@Description:PenCopy-PenDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_PenCopy-OH_Drawing_PenDestroy循环执行1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPenCopy::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityPenCopy OnTestStability");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Pen* pen1 = OH_Drawing_PenCopy(pen);
        OH_Drawing_PenDestroy(pen1);
    }
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0300
@Description:PenCreate-相关配置接口全调用-PenDestroy循环-PenAllFunc
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、PenCreate-相关配置接口全调用-PenDestroy循环-PenAllFunc
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPenAllFunc::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PenAllStability OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
        for (int j = 0; j < SETTING_FUNCTION_PEN_MAX; j++) {
            handlers_[j](pen);
        }
        OH_Drawing_PenDestroy(pen);
    }
}

void StabilityPenAllFunc::PenSetAntiAliasTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetAntiAlias(pen, (rand_.nextU() % 2 == 0)); // 2 for test,rand true or false
}

void StabilityPenAllFunc::PenSetColorTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetColor(pen, rand_.nextULessThan(0xffffffff));
}

void StabilityPenAllFunc::PenSetAlphaTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetAlpha(pen, rand_.nextULessThan(0xff));
}

void StabilityPenAllFunc::PenSetWidthTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetWidth(pen, rand_.nextRangeF(0, bitmapWidth_));
}

void StabilityPenAllFunc::PenSetMiterLimitTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetMiterLimit(pen, rand_.nextULessThan(10)); // 10 for test
}

void StabilityPenAllFunc::PenSetCapTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetCap(pen, DrawGetEnum(LINE_FLAT_CAP, LINE_ROUND_CAP, rand_.nextU()));
}

void StabilityPenAllFunc::PenSetJoinTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetJoin(pen, DrawGetEnum(LINE_MITER_JOIN, LINE_BEVEL_JOIN, rand_.nextU()));
}

void StabilityPenAllFunc::PenSetShaderEffectTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetShaderEffect(pen, colorShaderEffect_);
    OH_Drawing_PenSetShaderEffect(pen, linearGradientEffect_);
    OH_Drawing_PenSetShaderEffect(pen, linearGradientEffectLocal_);
    OH_Drawing_PenSetShaderEffect(pen, radialGradientEffect_);
    OH_Drawing_PenSetShaderEffect(pen, radialGradientEffectLocal_);
    OH_Drawing_PenSetShaderEffect(pen, sweepGradientEffect_);
    OH_Drawing_PenSetShaderEffect(pen, imageShaderEffect_);
    OH_Drawing_PenSetShaderEffect(pen, twoPointConicalGradientEffect_);
}

void StabilityPenAllFunc::PenSetShadowLayerTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetShadowLayer(pen, shadowLayer_);
}

void StabilityPenAllFunc::PenSetPathEffectTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetPathEffect(pen, pathEffect_);
}

void StabilityPenAllFunc::PenSetFilterTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetFilter(pen, filter_);
}

void StabilityPenAllFunc::PenSetBlendModeTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenSetBlendMode(pen, DrawGetEnum(BLEND_MODE_CLEAR, BLEND_MODE_LUMINOSITY, rand_.nextU()));
}

void StabilityPenAllFunc::PenResetTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenReset(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PEN_0400
@Description:Pen相关操作接口随机循环调用-PenRandomFunc
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Pen相关操作接口随机循环调用-PenRandomFunc
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPenRandomFunc::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("PenRandomFunc OnTestStability");
    TestRend rand;
    filter_ = OH_Drawing_FilterCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetColor(pen, rand.nextULessThan(0xffffffff));
    OH_Drawing_PenSetAlpha(pen, rand.nextULessThan(0xff));
    OH_Drawing_PenSetWidth(pen, rand.nextRangeF(0, bitmapWidth_));
    OH_Drawing_PenSetMiterLimit(pen, rand.nextULessThan(10)); // 10 for test
    OH_Drawing_PenSetCap(pen, LINE_FLAT_CAP);
    OH_Drawing_PenSetJoin(pen, LINE_MITER_JOIN);
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(rand.nextULessThan(0xffffffff));
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);
    for (int i = 0; i < testCount_; i++) {
        uint32_t funcIndexMax = OPERATION_FUNCTION_PEN_GET_FILTER;
        uint32_t index = rand.nextRangeU(0, funcIndexMax);
        handlers_[index](pen);
    }
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PenDestroy(pen);
}

void StabilityPenRandomFunc::PenIsAntiAliasTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenIsAntiAlias(pen);
}

void StabilityPenRandomFunc::PenGetColorTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenGetColor(pen);
}

void StabilityPenRandomFunc::PenGetAlphaTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenGetAlpha(pen);
}

void StabilityPenRandomFunc::PenGetWidthTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenGetWidth(pen);
}

void StabilityPenRandomFunc::PenGetMiterLimitTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenGetMiterLimit(pen);
}

void StabilityPenRandomFunc::PenGetCapTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenGetCap(pen);
}

void StabilityPenRandomFunc::PenGetJoinTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenGetJoin(pen);
}

void StabilityPenRandomFunc::PenGetFilterTest(OH_Drawing_Pen* pen)
{
    OH_Drawing_PenGetFilter(pen, filter_);
}