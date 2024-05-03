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

#include "drawing_region.h"

#include "utils/region.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const Path* CastToPath(const OH_Drawing_Path* cPath)
{
    return reinterpret_cast<const Path*>(cPath);
}

static Region* CastToRegion(OH_Drawing_Region* cRegion)
{
    return reinterpret_cast<Region*>(cRegion);
}

static const Rect* CastToRect(const OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<const Rect*>(cRect);
}

OH_Drawing_Region* OH_Drawing_RegionCreate()
{
    return (OH_Drawing_Region*)new Region();
}

bool OH_Drawing_RegionContains(OH_Drawing_Region* cRegion, int32_t x, int32_t y)
{
    Region* region = CastToRegion(cRegion);
    if (region == nullptr) {
        return false;
    }
    return region->Contains(x, y);
}

bool OH_Drawing_RegionOp(OH_Drawing_Region* cRegion, const OH_Drawing_Region* cDst, OH_Drawing_RegionOpMode op)
{
    Region* region = CastToRegion(cRegion);
    Region* dst = CastToRegion(const_cast<OH_Drawing_Region*>(cDst));
    if (region == nullptr || dst == nullptr) {
        return false;
    }
    return region->Op(*dst, static_cast<RegionOp>(op));
}

bool OH_Drawing_RegionSetRect(OH_Drawing_Region* cRegion, const OH_Drawing_Rect* cRect)
{
    const Rect* rect = CastToRect(cRect);
    Region* region = CastToRegion(cRegion);
    if (region == nullptr || rect == nullptr) {
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
