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
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <hilog/log.h>
#include "common/log_common.h"
#include "test_common.h"
#include "math.h"

constexpr int kSize = 1000;


/*
 * 用例名: largeclippedpath
 * 测试OH_Drawing_CanvasClipPath/OH_Drawing_CanvasScale/OH_Drawing_PathQuadTo/OH_Drawing_PathClose/OH_Drawing_BrushSetAntiAlias/OH_Drawing_BrushSetColor
 *  迁移基于skia gm\largeclippedpath.cpp->dm\largeclippedpath.cpp
 */
void LargeClippedPath::OnTestFunction(OH_Drawing_Canvas *canvas) {
    // 使用指定颜色清空画布
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0xFF, 0xFF));
    // 创建一个画刷brush对象
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0xFF));
    OH_Drawing_BrushSetAntiAlias(brush, true);
    // 创建两个path对象
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_Path *pathQuad = OH_Drawing_PathCreate();
    OH_Drawing_PathSetFillType(path, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING);
    // 按指定方向，向路径添加矩形轮廓
    // x和y源代码执行了位操作,x和y是偶数时执行顺时针,x和y是奇数时执行逆时针.
    constexpr static int kGridCount = 50;
    constexpr static float kCellSize = (float)kSize / kGridCount;
    constexpr static int kNumPetals = 9;
    for (int y = 0; y < kGridCount; ++y) {
        OH_Drawing_PathAddRect(path, 0, y * kCellSize, kSize, (y + 1) * kCellSize,
                               (y % 2 == 0) ? OH_Drawing_PathDirection::PATH_DIRECTION_CW : OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    }
    for (int x = 0; x < kGridCount; ++x) {
        OH_Drawing_PathAddRect(path, x * kCellSize, 0, (x + 1) * kCellSize, kSize,
                               (x % 2 == 0) ? OH_Drawing_PathDirection::PATH_DIRECTION_CW : OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    }
    // 裁剪一个自定义路径，源代码并未说是取交集还是差集DIFFERENCE，这里暂定取交集INTERSECT
    bool doAntiAlias = false;
    OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, doAntiAlias);
    // 画一个二阶贝塞尔圆滑曲线
    OH_Drawing_PathSetFillType(pathQuad, fType);

    OH_Drawing_PathMoveTo(pathQuad, 1, 0);
    for (int i = 1; i <= kNumPetals; ++i) {
        float c = 2 * M_PI * (i - .5f) / kNumPetals;
        float theta = 2 * M_PI * i / kNumPetals;
        OH_Drawing_PathQuadTo(pathQuad, cosf(c) * 2, sinf(c) * 2, cosf(theta), sinf(theta));
    }
    OH_Drawing_PathClose(pathQuad);
    // 创建矩形对象并将弧添加到路径中
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(-.75f, -.75f, .75f, .75f);
    OH_Drawing_PathAddArc(pathQuad, rect, 0, 360);
    OH_Drawing_CanvasTranslate(canvas, kSize / 2.f, kSize / 2.f);
    OH_Drawing_CanvasScale(canvas, kSize / 3.f, kSize / 3.f);
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
