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
#include "point_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_point.h"
#include "drawing_types.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
} // namespace

namespace Drawing {
void NativeDrawingPointTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float x = GetObject<float>();
    float y = GetObject<float>();

    OH_Drawing_Point* point = OH_Drawing_PointCreate(x, y);

    OH_Drawing_PointGetX(point, &x);
    OH_Drawing_PointGetX(nullptr, &x);

    OH_Drawing_PointGetY(point, &y);
    OH_Drawing_PointGetY(nullptr, &y);

    OH_Drawing_PointSet(point, x, y);
    OH_Drawing_PointSet(nullptr, x, y);

    OH_Drawing_PointDestroy(point);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingPointTest001(data, size);

    return 0;
}
