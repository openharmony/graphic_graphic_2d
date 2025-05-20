/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "drawing_path.h"

#include "drawing_canvas_utils.h"

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

static Matrix* CastToMatrix(OH_Drawing_Matrix* cMatrix)
{
    return reinterpret_cast<Matrix*>(cMatrix);
}

static const Rect& CastToRect(const OH_Drawing_Rect& cRect)
{
    return reinterpret_cast<const Rect&>(cRect);
}

static Rect* CastToRect(OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<Rect*>(cRect);
}

static const RoundRect& CastToRoundRect(const OH_Drawing_RoundRect& cRoundRect)
{
    return reinterpret_cast<const RoundRect&>(cRoundRect);
}

static Point* CastToPoint(OH_Drawing_Point2D* cPoint)
{
    return reinterpret_cast<Point*>(cPoint);
}

static const Point* CastToPoint(const OH_Drawing_Point2D* cPoint)
{
    return reinterpret_cast<const Point*>(cPoint);
}

OH_Drawing_Path* OH_Drawing_PathCreate()
{
    return (OH_Drawing_Path*)new Path;
}

OH_Drawing_Path* OH_Drawing_PathCopy(OH_Drawing_Path* cPath)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return (OH_Drawing_Path*)new Path(*path);
}

void OH_Drawing_PathDestroy(OH_Drawing_Path* cPath)
{
    if (!cPath) {
        return;
    }
    delete CastToPath(cPath);
}

OH_Drawing_ErrorCode OH_Drawing_PathSetPath(OH_Drawing_Path* cPath, OH_Drawing_Path* cOther)
{
    if (cPath == nullptr || cOther == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Path* path = CastToPath(cPath);
    Path* other = CastToPath(cOther);
    path->SetPath(*other);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_PathIsEmpty(OH_Drawing_Path* cPath, bool* isEmpty)
{
    if (cPath == nullptr || isEmpty == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *isEmpty = CastToPath(cPath)->IsEmpty();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_PathIsRect(OH_Drawing_Path* cPath, OH_Drawing_Rect* cRect, bool* isRect)
{
    if (cPath == nullptr || isRect == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Path* path = CastToPath(cPath);
    Rect* rect = CastToRect(cRect);
    *isRect = path->IsRect(rect);
    return OH_DRAWING_SUCCESS;
}

void OH_Drawing_PathMoveTo(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->MoveTo(x, y);
}

void OH_Drawing_PathRMoveTo(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->RMoveTo(x, y);
}

void OH_Drawing_PathLineTo(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->LineTo(x, y);
}

void OH_Drawing_PathRLineTo(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->RLineTo(x, y);
}

void OH_Drawing_PathArcTo(
    OH_Drawing_Path* cPath, float x1, float y1, float x2, float y2, float startDeg, float sweepDeg)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg);
}

void OH_Drawing_PathQuadTo(OH_Drawing_Path* cPath, float ctrlX, float ctrlY, float endX, float endY)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->QuadTo(ctrlX, ctrlY, endX, endY);
}

void OH_Drawing_PathRQuadTo(OH_Drawing_Path* cPath, float ctrlX, float ctrlY, float endX, float endY)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->RQuadTo(ctrlX, ctrlY, endX, endY);
}

void OH_Drawing_PathConicTo(OH_Drawing_Path* cPath, float ctrlX, float ctrlY, float endX, float endY, float weight)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->ConicTo(ctrlX, ctrlY, endX, endY, weight);
}

void OH_Drawing_PathRConicTo(OH_Drawing_Path* cPath, float ctrlX, float ctrlY, float endX, float endY, float weight)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->RConicTo(ctrlX, ctrlY, endX, endY, weight);
}

void OH_Drawing_PathCubicTo(
    OH_Drawing_Path* cPath, float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2, float endX, float endY)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->CubicTo(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
}

void OH_Drawing_PathRCubicTo(
    OH_Drawing_Path* cPath, float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2, float endX, float endY)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->RCubicTo(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
}

void OH_Drawing_PathAddRect(OH_Drawing_Path* cPath, float left,
    float top, float right, float bottom, OH_Drawing_PathDirection dir)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddRect(left, top, right, bottom, static_cast<PathDirection>(dir));
}

void OH_Drawing_PathAddRectWithInitialCorner(OH_Drawing_Path* cPath, const OH_Drawing_Rect* cRect,
    OH_Drawing_PathDirection dir, uint32_t start)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    if (cRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddRect(CastToRect(*cRect), start, static_cast<PathDirection>(dir));
}

void OH_Drawing_PathAddRoundRect(OH_Drawing_Path* cPath,
    const OH_Drawing_RoundRect* roundRect, OH_Drawing_PathDirection dir)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    if (roundRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddRoundRect(CastToRoundRect(*roundRect), static_cast<PathDirection>(dir));
}

void OH_Drawing_PathAddOvalWithInitialPoint(OH_Drawing_Path* cPath,
    const OH_Drawing_Rect* oval, uint32_t start, OH_Drawing_PathDirection dir)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    if (oval == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddOval(CastToRect(*oval), start, static_cast<PathDirection>(dir));
}

void OH_Drawing_PathAddArc(OH_Drawing_Path* cPath,
    const OH_Drawing_Rect* arc, float startAngle, float sweepAngle)
{
    if (arc == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddArc(CastToRect(*arc), startAngle, sweepAngle);
}

void OH_Drawing_PathAddPath(OH_Drawing_Path* cPath,
    const OH_Drawing_Path* src, const OH_Drawing_Matrix* matrix)
{
    if (src == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* srcPath = CastToPath(const_cast<OH_Drawing_Path*>(src));
    if (matrix == nullptr) {
        path->AddPath(*srcPath);
        return;
    }
    path->AddPath(*srcPath, *CastToMatrix(matrix));
}

void OH_Drawing_PathAddPathWithMatrixAndMode(OH_Drawing_Path* cPath,
    const OH_Drawing_Path* src, const OH_Drawing_Matrix* cMatrix, OH_Drawing_PathAddMode mode)
{
    if (mode < PATH_ADD_MODE_APPEND || mode > PATH_ADD_MODE_EXTEND) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* srcPath = CastToPath(const_cast<OH_Drawing_Path*>(src));
    if (srcPath == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    const Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        path->AddPath(*srcPath, static_cast<PathAddMode>(mode));
        return;
    }
    path->AddPath(*srcPath, *matrix, static_cast<PathAddMode>(mode));
}

void OH_Drawing_PathAddPathWithMode(OH_Drawing_Path* cPath, const OH_Drawing_Path* src, OH_Drawing_PathAddMode mode)
{
    if (mode < PATH_ADD_MODE_APPEND || mode > PATH_ADD_MODE_EXTEND) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* srcPath = CastToPath(const_cast<OH_Drawing_Path*>(src));
    if (srcPath == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddPath(*srcPath, static_cast<PathAddMode>(mode));
}

void OH_Drawing_PathAddPathWithOffsetAndMode(OH_Drawing_Path* cPath,
    const OH_Drawing_Path* src, float dx, float dy, OH_Drawing_PathAddMode mode)
{
    if (mode < PATH_ADD_MODE_APPEND || mode > PATH_ADD_MODE_EXTEND) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* srcPath = CastToPath(const_cast<OH_Drawing_Path*>(src));
    if (srcPath == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddPath(*srcPath, dx, dy, static_cast<PathAddMode>(mode));
}

void OH_Drawing_PathAddOval(OH_Drawing_Path* cPath, const OH_Drawing_Rect* oval, OH_Drawing_PathDirection dir)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    if (oval == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddOval(CastToRect(*oval), static_cast<PathDirection>(dir));
}

void OH_Drawing_PathAddPolygon(OH_Drawing_Path* cPath, const OH_Drawing_Point2D* cPoints, uint32_t count, bool isClosed)
{
    if (cPoints == nullptr || count == 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    const Point* points = CastToPoint(cPoints);
    std::vector<Point> pointsTemp(count);
    for (uint32_t idx = 0; idx < count; idx++) {
        pointsTemp[idx] = points[idx];
    }
    path->AddPoly(pointsTemp, count, isClosed);
}

void OH_Drawing_PathAddCircle(OH_Drawing_Path* cPath, float x, float y, float radius, OH_Drawing_PathDirection dir)
{
    if (dir < PATH_DIRECTION_CW || dir > PATH_DIRECTION_CCW) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    if (radius <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->AddCircle(x, y, radius, static_cast<PathDirection>(dir));
}

bool OH_Drawing_PathBuildFromSvgString(OH_Drawing_Path* cPath, const char* str)
{
    if (str == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return path->BuildFromSVGString(str);
}

bool OH_Drawing_PathContains(OH_Drawing_Path* cPath, float x, float y)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return path->Contains(x, y);
}

void OH_Drawing_PathTransform(OH_Drawing_Path* cPath, const OH_Drawing_Matrix* matrix)
{
    if (matrix == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->Transform(*CastToMatrix(matrix));
}

void OH_Drawing_PathTransformWithPerspectiveClip(OH_Drawing_Path* cPath, const OH_Drawing_Matrix* cMatrix,
    OH_Drawing_Path* dstPath, bool applyPerspectiveClip)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    const Matrix* matrix = CastToMatrix(cMatrix);
    if (matrix == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->TransformWithPerspectiveClip(*matrix, CastToPath(dstPath), applyPerspectiveClip);
}

void OH_Drawing_PathSetFillType(OH_Drawing_Path* cPath, OH_Drawing_PathFillType fillstyle)
{
    if (fillstyle < PATH_FILL_TYPE_WINDING || fillstyle > PATH_FILL_TYPE_INVERSE_EVEN_ODD) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->SetFillStyle(static_cast<PathFillType>(fillstyle));
}

OH_Drawing_ErrorCode OH_Drawing_PathGetFillType(OH_Drawing_Path* cPath, OH_Drawing_PathFillType* pathFillType)
{
    if (cPath == nullptr || pathFillType == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    Path* path = CastToPath(cPath);
    PathFillType* type = reinterpret_cast<PathFillType*>(pathFillType);
    *type = path->GetFillStyle();
    return OH_DRAWING_SUCCESS;
}

void OH_Drawing_PathClose(OH_Drawing_Path* cPath)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->Close();
}

void OH_Drawing_PathOffset(OH_Drawing_Path* cPath, OH_Drawing_Path* dst, float dx, float dy)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Path* dstPath = CastToPath(const_cast<OH_Drawing_Path*>(dst));
    path->Offset(dstPath, dx, dy);
}

void OH_Drawing_PathReset(OH_Drawing_Path* cPath)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    path->Reset();
}

float OH_Drawing_PathGetLength(OH_Drawing_Path* cPath, bool forceClosed)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return -1;
    }
    return path->GetLength(forceClosed);
}

bool OH_Drawing_PathIsClosed(OH_Drawing_Path* cPath, bool forceClosed)
{
    Path* path = CastToPath(cPath);
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return path->IsClosed(forceClosed);
}

bool OH_Drawing_PathGetPositionTangent(OH_Drawing_Path* cPath, bool forceClosed,
    float distance, OH_Drawing_Point2D* cPosition, OH_Drawing_Point2D* cTangent)
{
    Path* path = CastToPath(cPath);
    Point* position = CastToPoint(cPosition);
    Point* tangent = CastToPoint(cTangent);
    if (path == nullptr || position == nullptr || tangent == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return path->GetPositionAndTangent(distance, *position, *tangent, forceClosed);
}

OH_Drawing_ErrorCode OH_Drawing_PathGetSegment(OH_Drawing_Path* cPath, bool forceClosed,
    float start, float stop, bool startWithMoveTo, OH_Drawing_Path* cDst, bool* result)
{
    Path* path = CastToPath(cPath);
    Path* dst = CastToPath(cDst);
    if (path == nullptr || dst == nullptr || result == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    float length = path->GetLength(forceClosed);
    if (start < 0) {
        start = 0;
    }
    if (stop > length) {
        stop = length;
    }
    if (start >= stop) {
        *result = false;
        return OH_DRAWING_SUCCESS;
    }
    *result = path->GetSegment(start, stop, dst, startWithMoveTo, forceClosed);
    return OH_DRAWING_SUCCESS;
}

bool OH_Drawing_PathOp(OH_Drawing_Path* cPath, const OH_Drawing_Path* src, OH_Drawing_PathOpMode op)
{
    if (op < PATH_OP_MODE_DIFFERENCE || op > PATH_OP_MODE_REVERSE_DIFFERENCE) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return false;
    }
    Path* path = CastToPath(cPath);
    Path* srcPath = CastToPath(const_cast<OH_Drawing_Path*>(src));
    if (path == nullptr || srcPath == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return path->Op(*path, *srcPath, static_cast<PathOp>(op));
}

bool OH_Drawing_PathGetMatrix(OH_Drawing_Path* cPath, bool forceClosed,
    float distance, OH_Drawing_Matrix* cMatrix, OH_Drawing_PathMeasureMatrixFlags flag)
{
    if (flag < GET_POSITION_MATRIX || flag > GET_POSITION_AND_TANGENT_MATRIX) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return false;
    }
    Path* path = CastToPath(cPath);
    Matrix* matrix = CastToMatrix(cMatrix);
    if (path == nullptr || matrix == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return path->GetMatrix(forceClosed, distance, matrix, static_cast<PathMeasureMatrixFlags>(flag));
}

void OH_Drawing_PathGetBounds(OH_Drawing_Path* cPath, OH_Drawing_Rect* cRect)
{
    Path* path = CastToPath(cPath);
    Rect* rect = CastToRect(cRect);
    if (path == nullptr || rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    *rect = path->GetBounds();
}