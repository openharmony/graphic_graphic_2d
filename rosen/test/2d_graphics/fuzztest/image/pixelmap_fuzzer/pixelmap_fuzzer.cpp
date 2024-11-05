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

#include "pixelmap_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "pixel_map.h"

constexpr uint32_t WIDTH_FACTOR = 4;
constexpr uint32_t FORMAT_LENGTH = 5;
constexpr uint32_t STRING_LENGTH = 10;

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

/*
 * get string from g_data
 */
std::string GetStringFromData()
{
    std::unique_ptr<char[]> strings = std::make_unique<char[]>(STRING_LENGTH);
    if (strings == nullptr) {
        return "";
    }
    for (size_t i = 0; i < STRING_LENGTH; i++) {
        strings[i] = GetData<char>();
    }
    std::string str(strings.get(), STRING_LENGTH);
    return str;
}

/*
 * get parcel from g_data
 */
bool ChangeParcel(Parcel &parcel)
{
    if (!parcel.WriteBuffer(g_data, g_size)) {
        return false;
    }
    return true;
}

/*
 * get a pixelmap from opts
 */
std::unique_ptr<Media::PixelMap> GetPixelMapFromOpts()
{
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    auto pixelmap = Media::PixelMap::Create(opts);
    if (pixelmap == nullptr) {
        return nullptr;
    }
    return pixelmap;
}

/*
 * get a pixelmap from g_data
 */
std::unique_ptr<Media::PixelMap> GetPixelMapFromData()
{
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    size_t datalength = width * height * WIDTH_FACTOR;
    uint32_t* colorData = new uint32_t[datalength];
    for (size_t i = 0; i < width * height; i++) {
        colorData[i] = GetData<uint32_t>();
    }
    auto pixelmap = Media::PixelMap::Create(colorData, datalength, opts);
    if (colorData!=nullptr) {
        delete[] colorData;
        colorData = nullptr;
    }
    if (pixelmap == nullptr) {
        return nullptr;
    }

    return pixelmap;
}

/*
 * get yuv info
 */
Media::YUVDataInfo GetYUVDataInfo()
{
    Media::YUVDataInfo yuvInfo;
    Media::Size imageSize;
    imageSize.width = GetData<int32_t>();
    imageSize.height = GetData<int32_t>();
    yuvInfo.imageSize = imageSize;
    yuvInfo.yWidth = GetData<uint32_t>();
    yuvInfo.yHeight = GetData<uint32_t>();
    yuvInfo.uvWidth = GetData<uint32_t>();
    yuvInfo.uvHeight = GetData<uint32_t>();
    yuvInfo.yStride = GetData<uint32_t>();
    yuvInfo.uStride = GetData<uint32_t>();
    yuvInfo.vStride = GetData<uint32_t>();
    yuvInfo.uvStride = GetData<uint32_t>();
    yuvInfo.yOffset = GetData<uint32_t>();
    yuvInfo.uOffset = GetData<uint32_t>();
    yuvInfo.vOffset = GetData<uint32_t>();
    yuvInfo.uvOffset = GetData<uint32_t>();
    return yuvInfo;
}

/*
 * get yuv stride info
 */
Media::YUVStrideInfo GetYUVStrideInfo()
{
    Media::YUVStrideInfo yuvStrideInfo;
    yuvStrideInfo.yStride = GetData<uint32_t>();
    yuvStrideInfo.uvStride = GetData<uint32_t>();
    yuvStrideInfo.yOffset = GetData<uint32_t>();
    yuvStrideInfo.uvOffset = GetData<uint32_t>();
    return yuvStrideInfo;
}

/*
 * test pixelmap setImageInfo
 */
bool g_pixelMapSetImageInfoTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    Media::ImageInfo imageInfo;
    Media::Size infoSize;
    infoSize.width = GetData<int32_t>();
    infoSize.height = GetData<int32_t>();
    imageInfo.size = infoSize;
    imageInfo.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    imageInfo.colorSpace = static_cast<Media::ColorSpace>(GetData<int32_t>());
    imageInfo.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    imageInfo.baseDensity = GetData<int32_t>();
    char* encodedFormat = new char[FORMAT_LENGTH];
    if (encodedFormat == nullptr) {
        return false;
    }
    for (size_t i = 0; i < FORMAT_LENGTH; i++) {
        encodedFormat[i] = GetData<char>();
    }
    std::string str(encodedFormat);
    if (encodedFormat != nullptr) {
        delete [] encodedFormat;
        encodedFormat = nullptr;
    }
    imageInfo.encodedFormat = str;
    pixelMap->SetImageInfo(imageInfo, true);
    Media::YUVDataInfo yuvInfo = GetYUVDataInfo();
    pixelMap->SetImageYUVInfo(yuvInfo);
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Media::PixelFormat pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    pixelMap->AssignYuvDataOnType(pixelFormat, width, height);
    Media::YUVStrideInfo strides = GetYUVStrideInfo();
    pixelMap->UpdateYUVDataInfo(pixelFormat, width, height, strides);
    return true;
}

/*
 * test pixelmap GetPixels
 */
bool PixelMapGetPixelsTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    const uint8_t* pixel = pixelMap->GetPixel(width, height);
    if (pixel == nullptr) {
        return false;
    }
    const uint8_t* pixel8 = pixelMap->GetPixel8(width, height);
    if (pixel8 == nullptr) {
        return false;
    }
    const uint16_t* pixel16 = pixelMap->GetPixel16(width, height);
    if (pixel16 == nullptr) {
        return false;
    }
    const uint32_t* pixel32 = pixelMap->GetPixel32(width, height);
    if (pixel32 == nullptr) {
        return false;
    }
    void* writablePixels = pixelMap->GetWritablePixels();
    if (writablePixels == nullptr) {
        return false;
    }
    const uint8_t* pixels = pixelMap->GetPixels();
    if (pixels == nullptr) {
        return false;
    }
    uint32_t color;
    if (!(pixelMap->GetARGB32Color(width, height, color))) {
        return false;
    }
    uint8_t colorA = pixelMap->GetARGB32ColorA(color);
    uint8_t colorR = pixelMap->GetARGB32ColorR(color);
    uint8_t colorG = pixelMap->GetARGB32ColorG(color);
    uint8_t colorB = pixelMap->GetARGB32ColorB(color);
    uint32_t combinedColor = (static_cast<uint32_t>(colorA) << 24) |
                            (static_cast<uint32_t>(colorR) << 16) |
                            (static_cast<uint32_t>(colorG) << 8) |
                            static_cast<uint32_t>(colorB);
    if (combinedColor != color) {
        return false;
    }
    return true;
}

/*
 * test pixelmap ReadPixels
 */
bool PixelMapReadPixelsTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    uint64_t bufferSize = GetData<uint64_t>();
    uint32_t offset = GetData<uint32_t>();
    uint32_t stride = GetData<uint32_t>();
    Media::Rect rect;
    rect.left = GetData<int32_t>();
    rect.top = GetData<int32_t>();
    rect.width = GetData<int32_t>();
    rect.height = GetData<int32_t>();
    std::unique_ptr<uint8_t[]> dst = std::make_unique<uint8_t[]>(pixelMap->GetByteCount());
    if (dst == nullptr) {
        return false;
    }
    pixelMap->ReadPixels(bufferSize, offset, stride, rect, dst.get());
    pixelMap->ReadPixels(bufferSize, dst.get());
    pixelMap->ReadARGBPixels(bufferSize, dst.get());
    Media::Position position;
    position.x = GetData<int32_t>();
    position.y = GetData<int32_t>();
    uint32_t color;
    pixelMap->ReadPixel(position, color);
    return true;
}

/*
 * test pixelmap WritePixels
 */
bool PixelMapWritePixelsTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    Media::Position position;
    position.x = GetData<int32_t>();
    position.y = GetData<int32_t>();
    uint32_t color = GetData<uint32_t>();
    pixelMap->WritePixel(position, color);
    pixelMap->WritePixels(color);
    std::unique_ptr<uint8_t[]> src = std::make_unique<uint8_t[]>(pixelMap->GetByteCount());
    if (src == nullptr) {
        return false;
    }
    uint64_t bufferSize = GetData<uint64_t>();
    uint32_t offset = GetData<uint32_t>();
    uint32_t stride = GetData<uint32_t>();
    Media::Rect rect;
    rect.left = GetData<int32_t>();
    rect.top = GetData<int32_t>();
    rect.width = GetData<int32_t>();
    rect.height = GetData<int32_t>();
    pixelMap->WritePixels(src.get(), bufferSize, offset, stride, rect);
    pixelMap->WritePixels(src.get(), bufferSize);
    return true;
}

/*
 * test pixelmap transform
 */
bool PixelMapTransformTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    float scaleX = std::fmod(GetData<float>(), 1.0f);
    float scaleY = std::fmod(GetData<float>(), 1.0f);
    Media::AntiAliasingOption antiAliasingOption = static_cast<Media::AntiAliasingOption>(GetData<uint32_t>());
    pixelMap->scale(scaleX, scaleY, antiAliasingOption);
    float rotateDegrees = GetData<float>();
    pixelMap->rotate(rotateDegrees);
    float translateX = std::fmod(GetData<float>(), 10.0f);
    float translateY = std::fmod(GetData<float>(), 10.0f);
    pixelMap->translate(translateX, translateY);
    bool flipX = GetData<bool>();
    bool flipY = GetData<bool>();
    pixelMap->flip(flipX, flipY);
    Media::Rect rect;
    rect.left = GetData<int32_t>();
    rect.top = GetData<int32_t>();
    rect.width = GetData<int32_t>();
    rect.height = GetData<int32_t>();
    pixelMap->crop(rect);
    float percent = std::fmod(GetData<float>(), 1.0f);
    pixelMap->SetAlpha(percent);
    pixelMap->ToSdr();
    Media::PixelFormat pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    bool toSRGB = GetData<bool>();
    pixelMap->ToSdr(pixelFormat, toSRGB);
    return true;
}

/*
 * test pixelmap set image properties
 */
bool PixelMapSetImagePropertiesTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    // test set ImageInfo
    g_pixelMapSetImageInfoTest(pixelMap);
    // test write Pixels
    PixelMapWritePixelsTest(pixelMap);
    Media::Size size;
    size.width = GetData<int32_t>();
    size.height = GetData<int32_t>();
    Media::PixelFormat pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    pixelMap->ResetConfig(size, pixelFormat);
    Media::AlphaType alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    pixelMap->SetAlphaType(alphaType);
    std::string memoryName = GetStringFromData();
    pixelMap->SetMemoryName(memoryName);
    return true;
}

/*
 * test pixelmap get image properties
 */
bool PixelMapGetImagePropertiesTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    // test GetPixels
    if (!PixelMapGetPixelsTest(pixelMap)) {
        return false;
    }
    Media::ImageInfo imageInfo;
    pixelMap->GetImageInfo(imageInfo);
    if (pixelMap->GetPixelBytes() <= 0 ||
        pixelMap->GetRowBytes() <= 0 ||
        pixelMap->GetByteCount() <= 0 ||
        pixelMap->GetWidth() <= 0 ||
        pixelMap->GetHeight() <= 0 ||
        pixelMap->GetCapacity() <= 0 ||
        pixelMap->GetRowStride() <= 0 ||
        pixelMap->GetUniqueId() < 0) {
        return false;
    }
    if (imageInfo.pixelFormat == Media::PixelFormat::ASTC_4x4 ||
        imageInfo.pixelFormat == Media::PixelFormat::ASTC_6x6 ||
        imageInfo.pixelFormat == Media::PixelFormat::ASTC_8x8) {
        if (!pixelMap->IsAstc()) {
            return false;
        }
        Media::Size astcSize;
        pixelMap->GetAstcRealSize(astcSize);
        if (astcSize.width == 0 || astcSize.height == 0) {
            return false;
        }
        Media::TransformData transformData;
        pixelMap->GetTransformData(transformData);
    }
    if (pixelMap->GetBaseDensity() < 0) {
        return false;
    }
    // test ReadPixels
    PixelMapReadPixelsTest(pixelMap);
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::SHARE_MEM_ALLOC ||
        pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        if (!(pixelMap->GetFd())) {
            return false;
        }
    }
    std::string imagePropertyKey = GetStringFromData();
    int32_t intValue;
    std::string strValue;
    pixelMap->GetImagePropertyInt(imagePropertyKey, intValue);
    pixelMap->GetImagePropertyString(imagePropertyKey, strValue);
    Media::YUVDataInfo yuvDataInfo;
    pixelMap->GetImageYUVInfo(yuvDataInfo);
    return true;
}

/*
 * test all pixelmap interfaces
 */
bool PixelMapInterfaceTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    // test get Image Properties
    PixelMapGetImagePropertiesTest(pixelMap);
    // test get pixels
    PixelMapGetPixelsTest(pixelMap);
    // test set Image Properties
    PixelMapSetImagePropertiesTest(pixelMap);
    // test get Image Properties again
    PixelMapGetImagePropertiesTest(pixelMap);
    return true;
}

/*
 * test pixelmap IPC interface
 */
bool PixelMapIPCTest(std::unique_ptr<Media::PixelMap> &pixelMap)
{
    // test parcel pixelmap
    Parcel parcel;
    pixelMap->Marshalling(parcel);
    ChangeParcel(parcel);
    Media::PixelMap* unmarshallingPixelMap = Media::PixelMap::Unmarshalling(parcel);
    if (!unmarshallingPixelMap) {
        return false;
    }
    std::unique_ptr<Media::PixelMap> parcelPixelMap = std::unique_ptr<Media::PixelMap>(unmarshallingPixelMap);
    PixelMapInterfaceTest(parcelPixelMap);
    parcelPixelMap->Marshalling(parcel);

    // test tlv pixelmap
    std::vector<uint8_t> buff(g_data, g_data + g_size);
    Media::PixelMap* decodePixelMap = Media::PixelMap::DecodeTlv(buff);
    if (!decodePixelMap) {
        return false;
    }
    std::unique_ptr<Media::PixelMap> tlvPixelMap = std::unique_ptr<Media::PixelMap>(decodePixelMap);
    PixelMapInterfaceTest(tlvPixelMap);
    tlvPixelMap->EncodeTlv(buff);
    return true;
}

bool PixelMapMainFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // creatye from opts
    std::unique_ptr<Media::PixelMap> pixelMapFromOpts = GetPixelMapFromOpts();
    if (!pixelMapFromOpts) {
        return false;
    }
    PixelMapInterfaceTest(pixelMapFromOpts);

    // creatye from data
    std::unique_ptr<Media::PixelMap> pixelMapFromData = GetPixelMapFromData();
    if (!pixelMapFromData) {
        return false;
    }
    PixelMapInterfaceTest(pixelMapFromData);

    PixelMapIPCTest(pixelMapFromOpts);
    PixelMapIPCTest(pixelMapFromData);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::PixelMapMainFuzzTest(data, size);
    return 0;
}