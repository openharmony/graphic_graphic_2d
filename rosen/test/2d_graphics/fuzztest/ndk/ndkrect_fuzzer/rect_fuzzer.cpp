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

#include "rect_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"

#include "drawing_rect.h"
#include "drawing_types.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void RectFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(left, top, right, bottom);

    OH_Drawing_RectSetTop(nullptr, top);
    OH_Drawing_RectSetTop(rect, top);
    OH_Drawing_RectGetTop(nullptr);
    OH_Drawing_RectGetTop(rect);

    OH_Drawing_RectSetBottom(nullptr, bottom);
    OH_Drawing_RectSetBottom(rect, bottom);
    OH_Drawing_RectGetBottom(nullptr);
    OH_Drawing_RectGetBottom(rect);

    OH_Drawing_RectSetLeft(nullptr, left);
    OH_Drawing_RectSetLeft(rect, left);
    OH_Drawing_RectGetLeft(nullptr);
    OH_Drawing_RectGetLeft(rect);

    OH_Drawing_RectSetRight(nullptr, right);
    OH_Drawing_RectSetRight(rect, right);
    OH_Drawing_RectGetRight(nullptr);
    OH_Drawing_RectGetRight(rect);

    OH_Drawing_RectGetHeight(nullptr);
    OH_Drawing_RectGetHeight(rect);
    OH_Drawing_RectGetWidth(nullptr);
    OH_Drawing_RectGetWidth(rect);

    OH_Drawing_RectDestroy(rect);
    return;
}

void RectFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_Rect *rect1 = OH_Drawing_RectCreate(left, top, right, bottom);
    
    OH_Drawing_RectCopy(nullptr, rect1);
    OH_Drawing_RectCopy(rect, nullptr);
    OH_Drawing_RectCopy(rect, rect1);

    OH_Drawing_RectIntersect(nullptr, rect1);
    OH_Drawing_RectIntersect(rect, nullptr);
    OH_Drawing_RectIntersect(rect, rect1);

    OH_Drawing_RectJoin(nullptr, rect1);
    OH_Drawing_RectJoin(rect, nullptr);
    OH_Drawing_RectJoin(rect, rect1);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect1);
    return;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::RectFuzzTest000(data, size);
    OHOS::Rosen::Drawing::RectFuzzTest001(data, size);
    return 0;
}