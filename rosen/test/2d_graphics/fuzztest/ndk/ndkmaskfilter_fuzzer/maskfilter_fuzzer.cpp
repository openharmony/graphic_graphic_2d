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

#include "maskfilter_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_mask_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t BLUR_TYPE_ENUM_SIZE = 4;
} // namespace

namespace Drawing {

void MaskFilterTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t blurType = GetObject<uint32_t>();
    float sigma = GetObject<float>();
    bool respectCTM = GetObject<bool>();
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(
        static_cast<OH_Drawing_BlurType>(blurType % BLUR_TYPE_ENUM_SIZE), sigma, respectCTM);
    OH_Drawing_MaskFilterDestroy(maskFilter);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::MaskFilterTest(data, size);
    return 0;
}
