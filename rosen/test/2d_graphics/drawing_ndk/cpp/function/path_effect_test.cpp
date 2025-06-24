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

#include "path_effect_test.h"
#include "common/log_common.h"

#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>

void FunctionPathEffectCreateCornerPathEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);

    OH_Drawing_PathEffect* cornerPathEffect1 = OH_Drawing_CreateCornerPathEffect(0);
    DRAWING_LOGI("FunctionPathEffectCreateCornerPathEffect test: is cornerPathEffect1 nullptr = %{public}d",
        cornerPathEffect1 == nullptr);
    OH_Drawing_PathEffect* cornerPathEffect2 = OH_Drawing_CreateCornerPathEffect(-10);
    DRAWING_LOGI("FunctionPathEffectCreateCornerPathEffect test: is cornerPathEffect2 nullptr = %{public}d",
        cornerPathEffect2 == nullptr);
    OH_Drawing_PathEffect* cornerPathEffect3 = OH_Drawing_CreateCornerPathEffect(30);
    DRAWING_LOGI("FunctionPathEffectCreateCornerPathEffect test: is cornerPathEffect3 nullptr = %{public}d",
        cornerPathEffect3 == nullptr);
    OH_Drawing_PathEffect* cornerPathEffect4 = OH_Drawing_CreateCornerPathEffect(200);
    DRAWING_LOGI("FunctionPathEffectCreateCornerPathEffect test: is cornerPathEffect4 nullptr = %{public}d",
        cornerPathEffect4 == nullptr);

    OH_Drawing_PenSetPathEffect(pen, cornerPathEffect3);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect3);

    OH_Drawing_PenSetPathEffect(pen, cornerPathEffect4);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(400, 100, 600, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect4);
    
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect3);
    OH_Drawing_RectDestroy(rect4);
    OH_Drawing_PathEffectDestroy(cornerPathEffect1);
    OH_Drawing_PathEffectDestroy(cornerPathEffect2);
    OH_Drawing_PathEffectDestroy(cornerPathEffect3);
    OH_Drawing_PathEffectDestroy(cornerPathEffect4);
    OH_Drawing_PenDestroy(pen);
}

void FunctionPathEffectCreateDiscretePathEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);

    OH_Drawing_PathEffect* discretePathEffect1 = OH_Drawing_CreateDiscretePathEffect(0, 10);
    DRAWING_LOGI("FunctionPathEffectCreateDiscretePathEffect test: is discretePathEffect1 nullptr = %{public}d",
        discretePathEffect1 == nullptr);
    OH_Drawing_PathEffect* discretePathEffect2 = OH_Drawing_CreateDiscretePathEffect(-100, -10);
    DRAWING_LOGI("FunctionPathEffectCreateDiscretePathEffect test: is discretePathEffect2 nullptr = %{public}d",
        discretePathEffect2 == nullptr);
    OH_Drawing_PathEffect* discretePathEffect3 = OH_Drawing_CreateDiscretePathEffect(20, 10);
    DRAWING_LOGI("FunctionPathEffectCreateDiscretePathEffect test: is discretePathEffect3 nullptr = %{public}d",
        discretePathEffect3 == nullptr);
    OH_Drawing_PathEffect* discretePathEffect4 = OH_Drawing_CreateDiscretePathEffect(20, -50);
    DRAWING_LOGI("FunctionPathEffectCreateDiscretePathEffect test: is discretePathEffect4 nullptr = %{public}d",
        discretePathEffect4 == nullptr);

    OH_Drawing_PenSetPathEffect(pen, discretePathEffect1);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect1);

    OH_Drawing_PenSetPathEffect(pen, discretePathEffect2);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(400, 100, 600, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect2);

    OH_Drawing_PenSetPathEffect(pen, discretePathEffect3);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(100, 400, 300, 600);
    OH_Drawing_CanvasDrawRect(canvas, rect3);

    OH_Drawing_PenSetPathEffect(pen, discretePathEffect4);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(400, 400, 600, 600);
    OH_Drawing_CanvasDrawRect(canvas, rect4);
    
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_RectDestroy(rect3);
    OH_Drawing_RectDestroy(rect4);
    OH_Drawing_PathEffectDestroy(discretePathEffect1);
    OH_Drawing_PathEffectDestroy(discretePathEffect2);
    OH_Drawing_PathEffectDestroy(discretePathEffect3);
    OH_Drawing_PathEffectDestroy(discretePathEffect4);
    OH_Drawing_PenDestroy(pen);
}

void FunctionPathEffectCreateComposePathEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
    
    OH_Drawing_PathEffect* cornerPathEffect0 = OH_Drawing_CreateCornerPathEffect(50);
    OH_Drawing_PathEffect* composePathEffect1 = OH_Drawing_CreateComposePathEffect(nullptr, cornerPathEffect0);
    DRAWING_LOGI("FunctionPathEffectCreateComposePathEffect test: is composePathEffect1 nullptr = %{public}d",
        composePathEffect1 == nullptr);
    OH_Drawing_PathEffect* composePathEffect2 = OH_Drawing_CreateComposePathEffect(cornerPathEffect0, nullptr);
    DRAWING_LOGI("FunctionPathEffectCreateComposePathEffect test: is composePathEffect2 nullptr = %{public}d",
        composePathEffect2 == nullptr);
    
    OH_Drawing_PathEffect* discretePathEffect = OH_Drawing_CreateDiscretePathEffect(20, 10);
    OH_Drawing_Path* discretePath = OH_Drawing_PathCreate();
    OH_Drawing_Rect* pathRect = OH_Drawing_RectCreate(0, 0, 10, 10);
    OH_Drawing_PathAddRectWithInitialCorner(discretePath, pathRect, OH_Drawing_PathDirection::PATH_DIRECTION_CW, 0);
    OH_Drawing_PathEffect* pathDashEffect = OH_Drawing_CreatePathDashEffect(discretePath, 20, 0,
        OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_MORPH);
    OH_Drawing_PathEffect* composePathEffect3 = OH_Drawing_CreateComposePathEffect(pathDashEffect, discretePathEffect);
    DRAWING_LOGI("FunctionPathEffectCreateComposePathEffect test: is composePathEffect3 nullptr = %{public}d",
        composePathEffect3 == nullptr);

    OH_Drawing_PathEffect* cornerPathEffect1 = OH_Drawing_CreateCornerPathEffect(30);
    OH_Drawing_PathEffect* cornerPathEffect2 = OH_Drawing_CreateCornerPathEffect(100);
    OH_Drawing_PathEffect* composePathEffect4 =
        OH_Drawing_CreateComposePathEffect(cornerPathEffect2, cornerPathEffect1);
    DRAWING_LOGI("FunctionPathEffectCreateComposePathEffect test: is composePathEffect4 nullptr = %{public}d",
        composePathEffect4 == nullptr);
    OH_Drawing_PathEffect* composePathEffect5 =
        OH_Drawing_CreateComposePathEffect(cornerPathEffect1, cornerPathEffect2);
    DRAWING_LOGI("FunctionPathEffectCreateComposePathEffect test: is composePathEffect5 nullptr = %{public}d",
        composePathEffect4 == nullptr);

    OH_Drawing_PenSetPathEffect(pen, composePathEffect3);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect3);

    OH_Drawing_PenSetPathEffect(pen, composePathEffect4);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(400, 100, 600, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect4);

    OH_Drawing_PenSetPathEffect(pen, composePathEffect5);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect5 = OH_Drawing_RectCreate(100, 400, 300, 600);
    OH_Drawing_CanvasDrawRect(canvas, rect5);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PathDestroy(discretePath);
    OH_Drawing_RectDestroy(pathRect);
    OH_Drawing_RectDestroy(rect3);
    OH_Drawing_RectDestroy(rect4);
    OH_Drawing_RectDestroy(rect5);
    OH_Drawing_PathEffectDestroy(cornerPathEffect0);
    OH_Drawing_PathEffectDestroy(composePathEffect1);
    OH_Drawing_PathEffectDestroy(composePathEffect2);
    OH_Drawing_PathEffectDestroy(discretePathEffect);
    OH_Drawing_PathEffectDestroy(pathDashEffect);
    OH_Drawing_PathEffectDestroy(composePathEffect3);
    OH_Drawing_PathEffectDestroy(cornerPathEffect1);
    OH_Drawing_PathEffectDestroy(cornerPathEffect2);
    OH_Drawing_PathEffectDestroy(composePathEffect4);
    OH_Drawing_PathEffectDestroy(composePathEffect5);
    OH_Drawing_PenDestroy(pen);
}

void FunctionPathEffectCreatePathDashEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 1);
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Path* discretePath = OH_Drawing_PathCreate();
    OH_Drawing_Rect* pathRect = OH_Drawing_RectCreate(0, 0, 10, 10);
    OH_Drawing_PathAddRectWithInitialCorner(discretePath, pathRect, OH_Drawing_PathDirection::PATH_DIRECTION_CW, 0);

    OH_Drawing_PathEffect* pathDashEffect1 = OH_Drawing_CreatePathDashEffect(nullptr, 20, 0,
        OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_MORPH);
    DRAWING_LOGI("FunctionPathEffectCreatePathDashEffect test: is pathDashEffect1 nullptr = %{public}d",
        pathDashEffect1 == nullptr);
    OH_Drawing_PathEffect* pathDashEffect2 = OH_Drawing_CreatePathDashEffect(discretePath, 0, 0,
        OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_MORPH);
    DRAWING_LOGI("FunctionPathEffectCreatePathDashEffect test: is pathDashEffect2 nullptr = %{public}d",
        pathDashEffect2 == nullptr);
    OH_Drawing_PathEffect* pathDashEffect3 = OH_Drawing_CreatePathDashEffect(discretePath, -10, 0,
        OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_MORPH);
    DRAWING_LOGI("FunctionPathEffectCreatePathDashEffect test: is pathDashEffect3 nullptr = %{public}d",
        pathDashEffect3 == nullptr);
    
    OH_Drawing_PathEffect* pathDashEffect4 = OH_Drawing_CreatePathDashEffect(discretePath, 40, 10,
        OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_TRANSLATE);
    DRAWING_LOGI("FunctionPathEffectCreatePathDashEffect test: is pathDashEffect4 nullptr = %{public}d",
        pathDashEffect4 == nullptr);
    
    OH_Drawing_Path* discretePath5 = OH_Drawing_PathCreate();
    OH_Drawing_Rect* pathRect5 = OH_Drawing_RectCreate(10, 10, 20, 20);
    OH_Drawing_PathAddRectWithInitialCorner(discretePath5, pathRect5, OH_Drawing_PathDirection::PATH_DIRECTION_CW, 10);
    OH_Drawing_PathEffect* pathDashEffect5 = OH_Drawing_CreatePathDashEffect(discretePath5, 20, 0,
        OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_ROTATE);
    DRAWING_LOGI("FunctionPathEffectCreatePathDashEffect test: is pathDashEffect5 nullptr = %{public}d",
        pathDashEffect5 == nullptr);

    OH_Drawing_PenSetPathEffect(pen, pathDashEffect4);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect4);

    OH_Drawing_PenSetPathEffect(pen, pathDashEffect5);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect5 = OH_Drawing_RectCreate(400, 100, 600, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect5);
    
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PathDestroy(discretePath);
    OH_Drawing_PathDestroy(discretePath5);
    OH_Drawing_RectDestroy(pathRect);
    OH_Drawing_RectDestroy(pathRect5);
    OH_Drawing_RectDestroy(rect4);
    OH_Drawing_RectDestroy(rect5);
    OH_Drawing_PathEffectDestroy(pathDashEffect1);
    OH_Drawing_PathEffectDestroy(pathDashEffect2);
    OH_Drawing_PathEffectDestroy(pathDashEffect3);
    OH_Drawing_PathEffectDestroy(pathDashEffect4);
    OH_Drawing_PathEffectDestroy(pathDashEffect5);
    OH_Drawing_PenDestroy(pen);
}

void FunctionPathEffectCreateSumPathEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 10);
    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
    
    OH_Drawing_PathEffect* cornerPathEffect0 = OH_Drawing_CreateCornerPathEffect(50);
    OH_Drawing_PathEffect* sumPathEffect1 = OH_Drawing_CreateSumPathEffect(nullptr, cornerPathEffect0);
    DRAWING_LOGI("FunctionPathEffectCreateSumPathEffect test: is sumPathEffect1 nullptr = %{public}d",
        sumPathEffect1 == nullptr);
    OH_Drawing_PathEffect* sumPathEffect2 = OH_Drawing_CreateSumPathEffect(cornerPathEffect0, nullptr);
    DRAWING_LOGI("FunctionPathEffectCreateSumPathEffect test: is sumPathEffect2 nullptr = %{public}d",
        sumPathEffect2 == nullptr);
    
    OH_Drawing_PathEffect* discretePathEffect = OH_Drawing_CreateDiscretePathEffect(20, 10);
    OH_Drawing_PathEffect* cornerPathEffect1 = OH_Drawing_CreateCornerPathEffect(30);
    OH_Drawing_PathEffect* cornerPathEffect2 = OH_Drawing_CreateCornerPathEffect(100);
    
    OH_Drawing_PathEffect* sumPathEffect3 = OH_Drawing_CreateSumPathEffect(cornerPathEffect2, discretePathEffect);
    DRAWING_LOGI("FunctionPathEffectCreateSumPathEffect test: is sumPathEffect3 nullptr = %{public}d",
        sumPathEffect3 == nullptr);

    OH_Drawing_PathEffect* sumPathEffect4 = OH_Drawing_CreateSumPathEffect(cornerPathEffect2, cornerPathEffect1);
    DRAWING_LOGI("FunctionPathEffectCreateSumPathEffect test: is sumPathEffect4 nullptr = %{public}d",
        sumPathEffect4 == nullptr);
    OH_Drawing_PathEffect* sumPathEffect5 = OH_Drawing_CreateSumPathEffect(cornerPathEffect1, cornerPathEffect2);
    DRAWING_LOGI("FunctionPathEffectCreateSumPathEffect test: is sumPathEffect5 nullptr = %{public}d",
        sumPathEffect4 == nullptr);

    OH_Drawing_PenSetPathEffect(pen, sumPathEffect3);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect3);

    OH_Drawing_PenSetPathEffect(pen, sumPathEffect4);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(400, 100, 600, 300);
    OH_Drawing_CanvasDrawRect(canvas, rect4);

    OH_Drawing_PenSetPathEffect(pen, sumPathEffect5);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect5 = OH_Drawing_RectCreate(100, 400, 300, 600);
    OH_Drawing_CanvasDrawRect(canvas, rect5);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect3);
    OH_Drawing_RectDestroy(rect4);
    OH_Drawing_RectDestroy(rect5);
    OH_Drawing_PathEffectDestroy(cornerPathEffect0);
    OH_Drawing_PathEffectDestroy(sumPathEffect1);
    OH_Drawing_PathEffectDestroy(sumPathEffect2);
    OH_Drawing_PathEffectDestroy(sumPathEffect3);
    OH_Drawing_PathEffectDestroy(cornerPathEffect1);
    OH_Drawing_PathEffectDestroy(cornerPathEffect2);
    OH_Drawing_PathEffectDestroy(sumPathEffect4);
    OH_Drawing_PathEffectDestroy(sumPathEffect5);
    OH_Drawing_PenDestroy(pen);
}