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

#include "bitmap_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"

#include "drawing_bitmap.h"
#include "drawing_types.h"
#include "draw/brush.h"


namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t MAX_ARRAY_SIZE = 5000;
    constexpr uint32_t PATH_FIX = 6;
    constexpr uint32_t PATH_FOUR = 4;
} // namespace

void BitmapFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t width = GetObject<uint32_t>();
    uint32_t height = GetObject<uint32_t>();
    int32_t enum_1 = GetObject<int32_t>();
  
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat bitmapFormat { static_cast<OH_Drawing_ColorFormat>(enum_1 % PATH_FIX),
        static_cast<OH_Drawing_AlphaFormat>(enum_1 % PATH_FOUR) };
    OH_Drawing_BitmapBuild(nullptr, width, height, nullptr);
    OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
    OH_Drawing_BitmapGetHeight(nullptr);
    OH_Drawing_BitmapGetHeight(bitmap);
    OH_Drawing_BitmapGetWidth(nullptr);
    OH_Drawing_BitmapGetWidth(bitmap);
    OH_Drawing_BitmapGetPixels(nullptr);
    OH_Drawing_BitmapGetPixels(bitmap);
    
    OH_Drawing_BitmapGetAlphaFormat(nullptr);
    OH_Drawing_BitmapGetAlphaFormat(bitmap);
    OH_Drawing_BitmapGetColorFormat(nullptr);
    OH_Drawing_BitmapGetColorFormat(bitmap);

    OH_Drawing_BitmapDestroy(bitmap);
    return;
}

void BitmapFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_ARRAY_SIZE;
  
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo = {width, height,
        GetObject<OH_Drawing_ColorFormat>(), GetObject<OH_Drawing_AlphaFormat>()};
       
    OH_Drawing_BitmapGetImageInfo(nullptr, nullptr);
    OH_Drawing_BitmapGetImageInfo(bitmap, &imageInfo);

    uint32_t size_pix = width * height;
    uint32_t* pixels = new uint32_t[size_pix];
    for (size_t i = 0; i < size_pix; i++) {
        pixels[i] = GetObject<uint32_t>();
    }
    uint32_t rowBytes = GetObject<uint32_t>();
    OH_Drawing_Bitmap*  bitmap1 = OH_Drawing_BitmapCreateFromPixels(nullptr, nullptr, rowBytes);
    rowBytes = GetObject<uint32_t>();
    bitmap1 = OH_Drawing_BitmapCreateFromPixels(&imageInfo, pixels, rowBytes);
    if (pixels != nullptr) {
        delete[]  pixels;
        pixels = nullptr;
    }
    OH_Drawing_BitmapGetColorFormat(bitmap1);
    OH_Drawing_BitmapGetAlphaFormat(bitmap1);

    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_BitmapDestroy(bitmap1);
    return;
}

void BitmapFuzzTest002(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t width1 = GetObject<int32_t>() % width;
    int32_t height1 = GetObject<int32_t>() % height;
  
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();

    OH_Drawing_BitmapFormat bitmapFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    OH_Drawing_BitmapBuild(bitmap, width, height, &bitmapFormat);
    OH_Drawing_Image_Info imageInfo = {width, height, COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_UNPREMUL};
    
    uint32_t size_pix = width * height;
    uint32_t* pixels1 = new uint32_t[size_pix];
    for (size_t i = 0; i < size_pix; i++) {
        pixels1[i] = GetObject<uint32_t>();
    }

    OH_Drawing_BitmapReadPixels(nullptr, nullptr, nullptr, width * PATH_FOUR, width1, height1);
    width1 = GetObject<int32_t>() % width;
    height1 = GetObject<int32_t>() % height;
    OH_Drawing_BitmapReadPixels(bitmap, &imageInfo, pixels1, width * PATH_FOUR, width1, height1);
    if (pixels1 != nullptr) {
        delete[]  pixels1;
        pixels1 = nullptr;
    }
    
    OH_Drawing_BitmapDestroy(bitmap);
    return;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BitmapFuzzTest000(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest001(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest002(data, size);
    return 0;
}