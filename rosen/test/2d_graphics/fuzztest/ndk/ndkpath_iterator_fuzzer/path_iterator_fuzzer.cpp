/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "path_iterator_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_path.h"
#include "drawing_path_iterator.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_POINT_SIZE = 40;
constexpr size_t POINT_STEPS = 4;
} // namespace

namespace Drawing {

void NativeDrawingPathIteratorTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathLineTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathQuadTo(path, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_PathConicTo(path, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_PathCubicTo(path, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>(), GetObject<float>());
    OH_Drawing_PathClose(path);
    OH_Drawing_PathIterator* pathIterator = nullptr;
    OH_Drawing_PathIteratorCreate(path, &pathIterator);

    bool hasNext = false;
    uint32_t offset = 0;
    OH_Drawing_PathIteratorVerb verb = OH_Drawing_PathIteratorVerb::MOVE;
    OH_Drawing_Point2D point2d[MAX_POINT_SIZE] = {{0, 0}};
    OH_Drawing_PathIteratorHasNext(pathIterator, &hasNext);
    while (hasNext) {
        OH_Drawing_PathIteratorPeek(pathIterator, &verb);
        OH_Drawing_PathIteratorNext(pathIterator, point2d, MAX_POINT_SIZE, offset, &verb);
        OH_Drawing_PathIteratorHasNext(pathIterator, &hasNext);
        offset += POINT_STEPS;
    }

    OH_Drawing_PathIteratorDestroy(pathIterator);
    OH_Drawing_PathDestroy(path);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingPathIteratorTest(data, size);
    return 0;
}
