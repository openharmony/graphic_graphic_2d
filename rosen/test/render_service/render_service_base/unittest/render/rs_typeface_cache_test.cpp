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
#include "gmock/gmock.h"

#include "file_ex.h"
#include "memory/rs_memory_manager.h"
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

// Add test function forward declaration
void RemoveHashQueue(std::unordered_map<uint32_t, std::unordered_set<uint64_t>>& hashQueue, uint64_t uniqueId);
void PurgeMapWithPid(pid_t pid, std::unordered_map<uint32_t, std::unordered_set<uint64_t>>& map);

class MockRSTypefaceCache : public RSTypefaceCache {
public:
    MOCK_METHOD1(GetTypefacePid, pid_t(uint64_t uniqueId));
};

// 模拟单例类的函数
class SingletonMockRSTypefaceCache {
public:
    static MockRSTypefaceCache& Instance()
    {
        static MockRSTypefaceCache instance;
        return instance;
    }
};

/**
 * @tc.name: MemorySnapshotTest001
 * @tc.desc: Verify memory info of RSTypefaceCache
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, MemorySnapshotTest001, TestSize.Level1)
{
    // prepare: typeface, size, hash, MemoryOverCheck hook
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    EXPECT_NE(typeface, nullptr);
    uint32_t size = typeface->GetSize();
    EXPECT_NE(size, 0);
    uint32_t hash = typeface->GetHash();
    EXPECT_NE(hash, 0);
    // app1's pid is 123, app2's pid2 is 456, register same typeface
    pid_t pid1 = 123;
    pid_t pid2 = 456;
    uint64_t uniqueId1 = (((uint64_t)pid1) << 32) | (uint64_t)(typeface->GetUniqueID());
    uint64_t uniqueId2 = (((uint64_t)pid2) << 32) | (uint64_t)(typeface->GetUniqueID());

    // Get base memory snapshot info before test
    MemorySnapshotInfo baseInfo1;
    bool ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid1, baseInfo1);
    EXPECT_EQ(ret, false);
    MemorySnapshotInfo baseInfo2;
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid2, baseInfo2);
    EXPECT_EQ(ret, false);

    // branch1: pid1 register typeface
    uint8_t retHasTypeface = RSTypefaceCache::Instance().HasTypeface(uniqueId1, hash);
    EXPECT_EQ(retHasTypeface, Drawing::NO_REGISTER);
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId1, typeface);
    MemorySnapshotInfo currentInfo;
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid1, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, baseInfo1.cpuMemory + size);
    // branch2: whether pid2 typeface exits
    retHasTypeface = RSTypefaceCache::Instance().HasTypeface(uniqueId2, hash);
    EXPECT_EQ(retHasTypeface, Drawing::REGISTERED);
    currentInfo = { 0 };
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid2, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, baseInfo2.cpuMemory + size);
    // branch3: pid2 register typeface if exits
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId2, typeface);
    currentInfo = { 0 };
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid2, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, baseInfo2.cpuMemory + size);

    // pid1 ungister typeface
    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(uniqueId1);
    currentInfo = { 0 };
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid1, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, baseInfo1.cpuMemory);
    // pid2 ungister typeface
    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(uniqueId2);
    currentInfo = { 0 };
    ret = MemorySnapshot::Instance().GetMemorySnapshotInfoByPid(pid2, currentInfo);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(currentInfo.cpuMemory, baseInfo2.cpuMemory);

    // clear context
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(pid1);
    RSTypefaceCache::Instance().RemoveDrawingTypefacesByPid(pid2);
}

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
 * @tc.name: DumpTest001
 * @tc.desc: Verify function Dump(DfxString& log)
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, DumpTest001, TestSize.Level1) {
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/NotoSansCJK-Regular.ttc", content);
    auto typeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<uint8_t*>(content.data()), content.size(), 0, "");
    ASSERT_NE(typeface, nullptr);
    uint64_t uniqueId = typeface->GetHash();
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);
    DfxString log;
    RSTypefaceCache::Instance().Dump(log);
    EXPECT_TRUE(log.GetString().find("RSTypefaceCache Dump:") != std::string::npos);
    EXPECT_TRUE(log.GetString().find("pid") != std::string::npos);
    EXPECT_TRUE(log.GetString().find("hash_value") != std::string::npos);
    EXPECT_TRUE(log.GetString().find("familyname") != std::string::npos);
    EXPECT_TRUE(log.GetString().find("Pss") != std::string::npos);
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
 * @tc.name: ReplaySerializeTest002
 * @tc.desc: Verify function ReplaySerialize
 * @tc.type: FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplaySerializeTest002, TestSize.Level1)
{
    RSTypefaceCache typefaceCache;
    uint64_t uniqueId = 1;
    typefaceCache.typefaceHashCode_[uniqueId] = 0;
    uniqueId = 2; // font id
    uint64_t hash = 1;
    auto typeface = std::make_shared<Drawing::Typeface>(nullptr);
    typefaceCache.typefaceHashCode_[uniqueId] = hash;
    typefaceCache.typefaceHashMap_[hash] = std::make_tuple(typeface, 1);
    size_t fontCount = 0;
    std::stringstream stream;
    typefaceCache.ReplaySerialize(stream);
    stream.read(reinterpret_cast<char*>(&fontCount), sizeof(fontCount));
    EXPECT_FALSE(fontCount);
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
 * @tc.name: ReplayDeserializeTest002
 * @tc.desc: Verify function ReplayDeserialize
 * @tc.type: FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplayDeserializeTest002, TestSize.Level1)
{
    RSTypefaceCache typefaceCache;
    std::stringstream stream;
    size_t count = 1;
    stream.write(reinterpret_cast<char*>(&count), sizeof(count));
    uint64_t uniqueId = 0;
    stream.write(reinterpret_cast<char*>(&uniqueId), sizeof(uniqueId));
    constexpr size_t maxSize = 40'000'000; // font data max size
    size_t overloadSize = maxSize + 1;
    stream.write(reinterpret_cast<char*>(&overloadSize), sizeof(overloadSize));
    auto error = typefaceCache.ReplayDeserialize(stream);
    EXPECT_FALSE(error.empty());
}

/**
 * @tc.name: ReplayDeserializeTest003
 * @tc.desc: Verify function ReplayDeserialize
 * @tc.type: FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplayDeserializeTest003, TestSize.Level1)
{
    RSTypefaceCache typefaceCache;
    std::stringstream stream;
    size_t count = 1;
    stream.write(reinterpret_cast<char*>(&count), sizeof(count));
    uint64_t uniqueId = 0;
    stream.write(reinterpret_cast<char*>(&uniqueId), sizeof(uniqueId));
    size_t data = 1;
    size_t size = sizeof(data) + 1;
    stream.write(reinterpret_cast<char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<char*>(&data), sizeof(data));
    auto error = typefaceCache.ReplayDeserialize(stream);
    EXPECT_FALSE(error.empty());
}

/**
 * @tc.name: ReplayDeserializeTest004
 * @tc.desc: Verify function ReplayDeserialize
 * @tc.type: FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplayDeserializeTest004, TestSize.Level1)
{
    RSTypefaceCache typefaceCache;
    std::stringstream stream;
    size_t count = 1;
    stream.write(reinterpret_cast<char*>(&count), sizeof(count));
    uint64_t uniqueId = 0;
    stream.write(reinterpret_cast<char*>(&uniqueId), sizeof(uniqueId));
    size_t data = 1;
    size_t size = sizeof(data);
    stream.write(reinterpret_cast<char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<char*>(&data), sizeof(data));
    auto error = typefaceCache.ReplayDeserialize(stream);
    EXPECT_FALSE(error.empty());
}

/**
 * @tc.name: ReplayDeserializeTest005
 * @tc.desc: Verify function ReplayDeserialize
 * @tc.type: FUNC
 */
HWTEST_F(RSTypefaceCacheTest, ReplayDeserializeTest005, TestSize.Level1)
{
    RSTypefaceCache typefaceCache;
    auto typeface = Drawing::Typeface::MakeDefault();
    uint64_t uniqueId = 1;
    typefaceCache.CacheDrawingTypeface(uniqueId, typeface);

    std::stringstream stream;
    typefaceCache.ReplaySerialize(stream);
    auto error = typefaceCache.ReplayDeserialize(stream);
    EXPECT_TRUE(error.empty());
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

/**
 * @tc.name: HasTypeFaceTest001
 * @tc.desc: Verify function HasTypeface
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, HasTypeFaceTest001, TestSize.Level1)
{
    uint64_t uniqueId = 1;
    uint32_t hash = 2;
    uint8_t result = RSTypefaceCache::Instance().HasTypeface(uniqueId, hash);
    EXPECT_EQ(result, Drawing::REGISTERED);
}

/**
 * @tc.name: AddIfFound_001
 * @tc.desc: Verify function AddIfFound
 * @tc.type:FUNC
 * @tc.require:issueIB262Y
 */
HWTEST_F(RSTypefaceCacheTest, AddIfFound_001, TestSize.Level2)
{
    uint64_t uniqueId = 1;
    uint32_t hash = 2;

    EXPECT_CALL(SingletonMockRSTypefaceCache::Instance(), GetTypefacePid(_)).WillRepeatedly(testing::Return(0));
    EXPECT_FALSE(SingletonMockRSTypefaceCache::Instance().GetTypefacePid(uniqueId));

    SingletonMockRSTypefaceCache::Instance().typefaceHashMap_[hash] =
        std::tuple<std::shared_ptr<Drawing::Typeface>, uint32_t>(nullptr, 3);
    EXPECT_TRUE(SingletonMockRSTypefaceCache::Instance().typefaceHashMap_.find(hash) !=
        SingletonMockRSTypefaceCache::Instance().typefaceHashMap_.end());

    EXPECT_TRUE(SingletonMockRSTypefaceCache::Instance().AddIfFound(uniqueId, hash));
}

/**
 * @tc.name: HasTypeface_001
 * @tc.desc: Verify function HasTypeface
 * @tc.type:FUNC
 * @tc.require:issueIB262Y
 */
HWTEST_F(RSTypefaceCacheTest, HasTypeface_001, TestSize.Level2)
{
    uint64_t uniqueId = 1;
    uint32_t hash = 0;
    EXPECT_FALSE(hash);
    auto &typefaceHashCode = RSTypefaceCache::Instance().typefaceHashCode_;
    typefaceHashCode.clear();
    EXPECT_FALSE(typefaceHashCode.find(uniqueId) != typefaceHashCode.end());

    EXPECT_EQ(RSTypefaceCache::Instance().HasTypeface(uniqueId, hash), Drawing::NO_REGISTER);
}

/**
 * @tc.name: HasTypeface_002
 * @tc.desc: Verify function HasTypeface
 * @tc.type:FUNC
 * @tc.require:issueIB262Y
 */
HWTEST_F(RSTypefaceCacheTest, HasTypeface_002, TestSize.Level2)
{
    uint64_t uniqueId = 1;
    uint32_t hash = 2;
    EXPECT_TRUE(hash);

    RSTypefaceCache::Instance().typefaceHashQueue_[hash] = std::unordered_set<uint64_t>{};
    EXPECT_TRUE(RSTypefaceCache::Instance().typefaceHashQueue_.find(hash) !=
        RSTypefaceCache::Instance().typefaceHashQueue_.end());
    EXPECT_EQ(RSTypefaceCache::Instance().HasTypeface(uniqueId, hash), Drawing::REGISTERING);
}

/**
 * @tc.name: CacheDrawingTypeface_002
 * @tc.desc: Verify function CacheDrawingTypeface, !(typeface && uniqueId > 0)
 * @tc.type:FUNC
 * @tc.require:issueIB262Y
 */
HWTEST_F(RSTypefaceCacheTest, CacheDrawingTypeface_002, TestSize.Level2)
{
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    uint64_t uniqueId = 1;
    EXPECT_TRUE(!(typeface && uniqueId > 0));
    RSTypefaceCache::Instance().typefaceHashCode_.clear(); // key uniqueId not found
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);
    EXPECT_EQ(RSTypefaceCache::Instance().typefaceHashCode_.size(), 0);

    typeface = Drawing::Typeface::MakeDefault();
    EXPECT_NE(typeface, nullptr);
    uniqueId = 0;
    EXPECT_TRUE(!(typeface && uniqueId > 0));
    RSTypefaceCache::Instance().typefaceHashCode_.clear(); // key uniqueId not found
    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);
    EXPECT_EQ(RSTypefaceCache::Instance().typefaceHashCode_.size(), 0);
}

/**
 * @tc.name: PurgeMapWithPidTest
 * @tc.desc: Verify function PurgeMapWithPid
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, PurgeMapWithPidTest, TestSize.Level1) {
    std::unordered_map<uint32_t, std::unordered_set<uint64_t>> testMap1 = {
        {1, {0x0000000100010001, 0x0000000200020002, 0x0000000100030003}}, // pid 1, 2, 1
        {2, {0x0000000300040004, 0x0000000100050005}}, // pid 3, 1
        {3, {0x0000000200060006}} // pid 2
    };
    PurgeMapWithPid(1, testMap1);
    EXPECT_EQ(testMap1[1].size(), 1);
    EXPECT_EQ(testMap1[2].size(), 1);
    EXPECT_EQ(testMap1[3].size(), 1);

    std::unordered_map<uint32_t, std::unordered_set<uint64_t>> testMap2 = {
        {1, {0x0000000100010001}}, // only contain pid 1
        {2, {0x0000000200020002}}  // only contain pid 2
    };
    PurgeMapWithPid(1, testMap2);
    EXPECT_EQ(testMap2.size(), 1);

    std::unordered_map<uint32_t, std::unordered_set<uint64_t>> originalMap = {
        {1, {0x0001000100010001, 0x0002000200020002}},
        {2, {0x0003000300030003}}
    };
    auto mapCopy = originalMap;
    PurgeMapWithPid(999, mapCopy); // 999 is no exist pid
    EXPECT_EQ(mapCopy.size(), originalMap.size());
}

/**
 * @tc.name: RemoveHashQueueTest
 * @tc.desc: Verify function RemoveHashQueue
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, RemoveHashQueueTest, TestSize.Level1) {
    std::unordered_map<uint32_t, std::unordered_set<uint64_t>> emptyMap;
    RemoveHashQueue(emptyMap, 1001);
    EXPECT_TRUE(emptyMap.empty());
    std::unordered_map<uint32_t, std::unordered_set<uint64_t>> testMap = {
        {1, {1001, 1002, 1003}},
        {2, {2001, 2002}},
        {3, {3001}}
    };
    RemoveHashQueue(testMap, 1002);
    EXPECT_EQ(testMap[1].size(), 2);
    RemoveHashQueue(testMap, 3001);
    EXPECT_EQ(testMap.size(), 2);
    auto mapSizeBefore = testMap.size();
    RemoveHashQueue(testMap, 9999); // 9999 is no exist uniqueId
    EXPECT_EQ(testMap.size(), mapSizeBefore);
}

/**
 * @tc.name: InsertVariationTypeface001
 * @tc.desc: Verify function InsertVariationTypeface
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, InsertVariationTypeface001, TestSize.Level1)
{
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/Roboto-Regular.ttf", content);
    std::shared_ptr<Drawing::Typeface> baseTypeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<const uint8_t*>(content.data()), content.size(), 0, "test");
    ASSERT_NE(baseTypeface, nullptr);
    uint64_t baseUniqueId = RSTypefaceCache::GenGlobalUniqueId(baseTypeface->GetUniqueID());
    Drawing::FontArguments fontArgs;
    std::vector<Drawing::FontArguments::VariationPosition::Coordinate> coords = {{2003265652, 100.0f}};
    fontArgs.SetVariationDesignPosition({coords.data(), coords.size()});
    auto variationTypeface = baseTypeface->MakeClone(fontArgs);
    ASSERT_NE(variationTypeface, nullptr);
    uint64_t varUniqueId = RSTypefaceCache::GenGlobalUniqueId(variationTypeface->GetUniqueID());
    variationTypeface->SetFd(baseTypeface->GetFd());
    Drawing::SharedTypeface sharedTypeface(varUniqueId, variationTypeface);
    sharedTypeface.originId_ = baseUniqueId;

    int32_t result = RSTypefaceCache::Instance().InsertVariationTypeface(sharedTypeface);
    EXPECT_EQ(result, -1);

    RSTypefaceCache::Instance().CacheDrawingTypeface(baseUniqueId, baseTypeface);
    // not insert variation typeface
    result = RSTypefaceCache::Instance().InsertVariationTypeface(sharedTypeface);
    EXPECT_EQ(result, variationTypeface->GetFd());

    // has insert variation typeface
    result = RSTypefaceCache::Instance().InsertVariationTypeface(sharedTypeface);
    EXPECT_EQ(result, variationTypeface->GetFd());

    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(baseUniqueId);
    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(varUniqueId);

    // supplement coverage - clone failed
    variationTypeface->typefaceImpl_ = nullptr;
    RSTypefaceCache::Instance().CacheDrawingTypeface(baseUniqueId, variationTypeface);
    result = RSTypefaceCache::Instance().InsertVariationTypeface(sharedTypeface);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: UpdateDrawingTypefaceRefTest001
 * @tc.desc: Verify function UpdateDrawingTypefaceRef
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, UpdateDrawingTypefaceRefTest001, TestSize.Level1) {
    RSTypefaceCache::Instance().typefaceHashCode_.clear();
    RSTypefaceCache::Instance().typefaceHashMap_.clear();
    RSTypefaceCache::Instance().typefaceBaseHashMap_.clear();
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    EXPECT_NE(typeface, nullptr);
    pid_t pid1 = 123;
    uint64_t uniqueId = (((uint64_t)pid1) << 32) | (uint64_t)(typeface->GetUniqueID());
    EXPECT_EQ(RSTypefaceCache::Instance().GetDrawingTypefaceCache(uniqueId), nullptr);

    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);
    EXPECT_EQ(RSTypefaceCache::Instance().GetDrawingTypefaceCache(uniqueId), typeface);

    
    Drawing::SharedTypeface sharedTypeface(uniqueId, typeface);
    auto result = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_EQ(result, typeface);
}

/**
 * @tc.name: UpdateDrawingTypefaceRefTest002
 * @tc.desc: Verify function UpdateDrawingTypefaceRef
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, UpdateDrawingTypefaceRefTest002, TestSize.Level1) {
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    EXPECT_NE(typeface, nullptr);
    pid_t pid1 = 123;
    uint64_t uniqueId = (((uint64_t)pid1) << 32) | (uint64_t)(typeface->GetUniqueID());

    RSTypefaceCache::Instance().CacheDrawingTypeface(uniqueId, typeface);
    Drawing::SharedTypeface sharedTypeface(uniqueId, typeface);
    sharedTypeface.hasFontArgs_ = true;
    sharedTypeface.coords_ = { {2003265652, 100.0}, {2003072104, 62.5} };
    auto result = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_NE(result, typeface);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: UpdateDrawingTypefaceRefTest003
 * @tc.desc: Verify function UpdateDrawingTypefaceRef
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, UpdateDrawingTypefaceRefTest003, TestSize.Level1) {
    Drawing::SharedTypeface sharedTypeface;
    sharedTypeface.id_ = 3;
    sharedTypeface.hash_ = 789;
    auto result = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: UpdateDrawingTypefaceRefTest004
 * @tc.desc: Verify function UpdateDrawingTypefaceRef
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, UpdateDrawingTypefaceRefTest004, TestSize.Level1)
{
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/Roboto-Regular.ttf", content);
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<const uint8_t*>(content.data()), content.size(), 0, "test");
    ASSERT_NE(typeface, nullptr);
    pid_t pid = getpid();
    Drawing::SharedTypeface sharedTypeface(
        (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(typeface->GetUniqueID()), typeface);
    sharedTypeface.hasFontArgs_ = true;
    sharedTypeface.coords_ = { {2003265652, 100.0}, {2003072104, 62.5} };
    auto result = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_EQ(result, nullptr);
    RSTypefaceCache::Instance().CacheDrawingTypeface(sharedTypeface.id_, typeface);
    sharedTypeface.hasFontArgs_ = true;
    sharedTypeface.coords_ = { {2003265652, 80.0}, {2003072104, 62.5} };
    auto result1 = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_NE(result1, typeface);
    EXPECT_NE(result1, nullptr);
}

/**
 * @tc.name: UpdateDrawingTypefaceRefTest005
 * @tc.desc: Verify function RemoveHashMap
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, UpdateDrawingTypefaceRefTest005, TestSize.Level1)
{
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/Roboto-Regular.ttf", content);
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<const uint8_t*>(content.data()), content.size(), 0, "test");
    ASSERT_NE(typeface, nullptr);
    pid_t pid = getpid();
    Drawing::SharedTypeface sharedTypeface(
        (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(typeface->GetUniqueID()), typeface);
    sharedTypeface.hasFontArgs_ = true;
    sharedTypeface.coords_ = { {2003265652, 100.0}, {2003072104, 62.5} };
    auto result = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_NE(result, nullptr);
    RSTypefaceCache::Instance().CacheDrawingTypeface(sharedTypeface.id_, typeface);
    sharedTypeface.hasFontArgs_ = true;
    sharedTypeface.coords_ = { {2003265652, 80.0}, {2003072104, 62.5}, {2003072104, 72.5} };
    auto result1 = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_NE(result1, typeface);
    EXPECT_NE(result1, nullptr);
    sharedTypeface.hasFontArgs_ = true;
    sharedTypeface.coords_ = { {2003265652, 90.0}, {2003072104, 62.5}, {2003072104, 72.5}, {2003072104, 92.5} };
    auto result2 = RSTypefaceCache::Instance().UpdateDrawingTypefaceRef(sharedTypeface);
    EXPECT_NE(result2, typeface);
    EXPECT_NE(result2, nullptr);
    RSTypefaceCache::Instance().RemoveDrawingTypefaceByGlobalUniqueId(sharedTypeface.id_);
}

/**
 * @tc.name: UpdateDrawingTypefaceRefTest006
 * @tc.desc: Verify function RemoveHashMap
 * @tc.type:FUNC
 */
HWTEST_F(RSTypefaceCacheTest, UpdateDrawingTypefaceRefTest006, TestSize.Level1)
{
    std::vector<char> content;
    LoadBufferFromFile("/system/fonts/Roboto-Regular.ttf", content);
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromAshmem(reinterpret_cast<const uint8_t*>(content.data()), content.size(), 0, "test");
    ASSERT_NE(typeface, nullptr);
    pid_t pid = getpid();
    uint64_t globalID = (static_cast<uint64_t>(pid) << 32) | static_cast<uint64_t>(1);
    std::unordered_map<uint64_t, TypefaceTuple> typefaceHashMap;
    typefaceHashMap[globalID] = std::make_tuple(typeface, 1);
    RSTypefaceCache::Instance().RemoveHashMap(pid, typefaceHashMap, globalID);
}
} // namespace Rosen
} // namespace OHOS