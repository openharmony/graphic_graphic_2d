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

#include "shader_effect_test.h"

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
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0100
@Description:ShaderEffectCreateColorShader-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2. 创建和销毁ShaderEffect操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateColorShader::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateColorShader OnTestStability");
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ShaderEffect* shaderEffect =
            OH_Drawing_ShaderEffectCreateColorShader(rand.nextULessThan(0xffffffff));
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0200
@Description:ShaderEffectCreateLinearGradient-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Point和ShaderEffect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateLinearGradient::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateColorShader OnTestStability");
    TestRend rand;
    OH_Drawing_Point* startPt =
        OH_Drawing_PointCreate(rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_));
    OH_Drawing_Point* endPt =
        OH_Drawing_PointCreate(rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        const uint32_t Color[] = { rand.nextULessThan(0xffffffff), rand.nextULessThan(0xffffffff) };
        const float pos[] = { 0.0f, 1.0f };
        OH_Drawing_ShaderEffect* shaderEffect =
            OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, Color, pos, 2, DrawGetEnum(CLAMP, DECAL, i));
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0300
@Description:ShaderEffectCreateLinearGradientWithLocalMatrix-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Point和ShaderEffect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateLinearGradientWithLocalMatrix::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateColorShader OnTestStability");
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D startPt = { rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_) };
        OH_Drawing_Point2D endPt = { rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_) };
        const uint32_t Color[] = { rand.nextULessThan(0xffffffff), rand.nextULessThan(0xffffffff) };
        const float pos[] = { 0.0f, 1.0f };
        OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
        OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
            &startPt, &endPt, Color, pos, 2, DrawGetEnum(CLAMP, DECAL, i), matrix);
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
        OH_Drawing_MatrixDestroy(matrix);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0400
@Description:ShaderEffectCreateRadialGradient-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Point和ShaderEffect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateRadialGradient::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateRadialGradient OnTestStability");
    TestRend rand;
    OH_Drawing_Point* centerPt =
        OH_Drawing_PointCreate(rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        float radius = rand.nextRangeF(0, bitmapWidth_);
        const uint32_t Color[] = { rand.nextULessThan(0xffffffff), rand.nextULessThan(0xffffffff) };
        const float pos[] = { 0.0f, 1.0f };
        OH_Drawing_ShaderEffect* shaderEffect =
            OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, radius, Color, pos, 2, DrawGetEnum(CLAMP, DECAL, i));
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
    OH_Drawing_PointDestroy(centerPt);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0500
@Description:ShaderEffectCreateRadialGradientWithLocalMatrix-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Point和ShaderEffect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateRadialGradientWithLocalMatrix::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateRadialGradientWithLocalMatrix OnTestStability");
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D centerPt = { rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_) };
        float radius = rand.nextRangeF(0, bitmapWidth_);
        const uint32_t Color[] = { rand.nextULessThan(0xffffffff), rand.nextULessThan(0xffffffff) };
        const float pos[] = { 0.0f, 1.0f };
        OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
        OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
            &centerPt, radius, Color, pos, 2, DrawGetEnum(CLAMP, DECAL, i), matrix);
        OH_Drawing_MatrixDestroy(matrix);
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0600
@Description:ShaderEffectCreateSweepGradient-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Point和ShaderEffect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateSweepGradient::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateSweepGradient OnTestStability");
    TestRend rand;
    OH_Drawing_Point* centerPt =
        OH_Drawing_PointCreate(rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_));

    for (int i = 0; i < testCount_; i++) {
        const uint32_t Color[] = { rand.nextULessThan(0xffffffff), rand.nextULessThan(0xffffffff) };
        const float pos[] = { 0.0f, 1.0f };
        OH_Drawing_ShaderEffect* shaderEffect =
            OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, Color, pos, 2, DrawGetEnum(CLAMP, DECAL, i));
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
    OH_Drawing_PointDestroy(centerPt);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0700
@Description:ShaderEffectCreateImageShader-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Point和ShaderEffect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateImageShader::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateImageShader OnTestStability");
    TestRend rand;
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_Matrix* matrix =
        OH_Drawing_MatrixCreateTranslation(rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_));
    OH_Drawing_SamplingOptions* option =
        OH_Drawing_SamplingOptionsCreate(DrawGetEnum(FILTER_MODE_NEAREST, FILTER_MODE_LINEAR, rand.nextU()),
            DrawGetEnum(MIPMAP_MODE_NONE, MIPMAP_MODE_LINEAR, rand.nextU()));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateImageShader(
            image, DrawGetEnum(CLAMP, DECAL, i), DrawGetEnum(CLAMP, DECAL, i), option, matrix);

        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_SamplingOptionsDestroy(option);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_SHADER_EFFECT_0800
@Description:ShaderEffectCreateTwoPointConicalGradient-ShaderEffectDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Point和ShaderEffect相关操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落到执行前；
 */
void StabilityShaderEffectCreateTwoPointConicalGradient::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("ShaderEffectCreateTwoPointConicalGradient OnTestStability");
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        DRAWING_LOGI("ShaderEffectCreateTwoPointConicalGradient=%{public}d ", i);
        OH_Drawing_Point2D startPt = { rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_) };
        OH_Drawing_Point2D endPt = { rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_) };
        float startRadius = rand.nextRangeF(0, bitmapWidth_);
        float endRadius = rand.nextRangeF(0, bitmapWidth_);
        const uint32_t Color[] = { rand.nextULessThan(0xffffffff), rand.nextULessThan(0xffffffff) };
        const float pos[] = { 0.0f, 1.0f };
        OH_Drawing_Matrix* matrix =
            OH_Drawing_MatrixCreateTranslation(rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_));
        OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(
            &startPt, startRadius, &endPt, endRadius, Color, pos, 2, DrawGetEnum(CLAMP, DECAL, i), matrix);
        OH_Drawing_MatrixDestroy(matrix);
        OH_Drawing_ShaderEffectDestroy(shaderEffect);
    }
}