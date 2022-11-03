/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "color_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
bool ColorFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    bool isAntiAlias = GetObject<bool>();
    Brush brush;
    brush.SetAntiAlias(isAntiAlias);
    Color color;
    color.SetRgb(red, green, blue, alpha);
    color.GetAlpha();
    color.GetRed();
    color.GetGreen();
    color.GetBlue();
    brush.SetColor(color);

    uint32_t colorValue = GetObject<uint32_t>();
    color.SetColorQuad(colorValue);
    brush.SetColor(color);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ColorFuzzTest(data, size);
    return 0;
}
