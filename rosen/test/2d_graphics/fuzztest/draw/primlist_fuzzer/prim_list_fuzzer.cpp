/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language and permissions and
 * limitations under the License.
 */

#include "prim_list_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "draw/canvas.h"
#include "draw/prim_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int32_t BITMAP_WIDTH = 300;
static constexpr int32_t BITMAP_HEIGHT = 300;
constexpr size_t COLORFORMAT_SIZE = 12;
constexpr size_t ALPHAFORMAT_SIZE = 4;

bool PrimListFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    PrimList primList;
    float alphaFactor = GetObject<float>();
    primList.UpdateAlphaFactor(alphaFactor);
    primList.GetPrimCount();
    primList.GetPrimCountPerType();
    return true;
}

bool PrimListCanvasFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    Bitmap bitmap;
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    BitmapFormat format {static_cast<ColorType>(colorType % COLORFORMAT_SIZE),
                         static_cast<AlphaType>(alphaType % ALPHAFORMAT_SIZE)};
    bitmap.Build(BITMAP_WIDTH, BITMAP_HEIGHT, format);
    canvas.Bind(bitmap);

    Rect bounds(GetObject<scalar>(), GetObject<scalar>(),
                GetObject<scalar>(), GetObject<scalar>());
    canvas.BeginPrimListCollecting(bounds);
    Rect rect(GetObject<scalar>(), GetObject<scalar>(),
              GetObject<scalar>(), GetObject<scalar>());
    canvas.DrawRect(rect);
    auto collectedPrimList = canvas.EndPrimListCollecting();
    if (collectedPrimList != nullptr) {
        collectedPrimList->UpdateAlphaFactor(GetObject<float>());
        collectedPrimList->GetPrimCount();
        collectedPrimList->GetPrimCountPerType();
        canvas.DrawPrimList(*collectedPrimList);
    }

    return true;
}

bool PrimListCanvasFuzzTestNullptr(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    Canvas canvas;
    canvas.BeginPrimListCollecting(
        Rect(GetObject<scalar>(), GetObject<scalar>(),
             GetObject<scalar>(), GetObject<scalar>()));
    canvas.DrawRect(
        Rect(GetObject<scalar>(), GetObject<scalar>(),
             GetObject<scalar>(), GetObject<scalar>()));
    auto primList = canvas.EndPrimListCollecting();
    (void)primList;
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    OHOS::Rosen::Drawing::PrimListFuzzTest(data, size);
    OHOS::Rosen::Drawing::PrimListCanvasFuzzTest(data, size);
    OHOS::Rosen::Drawing::PrimListCanvasFuzzTestNullptr(data, size);
    return 0;
}