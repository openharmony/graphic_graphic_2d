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

#include "gtest/gtest.h"

#include "render/rs_image_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImageCacheUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageCacheUnitTest::SetUpTestCase() {}
void RSImageCacheUnitTest::TearDownTestCase() {}
void RSImageCacheUnitTest::SetUp() {}
void RSImageCacheUnitTest::TearDown() {}

/**
 * @tc.name: TestCheckUniqueIdIsEmpty
 * @tc.desc: Verify function CheckUniqueIdIsEmpty
 * @tc.type: FUNC
 * @tc.require:
 */

HWTEST_F(RSImageCacheUnitTest, TestCheckUniqueIdIsEmpty, TestSize.Level0)
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
 * @tc.name: TestInstance
 * @tc.desc: Verify function Instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestInstance, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
}

/**
 * @tc.name: TestCacheDrawingImage
 * @tc.desc: Verify function CacheDrawingImage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCacheDrawingImage, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestGetDrawingImageCache
 * @tc.desc: Verify function GetDrawingImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestGetDrawingImageCache, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_EQ(imageCache.GetDrawingImageCache(0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache.GetDrawingImageCache(0), nullptr);
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestIncreaseDrawingImageCacheRefCount
 * @tc.desc: Verify function IncreaseDrawingImageCacheRefCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestIncreaseDrawingImageCacheRefCount, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.IncreaseDrawingImageCacheRefCount(0);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestReleaseDrawingImageCache
 * @tc.desc: Verify function ReleaseDrawingImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleaseDrawingImageCache, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.ReleaseDrawingImageCache(0);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestGetPixelMapCache
 * @tc.desc: Verify function GetPixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestGetPixelMapCache, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_EQ(imageCache.GetPixelMapCache(0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache.GetPixelMapCache(0), nullptr);
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestIncreasePixelMapCacheRefCount
 * @tc.desc: Verify function IncreasePixelMapCacheRefCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestIncreasePixelMapCacheRefCount, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.IncreasePixelMapCacheRefCount(0);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestReleasePixelMapCache
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache, TestSize.Level1)
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
 * @tc.name: TestCheckRefCntAndReleaseImageCache
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCheckRefCntAndReleaseImageCache, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CheckRefCntAndReleaseImageCache(0, pixelMap, nullptr);
    EXPECT_TRUE(true);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    imageCache.CheckRefCntAndReleaseImageCache(0, pixelMap, nullptr);
    EXPECT_FALSE(imageCache.drawingImageCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestCacheRenderDrawingImageByPixelMapId
 * @tc.desc: Verify function CacheRenderDrawingImageByPixelMapId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCacheRenderDrawingImageByPixelMapId, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    EXPECT_FALSE(imageCache.pixelMapIdRelatedDrawingImageCache_.empty());
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: TestGetRenderDrawingImageCacheByPixelMapId
 * @tc.desc: Verify function GetRenderDrawingImageCacheByPixelMapId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestGetRenderDrawingImageCacheByPixelMapId, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    EXPECT_EQ(imageCache.GetRenderDrawingImageCacheByPixelMapId(0, 0), nullptr);
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    EXPECT_EQ(imageCache.GetRenderDrawingImageCacheByPixelMapId(0, 0), nullptr);
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: TestReleaseDrawingImageCacheByPixelMapId
 * @tc.desc: Verify function ReleaseDrawingImageCacheByPixelMapId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleaseDrawingImageCacheByPixelMapId, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    imageCache.ReleaseDrawingImageCacheByPixelMapId(0);
    EXPECT_FALSE(imageCache.pixelMapIdRelatedDrawingImageCache_.empty());
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: TestReleaseUniqueIdList
 * @tc.desc: Verify function ReleaseUniqueIdList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleaseUniqueIdList, TestSize.Level1)
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
 * @tc.name: TestGetDrawingImageCache001
 * @tc.desc: Verify function GetDrawingImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestGetDrawingImageCache001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheDrawingImage(1, img);
    EXPECT_EQ(imageCache.GetDrawingImageCache(1), img);
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestGetPixelMapCache001
 * @tc.desc: Verify function GetPixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestGetPixelMapCache001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CachePixelMap(1, pixelMap);
    EXPECT_EQ(imageCache.GetPixelMapCache(1), pixelMap);
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: TestIncreasePixelMapCacheRefCount001
 * @tc.desc: Verify function IncreasePixelMapCacheRefCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestIncreasePixelMapCacheRefCount001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CachePixelMap(1, pixelMap);
    imageCache.IncreasePixelMapCacheRefCount(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: TestReleasePixelMapCache001
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.CachePixelMap(1, pixelMap);
    imageCache.ReleasePixelMapCache(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: TestReleasePixelMapCache002
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache002, TestSize.Level1)
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
 * @tc.name: TestReleasePixelMapCache003
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache003, TestSize.Level1)
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
 * @tc.name: TestReleasePixelMapCache004
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache004, TestSize.Level1)
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
 * @tc.name: TestReleasePixelMapCache005
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache005, TestSize.Level1)
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
 * @tc.name: TestGetRenderDrawingImageCacheByPixelMapId001
 * @tc.desc: Verify function GetRenderDrawingImageCacheByPixelMapId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestGetRenderDrawingImageCacheByPixelMapId001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    EXPECT_EQ(imageCache.GetRenderDrawingImageCacheByPixelMapId(1, 0), img);
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: TestReleaseDrawingImageCacheByPixelMapId001
 * @tc.desc: Verify function ReleaseDrawingImageCacheByPixelMapId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleaseDrawingImageCacheByPixelMapId001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.CacheRenderDrawingImageByPixelMapId(1, img, 0);
    imageCache.ReleaseDrawingImageCacheByPixelMapId(1);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapIdRelatedDrawingImageCache_.clear();
}

/**
 * @tc.name: TestCachePixelMap
 * @tc.desc: Verify function CachePixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCachePixelMap, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 1;
    imageCache.CachePixelMap(uniqueId, nullptr);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: TestCachePixelMap001
 * @tc.desc: Verify function CachePixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCachePixelMap001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    uint64_t uniqueId = 0;
    imageCache.CachePixelMap(uniqueId, nullptr);
    EXPECT_TRUE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}

/**
 * @tc.name: TestCachePixelMap002
 * @tc.desc: Verify function CachePixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCachePixelMap002, TestSize.Level1)
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
 * @tc.name: TestCachePixelMap003
 * @tc.desc: Verify function CachePixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCachePixelMap003, TestSize.Level1)
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
 * @tc.name: TestCachePixelMap004
 * @tc.desc: Verify function CachePixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCachePixelMap004, TestSize.Level1)
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
 * @tc.name: TestCachePixelMap005
 * @tc.desc: Verify function CachePixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCachePixelMap005, TestSize.Level1)
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
 * @tc.name: TestCheckRefCntAndReleaseImageCache001
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCheckRefCntAndReleaseImageCache001, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 2));
    imageCache.CacheDrawingImage(1, img);
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap, img);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestCheckRefCntAndReleaseImageCache002
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCheckRefCntAndReleaseImageCache002, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    imageCache.CacheDrawingImage(1, img);
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap, img);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestCheckRefCntAndReleaseImageCache003
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCheckRefCntAndReleaseImageCache003, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    imageCache.CacheDrawingImage(1, img);
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap, img);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestCheckRefCntAndReleaseImageCache004
 * @tc.desc: Verify function CheckRefCntAndReleaseImageCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCheckRefCntAndReleaseImageCache004, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    auto img = std::make_shared<Drawing::Image>();
    imageCache.pixelMapCache_.emplace(1, std::make_pair(pixelMap, 1));
    imageCache.CacheDrawingImage(1, img);
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    imageCache.CheckRefCntAndReleaseImageCache(1, pixelMap, img);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.drawingImageCache_.clear();
}

/**
 * @tc.name: TestCachePixelMap006
 * @tc.desc: Verify function CachePixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestCachePixelMap006, TestSize.Level1)
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
 * @tc.name: TestReleasePixelMapCache006
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache006, TestSize.Level1)
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
 * @tc.name: TestReleasePixelMapCache007
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache007, TestSize.Level1)
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
 * @tc.name: TestReleasePixelMapCache008
 * @tc.desc: Verify function ReleasePixelMapCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSImageCacheUnitTest, TestReleasePixelMapCache008, TestSize.Level1)
{
    RSImageCache& imageCache = RSImageCache::Instance();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    imageCache.pixelMapCache_.emplace(0, std::make_pair(pixelMap, 0));
    pixelMap->allocatorType_ = Media::AllocatorType::HEAP_ALLOC;
    imageCache.ReleasePixelMapCache(1);
    EXPECT_FALSE(imageCache.pixelMapCache_.empty());
    imageCache.pixelMapCache_.clear();
}
} // namespace OHOS::Rosen