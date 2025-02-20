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
} // namespace OHOS::Rosen