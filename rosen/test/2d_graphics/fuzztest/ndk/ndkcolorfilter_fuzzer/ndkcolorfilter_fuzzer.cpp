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

#include "ndkcolorfilter_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_color_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MATRIX_SIZE = 20;
constexpr size_t BLENDMODE_SIZE = 29;
} // namespace

namespace Drawing {

void NativeDrawingColorFilterTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetObject<uint32_t>();
    uint32_t mode = GetObject<uint32_t>();
    OH_Drawing_ColorFilter* colorFilterOne = OH_Drawing_ColorFilterCreateBlendMode(color,
        static_cast<OH_Drawing_BlendMode>(mode % BLENDMODE_SIZE));
    OH_Drawing_ColorFilter* colorFilterTwo = OH_Drawing_ColorFilterCreateBlendMode(color,
        static_cast<OH_Drawing_BlendMode>(mode % BLENDMODE_SIZE));
    OH_Drawing_ColorFilter* colorFilterThree = OH_Drawing_ColorFilterCreateCompose(nullptr, colorFilterTwo);
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateCompose(colorFilterOne, colorFilterTwo);

    OH_Drawing_ColorFilterDestroy(colorFilterOne);
    OH_Drawing_ColorFilterDestroy(colorFilterTwo);
    OH_Drawing_ColorFilterDestroy(colorFilterThree);
    OH_Drawing_ColorFilterDestroy(colorFilter);
}

void NativeDrawingColorFilterTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_ColorFilter* colorFilterOne = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    OH_Drawing_ColorFilter* colorFilterTwo = OH_Drawing_ColorFilterCreateSrgbGammaToLinear();
    OH_Drawing_ColorFilter* colorFilterThree = OH_Drawing_ColorFilterCreateLuma();

    OH_Drawing_ColorFilterDestroy(colorFilterOne);
    OH_Drawing_ColorFilterDestroy(colorFilterTwo);
    OH_Drawing_ColorFilterDestroy(colorFilterThree);
}

void NativeDrawingColorFilterTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float matrix[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        matrix[i] = GetObject<float>();
    }
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateMatrix(matrix);
    OH_Drawing_ColorFilter* colorFilterOne = OH_Drawing_ColorFilterCreateMatrix(nullptr);

    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_ColorFilterDestroy(colorFilterOne);
}

void NativeDrawingColorFilterTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t mulColor = GetObject<uint32_t>();
    uint32_t addColor = GetObject<uint32_t>();
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateLighting(mulColor, addColor);

    OH_Drawing_ColorFilterDestroy(colorFilter);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingColorFilterTest001(data, size);
    OHOS::Rosen::Drawing::NativeDrawingColorFilterTest002(data, size);
    OHOS::Rosen::Drawing::NativeDrawingColorFilterTest003(data, size);
    OHOS::Rosen::Drawing::NativeDrawingColorFilterTest004(data, size);

    return 0;
}
