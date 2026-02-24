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

#include <parameters.h>

#include "gtest/gtest.h"

#include "feature/image_detail_enhancer/rs_image_detail_enhancer_thread.h"
#include "render/rs_image_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageCacheTest::SetUpTestCase() {}
void RSImageCacheTest::TearDownTestCase() {}
void RSImageCacheTest::SetUp() {}
void RSImageCacheTest::TearDown() {}

/**
 * @tc.name: CheckUniqueIdIsEmptyTest
 * @tc.desc: Verify function CheckUniqueIdIsEmpty
 * @tc.type:FUNC
 * @tc.require: issueIATB9R
 */

HWTEST_F(RSImageCacheTest, CheckUniqueIdIsEmptyTest, TestSize.Level0)
{
    for (uint64_t id = 0; id < 10; id++)
    {
        RSImageCache::Instance().CollectUniqueId(id);
    }
    EXPECT_FALSE(RSImageCache::Instance().CheckUniqueIdIsEmpty());

    RSImageCache::Instance().ReleaseUniqueIdList();
    {
        std::unique_lock<std::mutex> lock(RSImageCache::Instance().uniqueIdListMutex_);
        EXPECT_TRUE(RSImageCache::Instance().uniqueIdList_.empty());
    }
    EXPECT_TRUE(RSImageCache::Instance().CheckUniqueIdIsEmpty());

    {
        std::unique_lock<std::mutex> lock(RSImageCache::Instance().uniqueIdListMutex_);
        RSImageCache::Instance().uniqueIdList_.clear();
    }
}

/**
 * @tc.name: InstanceTest
 * @tc.desc: Verify function Instance
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, InstanceTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
}

/**
 * @tc.name: CacheDrawingImageTest
 * @tc.desc: Verify function CacheDrawingImage
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, CacheDrawingImageTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: GetDrawingImageCacheTest
 * @tc.desc: Verify function GetDrawingImageCache
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, GetDrawingImageCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_EQ(imageCache.GetDrawingImageCache(0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache.GetDrawingImageCache(0), nullptr);
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: IncreaseDrawingImageCacheRefCountTest
 * @tc.desc: Verify function IncreaseDrawingImageCacheRefCount
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, IncreaseDrawingImageCacheRefCountTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.IncreaseDrawingImageCacheRefCount(0);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: ReleaseDrawingImageCacheTest
 * @tc.desc: Verify function ReleaseDrawingImageCache
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, ReleaseDrawingImageCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.ReleaseDrawingImageCache(0);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: GetPixelMapCacheTest
 * @tc.desc: Verify function GetPixelMapCache
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, GetPixelMapCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_EQ(imageCache.GetPixelMapCache(0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache.GetPixelMapCache(0), nullptr);
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: IncreasePixelMapCacheRefCountTest
 * @tc.desc: Verify function IncreasePixelMapCacheRefCount
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, IncreasePixelMapCacheRefCountTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.IncreasePixelMapCacheRefCount(0);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    imageCache.ReleasePixelMapCache(0);
    EXPECT_TRUE(true);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.ReleasePixelMapCache(0);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: CheckRefCntAndReleaseImageCacheTest
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, CheckRefCntAndReleaseImageCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CheckRefCntAndReleaseImageCache(0, pixelMap);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.CheckRefCntAndReleaseImageCache(0, pixelMap);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: CacheEditablePixelMapTest
 * @tc.desc: Verify function CacheEditablePixelMap
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSImageCacheTest, CacheEditablePixelMapTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t invalidUniqueId = 0;
    uint64_t validUniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_TRUE(pixelMap != nullptr);
    imageCache.CacheEditablePixelMap(invalidUniqueId, nullptr);
    imageCache.CacheEditablePixelMap(invalidUniqueId, pixelMap);
    imageCache.CacheEditablePixelMap(validUniqueId, pixelMap);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        EXPECT_FALSE(imageCache.editablePixelMapCache_.empty());
    }
}

/**
 * @tc.name: GetEditablePixelMapCacheTest
 * @tc.desc: Verify function GetEditablePixelMapCache
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSImageCacheTest, GetEditablePixelMapCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t invalidUniqueId = 0;
    uint64_t validUniqueId = 1;
    auto pixelMap = imageCache.GetEditablePixelMapCache(invalidUniqueId);
    EXPECT_TRUE(pixelMap == nullptr);
    pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_TRUE(pixelMap);
    imageCache.CacheEditablePixelMap(validUniqueId, pixelMap);
    pixelMap = imageCache.GetEditablePixelMapCache(validUniqueId);
    EXPECT_TRUE(pixelMap != nullptr);
}

/**
 * @tc.name: IncreaseEditablePixelMapCacheRefCountTest
 * @tc.desc: Verify function IncreaseEditablePixelMapCacheRefCount
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSImageCacheTest, IncreaseEditablePixelMapCacheRefCountTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t invalidUniqueId = 0;
    uint64_t validUniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_TRUE(pixelMap);
    imageCache.IncreaseEditablePixelMapCacheRefCount(invalidUniqueId);
    imageCache.CacheEditablePixelMap(validUniqueId, pixelMap);
    imageCache.IncreaseEditablePixelMapCacheRefCount(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it != imageCache.editablePixelMapCache_.end());
        EXPECT_TRUE(it->second.second > 0);
    }
}

/**
 * @tc.name: DiscardEditablePixelMapCacheTest
 * @tc.desc: Verify function DiscardEditablePixelMapCache
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSImageCacheTest, DiscardEditablePixelMapCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t invalidUniqueId = 0;
    uint64_t validUniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_TRUE(pixelMap);
    imageCache.DiscardEditablePixelMapCache(invalidUniqueId);
    imageCache.CacheEditablePixelMap(validUniqueId, pixelMap);
    imageCache.DiscardEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it == imageCache.editablePixelMapCache_.end());
        imageCache.editablePixelMapCache_.emplace(validUniqueId, std::make_pair(nullptr, 0));
    }
    imageCache.DiscardEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it == imageCache.editablePixelMapCache_.end());
    }
}

/**
 * @tc.name: DecreaseRefCountAndDiscardEditablePixelMapCacheTest
 * @tc.desc: Verify function DecreaseRefCountAndDiscardEditablePixelMapCache
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSImageCacheTest, DecreaseRefCountAndDiscardEditablePixelMapCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t invalidUniqueId = 0;
    uint64_t validUniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_TRUE(pixelMap);
    imageCache.DecreaseRefCountAndDiscardEditablePixelMapCache(invalidUniqueId);
    imageCache.DiscardEditablePixelMapCache(validUniqueId);
    imageCache.CacheEditablePixelMap(validUniqueId, pixelMap);
    imageCache.IncreaseEditablePixelMapCacheRefCount(validUniqueId);
    imageCache.IncreaseEditablePixelMapCacheRefCount(validUniqueId);
    imageCache.DecreaseRefCountAndDiscardEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it != imageCache.editablePixelMapCache_.end());
        EXPECT_TRUE(it->second.second > 0);
    }
    imageCache.DecreaseRefCountAndDiscardEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it == imageCache.editablePixelMapCache_.end());
        imageCache.editablePixelMapCache_.emplace(validUniqueId, std::make_pair(nullptr, 0));
    }
    imageCache.DecreaseRefCountAndDiscardEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it == imageCache.editablePixelMapCache_.end());
    }
}

/**
 * @tc.name: DecreaseRefCountAndReleaseEditablePixelMapCacheTest
 * @tc.desc: Verify function DecreaseRefCountAndReleaseEditablePixelMapCache
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSImageCacheTest, DecreaseRefCountAndReleaseEditablePixelMapCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t invalidUniqueId = 0;
    uint64_t validUniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_TRUE(pixelMap);
    imageCache.DecreaseRefCountAndReleaseEditablePixelMapCache(invalidUniqueId);
    imageCache.DiscardEditablePixelMapCache(validUniqueId);
    imageCache.CacheEditablePixelMap(validUniqueId, pixelMap);
    imageCache.IncreaseEditablePixelMapCacheRefCount(validUniqueId);
    imageCache.IncreaseEditablePixelMapCacheRefCount(validUniqueId);
    imageCache.DecreaseRefCountAndReleaseEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it != imageCache.editablePixelMapCache_.end());
        EXPECT_TRUE(it->second.second > 0);
    }
    imageCache.DecreaseRefCountAndReleaseEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it == imageCache.editablePixelMapCache_.end());
        imageCache.editablePixelMapCache_.emplace(validUniqueId, std::make_pair(nullptr, 0));
    }
    imageCache.DecreaseRefCountAndReleaseEditablePixelMapCache(validUniqueId);
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheMutex_);
        auto it = imageCache.editablePixelMapCache_.find(validUniqueId);
        EXPECT_TRUE(it == imageCache.editablePixelMapCache_.end());
    }
}

/**
 * @tc.name: ReleaseEditablePixelMapCacheTest
 * @tc.desc: Verify function ReleaseEditablePixelMapCache
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSImageCacheTest, ReleaseEditablePixelMapCacheTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t validUniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    EXPECT_TRUE(pixelMap);
    imageCache.CacheEditablePixelMap(validUniqueId, pixelMap);
    imageCache.ReleaseEditablePixelMapCache();
    {
        std::lock_guard<std::mutex> lock(imageCache.editablePixelMapCacheToReleaseMutex_);
        EXPECT_TRUE(imageCache.editablePixelMapCacheToRelease_.empty());
    }
}

/**
 * @tc.name: CacheRenderDrawingImageByPixelMapIdTest
 * @tc.desc: Verify function CacheRenderDrawingImageByPixelMapId
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, CacheRenderDrawingImageByPixelMapIdTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    EXPECT_FALSE(imageCache.pixelMapIdRelatedDrawingImageCache_.empty());
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: GetRenderDrawingImageCacheByPixelMapIdTest
 * @tc.desc: Verify function GetRenderDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, GetRenderDrawingImageCacheByPixelMapIdTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_EQ(imageCache.GetRenderDrawingImageCacheByPixelMapId(0, 0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    EXPECT_EQ(imageCache.GetRenderDrawingImageCacheByPixelMapId(0, 0), nullptr);
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: ReleaseDrawingImageCacheByPixelMapIdTest
 * @tc.desc: Verify function ReleaseDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, ReleaseDrawingImageCacheByPixelMapIdTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    imageCache.ReleaseDrawingImageCacheByPixelMapId(0);
    EXPECT_FALSE(imageCache.pixelMapIdRelatedDrawingImageCache_.empty());
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: ReleaseUniqueIdListTest
 * @tc.desc: Verify function ReleaseUniqueIdList
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSImageCacheTest, ReleaseUniqueIdListTest, TestSize.Level1)
{
    for (uint64_t id = 0; id < 10; id++)
    {
        RSImageCache::Instance().CollectUniqueId(id);
    }

    RSImageCache::Instance().ReleaseUniqueIdList();
    EXPECT_TRUE(RSImageCache::Instance().uniqueIdList_.empty());
    RSImageCache::Instance().uniqueIdList_.clear();
}

/**
 * @tc.name: GetDrawingImageCacheTest001
 * @tc.desc: Verify function GetDrawingImageCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, GetDrawingImageCacheTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache.GetDrawingImageCache(1), img);
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: GetPixelMapCacheTest001
 * @tc.desc: Verify function GetPixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, GetPixelMapCacheTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CachePixelMap(1, pixelMap);
    EXPECT_EQ(imageCache.GetPixelMapCache(1), pixelMap);
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: IncreasePixelMapCacheRefCountTest001
 * @tc.desc: Verify function IncreasePixelMapCacheRefCount
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, IncreasePixelMapCacheRefCountTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CachePixelMap(1, pixelMap);
    imageCache.IncreasePixelMapCacheRefCount(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest001
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CachePixelMap(1, pixelMap);
    imageCache.ReleasePixelMapCache(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest002
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest002, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(nullptr, 0));
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest003
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest003, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(nullptr, 1));
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest004
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest004, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 0));
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest005
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest005, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: GetRenderDrawingImageCacheByPixelMapIdTest001
 * @tc.desc: Verify function GetRenderDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, GetRenderDrawingImageCacheByPixelMapIdTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    EXPECT_EQ(imageCache.GetRenderDrawingImageCacheByPixelMapId(1, 0), img);
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: ReleaseDrawingImageCacheByPixelMapIdTest001
 * @tc.desc: Verify function ReleaseDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, ReleaseDrawingImageCacheByPixelMapIdTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    imageCache.ReleaseDrawingImageCacheByPixelMapId(1);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: CachePixelMapTest
 * @tc.desc: Verify function CachePixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CachePixelMapTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 1;
    imageCache.CachePixelMap(uniqueId, nullptr);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: CachePixelMapTest001
 * @tc.desc: Verify function CachePixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CachePixelMapTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 0;
    imageCache.CachePixelMap(uniqueId, nullptr);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: CachePixelMapTest002
 * @tc.desc: Verify function CachePixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CachePixelMapTest002, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    imageCache.CachePixelMap(uniqueId, pixelMap);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: CachePixelMapTest003
 * @tc.desc: Verify function CachePixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CachePixelMapTest003, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 0;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    imageCache.CachePixelMap(uniqueId, pixelMap);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: CachePixelMapTest004
 * @tc.desc: Verify function CachePixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CachePixelMapTest004, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.CachePixelMap(uniqueId, pixelMap);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: CachePixelMapTest005
 * @tc.desc: Verify function CachePixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CachePixelMapTest005, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 0;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.CachePixelMap(uniqueId, pixelMap);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: CheckRefCntAndReleaseImageCacheTest001
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CheckRefCntAndReleaseImageCacheTest001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 2));
    imageCache.CacheDrawingImage(1, img);
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: CheckRefCntAndReleaseImageCacheTest002
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CheckRefCntAndReleaseImageCacheTest002, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    imageCache.CacheDrawingImage(1, img);
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: CheckRefCntAndReleaseImageCacheTest003
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CheckRefCntAndReleaseImageCacheTest003, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    imageCache.CacheDrawingImage(1, img);
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: CheckRefCntAndReleaseImageCacheTest004
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2B3G
 */
HWTEST_F(RSImageCacheTest, CheckRefCntAndReleaseImageCacheTest004, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    imageCache.CacheDrawingImage(1, img);
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: CachePixelMapTest006
 * @tc.desc: Verify function CachePixelMap
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageCacheTest, CachePixelMapTest006, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 1;
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    imageCache.CachePixelMap(uniqueId, pixelMap);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest006
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest006, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest007
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest007, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(0, std::make_pair(pixelMap, 1));
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleasePixelMapCacheTest008
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type:FUNC
 * @tc.require: issue#IB2LQP
 */
HWTEST_F(RSImageCacheTest, ReleasePixelMapCacheTest008, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(0, std::make_pair(pixelMap, 0));
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: ReleaseDrawingImageCacheByPixelMapIdTest002
 * @tc.desc: Verify function ReleaseDrawingImageCacheByPixelMapIdTest
 * @tc.type: FUNC
 * @tc.require: issue#IBZ6NM
 */
HWTEST_F(RSImageCacheTest, ReleaseDrawingImageCacheByPixelMapIdTest002, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(123, img, 0);
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    auto type = system::GetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "1");
    rsImageDetailEnhancerThread.SetScaledImage(123, img);
    imageCache.ReleaseDrawingImageCacheByPixelMapId(123);
    EXPECT_TRUE(imageCache.pixelMapIdRelatedDrawingImageCache_.empty());

    auto img2 = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(345, img2, 0);
    imageCache.ReleaseDrawingImageCacheByPixelMapId(345);
    EXPECT_TRUE(imageCache.pixelMapIdRelatedDrawingImageCache_.empty());
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", type);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: ReserveImageInfoTest
 * @tc.desc: Verify function ReserveImageInfoTest
 * @tc.type: FUNC
 * @tc.require: issue#IBZ6NM
 */
HWTEST_F(RSImageCacheTest, ReserveImageInfoTest, TestSize.Level1)
{
    // create imageCache
    RSImageCache& imageCache = RSImageCache::Instance();

    // create rsImage
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();

    //create extendImageObject
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap;
    Drawing::AdaptiveImageInfo imageInfo;
    auto extendImageObject = std::make_shared<RSExtendImageObject>(pixelMap, imageInfo);
    EXPECT_EQ(imageCache.rsImageInfoMap.size(), 0);

    NodeId id = 1;
    imageCache.ReserveImageInfo(nullptr, id, extendImageObject->weak_from_this());
    EXPECT_EQ(imageCache.rsImageInfoMap.size(), 0);

    EXPECT_NE(rsImage, nullptr);
    imageCache.ReserveImageInfo(rsImage, id, extendImageObject->weak_from_this());
    EXPECT_EQ(imageCache.rsImageInfoMap.size(), 0);
}

/**
 * @tc.name: RemoveImageMemForWindowTest
 * @tc.desc: Verify function RemoveImageMemForWindowTest
 * @tc.type: FUNC
 * @tc.require: issue#IBZ6NM
 */
HWTEST_F(RSImageCacheTest, RemoveImageMemForWindowTest, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap;
    Drawing::AdaptiveImageInfo imageInfo;
    auto extendImageObject = std::make_shared<RSExtendImageObject>(pixelMap, imageInfo);
    NodeId surfaceNodeId = 1;
    imageCache.rsImageInfoMap[surfaceNodeId].push_back(std::make_pair(rsImage, extendImageObject->weak_from_this()));
    EXPECT_EQ(imageCache.rsImageInfoMap.size(), 1);

    imageCache.RemoveImageMemForWindow(surfaceNodeId);
    EXPECT_EQ(imageCache.rsImageInfoMap.size(), 0);
}
#endif
} // namespace OHOS::Rosen