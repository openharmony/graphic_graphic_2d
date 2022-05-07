/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "path_interpolate_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "draw/path.h"

const int FUZZ_DATA_LEN = 0;

namespace OHOS {
namespace Rosen {
namespace Drawing {
    bool PathInterpolateFuzzTest(const uint8_t* data, size_t size)
    {
        bool result = false;
        Path path;
        Path ending;
        Path out;
        if (size > FUZZ_DATA_LEN) {
            return path.Interpolate(ending, reinterpret_cast<const uint32_t>(data), out);
        }
        return result;
    }
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PathInterpolateFuzzTest(data, size);
    return 0;
}