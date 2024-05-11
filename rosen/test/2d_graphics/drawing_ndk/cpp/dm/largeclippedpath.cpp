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
#include "largeclippedpath.h"

#include <hilog/log.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_round_rect.h>

#include "cmath"
#include "test_common.h"

#include "common/log_common.h"

constexpr int K_SIZE = 1000; // 1000 Size

void LargeClippedPath::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 使用指定颜色清空画布
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));
    // 创建一个画刷brush对象
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0xFF));
    OH_Drawing_BrushSetAntiAlias(brush, true);
    // 创建两个path对象
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* pathQuad = OH_Drawing_PathCreate();
    OH_Drawing_PathSetFillType(path, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING);
    // 按指定方向，向路径添加矩形轮廓
    // x和y源代码执行了位操作,x和y是偶数时执行顺时针,x和y是奇数时执行逆时针.
    constexpr static int kGridCount = 50; // 50 是网格的数量
    constexpr static float kCellSize = static_cast<float>(K_SIZE) / kGridCount;
    constexpr static int kNumPetals = 9;          // 9 是控制点数量
    constexpr static int kEvenOddCheckModulo = 2; // 2 用于在循环中检查索引的奇偶性
    for (int y = 0; y < kGridCount; ++y) {
        OH_Drawing_PathAddRect(path, 0, y * kCellSize, K_SIZE, (y + 1) * kCellSize,
            (y % kEvenOddCheckModulo == 0) ? OH_Drawing_PathDirection::PATH_DIRECTION_CW
                                           : OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    } // 0、y * kCellSize、kSize、(y + 1) * kCellSize用于确定垂直矩形的位置和大小
    for (int x = 0; x < kGridCount; ++x) {
        OH_Drawing_PathAddRect(path, x * kCellSize, 0, (x + 1) * kCellSize, K_SIZE,
            (x % 2 == 0) ? PATH_DIRECTION_CW : PATH_DIRECTION_CCW); // 2 取偶数
    } // 2 是用于确定水平矩形的位置和大小。
    // 裁剪一个自定义路径，源代码并未说是取交集还是差集DIFFERENCE，这里暂定取交集INTERSECT
    bool doAntiAlias = false;
    OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, doAntiAlias);
    // 画一个二阶贝塞尔圆滑曲线
    OH_Drawing_PathSetFillType(pathQuad, fType);

    OH_Drawing_PathMoveTo(pathQuad, 1, 0); // 1、0是贝塞尔曲线的起始坐标
    float n = 2;
    for (int i = 1; i <= kNumPetals; ++i) {
        float c = n * M_PI * (i - 0.5f) / kNumPetals; // 0.5f 曲线参数
        float theta = n * M_PI * i / kNumPetals;
        OH_Drawing_PathQuadTo(pathQuad, cosf(c) * n, sinf(c) * n, cosf(theta), sinf(theta));
    } // 2 * M_PI、i - .5f、i、kNumPetals用于计算贝塞尔曲线上的控制点,2 是用于PathQuadTo参数控制
    OH_Drawing_PathClose(pathQuad);
    // 创建矩形对象并将弧添加到路径中
    OH_Drawing_Rect* rect =
        OH_Drawing_RectCreate(-.75f, -.75f, .75f, .75f); // -.75f、.75f定义了矩形的左上角和右下角的坐标
    OH_Drawing_PathAddArc(pathQuad, rect, 0, 360);       // 0、360表示弧的起始角度和扫过角度
    OH_Drawing_CanvasTranslate(canvas, K_SIZE / 2.f, K_SIZE / 2.f); // K_SIZE / 2.f是用于平移画布的数值

    OH_Drawing_CanvasScale(canvas, K_SIZE / 3.f, K_SIZE / 3.f); // K_SIZE / 3.f是用于缩放画布的数值
    // 上画布
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, pathQuad);
    // 释放内存
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(pathQuad);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    brush = nullptr;
}
