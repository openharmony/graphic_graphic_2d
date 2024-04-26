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

#include "render/rs_typeface_cache.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSTypefaceCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTypefaceCacheTest::SetUpTestCase() {}
void RSTypefaceCacheTest::TearDownTestCase() {}
void RSTypefaceCacheTest::SetUp() {}
void RSTypefaceCacheTest::TearDown() {}

/**
 * @tc.name: GenGlobalUniqueIdTest001
 * @tc.desc: Verify function GenGlobalUniqueId
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, GenGlobalUniqueIdTest001, TestSize.Level1)
{
    EXPECT_NE(RSTypefaceCache::Instance().GenGlobalUniqueId(1), 0);
}

/**
 * @tc.name: GetTypefacePidTest001
 * @tc.desc: Verify function GetTypefacePid
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, GetTypefacePidTest001, TestSize.Level1)
{
    EXPECT_EQ(RSTypefaceCache::Instance().GetTypefacePid(1), 0);
}

/**
 * @tc.name: GetTypefaceIdTest001
 * @tc.desc: Verify function GetTypefaceId
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, GetTypefaceIdTest001, TestSize.Level1)
{
    EXPECT_NE(RSTypefaceCache::Instance().GetTypefaceId(1), 0);
}

/**
 * @tc.name: CacheDrawingTypefaceTest001
 * @tc.desc: Verify function CacheDrawingTypeface
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, CacheDrawingTypefaceTest001, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    RSTypefaceCache::Instance().CacheDrawingTypeface(1, typeface);
    RSTypefaceCache::Instance().CacheDrawingTypeface(2, typeface);
    EXPECT_FALSE(RSTypefaceCache::Instance().drawingTypefaceCache_.empty());
}

/**
 * @tc.name: RemoveDrawingTypefaceByGlobalUniqueIdTest001
 * @tc.desc: Verify function RemoveDrawingTypefaceByGlobalUniqueId
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, RemoveDrawingTypefaceByGlobalUniqueIdTest001, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(1);
    EXPECT_FALSE(RSTypefaceCache::Instance().drawingTypefaceCache_.empty());
}

/**
 * @tc.name: GetDrawingTypefaceCacheTest001
 * @tc.desc: Verify function GetDrawingTypefaceCache
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, GetDrawingTypefaceCacheTest001, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    EXPECT_EQ(RSTypefaceCache::Instance().GetDrawingTypefaceCache(1), nullptr);
}

/**
 * @tc.name: RemoveDrawingTypefacesByPidTest001
 * @tc.desc: Verify function RemoveDrawingTypefacesByPid
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, RemoveDrawingTypefacesByPidTest001, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(2);
    EXPECT_FALSE(RSTypefaceCache::Instance().drawingTypefaceCache_.empty());
}

/**
 * @tc.name: AddDelayDestroyQueueTest001
 * @tc.desc: Verify function AddDelayDestroyQueue
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, AddDelayDestroyQueueTest001, TestSize.Level1)
{
    RSTypefaceCache::Instance().AddDelayDestroyQueue(1);
    EXPECT_FALSE(RSTypefaceCache::Instance().drawingTypefaceCache_.empty());
}

/**
 * @tc.name: HandleDelayDestroyQueueTest001
 * @tc.desc: Verify function HandleDelayDestroyQueue
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, HandleDelayDestroyQueueTest001, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    RSTypefaceCache::Instance().HandleDelayDestroyQueue();
    EXPECT_FALSE(RSTypefaceCache::Instance().drawingTypefaceCache_.empty());
}
} // namespace Rosen
} // namespace OHOS