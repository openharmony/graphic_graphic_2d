/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "memory/rs_pixelmap_fd_track.h"

#include <filesystem>
#include <fstream>

namespace OHOS::Rosen {
class RSPixelMapFdTrackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPixelMapFdTrackTest::SetUpTestCase() {}
void RSPixelMapFdTrackTest::TearDownTestCase() {}
void RSPixelMapFdTrackTest::SetUp() {}
void RSPixelMapFdTrackTest::TearDown() {}

/**
 * @tc.name: AddFdRecordTest001
 * @tc.desc: Test function AddFdRecord with valid pid and SHARE_MEM_ALLOC
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 1);
}

/**
 * @tc.name: AddFdRecordTest002
 * @tc.desc: Test function AddFdRecord with invalid pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 0;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: AddFdRecordTest003
 * @tc.desc: Test function AddFdRecord with invalid allocType
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::HEAP_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: AddFdRecordTest004
 * @tc.desc: Test function AddFdRecord with DMA_ALLOC
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordTest004, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 2);
}

/**
 * @tc.name: AddFdRecordTest005
 * @tc.desc: Test function AddFdRecord with duplicate address
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordTest005, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    tracker.AddFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 1);
}

/**
 * @tc.name: RemoveFdRecordTest001
 * @tc.desc: Test function RemoveFdRecord with valid pid and address
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    tracker.RemoveFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: RemoveFdRecordTest002
 * @tc.desc: Test function RemoveFdRecord with invalid pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 0;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.RemoveFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: RemoveFdRecordTest003
 * @tc.desc: Test function RemoveFdRecord with invalid allocType
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::HEAP_ALLOC;

    tracker.RemoveFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: RemoveFdRecordTest004
 * @tc.desc: Test function RemoveFdRecord with non-existent pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordTest004, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.RemoveFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: RemoveFdRecordTest005
 * @tc.desc: Test function RemoveFdRecord with non-existent address
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordTest005, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr1 = reinterpret_cast<const void*>(0x1000);
    const void* addr2 = reinterpret_cast<const void*>(0x2000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr1, allocType);
    tracker.RemoveFdRecord(pid, addr2, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 1);
}

/**
 * @tc.name: RemoveFdRecordTest006
 * @tc.desc: Test function RemoveFdRecord with DMA_ALLOC
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordTest006, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    tracker.RemoveFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: CheckFdRecordAndKillProcessTest001
 * @tc.desc: Test function CheckFdRecordAndKillProcess with normal pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndKillProcessTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;

    bool result = tracker.CheckFdRecordAndKillProcess(pid);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CheckFdRecordAndKillProcessTest002
 * @tc.desc: Test function CheckFdRecordAndKillProcess with invalid pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndKillProcessTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 0;

    bool result = tracker.CheckFdRecordAndKillProcess(pid);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CheckFdRecordAndKillProcessTest003
 * @tc.desc: Test function CheckFdRecordAndKillProcess with negative pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndKillProcessTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = -1;

    bool result = tracker.CheckFdRecordAndKillProcess(pid);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ClearFdRecordByPidTest001
 * @tc.desc: Test function ClearFdRecordByPid with valid pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ClearFdRecordByPidTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    tracker.ClearFdRecordByPid(pid);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: ClearFdRecordByPidTest002
 * @tc.desc: Test function ClearFdRecordByPid with non-existent pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ClearFdRecordByPidTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;

    tracker.ClearFdRecordByPid(pid);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: CheckFdRecordAndGetPidsToKillTest001
 * @tc.desc: Test function CheckFdRecordAndGetPidsToKill with fd count exceeding limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndGetPidsToKillTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 25001; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    std::unordered_map<int32_t, int32_t> pidsToKill;
    tracker.CheckFdRecordAndGetPidsToKill(pidsToKill, pid);

    EXPECT_FALSE(pidsToKill.empty());
    EXPECT_EQ(pidsToKill.size(), 1);
    EXPECT_EQ(pidsToKill[pid], 25001);
}

/**
 * @tc.name: CheckFdRecordAndGetPidsToKillTest002
 * @tc.desc: Test function CheckFdRecordAndGetPidsToKill with fd total exceeding limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndGetPidsToKillTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 14501; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, allocType);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    std::unordered_map<int32_t, int32_t> pidsToKill;
    tracker.CheckFdRecordAndGetPidsToKill(pidsToKill, pid1);

    EXPECT_FALSE(pidsToKill.empty());
}

/**
 * @tc.name: CheckFdRecordAndGetPidsToKillTest003
 * @tc.desc: Test function CheckFdRecordAndGetPidsToKill when both fd count and total exceed limits
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndGetPidsToKillTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 25001; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, allocType);
    }

    for (int32_t i = 0; i < 5000; i++) {
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    std::unordered_map<int32_t, int32_t> pidsToKill;
    tracker.CheckFdRecordAndGetPidsToKill(pidsToKill, pid1);

    EXPECT_GE(pidsToKill.size(), 1);
    EXPECT_TRUE(pidsToKill.find(pid1) != pidsToKill.end());
}

/**
 * @tc.name: CheckFdCountByPidTest001
 * @tc.desc: Test function CheckFdCountByPid with pid below limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdCountByPidTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_GT(fdCount, 0);
}

/**
 * @tc.name: CheckFdCountByPidTest002
 * @tc.desc: Test function CheckFdCountByPid with non-existent pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdCountByPidTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    int32_t fdCount = 0;

    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: CheckFdTotalTest001
 * @tc.desc: Test function CheckFdTotal with total below limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;

    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CheckFdTotalTest002
 * @tc.desc: Test function CheckFdTotal with multiple pids
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    const void* addr1 = reinterpret_cast<const void*>(0x1000);
    const void* addr2 = reinterpret_cast<const void*>(0x2000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid1, addr1, allocType);
    tracker.AddFdRecord(pid2, addr2, allocType);

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: KillProcessByPidTest001
 * @tc.desc: Test function KillProcessByPid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, KillProcessByPidTest001, testing::ext::TestSize.Level1)
{
    int32_t pid = 0;
    std::string reason = "test reason";
    RSPixelMapFdTrack tracker;
    bool success = tracker.KillProcessByPid(pid, reason);
    EXPECT_FALSE(success);
    std::string folderPath = "/proc/" + std::to_string(pid);
    EXPECT_FALSE(std::filesystem::is_directory(folderPath));
}

/**
 * @tc.name: ReportFdOverLimitTest001
 * @tc.desc: Test function ReportFdOverLimit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ReportFdOverLimitTest001, testing::ext::TestSize.Level1)
{
    int32_t pid = 1234;
    RSPixelMapFdTrack::ReportFdOverLimit(pid);
    EXPECT_TRUE(pid);
}

/**
 * @tc.name: WriteFdMemInfoToFileTest001
 * @tc.desc: Test function WriteFdMemInfoToFile
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest001, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string fdMemInfoPath = "/data/service/el0/render_service/renderservice_fdmem.txt";
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
    ASSERT_TRUE(std::ifstream(fdMemInfoPath).good());
}

/**
 * @tc.name: WriteFdMemInfoToFileTest002
 * @tc.desc: Test function WriteFdMemInfoToFile when file cannot be opened
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest002, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string fdMemInfoPath = "/invalid/path/test.txt";
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
    EXPECT_FALSE(std::ifstream(fdMemInfoPath).good());
}

/**
 * @tc.name: ReadAshmemInfoFromFileTest001
 * @tc.desc: Test function ReadAshmemInfoFromFile with valid file
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadAshmemInfoFromFileTest001, testing::ext::TestSize.Level1)
{
    std::string testFilePath = "/proc/ashmem_process_info";
    std::string ashmemInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadAshmemInfoFromFile(testFilePath, ashmemInfo, testPid);

    EXPECT_FALSE(ashmemInfo.empty());
    EXPECT_TRUE(ashmemInfo.find(std::to_string(testPid)) == std::string::npos);
}

/**
 * @tc.name: ReadAshmemInfoFromFileTest002
 * @tc.desc: Test function ReadAshmemInfoFromFile with non-existent file
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadAshmemInfoFromFileTest002, testing::ext::TestSize.Level1)
{
    std::string nonExistentPath = "/invalid/path/test.txt";
    std::string ashmemInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadAshmemInfoFromFile(nonExistentPath, ashmemInfo, testPid);

    EXPECT_FALSE(ashmemInfo.empty());
    EXPECT_TRUE(ashmemInfo.find("Failed to open ashmemInfoFile") != std::string::npos);
}

/**
 * @tc.name: ReadDmaBufInfoFromFileTest001
 * @tc.desc: Test function ReadDmaBufInfoFromFile with valid file
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadDmaBufInfoFromFileTest001, testing::ext::TestSize.Level1)
{
    std::string testFilePath = "/proc/process_dmabuf_info";
    std::string dmaBufInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadDmaBufInfoFromFile(testFilePath, dmaBufInfo, testPid);

    EXPECT_FALSE(dmaBufInfo.empty());
    EXPECT_TRUE(dmaBufInfo.find(std::to_string(testPid)) == std::string::npos);
}

/**
 * @tc.name: ReadDmaBufInfoFromFileTest002
 * @tc.desc: Test function ReadDmaBufInfoFromFile with non-existent file
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadDmaBufInfoFromFileTest002, testing::ext::TestSize.Level1)
{
    std::string nonExistentPath = "/invalid/path/test.txt";
    std::string dmaBufInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadDmaBufInfoFromFile(nonExistentPath, dmaBufInfo, testPid);

    EXPECT_FALSE(dmaBufInfo.empty());
    EXPECT_TRUE(dmaBufInfo.find("Failed to open dmaBufInfoFile") != std::string::npos);
}

/**
 * @tc.name: FilterAshmemInfoByPid_NormalCase
 * @tc.desc: Test filtering ashmem log by pid with matching entries
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, FilterAshmemInfoByPid_NormalCase, testing::ext::TestSize.Level1)
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
    RSPixelMapFdTrack::FilterAshmemInfoByPid(output, input, 816);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterAshmemInfoByPid_EmptyInput
 * @tc.desc: Test filtering ashmem log with empty input
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, FilterAshmemInfoByPid_EmptyInput, testing::ext::TestSize.Level1)
{
    std::string input = "";
    std::string expectedOutput = "";
    std::string output;
    RSPixelMapFdTrack::FilterAshmemInfoByPid(output, input, 816);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterAshmemInfoByPid_NoMatch
 * @tc.desc: Test filtering ashmem log with no matching pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, FilterAshmemInfoByPid_NoMatch, testing::ext::TestSize.Level1)
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
    RSPixelMapFdTrack::FilterAshmemInfoByPid(output, input, 999);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterDmaBufInfoByPid_NormalCase
 * @tc.desc: Test filtering dmabuf log by pid with matching entries
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, FilterDmaBufInfoByPid_NormalCase, testing::ext::TestSize.Level1)
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
    RSPixelMapFdTrack::FilterDmaBufInfoByPid(output, input, 500);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterDmaBufInfoByPid_EmptyInput
 * @tc.desc: Test filtering dmabuf log with empty input
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, FilterDmaBufInfoByPid_EmptyInput, testing::ext::TestSize.Level1)
{
    std::string input = "";
    std::string expectedOutput = "";
    std::string output;
    RSPixelMapFdTrack::FilterDmaBufInfoByPid(output, input, 500);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: FilterDmaBufInfoByPid_NoMatch
 * @tc.desc: Test filtering dmabuf log with no matching pid
 * @tc.type: FUNC
 * @tc.require:: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, FilterDmaBufInfoByPid_NoMatch, testing::ext::TestSize.Level1)
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
    RSPixelMapFdTrack::FilterDmaBufInfoByPid(output, input, 999);

    ASSERT_EQ(output, expectedOutput);
}

/**
 * @tc.name: AddRemoveMultipleRecordsTest001
 * @tc.desc: Test adding and removing multiple fd records
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddRemoveMultipleRecordsRecordsTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr1 = reinterpret_cast<const void*>(0x1000);
    const void* addr2 = reinterpret_cast<const void*>(0x2000);
    const void* addr3 = reinterpret_cast<const void*>(0x3000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr1, allocType);
    tracker.AddFdRecord(pid, addr2, allocType);
    tracker.AddFdRecord(pid, addr3, allocType);

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 3);

    tracker.RemoveFdRecord(pid, addr1, allocType);
    tracker.RemoveFdRecord(pid, addr2, allocType);
    tracker.RemoveFdRecord(pid, addr3, allocType);

    fdCount = 0;
    result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: MultiplePidsTest001
 * @tc.desc: Test tracking fd records for multiple pids
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, MultiplePidsTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    int32_t pid3 = 300;
    const void* addr1 = reinterpret_cast<const void*>(0x1000);
    const void* addr2 = reinterpret_cast<const void*>(0x2000);
    const void* addr3 = reinterpret_cast<const void*>(0x3000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid1, addr1, allocType);
    tracker.AddFdRecord(pid2, addr2, allocType);
    tracker.AddFdRecord(pid3, addr3, allocType);

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid1, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 1);

    result = tracker.CheckFdCountByPid(pid2, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 1);

    result = tracker.CheckFdCountByPid(pid3, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 1);

    tracker.RemoveFdRecord(pid1, addr1, allocType);
    tracker.RemoveFdRecord(pid2, addr2, allocType);
    tracker.RemoveFdRecord(pid3, addr3, allocType);

    fdCount = 0;
    result = tracker.CheckFdCountByPid(pid1, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: MixedAllocatorTypesTest001
 * @tc.desc: Test tracking with mixed allocator types
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, MixedAllocatorTypesTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr1 = reinterpret_cast<const void*>(0x1000);
    const void* addr2 = reinterpret_cast<const void*>(0x2000);
    Media::AllocatorType allocType1 = Media::AllocatorType::SHARE_MEM_ALLOC;
    Media::AllocatorType allocType2 = Media::AllocatorType::DMA_ALLOC;

    tracker.AddFdRecord(pid, addr1, allocType1);
    tracker.AddFdRecord(pid, addr2, allocType2);

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 3);

    tracker.RemoveFdRecord(pid, addr1, allocType1);
    tracker.RemoveFdRecord(pid, addr2, allocType2);

    fdCount = 0;
    result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: ClearAndReAddTest001
 * @tc.desc: Test clearing and re-adding fd records
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ClearAndReAddTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    tracker.ClearFdRecordByPid(pid);
    tracker.AddFdRecord(pid, addr, allocType);
    tracker.RemoveFdRecord(pid, addr, allocType);

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: RemoveLastAddressTest001
 * @tc.desc: Test removing last address triggers cleanup
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveLastAddressTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    tracker.RemoveFdRecord(pid, addr, allocType);

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: RemoveLastAddressTest002
 * @tc.desc: Test removing last DMA address triggers cleanup
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveLastAddressTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);
    tracker.RemoveFdRecord(pid, addr, allocType);

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: RemoveLastAddressTest003
 * @tc.desc: Test removing last address triggers cleanup across multiple addresses
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveLastAddressTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr1 = reinterpret_cast<const void*>(0x1000);
    const void* addr2 = reinterpret_cast<const void*>(0x2000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr1, allocType);
    tracker.AddFdRecord(pid, addr2, allocType);
    tracker.RemoveFdRecord(pid, addr1, allocType);

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 1);

    tracker.RemoveFdRecord(pid, addr2, allocType);

    fdCount = 0;
    result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(fdCount, 0);
}

/**
 * @tc.name: CheckFdCountByPidExceedsLimitTest001
 * @tc.desc: Test function CheckFdCountByPid when fd count exceeds limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdCountByPidExceedsLimitTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 25001; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_FALSE(result);
    EXPECT_GT(fdCount, 25000);
}

/**
 * @tc.name: CheckFdCountByPidExceedsLimitTest002
 * @tc.desc: Test function CheckFdCountByPid with DMA alloc when fd count exceeds limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdCountByPidExceedsLimitTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;

    for (int32_t i = 0; i < 12501; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_FALSE(result);
    EXPECT_GT(fdCount, 25000);
}

/**
 * @tc.name: CheckFdTotalExceedsLimitTest001
 * @tc.desc: Test function CheckFdTotal when fd total exceeds limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalExceedsLimitTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 14501; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, allocType);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_FALSE(result);
    EXPECT_GT(topFdCount, 0);
}

/**
 * @tc.name: CheckFdTotalExceedsLimitTest002
 * @tc.desc: Test function CheckFdTotal with DMA alloc when fd total exceeds limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalExceedsLimitTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;

    for (int32_t i = 0; i < 7251; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, allocType);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_FALSE(result);
    EXPECT_GT(topFdCount, 0);
}

/**
 * @tc.name: CheckFdTotalMultiplePidsTest001
 * @tc.desc: Test function CheckFdTotal with multiple pids having different counts
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalMultiplePidsTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    int32_t pid3 = 300;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 10000; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, allocType);
    }

    for (int32_t i = 0; i < 15000; i++) {
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    for (int32_t i = 0; i < 5000; i++) {
        const void* addr3 = reinterpret_cast<const void*>(0x3000 + i * 8);
        tracker.AddFdRecord(pid3, addr3, allocType);
    }

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_FALSE(result);
    EXPECT_EQ(topFdCountPid, pid2);
    EXPECT_EQ(topFdCount, 15000);
}

/**
 * @tc.name: CheckFdTotalMultiplePidsTest002
 * @tc.desc: Test function CheckFdTotal skipping pids with lower fd count
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalMultiplePidsTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    int32_t pid3 = 300;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 5000; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, allocType);
    }

    for (int32_t i = 0; i < 10000; i++) {
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    for (int32_t i = 0; i < 2000; i++) {
        const void* addr3 = reinterpret_cast<const void*>(0x3000 + i * 8);
        tracker.AddFdRecord(pid3, addr3, allocType);
    }

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(topFdCountPid, 0);
    EXPECT_EQ(topFdCount, 0);
}

/**
 * @tc.name: CheckFdTotalMultiplePidsTest003
 * @tc.desc: Test function CheckFdTotal when multiple pids have same fd count, select higher pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalMultiplePidsTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 1;
    int32_t pid2 = 10;
    int32_t pid3 = 100;
    int32_t pid4 = 1000;
    int32_t pid5 = 10000;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    for (int32_t i = 0; i < 6000; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        const void* addr3 = reinterpret_cast<const void*>(0x3000 + i * 8);
        const void* addr4 = reinterpret_cast<const void*>(0x4000 + i * 8);
        const void* addr5 = reinterpret_cast<const void*>(0x5000 + i * 8);
        tracker.AddFdRecord(pid3, addr3, allocType);
        tracker.AddFdRecord(pid1, addr1, allocType);
        tracker.AddFdRecord(pid5, addr5, allocType);
        tracker.AddFdRecord(pid4, addr4, allocType);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_FALSE(result);
    EXPECT_EQ(topFdCountPid, pid5);
    EXPECT_EQ(topFdCount, 6000);
}

/**
 * @tc.name: CheckFdTotalMixedAllocTypeTest001
 * @tc.desc: Test function CheckFdTotal with mixed allocator types
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalMixedAllocTypeTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    Media::AllocatorType shareMemType = Media::AllocatorType::SHARE_MEM_ALLOC;
    Media::AllocatorType dmaType = Media::AllocatorType::DMA_ALLOC;

    for (int32_t i = 0; i < 20000; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, shareMemType);
    }

    for (int32_t i = 0; i < 6000; i++) {
        const void* addr2 = reinterpret_cast<const void*>(0x2000 + i * 8);
        tracker.AddFdRecord(pid2, addr2, dmaType);
    }

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_FALSE(result);
    EXPECT_EQ(topFdCountPid, pid1);
    EXPECT_EQ(topFdCount, 20000);
}
} // namespace OHOS::Rosen