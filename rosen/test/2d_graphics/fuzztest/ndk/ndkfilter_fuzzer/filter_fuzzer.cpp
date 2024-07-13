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

#include "filter_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_color_filter.h"
#include "drawing_filter.h"
#include "drawing_image_filter.h"
#include "drawing_mask_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t TYPE_ENUM_SIZE = 4;
} // namespace

namespace Drawing {

void FilterTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    float sigmaX = GetObject<float>();
    float sigmaY = GetObject<float>();
    uint32_t tileMode = GetObject<uint32_t>();
    OH_Drawing_ImageFilter* imageFilter = OH_Drawing_ImageFilterCreateBlur(sigmaX, sigmaY,
        static_cast<OH_Drawing_TileMode>(tileMode % TYPE_ENUM_SIZE), nullptr);
    OH_Drawing_FilterSetImageFilter(filter, imageFilter);
    OH_Drawing_FilterSetImageFilter(nullptr, imageFilter);
    OH_Drawing_FilterSetImageFilter(filter, nullptr);
    uint32_t blurType = GetObject<uint32_t>();
    float sigma = GetObject<float>();
    bool respectCTM = GetObject<bool>();
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(
        static_cast<OH_Drawing_BlurType>(blurType % TYPE_ENUM_SIZE), sigma, respectCTM);
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    OH_Drawing_FilterSetMaskFilter(nullptr, maskFilter);
    OH_Drawing_FilterSetMaskFilter(filter, nullptr);
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    OH_Drawing_FilterSetColorFilter(filter, nullptr);
    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_FilterSetColorFilter(nullptr, colorFilter);
    OH_Drawing_ColorFilter* colorFilterTwo = nullptr;
    OH_Drawing_FilterGetColorFilter(filter, colorFilterTwo);
    OH_Drawing_FilterGetColorFilter(nullptr, colorFilterTwo);
    OH_Drawing_FilterGetColorFilter(filter, nullptr);

    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_ImageFilterDestroy(imageFilter);
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_ColorFilterDestroy(colorFilter);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::FilterTest(data, size);
    return 0;
}
