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

#include "imagefilter_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_color_filter.h"
#include "drawing_image_filter.h"


namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t TILE_MODE_ENUM_SIZE = 4;
} // namespace

namespace Drawing {
void NativeImageFilterTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    float sigmaX = GetObject<float>();
    float sigmaY = GetObject<float>();
    uint32_t tileMode = GetObject<uint32_t>();
    OH_Drawing_ImageFilter* imageFilter = OH_Drawing_ImageFilterCreateBlur(sigmaX, sigmaY,
        static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), nullptr);
    OH_Drawing_ImageFilter* imageFilterTwo = OH_Drawing_ImageFilterCreateFromColorFilter(colorFilter, imageFilter);
    OH_Drawing_ImageFilter* imageFilterThree = OH_Drawing_ImageFilterCreateFromColorFilter(nullptr, imageFilter);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_ImageFilterDestroy(imageFilter);
    OH_Drawing_ImageFilterDestroy(imageFilterTwo);
    OH_Drawing_ImageFilterDestroy(imageFilterThree);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeImageFilterTest(data, size);
    return 0;
}
