/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <cstring>
#include <memory>
#include <sys/mman.h>
#include <vector>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "image_type.h"
#include "parameters.h"
#include "pixel_map.h"
#include "rs_profiler.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_pixelmap.h"
#include "surface_buffer.h"

using OHOS::sptr;
using OHOS::SurfaceBuffer;
using OHOS::Media::AllocatorType;
using OHOS::Media::ImageInfo;
using OHOS::Media::PIXEL_MAP_ERR;
using OHOS::Media::PixelMap;
using OHOS::Media::PixelMemInfo;
using OHOS::Rosen::RSProfiler;

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

constexpr int TEN = 10;
constexpr int HUNDRED = 100;

HWTEST(RSProfilerPixelMapTest, MarshalPixelMap, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    RSProfiler::testing_ = true;
    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }

    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);
    auto successfulMarshaling = RSProfiler::MarshalPixelMap(parcel, map);
    EXPECT_TRUE(successfulMarshaling);
}

HWTEST(RSProfilerPixelMapTest, UnmarshalPixelMap, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    RSProfiler::testing_ = true;

    Parcel parcel;
    auto pixelMap = RSProfiler::UnmarshalPixelMap(parcel, nullptr);

    EXPECT_EQ(pixelMap, nullptr);
}

HWTEST(RSProfilerPixelMapTest, PlaybackParcel, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    RSProfiler::testing_ = true;
    RSProfiler::SetSubMode(SubMode::READ_EMUL);

    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }

    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.allocatorType = AllocatorType::HEAP_ALLOC;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);

    uint8_t parcelMemory[sizeof(Parcel) + 1];
    auto* parcelPtr = new (parcelMemory + 1) Parcel;

    auto successfulMarshaling = RSProfiler::MarshalPixelMap(*parcelPtr, map);
    EXPECT_TRUE(successfulMarshaling);

    auto pixelMap = RSProfiler::UnmarshalPixelMap(*parcelPtr, nullptr);

    EXPECT_NE(pixelMap, nullptr);
    parcelPtr->~Parcel();
}

HWTEST(RSProfilerPixelMapTest, MarshalUnmarshalNstdPixelMap, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    constexpr auto PIXELMAP_STRING_PIXEL_CHECK = "persist.graphic.profiler.pixelcheck";
    OHOS::system::SetParameter(PIXELMAP_STRING_PIXEL_CHECK, "1");
    RSProfiler::testing_ = true;
    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }
 
    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.allocatorType = AllocatorType::HEAP_ALLOC;
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);
    EXPECT_NE(map, nullptr);
    auto successfulMarshaling = RSProfiler::MarshalPixelMap(parcel, map);
    EXPECT_TRUE(successfulMarshaling);
 
    // This will use UnmarshalPixelMapNstd
    auto pixelMapNstd = RSProfiler::UnmarshalPixelMap(parcel, nullptr);
 
    EXPECT_NE(pixelMapNstd, nullptr);
    OHOS::system::SetParameter(PIXELMAP_STRING_PIXEL_CHECK, "0");
}

HWTEST(RSProfilerPixelMapTest, MarshalUnmarshalPixelMap, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    RSProfiler::testing_ = true;
    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }

    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);
    EXPECT_NE(map, nullptr);
    auto successfulMarshaling = RSProfiler::MarshalPixelMap(parcel, map);
    EXPECT_TRUE(successfulMarshaling);

    auto pixelMap = RSProfiler::UnmarshalPixelMap(parcel, nullptr);

    EXPECT_NE(pixelMap, nullptr);
}

class RSProfilerPixelMapStorageTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        OHOS::system::SetParameter("persist.graphic.profiler.enabled", "1");
    }
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override
    {
        ImageCache::Reset();
    }
};

HWTEST_F(RSProfilerPixelMapStorageTest, PushPullHeapPixelMap, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }

    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.allocatorType = AllocatorType::HEAP_ALLOC;
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);
    EXPECT_NE(map, nullptr);
    auto id = ImageCache::New();
    auto successfulPushMap = PixelMapStorage::Push(id, *map);

    EXPECT_TRUE(successfulPushMap);

    auto memory = std::make_shared<std::array<uint8_t, HUNDRED>>();

    ImageInfo imageInfo;
    PixelMemInfo pixelMemInfo = { .base = memory->data(),
        .context = nullptr,
        .bufferSize = sizeof(*memory),
        .allocatorType = AllocatorType::HEAP_ALLOC,
        .isAstc = false,
        .displayOnly = false };
    size_t skipBytes;
    auto successfulPull = PixelMapStorage::Pull(id, imageInfo, pixelMemInfo, skipBytes);

    EXPECT_TRUE(successfulPull);

    auto id2 = ImageCache::New();
    auto successfulPushMemory = PixelMapStorage::Push(id2, imageInfo, pixelMemInfo, skipBytes);

    EXPECT_TRUE(successfulPushMemory);

    ImageInfo defaultImageInfo;
    PixelMemInfo defaultPixelMemInfo;
    size_t defaultSkipBytes;
    auto successfulDefaultPull = PixelMapStorage::Pull(id2, defaultImageInfo, defaultPixelMemInfo, defaultSkipBytes);

    EXPECT_TRUE(successfulDefaultPull);
}

HWTEST_F(RSProfilerPixelMapStorageTest, PushPullSharedPixelMap, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }

    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.allocatorType = AllocatorType::SHARE_MEM_ALLOC;
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);
    EXPECT_NE(map, nullptr);
    auto id = ImageCache::New();
    auto successfulPushMap = PixelMapStorage::Push(id, *map);

    EXPECT_TRUE(successfulPushMap);

    ImageInfo imageInfo;

    PixelMemInfo pixelMemInfo = { .base = reinterpret_cast<uint8_t*>(malloc(HUNDRED)),
        .context = nullptr,
        .bufferSize = HUNDRED,
        .allocatorType = AllocatorType::SHARE_MEM_ALLOC,
        .isAstc = false,
        .displayOnly = false };
    size_t skipBytes;
    auto successfulPull = PixelMapStorage::Pull(id, imageInfo, pixelMemInfo, skipBytes);

    EXPECT_TRUE(successfulPull);
    EXPECT_NE(pixelMemInfo.base, nullptr);
    EXPECT_EQ(pixelMemInfo.bufferSize, HUNDRED);

    auto id2 = ImageCache::New();
    auto successfulPushMemory = PixelMapStorage::Push(id2, imageInfo, pixelMemInfo, skipBytes);

    EXPECT_TRUE(successfulPushMemory);

    ImageInfo defaultImageInfo;
    PixelMemInfo defaultPixelMemInfo;
    size_t defaultSkipBytes;
    auto successfulDefaultPull = PixelMapStorage::Pull(id2, defaultImageInfo, defaultPixelMemInfo, defaultSkipBytes);

    EXPECT_TRUE(successfulDefaultPull);
}

HWTEST_F(RSProfilerPixelMapStorageTest, PushPullDMAPixelMap, TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }

    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.allocatorType = AllocatorType::DMA_ALLOC;
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);
    EXPECT_NE(map, nullptr);
    auto id = ImageCache::New();
    auto successfulPushMap = PixelMapStorage::Push(id, *map);
    std::cout << "Pushed map" << std::endl;

    EXPECT_TRUE(successfulPushMap);

    ImageInfo imageInfo;
    PixelMemInfo pixelMemInfo = { .base = reinterpret_cast<uint8_t*>(malloc(HUNDRED)),
        .context = nullptr,
        .bufferSize = HUNDRED,
        .allocatorType = AllocatorType::DMA_ALLOC,
        .isAstc = false,
        .displayOnly = false };
    size_t skipBytes;
    auto successfulPull = PixelMapStorage::Pull(id, imageInfo, pixelMemInfo, skipBytes);
    std::cout << "Pulled map" << std::endl;

    EXPECT_TRUE(successfulPull);

    auto id2 = ImageCache::New();
    auto successfulPushMemory = PixelMapStorage::Push(id2, imageInfo, pixelMemInfo, skipBytes);
    std::cout << "Pushed memory" << std::endl;

    EXPECT_TRUE(successfulPushMemory);

    ImageInfo defaultImageInfo;
    PixelMemInfo defaultPixelMemInfo;
    size_t defaultSkipBytes;
    auto successfulDefaultPull = PixelMapStorage::Pull(id2, defaultImageInfo, defaultPixelMemInfo, defaultSkipBytes);
    std::cout << "Pulled memory" << std::endl;

    EXPECT_TRUE(successfulDefaultPull);
}

HWTEST_F(RSProfilerPixelMapStorageTest, AlmostFullStorage, TestSize.Level1)
{
    // Constant defined in PixelMapStorage::Fits
    constexpr auto maxConsumption = 1024u * 1024u * 1024u;
    ImageCache::consumption_ = maxConsumption - 50;
    RSProfiler::testing_ = true;
    std::vector<uint32_t> colors(HUNDRED);
    for (int i = 0; i < colors.size(); ++i) {
        colors[i] = 4 * i;
    }

    Media::InitializationOptions options;
    options.size.width = TEN;
    options.size.height = TEN;
    options.srcRowStride = 0;
    options.pixelFormat = Media::PixelFormat::RGBA_8888;
    options.allocatorType = AllocatorType::HEAP_ALLOC;
    Parcel parcel;
    std::shared_ptr<Media::PixelMap> map = Media::PixelMap::Create(colors.data(), colors.size(), options);
    EXPECT_NE(map, nullptr);
    auto id = ImageCache::New();
    auto unsuccessfulPushMap = PixelMapStorage::Push(id, *map);

    EXPECT_FALSE(unsuccessfulPushMap);

    auto memory = std::make_shared<std::array<uint8_t, HUNDRED>>();

    ImageInfo imageInfo;
    PixelMemInfo pixelMemInfo = { .base = memory->data(),
        .context = nullptr,
        .bufferSize = sizeof(*memory),
        .allocatorType = AllocatorType::HEAP_ALLOC,
        .isAstc = false,
        .displayOnly = false };
    size_t skipBytes = 0;
    auto successfulDefaultPullWithFullStorage = PixelMapStorage::Pull(id, imageInfo, pixelMemInfo, skipBytes);

    EXPECT_TRUE(successfulDefaultPullWithFullStorage);

    auto memory_v2 = std::make_shared<std::array<uint8_t, HUNDRED>>();

    PixelMemInfo pixelMemInfo_v2 = { .base = memory_v2->data(),
        .context = nullptr,
        .bufferSize = HUNDRED,
        .allocatorType = AllocatorType::HEAP_ALLOC,
        .isAstc = false,
        .displayOnly = false };

    auto id2 = ImageCache::New();
    auto unsuccessfulPushMemory = PixelMapStorage::Push(id2, imageInfo, pixelMemInfo_v2, skipBytes);

    EXPECT_FALSE(unsuccessfulPushMemory);

    ImageInfo defaultImageInfo;
    PixelMemInfo defaultPixelMemInfo;
    size_t defaultSkipBytes;
    auto successfulDefaultPull = PixelMapStorage::Pull(id2, defaultImageInfo, defaultPixelMemInfo, defaultSkipBytes);

    EXPECT_TRUE(successfulDefaultPull);
}

HWTEST_F(RSProfilerPixelMapStorageTest, CopyImageData, TestSize.Level1)
{
    std::vector<uint8_t> dstImage;
    dstImage.reserve(HUNDRED);
    auto imagePtr = std::make_shared<Image>();
    imagePtr->data.resize(HUNDRED);
    bool successfulCopied = PixelMapStorage::CopyImageData(imagePtr.get(), dstImage.data(), dstImage.capacity());
    EXPECT_TRUE(successfulCopied);
}

HWTEST_F(RSProfilerPixelMapStorageTest, CopyImageDataJpeg, TestSize.Level1)
{
    const int CHANNELS = 3;
    auto jpegImg = std::make_shared<Image>();
    jpegImg->data.resize(sizeof(TextureHeader) + CHANNELS * HUNDRED);

    TextureHeader* header = reinterpret_cast<TextureHeader*>(jpegImg->data.data());

    header->rgbEncodedSize = HUNDRED;
    header->totalOriginalSize = CHANNELS * HUNDRED;
    header->magicNumber = 'JPEG';

    std::vector<uint8_t> dstImage;
    dstImage.reserve(HUNDRED * TEN);

    bool copiedJpeg = PixelMapStorage::CopyImageData(jpegImg.get(), dstImage.data(), dstImage.capacity());
    EXPECT_TRUE(copiedJpeg);
}

HWTEST_F(RSProfilerPixelMapStorageTest, CopyImageDataLZ4, TestSize.Level1)
{
    const int CHANNELS = 3;
    auto xlzz4Img = std::make_shared<Image>();
    xlzz4Img->data.resize(sizeof(TextureHeader) + CHANNELS * HUNDRED);

    TextureHeader* header = reinterpret_cast<TextureHeader*>(xlzz4Img->data.data());

    header->totalOriginalSize = CHANNELS * HUNDRED;
    header->magicNumber = 'XLZ4';

    std::vector<uint8_t> dstImage;
    dstImage.reserve(HUNDRED * TEN);

    bool copiedLz4 = PixelMapStorage::CopyImageData(xlzz4Img.get(), dstImage.data(), dstImage.capacity());
    EXPECT_TRUE(copiedLz4);
}

HWTEST_F(RSProfilerPixelMapStorageTest, CopyImageDataFailing, TestSize.Level1)
{
    std::vector<uint8_t> dstImage;
    dstImage.reserve(HUNDRED);
    auto imagePtr = std::make_shared<Image>();
    imagePtr->data.resize(HUNDRED);
    {
        std::cout << "Copy from null source" << std::endl;
        bool nullSrc = PixelMapStorage::CopyImageData(nullptr, dstImage.data(), dstImage.capacity());
        EXPECT_FALSE(nullSrc);
    }
    {
        std::cout << "Copy to null destination" << std::endl;
        bool nullDst = PixelMapStorage::CopyImageData(imagePtr.get(), nullptr, 100);
        EXPECT_FALSE(nullDst);
    }
    {
        std::cout << "Copy from empty source" << std::endl;
        std::vector<uint8_t> emptySrcImage;
        bool emptySrc = PixelMapStorage::CopyImageData(
            emptySrcImage.data(), emptySrcImage.size(), dstImage.data(), dstImage.capacity());
        EXPECT_FALSE(emptySrc);
    }
    {
        std::cout << "Copy to empty destination" << std::endl;
        std::vector<uint8_t> emptyDstImage;
        bool emptyDst = PixelMapStorage::CopyImageData(imagePtr.get(), emptyDstImage.data(), 0);
        EXPECT_FALSE(emptyDst);
    }
}

HWTEST_F(RSProfilerPixelMapStorageTest, SurfaceBuffer, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer;
    auto surfaceBuffer = PixelMapStorage::IncrementSurfaceBufferReference(buffer);
    EXPECT_EQ(surfaceBuffer, nullptr);
}

} // namespace OHOS::Rosen