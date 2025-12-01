/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "lattice_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_lattice.h"
#include "drawing_rect.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_DIV_COUNT = 10;
constexpr size_t MAX_RECT_TYPE_ENUM = 3;
} // namespace

namespace Drawing {

void NativeDrawingLatticeTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Lattice* lattice = nullptr;
    uint32_t xCount = GetObject<uint32_t>() % MAX_DIV_COUNT;
    uint32_t yCount = GetObject<uint32_t>() % MAX_DIV_COUNT;
    int* xDivs = new int[xCount];
    int* yDivs = new int[yCount];
    for (uint32_t i = 0; i < xCount; i++) {
        xDivs[i] = GetObject<int32_t>();
    }
    for (uint32_t i = 0; i < yCount; i++) {
        yDivs[i] = GetObject<int32_t>();
    }
    OH_Drawing_Rect *rect =
        OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    uint32_t rectTypeCount = GetObject<uint32_t>() % (MAX_DIV_COUNT * MAX_DIV_COUNT);
    uint32_t colorCount = GetObject<uint32_t>() % (MAX_DIV_COUNT * MAX_DIV_COUNT);
    OH_Drawing_LatticeRectType* rectTypes = new OH_Drawing_LatticeRectType[rectTypeCount];
    uint32_t* colors = new uint32_t[colorCount];
    for (uint32_t i = 0; i < rectTypeCount; i++) {
        rectTypes[i] = static_cast<OH_Drawing_LatticeRectType>(GetObject<uint32_t>() % (MAX_RECT_TYPE_ENUM + 1));
    }
    for (uint32_t i = 0; i < colorCount; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    OH_Drawing_LatticeCreate(xDivs, yDivs, xCount, yCount, GetObject<bool>() ? rect : nullptr,
        rectTypes, rectTypeCount, colors, colorCount, &lattice);
    OH_Drawing_LatticeDestroy(lattice);
    OH_Drawing_RectDestroy(rect);
    delete[] xDivs;
    delete[] yDivs;
    delete[] rectTypes;
    delete[] colors;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingLatticeTest(data, size);
    return 0;
}
