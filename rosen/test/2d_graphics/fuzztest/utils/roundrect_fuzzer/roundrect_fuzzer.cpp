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

#include "roundrect_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/round_rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t PATH_FIVE = 5;
    constexpr double PATH_DOUBLE = 1.0;
} // namespace

void RoundRectFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    uint32_t cornerPosA = GetObject<uint32_t>();

    RoundRect roundRect;
    roundRect.Offset(dx, dy);
    roundRect.IsSimpleRoundRect();
    roundRect.GetSimpleX();
    roundRect.GetSimpleY();

    roundRect.AdjustRadiiX(PATH_DOUBLE, PATH_DOUBLE, static_cast<RoundRect::CornerPos>(cornerPosA % PATH_FIVE),
        static_cast<RoundRect::CornerPos>(cornerPosA % PATH_FIVE));
    roundRect.AdjustRadiiY(PATH_DOUBLE, PATH_DOUBLE, static_cast<RoundRect::CornerPos>(cornerPosA % PATH_FIVE),
        static_cast<RoundRect::CornerPos>(cornerPosA % PATH_FIVE));
    roundRect.ClampToZero();
    roundRect.ScaleRadii();
    roundRect.SetCornerRadius(static_cast<RoundRect::CornerPos>(cornerPosA % PATH_FIVE), dx, dy);
    roundRect.GetCornerRadius(static_cast<RoundRect::CornerPos>(cornerPosA % PATH_FIVE));
}

void RoundRectFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    std::vector<Point> radiusXY;
    radiusXY.push_back({GetObject<scalar>(), GetObject<scalar>()});
    radiusXY.push_back({GetObject<scalar>(), GetObject<scalar>()});
    radiusXY.push_back({GetObject<scalar>(), GetObject<scalar>()});
    radiusXY.push_back({GetObject<scalar>(), GetObject<scalar>()});

    Rect r;
    RoundRect roundRect = RoundRect(r, xRad, yRad);
    RoundRect roundRect1 = RoundRect(roundRect);
    RoundRect roundRect2 = RoundRect(r, radiusXY);

    roundRect.SetRect(r);
    roundRect.GetRect();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::RoundRectFuzzTest000(data, size);
    OHOS::Rosen::Drawing::RoundRectFuzzTest001(data, size);

    return 0;
}