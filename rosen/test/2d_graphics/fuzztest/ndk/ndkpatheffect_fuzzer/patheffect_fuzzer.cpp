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

#include "patheffect_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_path.h"
#include "drawing_path_effect.h"
#include "drawing_types.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr uint32_t MAX_ARRAY_SIZE = 5000;
constexpr size_t PATH_EFFECT_TYPE_ENUM_SIZE = 3;
} // namespace

namespace Drawing {
void NativePathEffectTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t number = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    float* intervals = new float[number];
    for (size_t i = 0; i < number; i++) {
        intervals[i] = GetObject<float>();
    }

    float phase = GetObject<float>();
    OH_Drawing_PathEffect* pathEffectError = OH_Drawing_CreateDashPathEffect(nullptr, number, phase);
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, number, phase);
    if (intervals != nullptr) {
        delete[] intervals;
        intervals = nullptr;
    }

    OH_Drawing_PathEffectDestroy(pathEffectError);
    OH_Drawing_PathEffectDestroy(pathEffect);
}


void NativePathEffectTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float x = GetObject<float>();
    float y = GetObject<float>();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, x, y);
    OH_Drawing_PathLineTo(path, x, y);
    float advance = GetObject<float>();
    float phase = GetObject<float>();
    OH_Drawing_PathDashStyle type =
        static_cast<OH_Drawing_PathDashStyle>(GetObject<uint32_t>() % PATH_EFFECT_TYPE_ENUM_SIZE);

    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreatePathDashEffect(path, advance, phase, type);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathEffectDestroy(pathEffect);
}

void NativePathEffectTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t numberOne = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    float phaseOne = GetObject<float>();
    float* intervalsOne = new float[numberOne];
    for (size_t i = 0; i < numberOne; i++) {
        intervalsOne[i] = GetObject<float>();
    }
    OH_Drawing_PathEffect* pathEffectOne = OH_Drawing_CreateDashPathEffect(intervalsOne, numberOne, phaseOne);
    uint32_t numberTwo = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    float phaseTwo = GetObject<float>();
    float* intervalsTwo = new float[numberTwo];
    for (size_t i = 0; i < numberTwo; i++) {
        intervalsTwo[i] = GetObject<float>();
    }
    OH_Drawing_PathEffect* pathEffectTwo = OH_Drawing_CreateDashPathEffect(intervalsTwo, numberTwo, phaseTwo);
    OH_Drawing_PathEffect* pathEffectResult = OH_Drawing_CreateSumPathEffect(pathEffectOne, pathEffectTwo);
    if (intervalsOne != nullptr) {
        delete[] intervalsOne;
        intervalsOne = nullptr;
    }
    if (intervalsTwo != nullptr) {
        delete[] intervalsTwo;
        intervalsTwo = nullptr;
    }
    OH_Drawing_PathEffectDestroy(pathEffectOne);
    OH_Drawing_PathEffectDestroy(pathEffectTwo);
    OH_Drawing_PathEffectDestroy(pathEffectResult);
}

void NativePathEffectTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float segLength = GetObject<float>();
    float deviation = GetObject<float>();

    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDiscretePathEffect(segLength, deviation);
    OH_Drawing_PathEffectDestroy(pathEffect);
}

void NativePathEffectTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float radius = GetObject<float>();

    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateCornerPathEffect(radius);
    OH_Drawing_PathEffectDestroy(pathEffect);
}

void NativePathEffectTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float radiusOne = GetObject<float>();
    OH_Drawing_PathEffect* pathEffect1 = OH_Drawing_CreateCornerPathEffect(radiusOne);
    float radiusTwo = GetObject<float>();
    OH_Drawing_PathEffect* pathEffect2 = OH_Drawing_CreateCornerPathEffect(radiusTwo);

    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateComposePathEffect(pathEffect1, pathEffect2);
    OH_Drawing_PathEffectDestroy(pathEffect1);
    OH_Drawing_PathEffectDestroy(pathEffect2);
    OH_Drawing_PathEffectDestroy(pathEffect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativePathEffectTest001(data, size);
    OHOS::Rosen::Drawing::NativePathEffectTest002(data, size);
    OHOS::Rosen::Drawing::NativePathEffectTest003(data, size);
    OHOS::Rosen::Drawing::NativePathEffectTest004(data, size);
    OHOS::Rosen::Drawing::NativePathEffectTest005(data, size);
    OHOS::Rosen::Drawing::NativePathEffectTest006(data, size);
    return 0;
}
