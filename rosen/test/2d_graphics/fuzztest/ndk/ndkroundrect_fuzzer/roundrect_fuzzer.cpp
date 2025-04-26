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
#include "roundrect_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_rect.h"
#include "drawing_round_rect.h"
#include "drawing_types.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t ROUND_RECT_CORNER_POS_ENUM_SIZE = 4;
} // namespace

namespace Drawing {
void NativeDrawingRoundRectTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    float xRad = GetObject<float>();
    float yRad = GetObject<float>();
    float dx = GetObject<float>();
    float dy = GetObject<float>();
    uint32_t cornerPos = GetObject<uint32_t>();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);

    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(nullptr, xRad, yRad);
    roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);

    OH_Drawing_Corner_Radii radiusXY = {GetObject<float>(), GetObject<float>()};
    OH_Drawing_RoundRectSetCorner(nullptr,
        static_cast<OH_Drawing_CornerPos>(cornerPos % ROUND_RECT_CORNER_POS_ENUM_SIZE), radiusXY);
    OH_Drawing_RoundRectSetCorner(roundRect,
        static_cast<OH_Drawing_CornerPos>(cornerPos % ROUND_RECT_CORNER_POS_ENUM_SIZE), radiusXY);
    OH_Drawing_RoundRectSetCorner(roundRect, static_cast<OH_Drawing_CornerPos>(cornerPos), radiusXY);

    OH_Drawing_RoundRectGetCorner(nullptr,
        static_cast<OH_Drawing_CornerPos>(cornerPos % ROUND_RECT_CORNER_POS_ENUM_SIZE));
    OH_Drawing_RoundRectGetCorner(roundRect,
        static_cast<OH_Drawing_CornerPos>(cornerPos % ROUND_RECT_CORNER_POS_ENUM_SIZE));
    OH_Drawing_RoundRectGetCorner(roundRect, static_cast<OH_Drawing_CornerPos>(cornerPos));

    OH_Drawing_RoundRectOffset(roundRect, dx, dy);
    OH_Drawing_RoundRectOffset(nullptr, dx, dy);

    OH_Drawing_RoundRect* roundRect2 = OH_Drawing_RoundRectCopy(roundRect);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RoundRectDestroy(roundRect2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingRoundRectTest001(data, size);

    return 0;
}
