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

#include <gtest/gtest.h>

#include "render/rs_image_cache.h"
#include "platform/common/rs_log.h"
#include "image/image.h"
#include "pixel_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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

HWTEST_F(RSImageCacheTest, Singleton001, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest Singleton001 start";

    auto& instance1 = RSImageCache::Instance();
    auto& instance2 = RSImageCache::Instance();
    EXPECT_EQ(&instance1, &instance2);

    GTEST_LOG_(INFO) << "RSImageCacheTest Singleton001 start";
}

/**
 * @tc.name: DrawingImageCache001
 * @tc.desc: Verify the GetDrawingImageCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, DrawingImageCache001, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest DrawingImageCache001 start";

    auto& instance = RSImageCache::Instance();
    const uint64_t UNADDED_ID = 100;
    EXPECT_EQ(instance.GetDrawingImageCache(UNADDED_ID), nullptr);

    GTEST_LOG_(INFO) << "RSImageCacheTest DrawingImageCache001 start";
}

/**
 * @tc.name: DrawingImageCache002
 * @tc.desc: Verify the CacheDrawingImage && GetDrawingImageCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, DrawingImageCache002, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest DrawingImageCache002 start";

    auto& instance = RSImageCache::Instance();
    std::map<uint64_t, std::shared_ptr<Drawing::Image>>  imgMap;
    for (uint64_t uniqueId = 1;uniqueId < 16;uniqueId++) {
        auto img = std::make_shared<Drawing::Image>();
        instance.CacheDrawingImage(uniqueId, img);
        imgMap[uniqueId] = img;
    }
    for (const auto& pair : imgMap) {
        EXPECT_EQ(pair.second, instance.GetDrawingImageCache(pair.first));
    }

    GTEST_LOG_(INFO) << "RSImageCacheTest DrawingImageCache002 start";
}

/**
 * @tc.name: ModifyDrawingImageCacheRefCount001
 * @tc.desc: Verify the ReleaseDrawingImageCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, ModifyDrawingImageCacheRefCount001, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyDrawingImageCacheRefCount001 start";

    auto& instance = RSImageCache::Instance();
    const uint64_t ADDED_ID = 200;
    auto img = std::make_shared<Drawing::Image>();
    instance.CacheDrawingImage(ADDED_ID, img);
    EXPECT_EQ(instance.GetDrawingImageCache(ADDED_ID), img);
    instance.IncreaseDrawingImageCacheRefCount(ADDED_ID);
    instance.ReleaseDrawingImageCache(ADDED_ID);
    EXPECT_EQ(instance.GetDrawingImageCache(ADDED_ID), nullptr);

    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyDrawingImageCacheRefCount001 start";
}

/**
 * @tc.name: ModifyDrawingImageCacheRefCount002
 * @tc.desc: Verify the IncreaseDrawingImageCacheRefCount && ReleaseDrawingImageCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, ModifyDrawingImageCacheRefCount002, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyDrawingImageCacheRefCount002 start";

    auto& instance = RSImageCache::Instance();
    const uint64_t ADDED_ID = 300;
    auto img = std::make_shared<Drawing::Image>();
    instance.CacheDrawingImage(ADDED_ID, img);
    for (uint64_t count = 0;count < 10;count++) {
        instance.IncreaseDrawingImageCacheRefCount(ADDED_ID);
    }
    for (uint64_t count = 0;count < 9;count++) {
        instance.ReleaseDrawingImageCache(ADDED_ID);
        EXPECT_NE(instance.GetDrawingImageCache(ADDED_ID), nullptr);
    }
    instance.ReleaseDrawingImageCache(ADDED_ID);
    EXPECT_EQ(instance.GetDrawingImageCache(ADDED_ID), nullptr);

    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyDrawingImageCacheRefCount002 start";
}

/**
 * @tc.name: CachePixelMap001
 * @tc.desc: Verify the GetPixelMapCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, CachePixelMap001, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest CachePixelMap001 start";

    auto& instance = RSImageCache::Instance();
    const uint64_t UNADDED_ID = 400;
    EXPECT_EQ(instance.GetPixelMapCache(UNADDED_ID), nullptr);

    GTEST_LOG_(INFO) << "RSImageCacheTest CachePixelMap001 start";
}

/**
 * @tc.name: CachePixelMap002
 * @tc.desc: Verify the GetPixelMapCache && CachePixelMap
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, CachePixelMap002, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest CachePixelMap002 start";

    auto& instance = RSImageCache::Instance();
    std::map<uint64_t, std::shared_ptr<Media::PixelMap>>  pixelMap;
    for (uint64_t uniqueId = 1;uniqueId < 16;uniqueId++) {
        auto pixel = std::make_shared<Media::PixelMap>();
        instance.CachePixelMap(uniqueId, pixel);
        pixelMap[uniqueId] = pixel;
    }
    for (const auto& pair : pixelMap) {
        EXPECT_EQ(pair.second, instance.GetPixelMapCache(pair.first));
    }

    GTEST_LOG_(INFO) << "RSImageCacheTest CachePixelMap002 start";
}

/**
 * @tc.name: ModifyPixelMapCacheRefCount001
 * @tc.desc: Verify the ReleasePixelMapCache
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, ModifyPixelMapCacheRefCount001, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyPixelMapCacheRefCount001 start";

    auto& instance = RSImageCache::Instance();
    const uint64_t ADDED_ID = 500;
    auto pixel = std::make_shared<Media::PixelMap>();
    instance.CachePixelMap(ADDED_ID, pixel);
    EXPECT_EQ(instance.GetPixelMapCache(ADDED_ID), pixel);
    instance.IncreasePixelMapCacheRefCount(ADDED_ID);
    instance.ReleasePixelMapCache(ADDED_ID);
    EXPECT_EQ(instance.GetPixelMapCache(ADDED_ID), nullptr);

    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyPixelMapCacheRefCount001 start";
}

/**
 * @tc.name: ModifyPixelMapCacheRefCount001
 * @tc.desc: Verify the ReleasePixelMapCache && IncreasePixelMapCacheRefCount
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, ModifyPixelMapCacheRefCount002, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyPixelMapCacheRefCount002 start";

    auto& instance = RSImageCache::Instance();
    const uint64_t ADDED_ID = 600;
    auto pixel = std::make_shared<Media::PixelMap>();
    instance.CachePixelMap(ADDED_ID, pixel);
    for (uint64_t count = 0;count < 10;count++) {
        instance.IncreasePixelMapCacheRefCount(ADDED_ID);
    }
    for (uint64_t count = 0;count < 9;count++) {
        instance.ReleasePixelMapCache(ADDED_ID);
        EXPECT_NE(instance.GetPixelMapCache(ADDED_ID), nullptr);
    }
    instance.ReleasePixelMapCache(ADDED_ID);
    EXPECT_EQ(instance.GetPixelMapCache(ADDED_ID), nullptr);

    GTEST_LOG_(INFO) << "RSImageCacheTest ModifyPixelMapCacheRefCount002 start";
}

/**
 * @tc.name: RenderDrawingImageByPixelMapId001
 * @tc.desc: Verify CacheRenderDrawingImageByPixelMapId && GetRenderDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, RenderDrawingImageByPixelMapId001, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest RenderDrawingImageByPixelMapId001 start";

    auto& instance = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    const uint64_t ADDED_ID = 700;
    const pid_t P_ID = 1000;
    instance.CacheRenderDrawingImageByPixelMapId(ADDED_ID, img, P_ID);
    EXPECT_EQ(img, instance.GetRenderDrawingImageCacheByPixelMapId(ADDED_ID, P_ID));

    const uint64_t P_ID1 = 1001;
    EXPECT_EQ(nullptr, instance.GetRenderDrawingImageCacheByPixelMapId(ADDED_ID, P_ID1));

    const uint64_t ADDED_ID1 = 701;
    EXPECT_EQ(nullptr, instance.GetRenderDrawingImageCacheByPixelMapId(ADDED_ID1, P_ID));

    GTEST_LOG_(INFO) << "RSImageCacheTest RenderDrawingImageByPixelMapId001 start";
}

/**
 * @tc.name: RenderDrawingImageByPixelMapId002
 * @tc.desc: Verify ReleaseDrawingImageCacheByPixelMapId
 * @tc.type:FUNC
 */
HWTEST_F(RSImageCacheTest, RenderDrawingImageByPixelMapId002, TestSize.Level1) {
    GTEST_LOG_(INFO) << "RSImageCacheTest RenderDrawingImageByPixelMapId002 start";

    auto& instance = RSImageCache::Instance();
    auto img = std::make_shared<Drawing::Image>();
    auto pixel = std::make_shared<Media::PixelMap>();
    const uint64_t ADDED_ID = 800;
    const pid_t P_ID = 2000;

    instance.CachePixelMap(ADDED_ID, pixel);
    instance.CacheRenderDrawingImageByPixelMapId(ADDED_ID, img, P_ID);
    instance.IncreasePixelMapCacheRefCount(ADDED_ID);
    instance.ReleasePixelMapCache(ADDED_ID);
    EXPECT_EQ(nullptr, instance.GetPixelMapCache(ADDED_ID));
    EXPECT_EQ(nullptr, instance.GetRenderDrawingImageCacheByPixelMapId(ADDED_ID, P_ID));

    GTEST_LOG_(INFO) << "RSImageCacheTest RenderDrawingImageByPixelMapId002 start";
}
}
}
