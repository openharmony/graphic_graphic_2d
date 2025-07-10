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
#include "samplingoptions_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_sampling_options.h"
#include "drawing_types.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t SAMPLING_OPTIONS_FILTER_MODE_ENUM_SIZE = 2;
constexpr size_t SAMPLING_OPTIONS_MIPMAP_MODE_ENUM_SIZE = 3;
} // namespace

namespace Drawing {
void NativeDrawingSamplingOptionsTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t filterMode = GetObject<uint32_t>();
    uint32_t mipMapMode = GetObject<uint32_t>();

    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        static_cast<OH_Drawing_FilterMode>(filterMode % SAMPLING_OPTIONS_FILTER_MODE_ENUM_SIZE),
        static_cast<OH_Drawing_MipmapMode>(mipMapMode % SAMPLING_OPTIONS_MIPMAP_MODE_ENUM_SIZE));

    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
}

void NativeDrawingSamplingOptionsTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t filterMode = GetObject<uint32_t>();
    uint32_t mipMapMode = GetObject<uint32_t>();

    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        static_cast<OH_Drawing_FilterMode>(filterMode % SAMPLING_OPTIONS_FILTER_MODE_ENUM_SIZE),
        static_cast<OH_Drawing_MipmapMode>(mipMapMode % SAMPLING_OPTIONS_MIPMAP_MODE_ENUM_SIZE));
    OH_Drawing_SamplingOptions* samplingOptionsCopy = OH_Drawing_SamplingOptionsCopy(samplingOptions);

    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_SamplingOptionsDestroy(samplingOptionsCopy);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingSamplingOptionsTest001(data, size);
    OHOS::Rosen::Drawing::NativeDrawingSamplingOptionsTest002(data, size);

    return 0;
}
