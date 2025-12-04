/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_lattice.h"

#include "draw/canvas.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const Rect* CastToRect(const OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<const Rect*>(cRect);
}

void GetLatticeDividers(const int* divs, uint32_t count, std::vector<int>& dividers)
{
    if (count != 0) {
        dividers.reserve(count);
        for (uint32_t i = 0; i < count; i++) {
            dividers.push_back(divs[i]);
        }
    }
}

void GetLatticeRectTypes(const OH_Drawing_LatticeRectType* cRectTypes, uint32_t count,
    std::vector<Lattice::RectType>& latticeRectTypes)
{
    latticeRectTypes.reserve(count);
    for (uint32_t i = 0; i < count; i++) {
        latticeRectTypes.push_back(static_cast<Lattice::RectType>(cRectTypes[i]));
    }
}

void GetLatticeColors(const uint32_t* colors, uint32_t count, std::vector<Color>& latticeColors)
{
    latticeColors.reserve(count);
    for (uint32_t i = 0; i < count; i++) {
        Drawing::Color drawingColor;
        drawingColor.SetColorQuad(colors[i]);
        latticeColors.push_back(drawingColor);
    }
}

OH_Drawing_ErrorCode OH_Drawing_LatticeCreate(const int* xDivs, const int* yDivs, uint32_t xCount, uint32_t yCount,
    const OH_Drawing_Rect* cBounds, const OH_Drawing_LatticeRectType* cRectTypes, uint32_t rectTypeCount,
    const uint32_t* colors, uint32_t colorCount, OH_Drawing_Lattice** cLattice)
{
    if (xDivs == nullptr || yDivs == nullptr || xCount > 5 || yCount > 5 || cLattice == nullptr) { // 5 is max div size
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (cRectTypes == nullptr && rectTypeCount != 0) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (colors == nullptr && colorCount != 0) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (cRectTypes != nullptr && rectTypeCount != (xCount + 1) * (yCount + 1)) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (colors != nullptr && colorCount != (xCount + 1) * (yCount + 1)) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (cRectTypes != nullptr) {
        for (uint32_t i = 0; i < rectTypeCount; i++) {
            if (static_cast<uint32_t>(cRectTypes[i]) > static_cast<uint32_t>(OH_Drawing_LatticeRectType::FIXED_COLOR)) {
                return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
            }
        }
    }
    Lattice* lattice = new Lattice();
    GetLatticeDividers(xDivs, xCount, lattice->fXDivs);
    GetLatticeDividers(yDivs, yCount, lattice->fYDivs);
    lattice->fXCount = xCount;
    lattice->fYCount = yCount;
    const Rect* bounds = CastToRect(cBounds);
    if (bounds != nullptr) {
        lattice->fBounds.push_back(RectI(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom()));
    }
    if (cRectTypes != nullptr) {
        GetLatticeRectTypes(cRectTypes, rectTypeCount, lattice->fRectTypes);
    }
    if (colors != nullptr) {
        GetLatticeColors(colors, colorCount, lattice->fColors);
    }
    *cLattice = reinterpret_cast<OH_Drawing_Lattice*>(lattice);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_LatticeDestroy(OH_Drawing_Lattice* lattice)
{
    if (lattice == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    delete reinterpret_cast<Lattice*>(lattice);
    return OH_DRAWING_SUCCESS;
}