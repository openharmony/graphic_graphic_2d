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

#include "data_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/data.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ARRAY_MAX_SIZE = 5000;
} // namespace

namespace Drawing {
/*
 * 测试以下 Data 接口：
 * 1. BuildWithCopy
 * 2. BuildWithoutCopy
 * 3. BuildUninitialized
 * 4. BuildEmpty
 */
void DataFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto data1 = std::make_unique<Data>();
    size_t length = GetObject<size_t>() % ARRAY_MAX_SIZE + 1;
    char* textData = new char[length];
    for (size_t i = 0; i < length; i++) {
        textData[i] = GetObject<char>();
    }
    textData[length - 1] = '\0';
    data1->BuildWithCopy(textData, length);
    data1->BuildWithoutCopy(textData, length);
    data1->BuildUninitialized(length);
    data1->BuildEmpty();
    if (textData != nullptr) {
        delete [] textData;
        textData = nullptr;
    }
}

/*
 * 测试以下 Data 接口：
 * 1. MakeFromFileName
 * 2. WritableData
 * 3. GetSize
 * 4. GetData
 * 5. Serialize
 * 6. BuildFromMalloc
 */
void DataFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    Data data1;
    uint32_t str_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE + 1;
    char path[str_size];
    for (size_t i = 0; i < str_size; i++) {
        path[i] = GetObject<char>();
    }
    path[str_size - 1] = '\0';
    data1.MakeFromFileName(path);
    data1.WritableData();
    data1.GetSize();
    data1.GetData();
    data1.Serialize();
    char* textData = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        textData[i] = GetObject<char>();
    }
    textData[str_size - 1] = '\0';
    data1.BuildFromMalloc(textData, str_size);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::DataFuzzTest000(data, size);
    OHOS::Rosen::Drawing::DataFuzzTest001(data, size);
    return 0;
}