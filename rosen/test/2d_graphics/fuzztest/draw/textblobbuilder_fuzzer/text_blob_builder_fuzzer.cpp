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

#include "text_blob_builder_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/text_blob_builder.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
}
namespace Drawing {
/*
 * 测试以下 TextBlobBuilder 接口：
 * 1. AllocRunPos(const Font& font, int count, const Rect* bounds)
 * 2. Make()
 */
bool TextBlobBuilderFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    TextBlobBuilder builder;
    Font font;
    Rect rect {GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()};
    builder.AllocRunPos(font, GetObject<int>() % MAX_SIZE, &rect);
    builder.Make();
    return true;
}

/*
 * 测试以下 TextBlobBuilder 接口：
 * 1. AllocRunRSXform(const Font& font, int count)
 */
bool TextBlobBuilderFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    TextBlobBuilder builder;
    Font font;
    int count = GetObject<int>() % MAX_SIZE;
    builder.AllocRunRSXform(font, count);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::TextBlobBuilderFuzzTest001(data, size);
    OHOS::Rosen::Drawing::TextBlobBuilderFuzzTest002(data, size);
    return 0;
}
