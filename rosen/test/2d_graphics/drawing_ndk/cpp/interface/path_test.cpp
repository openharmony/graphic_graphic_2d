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

#include "path_test.h"

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

#include "test_common.h"

#include "common/log_common.h"

void PathTransformWithPerspectiveClip::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateRotation(
            rand.nextULessThan(360), rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathTransformWithPerspectiveClip(path, matrix, nullptr, applyPerspectiveClip);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_MatrixDestroy(matrix);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathSetFillType::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_PathAddRect(path, l, t, r, b, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
        OH_Drawing_PathSetFillType(path, fType);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathGetLength::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0; // 0 路径长度
    float x;
    float y;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        length = OH_Drawing_PathGetLength(path, bClosed);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathClose::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0; // 0 路径长度
    float x;
    float y;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathClose(path);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathOffset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0; // 0 路径长度
    float x;
    float y;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* pathDst = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathOffset(path, pathDst, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathOffset(path, pathDst, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_CanvasDrawPath(canvas, pathDst);
        OH_Drawing_PathReset(path);
        OH_Drawing_PathReset(pathDst);
    }
    OH_Drawing_PathDestroy(pathDst);
    OH_Drawing_PathDestroy(path);
}

void PathReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float x;
    float y;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0); //   0, 0 这些数字用于设置路径
    x = rand.nextULessThan(bitmapWidth_);
    y = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_PathLineTo(path, x, y);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathLineTo(path, x, y);
    OH_Drawing_CanvasDrawPath(canvas, path);
}

void PathQuadTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* pathQuad = OH_Drawing_PathCreate();
    OH_Drawing_PathSetFillType(pathQuad, PATH_FILL_TYPE_WINDING);
    OH_Drawing_PathMoveTo(pathQuad, 1, 0); //   1, 0 这些数字用于设置路径
    TestRend rand = TestRend();
    for (int i = 0; i < testCount_; i++) {
        float ctrlX = rand.nextUScalar1() * 300; // 300 用于生成随机坐标
        float ctrlY = rand.nextUScalar1() * 300; // 300 用于生成随机坐标
        float endX = rand.nextUScalar1() * 300;  // 300 用于生成随机坐标
        float endY = rand.nextUScalar1() * 300;  // 300 用于生成随机坐标
        OH_Drawing_PathQuadTo(pathQuad, ctrlX, ctrlY, endX, endY);
    }
    OH_Drawing_CanvasDrawPath(canvas, pathQuad);
    OH_Drawing_PathDestroy(pathQuad);
}

void PathEffectCreateDashPathEffect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand = TestRend();
    float intervals[2] = { 1, 1 };
    float x = rand.nextUScalar1() * 300;  // 300 用于生成随机坐标
    float y = rand.nextUScalar1() * 300;  // 300 用于生成随机坐标
    float x1 = rand.nextUScalar1() * 300; // 300 用于生成随机坐标
    float y1 = rand.nextUScalar1() * 300; // 300 用于生成随机坐标
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathEffect* pathEffect =
            OH_Drawing_CreateDashPathEffect(intervals, 2, 0); // 2, 0 这些数字用于设置路径
        OH_Drawing_PathEffectDestroy(pathEffect);
    }
    OH_Drawing_CanvasDrawLine(canvas, x, y, x1, y1);
}

void PathCopy::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand = TestRend();
    float ctrlX = rand.nextUScalar1() * 150; // 150 用于生成随机坐标
    float ctrlY = rand.nextUScalar1() * 150; // 150 用于生成随机坐标
    float endX = rand.nextUScalar1() * 150;  // 150 用于生成随机坐标
    float endY = rand.nextUScalar1() * 150;  // 150 用于生成随机坐标
    OH_Drawing_Path* pathQuad = OH_Drawing_PathCreate();
    OH_Drawing_PathSetFillType(pathQuad, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING);
    OH_Drawing_PathMoveTo(pathQuad, 1, 0); //   1, 0 这些数字用于设置路径
    OH_Drawing_PathQuadTo(pathQuad, ctrlX, ctrlY, endX, endY);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* pathCopy = OH_Drawing_PathCopy(pathQuad);
        OH_Drawing_CanvasDrawPath(canvas, pathCopy);
        OH_Drawing_PathDestroy(pathCopy);
    }
    OH_Drawing_PathDestroy(pathQuad);
}

void PathCreate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand = TestRend();
    for (int i = 0; i < testCount_; i++) {
        float ctrlX = rand.nextUScalar1() * 150; // 150 用于生成随机坐标
        float ctrlY = rand.nextUScalar1() * 150; // 150 用于生成随机坐标
        float endX = rand.nextUScalar1() * 150;  // 150 用于生成随机坐标
        float endY = rand.nextUScalar1() * 150;  // 150 用于生成随机坐标
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 1, 0); //   1, 0 这些数字用于设置路径
        OH_Drawing_PathQuadTo(path, ctrlX, ctrlY, endX, endY);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathDestroy(path);
    }
}

void PathLineTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand = TestRend();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0); //   0, 0 这些数字用于设置路径
    for (int i = 0; i < testCount_; i++) {
        float x = rand.nextUScalar1() * 300; // 300 用于生成随机坐标
        float y = rand.nextUScalar1() * 300; // 300 用于生成随机坐标
        OH_Drawing_PathLineTo(path, x, y);
    }
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathArcTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand = TestRend();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0); //   0, 0 这些数字用于设置路径
    for (int i = 0; i < testCount_; i++) {
        float x = rand.nextUScalar1() * 300;            // 300 用于生成随机坐标
        float y = rand.nextUScalar1() * 300;            // 300 用于生成随机坐标
        float x1 = rand.nextUScalar1() * 300;           // 300 用于生成随机坐标
        float y1 = rand.nextUScalar1() * 300;           // 300 用于生成随机坐标
        OH_Drawing_PathArcTo(path, x, y, x1, y1, 0, 0); // 0, 0  这些数字用于设置原点
    }
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathConicTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand = TestRend();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0); //   0, 0 这些数字用于设置路径
    for (int i = 0; i < testCount_; i++) {
        float ctrlX = rand.nextUScalar1() * 150;                   // 150 用于生成随机坐标
        float ctrlY = rand.nextUScalar1() * 150;                   // 150 用于生成随机坐标
        float endX = rand.nextUScalar1() * 150;                    // 150 用于生成随机坐标
        float endY = rand.nextUScalar1() * 150;                    // 150 用于生成随机坐标
        OH_Drawing_PathConicTo(path, ctrlX, ctrlY, endX, endY, 0); // 0 是传递给OH_Drawing_PathConicTo函数的权重参数
    }
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathAddPathWithMatrixAndMode::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand = TestRend();
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(
        m, 1, 0, 0, 0, 1, 0, 0, 0, 1);   // 1, 0, 0, 0, 1, 0, 0, 0, 1  这些数字定义了变换矩阵的初始状态
    OH_Drawing_MatrixRotate(m, 1, 0, 1); // 1, 0, 1  这些数字是_MatrixRotate参数值
    OH_Drawing_CanvasSave(canvas);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* sourcePath = OH_Drawing_PathCreate();
        OH_Drawing_Path* targetPath = OH_Drawing_PathCreate();
        float x = rand.nextUScalar1() * 300;                      // 300 用于生成随机坐标
        float y = rand.nextUScalar1() * 30;                       // 30 用于生成随机坐标
        float endX = rand.nextUScalar1() * 300;                   // 300 用于生成随机坐标
        float endY = rand.nextUScalar1() * 300;                   // 300 用于生成随机坐标
        OH_Drawing_PathMoveTo(sourcePath, 0, 0);                  //   0, 0 这些数字用于设置路径
        OH_Drawing_PathArcTo(sourcePath, x, y, endX, endY, 0, 0); // 0, 0  PathArcTo参数值
        OH_Drawing_PathArcTo(targetPath, x, y, endX, endY, 0, 0); // 0, 0  PathArcTo参数值
        OH_Drawing_PathAddPathWithMatrixAndMode(targetPath, sourcePath, m, PATH_ADD_MODE_EXTEND);
        OH_Drawing_CanvasDrawPath(canvas, targetPath);
        OH_Drawing_PathDestroy(targetPath);
        OH_Drawing_PathDestroy(sourcePath);
    }
    OH_Drawing_MatrixDestroy(m);
}

void PathAddOvalWithInitialPoint::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;                               // 100 用于高度计算
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);   // 100 用于宽度计算
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100); // 100 用于高度计算
        OH_Drawing_Rect* recv = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_RectSetLeft(recv, renderer.nextF());
        OH_Drawing_CanvasDrawOval(canvas, recv);

        // 添加椭圆路径点
        uint32_t start = renderer.nextU();
        OH_Drawing_PathAddOvalWithInitialPoint(path, recv, start, PATH_DIRECTION_CW);

        OH_Drawing_RectDestroy(recv); // 在循环内部销毁 OH_Drawing_Rect 对象
    }

    // 绘制路径
    OH_Drawing_CanvasDrawPath(canvas, path);

    // 销毁对象
    OH_Drawing_PathDestroy(path);
}

void PathAddArc::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // OH_Drawing_PathSetDirection(path, PATH_DIRECTION_CW); // 设置路径方向为顺时针
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;                               // 100 用于高度计算
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);   // 100 用于宽度计算
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100); // 100 用于高度计算
        OH_Drawing_Rect* recv = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_RectSetLeft(recv, renderer.nextF());
        OH_Drawing_CanvasDrawOval(canvas, recv);

        OH_Drawing_PathAddArc(path, recv, renderer.nextF(), renderer.nextF());
        OH_Drawing_RectDestroy(recv); // 在循环内部销毁 OH_Drawing_Rect 对象
    }
    // 绘制路径
    OH_Drawing_CanvasDrawPath(canvas, path);

    // 销毁对象
    OH_Drawing_PathDestroy(path);
}

void PathCubicTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathCubicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
            rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), rand.nextULessThan(bitmapWidth_),
            rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathCubicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), rand.nextULessThan(bitmapWidth_),
        rand.nextULessThan(bitmapHeight_));
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathRMoveTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathRMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    }
    OH_Drawing_PathRLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathRLineTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathRLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathRLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathRQuadTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathRQuadTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
            rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathRQuadTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathRConicTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathRConicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
            rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
            rand.nextULessThan(10)); // 10 产生小于10的随机整数
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathRConicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
        rand.nextULessThan(10)); // 10 产生小于10的随机整数
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathRCubicTo::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathRCubicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
            rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), rand.nextULessThan(bitmapWidth_),
            rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathRCubicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), rand.nextULessThan(bitmapWidth_),
        rand.nextULessThan(bitmapHeight_));
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void PathAddRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathAddRect(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
            rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), PATH_DIRECTION_CCW);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathAddRectWithInitialCorner::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l;
    float t;
    float r;
    float b;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        l = rand.nextULessThan(bitmapWidth_);
        t = rand.nextULessThan(bitmapHeight_);
        r = l + rand.nextULessThan(bitmapWidth_);
        b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rc = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_PathAddRectWithInitialCorner(path, rc, OH_Drawing_PathDirection::PATH_DIRECTION_CCW, 0);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        OH_Drawing_RectDestroy(rc);
    }
    OH_Drawing_PathDestroy(path);
}

void PathAddRoundRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l;
    float t;
    float r;
    float b;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        l = rand.nextULessThan(bitmapWidth_);
        t = rand.nextULessThan(bitmapHeight_);
        r = l + rand.nextULessThan(bitmapWidth_);
        b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rc = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_RoundRect* rrc = OH_Drawing_RoundRectCreate(rc, rand.nextULessThan(10), rand.nextULessThan(20));
        OH_Drawing_PathAddRoundRect(path, rrc, PATH_DIRECTION_CCW);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        OH_Drawing_RectDestroy(rc);
        OH_Drawing_RoundRectDestroy(rrc);
    }
    OH_Drawing_PathDestroy(path);
}

void PathAddPath::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathLineTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateRotation(rand.nextULessThan(360),
        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_)); // 设定小于360的旋转角度
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathAddPath(path, srcPath, matrix);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathAddPath(path, srcPath, matrix);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_PathDestroy(path);
}

void PathTransform::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    static const int points[] = { 10, 10, 15, 5, 20, 20, 30, 5, 25, 20, 15, 12, 21, 21, 30, 30, 12, 4, 32, 28, 20, 18,
        12, 10 }; // 10, 10, 15, 5, 20, 20, 30, 5, 25, 20, 15, 12, 21, 21, 30, 30, 12, 4, 32, 28, 20, 18,12, 10
                  // 这些数字定义了一组点的坐标
    static const int kMaxPathSize = 10; // 10 定义了每次迭代中从points数组中选择的点的最大数量
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    TestRend rand = TestRend();
    int size = sizeof(points) / sizeof(points[0]);
    int hSize = size / 2; // 2 points数组长度的一半
    for (int i = 0; i < kMaxPathSize; ++i) {
        int xTrans = 10 + 40 * (i % (kMaxPathSize / 2)); // 10,40,2  用于计算路径中平移量
        int yTrans = 0;                                  // 0 用于计算路径中平移量
        if (i > kMaxPathSize / 2 - 1) {                  // 2,1 用于条件判断
            yTrans = 40;                                 // 40 用于计算路径中平移量
        }
        int base1 = 2 * rand.nextULessThan(hSize); // 2 乘以2是为了得到正确的索引
        int base2 = 2 * rand.nextULessThan(hSize); // 2 乘以2是为了得到正确的索引
        int base3 = 2 * rand.nextULessThan(hSize); // 2 乘以2是为了得到正确的索引
        OH_Drawing_PathMoveTo(path, float(points[base1] + xTrans), float(points[base1 + 1] + yTrans)); // 1 用于设置路径
        OH_Drawing_PathLineTo(path, float(points[base2] + xTrans), float(points[base2 + 1] + yTrans)); // 1 用于设置路径
        OH_Drawing_PathLineTo(path, float(points[base3] + xTrans), float(points[base3 + 1] + yTrans)); // 1 用于设置路径
    }
    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreateScale(3, 3, 0, 0); // 3, 3, 0, 0  这些数字用于创建一个变换矩阵
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathTransform(path, m);
    }
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_PathDestroy(path);
}

void PathAddOval::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    TestRend rand;
    float x1 = rand.nextULessThan(bitmapWidth_);
    float y1 = rand.nextULessThan(bitmapHeight_);
    float x2 = rand.nextULessThan(bitmapWidth_);
    float y2 = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(x1, y1, x2, y2);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathAddOval(path, rect, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathAddOval(path, rect, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path);
}

// 有花费时间但是没有图片
void PathContains::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    TestRend rand;
    OH_Drawing_Point2D pts;
    float x1 = rand.nextULessThan(bitmapWidth_);
    float y1 = rand.nextULessThan(bitmapHeight_);
    float x2 = rand.nextULessThan(bitmapWidth_);
    float y2 = rand.nextULessThan(bitmapHeight_);
    float ptsx = rand.nextULessThan(bitmapWidth_);
    float ptsy = rand.nextULessThan(bitmapHeight_);
    pts.x = ptsx;
    pts.y = ptsy;
    OH_Drawing_PathAddRect(path, x1, y1, x2, y2, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathContains(path, ptsx, ptsy);
    }
    OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POINTS, 1, &pts); // 1 用于数量计算
    OH_Drawing_PathDestroy(path);
}

void PathAddPathWithMode::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathLineTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathAddPathWithMode(path, srcPath, addMode);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathAddPathWithMode(path, srcPath, addMode);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
}

void PathAddPathWithOffsetAndMode::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathLineTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathAddPathWithOffsetAndMode(
            path, srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), addMode);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_PathAddPathWithOffsetAndMode(
        path, srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), addMode);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
}
