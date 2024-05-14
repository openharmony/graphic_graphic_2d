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

#include "rscolor_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "common/rs_color.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}
bool DoRSColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    RSColor color1(r, g, b, a);
    RSColor color2;
    RSColor color3(r);
    RSColor color4(r, g, b);
    RSColor color5(color2);
    RSColor color6 = color2;
    color4.GetBlue();
    color5.GetBlue();
    color6.GetBlue();
    return true;
}
bool DoOperator(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    RSColor color1(r, g, b, a);
    RSColor color2;
    if (color1 == color2) {
    }
    if (color1 != color2) {
    }
    RSColor resultColor = color1 + color2;
    resultColor = color1 - color2;
    float scaleFactor = GetData<float>();
    color1 *= scaleFactor;
    resultColor = color1 * scaleFactor;
    return true;
}
bool DoHaveParameters(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSColor color;
    uint32_t rgba = GetData<uint32_t>();
    RSColor::FromRgbaInt(rgba);
    RSColor::FromArgbInt(rgba);
    RSColor::FromBgraInt(rgba);
    int16_t blue = GetData<int16_t>();
    int16_t green = GetData<int16_t>();
    int16_t red = GetData<int16_t>();
    int16_t alpha = GetData<int16_t>();
    float alpha1 = GetData<float>();
    color.SetBlue(blue);
    color.SetGreen(green);
    color.SetRed(red);
    color.SetAlpha(alpha);
    color.MultiplyAlpha(alpha1);
    return true;
}
bool DoNoParameters(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    uint32_t a = GetData<uint32_t>();
    RSColor color1(r, g, b, a);
    color1.AsRgbaInt();
    color1.AsArgbInt();
    color1.AsBgraInt();
    color1.GetBlue();
    color1.GetGreen();
    color1.GetRed();
    color1.GetAlpha();
    RSColor::GetBytesPerPixelInt();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRSColor(data, size);        // RSColor
    OHOS::Rosen::DoOperator(data, size);       // operator
    OHOS::Rosen::DoHaveParameters(data, size); // Have Parameters
    OHOS::Rosen::DoNoParameters(data, size);   // NoParameters
    return 0;
}
