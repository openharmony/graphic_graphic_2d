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

#include "c/drawing_path.h"

#include "draw/path.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Path* CastToPath(OH_Drawing_Path* cPath)
{
    return reinterpret_cast<Path*>(cPath);
}

OH_Drawing_Path* OH_Drawing_PathCreate()
{
    return (OH_Drawing_Path*)new Path;
}

void OH_Drawing_PathDestory(OH_Drawing_Path* cPath)
{
    delete CastToPath(cPath);
}

void OH_Drawing_PathMoveTo(OH_Drawing_Path* cPath, float x, float y)
{
    CastToPath(cPath)->MoveTo(x, y);
}

void OH_Drawing_PathLineTo(OH_Drawing_Path* cPath, float x, float y)
{
    CastToPath(cPath)->LineTo(x, y);
}

void OH_Drawing_PathArcTo(
    OH_Drawing_Path* cPath, float x1, float y1, float x2, float y2, float startDeg, float sweepDeg)
{
    CastToPath(cPath)->ArcTo(x1, y1, x2, y2, startDeg, sweepDeg);
}

void OH_Drawing_PathQuadTo(OH_Drawing_Path* cPath, float ctrlX, float ctrlY, float endX, float endY)
{
    CastToPath(cPath)->QuadTo(ctrlX, ctrlY, endX, endY);
}

void OH_Drawing_PathCubicTo(
    OH_Drawing_Path* cPath, float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2, float endX, float endY)
{
    CastToPath(cPath)->CubicTo(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
}

void OH_Drawing_PathClose(OH_Drawing_Path* cPath)
{
    CastToPath(cPath)->Close();
}

void OH_Drawing_PathReset(OH_Drawing_Path* cPath)
{
    CastToPath(cPath)->Reset();
}
