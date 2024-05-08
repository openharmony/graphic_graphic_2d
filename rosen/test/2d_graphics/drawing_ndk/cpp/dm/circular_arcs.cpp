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
#include "circular_arcs.h"

#include <cmath>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <vector>

#include "test_common.h"

#include "common/log_common.h"

enum {
    K_W = 820,
    K_H = 1090,
};

CircularArcStrokeMatrix::CircularArcStrokeMatrix()
{
    // file gm/circulararcs.cpp
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "circular_arc_stroke_matrix";
}

void CircularArcStrokeMatrix::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    static constexpr float coordinate = 40.f;
    static constexpr float kStrokeWidth = 5.f;
    static constexpr float kStart = 89.f;
    static constexpr float kSweep = 180.f / M_PI; // one radian

    std::vector<OH_Drawing_Matrix*> matrices = CreateMatrices(coordinate);
    int baseMatrixCnt = matrices.size();
    ApplyRotations(matrices, baseMatrixCnt, coordinate);
    int x = 0;
    int y = 0;
    static constexpr float kPad = 2 * kStrokeWidth;
    OH_Drawing_CanvasTranslate(canvas, kPad, kPad);
    auto bounds = OH_Drawing_RectCreate(0, 0, 2 * coordinate, 2 * coordinate); // 2cout

    for (auto cap : { LINE_ROUND_CAP, LINE_FLAT_CAP, LINE_SQUARE_CAP }) {
        for (const auto m : matrices) {
            auto pen = OH_Drawing_PenCreate();
            OH_Drawing_PenSetCap(pen, cap);
            OH_Drawing_PenSetAntiAlias(pen, true);
            OH_Drawing_PenSetWidth(pen, kStrokeWidth);
            OH_Drawing_CanvasSave(canvas);
            {
                OH_Drawing_CanvasTranslate(canvas, x * (2 * coordinate + kPad), y * (2 * coordinate + kPad)); // 2 距离
                OH_Drawing_CanvasConcatMatrix(canvas, m);
                OH_Drawing_PenSetColor(pen, 0x80FF0000); // red
                OH_Drawing_CanvasAttachPen(canvas, pen);
                OH_Drawing_CanvasDrawArc(canvas, bounds, kStart, kSweep);
                OH_Drawing_PenSetColor(pen, 0x800000FF); // blue
                OH_Drawing_CanvasAttachPen(canvas, pen);
                OH_Drawing_CanvasDrawArc(canvas, bounds, kStart, kSweep - 360.f); // kSweep - 360.f坐标
            }
            OH_Drawing_CanvasRestore(canvas);
            OH_Drawing_CanvasDetachPen(canvas);
            OH_Drawing_PenDestroy(pen);
            ++x;
            if (x == baseMatrixCnt) {
                x = 0;
                ++y;
            }
        }
    }

    for (auto m : matrices) {
        OH_Drawing_MatrixDestroy(m);
    }
    OH_Drawing_RectDestroy(bounds);
}

std::vector<OH_Drawing_Matrix*> CircularArcStrokeMatrix::CreateMatrices(float coordinate)
{
    std::vector<OH_Drawing_Matrix*> matrices;

    matrices.push_back(OH_Drawing_MatrixCreateRotation(coordinate, coordinate, 45.f));

    OH_Drawing_Matrix* mI = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(mI, 1, 0, 0, 0, 1, 0, 0, 0, 1); // 1矩阵
    matrices.push_back(mI);

    OH_Drawing_Matrix* m1 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m1, -1, 0, 2 * coordinate, 0, 1, 0, 0, 0, 1); // -1, 2, 1矩阵
    OH_Drawing_Matrix* m2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m2, 1, 0, 0, 0, -1, 2 * coordinate, 0, 0, 1); // 1, -1, 2矩阵
    OH_Drawing_Matrix* m3 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m3, 1, 0, 0, 0, -1, 2 * coordinate, 0, 0, 1); // -1, 1, 2矩阵
    OH_Drawing_Matrix* m4 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m4, 0, -1, 2 * coordinate, -1, 0, 2 * coordinate, 0, 0, 1); // 1,-1,2矩阵
    OH_Drawing_Matrix* m5 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m5, 0, -1, 2 * coordinate, 1, 0, 0, 0, 0, 1); // -1,1,2矩阵
    OH_Drawing_Matrix* m6 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m6, 0, 1, 0, 1, 0, 0, 0, 0, 1); // 1矩阵
    OH_Drawing_Matrix* m7 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m7, 0, 1, 0, -1, 0, 2 * coordinate, 0, 0, 1); // -1, 1, 2矩阵

    matrices.push_back(m1);
    matrices.push_back(m2);
    matrices.push_back(m3);
    matrices.push_back(m4);
    matrices.push_back(m5);
    matrices.push_back(m6);
    matrices.push_back(m7);
    return matrices;
}

void CircularArcStrokeMatrix::ApplyRotations(
    std::vector<OH_Drawing_Matrix*>& matrices, int baseMatrixCnt, float coordinate)
{
    OH_Drawing_Matrix* tinyCW = OH_Drawing_MatrixCreateRotation(0.001f, coordinate, coordinate); // 0.001f 旋转角度
    for (int i = 0; i < baseMatrixCnt; ++i) {
        OH_Drawing_Matrix* mTotal = OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixConcat(mTotal, matrices[i], tinyCW);
        matrices.push_back(mTotal);
    }
    OH_Drawing_MatrixDestroy(tinyCW);

    OH_Drawing_Matrix* tinyCCW = OH_Drawing_MatrixCreateRotation(-0.001f, coordinate, coordinate); // -0.001f 旋转角度
    for (int i = 0; i < baseMatrixCnt; ++i) {
        OH_Drawing_Matrix* mTotal = OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixConcat(mTotal, matrices[i], tinyCCW);
        matrices.push_back(mTotal);
    }
    OH_Drawing_MatrixDestroy(tinyCCW);

    OH_Drawing_Matrix* cw45 = OH_Drawing_MatrixCreateRotation(45.f, coordinate, coordinate); // 45.f 旋转角度
    for (int i = 0; i < baseMatrixCnt; ++i) {
        OH_Drawing_Matrix* mTotal = OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixConcat(mTotal, matrices[i], cw45);
        matrices.push_back(mTotal);
    }
    OH_Drawing_MatrixDestroy(cw45);
}