/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "pathiterator_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "draw/path_iterator.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MIN_ARRAY_SIZE = 4;
} // namespace
namespace Drawing {
/*
 * 测试以下 PathIterator 接口：
 * 1. PathIterator(const Path&)
 * 2. Next(...)
 * 3. Peek()
 */
bool PathIteratorFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    PathIterator iter(path);
    uint32_t count = GetObject<uint32_t>() % MIN_ARRAY_SIZE + MIN_ARRAY_SIZE;
    Point points[count];
    auto verb = iter.Next(points);
    auto verb2 = iter.Peek();

    return (verb == verb2);
}

/*
 * 测试以下 PathIter 接口：
 * 1. PathIter(...)
 * 2. ConicWeight()
 * 3. Next(...)
 */
bool PathIterFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    path.MoveTo(GetObject<float>(), GetObject<float>());
    path.LineTo(GetObject<float>(), GetObject<float>());
    path.QuadTo(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    path.CubicTo(GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    PathIter iter(path, GetObject<bool>());
    uint32_t count = GetObject<uint32_t>() % MIN_ARRAY_SIZE + MIN_ARRAY_SIZE;
    Point points[count];
    PathVerb verb;
    do {
        verb = iter.Next(points);
    } while (verb != PathVerb::CONIC && verb != PathVerb::DONE);
    if (verb == PathVerb::CONIC) {
        iter.ConicWeight();
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PathIteratorFuzzTest001(data, size);
    OHOS::Rosen::Drawing::PathIterFuzzTest001(data, size);
    return 0;
}