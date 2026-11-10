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

#include <chrono>
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

    for (int32_t i = 0; i < 29001; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    std::unordered_map<int32_t, int32_t> pidsToKill;
    tracker.CheckFdRecordAndGetPidsToKill(pidsToKill, pid);

    EXPECT_FALSE(pidsToKill.empty());
    EXPECT_EQ(pidsToKill.size(), 1);
    EXPECT_EQ(pidsToKill[pid], 29001);
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
 * @tc.desc: Test function ReportFdOverLimit with FD type
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ReportFdOverLimitTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 1234;
    tracker.ReportFdOverLimit(pid, "FD", RSPixelMapFdTrack::MAX_APP_FD, RSPixelMapFdTrack::MAX_APP_FD + 1);
    EXPECT_TRUE(pid);
}

/**
 * @tc.name: ReportFdOverLimitTest002
 * @tc.desc: Test function ReportFdOverLimit with FD_WARNING type
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ReportFdOverLimitTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 1234;
    tracker.ReportFdOverLimit(pid, "FD_WARNING", RSPixelMapFdTrack::WARNING_THRESHOLD,
        RSPixelMapFdTrack::WARNING_THRESHOLD);
    EXPECT_TRUE(pid);
}

/**
 * @tc.name: WriteFdMemInfoToFileTest001
 * @tc.desc: Test function WriteFdMemInfoToFile with valid path (file will be created)
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest001, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string fdMemInfoPath = "/data/service/el0/render_service/renderservice_fdmem_test.txt";
    std::filesystem::remove(fdMemInfoPath);
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
    std::ifstream file1(fdMemInfoPath);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_TRUE(file1.good());
#else
    EXPECT_FALSE(file1.good());
#endif
    file1.close();
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
    std::ifstream file2(fdMemInfoPath);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_TRUE(file2.good());
#else
    EXPECT_FALSE(file2.good());
#endif
    file2.close();
    std::filesystem::remove(fdMemInfoPath);
}

/**
 * @tc.name: WriteFdMemInfoToFileTest002
 * @tc.desc: Test function WriteFdMemInfoToFile when parent directory does not exist
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
 * @tc.name: WriteFdMemInfoToFileTest003
 * @tc.desc: Test function WriteFdMemInfoToFile when path is an existing directory
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest003, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string fdMemInfoPath = "/data/service/el0/render_service";
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
    EXPECT_FALSE(std::filesystem::is_regular_file(fdMemInfoPath));
}

/**
 * @tc.name: WriteFdMemInfoToFileTest004
 * @tc.desc: Test function WriteFdMemInfoToFile with path ending with "/" (filename is empty)
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest004, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string fdMemInfoPath = "/invalid/path/";
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
    EXPECT_FALSE(std::filesystem::exists(fdMemInfoPath));
}

/**
 * @tc.name: WriteFdMemInfoToFileTest005
 * @tc.desc: Test function WriteFdMemInfoToFile with empty directory path (only filename)
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest005, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string fdMemInfoPath = "renderservice_fdmem_test.txt";
    std::filesystem::remove(fdMemInfoPath);
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_TRUE(std::ifstream(fdMemInfoPath).good());
#else
    EXPECT_FALSE(std::ifstream(fdMemInfoPath).good());
#endif
    std::filesystem::remove(fdMemInfoPath);
}

/**
 * @tc.name: WriteFdMemInfoToFileTest006
 * @tc.desc: Test function WriteFdMemInfoToFile when file cannot be opened (permission denied)
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest006, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string fdMemInfoPath = "/proc/renderservice_fdmem_test.txt";
    RSPixelMapFdTrack::WriteFdMemInfoToFile(fdMemInfoPath, fdMemInfo);
    EXPECT_FALSE(std::ifstream(fdMemInfoPath).good());
}

/**
 * @tc.name: WriteFdMemInfoToFileTest007
 * @tc.desc: Test function WriteFdMemInfoToFile with symlink path
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, WriteFdMemInfoToFileTest007, testing::ext::TestSize.Level1)
{
    std::string fdMemInfo = "test info";
    std::string targetFile = "/data/service/el0/render_service/renderservice_fdmem_test_target.txt";
    std::string symlinkPath = "/data/service/el0/render_service/renderservice_fdmem_test_symlink.txt";

    std::filesystem::remove(targetFile);
    std::filesystem::remove(symlinkPath);

    std::ofstream(targetFile) << "initial content";
    std::filesystem::create_symlink(targetFile, symlinkPath);

    RSPixelMapFdTrack::WriteFdMemInfoToFile(symlinkPath, fdMemInfo);

    std::ifstream file(targetFile);
    EXPECT_TRUE(file.good());
    std::string content;
    std::getline(file, content, '\0');
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_TRUE(content.find(fdMemInfo) != std::string::npos);
#else
    EXPECT_FALSE(content.find(fdMemInfo) != std::string::npos);
#endif
    file.close();

    std::filesystem::remove(targetFile);
    std::filesystem::remove(symlinkPath);
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

#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_FALSE(ashmemInfo.empty());
#else
    EXPECT_TRUE(ashmemInfo.empty());
#endif
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

#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_FALSE(ashmemInfo.empty());
#else
    EXPECT_TRUE(ashmemInfo.empty());
#endif
    EXPECT_FALSE(ashmemInfo.find("Failed to check ashmemInfoPath") != std::string::npos);
}

/**
 * @tc.name: ReadAshmemInfoFromFileTest003
 * @tc.desc: Test function ReadAshmemInfoFromFile when path is a directory (not a regular file)
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadAshmemInfoFromFileTest003, testing::ext::TestSize.Level1)
{
    std::string dirPath = "/proc";
    std::string ashmemInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadAshmemInfoFromFile(dirPath, ashmemInfo, testPid);

    EXPECT_TRUE(ashmemInfo.empty());
    EXPECT_FALSE(ashmemInfo.find("Failed to check ashmemInfoPath") != std::string::npos);
}

/**
 * @tc.name: ReadAshmemInfoFromFileTest004
 * @tc.desc: Test function ReadAshmemInfoFromFile when file cannot be opened (permission denied)
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadAshmemInfoFromFileTest004, testing::ext::TestSize.Level1)
{
    std::string testFilePath = "/proc/sysrq-trigger";
    std::string ashmemInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadAshmemInfoFromFile(testFilePath, ashmemInfo, testPid);

    EXPECT_TRUE(ashmemInfo.find(std::to_string(testPid)) == std::string::npos);
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

#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_FALSE(dmaBufInfo.empty());
#else
    EXPECT_TRUE(dmaBufInfo.empty());
#endif
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

#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_FALSE(dmaBufInfo.empty());
#else
    EXPECT_TRUE(dmaBufInfo.empty());
#endif
    EXPECT_FALSE(dmaBufInfo.find("Failed to check dmaBufInfoPath") != std::string::npos);
}

/**
 * @tc.name: ReadDmaBufInfoFromFileTest003
 * @tc.desc: Test function ReadDmaBufInfoFromFile when path is a directory (not a regular file)
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadDmaBufInfoFromFileTest003, testing::ext::TestSize.Level1)
{
    std::string dirPath = "/proc";
    std::string dmaBufInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadDmaBufInfoFromFile(dirPath, dmaBufInfo, testPid);

    EXPECT_TRUE(dmaBufInfo.empty());
    EXPECT_FALSE(dmaBufInfo.find("Failed to check dmaBufInfoPath") != std::string::npos);
}

/**
 * @tc.name: ReadDmaBufInfoFromFileTest004
 * @tc.desc: Test function ReadDmaBufInfoFromFile when file cannot be opened (permission denied)
 * @tc.type: FUNC
 * @tc.require: issue#23694
 */
HWTEST_F(RSPixelMapFdTrackTest, ReadDmaBufInfoFromFileTest004, testing::ext::TestSize.Level1)
{
    std::string testFilePath = "/proc/sysrq-trigger";
    std::string dmaBufInfo;
    int32_t testPid = -1;
    RSPixelMapFdTrack::ReadDmaBufInfoFromFile(testFilePath, dmaBufInfo, testPid);

    EXPECT_TRUE(dmaBufInfo.find(std::to_string(testPid)) == std::string::npos);
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

    for (int32_t i = 0; i < 29001; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_FALSE(result);
    EXPECT_GT(fdCount, 29000);
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

    for (int32_t i = 0; i < 14501; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    int32_t fdCount = 0;
    bool result = tracker.CheckFdCountByPid(pid, fdCount);
    EXPECT_FALSE(result);
    EXPECT_GT(fdCount, 29000);
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

/**
 * @tc.name: KillProcessByPidTest002
 * @tc.desc: Test function KillProcessByPid with non-zero pid to exercise RS_ENABLE_UNI_RENDER branch
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, KillProcessByPidTest002, testing::ext::TestSize.Level1)
{
    int32_t pid = 99999;
    std::string reason = "fd count exceeds limit";
    RSPixelMapFdTrack tracker;
    bool success = tracker.KillProcessByPid(pid, reason);
    EXPECT_FALSE(success);
}

/**
 * @tc.name: GetTopFdCountPidTest001
 * @tc.desc: Test function GetTopFdCountPid selects the pid with the largest fd count
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopFdCountPidTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    tracker.fdCountTracker_[100] = 3000;
    tracker.fdCountTracker_[200] = 5000;

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    tracker.GetTopFdCountPid(topFdCountPid, topFdCount);

    EXPECT_EQ(topFdCountPid, 200);
    EXPECT_EQ(topFdCount, 5000);
}

/**
 * @tc.name: GetTopFdCountPidTest002
 * @tc.desc: Test function GetTopFdCountPid skips pids whose fd count is below the current top
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopFdCountPidTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    // seed a running top so the only entry is strictly smaller and must be skipped
    tracker.fdCountTracker_[100] = 3000;

    int32_t topFdCountPid = 200;
    int32_t topFdCount = 5000;
    tracker.GetTopFdCountPid(topFdCountPid, topFdCount);

    EXPECT_EQ(topFdCountPid, 200);
    EXPECT_EQ(topFdCount, 5000);
}

/**
 * @tc.name: GetTopFdCountPidTest003
 * @tc.desc: Test function GetTopFdCountPid keeps the higher pid when fd counts are equal
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopFdCountPidTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    tracker.fdCountTracker_[100] = 5000;
    tracker.fdCountTracker_[200] = 5000;

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    tracker.GetTopFdCountPid(topFdCountPid, topFdCount);

    EXPECT_EQ(topFdCountPid, 200);
    EXPECT_EQ(topFdCount, 5000);
}

/**
 * @tc.name: GetTopFdCountPidTest004
 * @tc.desc: Test function GetTopFdCountPid keeps the current pid when an equal-count pid is lower
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopFdCountPidTest004, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    tracker.fdCountTracker_[100] = 5000;

    int32_t topFdCountPid = 300;
    int32_t topFdCount = 5000;
    tracker.GetTopFdCountPid(topFdCountPid, topFdCount);

    EXPECT_EQ(topFdCountPid, 300);
    EXPECT_EQ(topFdCount, 5000);
}

/**
 * @tc.name: CheckFdWarningAndReportTest001
 * @tc.desc: Test function CheckFdWarningAndReport does not report when fd total is below threshold
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 - 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();

    EXPECT_TRUE(tracker.lastWarningTime_.empty());
}

/**
 * @tc.name: CheckFdWarningAndReportTest002
 * @tc.desc: Test function CheckFdWarningAndReport does not report when fd total exceeds max limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::MAX_TOTAL_FD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();

    EXPECT_TRUE(tracker.lastWarningTime_.empty());
}

/**
 * @tc.name: CheckFdWarningAndReportTest003
 * @tc.desc: Test function CheckFdWarningAndReport does not report when no valid top pid exists
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    // fdTotal_ and fdCountTracker_ can only desync in abnormal states, so seed the members directly
    tracker.fdTotal_ = RSPixelMapFdTrack::WARNING_THRESHOLD + 1;

    tracker.CheckFdWarningAndReport();

    EXPECT_TRUE(tracker.lastWarningTime_.empty());
}

/**
 * @tc.name: CheckFdWarningAndReportTest004
 * @tc.desc: Test function CheckFdWarningAndReport does not report when fd total equals warning threshold
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest004, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();

    EXPECT_TRUE(tracker.lastWarningTime_.empty());
}

/**
 * @tc.name: CheckFdWarningAndReportTest005
 * @tc.desc: Test function CheckFdWarningAndReport reports when fd total reaches max total limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest005, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::MAX_TOTAL_FD / 2; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();

    EXPECT_EQ(tracker.lastWarningTime_.size(), 1);
    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);
}

/**
 * @tc.name: CheckFdWarningAndReportTest006
 * @tc.desc: Test function CheckFdWarningAndReport reports only the top fd count pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest006, testing::ext::TestSize.Level1)
{
    int32_t otherPidDmaRecords = 1000;
    RSPixelMapFdTrack tracker;
    int32_t topPid = 100;
    int32_t otherPid = 200;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(topPid, addr, allocType);
    }
    for (int32_t i = 0; i < otherPidDmaRecords; i++) {
        const void* addr = reinterpret_cast<const void*>(0x100000 + i * 8);
        tracker.AddFdRecord(otherPid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();

    EXPECT_EQ(tracker.lastWarningTime_.size(), 1);
    EXPECT_EQ(tracker.lastWarningTime_.count(topPid), 1);
    EXPECT_EQ(tracker.lastWarningTime_.count(otherPid), 0);
}

/**
 * @tc.name: CheckFdWarningAndReportTest007
 * @tc.desc: Test function CheckFdWarningAndReport skips report within rate limit window
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest007, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();
    ASSERT_EQ(tracker.lastWarningTime_.count(pid), 1);
    int64_t firstReportTime = tracker.lastWarningTime_[pid];

    tracker.CheckFdWarningAndReport();

    EXPECT_EQ(tracker.lastWarningTime_[pid], firstReportTime);
    EXPECT_EQ(tracker.lastWarningTime_.size(), 1);
}

/**
 * @tc.name: CheckFdWarningAndReportTest008
 * @tc.desc: Test function CheckFdWarningAndReport reports again after rate limit expires
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest008, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }
    auto now = std::chrono::steady_clock::now();
    int64_t nowNs = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    // last warning happened exactly one rate limit window ago, so a new warning must be reported
    int64_t expiredTimeNs = nowNs - RSPixelMapFdTrack::WARNING_RATE_LIMIT_SECONDS * std::nano::den;
    tracker.lastWarningTime_[pid] = expiredTimeNs;

    tracker.CheckFdWarningAndReport();

    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);
    EXPECT_GT(tracker.lastWarningTime_[pid], expiredTimeNs);
}

/**
 * @tc.name: CheckFdWarningAndReportTest009
 * @tc.desc: Test function CheckFdWarningAndReport reports when fd total is above warning threshold
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest009, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();

    EXPECT_EQ(tracker.lastWarningTime_.size(), 1);
    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);
}

/**
 * @tc.name: CheckFdWarningAndReportTest010
 * @tc.desc: Test function CheckFdWarningAndReport prunes rate limit entries whose window expired
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdWarningAndReportTest010, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    int32_t otherPid = 200;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }
    auto now = std::chrono::steady_clock::now();
    int64_t nowNs = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    // otherPid holds an expired entry and pid holds a fresh one, only the expired entry is pruned
    int64_t expiredTimeNs = nowNs - RSPixelMapFdTrack::WARNING_RATE_LIMIT_SECONDS * std::nano::den;
    tracker.lastWarningTime_[otherPid] = expiredTimeNs;
    tracker.lastWarningTime_[pid] = nowNs;

    tracker.CheckFdWarningAndReport();

    // the fresh entry still rate limits pid, so it is kept unchanged and no new warning fires
    EXPECT_EQ(tracker.lastWarningTime_[pid], nowNs);
    // the expired entry can no longer suppress a report, so it is pruned even if otherPid is not
    // the current top pid
    EXPECT_EQ(tracker.lastWarningTime_.count(otherPid), 0);
    EXPECT_EQ(tracker.lastWarningTime_.size(), 1);
}

/**
 * @tc.name: CheckFdTotalBoundaryTest001
 * @tc.desc: Test function CheckFdTotal with fd total exactly at the limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdTotalBoundaryTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::MAX_TOTAL_FD / 4; i++) {
        const void* addr1 = reinterpret_cast<const void*>(0x1000 + i * 8);
        const void* addr2 = reinterpret_cast<const void*>(0x100000 + i * 8);
        tracker.AddFdRecord(pid1, addr1, allocType);
        tracker.AddFdRecord(pid2, addr2, allocType);
    }

    int32_t topFdCountPid = 0;
    int32_t topFdCount = 0;
    bool result = tracker.CheckFdTotal(topFdCountPid, topFdCount);
    EXPECT_TRUE(result);
    EXPECT_EQ(topFdCountPid, 0);
    EXPECT_EQ(topFdCount, 0);
}

/**
 * @tc.name: CheckFdRecordAndKillProcessWarningTest001
 * @tc.desc: Test function CheckFdRecordAndKillProcess triggers fd warning when total is in warning range
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndKillProcessWarningTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    bool result = tracker.CheckFdRecordAndKillProcess(pid);

    EXPECT_TRUE(result);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);
#else
    EXPECT_TRUE(tracker.lastWarningTime_.empty());
#endif
}

/**
 * @tc.name: CheckFdRecordAndKillProcessKillTest001
 * @tc.desc: Test function CheckFdRecordAndKillProcess reports FD over limit and kills when count exceeds limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndKillProcessKillTest001, testing::ext::TestSize.Level1)
{
    constexpr int32_t nonExistentPid = 123456789; // larger than kernel pid_max, never a real process
    RSPixelMapFdTrack tracker;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::MAX_APP_FD + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(nonExistentPid, addr, allocType);
    }

    bool result = tracker.CheckFdRecordAndKillProcess(nonExistentPid);

#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_FALSE(result);
#else
    EXPECT_TRUE(result);
#endif
    // fd total exceeds max limit, so the warning path must be skipped
    EXPECT_TRUE(tracker.lastWarningTime_.empty());
}

/**
 * @tc.name: ClearFdRecordByPidLastWarningTimeTest001
 * @tc.desc: Test function ClearFdRecordByPid clears warning rate limit state of the pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ClearFdRecordByPidLastWarningTimeTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();
    ASSERT_EQ(tracker.lastWarningTime_.count(pid), 1);

    tracker.ClearFdRecordByPid(pid);
    EXPECT_TRUE(tracker.lastWarningTime_.empty());
    EXPECT_EQ(tracker.fdTotal_, 0);

    for (int32_t i = 0; i < RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }
    // rate limit state was cleared, so the warning is reported again immediately
    tracker.CheckFdWarningAndReport();
    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);
}

/**
 * @tc.name: AddFdRecordWithTagTest001
 * @tc.desc: Test function AddFdRecord stores the uniqueId tag of the pixelmap
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordWithTagTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    constexpr uint64_t uniqueId = 12345;

    tracker.AddFdRecord(pid, addr, allocType, uniqueId);

    EXPECT_EQ(tracker.addrTracker_[pid][addr], uniqueId);
}

/**
 * @tc.name: AddFdRecordWithTagTest002
 * @tc.desc: Test function AddFdRecord keeps the first tag when adding a duplicate address
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordWithTagTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    constexpr uint64_t firstUniqueId = 12345;
    constexpr uint64_t secondUniqueId = 67890;

    tracker.AddFdRecord(pid, addr, allocType, firstUniqueId);
    tracker.AddFdRecord(pid, addr, allocType, secondUniqueId);

    EXPECT_EQ(tracker.addrTracker_[pid][addr], firstUniqueId);
}

/**
 * @tc.name: AddFdRecordWithTagTest003
 * @tc.desc: Test function AddFdRecord isolates tags of the same address across pids
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordWithTagTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    constexpr uint64_t uniqueId1 = 12345;
    constexpr uint64_t uniqueId2 = 67890;

    tracker.AddFdRecord(pid1, addr, allocType, uniqueId1);
    tracker.AddFdRecord(pid2, addr, allocType, uniqueId2);

    EXPECT_EQ(tracker.addrTracker_[pid1][addr], uniqueId1);
    EXPECT_EQ(tracker.addrTracker_[pid2][addr], uniqueId2);
}

/**
 * @tc.name: AddFdRecordWithTagTest004
 * @tc.desc: Test function AddFdRecord without uniqueId stores zero tag
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, AddFdRecordWithTagTest004, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    tracker.AddFdRecord(pid, addr, allocType);

    EXPECT_EQ(tracker.addrTracker_[pid][addr], 0);
}

/**
 * @tc.name: RemoveFdRecordTagTest001
 * @tc.desc: Test function RemoveFdRecord clears the tag so a re-added address gets the new tag
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordTagTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    constexpr uint64_t firstUniqueId = 12345;
    constexpr uint64_t secondUniqueId = 67890;

    tracker.AddFdRecord(pid, addr, allocType, firstUniqueId);
    tracker.RemoveFdRecord(pid, addr, allocType);
    tracker.AddFdRecord(pid, addr, allocType, secondUniqueId);

    EXPECT_EQ(tracker.addrTracker_[pid][addr], secondUniqueId);
}

/**
 * @tc.name: ClearFdRecordByPidTagTest001
 * @tc.desc: Test function ClearFdRecordByPid clears tags of the pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ClearFdRecordByPidTagTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    const void* addr = reinterpret_cast<const void*>(0x1000);
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    constexpr uint64_t uniqueId = 12345;
    constexpr uint64_t newUniqueId = 67890;

    tracker.AddFdRecord(pid, addr, allocType, uniqueId);
    tracker.ClearFdRecordByPid(pid);
    EXPECT_TRUE(tracker.addrTracker_.find(pid) == tracker.addrTracker_.end());

    tracker.AddFdRecord(pid, addr, allocType, newUniqueId);
    EXPECT_EQ(tracker.addrTracker_[pid][addr], newUniqueId);
}

/**
 * @tc.name: RemoveFdRecordUniqueIdStatTest001
 * @tc.desc: Test function RemoveFdRecord decrements the uniqueId stat maintained by the tracker
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordUniqueIdStatTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    constexpr uint64_t uniqueId = 12345;

    // each DMA pixelmap holds 2 fds, so the stat counts fds instead of pixelmaps
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType, uniqueId);
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x2000), allocType, uniqueId);
    EXPECT_EQ(tracker.pixelMapFdCountTracker_[uniqueId], 4);
    auto topIt = tracker.topUniqueIdTracker_.find({4, uniqueId, pid});
    ASSERT_TRUE(topIt != tracker.topUniqueIdTracker_.end());
    // pid rides in the top entry as payload and is printed by the dump
    EXPECT_EQ(topIt->pid, pid);

    tracker.RemoveFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType);
    EXPECT_EQ(tracker.pixelMapFdCountTracker_[uniqueId], 2);
    EXPECT_EQ(tracker.topUniqueIdTracker_.count({2, uniqueId, pid}), 1);
    EXPECT_EQ(tracker.topUniqueIdTracker_.count({4, uniqueId, pid}), 0);

    tracker.RemoveFdRecord(pid, reinterpret_cast<const void*>(0x2000), allocType);
    EXPECT_TRUE(tracker.pixelMapFdCountTracker_.find(uniqueId) == tracker.pixelMapFdCountTracker_.end());
    EXPECT_TRUE(tracker.topUniqueIdTracker_.empty());
}

/**
 * @tc.name: UpdateUniqueIdStatTest001
 * @tc.desc: Test function UpdateUniqueIdStat ignores a late non-positive delta without a stat entry
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, UpdateUniqueIdStatTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    constexpr uint64_t uniqueId = 12345;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;

    // the stat entry is dropped when its fd count reaches zero with the last record
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType, uniqueId);
    tracker.RemoveFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType);
    EXPECT_TRUE(tracker.pixelMapFdCountTracker_.find(uniqueId) == tracker.pixelMapFdCountTracker_.end());

    // a late negative delta arrives after the stat entry is gone, it must not resurrect the entry
    tracker.UpdateUniqueIdStat(uniqueId, pid, -1);
    EXPECT_TRUE(tracker.pixelMapFdCountTracker_.empty());
    EXPECT_TRUE(tracker.topUniqueIdTracker_.empty());

    // a zero delta must not create an entry either
    tracker.UpdateUniqueIdStat(uniqueId, pid, 0);
    EXPECT_TRUE(tracker.pixelMapFdCountTracker_.empty());
    EXPECT_TRUE(tracker.topUniqueIdTracker_.empty());
}

/**
 * @tc.name: RemoveFdRecordLastWarningTimeTest001
 * @tc.desc: Test function RemoveFdRecord keeps warning rate limit state of the pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordLastWarningTimeTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    constexpr uint64_t uniqueId = 12345;

    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType, uniqueId);
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x2000), allocType, uniqueId);
    tracker.lastWarningTime_[pid] = 1000;

    // the pid still holds records, so the rate limit state is kept
    tracker.RemoveFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType);
    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);

    // the last record of the pid is removed, but the rate limit state is independent of record
    // existence and is kept
    tracker.RemoveFdRecord(pid, reinterpret_cast<const void*>(0x2000), allocType);
    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);
}

/**
 * @tc.name: RemoveFdRecordLastWarningTimeTest002
 * @tc.desc: Test function RemoveFdRecord keeps the pid rate limited after records are re-added
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, RemoveFdRecordLastWarningTimeTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::DMA_ALLOC;
    constexpr int32_t recordCount = RSPixelMapFdTrack::WARNING_THRESHOLD / 2 + 1;
    for (int32_t i = 0; i < recordCount; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }

    tracker.CheckFdWarningAndReport();
    ASSERT_EQ(tracker.lastWarningTime_.count(pid), 1);
    int64_t firstReportTime = tracker.lastWarningTime_[pid];

    for (int32_t i = 0; i < recordCount; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.RemoveFdRecord(pid, addr, allocType);
    }
    EXPECT_EQ(tracker.lastWarningTime_.count(pid), 1);

    for (int32_t i = 0; i < recordCount; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(pid, addr, allocType);
    }
    // the rate limit state survived the empty record set, so the re-added pid is still rate
    // limited and no new warning is reported
    tracker.CheckFdWarningAndReport();

    EXPECT_EQ(tracker.lastWarningTime_[pid], firstReportTime);
    EXPECT_EQ(tracker.lastWarningTime_.size(), 1);
}

/**
 * @tc.name: ClearFdRecordByPidUniqueIdStatTest001
 * @tc.desc: Test function ClearFdRecordByPid clears uniqueId stats of the pid
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, ClearFdRecordByPidUniqueIdStatTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    constexpr uint64_t firstUniqueId = 12345;
    constexpr uint64_t secondUniqueId = 67890;

    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType, firstUniqueId);
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x2000), allocType, firstUniqueId);
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x3000), allocType, secondUniqueId);
    EXPECT_EQ(tracker.pixelMapFdCountTracker_.size(), 2);
    EXPECT_EQ(tracker.topUniqueIdTracker_.size(), 2);

    tracker.ClearFdRecordByPid(pid);
    EXPECT_TRUE(tracker.pixelMapFdCountTracker_.empty());
    EXPECT_TRUE(tracker.topUniqueIdTracker_.empty());
}

/**
 * @tc.name: GetTopPixelMapsInfoTest001
 * @tc.desc: Test function GetTopPixelMapsInfo appends entries even when fd total is within limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest001, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    // fd total stays far below the limit, the section must still be produced
    constexpr uint64_t uniqueId = 12345;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType, uniqueId);
    std::string dumpInfo;

    tracker.GetTopPixelMapsInfo(dumpInfo);

    EXPECT_TRUE(dumpInfo.find("uniqueId: 12345, pid: 100, fdCount: 1") != std::string::npos);
}

/**
 * @tc.name: GetTopPixelMapsInfoTest002
 * @tc.desc: Test function GetTopPixelMapsInfo appends top tags sorted by fd count descending
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest002, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid1 = 100;
    int32_t pid2 = 200;
    constexpr uint64_t bigUniqueId = 300;
    constexpr uint64_t smallUniqueId = 100;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    for (int32_t i = 0; i < 3; i++) {
        tracker.AddFdRecord(pid1, reinterpret_cast<const void*>(0x1000 + i * 8), allocType, bigUniqueId);
    }
    for (int32_t i = 0; i < 2; i++) {
        tracker.AddFdRecord(pid2, reinterpret_cast<const void*>(0x2000 + i * 8), allocType, smallUniqueId);
    }
    // untagged pixelmap must be excluded from the dump
    tracker.AddFdRecord(pid2, reinterpret_cast<const void*>(0x3000), allocType, 0);

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 300, pid: 100, fdCount: 3") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 100, pid: 200, fdCount: 2") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 300") < dumpInfo.find("uniqueId: 100"));
    EXPECT_TRUE(dumpInfo.find("uniqueId: 0,") == std::string::npos);
}

/**
 * @tc.name: GetTopPixelMapsInfoTest003
 * @tc.desc: Test function GetTopPixelMapsInfo appends at most top 10 uniqueIds
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest003, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    uint64_t tagNum = 11;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    // uniqueId t owns t pixelmaps, so uniqueId 1 is rank 11 and must be dropped
    for (uint64_t uniqueId = 1; uniqueId <= tagNum; uniqueId++) {
        for (uint64_t i = 0; i < uniqueId; i++) {
            const void* addr = reinterpret_cast<const void*>(0x10000 + uniqueId * 0x100 + i * 8);
            tracker.AddFdRecord(pid, addr, allocType, uniqueId);
        }
    }

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 11, pid: 100, fdCount: 11") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 2, pid: 100, fdCount: 2") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 1,") == std::string::npos);
}

/**
 * @tc.name: GetTopPixelMapsInfoTest004
 * @tc.desc: Test function GetTopPixelMapsInfo keeps all tags when there are exactly 10
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest004, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    uint64_t tagNum = 10;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    for (uint64_t uniqueId = 1; uniqueId <= tagNum; uniqueId++) {
        for (uint64_t i = 0; i < uniqueId; i++) {
            const void* addr = reinterpret_cast<const void*>(0x10000 + uniqueId * 0x100 + i * 8);
            tracker.AddFdRecord(pid, addr, allocType, uniqueId);
        }
    }

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 10, pid: 100, fdCount: 10") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 1, pid: 100, fdCount: 1") != std::string::npos);
}

/**
 * @tc.name: GetTopPixelMapsInfoTest005
 * @tc.desc: Test function GetTopPixelMapsInfo appends no entry when all pixelmaps are untagged
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest005, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    // untagged pixelmaps never enter the uniqueId stat, so no entry is appended
    tracker.AddFdRecord(100, reinterpret_cast<const void*>(0x1000), allocType, 0);
    tracker.AddFdRecord(200, reinterpret_cast<const void*>(0x2000), allocType, 0);

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.empty());
}

/**
 * @tc.name: GetTopPixelMapsInfoTest006
 * @tc.desc: Test function GetTopPixelMapsInfo sorts equal fd counts by uniqueId ascending
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest006, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    constexpr uint64_t firstUniqueId = 100;
    constexpr uint64_t secondUniqueId = 200;
    constexpr uint64_t thirdUniqueId = 300;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    for (int32_t i = 0; i < 2; i++) {
        tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000 + i * 8), allocType, thirdUniqueId);
        tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x2000 + i * 8), allocType, firstUniqueId);
        tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x3000 + i * 8), allocType, secondUniqueId);
    }

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 100") < dumpInfo.find("uniqueId: 200"));
    EXPECT_TRUE(dumpInfo.find("uniqueId: 200") < dumpInfo.find("uniqueId: 300"));
}

/**
 * @tc.name: GetTopPixelMapsInfoTest007
 * @tc.desc: Test function GetTopPixelMapsInfo appends the section after existing content
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest007, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    std::string existingContent = "old fd mem info";
    constexpr uint64_t uniqueId = 12345;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType, uniqueId);

    std::string dumpInfo = existingContent;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.rfind(existingContent, 0) == 0);
    EXPECT_TRUE(dumpInfo.size() > existingContent.size());
    EXPECT_TRUE(dumpInfo.find("uniqueId: 12345, pid: 100, fdCount: 1") != std::string::npos);
}

/**
 * @tc.name: GetTopPixelMapsInfoTest008
 * @tc.desc: Test function GetTopPixelMapsInfo prints only the low 32 bits of uniqueId
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest008, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    // uniqueId embeds pid in the high 32 bits, the dump must keep the image id part only
    constexpr uint32_t imageId = 4660;
    constexpr uint64_t fullUniqueId = (100ULL << 32) | imageId;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), allocType, fullUniqueId);

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 4660, pid: 100, fdCount: 1") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find(std::to_string(fullUniqueId)) == std::string::npos);
}

/**
 * @tc.name: GetTopPixelMapsInfoTest009
 * @tc.desc: Test function GetTopPixelMapsInfo ranks uniqueIds by fd count, not pixelmap count
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest009, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t pid = 100;
    constexpr uint64_t shareMemUniqueId = 100;
    constexpr uint64_t dmaUniqueId = 200;
    constexpr uint64_t mixedUniqueId = 300;
    Media::AllocatorType shareMemType = Media::AllocatorType::SHARE_MEM_ALLOC;
    Media::AllocatorType dmaType = Media::AllocatorType::DMA_ALLOC;
    // a DMA pixelmap holds 2 fds and a SHARE_MEM pixelmap holds 1 fd, the stat must count fds
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x1000), dmaType, dmaUniqueId);
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x2000), shareMemType, shareMemUniqueId);
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x3000), dmaType, mixedUniqueId);
    tracker.AddFdRecord(pid, reinterpret_cast<const void*>(0x4000), shareMemType, mixedUniqueId);

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 300, pid: 100, fdCount: 3") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 200, pid: 100, fdCount: 2") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 100, pid: 100, fdCount: 1") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 300") < dumpInfo.find("uniqueId: 200"));
    EXPECT_TRUE(dumpInfo.find("uniqueId: 200") < dumpInfo.find("uniqueId: 100"));
}

/**
 * @tc.name: GetTopPixelMapsInfoTest010
 * @tc.desc: Test function GetTopPixelMapsInfo breaks equal fd counts by larger pid, then uniqueId
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, GetTopPixelMapsInfoTest010, testing::ext::TestSize.Level1)
{
    RSPixelMapFdTrack tracker;
    int32_t smallPid = 100;
    int32_t largePid = 200;
    constexpr uint64_t firstUniqueId = 1;
    constexpr uint64_t secondUniqueId = 2;
    constexpr uint64_t thirdUniqueId = 3;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    // all entries tie on fd count: the larger pid ranks first, then the same pid sorts by uniqueId
    tracker.AddFdRecord(smallPid, reinterpret_cast<const void*>(0x1000), allocType, firstUniqueId);
    tracker.AddFdRecord(smallPid, reinterpret_cast<const void*>(0x2000), allocType, secondUniqueId);
    tracker.AddFdRecord(largePid, reinterpret_cast<const void*>(0x3000), allocType, thirdUniqueId);

    std::string dumpInfo;
    tracker.GetTopPixelMapsInfo(dumpInfo);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 3, pid: 200, fdCount: 1") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 1, pid: 100, fdCount: 1") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 2, pid: 100, fdCount: 1") != std::string::npos);
    EXPECT_TRUE(dumpInfo.find("uniqueId: 3") < dumpInfo.find("uniqueId: 1"));
    EXPECT_TRUE(dumpInfo.find("uniqueId: 1") < dumpInfo.find("uniqueId: 2"));
}

/**
 * @tc.name: CheckFdRecordAndKillProcessDumpTest001
 * @tc.desc: Test function CheckFdRecordAndKillProcess dumps top pixelmaps by uniqueId when total exceeds limit
 * @tc.type: FUNC
 * @tc.require: issue#21888
 */
HWTEST_F(RSPixelMapFdTrackTest, CheckFdRecordAndKillProcessDumpTest001, testing::ext::TestSize.Level1)
{
    constexpr int32_t nonExistentPid = 123456789; // larger than kernel pid_max, never a real process
    constexpr uint64_t uniqueId = 987654321;
    RSPixelMapFdTrack tracker;
    Media::AllocatorType allocType = Media::AllocatorType::SHARE_MEM_ALLOC;
    for (int32_t i = 0; i < RSPixelMapFdTrack::MAX_APP_FD + 1; i++) {
        const void* addr = reinterpret_cast<const void*>(0x1000 + i * 8);
        tracker.AddFdRecord(nonExistentPid, addr, allocType, uniqueId);
    }

    bool result = tracker.CheckFdRecordAndKillProcess(nonExistentPid);

#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_FALSE(result);
    std::ifstream file("/data/service/el0/render_service/renderservice_fdmem.txt");
    std::string content;
    std::getline(file, content, '\0');
    file.close();
    EXPECT_TRUE(content.find("=== TopFdPixelMapsByUniqueId ===") != std::string::npos);
    EXPECT_TRUE(content.find("uniqueId: 987654321, pid: 123456789, fdCount: 29001") != std::string::npos);
#else
    EXPECT_TRUE(result);
#endif
    // fd total exceeds max limit, so the warning path must be skipped
    EXPECT_TRUE(tracker.lastWarningTime_.empty());
}
} // namespace OHOS::Rosen