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
#include "feature/gpuComposition/rs_vk_image_manager.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "recording/recording_canvas.h"
#include "surface_buffer_impl.h"

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

void RSVKImageManagerTest::SetUpTestCase()
{
}

void RSVKImageManagerTest::TearDownTestCase() {}
void RSVKImageManagerTest::SetUp()
{
    RsVulkanContext::SetRecyclable(false);
    buffer_ = CreateBuffer();
    vkImageManager_ = std::make_shared<RSVkImageManager>();
}
void RSVKImageManagerTest::TearDown() {}

/**
 * @tc.name: MapAndUnMapVkImage001
 * @tc.desc: Map And UnMap VkImage, check cacheSeq size
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVkImage001, TestSize.Level1)
{
    auto size = vkImageManager_->imageCacheSeqs_.size();
    auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        new SurfaceBufferImpl(), SyncFence::INVALID_FENCE, fakeTid_);
    EXPECT_EQ(image, nullptr);
    EXPECT_EQ(size, vkImageManager_->imageCacheSeqs_.size());

    image = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);
    EXPECT_NE(image, nullptr);
    EXPECT_EQ(size + 1, vkImageManager_->imageCacheSeqs_.size());
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetBufferId());
    EXPECT_EQ(size, vkImageManager_->imageCacheSeqs_.size());
}

/**
 * @tc.name: MapAndUnMapVkImage002
 * @tc.desc: Map 10 VkImages, check cacheSeq size is 10,
 *           UnMap all VkImage, check cacheSeq size is 0
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVkImage002, TestSize.Level1)
{
    const uint32_t cacheNums = 10;
    uint64_t bufferSeqNums[10] = { 0 };
    for (uint32_t i = 1; i <= cacheNums; i++) {
        auto buffer = CreateBuffer();
        ASSERT_NE(buffer, nullptr);

        bufferSeqNums[i - 1] = buffer->GetBufferId();
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
 * @tc.name: MapAndUnMapVkImage003
 * @tc.desc: Map 50 VkImages, check cacheSeq size is not over MAX_CACHE_SIZE(40),
 *           UnMap all VkImage, check cacheSeq size is 0
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSVKImageManagerTest, MapAndUnMapVkImage003, TestSize.Level1)
{
    const uint32_t cacheNums = 50;
    uint64_t bufferSeqNums[50] = { 0 };
    const uint32_t MAX_CACHE_SIZE = 40;
    for (uint32_t i = 1; i <= cacheNums; i++) {
        auto buffer = CreateBuffer();
        ASSERT_NE(buffer, nullptr);

        bufferSeqNums[i - 1] = buffer->BufferId();
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(
            buffer, SyncFence::INVALID_FENCE, fakeTid_);
        EXPECT_NE(imageCache, nullptr);
        if (i <= MAX_CACHE_SIZE) {
            EXPECT_EQ(i, vkImageManager_->imageCacheSeqs_.size());
        } else {
            EXPECT_EQ(MAX_CACHE_SIZE, vkImageManager_->imageCacheSeqs_.size());
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
        vkImageManager_->UnMapImageFromSurfaceBuffer(bufferVector[i - 1]->GetBufferId());
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
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetBufferId());
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
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetBufferId());
    EXPECT_EQ(size, vkImageManager_->imageCacheSeqs_.size());

    sptr<SyncFence> tempFence = new SyncFence(-1);
    auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(
        buffer_, tempFence, fakeTid_, drawingSurface.get());
    EXPECT_NE(image, nullptr);
    size = vkImageManager_->imageCacheSeqs_.size();
    vkImageManager_->UnMapImageFromSurfaceBuffer(buffer_->GetBufferId());
    EXPECT_EQ(size - 1, vkImageManager_->imageCacheSeqs_.size());
}

/**
 * @tc.name: MapVkImageFromSurfaceBufferTest
 * @tc.desc: Test MapVkImageFromSurfaceBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, MapVkImageFromSurfaceBufferTest, TestSize.Level1)
{
    vkImageManager_->imageCacheSeqs_.clear();
    auto size = vkImageManager_->imageCacheSeqs_.size();
    auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(size+1, vkImageManager_->imageCacheSeqs_.size());
    const auto& vkTextureInfo = image->GetBackendTexture().GetTextureInfo().GetVKTextureInfo();
    ASSERT_NE(vkTextureInfo, nullptr);
    auto preBufferModel = vkTextureInfo->ycbcrConversionInfo.ycbcrModel;

    auto bufferId = buffer_->GetBufferId();
    vkImageManager_->imageCacheSeqs_[bufferId] = nullptr;
    image = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);
    EXPECT_EQ(image, nullptr); // imageCache is null
    
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    Drawing::BackendTexture backendTexture(true);
    Drawing::TextureInfo textureInfo;
    std::shared_ptr<Drawing::VKTextureInfo> imageInfo = nullptr;
    textureInfo.SetVKTextureInfo(imageInfo);
    backendTexture.SetTextureInfo(textureInfo);
    auto vkImage = std::make_shared<VkImageResource>(nativeWindowBuffer, backendTexture, nullptr);
    vkImageManager_->imageCacheSeqs_[bufferId] = vkImage;
    ASSERT_NE(vkImage, nullptr);
    EXPECT_EQ(vkImage->GetBackendTexture().GetTextureInfo().GetVKTextureInfo(), nullptr);
    image = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);
    
    vkImageManager_->imageCacheSeqs_.clear();
    vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);

    sptr<SurfaceBuffer> buffer = CreateBuffer();
    ASSERT_NE(buffer, nullptr);
    nativeWindowBuffer = nullptr;
    Drawing::BackendTexture backendTexture2(true);
    Drawing::TextureInfo textureInfo2;
    std::shared_ptr<Drawing::VKTextureInfo> imageInfo2 = std::make_shared<Drawing::VKTextureInfo>();
    auto ycbcrModel = VkSamplerYcbcrModelConversion::VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_2020;
    imageInfo2->ycbcrConversionInfo.ycbcrModel = ycbcrModel;
    textureInfo2.SetVKTextureInfo(imageInfo2);
    backendTexture2.SetTextureInfo(textureInfo2);
    auto vkImage2 = std::make_shared<VkImageResource>(nativeWindowBuffer, backendTexture2, nullptr);
    vkImageManager_->imageCacheSeqs_[bufferId] = vkImage2;
    vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);
    ASSERT_NE(vkImageManager_->imageCacheSeqs_[bufferId], nullptr);
    EXPECT_NE(ycbcrModel, preBufferModel);
}

/**
 * @tc.name: CreateImageCacheFromBuffer001
 * @tc.desc: call CreateImageCacheFromBuffer, check ImageCache will be created
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSVKImageManagerTest, CreateImageCacheFromBuffer001, TestSize.Level1)
{
    EXPECT_NE(vkImageManager_->CreateImageCacheFromBuffer(buffer_, BufferFence_), nullptr);

    // invalid buffer
    EXPECT_EQ(vkImageManager_->CreateImageCacheFromBuffer(
        new SurfaceBufferImpl(), SyncFence::INVALID_FENCE), nullptr);

    EXPECT_EQ(vkImageManager_->CreateImageCacheFromBuffer(nullptr, SyncFence::INVALID_FENCE), nullptr);
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
    BufferDrawParam params;
    params.buffer = nullptr;
    params.acquireFence = nullptr;
    params.threadIndex = 0;
    params.ignoreAlpha = false;
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSVkImageManager>();
    auto res = imageManager->CreateImageFromBuffer(*canvas, params, drawingColorSpace);
    EXPECT_EQ(res, nullptr);

    params.buffer = SurfaceBuffer::Create();
    res = imageManager->CreateImageFromBuffer(*canvas, params, drawingColorSpace);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: CreateImageFromBufferTest002
 * @tc.desc: Test RSVKImageManager Func CreateImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, CreateImageFromBufferTest002, TestSize.Level1)
{
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSVkImageManager>();
    auto drawingRecordingCanvas = std::make_unique<Drawing::RecordingCanvas>(100, 100);
    drawingRecordingCanvas->SetGrRecordingContext(std::make_shared<Drawing::GPUContext>());
    auto recordingCanvas = std::make_shared<RSPaintFilterCanvas>(drawingRecordingCanvas.get());
    EXPECT_NE(recordingCanvas, nullptr);
    BufferDrawParam params;
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
    params.buffer = CreateBuffer();
    params.ignoreAlpha = true;
    EXPECT_NE(params.buffer, nullptr);
    if (params.buffer && recordingCanvas) {
        EXPECT_FALSE(params.buffer->IsBufferDeleted());
        EXPECT_EQ(imageManager->CreateImageFromBuffer(*recordingCanvas, params, drawingColorSpace), nullptr);

        params.buffer->SetBufferDeletedFlag(BufferDeletedFlag::DELETED_FROM_CACHE);
        EXPECT_TRUE(params.buffer->IsBufferDeleted());
        EXPECT_EQ(imageManager->CreateImageFromBuffer(*recordingCanvas, params, drawingColorSpace), nullptr);

        params.buffer->SetBufferDeletedFlag(BufferDeletedFlag::DELETED_FROM_RS);
        EXPECT_TRUE(params.buffer->IsBufferDeleted());
        EXPECT_EQ(imageManager->CreateImageFromBuffer(*recordingCanvas, params, drawingColorSpace), nullptr);
    }
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
    BufferDrawParam params;
    params.acquireFence = nullptr;
    params.threadIndex = 0;
    params.buffer = nullptr;
    auto res = imageManager->GetIntersectImage(imgCutRect, context, params);
    EXPECT_EQ(res, nullptr);
    params.buffer = SurfaceBuffer::Create();
    res = imageManager->GetIntersectImage(imgCutRect, context, params);
    EXPECT_EQ(res, nullptr);
    params.buffer = buffer_;
    params.acquireFence = BufferFence_;
    res = imageManager->GetIntersectImage(imgCutRect, context, params);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: NewImageCacheFromBufferTest
 * @tc.desc: NewImageCacheFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, NewImageCacheFromBufferTest, TestSize.Level1)
{
    std::shared_ptr<RSVkImageManager> imageManager = std::make_shared<RSVkImageManager>();
    sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    pid_t threadIndex = 0;
    bool isProtectedCondition = true;
    auto res = imageManager->NewImageCacheFromBuffer(buffer, threadIndex, isProtectedCondition);
    EXPECT_EQ(res, nullptr);
    buffer = SurfaceBuffer::Create();
    res = imageManager->NewImageCacheFromBuffer(buffer, threadIndex, isProtectedCondition);
    EXPECT_EQ(res, nullptr);
    isProtectedCondition = false;
    res = imageManager->NewImageCacheFromBuffer(buffer, threadIndex, isProtectedCondition);
    EXPECT_EQ(res, nullptr);
}
} // namespace OHOS::Rosen
