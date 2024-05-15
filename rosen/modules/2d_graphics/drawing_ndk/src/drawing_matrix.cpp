/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "drawing_matrix.h"

#include <vector>

#include "utils/matrix.h"
#include "utils/rect.h"

static constexpr int POLY_POINT_COUNT_MAX = 4;
using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Matrix* CastToMatrix(OH_Drawing_Matrix* cMatrix)
{
    return reinterpret_cast<Matrix*>(cMatrix);
}

static const Matrix* CastToMatrix(const OH_Drawing_Matrix* cMatrix)
{
    return reinterpret_cast<const Matrix*>(cMatrix);
}

static Point* CastToPoint(OH_Drawing_Point2D* cPoint)
{
    return reinterpret_cast<Point*>(cPoint);
}

static const Point* CastToPoint(const OH_Drawing_Point2D* cPoint)
{
    return reinterpret_cast<const Point*>(cPoint);
}

static Rect& CastToRect(OH_Drawing_Rect& cRect)
{
    return reinterpret_cast<Rect&>(cRect);
}

static const Rect& CastToRect(const OH_Drawing_Rect& cRect)
{
    return reinterpret_cast<const Rect&>(cRect);
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreate()
{
    return (OH_Drawing_Matrix*)new Matrix();
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreateRotation(float deg, float x, float y)
{
    Matrix* matrix = new Matrix();
    if (matrix == nullptr) {
        return nullptr;
    }
    matrix->Rotate(deg, x, y);
    return (OH_Drawing_Matrix*)matrix;
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreateScale(float sx, float sy, float px, float py)
{
    Matrix* matrix = new Matrix();
    if (matrix == nullptr) {
        return nullptr;
    }
    matrix->Scale(sx, sy, px, py);
    return (OH_Drawing_Matrix*)matrix;
}

OH_Drawing_Matrix* OH_Drawing_MatrixCreateTranslation(float dx, float dy)
{
    Matrix* matrix = new Matrix();
    if (matrix == nullptr) {
        return nullptr;
    }
    matrix->Translate(dx, dy);
    return (OH_Drawing_Matrix*)matrix;
}

void OH_Drawing_MatrixSetMatrix(OH_Drawing_Matrix* cMatrix, float scaleX, float skewX, float transX,
    float skewY, float scaleY, float transY, float persp0, float persp1, float persp2)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
}

bool OH_Drawing_MatrixSetRectToRect(OH_Drawing_Matrix* cMatrix, const OH_Drawing_Rect* src,
    const OH_Drawing_Rect* dst, OH_Drawing_ScaleToFit stf)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr || src == nullptr || dst == nullptr) {
        return false;
    }
    return matrix->SetRectToRect(CastToRect(*src), CastToRect(*dst), static_cast<ScaleToFit>(stf));
}


void OH_Drawing_MatrixPreScale(OH_Drawing_Matrix* cMatrix, float sx, float sy, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->PreScale(sx, sy, px, py);
}

void OH_Drawing_MatrixPreTranslate(OH_Drawing_Matrix* cMatrix, float dx, float dy)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->PreTranslate(dx, dy);
}


void OH_Drawing_MatrixPreRotate(OH_Drawing_Matrix* cMatrix, float degree, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->PreRotate(degree, px, py);
}

void OH_Drawing_MatrixPostScale(OH_Drawing_Matrix* cMatrix, float sx, float sy, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->PostScale(sx, sy, px, py);
}

void OH_Drawing_MatrixPostTranslate(OH_Drawing_Matrix* cMatrix, float dx, float dy)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->PostTranslate(dx, dy);
}

void OH_Drawing_MatrixPostRotate(OH_Drawing_Matrix* cMatrix, float degree, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->PostRotate(degree, px, py);
}

void OH_Drawing_MatrixReset(OH_Drawing_Matrix* cMatrix)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Reset();
}

void OH_Drawing_MatrixConcat(OH_Drawing_Matrix* cTotal, const OH_Drawing_Matrix* cA,
    const OH_Drawing_Matrix* cB)
{
    if (cTotal == nullptr || cA == nullptr || cB == nullptr) {
        return;
    }
    Matrix* total = CastToMatrix(cTotal);
    Matrix* a = CastToMatrix(const_cast<OH_Drawing_Matrix*>(cA));
    Matrix* b = CastToMatrix(const_cast<OH_Drawing_Matrix*>(cB));
    *total = *a;
    total->PreConcat(*b);
}

float OH_Drawing_MatrixGetValue(OH_Drawing_Matrix* cMatrix, int index)
{
    // 3x3 matrix index is between 0-8
    if (cMatrix == nullptr || index < 0 || index > 8) {
        return 0;
    }
    Matrix* matrix = CastToMatrix(cMatrix);
    return matrix->Get(index);
}

void OH_Drawing_MatrixRotate(OH_Drawing_Matrix* cMatrix, float degree, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Rotate(degree, px, py);
}

void OH_Drawing_MatrixTranslate(OH_Drawing_Matrix* cMatrix, float dx, float dy)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Translate(dx, dy);
}

void OH_Drawing_MatrixScale(OH_Drawing_Matrix* cMatrix, float sx, float sy, float px, float py)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    matrix->Scale(sx, sy, px, py);
}

bool OH_Drawing_MatrixInvert(OH_Drawing_Matrix* cMatrix, OH_Drawing_Matrix* inverse)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    Matrix* inverseMatrix = CastToMatrix(inverse);
    if (inverseMatrix == nullptr) {
        return false;
    }
    return matrix->Invert(*inverseMatrix);
}

bool OH_Drawing_MatrixIsEqual(OH_Drawing_Matrix* cMatrix, OH_Drawing_Matrix* other)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    Matrix* otherMatrix = CastToMatrix(other);
    if (otherMatrix == nullptr) {
        return false;
    }
    return (*matrix == *otherMatrix);
}

bool OH_Drawing_MatrixSetPolyToPoly(OH_Drawing_Matrix* cMatrix, const OH_Drawing_Point2D* src,
    const OH_Drawing_Point2D* dst, uint32_t count)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    if (count > POLY_POINT_COUNT_MAX) {
        return false;
    }
    return matrix->SetPolyToPoly(CastToPoint(src), CastToPoint(dst), count);
}

void OH_Drawing_MatrixMapPoints(const OH_Drawing_Matrix* cMatrix, const OH_Drawing_Point2D* src,
    OH_Drawing_Point2D* dst, int count)
{
    const Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return;
    }
    const Point* srcTemp = CastToPoint(src);
    Point* dstTemp = CastToPoint(dst);
    std::vector<Point> srcPoints(count);
    std::vector<Point> dstPoints(count);
    for (int idx = 0; idx < count; idx++) {
        srcPoints[idx] = srcTemp[idx];
        dstPoints[idx] = dstTemp[idx];
    }
    matrix->MapPoints(dstPoints, srcPoints, count);
    for (int idx = 0; idx < count; idx++) {
        dstTemp[idx] = dstPoints[idx];
    }
}

bool OH_Drawing_MatrixMapRect(const OH_Drawing_Matrix* cMatrix, const OH_Drawing_Rect* src, OH_Drawing_Rect* dst)
{
    const Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    return matrix->MapRect(CastToRect(*dst), CastToRect(*src));
}

bool OH_Drawing_MatrixIsIdentity(OH_Drawing_Matrix* cMatrix)
{
    Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        return false;
    }
    return matrix->IsIdentity();
}

void OH_Drawing_MatrixDestroy(OH_Drawing_Matrix* cMatrix)
{
    delete CastToMatrix(cMatrix);
}
