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

#include "point_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/point.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
/*
 * 测试以下 PointF 接口：
 * 1. SetX
 * 2. GetX
 * 3. SetY
 * 4. GetY
 * 5. Set
 * 6. IsZero
 * 7. Offset
 * 8. += 运算符
 * 9. -= 运算符
 * 10. *= 运算符
 * 11. /= 运算符
 */
void PointFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    scalar scale = GetObject<scalar>();
    scalar divisor = GetObject<scalar>();

    PointF pointf;
    PointF pointf1 = PointF(x, y);

    pointf.SetX(x);
    pointf.GetX();
    pointf.SetY(y);
    pointf.GetY();
    pointf.Set(x, y);

    pointf.IsZero();
    pointf.Offset(x, y);

    pointf1 += pointf;
    pointf1 -= pointf;
    pointf1 *= scale;
    pointf1 /= divisor;
}

/*
 * 测试以下 PointF 接口：
 * 1. 构造函数
 * 2. + 运算符
 * 3. - 运算符
 * 4. 比较运算符 ==
 * 5. 比较运算符 !=
 * 6. *= 运算符
 * 7. /= 运算符
 */
void PointFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    scalar scale = GetObject<scalar>();
    scalar divisor = GetObject<scalar>();

    PointF pointf;
    PointF pointf1 = PointF(x, y);
    PointF pointf2 = PointF(pointf);

    pointf = pointf1 + pointf2;
    pointf = pointf1 - pointf2;
    pointf = scale * pointf2;
    pointf = pointf1 * scale;
    pointf = pointf1 / divisor;

    if (pointf == pointf1) {}
    if (pointf != pointf1) {}
}

/*
 * 测试以下 PointI 接口：
 * 1. SetX
 * 2. GetX
 * 3. SetY
 * 4. GetY
 * 5. Set
 * 6. += 运算符
 * 7. -= 运算符
 * 8. *= 运算符
 * 9. /= 运算符
 */
void PointFuzzTest002(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    int x = GetObject<int>();
    int y = GetObject<int>();
    scalar scale = GetObject<scalar>();
    scalar divisor = GetObject<scalar>();

    PointI pointi;
    PointI pointi1 = PointI(x, y);

    pointi.SetX(x);
    pointi.GetX();
    pointi.SetY(y);
    pointi.GetY();
    pointi.Set(x, y);

    pointi1 += pointi;
    pointi1 -= pointi;
    pointi1 *= scale;
    pointi1 /= divisor;
}

/*
 * 测试以下 PointI 接口：
 * 1. 构造函数
 * 2. + 运算符
 * 3. - 运算符
 * 4. 比较运算符 ==
 * 5. 比较运算符 !=
 * 6. *= 运算符
 * 7. /= 运算符
 */
void PointFuzzTest003(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    int x = GetObject<int>();
    int y = GetObject<int>();
    scalar scale = GetObject<scalar>();
    scalar divisor = GetObject<scalar>();

    PointI pointi;
    PointI pointi1 = PointI(x, y);
    PointI pointi2 = PointI(pointi1);

    pointi = pointi1 + pointi2;
    pointi = pointi1 - pointi2;
    pointi = scale * pointi2;
    pointi = pointi1 * scale;
    pointi = pointi1 / divisor;

    if (pointi == pointi1) {}
    if (pointi != pointi1) {}
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PointFuzzTest000(data, size);
    OHOS::Rosen::Drawing::PointFuzzTest001(data, size);
    OHOS::Rosen::Drawing::PointFuzzTest002(data, size);
    OHOS::Rosen::Drawing::PointFuzzTest003(data, size);
    return 0;
}