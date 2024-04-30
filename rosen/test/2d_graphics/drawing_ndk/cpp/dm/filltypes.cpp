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
#include "filltypes.h"

#include <array>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_common.h"

FillType::FillType()
{
    bitmapWidth_ = 835;  // 835 is bitmapWidth
    bitmapHeight_ = 840; // 840 is bitmapHeight
    fileName_ = "filltypes";
}

FillType::~FillType()
{
    OH_Drawing_PathDestroy(path);
    path = nullptr; // 避免悬挂指针
}

void FillType::showPath(
    OH_Drawing_Canvas* canvas, int x, int y, OH_Drawing_PathFillType ft, float scale, OH_Drawing_Brush* brush)
{
    // 创建一个矩形对象为图像画布
    float right = 150;                                                       // 150 矩阵创建参数
    float bottom = 150;                                                      // 150 矩阵创建参数
    OH_Drawing_Rect* rectAngLe = OH_Drawing_RectCreate(0, 0, right, bottom); // 0, 0, 创建矩阵对象参数
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_CanvasTranslate(canvas, x, y);
    OH_Drawing_CanvasClipRect(canvas, rectAngLe, OH_Drawing_CanvasClipOp::INTERSECT, false);
    // 使用指定白色清空画布底色
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));

    OH_Drawing_PathSetFillType(path, ft);
    // 其中这里平移画布的x=rectAngLe.left + rectAngLe.right.y=rectAngLe.top + rectAngLe.bottom
    OH_Drawing_CanvasTranslate(canvas, right * SK_SCALAR_HALF, bottom * SK_SCALAR_HALF);
    OH_Drawing_CanvasScale(canvas, scale, scale);
    OH_Drawing_CanvasTranslate(canvas, -(right * SK_SCALAR_HALF), -(bottom * SK_SCALAR_HALF));
    // 上画布
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_RectDestroy(rectAngLe);
}

// FillTypeGM::showFour 左上角、右上角、左下角、右下角四个为一组选择枚举类型进行变换
void FillType::showFour(OH_Drawing_Canvas* canvas, float scale, OH_Drawing_Brush* brush)
{
    showPath(canvas, 0, 0, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING, scale, brush);           // 0, 0 showPath
    showPath(canvas, 200, 0, OH_Drawing_PathFillType::PATH_FILL_TYPE_EVEN_ODD, scale, brush);        // 200, 0 showPath
    showPath(canvas, 0, 200, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_WINDING, scale, brush); // 0, 200 showPath
    showPath(canvas, 200, 200, OH_Drawing_PathFillType::PATH_FILL_TYPE_INVERSE_EVEN_ODD, scale,
        brush); //  200, 200 showPath
}

void FillType::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 创建画刷brush对象
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    // 使用指定颜色设置清空画布底色:
    // sk源代码色度是0xFFDDDDDD为RGB221图像出来0xFFD9D9D9灰度RGB217，测试功能代码设置xFFD9D9D9
    OH_Drawing_CanvasClear(canvas, 0xFFD9D9D9);
    // 创建矩形对象并将弧添加到路径中
    float x = 50; // 50 矩阵创建参数
    float y = 50; // 50 矩阵创建参数
    float r = 45; // 45 矩阵创建参数
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(x - r, y - r, x + r, y + r);
    x = 100; // 100 矩阵创建参数
    y = 100; // 100 矩阵创建参数
    r = 45;  // 45矩阵创建参数
    OH_Drawing_Rect* rects = OH_Drawing_RectCreate(x - r, y - r, x + r, y + r);
    float startAngle = 0;     // 0 PathAddArc参数
    float sweepAngle = 360.0; // 360.0 PathAddArc参数
    OH_Drawing_PathAddArc(path, rect, startAngle, sweepAngle);
    OH_Drawing_PathAddArc(path, rects, startAngle, sweepAngle);

    // 上述前置动作已做完
    OH_Drawing_CanvasTranslate(canvas, 20, 20); // 20,20 Indicates the distance to translate on x-axis、y-axis
    const float scale = 1.25;                   // 1.25：左侧两个与右侧两个的图像放大缩小
    OH_Drawing_BrushSetAntiAlias(brush, false);
    showFour(canvas, SK_SCALAR1, brush);
    OH_Drawing_CanvasTranslate(canvas, 450, 0); // 450, 0 Translate参数
    showFour(canvas, scale, brush);
    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_CanvasTranslate(canvas, -450, 450); //-450, 450 Translate参数
    showFour(canvas, SK_SCALAR1, brush);
    OH_Drawing_CanvasTranslate(canvas, 450, 0); // 450, 0 Translate参数
    showFour(canvas, scale, brush);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rects);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
}
