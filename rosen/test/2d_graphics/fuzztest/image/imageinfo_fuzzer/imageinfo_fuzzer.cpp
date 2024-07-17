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

#include "imageinfo_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "image/image_info.h"
#include "image/image.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ALPHATYPE_SIZE = 4;
constexpr size_t COLORTYPE_SIZE = 10;
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_SIZE = 5000;
} // namespace
namespace Drawing {
bool ImageInfoFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    ImageInfo imageInfo = ImageInfo(width, height, static_cast<ColorType>(colorType % COLORTYPE_SIZE),
        static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    imageInfo.GetBytesPerPixel();
    imageInfo.GetBound();
    return true;
}

bool ImageInfoFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ImageInfo imageInfo;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    imageInfo.MakeN32Premul(width, height);
    return true;
}

bool ImageInfoFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ImageInfo imageInfo;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    imageInfo.SetWidth(width);
    imageInfo.GetWidth();
    imageInfo.SetHeight(height);
    imageInfo.GetHeight();
    imageInfo.SetColorType(static_cast<ColorType>(colorType % COLORTYPE_SIZE));
    imageInfo.GetColorType();
    imageInfo.SetAlphaType(static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    imageInfo.GetAlphaType();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGBLinear();
    imageInfo.SetColorSpace(colorSpace);
    imageInfo.GetColorSpace();
    imageInfo.GetBytesPerPixel();
    imageInfo.GetBound();
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ImageInfoFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ImageInfoFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ImageInfoFuzzTest003(data, size);
    return 0;
}