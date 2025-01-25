/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "image/bitmap.h"
#include "draw/color.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_ARRAY_SIZE = 5000;
constexpr size_t ALPHATYPE_SIZE = 4;
constexpr size_t COLORTYPE_SIZE = 10;
} // namespace

namespace Drawing {
/*
 * 测试以下 Bitmap 接口：
 * 1. Build(int width, int height, const BitmapFormat& format)
 * 2. GetWidth()
 * 3. GetHeight()
 * 4. GetPixels()
 * 5. ClearWithColor(ColorType color)
 * 6. Free()
 */
bool BitmapFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int width = GetObject<int>();
    int height = GetObject<int>();
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    if (bitmap.GetWidth() != width || bitmap.GetHeight() != height || bitmap.GetPixels() == nullptr) {
        return false;
    }
    bitmap.ClearWithColor(COLORTYPE_UNKNOWN);
    bitmap.Free();
    return true;
}

/*
 * 测试以下 Bitmap 接口：
 * 1. Build(const ImageInfo& info, int32_t stride)
 * 2. GetRowBytes()
 * 3. GetColorType()
 * 4. GetAlphaType()
 * 5. ExtractSubset(Bitmap& dst, const Rect& subset)
 */
bool BitmapFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int imageInfoWidth = GetObject<int>();
    int imageInfoHeight = GetObject<int>();
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    ImageInfo imageInfo = ImageInfo(imageInfoWidth, imageInfoHeight,
        static_cast<ColorType>(colorType % COLORTYPE_SIZE), static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    int32_t stride = GetObject<int32_t>();
    bitmap.Build(imageInfo, stride);
    bitmap.GetRowBytes();
    bitmap.GetColorType();
    bitmap.GetAlphaType();
    Bitmap dst;
    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    Rect subset {
        left,
        top,
        right,
        bottom,
    };
    bitmap.ExtractSubset(dst, subset);
    return true;
}

/*
 * 测试以下 Bitmap 接口：
 * 1. Bitmap()
 * 2. ReadPixels(const ImageInfo& info, uint32_t* pixels, size_t rowBytes, int32_t srcX, int32_t srcY)
 */
bool BitmapFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int imageInfoWidth = GetObject<int>();
    int imageInfoHeight = GetObject<int>();
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    ImageInfo imageInfo = ImageInfo(imageInfoWidth, imageInfoHeight,
        static_cast<ColorType>(colorType % COLORTYPE_SIZE), static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    int32_t width = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t srcX = GetObject<int32_t>() % width;
    int32_t srcY = GetObject<int32_t>() % height;
    uint32_t size_pix = width * height;
    uint32_t* pixels = new uint32_t[size_pix];
    for (size_t i = 0; i < size_pix; i++) {
        pixels[i] = GetObject<uint32_t>();
    }
    size_t dstRowBytes = GetObject<size_t>();
    bitmap.ReadPixels(imageInfo, pixels, dstRowBytes, srcX, srcY);
    if (pixels != nullptr) {
        delete[]  pixels;
        pixels = nullptr;
    }
    return true;
}

/*
 * 测试以下 Bitmap 接口：
 * 1. Bitmap()
 * 2. ComputeByteSize()
 * 3. PeekPixels(Pixmap& pixmap)
 * 4. SetPixels(uint32_t* pixels)
 * 5. GetPixels()
 */bool BitmapFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    bitmap.ComputeByteSize();
    Pixmap pixmap;
    bitmap.PeekPixels(pixmap);
    int32_t width = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    uint32_t size_pix = width * height;
    uint32_t* pixels = new uint32_t[size_pix];
    for (size_t i = 0; i < size_pix; i++) {
        pixels[i] = GetObject<uint32_t>();
    }
    bitmap.SetPixels(pixels);
    bitmap.GetPixels();
    if (pixels != nullptr) {
        delete[]  pixels;
        pixels = nullptr;
    }
    return true;
}

/*
 * 测试以下 Bitmap 接口：
 * 1. Bitmap()
 * 2. CopyPixels(Bitmap& dst, int srcLeft, int srcTop)
 * 3. SetImmutable()
 * 4. IsImmutable()
 * 5. IsValid()
 * 6. IsEmpty()
 * 7. ClearWithColor(ColorQuad color)
 * 8. GetColor(int x, int y)
 * 9. Free()
 */
bool BitmapFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    Bitmap dst;
    int srcLeft = GetObject<int>();
    int srcTop = GetObject<int>();
    bitmap.CopyPixels(dst, srcLeft, srcTop);
    bitmap.SetImmutable();
    bitmap.IsImmutable();
    bitmap.IsValid();
    bitmap.IsEmpty();
    ColorQuad color = GetObject<ColorQuad>();
    bitmap.ClearWithColor(color);
    bitmap.GetColor(srcLeft, srcTop);
    bitmap.Free();

    return true;
}

/*
 * 测试以下 Bitmap 接口：
 * 1. Bitmap()
 * 2. GetFormat()
 * 3. SetFormat(const BitmapFormat& format)
 * 4. SetInfo(const ImageInfo& info)
 * 5. GetImageInfo()
 * 6. GetPixmap()
 * 7. TryAllocPixels(const ImageInfo& info)
 * 8. Serialize()
 * 9. Deserialize(Data* data)
 */
bool BitmapFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    BitmapFormat format;
    bitmap.GetFormat();
    bitmap.SetFormat(format);
    int imageInfoWidth = GetObject<int>();
    int imageInfoHeight = GetObject<int>();
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    ImageInfo imageInfo = ImageInfo(imageInfoWidth, imageInfoHeight,
        static_cast<ColorType>(colorType % COLORTYPE_SIZE), static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    bitmap.SetInfo(imageInfo);
    bitmap.GetImageInfo();
    bitmap.GetPixmap();
    bitmap.TryAllocPixels(imageInfo);
    bitmap.Serialize();
    bitmap.Deserialize(nullptr);
    return true;
}

/*
 * 测试以下 Bitmap 接口：
 * 1. Bitmap()
 * 2. InstallPixels(const ImageInfo& info, uint32_t* pixels, size_t rowBytes, void* releaseProc, void* context)
 */
bool BitmapFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int imageInfoWidth = GetObject<int>();
    int imageInfoHeight = GetObject<int>();
    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    ImageInfo imageInfo = ImageInfo(imageInfoWidth, imageInfoHeight,
        static_cast<ColorType>(colorType % COLORTYPE_SIZE), static_cast<AlphaType>(alphaType % ALPHATYPE_SIZE));
    int32_t width = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_ARRAY_SIZE;
    uint32_t size_pix = width * height;
    uint32_t* pixels = new uint32_t[size_pix];
    for (size_t i = 0; i < size_pix; i++) {
        pixels[i] = GetObject<uint32_t>();
    }
    size_t rowBytes = GetObject<size_t>();
    bitmap.InstallPixels(imageInfo, pixels, rowBytes, nullptr, nullptr);
    if (pixels != nullptr) {
        delete[] pixels;
        pixels = nullptr;
    }
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BitmapFuzzTest001(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest002(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest003(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest004(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest005(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest006(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest007(data, size);
    return 0;
}