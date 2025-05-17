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
    auto image = vkImageManager_->MapVkImageFromSurfaceBuffer(new SurfaceBufferImpl(), SyncFence::INVALID_FENCE, fakeTid_);
    EXPECT_EQ(image, nullptr);
    EXPECT_EQ(size, vkImageManager_->imageCacheSeqs_.size());

    image = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer_, BufferFence_, fakeTid_);
    EXPECT_NE(image, nullptr);
    EXPECT_EQ(size + 1, vkImageManager_->imageCacheSeqs_.size());
    vkImageManager_->UnMapVkImageFromSurfaceBuffer(buffer_->GetSeqNum());
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
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer, SyncFence::INVALID_FENCE, fakeTid_);
        EXPECT_NE(imageCache, nullptr);
        EXPECT_EQ(i, vkImageManager_->imageCacheSeqs_.size());
    }

    std::string dumpString = "";
    vkImageManager_->DumpVkImageInfo(dumpString);
    EXPECT_NE(dumpString, "");

    for (uint32_t i = cacheNums; i >= 1; i--) {
        vkImageManager_->UnMapVkImageFromSurfaceBuffer(bufferSeqNums[i - 1]);
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
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer, SyncFence::INVALID_FENCE, fakeTid_);
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
        vkImageManager_->UnMapVkImageFromSurfaceBuffer(bufferSeqNums[i - 1]);
    }
    EXPECT_EQ(0, vkImageManager_->imageCacheSeqs_.size());
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
    EXPECT_EQ(vkImageManager_->CreateImageCacheFromBuffer(new SurfaceBufferImpl(), SyncFence::INVALID_FENCE), nullptr);
}

/**
 * @tc.name: ShrinkCachesIfNeeded001
 * @tc.desc: Map 50 VKImages, call ShrinkCachesIfNeeded, check cacheQueue size
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSVKImageManagerTest, ShrinkCachesIfNeeded001, TestSize.Level1)
{
    const uint32_t cacheNums = 50;
    uint32_t bufferSeqNums[50] = { 0 };
    const uint32_t MAX_CACHE_SIZE = 16;
    for (uint32_t i = 1; i <= cacheNums; i++) {
        auto buffer = CreateBuffer();
        ASSERT_NE(buffer, nullptr);

        bufferSeqNums[i - 1] = buffer->GetSeqNum();
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(buffer, SyncFence::INVALID_FENCE, fakeTid_);
        EXPECT_NE(imageCache, nullptr);
        EXPECT_EQ(i, vkImageManager_->cacheQueue_.size());
    }
    vkImageManager_->ShrinkCachesIfNeeded();
    EXPECT_EQ(MAX_CACHE_SIZE, vkImageManager_->cacheQueue_.size());
}
} // namespace OHOS::Rosen
