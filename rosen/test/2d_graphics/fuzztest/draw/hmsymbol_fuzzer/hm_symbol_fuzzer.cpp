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

#include "hm_symbol_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/hm_symbol.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool HmSymbolFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    DrawingHMSymbol hmSymbol;

    Path path;
    Path path1;
    Path path2;
    std::vector<Path> paths { path1, path2 };
    hmSymbol.PathOutlineDecompose(path, paths);

    size_t v1 = GetObject<size_t>();
    size_t v2 = GetObject<size_t>();
    size_t v3 = GetObject<size_t>();
    size_t v4 = GetObject<size_t>();
    std::vector<std::vector<size_t>> multMap { {v1, v2}, {v3, v4} };
    Path path3;
    Path path4;
    std::vector<Path> multPaths {path3, path4};
    hmSymbol.MultilayerPath(multMap, paths, multPaths);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::HmSymbolFuzzTest(data, size);
    return 0;
}
