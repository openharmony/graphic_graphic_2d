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

#include "sampling_options_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/sampling_options.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t MATH_TWO = 2;
    constexpr uint32_t MATH_THREE = 3;
} // namespace

void SamplingOptionsFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t mm = GetObject<uint32_t>();
    uint32_t fm = GetObject<uint32_t>();

    SamplingOptions sam;
    SamplingOptions sam1 = SamplingOptions(static_cast<FilterMode>(fm % MATH_TWO));
    SamplingOptions sam2 = SamplingOptions(static_cast<FilterMode>(fm % MATH_TWO),
        static_cast<MipmapMode>(mm % MATH_THREE));
    CubicResampler c = CubicResampler::CatmullRom();
    SamplingOptions sam3 = SamplingOptions(c);
    sam.GetUseCubic();
    sam.GetFilterMode();
    sam.GetMipmapMode();
    sam.GetCubicCoffB();
    sam.GetCubicCoffC();

    if (sam1 == sam2) {}
    if (sam1 != sam2) {}
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::SamplingOptionsFuzzTest000(data, size);

    return 0;
}