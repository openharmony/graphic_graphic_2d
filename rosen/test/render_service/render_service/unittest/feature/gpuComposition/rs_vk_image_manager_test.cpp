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

#include "gtest/gtest.h"
#include "surface_buffer_impl.h"
#include "feature/gpuComposition/rs_vk_image_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSVKImageManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSVkImageManager> vkImageManager_ = nullptr;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    sptr<SyncFence> BufferFence_ = SyncFence::INVALID_FENCE;
    const int32_t fakeTid_ = 1;
};

static sptr<SurfaceBuffer> CreateBuffer()
{
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    if (!buffer) {
        return nullptr;
    }
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 100,
        .strideAlignment = 0x8, // set 0x8 as default value to alloc SurfaceBufferImpl
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888, // PixelFormat
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = buffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        return nullptr;
    }
    return buffer;
}

void RSVKImageManagerTest::SetUpTestCase() {}
void RSVKImageManagerTest::TearDownTestCase() {}
void RSVKImageManagerTest::SetUp()
{
    RsVulkanContext::SetRecyclable(false);
    buffer_ = CreateBuffer();
    vkImageManager_ = std::make_shared<RSVkImageManager>();
}
void RSVKImageManagerTest::TearDown() {}

/**
 * @tc.name: MapAndUnMapVKImage001
 * @tc.desc: Map and UnMap VKImage, check CacheSeqs size
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVKImage001, TestSize.Level1)
{
    auto size = vkImageManager_->imageCacheSeqs_.size();
    auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        new SurfaceBufferImpl(), SyncFence::INVALID_FENCE, fakeTid_);
    EXPECT_EQ(image, nullptr);
    EXPECT_EQ(size, vkImageManager_->imageCacheSeqs_.size());

    image = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);
    EXPECT_NE(image, nullptr);
    EXPECT_EQ(size + 1, vkImageManager_->imageCacheSeqs_.size());
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetSeqNum());
    EXPECT_EQ(size, vkImageManager_->imageCacheSeqs_.size());
}

/**
 * @tc.name: MapAndUnMapVKImage002
 * @tc.desc: Map 10 VKImages, check cacheSeqs size is 10
 *           UnMap all VKImage, check cacheSeqs size is 0
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVKImage002, TestSize.Level1)
{
    const uint32_t cacheNums = 10;
    uint32_t bufferSeqNums[10] = { 0 };
    for (uint32_t i = 1; i <= cacheNums; i++) {
        auto buffer = CreateBuffer();
        ASSERT_NE(buffer, nullptr);

        bufferSeqNums[i - 1] = buffer->GetSeqNum();
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(
            buffer, SyncFence::INVALID_FENCE, fakeTid_);
        EXPECT_NE(imageCache, nullptr);
        EXPECT_EQ(i, vkImageManager_->imageCacheSeqs_.size());
    }

    std::string dumpString = "";
    vkImageManager_->DumpVkImageInfo(dumpString);
    EXPECT_NE(dumpString, "");

    for (uint32_t i = cacheNums; i >= 1; i--) {
        vkImageManager_->UnMapImageFromSurfaceBuffer(bufferSeqNums[i - 1]);
        EXPECT_EQ(i - 1, vkImageManager_->imageCacheSeqs_.size());
    }
}

/**
 * @tc.name: MapAndUnMapVKImage003
 * @tc.desc: Map 50 VKImages, check cacheSeqs size is not over MAX_CACHE_SIZE(40)
 *           UnMap all VKImage, check cacheSeqs size is 0
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVKImage003, TestSize.Level1)
{
    const uint32_t cacheNums = 50;
    uint32_t bufferSeqNums[50] = { 0 };
    const uint32_t MAX_CACHE_SIZE = 40;
    for (uint32_t i = 1; i <= cacheNums; i++) {
        auto buffer = CreateBuffer();
        ASSERT_NE(buffer, nullptr);

        bufferSeqNums[i - 1] = buffer->GetSeqNum();
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(
            buffer, SyncFence::INVALID_FENCE, fakeTid_);
        EXPECT_NE(imageCache, nullptr);
        if (i <= MAX_CACHE_SIZE) {
            EXPECT_EQ(i, vkImageManager_->imageCacheSeqs_.size());
        } else {
            EXPECT_EQ(MAX_CACHE_SIZE + 1, vkImageManager_->imageCacheSeqs_.size());
        }
    }

    std::string dumpString = "";
    vkImageManager_->DumpVkImageInfo(dumpString);
    EXPECT_NE(dumpString, "");

    for (uint32_t i = cacheNums; i >= 1; i--) {
        vkImageManager_->UnMapImageFromSurfaceBuffer(bufferSeqNums[i - 1]);
    }
    EXPECT_EQ(0, vkImageManager_->imageCacheSeqs_.size());
}

/**
 * @tc.name: MapAndUnMapVKImage004
 * @tc.desc: Map 10 VKImages and map them again, check imageCacheSeqs size
 *           UnMap all VKImage, check cacheSeqs size is 0
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVKImage004, TestSize.Level1)
{
    const uint32_t cacheNums = 10;
    std::vector<sptr<SurfaceBuffer>> bufferVector;
    for (uint32_t i = 1; i <= cacheNums; i++) {
        auto buffer = CreateBuffer();
        ASSERT_NE(buffer, nullptr);
        bufferVector.push_back(buffer);
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(
            buffer, SyncFence::INVALID_FENCE, fakeTid_);
        EXPECT_NE(imageCache, nullptr);
        EXPECT_EQ(i, vkImageManager_->imageCacheSeqs_.size());
    }

    for (uint32_t i = 1; i <= cacheNums; i++) {
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(
            bufferVector[i - 1], SyncFence::INVALID_FENCE, fakeTid_);
        EXPECT_NE(imageCache, nullptr);
        EXPECT_EQ(cacheNums, vkImageManager_->imageCacheSeqs_.size());
    }

    for (uint32_t i = cacheNums; i >= 1; i--) {
        vkImageManager_->UnMapImageFromSurfaceBuffer(bufferVector[i - 1]->GetSeqNum());
    }
    EXPECT_EQ(0, vkImageManager_->imageCacheSeqs_.size());
}

/**
 * @tc.name: MapAndUnMapVKImage005
 * @tc.desc: Map a vkimage with drawingSurface or invalid drawingSurface
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVKImage005, TestSize.Level1)
{
    auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        buffer_, nullptr, fakeTid_, nullptr);
    EXPECT_NE(image, nullptr);
    image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        buffer_, BufferFence_, fakeTid_, nullptr);
    EXPECT_NE(image, nullptr);
    image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        nullptr, BufferFence_, fakeTid_, nullptr);
    EXPECT_EQ(image, nullptr);
    image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        nullptr, nullptr, fakeTid_, nullptr);
    EXPECT_EQ(image, nullptr);

    auto drawingSurface = std::make_unique<Drawing::Surface>();
    EXPECT_NE(drawingSurface, nullptr);
    if (drawingSurface) {
        image = vkImageManager_->MapVkImageFromSurfaceBuffer(
            buffer_, BufferFence_, fakeTid_, drawingSurface.get());
        EXPECT_NE(image, nullptr);
        image = vkImageManager_->MapVkImageFromSurfaceBuffer(
            buffer_, nullptr, fakeTid_, drawingSurface.get());
        EXPECT_NE(image, nullptr);
    }

    auto size = vkImageManager_->imageCacheSeqs_.size();
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetSeqNum());
    EXPECT_EQ(size - 1, vkImageManager_->imageCacheSeqs_.size());
}

/**
 * @tc.name: MapAndUnMapVKImage006
 * @tc.desc: Map a vkimage with invalid vkdevice and fence
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVKImage006, TestSize.Level1)
{
    VkDevice device = RsVulkanContext::GetSingleton().GetRsVulkanInterface().device_;
    auto drawingSurface = std::make_unique<Drawing::Surface>();
    EXPECT_NE(drawingSurface, nullptr);
    if (drawingSurface) {
        RsVulkanContext::GetSingleton().GetRsVulkanInterface().device_ = VK_NULL_HANDLE;
        auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(
            buffer_, BufferFence_, fakeTid_, drawingSurface.get());
        EXPECT_EQ(image, nullptr); // Map fail due to vkdevice is null
    }
    RsVulkanContext::GetSingleton().GetRsVulkanInterface().device_ = device;
    auto size = vkImageManager_->imageCacheSeqs_.size();
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetSeqNum());
    EXPECT_EQ(size, vkImageManager_->imageCacheSeqs_.size());

    sptr<SyncFence> tempFence = new SyncFence(-1);
    auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        buffer_, tempFence, fakeTid_, drawingSurface.get());
    EXPECT_NE(image, nullptr);
    size = vkImageManager_->imageCacheSeqs_.size();
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetSeqNum());
    EXPECT_EQ(size - 1, vkImageManager_->imageCacheSeqs_.size());
}

/**
 * @tc.name: CreateImageCacheFromBuffer001
 * @tc.desc: call CreateImageCacheFromBuffer, check Image will be created
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, CreateImageCacheFromBuffer001, TestSize.Level1)
{
    EXPECT_NE(vkImageManager_->CreateImageCacheFromBuffer(buffer_, BufferFence_), nullptr);

    // invalid buffer
    EXPECT_EQ(vkImageManager_->CreateImageCacheFromBuffer(
        new SurfaceBufferImpl(), SyncFence::INVALID_FENCE), nullptr);
}

/**
 * @tc.name: CreateTest
 * @tc.desc: CreateTest
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, CreateTest, TestSize.Level1)
{
    std::shared_ptr<RSImageManager> imageManager;
    std::shared_ptr<RenderContext> renderContext = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_VK
    imageManager = RSImageManager::Create(renderContext);
    ASSERT_NE(imageManager, nullptr);
#endif // RS_ENABLE_VK
#ifdef RS_ENABLE_GL
    imageManager = RSImageManager::Create(renderContext);
    ASSERT_NE(imageManager, nullptr);
#endif // RS_ENABLE_GL
}

/**
 * @tc.name: CreateImageFromBufferTest
 * @tc.desc: CreateImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, CreateImageFromBufferTest, TestSize.Level1)
{
    int canvasHeight = 10;
    int canvasWidth = 10;
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(canvasHeight, canvasWidth);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    uint32_t threadIndex = 0;
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSVkImageManager>();
    auto res = imageManager->CreateImageFromBuffer(*canvas, buffer, acquireFence, threadIndex, drawingColorSpace);
    EXPECT_EQ(res, nullptr);

    buffer = SurfaceBuffer::Create();
    res = imageManager->CreateImageFromBuffer(*canvas, buffer, acquireFence, threadIndex, drawingColorSpace);
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name: GetIntersectImageTest
 * @tc.desc: GetIntersectImage
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, GetIntersectImageTest, TestSize.Level1)
{
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSVkImageManager>();
    Drawing::RectI imgCutRect = Drawing::RectI{0, 0, 10, 10};
    std::shared_ptr<Drawing::GPUContext> context = std::make_shared<Drawing::GPUContext>();
    sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    pid_t threadIndex = 0;
    buffer = SurfaceBuffer::Create();
    auto res = imageManager->GetIntersectImage(imgCutRect, context, buffer, acquireFence, threadIndex);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: NewImageCacheFromBufferTest
 * @tc.desc: NewImageCacheFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVkImageManagerTest, NewImageCacheFromBufferTest, TestSize.Level1)
{
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSVkImageManager>();
    sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    pid_t threadIndex = 0;
    bool isProtectedCondition = true;
    auto res = imageManager->newImageCacheFromBuffer(buffer, threadIndex, isProtectedCondition);
    EXPECT_EQ(res, nullptr);
    buffer = SurfaceBuffer::Create();
    res = imageManager->newImageCacheFromBuffer(buffer, threadIndex, isProtectedCondition);
    EXPECT_NE(res, nullptr);
    isProtectedCondition = false;
    res = imageManager->newImageCacheFromBuffer(buffer, threadIndex, isProtectedCondition);
    EXPECT_NE(res, nullptr);
}
} // namespace OHOS::Rosen
