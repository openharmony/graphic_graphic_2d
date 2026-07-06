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

#include "draw_image_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <securec.h>

#include "draw/canvas.h"
#include "draw/surface.h"
#include "get_object.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "image/image_info.h"
#include "image/pixmap.h"
#ifdef RS_ENABLE_GPU
#include "image/gpu_context.h"
#include "image/yuv_info.h"
#include "draw/surface.h"
#endif
#include "effect/color_space.h"
#include "utils/data.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t MAX_SIZE = 1000;
constexpr size_t COLORTYPE_NUM = 10;
constexpr size_t ALPHATYPE_NUM = 4;
} // namespace

namespace Drawing {
/*
 *  测试使用 Bitmap 构建 Image 的功能，包括:
 *  1. Image::BuildFromBitmap()
 */
bool ImageFuzzTest000(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    if (!bitmap.Build(width, height, bitmapFormat)) {
        return false;
    }
    if (bitmap.GetWidth() != width || bitmap.GetHeight() != height) {
        return false;
    }
    Image image1;
    image1.BuildFromBitmap(bitmap);

    uint32_t colorType = GetObject<uint32_t>();
    uint32_t alphaType = GetObject<uint32_t>();
    size_t rowBytes = GetObject<size_t>() % MAX_SIZE;
    uint32_t strSize = GetObject<uint32_t>() % MAX_SIZE + 1;
    Pixmap pix;
    ImageInfo imageInfo = ImageInfo(width, height, static_cast<ColorType>(colorType % COLORTYPE_NUM),
        static_cast<AlphaType>(alphaType % ALPHATYPE_NUM));
    std::vector<char> addr(strSize);
    for (size_t i = 0; i < strSize; i++) {
        addr[i] = GetObject<char>();
    }
    addr[strSize - 1] = '\0';
    Pixmap pixmap(imageInfo, addr.data(), rowBytes);
    Image image2;
    image2.MakeFromRaster(pixmap, nullptr, nullptr);
    return true;
}

/*
 *  测试 Image 的基本功能，包括:
 *  1. GetHeadroom()
 *  2. SetHeadroom(float)
 */
bool ImageFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    Image image;
    Bitmap bitmap;
    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    float headroom = GetObject<int>() / 2.0f;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    if (!bitmap.Build(width, height, bitmapFormat)) {
        return false;
    }
    if (bitmap.GetWidth() != width || bitmap.GetHeight() != height) {
        return false;
    }
    image.BuildFromBitmap(bitmap);
    image.AsLegacyBitmap(bitmap);
    image.SetHeadroom(headroom);
    image.GetHeadroom();
    GPUContext gpuContext;
#ifdef RS_ENABLE_VK
    skgpu::VulkanBackendContext grVkBackendContext {};
    gpuContext.BuildFromVK(grVkBackendContext);
#endif
    OH_NativeBuffer *NativeBuffer = static_cast<OH_NativeBuffer *>(bitmap.GetPixels());
    std::shared_ptr<Drawing::Data> imageData = std::make_shared<Drawing::Data>();
    imageData->BuildFromOHNativeBuffer(NativeBuffer, GetObject<size_t>() % MAX_SIZE);
    std::shared_ptr<Image> image2 = std::make_shared<Image>();
    image2->BuildFromCompressed(gpuContext, imageData, GetObject<size_t>(), GetObject<size_t>(),
        CompressedType::ASTC_RGBA8_4x4);
    Drawing::Image image3;
    Drawing::TextureInfo info;
    info.SetWidth(GetObject<int>());
    auto colorSpace = std::make_shared<Drawing::ColorSpace>(Drawing::ColorSpace::ColorSpaceType::NO_TYPE);
    image3.BuildFromTexture(gpuContext, info, Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, colorSpace);
    Image image4;
    std::shared_ptr<Image> img = std::make_shared<Image>();
    if (img != nullptr) {
        image4.BuildSubset(img, Drawing::RectI(GetObject<int32_t>(), GetObject<int32_t>(), GetObject<int32_t>(),
            GetObject<int32_t>()), gpuContext);
    }
    TextureOrigin origin = TextureOrigin::TOP_LEFT;
    auto textureInfo = image.GetBackendTexture(false, &origin);
    textureInfo = image2->GetBackendTexture(false, &origin);
    textureInfo = image3.GetBackendTexture(false, &origin);
    textureInfo = image4.GetBackendTexture(false, &origin);
    image.EncodeToData(EncodedImageFormat::PNG, GetObject<int>());
    return true;
}

/*
 *  测试从 Image 缩放的功能:
 *  1. Image::BuildFromBitmap()
 *  2. Image::ScaleImage()
 */
bool ImageFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    Bitmap srcBitmap;
    int srcWidth = GetObject<int>() % MAX_SIZE;
    int srcHeight = GetObject<int>() % MAX_SIZE;
    if (!srcBitmap.Build(srcWidth, srcHeight, bitmapFormat)) {
        return false;
    }
    if (srcBitmap.GetWidth() != srcWidth || srcBitmap.GetHeight() != srcHeight) {
        return false;
    }
    std::shared_ptr<Image> srcImage = std::make_shared<Image>();
    srcImage->BuildFromBitmap(srcBitmap);
    Bitmap dstBitmap;
    int dstWidth = GetObject<int>() % MAX_SIZE;
    int dstHeight = GetObject<int>() % MAX_SIZE;
    if (!dstBitmap.Build(dstWidth, dstHeight, bitmapFormat)) {
        return false;
    }
    if (dstBitmap.GetWidth() != dstWidth || dstBitmap.GetHeight() != dstHeight) {
        return false;
    }
    std::shared_ptr<Image> dstImage = std::make_shared<Image>();
    dstImage->BuildFromBitmap(dstBitmap);

    ScalingType type = static_cast<ScalingType>(static_cast<uint32_t>(data[0]) %
        static_cast<uint32_t>(ScalingType::OPTION_INVALID));
    ScalingOption option = {RectI(0, 0, srcWidth, srcHeight), RectI(0, 0, dstWidth, dstHeight), type};
    Image::ScaleImage(srcImage, dstImage, option);
    return true;
}

/*
 *  测试 Image(void* rawImg) 构造函数以及非 GPU 方法，包括:
 *  1. Image::Image(void* rawImg)
 *  2. Image::ReadPixels(Bitmap&, int, int)
 *  3. Image::ReadPixels(Pixmap&, int, int)
 *  4. Image::ReadPixels(ImageInfo, void*, size_t, int32_t, int32_t)
 *  5. Image::ScalePixels()
 *  6. Image::EncodeToData()
 */
bool ImageFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    Image image;

    Bitmap bitmap;
    int width = GetObject<int>() % MAX_SIZE + 1;
    int height = GetObject<int>() % MAX_SIZE + 1;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    image.BuildFromBitmap(bitmap);

    image.ReadPixels(bitmap, GetObject<int>(), GetObject<int>());

    Pixmap pixmap(bitmap.GetImageInfo(), bitmap.GetPixels(), bitmap.GetRowBytes());
    image.ReadPixels(pixmap, GetObject<int>(), GetObject<int>());

    Bitmap scaleBitmap;
    int scaleWidth = GetObject<int>() % MAX_SIZE + 1;
    int scaleHeight = GetObject<int>() % MAX_SIZE + 1;
    scaleBitmap.Build(scaleWidth, scaleHeight, bitmapFormat);
    SamplingOptions sampling;
    image.ScalePixels(scaleBitmap, sampling, GetObject<bool>());

    EncodedImageFormat format = static_cast<EncodedImageFormat>(
        static_cast<uint32_t>(GetObject<uint32_t>()) % static_cast<uint32_t>(EncodedImageFormat::UNKNOWN));
    image.EncodeToData(format, GetObject<int>());

    return true;
}

#ifdef RS_ENABLE_GPU
/*
 *  测试 Image 的 GPU 相关方法，包括:
 *  1. Image::MakeFromYUVAPixmaps()
 *  2. Image::BuildFromBitmap(GPUContext&)
 *  3. Image::MakeFromEncoded()
 *  4. Image::BuildFromCompressed()
 *  5. Image::BuildFromSurface()
 *  6. Image::BuildFromTexture()
 *  7. Image::BuildSubset()
 *  8. Image::GetBackendTexture()
 *  9. Image::IsValid(GPUContext*)
 */
bool ImageFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    GPUContext gpuContext;

    Bitmap bitmap;
    int width = GetObject<int>() % MAX_SIZE + 1;
    int height = GetObject<int>() % MAX_SIZE + 1;
    BitmapFormat bitmapFormat = { COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL };
    bool buildBitmap = bitmap.Build(width, height, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(gpuContext, bitmap, GetObject<bool>());

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    auto encodeData = std::make_shared<Data>();
    auto dataText = std::make_unique<char[]>(length);
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    encodeData->BuildWithCopy(dataText.get(), length - 1);
    image.MakeFromEncoded(encodeData);

    CompressedType compressedType = static_cast<CompressedType>(
        static_cast<uint32_t>(GetObject<uint32_t>()) % static_cast<uint32_t>(CompressedType::ASTC_RGBA10_4x4));
    auto compressedData = std::make_shared<Data>();
    compressedData->BuildWithCopy(dataText.get(), length - 1);
    image.BuildFromCompressed(gpuContext, compressedData, GetObject<int>(), GetObject<int>(),
        compressedType, nullptr);

    Surface surface;
    image.BuildFromSurface(gpuContext, surface,
        static_cast<TextureOrigin>(GetObject<uint32_t>() % DATA_MIN_SIZE), bitmapFormat, nullptr);

    TextureInfo textureInfo;
    textureInfo.SetWidth(GetObject<int>());
    textureInfo.SetHeight(GetObject<int>());
    textureInfo.SetTarget(GetObject<unsigned int>());
    textureInfo.SetID(GetObject<unsigned int>());
    textureInfo.SetFormat(GetObject<unsigned int>());
    image.BuildFromTexture(gpuContext, textureInfo,
        static_cast<TextureOrigin>(GetObject<uint32_t>() % DATA_MIN_SIZE), bitmapFormat, nullptr);

    std::shared_ptr<Image> subsetImage = std::make_shared<Image>();
    subsetImage->BuildFromBitmap(bitmap);
    RectI rect(GetObject<int>(), GetObject<int>(), GetObject<int>(), GetObject<int>());
    image.BuildSubset(subsetImage, rect, gpuContext);

    image.IsValid(&gpuContext);
    image.IsValid(nullptr);

    return true;
}
#endif

bool ImageFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    GPUContext gpuContext;

    Bitmap bitmap;
    int width = GetObject<int>() % MAX_SIZE;
    int height = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_RGBA_8888, ALPHATYPE_PREMUL };
    if (!bitmap.Build(width, height, bitmapFormat)) {
        return false;
    }
    if (bitmap.GetWidth() != width || bitmap.GetHeight() != height) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(gpuContext, bitmap, GetObject<bool>());
    Bitmap dst;
    image.GetROPixels(dst);
    YUVInfo yuvInfo = {
        GetObject<int>(),
        GetObject<int>(),
        YUVInfo::PlaneConfig::Y_UV,
        YUVInfo::SubSampling::K420,
        YUVInfo::YUVColorSpace::JPEG_FULL_YUVCOLORSPACE,
        YUVInfo::YUVDataType::UNORM_8,
    };
    Image::MakeFromYUVAPixmaps(gpuContext, yuvInfo, bitmap.GetPixels());
    ImageInfo info1 = ImageInfo::MakeN32Premul(GetObject<int32_t>() % MAX_SIZE, GetObject<int32_t>() % MAX_SIZE);
    size_t pixelsSize = GetObject<size_t>() % MAX_SIZE + 1;
    auto pixelsBuf = std::make_unique<uint8_t[]>(pixelsSize);
    for (size_t i = 0; i < pixelsSize; i++) {
        pixelsBuf[i] = GetObject<uint8_t>();
    }
    std::shared_ptr<Drawing::Data> imageData = std::make_shared<Drawing::Data>();
    imageData->BuildWithCopy(pixelsBuf.get(), pixelsSize);
    auto image2 = Image::MakeRasterData(info1, imageData, GetObject<size_t>() % MAX_SIZE);
    ImageInfo imageInfo(GetObject<int>() % MAX_SIZE, GetObject<int>() % MAX_SIZE,
        COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE);
    if (!dst.Build(imageInfo)) {
        return false;
    }
    image.ReadPixels(imageInfo, dst.GetPixels(), dst.GetRowBytes(), GetObject<int32_t>(), GetObject<int32_t>());
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::ImageFuzzTest000(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ImageFuzzTest003(data, size);
#ifdef RS_ENABLE_GPU
    OHOS::Rosen::Drawing::ImageFuzzTest004(data, size);
#endif
    OHOS::Rosen::Drawing::ImageFuzzTest005(data, size);
    return 0;
}