/*
 * Copyright (C) 2024-2025 Huawei Device Co., Ltd.
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

void NativeDrawingRegionTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    OH_Drawing_Path* path = OH_Drawing_PathCreate();

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    OH_Drawing_RegionGetBoundaryPath(nullptr, path);
    OH_Drawing_RegionGetBoundaryPath(firstRegion, nullptr);
    OH_Drawing_RegionGetBoundaryPath(firstRegion, path);
    OH_Drawing_RegionGetBoundaryPath(secondRegion, path);

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingRegionTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    OH_Drawing_RegionGetBounds(nullptr, secondRect);
    OH_Drawing_RegionGetBounds(firstRegion, nullptr);
    OH_Drawing_RegionGetBounds(secondRegion, secondRect);

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
}

void NativeDrawingRegionTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    bool isComplex = false;
    OH_Drawing_RegionIsComplex(nullptr, &isComplex);
    OH_Drawing_RegionIsComplex(firstRegion, nullptr);
    OH_Drawing_RegionIsComplex(firstRegion, &isComplex);
    OH_Drawing_RegionIsComplex(secondRegion, &isComplex);

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
}

void NativeDrawingRegionTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    bool isEmpty = false;
    OH_Drawing_RegionIsEmpty(nullptr, &isEmpty);
    OH_Drawing_RegionIsEmpty(firstRegion, nullptr);
    OH_Drawing_RegionIsEmpty(firstRegion, &isEmpty);
    OH_Drawing_RegionIsEmpty(secondRegion, &isEmpty);

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
}

void NativeDrawingRegionTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    bool isRect = false;
    OH_Drawing_RegionIsRect(nullptr, &isRect);
    OH_Drawing_RegionIsRect(firstRegion, nullptr);
    OH_Drawing_RegionIsRect(firstRegion, &isRect);
    OH_Drawing_RegionIsRect(secondRegion, &isRect);

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
}

void NativeDrawingRegionTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    bool isContains = false;
    OH_Drawing_RegionQuickContains(nullptr,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), &isContains);
    OH_Drawing_RegionQuickContains(firstRegion,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), nullptr);
    OH_Drawing_RegionQuickContains(firstRegion,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), &isContains);
    OH_Drawing_RegionQuickContains(secondRegion,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), &isContains);

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
}

void NativeDrawingRegionTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    bool isReject = false;
    OH_Drawing_RegionQuickReject(nullptr,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), &isReject);
    OH_Drawing_RegionQuickReject(firstRegion,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), nullptr);
    OH_Drawing_RegionQuickReject(firstRegion,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), &isReject);
    OH_Drawing_RegionQuickReject(secondRegion,
        GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(), &isReject);

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
}

void NativeDrawingRegionTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }

    OH_Drawing_Region* firstRegion = OH_Drawing_RegionCreate();
    float firstLeft = GetObject<float>();
    float firstTop = GetObject<float>();
    float firstRight = GetObject<float>();
    float firstBottom = GetObject<float>();
    OH_Drawing_Rect* firstRect = OH_Drawing_RectCreate(firstLeft, firstTop, firstRight, firstBottom);
    OH_Drawing_RegionSetRect(firstRegion, firstRect);

    OH_Drawing_Region* secondRegion = OH_Drawing_RegionCreate();
    float secondLeft = GetObject<float>();
    float secondTop = GetObject<float>();
    float secondRight = GetObject<float>();
    float secondBottom = GetObject<float>();
    OH_Drawing_Rect* secondRect = OH_Drawing_RectCreate(secondLeft, secondTop, secondRight, secondBottom);
    OH_Drawing_RegionSetRect(secondRegion, secondRect);

    uint32_t op = GetObject<uint32_t>();
    OH_Drawing_RegionOp(firstRegion, secondRegion, static_cast<OH_Drawing_RegionOpMode>(op % OPMODE_SIZE));

    OH_Drawing_RegionTranslate(nullptr, GetObject<int32_t>(), GetObject<int32_t>());
    OH_Drawing_RegionTranslate(firstRegion, GetObject<int32_t>(), GetObject<int32_t>());
    OH_Drawing_RegionTranslate(secondRegion, GetObject<int32_t>(), GetObject<int32_t>());

    OH_Drawing_RegionDestroy(firstRegion);
    OH_Drawing_RegionDestroy(secondRegion);
    OH_Drawing_RectDestroy(firstRect);
    OH_Drawing_RectDestroy(secondRect);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    //initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingRegionTest001(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest002(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest003(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest004(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest005(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest006(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest007(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest008(data, size);
    OHOS::Rosen::Drawing::NativeDrawingRegionTest009(data, size);
    return 0;
}
