/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "ndkregion_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_path.h"
#include "drawing_rect.h"
#include "drawing_region.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t OPMODE_SIZE = 6;
} // namespace

namespace Drawing {

void NativeDrawingRegionTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    OH_Drawing_RegionContains(nullptr, x, y);
    OH_Drawing_RegionContains(region, x, y);

    OH_Drawing_Region* dst = OH_Drawing_RegionCreate();
    OH_Drawing_Region* region2 = OH_Drawing_RegionCopy(dst);
    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(nullptr, dst, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));
    OH_Drawing_RegionOp(region, dst, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_RegionSetRect(nullptr, rect);
    OH_Drawing_RegionSetRect(region, rect);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_RegionSetPath(nullptr, path, dst);
    OH_Drawing_RegionSetPath(region, path, dst);

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RegionDestroy(dst);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RegionDestroy(nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingRegionTest001(data, size);
    return 0;
}
