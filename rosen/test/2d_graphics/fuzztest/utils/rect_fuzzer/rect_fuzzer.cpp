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
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void RectFuzzTest000(const uint8_t* data, size_t size)
{
    int pos = GetObject<int>();

    RectI rect;

    rect.SetLeft(pos);
    rect.GetLeft();
    rect.SetTop(pos);
    rect.GetTop();
    rect.SetRight(pos);
    rect.GetRight();
    rect.SetBottom(pos);
    rect.GetBottom();
    rect.GetHeight();
    rect.GetWidth();

    rect.IsEmpty();
    rect.IsValid();
}

void RectFuzzTest001(const uint8_t* data, size_t size)
{
    int dx = GetObject<int>();
    int dy = GetObject<int>();
    int l = GetObject<int>();
    int t = GetObject<int>();
    int r = GetObject<int>();
    int b = GetObject<int>();

    RectI rect1 = RectI(l, t, r, b);
    RectI rect2 = RectI(rect1);

    rect1.Offset(dx, dy);
    rect1.MakeOutset(dx, dy);

    rect1.Contains(rect2);
    rect1.Join(rect2);
    rect1.ToString();
    if (rect1 == rect2) {}
    if (rect1 != rect2) {}
}

void RectFuzzTest002(const uint8_t* data, size_t size)
{
    scalar pos = GetObject<scalar>();
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();

    RectF rectf;
    RectI recti;
    RectF rectf1 = RectF(l, t, r, b);
    RectF rectf2 = RectF(rectf1);
    RectF rectf3 = RectF(recti);

    rectf.SetLeft(pos);
    rectf.GetLeft();
    rectf.SetTop(pos);
    rectf.GetTop();
    rectf.SetRight(pos);
    rectf.GetRight();
    rectf.SetBottom(pos);
    rectf.GetBottom();
    rectf.GetWidth();
    rectf.GetHeight();
    rectf.IsValid();
    rectf.IsEmpty();

    rectf.Intersect(rectf1);
    rectf.Join(rectf1);

    if (rectf == rectf1) {}
    if (rectf != rectf1) {}
}

void RectFuzzTest003(const uint8_t* data, size_t size)
{
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();

    RectF rectf = RectF(l, t, r, b);
    rectf.Offset(dx, dy);
    rectf.MakeOutset(dx, dy);
    rectf.Round();
    rectf.RoundOut();
    rectf.ToString();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::RectFuzzTest000(data, size);
    OHOS::Rosen::Drawing::RectFuzzTest001(data, size);
    OHOS::Rosen::Drawing::RectFuzzTest002(data, size);
    OHOS::Rosen::Drawing::RectFuzzTest003(data, size);
    
    return 0;
}