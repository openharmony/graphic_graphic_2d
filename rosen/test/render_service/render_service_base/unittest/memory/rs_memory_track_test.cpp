/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "memory/rs_memory_track.h"

namespace OHOS::Rosen {
class RSMemoryTrackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemoryTrackTest::SetUpTestCase() {}
void RSMemoryTrackTest::TearDownTestCase() {}
void RSMemoryTrackTest::SetUp() {}
void RSMemoryTrackTest::TearDown() {}

/**
 * @tc.name: PixelFormat2StringTest
 * @tc.desc: Test PixelFormat2String with all defined and invalid PixelFormat values.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, PixelFormat2StringTest, testing::ext::TestSize.Level1)
{
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::ARGB_8888), "ARGB_8888");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::RGB_565), "RGB_565");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::RGBA_8888), "RGBA_8888");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::BGRA_8888), "BGRA_8888");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::RGB_888), "RGB_888");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::ALPHA_8), "ALPHA_8");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::RGBA_F16), "RGBA_F16");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::NV21), "NV21");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::NV12), "NV12");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::RGBA_1010102), "RGBA_1010102");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::YCBCR_P010), "YCBCR_P010");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::YCRCB_P010), "YCRCB_P010");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::RGBA_U16), "RGBA_U16");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::YUV_400), "YUV_400");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::CMYK), "CMYK");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::ASTC_4x4), "ASTC_4x4");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::ASTC_6x6), "ASTC_6x6");
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(OHOS::Media::PixelFormat::ASTC_8x8), "ASTC_8x8");
    auto invalidFormat = static_cast<OHOS::Media::PixelFormat>(999);
    EXPECT_EQ(MemoryTrack::Instance().PixelFormat2String(invalidFormat), "999");
}

/**
 * @tc.name: PixelMapInfo2StringTest
 * @tc.desc: Test AllocatorType combinations in PixelMapInfo2String.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, PixelMapInfo2StringTest, testing::ext::TestSize.Level1)
{
    MemoryInfo info;
    info.allocType = OHOS::Media::AllocatorType::DEFAULT;
    info.pixelMapFormat = OHOS::Media::PixelFormat::RGB_565;
    EXPECT_TRUE(MemoryTrack::Instance().PixelMapInfo2String(info).find("DEFAULT") != std::string::npos);
    info.allocType = OHOS::Media::AllocatorType::HEAP_ALLOC;
    EXPECT_TRUE(MemoryTrack::Instance().PixelMapInfo2String(info).find("HEAP") != std::string::npos);
    info.allocType = OHOS::Media::AllocatorType::SHARE_MEM_ALLOC;
    EXPECT_TRUE(MemoryTrack::Instance().PixelMapInfo2String(info).find("SHARE_MEM") != std::string::npos);
    info.allocType = OHOS::Media::AllocatorType::CUSTOM_ALLOC;
    EXPECT_TRUE(MemoryTrack::Instance().PixelMapInfo2String(info).find("CUSTOM") != std::string::npos);
    info.allocType = OHOS::Media::AllocatorType::DMA_ALLOC;
    EXPECT_TRUE(MemoryTrack::Instance().PixelMapInfo2String(info).find("DMA") != std::string::npos);
    info.allocType = static_cast<OHOS::Media::AllocatorType>(99);
    EXPECT_TRUE(MemoryTrack::Instance().PixelMapInfo2String(info).find("UNKNOW") != std::string::npos);
}

/**
 * @tc.name: AllocatorType2StringTest_DefinedValues
 * @tc.desc: Test AllocatorType2String with all defined AllocatorType values.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, AllocatorType2StringTest, testing::ext::TestSize.Level1)
{
    EXPECT_EQ(MemoryTrack::Instance().AllocatorType2String(OHOS::Media::AllocatorType::DEFAULT), "DEFAULT");
    EXPECT_EQ(MemoryTrack::Instance().AllocatorType2String(OHOS::Media::AllocatorType::HEAP_ALLOC), "HEAP");
    EXPECT_EQ(MemoryTrack::Instance().AllocatorType2String(OHOS::Media::AllocatorType::SHARE_MEM_ALLOC), "SHARE_MEM");
    EXPECT_EQ(MemoryTrack::Instance().AllocatorType2String(OHOS::Media::AllocatorType::CUSTOM_ALLOC), "CUSTOM");
    EXPECT_EQ(MemoryTrack::Instance().AllocatorType2String(OHOS::Media::AllocatorType::DMA_ALLOC), "DMA");
    auto invalidType = static_cast<OHOS::Media::AllocatorType>(99);
    EXPECT_EQ(MemoryTrack::Instance().AllocatorType2String(invalidType), "UNKNOW");
}

/**
 * @tc.name: MemoryNodeOfPidConstructorTest
 * @tc.desc: Test the constructor of MemoryNodeOfPid.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, MemoryNodeOfPidConstructorTest, testing::ext::TestSize.Level1)
{
    size_t size = 1024;
    NodeId id = 1;
    MemoryNodeOfPid node(size, id);
    ASSERT_EQ(node.GetMemSize(), size);
}

/**
 * @tc.name: MemoryNodeOfPidGetMemSizeTest
 * @tc.desc: Test the GetMemSize method of MemoryNodeOfPid.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, MemoryNodeOfPidGetMemSizeTest, testing::ext::TestSize.Level1)
{
    MemoryNodeOfPid node;
    size_t size = 2048;
    node.SetMemSize(size);
    ASSERT_EQ(node.GetMemSize(), size);
}

/**
 * @tc.name: MemoryNodeOfPidSetMemSizeTest
 * @tc.desc: Test the SetMemSize method of MemoryNodeOfPid.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, MemoryNodeOfPidSetMemSizeTest, testing::ext::TestSize.Level1)
{
    MemoryNodeOfPid node;
    size_t size = 4096;
    node.SetMemSize(size);
    ASSERT_EQ(node.GetMemSize(), size);
}

/**
 * @tc.name: MemoryNodeOfPidOperatorEqualTest
 * @tc.desc: Test the operator== method of MemoryNodeOfPid.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, MemoryNodeOfPidOperatorEqualTest, testing::ext::TestSize.Level1)
{
    size_t size = 1024;
    NodeId id = 1;
    MemoryNodeOfPid node1(size, id);
    MemoryNodeOfPid node2(size, id);
    ASSERT_TRUE(node1 == node2);
}

/**
 * @tc.name: AddNodeRecordTest001
 * @tc.desc: Test adding a node record with valid parameters.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, AddNodeRecordTest001, testing::ext::TestSize.Level1)
{
    NodeId id = 1001;
    MemoryInfo info = {1024, ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemoryGraphic result = MemoryTrack::Instance().CountRSMemory(ExtractPid(id));
    ASSERT_GT(result.GetCpuMemorySize(), 0);
    MemoryTrack::Instance().RemoveNodeRecord(id);
}

/**
 * @tc.name: AddNodeRecordTest002
 * @tc.desc: Test adding a node record with duplicate NodeId.
 * @tc.type: FUNC
 */
HWTEST_F(RSMemoryTrackTest, AddNodeRecordTest002, testing::ext::TestSize.Level1)
{
    NodeId id = 1002;
    MemoryInfo info1 = {2048, ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryInfo info2 = {4096, ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info1);
    MemoryTrack::Instance().AddNodeRecord(id, info2);
    MemoryGraphic result = MemoryTrack::Instance().CountRSMemory(ExtractPid(id));
    ASSERT_EQ(result.GetCpuMemorySize(), 4096);
    MemoryTrack::Instance().RemoveNodeRecord(id);
}

/**
 * @tc.name: AddNodeRecordTest003
 * @tc.desc: Test adding node with same size as existing record.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, AddNodeRecordTest003, testing::ext::TestSize.Level1)
{
    NodeId id = 3001;
    MemoryInfo info1 = {2048, ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryInfo info2 = {2048, ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info1);
    MemoryTrack::Instance().AddNodeRecord(id, info2);
    MemoryGraphic result = MemoryTrack::Instance().CountRSMemory(ExtractPid(id));
    ASSERT_EQ(result.GetCpuMemorySize(), 2048);
    MemoryTrack::Instance().RemoveNodeRecord(id);
}

/**
 * @tc.name: RemoveNodeRecordTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemoveNodeRecordTest, testing::ext::TestSize.Level1)
{
    NodeId id = 1;
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
    MemoryNodeOfPid memoryNodeOfPid;
    auto mem = memoryNodeOfPid.GetMemSize();
    ASSERT_EQ(mem, 0);
    MemoryTrack::Instance().RemoveNodeRecord(id);
}

/**
 * @tc.name: CountRSMemoryTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, CountRSMemoryTest001, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    pid_t pid1 = -1;
    MemoryTrack::Instance().CountRSMemory(pid1);
    auto mem = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(mem, 0);
}

/**
 * @tc.name: CountRSMemoryTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, CountRSMemoryTest002, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    pid_t pid1 = 0;
    MemoryTrack::Instance().CountRSMemory(pid1);
    auto mem = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(mem, 0);
}

/**
 * @tc.name: CountRSMemoryTest003
 * @tc.desc: Test counting memory for a valid PID with multiple nodes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, CountRSMemoryTest003, testing::ext::TestSize.Level1)
{
    pid_t pid = 1234;
    NodeId id1 = 2001;
    NodeId id2 = 2002;
    MemoryInfo info1 = {1024, pid, id1, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryInfo info2 = {2048, pid, id2, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id1, info1);
    MemoryTrack::Instance().AddNodeRecord(id2, info2);
    MemoryGraphic result = MemoryTrack::Instance().CountRSMemory(pid);
    ASSERT_EQ(result.GetCpuMemorySize(), 1024 + 2048);
    MemoryTrack::Instance().RemoveNodeRecord(id1);
    MemoryTrack::Instance().RemoveNodeRecord(id2);
}

/**
 * @tc.name: CountRSMemoryTest004
 * @tc.desc: Test counting memory for PID with no nodes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, CountRSMemoryTest004, testing::ext::TestSize.Level1)
{
    pid_t pid = 4001;
    MemoryGraphic result = MemoryTrack::Instance().CountRSMemory(pid);
    ASSERT_EQ(result.GetCpuMemorySize(), 0);
    ASSERT_EQ(result.GetPid(), 0);
}

/**
 * @tc.name: DumpMemoryStatisticsTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryStatisticsTest, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string str1 = log.GetString();
    ASSERT_EQ(str1, "");
    std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func;
    MemoryTrack::Instance().DumpMemoryStatistics(log, func);
    std::string str2 = log.GetString();
    ASSERT_NE(str2, "");
}

/**
 * @tc.name: AddPictureRecordTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, AddPictureRecordTest, testing::ext::TestSize.Level1)
{
    const void* addr = nullptr;
    MemoryInfo info;
    MemoryTrack& test1 =  MemoryTrack::Instance();
    test1.AddPictureRecord(addr, info);
    EXPECT_TRUE(test1.memPicRecord_.count(addr));
}

/**
 * @tc.name: RemovePictureRecordTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemovePictureRecordTest, testing::ext::TestSize.Level1)
{
    const void* addr = nullptr;
    MemoryInfo info;
    MemoryTrack& test1 = MemoryTrack::Instance();
    test1.AddPictureRecord(addr, info);
    EXPECT_TRUE(test1.memPicRecord_.count(addr));
    test1.RemovePictureRecord(addr);
    EXPECT_FALSE(test1.memPicRecord_.count(addr));
}

/**
 * @tc.name: UpdatePictureInfoTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, UpdatePictureInfoTest, testing::ext::TestSize.Level1)
{
    const void* addr = nullptr;
    MemoryInfo info;
    NodeId nodeId = 1;
    pid_t pid = -1;
    MemoryTrack& test1 = MemoryTrack::Instance();
    test1.AddPictureRecord(addr, info);
    test1.UpdatePictureInfo(addr, nodeId, pid);
    auto itr = test1.memPicRecord_.find(addr);
    MemoryInfo info2 = itr->second;
    EXPECT_EQ(-1, info2.pid);  //1.for test
}

/**
 * @tc.name: GetAppMemorySizeInMBTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetAppMemorySizeInMBTest, testing::ext::TestSize.Level1)
{
    float ret = MemoryTrack::Instance().GetAppMemorySizeInMB();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetAppMemorySizeInMBTest002
 * @tc.desc: Test calculating app memory size with multiple pixel records.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetAppMemorySizeInMBTest002, testing::ext::TestSize.Level1)
{
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    const void* addr2 = reinterpret_cast<void*>(0x2000);
    MemoryInfo info1 = {2 * 1024 * 1024, 0, 0, MEMORY_TYPE::MEM_PIXELMAP};
    MemoryInfo info2 = {3 * 1024 * 1024, 0, 0, MEMORY_TYPE::MEM_PIXELMAP};
    MemoryTrack::Instance().AddPictureRecord(addr1, info1);
    MemoryTrack::Instance().AddPictureRecord(addr2, info2);
    float sizeMB = MemoryTrack::Instance().GetAppMemorySizeInMB();
    ASSERT_FLOAT_EQ(sizeMB, (2 + 3) / 2.0f); // 50% 的权重
    MemoryTrack::Instance().RemovePictureRecord(addr1);
    MemoryTrack::Instance().RemovePictureRecord(addr2);
}

/**
 * @tc.name: MemoryType2StringTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, MemoryType2StringTest001, testing::ext::TestSize.Level1)
{
    MEMORY_TYPE type = MEMORY_TYPE::MEM_PIXELMAP;
    const char* ret = MemoryTrack::Instance().MemoryType2String(type);
    ASSERT_EQ(ret, "pixelmap");
}

/**
 * @tc.name: MemoryType2StringTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, MemoryType2StringTest002, testing::ext::TestSize.Level1)
{
    MEMORY_TYPE type = MEMORY_TYPE::MEM_SKIMAGE;
    const char* ret = MemoryTrack::Instance().MemoryType2String(type);
    ASSERT_EQ(ret, "skimage");
}

/**
 * @tc.name: MemoryType2StringTest003
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, MemoryType2StringTest003, testing::ext::TestSize.Level1)
{
    MEMORY_TYPE type = MEMORY_TYPE::MEM_RENDER_NODE;
    const char* ret = MemoryTrack::Instance().MemoryType2String(type);
    ASSERT_EQ(ret, "");
}

/**
 * @tc.name: GenerateDumpTitleTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GenerateDumpTitleTest, testing::ext::TestSize.Level1)
{
    MemoryInfo info;
    // for test
    uint64_t windowId = 1;
    std::string windowName = "My Window";
    RectI nodeFrameRect;
    std::string ret = MemoryTrack::Instance().GenerateDumpTitle();
    ASSERT_TRUE(!ret.empty());
    ret = MemoryTrack::Instance().GenerateDetail(info, windowId, windowName, nodeFrameRect);
    ASSERT_TRUE(!ret.empty());
}

/**
 * @tc.name: DumpMemoryNodeStatisticsTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryNodeStatisticsTest, testing::ext::TestSize.Level1)
{
    DfxString log;
    MemoryTrack::Instance().DumpMemoryNodeStatistics(log);
    std::string  str = log.GetString();
    EXPECT_NE("", str);
}

/**
 * @tc.name: RemoveNodeFromMapTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemoveNodeFromMapTest001, testing::ext::TestSize.Level1)
{
    MemoryInfo info = {.pid = -1, .size = sizeof(10)}; //for test
    const NodeId id = 1; // fot test
    MemoryTrack& test1 = MemoryTrack::Instance();
    test1.AddNodeRecord(id, info);
    pid_t pidTest;
    size_t sizeTest;
    test1.RemoveNodeFromMap(id, pidTest, sizeTest);
    EXPECT_EQ(1, pidTest); //for test
    EXPECT_EQ(sizeof(10), sizeTest); //for test
}

/**
 * @tc.name: RemoveNodeFromMapTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemoveNodeFromMapTest002, testing::ext::TestSize.Level1)
{
    MemoryInfo info = {.pid = 0, .size = sizeof(10)}; //for test
    const NodeId id = 0; // fot test
    MemoryTrack& test1 = MemoryTrack::Instance();
    test1.AddNodeRecord(id, info);
    pid_t pidTest;
    size_t sizeTest;
    test1.RemoveNodeFromMap(id, pidTest, sizeTest);
    EXPECT_EQ(0, pidTest); //for test
    EXPECT_EQ(sizeof(10), sizeTest); //for test
}

/**
 * @tc.name: RemoveNodeFromMapTest003
 * @tc.desc: Test removing non-existent NodeId.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemoveNodeFromMapTest003, testing::ext::TestSize.Level1)
{
    NodeId invalidId = 9999;
    pid_t pid;
    size_t size;
    bool ret = MemoryTrack::Instance().RemoveNodeFromMap(invalidId, pid, size);
    ASSERT_FALSE(ret); // Should return false
}

/**
 * @tc.name: GetPictureRecordMemInfoTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetPictureRecordMemInfoTest001, testing::ext::TestSize.Level1)
{
    void* addr = reinterpret_cast<void*>(0x12345678); // for index
    MemoryInfo info = { 1, 2, 3, 4, MEMORY_TYPE::MEM_SKIMAGE, 5, // for test, compare
        OHOS::Media::AllocatorType::HEAP_ALLOC, OHOS::Media::PixelFormat::ARGB_8888};
    MemoryTrack::Instance().AddPictureRecord(addr, info);
    MemoryInfo testInfo = { 0 };
    bool ret = MemoryTrack::Instance().GetPictureRecordMemInfo(addr, testInfo);
    ASSERT_TRUE(ret);
    ASSERT_EQ(testInfo.size, info.size);
    ASSERT_EQ(testInfo.pid, info.pid);
    ASSERT_EQ(testInfo.nid, info.nid);
    ASSERT_EQ(testInfo.uid, info.uid);
    ASSERT_EQ(testInfo.type, info.type);
    ASSERT_EQ(testInfo.initialPid, info.initialPid);
    ASSERT_EQ(testInfo.allocType, info.allocType);
    ASSERT_EQ(testInfo.pixelMapFormat, info.pixelMapFormat);
    MemoryTrack::Instance().RemovePictureRecord(addr);
    ret = MemoryTrack::Instance().GetPictureRecordMemInfo(addr, testInfo);
    ASSERT_FALSE(ret);
}
} // namespace OHOS::Rosen