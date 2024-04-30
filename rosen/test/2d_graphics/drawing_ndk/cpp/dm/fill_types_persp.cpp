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
#include "fill_types_persp.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

#include "common/log_common.h"

enum {
    K_W = 835, // 835是位图宽度
    K_H = 840, // 840是位图高度
};

FillTypePersp::FillTypePersp()
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "filltypespersp";
}

void FillTypePersp::MakePath()
{
    if (fPath) {
        OH_Drawing_PathDestroy(fPath);
    }
    fPath = OH_Drawing_PathCreate();
    float centerX1 = 50;  // 50 创建矩形对象参数
    float centerY1 = 50;  // 40 创建矩形对象参数
    float radius1 = 45;   // 45 创建矩形对象参数
    float centerX2 = 100; // 100 创建矩形对象参数
    float centerY2 = 100; // 100 创建矩形对象参数
    float radius2 = 45;   // 45 创建矩形对象参数
    DrawPathAddCircle(fPath, centerX1, centerY1, radius1);
    DrawPathAddCircle(fPath, centerX2, centerY2, radius2);
}

void FillTypePersp::ShowPath(
    OH_Drawing_Canvas* canvas, int x, int y, OH_Drawing_PathFillType ft, float scale, OH_Drawing_Brush* brush)
{
    DrawRect r = { 0, 0, 150, 150 }; // 0, 0, 150, 150 DrawRect 参数
    OH_Drawing_Rect* rc = DrawCreateRect(r);
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasTranslate(canvas, x, y);
    OH_Drawing_CanvasClipRect(canvas, rc, OH_Drawing_CanvasClipOp::INTERSECT, false);
    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_PathSetFillType(fPath, ft);
    OH_Drawing_CanvasTranslate(canvas, r.CenterX(), r.CenterY());
    OH_Drawing_CanvasScale(canvas, scale, scale);
    OH_Drawing_CanvasTranslate(canvas, -r.CenterX(), -r.CenterY());
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, fPath);
    OH_Drawing_RectDestroy(rc);
    OH_Drawing_CanvasRestore(canvas);
}

void FillTypePersp::ShowFour(OH_Drawing_Canvas* canvas, float scale, bool aa)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Point* center = OH_Drawing_PointCreate(100, 100); // 100, 100  PointCreate参数值
    uint32_t colors[] = { DRAW_COLORBLUE, DRAW_COLORRED, DRAW_COLORGREEN };
    float pos[] = { 0, 0.5,
        1.0 }; //{ 0, 0.5, 1.0 }   Indicates the relative position of each corresponding color in the colors array.
    OH_Drawing_ShaderEffect* effect = OH_Drawing_ShaderEffectCreateRadialGradient(
        center, 100, colors, pos, 3, OH_Drawing_TileMode::CLAMP); // 100Indicates the radius of the circle for this
                                                                  // gradient.3Indicates the number of colors and pos.
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_BrushSetAntiAlias(brush, aa);
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    ShowPath(canvas, 0, 0, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING, scale,
        brush); // 0 、0是绘制路径时的平移量
    ShowPath(canvas, 200, 0, OH_Drawing_PathFillType::PATH_FILL_TYPE_EVEN_ODD, scale,
        brush); // 200、 0  是绘制路径时的平移量
    ShowPath(canvas, 0, 200, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_WINDING, scale,
        brush); // 0、200  是绘制路径时的平移量
    ShowPath(canvas, 200, 200, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_EVEN_ODD, scale,
        brush); // 200、200  是绘制路径时的平移量

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PointDestroy(center);
}

void FillTypePersp::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    MakePath();

    OH_Drawing_Brush* bkgnrd = OH_Drawing_BrushCreate();
    OH_Drawing_Point* center = OH_Drawing_PointCreate(100, 100); // 100, 100  PointCreate参数值
    uint32_t colors[] = { DRAW_COLORBLACK, DRAW_COLORCYAN, DRAW_COLORYELLOW, DRAW_COLORWHITE };
    float pos[] = {
        0, 0.25, 0.75, 1.0
    }; //{ 0, 0.25, 0.75, 1.0 }  Indicates the relative position of each corresponding color in the colors array.
    OH_Drawing_ShaderEffect* effect = OH_Drawing_ShaderEffectCreateRadialGradient(
        center, 1000, colors, pos, 4, OH_Drawing_TileMode::CLAMP); // 1000 Indicates the radius of the circle for this
                                                                   // gradient.4 Indicates the number of colors and pos.
    OH_Drawing_BrushSetShaderEffect(bkgnrd, effect);
    OH_Drawing_CanvasAttachBrush(canvas, bkgnrd);

    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasTranslate(canvas, 100, 100); // 100, 100  Indicates the distance to translate on x-axis、y-axis.

    OH_Drawing_Matrix* mat = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(mat, 1, 0, 0, 0, 1, 0, 0, 1.0 / 1000,
        1); // 1、 0、 0、 0、 1、 0、 0、 1.0 / 1000都是矩阵参数
    OH_Drawing_CanvasConcatMatrix(canvas, mat);
    OH_Drawing_CanvasDrawBackground(canvas, bkgnrd);
    OH_Drawing_CanvasRestore(canvas);

    OH_Drawing_Matrix* persp = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(
        persp, 1, 0, 0, 0, 1, 0, -1.0 / 1800, 1.0 / 500, 1); //  1, 0, 0, 0, 1, 0, -1.0 / 1800, 1.0 / 500, 1 矩阵参数
    OH_Drawing_CanvasConcatMatrix(canvas, persp);
    OH_Drawing_CanvasTranslate(canvas, 20, 20); // 20, 20 Translate 参数

    float scale = 5.0 / 4;                      //   5.0 / 4 ShowFour 参数
    ShowFour(canvas, 1.0, false);               // 1.0 ShowFour
    OH_Drawing_CanvasTranslate(canvas, 450, 0); // 450, 0 参数
    ShowFour(canvas, scale, false);

    OH_Drawing_CanvasTranslate(canvas, -450, 450); //  -450, 450  Translate参数
    ShowFour(canvas, 1, true);                     //  1 ShowFour
    OH_Drawing_CanvasTranslate(canvas, 450, 0);    // 450, 0  Translate参数
    ShowFour(canvas, scale, true);

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_BrushDestroy(bkgnrd);
    OH_Drawing_MatrixDestroy(mat);
}
