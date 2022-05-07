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

#include "path_op_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "draw/path.h"

const int FUZZ_DATA_LEN = 0;
const int CONSTANTS_NUMBER_FIVE = 5;

namespace OHOS {
namespace Rosen {
namespace Drawing {
    bool PathOpFuzzTest(const uint8_t* data, size_t size)
    {
        bool result = false;
        Path path;
        PathOp pathOp = static_cast<PathOp>(size % CONSTANTS_NUMBER_FIVE);
        Path path1;
        Path path2;
        if (size > FUZZ_DATA_LEN) {
            return path.Op(path1, path2, pathOp);
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
    OHOS::Rosen::Drawing::PathOpFuzzTest(data, size);
    return 0;
}

