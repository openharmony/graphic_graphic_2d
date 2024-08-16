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
    uint64_t uniqueIdF = 1;
    uint64_t uniqueIdS = 2;
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueIdF, typeface);
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueIdS, typeface);
    EXPECT_FALSE(RSTypefaceCache::Instance().typefaceHashMap_.empty());
}

/**
 * @tc.name: RemoveDrawingTypefaceByGlobalUniqueIdTest001
 * @tc.desc: Verify function RemoveDrawingTypefaceByGlobalUniqueId
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, RemoveDrawingTypefaceByGlobalUniqueIdTest001, TestSize.Level1)
{
    uint64_t uniqueIdF = 1;
    uint64_t uniqueIdS = 3;
    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(uniqueIdF);
    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(uniqueIdS);
    EXPECT_FALSE(RSTypefaceCache::Instance().typefaceHashMap_.empty());
}

/**
 * @tc.name: GetDrawingTypefaceCacheTest001
 * @tc.desc: Verify function GetDrawingTypefaceCache
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, GetDrawingTypefaceCacheTest001, TestSize.Level1)
{
    uint64_t uniqueIdF = 1;
    uint64_t uniqueIdS = 2;
    EXPECT_EQ(RSTypefaceCache::Instance().GetDrawingTypefaceCache(uniqueIdF), nullptr);
    EXPECT_NE(RSTypefaceCache::Instance().GetDrawingTypefaceCache(uniqueIdS), nullptr);
}

/**
 * @tc.name: RemoveDrawingTypefacesByPidTest001
 * @tc.desc: Verify function RemoveDrawingTypefacesByPid
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, RemoveDrawingTypefacesByPidTest001, TestSize.Level1)
{
    uint64_t uniqueId = 2;
    pid_t pid = static_cast<pid_t>(uniqueId >> 32);
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(uniqueId);
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(pid);
    EXPECT_TRUE(RSTypefaceCache::Instance().typefaceHashCode_.empty());
}

/**
 * @tc.name: AddDelayDestroyQueueTest001
 * @tc.desc: Verify function AddDelayDestroyQueue
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, AddDelayDestroyQueueTest001, TestSize.Level1)
{
    uint64_t uniqueId = 1;
    RSTypefaceCache::Instance().AddDelayDestroyQueue(uniqueId);
    EXPECT_TRUE(RSTypefaceCache::Instance().typefaceHashCode_.empty());
}

/**
 * @tc.name: HandleDelayDestroyQueueTest001
 * @tc.desc: Verify function HandleDelayDestroyQueue
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, HandleDelayDestroyQueueTest001, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    uint64_t uniqueIdF = 1;
    uint64_t uniqueIdS = 2;
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueIdF, typeface);
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueIdS, typeface);
    RSTypefaceCache::Instance().HandleDelayDestroyQueue();
    RSTypefaceCache::Instance().Dump();
    EXPECT_FALSE(RSTypefaceCache::Instance().typefaceHashCode_.empty());
}

/**
 * @tc.name: ReplaySerializeTest001
 * @tc.desc: Verify function ReplaySerialize
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplaySerializeTest001, TestSize.Level1)
{
    auto typeface = Drawing::Typeface::MakeDefault();
    uint64_t uniqueId = 1;
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);

    size_t fontCount = 0;
    std::stringstream fontStream;
    RSTypefaceCache::Instance().ReplaySerialize(fontStream);
    fontStream.read(reinterpret_cast<char*>(&fontCount), sizeof(fontCount));
    EXPECT_NE(fontCount, 0);
}

/**
 * @tc.name: ReplayDeserializeTest001
 * @tc.desc: Verify function ReplayDeserialize
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplayDeserializeTest001, TestSize.Level1)
{
    uint64_t uniqueId = 1;
    uint64_t replayMask = (uint64_t)1 << (30 + 32);
    std::stringstream fontStream;
    RSTypefaceCache::Instance().ReplaySerialize(fontStream);
    RSTypefaceCache::Instance().ReplayDeserialize(fontStream);
    EXPECT_NE(RSTypefaceCache::Instance().typefaceHashCode_.find(uniqueId | replayMask),
              RSTypefaceCache::Instance().typefaceHashCode_.end());
}

/**
 * @tc.name: ReplayClearTest001
 * @tc.desc: Verify function ReplayClear
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplayClearTest001, TestSize.Level1)
{
    uint64_t uniqueId = 1;
    uint64_t replayMask = (uint64_t)1 << (30 + 32);
    RSTypefaceCache::Instance().ReplayClear();
    EXPECT_EQ(RSTypefaceCache::Instance().typefaceHashCode_.find(uniqueId | replayMask),
              RSTypefaceCache::Instance().typefaceHashCode_.end());
}

} // namespace Rosen
} // namespace OHOS