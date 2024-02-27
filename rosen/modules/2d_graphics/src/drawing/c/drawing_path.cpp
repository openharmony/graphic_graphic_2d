/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "c/drawing_point.h"
#include "c/drawing_path.h"

#include "draw/path.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Path* CastToPath(OH_Drawing_Path* cPath)
{
    return reinterpret_cast<Path*>(cPath);
}

static const Matrix* CastToMatrix(const OH_Drawing_Matrix* cMatrix)
{
    return reinterpret_cast<const Matrix*>(cMatrix);
}

static const Rect& CastToRect(const OH_Drawing_Rect& cRect)
{
    return reinterpret_cast<const Rect&>(cRect);
}

static const RoundRect& CastToRoundRect(const OH_Drawing_RoundRect& cRoundRect)
{
    return reinterpret_cast<const RoundRect&>(cRoundRect);
}

OH_Drawing_Path* OH_Drawing_PathCreate()
{
    return (OH_Drawing_Path*)new Path;
}

OH_Drawing_Path* OH_Drawing_PathCopy(OH_Drawing_Path* cPath)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_Path*)new Path(*path);
}

void OH_Drawing_PathDestroy(OH_Drawing_Path* cPath)
{
    delete CastToPath(cPath);
}

void OH_Drawing_PathMoveTo(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->MoveTo(x, y);
}

void OH_Drawing_PathLineTo(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->LineTo(x, y);
}

void OH_Drawing_PathArcTo(
    OH_Drawing_Path* cPath, float x1, float y1, float x2, float y2, float startDeg, float sweepDeg)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg);
}

void OH_Drawing_PathQuadTo(OH_Drawing_Path* cPath, float ctrlX, float ctrlY, float endX, float endY)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->QuadTo(ctrlX, ctrlY, endX, endY);
}

void OH_Drawing_PathCubicTo(
    OH_Drawing_Path* cPath, float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2, float endX, float endY)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->CubicTo(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
}

void OH_Drawing_PathAddRect(OH_Drawing_Path* cPath, float left,
    float top, float right, float bottom, OH_Drawing_PathDirection dir)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW) {
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->AddRect(left, top, right, bottom, static_cast<PathDirection>(dir));
}

void OH_Drawing_PathAddRoundRect(OH_Drawing_Path* cPath,
    const OH_Drawing_RoundRect* roundRect, OH_Drawing_PathDirection dir)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW ||
        roundRect == nullptr) {
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->AddRoundRect(CastToRoundRect(*roundRect), static_cast<PathDirection>(dir));
}

void OH_Drawing_PathAddArc(OH_Drawing_Path* cPath,
    const OH_Drawing_Rect* oval, float startAngle, float sweepAngle)
{
    if (oval == nullptr) {
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->AddArc(CastToRect(*oval), startAngle, sweepAngle);
}

void OH_Drawing_PathAddPath(OH_Drawing_Path* cPath,
    const OH_Drawing_Path* src, const OH_Drawing_Matrix* matrix)
{
    if (src == nullptr) {
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    Path* srcPath = CastToPath(const_cast<OH_Drawing_Path*>(src));
    if (matrix == nullptr) {
        path->AddPath(*srcPath);
        return;
    }
    path->AddPath(*srcPath, *CastToMatrix(matrix));
}

bool OH_Drawing_PathContains(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return false ;
    }
    return path->Contains(x, y);
}

void OH_Drawing_PathTransform(OH_Drawing_Path* cPath, const OH_Drawing_Matrix* matrix)
{
    if (matrix == nullptr) {
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->Transform(*CastToMatrix(matrix));
}

void OH_Drawing_PathSetFillType(OH_Drawing_Path* cPath, OH_Drawing_PathFillType fillstyle)
{
    if (fillstyle < PATH_FILL_TYPE_WINDING || fillstyle > PATH_FILL_TYPE_INVERSE_EVEN_ODD) {
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->SetFillStyle(static_cast<PathFillType>(fillstyle));
}

void OH_Drawing_PathClose(OH_Drawing_Path* cPath)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->Close();
}

void OH_Drawing_PathReset(OH_Drawing_Path* cPath)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        return;
    }
    path->Reset();
}
