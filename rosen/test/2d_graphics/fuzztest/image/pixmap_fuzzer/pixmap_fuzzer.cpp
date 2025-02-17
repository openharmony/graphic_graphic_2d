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

#include "pixmap_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"

#include "image/pixmap.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t COLORTYPE_NUM = 10;
constexpr size_t ALPHATYPE_NUM = 4;
constexpr size_t FILTERMODE_NUM = 2;
constexpr size_t ARRAY_MAX_SIZE = 5000;
} // namespace

namespace Drawing {
void PixmapFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    int x = GetObject<int>();
    int y = GetObject<int>();
    int width = GetObject<int>();
    int height = GetObject<int>();
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    size_t rowBytes = GetObject<size_t>();
    uint32_t str_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE + 1;
    uint32_t fm = GetObject<uint32_t>();

    Pixmap pix;
    ImageInfo imageInfo = ImageInfo(width, height, static_cast<ColorType>(colorType % COLORTYPE_NUM),
        static_cast<AlphaType>(alphaType % ALPHATYPE_NUM));
    char* addr = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        addr[i] = GetObject<char>();
    }
    addr[str_size - 1] = '\0';
    Pixmap pixmap = Pixmap(imageInfo, addr, rowBytes);
    SamplingOptions options(static_cast<FilterMode>(fm % FILTERMODE_NUM));
    pix.ScalePixels(pixmap, options);
    pix.GetColorType();
    pix.GetAlphaType();
    pix.GetColor(x, y);
    pix.GetRowBytes();
    pix.GetAddr();
    pix.GetWidth();
    pix.GetHeight();
    pix.GetColorSpace();
    if (addr != nullptr) {
        delete [] addr;
        addr = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::PixmapFuzzTest000(data, size);
    return 0;
}