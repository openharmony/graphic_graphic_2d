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

#include "point3_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/point3.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void Point3FuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    scalar z = GetObject<scalar>();
    scalar scale = GetObject<scalar>();
    scalar divisor = GetObject<scalar>();

    Point3 point3;
    Point3 point3_1 = Point3(x, y, z);
    Point3 point3_2 = Point3(point3_1);

    point3.SetX(x);
    point3.GetX();
    point3.SetY(y);
    point3.GetY();
    point3.SetZ(z);
    point3.GetZ();

    point3 += point3_1;
    point3 -= point3_1;
    point3 *= scale;
    point3 /= divisor;

    point3 = point3_1 + point3_2;
    point3 = point3_1 - point3_2;
    point3 = scale * point3_2;
    point3 = point3_1 * scale;
    point3 = point3_1 / divisor;

    if (point3 == point3_1) {}
    if (point3 != point3_1) {}
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::Point3FuzzTest000(data, size);
    return 0;
}