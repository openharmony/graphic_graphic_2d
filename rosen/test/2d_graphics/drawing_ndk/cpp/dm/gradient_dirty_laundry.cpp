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
#include "gradient_dirty_laundry.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <time.h>
#include <array>
#include "common/log_common.h"
#include "test_common.h"

enum {
    WIDTH = 640,
    HEIGHT = 615
};

Gradients::Gradients() {
    // skia dm file gm/gradient_dirty_laundry.cpp
    bitmapWidth_ = WIDTH;
    bitmapHeight_ = HEIGHT;
    fileName_ = "gradient_dirty_laundry";
}

struct GradData {
    int fCount = 40;
    const uint32_t *fColors;
    const float *fPos;
};

constexpr int kNumColorChoices = 40;
Color gColors[kNumColorChoices] = {
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000,
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000, // 8 lines, 40 colors
};

constexpr GradData gGradData[] = {
    {40, gColors, nullptr}};
GradData data = gGradData[0];

// 定义三种着色器
OH_Drawing_ShaderEffect *MakeLinear(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
                                    const GradData &data, OH_Drawing_TileMode tileMode) {
    int fCount = 40;
    int ArraySize = 100;
    float positions[ArraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f;
    positions[1] = 0.0f;
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fCount; ++i) {
        positions[i] = static_cast<float>(i) / (fCount - 1.0f);
    }
    DRAWING_LOGI("GradientsGM MakeLinear");
    OH_Drawing_ShaderEffect *MakeLinear =
        OH_Drawing_ShaderEffectCreateLinearGradient(firstPoint, secondPoint, data.fColors, positions, fCount, tileMode);
    return MakeLinear;
}
OH_Drawing_ShaderEffect *MakeRadial(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
                                    const GradData &data, OH_Drawing_TileMode tileMode) {
    DRAWING_LOGI("GradientsGM MakeRadial");

    // 圆心和圆半径坐标
    OH_Drawing_Point2D pts[] = {{0, 0}, {IntToScalar(100), IntToScalar(100)}};
    OH_Drawing_Point2D Point = {ScalarAve(pts[0].x, pts[1].x), ScalarAve(pts[0].y, pts[1].y)};
    OH_Drawing_Point *centerPt = OH_Drawing_PointCreate(Point.x, Point.y);
    int fCount = 40;
    int ArraySize = 100;
    float positions[ArraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f;
    positions[1] = 0.0f;
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fCount; ++i) {
        positions[i] = static_cast<float>(i) / (fCount - 1.0f);
    }
    OH_Drawing_ShaderEffect *MakeRadial =
        OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, Point.x, data.fColors, positions, fCount, tileMode);
    return MakeRadial;
}
OH_Drawing_ShaderEffect *MakeSweep(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
                                   const GradData &data, OH_Drawing_TileMode tileMode) {
    DRAWING_LOGI("GradientsGM MakeSweep");
    // 圆心和圆半径坐标
    OH_Drawing_Point2D pts[] = {{0, 0}, {IntToScalar(100), IntToScalar(100)}};
    OH_Drawing_Point2D Point = {ScalarAve(pts[0].x, pts[1].x), ScalarAve(pts[0].y, pts[1].y)};
    OH_Drawing_Point *centerPt = OH_Drawing_PointCreate(Point.x, Point.y);
    int fCount = 40;
    int ArraySize = 100;
    float positions[ArraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f;
    positions[1] = 0.0f;
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fCount; ++i) {
        positions[i] = static_cast<float>(i) / (fCount - 1.0f);
    }
    OH_Drawing_ShaderEffect *MakeSweep =
        OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, data.fColors, positions, fCount, tileMode);
    return MakeSweep;
}

typedef OH_Drawing_ShaderEffect *(*GradMaker)(const OH_Drawing_Point *firstPoint, const OH_Drawing_Point *secondPoint,
                                              const GradData &, OH_Drawing_TileMode);
constexpr GradMaker gGradMakers[] = {
    MakeLinear,
    MakeRadial,
    MakeSweep,
};

void Gradients::OnTestFunction(OH_Drawing_Canvas *canvas) {
    DRAWING_LOGI("GradientsGM::OnTestFunction start");

    // 使用指定颜色设置清空画布底色
    OH_Drawing_CanvasClear(canvas, 0xFFDDDDDD);
    // 创建一个画刷brush对象设置抗锯齿属性
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    // 创建一个矩形对象
    OH_Drawing_Rect *rectAngLe = OH_Drawing_RectCreate(0, 0, IntToScalar(100), IntToScalar(100));
    // 平移画布
    OH_Drawing_CanvasTranslate(canvas, IntToScalar(20), IntToScalar(20));

    for (size_t i = 0; i < SK_ARRAY_COUNT(gGradData); i++) {
        OH_Drawing_CanvasSave(canvas);
        for (size_t j = 0; j < SK_ARRAY_COUNT(gGradMakers); j++) {
            OH_Drawing_BrushSetShaderEffect(brush,
                                            gGradMakers[j](firstPoint, secondPoint, gGradData[i], OH_Drawing_TileMode::CLAMP));
            OH_Drawing_CanvasAttachBrush(canvas, brush);
            OH_Drawing_CanvasDrawRect(canvas, rectAngLe);
            OH_Drawing_CanvasTranslate(canvas, 0, IntToScalar(120));
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, IntToScalar(120), 0);
    }
    DRAWING_LOGI("GradientsGM::OnTestFunction end");
    OH_Drawing_RectDestroy(rectAngLe);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
}
