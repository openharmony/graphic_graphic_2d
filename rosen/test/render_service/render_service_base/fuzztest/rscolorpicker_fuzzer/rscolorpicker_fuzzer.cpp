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
#include <fuzzer/FuzzedDataProvider.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "image/pixmap.h"
#include "render/rs_color_picker.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
int g_two = 240;
int g_three = 300;
int g_one = 180;
int g_six = 361;
int g_sixty = 60;
int g_zero = 120;
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
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
    FuzzedDataProvider fdp(data, size);
    uint32_t werrorCodeidth = fdp.ConsumeIntegralInRange<uint32_t>(0, 2);
    double coordinates[4]; // 4 is number of array members
    coordinates[0] = GetData<double>(); // 0 is index of left
    coordinates[1] = GetData<double>(); // 1 is index of top
    coordinates[2] = GetData<double>(); // 2 is index of right
    coordinates[3] = GetData<double>(); // 3 is index of bottom
    RSColorPicker::CreateColorPicker(pixmap, werrorCodeidth);
    RSColorPicker::CreateColorPicker(pixmap, coordinates, werrorCodeidth);
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
    uint32_t color = GetData<uint32_t>();
    rsColorPicker->GetAverageColor(color);
    rsColorPicker->featureColors_.push_back({ 0, 1 });
    rsColorPicker->GetAverageColor(color);
    rsColorPicker->featureColors_.clear();
    rsColorPicker->featureColors_.push_back({ 0, 0 });
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
    HSV color;
    color.h = GetData<int>();
    color.s = GetData<double>();
    color.v = GetData<double>();

    color.h = g_six;
    rsColorPicker->AdjustHSVToDefinedIterval(color);
    color.h = -1;
    rsColorPicker->AdjustHSVToDefinedIterval(color);
    color.s = 101.f;
    rsColorPicker->AdjustHSVToDefinedIterval(color);
    color.s = -1.f;
    rsColorPicker->AdjustHSVToDefinedIterval(color);
    color.v = 101.f;
    rsColorPicker->AdjustHSVToDefinedIterval(color);
    color.v = -1.f;
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

    auto pixmap = std::make_shared<Pixmap>();
    auto rsColorPicker = std::make_shared<RSColorPicker>(pixmap);
    HSV color;
    color.h = GetData<int>();
    color.s = GetData<double>();
    color.v = GetData<double>();
    rsColorPicker->HSVtoRGB(color);
    color.h = 0;
    rsColorPicker->HSVtoRGB(color);
    color.h = g_sixty;
    rsColorPicker->HSVtoRGB(color);
    color.h = g_zero;
    rsColorPicker->HSVtoRGB(color);
    color.h = g_one;
    rsColorPicker->HSVtoRGB(color);
    color.h = g_two;
    rsColorPicker->HSVtoRGB(color);
    color.h = g_three;
    rsColorPicker->HSVtoRGB(color);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreateColorPicker(data, size);
    OHOS::Rosen::DoGetLargestProportionColor(data, size);
    OHOS::Rosen::DoGetHighestSaturationColor(data, size);
    OHOS::Rosen::DoGetAverageColor(data, size);
    OHOS::Rosen::DoIsBlackOrWhiteOrGrayColor(data, size);
    OHOS::Rosen::DoIsEquals(data, size);
    OHOS::Rosen::DoRGB2HSV(data, size);
    OHOS::Rosen::DoAdjustHSVToDefinedIterval(data, size);
    OHOS::Rosen::DoHSVtoRGB(data, size);
    return 0;
}
