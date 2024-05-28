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
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>

#include "shader_effect_test.h"
#include "test_common.h"

#include "common/log_common.h"

const int NUM_2 = 2;
const int NUM_20 = 20;
const int NUM_50 = 50;
const int NUM_300 = 300;

void ShaderEffectCreateLinearGradient::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(NUM_20, NUM_20);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(NUM_300, NUM_300);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(NUM_20, NUM_20, NUM_300, NUM_300); // 创建一个矩形
    uint32_t colors[] = { 0xFFFFFFFF };
    float pos[] = { 0.0f, 1.0f };
    OH_Drawing_ShaderEffect* ShaderEffect;
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < testCount_; i++) {
        ShaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, pos, NUM_2, CLAMP);
        OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    }
    ShaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, pos, NUM_2, CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, ShaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);

    OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
}

void ShaderEffectCreateRadialGradient::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(NUM_20, NUM_20);
    float pos[] = { 0.0f, 1.0f };
    uint32_t colors[] = { 0xFFFFFFFF }; // 白色到黑色的渐变
    uint32_t size = NUM_20;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(NUM_50, NUM_50, NUM_300, NUM_300); // 创建一个矩形
    OH_Drawing_ShaderEffect* ShaderEffect;
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    for (int i = 0; i < testCount_; i++) {
        ShaderEffect = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, NUM_20, colors, pos, size, CLAMP);
        OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    }
    ShaderEffect = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, NUM_20, colors, pos, size, CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, ShaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);

    OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PointDestroy(centerPt);
}

void ShaderEffectCreateImageShader::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_SamplingOptions* option = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NONE);

    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateTranslation(rand.nextF(), rand.nextF());
    OH_Drawing_ShaderEffect* ShaderEffect;
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(NUM_20, NUM_50, NUM_300, NUM_300);
    for (int i = 0; i < testCount_; i++) {
        ShaderEffect = OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, option, matrix);
        OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    }

    ShaderEffect = OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, option, matrix);
    OH_Drawing_BrushSetShaderEffect(brush, ShaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);

    OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(option);
}

void ShaderEffectCreateSweepGradient::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(NUM_20, NUM_20);
    int fCount = NUM_50;
    float positions[NUM_50];

    positions[0] = 0.0f;
    positions[1] = 0.1f; // 0.1 位置值
    OH_Drawing_ShaderEffect* ShaderEffect;
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    uint32_t colors[] = { 0xFFFFFFFF };
    for (int i = 2; i < fCount; ++i) {
        positions[i] = static_cast<float>(i) / (fCount - 1.0f); // 1.0 边界值 -1
    }
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(NUM_20, NUM_50, NUM_300, NUM_300);
    for (int i = 0; i < testCount_; i++) {
        ShaderEffect = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, colors, positions, fCount, CLAMP);
        OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    }

    ShaderEffect = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, colors, positions, fCount, CLAMP);
    OH_Drawing_BrushSetShaderEffect(brush, ShaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawOval(canvas, rect);

    OH_Drawing_ShaderEffectDestroy(ShaderEffect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PointDestroy(centerPt);
    OH_Drawing_BrushDestroy(brush);
}