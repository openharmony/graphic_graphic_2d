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
#include <fstream>
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
    EXPECT_EQ(node.GetMemSize(), size);
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
    EXPECT_EQ(node.GetMemSize(), size);
}

/**
 * @tc.name: RegisterNodeMemTest001
 * @tc.desc: Test RegisterNodeMemTest001.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RegisterNodeMemTest001, testing::ext::TestSize.Level1)
{
    pid_t testPid = -1;
    size_t testSize = 100;
    MemoryTrack::Instance().RegisterNodeMem(testPid, testSize, MEMORY_TYPE::MEM_RENDER_NODE);
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.first, testSize);
}

/**
 * @tc.name: RegisterNodeMemTest002
 * @tc.desc: Test RegisterNodeMemTest002.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RegisterNodeMemTest002, testing::ext::TestSize.Level1)
{
    pid_t testPid = -1;
    size_t testSize = 200;
    MemoryTrack::Instance().RegisterNodeMem(testPid, testSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.second, testSize);
}

/**
 * @tc.name: RegisterNodeMemTest003
 * @tc.desc: Test RegisterNodeMemTest003.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RegisterNodeMemTest003, testing::ext::TestSize.Level1)
{
    pid_t testPid = -2;
    size_t testSize = 150;
    MemoryTrack::Instance().RegisterNodeMem(testPid, testSize, static_cast<MEMORY_TYPE>(999));
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.first, 0);
    EXPECT_EQ(memData.second, 0);
}

/**
 * @tc.name: UnRegisterNodeMemTest001
 * @tc.desc: Test RegisterNodeMemTest001.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, UnRegisterNodeMemTest001, testing::ext::TestSize.Level1)
{
    pid_t testPid = -3;
    size_t initialSize = 200;
    size_t unregisterSize = 100;
    MemoryTrack::Instance().RegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_NODE);
    MemoryTrack::Instance().UnRegisterNodeMem(testPid, unregisterSize, MEMORY_TYPE::MEM_RENDER_NODE);
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.first, initialSize - unregisterSize);
    EXPECT_EQ(memData.second, 0);
}

/**
 * @tc.name: UnRegisterNodeMemTest002
 * @tc.desc: Test RegisterNodeMemTest002.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, UnRegisterNodeMemTest002, testing::ext::TestSize.Level1)
{
    pid_t testPid = -3;
    size_t initialSize = 300;
    size_t unregisterSize = 150;
    MemoryTrack::Instance().RegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    MemoryTrack::Instance().UnRegisterNodeMem(testPid, unregisterSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.second, initialSize - unregisterSize);
}

/**
 * @tc.name: UnRegisterNodeMemTest003
 * @tc.desc: Test RegisterNodeMemTest003.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, UnRegisterNodeMemTest003, testing::ext::TestSize.Level1)
{
    pid_t testPid = -4;
    size_t initialSize = 200;
    size_t unregisterSize = 100;
    MemoryTrack::Instance().RegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_NODE);
    MemoryTrack::Instance().UnRegisterNodeMem(testPid, unregisterSize, static_cast<MEMORY_TYPE>(999));
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.first, initialSize);
}

/**
 * @tc.name: UnRegisterNodeMemTest004
 * @tc.desc: Test RegisterNodeMemTest004.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, UnRegisterNodeMemTest004, testing::ext::TestSize.Level1)
{
    pid_t testPid = -10;
    size_t initialSize = 100;
    size_t unregisterSize = 200;
    MemoryTrack::Instance().RegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_NODE);
    MemoryTrack::Instance().UnRegisterNodeMem(testPid, unregisterSize, MEMORY_TYPE::MEM_RENDER_NODE);
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.first, 0);
}

/**
 * @tc.name: UnRegisterNodeMemTest005
 * @tc.desc: Test RegisterNodeMemTest005.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, UnRegisterNodeMemTest005, testing::ext::TestSize.Level1)
{
    pid_t testPid = -11;
    size_t initialSize = 100;
    size_t unregisterSize = 200;
    MemoryTrack::Instance().RegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    MemoryTrack::Instance().UnRegisterNodeMem(testPid, unregisterSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    auto& memData = MemoryTrack::Instance().nodeMemOfPid_[testPid];
    EXPECT_EQ(memData.second, 0);
}

/**
 * @tc.name: UnRegisterNodeMemTest006
 * @tc.desc: Test RegisterNodeMemTest006.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, UnRegisterNodeMemTest006, testing::ext::TestSize.Level1)
{
    pid_t testPid = -12;
    size_t initialSize = 100;
    MemoryTrack::Instance().RegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_NODE);
    MemoryTrack::Instance().RegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    MemoryTrack::Instance().UnRegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_NODE);
    MemoryTrack::Instance().UnRegisterNodeMem(testPid, initialSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    EXPECT_EQ(MemoryTrack::Instance().nodeMemOfPid_.find(testPid), MemoryTrack::Instance().nodeMemOfPid_.end());
}

/**
 * @tc.name: GetNodeMemoryOfPid001
 * @tc.desc: Test GetNodeMemoryOfPid001.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetNodeMemoryOfPid001, testing::ext::TestSize.Level1)
{
    pid_t testPid = -5;
    size_t registeredSize = 1000;
    size_t expectedSize = registeredSize / BYTE_CONVERT;
    MemoryTrack::Instance().RegisterNodeMem(testPid, registeredSize, MEMORY_TYPE::MEM_RENDER_NODE);
    size_t result = MemoryTrack::Instance().GetNodeMemoryOfPid(testPid, MEMORY_TYPE::MEM_RENDER_NODE);
    EXPECT_EQ(result, expectedSize);
}

/**
 * @tc.name: GetNodeMemoryOfPid002
 * @tc.desc: Test GetNodeMemoryOfPid002.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetNodeMemoryOfPid002, testing::ext::TestSize.Level1)
{
    pid_t testPid = -5;
    size_t registeredSize = 2000;
    size_t expectedSize = registeredSize / BYTE_CONVERT;
    MemoryTrack::Instance().RegisterNodeMem(testPid, registeredSize, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    size_t result = MemoryTrack::Instance().GetNodeMemoryOfPid(testPid, MEMORY_TYPE::MEM_RENDER_DRAWABLE_NODE);
    EXPECT_EQ(result, expectedSize);
}

/**
 * @tc.name: GetNodeMemoryOfPid003
 * @tc.desc: Test GetNodeMemoryOfPid003.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetNodeMemoryOfPid003, testing::ext::TestSize.Level1)
{
    pid_t testPid = -5;
    size_t registeredSize = 1000;
    MemoryTrack::Instance().RegisterNodeMem(testPid, registeredSize, MEMORY_TYPE::MEM_RENDER_NODE);
    size_t result = MemoryTrack::Instance().GetNodeMemoryOfPid(testPid, static_cast<MEMORY_TYPE>(999));
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: GetNodeMemoryOfPid004
 * @tc.desc: Test GetNodeMemoryOfPid004.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetNodeMemoryOfPid004, testing::ext::TestSize.Level1)
{
    pid_t testPid = -5;
    pid_t nonExistentPid = -6;
    size_t registeredSize = 1000;
    MemoryTrack::Instance().RegisterNodeMem(testPid, registeredSize, MEMORY_TYPE::MEM_RENDER_NODE);
    size_t result = MemoryTrack::Instance().GetNodeMemoryOfPid(nonExistentPid, MEMORY_TYPE::MEM_RENDER_NODE);
    EXPECT_EQ(result, 0);
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
    EXPECT_EQ(node.GetMemSize(), size);
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
    EXPECT_TRUE(node1 == node2);
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
    EXPECT_GT(result.GetCpuMemorySize(), 0);
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
    EXPECT_EQ(result.GetCpuMemorySize(), 4096);
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
    EXPECT_EQ(result.GetCpuMemorySize(), 2048);
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
    EXPECT_EQ(mem, 0);
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
    EXPECT_EQ(mem, 0);
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
    EXPECT_EQ(mem, 0);
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
 * @tc.name: DumpMemoryStatisticsTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryStatisticsTest001, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string str1 = log.GetString();
    ASSERT_EQ(str1, "");
    bool isLite = false;
    std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func;
    MemoryTrack::Instance().DumpMemoryStatistics(log, func, isLite);
    std::string str2 = log.GetString();
    ASSERT_NE(str2, "");
}

/**
 * @tc.name: DumpMemoryStatisticsTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryStatisticsTest002, testing::ext::TestSize.Level1)
{
    DfxString log;
    std::string str1 = log.GetString();
    ASSERT_EQ(str1, "");
    bool isLite = true;
    std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func;
    MemoryTrack::Instance().DumpMemoryStatistics(log, func, isLite);
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
 * @tc.name: AddPictureRecordTest2
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, AddPictureRecordTest2, testing::ext::TestSize.Level1)
{
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    const void* addr2 = reinterpret_cast<void*>(0x2000);
    const void* addr3 = reinterpret_cast<void*>(0x3000);
    MemoryInfo info1 = {2 * 1024 * 1024, 123, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryInfo info2 = {3 * 1024 * 1024, 234, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryInfo info3 = {3 * 1024 * 1024, 345, 0, 0, MEMORY_TYPE::MEM_SKIMAGE,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryTrack& test1 =  MemoryTrack::Instance();
    test1.AddPictureRecord(addr1, info1);
    test1.AddPictureRecord(addr2, info2);
    test1.AddPictureRecord(addr3, info3);
    EXPECT_EQ(test1.CountFdRecordOfPid(123), 1);
    EXPECT_EQ(test1.CountFdRecordOfPid(234), 1);
    EXPECT_EQ(test1.CountFdRecordOfPid(345), 0);
    test1.RemovePictureRecord(addr1);
    test1.RemovePictureRecord(addr2);
    test1.RemovePictureRecord(addr3);
}

/**
 * @tc.name: AddPictureRecordTest3
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, AddPictureRecordTest3, testing::ext::TestSize.Level1)
{
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    const void* addr2 = reinterpret_cast<void*>(0x2000);
    const void* addr3 = reinterpret_cast<void*>(0x3000);
    MemoryInfo info1 = {2 * 1024 * 1024, 123, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryTrack& test1 =  MemoryTrack::Instance();
    test1.AddPictureRecord(addr1, info1);
    test1.AddPictureRecord(addr2, info1);
    test1.AddPictureRecord(addr3, info1);
    EXPECT_EQ(test1.CountFdRecordOfPid(123), 3);
    test1.RemovePictureRecord(addr1);
    test1.RemovePictureRecord(addr2);
    test1.RemovePictureRecord(addr3);
}

/**
 * @tc.name: CountFdRecordOfPidTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, CountFdRecordOfPidTest, testing::ext::TestSize.Level1)
{
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    MemoryInfo info1 = {2 * 1024 * 1024, 123, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryTrack& test1 =  MemoryTrack::Instance();
    test1.AddPictureRecord(addr1, info1);
    EXPECT_EQ(test1.CountFdRecordOfPid(123), 1);
    test1.RemovePictureRecord(addr1);
}

/**
 * @tc.name: CountFdRecordOfPidTest2
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, CountFdRecordOfPidTest2, testing::ext::TestSize.Level1)
{
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    MemoryInfo info1 = {2 * 1024 * 1024, 123, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryTrack& test1 =  MemoryTrack::Instance();
    EXPECT_EQ(test1.CountFdRecordOfPid(234), 0);
    EXPECT_EQ(test1.CountFdRecordOfPid(123), 0);
    test1.AddPictureRecord(addr1, info1);
    EXPECT_EQ(test1.CountFdRecordOfPid(123), 1);
    test1.RemovePictureRecord(addr1);
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
 * @tc.name: RemovePictureRecordTest2
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemovePictureRecordTest2, testing::ext::TestSize.Level1)
{
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    const void* addr2 = reinterpret_cast<void*>(0x2000);
    const void* addr3 = reinterpret_cast<void*>(0x3000);
    MemoryInfo info1 = {2 * 1024 * 1024, 123, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryInfo info2 = {3 * 1024 * 1024, 234, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryInfo info3 = {3 * 1024 * 1024, 345, 0, 0, MEMORY_TYPE::MEM_SKIMAGE,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryTrack& test1 =  MemoryTrack::Instance();
    test1.AddPictureRecord(addr1, info1);
    test1.AddPictureRecord(addr2, info2);
    test1.AddPictureRecord(addr3, info3);
    test1.RemovePictureRecord(addr1);
    EXPECT_EQ(test1.CountFdRecordOfPid(123), 0);
    EXPECT_EQ(test1.CountFdRecordOfPid(234), 1);
    EXPECT_EQ(test1.CountFdRecordOfPid(345), 0);
    test1.RemovePictureRecord(addr2);
    test1.RemovePictureRecord(addr3);
}

/**
 * @tc.name: RemovePictureRecordTest3
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemovePictureRecordTest3, testing::ext::TestSize.Level1)
{
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    const void* addr2 = reinterpret_cast<void*>(0x2000);
    const void* addr3 = reinterpret_cast<void*>(0x3000);
    MemoryInfo info1 = {2 * 1024 * 1024, 123, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryTrack& test1 =  MemoryTrack::Instance();
    test1.AddPictureRecord(addr1, info1);
    test1.AddPictureRecord(addr2, info1);
    test1.AddPictureRecord(addr3, info1);
    test1.RemovePictureRecord(addr1);
    EXPECT_EQ(test1.CountFdRecordOfPid(123), 2);
    test1.RemovePictureRecord(addr2);
    test1.RemovePictureRecord(addr3);
}

/**
 * @tc.name: KillProcessByPid
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, KillProcessByPid, testing::ext::TestSize.Level1)
{
    uint32_t pid = 1234;
    std::string reason = "test reason";
    
    const void* addr1 = reinterpret_cast<void*>(0x1000);
    MemoryInfo info1 = {2 * 1024 * 1024, pid, 0, 0, MEMORY_TYPE::MEM_PIXELMAP,
        Media::AllocatorType::SHARE_MEM_ALLOC, Media::PixelFormat::ARGB_8888};
    MemoryTrack& test1 =  MemoryTrack::Instance();
    test1.AddPictureRecord(addr1, info1);
    EXPECT_EQ(test1.CountFdRecordOfPid(pid), 1);
    MemoryTrack::Instance().KillProcessByPid(pid, reason);
    test1.RemovePictureRecord(addr1);
}

/**
 * @tc.name: FdOverReport
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, FdOverReport, testing::ext::TestSize.Level1)
{
    pid_t pid = 1234;
    std::string hidumperReport = "report";
    MemoryTrack::Instance().FdOverReport(pid, "RENDER_MEMORY_OVER_WARNING", hidumperReport);
    std::string filePath = "/data/service/el0/render_service/renderservice_fdmem.txt";
    ASSERT_FALSE(std::ifstream(filePath).good());
}

/**
 * @tc.name: WriteInfoToFile
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, WriteInfoToFile, testing::ext::TestSize.Level1)
{
    std::string memInfo = "info";
    std::string hidumperReport = "";
    std::string filePath = "/data/service/el0/render_service/renderservice_fdmem.txt";
    MemoryTrack::Instance().WriteInfoToFile(filePath, memInfo, hidumperReport);
    ASSERT_FALSE(std::ifstream(filePath).good());
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
    EXPECT_EQ(ret, 0);
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
    EXPECT_EQ(ret, "pixelmap");
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
    EXPECT_EQ(ret, "skimage");
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
    EXPECT_EQ(ret, "");
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
    EXPECT_TRUE(!ret.empty());
    ret = MemoryTrack::Instance().GenerateDetail(info, windowId, windowName, nodeFrameRect);
    EXPECT_TRUE(!ret.empty());
}

/**
 * @tc.name: DumpMemoryNodeStatisticsTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryNodeStatisticsTest001, testing::ext::TestSize.Level1)
{
    DfxString log;
    bool isLite = false;
    MemoryTrack::Instance().DumpMemoryNodeStatistics(log, isLite);
    std::string  str = log.GetString();
    EXPECT_NE("", str);
}

/**
 * @tc.name: DumpMemoryNodeStatisticsTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryNodeStatisticsTest002, testing::ext::TestSize.Level1)
{
    DfxString log;
    bool isLite = true;
    MemoryTrack::Instance().DumpMemoryNodeStatistics(log, isLite);
    std::string  str = log.GetString();
    EXPECT_NE("", str);
}

/**
 * @tc.name: DumpMemoryPicStatisticsTest001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryPicStatisticsTest001, testing::ext::TestSize.Level1)
{
    DfxString log;
    bool isLite = false;
    std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func =
        [] (uint64_t) {
            return std::make_tuple(0ULL, "", RectI(), false);
        };
    MemoryTrack::Instance().DumpMemoryPicStatistics(log, func, isLite);
    std::string  str = log.GetString();
    EXPECT_NE("", str);
}

/**
 * @tc.name: DumpMemoryPicStatisticsTest002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, DumpMemoryPicStatisticsTest002, testing::ext::TestSize.Level1)
{
    DfxString log;
    bool isLite = true;
    std::function<std::tuple<uint64_t, std::string, RectI, bool> (uint64_t)> func =
        [] (uint64_t) {
            return std::make_tuple(0ULL, "", RectI(), false);
        };
    MemoryTrack::Instance().DumpMemoryPicStatistics(log, func, isLite);
    std::string  str = log.GetString();
    EXPECT_NE("", str);
}

/**
 * @tc.name: RemovePidRecordTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, RemovePidRecordTest, testing::ext::TestSize.Level1)
{
    pid_t pidTest = -1;
    MemoryTrack::Instance().RemovePidRecord(pidTest);
    EXPECT_EQ(-1, pidTest); //for test
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
    EXPECT_EQ(-1, pidTest); //for test
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

#ifdef RS_MEMORY_INFO_MANAGER
/**
 * @tc.name: SetNodeOnTreeStatusTest001
 * @tc.desc: Test setting non-existent node on tree status.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, SetNodeOnTreeStatusTest001, testing::ext::TestSize.Level1)
{
    NodeId invalidId = 9999;
    bool isRootNodeOnTreeChanged = true;
    bool isOnTree = true;
    const void* addr = &invalidId;
    MemoryTrack::Instance().SetNodeOnTreeStatus(invalidId, isRootNodeOnTreeChanged, isOnTree);
    EXPECT_EQ(MemoryTrack::Instance().GetNodeOnTreeStatus(addr), NODE_ON_TREE_STATUS::STATUS_INVALID);
}

/**
 * @tc.name: GetNodeOnTreeStatusTest001
 * @tc.desc: Test getting non-existent node on tree status.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetNodeOnTreeStatusTest001, testing::ext::TestSize.Level1)
{
    NodeId invalidId = 9999;
    const void* addr = &invalidId;
    EXPECT_EQ(MemoryTrack::Instance().GetNodeOnTreeStatus(addr), NODE_ON_TREE_STATUS::STATUS_INVALID);
}

/**
 * @tc.name: SetNodeOnTreeStatusTest002
 * @tc.desc: Test setting node on tree status and get it.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, GetNodeOnTreeStatusTest002, testing::ext::TestSize.Level1)
{
    NodeId invalidId = 9999;
    bool isRootNodeOnTreeChanged = true;
    bool isOnTree = true;
    MemoryInfo info;
    info.nid = invalidId;
    const void* addr = &invalidId;
    MemoryTrack::Instance().AddPictureRecord(addr, info);
    EXPECT_EQ(MemoryTrack::Instance().GetNodeOnTreeStatus(addr), NODE_ON_TREE_STATUS::STATUS_INVALID);
    MemoryTrack::Instance().AddNodeRecord(invalidId, info);
    MemoryTrack::Instance().SetNodeOnTreeStatus(invalidId, isRootNodeOnTreeChanged, isOnTree);
    EXPECT_EQ(MemoryTrack::Instance().GetNodeOnTreeStatus(addr), NODE_ON_TREE_STATUS::STATUS_ON_TREE_IN_ROOT);
    isRootNodeOnTreeChanged = false;
    isOnTree = true;
    MemoryTrack::Instance().SetNodeOnTreeStatus(invalidId, isRootNodeOnTreeChanged, isOnTree);
    EXPECT_EQ(MemoryTrack::Instance().GetNodeOnTreeStatus(addr), NODE_ON_TREE_STATUS::STATUS_ON_TREE);
    isRootNodeOnTreeChanged = true;
    isOnTree = false;
    MemoryTrack::Instance().SetNodeOnTreeStatus(invalidId, isRootNodeOnTreeChanged, isOnTree);
    EXPECT_EQ(MemoryTrack::Instance().GetNodeOnTreeStatus(addr), NODE_ON_TREE_STATUS::STATUS_OFF_TREE_IN_ROOT);
    isRootNodeOnTreeChanged = false;
    isOnTree = false;
    MemoryTrack::Instance().SetNodeOnTreeStatus(invalidId, isRootNodeOnTreeChanged, isOnTree);
    EXPECT_EQ(MemoryTrack::Instance().GetNodeOnTreeStatus(addr), NODE_ON_TREE_STATUS::STATUS_OFF_TREE);
}
#endif

/**
 * @tc.name: FilterAshmemInfoByPid_NormalCase
 * @tc.desc: Test filtering ashmem log by pid with matching entries.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, FilterAshmemInfoByPid_NormalCase, testing::ext::TestSize.Level1)
{
    std::string input =
        "Process ashmem overview info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name Virtual_size Physical_size\n"
        "Total ashmem  of [xxxxx] virtual size is  541, physical size is 4096\n"
        "Process ashmem detail info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name\tProcess_ID\tFd\tCnode_idx\tApplicant_Pid\tAshmem_name\tVirtual_size\tPhysical_size\tmagic\n"
        "XXXXX\t815\t22\t328234\t816\tdev/ashmem/EXTRawData\t541\t4096\t7\n"
        "XXXXX\t816\t22\t328234\t816\tdev/ashmem/EXTRawData\t541\t4096\t7\n"
        "XXXXX\t817\t22\t328234\t816\tdev/ashmem/EXTRawData\t541\t4096\t7\n"
        "---------------------------------------------------------------------------------\n";

    std::string expectedOutput =
        "Process ashmem overview info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name Virtual_size Physical_size\n"
        "Total ashmem  of [xxxxx] virtual size is  541, physical size is 4096\n"
        "Process ashmem detail info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name\tProcess_ID\tFd\tCnode_idx\tApplicant_Pid\tAshmem_name\tVirtual_size\tPhysical_size\tmagic\n"
        "XXXXX\t816\t22\t328234\t816\tdev/ashmem/EXTRawData\t541\t4096\t7\n"
        "---------------------------------------------------------------------------------\n";

    std::string output;
    MemoryTrack::FilterAshmemInfoByPid(output, input, 816);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterAshmemInfoByPid_EmptyInput
 * @tc.desc: Test filtering ashmem log with empty input.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, FilterAshmemInfoByPid_EmptyInput, testing::ext::TestSize.Level1)
{
    std::string input = "";
    std::string expectedOutput = "";
    std::string output;
    MemoryTrack::FilterAshmemInfoByPid(output, input, 816);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterAshmemInfoByPid_NoMatch
 * @tc.desc: Test filtering ashmem log with no matching pid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, FilterAshmemInfoByPid_NoMatch, testing::ext::TestSize.Level1)
{
    std::string input =
        "Process ashmem overview info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name Virtual_size Physical_size\n"
        "Total ashmem  of [xxxxx] virtual size is  541, physical size is 4096\n"
        "Process ashmem detail info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name\tProcess_ID\tFd\tCnode_idx\tApplicant_Pid\tAshmem_name\tVirtual_size\tPhysical_size\tmagic\n"
        "XXXXX\t815\t22\t328234\t816\tdev/ashmem/EXTRawData\t541\t4096\t7\n"
        "---------------------------------------------------------------------------------\n";

    std::string expectedOutput =
        "Process ashmem overview info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name Virtual_size Physical_size\n"
        "Total ashmem  of [xxxxx] virtual size is  541, physical size is 4096\n"
        "Process ashmem detail info:\n"
        "---------------------------------------------------------------------------------\n"
        "Process_name\tProcess_ID\tFd\tCnode_idx\tApplicant_Pid\tAshmem_name\tVirtual_size\tPhysical_size\tmagic\n"
        "---------------------------------------------------------------------------------\n";

    std::string output;
    MemoryTrack::FilterAshmemInfoByPid(output, input, 999);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterDmaheapInfoByPid_NormalCase
 * @tc.desc: Test filtering dmaheap log by pid with matching entries.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, FilterDmaheapInfoByPid_NormalCase, testing::ext::TestSize.Level1)
{
    std::string input =
        "Dma-buf objects usage of processes:\n"
        "Process    pid    fd    size_bytes    ino    exp_pid    exp_task_comm    buf_name    exp_name\n"
        "xxxxx    439    14    12288    2    439    xxxxx    NULL    vmalloc\n"
        "xxxxx    439    17    12288    5    439    xxxxx    NULL    vmalloc\n"
        "Total dmabuf size of xxxxx: 49152 bytes\n"
        "yyyyy    500     12    3686400    1    500    yyyyy    NULL    vmalloc\n"
        "yyyyy    500     20    3686400    42    499    yyyyy    525    vmalloc\n"
        "Total dmabuf size of yyyyy: 25804800 bytes\n";

    std::string expectedOutput =
        "Dma-buf objects usage of processes:\n"
        "Process    pid    fd    size_bytes    ino    exp_pid    exp_task_comm    buf_name    exp_name\n"
        "yyyyy    500     12    3686400    1    500    yyyyy    NULL    vmalloc\n"
        "yyyyy    500     20    3686400    42    499    yyyyy    525    vmalloc\n"
        "Total dmabuf size of yyyyy: 25804800 bytes\n";

    std::string output;
    MemoryTrack::FilterDmaheapInfoByPid(output, input, 500);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterDmaheapInfoByPid_EmptyInput
 * @tc.desc: Test filtering dmaheap log with empty input.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, FilterDmaheapInfoByPid_EmptyInput, testing::ext::TestSize.Level1)
{
    std::string input = "";
    std::string expectedOutput = "";
    std::string output;
    MemoryTrack::FilterDmaheapInfoByPid(output, input, 500);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterDmaheapInfoByPid_NoMatch
 * @tc.desc: Test filtering dmaheap log with no matching pid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryTrackTest, FilterDmaheapInfoByPid_NoMatch, testing::ext::TestSize.Level1)
{
    std::string input =
        "Dma-buf objects usage of processes:\n"
        "Process    pid    fd    size_bytes    ino    exp_pid    exp_task_comm    buf_name    exp_name\n"
        "xxxxx    439    14    12288    2    439    xxxxx    NULL    vmalloc\n"
        "Total dmabuf size of xxxxx: 49152 bytes\n";

    std::string expectedOutput =
        "Dma-buf objects usage of processes:\n"
        "Process    pid    fd    size_bytes    ino    exp_pid    exp_task_comm    buf_name    exp_name\n";

    std::string output;
    MemoryTrack::FilterDmaheapInfoByPid(output, input, 999);

    ASSERT_EQ(output, expectedOutput);
}

} // namespace OHOS::Rosen