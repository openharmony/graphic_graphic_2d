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

#include "rscolorpicker_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "image/pixmap.h"
#include "render/rs_color_picker.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;
auto pixmap = std::make_shared<Pixmap>();
auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);

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

bool DoCreateColorPicker(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto pixmap = std::make_shared<Pixmap>();
    uint32_t werrorCodeidth = GetData<uint32_t>();
    double coordinates = GetData<double>();
    RSColorPicker::CreateColorPicker(pixmap, werrorCodeidth);
    RSColorPicker::CreateColorPicker(pixmap, &coordinates, werrorCodeidth);
    return true;
}
bool DoGetLargestProportionColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetData<uint32_t>();
    rsColorPicker->GetLargestProportionColor(color);
    return true;
}
bool DoGetHighestSaturationColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetData<uint32_t>();
    rsColorPicker->GetHighestSaturationColor(color);
    return true;
}
bool DoGetAverageColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetData<uint32_t>();
    rsColorPicker->GetAverageColor(color);
    return true;
}
bool DoIsBlackOrWhiteOrGrayColor(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetData<uint32_t>();
    rsColorPicker->IsBlackOrWhiteOrGrayColor(color);
    return true;
}
bool DoIsEquals(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    double val1 = GetData<double>();
    double val2 = GetData<double>();
    rsColorPicker->IsEquals(val1, val2);
    return true;
}

bool DoRGB2HSV(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t rgb = GetData<uint32_t>();
    rsColorPicker->RGB2HSV(rgb);
    return true;
}

bool DoAdjustHSVToDefinedIterval(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    HSV color;
    color.h = GetData<int>();
    color.s = GetData<double>();
    color.v = GetData<double>();

    rsColorPicker->AdjustHSVToDefinedIterval(color);
    return true;
}
bool DoHSVtoRGB(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    HSV color;
    color.h = GetData<int>();
    color.s = GetData<double>();
    color.v = GetData<double>();
    rsColorPicker->HSVtoRGB(color);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreateColorPicker(data, size);         // CreateColorPicker
    OHOS::Rosen::DoGetLargestProportionColor(data, size); // GetLargestProportionColor
    OHOS::Rosen::DoGetHighestSaturationColor(data, size); // GetHighestSaturationColor
    OHOS::Rosen::DoGetAverageColor(data, size);           // GetAverageColor
    OHOS::Rosen::DoIsBlackOrWhiteOrGrayColor(data, size); // IsBlackOrWhiteOrGrayColor
    OHOS::Rosen::DoIsEquals(data, size);                  // IsEquals
    OHOS::Rosen::DoRGB2HSV(data, size);                   // RGB2HSV
    OHOS::Rosen::DoAdjustHSVToDefinedIterval(data, size); // AdjustHSVToDefinedIterval
    OHOS::Rosen::DoHSVtoRGB(data, size);                  // HSVtoRGB
    return 0;
}
