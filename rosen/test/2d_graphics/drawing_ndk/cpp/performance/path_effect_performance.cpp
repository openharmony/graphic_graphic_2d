/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "path_effect_performance.h"

#include <native_drawing/drawing_path_effect.h>

#include "test_common.h"


void PerformancePathEffectCreateDiscretePathEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDiscretePathEffect(1.0 + i * 0.01, 1.0 + i * 0.01);
        OH_Drawing_PathEffectDestroy(pathEffect);
    }
}

void PerformancePathEffectCreateCornerPathEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateCornerPathEffect(1 + i * 0.01);
        OH_Drawing_PathEffectDestroy(pathEffect);
    }
}

void PerformancePathEffectCreateComposePathEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathEffect* pathEffect1 = OH_Drawing_CreateDiscretePathEffect(1.0 + i * 0.01, 1.0 + i * 0.01);
        OH_Drawing_PathEffect* pathEffect2 = OH_Drawing_CreateCornerPathEffect(1 + i * 0.01);
        OH_Drawing_PathEffect* pathEffect3 = OH_Drawing_CreateComposePathEffect(pathEffect1, pathEffect2);
        OH_Drawing_PathEffectDestroy(pathEffect1);
        OH_Drawing_PathEffectDestroy(pathEffect2);
        OH_Drawing_PathEffectDestroy(pathEffect3);
    }
}

void PerformancePathEffectCreatePathDashEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, 5, 5);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreatePathDashEffect(path,
            10 + i * 0.01f, 5 + i * 0.01f, OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_TRANSLATE);
        OH_Drawing_PathEffectDestroy(pathEffect);
    }
}

void PerformancePathEffectCreateSumPathEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathEffect* pathEffect1 = OH_Drawing_CreateDiscretePathEffect(1.0 + i * 0.01, 1.0 + i * 0.01);
        OH_Drawing_PathEffect* pathEffect2 = OH_Drawing_CreateCornerPathEffect(1 + i * 0.01);
        OH_Drawing_PathEffect* pathEffect3 = OH_Drawing_CreateSumPathEffect(pathEffect1, pathEffect2);
        OH_Drawing_PathEffectDestroy(pathEffect1);
        OH_Drawing_PathEffectDestroy(pathEffect2);
        OH_Drawing_PathEffectDestroy(pathEffect3);
    }
}