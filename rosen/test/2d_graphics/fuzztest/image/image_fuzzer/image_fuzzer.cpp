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

#include "image_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "draw/surface.h"
#include "image/bitmap.h"
#include "image/gpu_context.h"
#include "image/image.h"
#include "utils/rect.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t COMTYPE_SIZE = 7;
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t ENCODEFORMAT_SIZE = 4;
constexpr size_t FILTERMODE_SIZE = 2;
constexpr size_t FUNCTYPE_SIZE = 4;
constexpr size_t MAX_SIZE = 5000;
constexpr size_t MATRIXTYPE_SIZE = 5;
constexpr size_t ORIGIN_SIZE = 2;
} // namespace

namespace Drawing {
/*
 *  测试使用 Bitmap 构建 Image 的功能，包括:
 *  1. Image::BuildFromBitmap()
 */
bool BuildImageFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    Bitmap bitmap;
    int width = static_cast<int>(data[0]);
    int height = static_cast<int>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    if (bitmap.GetWidth() != width || bitmap.GetHeight() != height) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    return true;
}

/*
 *  测试 Image 的基本功能，包括:
 *  1. AsLegacyBitmap()
 *  2. GetWidth()
 *  3. GetHeight()
 *  4. GetColorType()
 *  5. GetAlphaType()
 *  6. GetColorSpace()
 *  7. GetUniqueID()
 *  8. GetImageInfo()
 *  9. Serialize()
 *  10. Deserialize()
 *  11. MakeRasterImage()
 *  12. GetHeadroom()
 *  13. SetHeadroom(float)
 */
bool ImageFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    image.AsLegacyBitmap(bitmap);
    image.GetWidth();
    image.GetHeight();
    image.GetColorType();
    image.GetAlphaType();
    image.GetColorSpace();
    image.GetUniqueID();
    image.GetImageInfo();
    auto dataVal = image.Serialize();
    image.Deserialize(dataVal);
    image.MakeRasterImage();
    float headroom = GetObject<float>();
    image.SetHeadroom(headroom);
    image.GetHeadroom();
    return true;
}

/*
 *  测试从 Bitmap 构建 Image 的功能:
 *  1. Image::BuildFromBitmap()
 */
bool ImageFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    auto image = std::make_shared<Image>();
    return image->BuildFromBitmap(bitmap);
}

/*
 *  测试从 Pixmap 构建栅格 Image 的功能:
 *  1. Image::MakeFromRaster()
 */
bool ImageFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    ImageInfo imageInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_UNPREMUL };
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* rawImg = new char[count];
    for (size_t i = 0; i < count; i++) {
        rawImg[i] = GetObject<char>();
    }
    rawImg[count - 1] = '\0';
    Pixmap pixmap = Pixmap(imageInfo, static_cast<void*>(rawImg), count);
    std::shared_ptr<Image> image = Image::MakeFromRaster(pixmap, nullptr, nullptr);
    if (rawImg != nullptr) {
        delete [] rawImg;
        rawImg = nullptr;
    }
    return true;
}

/*
 *  测试从 Data 构建栅格 Image 的功能:
 *  1. Image::MakeRasterData()
 */
bool ImageFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    ImageInfo imageInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_UNPREMUL };
    std::shared_ptr<Image> image = Image::MakeRasterData(imageInfo, dataVal, length);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 *  测试 Image 的属性和像素操作，包括:
 *  1. GetROPixels()
 *  2. IsLazyGenerated()
 *  3. CanPeekPixels()
 *  4. IsTextureBacked()
 *  5. IsOpaque()
 */
bool ImageFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    image.GetROPixels(bitmap);
    image.IsLazyGenerated();
    image.CanPeekPixels();
    image.IsTextureBacked();
    image.IsOpaque();
    return true;
}

/*
 *  测试从 GPUContext 和 Bitmap 构建 Image 的功能:
 *  1. Image::BuildFromBitmap()
 *  2. Image::BuildFromBitmap(GPUContext& gpuContext, Bitmap& bitmap)
 */
bool ImageFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    GPUContext gpuContext;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    std::unique_ptr<Image> image = std::make_unique<Image>();
    image->BuildFromBitmap(gpuContext, bitmap);
    return true;
}

/*
 *  测试从 Data 构建编码的 Image 的功能:
 *  1. Image::MakeFromEncoded()
 */
bool ImageFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    image.MakeFromEncoded(dataVal);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 *  测试从压缩数据构建 Image 的功能:
 *  1. Image::BuildFromCompressed()
 */
bool ImageFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    GPUContext gpuContext;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    uint32_t comType = GetObject<uint32_t>();
    std::unique_ptr<Image> image = std::make_unique<Image>();
    image->BuildFromCompressed(gpuContext, dataVal, width, height,
        static_cast<CompressedType>(comType % COMTYPE_SIZE));
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 *  测试从纹理构建 Image 的功能:
 *  1. Image::BuildFromTexture()
 */
bool ImageFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    GPUContext gpuContext;
    TextureInfo textureInfo;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    bool isMipMapped = GetObject<bool>();
    unsigned int target = GetObject<unsigned int>();
    unsigned int textId = GetObject<unsigned int>();
    unsigned int format = GetObject<unsigned int>();
    textureInfo.SetWidth(width);
    textureInfo.GetWidth();
    textureInfo.SetHeight(height);
    textureInfo.GetHeight();
    textureInfo.SetIsMipMapped(isMipMapped);
    textureInfo.GetIsMipMapped();
    textureInfo.SetTarget(target);
    textureInfo.GetTarget();
    textureInfo.SetID(textId);
    textureInfo.GetID();
    textureInfo.SetFormat(format);
    textureInfo.GetFormat();
    uint32_t origin = GetObject<uint32_t>();
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    uint32_t funcType = GetObject<uint32_t>();
    uint32_t matrixType = GetObject<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    image.BuildFromTexture(gpuContext, textureInfo, static_cast<TextureOrigin>(origin % ORIGIN_SIZE),
        bitmapFormat, colorSpace);
    return true;
}

/*
 *  测试从 Surface 构建 Image 的功能:
 *  1. Image::BuildFromSurface()
 */
bool ImageFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    GPUContext gpuContext;
    Surface surface;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    surface.Bind(bitmap);

    uint32_t origin = GetObject<uint32_t>();
    uint32_t funcType = GetObject<uint32_t>();
    uint32_t matrixType = GetObject<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    image.BuildFromSurface(gpuContext, surface, static_cast<TextureOrigin>(origin % ORIGIN_SIZE),
        bitmapFormat, colorSpace);
    return true;
}

/*
 *  测试从 Image 中读取像素到 Bitmap 的功能:
 *  1. Image::ReadPixels(Bitmap&)
 */
bool ImageFuzzTest011(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    image.ReadPixels(bitmap, width, height);
    return true;
}

/*
 *  测试从 Image 中读取像素到 Pixmap 的功能:
 *  1. Image::ReadPixels(Pixmap&)
 */
bool ImageFuzzTest012(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    bool buildFromBitmap = image.BuildFromBitmap(bitmap);
    if (!buildFromBitmap) {
        return false;
    }

    ImageInfo imageInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_UNPREMUL };
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* rawImg = new char[count];
    for (size_t i = 0; i < count; i++) {
        rawImg[i] = GetObject<char>();
    }
    rawImg[count - 1] = '\0';
    Pixmap pixmap = Pixmap(imageInfo, static_cast<void*>(rawImg), count);
    image.ReadPixels(pixmap, width, height);
    if (rawImg != nullptr) {
        delete [] rawImg;
        rawImg = nullptr;
    }
    return true;
}

/*
 *  测试从 Image 中读取像素到缓冲区的功能:
 *  1. Image::ReadPixels(ImageInfo, void*, size_t, int, int)
 */
bool ImageFuzzTest013(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    bool buildFromBitmap = image.BuildFromBitmap(bitmap);
    if (!buildFromBitmap) {
        return false;
    }
    ImageInfo imageInfo = { width, height, COLORTYPE_RGBA_8888, ALPHATYPE_UNPREMUL };
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* rawImg = new char[count];
    for (size_t i = 0; i < count; i++) {
        rawImg[i] = GetObject<char>();
    }
    rawImg[count - 1] = '\0';
    image.ReadPixels(imageInfo, static_cast<void*>(rawImg), count, width, height);
    if (rawImg != nullptr) {
        delete [] rawImg;
        rawImg = nullptr;
    }
    return true;
}

/*
 *  测试缩放像素到 Bitmap 的功能:
 *  1. Image::ScalePixels()
 */
bool ImageFuzzTest014(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    bool buildFromBitmap = image.BuildFromBitmap(bitmap);
    if (!buildFromBitmap) {
        return false;
    }
    uint32_t fm = GetObject<uint32_t>();
    SamplingOptions sampling = SamplingOptions(static_cast<FilterMode>(fm % FILTERMODE_SIZE));
    bool allowCachingHint = GetObject<bool>();
    image.ScalePixels(bitmap, sampling, allowCachingHint);
    return true;
}

/*
 *  测试将 Image 编码为指定格式的功能:
 *  1. Image::EncodeToData()
 */
bool ImageFuzzTest015(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Image image;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    bool buildFromBitmap = image.BuildFromBitmap(bitmap);
    if (!buildFromBitmap) {
        return false;
    }
    int quality = GetObject<int>();
    uint32_t encodedImageFormat = GetObject<uint32_t>();
    image.EncodeToData(static_cast<EncodedImageFormat>(encodedImageFormat % ENCODEFORMAT_SIZE), quality);
    return true;
}

/*
 *  测试从 YUVAPixmaps 构建 Image 的功能:
 *  1. Image::MakeFromYUVAPixmaps()
 */
bool ImageFuzzTest016(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    GPUContext gpuContext;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    YUVInfo info = YUVInfo(width, height, YUVInfo::PlaneConfig::Y_UV, YUVInfo::SubSampling::K420,
        YUVInfo::YUVColorSpace::JPEG_FULL_YUVCOLORSPACE, YUVInfo::YUVDataType::UNORM_8);
    int32_t widthT = GetObject<uint8_t>() % MAX_SIZE;
    int32_t heightT = GetObject<uint8_t>() % MAX_SIZE;
    Media::InitializationOptions opts;
    opts.size.width = widthT;
    opts.size.height = heightT;
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetObject<int32_t>() % 14); // Max Media::PixelFormat is 14
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetObject<int32_t>() % 14);    // Max Media::PixelFormat is 14
    opts.alphaType = static_cast<Media::AlphaType>(GetObject<int32_t>() % 4);         // Max Media::AlphaType is 4
    opts.scaleMode = static_cast<Media::ScaleMode>(GetObject<int32_t>() % 2);         // Max Media::ScaleMode is 2
    opts.editable = GetObject<bool>();
    opts.useSourceIfMatch = GetObject<bool>();
    auto pixelmap = Media::PixelMap::Create(opts);
    if (pixelmap != nullptr) {
        std::shared_ptr<Image> image = Image::MakeFromYUVAPixmaps(gpuContext, info,
            const_cast<void *>(reinterpret_cast<const void*>(pixelmap->GetPixels())));
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
    OHOS::Rosen::Drawing::BuildImageFuzzTest(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest003(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest004(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest005(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest006(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest007(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest008(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest009(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest010(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest011(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest012(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest013(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest014(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest015(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest016(data, size);
    return 0;
}