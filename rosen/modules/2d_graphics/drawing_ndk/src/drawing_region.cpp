/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "drawing_region.h"

#include "drawing_canvas_utils.h"

#include "utils/region.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const Path* CastToPath(const OH_Drawing_Path* cPath)
{
    return reinterpret_cast<const Path*>(cPath);
}

static Path* CastToPath(OH_Drawing_Path* cPath)
{
    return reinterpret_cast<Path*>(cPath);
}

static const Region* CastToRegion(const OH_Drawing_Region* cRegion)
{
    return reinterpret_cast<const Region*>(cRegion);
}

static Region* CastToRegion(OH_Drawing_Region* cRegion)
{
    return reinterpret_cast<Region*>(cRegion);
}

static const Rect* CastToRect(const OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<const Rect*>(cRect);
}

static Rect* CastToRect(OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<Rect*>(cRect);
}

OH_Drawing_Region* OH_Drawing_RegionCreate()
{
    return (OH_Drawing_Region*)new Region();
}

OH_Drawing_Region* OH_Drawing_RegionCopy(const OH_Drawing_Region* region)
{
    if (region == nullptr) {
        return nullptr;
    }
    const Region* reg = reinterpret_cast<const Region*>(region);
    return (OH_Drawing_Region*)new Region(*reg);
}

bool OH_Drawing_RegionContains(OH_Drawing_Region* cRegion, int32_t x, int32_t y)
{
    Region* region = CastToRegion(cRegion);
    if (region == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return region->Contains(x, y);
}

bool OH_Drawing_RegionOp(OH_Drawing_Region* cRegion, const OH_Drawing_Region* cDst, OH_Drawing_RegionOpMode op)
{
    Region* region = CastToRegion(cRegion);
    Region* dst = CastToRegion(const_cast<OH_Drawing_Region*>(cDst));
    if (region == nullptr || dst == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    if (op < REGION_OP_MODE_DIFFERENCE || op > REGION_OP_MODE_REPLACE) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return false;
    }
    return region->Op(*dst, static_cast<RegionOp>(op));
}

bool OH_Drawing_RegionSetRect(OH_Drawing_Region* cRegion, const OH_Drawing_Rect* cRect)
{
    const Rect* rect = CastToRect(cRect);
    Region* region = CastToRegion(cRegion);
    if (region == nullptr || rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    int left = rect->GetLeft();
    int right = rect->GetRight();
    int top = rect->GetTop();
    int bottom = rect->GetBottom();
    RectI rectI(left, top, right, bottom);
    return region->SetRect(rectI);
}

bool OH_Drawing_RegionSetPath(OH_Drawing_Region* cRegion, const OH_Drawing_Path* cPath, const OH_Drawing_Region* cClip)
{
    Region* region = CastToRegion(cRegion);
    const Path* path = CastToPath(cPath);
    Region* clip = CastToRegion(const_cast<OH_Drawing_Region*>(cClip));
    if (region == nullptr || path == nullptr || clip == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return region->SetPath(*path, *clip);
}

void OH_Drawing_RegionDestroy(OH_Drawing_Region* cRegion)
{
    if (cRegion == nullptr) {
        return;
    }
    delete CastToRegion(cRegion);
}

// LCOV_EXCL_START
OH_Drawing_ErrorCode OH_Drawing_RegionEmpty(OH_Drawing_Region* cRegion)
{
    if (cRegion == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    Region* region = CastToRegion(cRegion);

    region->SetEmpty();
    return OH_DRAWING_SUCCESS;
}
// LCOV_EXCL_STOP

OH_Drawing_ErrorCode OH_Drawing_RegionGetBoundaryPath(const OH_Drawing_Region* cRegion, OH_Drawing_Path* cPath)
{
    const Region* region = CastToRegion(cRegion);
    Path* path = CastToPath(cPath);
    if (region == nullptr || path == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    *path = Path();
    region->GetBoundaryPath(path);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RegionGetBounds(const OH_Drawing_Region* cRegion, OH_Drawing_Rect* cRect)
{
    const Region* region = CastToRegion(cRegion);
    Rect* rect = CastToRect(cRect);
    if (region == nullptr || rect == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    RectI rectI = region->GetBounds();
    rect->SetLeft(rectI.GetLeft());
    rect->SetTop(rectI.GetTop());
    rect->SetRight(rectI.GetRight());
    rect->SetBottom(rectI.GetBottom());
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RegionIsComplex(const OH_Drawing_Region* cRegion, bool* isComplex)
{
    const Region* region = CastToRegion(cRegion);
    if (region == nullptr || isComplex == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    *isComplex = region->IsComplex();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RegionIsEmpty(const OH_Drawing_Region* cRegion, bool* isEmpty)
{
    const Region* region = CastToRegion(cRegion);
    if (region == nullptr || isEmpty == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    *isEmpty = region->IsEmpty();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RegionIsRect(const OH_Drawing_Region* cRegion, bool* isRect)
{
    const Region* region = CastToRegion(cRegion);
    if (region == nullptr || isRect == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    *isRect = region->IsRect();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RegionQuickContains(const OH_Drawing_Region* cRegion,
    int32_t left, int32_t top, int32_t right, int32_t bottom, bool* isContained)
{
    const Region* region = CastToRegion(cRegion);
    if (region == nullptr || isContained == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    *isContained = region->QuickContains(RectI{left, top, right, bottom});
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RegionQuickReject(const OH_Drawing_Region* cRegion,
    int32_t left, int32_t top, int32_t right, int32_t bottom, bool* isReject)
{
    const Region* region = CastToRegion(cRegion);
    if (region == nullptr || isReject == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    *isReject = region->QuickReject(RectI{left, top, right, bottom});
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_RegionTranslate(OH_Drawing_Region* cRegion, int32_t dx, int32_t dy)
{
    Region* region = CastToRegion(cRegion);
    if (region == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    region->Translate(dx, dy);
    return OH_DRAWING_SUCCESS;
}