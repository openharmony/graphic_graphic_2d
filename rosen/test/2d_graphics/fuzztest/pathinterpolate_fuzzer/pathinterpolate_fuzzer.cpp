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

#include "pathinterpolate_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "draw/path.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (data == nullptr || objectSize > size) {
        return 0;
    }
    auto ret = memcpy_s(&object, objectSize, data, objectSize);
    if (ret != EOK) {
        return 0;
    }
    return objectSize;
}

bool PathInterpolateFuzzTest(const uint8_t* data, size_t size)
{
    scalar weight;
    if (data == nullptr || size < sizeof(scalar)) {
        return false;
    }
    Path path;
    Path ending;
    Path out;
    size_t startPos = 0;
    GetObject<scalar>(weight, data + startPos, size - startPos);
    path.Interpolate(ending, weight, out);
    return true;
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