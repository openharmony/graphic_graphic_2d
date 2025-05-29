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

#include "ndkmemorystream_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_memory_stream.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {

void NativeDrawingMemoryStreamTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool copyData = GetObject<bool>();
    uint32_t str_size = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    char* str = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        str[i] = GetObject<char>();
    }

    OH_Drawing_MemoryStream* memoryStream = OH_Drawing_MemoryStreamCreate(str, str_size, copyData);
    OH_Drawing_MemoryStreamCreate(nullptr, 0, copyData);

    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }

    OH_Drawing_MemoryStreamDestroy(memoryStream);
    OH_Drawing_MemoryStreamDestroy(nullptr);
}


} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingMemoryStreamTest(data, size);
    return 0;
}
