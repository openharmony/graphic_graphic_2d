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

#include <array>
#include <ctime>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_common.h"

#include "common/log_common.h"

enum { WIDTH = 640, HEIGHT = 615 }; // 640,615 定义了位图的宽度和高度

Gradients::Gradients()
{
    bitmapWidth_ = WIDTH;
    bitmapHeight_ = HEIGHT;
    fileName_ = "gradient_dirty_laundry";
}

struct GradData {
    int fCount = 40; // 40  表示颜色数量
    const uint32_t* fColors;
    const float* fPos;
};

constexpr int K_NUM_COLOR_CHOICES = 40; // 40 定义了颜色数组的大小
Color g_colors[K_NUM_COLOR_CHOICES] = {
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF,
    0xFF000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF,
    0xFFFFFFFF, 0xFF000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000, 0xFFFF0000, 0xFF00FF00,
    0xFF0000FF, 0xFFFFFFFF, 0xFF000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000, 0xFFFF0000,
    0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000, // 8 lines, 40 colors
};

constexpr GradData G_GRAD_DATA[] = { { 40, g_colors, nullptr } }; // 40  表示颜色数量
GradData g_data = G_GRAD_DATA[0];

// 定义三种着色器
OH_Drawing_ShaderEffect* MakeLinear(const OH_Drawing_Point* firstPoint, const OH_Drawing_Point* secondPoint,
    const GradData& g_data, OH_Drawing_TileMode tileMode)
{
    int fCount = 40;     // 40  表示颜色数量
    int arraySize = 100; // 100 用于存储渐变效果中每种颜色的相对位置
    float positions[arraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f; // 0.0f 用于初始化positions数组的元素
    positions[1] = 0.0f; // 0.0f 用于初始化positions数组的元素
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fCount; ++i) {
        positions[i] = static_cast<float>(i) / (fCount - 1.0f);
    }
    DRAWING_LOGI("GradientsGM MakeLinear");
    OH_Drawing_ShaderEffect* MakeLinear = OH_Drawing_ShaderEffectCreateLinearGradient(
        firstPoint, secondPoint, g_data.fColors, positions, fCount, tileMode);
    return MakeLinear;
}
OH_Drawing_ShaderEffect* MakeRadial(const OH_Drawing_Point* firstPoint, const OH_Drawing_Point* secondPoint,
    const GradData& g_data, OH_Drawing_TileMode tileMode)
{
    DRAWING_LOGI("GradientsGM MakeRadial");

    // 圆心和圆半径坐标
    OH_Drawing_Point2D pts[] = { { 0, 0 }, { 100.0, 100.0 } };//100 坐标点
    OH_Drawing_Point2D Point = { SCALAR_AVE(pts[0].x, pts[1].x), SCALAR_AVE(pts[0].y, pts[1].y) };
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(Point.x, Point.y);
    int fCount = 40;     // 40  表示颜色数量
    int arraySize = 100; // 100 用于存储渐变效果中每种颜色的相对位置
    float positions[arraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f; // 0.0f 用于初始化positions数组的元素
    positions[1] = 0.0f; // 0.0f 用于初始化positions数组的元素
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fCount; ++i) {
        positions[i] = static_cast<float>(i) / (fCount - 1.0f);
    }
    OH_Drawing_ShaderEffect* MakeRadial =
        OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, Point.x, g_data.fColors, positions, fCount, tileMode);
    return MakeRadial;
}
OH_Drawing_ShaderEffect* MakeSweep(const OH_Drawing_Point* firstPoint, const OH_Drawing_Point* secondPoint,
    const GradData& g_data, OH_Drawing_TileMode tileMode)
{
    DRAWING_LOGI("GradientsGM MakeSweep");
    // 圆心和圆半径坐标
    OH_Drawing_Point2D pts[] = { { 0, 0 }, { 100.0, 100.0 } }; // 100 设置点
    OH_Drawing_Point2D Point = { (pts[0].x+ pts[1].x)/2, (pts[0].y+ pts[1].y)/2 };// 2 取中点
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(Point.x, Point.y);
    int fCount = 40;     // 40  表示颜色数量
    int arraySize = 100; // 100 用于存储渐变效果中每种颜色的相对位置
    float positions[arraySize];
    // 初始化数组的前两个元素为0.0f
    positions[0] = 0.0f; // 0.0f 用于初始化positions数组的元素
    positions[1] = 0.0f; // 0.0f 用于初始化positions数组的元素
    // 用等间距的值填充数组的其余部分
    for (int i = 2; i < fCount; ++i) {
        positions[i] = static_cast<float>(i) / (fCount - 1.0f);
    }
    OH_Drawing_ShaderEffect* MakeSweep =
        OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, g_data.fColors, positions, fCount, tileMode);
    return MakeSweep;
}

typedef OH_Drawing_ShaderEffect* (*GradMaker)(
    const OH_Drawing_Point* firstPoint, const OH_Drawing_Point* secondPoint, const GradData&, OH_Drawing_TileMode);
constexpr GradMaker G_GRAD_MAKERS[] = {
    MakeLinear,
    MakeRadial,
    MakeSweep,
};

void Gradients::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("GradientsGM::OnTestFunction start");

    // 使用指定颜色设置清空画布底色
    OH_Drawing_CanvasClear(canvas, 0xFFDDDDDD);
    // 创建一个画刷brush对象设置抗锯齿属性
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);

    OH_Drawing_Rect* rectAngLe = OH_Drawing_RectCreate(0, 0, IntToScalar(100), IntToScalar(100)); //100 创建一个矩形对象
    // 平移画布
    OH_Drawing_CanvasTranslate(canvas, IntToScalar(20),
        IntToScalar(20)); // 20 是用于平移

    for (size_t i = 0; i < SK_ARRAY_COUNT(G_GRAD_DATA); i++) {
        OH_Drawing_CanvasSave(canvas);
        for (size_t j = 0; j < SK_ARRAY_COUNT(G_GRAD_MAKERS); j++) {
            OH_Drawing_BrushSetShaderEffect(
                brush, G_GRAD_MAKERS[j](firstPoint, secondPoint, G_GRAD_DATA[i], OH_Drawing_TileMode::CLAMP));
            OH_Drawing_CanvasAttachBrush(canvas, brush);
            OH_Drawing_CanvasDrawRect(canvas, rectAngLe);
            OH_Drawing_CanvasTranslate(
                canvas, 0, IntToScalar(120)); // 120 Indicates the distance to translate on x-axis、y-axis.
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, IntToScalar(120), 0); // 120 Indicates the distance to translate on x-axis
    }
    DRAWING_LOGI("GradientsGM::OnTestFunction end");
    OH_Drawing_RectDestroy(rectAngLe);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
}
